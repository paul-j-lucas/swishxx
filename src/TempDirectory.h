/*
**      SWISH++
**      src/TempDirectory.h
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

#ifndef TempDirectory_H
#define TempDirectory_H

// local
#include "conf_string.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %TempDirectory is-a conf&lt;string&gt; containing the name of the
 * directory to use for temporary files during indexing.
 *
 * This is the same as index's -T command-line option.
 */
class TempDirectory : public conf<std::string> {
public:
  TempDirectory() : conf<std::string>( "TempDirectory", default_value() ) { }
  CONF_STRING_ASSIGN_OPS( TempDirectory )

private:
  static char const* default_value();
};

///////////////////////////////////////////////////////////////////////////////

#endif /* TempDirectory_H */
/* vim:set et sw=2 ts=2: */
