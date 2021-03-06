/*
**      SWISH++
**      src/ExtractExtension.h
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

#ifndef ExtractExtension_H
#define ExtractExtension_H

// local
#include "config.h"
#include "conf_string.h"
#include "swishxx-config.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * An %ExtractExtension is-a conf&lt;string&gt; containing the extension to
 * append to filenames during extraction.
 *
 * This is the same as extract's \c -x command-line option.
 */
class ExtractExtension : public conf<std::string> {
public:
  ExtractExtension() :
      conf<std::string>( "ExtractExtension", ExtractExtension_Default ) { }
  CONF_STRING_ASSIGN_OPS( ExtractExtension )
};

extern ExtractExtension extract_extension;

///////////////////////////////////////////////////////////////////////////////

#endif /* ExtractExtension_H */
/* vim:set et sw=2 ts=2: */
