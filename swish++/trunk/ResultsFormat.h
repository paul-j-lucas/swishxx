/*
**	SWISH++
**	ResultsFormat.h
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

#ifndef ResultsFormat_H
#define ResultsFormat_H

// local
#include "conf_enum.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class ResultsFormat : public conf_enum
//
// DESCRIPTION
//
//	A ResultsFormat is-a conf_enum containing the search results format:
//	classic or XML.
//
//	This is the same as search's -F command-line option.
//
//*****************************************************************************
{
public:
	ResultsFormat() : conf_enum( "ResultsFormat", legal_values_ ) { }
	CONF_ENUM_ASSIGN_OPS( ResultsFormat )
private:
	static char const *const legal_values_[];
};

extern ResultsFormat results_format;

#endif	/* ResultsFormat_H */
/* vim:set noet sw=8 ts=8: */
