/*
**	SWISH++
**	token.h
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

#ifndef	token_H
#define	token_H

// standard
#include <iostream>

// local
#include "config.h"
#include "fake_ansi.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class token
//
// DESCRIPTION
//
//	This class is used to contain a parsed token from an input stream.
//
//*****************************************************************************
{
public:
	enum type {
		no_token,
		word_token,
		word_star_token,
		and_token,
		or_token,
		not_token,
		lparen_token,
		rparen_token,
		equal_token,
	};

	token() : type_( no_token )		{ }
	explicit token( std::istream &in )	{ in >> *this; }

	operator	type() const		{ return type_; }
	int		length() const		{ return len_; }
	char const*	str() const		{ return buf_; }
	char const*	lower_str() const	{ return lower_buf_; }
	void		put_back()		{ hold( this ); }

	friend std::istream& operator>>( std::istream&, token& );
private:
	type		type_;
	char		buf_[ Word_Hard_Max_Size + 1 ];
	char		lower_buf_[ Word_Hard_Max_Size + 1 ];
	int		len_;
	static token*	hold( token* = 0 );
};

#endif	/* token_H */
