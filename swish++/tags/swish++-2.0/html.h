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
#include "fake_ansi.h"
#include "file_vector.h"

enum {
	no_meta_id		= -1,
	meta_id_not_found	= -2,
};

char const*	grep_title( file_vector<char> const& );

char		convert_entity(
			file_vector<char>::const_iterator &pos,
			file_vector<char>::const_iterator end
		);

void		parse_html_tag(
			file_vector<char>::const_iterator &pos,
			file_vector<char>::const_iterator end
#ifdef	FEATURE_CLASS
			, bool is_new_file = false
#endif	/* FEATURE_CLASS */
		);

#endif	/* html_H */
