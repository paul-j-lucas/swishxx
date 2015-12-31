/*
**      SWISH++
**      src/conf_set.cpp
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
#include "conf_set.h"

// standard
#include <cstring>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void conf_set::parse_value( char *line ) {
  for ( char const *s; (s = ::strtok( line, " \r\t" )); line = nullptr )
    insert( s );
}

void conf_set::reset() {
  clear();
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
