/*
**	SWISH++
**	FilterExtension.c
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
#include "FilterExtension.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	FilterExtension::FilterExtension() :
//
// DESCRIPTION
//
//	Construct (initialize) a FilterExtension: load the extension map
//	with default common filters.
//
//*****************************************************************************
	conf_var( "FilterExtension" )
{
#if 0	/* I'm not convinced that default filters are a good idea. */

	static char const *const default_filter_table[] = {
		"gz",	"gunzip -c %f > @E",
		"Z",	"uncompress -c %f > @E",
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
	/* virtual */ void FilterExtension::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse a FilterExtension configuration file line.  The format of such
//	a line is:
//
//		ext	command
//
//	where "ext" is a filename extension and "command" is the command-line
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
	char const *const ext = ::strtok( line, " \r\t" );
	if ( !ext ) {
		cerr << error << "no filename extension" << endl;
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
	// a @ meaning the target filename.
	//
	bool	found_target = false;
	int	substitutions = 0;

	for ( register char const*
		s = command; *s && ( s = ::strpbrk( s, "%@" ) ); ++s
	) {
		if ( *s == '@' )
			if ( found_target ) {
				cerr << error << "more than one @" << endl;
				::exit( Exit_Config_File );
			} else
				found_target = true;

		switch ( s[1] ) {
			case 'e':
			case 'E':
			case 'f':
				++substitutions;
				continue;
		}
		cerr << error << "non-[eEf] character after " << *s << endl;
		::exit( Exit_Config_File );
	}

	if ( substitutions < 2 ) {
		cerr	<< error << "at least two substitutions are required"
			<< endl;
		::exit( Exit_Config_File );
	}
	if ( !found_target ) {
		cerr << error << "filter does not contain required @" << endl;
		::exit( Exit_Config_File );
	}

	map_.insert( map_type::value_type(
		::strdup( ext ), value_type( ::strdup( command ) )
	) );
}
