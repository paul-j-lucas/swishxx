/*
**	SWISH++
**	SearchDaemon.h
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

#ifndef SearchDaemon_H
#define SearchDaemon_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class SearchDaemon : public conf<bool>
//
// DESCRIPTION
//
//	A SearchDaemon is-a conf<bool> containing the Boolean value indicating
//	whether "search" should run in the background as a daemon process.
//
//	This is the same as search's -b command-line option.
//
//*****************************************************************************
{
public:
	SearchDaemon() : conf<bool>( "SearchDaemon", false ) { }
	CONF_BOOL_ASSIGN_OPS( SearchDaemon )
};

#endif	/* SearchDaemon_H */

#endif	/* SEARCH_DAEMON */
