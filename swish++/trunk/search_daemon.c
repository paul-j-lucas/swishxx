/*
**	SWISH++
**	search_daemon.c
**
**	Copyright (C) 1998  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
** 
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
** 
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef	SEARCH_DAEMON

// standard
#include <arpa/inet.h>			/* for Internet networking stuff */
#include <cerrno>
#include <cstdlib>			/* for exit(2) */
#include <cstring>
#include <ctime>			/* needed by sys/resource.h */
#include <fstream>
#include <iostream>
#include <sys/resource.h>		/* for RLIMIT_* */
#include <sys/socket.h>			/* for bind(3), socket(3), etc. */
#include <sys/un.h>			/* for sockaddr_un */
#include <unistd.h>			/* for fork(2), setsid(2), unlink(2) */

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif

// local
#include "exit_codes.h"
#include "platform.h"
#include "search_thread.h"
#include "util.h"			/* for max_out_limit() */

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void become_daemon(
		char const *pid_file_name,
		char const *socket_file_name,
		int socket_queue_size, int socket_timeout,
		int min_threads, int max_threads, int thread_timeout
	)
//
// DESCRIPTION
//
//	Do things needed to becomd a daemon process: increase resource limits,
//	create a socket, detach from the terminal, record our PID, change to
//	the root directory, and finally service requests.  This function never
//	returns.
//
// PARAMETERS
//
//	pid_file_name		The full path to the file to record the
//				daemon's process ID in.
//
//	socket_file_name	The full path to the file to use for the Unix
//				domain socket.
//
//	socket_queue_size	Maximum number of queued connections for a
//				socket.  (See the comment in config.h.)
//
//	socket_timeout		The number of seconds a client has to complete
//				a search request.
//
//	min_threads		The minimum number of threads to maintain in
//				the thread pool.
//
//	max_threads		The maximum number of threads to allow to exist
//				in the thread pool.
//
//	thread_timeout		The number of seconds until an idle spare
//				thread times out and destroys itself.
//
// SEE ALSO
//
//	W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
//	Addison-Wesley, Reading, MA, 1993.
//
//	---.  "Unix Network Programming, Vol 1, 2nd ed."  Prentice-Hall, Upper
//	Saddle River, NJ, 1998.  Chapter 14.
//
//*****************************************************************************
{
	////////// Increase resource limits (hopefully) ///////////////////////

#ifdef	RLIMIT_CPU				/* SVR4, 4.3+BSD */
	//
	// Max-out the amount of CPU time we can run since we will be a daemon
	// and will run indefinitely.
	//
	max_out_limit( RLIMIT_CPU );
#endif
#ifdef	RLIMIT_NOFILE				/* SVR4 */
	//
	// Max-out the number of file descriptors we can have open to be able
	// to service as many clients concurrently as possible.
	//
	max_out_limit( RLIMIT_NOFILE );
#elif	defined( RLIMIT_OFILE )			/* 4.3+BSD name for NOFILE */
	max_out_limit( RLIMIT_OFILE );
#endif
	////////// Create a socket ////////////////////////////////////////////

	int const sock_fd = ::socket( AF_LOCAL, SOCK_STREAM, 0 );
	if ( sock_fd == -1 ) {
		cerr << error << "socket() failed" << error_string;
		::exit( Exit_No_Socket );
	}

	struct sockaddr_un addr;
	::memset( &addr, 0, sizeof addr );
	addr.sun_family = AF_LOCAL;
	::strncpy( addr.sun_path, socket_file_name, sizeof( addr.sun_path )-1 );

	// The socket file can not already exist prior to the bind().
	if ( ::unlink( socket_file_name ) == -1 && errno != ENOENT ) {
		cerr	<< error << "can not delete \"" << socket_file_name
			<< '"' << error_string;
		::exit( Exit_No_Unlink );
	}
	if ( ::bind( sock_fd, (struct sockaddr*)&addr, sizeof addr ) == -1 ) {
		cerr << error << "bind() failed" << error_string;
		::exit( Exit_No_Bind );
	}
	if ( ::listen( sock_fd, socket_queue_size ) == -1 ) {
		cerr << error << "listen() failed" << error_string;
		::exit( Exit_No_Listen );
	}

#ifndef DEBUG_threads
	////////// Detach from terminal ///////////////////////////////////////
	//
	// From [Stevens 1993], p. 417, "Coding Rules":
	//
	//	The first thing to do is call fork and have the parent exit.
	//	This does several things.  First, if the daemon was started as
	//	a simple shell command, having the parent terminate makes the
	//	shell think that the command is done.  Second, the child
	//	inherits the process group ID of the parent but gets a new
	//	process ID, so we're guaranteed that the child is not a process
	//	group leader.  This is a prerequisite for the call to setsid
	//	that is done next.
	//
	pid_t const child_pid = ::fork();
	if ( child_pid == -1 ) {
		cerr << error << "fork() failed" << error_string;
		::exit( Exit_No_Fork );
	}
	if ( child_pid > 0 )			// parent process ...
		::exit( Exit_Success );		// ... just exit as described

	//
	// If requested, record our PID to a file.
	//
	if ( pid_file_name && *pid_file_name ) {
		ofstream pid_file( pid_file_name );
		if ( !pid_file ) {
			cerr	<< error << '"' << pid_file_name << "\": "
				<< error_string;
			::exit( Exit_No_Write_PID );
		}
		pid_file << ::getpid() << endl;
	}

	//
	// Ibid.:
	//
	//	Call setsid to create a new session.  The process (1) becomes a
	//	session leader of a new session, (2) becomes the process group
	//	leader of a new process group, and (3) has no controlling
	//	terminal.
	//
	::setsid();

	//
	// Ibid.:
	//
	//	Change the current working directory to the root directory.
	//	The current working directory inherited from the parent could
	//	be on a mounted filesystem.  Since daemons normally exist until
	//	the system is rebooted, if the daemon stays on a mounted
	//	filesystem, that filesystem can not be unmounted.
	//
	if ( ::chdir( "/" ) == -1 ) {
		cerr << error << "chdir() failed" << error_string;
		::exit( Exit_No_Change_Dir );
	}
#endif	/* DEBUG_threads */

	////////// Accept requests ////////////////////////////////////////////

	search_thread::socket_timeout = socket_timeout;
	thread_pool threads = thread_pool( new search_thread( threads ),
		min_threads, max_threads, thread_timeout
	);
	while ( true ) {
		struct sockaddr_un addr;
		PJL_SOCKLEN_TYPE addr_len = sizeof addr;
#		ifdef DEBUG_threads
		cerr << "waiting for request" << endl;
#		endif
		int const accept_fd = ::accept(
			sock_fd, (struct sockaddr*)&addr, &addr_len
		);
		if ( accept_fd == -1 ) {
			switch ( errno )
				case ECONNABORTED:	// POSIX.1g
				case EINTR:
				case EPROTO:		// SVR4
					continue;
			cerr << error << "accept() failed" << error_string;
			::exit( Exit_No_Accept );
		}
#		ifdef DEBUG_threads
		cerr << "dispatching request" << endl;
#		endif
		threads.new_task( accept_fd );
	}
}

#endif	/* SEARCH_DAEMON */
