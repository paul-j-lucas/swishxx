/*
**	SWISH++
**	PidFile.h
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

#ifdef	SEARCH_DAEMON

#ifndef PidFile_H
#define PidFile_H

// local
#include "conf_string.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class PidFile : public conf<std::string>
//
// DESCRIPTION
//
//	A PidFile is-a conf<std::string> containing the name of the file to
//	record the search daemon's process ID.
//
//	This is the same as search's -P command-line option.
//
//*****************************************************************************
{
public:
	PidFile() : conf<std::string>( "PidFile" ) { }
	CONF_STRING_ASSIGN_OPS( PidFile )
};

extern PidFile pid_file_name;

#endif	/* PidFile_H */

#endif	/* SEARCH_DAEMON */
