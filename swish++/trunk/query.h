/*
**	SWISH++
**	query.h
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

#ifndef	query_H
#define	query_H

// standard
#include <map>
#include <set>
#include <string>
#include <utility>			/* for pair<> */

// local
#include "fake_ansi.h"			/* for std */
#include "index_segment.h"
#include "token.h"

typedef	std::map< int, int > search_results;
//
//	A search_results contains a set of search results where the key int is
//	a file index and the value int is that file's rank.

typedef	std::pair< index_segment::const_iterator, index_segment::const_iterator>
	word_range;
//
//	A word_range is-a pair of iterators marking the beginning and end of a
//	range over which a given word matches.

typedef	std::set< std::string > stop_word_set;

bool	parse_query( token_stream&, search_results&, stop_word_set& );

#endif	/* query_H */
