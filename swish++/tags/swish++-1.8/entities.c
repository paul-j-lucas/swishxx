/*
**	SWISH++
**	entities.c
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

// local
#include "entities.h"

//
// All numeric entities are from the ISO 8859-1 character set.
//
char const num_entities[] = {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//   0
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  31
	' ', '!', '"', '#', '$', '%', '&', '\'',//  32
	'(', ')', '*', '+', ',', '-', '.', '/',	//  |
	'0', '1', '2', '3', '4', '5', '6', '7',	//  |
	'8', '9', ':', ';', '<', '=', '>', '?',	//  |
	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',	//  |
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',	//  |
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',	//  |
	'X', 'Y', 'Z', '[', '\\', ']', '^', '_',//  |
	'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',	//  |
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',	//  |
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',	//  |
	'x', 'y', 'z', '{', '|', '}', '~', ' ',	// 127
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	// 128
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	// 159
	' ', '!', ' ', '#', ' ', ' ', '|', ' ',	// 160
	' ', ' ', ' ', '"', ' ', '-', ' ', ' ',	//  |
	' ', ' ', '2', '3', '\'', ' ', ' ', '.',//  |
	' ', '1', ' ', '"', ' ', ' ', ' ', '?',	//  |
	'A', 'A', 'A', 'A', 'A', 'A', 'E', 'C',	//  |
	'E', 'E', 'E', 'E', 'I', 'I', 'I', 'I',	//  |
	'D', 'N', 'O', 'O', 'O', 'O', 'O', ' ',	//  |
	'O', 'U', 'U', 'U', 'U', 'Y', ' ', 's',	//  |
	'a', 'a', 'a', 'a', 'a', 'a', 'e', 'c',	//  |
	'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i',	//  |
	'd', 'n', 'o', 'o', 'o', 'o', 'o', ' ',	//  |
	'o', 'u', 'u', 'u', 'u', 'y', ' ', 'y',	// 255
};

//*****************************************************************************
//
// SYNOPSIS
//
	char_entity_map::char_entity_map()
//
// DESCRIPTION
//
//	Construct (initialize) a char_entity_map.  Any entity that isn't
//	listed below will convert to a space.  Note that is isn't necessary
//	to convert "&lt;" and "&gt;" since such entities aren't indexed
//	anyway (but "&amp;" is when part of an acronym like "AT&T").
//
// SEE ALSO
//
//	html.c		convert_entity()
//
//*****************************************************************************
{
	struct char_entity {
		char const*	name;
		char		char_equiv;
	};
	static char_entity const char_entity_table[] = {
		"amp",    '&',
		"Aacute", 'A', "aacute", 'a',
		"Acirc",  'A', "acirc",  'a',
		"AElig",  'A', "aelig",  'a',
		"Agrave", 'A', "agrave", 'a',
		"Aring",  'A', "aring",  'a',
		"Atilde", 'A', "atilde", 'a',
		"Auml",   'A', "auml",   'a',
		"Ccedil", 'C', "ccedil", 'c',
		"Eacute", 'E', "eacute", 'e',
		"Ecirc",  'E', "ecirc",  'e',
		"Egrave", 'E', "egrave", 'e',
		"ETH",    'D', "eth",    'd',
		"Euml",   'E', "euml",   'e',
		"Iacute", 'I', "iacute", 'i',
		"Icirc",  'I', "icirc",  'i',
		"Igrave", 'E', "igrave", 'i',
		"Iuml",   'I', "iuml",   'i',
		"Ntilde", 'N', "ntilde", 'n',
		"Oacute", 'O', "oacute", 'o',
		"Ocirc",  'O', "ocirc",  'o',
		"Ograve", 'O', "ograve", 'o',
		"Oslash", 'O', "oslash", 'o',
		"Otilde", 'O', "otilde", 'o',
		"Ouml",   'O', "ouml",   'o',
		"szlig",  's',
		"Uacute", 'U', "uacute", 'u',
		"Ucirc",  'U', "ucirc",  'u',
		"Ugrave", 'U', "ugrave", 'u',
		"Uuml",   'U', "uuml",   'u',
		"Yacute", 'Y', "yacute", 'y',
		"yuml",   'y',

		0
	};

	for ( register char_entity const *e = char_entity_table; e->name; ++e )
		map_[ e->name ] = e->char_equiv;
}
