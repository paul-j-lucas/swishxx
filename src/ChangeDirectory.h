/*
**      SWISH++
**      src/ChangeDirectory.h
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

#ifndef ChangeDirectory_H
#define ChangeDirectory_H

// local
#include "conf_string.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %ChangeDirectory is-a \c conf&lt;std::string&gt; containing the name of
 * the directory to \b chdir(2) to just before indexing.
 *
 * This is the same as index's \c -C command-line option.
 */
class ChangeDirectory : public conf<std::string> {
public:
  ChangeDirectory() : conf<std::string>( "ChangeDirectory" ) { }
  CONF_STRING_ASSIGN_OPS( ChangeDirectory )

protected:
  // inherited
  virtual void parse_value( char *line );
};

///////////////////////////////////////////////////////////////////////////////
#endif /* ChangeDirectory_H */
/* vim:set et sw=2 ts=2: */
