/*
**	SWISH++
**	Group.c
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
#include <sys/types.h>			/* needed by FreeBSD systems */
#include <grp.h>

// local
#include "platform.h"
#include "Group.h"
#include "exit_codes.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void Group::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse a group name and look it up to ensure it's valid.
//
// PARAMETERS
//
//	line	The line of text to be parsed.
//
//*****************************************************************************
{
	conf<string>::parse_value( line );
	char const *const group_name = operator char const*();
	struct group const *const g = ::getgrnam( group_name );
	if ( !g ) {
		error()	<< '"' << name() << "\" value of \""
			<< group_name << "\" does not exist" << endl;
		::exit( Exit_No_Group );
	}
	gid_ = g->gr_gid;
}
