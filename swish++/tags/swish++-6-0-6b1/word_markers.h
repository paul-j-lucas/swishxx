/*
**	SWISH++
**	word_markers.h
**
**	Copyright (C) 2003  Paul J. Lucas
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

#ifndef	word_markers_H
#define	word_markers_H

unsigned char const	Stop_Marker			= '\x80';
//			This byte marks the end of a list of numbers (such as a
//			meta name list) for a word entry in an index file.  It
//			also stops a word entry itself.

unsigned char const	Meta_Name_List_Marker		= '\x01';
//			This byte marks the beginning of a meta name list for a
//			word entry in an index file.

#ifdef	FEATURE_word_pos
unsigned char const	Word_Pos_List_Marker		= '\x02';
//			This byte marks the beginning of a word position delta
//			list for a word entry in an index file.
#endif

unsigned char const	Word_Entry_Continues_Marker	= '\x00';
//			This byte marks that a word entry continues (the
//			opposite of the Stop_Marker).

#endif	/* word_markers_H */
/* vim:set noet sw=8 ts=8: */
