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

// local
#include "conf_var.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class conf<char const*> : public conf_var
//
// DESCRIPTION
//
//	A conf<char const*> is-a conf_var for containing a configuration
//	variable string value.
//
//*****************************************************************************
{
public:
	operator char const*() const { return value_; }
protected:
	conf( char const *name, char const *default_value = "" );
	conf<char const*>& operator=( char const *s ) {
		parse_const_value( s );
		return *this;
	}
private:
	char const*	const default_value_;
	char const*	value_;

	virtual void	parse_value( char *line );
};

#endif	/* conf_string_H */
