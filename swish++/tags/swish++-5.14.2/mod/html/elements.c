/*
**	SWISH++
**	mod/html/elements.c
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

#ifdef	MOD_html

// local
#include "elements.h"

//*****************************************************************************
//
// SYNOPSIS
//
	element_map::element_map()
//
// DESCRIPTION
//
//	Construct (initialize) an element_map.
//
// SEE ALSO
//
//	Shane McCarron, et al.  "XHTML 2.0," World Wide Web Consortium,
//	August 2002.
//		http://www.w3.org/TR/xhtml2/
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "Index of Elements,"
//	HTML 4.0 Specification, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/REC-html40/index/elements.html
//
//	Marcin Sawicki, et al.  "Ruby Annotation," World Wide Web Consortium,
//	April 6, 2001.
//		http://www.w3.org/TR/2001/PR-ruby-20010406/
//
//	Netscape Communications Corporation.  "HTML Tag Reference," January
//	1998.
//		http://developer.netscape.com/docs/manuals/htmlguid/index.htm
//
//*****************************************************************************
{
	typedef char const *const cpcc;
	//
	// Declare character pointers having values equal to element's
	// end_tag_value enum so that we can have a simple array of strings
	// below where each "row" has varying length.  It's kind of a hack, but
	// it works.
	//
	cpcc F = reinterpret_cast< char const* >( element::forbidden );
	cpcc O = reinterpret_cast< char const* >( element::optional  );
	cpcc R = reinterpret_cast< char const* >( element::required  );

	//
	// Elements that have forbidden end tags (obviously) have no tags
	// listed for them in the rows below.
	//
	// Elements that have optional end tags have a null-terminated list of
	// tags that close them, either explicitly (their own end tag) or
	// implicitly (a tag from some other element).
	//
	// Elements that have required end tags simply list their single end
	// tag.  Yes, it's easy, given a start tag, to know what it's end tag
	// is (the same tag, but with a leading '/'); however, the code in
	// parse_html_tag() in mod_html.c is made simpler by explicitly giving
	// the end tag here rather than having to construct a temporary string
	// prepending a '/' to the tag.
	//
	static char const *const end_tag_table[] = {

	//	element		end tag	tags that close it	status
	//	---------------	-------	-----------------------	---------------
		"a",		R,	"/a",
		"abbr",		R,	"/abbr",
		"acronym",	R,	"/acronym",
		"address",	R,	"/address",
		"applet",	R,	"/applet",		// deprecated
		"area",		F,
		"b",		R,	"/b",
		"base",		F,
		"basefont",	F,				// deprecated
		"bdo",		R,	"/bdo",
		"big",		R,	"/big",
		"blink",	R,	"/blink",		// nonstandard
		"blockquote",	R,	"/blockquote",
		"body",		O,	"/body", "/html", 0,
		"br",		F,
		"button",	R,	"/button",
		"caption",	R,	"/caption",
		"center",	R,	"/center",		// deprecated
		"cite",		R,	"/cite",
		"code",		R,	"/code",
		"col",		F,

		"colgroup",	O,	"colgroup", "/colgroup",
					"tbody", "tfoot", "thead",
					"tr",
					"/table",
					0,

		"dd",		O,	"dd", "/dd", "/dl", "dt", "/dt", 0,
		"del",		R,	"/del",
		"dfn",		R,	"/dfn",
		"dir",		R,	"/dir",			// deprecated
		"div",		R,	"/div",
		"dl",		R,	"/dl",
		"dt",		O,	"dt", "/dt", "/dl", 0,
		"em",		R,	"/em",
		"embed",	R,	"/embed",		// nonstandard
		"fieldset",	R,	"/fieldset",
		"font",		R,	"/font",		// deprecated
		"form",		R,	"/form",
		"frame",	F,
		"frameset",	R,	"/frameset",
		"h",		R,	"/h",			// XHTML 2.0
		"h1",		R,	"/h1",
		"h2",		R,	"/h2",
		"h3",		R,	"/h3",
		"h4",		R,	"/h4",
		"h5",		R,	"/h5",
		"h6",		R,	"/h6",
		"head",		O,	"body", "/body", "/head", "/html", 0,
		"hr",		F,
		"html",		O,	"/html", 0,
		"i",		R,	"/i",
		"iframe",	R,	"/iframe",
		"ilayer",	R,	"/ilayer",		// nonstandard
		"img",		F,
		"input",	F,
		"ins",		R,	"/ins",
		"isindex",	F,				// deprecated
		"kbd",		R,	"/kbd",
		"keygen",	F,				// nonstandard
		"label",	R,	"/label",
		"layer",	R,	"/layer",		// nonstandard
		"legend",	R,	"/legend",
		"li",		O,	"li", "/li", "/ol", "/ul", 0,
		"line",		R,	"/line",		// XHTML 2.0
		"link",		F,
		"map",		R,	"/map",
		"menu",		R,	"/menu",		// deprecated
		"meta",		F,
		"multicol",	R,	"/multicol",		// nonstandard
		"name",		R,	"/name",		// XHTML 2.0
		"nl",		R,	"/nl",			// XHTML 2.0
		"nobr",		F,				// nonstandard
		"noembed",	R,	"/noembed",		// nonstandard
		"noframes",	R,	"/noframes",
		"nolayer",	R,	"/nolayer",		// nonstandard
		"noscript",	R,	"/noscript",
		"object",	R,	"/object",
		"ol",		R,	"/ol",
		"optgroup",	R,	"/optgroup",

		"option",	O,	"/optgroup",
					"option", "/option",
					"/select",
					0,

		"p",		O,	"address", "/address",
					"applet", "/applet",
					"blockquote", "/blockquote",
					"/body",
					"br",
					"caption", "/caption",
					"center", "/center",
					"dd", "/dd",
					"div", "/div",
					"dl", "/dl",
					"dt", "/dt",
					"embed", "/embed",
					"form", "/form",
					"frame", "/frame",
					"frameset", "/frameset",
					"h1", "/h1",
					"h2", "/h2",
					"h3", "/h3",
					"h4", "/h4",
					"h5", "/h5",
					"h6", "/h6",
					"hr",
					"/html",
					"layer", "/layer",
					"li", "/li",
					"map", "/map",
					"multicol", "/multicol",
					"noembed", "/noembed",
					"noframes", "/noframes",
					"nolayer", "/nolayer",
					"noscript", "/noscript",
					"object", "/object",
					"ol", "/ol",
					"p", "/p",
					"plaintext",
					"pre", "/pre",
					"script", "/script",
					"select", "/select",
					"style", "/style",
					"table", "/table",
					"tbody", "/tbody"
					"td", "/td",
					"tfoot", "/tfoot"
					"th", "/th",
					"thead", "/thead"
					"tr", "/tr",
					"ul", "/ul",
					"xmp", "/xmp",
					0,

		"param",	F,
		"plaintext",	F,				// deprecated
		"pre",		R,	"/pre",
		"q",		R,	"/q",			// deprecated
		"quote",	R,	"/quote",		// XHTML 2.0

		"ruby",		R,	"/ruby",		// ruby elements
		"rb",		R,	"/rb",
		"rbc",		R,	"/rbc",
		"rp",		R,	"/rp",
		"rt",		R,	"/rt",
		"rtc",		R,	"/rtc",

		"s",		R,	"/s",			// deprecated
		"samp",		R,	"/samp",
		"script",	R,	"/script",
		"section",	R,	"/section",		// XHTML 2.0
		"select",	R,	"/select",
		"server",	R,	"/server",		// nonstandard
		"small",	R,	"/small",
		"spacer",	F,				// nonstandard
		"span",		R,	"/span",
		"strike",	R,	"/strike",		// deprecated
		"strong",	R,	"/strong",
		"style",	R,	"/style",
		"sub",		R,	"/sub",
		"sup",		R,	"/sup",
		"table",	R,	"/table",
		"tbody",	O,	"tbody", "/tbody", 0,

		"td",		O,	"tbody", "/tbody",
					"td", "/td",
					"tfoot", "/tfoot",
					"th",
					"/table",
					"tr", "/tr",
					0,

		"textarea",	R,	"/textarea",
		"tfoot",	O,	"tbody", "/tfoot", "thead", 0,

		"th",		O,	"tbody", "/tbody",
					"td",
					"tfoot", "/tfoot",
					"th", "/th",
					"/table",
					"tr", "/tr",
					0,

		"thead",	O,	"tbody", "tfoot", "/thead", 0,
		"title",	R,	"/title",

		"tr",		O,	"tbody", "/tbody",
					"tfoot", "/tfoot",
					"/thead",
					"tr", "/tr",
					"/table",
					0,

		"tt",		R,	"/tt",
		"u",		R,	"/u",			// deprecated
		"ul",		R,	"/ul",
		"var",		R,	"/var",
		"wbr",		F,				// nonstandard
		"xmp",		R,	"/xmp",			// deprecated

		0
	};

	for ( register char const *const *p = end_tag_table; *p; ++p ) {
		//
		// The ugly double, old-style casts below are necessary because
		// C++ has become overly pedantic about casting to enums.
		//
		element::end_tag_value const
			v = (element::end_tag_value const)(int const)(p[1]);
		element &e = insert(
			value_type( *p++, element( v ) )
		).first->second;

		switch ( v ) {

			case element::forbidden:
				// do nothing
				break;

			case element::optional:
				//
				// Elements that have optional end tags have a
				// null-terminated list of closing tags.
				//
				while ( *++p )
					e.close_tags.insert( *p );
				break;

			case element::required:
				//
				// Elements that have required and tags have a
				// single closing tag.
				//
				e.close_tags.insert( *++p );
				break;
		}
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ element_map const& element_map::instance()
//
// DESCRIPTION
//
//	Define and initialize (exactly once) a static instance of element_map
//	and return a reference to it.  The reason for this function is to
//	guarantee that there is exactly one instance of it and that it is
//	initialized before its first use across all translation units,
//	something that would not guaranteed if it were defined and initialized
//	at file scope.
//
// RETURN VALUE
//
//	Returns a reference to a static instance of an initialized element_map.
//
// SEE ALSO
//
//	Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++ Reference
//	Manual."  Addison-Wesley, Reading, MA, 1990.  p. 19.
//
//*****************************************************************************
{
	static element_map const m;
	return m;
}

#endif	/* MOD_html */
