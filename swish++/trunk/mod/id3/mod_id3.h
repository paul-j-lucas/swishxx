/*
**	SWISH++
**	mod/id3/mod_id3.h
**
**	Copyright (C) 2002  Paul J. Lucas
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

#ifdef	MOD_id3

#ifndef	mod_id3_H
#define	mod_id3_H

// local
#include "indexer.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class id3_indexer : public indexer
//
// DESCRIPTION
//
//	An id3_indexer is-an indexer for indexing ID3 tag information typically
//	found in MP3 files.
//
// SEE ALSO
//
//	Martin Nilsson.  "ID3 tag version 2.4.0 - Main Structure," GROUP,
//	November 2000.
//
//*****************************************************************************
{
public:
	id3_indexer() : indexer( "ID3" ) { }

	virtual char const*	find_title( PJL::mmap_file const& ) const;
	virtual void		index_words(
					encoded_char_range const&,
					int meta_id = No_Meta_ID
				);
private:
	void	index_id3v1_tags( char const*, char const* );
	bool	index_id3v2_tags( char const*, char const* );
};

#endif	/* mod_id3_H */

#endif	/* MOD_id3 */
