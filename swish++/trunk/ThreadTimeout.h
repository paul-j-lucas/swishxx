/*
**	SWISH++
**	ThreadTimeout.h
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

#ifndef ThreadTimeout_H
#define ThreadTimeout_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class ThreadTimeout : public conf<int>
//
// DESCRIPTION
//
//	A ThreadTimeout is-a conf<int> containing the number of seconds until
//	and idle spare thread times out and destroys itself.
//
//	This is the same as index's -o command-line option.
//
//*****************************************************************************
{
public:
	ThreadTimeout() :
		conf<int>( "ThreadTimeout", ThreadTimeout_Default ) { }
	CONF_INT_ASSIGN_OPS( ThreadTimeout )
};

extern ThreadTimeout thread_timeout;

#endif	/* ThreadTimeout_H */

#endif	/* SEARCH_DAEMON */
