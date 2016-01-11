/*
**      SWISH++
**      src/indexer.cpp
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
#include "encoded_char.h"
#include "file_info.h"
#include "ExcludeMeta.h"
#include "IncludeMeta.h"
#include "indexer.h"
#include "iso8859-1.h"
#include "meta_map.h"
#include "stop_words.h"
#include "StoreWordPositions.h"
#include "util.h"
#include "word_info.h"
#include "word_util.h"

// standard
#include <cstring>

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

extern unsigned long  num_indexed_words;
extern unsigned long  num_total_words;
#ifdef WITH_WORD_POS
extern int            word_pos;
#endif /* WITH_WORD_POS */
extern word_map       words;

int                   indexer::suspend_indexing_count_ = 0;
indexer*              indexer::text_indexer_ = nullptr;

///////////////////////////////////////////////////////////////////////////////

indexer::indexer( char const *mod_name ) {
  indexer *&i = map_ref()[ to_lower( mod_name ) ];
  if ( i ) {
    internal_error
      << "indexer::indexer(\"" << mod_name << "\"): "
          "registered more than once" << report_error;
  }
  i = this;
}

indexer::~indexer() {
  // do nothing
}

unique_ptr<option_stream::spec[]>
indexer::all_mods_options( option_stream::spec const *main_spec ) {
  option_stream::spec const *s;

  ////////// Count all options ////////////////////////////////////////////////

  int option_count = 0;
  for ( s = main_spec; s->long_name; ++s )
    ++option_count;
  for ( auto const &mod : map_ref() )
    if ( (s = mod.second->option_spec()) )
      while ( s->long_name )
        ++option_count, ++s;

  ////////// Make combined option_spec ////////////////////////////////////////

  unique_ptr<option_stream::spec[]> combined_spec(
    new option_stream::spec[ option_count + 1 ]
  );
  auto c = combined_spec.get();

  for ( s = main_spec; s->long_name; ++s )
    *c++ = *s;
  for ( auto const &mod : map_ref() )
    if ( (s = mod.second->option_spec()) )
      while ( s->long_name )
        *c++ = *s++;
  c->long_name  = nullptr;
  c->arg_type   = 0;
  c->short_name = 0;
  return combined_spec;
}

void indexer::all_mods_post_options() {
  for ( auto const &mod : map_ref() )
    mod.second->post_options();
}

bool indexer::any_mod_claims_option( option_stream::option const &opt ) {
  for ( auto const &mod : map_ref() )
    if ( mod.second->claims_option( opt ) )
      return true;
  return false;
}

void indexer::all_mods_usage( ostream &o ) {
  for ( auto const &mod : map_ref() )
    mod.second->usage( o );
}

bool indexer::claims_option( option_stream::option const& ) {
  return false;
}

int indexer::find_meta( char const *meta_name ) {
  if ( exclude_meta_names.contains( meta_name ) )
    return Meta_ID_None;

  if ( !include_meta_names.empty() ) {
    //
    // There were meta names explicitly given: see if the meta name is among
    // them.  If not, forget it; if so, possibly reassign the name.
    //
    auto const found = include_meta_names.find( meta_name );
    if ( found == include_meta_names.end() )
      return Meta_ID_None;
    meta_name = found->second;
  }

  //
  // Look up the meta name to get its associated unique integer ID.
  //
  auto const found = meta_names.find( meta_name );
  if ( found != meta_names.end() )
    return found->second;
  //
  // New meta name: add it.  Do this in two statements intentionally because
  // C++ doesn't guarantee that the RHS of assignment is evaluated first.
  //
  int const meta_id = static_cast<int>( meta_names.size() );
  return meta_names[ new_strdup( meta_name ) ] = meta_id;
}

char const* indexer::find_title( mmap_file const& ) const {
  return nullptr;
}

