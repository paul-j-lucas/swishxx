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

// local
#include "enc_int.h"
#include "file_list.h"
#include "word_markers.h"

file_list::byte const file_list::const_iterator::end_value = 0;

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
	register byte const *p = ptr_;
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
				default:		// must be a list marker
					while ( *p != Stop_Marker )
						while ( *p++ & 0x80 ) ;
					++p;
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
	if ( !c_ || c_ == &end_value ) {
		//
		// If c_'s value is the "already at end" value (null), or the
		// "just hit end" value, set to the "already at end" value.
		//
		c_ = 0;			
		return *this;
	}

	v_.index_	= dec_int( c_ );
	v_.occurrences_	= dec_int( c_ );
	v_.rank_	= dec_int( c_ );

	if ( !v_.meta_ids_.empty() )
		v_.meta_ids_.clear();

#ifdef	FEATURE_word_pos
	if ( v_.pos_deltas_.empty() )
		v_.pos_deltas_.reserve( v_.occurrences_ );
	else
		v_.pos_deltas_.clear();
#endif

	while ( true ) {
		//
		// At this point, c_ must be pointing to a marker.
		//
		switch ( *c_++ ) {
			case Stop_Marker:
				//
				// Reached the end of file list: set iterator
				// to the "just hit end" value.
				//
				c_ = &end_value;
				// no break;

			case Word_Entry_Continues_Marker:
				return *this;

			case Meta_Name_List_Marker:
				while ( *c_ != Stop_Marker )
					v_.meta_ids_.insert( dec_int( c_ ) );
				break;
#ifdef	FEATURE_word_pos
			case Word_Pos_List_Marker:
				while ( *c_ != Stop_Marker )
					v_.pos_deltas_.push_back( dec_int(c_) );
				break;
#endif
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
/* vim:set noet sw=8 ts=8: */
