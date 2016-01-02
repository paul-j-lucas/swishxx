/*
**      SWISH++
**      src/TitleLines.h
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

#ifndef TitleLines_H
#define TitleLines_H

// local
#include "config.h"
#include "conf_unsigned.h"
#include "swishxx-config.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %TitleLines is-a conf&lt;unsigned&gt; containing the number of lines to
 * look into a file for its "title" (whatever that means for a given file
 * format).
 *
 * This is the same as index's \c -t command-line option.
 */
class TitleLines : public conf<unsigned> {
public:
  TitleLines() : conf<unsigned>( "TitleLines", TitleLines_Default ) { }
  CONF_INT_ASSIGN_OPS( TitleLines )
};

extern TitleLines num_title_lines;

///////////////////////////////////////////////////////////////////////////////

#endif /* TitleLines_H */
/* vim:set et sw=2 ts=2: */
