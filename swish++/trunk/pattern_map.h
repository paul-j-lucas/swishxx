/*
**	SWISH++
**	pattern_map.h
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

#ifndef pattern_map_H
#define pattern_map_H

// standard
#include <algorithm>			/* for find_if() */
#ifndef	WIN32
#include <fnmatch.h>			/* for fnmatch(3) */
#endif
#include <map>

// local
#ifdef	WIN32
//
// The fnmatch() function, for whatever reason, isn't currently available in
// the Unix-like environment being used to compile SWISH++ under Windows, so
// we use a local copy.
//
#include "fnmatch.h"
#endif
#include "less.h"

//*****************************************************************************
//
// SYNOPSIS
//
	template< class T >
	class pattern_map : public std::map< char const*, T >
//
// DESCRIPTION
//
//	A pattern_map is-a map that maps a filename pattern to an object of
//	type T.
//
// SEE ALSO
//
//	International Standards Organizaion.  "ISO/IEC 9945-2: Information
//	Technology -- Portable Operating System Interface (POSIX) -- Part 2:
//	Shell and Utilities," 1993.
//
// NOTE
//
//	Note that the declaration of std::map has a default "Compare" template
//	parameter of "less< key_type >" and, since we've included less.h above
//	that defines "less< char const* >", C-style string comparisons work
//	properly.
//
//*****************************************************************************
{
public:
	typedef std::map< char const*, T > map_type;
	typedef typename map_type::key_type key_type;
	typedef typename map_type::value_type value_type;
	typedef typename map_type::iterator iterator;
	typedef typename map_type::const_iterator const_iterator;

	//
	// find() must be overridden to use our own comparison class.
	//
	iterator find( char const *file_name ) {
		return ::find_if( begin(), end(), pattern_match( file_name ) );
	}
	const_iterator find( char const *file_name ) const {
		return ::find_if( begin(), end(), pattern_match( file_name ) );
	}

	bool matches( char const *file_name ) const {
		return find( file_name ) != end();
	}
	void insert( char const *pattern, T const &t ) { (*this)[pattern] = t; }
	void insert( value_type const &n ) { map_type::insert( n ); }
private:
	class pattern_match : public unary_function< value_type const&, bool > {
	public:
		pattern_match( char const *file_name ) :
			file_name_( file_name ) { }
		bool operator()( value_type const &map_node ) const {
			return !::fnmatch( map_node.first, file_name_, 0 );
		}
	private:
		char const *const file_name_;
	};
};

#endif	/* pattern_map_H */
