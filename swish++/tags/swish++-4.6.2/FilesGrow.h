/*
**	SWISH++
**	FilesGrow.h
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

#ifndef FilesGrow_H
#define FilesGrow_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class FilesGrow : public conf<int>
//
// DESCRIPTION
//
//	A FilesGrow is-a conf<int> containing either the absolute number or
//	percentage of files to grow reserved space for when incrementally
//	indexing.
//
//	This is the same as index's -G command-line option.
//
//*****************************************************************************
{
public:
	FilesGrow() : conf<int>( "FilesGrow", FilesGrow_Default, 1 ) { }
	CONF_INT_ASSIGN_OPS( FilesGrow )

	int operator()( int size ) {
		return	size + (is_percentage_ ?
			size * operator int() / 100 : operator int()
		);
	}
protected:
	virtual void	parse_value( char const *var_name, char *line );
private:
	bool	is_percentage_;
};

#endif	/* FilesGrow_H */
