/*
**	SWISH++
**	conf_int.h
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

#ifndef conf_int_H
#define conf_int_H

// system
#include <climits>				/* for INT_MAX */
#include <string>

// local
#include "conf_var.h"

//*****************************************************************************
//
// SYNOPSIS
//
	template<> class conf<int> : public conf_var
//
// DESCRIPTION
//
//	A conf<int> is-a conf_var for containing the value of an integer
//	configuration variable.
//
//*****************************************************************************
{
public:
	conf<int>& operator++() {
		++value_;
		return *this;
	}
	conf<int> operator++( int ) {
		conf<int> tmp = *this;
		++value_;
		return tmp;
	}
	conf<int>& operator--() {
		--value_;
		return *this;
	}
	conf<int> operator--( int ) {
		conf<int> tmp = *this;
		--value_;
		return tmp;
	}
	operator int() const { return value_; }
protected:
	conf( char const *name,
		int default_value, int min = 0, int max = INT_MAX
	);
	conf<int>& operator=( int );
	CONF_VAR_ASSIGN_OPS( conf<int> )

	virtual void	parse_value( char *line );
private:
	int const	default_value_, min_, max_;
	int		value_;

	virtual void	reset() { value_ = default_value_; }
};

#define	CONF_INT_ASSIGN_OPS(T)			\
	T& operator=( int i ) {			\
		conf<int>::operator=( i );	\
		return *this;			\
	}					\
	T& operator=( std::string const &s ) {	\
		conf<int>::operator=( s );	\
		return *this;			\
	}					\
	T& operator=( char const *s ) {		\
		conf<int>::operator=( s );	\
		return *this;			\
	}					\

#endif	/* conf_int_H */
