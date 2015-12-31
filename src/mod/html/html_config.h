/*
**      SWISH++
**      src/mod/html/html_config.h
**
**      Copyright (C) 2001-2015  Paul J. Lucas
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

#ifndef html_config_H
#define html_config_H

////////// HTML and XHTML parameters //////////////////////////////////////////

/**
 * The maximum size of an entity reference, e.g., "&eacute;" NOT counting the
 * leading '&' or the trailing ';'.  You should have no reason to change this.
 */
int const Entity_Max_Size = 6;

/**
 * The maximum size of an HTML or XHTML tag name, e.g., \c BLOCKQUOTE.  You
 * might need to increase this if you are indexing HTML or XHTML documents that
 * contain non-standard tags and at least one of them is longer than the above.
 */
int const Tag_Name_Max_Size = 10;

///////////////////////////////////////////////////////////////////////////////

#endif /* html_config_H */
/* vim:set et sw=2 ts=2: */
