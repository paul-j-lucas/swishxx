/*
**	SWISH++
**	conf_enum.c
**
**	Copyright (C) 2000  Paul J. Lucas
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
#include "auto_vec.h"
#include "conf_enum.h"
#include "exit_codes.h"
#include "platform.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

extern char const*	me;

//*****************************************************************************
//
// SYNOPSIS
//
	conf_enum::conf_enum(
		char const *name, char const *const legal_values[]
	) :
//
// DESCRIPTION
//
//	Construct (initialize) a conf_enum.
//
// PARAMETERS
//
//	name		The name of the configuration variable.
//
//	legal_values	The set of leval values.
//
//*****************************************************************************
	conf<std::string>( name, legal_values[0] ),
	legal_values_( legal_values )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void conf_enum::parse_value( char *line )
//
// DESCRIPTION
//
//	Parse an enum value from the line of text.  It must be one of the legal
//	values.
//
// PARAMETERS
//
//	line	The line of text to be parsed.
//
//*****************************************************************************
{
	auto_vec< char > const lower( to_lower_r( line ) );
	if ( *lower )
		for ( char const *const *v = legal_values_; *v; ++v )
			if ( !::strcmp( lower, *v ) ) {
				conf<string>::parse_value( lower );
				return;
			}

	cerr << error << '"' << name() << "\" is not one of: ";
	bool comma = false;
	for ( char const *const *value = legal_values_; *value; ++value ) {
		if ( comma )
			cerr << ", ";
		else
			comma = true;
		cerr << *value;
	}
	cerr << endl;
	::exit( Exit_Config_File );
}
