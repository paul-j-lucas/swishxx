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
#include <memory>			/* for auto_ptr<T> */
#include <vector>

// local
#include "enc_int.h"
#include "exit_codes.h"
#include "file_list.h"
#include "IndexFile.h"
#include "query.h"
#include "query_node.h"
#include "stem_word.h"
#include "StemWords.h"
#include "util.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

struct parse_args {
	//
	// This is a simple struct used to pass a bunch of function arguments
	// around as a single unit.
	//
	typedef query_node::pool_type node_pool;

	node_pool&			pool;
	token_stream&			query;
	and_node::child_node_list	and_nodes;
	bool				ignore;
	int				meta_id;
	query_node*			node;
	stop_word_set&			stop_words_found;
#ifdef	FEATURE_word_pos
	bool&				got_near;
#endif

	parse_args( node_pool &p, token_stream &t, stop_word_set &s
#ifdef	FEATURE_word_pos
		, bool &b
#endif
	) : pool( p ), query( t ), stop_words_found( s ), meta_id( No_Meta_ID )
#ifdef	FEATURE_word_pos
		, got_near( b )
#endif
	{
	}

	parse_args( parse_args const &a ) :
		meta_id( a.meta_id ), pool( a.pool ), query( a.query ),
		stop_words_found( a.stop_words_found )
#ifdef	FEATURE_word_pos
		, got_near( a.got_near )
#endif
	{
	}
};

extern index_segment	files, meta_names, stop_words, words;

static bool	parse_meta( parse_args& );
static bool	parse_primary( parse_args& );
static bool	parse_query2( parse_args& );
static bool	parse_optional_relop( token_stream&, token::type& );

//*****************************************************************************
//
// SYNOPSIS
//
	static int get_meta_id( index_segment::const_iterator i )
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
	return dec_int( p );
}

#ifdef	FEATURE_word_pos
//*****************************************************************************
//
// SYNOPSIS
//
	static void assert_index_has_word_pos_data()
//
// DESCRIPTION
//
//	The current query has a "near" in it: check that the current index has
//	word-position data stored in order to evaluate the "near".  If it
//	doesn't, complain.
//
//*****************************************************************************
{
	//
	// A simple way to check that the current index has word-position data
	// stored is to get the file_list for the first word in the index then
	// look to see if pos_delta_ is empty: if it is, no word-position data
	// was stored.
	//
	file_list const list( words.begin() );
	file_list::const_iterator const file( list.begin() );
	if ( file->pos_deltas_.empty() ) {
		extern IndexFile index_file_name;
		error()	<< '"' << index_file_name
			<< "\" does not contain word position data"
			<< endl;
		::exit( Exit_No_Word_Pos_Data );
	}
}
#endif	/* FEATURE_word_pos */

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_query(
		token_stream &query, search_results &results,
		stop_word_set &stop_words_found
	)
