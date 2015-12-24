/*
**      SWISH++
**      src/search_daemon.cpp
**
**      Copyright (C) 1998  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "exit_codes.h"
#include "Group.h"
#include "PidFile.h"
#ifdef __APPLE__
#include "LaunchdCooperation.h"
#endif /* __APPLE__ */
#include "SearchBackground.h"
#include "SearchDaemon.h"
#include "SocketAddress.h"
#include "SocketFile.h"
#include "SocketQueueSize.h"
#include "SocketTimeout.h"
#include "ThreadsMax.h"
#include "ThreadsMin.h"
#include "ThreadTimeout.h"
#include "search_thread.h"
#include "User.h"
#include "util.h"                       /* for max_out_limit() */

// standard
#include <algorithm>                    /* for max() */
#include <arpa/inet.h>                  /* for Internet networking stuff */
#include <cerrno>
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <time.h>                       /* needed by sys/resource.h */
#include <sys/time.h>                   /* needed by FreeBSD systems */
#include <sys/resource.h>               /* for RLIMIT_* */
#include <sys/socket.h>                 /* for bind(3), socket(3), etc. */
#include <sys/un.h>                     /* for sockaddr_un */
#include <unistd.h>                     /* for fork(2), setsid(2), unlink(2) */

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif

using namespace PJL;
using namespace std;

