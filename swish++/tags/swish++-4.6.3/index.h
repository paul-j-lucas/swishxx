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
#include "file_vector.h"
#include "html.h"				/* for No_Meta_ID */

void		index_words(
			file_vector::const_iterator pos,
			file_vector::const_iterator end,
			bool is_html, int meta_id = No_Meta_ID,
			bool is_new_file = false
		);

#endif	/* index_H */
