/*
**	SWISH++
**	elements.h
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

#ifndef element_map_H
#define element_map_H

// standard
#include <map>

// local
#include "my_set.h"

class element_map;

//*****************************************************************************
//
// SYNOPSIS
//
	class element
//
// DESCRIPTION
//
//	An element contains the information we need about HTML elements.  We
//	only need information about:
//
//	1. Whether an element's end tag is forbidden, optional, or required.
//
//	2. For elements with optional end tags, what tags, some possibly from
//	   other elements, close them.
//
//*****************************************************************************
{
public:
	enum end_tag_value { forbidden, optional, required };

	end_tag_value const	end_tag;
	string_set		close_tags;
private:
	explicit element( end_tag_value v ) : end_tag( v ) { }
	friend class element_map;
};

//*****************************************************************************
//
// SYNOPSIS
//
	struct element_map : std::map< char const*, element >
//
// DESCRIPTION
//
//	An element_map is-a map from the character strings for HTML elements
//	to instances of the element class declared above.  The only reason
//	for having a derived class rather than a typedef is so that we can
//	have a custom constructor that initializes itself.
//
// NOTE
//
//	Note that the declaration of std::map has a default "Compare"
//	template parameter of "less< key_type >" and, since we've included
//	less.h above that defines "less< char const* >", C-style string
//	comparisons work properly.
//
//*****************************************************************************
{
	element_map();
};

#endif	/* element_map_H */
