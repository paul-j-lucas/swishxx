/*
**	SWISH++
**	SocketTimeout.h
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

#ifndef SocketTimeout_H
#define SocketTimeout_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class SocketTimeout : public conf<int>
//
// DESCRIPTION
//
//	A SocketTimeout is-a conf<int> containing the number of seconds a
//	client has to complete a search request before being disconnected.
//	This is to prevent a client from connecting, not completing a request,
//	and causing the thread servicing the request to wait forever.
//
//	This is the same as index's -o command-line option.
//
//*****************************************************************************
{
public:
	SocketTimeout() :
		conf<int>( "SocketTimeout", SocketTimeout_Default, 2 ) { }
	CONF_INT_ASSIGN_OPS( SocketTimeout )
};

#endif	/* SocketTimeout_H */

#endif	/* SEARCH_DAEMON */
