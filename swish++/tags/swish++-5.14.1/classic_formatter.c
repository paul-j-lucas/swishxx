/*
**	SWISH++
**	classic_formatter.c
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

// local
#include "classic_formatter.h"
#include "file_info.h"
#include "index_segment.h"
#include "platform.h"
#include "ResultSeparator.h"
#include "util.h"			/* for FOR_EACH */

extern index_segment directories;

//*****************************************************************************
//
// SYNOPSIS
//
	void classic_formatter::pre( stop_word_set const &stop_words ) const
//
// DESCRIPTION
//
//	Output search-result "meta" information: the set of stop words found in
//	the query (if any) and the number of results.
//
// PARAMETERS
//
//	stop_words	The set of stop words.
//
//*****************************************************************************
{
	// Print stop-words, if any.
	if ( !stop_words.empty() ) {
		out_ << "# ignored:";
		FOR_EACH( stop_word_set, stop_words, word )
#ifdef	PJL_GCC_295 /* see the comment in platform.h */
			out_ << ' ' << word->c_str();
#else
			out_ << ' ' << *word;
#endif
		out_ << '\n';
	}
	out_ << "# results: " << results_ << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	void classic_formatter::result( int rank, file_info const &fi ) const
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
	out_ <<	rank << result_separator
	     <<	directories[ fi.dir_index() ] << '/' << fi.file_name()
	     << result_separator << fi.size()
	     << result_separator << fi.title() << '\n';
}
