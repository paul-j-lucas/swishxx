/*
**	SWISH++
**	meta_map.h
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

#ifndef	meta_map_H
#define	meta_map_H

// standard
#include <map>

// local
#include "less.h"

//*****************************************************************************
//
// SYNOPSIS
//
	typedef std::map< char const*, int > meta_map;
//
// DESCRIPTION
//
//	A meta_map is used to map a META name to its unique integer ID. IDs
//	start at zero.
//
//	Note that the declaration of std::map has a default "Compare" template
//	parameter of "less< key_type >" and, since we've included less.h above
//	that defines "less< char const* >", C-style string comparisons work
//	properly.
//
//*****************************************************************************

extern meta_map	meta_names;

#endif	/* meta_map_H */