//
// Turn a socket file descriptor into a server socket.  See also: W. Richard
// Stevens.  "Unix Network Programming, Vol 1, 2nd ed." Prentice-Hall, Upper
// Saddle River, NJ, 1998.
//
// From [Stevens 1998], pp. 194-197, "SO_REUSEADDR and SO_REUSEPORT Socket
// Options":
//
//      By default, when the listening server is restarted by calling socket,
//      bind, and listen, the call to bind fails because the listening server
//      is trying to bind a port that is part of an existing connection (the
//      one being handled by the previously spawned child).  But if the server
//      sets the SO_REUSEADDR socket option between calls to socket and bind,
//      the latter function will succeed.  All TCP servers should specify this
//      socket option to allow the server to be restarted in this situation.
//
// The reason this is done as a macro rather than a function is because if it
// were a function, we'd have to pass additional parameters to generate the
// correct error messages and exit codes.  That's too much work.
//
#define BIND_SOCKET(FD,ADR,TYPE) {                                  \
    int const on = 1;                                               \
    ::setsockopt( FD, SOL_SOCKET, SO_REUSEADDR,                     \
        reinterpret_cast<char const*>( &on ), sizeof on             \
    );                                                              \
    if ( ::bind( FD, (struct sockaddr*)&ADR, sizeof ADR ) == -1 ) { \
        error() << #TYPE " bind() failed" << error_string;          \
        ::exit( Exit_No_##TYPE##_Bind );                            \
    }                                                               \
    if ( ::listen( FD, socket_queue_size ) == -1 ) {                \
        error() << #TYPE " listen() failed" << error_string;        \
        ::exit( Exit_No_##TYPE##_Listen );                          \
    }                                                               \
}

static void detach_from_terminal();
static void handle_accept( int fd );
static int  open_tcp_socket();
static int  open_unix_socket();
void        reset_socket( int fd );
static void set_signal_handlers();

//*****************************************************************************
//
// SYNOPSIS
//
        void become_daemon()
//
// DESCRIPTION
//
//      Do things needed to becomd a daemon process: increase resource limits,
//      create a socket, detach from the terminal, record our PID, change to
//      the root directory, and finally service requests.  This function never
//      returns.
//
// SEE ALSO
//
//      W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
//      Addison-Wesley, Reading, MA, 1993.
//
//      ---.  "Unix Network Programming, Vol 1, 2nd ed."  Prentice-Hall, Upper
//      Saddle River, NJ, 1998.  Chapter 14.
//
//*****************************************************************************
{
#ifdef __APPLE__
    if ( !launchd_cooperation ) {
#endif
        ////////// Increase resource limits (hopefully) ///////////////////////

#ifdef RLIMIT_CPU                      /* SVR4, 4.3+BSD */
        //
        // Max-out the amount of CPU time we can run since we will be a daemon
        // and will run indefinitely.
        //
        max_out_limit( RLIMIT_CPU );
#endif
#ifdef RLIMIT_NOFILE                   /* SVR4 */
        //
        // Max-out the number of file descriptors we can have open to be able
        // to service as many clients concurrently as possible.
        //
        max_out_limit( RLIMIT_NOFILE );
#elif   defined( RLIMIT_OFILE )         /* 4.3+BSD name for NOFILE */
        max_out_limit( RLIMIT_OFILE );
#endif
#ifdef __APPLE__
    }
#endif

    ////////// Create socket(s) ///////////////////////////////////////////////

    bool const is_tcp  = daemon_type == "tcp"  || daemon_type == "both";
    int const tcp_fd = is_tcp ? open_tcp_socket() : -1;

    bool const is_unix = daemon_type == "unix" || daemon_type == "both";
    int const unix_fd = is_unix ? open_unix_socket() : -1;

    int const max_fd = max( tcp_fd, unix_fd ) + 1;

    ////////// Do miscellaneous daemon stuff //////////////////////////////////

#ifndef DEBUG_threads
    if ( search_background )
        detach_from_terminal();
    //
    // If requested, record our PID to a file.
    //
    if ( pid_file_name && *pid_file_name ) {
        ofstream pid_file( pid_file_name );
        if ( !pid_file ) {
            error() << '"' << pid_file_name << "\": " << error_string;
            ::exit( Exit_No_Write_PID );
        }
        pid_file << ::getpid() << endl;
    }

#ifdef __APPLE__
    if ( !launchd_cooperation ) {
#endif
        //
        // If we're root and we've been requested to change our process
        // user/group, do so.
        //
        if ( !group.change_to_gid() ) {
            error() << "can't change group to \"" << group << "\": "
                    << error_string;
            ::exit( Exit_No_Group );
        }
        if ( !user.change_to_uid() ) {
            error() << "can't change user to \"" << user << "\": "
                    << error_string;
            ::exit( Exit_No_Group );
        }

        //
        // From [Stevens 1993], p. 417, "Coding Rules":
        //
        //      Call setsid to create a new session.  The process (1) becomes a
        //      session leader of a new session, (2) becomes the process group
        //      leader of a new process group, and (3) has no controlling
        //      terminal.
        //
        ::setsid();

        //
        // Ibid.:
        //
        //      Change the current working directory to the root directory.
        //      The current working directory inherited from the parent could
        //      be on a mounted filesystem.  Since daemons normally exist until
        //      the system is rebooted, if the daemon stays on a mounted
        //      filesystem, that filesystem can not be unmounted.
        //
        if ( ::chdir( "/" ) == -1 ) {
            error() << "chdir() failed" << error_string;
            ::exit( Exit_No_Change_Dir );
        }
#ifdef __APPLE__
    }
#endif
#endif /* DEBUG_threads */

    set_signal_handlers();

    ////////// Accept requests ////////////////////////////////////////////////

    search_thread::socket_timeout = socket_timeout;
    while ( true ) {
#       ifdef DEBUG_threads
        cerr << "waiting for request\n";
#       endif

        fd_set rset;
        FD_ZERO( &rset );
        if ( is_tcp )
            FD_SET( tcp_fd, &rset );
        if ( is_unix )
            FD_SET( unix_fd, &rset );
        //
        // Sit around and wait until one of the socket file descriptors is
        // "ready."  See: [Stevens 1998], pp. 150-154.
        //
        int const num_fds = ::select( max_fd, &rset, 0, 0, 0 );
        if ( !num_fds )
            continue;
        if ( num_fds == -1 ) {
            if ( errno == EINTR )
                continue;
            error() << "select() failed" << error_string;
            ::exit( Exit_No_Select );
        }

        //
        // Handle one or both requests.
        //
        if ( is_tcp && FD_ISSET( tcp_fd, &rset ) ) {
            struct sockaddr_in addr;
            socklen_t len = sizeof addr;
            handle_accept( ::accept( tcp_fd, (struct sockaddr*)&addr, &len ) );
        }
        if ( is_unix && FD_ISSET( unix_fd, &rset ) ) {
            struct sockaddr_un addr;
            socklen_t len = sizeof addr;
            handle_accept( ::accept( unix_fd, (struct sockaddr*)&addr, &len ) );
        }
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        void detach_from_terminal()
//
// DESCRIPTION
//
//      From [Stevens 1993], p. 417, "Coding Rules":
//
//          The first thing to do is call fork and have the parent exit.  This
//          does several things.  First, if the daemon was started as a simple
//          shell command, having the parent terminate makes the shell think
//          that the command is done.  Second, the child inherits the process
//          group ID of the parent but gets a new process ID, so we're
//          guaranteed that the child is not a process group leader.  This is a
//          prerequisite for the call to setsid that is done next.
//
// SEE ALSO
//
//      W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
//      Addison-Wesley, Reading, MA, 1993.
//
//*****************************************************************************
{
    pid_t const child_pid = ::fork();
    if ( child_pid == -1 ) {
        error() << "fork() failed" << error_string;
        ::exit( Exit_No_Fork );
    }
    if ( child_pid > 0 )                // parent process ...
        ::exit( Exit_Success );         // ... just exit as described
}

//*****************************************************************************
//
// SYNOPSIS
//
        void handle_accept( int fd )
//
// DESCRIPTION
//
//      Handle a recently accepted socket file descriptor.  If the accept(2)
//      went OK, try to queue the request.  If that doesn't work (because all
//      the request threads are busy and the number of threads in the thread
//      pool has been maxed out, reset the TCP connection effectively telling
//      the client to "go away and try again later."
//
// PARAMETERS
//
//      fd  The file descriptor for the accepted socket.
//
//*****************************************************************************
{
    if ( fd == -1 ) {
        switch ( errno ) {
            case ECONNABORTED:          // POSIX.1g
            case EINTR:
#ifdef EPROTO
            case EPROTO:                // SVR4
#endif
                return;
        }
        cerr << error << "accept() failed" << error_string;
        ::exit( Exit_No_Accept );
    }

    static thread_pool threads = thread_pool(
        new search_thread( threads ), min_threads, max_threads, thread_timeout
    );
#   ifdef DEBUG_threads
    cerr << "queueing request\n";
#   endif
    if ( !threads.new_task( fd ) ) {
        reset_socket( fd );
        ::close( fd );
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        int open_tcp_socket()
//
// DESCRIPTION
//
//      Create, bind, and listen on a TCP socket.
//
// RETURN VALUE
//
//      Returns the associated Unix file descriptor.
//
//*****************************************************************************
{
    int const fd = ::socket( AF_INET, SOCK_STREAM, 0 );
    if ( fd == -1 ) {
        error() << "TCP socket() failed" << error_string;
        ::exit( Exit_No_TCP_Socket );
    }
    struct sockaddr_in addr;
    ::memset( &addr, 0, sizeof addr );
    addr.sin_family = AF_INET;
    addr.sin_addr = socket_address.addr();
    addr.sin_port = htons( socket_address.port() );
    BIND_SOCKET(fd,addr,TCP);
    return fd;
}

//*****************************************************************************
//
// SYNOPSIS
//
        int open_unix_socket()
//
// DESCRIPTION
//
//      Create, bind, and listen on a Unix domain socket.
//
// RETURN VALUE
//
//      Returns the associated Unix file descriptor.
//
//*****************************************************************************
{
    int const fd = ::socket( AF_LOCAL, SOCK_STREAM, 0 );
    if ( fd == -1 ) {
        error() << "Unix socket() failed" << error_string;
        ::exit( Exit_No_Unix_Socket );
    }
    struct sockaddr_un addr;
    ::memset( &addr, 0, sizeof addr );
    addr.sun_family = AF_LOCAL;
    ::strncpy( addr.sun_path, socket_file_name, sizeof( addr.sun_path )-1 );
    // The socket file can not already exist prior to the bind().
    if ( ::unlink( socket_file_name ) == -1 && errno != ENOENT ) {
        error() << "can not delete \"" << socket_file_name << '"'
                << error_string;
        ::exit( Exit_No_Unlink );
    }
    BIND_SOCKET(fd,addr,Unix);
    return fd;
}

//*****************************************************************************
//
// SYNOPSIS
//
    void reset_socket( int fd )
//
// DESCRIPTION
//
//      Set the file descriptor for the TCP socket to be reset upon close(2) by
//      using the SO_LINGER socket option.  From [Stevens 1998], p. 187,
//      "SO_LINGER Socket Option":
//
//          If l_onoff is nonzero and l_linger is 0, TCP aborts the connection
//          when it is closed.  That is, TCP discards any data still remaining
//          in the socket send buffer and sends an RST to the peer, not the
//          normal four-packet connection termination sequence.
//
//      Note: this is not implemented in Linux 2.2.x kernels so the normal
//      four-packet sequence is done instead.
//
// PARAMETERS
//
//      fd  The file descriptor of the socket.
//
//*****************************************************************************
{
    struct linger li;
    li.l_onoff  = 1;
    li.l_linger = 0;
    ::setsockopt( fd, SOL_SOCKET, SO_LINGER,
        reinterpret_cast<char const*>( &li ), sizeof li
    );
}

//*****************************************************************************
//
// SYNOPSIS
//
        void set_signal_handlers()
//
// DESCRIPTION
//
//      Set the disposition for various signals.
//
//*****************************************************************************
{
    struct sigaction sa;
    ::memset( &sa, 0, sizeof sa );
    //
    // Ignore SIGPIPE when we try to sent to clients that are no longer
    // connected; instead, we deal with the EPIPE error.
    //
    sa.sa_handler = SIG_IGN;
    ::sigaction( SIGPIPE, &sa, 0 );
}

/* vim:set et sw=4 ts=4: */
