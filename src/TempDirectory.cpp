/*
**      SWISH++
**      src/TempDirectory.c
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

// standard
#include <cstdlib>                              /* for getenv(3) */

// local
#include "config.h"
#include "TempDirectory.h"

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        char const* TempDirectory::default_value()
//
// DESCRIPTION
//
//      Determine the default value for the temporary directory.  If the
//      environment variable TMPDIR is set, use that; otherwise use the
//      compiled-in default.
//
// RETURN VALUE
//
//      Returns the default value to use for the temporary directory.
//
//*****************************************************************************
{
    char const *const tmpdir = ::getenv( "TMPDIR" );
    return tmpdir ? tmpdir : TempDirectory_Default;
}
/* vim:set et sw=4 ts=4: */
