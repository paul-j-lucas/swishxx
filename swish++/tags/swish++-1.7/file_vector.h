/*
**	PJL C++ Library
**	file_vector.h
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

#ifndef file_vector_H
#define file_vector_H

// standard
#include <iostream>
#include <iterator>
#include <sys/types.h>				/* for off_t */

// local
#include "fake_ansi.h"

class file_vector_base {
public:
	typedef off_t size_type;
	typedef ptrdiff_t difference_type;

	file_vector_base()			{ init(); }
	file_vector_base( char const *path, ios::open_mode mode = ios::in ) {
		init();
		open( path, mode );
	}
	~file_vector_base()			{ close(); }

	bool		open( char const *path, ios::open_mode = ios::in );
	void		close();
	bool		empty() const		{ return !size_; }
	int		error() const		{ return error_; }
	size_type	max_size() const	{ return size_; }
	size_type	size() const		{ return size_; }
	operator	bool() const		{ return !error_; }

protected:
	void*		base() const		{ return addr_; }
private:
	size_type	size_;
	int		fd_;			// Unix file descriptor
	void		*addr_;
	int		error_;
	void		init();
};

//*****************************************************************************
//
// SYNOPSYS
//
//	file_vector< T >
//
// DESCRIPTION
//
//	A file_vector is an object that maps a file into memory (via the Unix
//	system call mmap(2)) allowing it to be accessed via iterators.
//	Processing a file, especially files accessed randomly, is MUCH faster
//	than standard I/O.  The type T is usually a character type, but could
//	be a numeric type for reading vectors or matricies of, say, long or
//	double.
//
// SEE ALSO
//
//	mmap(2)
//
//*****************************************************************************

template< class T > class file_vector : public file_vector_base {
public:
	////////// typedefs ///////////////////////////////////////////////////

	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type& reference;
	typedef value_type const& const_reference;

	////////// constructors ///////////////////////////////////////////////

	file_vector() { }
	file_vector( char const *path, ios::open_mode mode = ios::in ) :
		file_vector_base( path, mode ) { }

	////////// iterators //////////////////////////////////////////////////

	typedef pointer iterator;
	typedef const_pointer const_iterator;

	typedef reverse_bidirectional_iterator<
		iterator, value_type, reference, difference_type
	> reverse_iterator;

	typedef reverse_bidirectional_iterator<
		const_iterator, value_type, const_reference, difference_type
	> const_reverse_iterator;

	iterator	begin()		{ return (iterator)base(); }
	const_iterator	begin() const	{ return (const_iterator)base(); }
	iterator	end()		{ return begin() + size(); }
	const_iterator	end() const	{ return begin() + size(); }

	reverse_iterator rbegin() {
		return reverse_iterator( end() );
	}
	reverse_iterator rend() {
		return reverse_iterator( begin() );
	}
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator( end() );
	}
	const_reverse_iterator rend() const {
		return const_reverse_iterator( begin() );
	}

	////////// element access /////////////////////////////////////////////

	reference	back()			{ return *( end() - 1 ); }
	const_reference	back() const		{ return *( end() - 1 ); }
	reference	front()			{ return *begin(); }
	const_reference	front() const		{ return *begin(); }

	reference operator[]( size_type i ) {
		return *( begin() + i );
	}
	const_reference operator[]( size_type i ) const {
		return *( begin() + i );
	}
};

#endif	/* file_vector_H */
