/*
**      SWISH++
**      src/mod/rtf/mod_rtf.h
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

#ifndef mod_rtf_H
#define mod_rtf_H

// local
#include "indexer.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * An %rtf_indexer is-an indexer for indexing RTF (Rich Text Format) files.
 *
 * See also:
 *    Nathaniel S. Borenstein.  "RFC: 1563: The text/enriched MIME Content-
 *    type," Network Working Group of the Internet Engineering Task Force,
 *    January 1994.
 *
 *    Nathaniel S. Borenstein and Ned Freed.  "RFC: 1341: MIME (Multipurpose
 *    Internet Mail Extensions): Mechanisms for Specifying and Describing the
 *    Format of Internet Message Bodies," Network Working Group of the Internet
 *    Engineering Task Force, June 1992.
 */
class rtf_indexer : public indexer {
public:
  rtf_indexer() : indexer( "RTF" ) { }

  // inherited
  char const* find_title( PJL::mmap_file const& ) const;
  void index_words( encoded_char_range const&, meta_id_type = Meta_ID_None);
};

///////////////////////////////////////////////////////////////////////////////

#endif /* mod_rtf_H */
/* vim:set et sw=2 ts=2: */
