/*
**      SWISH++
**      src/mod/latex/mod_latex.cpp
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

// local
#include "commands.h"
#include "config.h"
#include "encoded_char.h"
#include "iso8859-1.h"
#include "latex_config.h"
#include "mod_latex.h"
#include "TitleLines.h"
#include "util.h"
#include "word_util.h"

using namespace PJL;
using namespace std;

////////// local functions ////////////////////////////////////////////////////

/**
 * Finds a "left" character of a pair.
 * @param c The iterator to use.  It is presumed to be positioned at any
 * character either before or at the "left" character.  It is repositioned
 * after the "left" character only if it was found.
 * @param left The left-hand character to find.
 * @return Returns \c true only if the match was found.
 */
static bool find_left( encoded_char_range::const_iterator &c, char left ) {
  for ( auto d = c; !d.at_end(); ++d ) {
    if ( *d == left ) {
      c = ++d;
      return true;
    }
    if ( *d == '\n' || *d == '\r' )
      break;
  } // for
  return false;
}

/**
 * Given a "left" character of a pair, find its matching "right" character
 * taking care to "balance" intervening pairs, e.g., given '{', find its
 * matching '}'.
 *
 * @param c The iterator to use.  It is presumed to be positioned at any
 * character past the "left" character.  It is repositioned after the matching
 * character only if it was found.
 * @param left The left-hand character to find the match for.  It must be
 * either \c '{' or \c '['.
 * @return Returns \c true only if the match was found.
 */
static bool find_match( encoded_char_range::const_iterator &c, char left ) {
  char const right = left == '{' ? '}' : ']';
  int nesting = 0;

  for ( auto d = c; !d.at_end(); ++d ) {
    if ( *d == left )
      ++nesting;
    else if ( *d == right && --nesting <= 0 ) {
      c = ++d;
      return true;
    }
  } // for
  return false;
}

////////// member functions ///////////////////////////////////////////////////

char const* LaTeX_indexer::find_title( mmap_file const &file ) const {
  int lines = 0;

  encoded_char_range::const_iterator c( file.begin(), file.end() );
  while ( !c.at_end() ) {
    if ( *c == '\n' && ++lines > num_title_lines ) {
      //
      // Didn't find \title{...} within first num_title_lines lines of file:
      // forget it.
      //
      return nullptr;
    }

    //
    // Did we find \title{ yet?
    //
    if ( *c++ != '\\' || !move_if_match( c, "title" ) )
      continue;
    if ( c.at_end() )
      return nullptr;
    if ( *c != '{' )
      continue;
    if ( (++c).at_end() )
      return nullptr;

    //
    // Found the \title{ command: mark the postition after it and look for the
    // closing }.
    //
    mmap_file::const_iterator const start = c.pos();
    while ( !(++c).at_end() )
      if ( *c == '}' )
        return tidy_title( start, c.pos() );
  } // while

  //
  // The file has less than num_title_lines lines and no \title{ was found.
  //
  return nullptr;
}

void LaTeX_indexer::index_words( encoded_char_range const &e, int meta_id ) {
  char        word[ Word_Hard_Max_Size + 1 ];
  bool        in_word = false;
  int         len;
  char const* substitution = nullptr;

  encoded_char_range::const_iterator c = e.begin();
  while ( !c.at_end() ) {
    char ch;

    if ( substitution ) {
      if ( *substitution ) {
        //
        // parse_latex_command() previously returned text to be substituted and
        // indexed: take the next character from said text.
        //
        ch = *substitution++;
        goto collect;
      }
      substitution = nullptr;           // reached end of substitution
    }
    ch = iso8859_1_to_ascii( *c++ );

    ////////// Collect a word /////////////////////////////////////////////////

collect:
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
      in_word = false;            // too big: skip chars
      while ( !c.at_end() && is_word_char( iso8859_1_to_ascii( *c++ ) ) )
        ;
      continue;
    }

    ////////// See if it's a special character ////////////////////////////////

    switch ( ch ) {
      case '%':
        //
        // Skip a comment that starts at a '%' that includes all the remaining
        // characters on the line and any leading whitespace on the next line.
        //
        for ( bool newline = false; !c.at_end(); ++c ) {
          if ( *c == '\n' )
            newline = true;
          else if ( newline && !is_space( *c ) )
            break;
        } // for
        continue;
      case '~':
        ch = ' ';
        break;
      case '\\':
        //
        // Parse a LaTeX command: it may return text to be substituted for the
        // command and indexed.
        //
        substitution = parse_latex_command( c );
        continue;
    } // switch

    ////////// Index a word, maybe ////////////////////////////////////////////

    if ( in_word ) {
      //
      // We ran into a non-word character, so index the word up to, but not
      // including, it.
      //
      in_word = false;
      index_word( word, len, meta_id );
    }
  } // while

  if ( in_word ) {
    //
    // We ran into 'end' while still accumulating characters into a word, so
    // just index what we've got.
    //
    index_word( word, len, meta_id );
  }
}

char const*
LaTeX_indexer::parse_latex_command( encoded_char_range::const_iterator &c ) {
  if ( c.at_end() )
    return nullptr;

  ////////// Deal with elements of a class not to index ///////////////////////

  char command_buf[ Command_Name_Max_Size + 1 ];  // +1 for null
  //
  // Copy only the command name by stopping at a whitespace character (or
  // running into the end of the tag).
  //
  char *to = command_buf;
  encoded_char_range::const_iterator from = c;
  while ( !from.at_end() && is_alnum( *from ) ) {
    //
    // Check to see if the name is too long to be a valid one for a LaTeX
    // command: if it is, invalidate it by writing "garbage" into the name.
    //
    if ( to - command_buf >= Command_Name_Max_Size ) {
      to = command_buf;
      *to++ = '\1';
      break;
    }
    *to++ = *from++;
  } // while
  *to = '\0';

  ////////// Look-up command //////////////////////////////////////////////////

  static command_map const &commands = command_map::instance();
  command_map::const_iterator const cmd = commands.find( command_buf );
  if ( cmd == commands.end() )
    goto skip;

  //
  // We found the command in our internal table: now do different stuff
  // depending upon the action.
  //
  switch ( *cmd->second.action ) {
    case '{':
    case '[': {
      //
      // Just because the command is supposed to use either '{' or '[' doesn't
      // mean it's actually there: try to find it first.  If not found, forget
      // it.
      //
      if ( !find_left( c, *cmd->second.action ) )
        goto skip;
      //
      // Find the matching '}' or ']' and index the words in between.
      //
      encoded_char_range::const_iterator end = c;
      if ( find_match( end, *cmd->second.action ) ) {
        index_words( encoded_char_range( c, end ) );
        c = end;
      }
      return nullptr;
    }
    default: {
        //
        // Substitute the text of the command's "action" as the text to be
        // indexed.
        //
        return cmd->second.action;
    }
  } // switch

skip:
  c = from;
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
