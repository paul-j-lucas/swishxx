/*
**	SWISH++
**	token.c
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

// standard
#include <cctype>
#include <cstring>
#include <iostream>

// local
#include "config.h"
#include "platform.h"
#include "token.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SNYOPSIS
//
	/* static */ token* token::hold( token *t )
//
// DESCRIPTION
//
//	Put back and hold previosuly scanned tokens.  We need somplace to
//	store tokens that were put back but obviously can't declare a token
//	static data member inside token directly.
//
//*****************************************************************************
{
	// Our query parser needs at most 2 look-ahead tokens.
	static token v[ 2 ];
	static int top = -1;
	if ( t ) {
		v[ ++top ] = *t;
		return v + top;
	} else
		return top >= 0 ? v + top-- : 0;
}

//*****************************************************************************
//
// SNYOPSIS
//
	istream& operator>>( istream &in, token &t )
//
// DESCRIPTION
//
//	Extract a token from the given input stream.
//
// PARAMETERS
//
//	in	The input stream to extract the token from.
//
//	t	The newly extracted token.
//
// RETURN VALUE
//
//	Returns the given input stream as is standard practice.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA.  p. 621.
//
//*****************************************************************************
{
	token *const hold = token::hold();
	if ( hold ) {
		//
		// If there was a token previously put back, use it.
		//
		t = *hold;
		return in;
	}
	t.type_ = token::no_token;
	bool in_word = false;
	char c;

	while ( in.get( c ) ) {

		if ( is_word_char( c ) ) {
			if ( !in_word ) {
				// start a new word
				t.buf_[ 0 ] = c;
				t.len_ = 1;
				in_word = true;
				continue;
			}
			if ( t.len_ < Word_Hard_Max_Size ) {
				// continue same word
				t.buf_[ t.len_++ ] = c;
				continue;
			}
			in_word = false;		// too big: skip chars
			while ( in.get( c ) && is_word_char( c ) ) ;
			continue;
		}

		if ( in_word ) {
			if ( c == '*' )
				t.type_ = token::word_star_token;
			else
				in.putback( c );
			break;
		}

		switch ( c ) {
			case '(':
				t.type_ = token::lparen_token;
				return in;
			case ')':
				t.type_ = token::rparen_token;
				return in;
			case '=':
				t.type_ = token::equal_token;
				return in;
		}
	}

	if ( in_word ) {
		t.buf_[ t.len_ ] = '\0';
		::strcpy( t.lower_buf_, to_lower( t.buf_ ) );
		if ( t.type_ )
			return in;

		//
		// Check to see if the word is an operator.  For only 3
		// comparisons, linear search is good enough.
		//
		if ( !::strcmp( t.lower_buf_, "and" ) )
			t.type_ = token::and_token;
		else if ( !::strcmp( t.lower_buf_, "or"  ) )
			t.type_ = token::or_token;
		else if ( !::strcmp( t.lower_buf_, "not" ) )
			t.type_ = token::not_token;
		else
			t.type_ = token::word_token;
	}

	return in;
}
