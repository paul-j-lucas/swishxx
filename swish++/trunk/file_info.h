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
#include <cstddef>			/* for size_t */
#include <vector>

// local
#include "fake_ansi.h"			/* for std */
#include "my_set.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class file_info
//
// DESCRIPTION
//
//	This is used to contain information for every file encountered during
//	indexing.  A static data member keeps track of all dynamically
//	allocated instances so thay can be iterated over later.
//
//*****************************************************************************
{
public:
	typedef std::vector< file_info* > list_type;
	typedef list_type::const_iterator const_iterator;
	typedef PJL::char_ptr_set name_set_type;

	file_info(
		char const *path_name, int dir_index, size_t file_size,
		char const *title, int num_words = 0
	);

	int			dir_index() const	{ return dir_index_; }
	char const*		file_name() const	{ return file_name_; }
	int			num_words() const	{ return num_words_; }
	size_t			size() const    	{ return size_; }
	char const*		title() const		{ return title_; }

	static const_iterator	begin()		{ return list_.begin();}
	static const_iterator	end()		{ return list_.end(); }
	static int		current_index()	{ return list_.size() - 1; }
	static void		inc_words() 	{ ++list_.back()->num_words_; }
	static file_info*	ith_info( int i ) { return list_[ i ]; }
	static int		num_files()	{ return list_.size(); }
	static bool		seen_file( char const *file_name ) {
					return name_set_.contains( file_name );
				}
private:
	char const *const	file_name_;
	int const		dir_index_;
	int			num_words_;
	size_t const		size_;
	char const *const	title_;

	static list_type	list_;
	static name_set_type	name_set_;

	void			construct();
};

#endif	/* file_info_H */
