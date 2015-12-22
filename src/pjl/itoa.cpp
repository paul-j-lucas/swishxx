/*
**      PJL C++ Library
**      itoa.c
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
#include "char_buffer_pool.h"
#include "itoa.h"
#include "platform.h"

using namespace std;

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        char const* ltoa( register long n )
//
// DESCRIPTION
//
//      Converts a long integer to a string.  The string returned is from an
//      internal pool of string buffers.  The time you get into trouble is if
//      you hang on to more than Num_Buffers strings.  This doesn't normally
//      happen in practice, however.
//
// PARAMETERS
//
//      n   The long integer to be converted.
//
// RETURN VALUE
//
//      A pointer to the string.
//
// CAVEAT
//
//      This function is NOT thread-safe because it uses char_buffer_pool which
//      isn't.
//
// SEE ALSO
//
//      Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language,
//      2nd ed."  Addison-Wesley, Reading, MA, 1988.  pp. 63-64.
//
//*****************************************************************************
{
    static char_buffer_pool<25,5> buf;
    register char   *s = buf.next();
    bool const      is_neg = n < 0;

    if ( is_neg ) n = -n;
    do {                                // generate digits in reverse
        *s++ = n % 10 + '0';
    } while ( n /= 10 );
    if ( is_neg ) *s++ = '-';
    *s = '\0';

    // now reverse the string
    for ( register char *t = buf.current(); t < s; ++t ) {
        char const tmp = *--s; *s = *t; *t = tmp;
    }

    return buf.current();
}

} // namespace PJL
/* vim:set et sw=4 ts=4: */
