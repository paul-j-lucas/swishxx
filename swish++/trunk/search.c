/*
**	SWISH++
**	search.c
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
#include <cstdlib>			/* for exit(2) */
#include <cstring>
#include <functional>			/* for binary_function<> */
#include <iomanip>			/* for omanip<> */
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#ifndef	WIN32
#include <ctime>			/* needed by sys/resource.h */
#include <sys/resource.h>		/* for RLIMIT_* */
#endif
#include <utility>			/* for pair<> */
#include <vector>

// local
#include "bcd.h"
#include "config.h"
#include "exit_codes.h"
#include "file_info.h"
#include "file_list.h"
#include "file_vector.h"
#include "indexer.h"
#include "IndexFile.h"
#include "index_segment.h"
#include "less.h"
#include "my_set.h"
#include "option_stream.h"
#include "platform.h"
#include "ResultsMax.h"
#include "search.h"
#include "stem_word.h"
#include "StemWords.h"
#include "token.h"
#include "util.h"
#include "version.h"
#include "WordFilesMax.h"
#include "WordPercentMax.h"
#include "word_util.h"
#ifdef	SEARCH_DAEMON
#include "auto_vec.h"
#include "PidFile.h"
#include "SearchDaemon.h"
#include "SocketFile.h"
#include "SocketQueueSize.h"
#include "SocketTimeout.h"
#include "ThreadsMax.h"
#include "ThreadsMin.h"
#include "ThreadTimeout.h"
#endif	/* SEARCH_DAEMON */

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

typedef	pair< int, int > search_result_type;
//
//	A search_result_type is an individual search result where the first
//	int is a file index and the second int is that file's rank.

typedef	map< int, int > search_results_type;
//
//	A search_results_type contains a set of search results.

typedef	pair< index_segment::const_iterator, index_segment::const_iterator >
	find_result_type;
//
//	A find_result_type is-a pair of iterators marking the beginning and end
//	of a range over which a given word matches.

//*****************************************************************************
//
// SYNOPSIS
//
	struct sort_by_rank : binary_function<
		search_result_type const&, search_result_type const&, bool
	>
//
// DESCRIPTION
//
//	A sort_by_rank is-a binary_function used to sort search results by rank
//	in descending order (highest rank first).
//
//*****************************************************************************
{
	result_type
	operator()( first_argument_type a, second_argument_type b ) {
		return a.second > b.second;
	}
};

//*****************************************************************************
//
//	Global declarations
//
//*****************************************************************************

char const*	me;				// executable name
#ifdef	SEARCH_DAEMON
SearchDaemon	am_daemon;
#endif
index_segment	files, meta_names, stop_words, words;
ResultsMax	max_results;
StemWords	stem_words;
WordFilesMax	word_file_max;
WordPercentMax	word_percent_max;

#ifdef	SEARCH_DAEMON
void		become_daemon(
			char const*, char const*, int, int, int, int, int
		);
#endif
void		dump_single_word( char const*, ostream& = cout );
void		dump_word_window( char const*, int, int, ostream& = cout );
int		get_meta_id( index_segment::const_iterator );

bool		parse_meta(
			token_stream&, search_results_type&, set< string >&,
			bool&, int = No_Meta_ID
		);
bool		parse_primary(
			token_stream&, search_results_type&, set< string >&,
			bool&, int = No_Meta_ID
		);
bool		parse_query(
			token_stream&, search_results_type&, set< string >&,
			bool&, int = No_Meta_ID
		);
bool		parse_optional_relop( token_stream&, token::type& );

inline omanip< char const* > index_file_info( int index ) {
	return omanip< char const* >( file_info::out, files[ index ] );
}

//*****************************************************************************
//
// SYNOPSIS
//
	int main( int argc, char *argv[] )
