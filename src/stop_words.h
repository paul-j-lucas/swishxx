/*
**      SWISH++
**      src/stop_word.h
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

#ifndef stop_words_H
#define stop_words_H

// local
#include "pjl/less.h"
#include "pjl/mmap_file.h"

// standard
#include <set>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %stop_word_set is-a \c set&lt;char const*&gT.  The only reason for having
 * a derived class rather than a typedef is so that we can have custom
 * constructors.
 */
struct stop_word_set : std::set<char const*> {
  stop_word_set( char const *file_name = nullptr );
  stop_word_set( PJL::mmap_file const &index_file );
};

extern stop_word_set* stop_words;

///////////////////////////////////////////////////////////////////////////////

#endif /* stop_words_H */
/* vim:set et sw=2 ts=2: */
