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
#include "enc_int.h"
#include "file_list.h"
#include "word_markers.h"

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
	size_ = 0;
	//
	// It would be nice if there were a way to calculate the size of the
	// file list other than by just marching though it.  Since this should
	// be as fast as possible, a much simplified version of the dec_int()
	// code has been inlined here by hand -- a few times.  (We also don't
	// care what the actual numbers are, so there's no point in computing
	// them, so we save having to do two shifts, and logical or for each
	// file.)
	//
	register unsigned char const *p = ptr_;
	while ( true ) {
		++size_;
		while ( *p++ & 0x80 ) ;			// skip file index
		while ( *p++ & 0x80 ) ;			// skip occurrences
		while ( *p++ & 0x80 ) ;			// skip rank

		bool more_lists = true;
		while ( more_lists ) {
			//
			// At this point, p must be pointing to a marker.
			//
			switch ( *p++ ) {		// skip marker
				case Stop_Marker:
					return size_;
				case Word_Entry_Continues_Marker:
					more_lists = false;
					break;
				default:		// skip number
					while ( *p++ != Stop_Marker ) ;
			}
		}
	}
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
//	index.c		write_full_index() for a description of the index file
//			format.
//
//*****************************************************************************
{
	if ( !c_ )					// at "end"
		return *this;

	v_.index_ = dec_int( c_ );
	v_.occurrences_ = dec_int( c_ );
	v_.rank_ = dec_int( c_ );

	if ( !v_.meta_ids_.empty() )
		v_.meta_ids_.erase( v_.meta_ids_.begin(), v_.meta_ids_.end() );

	while ( true ) {
		//
		// At this point, c_ must be pointing to a marker.
		//
		switch ( *c_++ ) {
			case Stop_Marker:
				//
				// Reached end of file list: set iterator to
				// end.
				//
				c_ = 0;
				// no break;

			case Word_Entry_Continues_Marker:
				return *this;

			case Meta_Name_List_Marker:
				while ( *c_ != Stop_Marker )
					v_.meta_ids_.insert( dec_int( c_ ) );
				break;

			default:
				// Encountered a list marker we don't know
				// about: we are decoding a possibly future
				// index file format that has new list types.
				// Since we don't know what to do with it, just
				// skip all the numbers in it.
				//
				while ( *c_ != Stop_Marker )
					dec_int( c_ );
		}
		++c_;					// skip Stop_Marker
	}
}
