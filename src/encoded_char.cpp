/*
**      SWISH++
**      src/encoded_char.cpp
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

// standard
#include <cctype>

encoded_char_range::decoder::set_type encoded_char_range::decoder::decoders_;

///////////////////////////////////////////////////////////////////////////////

encoded_char_range::decoder::~decoder() {
  // do nothing
}

void encoded_char_range::decoder::reset_all() {
  for ( auto &d : decoders_ )
    d->reset();
}

char* to_lower( encoded_char_range const &range ) {
  extern PJL::char_buffer_pool<128,5> lower_buf;
  char *p = lower_buf.next();
  for ( auto c = range.begin(); !c.at_end(); ++c )
    *p++ = to_lower( *c );
  *p = '\0';
  return lower_buf.current();
}

void trim_left( encoded_char_range::const_iterator *begin,
                encoded_char_range::const_iterator const &end ) {
  while ( *begin < end && isspace( **begin ) )
    ++*begin;
}

void trim_right( encoded_char_range::const_iterator const &begin,
                 encoded_char_range::const_iterator *end ) {
  auto first_trailing_space = begin;
  bool is_space = false;
  for ( auto pos = begin; pos < *end; ++pos ) {
    if ( isspace( *pos ) ) {
      if ( !is_space ) {
        first_trailing_space = pos;
        is_space = true;
      }
    } else {
      is_space = false;
    }
  } // for
  if ( is_space )
    *end = first_trailing_space;
}

bool skip_char( encoded_char_range::const_iterator *i, char c, size_t limit ) {
  for ( auto j = *i; !j.at_end() && limit; ++j, --limit ) {
    if ( *j == c ) {
      *i = ++j;
      return true;
    }
    if ( *j == '\n' || *j == '\r' )
      break;
  } // for
  return false;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
