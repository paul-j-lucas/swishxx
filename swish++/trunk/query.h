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

typedef	std::map< int, int > search_results_type;
//
//	A search_results_type contains a set of search results.

typedef	std::pair< index_segment::const_iterator, index_segment::const_iterator>
	find_result_type;
//
//	A find_result_type is-a pair of iterators marking the beginning and end
//	of a range over which a given word matches.

int	get_meta_id( index_segment::const_iterator );

bool	parse_meta(
		token_stream&, search_results_type&, std::set< std::string >&,
		bool&, int = No_Meta_ID
	);
bool	parse_primary(
		token_stream&, search_results_type&, std::set< std::string >&,
		bool&, int = No_Meta_ID
	);
bool	parse_query(
		token_stream&, search_results_type&, std::set< std::string >&,
		bool&, int = No_Meta_ID
	);
bool	parse_optional_relop( token_stream&, token::type& );

#endif	/* query_H */
