/*
**	SWISH++
**	SocketFile.c
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
#include <cstdlib>			/* for exit(2) */

// local
#include "exit_codes.h"
#include "SocketFile.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	SocketFile& SocketFile::operator=( char const *s )
//
// DESCRIPTION
//
//	Assign a string value.  Ensure that it's a full path (begins with a
//	'/').
//
// PARAMETERS
//
//	s	The string to be parsed.
//
// RETURN VALUE
//
//	Returns a reference to itself as is customary.
//
//*****************************************************************************
{
	conf<string>::operator=( s );
	if ( *operator char const*() != '/' ) {
		cerr << error << "must be a full path" << endl;
		::exit( Exit_Config_File );
	}
	return *this;
}

#endif	/* SEARCH_DAEMON */
