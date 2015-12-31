/*
**      SWISH++
**      src/mod/mail/multipart.cpp
**
**      Copyright (C) 2000-2015  Paul J. Lucas
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

// local
#include "encoded_char.h"
#include "mod_mail.h"
#include "util.h"
#include "word_util.h"

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

void mail_indexer::index_multipart( char const *&c, char const *end ) {
  char const *nl;
  //
  // Find the beginning boundary string.
  //
  while ( (nl = find_newline( c, end )) != end ) {
    char const *const d = c;
    c = skip_newline( nl, end );
    if ( boundary_cmp( d, nl, boundary_stack_.back().c_str() ) )
      break;
  } // while
  if ( nl == end )
    return;

  while ( c != end ) {
    char const *const part_begin = c;
    //
    // Find the ending boundary string.
    //
    char const *part_end = end;
    while ( (nl = find_newline( c, end )) != end ) {
      part_end = c;
      c = skip_newline( nl, end );
      if ( boundary_cmp( part_end, nl, boundary_stack_.back().c_str() ) )
        break;
    } // while

    //
    // Index the words between the boundaries.
    //
    encoded_char_range::decoder::reset_all();
    encoded_char_range const part( part_begin, part_end );
    index_words( part );

    //
    // See if the boundary string is the final one, i.e., followed by "--".
    //
    if ( part_end == end || (nl[-1] == '-' && nl[-2] == '-') )
      break;
  } // while
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
