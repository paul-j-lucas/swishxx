/*
**      SWISH++
**      src/ExcludeFile.h
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

#ifndef ExcludeFile_H
#define ExcludeFile_H

// local
#include "conf_var.h"
#include "pjl/pattern_map.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * An %ExcludeFile is-a conf_var containing the set of filename patterns to
 * exclude during either indexing or extraction.
 *
 * This is the same as either index's or extract's \c -E command-line option.
 *
 * The \c bool template parameter is not used here.  It's simply to reuse the
 * code for pattern_map as-is.
 */
class ExcludeFile : public conf_var, public pattern_map<bool> {
public:
  ExcludeFile() : conf_var( "ExcludeFile" ) { }
  CONF_VAR_ASSIGN_OPS( ExcludeFile )

  void insert( char const *pattern ) {
    pattern_map<bool>::insert( pattern, false );
  }

private:
  // inherited
  virtual void parse_value( char *line );
  virtual void reset();
};

extern ExcludeFile exclude_patterns;

///////////////////////////////////////////////////////////////////////////////

#endif /* ExcludeFile_H */
/* vim:set et sw=2 ts=2: */
