/*
**      SWISH++
**      src/mod/rtf/mod_rtf.cpp
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
#include "config.h"
#include "iso8859-1.h"
#include "mod_rtf.h"
#include "util.h"
#include "word_util.h"

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

void rtf_indexer::index_words( encoded_char_range const &e, int ) {
  char  word[ Word_Hard_Max_Size + 1 ];
  bool  in_word = false;
  int   len;

  encoded_char_range::const_iterator c = e.begin();
  while ( !c.at_end() ) {
    char ch = iso8859_1_to_ascii( *c++ );

    ////////// Collect a word /////////////////////////////////////////////////

    if ( is_word_char( ch ) ) {
      if ( !in_word ) {                 // start a new word
        word[ 0 ] = ch;
        len = 1;
        in_word = true;
        continue;
      }
      if ( len < Word_Hard_Max_Size ) { // continue same word
        word[ len++ ] = ch;
        continue;
      }
      in_word = false;                  // too big: skip chars
      while ( !c.at_end() && is_word_char( iso8859_1_to_ascii( *c++ ) ) )
        ;
      continue;
    }

    if ( in_word ) {
      //
      // We ran into a non-word character, so index the word up to, but not
      // including, it.
      //
      in_word = false;
      index_word( word, len );
    }

    if ( ch == '<' ) {
      //
      // This is probably the start of command; if so, skip everything until
      // the terminating '>'.
      //
      if ( c.at_end() )
        break;
      if ( (ch = *c++) == '<' )
        continue;                       // a literal '<': ignore it
      while ( !c.at_end() && *c++ != '>' )
        ;
    }
  } // while

  if ( in_word ) {
    //
    // We ran into 'end' while still accumulating characters into a word, so
    // just index what we've got.
    //
    index_word( word, len );
  }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
