/*
**      SWISH++
**      src/WordThreshold.c
**
**      Copyright (C) 2001  Paul J. Lucas
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
#include <sys/types.h>                  /* needed by FreeBSD systems */
#include <cstdlib>                      /* for atoi(3), exit(3) */
#include <unistd.h>                     /* for geteuid(2) */

// local
#include "exit_codes.h"
#include "util.h"
#include "WordThreshold.h"

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        void WordThreshold::parse_value( char *line )
//
// DESCRIPTION
//
//      Parse a word threshold value, but allow only the super-user to specify
//      a value larger than the default.
//
// PARAMETERS
//
//      line    The line of text to be parsed.
//
//*****************************************************************************
{
    conf<int>::parse_value( line );
    if ( operator int() > WordThreshold_Default && ::geteuid() != 0 ) {
        error() << "value may be increased only by root: permission denied"
                << endl;
        ::exit( Exit_Not_Root );
    }
}
/* vim:set et sw=4 ts=4: */
