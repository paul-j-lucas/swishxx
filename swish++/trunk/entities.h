/*
**	SWISH++
**	entities.h
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

#ifdef	MOD_HTML

#ifndef	entities_H
#define	entities_H

// standard
#include <map>

// local
#include "less.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class char_entity_map
//
// DESCRIPTION
//
//	A char_entity_map is used to perform fast look-up of a character entity
//	reference.  It uses char const* to point directly to the C-strings in
//	the char_entity_table[] vector; no strings are copied.
//
//	The contructor is private, however, to ensure that only instance() can
//	be called to initialize and access a single, static instance.
//
// SEE ALSO
//
//	entites.c	char_entity_table[], instance()
//	mod_html.c	entity_to_ascii()
//
//*****************************************************************************
{
public:
	typedef char const* key_type;
	typedef char value_type;

	static char_entity_map const& instance();

	value_type operator[]( key_type key ) const {
		map_type::const_iterator const i = map_.find( key );
		return i != map_.end() ? i->second : ' ';
	}
private:
	char_entity_map();
	//
	// Note that the declaration of std::map has a default "Compare"
	// template parameter of "less< key_type >" and, since we've included
	// less.h above that defines "less< char const* >", C-style string
	// comparisons work properly.
	//
	typedef std::map< key_type, value_type > map_type;
	map_type map_;
};

#endif	/* entities_H */

#endif	/* MOD_HTML */
