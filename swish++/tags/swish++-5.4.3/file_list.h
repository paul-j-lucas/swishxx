/*
**	SWISH++
**	file_list.h
**
**	Copyright (C) 1998  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef	file_list_H
#define	file_list_H

// standard
#include <cstddef>			/* for ptrdiff_t */
#include <iterator>

// local
#include "fake_ansi.h"			/* for iterator, std */
#include "index_segment.h"
#include "word_info.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class file_list
//
// DESCRIPTION
//
//	This class, given a index_segment::const_iterator, accesses the list of
//	files the word is in.  Once an instance is created, the list of files
//	can be iterated over.
//
// SEE ALSO
//
//	index_segment.h
//	word_info.h
//	index.c		write_full_index() for a description of the index file
//			format.
//
//*****************************************************************************
{
public:
	////////// typedefs ///////////////////////////////////////////////////

	typedef int size_type;
	typedef ptrdiff_t difference_type;

	typedef word_info::file value_type;
	typedef value_type const* const_pointer;
	typedef value_type const& const_reference;

	////////// constructors ///////////////////////////////////////////////

	file_list( index_segment::const_iterator const &iter ) :
		ptr_( reinterpret_cast<unsigned char const*>( *iter ) ),
		size_( -1 )			// -1 = "haven't computed yet"
	{
		while ( *ptr_++ ) ;		// skip past word
	}

	////////// member functions ///////////////////////////////////////////

	size_type size() const { return size_ != -1 ? size_ : calc_size(); }

	////////// iterators //////////////////////////////////////////////////

	class const_iterator;
	friend class const_iterator;

	class const_iterator :
		public std::iterator< std::forward_iterator_tag, value_type > {
	public:
		const_iterator() { }

		const_reference operator* () const { return  v_; }
		const_pointer   operator->() const { return &v_; }

		const_iterator& operator++();
		const_iterator  operator++(int) {
			const_iterator tmp = *this;
			return ++*this, tmp;
		}

		friend bool
		operator==( const_iterator const &i, const_iterator const &j ) {
			return i.c_ == j.c_;
		}

		friend bool
		operator!=( const_iterator const &i, const_iterator const &j ) {
			return !( i == j );
		}

	private:
		const_iterator( unsigned char const *p ) : c_( p ) {
			if ( c_ ) operator++();
		}
		unsigned char const *c_;
		value_type v_;
		friend class file_list;
	};

	const_iterator begin() const	{ return const_iterator( ptr_ ); }
	const_iterator end() const	{ return const_iterator( 0 ); }
private:
	unsigned char const	*ptr_;
	mutable size_type	size_;

	size_type calc_size() const;
};

#endif	/* file_list_H */
