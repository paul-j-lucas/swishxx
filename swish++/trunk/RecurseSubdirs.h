/*
**	SWISH++
**	RecurseSubdirs.h
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

#ifndef RecurseSubdirs_H
#define RecurseSubdirs_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class RecurseSubdirs : public conf<bool>
//
// DESCRIPTION
//
//	A RecurseSubdirs is-a conf<bool> containing the Boolean value
//	indicating whether to recurse subdirectories either during indexing or
//	extraction.
//
//	This is the same as either index's or extract's -r command-line option.
//
//*****************************************************************************
{
public:
	RecurseSubdirs() : conf<bool>( "RecurseSubdirs", true ) { }
	CONF_BOOL_ASSIGN_OPS( RecurseSubdirs )
};

#endif	/* RecurseSubdirs_H */
