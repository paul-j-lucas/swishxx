/*
**      SWISH++
**      src/charsets/utf16.cpp
**
**      Copyright (C) 2002-2015  Paul J. Lucas
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
#include "encoded_char.h"
#include "unicode.h"

///////////////////////////////////////////////////////////////////////////////

encoded_char_range::value_type
charset_utf16be( encoded_char_range::const_pointer,
                 encoded_char_range::const_pointer &c,
                 encoded_char_range::const_pointer end ) {
  if ( c == end || c+1 == end )
    return ' ';
  ucs4 const u = (static_cast<ucs4>( c[0] ) << 8) | c[1];
  c += 2;
  return unicode_to_ascii( u );
}

encoded_char_range::value_type
charset_utf16le( encoded_char_range::const_pointer,
                 encoded_char_range::const_pointer &c,
                 encoded_char_range::const_pointer end ) {
  if ( c == end || c+1 == end )
    return ' ';
  ucs4 const u = (static_cast<ucs4>( c[1] ) << 8) | c[0];
  c += 2;
  return unicode_to_ascii( u );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
