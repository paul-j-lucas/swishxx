/*
**	SWISH++
**	TempDirectory.h
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

#ifndef TempDirectory_H
#define TempDirectory_H

// local
#include "config.h"
#include "conf_string.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class TempDirectory : public conf<char const*>
//
// DESCRIPTION
//
//	A TempDirectory is-a conf<char const*> containing the name of the
//	directory to use for temporary files during indexing.
//
//	This is the same as index's -T command-line option.
//
//*****************************************************************************
{
public:
	TempDirectory() :
		conf<char const*>( "TempDirectory", Temp_Directory_Default ) { }

	TempDirectory& operator=( char const *s ) {
		conf<char const*>::operator=( s );
		return *this;
	}
};

#endif	/* TempDirectory_H */
