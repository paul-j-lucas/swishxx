/*
**	SWISH++
**	mod/rtf/mod_rtf.c
**
**	Copyright (C) 2000  Paul J. Lucas
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

#ifdef	MOD_rtf

// local
#include "config.h"
#include "mod_rtf.h"
#include "util.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	void rtf_indexer::index_words( encoded_char_range const &e, int )
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	text/enriched content type.
//
//	The code is copied-and-pasted from indexer::index_words() because,
//	despite the code redundancy (which isn't all that much), this is much
//	faster than, say, calling an overrideable virtual function for every
//	single character.
//
// PARAMETERS
//
//	e	The encoded character range to be indexed.
//
// SEE ALSO
//
//	Nathaniel S. Borenstein.  "RFC 1563: The text/enriched MIME Content-
//	type," Network Working Group of the Internet Engineering Task Force,
//	January 1994.
//
//	Nathaniel S. Borenstein and Ned Freed.  "RFC: 1341: MIME (Multipurpose
//	Internet Mail Extensions): Mechanisms for Specifying and Describing the
//	Format of Internet Message Bodies," Network Working Group of the
//	Internet Engineering Task Force, June 1992.
//
//*****************************************************************************
{
	char	word[ Word_Hard_Max_Size + 1 ];
	bool	in_word = false;
	int	len;

	encoded_char_range::const_iterator c = e.begin();
	while ( !c.at_end() ) {
		register char ch = *c++;

		////////// Collect a word /////////////////////////////////////

		if ( is_word_char( ch ) ) {
			if ( !in_word ) {
				// start a new word
				word[ 0 ] = ch;
				len = 1;
				in_word = true;
				continue;
			}
			if ( len < Word_Hard_Max_Size ) {
				// continue same word
				word[ len++ ] = ch;
				continue;
			}
			in_word = false;	// too big: skip chars
			while ( !c.at_end() && is_word_char( *c++ ) ) ;
			continue;
		}

		if ( in_word ) {
			//
			// We ran into a non-word character, so index the word
			// up to, but not including, it.
			//
			in_word = false;
			index_word( word, len );
		}

		if ( ch == '<' ) {
			//
			// This is probably the start of command; if so, skip
			// everything until the terminating '>'.
			//
			if ( c.at_end() )
				break;
			if ( (ch = *c++) == '<' )
				continue;	// a literal '<': ignore it
			while ( !c.at_end() && *c++ != '>' ) ;
		}
	}
	if ( in_word ) {
		//
		// We ran into 'end' while still accumulating characters into a
		// word, so just index what we've got.
		//
		index_word( word, len );
	}
}

#endif	/* MOD_rtf */
