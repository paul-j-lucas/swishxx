/*
**	SWISH++
**	StopWordFile.h
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

#ifndef StopWordFile_H
#define StopWordFile_H

// local
#include "conf_string.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class StopWordFile : public conf<std::string>
//
// DESCRIPTION
//
//	A StopWordFile is-a conf<std::string> containing the name of the file
//	containing the stop-words to use instead of the compiled-in set.
//
//	This is the same as either index's or extract's -s command-line option.
//
//*****************************************************************************
{
public:
	StopWordFile() : conf<std::string>( "StopWordFile" ) { }
	CONF_STRING_ASSIGN_OPS( StopWordFile )
};

#endif	/* StopWordFile_H */
