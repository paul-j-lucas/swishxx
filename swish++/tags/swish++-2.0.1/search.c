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
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <strstream>
#include <vector>

// local
#include "config.h"
#include "fake_ansi.h"
#include "file_index.h"
#include "file_list.h"
#include "html.h"
#include "less.h"
#include "my_set.h"
#include "stem_word.h"
#include "token.h"
#include "util.h"
#include "version.h"
#include "word_index.h"

extern "C" {
	extern char*	optarg;
	extern int	optind, opterr;
}

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

typedef map< int, int > result_type;
typedef pair< word_index::const_iterator, word_index::const_iterator >
	find_result_type;

char const*	me;				// executable name
file_index	files;
word_index	words, stop_words, meta_names;
bool		stem_words;
string_set	stop_words_found;

void	dump_single_word( char const *word );
void	dump_word_window( char const *word, int window_size, int match );
int	get_meta_id( word_index::const_iterator );
bool	parse_meta( istream&, result_type&, bool &ignore, int = no_meta_id );
bool	parse_primary( istream&, result_type&, bool &ignore, int = no_meta_id );
bool	parse_query( istream&, result_type&, bool &ignore, int = no_meta_id );
bool	parse_optional_relop( istream&, token::type& );
void	usage();

//*****************************************************************************
//
// SYNOPSIS
//
	struct sort_by_rank /* :
		binary_function<
			result_type::value_type const&,
			result_type::value_type const&,
			bool
		> */
//
// DESCRIPTION
//
//	A binary_function used to sort the search results by rank in
//	descending order (highest rank first).
//
// BUGS
//
//	This struct should be derived from binary_function, but g++ 2.8 barfs
//	on it.  It must be a compiler bug.
//
//*****************************************************************************
{
	bool operator()(
		result_type::value_type const &a,
		result_type::value_type const &b
	) {
		return a.second > b.second;
	}
};

//*****************************************************************************
//
// SYNOPSIS
//
	int main( int argc, char *argv[] )
