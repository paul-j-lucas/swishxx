/*
**      PJL C++ Library
**      pattern_map.h
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

#ifndef pattern_map_H
#define pattern_map_H

// local
#include "config.h"
#include "less.h"

// standard
#include <algorithm>                    /* for find_if() */
#include <fnmatch.h>                    /* for fnmatch(3) */
#include <map>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %pattern_map is-a std::map that maps a filename pattern to an object of
 * type T.
 *
 * Note that the declaration of std::map has a default \e Compare template
 * parameter of \c less&lt;key_type&gt; and, since we've included \c less.h
 * above that defines <code>less&lt;char const*&gt;</code>, C-style string
 * comparisons work properly.
 *
 * See also:
 *    International Standards Organizaion.  "ISO/IEC 9945-2: Information
 *    Technology -- Portable Operating System Interface (POSIX) -- Part 2:
 *    Shell and Utilities," 1993.
 */
template<typename T>
class pattern_map : public std::map<char const*,T> {
public:
  typedef typename std::map<char const*,T> map_type;
  typedef typename map_type::key_type key_type;
  typedef typename map_type::value_type value_type;
  typedef typename map_type::iterator iterator;
  typedef typename map_type::const_iterator const_iterator;

  //
  // find() must be overridden to use our own comparison class.
  //
  iterator find( char const *file_name ) {
    //
    // Using find_if() makes this run in O(n) instead of O(lg n), but there's
    // no choice because no ordering can be imposed on filename patterns, i.e.,
    // either a filename matches a pattern or it doesn't.  Continuing to use a
    // map still allows O(lg n) insertion, however.
    //
    return std::find_if(
      this->begin(), this->end(), pattern_match( file_name )
    );
  }

  const_iterator find( char const *file_name ) const {
    return std::find_if(
      this->begin(), this->end(), pattern_match( file_name )
    );
  }

  bool matches( char const *file_name ) const {
    return find( file_name ) != this->end();
  }

  void insert( char const *pattern, T const &t ) {
    (*this)[pattern] = t;
  }

  void insert( value_type const &n ) {
    map_type::insert( n );
  }

private:
  /**
   * A %pattern_match is-a unary_function to serve as a predicate to
   * \c find_if() above.
   */
  struct pattern_match : std::unary_function<value_type const&,bool> {
    pattern_match( char const *file_name ) : file_name_( file_name ) { }

    bool operator()( value_type const &map_node ) const {
      return !::fnmatch( map_node.first, file_name_, 0 );
    }

  private:
    char const *const file_name_;
  };
};

///////////////////////////////////////////////////////////////////////////////

#endif /* pattern_map_H */
/* vim:set et sw=2 ts=2: */
