/*
**	SWISH++
**	file_info.c
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

// local
#include "config.h"
#include "fake_ansi.h"				/* for std, *_CAST */
#include "file_info.h"

extern int		num_files_reserve;
file_info::set_type	file_info::set_;

//*****************************************************************************
//
// SYNOPSIS
//
	file_info::file_info(
		char const *file_name, size_t size, char const *title
	)
//
// DESCRIPTION
//
//	Construct a file_info.  This is out-of-line since it's doubtful that
//	a compiler will inline the initialization of all the data members.
//
//*****************************************************************************
:
	file_name_( file_name ), title_( title ), size_( size ), num_words_( 0 )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
	void* file_info::operator new( size_t size )
//
// DESCRIPTION
//
//	Allocate the memory as usual for an instance of file_info, but also
//	record its address so the entire collection can be iterated over
//	later.  Also, the first time through, reserve num_files_reserve slots
//	for files.  If exceeded, the vector will automatically grow, but with a
//	slight performance penalty.
//
// PARAMETERS
//
//	size	The size of the object to be allocated passed to us by the
//		implementation.
//
// RETURN VALUE
//
//	The pointer to the raw memory for the object to be constructed in.
//
//*****************************************************************************
{
	if ( set_.empty() )
		set_.reserve( num_files_reserve );
	void *const p = std::operator new( size );
	set_.push_back( STATIC_CAST( file_info* )( p ) );
	return p;
}

//*****************************************************************************
//
// SYNOPSIS
//
	std::ostream& operator<<( std::ostream &o, file_info const &f )
//
// DESCRIPTION
//
//	Write the representation for a file_info to the given stream.
//
// PARAMETERS
//
//	o	The ostream to write to.
//
// RETURN VALUE
//
//	Returns the given ostream as is standard practice.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA.  p. 612.
//
//*****************************************************************************
{
	return o << f.file_name_ << ' ' << f.size_ << ' ' << f.title_;
}
