/*
**	SWISH++
**	FilesReserve.h
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

#ifndef FilesReserve_H
#define FilesReserve_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class FilesReserve : public conf<int>
//
// DESCRIPTION
//
//	A FilesReserve is-a conf<int> containing the initial number of files to
//	reserve space for; see file_info::operator new() in file_info.c for
//	details.
//
//	This is the same as index's -F command-line option.
//
//*****************************************************************************
{
public:
	FilesReserve() : conf<int>( "FilesReserve", FilesReserve_Default ) { }
	CONF_INT_ASSIGN_OPS( FilesReserve )
};

#endif	/* FilesReserve_H */
