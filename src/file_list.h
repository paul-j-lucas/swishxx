/*
**      SWISH++
**      src/file_list.h
**
**      Copyright (C) 1998-2015  Paul J. Lucas
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

#ifndef file_list_H
#define file_list_H

// local
#include "index_segment.h"
#include "word_info.h"

// standard
#include <cstddef>                  /* for ptrdiff_t */
#include <iterator>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %file_list accesses the list of files the word is in.  Once an instance is
 * created, the list of files can be iterated over.
 */
class file_list {
  typedef unsigned char byte;         // for convenience
public:
  ////////// typedefs /////////////////////////////////////////////////////////

  typedef int size_type;
  typedef ptrdiff_t difference_type;
  typedef word_info::file value_type;
  typedef value_type const* const_pointer;
  typedef value_type const& const_reference;

  ////////// constructors /////////////////////////////////////////////////////

  file_list( index_segment::const_iterator const &iter ) :
    ptr_( reinterpret_cast<byte const*>( *iter ) ),
    size_( -1 )                         // -1 = "haven't computed yet"
  {
    while ( *ptr_++ ) ;                 // skip past word
  }

  ////////// iterators ////////////////////////////////////////////////////////

  class const_iterator;
  friend class const_iterator;

  class const_iterator :
    public std::iterator<std::forward_iterator_tag,value_type> {
  public:
    const_iterator() { }
    const_iterator( const_iterator const& ) = default;
    const_iterator& operator=( const_iterator const& ) = default;

    const_reference operator* () const { return  v_; }
    const_pointer   operator->() const { return &v_; }

    const_iterator& operator++();
    const_iterator operator++(int);

    friend bool operator==( const_iterator const &i, const_iterator const &j ) {
      return i.c_ == j.c_;
    }

    friend bool operator!=( const_iterator const &i, const_iterator const &j ) {
      return !( i == j );
    }

  private:
    const_iterator( byte const *p ) : c_( p ) {
      if ( c_ )
        operator++();
    }

    byte const *c_;
    value_type v_;

    static byte const end_value;
    friend class file_list;
  };

  ////////// member functions /////////////////////////////////////////////////

  const_iterator  begin() const       { return const_iterator( ptr_ ); }
  const_iterator  end() const         { return const_iterator( nullptr ); }
  size_type       size() const;

private:
  byte const       *ptr_;
  mutable size_type size_;

  /**
   * Calculates the size of the file list (the number of files the word is in)
   * and caches the result.
   *
   * @return Returns said size.
   */
  size_type calc_size() const;
};

////////// inlines ////////////////////////////////////////////////////////////

inline file_list::const_iterator file_list::const_iterator::operator++(int) {
  const_iterator const temp( *this );
  return ++*this, temp;
}

inline file_list::size_type file_list::size() const {
  return size_ != -1 ? size_ : calc_size();
}

///////////////////////////////////////////////////////////////////////////////

#endif /* file_list_H */
/* vim:set et sw=2 ts=2: */
