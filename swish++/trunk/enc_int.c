/*
**	SWISH++
**	enc_int.c
**
**	Copyright (C) 2003  Paul J. Lucas
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
#include "enc_int.h"
#include "platform.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	int dec_int( register unsigned char const *&p )
//
// DESCRIPTION
//
//	Decode an integer from an encoded byte sequence.  See the comment for
//	enc_int() for details of the encoding scheme.
//
// PARAMETERS
//
//	p	A pointer to the start of the encoded integer.  After an
//		integer is decoded, it is left one past the last byte.
//
// RETURN VALUE
//
//	The integer.
//
//*****************************************************************************
{
	register unsigned n = 0;
	do {
		n = (n << 7) | (*p & 0x7Fu);
	} while ( *p++ & 0x80u );
	return n;
}

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& enc_int( ostream &o, register unsigned n )
//
// DESCRIPTION
//
//	Write an unsigned integer to the given ostream in an encoded format.
//	The format uses a varying number of bytes.  For a given byte, only the
//	lower 7 bits are used for data; the high bit, if set, is used to
//	indicate whether the number continues into the next byte.  The encoded
//	number is written to the given ostream starting with the most
//	significant byte.
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
	unsigned char buf[ 20 ];
	//
	// Encode the number (in reverse because it's easier) just like atoi().
	//
	register unsigned char *p = buf + sizeof buf;
	do {
		*--p = 0x80u | (n & 0x7Fu);
	} while ( n >>= 7 );
	buf[ sizeof buf - 1 ] &= 0x7Fu;	// clear last "continuation bit"

	return o.write( reinterpret_cast<char*>( p ), buf + sizeof buf - p );
}
