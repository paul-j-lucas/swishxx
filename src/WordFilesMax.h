/*
**      SWISH++
**      src/WordFilesMax.h
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

#ifndef WordFilesMax_H
#define WordFilesMax_H

// local
#include "conf_int.h"

// standard
#include <climits>

///////////////////////////////////////////////////////////////////////////////

/**
 * A WordFilesMax is-a conf<int> containing the maximum number of files a
 * word may occur in before it is discarded as being too frequent.
 *
 * This is the same as index's \c -f command-line option.
 */
class WordFilesMax : public conf<int> {
public:
  WordFilesMax() : conf<int>( "WordFilesMax", INT_MAX, 2 ) { }
  CONF_INT_ASSIGN_OPS( WordFilesMax )
};

extern WordFilesMax word_files_max;

///////////////////////////////////////////////////////////////////////////////

#endif /* WordFilesMax_H */
/* vim:set et sw=2 ts=2: */
