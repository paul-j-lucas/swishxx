/*
**	SWISH++
**	AssociateMeta.h
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

#ifndef AssociateMeta_H
#define AssociateMeta_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class AssociateMeta : public conf<bool>
//
// DESCRIPTION
//
//	An AssociateMeta is-a conf<bool> containing the Boolean value
//	indicating whether to associate words to meta names during indexing.
//
//	This is the same as index's -A command-line option.
//
//*****************************************************************************
{
public:
	AssociateMeta() : conf<bool>( "AssociateMeta", true ) { }
	CONF_BOOL_ASSIGN_OPS( AssociateMeta )
};

extern AssociateMeta associate_meta;

#endif	/* AssociateMeta_H */
/* vim:set noet sw=8 ts=8: */
