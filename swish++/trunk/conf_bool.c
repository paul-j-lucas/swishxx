/*
**	SWISH++
**	conf_bool.c
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
#include <iostream>

// local
#include "conf_bool.h"
#include "exit_codes.h"
#include "util.h"

extern char const*	me;

//*****************************************************************************
//
// SYNOPSIS
//
	conf<bool>::conf( char const *name, bool default_value ) :
//
// DESCRIPTION
//
//	Construct (initialize) a conf<bool>.
//
// PARAMETERS
//
//	name	The name of the configuration variable.
//
//*****************************************************************************
	conf_var( name ),
	default_value_( default_value ),
	value_( default_value )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	void conf<bool>::parse_value( char const *var_name, char *line )
//
// DESCRIPTION
//
//	Parse a Boolean value from the line of text.  Acceptable values
//	(regardless of case) are: f, false, n, no, off, on, t, true, y, yes
//
// PARAMETERS
//
//	var_name	The name of the configuration variable.
//
//	line		The line of text to be parsed.
//
//*****************************************************************************
{
	char const *const lower = to_lower( line );

	if ( *lower ) {
		if (	!::strcmp( lower, "false" ) ||
			!::strcmp( lower, "no" ) ||
			!::strcmp( lower, "off" ) ||
			( lower[1] == '\0' && (*lower == 'f' || *lower == 'n') )
		) {
			operator=( false );
			return;
		}
		if (	!::strcmp( lower, "true" ) ||
			!::strcmp( lower, "on" ) ||
			!::strcmp( lower, "yes" ) ||
			( lower[1] == '\0' && (*lower == 't' || *lower == 'y') )
		) {
			operator=( true );
			return;
		}
	}
	cerr << error << '"' << name() << "\" is not one of: "
		"f, false, n, no, off, on, t, true, y, yes" << endl;
	::exit( Exit_Config_File );
}
