/*
**      SWISH++
**      src/conf_enum.h
**
**      Copyright (C) 2000-2015  Paul J. Lucas
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

#ifndef conf_enum_H
#define conf_enum_H

// local
#include "conf_string.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %conf_enum is-a conf&lt;std::string&gt; whose value must be of a
 * pre-determined set of legal values.
 */
class conf_enum : public conf<std::string> {
public:
  /**
   * Checks to see if a given value is legal, i.e., among the pre-determined
   * set of legal values.
   *
   * @param value The value to be checked.
   * @param err The ostream to write an error message to, if any.
   * @return Returns \c true only if the value is legal.
   */
  bool is_legal( char const*, std::ostream& = std::cerr ) const;

protected:
  conf_enum( char const *name, char const *const legal_values[] );
  CONF_STRING_ASSIGN_OPS( conf_enum )

  // inherited
  virtual void parse_value( char *line );

private:
  char const *const *const legal_values_;
};

#define CONF_ENUM_ASSIGN_OPS(T)           \
  T& operator=( std::string const &s ) {  \
    conf_enum::operator=( s );            \
    return *this;                         \
  }                                       \
  T& operator=( char const *s ) {         \
    conf_enum::operator=( s );            \
    return *this;                         \
  }

///////////////////////////////////////////////////////////////////////////////

#endif /* conf_enum_H */
/* vim:set et sw=2 ts=2: */
