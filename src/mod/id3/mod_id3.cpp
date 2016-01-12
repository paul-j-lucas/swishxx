/*
**      SWISH++
**      src/mod/id3/mod_id3.cpp
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
#include "config.h"
#include "AssociateMeta.h"
#include "encoded_char.h"
#include "id3v1.h"
#include "id3v2.h"
#include "mod_id3.h"

// standard
#include <cassert>

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

char const* id3_indexer::find_title( mmap_file const &file ) const {
  char const *c = file.begin();

  id3v2_header header;
  if ( header.parse( c, file.end() ) ) {
    //
    // We have to go through all the ID3v2 frames looking for a TIT2 (title)
    // frame.
    //
    char const *const frame_end = c + header.all_frames_size();
    while ( c < frame_end ) {
      id3v2_frame frame;
      switch ( frame.parse_header( c, header ) ) {
        case id3v2_frame::hr_success:
          if ( !::strcmp( frame.id_, "tit2" ) )
            break;
          // fall through
        case id3v2_frame::hr_failure:
          continue;
        case id3v2_frame::hr_end_of_frames:
          goto id3v1;
      } // switch
      return tidy_title(
        //
        // We have to add 1 byte to skip past the text encoding ID present in
        // all text frames.
        //
        frame.content_begin_ + 1, frame.content_end_
      );
    } // while
  }

  //
  // Either no ID3v2 tag was present or it didn't have a TIT2 (title) frame --
  // see if it has an ID3v1 tag.
  //
id3v1:
  c = file.begin();
  if ( has_id3v1_tag( c, file.end() ) )
    return tidy_title( c, c + ID3v1_Title_Size );

  return nullptr;
}

void id3_indexer::index_id3v1_tags( char const *c, char const *end ) {
  struct id3v1_field {
    char const *name;
    size_t      length;
  };
  static id3v1_field const field_table[] = {
    { "title",    ID3v1_Title_Size    },
    { "artist",   ID3v1_Artist_Size   },
    { "album",    ID3v1_Album_Size    },
    { "year",     ID3v1_Year_Size     },
    { "comments", ID3v1_Comments_Size },
    { "genre",    ID3v1_Genre_Size    },
    { nullptr, 0 }
  };

  if ( !has_id3v1_tag( c, end ) )
    return;

  for ( auto field = field_table; field->name; c += field++->length ) {
    meta_id_type meta_id = Meta_ID_None;
    if ( associate_meta ) {
      //
      // Do not index the words in the value of the field if either the name of
      // the field (canonicalized to lower case) is among the set of meta names
      // to exclude or not among the set to include.
      //
      if ( (meta_id = find_meta( field->name )) == Meta_ID_None )
        continue;
    }
    if ( *field->name == 'g'/* 'g'enre */ ) {
      //
      // Genre is a special case since it is encoded as one byte.
      //
      if ( id3v1_genre const *const g = find_genre( *c ) ) {
        encoded_char_range const e( g->name, g->name + g->length );
        indexer::index_words( e, meta_id );
      }
    } else {
      encoded_char_range const e( c, c + field->length );
      indexer::index_words( e, meta_id );
    }
  } // for
}

bool id3_indexer::index_id3v2_tags( char const *c, char const *end ) {
  id3v2_header header;
  if ( !header.parse( c, end ) )
    return false;

  char const *const frame_end = c + header.all_frames_size();
  bool indexed_at_least_1_frame = false;

  while ( c < frame_end ) {
    id3v2_frame frame;
    switch ( frame.parse_header( c, header ) ) {
      case id3v2_frame::hr_end_of_frames:
        c = frame_end;
        break;
      case id3v2_frame::hr_success: {
        id3v2_frame::parser_ptr const parser_ptr =
          id3v2_frame::find_parser( frame.id_ );
        if ( parser_ptr ) {
          (frame.*parser_ptr)();
          indexed_at_least_1_frame = true;
        }
        break;
      }
      default:
        assert( false );
    } // switch
  } // while

  return indexed_at_least_1_frame;
}

void id3_indexer::index_words( encoded_char_range const &e, meta_id_type ) {
  encoded_char_range::const_iterator c = e.begin();
  //
  // Index the ID3v2 tag if it exists; index the ID3v1 tag only if it doesn't.
  //
  if ( !index_id3v2_tags( c.pos(), c.end_pos() ) )
    index_id3v1_tags( c.pos(), c.end_pos() );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
