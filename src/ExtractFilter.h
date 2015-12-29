/*
**      SWISH++
**      src/ExtractFilter.h
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

#ifndef ExtractFilter_H
#define ExtractFilter_H

// local
#include "conf_bool.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * An %ExtractFilter is-a conf&lt;bool&gt; containing the Boolean value
 * indicating whether to extract as a filter, i.e., extract a single file to
 * standard output.
 *
 * This is the same as extract's \c -f command-line option.
 */
class ExtractFilter : public conf<bool> {
public:
  ExtractFilter() : conf<bool>( "ExtractFilter", false ) { }
  CONF_BOOL_ASSIGN_OPS( ExtractFilter )
};

extern ExtractFilter extract_as_filter;

///////////////////////////////////////////////////////////////////////////////

#endif /* ExtractFilter_H */
/* vim:set et sw=2 ts=2: */
