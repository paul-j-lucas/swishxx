/*
**	SWISH++
**	IncludeExtension.h
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

#ifndef IncludeExtension_H
#define IncludeExtension_H

// standard
#include <map>
#include <string>

// local
#include "conf_var.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class IncludeExtension :
		public conf_var, public std::map< std::string, bool >
//
// DESCRIPTION
//
//	An IncludeExtension is-a conf_var containing the set of filename
//	extensions of files to include during either indexing or extraction.
//	Additionally, an extension can be inserted marked as being one for HTML
//	files.
//
//	This is the same as either index's or extract's -e and index's -h
//	command-line option.
//
//*****************************************************************************
{
public:
	IncludeExtension() : conf_var( "IncludeExtension" ) {
		alias_name( "HTMLExtension" );
	}

	bool contains( key_type s ) const { return find( s ) != end(); }
	void insert( key_type extension, bool is_html_ext = false ) {
		(*this)[ extension ] = is_html_ext;
	}
private:
	virtual void	parse_value( char const *var_name, char *line );
	virtual void	reset() { clear(); }
};

#endif	/* IncludeExtension_H */
