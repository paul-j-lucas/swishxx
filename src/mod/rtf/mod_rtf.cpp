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
#include "TitleLines.h"
#include "word_util.h"

// standard
#include <cctype>

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

char const* rtf_indexer::find_title( mmap_file const &file ) const {
  unsigned lines = 0;
  //
  // Look for a title like:
  //
  //    {\title This is a title}
  //
  encoded_char_range::const_iterator c( file.begin(), file.end() );
  for ( ; !c.at_end(); ++c ) {
    if ( *c == '\n' && ++lines > num_title_lines ) {
      //
      // Didn't find \title within first num_title_lines lines of file: forget
      // it.
      //
      break;
    }

    if ( *c == '\\' && !(++c).at_end() && move_if_match( c, "title " ) ) {
      //
      // Found the \title control word.
      //
      if ( !c.at_end() ) {
        for ( auto const start = c.pos(); !c.at_end(); ++c ) {
          switch ( *c ) {
            case '}':
              return tidy_title( start, c.pos() );
            case '\n':
              if ( ++lines > num_title_lines )
                return nullptr;
          } // switch
        } // for
      }
      break;
    }
  } // for
  return nullptr;
}

void rtf_indexer::index_words( encoded_char_range const &e, int ) {
  char  word[ Word_Hard_Max_Size + 1 ];
  char  control[ Word_Hard_Max_Size + 1 ];
  bool  in_control = false, in_word = false, restart = false;
  int   len, control_len;

  for ( auto c = e.begin(); !c.at_end(); ) {
    char ch = iso8859_1_to_ascii( *c++ );

    ////////// Handle escaped characters //////////////////////////////////////

restart:
    if ( ch == '\\' ) {
      if ( c.at_end() )
        break;
      switch ( *c ) {
        case '\\':                      // literal '\'
          ++c;
          break;
        case '-':                       // optional hyphen
        case '_':                       // nonbreaking hyphen
          ch = '-';
          ++c;
          break;
        case '~':                       // nonbreaking space
        case '{':                       // don't care about literal '{' or '}'
        case '}':
          ch = ' ';
          ++c;
          break;
      } // switch
    }

    ////////// Ignore control words ///////////////////////////////////////////

    if ( in_control ) {
      if ( isalnum( ch ) ) {
        if ( control_len < Word_Hard_Max_Size ) {
          control[ control_len++ ] = ch;
        } else {                        // too big: skip chars
          in_control = false;
          while ( !c.at_end() && is_word_char( iso8859_1_to_ascii( *c++ ) ) )
            ;
        }
        continue;
      }
      in_control = false;
      if ( ch != ' ' ) {
        //
        // If we have a word immediately followed by a control word (like
        // "word\control"), we have to wait until we reach the end of the
        // control word to know whether to index "word" because we have to
        // handle the special case of "\rquote" below.
        //
        // If the control word is terminated by anything other than a space,
        // then it can't be "\rquote" because it's always followed by a space.
        // Now that we know it's not "\rquote", index the word.
        //
        // However, we have to "restart" character analysis on the character
        // that's already in "ch", hence this special "restart" state.
        //
        restart = true;
        goto if_in_word;
      }
      //
      // Special case: convert "\rquote " to '\'' so it can form part of words.
      // (The only reason we have to collect the characters comprising the
      // control word at all is to compare it to "rquote".)
      //
      control[ control_len ] = '\0';
      if ( ::strcmp( control, "rquote" ) == 0 ) {
        ch = '\'';
        goto if_is_word_char;
      }
      continue;
    }

    if ( ch == '\\' ) {
      control_len = 0;
      in_control = true;
      continue;
    }

    ////////// Ignore HYPERLINKs //////////////////////////////////////////////

    if ( ch == '{' && c.safe_deref() == 'H' ) {
      if ( move_if_match( c, "HYPERLINK " ) ) {
        skip_char( &c, '}' );
        continue;
      }
    }

    ////////// Collect a word /////////////////////////////////////////////////

if_is_word_char:
    if ( is_word_char( ch ) ) {
      if ( !in_word ) {                 // start a new word
        word[0] = ch;
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

if_in_word:
    if ( in_word ) {
      //
      // We ran into a non-word character, so index the word up to, but not
      // including, it.
      //
      in_word = false;
      index_word( word, len );
    }

    if ( restart ) {
      restart = false;
      goto restart;
    }
  } // for

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
