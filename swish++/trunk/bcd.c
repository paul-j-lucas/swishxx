/*
**	SWISH++
**	bcd.c
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
#include <iostream>

// local
#include "platform.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& bcd( ostream &o, register unsigned n )
//
// DESCRIPTION
//
//	Write an unsigned integer to the given ostream in binary-coded decimal
//	(BCD).  The number is terminated either by an 0xA nybble or 0xAA byte
//	to mark the end.
//
// PARAMETERS
//
//	o	The ostream to write to.
//
//	n	The number to be written.
//
// RETURN VALUE
//
//	Returns the passed-in ostream.
//
//*****************************************************************************
{
	//
	// First expand the digits into separate bytes (in reverse because it's
	// easier) just like atoi().
	//
	unsigned char buf[ 20 ];
	register unsigned char *p = buf;
	do {
		*p++ = n % 10;
	} while ( n /= 10 );
	--p;

	//
	// Now go through the bytes and compress them into BCD.
	//
	register bool high = true;
	register unsigned char byte;
	do {
		if ( high )
			byte = *p << 4;
		else
			o << static_cast<unsigned char>( byte | *p );
		high = !high;
	} while ( p-- > buf );
	return o << static_cast<unsigned char>( high ? 0xAA : byte | 0x0A );
}

//*****************************************************************************
//
// SYNOPSIS
//
	int parse_bcd( register unsigned char const *&p )
//
// DESCRIPTION
//
//	Parse an integer from a BCD-encoded byte sequence.  An integer that has
//	an odd number of digits is terminated by a low-order nybble with the
//	value 0xA; an integer that has an even number of digits is terminated
//	by a a byte with the value 0xAA.
//
// PARAMETERS
//
//	p	A pointer to the start of the BCD-encoded integer.  After an
//		integer is parsed, it is left one past the terminator
//		character.
//
// RETURN VALUE
//
//	The integer.
//
//*****************************************************************************
{
	register int n = 0;
	register bool high = true;
	while ( *p != 0xAA ) {
		if ( high ) {
			n = n * 10 + ( (*p & 0xF0) >> 4 );
			if ( (*p & 0x0F) == 0x0A )
				break;
		} else
			n = n * 10 + (*p++ & 0x0F);
		high = !high;
	}
	++p;					// leave one past terminator
	return n;
}
