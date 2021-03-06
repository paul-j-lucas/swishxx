/*
**      SWISH++
**      src/conf_string.h
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

#ifndef conf_string_H
#define conf_string_H

// local
#include "config.h"
#include "conf_var.h"

// standard
#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * A conf&lt;std::string&gt; is-a conf_var for containing a configuration
 * variable string value.
 */
template<>
class conf<std::string> : public conf_var {
public:
  typedef std::string::size_type size_type;

  size_type length() const {
    return value_.length();
  }

  size_type size() const {
    return value_.size();
  }

  operator char const*() const {
    return value_.c_str();
  }

  friend bool operator==( conf<std::string> const &i,
                          conf<std::string> const &j ) {
    return i.value_ == j.value_;
  }
  friend bool operator!=( conf<std::string> const &i,
                          conf<std::string> const &j ) {
      return !(i == j);
  }

  friend bool operator==( conf<std::string> const &i, std::string const &s ) {
    return i.value_ == s;
  }
  friend bool operator!=( conf<std::string> const &i, std::string const &s ) {
    return !(i == s);
  }

  friend bool operator==( std::string const &s, conf<std::string> const &i ) {
    return i == s;
  }
  friend bool operator!=( std::string const &s, conf<std::string> const &i ) {
    return i != s;
  }

  friend bool operator==( conf<std::string> const &i, char const *s ) {
    return i.value_ == s;
  }
  friend bool operator!=( conf<std::string> const &i, char const *s ) {
    return !(i == s);
  }

  friend bool operator==( char const *s, conf<std::string> const &i ) {
    return i == s;
  }
  friend bool operator!=( char const *s, conf<std::string> const &i ) {
    return i != s;
  }

protected:
  conf( char const *name, char const *default_value = "" );
  CONF_VAR_ASSIGN_OPS( conf<std::string> )

  conf<std::string>& operator+=( std::string const &s ) {
    value_ += s;
    return *this;
  }
  conf<std::string>& operator+=( char const *s ) {
    value_ += s;
    return *this;
  }

  // inherited
  virtual void parse_value( char *line );

private:
  std::string const default_value_;
  std::string       value_;

  // inherited
  virtual void reset();
};

#define CONF_STRING_ASSIGN_OPS(T)         \
  T& operator=( std::string const &s ) {  \
    conf<std::string>::operator=( s );    \
    return *this;                         \
  }                                       \
  T& operator=( char const *s ) {         \
    conf<std::string>::operator=( s );    \
    return *this;                         \
  }                                       \
  T& operator+=( std::string const &s ) { \
    conf<std::string>::operator+=( s );   \
    return *this;                         \
  }                                       \
  T& operator+=( char const *s ) {        \
    conf<std::string>::operator+=( s );   \
    return *this;                         \
  }

///////////////////////////////////////////////////////////////////////////////

#endif /* conf_string_H */
/* vim:set et sw=2 ts=2: */
