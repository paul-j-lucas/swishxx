/*
**	SWISH++
**	Incremental.h
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

#ifndef Incremental_H
#define Incremental_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class Incremental : public conf<bool>
//
// DESCRIPTION
//
//	An Incremental is-a conf<bool> containing the Boolean value indicating
//	whether to index incrementally.
//
//	This is the same as index's -I command-line option.
//
//*****************************************************************************
{
public:
	Incremental() : conf<bool>( "Incremental", false ) { }
	CONF_BOOL_ASSIGN_OPS( Incremental )
};

extern Incremental incremental;

#endif	/* Incremental_H */
