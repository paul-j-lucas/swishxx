/*
**      SWISH++
**      src/TempDirectory.cpp
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
#include "swishxx-config.h"
#include "TempDirectory.h"

// standard
#include <cstdlib>                      /* for getenv(3) */

using namespace std;

///////////////////////////////////////////////////////////////////////////////

char const* TempDirectory::default_value() {
  auto const tmpdir = ::getenv( "TMPDIR" );
  return tmpdir ? tmpdir : TempDirectory_Default;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
