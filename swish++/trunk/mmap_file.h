/*
**	PJL C++ Library
**	mmap_file.h
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

#ifndef mmap_file_H
#define mmap_file_H

// standard
#include <cstddef>			/* for ptrdiff_t, size_t */
#include <fstream>			/* for openmode */
#include <iterator>
#include <sys/mman.h>			/* for mmap(2) */

//
// Some C headers on some implementations define open/close as macros to remap
// them to functions that handle files larger than 2 GB.  This works fine in C,
// but can break C++ where classes have open/close member functions.  Undefine
// them and live with the consequence of not being able to deal with large
// files on such (brain-damaged) implementations.
//
#ifdef	open
#undef	open
#endif
#ifdef	close
#undef	close
#endif

// local
#include "fake_ansi.h"			/* for std */

#ifndef	PJL_NO_NAMESPACES
namespace PJL {
#else
#define	PJL /* nothing */
#endif

//*****************************************************************************
//
// SYNOPSYS
//
	class mmap_file
//
// DESCRIPTION
//
//	An mmap_file is an object that maps a file into memory (via the Unix
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

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef char value_type;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type& reference;
	typedef value_type const& const_reference;

	enum behavior_type {
#ifdef	PJL_NO_MADVISE
		normal,
		random,
		sequential,
#else
		normal		= MADV_NORMAL,
		random		= MADV_RANDOM,
		sequential	= MADV_SEQUENTIAL,
#endif
	};

	////////// constructors & destructor //////////////////////////////////

	mmap_file()			{ init(); }
	mmap_file( char const *path, std::ios::openmode mode = std::ios::in ) {
		init();
		open( path, mode );
	}
	~mmap_file()			{ close(); }

	////////// iterators //////////////////////////////////////////////////

	typedef pointer iterator;
	typedef const_pointer const_iterator;
	typedef std::reverse_iterator< iterator > reverse_iterator;
	typedef std::reverse_iterator< const_iterator > const_reverse_iterator;

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
	int		behavior( behavior_type ) const;
	reference	front()			{ return *begin(); }
	const_reference	front() const		{ return *begin(); }
	bool		open(
				char const *path,
				std::ios::openmode = std::ios::in
			);
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
	int		fd_;			// Unix file descriptor
	void		*addr_;
	mutable int	errno_;
	void		init();
};

#ifndef	PJL_NO_NAMESPACES
}
#endif

#endif	/* mmap_file_H */
