/*
**	PJL C++ Library
**	fdbuf.h
**
**	Copyright (C) 2001  Paul J. Lucas
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

// standard
#include <cerrno>
#include <cstring>
#include <unistd.h>

// local
#include "fdbuf.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
namespace PJL {
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void fdbuf::init( int fd )
//
// DESCRIPTION
//
//	Initialize an fdbuf.
//
//*****************************************************************************
{
	fd_ = fd;
	setg( rbuf_, rbuf_ + buf_size, rbuf_ + buf_size );
	setp( wbuf_, wbuf_ + buf_size );
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ int fdbuf::sync()
//
// DESCRIPTION
//
//	Synchronize the write buffer by writing it out.
//
// RETURN VALUE
//
//	On success, returns 0; on failure, returns -1.
//
//*****************************************************************************
{
	int const len = pptr() - pbase();
	if ( write_buf( wbuf_, len ) != len )
		return -1;
	pbump( -len );
	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ fdbuf::int_type fdbuf::overflow( int_type c )
//
// DESCRIPTION
//
//	This function is called when a single character is to be output.
//
// PARAMETERS
//
//	c	The single character to be stuffed into the buffer unless it is
//		EOF.
//
// RETURN VALUE
//
//	On success, returns the passed-in character; on failure, returns EOF.
//
//*****************************************************************************
{
	if ( sync() )
		return EOF;
	if ( c != EOF ) {
		*pptr() = c;
		pbump( 1 );
	}
	return c;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ fdbuf::int_type fdbuf::underflow()
//
// DESCRIPTION
//
//	This function is called when the buffer is underflowed, i.e., more
//	characters need to be read from the source.
//
// RETURN VALUE
//
//	Returns the next character from the source or EOF on error.
//
//*****************************************************************************
{
	if ( gptr() == egptr() ) {
		//
		// The get-pointer is at the end-pointer meaning that the read-
		// buffer is exhausted: read more.
		//
		ssize_t bytes;
		while ( true ) {
			if ( (bytes = ::read( fd_, rbuf_, buf_size )) > 0 ) {
				//
				// We read some bytes so stop.
				//
				break;
			}
			if ( bytes < 0 && (errno == EAGAIN || errno == EINTR) )
				continue;
			return EOF;
		}
		setg( rbuf_, rbuf_, rbuf_ + bytes );
	}
	return *gptr();
}

//*****************************************************************************
//
// SYNOPSIS
//
	streamsize fdbuf::write_buf( char const *buf, streamsize len )
//
// DESCRIPTION
//
//	Write a buffer to a Unix file descriptor.
//
// PARAMETERS
//
//	buf	The buffer to be written.
//
//	len	The length of the buffer.
//
// RETURN VALUE
//
//	On success, returns the number of bytes written; on failure, returns
//	-1.
//
// SEE ALSO
//
//	W. Richard Stevens.  "Unix Network Programming, Vol 1, 2nd ed."
//	Prentice-Hall, Upper Saddle River, NJ, 1998.
//
//*****************************************************************************
{
	streamsize total_bytes_written = 0;
	while ( true ) {
		ssize_t const bytes_written = ::write( fd_, buf, len );
		if ( bytes_written >= 0 ) {
			//
			// Account for partial-writes in case the Unix file
			// descriptor happens to be attached to a socket.  From
			// [Stevens 1998], p. 77:
			//
			//	Stream sockets (e.g., TCP sockets) exhibit a
			//	behavior with the read and write functions that
			//	differs from normal file I/O.  A read or write
			//	on a stream socket might input or output fewer
			//	bytes than requested, but this is not an error
			//	condition.  The reason is that buffer limits
			//	might be reached for the kernel.  All that is
			//	required is for the caller to invoke the read
			//	or write function again, to input or output the
			//	remaining bytes.
			//
			// Note that if the file descriptor isn't attached to a
			// socket, the entire buffer will be written, so the
			// loop will exit.
			//
			total_bytes_written += bytes_written;
			if ( bytes_written == len )
				return total_bytes_written;
			len -= bytes_written;
			buf += bytes_written;
			continue;
		}
		if ( errno == EINTR )		// interrupt-proof
			continue;
		return -1;
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	streamsize fdbuf::xsputn( char const *buf, streamsize len )
//
// DESCRIPTION
//
//	This function is called to output an entire buffer (as opposed to a
//	single character).
//
// RETURN VALUE
//
//	On success, returns the number of bytes written; on failure, returns
//	-1.
//
//*****************************************************************************
{
	if ( epptr() - pptr() >= len ) {
		//
		// The contents of buf will fit into the existing put-buffer so
		// just put it there.
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

#ifndef	PJL_NO_NAMESPACES
}
#endif
/* vim:set noet sw=8 ts=8: */
