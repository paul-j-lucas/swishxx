/*
**	SWISH++
**	conf_var.h
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

#ifndef conf_var_H
#define conf_var_H

// standard
#include <iostream>
#include <map>

// local
#include "fake_ansi.h"				/* for std */
#include "less.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class conf_var
//
// DESCRIPTION
//
//	A conf_var is the abstract base class for all configuration file
//	variables.  It parses a given line and, based on the variable name,
//	which is the first word on the line, dispatches to an appropriate
//	instance of a derived class to parse the rest of the line.
//
//*****************************************************************************
{
public:
	static void		parse_line( char *line, int line_no );
	char const*		name() const { return name_; }
protected:
	conf_var( char const *var_name ) : name_( var_name ) {
		map_ref()[ name_ ] = this;
	}

	virtual void		parse_value( char *line ) = 0;
	void			parse_const_value( char const *line );

	static ostream&		error()	{ return msg( "error" ); }
	static ostream&		warn()	{ return msg( "warning" ); }
private:
	// Note that the declaration of std::map has a default "Compare"
	// template parameter of "less< key_type >" and, since we've included
	// less.h above that defines "less< char const* >", C-style string
	// comparisons work properly.
	//
	typedef std::map< char const*, conf_var* > map_type;

	char const		*const name_;
	static int		current_config_file_line_no_;

	static map_type&	map_ref();
	static ostream&		msg( char const *kind );
};

//*****************************************************************************
//
// SYNOPSIS
//
	template< class T > class conf;
//
// DESCRIPTION
//
//	This template declaration allows specializations for T later.
//
//*****************************************************************************

#endif	/* conf_var_H */
