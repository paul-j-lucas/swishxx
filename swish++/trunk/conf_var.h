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
#include <string>

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
	virtual ~conf_var();

	void		alias_name( char const *var_name );
	//		An instance of a derived class can call this in its
	//		constructor to have more than one variable name map to
	//		itself in the case where what it does is very similar
	//		for all names and doesn't justify creating another
	//		derived class.

	virtual void	parse_value( char const *var_name, char *line ) = 0;
	//		Derived classes must define this to parse a line and
	//		set their value.  The var_name is the name of the
	//		configuration variable whose line is being parsed that
	//		can differ from the name of the derived instance
	//		variable when an alias is involved.

	void		parse_const_value( char const *line );
	//		This is a convenience function that can be called to
	//		parse a line where the line is const.  This is used by
	//		operator=().

	virtual void	reset() = 0;
	//		Reset value to default.  Derived classes must define
	//		this.

	static std::ostream&	error( std::ostream& );
	static std::ostream&	warning( std::ostream& );
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
	static std::ostream&	msg( std::ostream&, char const *label );
	static void		parse_line( char *line, int line_no );
	static void		reset_all();
};

inline std::ostream& conf_var::error( std::ostream &o ) {
	return msg( o, "error" );
}
inline std::ostream& conf_var::warning( std::ostream &o ) {
	return msg( o, "warning" );
}

//
// We define this macro for convenience since operator=() is not inherited.
//
#define	CONF_VAR_ASSIGN_OPS(T)				\
	T& operator=( std::string const &s ) {		\
		parse_const_value( s.c_str() );		\
		return *this;				\
	}						\
	T& operator=( char const *s ) {			\
		parse_const_value( s );			\
		return *this;				\
	}

//
// This template declaration allows specializations for T later.
//
template< class T > class conf;

#endif	/* conf_var_H */
