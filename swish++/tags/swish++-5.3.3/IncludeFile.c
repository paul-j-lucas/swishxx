/*
**	SWISH++
**	IncludeFile.c
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
#include "exit_codes.h"
#include "IncludeFile.h"
#include "platform.h"
#include "util.h"			/* for error() */

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void IncludeFile::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse the line of text of the form:
//
//		mod_name pattern1 pattern2 ...
//
// PARAMETERS
//
//	line	The line of text to be parsed.
//
//*****************************************************************************
{
	char const *const mod_name = ::strtok( line, " \r\t" );
	if ( !mod_name ) {
		error() << "no indexer module name\n";
		::exit( Exit_Config_File );
	}
	indexer *const i = indexer::find_indexer( mod_name );
	if ( !i ) {
		error() << '"' << mod_name << "\": no such indexing module\n";
		::exit( Exit_Config_File );
	}

	for ( register char const *s; s = ::strtok( 0, " \r\t" ); )
		insert( ::strdup( s ), i );
}