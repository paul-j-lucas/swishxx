/*
**	SWISH++
**	word_index.h
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

#ifndef	word_index_H
#define	word_index_H

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
	class word_index
//
// DESCRIPTION
//
//	An instance of this class is used to access either the word, stop-word,
//	or meta-name index portions of a generated index.
//
//	By implementing fully-blown random access iterators for it, the STL
//	algorithms work, in particular binary_search() and equal_range() that
//	are used to look up words.
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

	enum {
		word_index_index = 0,
		// file_index    = 2,
		stop_word_index  = 1,
		meta_name_index  = 3,
	};

	////////// constructors ///////////////////////////////////////////////

	word_index() { }
	word_index( file_vector const &file, int i = word_index_index ) {
		set_index_file( file, i );
	}

	////////// member functions ///////////////////////////////////////////

	void		set_index_file(
				file_vector const&, int = word_index_index
			);
	size_type	size() const		{ return num_words_; }

	////////// iterators //////////////////////////////////////////////////

	class const_iterator;
	friend class const_iterator;

	class const_iterator :
		public random_access_iterator< value_type, difference_type > {
	public:
		const_iterator() { }

		const_reference operator*() const { return (*index_)[ word_ ]; }

		const_iterator& operator++() {
			return ++word_, *this;
		}
		const_iterator& operator--() {
			return --word_, *this;
		}
		const_iterator operator++(int) {
			return const_iterator( index_, word_++ );
		}
		const_iterator operator--(int) {
			return const_iterator( index_, word_-- );
		}
		const_iterator& operator+=( int n ) {
			return word_ += n, *this;
		}
		const_iterator& operator-=( int n ) {
			return word_ -= n, *this;
		}

		friend bool
		operator==( const_iterator const &i, const_iterator const &j ) {
			return i.word_ == j.word_;
		}
		friend bool
		operator!=( const_iterator const &i, const_iterator const &j ) {
			return !( i == j );
		}

		friend const_iterator
		operator+( const_iterator const &i, int n ) {
			return const_iterator( i.index_, i.word_ + n );
		}
		friend const_iterator
		operator-( const_iterator const &i, int n ) {
			return const_iterator( i.index_, i.word_ - n );
		}

		friend difference_type
		operator-( const_iterator const &i, const_iterator const &j ) {
			return i.word_ - j.word_;
		}
	private:
		word_index const *index_;
		long word_;
		const_iterator( word_index const *index, long word_index ) :
			index_( index ), word_( word_index ) { }
		friend class word_index;
	};

	const_iterator begin() const {
		return const_iterator( this, 0 );
	}
	const_iterator end() const {
		return const_iterator( this, num_words_ );
	}
	const_reference operator[]( long i ) const {
		return begin_ + offset_[ i ];
	}
private:
	file_vector::const_iterator	begin_;
	size_type			num_words_;
	off_t const			*offset_;
};

#endif	/* word_index_H */
