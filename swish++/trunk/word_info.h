/*
**	SWISH++
**	word_info.h
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

#ifndef	word_info_H
#define	word_info_H

// standard
#include <list>
#include <map>
#include <set>
#include <string>

// local
#include "fake_ansi.h"				/* for explicit, std */
#include "indexer.h"				/* for No_Meta_ID */
#include "my_set.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class word_info
//
// DESCRIPTION
//
//	An instance of this class stores information for a word in the index
//	being generated.
//
//*****************************************************************************
{
public:
	struct file {
		typedef my_set< short > meta_set;

		int		index_;		// occurs in i-th file
		meta_set	meta_ids_;	// meta name(s) associated with
		short		occurrences_;	// in this file only
		short		rank_;

		file() { }
		explicit file( int index ) :
			index_( index ), occurrences_( 1 ), rank_( 0 ) { }
		file( int index, int meta_id ) :
			index_( index ), occurrences_( 1 ), rank_( 0 )
		{
			if ( meta_id != No_Meta_ID )
				meta_ids_.insert( meta_id );
		}

		// default "file( file const& )" is ok
		// default "file& operator=( file const& )" is ok

		void	write_meta_ids( std::ostream& ) const;
	};

	typedef std::list< file > file_list;

	file_list	files_;			// the files this word is in
	int		occurrences_;		// over all files

	word_info() : occurrences_( 0 ) { }
};

typedef std::map< std::string, word_info > word_map;

#endif	/* word_info_H */
