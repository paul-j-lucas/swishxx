/*
**	SWISH++
**	file_info.h
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

#ifndef	file_info_H
#define	file_info_H

// standard
#include <cstddef>				/* for size_t */
#include <iostream>
#include <sys/types.h>				/* for off_t */
#include <vector>

// local
#include "my_set.h"

//*****************************************************************************
//
// SYNOPSIS
//
	struct file_info
//
// DESCRIPTION
//
//	This struct is used to contain information for every file encountered
//	during indexing.  A static data member keeps track of all dynamically
//	allocated instances so thay can be iterated over later.
//
//*****************************************************************************
{
	typedef std::vector< file_info* > list_type;
	typedef char_ptr_set name_set_type;

	static list_type	list_;

	file_info(
		char const *file_name, size_t file_size, char const *title,
		int num_words = 0
	);

	char const*		file_name() const	{ return file_name_; }
	int			num_words() const	{ return num_words_; }
	off_t			size() const		{ return size_; }
	char const*		title() const		{ return title_; }

	static int		current_index()	{ return list_.size() - 1; }
	static void		inc_words()	{ ++list_.back()->num_words_; }
	static std::ostream&	out( std::ostream&, char const* );
	static file_info*	parse( char const* );
	static bool		seen_file( char const *file_name ) {
					return name_set_.contains( file_name );
				}
private:
	char const *const	file_name_;
	int			num_words_;
	off_t const		size_;
	char const *const	title_;

	static name_set_type	name_set_;
};

#endif	/* file_info_H */
