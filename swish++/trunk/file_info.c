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

// standard
#include <cstring>

// local
#include "bcd.h"
#include "config.h"
#include "fake_ansi.h"			/* for std, *_CAST */
#include "file_info.h"
#include "FilesReserve.h"

file_info::list_type		file_info::list_;
file_info::name_set_type	file_info::name_set_;

FilesReserve			files_reserve;

//*****************************************************************************
//
// SYNOPSIS
//
	file_info::file_info(
		char const *file_name, size_t file_size, char const *title,
		int num_words
	)
//
// DESCRIPTION
//
//	Construct a file_info.  If a title is given, use it; otherwise set the
//	title to be (just) the file name (not the path name).
//
//	Additionally record its address in a list so the entire list can be
//	iterated over later in the order encountered.  The first time through,
//	reserve files_reserve slots for files.  If exceeded, the vector will
//	automatically grow, but with a slight performance penalty.
//
//*****************************************************************************
:
	file_name_( ::strdup( file_name ) ),
	title_( title ? ::strdup( title ) :
		//
		// Note that in the cases below, title_ will end up pointing
		// within file_name_, i.e., they will share storage.
		//
		(title = ::strrchr( file_name_, '/' )) ? title + 1 : file_name_
	),
	size_( file_size ), num_words_( num_words )
{
	if ( list_.empty() )
		list_.reserve( files_reserve );
	list_.push_back( this );
	name_set_.insert( file_name_ );
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ ostream& file_info::out( ostream &o, char const *p )
//
// DESCRIPTION
//
//	Parse a file_info from an index file and write it to an ostream.
//
// PARAMETERS
//
//	o	The ostream to write to.
//
//	p	A pointer to the first character containing a file_info inside
//		an index file.
//
// RETURN VALUE
//
//	The passed-in ostream.
//
//*****************************************************************************
{
	unsigned char const *u = REINTERPRET_CAST(unsigned char const*)( p );
	o << u;
	while ( *u++ ) ;				// skip past filename
	off_t const size = parse_bcd( u );
	int const num_words = parse_bcd( u );
	return o << ' ' << size << ' ' << u;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ file_info* file_info::parse( char const *p )
//
// DESCRIPTION
//
//	Parse a file_info from an index file to reconstitute an instance.
//
// PARAMETERS
//
//	p	A pointer to the first character containing a file_info inside
//		an index file.
//
//*****************************************************************************
{
	unsigned char const *u = REINTERPRET_CAST(unsigned char const*)( p );
	while ( *u++ ) ;				// skip past filename
	off_t const size = parse_bcd( u );
	int const num_words = parse_bcd( u );
	return new file_info(
		p, size, REINTERPRET_CAST(char const*)( u ), num_words
	);
}
