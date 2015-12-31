/*
**      SWISH++
**      src/IncludeMeta.cpp
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
#include "config.h"
#include "IncludeMeta.h"
#include "exit_codes.h"
#include "util.h"                       /* for error(), new_strdup() */

// standard
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <memory>                       /* for unique_ptr */

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

void IncludeMeta::parse_value( char *line ) {
  //
  // Parses the line of text by splitting it into words that are separated by
  // whitespace.  Additionally, "words" can be further split by '=' to do meta
  // name reassignment, e.g.: adr=address.
  //
  unique_ptr<char[]> lower( to_lower_r( line ) );
  char *p = lower.get();
  for ( char const *meta_name; (meta_name = ::strtok( p, " \r\t" ));
        p = nullptr ) {
    //
    // See if the meta name contains a reassignment: if so, chop it at the '='.
    //
    // The const_cast<>() is needed for Sun's (wrong!) implementation of
    // strchr() returning char const*.  Its use is harmless for other
    // compilers.
    //
    char *reassign = const_cast<char*>( ::strchr( meta_name, '=' ) );
    if ( reassign ) {
      *reassign = '\0';
      if ( !*++reassign ) {
        error() << "name expected after '='\n";
        ::exit( Exit_Config_File );
      }
    }
    char *const m = new_strdup( meta_name );
    insert( value_type( m, reassign ? new_strdup( reassign ) : m ));
  } // for
}

void IncludeMeta::reset() {
  clear();
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
