/*
**	SWISH++
**	conf_var.c
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
#include <cctype>
#include <cstring>
#include <iostream>

// local
#include "conf_var.h"
#include "util.h"

extern char const	*me;

int conf_var::current_config_file_line_no_ = 0;

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ conf_var::map_type& conf_var::map_ref()
//
// DESCRIPTION
//
//	Define and initialize (exactly once) a static data member for
//	conf_var and return a reference to it.  The reason for this function
//	is to guarantee that the map is initialized before its first use
//	across all translation units, something that would not guaranteed if
//	it were a static data member initialized at file scope.
//
//	We also load the map with all configuration variable names so we can
//	tell the difference between a variable that doesn't exist (and that
//	we will complain about to the user) and one that simply isn't used in
//	a particular executable (and will be silently ignored).
//
// SEE ALSO
//
//	Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++
//	Reference Manual."  Addison-Wesley, Reading, MA.  p. 19.
//
//*****************************************************************************
{
	static map_type map;
	if ( map.empty() ) {
		map[ "ExcludeClass"	] = 0;
		map[ "ExcludeExtension"	] = 0;
		map[ "ExcludeMeta"	] = 0;
		map[ "FilesReserve"	] = 0;
		map[ "FilterExtension"	] = 0;
		map[ "FollowLinks"	] = 0;
		map[ "IncludeExtension"	] = 0;
		map[ "IncludeMeta"	] = 0;
		map[ "IndexFile"	] = 0;
		map[ "RecurseSubdirs"	] = 0;
		map[ "ResultsMax"	] = 0;
		map[ "StemWords"	] = 0;
		map[ "StopWordFile"	] = 0;
		map[ "TempDirectory"	] = 0;
		map[ "TitleLines"	] = 0;
		map[ "Verbosity"	] = 0;
		map[ "WordFilesMax"	] = 0;
		map[ "WordPercentMax"	] = 0;
	}
	return map;
}

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& conf_var::msg( char const *kind )
//
// DESCRIPTION
//
//	Emit the standard message preamble to standard error.
//
//*****************************************************************************
{
	cerr << me;
	if ( current_config_file_line_no_ ) {
		cerr << ": config file line " << current_config_file_line_no_;
		current_config_file_line_no_ = 0;
	}
	cerr << ": " << kind << ": ";
	return cerr;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void conf_var::parse_const_value( char const *line )
//
// DESCRIPTION
//
//	Parse a line that can't be modified by simply copying it and calling
//	parse_value() on the copy.
//
//*****************************************************************************
{
	char *const line_copy = new_strdup( line );
	parse_value( line_copy );
	delete[] line_copy;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void conf_var::parse_line( char *line, int line_no )
//
// DESCRIPTION
//
//	Parse a non-comment or non-blank line from a the configuration file,
//	the first word of which is the variable name.  Look up the variable
//	in our map and delegate the parsing of the rest of the line to an
//	instance of a derived class that knows how to parse its own line
//	format.
//
// PARAMETERS
//
//	line		A line from a configuration file to be parsed.
//
//	line_no		The line number of the line.
//
//*****************************************************************************
{
	current_config_file_line_no_ = line_no;
	::strtok( line, " \r\t" );		// just the variable name
	map_type::const_iterator const i = map_ref().find( line );
	if ( i == map_ref().end() ) {
		warn()	<< '"' << line
			<< "\" in config. file unrecognized; ignored\n";
		return;
	}
	if ( i->second ) {
		register char *value = ::strtok( 0, "\n" );
		while ( *value && isspace( *value ) )
			++value;
		i->second->parse_value( value );
	} // else
	//	This config. variable is not used by the current executable:
	//	silently ignore it.
}
