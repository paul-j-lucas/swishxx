/*
**      SWISH++
**      src/iso8859-1.h
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

#ifndef iso8859_1_H
#define iso8859_1_H

/**
 * Characters from the ISO 8859-1 character set mapped to 7-bit ASCII.
 */
extern char const iso8859_1_map[ 256 ];

///////////////////////////////////////////////////////////////////////////////

/**
 * Converts an 8-bit ISO 8859-1 (Latin 1) character to its closest 7-bit ASCII
 * equivalent.  (This mostly means that accents are stripped.)
 *
 * This function exists to ensure that the value of the character used to
 * index the \c iso8859_1_map[] vector declared above is unsigned.
 *
 * See also:
 *    International Standards Organization.  "ISO 8859-1: Information
 *    Processing -- 8-bit single-byte coded graphic character sets -- Part 1:
 *    Latin alphabet No. 1," 1987.
 *
 * @param c The ISO 8859-1 character to be converted.
 * @return Returns the character that is the closest ASCII equivalent.
 */
inline char iso8859_1_to_ascii( char c ) {
  return iso8859_1_map[ static_cast<unsigned char>( c ) ];
}

///////////////////////////////////////////////////////////////////////////////

#endif /* iso8859_1_H */
/* vim:set et sw=2 ts=2: */
