/*
**	SWISH++
**	FilesGrow.c
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
#include "FilesGrow.h"
#include "exit_codes.h"

extern char const*	me;

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	void FilesGrow::parse_value( char const *var_name, char *line )
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
	conf<int>::parse_value( var_name, line );
	is_percentage_ = !!::strchr( line, '%' );
}
