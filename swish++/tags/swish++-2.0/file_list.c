/*
**	SWISH++
**	file_list.c
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
#include <cctype>

// local
#include "file_list.h"
#include "html.h"				/* for no_meta_id */

//*****************************************************************************
//
// SYNOPSIS
//
	file_list::size_type file_list::calc_size() const
//
// DESCRIPTION
//
//	Calculate the size of the file list (the number of files the word is
//	in) and cache the result.
//
// RETURN VALUE
//
//	Returns the size.
//
//*****************************************************************************
{
#ifdef	PJL_NO_MUTABLE
#	define THIS	( CONST_CAST( file_list* )( this ) )
#else
#	define THIS	this
#endif
	THIS->size_ = 0;
	for ( register char const *c = ptr; *c; ++c )
		if ( *c == ' ' )
			++THIS->size_;
	return THIS->size_ /= 2;
}

//*****************************************************************************
//
// SYNOPSIS
//
	file_list::const_iterator& file_list::const_iterator::operator++()
//
// DESCRIPTION
//
//	Advance a file_list::const_iterator.
//
// RETURN VALUE
//
//	Reference to itself as is standard practice for iterators.
//
// SEE ALSO
//
//	word_index.h
//	index.c		write_full_index() for a description of the index file
//			format.
//
//*****************************************************************************
{
	if ( !*c_ ) {
		//
		// Reached end of null-terminated list: set iterator to end.
		//
		c_ = 0;
		return *this;
	}

	v_.index_ = 0;
	while ( isdigit( *c_ ) ) v_.index_ = v_.index_ * 10 + *c_++ - '0';

	if ( !v_.meta_ids_.empty() )
		v_.meta_ids_.erase( v_.meta_ids_.begin(), v_.meta_ids_.end() );
	while ( *c_ == '\2' ) {			// is a META ID
		++c_;
		register int meta_id = 0;
		while ( isdigit( *c_ ) ) meta_id = meta_id * 10 + *c_++ - '0';
		v_.meta_ids_.insert( meta_id );
	}
	++c_;					// skip past non-digit

	v_.rank_ = 0;
	while ( *c_ != '\1' ) v_.rank_  = v_.rank_  * 10 + *c_++ - '0';
	++c_;					// skip past non-digit

	return *this;
}
