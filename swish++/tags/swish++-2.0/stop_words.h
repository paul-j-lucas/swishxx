/*
**	SWISH++
**	stop_word.h
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

#ifndef stop_words_H
#define stop_words_H

// local
#include "my_set.h"

struct stop_word_set : string_set {
	stop_word_set( char const *file_name = 0 );
private:
	static char const *const default_stop_word_table[];
};

extern stop_word_set*	stop_words;

#endif	/* stop_words_H */
