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
#include <new>
#include <string>
#include <sys/types.h>				/* for off_t */
#include <vector>

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
	int		num_words_;

	file_info( char const *file_name, size_t size, char const *title );

	typedef std::vector< file_info* > set_type;
	static set_type set_;

	void*		operator new( size_t );
	void		operator delete( void *p ) {
				// just here to get rid of warning
				std::operator delete( p );
			}

	static int	current_index()		{ return set_.size() - 1; }
	static file_info& current_file()	{ return *set_.back(); }

	friend std::ostream& operator<<( std::ostream&, file_info const& );
private:
	std::string	file_name_;
	off_t		size_;
	std::string	title_;
};

#endif	/* file_info_H */
