/*
**	SWISH++
**	ResultSeparator.h
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

#ifndef ResultSeparator_H
#define ResultSeparator_H

// local
#include "conf_string.h"
#include "fake_ansi.h"			/* for std */

//*****************************************************************************
//
// SYNOPSIS
//
	class ResultSeparator : public conf<std::string>
//
// DESCRIPTION
//
//	A ResultSeparator is-a conf<std::string> containing the string used to
//	separate parts of a search result.
//
//	This is the same as searc's -g command-line option.
//
//*****************************************************************************
{
public:
	ResultSeparator() : conf<std::string>( "ResultSeparator", " " ) { }
	CONF_STRING_ASSIGN_OPS( ResultSeparator )
};

extern ResultSeparator result_separator;

#endif	/* ResultSeparator_H */
