/*
**	SWISH++
**	html.h
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

#ifndef	html_H
#define	html_H

// local
#include "file_vector.h"

enum {
	No_Meta_ID		= -1,
	Meta_ID_Not_Found	= -2,
};

char		entity_to_ascii(
			file_vector::const_iterator &pos,
			file_vector::const_iterator end
		);

char const*	grep_title( file_vector const& );

void		parse_html_tag(
			file_vector::const_iterator &pos,
			file_vector::const_iterator end,
			bool is_new_file = false
		);

#endif	/* html_H */
