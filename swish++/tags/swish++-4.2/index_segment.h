/*
**	SWISH++
**	index_segment.h
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

#ifndef	index_segment_H
#define	index_segment_H

// standard
#include <cstddef>				/* for ptrdiff_t */
#include <iterator>
#include <sys/types.h>				/* for off_t */

// local
#include "file_vector.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class index_segment
//
// DESCRIPTION
//
//	An instance of this class is used to access either the word, stop-word,
//	file, or meta-name index portions of a generated index.
//
//	By implementing fully-blown random access iterators for it, the STL
//	algorithms work, in particular binary_search() and equal_range() that
//	are used to do look ups.
//
//*****************************************************************************
{
public:
	////////// typedefs ///////////////////////////////////////////////////

	typedef long size_type;
	typedef ptrdiff_t difference_type;

	typedef char* value_type;
	typedef char** pointer;
	typedef char const** const_pointer;
	typedef char* reference;
	typedef char const* const_reference;

	enum segment_id {
		word_index	= 0,
		stop_word_index	= 1,
		file_index	= 2,
		meta_name_index	= 3,
	};

	////////// constructors ///////////////////////////////////////////////

	index_segment() { }
	index_segment( file_vector const &file, segment_id id ) {
		set_index_file( file, id );
	}

	////////// member functions ///////////////////////////////////////////

	void		set_index_file( file_vector const&, segment_id );
	size_type	size() const		{ return num_entries_; }

	const_reference operator[]( long i ) const {
		return begin_ + offset_[ i ];
	}
	////////// iterators //////////////////////////////////////////////////

	class const_iterator;
	friend class const_iterator;

	class const_iterator :
		public random_access_iterator< value_type, difference_type > {
	private:
		index_segment const *index_;
		long i_;
		const_iterator( index_segment const *index, long i ) :
			index_( index ), i_( i ) { }
		friend class index_segment;
	public:
		const_iterator() { }

		const_reference operator*() const { return (*index_)[ i_ ]; }

		const_iterator& operator++() {
			return ++i_, *this;
		}
		const_iterator& operator--() {
			return --i_, *this;
		}
		const_iterator operator++(int) {
			return const_iterator( index_, i_++ );
		}
		const_iterator operator--(int) {
			return const_iterator( index_, i_-- );
		}
		const_iterator& operator+=( int n ) {
			return i_ += n, *this;
		}
		const_iterator& operator-=( int n ) {
			return i_ -= n, *this;
		}

		friend bool
		operator==( const_iterator const &i, const_iterator const &j ) {
			return i.i_ == j.i_;
		}
		friend bool
		operator!=( const_iterator const &i, const_iterator const &j ) {
			return !( i == j );
		}

		friend const_iterator
		operator+( const_iterator const &i, int n ) {
			return const_iterator( i.index_, i.i_ + n );
		}
		friend const_iterator
		operator-( const_iterator const &i, int n ) {
			return const_iterator( i.index_, i.i_ - n );
		}

		friend difference_type
		operator-( const_iterator const &i, const_iterator const &j ) {
			return i.i_ - j.i_;
		}
	};

	const_iterator begin() const {
		return const_iterator( this, 0 );
	}
	const_iterator end() const {
		return const_iterator( this, num_entries_ );
	}
private:
	file_vector::const_iterator	begin_;
	size_type			num_entries_;
	off_t const			*offset_;
};

#endif	/* index_segment_H */