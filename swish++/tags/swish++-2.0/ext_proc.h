/*
**	SWISH++
**	ext_proc.h
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

#ifndef ext_proc_H
#define ext_proc_H

// standard
#include <map>

// local
#include "fake_ansi.h"
#include "less.h"

bool		process_file( char const *path_name, char const *file_name );

//*****************************************************************************
//
// SYNOPSIS
//
	class ext_proc_map
//
// DESCRIPTION
//
//	An ext_proc_map is used to perform fast look-up of a filename
//	extension to determine if that file needs to be preprocessed before
//	it can be operated upon, e.g., uncompressed first.  It uses char
//	const* to point directly to the C-strings in the ext_proc_table[]
//	vector; no strings are copied.
//
// SEE ALSO
//
//	ext_proc.c	ext_proc_table[]
//	extract.c	do_file()
//
//*****************************************************************************
{
public:
	typedef char const* key_type;
	struct value_type {
		char const *undo;
		char const *redo;

		value_type( char const *u = 0, char const *r = 0 ) :
			undo( u ), redo( r ) { }
	};
	typedef value_type const* const_pointer;

	ext_proc_map();

	const_pointer operator[]( key_type key ) const {
		map_type::const_iterator const i = map_.find( key );
		return i != map_.end() ? &i->second : 0;
	}
private:
	//
	// Note that the declaration of std::map has a default "Compare"
	// template parameter of "less< key_type >" and, since we've included
	// less.h above that defines "less< char const* >", C-style string
	// comparisons work properly.
	//
	typedef std::map< key_type, value_type > map_type;
	map_type map_;
};

#endif	/* ext_proc_H */
