/*
**      SWISH++
**      src/word_info.h
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

#ifndef word_info_H
#define word_info_H

// local
#include "indexer.h"                            /* for Meta_ID_None */
#include "pjl/pjl_set.h"

// standard
#include <list>
#include <map>
#include <set>
#include <string>
#ifdef FEATURE_word_pos
#include <vector>
#endif

//*****************************************************************************
//
// SYNOPSIS
//
        class word_info
//
// DESCRIPTION
//
//      A word_info stores information for a word in the index being generated.
//
//*****************************************************************************
{
public:
    struct file {
        //
        // Every word occurs in one or more files.  An instance of this class
        // stores information for each file a given word occurs in.
        //
        typedef PJL::pjl_set<short> meta_set;
        meta_set        meta_ids_;      // meta name(s) associated with
        bool            has_meta_id( int ) const;
        void            write_meta_ids( std::ostream& ) const;

#ifdef FEATURE_word_pos
        typedef std::vector<short> pos_delta_list;
        int             last_absolute_word_pos_;
        pos_delta_list  pos_deltas_;
        void            add_word_pos( int );
        void            write_word_pos( std::ostream& ) const;
#endif
        int             index_;         // occurs in i-th file
        int             occurrences_;   // in this file only
        int             rank_;

        file();
        explicit file( int index );

        // default copy constructor is OK
        // default assignment operator is OK
    };

    typedef std::list<file> file_list;

    file_list   files_;                 // the files this word is in
    int         occurrences_;           // over all files

    word_info() : occurrences_( 0 ) { }
};

typedef std::map< std::string, word_info > word_map;

////////// inlines ////////////////////////////////////////////////////////////

#ifdef FEATURE_word_pos
inline void word_info::file::add_word_pos( int absolute_pos ) {
    if ( pos_deltas_.empty() )
        pos_deltas_.push_back( absolute_pos );
    else {
        //
        // Store deltas rather than absolute positions because integers are
        // stored in a variable-length binary representation in the generated
        // index file and smaller integers take less bytes.
        // 
        pos_deltas_.push_back( absolute_pos - last_absolute_word_pos_ );
    }
    last_absolute_word_pos_ = absolute_pos;
}
#endif /* FEATURE_word_pos */

inline bool word_info::file::has_meta_id( int meta_id ) const {
    return meta_id == Meta_ID_None || meta_ids_.contains( meta_id );
}
#endif /* word_info_H */
/* vim:set et sw=4 ts=4: */
