/*
**	SWISH++
**	DirectoriesGrow.h
**
**	Copyright (C) 2001  Paul J. Lucas
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

#ifndef DirectoriesGrow_H
#define DirectoriesGrow_H

// local
#include "config.h"
#include "conf_percent.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class DirectoriesGrow : public conf_percent
//
// DESCRIPTION
//
//	A DirectoriesGrow is-a conf_percent containing either the absolute
//	number or percentage of directories to grow reserved space for when
//	incrementally indexing.
//
//	This is the same as index's -G command-line option.
//
//*****************************************************************************
{
public:
	DirectoriesGrow() :
		conf_percent( "DirectoriesGrow", DirectoriesGrow_Default, 1 ) {}
	CONF_PERCENT_ASSIGN_OPS( DirectoriesGrow )
};

#endif	/* DirectoriesGrow_H */
