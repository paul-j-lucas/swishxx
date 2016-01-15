/*
**      SWISH++
**      src/word_info.h
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

#ifndef word_info_H
#define word_info_H

// local
#include "indexer.h"                            /* for Meta_ID_None */
#include "meta_id.h"
#include "pjl/pjl_set.h"

// standard
#include <list>
#include <map>
#include <set>
#include <string>
#ifdef WITH_WORD_POS
#include <vector>
#endif /* WITH_WORD_POS */

///////////////////////////////////////////////////////////////////////////////

/**
 * A %word_info stores information for a word in the index being generated.
 */
class word_info {
public:
  /**
   * Every word occurs in one or more files.  A %file stores information for
   * each file a given word occurs in.
   */
  struct file {
    typedef PJL::pjl_set<meta_id_type> meta_id_set;

    meta_id_set meta_ids_;              // meta name(s) associated with

    bool has_meta_id( meta_id_type ) const;
    void write_meta_ids( std::ostream& ) const;

#ifdef WITH_WORD_POS
    typedef short delta_type;
    typedef std::vector<delta_type> pos_delta_list;
    pos_delta_list pos_deltas_;

    void add_word_pos( unsigned );
    void write_word_pos( std::ostream& ) const;
#endif /* WITH_WORD_POS */

    unsigned index_;                    // occurs in i-th file
    unsigned occurrences_;              // in this file only
    unsigned rank_;

    file();
    explicit file( unsigned index );

    file( file const& ) = default;
    file& operator=( file const& ) = default;
  };

  typedef std::list<file> file_list;

  file_list   files_;                 // the files this word is in
  unsigned    occurrences_;           // over all files

  word_info() : occurrences_( 0 ) { }

  word_info( word_info const& ) = default;
  word_info& operator=( word_info const& ) = default;
};

typedef std::map<std::string,word_info> word_map;

////////// inlines ////////////////////////////////////////////////////////////

#ifdef WITH_WORD_POS
inline void word_info::file::add_word_pos( unsigned absolute_pos ) {
  if ( pos_deltas_.empty() )
    pos_deltas_.push_back( absolute_pos );
  else {
    //
    // Store deltas rather than absolute positions because integers are stored
    // in a variable-length binary representation in the generated index file
    // and smaller integers take less bytes.
    // 
    delta_type const prev_pos = pos_deltas_.back();
    pos_deltas_.push_back( absolute_pos - prev_pos );
  }
}
#endif /* WITH_WORD_POS */

inline bool word_info::file::has_meta_id( meta_id_type meta_id ) const {
  return meta_id == Meta_ID_None || meta_ids_.contains( meta_id );
}

///////////////////////////////////////////////////////////////////////////////

#endif /* word_info_H */
/* vim:set et sw=2 ts=2: */
