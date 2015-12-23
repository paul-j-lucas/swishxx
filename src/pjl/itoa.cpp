/*
**      PJL C++ Library
**      itoa.cpp
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

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

char const* ltoa( long n ) {
  static char_buffer_pool<25,5> buf;
  char        *s = buf.next();
  bool const  is_neg = n < 0;

  if ( is_neg ) n = -n;
  do {                                  // generate digits in reverse
    *s++ = n % 10 + '0';
  } while ( n /= 10 );
  if ( is_neg ) *s++ = '-';
  *s = '\0';

  // now reverse the string
  for ( char *t = buf.current(); t < s; ++t ) {
    char const tmp = *--s; *s = *t; *t = tmp;
  }

  return buf.current();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL
/* vim:set et sw=2 ts=2: */
