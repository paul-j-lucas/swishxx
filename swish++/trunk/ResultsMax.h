/*
**	SWISH++
**	ResultsMax.h
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

#ifndef ResultsMax_H
#define ResultsMax_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class ResultsMax : public conf<int>
//
// DESCRIPTION
//
//	A ResultsMax is-a conf_int containing the maximum number of results
//	to return from a search.
//
//	This is the same as search's -m command-line option.
//
//*****************************************************************************
{
public:
	ResultsMax() : conf<int>( "ResultsMax", ResultsMax_Default, 1 ) { }

	ResultsMax& operator=( int i ) {
		conf<int>::operator=( i );
		return *this;
	}
	ResultsMax& operator=( char const *s ) {
		conf<int>::operator=( s );
		return *this;
	}
};

#endif	/* ResultsMax_H */
