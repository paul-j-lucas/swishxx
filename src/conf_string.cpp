/*
**      SWISH++
**      src/conf_string.cpp
**
**      Copyright (C) 1998-2015  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "conf_string.h"
#include "exit_codes.h"
#include "util.h"                       /* for error() */

// standard
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <iostream>

using namespace std;

extern char const*  me;

///////////////////////////////////////////////////////////////////////////////

conf<string>::conf( char const *name, char const *default_value ) :
  conf_var( name ),
  default_value_( default_value ),
  value_( default_value )
{
  // do nothing else
}

void conf<string>::parse_value( char *line ) {
  if ( !line || !*line ) {
    error() << '"' << name() << "\" has no value\n";
    ::exit( Exit_Config_File );
  }

  //
  // If the first non-whitespace character is a quote and the last
  // non-whitespace character is the SAME quote, strip the quotes.
  //
  if ( *line == '\'' || *line == '"' ) {
    char *const last = line + ::strlen( line ) - 1;
    if ( *line == *last )
      ++line, *last = '\0';
  }

  value_ = line;
}

void conf<string>::reset() {
  value_ = default_value_;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
