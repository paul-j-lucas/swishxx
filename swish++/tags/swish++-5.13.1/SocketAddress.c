/*
**	SWISH++
**	SocketAddress.c
**
**	Copyright (C) 2000  Paul J. Lucas
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
#include <iostream>
#include <netdb.h>			/* for gethostbyname(3), et al */
#include <sys/types.h>			/* needed by FreeBSD systems */
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>			/* for atoi(3) */

// local
#include "exit_codes.h"
#include "platform.h"
#include "SocketAddress.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void SocketAddress::convert_host( char const *hostname_or_ip )
//
// DESCRIPTION
//
//	Parse either a hostname (foo.bar.com) or an IP address (123.45.67.89)
//	and convert it to a value suitable for use as an Internet address.  A
//	value of '*' means "any IP address."
//
// PARAMETERS
//
//	hostname_or_ip	The host name or IP address to be parsed.
//
// SEE ALSO
//
//	W. Richard Stevens.  "Unix Network Programming, Vol 1, 2nd ed."
//	Prentice-Hall, Upper Saddle River, NJ, 1998, section 9.3.
//
//*****************************************************************************
{
	if ( *hostname_or_ip == '*' ) {
		addr_.s_addr = htonl( INADDR_ANY );
		return;
	}

	struct hostent const *const host = ::gethostbyname( hostname_or_ip );
	if ( !host ) {
		error()	<< '"' << name() << "\" value of \"" << hostname_or_ip
			<< "\" is invalid\n";
		::exit( Exit_No_Host_or_IP );
	}

	//
	// The h_addr_list member is defined as "char**" even though it is
	// really "struct in_addr**" (this seems really brain damaged), so we
	// have to do an ugly cast.
	//
	addr_ = *reinterpret_cast<struct in_addr*>( host->h_addr_list[0] );
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void SocketAddress::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse the IP address (maybe) and port number from the line of text.
//
// PARAMETERS
//
//	line	The line of text to be parsed.
//
//*****************************************************************************
{
	conf<string>::parse_value( line );
	char *const colon = ::strchr( line, ':' );
	if ( colon ) {
		*colon = '\0';
		convert_host( line );
		port_ = ::atoi( colon + 1 );
	} else
		port_ = ::atoi( line );

	if ( !port_ )
		::exit( Exit_Config_File );
}
