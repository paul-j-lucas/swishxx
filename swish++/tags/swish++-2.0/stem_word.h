/*
**	SWISH++
**	stem_word.h
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

#ifndef	stem_word_H
#define	stem_word_H

// local
#include "less.h"

extern char const*	stem_word( char const *word );

//*****************************************************************************
//
// SYNOPSIS
//
	struct less_stem : less< char const * >
//
// DESCRIPTION
//
//	A less_stem is-a less< char const* > that compares C-style strings
//	but stems (suffix strips) the words before comparison.
//
//*****************************************************************************
{
	less_stem() { }
	// This constructor doesn't need to be defined, but g++ 2.8.0 complains
	// if it isn't and you try to define a "const less_stem" object.

	bool operator()( char const *a, char const *b ) const {
		return std::strcmp( stem_word( a ), stem_word( b ) ) < 0;
	}
};

#endif	/* stem_word_H */
