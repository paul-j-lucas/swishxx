/*
**	SWISH++
**	SocketQueueSize.h
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

#ifndef SocketQueueSize_H
#define SocketQueueSize_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class SocketQueueSize : public conf<int>
//
// DESCRIPTION
//
//	A SocketQueueSize is-a conf<int> containing the maximum number of
//	connections allowed to queue by the kernel for a socket.
//
//	This is the same as search's -q command-line option.
//
// SEE ALSO
//
//	W. Richard Stevens.  "Unix Network Programming, Vol 1, 2nd ed."
//	Prentice-Hall, Upper Saddle River, NJ, 1998.  Section 4.5.
//
//*****************************************************************************
{
public:
	SocketQueueSize() :
		conf<int>( "SocketQueueSize", SocketQueueSize_Default, 1 ) { }
	CONF_INT_ASSIGN_OPS( SocketQueueSize )
};

#endif	/* SocketQueueSize_H */

#endif	/* SEARCH_DAEMON */
