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
#include "bcd.h"
#include "fake_ansi.h"				/* for CONST_CAST */
#include "file_list.h"

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

	//
	// It would be nice if there were a way to calculate the size of the
	// file list other than by just marching though it.  Since this should
	// be as fast as possible, a much simplified version of the parse_bcd()
	// code has been inlined here by hand -- twice.  (We also don't care
	// what the actual numbers are, so there's no point in computing them,
	// so we save having to do two multiplies, adds, shifts, and logical
	// ands for each file.)
	//
	for ( register unsigned char const *p = ptr_; *p != 0xFF; ) {
		while ( (*p++ & 0x0F) != 0x0A ) ;	// skip past file index
		if ( *p == 0xEE ) {			// META ID list follows
			while ( *++p != 0xEE ) ;
			++p;				// skip past 0xEE
		}
		while ( (*p++ & 0x0F) != 0x0A ) ;	// skip past rank
		++THIS->size_;
	}
	return size_;
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
	if ( *c_ == 0xFF ) {
		//
		// Reached the end of the list: set iterator to end.
		//
		c_ = 0;
		return *this;
	}

	v_.index_ = parse_bcd( c_ );

	if ( !v_.meta_ids_.empty() )
		v_.meta_ids_.erase( v_.meta_ids_.begin(), v_.meta_ids_.end() );
	if ( *c_ == 0xEE ) {			// META ID list follows
		++c_;				// skip past 0xEE
		while ( *c_ != 0xEE )
			v_.meta_ids_.insert( parse_bcd( c_ ) );
		++c_;				// skip past 0xEE
	}

	v_.rank_ = parse_bcd( c_ );

	return *this;
}
