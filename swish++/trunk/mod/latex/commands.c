/*
**	SWISH++
**	mod/latex/commands.c
**
**	Copyright (C) 2002  Paul J. Lucas
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

#ifdef	MOD_latex

// local
#include "commands.h"

//*****************************************************************************
//
// SYNOPSIS
//
	command_map::command_map()
//
// DESCRIPTION
//
//	Construct (initialize) an command_map.
//
//*****************************************************************************
{
	//
	// A command maps to an action which is what to do about it when
	// encountered in a document.  In most cases, the action is simply text
	// to be substituted.  In cases where the action is either '{' or '[',
	// the character is to be "balanced" by looking for the closing '}' or
	// ']' and the words in between are indexed seperately.
	//
	static command const command_table[] = {

	//	\name			action
	//	-----------------------	-------
		"\"",			"",
		"#",			"#",
		"$",			"$",
		"%",			"%",
		"&",			"&",
		"'",			"",
		".",			"",
		"=",			"",
		"@",			"",
		"\\",			"\n",
		"^",			"",
		"_",			"_",
		"`",			"",
		"{",			" ",
		"}",			" ",

		"aa",			"a",
		"AA",			"A",
		"ae",			"ae",
		"AE",			"AE",
		"b",			"",
		"c",			"",
		"d",			"",
		"H",			"",
		"i",			"i",
		"j",			"j",
		"LaTeX",		"LaTeX",
		"l",			"l",
		"L",			"L",
		"OE",			"OE",
		"ss",			"ss",
		"t",			"",
		"TeX",			"TeX",
		"u",			"",
		"v",			"",

		"author",		"{",
		"caption",		"{",
		"chapter",		"{",
		"emph",			"{",
		"fbox",			"{",
		"footnote",		"{",
		"item",			"[",
		"mbox",			"{",
		"paragraph",		"{",
		"part",			"{",
		"section",		"{",
		"sout",			"{",
		"subparagraph",		"{",
		"subsection",		"{",
		"subsubsection",	"{",
		"textbf",		"{",
		"textit",		"{",
		"textmd",		"{",
		"textnormal",		"{",
		"textrm",		"{",
		"textsc",		"{",
		"textsf",		"{",
		"textsl",		"{",
		"texttt",		"{",
		"textup",		"{",
		"title",		"{",
		"uline",		"{",
		"underline",		"{",
		"uwave",		"{",
		"xout",			"{",

		0
	};

	for ( register command const *c = command_table; c->name; ++c )
		operator[]( c->name ).action = c->action;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ command_map const& command_map::instance()
//
// DESCRIPTION
//
//	Define and initialize (exactly once) a static instance of command_map
//	and return a reference to it.  The reason for this function is to
//	guarantee that there is exactly one instance of it and that it is
//	initialized before its first use across all translation units,
//	something that would not guaranteed if it were defined and initialized
//	at file scope.
//
// RETURN VALUE
//
//	Returns a reference to a static instance of an initialized command_map.
//
// SEE ALSO
//
//	Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++ Reference
//	Manual."  Addison-Wesley, Reading, MA, 1990.  p. 19.
//
//*****************************************************************************
{
	static command_map const m;
	return m;
}

#endif	/* MOD_latex */
