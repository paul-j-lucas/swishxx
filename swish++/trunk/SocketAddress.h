/*
**	SWISH++
**	SocketAddress.h
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

#ifdef	SEARCH_DAEMON

#ifndef SocketAddress_H
#define SocketAddress_H

// standard
#include <arpa/inet.h>			/* for htonl(3), INADDR_ANY */
#include <netinet/in.h>			/* for struct in_addr */

// local
#include "config.h"
#include "conf_string.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class SocketAddress : public conf<std::string>
//
// DESCRIPTION
//
//	A SocketAddress is-a conf<std::string> containing the IP address
//	(maybe) and the port number of the TCP socket to use when 'search' is
//	running as a daemon.
//
//	This is the same as search's -a command-line option.
//
//*****************************************************************************
{
public:
	SocketAddress() :
		conf<std::string>( "SocketAddress" ),
		port_( SocketPort_Default ) {
			addr_.s_addr = htonl( INADDR_ANY );
		}

	CONF_STRING_ASSIGN_OPS( SocketAddress )

	struct in_addr	addr() const { return addr_; }
	int		port() const { return port_; }
private:
	struct in_addr	addr_;
	int		port_;

	void		convert_host( char const *hostname_or_ip );
	virtual void	parse_value( char *line );
};

extern SocketAddress socket_address;

#endif	/* SocketAddress_H */

#endif	/* SEARCH_DAEMON */
