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

//*****************************************************************************
//
// SYNOPSIS
//
	char_entity_map::char_entity_map()
//
// DESCRIPTION
//
//	Construct (initialize) a char_entity_map.  Any entity that isn't listed
//	below will convert to a space.  Note that is isn't necessary to convert
//	"&lt;" and "&gt;" since such entities aren't indexed anyway.  However,
//	if is necessary to convert "&amp;" (so it can be part of an acronym
//	like "AT&T") and "&apos;" (so it can be part of a contracted word like
//	"can't").
//
// SEE ALSO
//
//	html.c		entity_to_ascii()
//
//*****************************************************************************
{
	struct char_entity {
		char const*	name;
		char		char_equiv;
	};
	static char_entity const char_entity_table[] = {
		"amp",    '&', "apos",   '\'',		// apos is in XHTML
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
		"Scaron", 'S', "scaron", 's',		// in XHTML
		"szlig",  's',
		"Uacute", 'U', "uacute", 'u',
		"Ucirc",  'U', "ucirc",  'u',
		"Ugrave", 'U', "ugrave", 'u',
		"Uuml",   'U', "uuml",   'u',
		"Yacute", 'Y', "yacute", 'y',
		"Yuml",   'Y', "yuml",   'y',

		0
	};

	for ( register char_entity const *e = char_entity_table; e->name; ++e )
		map_[ e->name ] = e->char_equiv;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ char_entity_map const& char_entity_map::instance()
//
// DESCRIPTION
//
//	Define and initialize (exactly once) a static instance of
//	char_entity_map and return a reference to it.  The reason for this
//	function is to guarantee that there is exactly one instance of it and
//	that it is initialized before its first use across all translation
//	units, something that would not guaranteed if it were defined and
//	initialized at file scope.
//
// RETURN VALUE
//
//	Returns a reference to a static instance of an initialized
//	char_entity_map.
//
// SEE ALSO
//
//	Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++ Reference
//	Manual."  Addison-Wesley, Reading, MA, 1990.  p. 19.
//
//*****************************************************************************
{
	static char_entity_map m;
	return m;
}