//
// DESCRIPTION
//
//	Parse the command line, initialize, call other functions ... the usual
//	things that are done in main().
//
// PARAMETERS
//
//	argc	The number of arguments.
//
//	argv	A vector of the arguments; argv[argc] is null.  Aside from the
//		options below, the arguments form the query.
//
// SEE ALSO
//
//	W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
//	Addison-Wesley, Reading, MA, 1993.
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA, 1997.  pp. 116-118.
//
//*****************************************************************************
{
#include "search_options.c"			/* defines opt_spec */

	me = ::strrchr( argv[0], '/' );		// determine base name ...
	me = me ? me + 1 : argv[0];		// ... of executable

#ifdef	RLIMIT_AS				/* SVR4 */
	max_out_limit( RLIMIT_AS );		// max-out total avail. memory
#endif
	/////////// Process command-line options //////////////////////////////

	IndexFile	index_file_name;

#ifdef	SEARCH_DAEMON
	ThreadsMax	max_threads;
	ThreadsMin	min_threads;
	PidFile		pid_file_name;
	SocketFile	socket_file_name;
	SocketQueueSize	socket_queue_size;
	SocketTimeout	socket_timeout;
	ThreadTimeout	thread_timeout;
#endif

	search_options opt( &argc, &argv, opt_spec );
	if ( !opt )
		::exit( Exit_Usage );

	if ( !( argc ||
#ifdef	SEARCH_DAEMON
		opt.daemon_opt ||
#endif
		opt.dump_entire_index_opt ||
		opt.dump_meta_names_opt ||
		opt.dump_stop_words_opt
	) ) {
		cerr << usage;
		::exit( Exit_Usage );
	}

	//
	// First, parse the config. file (if any); then override variables
	// specified on the command line with options.
	//
	conf_var::parse_file( opt.config_file_name_arg );

	if ( opt.index_file_name_arg )
		index_file_name = opt.index_file_name_arg;
	if ( opt.max_results_arg )
		max_results = opt.max_results_arg;
	if ( opt.stem_words_opt )
		stem_words = true;
	if ( opt.word_file_max_arg )
		word_file_max = opt.word_file_max_arg;
	if ( opt.word_percent_max_arg )
		word_percent_max = opt.word_percent_max_arg;

#ifdef	SEARCH_DAEMON
	if ( opt.daemon_opt )
		am_daemon = true;
	if ( opt.max_threads_arg )
		max_threads = opt.max_threads_arg;
	if ( opt.min_threads_arg )
		min_threads = opt.min_threads_arg;
	if ( opt.pid_file_name_arg )
		pid_file_name = opt.pid_file_name_arg;
	if ( opt.socket_file_name_arg )
		socket_file_name = opt.socket_file_name_arg;
	if ( opt.socket_queue_size_arg )
		socket_queue_size = opt.socket_queue_size_arg;
	if ( opt.socket_timeout_arg )
		socket_timeout = opt.socket_timeout_arg;
	if ( opt.thread_timeout_arg )
		thread_timeout = opt.thread_timeout_arg;
#endif	/* SEARCH_DAEMON */

	/////////// Load index file ///////////////////////////////////////////

	file_vector the_index( index_file_name );
	if ( !the_index ) {
		cerr	<< error << "could not read index from \""
			<< index_file_name << '"' << endl;
		::exit( Exit_No_Read_Index );
	}
	words     .set_index_file( the_index, index_segment::word_index );
	stop_words.set_index_file( the_index, index_segment::stop_word_index );
	files     .set_index_file( the_index, index_segment::file_index );
	meta_names.set_index_file( the_index, index_segment::meta_name_index );

#ifdef	SEARCH_DAEMON
	////////// Become a daemon ////////////////////////////////////////////

	if ( am_daemon )			// function does not return
		become_daemon(
			pid_file_name,
			socket_file_name, socket_queue_size, socket_timeout,
			min_threads, max_threads, thread_timeout
		);
#endif	/* SEARCH_DAEMON */

	////////// Perform the query //////////////////////////////////////////

	service_request( argv, opt );
	::exit( Exit_Success );
}

