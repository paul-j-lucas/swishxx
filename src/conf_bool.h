/*
**      SWISH++
**      src/conf_bool.h
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

#ifndef conf_bool_H
#define conf_bool_H

// local
#include "conf_var.h"

// standard
#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %conf<bool> is-a conf_var for containing the value of a Boolean
 * configuration variable.
 *
 * Acceptable values (regardless of case) are: f, false, n, no, off, on, t,
 * true, y, yes.
 */
template<> class conf<bool> : public conf_var {
public:
  operator bool() const { return value_; }

protected:
  conf( char const *name, bool default_value );
  conf<bool>& operator=( bool new_value ) {
    value_ = new_value;
    return *this;
  }
  CONF_VAR_ASSIGN_OPS( conf<bool> )

  // inherited
  virtual void parse_value( char *line );

private:
  bool const default_value_;
  bool value_;

  // inherited
  virtual void reset();
};

#define CONF_BOOL_ASSIGN_OPS(T)           \
  T& operator=( bool b ) {                \
    conf<bool>::operator=( b );           \
    return *this;                         \
  }                                       \
  T& operator=( std::string const &s ) {  \
    conf<bool>::operator=( s );           \
    return *this;                         \
  }                                       \
  T& operator=( char const *s ) {         \
    conf<bool>::operator=( s );           \
    return *this;                         \
  }

///////////////////////////////////////////////////////////////////////////////

#endif /* conf_bool_H */
/* vim:set et sw=2 ts=2: */
