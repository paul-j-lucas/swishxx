/*
**      PJL C++ Library
**      itoa.h
**
**      Copyright (C) 1998-2015  Paul J. Lucas
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

#ifndef itoa_H
#define itoa_H

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * Converts a long integer to a string.  The string returned is from an
 * internal pool of string buffers.  The time you get into trouble is if you
 * hang on to more than Num_Buffers strings.  This doesn't normally happen in
 * practice, however.
 *
 * This function is \e not thread-safe because it uses char_buffer_pool which
 * isn't.
 *
 * See also:
 *    Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language, 2nd
 *    ed."  Addison-Wesley, Reading, MA, 1988.  pp. 63-64.
 *
 * @param n The long integer to be converted.
 * @return A pointer to the string.
 */
char const* ltoa( long n );

/**
 * Converts an integer to a string.  The string returned is from an internal
 * pool of string buffers.  The time you get into trouble is if you hang on to
 * more than Num_Buffers strings.  This doesn't normally happen in practice,
 * however.
 *
 * This function is \e not thread-safe because it uses char_buffer_pool which
 * isn't.
 *
 * See also:
 *    Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language, 2nd
 *    ed."  Addison-Wesley, Reading, MA, 1988.  pp. 63-64.
 *
 * @param n The long integer to be converted.
 * @return A pointer to the string.
 */
inline char const* itoa( int n ) {
  return PJL::ltoa( n );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* itoa_H */
/* vim:set et sw=2 ts=2: */
