/*
**	SWISH++
**	conf_enum.h
**
**	Copyright (C) 2000  Paul J. Lucas
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

#ifndef conf_enum_H
#define conf_enum_H

// local
#include "conf_string.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class conf_enum : public conf<std::string>
//
// DESCRIPTION
//
//	A conf_enum is-a conf<std::string> whose value must be of a
//	pre-determined set of legal values.
//
//*****************************************************************************
{
public:
	bool	is_legal( char const*, std::ostream& = std::cerr ) const;
protected:
	conf_enum( char const *name, char const *const legal_values[] );
	CONF_STRING_ASSIGN_OPS( conf_enum )

	virtual void	parse_value( char *line );
private:
	char const *const *const legal_values_;
};

#define	CONF_ENUM_ASSIGN_OPS(T)				\
	T& operator=( std::string const &s ) {		\
		conf_enum::operator=( s );		\
		return *this;				\
	}						\
	T& operator=( char const *s ) {			\
		conf_enum::operator=( s );		\
		return *this;				\
	}

#endif	/* conf_enum_H */
/* vim:set noet sw=8 ts=8: */
