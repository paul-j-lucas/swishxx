/*
**      SWISH++
**      src/conf_bool.cpp
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
#include "config.h"
#include "conf_bool.h"
#include "exit_codes.h"
#include "util.h"

// standard
#include <cstdlib>                      /* for exit(3) */
#include <iostream>

using namespace PJL;
using namespace std;

extern char const*  me;

///////////////////////////////////////////////////////////////////////////////

conf<bool>::conf( char const *name, bool default_value ) :
  conf_var( name ),
  default_value_( default_value ),
  value_( default_value )
{
  // do nothing else
}

void conf<bool>::parse_value( char *line ) {
  bool value;
  if ( parse( line, &value ) ) {
    operator=( value );
    return;
  }
  error() << '"' << name() << "\" is not one of: "
             "0, f, false, n, no, off, 1, on, t, true, y, yes\n";
  ::exit( Exit_Config_File );
}

void conf<bool>::reset() {
  value_ = default_value_;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
