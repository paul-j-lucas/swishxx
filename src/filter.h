/*
**      SWISH++
**      src/filter.h
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

#ifndef filter_H
#define filter_H

// standard
#include <string>
#include <unistd.h>                     /* for unlink(2) */

///////////////////////////////////////////////////////////////////////////////

/**
 * A %filter is a light-weight class that contains a Unix command-line and
 * knows how to execute itself on a file to create a filtered file.  The
 * destructor deletes the filtered file.
 */
class filter {
public:
  explicit filter( char const *command ) : command_template_( command ) { }
  ~filter();

  // default copy constructor is fine
  // default assignment operator is fine

  char const* substitute( char const *file_name );
  char const* substitute( std::string const &file_name );
  char const* exec() const;

private:
  char const *command_template_;
  std::string command_;
  std::string target_file_name_;
};

////////// Inlines ////////////////////////////////////////////////////////////

inline filter::~filter() {
  if ( !target_file_name_.empty() )
    ::unlink( target_file_name_.c_str() );
}

inline char const* filter::substitute( std::string const &file_name ) {
  return substitute( file_name.c_str() );
}

///////////////////////////////////////////////////////////////////////////////

#endif /* filter_H */
/* vim:set et sw=2 ts=2: */
