/*
**	SWISH++
**	IncludeFile.h
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

#ifndef IncludeFile_H
#define IncludeFile_H

// local
#include "conf_var.h"
#include "pattern_map.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class IncludeFile : public conf_var, public pattern_map< bool >
//
// DESCRIPTION
//
//	An IncludeFile is-a conf_var containing the set of filename patterns
//	to include during either indexing or extraction.  Additionally, a
//	pattern can be inserted marked as being one for HTML or XHTML files.
//
//	This is the same as either index's or extract's -e and index's -h
//	command-line option.
//
//*****************************************************************************
{
public:
	IncludeFile() : conf_var( "IncludeFile" ) { alias_name( "HTMLFile" ); }
	CONF_VAR_ASSIGN_OPS( IncludeFile )
private:
	virtual void	parse_value( char const *var_name, char *line );
	virtual void	reset() { clear(); }
};

#endif	/* IncludeFile_H */
