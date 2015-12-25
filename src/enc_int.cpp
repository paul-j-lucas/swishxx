/*
**      SWISH++
**      src/enc_int.cpp
**
**      Copyright (C) 2003-2015  Paul J. Lucas
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
#include "enc_int.h"

// standard
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

int dec_int( unsigned char const *&p ) {
  unsigned n = 0;
  do {
    n = (n << 7) | (*p & 0x7Fu);
  } while ( *p++ & 0x80u );
  return n;
}

ostream& enc_int( ostream &o, unsigned long n ) {
  unsigned char buf[ 20 ];
  //
  // Encode the long (in reverse because it's easier) just like atoi().
  //
  unsigned char *p = buf + sizeof buf;
  do {
    *--p = 0x80u | (n & 0x7Fu);
  } while ( n >>= 7 );
  buf[ sizeof buf - 1 ] &= 0x7Fu;       // clear last "continuation bit"

  return o.write( reinterpret_cast<char*>( p ), buf + sizeof buf - p );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
