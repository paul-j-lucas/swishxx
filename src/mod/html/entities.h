/*
**      SWISH++
**      src/mod/html/entities.h
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

#ifndef entities_H
#define entities_H

// local
#include "pjl/less.h"

// standard
#include <map>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %char_entity_map is used to perform fast look-up of a character entity
 * reference.  It uses <code>char const*</code> to point directly to the
 * C-strings in the \c char_entity_table[] vector; no strings are copied.
 *
 * The contructor is private, however, to ensure that only instance() can
 * be called to initialize and access a single, static instance.
 */
class char_entity_map {
public:
  typedef char const* key_type;
  typedef char value_type;

  static char_entity_map const& instance();

  value_type operator[]( key_type key ) const {
    map_type::const_iterator const found = map_.find( key );
    return found != map_.end() ? found->second : ' ';
  }

private:
  /**
   * Constructs (initializes) a %char_entity_map.  Any entity that isn't listed
   * below will convert to a space.  Note that is isn't necessary to convert
   * "&lt;" and "&gt;" since such entities aren't indexed anyway.  However, it
   * is necessary to convert "&amp;" (so it can be part of an acronym like
   * "AT&T") and "&apos;" (so it can be part of a contracted word like
   * "can't").
   */
  char_entity_map();

  typedef std::map<key_type,value_type> map_type;
  map_type map_;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* entities_H */
/* vim:set et sw=2 ts=2: */
