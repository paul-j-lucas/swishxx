/*
**      SWISH++
**      src/file_info.h
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

#ifndef file_info_H
#define file_info_H

// local
#include "pjl/hash.h"

// standard
#include <cstddef>                      /* for size_t */
#include <vector>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %file_info contains information for every file encountered during
 * indexing.  A static data member keeps track of all dynamically allocated
 * instances so thay can be iterated over later.
 */
class file_info {
public:
  typedef std::vector<file_info*> list_type;
  typedef list_type::const_iterator const_iterator;
  typedef size_t size_type;
  typedef PJL::unordered_char_ptr_set name_set_type;

  /**
   * Constructs a %file_info.  If a title is given, use it; otherwise set the
   * title to be (just) the file name (not the path name).
   *
   * Additionally record its address in a list so the entire list can be
   * iterated over later in the order encountered.  The first time through,
   * reserve files_reserve slots for files.  If exceeded, the vector will
   * automatically grow, but with a slight performance penalty.
   *
   * @param path_name The full path name of the file.
   * @param dir_index The numerical index of the directory.
   * @param file_size The size of the file in bytes.
   * @param title     The title of the file only if not null.
   * @param num_words The number of words in the file.
   */
  file_info( char const *path_name, unsigned dir_index, size_t file_size,
             char const *title, unsigned num_words = 0 );

  /**
   * Constructs a %file_info from the raw data inside an index file.
   *
   * @param p The pointer to the raw file_info data.
   */
  file_info( unsigned char const *ptr_into_index_file );

  unsigned dir_index() const {
    return dir_index_;
  }

  char const* file_name() const {
    return file_name_;
  }

  unsigned num_words() const {
    return num_words_;
  }

  size_type size() const {
    return file_size_;
  }

  char const* title() const {
    return title_;
  }

  static const_iterator begin() {
    return list_.begin();
  }

  static const_iterator end() {
    return list_.end();
  }

  static unsigned current_index() {
    return static_cast<unsigned>( list_.size() - 1 );
  }

  static void inc_words() {
    ++list_.back()->num_words_;
  }

  static file_info* ith_info( unsigned i ) {
    return list_[i];
  }

  static size_type num_files() {
    return list_.size();
  }

  static bool seen_file( char const *file_name ) {
    return name_set_.find( file_name ) != name_set_.end();
  }

private:
  unsigned const        dir_index_;
  char const *const     file_name_;
  size_type const       file_size_;
  unsigned              num_words_;
  char const *const     title_;

  static list_type      list_;
  static name_set_type  name_set_;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* file_info_H */
/* vim:set et sw=2 ts=2: */
