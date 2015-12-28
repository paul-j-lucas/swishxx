/*
**      SWISH++
**      src/conf_filter.cpp
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
#include "exit_codes.h"
#include "conf_filter.h"

// standard
#include <cstdlib>                      /* for exit(3) */
#include <cstring>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void conf_filter::parse_value( char *line ) {
  char const *const pattern = ::strtok( line, " \r\t" );
  if ( !pattern ) {
    error() << "no pattern\n";
    ::exit( Exit_Config_File );
  }
  char const *const command = ::strtok( 0, "\n" );
  if ( !command ) {
    error() << "no filter command\n";
    ::exit( Exit_Config_File );
  }

  //
  // Check a filter command's %@ substitutions to ensure they're valid, that
  // there are at least two of them, and that exactly one of them is a @
  // meaning the target filename.  Also ignore %% or @@ respresenting literal @
  // or %, respectively.
  //
  bool found_target = false;
  int num_substitutions = 0;

  for ( char const* s = command; *s && ( s = ::strpbrk( s, "%@" ) ); ++s ) {
    if ( s[0] == s[1] ) {               // %% or @@ ...
      ++s;                              // ... skip past it
      continue;
    }

    if ( *s == '@' ) {
      if ( found_target ) {
        error() << "more than one @\n";
        ::exit( Exit_Config_File );
      } else {
        found_target = true;
        continue;
      }
    }

    switch ( s[1] ) {
      case 'b':
      case 'B':
      case 'e':
      case 'E':
      case 'f':
      case 'F':
        ++num_substitutions;
        continue;
    }
    error() << "non-[bBeEfF%] character after %\n";
    ::exit( Exit_Config_File );
  } // for

  if ( num_substitutions < 1 ) {
    error() << "at least 1 substitution is required\n";
    ::exit( Exit_Config_File );
  }
  if ( !found_target ) {
    error() << "filter does not contain required @\n";
    ::exit( Exit_Config_File );
  }

  map_.insert( map_type::value_type(
    ::strdup( pattern ), value_type( ::strdup( command ) )
  ) );
}

void conf_filter::reset() {
  map_.clear();
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
