/*
**      SWISH++
**      src/mod/man/mod_man.cpp
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

// local
#include "config.h"
#include "AssociateMeta.h"
#include "config.h"
#include "encoded_char.h"
#include "iso8859-1.h"
#include "mod_man.h"
#include "TitleLines.h"
#include "util.h"
#include "word_util.h"

// standard
#include <cctype>
#include <cstring>

using namespace PJL;
using namespace std;

////////// local functions ////////////////////////////////////////////////////

/**
 * Checkss to see if the current macro is the start of a comment. If so, skip
 * it by scanning for the newline.
 *
 * @param c The iterator to use.  It is presumed to be positioned at the first
 * character after the \c '.'.  If the macro is the start of a comment, it is
 * repositioned at the first character past the macro, i.e., past the newline;
 * otherwise, it is not touched.
 * @param end The iterator marking the end of the file.
 * @return Returns \c true only if the current macro is the beginning of a
 * comment.
 */
static bool is_man_comment( char const *&c, char const *end ) {
  if ( move_if_match( c, end, "\\\"" ) ) {
    c = skip_newline( find_newline( c, end ), end );
    return true;
  }
  return false;
}

/**
 * Parses a backslash followed by "something" and treat it as if it weren't
 * even there.
 *
 * See also:
 *    Joseph F. Ossanna & Brian W. Kernighan.  "Troff User's Manual," Computing
 *    Science Technical Report No. 54, AT&T Bell Laboratories, Murry Hill, NJ,
 *    November 1992.
 *
 * @param c The iterator to use.  It must be positioned at the character after
 * the \c '\'; it is repositioned at the first character after the baclskash
 * sequence.
 * @param end The iterator marking the end of the file.
 */
static void parse_backslash( char const *&c, char const *end ) {
  if ( c == end )
    return;
  char const ch = *c++;
  if ( c == end )
    return;

  switch ( ch ) {
    case '*':                           // \fx or \f(xx
    case 'f':
    case 'g':
    case 'n':
      if ( *c++ != '(' || c == end )
        break;
      // no break;

    case '(':                           // \(xx
      if ( ++c != end )
        ++c;
      break;

    case 'b':                           // \b'xxxxx'
    case 'C':
    case 'D':
    case 'h': case 'H':
    case 'l':
    case 'L':
    case 'N':
    case 'o':
    case 'S':
    case 'v':
    case 'w':
    case 'x': case 'X':
      if ( *c != '\'' )
        break;
      ++c;
      while ( c != end && *c++ != '\'' )
        ;
      break;

    case 's':                           // \sN, \s+N, \s-N
      if ( (*c == '+' || *c == '-') && ++c == end )
        break;
      // no break;

    case '\n':                          // escaped newline ...
      ++c;                              // ... make it disappear
      break;

    case '-':                           // \-
      --c;                              // transform an en-dash ...
      break;                            // ... to just a '-'

    default:
      /* do nothing except skip over the '\' */;
  } // switch
}

////////// member functions ///////////////////////////////////////////////////

char const* man_indexer::find_title( mmap_file const &file ) const {
  unsigned  lines = 0;
  bool      newline = true;

  for ( auto c = file.begin(); c != file.end(); ) {
    if ( newline && ++lines > num_title_lines ) {
      //
      // Didn't find ".SH NAME" within first num_title_lines lines of file:
      // forget it.
      //
      return nullptr;
    }

    //
    // Find the start of a macro, i.e., a line that begins with a '.' (dot).
    //
    if ( !(newline && *c == '.') ) {    // not macro: forget it
      newline = *c++ == '\n';
      continue;
    }

    //
    // Found a macro: is it a comment?  If so, skip it.
    //
    if ( is_man_comment( ++c, file.end() ) )
      continue;

    //
    // Is the macro ".SH NAME"?
    //
    newline = false;
    if ( !move_if_match( c, file.end(), "SH" ) )
      continue;
    while ( c != file.end() && is_space( *c ) )
      ++c;
    if ( !move_if_match( c, file.end(), "NAME" ) )
      continue;

    //
    // Found ".SH NAME": skip the newline to get to the beginning of the title
    // on the next line.  The end of the title is the end of that next line.
    //
    c = skip_newline( find_newline( c, file.end() ), file.end() );
    char *const title = tidy_title( c, find_newline( c, file.end() ) );

    //
    // Go through the title and process backslashed character sequences in case
    // there are things like \fBword\fP in it so they can be stripped out.
    //
    char *d = title;
    for ( c = title; *c; ++c ) {
      if ( *c == '\\' )
        parse_backslash( ++c, file.end() );
      *d++ = *c;
    } // for
    *d = '\0';

    return title;
  } // for

  //
  // The file has less than num_title_lines lines and no .SH NAME was found.
  //
  return nullptr;
}

