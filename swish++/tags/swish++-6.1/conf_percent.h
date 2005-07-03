/*
**	SWISH++
**	conf_percent.h
**
**	Copyright (C) 2001  Paul J. Lucas
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

#ifndef conf_percent_H
#define conf_percent_H

// local
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class conf_percent : public conf<int>
//
// DESCRIPTION
//
//	A conf_percent is-a conf<int> for containing the value of an integer
//	either an as absolute number or a percentage.
//
//*****************************************************************************
{
public:
	int operator()( int size ) {
		return	size + (is_percentage_ ?
			size * operator int() / 100 : operator int()
		);
	}
protected:
	conf_percent( char const *name,
		int default_value, int min = 0, int max = INT_MAX
	);
	CONF_INT_ASSIGN_OPS( conf_percent )

	virtual void	parse_value( char *line );
private:
	bool		is_percentage_;
};

inline conf_percent::conf_percent( char const *name,
	int default_value, int min, int max
)
	: conf<int>( name, default_value, min, max )
{
}

#define	CONF_PERCENT_ASSIGN_OPS(T)		\
	T& operator=( int i ) {			\
		conf_percent::operator=( i );	\
		return *this;			\
	}					\
	T& operator=( std::string const &s ) {	\
		conf_percent::operator=( s );	\
		return *this;			\
	}					\
	T& operator=( char const *s ) {		\
		conf_percent::operator=( s );	\
		return *this;			\
	}					\

#endif	/* conf_percent_H */
/* vim:set noet sw=8 ts=8: */
