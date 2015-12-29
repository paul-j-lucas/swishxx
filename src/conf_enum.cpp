/*
**      SWISH++
**      src/conf_enum.cpp
**
**      Copyright (C) 2000-2015  Paul J. Lucas
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
#include "conf_enum.h"
#include "exit_codes.h"
#include "util.h"

// standard
#include <cstdlib>                      /* for exit(3) */
#include <iostream>
#include <memory>                       /* for unique_ptr */

using namespace PJL;
using namespace std;

extern char const*  me;

///////////////////////////////////////////////////////////////////////////////

conf_enum::conf_enum( char const *name, char const *const legal_values[] ) :
  conf<std::string>( name, legal_values[0] ),
  legal_values_( legal_values )
{
  // do nothing else
}

bool conf_enum::is_legal( char const *value, ostream &err ) const {
  if ( *value ) {
    unique_ptr<char[]> const lower( to_lower_r( value ) );
    for ( auto v = legal_values_; *v; ++v )
      if ( !::strcmp( lower.get(), *v ) )
        return true;
  }
  err << error << '"' << name() << "\" is not one of: ";
  bool comma = false;
  for ( auto v = legal_values_; *v; ++v ) {
    if ( comma )
      err << ", ";
    else
      comma = true;
    err << *v;
  } // for
  err << '\n';
  return false;
}

void conf_enum::parse_value( char *line ) {
  if ( !is_legal( line ) )
    ::exit( Exit_Config_File );
  unique_ptr<char[]> lower( to_lower_r( line ) );
  conf<string>::parse_value( lower.get() );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
