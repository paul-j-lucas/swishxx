/*
**	SWISH++
**	file_info.c
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

// standard
#include <cstring>

// local
#include "config.h"
#include "directory.h"
#include "FilesReserve.h"
#include "file_info.h"
#include "platform.h"
#include "util.h"			/* for new_strdup() */

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

file_info::list_type		file_info::list_;
file_info::name_set_type	file_info::name_set_;

FilesReserve			files_reserve;

//*****************************************************************************
//
// SYNOPSIS
//
	file_info::file_info(
		char const *path_name, int dir_index, size_t file_size,
		char const *title, int num_words
	)
//
// DESCRIPTION
//
//	Construct a file_info.  If a title is given, use it; otherwise set the
//	title to be (just) the file name (not the path name).
//
//	Additionally record its address in a list so the entire list can be
//	iterated over later in the order encountered.  The first time through,
//	reserve files_reserve slots for files.  If exceeded, the vector will
//	automatically grow, but with a slight performance penalty.
//
// PARAMETERS
//
//	path_name	The full path name of the file.
//
//	dir_index	The numerical index of the directory.
//
//	file_size	The size of the file in bytes.
//
//	title		The title of the file only if not null.
//
//	num_words	The number of words in the file.
//
//*****************************************************************************
	: file_name_(
		//
		// First duplicate the entire path name and put it into the set
		// of files encountered; then make file_name_ point to the base
		// name inside the same string, i.e., it shares storage.
		//
		pjl_basename(
			*name_set_.insert( new_strdup( path_name ) ).first
		)
	  ),
	  dir_index_( dir_index ),
	  num_words_( num_words ), size_( file_size ),
	  title_(
		//
		// If there was a title given, use that; otherwise the title is
		// the file name.  Note that it too shares storage.
		//
		title ? new_strdup( title ) : file_name_
	  )
{
	if ( list_.empty() )
		list_.reserve( files_reserve );
	list_.push_back( this );
}
