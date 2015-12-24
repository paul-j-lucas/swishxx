/*
**      SWISH++
**      src/conf_bool.cpp
**
**      Copyright (C) 1998  Paul J. Lucas
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
#include <memory>                       /* for unique_ptr */
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
  unique_ptr<char[]> const lower_ptr( to_lower_r( line ) );
  char const *const lower = lower_ptr.get();
  if ( *lower ) {
    if ( !::strcmp( lower, "false" ) ||
         !::strcmp( lower, "no" ) ||
         !::strcmp( lower, "off" ) ||
         ( lower[1] == '\0' && (*lower == 'f' || *lower == 'n') ) ) {
      operator=( false );
      return;
    }
    if ( !::strcmp( lower, "true" ) ||
         !::strcmp( lower, "on" ) ||
         !::strcmp( lower, "yes" ) ||
         ( lower[1] == '\0' && (*lower == 't' || *lower == 'y') ) ) {
      operator=( true );
      return;
    }
  }
  error() << '"' << name() << "\" is not one of: "
             "f, false, n, no, off, on, t, true, y, yes\n";
  ::exit( Exit_Config_File );
}

void conf<bool>::reset() {
  value_ = default_value_;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
