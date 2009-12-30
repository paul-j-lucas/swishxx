/*
**	SWISH++
**	src/conf_set.c
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

// standard
#include <cstring>

// local
#include "conf_set.h"
#include "platform.h"

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void conf_set::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse the line of text by splitting it into words that are separated by
//	whitespace.
//
// PARAMETERS
//
//	line	The line of text to be parsed.
//
//*****************************************************************************
{
	for ( register char const *s; s = ::strtok( line, " \r\t" ); line = 0 )
		insert( s );
}
