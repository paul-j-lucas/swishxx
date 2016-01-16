/*
**      PJL C++ Library
**      less.h
**
**      Copyright (C) 1998-2016  Paul J. Lucas
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

#ifndef less_H
#define less_H

// standard
#include <cstddef>                      /* for size_t */
#include <cstring>
#include <functional>                   /* for less */

namespace std {

///////////////////////////////////////////////////////////////////////////////

/**
 * Specialize %less so that C-style strings <code>char const*</code> will work
 * properly with STL containers.
 */
template<>
struct less<char const*> {
  typedef char const* first_argument_type;
  typedef char const* second_argument_type;
  typedef bool result_type;

  less() { }

  result_type
  operator()( first_argument_type i, second_argument_type j ) const {
    return std::strcmp( i, j ) < 0;
  }
};

template<typename T> struct less_n;

/**
 * A %less_n is-a less&lt;char const*&gt; that compares C-style strings, but
 * only for a certain maximum length.
 */
template<>
struct less_n<char const*> : less<char const*> {
  less_n<char const*>( size_t max_len ) : n_( max_len ) { }

  result_type
  operator()( first_argument_type i, second_argument_type j ) const {
    return std::strncmp( i, j, n_ ) < 0;
  }

private:
  size_t const n_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace std

#endif /* less_H */
/* vim:set et sw=2 ts=2: */
