/*
**	SWISH++
**	conf_int.c
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
#include "conf_int.h"
#include "exit_codes.h"
#include "util.h"

extern char const*	me;

//*****************************************************************************
//
// SYNOPSIS
//
	conf<int>::conf( char const *name,
		int default_value, int min, int max
	) :
//
// DESCRIPTION
//
//	Construct (initialize) a conf<int>.
//
// PARAMETERS
//
//	name	The name of the configuration variable.
//
//*****************************************************************************
	conf_var( name ),
	default_value_( default_value ), min_( min ), max_( max ),
	value_( default_value )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
	conf<int>& conf<int>::operator=( int new_value )
//
// DESCRIPTION
//
//	Assign a new value to the configuration variable, but only if its value
//	is within the legal range; otherwise complain.
//
// PARAMETERS
//
//	new_value	The potential new value.
//
//*****************************************************************************
{
	if ( new_value >= min_ && new_value <= max_ ) {
		value_ = new_value;
		return *this;
	}

	cerr	<< error << '"' << name() << "\" value \""
		<< new_value << "\" not in range [" << min_ << '-';

	if ( max_ == INT_MAX )
		cerr << "infinity";
	else
		cerr << max_;

	cerr << ']' << endl;
	::exit( Exit_Config_File );
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	void conf<int>::parse_value( char const *var_name, char *line )
//
// DESCRIPTION
//
//	Parse an integer value from a configuration file line.  If successful,
//	assign the value to ourselves; otherwise complain.  The string
//	"infinity" (regardless of case) is accepted as a legal value.
//
// PARAMETERS
//
//	var_name	The name of the configuration variable.
//
//	line		The line to be parsed.
//
//*****************************************************************************
{
	if ( !line || !*line ) {
		cerr << error << '"' << name() << "\" has no value" << endl;
		::exit( Exit_Config_File );
	}
	if ( !::strcmp( to_lower( line ), "infinity" ) ) {
		operator=( INT_MAX );
		return;
	}
	int const n = ::atoi( line );
	if ( n || *line == '0' ) {
		operator=( n );
		return;
	}

	cerr << error << '"' << name() << "\" has a non-numeric value" << endl;
	::exit( Exit_Config_File );
}
