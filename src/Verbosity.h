/*
**      SWISH++
**      src/Verbosity.h
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

#ifndef Verbosity_H
#define Verbosity_H

// local
#include "conf_int.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %Verbosity is-a conf&lt;int&gt; containing the verbosity level, i.e., how
 * much information to print during either indexing or extraction.
 *
 * This is the same as either index's or extract's \c -v command-line option.
 */
class Verbosity : public conf<int> {
public:
  Verbosity() : conf<int>( "Verbosity", 0, 0, 4 ) { }
  CONF_INT_ASSIGN_OPS( Verbosity )
};

extern Verbosity verbosity;

///////////////////////////////////////////////////////////////////////////////

#endif /* Verbosity_H */
/* vim:set et sw=2 ts=2: */
