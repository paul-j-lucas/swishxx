/*
**	SWISH++
**	conf_string.c
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
#include <iostream>

// local
#include "conf_string.h"
#include "exit_codes.h"

extern char const*	me;

//*****************************************************************************
//
// SYNOPSIS
//
	conf<char const*>::conf( char const *name, char const *default_value ) :
//
// DESCRIPTION
//
//	Construct a conf<char const*> setting its name and default value.
//
// PARAMETERS
//
//	name		The name of the configuration variable.
//
//	default_value	Its default value.
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
	/* virtual */ void conf<char const*>::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse a single string value from the line of text.
//
// PARAMETERS
//
//	line	The line of text to be parsed.
//
//*****************************************************************************
{
	if ( !line || !*line ) {
		cerr << error << '"' << name() << "\" has no value" << endl;
		::exit( Exit_Config_File );
	}
	value_ = ::strdup( line );
}
