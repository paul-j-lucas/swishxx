/*
**      SWISH++
**      src/vlq.h
**
**      Copyright (C) 2003-2016  Paul J. Lucas
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

#ifndef vlq_H
#define vlq_H

// local
#include "pjl/omanip.h"

// standard
#include <iostream>

namespace PJL {

/**
 * From Wikipedia <https://en.wikipedia.org/wiki/Variable-length_quantity>:
 *
 *    A variable-length quantity (VLQ) is a universal code that uses an
 *    arbitrary number of binary octets (eight-bit bytes) to represent an
 *    arbitrarily large integer. It was defined for use in the standard MIDI
 *    file format[1] to save additional space for a resource constrained
 *    system, and is also used in the later Extensible Music Format (XMF). A
 *    VLQ is essentially a base-128 representation of an unsigned integer with
 *    the addition of the eighth bit to mark continuation of bytes.
 */
namespace vlq {

///////////////////////////////////////////////////////////////////////////////

typedef unsigned long value_type;

/**
 * Decodes a VLQ.
 *
 * @param p A pointer to the start of the encoded integer.  After an integer is
 * decoded, it is left one past the last byte.
 * @return Returns the decoded integer.
 */
value_type decode( unsigned char const *&p );

/**
 * Writes a unsigned integer to the given ostream as a VLQ.
 *
 * @param o The ostream to write to.
 * @param n The unsigned integer to be written.
 * @return Returns \a o.
 */
std::ostream& encode( std::ostream &o, value_type n );

inline PJL::omanip<value_type> encode( value_type n ) {
  return PJL::omanip<value_type>( encode, n );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace vlq
} // namespace PJL

#endif /* vlq_H */
/* vim:set et sw=2 ts=2: */
