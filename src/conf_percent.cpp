/*
**	    SWISH++
**	    src/conf_percent.cpp
**
**	    Copyright (C) 2001-2015  Paul J. Lucas
**
**	    This program is free software; you can redistribute it and/or modify
**	    it under the terms of the GNU General Public License as published by
**	    the Free Software Foundation; either version 2 of the License, or
**	    (at your option) any later version.
**
**	    This program is distributed in the hope that it will be useful,
**	    but WITHOUT ANY WARRANTY; without even the implied warranty of
**	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	    GNU General Public License for more details.
**
**	    You should have received a copy of the GNU General Public License
**	    along with this program; if not, write to the Free Software
**	    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "config.h"
#include "conf_percent.h"
#include "exit_codes.h"

// standard
#include <cstdlib>                      /* for exit(3) */
#include <cstring>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void conf_percent::parse_value( char *line ) {
	conf<int>::parse_value( line );
	if ( (is_percentage_ = !!::strchr( line, '%' )) ) {
		int const value = *this;
		if ( value < 0 || value > 101 ) {
			error()	<< '"' << name() << "\" value \"" << value
              << "\" not in range [0-101]%\n";
			::exit( Exit_Config_File );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
