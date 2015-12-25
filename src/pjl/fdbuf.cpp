/*
**      PJL C++ Library
**      fdbuf.cpp
**
**      Copyright (C) 2001-2015  Paul J. Lucas
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
#include "config.h"
#include "fdbuf.h"

// standard
#include <cerrno>
#include <cstring>
#include <unistd.h>

using namespace std;

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

void fdbuf::init( int fd ) {
  fd_ = fd;
  setg( rbuf_, rbuf_ + buf_size, rbuf_ + buf_size );
  setp( wbuf_, wbuf_ + buf_size );
}

int fdbuf::sync() {
  int const len = pptr() - pbase();
  if ( write_buf( wbuf_, len ) != len )
    return -1;
  pbump( -len );
  return 0;
}

fdbuf::int_type fdbuf::overflow( int_type c ) {
  if ( sync() )
    return EOF;
  if ( c != EOF ) {
    *pptr() = c;
    pbump( 1 );
  }
  return c;
}

fdbuf::int_type fdbuf::underflow() {
  if ( gptr() == egptr() ) {
    //
    // The get-pointer is at the end-pointer meaning that the read-buffer is
    // exhausted: read more.
    //
    ssize_t bytes;
    while ( true ) {
      if ( (bytes = ::read( fd_, rbuf_, buf_size )) > 0 )
        break;                          // we read some bytes so stop
      if ( bytes < 0 && (errno == EAGAIN || errno == EINTR) )
        continue;
      return EOF;
    } // while
    setg( rbuf_, rbuf_, rbuf_ + bytes );
  }
  return *gptr();
}

streamsize fdbuf::write_buf( char const *buf, streamsize len ) {
  //
  // See:
  //    W. Richard Stevens.  "Unix Network Programming, Vol 1, 2nd ed."
  //    Prentice-Hall, Upper Saddle River, NJ, 1998.
  //
  streamsize total_bytes_written = 0;
  while ( true ) {
    ssize_t const bytes_written = ::write( fd_, buf, len );
    if ( bytes_written >= 0 ) {
      //
      // Account for partial-writes in case the Unix file descriptor happens to
      // be attached to a socket.  From [Stevens 1998], p. 77:
      //
      //      Stream sockets (e.g., TCP sockets) exhibit a behavior with the
      //      read and write functions that differs from normal file I/O.  A
      //      read or write on a stream socket might input or output fewer
      //      bytes than requested, but this is not an error condition.  The
      //      reason is that buffer limits might be reached for the kernel.
      //      All that is required is for the caller to invoke the read or
      //      write function again, to input or output the remaining bytes.
      //
      // Note that if the file descriptor isn't attached to a socket, the
      // entire buffer will be written, so the loop will exit.
      //
      total_bytes_written += bytes_written;
      if ( bytes_written == len )
        return total_bytes_written;
      len -= bytes_written;
      buf += bytes_written;
      continue;
    }
    if ( errno == EINTR )               // interrupt-proof
      continue;
    return -1;
  } // while
}

streamsize fdbuf::xsputn( char const *buf, streamsize len ) {
  if ( epptr() - pptr() >= len ) {
    //
    // The contents of buf will fit into the existing put-buffer so just put it
    // there.
    //
    ::memcpy( pptr(), buf, len );
    pbump( len );
    return len;
  }
  //
  // The contents of buf will not fit into the existing put-buffer so
  // syncronize what's currently there then write out the contents of buf
  // directly.
  //
  return sync() ? -1 : write_buf( buf, len );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL
/* vim:set et sw=2 ts=2: */
