/*
**      SWISH++
**      src/ChangeDirectory.cpp
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
#include "exit_codes.h"
#include "ChangeDirectory.h"

// standard
#include <cerrno>
#include <cstdio>                       /* for strerror(3) */
#include <unistd.h>                     /* for chdir(2) */

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void ChangeDirectory::parse_value( char *line ) {
  conf<string>::parse_value( line );
  auto const dir = operator char const*();

  if ( ::chdir( dir ) == -1 ) {
    error() << '"' << dir << "\": " << strerror( errno ) << endl;
    ::exit( Exit_No_Chdir );
  }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
