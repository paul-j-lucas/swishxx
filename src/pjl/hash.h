/*
**      PJL C++ Library
**      hash.h
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

#ifndef pjl_hash_H
#define pjl_hash_H

// local
#include "config.h"

// standard
#include <cstddef>                      /* for size_t */
#include <cstring>
#include <functional>                   /* for hash */
#include <memory>                       /* for unique_ptr */
#include <string>
#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////

namespace PJL {

#if   SIZEOF_SIZE_T == 4
size_t const Hash_Init = 2166136261ul;
#elif SIZEOF_SIZE_T == 8
size_t const Hash_Init = 14695981039346656037ul;
#else
# error "Unexpected sizeof(size_t)"
#endif /* SIZEOF_SIZE_T */

/**
 * Generic hash function that hashes a byte sequence.
 *
 * @param p A pointer to the first byte in the sequence to hash.
 * @param len The number of bytes in the sequence.
 * @param init The initialization value.
 * @return Returns the hash code for the given byte sequence.
 */
size_t hash_bytes( void const *p, size_t len, size_t init = Hash_Init );

/**
 * Generic hash function that hashes the memory occupied by a value of some
 * type \a T.
 *
 * @tparam T The value's type.
 * @param value The value to hash.
 * @return Returns the hash code for the given value.
 */
template<typename T>
inline size_t hash_bytes( T const &value ) {
  return hash_bytes( &value, sizeof( value ) );
}

/**
 * Hash function that hashes a C string.
 *
 * @param s A pointer to the C string to hash.
 * @param init The initialization value.
 * @return Returns the hash code for the given C string.
 */
size_t hash_string( char const *s, size_t init = Hash_Init );

} // namespace PJL

///////////////////////////////////////////////////////////////////////////////

namespace std {

/**
 * Specialization <code>std::hash&lt;char const*&gt; for C strings.
 */
template<>
inline size_t hash<char const*>::operator()( char const *s ) const noexcept {
  return PJL::hash_string( s );
}

} // namespace std

///////////////////////////////////////////////////////////////////////////////

namespace PJL {

/**
 * A version of \c std::equal_to for C strings.
 */
struct char_ptr_equal_to {
  typedef char const* argument_type;
  typedef bool result_type;

  result_type operator()( argument_type i, argument_type j ) const {
    return std::strcmp( i, j ) == 0;
  }
};

/**
 * A version of \c std::unordered_set for C strings.
 */
using unordered_char_ptr_set =
  std::unordered_set<char const*,std::hash<char const*>,char_ptr_equal_to>;

} // namespace PJL

///////////////////////////////////////////////////////////////////////////////

#endif  /* pjl_hash_H */
/* vim:set et ts=2 sw=2: */
