/*
**	SWISH++
**	StemWords.h
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

#ifndef StemWords_H
#define StemWords_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class StemWords : public conf<bool>
//
// DESCRIPTION
//
//	A StemWords is-a conf<bool> containing the Boolean value indicating
//	whether to stem words prior to a search.
//
//	This is the same as search's -s command-line option.
//
//*****************************************************************************
{
public:
	StemWords() : conf<bool>( "StemWords", false ) { }
	CONF_BOOL_ASSIGN_OPS( StemWords )
};

extern StemWords stem_words;

#endif	/* StemWords_H */
/* vim:set noet sw=8 ts=8: */
