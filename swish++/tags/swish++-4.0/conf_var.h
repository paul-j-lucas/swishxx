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
	static void		parse_file( char const *file_name );
	char const*		name() const { return name_; }
protected:
	conf_var( char const *var_name ) : name_( var_name ) {
		alias_name( name_ );
	}
	void		alias_name( char const *var_name );
	static void	parse_line( char *line, int line_no );
	void		parse_const_value( char const *line );
	virtual void	parse_value( char const *var_name, char *line ) = 0;
	virtual void	reset() = 0;

	static std::ostream&	error( std::ostream &o ) {
					return msg( o, "error" );
				}
	static std::ostream&	warning( std::ostream &o ) {
					return msg( o, "warning" );
				}
private:
	// Note that the declaration of std::map has a default "Compare"
	// template parameter of "less< key_type >" and, since we've included
	// less.h above that defines "less< char const* >", C-style string
	// comparisons work properly.
	//
	typedef std::map< char const*, conf_var* > map_type;

	char const		*const name_;
	static int		current_config_file_line_no_;

	void			init( conf_var *alias = 0 );
	static map_type&	map_ref();
	static std::ostream&	msg( std::ostream&, char const *label );
	static void		reset_all();
};

//
// We define this macro for convenience since operator=() is not inherited.
//
#define	CONF_VAR_ASSIGN_OPS(T)				\
	T& operator=( string const &s ) {		\
		parse_const_value( s.c_str() );		\
		return *this;				\
	}						\
	T& operator=( char const *s ) {			\
		parse_const_value( s );			\
		return *this;				\
	}

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
