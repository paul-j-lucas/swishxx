/*
**	    SWISH++
**	    enc_int.h
**
**	    Copyright (C) 2003  Paul J. Lucas
**
**	    This program is free software; you can redistribute it and/or modify
**	    it under the terms of the GNU General Public License as published by
**	    the Free Software Foundation; either version 2 of the License, or
**	    (at your option) any later version.
**
**	    This program is distributed in the hope that it will be useful,
**	    but WITHOUT ANY WARRANTY; without even the implied warranty of
**	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	    GNU General Public License for more details.
**
**	    You should have received a copy of the GNU General Public License
**	    along with this program; if not, write to the Free Software
**	    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef	enc_int_H
#define	enc_int_H

// standard
#include <iostream>

// local
#include "omanip.h"

int		        dec_int( unsigned char const*& );
std::ostream&	enc_int( std::ostream&, unsigned );

inline PJL::omanip<unsigned> enc_int( unsigned n ) {
	return PJL::omanip<unsigned>( enc_int, n );
}

#endif	/* enc_int_H */
/* vim:set et sw=4 ts=4: */
