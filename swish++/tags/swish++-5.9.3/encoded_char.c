/*
**	SWISH++
**	encoded_char.c
**
**	Copyright (C) 2002  Paul J. Lucas
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

#ifdef	MOD_mail

// local
#include "encoded_char.h"

encoded_char_range::decoder::set_type encoded_char_range::decoder::set_;

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void encoded_char_range::decoder::reset_all()
//
// SYNOPSIS
//
//	Reset all the registered decoders.
//
//*****************************************************************************
{
	for ( set_type::iterator i = set_.begin(); i != set_.end(); ++i )
		(*i)->reset();
}

#endif	/* MOD_mail */
