/*
**      SWISH++
**      src/word_info.cpp
**
**      Copyright (C) 1998  Paul J. Lucas
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
#include "enc_int.h"
#include "word_info.h"
#include "word_markers.h"
#include "util.h"

using namespace std;

word_info::file::file() {
    // do nothing
}

word_info::file::file( int index ) :
#ifdef FEATURE_word_pos
    last_absolute_word_pos_( 0 ),
#endif
    index_( index ), occurrences_( 1 ), rank_( 0 )
{
    // do nothing else
}

void word_info::file::write_meta_ids( ostream &o ) const {
    o << Meta_Name_List_Marker << assert_stream;
    FOR_EACH( meta_set, meta_ids_, meta_id )
        o << enc_int( *meta_id ) << assert_stream;
    o << Stop_Marker << assert_stream;
}

#ifdef FEATURE_word_pos
void word_info::file::write_word_pos( ostream &o ) const {
    o << Word_Pos_List_Marker << assert_stream;
    FOR_EACH( pos_delta_list, pos_deltas_, pos_delta )
        o << enc_int( *pos_delta ) << assert_stream;
    o << Stop_Marker << assert_stream;
}
#endif /* FEATURE_word_pos */
/* vim:set et sw=4 ts=4: */
