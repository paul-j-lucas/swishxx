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
#include <cstddef>			/* for size_t */
#include <iterator>

#ifdef	WIN32
#include <windows.h>
#endif

//*****************************************************************************
//
// SYNOPSYS
//
	class file_vector
//
// DESCRIPTION
//
//	A file_vector is an object that maps a file into memory (via the Unix
//	system call mmap(2)) allowing it to be accessed via iterators.
//	Processing a file, especially files accessed randomly, is MUCH faster
//	than standard I/O.
//
// SEE ALSO
//
//	mmap(2)
//
//	W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
//	Addison-Wesley, Reading, MA, 1993.  pp. 407-413
//
//*****************************************************************************
{
public:
	////////// typedefs ///////////////////////////////////////////////////

#ifdef	WIN32
	typedef DWORD size_type;
#else
	typedef size_t size_type;
#endif
	typedef ptrdiff_t difference_type;
	typedef char value_type;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type& reference;
	typedef value_type const& const_reference;

	////////// constructors & destructor //////////////////////////////////

	file_vector()			{ init(); }
	file_vector( char const *path, ios::open_mode mode = ios::in ) {
		init();
		open( path, mode );
	}
	~file_vector()			{ close(); }

	////////// iterators //////////////////////////////////////////////////

	typedef pointer iterator;
	typedef const_pointer const_iterator;

	typedef reverse_bidirectional_iterator<
		iterator, value_type, reference, difference_type
	> reverse_iterator;

	typedef reverse_bidirectional_iterator<
		const_iterator, value_type, const_reference, difference_type
	> const_reverse_iterator;

	iterator	begin()		{ return (iterator)addr_; }
	const_iterator	begin() const	{ return (const_iterator)addr_; }
	iterator	end()		{ return begin() + size_; }
	const_iterator	end() const	{ return begin() + size_; }

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

	////////// member functions ///////////////////////////////////////////

	operator	bool() const		{ return !errno_; }

	reference	back()			{ return *( end() - 1 ); }
	const_reference	back() const		{ return *( end() - 1 ); }
	reference	front()			{ return *begin(); }
	const_reference	front() const		{ return *begin(); }
	bool		open( char const *path, ios::open_mode = ios::in );
	void		close();
	bool		empty() const		{ return !size_; }
	int		error() const		{ return errno_; }
	size_type	max_size() const	{ return size_; }
	size_type	size() const		{ return size_; }

	reference operator[]( size_type i ) {
		return *( begin() + i );
	}
	const_reference operator[]( size_type i ) const {
		return *( begin() + i );
	}
private:
	size_type	size_;
#ifdef	WIN32
	HANDLE		fd_, map_;
#else
	int		fd_;			// Unix file descriptor
#endif
	void		*addr_;
	int		errno_;
	void		init();
};

#endif	/* file_vector_H */
