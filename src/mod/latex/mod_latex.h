/*
**      SWISH++
**      src/mod/latex/mod_latex.h
**
**      Copyright (C) 2002-2015  Paul J. Lucas
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

#ifndef mod_latex_H
#define mod_latex_H

// local
#include "indexer.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %LaTeX_indexer is-an indexer for indexing LaTeX files.
 */
class LaTeX_indexer : public indexer {
public:
  LaTeX_indexer() : indexer( "LaTeX" ) { }

  // inherited
  char const* find_title( PJL::mmap_file const& ) const;
  void index_words( encoded_char_range const&, int meta_id = Meta_ID_None);

private:
  char const* parse_latex_command( encoded_char_range::const_iterator& );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* mod_latex_H */
/* vim:set et sw=2 ts=2: */
