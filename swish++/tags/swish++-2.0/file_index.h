/*
**	SWISH++
**	file_index.h
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

#ifndef	file_index_H
#define	file_index_H

// standard
#include <cstddef>				/* for ptrdiff_t */
#include <iterator>
#include <sys/types.h>				/* for off_t */

// local
#include "fake_ansi.h"
#include "file_vector.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class file_index
//
// DESCRIPTION
//
//	An instance of this class is used to access the file portion of a
//	generated index, i.e., access the (partial) path name of the ith file
//	in the range [0,size).
//
// SEE ALSO
//
//	file_vector.[ch]
//	word_index.[ch]
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

	////////// constructors ///////////////////////////////////////////////

	file_index() { }
	file_index( file_vector<char> const &file ) { set_index_file( file ); }

	////////// member functions ///////////////////////////////////////////

	void		set_index_file( file_vector<char> const& );
	size_type	size() const		{ return num_files_; }

	const_reference operator[]( size_type i ) const {
		return begin_ + offset_[ i ];
	}
private:
	file_vector<char>::const_iterator	begin_;
	size_type				num_files_;
	off_t const				*offset_;
};

#endif	/* file_index_H */
