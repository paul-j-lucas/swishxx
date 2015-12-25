/*
**      SWISH++
**      src/enc_int.h
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

#ifndef enc_int_H
#define enc_int_H

// local
#include "pjl/omanip.h"

// standard
#include <iostream>

///////////////////////////////////////////////////////////////////////////////

/**
 * Decodes an integer from an encoded byte sequence.  See the comment for
 * \c enc_int() for details of the encoding scheme.
 *
 * @param p A pointer to the start of the encoded integer.  After an integer is
 * decoded, it is left one past the last byte.
 * @return Returns the decoded integer.
 */
int dec_int( unsigned char const *&p );

/**
 * Writes an <code>unsigned long</code> to the given ostream in an encoded
 * format.  The format uses a varying number of bytes.  For a given byte, only
 * the lower 7 bits are used for data; the high bit, if set, is used to
 * indicate whether the integer continues into the next byte.  The encoded
 * integer is written to the given ostream starting with the most significant
 * byte.
 *
 * @param o The ostream to write to.
 * @param n The long to be written.
 * @return Returns \a o.
 */
std::ostream& enc_int( std::ostream &o, unsigned long n );

inline PJL::omanip<unsigned long> enc_int( unsigned long n ) {
  return PJL::omanip<unsigned long>( enc_int, n );
}

///////////////////////////////////////////////////////////////////////////////

#endif /* enc_int_H */
/* vim:set et sw=2 ts=2: */
