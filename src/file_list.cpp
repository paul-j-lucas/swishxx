/*
**      SWISH++
**      src/file_list.cpp
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
#include "file_list.h"
#include "pjl/vlq.h"
#include "word_markers.h"

using namespace PJL;

///////////////////////////////////////////////////////////////////////////////

file_list::byte const file_list::const_iterator::end_value = 0;

file_list::size_type file_list::calc_size() const {
  size_ = 0;
  //
  // It would be nice if there were a way to calculate the size of the file
  // list other than by just marching though it.  Since this should be as fast
  // as possible, a much simplified version of the vlq::decode() code has been
  // inlined here by hand -- a few times.  (We also don't care what the actual
  // numbers are, so there's no point in computing them, so we save having to
  // do two shifts, and logical or for each file.)
  //
  byte const *p = ptr_;
  while ( true ) {
    ++size_;
    while ( *p++ & 0x80 ) ;             // skip file index
    while ( *p++ & 0x80 ) ;             // skip occurrences
    while ( *p++ & 0x80 ) ;             // skip rank

    bool more_lists = true;
    while ( more_lists ) {
      //
      // At this point, p must be pointing to a marker.
      //
      switch ( *p++ ) {                 // skip marker
        case Stop_Marker:
          return size_;
        case Word_Entry_Continues_Marker:
          more_lists = false;
          break;
        default:                        // must be a list marker
          while ( *p != Stop_Marker )
            while ( *p++ & 0x80 )
              ;
          ++p;
      } // switch
    } // while
  } // while
}

file_list::const_iterator& file_list::const_iterator::operator++() {
  if ( !c_ || c_ == &end_value ) {
    //
    // If c_'s value is the "already at end" value (null), or the "just hit
    // end" value, set to the "already at end" value.
    //
    c_ = 0;         
    return *this;
  }

  v_.index_       = vlq::decode( c_ );
  v_.occurrences_ = vlq::decode( c_ );
  v_.rank_        = vlq::decode( c_ );

  if ( !v_.meta_ids_.empty() )
    v_.meta_ids_.clear();

#ifdef WITH_WORD_POS
  if ( v_.pos_deltas_.empty() )
    v_.pos_deltas_.reserve( v_.occurrences_ );
  else
    v_.pos_deltas_.clear();
#endif /* WITH_WORD_POS */

  while ( true ) {
    //
    // At this point, c_ must be pointing to a marker.
    //
    switch ( *c_++ ) {
      case Stop_Marker:
        //
        // Reached the end of file list: set iterator to the "just hit end"
        // value.
        //
        c_ = &end_value;
        // no break;

      case Word_Entry_Continues_Marker:
        return *this;

      case Meta_Name_List_Marker:
        while ( *c_ != Stop_Marker )
          v_.meta_ids_.insert( vlq::decode( c_ ) );
        break;
#ifdef WITH_WORD_POS
      case Word_Pos_List_Marker:
        while ( *c_ != Stop_Marker )
          v_.pos_deltas_.push_back( vlq::decode(c_) );
        break;
#endif /* WITH_WORD_POS */
      default:
        //
        // Encountered a list marker we don't know about: we are decoding a
        // possibly future index file format that has new list types.  Since we
        // don't know what to do with it, just skip all the numbers in it.
        //
        while ( *c_ != Stop_Marker )
          (void)vlq::decode( c_ );
    } // switch
    ++c_;                               // skip Stop_Marker
  } // while
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
