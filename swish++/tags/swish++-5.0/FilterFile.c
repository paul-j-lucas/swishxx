/*
**	SWISH++
**	FilterFile.c
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
#include <cstdlib>			/* for exit(2) */
#include <cstring>

// local
#include "exit_codes.h"
#include "FilterFile.h"
#include "platform.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	FilterFile::FilterFile() :
//
// DESCRIPTION
//
//	Construct (initialize) a FilterFile: load the pattern map with
//	default common filters.
//
//*****************************************************************************
	conf_var( "FilterFile" )
{
#if 0	/* I'm not convinced that default filters are a good idea. */

	static char const *const default_filter_table[] = {
		"gz",	"gunzip -c %f > @%F",
		"Z",	"uncompress -c %f > @%F",
		0
	};

	for ( register char const *const *p = default_filter_table; *p; p += 2 )
		map_.insert( map_type::value_type( p[0], value_type( p[1] ) ) );
#endif
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void FilterFile::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse a FilterFile configuration file line.  The format of such a line
//	is:
//
//		pattern	command
//
//	where "pattern" is a filename pattern and "command" is the command-line
//	for executing the filter on a file.
//
//	Furthermore, ensure the filter contains % and @ filename substitutions.
//
// PARAMETERS
//
//	line	The line to be parsed.
//
//*****************************************************************************
{
	char const *const pattern = ::strtok( line, " \r\t" );
	if ( !pattern ) {
		cerr << error << "no filename pattern" << endl;
		::exit( Exit_Config_File );
	}
	char const *const command = ::strtok( 0, "\n" );
	if ( !command ) {
		cerr << error << "no filter command" << endl;
		::exit( Exit_Config_File );
	}

	//
	// Check a filter command's %@ substitutions to ensure they're valid,
	// that there are at least two of them and that exactly one of them is
	// a @ meaning the target filename.  Also ignore %% or @@ respresenting
	// literal @ or %, respectively.
	//
	bool	found_target = false;
	int	num_substitutions = 0;

	for ( register char const*
		s = command; *s && ( s = ::strpbrk( s, "%@" ) ); ++s
	) {
		if ( s[0] == s[1] ) {		// %% or @@ ...
			++s;			// ... skip past it
			continue;
		}

		if ( *s == '@' )
			if ( found_target ) {
				cerr << error << "more than one @" << endl;
				::exit( Exit_Config_File );
			} else {
				found_target = true;
				continue;
			}

		switch ( s[1] ) {
			case 'b':
			case 'B':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
				++num_substitutions;
				continue;
		}
		cerr << error << "non-[bBeEfF%] character after %" << endl;
		::exit( Exit_Config_File );
	}

	if ( num_substitutions < 1 ) {
		cerr << error << "at least 1 substitution is required" << endl;
		::exit( Exit_Config_File );
	}
	if ( !found_target ) {
		cerr << error << "filter does not contain required @" << endl;
		::exit( Exit_Config_File );
	}

	map_.insert( map_type::value_type(
		::strdup( pattern ), value_type( ::strdup( command ) )
	) );
}
