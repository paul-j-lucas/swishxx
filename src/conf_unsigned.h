/*
**      SWISH++
**      src/conf_unsigned.h
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

#ifndef conf_unsigned_H
#define conf_unsigned_H

// local
#include "conf_var.h"

// system
#include <limits>
#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * A conf&lt;unsigned&gt; is-a conf_var for containing the value of an unsigned
 * integer configuration variable.
 */
template<>
class conf<unsigned> : public conf_var {
public:
  typedef unsigned value_type;

  conf& operator++() {
    ++value_;
    return *this;
  }

  conf operator++( int ) {
    conf tmp = *this;
    ++value_;
    return tmp;
  }

  conf& operator--() {
    --value_;
    return *this;
  }

  conf operator--( int ) {
    conf tmp = *this;
    --value_;
    return tmp;
  }

  operator value_type() const {
    return value_;
  }

protected:
  conf( char const *name, value_type default_value, value_type min = 0,
        value_type max = std::numeric_limits<value_type>::max() );
  conf& operator=( value_type );
  CONF_VAR_ASSIGN_OPS( conf )

  // inherited
  virtual void parse_value( char *line );

private:
  unsigned const default_value_, min_, max_;
  unsigned       value_;

  virtual void reset();
};

#define CONF_INT_ASSIGN_OPS(T)            \
  T& operator=( unsigned i ) {            \
    conf<unsigned>::operator=( i );       \
    return *this;                         \
  }                                       \
  T& operator=( std::string const &s ) {  \
    conf<unsigned>::operator=( s );       \
    return *this;                         \
  }                                       \
  T& operator=( char const *s ) {         \
    conf<unsigned>::operator=( s );       \
    return *this;                         \
  }

///////////////////////////////////////////////////////////////////////////////

#endif /* conf_unsigned_H */
/* vim:set et sw=2 ts=2: */