//
// DESCRIPTION
//
//	Parse the command line, initialize, call other functions ... the
//	usual things that are done in main().
//
// PARAMETERS
//
//	argc	The number of arguments.
//
//	argv	A vector of the arguments; argv[argc] is null.  Aside from
//		the options below, the arguments form the query.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA.  pp. 116-118.
//
//*****************************************************************************
{
	me = ::strrchr( argv[0], '/' );		// determine base name ...
	me = me ? me + 1 : argv[0];		// ... of executable

	/////////// Process command-line options //////////////////////////////

	bool		dump_entire_index = false;
	int		dump_match = 0;
	bool		dump_meta_names = false;
	bool		dump_stop_words = false;
	int		dump_window_size = 0;
	bool		dump_word_index = false;
	char const*	index_file_name = Index_Filename_Default;
	int		max_results = Results_Max_Default;
	bool		print_result_count_only = false;
	int		skip_results = 0;

	::opterr = 1;
	char const opts[] = "dDi:m:Mr:RsSVw:";
	for ( int opt; (opt = ::getopt( argc, argv, opts )) != EOF; )
		switch ( opt ) {

			case 'd': // Dump query word indices to standard out.
				dump_word_index = true;
				break;

			case 'D': // Dump entire word index to standard out.
				dump_entire_index = true;
				break;

			case 'i': // Specify index file overriding the default.
				index_file_name = ::optarg;
				break;

			case 'm': // Specify max. number of results.
				max_results = ::atoi( ::optarg );
				break;

			case 'M': // Dump meta-name list.
				dump_meta_names = true;
				break;

			case 'r': // Specify number of initial results to skip.
				skip_results = ::atoi( ::optarg );
				if ( skip_results < 0 )
					skip_results = 0;
				break;

			case 'R': // Print result-count only.
				print_result_count_only = true;
				break;

			case 's': // Stem words.
				stem_words = true;
				break;

			case 'S': // Dump stop-word list.
				dump_stop_words = true;
				break;

			case 'V': // Display version and exit.
				cout << "SWISH++ " << version << endl;
				::exit( 0 );

			case 'w': { // Dump words around query words.
				dump_window_size = ::atoi( ::optarg );
				if ( dump_window_size < 0 )
					dump_window_size  = 0;
				char const *comma = ::strchr( ::optarg, ',' );
				if ( comma &&
					(dump_match = ::atoi( comma + 1 )) < 0
				)
					dump_match = 0;
				break;
			}

			case '?': // Bad option.
				usage();
		}

	argc -= ::optind, argv += ::optind;
	if ( !(argc || dump_entire_index || dump_meta_names || dump_stop_words) )
		usage();

	/////////// Load index file ///////////////////////////////////////////

	file_vector<char> the_index( index_file_name );
	if ( !the_index ) {
		cerr	<< me << ": could not read index from "
			<< index_file_name << endl;
		::exit( 2 );
	}
	words.set_index_file( the_index );
	stop_words.set_index_file( the_index, word_index::stop_word_index );
	files.set_index_file( the_index );
	meta_names.set_index_file( the_index, word_index::meta_name_index );

	/////////// Dump stuff if requested ///////////////////////////////////

	if ( dump_window_size ) {
		while ( *argv )
			dump_word_window( *argv++, dump_window_size, dump_match );
		return 0;
	}
	if ( dump_word_index ) {
		while ( *argv )
			dump_single_word( *argv++ );
		return 0;
	}
	if ( dump_entire_index )
		FOR_EACH( word_index, words, w ) {
			cout << *w << '\n';
			file_list list( w );
			FOR_EACH( file_list, list, file )
				cout	<< "  " << file->rank_ << ' '
					<< files[ file->index_ ] << '\n';
			cout << '\n';
		}

	if ( dump_stop_words )
		copy( stop_words.begin(), stop_words.end(),
			ostream_iterator< char const* >( cout, "\n" )
		);

	if ( dump_meta_names )
		copy( meta_names.begin(), meta_names.end(),
			ostream_iterator< char const* >( cout, "\n" )
		);

	if ( dump_entire_index || dump_meta_names || dump_stop_words )
		return 0;

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
	istrstream query_stream( query.c_str() );

	result_type results;
	bool ignore;
	if ( !( parse_query( query_stream, results, ignore ) &&
		query_stream.eof()
	) ) {
		cerr << me << ": malformed query" << endl;
		::exit( 3 );
	}

	////////// Print the results //////////////////////////////////////////

	// Print stop-words, if any.
	if ( stop_words_found.size() ) {
		cout << "# ignored:";
		FOR_EACH( string_set, stop_words_found, word )
			cout << ' ' << *word;
		cout << '\n';
	}

	cout << "# results: " << results.size() << '\n';
	if ( print_result_count_only )
		return 0;

	if ( skip_results >= results.size() )
		return 0;

	// Copy the results to a vector to sort them by rank.
	typedef vector< result_type::value_type > sorted_result_type;
	sorted_result_type sorted;
	sorted.reserve( results.size() );
	::copy( results.begin(), results.end(), ::back_inserter( sorted ) );
	::sort( sorted.begin(), sorted.end(), sort_by_rank() );
	double const normalize = 100.0 / sorted[0].second;	// highest rank

	for ( sorted_result_type::const_iterator
		i  = sorted.begin() + skip_results;
		i != sorted.end() && max_results-- > 0;
		++i
	)
		cout	<< int( i->second * normalize )
			<< ' ' << files[ i->first ] << '\n';

	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void dump_single_word( char const *word )
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
//*****************************************************************************
{
	char const *const lower_word = to_lower( word );
	less< char const* > const comparator;

	if ( !is_ok_word( word ) || ::binary_search(
		stop_words.begin(), stop_words.end(), lower_word, comparator
	) ) {
		cout << "# ignored: " << word << endl;
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
		cout << "# not found: " << word << endl;
		return;
	}
	file_list list( found.first );
	FOR_EACH( file_list, list, file )
		cout << file->rank_ << ' ' << files[ file->index_ ] << '\n';
	cout << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	void dump_word_window( char const *word, int window_size, int match )
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
//*****************************************************************************
{
	char const *const lower_word = to_lower( word );
	less< char const* > const comparator;

	if ( !is_ok_word( word ) || ::binary_search(
		stop_words.begin(), stop_words.end(), lower_word, comparator
	) ) {
		cout << "# ignored: " << word << endl;
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
		cout << "# not found: " << word << endl;
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
		cout << *found.first << '\n';
		++i;
	}
	cout << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	int get_meta_id( word_index::const_iterator i )
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
	register char const *c = *i;
	while ( *c++ ) ;			// skip past word
	register int id = 0;
	while ( *c ) id = id * 10 + *c++ - '0';
	return id;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_query(
		istream &query, result_type &result, bool &ignore, int meta_id
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
//	query	The istream from which the query string is extracted.
//
//	result	Where the result of performing the (sub)query is deposited.
//
//	ignore	Set to true only if this (sub)query should be ignored.
//
//	meta_id	The meta ID to constrain the matches against, if any.
//
// RETURN VALUE
//
//	Returns true only if a query was successfully parsed.
//
// SEE ALSO
//
//	Alfred V. Aho, Ravi Sethi, Jeffrey D. Ullman.  "Compilers:
//	Principles, Techniques, and Tools," Addison-Wesley, Reading, MA,
//	1986, pp. 44-48.
//
//*****************************************************************************
{
	if ( !parse_meta( query, result, ignore, meta_id ) )
		return false;

	//
	// This is parse_rest placed inside parse_query since every "primary"
	// is followed by a "rest" in the grammar.
	//
	token::type relop;
	while ( parse_optional_relop( query, relop ) ) {
		result_type result1;
		bool ignore1;
		if ( !parse_meta( query, result1, ignore1, meta_id ) )
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
				result_type result2;
				FOR_EACH( result_type, result1, i ) {
					result_type::const_iterator found =
						result.find( i->first );
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
				FOR_EACH( result_type, result1, i )
					result[ i->first ] += i->second;
				break;
			}
		}
	}
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_meta(
		istream &query, result_type &result, bool &ignore, int meta_id
	)
//
// DESCRIPTION
//
//	Parse a meta name from the query using look-ahead.
//
// PARAMETERS
//
//	query	The istream from which the query string is extracted.
//
//	result	Where the result of performing the (sub)query is deposited.
//
//	ignore	Set to true only if this (sub)query should be ignored.
//
//	meta_id	The meta ID to constrain the matches against, if any.
//
// RETURN VALUE
//
//	Returns true only if a query was successfully parsed.
//
//*****************************************************************************
{
	token t( query );
	if ( t == token::word_token ) {			// meta name ...
		token t2( query );
		if ( t2 == token::equal_token ) {	// ... followed by '='
			less< char const* > const comparator;
			find_result_type const found = ::equal_range(
				meta_names.begin(), meta_names.end(),
				t.lower_str(), comparator
			);
			meta_id = found.first != meta_names.end() &&
				!comparator( t.lower_str(), *found.first ) ?
				get_meta_id( found.first )
			:
				meta_id_not_found;
			goto no_put_back;
		}
		t2.put_back();				// put back '='
	}
	t.put_back();					// put back name

no_put_back:
	return parse_primary( query, result, ignore, meta_id );
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_optional_relop( istream &query, token::type &relop )
//
// DESCRIPTION
//
//	Parse an optional relational operator of either "and" or "or" from
//	the given istream.  In the absense of a relational operator, "and" is
//	implied.
//
// PARAMETERS
//
//	query	The istream from which the relational operator string is
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
	token t( query );
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
			t.put_back();
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
		istream &query, result_type &result, bool &ignore, int meta_id
	)
//
// DESCRIPTION
//
//	Parse a primary from the given istream.
//
// PARAMETERS
//
//	query	The istream from which the primary is extracted.
//
//	result	Where the result of performing the (sub)query is deposited.
//
//	ignore	Set to true only if the word should be ignored.
//
//	meta_id	The meta ID to constrain the matches against, if any.
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
			less< char const* > const comparator;
			less_stem const stem_comparator;
			//
			// First check to see if the word wasn't indexed either
			// because it's not an "OK" word according to the
			// heuristics employed or because it's a stop-word.
			//
			if ( !is_ok_word( t.str() ) || ( stem_words ?
				::binary_search(
					stop_words.begin(), stop_words.end(),
					t.lower_str(), stem_comparator
				)
			:
				::binary_search(
					stop_words.begin(), stop_words.end(),
					t.lower_str(), comparator
				)
			) ) {
				stop_words_found.insert( ::strdup( t.str() ) );
#				ifdef DEBUG_parse_query
				cerr << "---> word \"" << t.str() << "\" (ignored)" << endl;
#				endif
				return ignore = true;
			}
			//
			// Look up the word.
			//
			found = stem_words ?
				::equal_range( words.begin(), words.end(),
					t.lower_str(), stem_comparator
				)
			:
				::equal_range( words.begin(), words.end(),
					t.lower_str(), comparator
				);
			if ( found.first == words.end() || ( stem_words ?
				stem_comparator( t.lower_str(), *found.first )
			:
				comparator( t.lower_str(), *found.first )
			) ) {
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
			if ( !parse_query( query, result, ignore, meta_id ) )
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
			result_type temp;
			if ( !parse_primary( query, temp, ignore, meta_id ) )
				return false;
#			ifdef DEBUG_parse_query
			cerr << "---> end not" << endl;
#			endif
			if ( !ignore ) {
				//
				// Iterate over all files and mark the ones the
				// results are NOT in.
				//
				for ( register int i = 0; i < files.size(); ++i)
					if ( temp.find( i ) == temp.end() )
						result[ i ] = 100;
			}
			return true;
		}

		default:
			return false;
	}

	//
	// Found a word or set of words matching a wildcard: iterate over all
	// files the word(s) is/are in and add their ranks together, but only
	// if the meta-names match (if any).
	//
#	ifdef DEBUG_parse_query
	cerr << "---> word \"" << t.str() << "\", meta-ID=" << meta_id << endl;
#	endif
	while ( found.first != found.second ) {
		file_list list( found.first++ );
		FOR_EACH( file_list, list, file )
			if (	meta_id == no_meta_id ||
				file->meta_ids_.find( meta_id )
			)
				result[ file->index_ ] += file->rank_;
	}

	return true;
}

//*****************************************************************************
//
//	Miscellaneous function(s)
//
//*****************************************************************************

void usage() {
	cerr <<	"usage: " << me << " [options] query\n"
	" options:\n"
	" --------\n"
	"  -d              : Dump query word indices to standard out and exit\n"
	"  -D              : Dump entire word index to standard out and exit\n"
	"  -i index_file   : Name of index file to use [default: " << Index_Filename_Default << "]\n"
	"  -m max_results  : Maximum number of results [default: " << Results_Max_Default << "]\n"
	"  -M              : Dump meta-name index to standard out and exit\n"
	"  -R              : Print result-count only [default: no]\n"
	"  -r skip_results : Number of initial results to skip [default: 0]\n"
	"  -s              : Stem words prior to search [default: no]\n"
	"  -S              : Dump stop-word index to standard out and exit\n"
	"  -V              : Print version number and exit\n"
	"  -w size[,match] : Dump size window of words around query words [default: 0]\n";
	::exit( 1 );
}
