/*
**	SWISH++
**	TitleLines.h
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

#ifndef TitleLines_H
#define TitleLines_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class TitleLines : public conf<int>
//
// DESCRIPTION
//
//	A TitleLines is-a conf<int> containing the number of lines to look at
//	for a <TITLE> tag.
//
//	This is the same as index's -t command-line option.
//
//*****************************************************************************
{
public:
	TitleLines() : conf<int>( "TitleLines", TitleLines_Default ) { }
	CONF_INT_ASSIGN_OPS( TitleLines )
};

#endif	/* TitleLines_H */
