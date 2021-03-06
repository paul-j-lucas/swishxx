/*
**      SWISH++
**      src/IncludeMeta.h
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

#ifndef IncludeMeta_H
#define IncludeMeta_H

// local
#include "conf_var.h"
#include "pjl/less.h"

// standard
#include <map>

///////////////////////////////////////////////////////////////////////////////

/**
 * An %IncludeMeta is-a conf_var and a map containing the set of meta names
 * (and their possible reassigned names) to include during indexing.
 *
 * This is the same as index's \c -m command-line option.
 */
class IncludeMeta : public conf_var, public std::map<char const*,char const*> {
public:
  IncludeMeta() : conf_var( "IncludeMeta" ) { }
  CONF_VAR_ASSIGN_OPS( IncludeMeta )

  // inherited
  virtual void parse_value( char *line );

private:
  // inherited
  virtual void reset();
};

extern IncludeMeta include_meta_names;

///////////////////////////////////////////////////////////////////////////////

#endif /* IncludeMeta_H */
/* vim:set et sw=2 ts=2: */