//*****************************************************************************
//
// SYNOPSIS
//
	void dump_single_word( char const *word, ostream &out )
//
// DESCRIPTION
//
//	Dump the list of files a word is in and ranks therefore to standard
//	output.
//
// PARAMETERS
//
//	word	The word to have its index dumped.
//
//	out	The ostream to dump to.
//
//*****************************************************************************
{
#ifdef	SEARCH_DAEMON
	auto_vec< char > const lower_word( to_lower_r( word ) );
#else
	char const *const lower_word = to_lower( word );
#endif
	less< char const* > const comparator;

	if ( !is_ok_word( word ) || ::binary_search(
		stop_words.begin(), stop_words.end(), lower_word, comparator
	) ) {
		out << "# ignored: " << word << endl;
		return;
	}
	//
	// Look up the word.
	//
	find_result_type const found = ::equal_range(
		words.begin(), words.end(), lower_word, comparator
	);
	if ( found.first == words.end() ||
		comparator( lower_word, *found.first )
	) {
		out << "# not found: " << word << endl;
		return;
	}
	file_list const list( found.first );
	FOR_EACH( file_list, list, file )
		out	<< file->occurrences_ << ' ' << file->rank_ << ' '
			<< index_file_info( file->index_ ) << '\n';
	out << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	void dump_word_window(
		char const *word, int window_size, int match, ostream &out
	)
//
// DESCRIPTION
//
//	Dump a "window" of words from the index around the given word to
//	standard output.
//
// PARAMETERS
//
//	word		The word.
//
//	window_size	The number of lines the window is to contain at most.
//
//	match		The number of characters to compare.
//
//	out		The ostream to dump to.
//
//*****************************************************************************
{
#ifdef	SEARCH_DAEMON
	auto_vec< char > const lower_word( to_lower_r( word ) );
#else
	char const *const lower_word = to_lower( word );
#endif
	less< char const* > const comparator;

	if ( !is_ok_word( word ) || ::binary_search(
		stop_words.begin(), stop_words.end(), lower_word, comparator
	) ) {
		out << "# ignored: " << word << endl;
		return;
	}
	//
	// Look up the word.
	//
	find_result_type found = ::equal_range(
		words.begin(), words.end(), lower_word, comparator
	);
	if ( found.first == words.end() ||
		comparator( lower_word, *found.first )
	) {
		out << "# not found: " << word << endl;
		return;
	}

	//
	// Dump the window by first "backing up" half the window size, then
	// going forward.
	//
	int i = window_size / 2;
	while ( found.first != words.begin() && i-- > 0 )
		--found.first;
	for (	i = 0;
		found.first != words.end() && i < window_size;
		++found.first
	) {
		int const cmp = ::strncmp( *found.first, lower_word, match );
		if ( cmp < 0 )
			continue;
		if ( cmp > 0 )
			break;
		out << *found.first << '\n';
		++i;
	}
	out << '\n';
}

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
	unsigned char const *p = REINTERPRET_CAST(unsigned char const*)( *i );
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
				cerr << "---> performing and" << endl;
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
				cerr << "---> performing or" << endl;
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
				cerr	<< "parse_query(): got non and/or token"
					<< endl;
				::abort();
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
				<< '"' << endl;
#			endif
			relop = t;
			return true;

		default:
			query.put_back( t );
			if ( t == token::rparen_token )
				return false;
#			ifdef DEBUG_parse_query
			cerr << "---> relop \"and\" (implicit)" << endl;
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
					<< "\" (ignored: not OK)" << endl;
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
			cerr << "---> '('" << endl;
#			endif
			if ( !parse_query(
				query, result, stop_words_found, ignore, meta_id
			) )
				return false;
			query >> t;
#			ifdef DEBUG_parse_query
			if ( t == token::rparen_token )
				cerr << "---> ')'" << endl;
#			endif
			return t == token::rparen_token;

		case token::not_token: {
#			ifdef DEBUG_parse_query
			cerr << "---> begin not" << endl;
#			endif
			search_results_type temp;
			if ( !parse_primary(
				query, temp, stop_words_found, ignore, meta_id
			) )
				return false;
#			ifdef DEBUG_parse_query
			cerr << "---> end not" << endl;
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
	cerr << "---> word \"" << t.str() << "\", meta-ID=" << meta_id << endl;
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
				<< "\" (ignored: too frequent)" << endl;
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

//*****************************************************************************
//
// SYNOPSIS
//
	void search( char const *query,
		int skip_results, int max_results,
		ostream &out, ostream &err
	)
//
// DESCRIPTION
//
//	Parse a query, perform a search, and output the results.
//
// PARAMETERS
//
//	query		The text of the query.
//
//	skip_results	The number of initial results to skip.
//
//	max_results	The maximum number of results to output.
//
//	out		The ostream to print the results to.
//
//	err		The ostream to print errors to.
//
//*****************************************************************************
{
	token_stream		query_stream( query );
	search_results_type	results;
	set< string >		stop_words_found;
	bool			ignore;

	if ( !( parse_query( query_stream, results, stop_words_found, ignore )
		&& query_stream.eof()
	) ) {
		err << error << "malformed query" << endl;
#ifdef	SEARCH_DAEMON
		if ( am_daemon )
			return;
#endif
		::exit( Exit_Malformed_Query );
	}

	////////// Print the results //////////////////////////////////////////

	// Print stop-words, if any.
	if ( !stop_words_found.empty() ) {
		out << "# ignored:";
		FOR_EACH( set< string >, stop_words_found, word )
			out << ' ' << *word;
		out << '\n';
	}

	out << "# results: " << results.size() << '\n';
	if ( skip_results >= results.size() || !max_results )
		return;

	// Copy the results to a vector to sort them by rank.
	typedef vector< search_result_type > sorted_results_type;
	sorted_results_type sorted;
	sorted.reserve( results.size() );
	::copy( results.begin(), results.end(), ::back_inserter( sorted ) );
	::sort( sorted.begin(), sorted.end(), sort_by_rank() );
	double const normalize = 100.0 / sorted[0].second;	// highest rank

	for ( sorted_results_type::const_iterator
		i  = sorted.begin() + skip_results;
		i != sorted.end() && max_results-- > 0;
		++i
	)
		out	<< int( i->second * normalize ) << ' '
			<< index_file_info( i->first ) << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	search_options::search_options(
		int *argc, char ***argv,
		option_stream::spec const opt_spec[],
		ostream &err
	)
//
// DESCRIPTION
//
//	Construct (initialze) a search_options by parsing options from the argv
//	vector according to the given option specification.
//
// PARAMETERS
//
//	argc		A pointer to the number of arguments.  The value is
//			decremented by the number of options and their
//			arguments.
//
//	argv		A pointer to the argv vector.  This pointer is
//			incremented by the number of options and their
//			arguments.
//
//	opt_spec	The set of options to allow and their parameters.
//
//	err		The ostream to print errors to.
//
//*****************************************************************************
	: bad_( false )
{
	config_file_name_arg		= ConfigFile_Default;
	dump_entire_index_opt		= false;
	dump_match_arg			= 0;
	dump_meta_names_opt		= false;
	dump_stop_words_opt		= false;
	dump_window_size_arg		= 0;
	dump_word_index_opt		= false;
	index_file_name_arg		= 0;
	max_results_arg			= 0;
	skip_results_arg		= 0;
	stem_words_opt			= false;
	word_file_max_arg		= 0;
	word_percent_max_arg		= 0;
#ifdef	SEARCH_DAEMON
	daemon_opt			= false;
	max_threads_arg			= 0;
	min_threads_arg			= 0;
	pid_file_name_arg		= 0;
	socket_file_name_arg		= 0;
	socket_queue_size_arg		= 0;
	socket_timeout_arg		= 0;
	thread_timeout_arg		= 0;
#endif
	option_stream opt_in( *argc, *argv, opt_spec );
	for ( option_stream::option opt; opt_in >> opt; )
		switch ( opt ) {

#ifdef	SEARCH_DAEMON
			case 'b': // Run in background as a daemon.
				daemon_opt = true;
				break;
#endif
			case 'c': // Specify config. file.
				config_file_name_arg = opt.arg();
				break;

			case 'd': // Dump query word indices.
				dump_word_index_opt = true;
				break;

			case 'D': // Dump entire word index.
				dump_entire_index_opt = true;
				break;

			case 'f': // Specify the word/file file maximum.
				word_file_max_arg = opt.arg();
				break;

			case 'i': // Specify index file overriding the default.
				index_file_name_arg = opt.arg();
				break;

			case 'm': // Specify max. number of results.
				max_results_arg = opt.arg();
				break;

			case 'M': // Dump meta-name list.
				dump_meta_names_opt = true;
				break;
#ifdef	SEARCH_DAEMON
			case 'o': // Specify socket timeout.
				socket_timeout_arg = ::atoi( opt.arg() );
				break;

			case 'O': // Specify thread timeout.
				thread_timeout_arg = ::atoi( opt.arg() );
				break;
#endif
			case 'p': // Specify the word/file percentage.
				word_percent_max_arg = opt.arg();
				break;
#ifdef	SEARCH_DAEMON
			case 'P': // Specify PID file.
				pid_file_name_arg = opt.arg();
				break;

			case 'q': // Specify socket queue size.
				socket_queue_size_arg = ::atoi( opt.arg() );
				if ( socket_queue_size_arg < 1 )
					socket_queue_size_arg = 1;
				break;
#endif
			case 'r': // Specify number of initial results to skip.
				skip_results_arg = ::atoi( opt.arg() );
				if ( skip_results_arg < 0 )
					skip_results_arg = 0;
				break;

			case 's': // Stem words.
				stem_words_opt = true;
				break;

			case 'S': // Dump stop-word list.
				dump_stop_words_opt = true;
				break;
#ifdef	SEARCH_DAEMON
			case 't': // Minimum number of concurrent threads.
				min_threads_arg = ::atoi( opt.arg() );
				break;

			case 'T': // Maximum number of concurrent threads.
				max_threads_arg = ::atoi( opt.arg() );
				break;

			case 'u': // Specify Unix domain socket file.
				socket_file_name_arg = opt.arg();
				break;
#endif
			case 'V': // Display version and exit.
				err << "SWISH++ " << version << endl;
#ifdef	SEARCH_DAEMON
				if ( !am_daemon )
#endif
					::exit( Exit_Success );
#ifdef	SEARCH_DAEMON
				bad_ = true;
				return;
#endif
			case 'w': { // Dump words around query words.
				dump_window_size_arg = ::atoi( opt.arg() );
				if ( dump_window_size_arg < 0 )
					dump_window_size_arg = 0;
				char const *comma = ::strchr( opt.arg(), ',' );
				if ( !comma )
					break;
				dump_match_arg = ::atoi( comma + 1 );
				if ( dump_match_arg < 0 )
					dump_match_arg = 0;
				break;
			}

			default: // Bad option.
				err << usage;
				bad_ = true;
				return;
		}

	*argc -= opt_in.shift(), *argv += opt_in.shift();
}

//*****************************************************************************
//
// SYNOPSIS
//
	void service_request(
		char *argv[], search_options const &opt,
		ostream &out, ostream &err
	)
//
// DESCRIPTION
//
//	Service a request either from the command line or from a client via a
//	socket.
//
// PARAMETERS
//
//	argv	The post-option-parsed set of command line arguments, i.e., all
//		the options have been stripped.
//
//	opt	The set of options specified for this request.
//
//	out	The ostream to send results to.
//
//	err	The ostream to send errors to.
//
//*****************************************************************************
{
	/////////// Dump stuff if requested ///////////////////////////////////

	if ( opt.dump_window_size_arg ) {
		while ( *argv )
			dump_word_window( *argv++,
				opt.dump_window_size_arg, opt.dump_match_arg,
				out
			);
		return;
	}
	if ( opt.dump_word_index_opt ) {
		while ( *argv )
			dump_single_word( *argv++, out );
		return;
	}
	if ( opt.dump_entire_index_opt ) {
		FOR_EACH( index_segment, words, word ) {
			out << *word << '\n';
			file_list const list( word );
			FOR_EACH( file_list, list, file )
				out	<< "  " << file->occurrences_ << ' '
					<< file->rank_ << ' '
					<< index_file_info( file->index_ )
					<< '\n';
			out << '\n';
		}
		return;
	}
	if ( opt.dump_stop_words_opt ) {
		::copy( stop_words.begin(), stop_words.end(),
			ostream_iterator< char const* >( out, "\n" )
		);
		return;
	}
	if ( opt.dump_meta_names_opt ) {
		::copy( meta_names.begin(), meta_names.end(),
			ostream_iterator< char const* >( out, "\n" )
		);
		return;
	}

	////////// Perform the query //////////////////////////////////////////

	//
	// Paste the rest of the command line together into a single query
	// string.
	//
	string query = *argv++;
	while ( *argv ) {
		query += ' ';
		query += *argv++;
	}
	search( query.c_str(),
		opt.skip_results_arg,
		opt.max_results_arg ?
			::atoi( opt.max_results_arg ) : max_results,
		out, err
	);
}

//*****************************************************************************
//
//	Miscellaneous function(s)
//
//*****************************************************************************

ostream& usage( ostream &err ) {
	err <<	"usage: " << me << " [options] query\n"
	"options: (unambiguous abbreviations may be used for long options)\n"
	"========\n"
	"-?   | --help             : Print this help message\n"
#ifdef SEARCH_DAEMON
	"-b   | --daemon           : Run in the background as a daemon [default: no]\n"
#endif
	"-c f | --config-file f    : Name of configuration file [default: " << ConfigFile_Default << "]\n"
	"-d   | --dump-words       : Dump query word indices, exit\n"
	"-D   | --dump-index       : Dump entire word index, exit\n"
	"-f n | --word-files n     : Word/file maximum [default: infinity]\n"
	"-i f | --index-file f     : Name of index file [default: " << IndexFile_Default << "]\n"
	"-m n | --max-results n    : Maximum number of results [default: " << ResultsMax_Default << "]\n"
	"-M   | --dump-meta        : Dump meta-name index, exit\n"
#ifdef SEARCH_DAEMON
	"-o s | --socket-timeout s : Search client request timeout [default: " << SocketTimeout_Default << "]\n"
	"-O s | --thread-timeout s : Idle spare thread timeout [default: " << ThreadTimeout_Default << "]\n"
#endif
	"-p n | --word-percent n   : Word/file percentage [default: 100]\n"
#ifdef SEARCH_DAEMON
	"-P f | --pid-file f       : Name of file to record daemon PID in [default: none]\n"
	"-q n | --queue-size n     : Maximum queued socket connections [default: " << SocketQueueSize_Default << "]\n"
#endif
	"-r n | --skip-results n   : Number of initial results to skip [default: 0]\n"
	"-s   | --stem-words       : Stem words prior to search [default: no]\n"
	"-S   | --dump-stop        : Dump stop-word index, exit\n"
#ifdef SEARCH_DAEMON
	"-t n | --min-threads n    : Minimum number of threads [default: " << ThreadsMin_Default << "] \n"
	"-T n | --max-threads n    : Maximum number of threads [default: " << ThreadsMax_Default << "] \n"
	"-u f | --socket-file f    : Name of socket file [default: " << SocketFile_Default << "]\n"
#endif
	"-V   | --version          : Print version number, exit\n"
	"-w n[,m] | --window n[,m] : Dump window of words around query words [default: 0]\n";
	return err;
}
