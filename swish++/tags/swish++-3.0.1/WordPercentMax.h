/*
**	SWISH++
**	WordPercentMax.h
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

#ifndef WordPercentMax_H
#define WordPercentMax_H

// local
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class WordPercentMax : public conf<int>
//
// DESCRIPTION
//
//	An WordPercentMax is-a conf<int> containing the maximum percentage of
//	files a word may occur in before it is discarded as being too
//	frequent.
//
//	This is the same as index's -p command-line option.
//
//*****************************************************************************
{
public:
	WordPercentMax() : conf<int>( "WordPercentMax", 100, 1, 101 ) { }

	WordPercentMax& operator=( int i ) {
		conf<int>::operator=( i );
		return *this;
	}
	WordPercentMax& operator=( char const *s ) {
		conf<int>::operator=( s );
		return *this;
	}
};

#endif	/* WordPercentMax_H */
