/*
**      SWISH++
**      mod/id3/mod_id3.c
**
**      Copyright (C) 2002  Paul J. Lucas
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

#ifdef  MOD_id3

// local
#include "AssociateMeta.h"
#include "encoded_char.h"
#include "id3v1.h"
#include "id3v2.h"
#include "mod_id3.h"

using namespace PJL;
using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        char const* id3_indexer::find_title( mmap_file const &file ) const
//
// DESCRIPTION
//
//      Look at the ID3 tags (v2 and v1) for a title.
//
// PARAMETERS
//
//      file    The file presumed to contain an ID3 tag.
//
// RETURN VALUE
//
//      Returns the title string or null if no title can be found.
//
//*****************************************************************************
{
    char const *c = file.begin();

    id3v2_header header;
    if ( header.parse( c, file.end() ) ) {
        //
        // We have to go through all the ID3v2 frames looking for a TIT2
        // (title) frame.
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
            }
            return tidy_title(
                //
                // We have to add 1 byte to skip past the text encoding ID
                // present in all text frames.
                //
                frame.content_begin_ + 1, frame.content_end_
            );
        }
    }

    //
    // Either no ID3v2 tag was present or it didn't have a TIT2 (title) frame
    // -- see if it has an ID3v1 tag.
    //
id3v1:
    c = file.begin();
    if ( has_id3v1_tag( c, file.end() ) )
        return tidy_title( c, c + ID3v1_Title_Size );

    return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void id3_indexer::index_id3v1_tags( char const *c, char const *end )
//
// SYNOPSIS
//
//      Index the ID3v1 tag in the file (if it has any).
//
// PARAMETERS
//
//      c       A pointer to the beginning of the file.
//
//      end     A pointer to one past the end of the file.
//
//*****************************************************************************
{
    struct id3v1_field {
        char const* name;
        int         length;
    };
    static id3v1_field const field_table[] = {
        "title",    ID3v1_Title_Size,
        "artist",   ID3v1_Artist_Size,
        "album",    ID3v1_Album_Size,
        "year",     ID3v1_Year_Size,
        "comments", ID3v1_Comments_Size,
        "genre",    ID3v1_Genre_Size,
        0
    };

    if ( !has_id3v1_tag( c, end ) )
        return;

    for ( id3v1_field const *f = field_table; f->name; c += f++->length ) {
        int meta_id = Meta_ID_None;
        if ( associate_meta ) {
            //
            // Do not index the words in the value of the field if either the
            // name of the field (canonicalized to lower case) is among the set
            // of meta names to exclude or not among the set to include.
            //
            if ( (meta_id = find_meta( f->name )) == Meta_ID_None )
                continue;
        }
        if ( *f->name == 'g'/* 'g'enre */ ) {
            //
            // Genre is a special case since it is encoded as one byte.
            //
            if ( id3v1_genre const *const g = find_genre( *c ) ) {
                encoded_char_range const e( g->name, g->name + g->length );
                indexer::index_words( e, meta_id );
            }
        } else {
            encoded_char_range const e( c, c + f->length );
            indexer::index_words( e, meta_id );
        }
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool id3_indexer::index_id3v2_tags( char const *c, char const *end )
//
// SYNOPSIS
//
//      Index the ID3v2 tag frames in the file (if it has any).
//
// PARAMETERS
//
//      c       The pointer to the beginning of the file to index.
//
//      end     The pointer to one past the end of the file to index.
//
// RETURN VALUE
//
//      Returns true only if at least one ID3v2 tag was parsed.
//
//*****************************************************************************
{
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
            case id3v2_frame::hr_success:
                id3v2_frame::parser const parser =
                    id3v2_frame::find_parser( frame.id_ );
                if ( parser ) {
                    (frame.*parser)();
                    indexed_at_least_1_frame = true;
                }
        }
    }

    return indexed_at_least_1_frame;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void id3_indexer::index_words( encoded_char_range const &e, int )
//
// DESCRIPTION
//
//      Index the words in the ID3 tag of an MP3 file.
//
// PARAMETERS
//
//      e   The encoded character range to be indexed.
//
//*****************************************************************************
{
    encoded_char_range::const_iterator c = e.begin();
    //
    // Index the ID3v2 tag if it exists; index the ID3v1 tag only if it
    // doesn't.
    //
    if ( !index_id3v2_tags( c.pos(), c.end_pos() ) )
        index_id3v1_tags( c.pos(), c.end_pos() );
}

#endif  /* MOD_id3 */
/* vim:set et sw=4 ts=4: */
