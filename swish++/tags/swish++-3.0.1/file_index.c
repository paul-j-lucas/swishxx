/*
**	SWISH++
**	file_index.c
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

// local
#include "file_index.h"
#include "util.h"

//*****************************************************************************
//
// SYNOPSIS
//
	void file_index::set_index_file( file_vector<char> const &file )
//
// DESCRIPTION
//
//	Set the index file to use by setting data members to the proper
//	positions withing the index file.
//
// CAVEAT
//
//	Ideally, this function would be part of the constructor, but the name
//	of the index file can be passed in via the command line and that isn't
//	parsed until after the instance is constructed.
//
// SEE ALSO
//
//	index.c		write_full_index() -- format of index file
//
//*****************************************************************************
{
	begin_ = file.begin();
	get_index_info( file, 2, &num_files_, &offset_ );
}
