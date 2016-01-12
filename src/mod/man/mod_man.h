/*
**      SWISH++
**      src/mod/man/mod_man.h
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

#ifndef mod_man_H
#define mod_man_H

// local
#include "indexer.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %man_indexer is-an indexer for indexing Unix manual pages.
 */
class man_indexer : public indexer {
public:
  man_indexer() : indexer( "man" ) { }

  // inherited
  char const* find_title( PJL::mmap_file const& ) const;
  void index_words( encoded_char_range const&, meta_id_type = Meta_ID_None );

private:
  /**
   * Parses a macro.  If it is "SH" (section heading), parse the name of the
   * heading and make that a meta name.  Then scan for the next \c .SH to mark
   * the end of the range and index all the words in between as being
   * associated with the section heading meta name.
   *
   * @param c The iterator to use.  It must be positioned at the character
   * after the '.'; it is repositioned.
   * @param end The iterator marking the end of the file.
   */
  void parse_man_macro( char const *&c, char const *end );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* mod_man_H */
/* vim:set et sw=2 ts=2: */
