/*
**	SWISH++
**	index.h
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

#ifndef	index_H
#define	index_H

// local
#include "fake_ansi.h"
#include "file_vector.h"
#include "html.h"

void		index_words(
			file_vector<char>::const_iterator pos,
			file_vector<char>::const_iterator end,
			bool is_html,
			int meta_id = no_meta_id
		);

#endif	/* index_H */
