/*
**	SWISH++
**	FilterExtension.h
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

#ifndef FilterExtension_H
#define FilterExtension_H

// standard
#include <map>

// local
#include "conf_var.h"
#include "filter.h"
#include "less.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class FilterExtension : public conf_var
//
// DESCRIPTION
//
//	A FilterExtension is-a conf_var for mapping a filename extension to a
//	filter (being a Unix process called via command-line).  Certain
//	filename extensions needs to be filtered first, e.g., uncompressed.
//
//*****************************************************************************
{
public:
	typedef char const* key_type;
	typedef filter value_type;
	typedef value_type const* const_pointer;

	FilterExtension();

	const_pointer operator[]( key_type key ) const {
		map_type::const_iterator const i = map_.find( key );
		return i != map_.end() ? &i->second : 0;
	}
private:
	// Note that the declaration of std::map has a default "Compare"
	// template parameter of "less< key_type >" and, since we've included
	// less.h above that defines "less< char const* >", C-style string
	// comparisons work properly.
	//
	typedef std::map< key_type, value_type > map_type;
	map_type map_;

	virtual void	parse_value( char *line );
};

#endif	/* FilterExtension_H */
