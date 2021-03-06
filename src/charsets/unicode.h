/*
**      SWISH++
**      src/charsets/unicode.h
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

#ifndef unicode_H
#define unicode_H

// local
#include "iso8859-1.h"

// standard
#include <cstdint>

///////////////////////////////////////////////////////////////////////////////

typedef uint32_t ucs4;

/**
 * Convert a 32-bit Unicode character to its closest 7-bit ASCII equivalent.
 * (This mostly means that accents are stripped.)  If there is no closest
 * equivalent, ' ' (space) is returned.
 *
 * This function exists to ensure that the value of the character used to index
 * \c iso8859_1_map[] is within range.
 *
 * See also:
 *    International Standards Organization.  "ISO 8859-1: Information
 *    Processing -- 8-bit single-byte coded graphic character sets -- Part 1:
 *    Latin alphabet No. 1," 1987.
 *
 * @param c The character to be converted.
 * @return Returns the equivalent ASCII character.
 */
inline char unicode_to_ascii( ucs4 c ) {
  return  c < sizeof( iso8859_1_map ) / sizeof( iso8859_1_map[0] ) ?
          iso8859_1_to_ascii( c ) : ' ';
}

///////////////////////////////////////////////////////////////////////////////

#endif /* unicode_H */
/* vim:set et sw=2 ts=2: */
