/*
**      SWISH++
**      src/IncludeFile.cpp
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
#include "IncludeFile.h"
#include "util.h"                       /* for error(), new_strdup() */

// standard
#include <cstdlib>                      /* for exit(3) */
#include <cstring>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void IncludeFile::parse_value( char *line ) {
  auto const mod_name = ::strtok( line, " \r\t" );
  if ( !mod_name ) {
    error() << "no indexer module name\n";
    ::exit( Exit_Config_File );
  }

  if ( indexer *const i = indexer::find_indexer( mod_name ) ) {
    for ( char const *s; (s = ::strtok( nullptr, " \r\t" )); )
      insert( new_strdup( s ), i );
  } else {
    error() << '"' << mod_name << "\": no such indexing module\n";
    ::exit( Exit_Config_File );
  }
}

void IncludeFile::reset() {
  clear();
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
