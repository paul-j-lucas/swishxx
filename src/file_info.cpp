/*
**      SWISH++
**      src/file_info.cpp
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
#include "FilesReserve.h"
#include "config.h"
#include "directory.h"
#include "file_info.h"
#include "pjl/vlq.h"
#include "util.h"                       /* for new_strdup() */

// standard
#include <cstring>

using namespace PJL;
using namespace std;

file_info::list_type      file_info::list_;
file_info::name_set_type  file_info::name_set_;

FilesReserve              files_reserve;

///////////////////////////////////////////////////////////////////////////////

file_info::file_info( char const *path_name, unsigned dir_index,
                      size_t file_size, char const *title,
                      unsigned num_words ) :
  dir_index_( dir_index ),
  file_name_(
    //
    // First duplicate the entire path name and put it into the set of files
    // encountered; then make file_name_ point to the base name inside the same
    // string, i.e., it shares storage.
    //
    pjl_basename( *name_set_.insert( new_strdup( path_name ) ).first )
  ),
  file_size_( file_size ), num_words_( num_words ),
  title_(
    //
    // If there was a title given, use that; otherwise the title is the file
    // name.  Note that it too shares storage.
    //
    title ? new_strdup( title ) : file_name_
  )
{
  if ( list_.empty() )
    list_.reserve( files_reserve );
  list_.push_back( this );
}

file_info::file_info( unsigned char const *p ) :
  dir_index_( vlq::decode( p ) ),
  file_name_( reinterpret_cast<char const*>( p ) ),
  file_size_(
    vlq::decode( p += ::strlen( reinterpret_cast<char const*>( p ) ) + 1 )
  ),
  num_words_( vlq::decode( p ) ),
  title_( reinterpret_cast<char const*>( p ) )
{
  // do nothing else
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
