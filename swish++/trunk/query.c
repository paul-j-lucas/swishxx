/*
**	SWISH++
**	query.c
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

// standard
#include <algorithm>			/* for binary_search(), etc */
#ifdef	DEBUG_parse_query
#include <iostream>
#endif

// local
#include "bcd.h"
#include "file_list.h"
#include "query.h"
#include "stem_word.h"
#include "StemWords.h"
#include "util.h"
#include "WordFilesMax.h"
#include "WordPercentMax.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

extern index_segment	files, meta_names, stop_words, words;

//*****************************************************************************
//
// SYNOPSIS
//
	int get_meta_id( index_segment::const_iterator i )
//
// DESCRIPTION
//
//	For a given meta name, return its numeric ID that words in the index
//	refer to.
//
// PARAMETERS
//
//	i	An iterator positioned at a meta name.
//
// RETURN VALUE
//
//	The numeric ID of the meta name in the range [0,N).
//
//*****************************************************************************
{
	unsigned char const *p = reinterpret_cast<unsigned char const*>( *i );
	while ( *p++ ) ;			// skip past word
	return parse_bcd( p );
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_too_frequent( int file_count )
//
// DESCRIPTION
//
//	Checks to see if a word is too frequent by either exceeding the maximum
//	number or percentage of files it can be in.
//
// PARAMETERS
//
//	file_count	The number of files a word occurs in.
//
// RETURN VALUE
//
//	Returns true only if a word is too frequent.
//
//*****************************************************************************
{
	return	file_count > word_file_max ||
		file_count * 100 / files.size() >= word_percent_max;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_query(
		token_stream &query, search_results_type &result,
		set< string > &stop_words_found, bool &ignore, int meta_id
	)
//
// DESCRIPTION
//
//	Parse a query via predictive top-down recursive decent.  The grammar
//	for a query is:
//
//		query:		query optional_relop meta
//			|	meta
//
//		meta:		meta_name = primary
//			|	primary
//
//		meta_name:	word
//
//		primary:	'(' query ')'
//			|	'not' primary
//			|	word
//			|	word*
//
//		optional_relop:	'and'
//			|	'or'
//			|	(empty)
//
//	However, the 'query' production is left-recursive which doesn't work
//	for top-down parsers.  It must therefore be rewritten to be right-
//	recursive as:
//
//		query:		meta rest
//
//		rest:		optional_relop meta rest
//			|	(empty)
//
// PARAMETERS
//
//	query			The token_stream whence the query string is
//				extracted.
//
//	result			The query results go here.
//
//	stop_words_found	The set of stop-words in the query.
//
//	ignore			Set to true only if this (sub)query should be
//				ignored.
//
//	meta_id			The meta ID to constrain the matches against,
//				if any.
//
// RETURN VALUE
//
//	Returns true only if a query was successfully parsed.
//
// SEE ALSO
//
//	Alfred V. Aho, Ravi Sethi, Jeffrey D. Ullman.  "Compilers: Principles,
//	Techniques, and Tools," Addison-Wesley, Reading, MA, 1986, pp. 44-48.
//
//*****************************************************************************
{
	if ( !parse_meta( query, result, stop_words_found, ignore, meta_id ) )
		return false;

	//
	// This is parse_rest placed inside parse_query since every "primary"
	// is followed by a "rest" in the grammar.
	//
	token::type relop;
	while ( parse_optional_relop( query, relop ) ) {
		search_results_type result1;
		bool ignore1;
		if ( !parse_meta(
			query, result1, stop_words_found, ignore1, meta_id
		) )
			return false;
		if ( ignore ) {
			if ( !ignore1 ) {	// result is simply the RHS
				result = result1;
				ignore = false;
			}
			continue;
		}
		if ( ignore1 )			// result is simply the LHS
			continue;

		switch ( relop ) {

			case token::and_token: {
#				ifdef DEBUG_parse_query
				cerr << "---> performing and\n";
#				endif
				search_results_type result2;
				FOR_EACH( search_results_type, result1, i ) {
					search_results_type::const_iterator
						found = result.find( i->first );
					if ( found != result.end() )
						result2[ found->first ] =
						(found->second + i->second) / 2;
				}
				result.swap( result2 );
				break;
			}

			case token::or_token: {
#				ifdef DEBUG_parse_query
				cerr << "---> performing or\n";
#				endif
				FOR_EACH( search_results_type, result1, i )
					result[ i->first ] += i->second;
				break;
			}

			default:
				//
				// We should never get anything other than an
				// and_token or an or_token.  If we get there,
				// the programmer goofed.
				//
				internal_error
					<< "parse_query(): "
					   "got non-and/or token\n"
					<< report_error;
		}
	}
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_meta(
		token_stream &query, search_results_type &result,
		set< string > &stop_words_found, bool &ignore, int meta_id
	)
//
// DESCRIPTION
//
//	Parse a meta name from the query using look-ahead.
//
// PARAMETERS
//
//	query			The token_stream whence the query string is
//				extracted.
//
//	result			The query results go here.
//
//	stop_words_found	The set of stop-words in the query.
//
//	ignore			Set to true only if this (sub)query should be
//				ignored.
//
//	meta_id			The meta ID to constrain the matches against,
//				if any.
//
// RETURN VALUE
//
//	Returns true only if a query was successfully parsed.
//
//*****************************************************************************
{
	token const t( query );
	if ( t == token::word_token ) {			// meta name ...
		token const t2( query );
		if ( t2 == token::equal_token ) {	// ... followed by '='
			less< char const* > const comparator;
			find_result_type const found = ::equal_range(
				meta_names.begin(), meta_names.end(),
				t.lower_str(), comparator
			);
			meta_id = found.first != meta_names.end() &&
				!comparator( t.lower_str(), *found.first )
			?
				get_meta_id( found.first )
			:
				Meta_ID_Not_Found;
			goto no_put_back;
		}
		query.put_back( t2 );
	}
	query.put_back( t );

no_put_back:
	return parse_primary( query, result, stop_words_found, ignore, meta_id);
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_optional_relop( token_stream &query, token::type &relop )
//
// DESCRIPTION
//
//	Parse an optional relational operator of either "and" or "or" from the
//	given token_stream.  In the absense of a relational operator, "and" is
//	implied.
//
// PARAMETERS
//
//	query	The token_stream whence the relational operator string is
//		extracted (if present).
//
//	relop	Where the type of the relational operator is deposited.
//
// RETURN VALUE
//
//	Returns true unless no token at all could be extracted.
//
//*****************************************************************************
{
	token const t( query );
	switch ( t ) {

		case token::no_token:
			return false;

		case token::and_token:
		case token::or_token:
#			ifdef DEBUG_parse_query
			cerr	<< "---> relop \""
				<< ( t == token::and_token ? "and" : "or" )
				<< "\"\n";
#			endif
			relop = t;
			return true;

		default:
			query.put_back( t );
			if ( t == token::rparen_token )
				return false;
#			ifdef DEBUG_parse_query
			cerr << "---> relop \"and\" (implicit)\n";
#			endif
			relop = token::and_token;
			return true;
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_primary(
		token_stream &query, search_results_type &result,
		set< string > &stop_words_found, bool &ignore, int meta_id
	)
//
// DESCRIPTION
//
//	Parse a "primary" from the given token_stream.
//
// PARAMETERS
//
//	query			The token_stream whence the query string is
//				extracted.
//
//	result			The query results go here.
//
//	stop_words_found	The set of stop-words in the query.
//
//	ignore			Set to true only if this (sub)query should be
//				ignored.
//
//	meta_id			The meta ID to constrain the matches against,
//				if any.
//
// RETURN VALUE
//
//	Returns true only if a primary was successfully parsed.
//
//*****************************************************************************
{
	ignore = false;
	find_result_type found;
	token t( query );

	switch ( t ) {

		case token::word_token: {
			less_stem const comparator( stem_words );
			//
			// First check to see if the word wasn't indexed either
			// because it's not an "OK" word according to the
			// heuristics employed or because it's a stop-word.
			//
			if ( !is_ok_word( t.str() ) || ::binary_search(
				stop_words.begin(), stop_words.end(),
				t.lower_str(), comparator
			) ) {
				stop_words_found.insert( t.str() );
#				ifdef DEBUG_parse_query
				cerr	<< "---> word \"" << t.str()
					<< "\" (ignored: not OK)\n";
#				endif
				return ignore = true;
			}
			//
			// Look up the word.
			//
			found = ::equal_range( words.begin(), words.end(),
				t.lower_str(), comparator
			);
			if ( found.first == words.end() ||
				comparator( t.lower_str(), *found.first ) ) {
				//
				// The following "return true" indicates that a
				// word was parsed successfully, not that we
				// found the word.
				//
				return true;
			}
			break;
		}

		case token::word_star_token: {
			less_n< char const* > const comparator( t.length() );
			//
			// Look up all matching words.
			//
			found = ::equal_range( words.begin(), words.end(),
				t.lower_str(), comparator
			);
			if ( found.first == words.end() ||
				comparator( t.lower_str(), *found.first ) ) {
				//
				// The following "return true" indicates that a
				// word was parsed successfully, not that we
				// found the word.
				//
				return true;
			}
			break;
		}

		case token::lparen_token:
#			ifdef DEBUG_parse_query
			cerr << "---> '('\n";
#			endif
			if ( !parse_query(
				query, result, stop_words_found, ignore, meta_id
			) )
				return false;
			query >> t;
#			ifdef DEBUG_parse_query
			if ( t == token::rparen_token )
				cerr << "---> ')'\n";
#			endif
			return t == token::rparen_token;

		case token::not_token: {
#			ifdef DEBUG_parse_query
			cerr << "---> begin not\n";
#			endif
			search_results_type temp;
			if ( !parse_primary(
				query, temp, stop_words_found, ignore, meta_id
			) )
				return false;
#			ifdef DEBUG_parse_query
			cerr << "---> end not\n";
#			endif
			if ( !ignore ) {
				//
				// Iterate over all files and mark the ones the
				// results are NOT in.
				//
				for ( int i = 0; i < files.size(); ++i )
					if ( temp.find( i ) == temp.end() )
						result[ i ] = 100;
			}
			return true;
		}

		default:
			return false;
	}

#	ifdef DEBUG_parse_query
	cerr << "---> word \"" << t.str() << "\", meta-ID=" << meta_id << "\n";
#	endif
	//
	// Found a word or set of words matching a wildcard: iterate over all
	// files the word(s) is/are in and add their ranks together, but only
	// if the meta-names match (if any).
	//
	// Also start off assuming that this (sub)query should be ignored until
	// we get at least one word that isn't too frequent.
	//
	ignore = true;
	for ( ; found.first != found.second; ++found.first ) {
		file_list const list( found.first );
		if ( is_too_frequent( list.size() ) ) {
			stop_words_found.insert( t.str() );
#			ifdef DEBUG_parse_query
			cerr	<< "---> word \"" << t.str()
				<< "\" (ignored: too frequent)\n";
#			endif
		} else {
			ignore = false;
			FOR_EACH( file_list, list, file )
				if (	meta_id == No_Meta_ID ||
					file->meta_ids_.contains( meta_id )
				)
					result[ file->index_ ] += file->rank_;
		}
	}

	return true;
}
