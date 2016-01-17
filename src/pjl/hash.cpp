/*
**      PJL C++ Library
**      hash.cpp
**
**      Copyright (C) 2016  Paul J. Lucas
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
#include "hash.h"

using namespace std;

namespace PJL {

#if   SIZEOF_SIZE_T == 4
size_t const Hash_Prime = 16777619ul;
#elif SIZEOF_SIZE_T == 8
size_t const Hash_Prime = 1099511628211ul;
#else
# error "Unexpected sizeof(size_t)"
#endif /* SIZEOF_SIZE_T */

///////////////////////////////////////////////////////////////////////////////

size_t hash_bytes( void const *p, size_t len, size_t result ) {
  auto c = reinterpret_cast<unsigned char const*>( p );
  auto const end = c + len;
  //
  // FNV-1a (Fowler/Noll/Vo) hashing algorithm.
  //
  while ( c < end ) {
    result *= Hash_Prime;
    result ^= *c++;
  } // while
  return result;
}

size_t hash_string( char const *s, size_t result ) {
  //
  // FNV-1a (Fowler/Noll/Vo) hashing algorithm.
  //
  while ( *s ) {
    result *= Hash_Prime;
    result ^= *s++;
  } // while
  return result;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL
/* vim:set et ts=2 sw=2: */
