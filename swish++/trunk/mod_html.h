/*
**	SWISH++
**	mod_html.h
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

#ifdef	MOD_HTML

#ifndef	mod_html_H
#define	mod_html_H

// standard
#include <utility>			/* for pair<> */
#include <vector>

// local
#include "elements.h"
#include "encoded_char.h"
#include "indexer.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class HTML_indexer : public indexer
//
// DESCRIPTION
//
//	An HTML_indexer is-an indexer for indexing HTML or XHTML files.
//
// SEE ALSO
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "HTML 4.0
//	Specification," World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/
//
//*****************************************************************************
{
public:
	HTML_indexer() : indexer( "HTML" ) { }

	virtual char const*	find_title( file_vector const& ) const;
	virtual void		index_words(
					encoded_char_range::const_iterator&,
					int meta_id = No_Meta_ID
				);
	virtual void		new_file();
private:
	// The element_stack keeps track of all the HTML or XHTML elements we
	// encounter until they are closed.  The first member of the pair is a
	// pointer to the element_map's value_type or the key/value pair of the
	// map where the key is the element name and the value is a pointer to
	// the element.  The second member of the pair is a flag indicating
	// whether the words between the start and end tags of that element are
	// not to be indexed (true = "don't index").
	//
	// Note: I can't use an actual STL stack since I need to be able to
	// clear the entire stack and, unfortunately, clear() isn't supported
	// for stacks...an oversight in STL, IMHO.
	//
	typedef std::vector< std::pair< element_map::value_type const*, bool > >
		stack_type;
	static stack_type element_stack_;

	void	parse_html_tag( encoded_char_range::const_iterator& );
};

#endif	/* mod_html_H */

#endif	/* MOD_HTML */