//
// DESCRIPTION
//
//	Parse a query.  This is merely a front-end for parse_query2(), but has
//	a less ugly API.
//
// PARAMETERS
//
//	query			The token_stream whence the query string is
//				extracted.
//
//	results			The query results go here.
//
//	stop_words_found	The set of stop-words in the query.
//
// RETURN RESULT
//
//	Returns true only if a query was successfully parsed.
//
//*****************************************************************************
{
	parse_args::node_pool pool;

#ifdef	FEATURE_word_pos
	bool got_near = false;
	parse_args args( pool, query, stop_words_found, got_near );
#else
	parse_args args( pool, query, stop_words_found );
#endif
	if ( !parse_query2( args ) )
		return false;

#ifdef	FEATURE_word_pos
	if ( args.got_near ) {
		assert_index_has_word_pos_data();
		//
		// We got a "near" somewhere in the query: walk the tree and
		// distirbute the terms of all the near nodes.
		//
		near_node::distributor const d( 0 );
		args.node = args.node->visit( d );
#		ifdef DEBUG_eval_query
		args.node->print( cerr );
		cerr << endl;
#		endif
	}
#endif	/* FEATURE_word_pos */
	args.node->eval( results );
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_query2( parse_args &args )
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
//			|	'not' meta
//			|	word
//			|	word*
//
//		optional_relop:	'and'
//			|	'near'
//			|	'not' 'near'
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
//	meta_id			The meta ID to constrain the matches against,
//				if any.
//
//	node			The query node tree gets built here.
//
//	stop_words_found	The set of stop-words in the query.
//
//	and_nodes		The partial trees to be and-ed together.
//
//	ignore			Set to true only if this (sub)query should be
//				ignored.
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
	if ( !parse_meta( args ) )
		return false;

	//
	// This is parse_rest placed inside parse_query since every "primary"
	// is followed by a "rest" in the grammar.
	//
	token::type relop;
	while ( parse_optional_relop( args.query, relop ) ) {
		parse_args args_rhs( args );
		if ( !parse_meta( args_rhs ) )
			return false;
		if ( args.ignore ) {
			if ( !args_rhs.ignore ) {
				// results are simply the RHS
				args.node = args_rhs.node;
				args.ignore = false;
			}
			continue;
		}
		if ( args_rhs.ignore )		// results are simply the LHS
			continue;

		switch ( relop ) {

			case token::and_token:
				//
				// Defer performing the "and" until later so
				// that all the "and"s at the same level can be
				// performed together.
				//
				args.and_nodes.push_back( args_rhs.node );
				break;
#ifdef	FEATURE_word_pos
			case token::near_token:
			case token::not_near_token: {
				//
				// Ensure that neither child node of the
				// near_node we want to create is a not_node
				// since it's nonsensical.
				//
				if ( dynamic_cast<not_node*>( args_rhs.node ) )
					return false;
				query_node *const lhs_node =
					args.and_nodes.empty() ? args.node :
					new and_node(
						args.pool, args.and_nodes
					);
				if ( dynamic_cast<not_node*>( lhs_node ) )
					return false;
				//
				// If both child nodes have meta IDs specified,
				// then they must be equal; if not, the results
				// must be empty.
				//
				if (	args    .meta_id != No_Meta_ID &&
					args_rhs.meta_id != No_Meta_ID &&
					args    .meta_id != args_rhs.meta_id
				) {
					args.node = new empty_node;
					break;
				}
				args.got_near = true;
				args.node = relop == token::not_near_token ?
					new not_near_node( args.pool,
						lhs_node, args_rhs.node
					) :
					new near_node( args.pool,
						lhs_node, args_rhs.node
					);
				break;
			}
#endif	/* FEATURE_word_pos */

			case token::or_token:
				args.node = new or_node(
					args.pool,
					args.and_nodes.empty() ? args.node :
					new and_node(
						args.pool, args.and_nodes
					),
					args_rhs.node
				);
				break;

			default://
				// We should never get anything other than an
				// and_token, near_token, or an or_token.  If
				// we get here, the programmer goofed.
				//
				internal_error
					<< "parse_query2(): unexpected token"
					<< report_error;
		}
	}

	if ( !args.and_nodes.empty() ) {
		args.and_nodes.push_back( args.node );
		args.node = new and_node( args.pool, args.and_nodes );
	}
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_meta( parse_args &args )
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
//	meta_id			The meta ID to constrain the matches against,
//				if any.
//
//	node			The query node tree gets built here.
//
//	stop_words_found	The set of stop-words in the query.
//
//	and_nodes		The partial trees to be and-ed together.
//
//	ignore			Set to true only if this (sub)query should be
//				ignored.
//
// RETURN VALUE
//
//	Returns true only if a query was successfully parsed.
//
//*****************************************************************************
{
	token const t( args.query );
	if ( t == token::word_token ) {			// meta name ...
		token const t2( args.query );
		if ( t2 == token::equal_token ) {	// ... followed by '='
			less< char const* > const comparator;
			word_range const range = ::equal_range(
				meta_names.begin(), meta_names.end(),
				t.lower_str(), comparator
			);
			args.meta_id = range.first != meta_names.end() &&
				!comparator( t.lower_str(), *range.first )
			?
				get_meta_id( range.first )
			:
				Meta_ID_Not_Found;
			goto parsed_meta_id;
		}
		args.query.put_back( t2 );
	}
	args.query.put_back( t );

parsed_meta_id:
	return parse_primary( args );
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_optional_relop( token_stream &query, token::type &relop )
//
// DESCRIPTION
//
//	Parse an optional relational operator of "and", "near", or "or" from
//	the given token_stream.  In the absense of a relational operator, "and"
//	is implied.
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
//	Returns true unless no token at all could be parsed.
//
//*****************************************************************************
{
	token const t( query );
	token::type t_type = t;
	switch ( t_type ) {

		case token::no_token:
			return false;

#ifdef	FEATURE_word_pos
		case token::not_token: {
			token const t2( query );
			if ( t2 != token::near_token ) {
				query.put_back( t2 );
				break;
			}
			t_type = token::not_near_token;
		}
		case token::near_token:
#endif
		case token::and_token:
		case token::or_token:
#			ifdef DEBUG_parse_query
			cerr << "---> relop \"";
			switch ( t_type ) {
				case token::and_token:
					cerr << "and";
					break;
#ifdef	FEATURE_word_pos
				case token::not_near_token:
					cerr << "not ";
					// no break;
				case token::near_token:
					cerr << "near";
					break;
#endif
				case token::or_token:
					cerr << "or";
					break;
			}
			cerr << "\"\n";
#			endif
			relop = t_type;
			return true;
	}
	query.put_back( t );
	if ( t == token::rparen_token )
		return false;
#	ifdef DEBUG_parse_query
	cerr << "---> relop \"and\" (implicit)\n";
#	endif
	relop = token::and_token;
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_primary( parse_args &args )
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
//	meta_id			The meta ID to constrain the matches against,
//				if any.
//
//	node			The query node tree gets built here.
//
//	stop_words_found	The set of stop-words in the query.
//
//	and_nodes		The partial trees to be and-ed together.
//
//	ignore			Set to true only if this (sub)query should be
//				ignored.
//
// RETURN VALUE
//
//	Returns true only if a primary was successfully parsed.
//
//*****************************************************************************
{
	args.ignore = false;
	args.node = new empty_node;
	word_range range;
	token t( args.query );

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
				args.stop_words_found.insert( t.str() );
#				ifdef DEBUG_parse_query
				cerr	<< "---> word \"" << t.str()
					<< "\" (ignored: not OK)\n";
#				endif
				//
				// The following "return true" indicates that a
				// word was parsed successfully, not that we
				// found the word.
				//
				return args.ignore = true;
			}
			//
			// Look up the word.
			//
			range = ::equal_range( words.begin(), words.end(),
				t.lower_str(), comparator
			);
			if ( range.first == words.end() ||
				comparator( t.lower_str(), *range.first ) ) {
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
			range = ::equal_range( words.begin(), words.end(),
				t.lower_str(), comparator
			);
			if ( range.first == words.end() ||
				comparator( t.lower_str(), *range.first ) ) {
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
			if ( !parse_query2( args ) )
				return false;
			args.query >> t;
#			ifdef DEBUG_parse_query
			if ( t == token::rparen_token )
				cerr << "---> ')'\n";
#			endif
			return t == token::rparen_token;

		case token::not_token: {
#			ifdef DEBUG_parse_query
			cerr << "---> begin not\n";
#			endif
			parse_args temp( args );
			if ( !parse_meta( temp ) )
				return false;
#			ifdef DEBUG_parse_query
			cerr << "---> end not\n";
#			endif
			if ( temp.node )
				args.node = new not_node( args.pool, temp.node );
			return true;
		}

		default:
			return false;
	}

#	ifdef DEBUG_parse_query
	cerr	<< "---> word \"" << t.str()
		<< "\", meta-ID=" << args.meta_id << "\n";
#	endif
	//
	// Found a word or set of words matching a wildcard: iterate over all
	// files the word(s) is/are in and add their ranks together, but only
	// if the meta-names match (if any).
	//
	// Also start off assuming that this (sub)query should be ignored until
	// we get at least one word that isn't too frequent.
	//
	args.ignore = true;
	FOR_EACH_IN_PAIR( index_segment, range, i ) {
		file_list const list( i );
		if ( is_too_frequent( list.size() ) ) {
			args.stop_words_found.insert( t.lower_str() );
#			ifdef DEBUG_parse_query
			cerr	<< "---> word \"" << t.str()
				<< "\" (ignored: too frequent)\n";
#			endif
		} else
			args.ignore = false;
	}

	if ( !args.ignore )
		args.node = new word_node(
			args.pool, t.str(), range, args.meta_id
		);
	return true;
}
