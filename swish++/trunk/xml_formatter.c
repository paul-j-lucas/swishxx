/*
**	SWISH++
**	xml_formatter.c
**
**	Copyright (C) 2001  Paul J. Lucas
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

// standard
#include <string>

// local
#include "file_info.h"
#include "index_segment.h"
#include "platform.h"
#include "util.h"
#include "xml_formatter.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

extern index_segment directories;

//*****************************************************************************
//
// SYNOPSIS
//
	string escape_lt( char const *s )
//
// DESCRIPTION
//
//	Escape all '<' characters in a given string by replacing them with
//	"&lt;".
//
// PARAMETERS
//
//	s	The string to be escaped.
//
// RETURN VALUE
//
//	Returns a new string having all '<'s replaced with "&lt;".
//
//*****************************************************************************
{
	string result = s;
	for ( register string::size_type
		pos = 0;
		(pos = result.find( '<', pos )) != string::npos;
		pos += 4
	)
		result.replace( pos, 1, "&lt;" );
	return result;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void xml_formatter::pre( stop_word_set const &stop_words ) const
//
// DESCRIPTION
//
//	Output search-result "meta" information before the results themselves:
//	the set of stop words found in the query (if any) and the number of
//	results.
//
// PARAMETERS
//
//	stop_words	The set of stop words.
//
//*****************************************************************************
{
	out_ <<	"<!DOCTYPE xxxx yyyy\n"
		" \"http://homepage.mac.com/pauljlucas/software/swish/swish.dtd\">\n"
		"<?xml version=\"1.0\" charset=\"us-ascii\"?>\n"
		"<SearchResults>\n";
	// Print stop-words, if any.
	if ( !stop_words.empty() ) {
		out_ << "  <IgnoredList>\n";
		FOR_EACH( stop_word_set, stop_words, word ) {
			out_ <<	"    <Ignored>"
#ifdef	PJL_GCC_295 /* see the comment in platform.h */
			     <<	word->c_str()
#else
			     <<	*word
#endif
			     <<	"</Ignored>\n";
		}
		out_ << "  </IgnoredList>\n";
	}
	out_ << "  <ResultCount>" << results_ << "</ResultCount>\n";
	if ( results_ )
		out_ << "  <ResultList>\n";
}

//*****************************************************************************
//
// SYNOPSIS
//
	void xml_formatter::result( int rank, file_info const &fi ) const
//
// DESCRIPTION
//
//	Output an individual search result's information: it's rank, path,
//	size, and title.
//
// PARAMETERS
//
//	rank	The rank (1-100) of the result.
//
//	fi	The search result's file information.
//
//*****************************************************************************
{
	out_ <<	"    <File>\n"
		"      <Rank>" << rank << "</Rank>\n"
		"      <Path>"
	     <<	directories[ fi.dir_index() ] << '/' << fi.file_name()
	     <<			"</Path>\n"
		"      <Size>" << fi.size() << "</Size>\n"
		"      <Title>";
	if ( ::strchr( fi.title(), '<' ) )
		out_ << escape_lt( fi.title() );
	else
		out_ << fi.title();
	out_ <<			"</Title>\n"
		"    </File>\n";
}

//*****************************************************************************
//
// SYNOPSIS
//
	void xml_formatter::post() const
//
// DESCRIPTION
//
//	Output end tags of XML elements.
//
//*****************************************************************************
{
	if ( results_ )
		out_ << "  </ResultList>\n";
	out_ << "</SearchResults>\n";
}
