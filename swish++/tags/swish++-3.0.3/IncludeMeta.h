/*
**	SWISH++
**	IncludeMeta.h
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

#ifndef IncludeMeta_H
#define IncludeMeta_H

// local
#include "conf_set.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class IncludeMeta : public conf_set
//
// DESCRIPTION
//
//	An IncludeMeta is-a conf_set containing the set of META names to
//	include during indexing.
//
//	This is the same as index's -m command-line option.
//
//*****************************************************************************
{
public:
	IncludeMeta() : conf_set( "IncludeMeta" ) { }
};

#endif	/* IncludeMeta_H */
