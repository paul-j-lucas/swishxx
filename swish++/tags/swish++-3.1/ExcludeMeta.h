/*
**	SWISH++
**	ExcludeMeta.h
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

#ifndef ExcludeMeta_H
#define ExcludeMeta_H

// local
#include "conf_set.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class ExcludeMeta : public conf_set
//
// DESCRIPTION
//
//	An ExcludeMeta is-a conf_set containing the set of META names to
//	exclude during indexing.
//
//	This is the same as index's -M command-line option.
//
//*****************************************************************************
{
public:
	ExcludeMeta() : conf_set( "ExcludeMeta" ) { }
};

#endif	/* ExcludeMeta_H */