void man_indexer::index_words( encoded_char_range const &e,
                               meta_id_type meta_id ) {
  char  word[ Word_Hard_Max_Size + 1 ];
  bool  in_word = false;
  int   len;
  bool  newline = false;

  char const* c = e.begin_pos();
  while ( c != e.end_pos() ) {
    char const ch = iso8859_1_to_ascii( *c++ );

    ////////// Collect a word /////////////////////////////////////////////////

    if ( is_word_char( ch ) ) {
      if ( !in_word ) {                 // start a new word
        word[0] = ch;
        len = 1;
        in_word = true;
        goto next_c;
      }
      if ( len < Word_Hard_Max_Size ) { // continue same word
        word[ len++ ] = ch;
        goto next_c;
      }
      in_word = false;                  // too big: skip chars
      while ( c != e.end_pos() && is_word_char( iso8859_1_to_ascii( *c++ ) ) )
        ;
      goto next_c;
    }

    if ( ch == '\\' ) {
      //
      // Got a backslash sequence: skip over it and treat it as though it
      // weren't even there.
      //
      parse_backslash( c, e.end_pos() );
      goto next_c;
    }

    if ( in_word ) {
      //
      // We ran into a non-word character, so index the word up to, but not
      // including, it.
      //
      in_word = false;
      index_word( word, len, meta_id );
    }

    if ( newline && ch == '.' && meta_id == Meta_ID_None ) {
      //
      // If we're at the first character on a line and the character is a '.'
      // (the start of a macro), parse it.
      //
      parse_man_macro( c, e.end_pos() );
    }

next_c:
    if ( c == e.end_pos() )
      break;
    newline = ch == '\n';
  } // while

  if ( in_word ) {
    //
    // We ran into 'end' while still accumulating characters into a word, so
    // just index what we've got.
    //
    index_word( word, len, meta_id );
  }
}

void man_indexer::parse_man_macro( char const *&c, char const *end ) {
  if ( !move_if_match( c, end, "SH" ) )
    return;
  char const *const nl = find_newline( c, end );
  if ( !nl )
    return;
  while ( c != nl && !is_word_char( *c ) ) {
    //
    // Skip non-word characters after the 'H' in "SH" and before the first word
    // of the section heading.
    //
    ++c;
  } // while
  char const *const begin = c;

  ////////// Parse the name of the section heading. ///////////////////////////

  char  word[ Word_Hard_Max_Size + 1 ];
  int   len = 0;

  while ( c != nl ) {
    char ch = *c++;
    if ( ch == ' ' )
      ch = '-';
    if ( !is_word_char( ch ) )
      break;
    if ( len < Word_Hard_Max_Size ) {
      word[ len++ ] = tolower( ch );
      continue;
    }
    while ( c != nl && is_word_char( *c++ ) ) // too big: skip chars
      ;
    return;
  } // while

  while ( len > 0 && word[ len - 1 ] == '-' )
    --len;
  if ( len < Word_Min_Size )
    return;
  word[ len ] = '\0';

  ////////// Find the next .SH macro. /////////////////////////////////////////

  bool newline = false;
  while ( c != end ) {
    if ( !newline || *c != '.' ) {      // not macro: forget it
      newline = *c++ == '\n';
      continue;
    }
    newline = *c++ == '\n';

    //
    // Found a macro: is it a comment?  If so, skip it.
    //
    if ( is_man_comment( c, end ) ) {
      newline = true;
      continue;
    }

    //
    // Is the macro ".SH"?  If so, back up the iterator before the '.' so the
    // indexing will stop at the right place.
    //
    if ( move_if_match( c, end, "SH" ) ) {
      c -= 4;                           // 3 for ".SH" + 1 before that
      break;
    }
  } // while

  ////////// Index what's in between. /////////////////////////////////////////

  //
  // Potentially index the words in the section where they are associated with
  // the name of the section as a meta name.
  //
  meta_id_type meta_id = Meta_ID_None;
  if ( associate_meta ) {
    //
    // Do not index the words in the section if either the name of the section
    // (canonicalized to lower case) is among the set of meta names to exclude
    // or not among the set to include.
    //
    if ( (meta_id = find_meta( word )) == Meta_ID_None )
      return;
  }
  //
  // Index the words in between the two .SH macros marking them as being
  // associated with the value of the current section heading name.
  //
  index_words( encoded_char_range( begin, c ), meta_id );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