void indexer::index_word( char *word, size_t len, int meta_id ) {
  ++num_total_words;
#ifdef WITH_WORD_POS
  ++word_pos;
#endif /* WITH_WORD_POS */

  if ( len < Word_Hard_Min_Size )
    return;

  if ( suspend_indexing_count_ > 0 ) {
    //
    // A derived indexer class has called suspend_indexing(), so do nothing
    // more.
    //
    // This facility is currently used by HTML_indexer to indicate that the
    // word is within an HTML or XHTML element's begin/end tags whose begin
    // tag's CLASS attribute value is among the set of class names not to
    // index, so do nothing.
    //
    return;
  }

  ////////// Strip chars not in Word_Begin_Chars/Word_End_Chars ///////////////

  for ( int i = len - 1; i >= 0; --i ) {
    if ( is_word_end_char( word[ i ] ) )
      break;
    --len;
  } // for
  if ( len < Word_Hard_Min_Size )
    return;

  word[ len ] = '\0';

  while ( *word ) {
    if ( is_word_begin_char( *word ) )
      break;
    --len, ++word;
  } // while
  if ( len < Word_Hard_Min_Size )
    return;

  ////////// Stop-word checks /////////////////////////////////////////////////

  if ( !is_ok_word( word ) )
    return;

  char const *const lower_word = to_lower( word );
  if ( stop_words->contains( lower_word ) )
    return;

  ////////// Add the word /////////////////////////////////////////////////////

  file_info::inc_words();
  ++num_indexed_words;

  word_info &wi = words[ lower_word ];
  ++wi.occurrences_;

  if ( !wi.files_.empty() ) {
    //
    // We've seen this word before: determine whether we've seen it before in
    // THIS file, and, if so, increment the number of occurrences.
    //
    word_info::file &last_file = wi.files_.back();
    if ( last_file.index_ == file_info::current_index() ) {
      ++last_file.occurrences_;
      goto skip_push_back;
    }
  }

  //
  // First time word occurred in current file.
  //
  wi.files_.push_back( word_info::file( file_info::current_index() ) );

skip_push_back:
  word_info::file &last_file = wi.files_.back();
  if ( meta_id != Meta_ID_None )
    last_file.meta_ids_.insert( meta_id );
#ifdef WITH_WORD_POS
  if ( store_word_positions )
    last_file.add_word_pos( word_pos );
#endif /* WITH_WORD_POS */
}

void indexer::index_words( encoded_char_range const &e, int meta_id ) {
  char  word[ Word_Hard_Max_Size + 1 ];
  bool  in_word = false;
  int   len;

  for ( auto c = e.begin(); !c.at_end(); ) {
    char const ch = iso8859_1_to_ascii( *c++ );

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
      index_word( word, len, meta_id );
    }
  } // for

  if ( in_word ) {
    //
    // We ran into 'end' while still accumulating characters into a word, so
    // just index what we've got.
    //
    index_word( word, len, meta_id );
  }
}

indexer::map_type& indexer::map_ref() {
  static map_type m;
  static bool init;
  if ( !init ) {
    init = true;                        // must set this before init_modules()
    init_modules();                     // defined in init_modules.c
    static indexer text( "text" );
    text_indexer_ = &text;
  }
  return m;
}

option_stream::spec const* indexer::option_spec() const {
  return nullptr;
}

void indexer::post_options() {
  // do nothing
}

char* indexer::tidy_title( char const *begin, char const *end ) {
  // Remove leading spaces
  while ( begin < end && is_space( *begin ) )
    ++begin;

  // Remove trailing spaces
  while ( begin < --end && is_space( *end ) )
    ;
  ++end;

  // Squeeze/convert multiple whitespace characters to single spaces.
  static char title[ Title_Max_Size + 1 ];
  int consec_spaces = 0, len = 0;
  while ( begin < end ) {
    char c = *begin++;
    if ( is_space( c ) ) {
      if ( ++consec_spaces >= 2 )
        continue;
      c = ' ';
    } else {
      consec_spaces = 0;
    }

    title[ len++ ] = c;
    if ( len == Title_Max_Size ) {
      ::strcpy( title + Title_Max_Size - 3, "..." );
      break;
    }
  } // while
  title[ len ] = '\0';
  return title;
}

void indexer::usage( ostream& ) const {
  // do nothing
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
