/*
**	SWISH++
**	conf_string.h
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

#ifndef conf_string_H
#define conf_string_H

// standard
#include <string>

// local
#include "conf_var.h"

//*****************************************************************************
//
// SYNOPSIS
//
	template<> class conf<string> : public conf_var
//
// DESCRIPTION
//
//	A conf<string> is-a conf_var for containing a configuration variable
//	string value.
//
//*****************************************************************************
{
public:
	operator char const*() const	{ return value_.c_str(); }
protected:
	conf( char const *name, char const *default_value = "" );
	CONF_VAR_ASSIGN_OPS( conf<string> )
private:
	string const	default_value_;
	string		value_;

	virtual void	parse_value( char const *var_name, char *line );
	virtual void	reset()		{ value_ = default_value_; }
};

#define	CONF_STRING_ASSIGN_OPS(T)		\
	T& operator=( string const &s ) {	\
		conf<string>::operator=( s );	\
		return *this;			\
	}					\
	T& operator=( char const *s ) {		\
		conf<string>::operator=( s );	\
		return *this;			\
	}

#endif	/* conf_string_H */
