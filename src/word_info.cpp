/*
**      SWISH++
**      src/word_info.cpp
**
**      Copyright (C) 1998-2016  Paul J. Lucas
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
#include "pjl/vlq.h"
#include "util.h"
#include "word_info.h"
#include "word_markers.h"

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

word_info::file::file() {
  // do nothing
}

word_info::file::file( int index ) :
#ifdef WITH_WORD_POS
  last_absolute_word_pos_( 0 ),
#endif /* WITH_WORD_POS */
  index_( index ), occurrences_( 1 ), rank_( 0 )
{
  // do nothing else
}

void word_info::file::write_meta_ids( ostream &o ) const {
  o << Meta_Name_List_Marker << assert_stream;
  for ( auto meta_id : meta_ids_ )
    o << vlq::encode( meta_id ) << assert_stream;
  o << Stop_Marker << assert_stream;
}

#ifdef WITH_WORD_POS
void word_info::file::write_word_pos( ostream &o ) const {
  o << Word_Pos_List_Marker << assert_stream;
  for ( auto pos_delta : pos_deltas_ )
    o << vlq::encode( pos_delta ) << assert_stream;
  o << Stop_Marker << assert_stream;
}
#endif /* WITH_WORD_POS */

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
