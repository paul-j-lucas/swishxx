/*
**	SWISH++
**	index.c
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
#include <cmath>			/* for log(3) */
#include <cstdlib>			/* for exit(2) */
#include <cstring>
#include <ctime>
#include <sys/time.h>			/* needed by FreeBSD systems */
#include <fstream>
#include <iomanip>			/* for setfill(), setw() */
#include <iostream>
#include <iterator>
#include <string>
#include <sys/resource.h>		/* for RLIMIT_* */
#include <sys/types.h>
#include <unistd.h>			/* for unlink(2) */
#include <vector>

// local
#include "bcd.h"
#include "config.h"
#include "directory.h"
#ifdef	MOD_HTML
#include "elements.h"
#include "ExcludeClass.h"
#endif
#include "ExcludeFile.h"
#include "ExcludeMeta.h"
#include "exit_codes.h"
#include "file_info.h"
#include "file_list.h"
#include "FilesGrow.h"
#include "FilesReserve.h"
#include "FilterFile.h"
#ifdef	MOD_HTML
#include "mod_html.h"
#endif
#include "IncludeFile.h"
#include "IncludeMeta.h"
#include "Incremental.h"
#include "IndexFile.h"
#include "indexer.h"
#include "index_segment.h"
#include "itoa.h"
#include "meta_map.h"
#include "mmap_file.h"
#include "option_stream.h"
#include "platform.h"
#include "RecurseSubdirs.h"
#include "StopWordFile.h"
#include "stop_words.h"
#include "TempDirectory.h"
#include "TitleLines.h"
#include "util.h"
#include "Verbosity.h"
#include "version.h"
#include "WordFilesMax.h"
#include "word_info.h"
#include "WordPercentMax.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

ExcludeFile		exclude_patterns;	// do not index these
IncludeFile		include_patterns;	// do index these
#ifdef	MOD_HTML
ExcludeClass		exclude_class_names;	// class names not to index
#endif
ExcludeMeta		exclude_meta_names;	// meta names not to index
IncludeMeta		include_meta_names;	// meta names to index
FilesGrow		files_grow;
FilterFile		filters;
Incremental		incremental;
char const*		me;			// executable name
meta_map		meta_names;
int			num_examined_files;
int			num_temp_files;
TitleLines		num_title_lines;
long			num_total_words;	// over all files indexed
long			num_indexed_words;	// over all files indexed
long			num_unique_words;	// over all files indexed
vector<string>		partial_index_file_names;
RecurseSubdirs		recurse_subdirectories;
string			temp_file_name_prefix;
Verbosity		verbosity;		// how much to print
word_map		words;			// the index being generated
WordFilesMax		word_file_max;
WordPercentMax		word_percent_max;

void			load_old_index( char const *index_file_name );
void			merge_indicies( ostream& );
void			rank_full_index();
extern "C" void		remove_temp_files( void );
ostream&		usage( ostream& = cerr );
void			write_file_index( ostream&, off_t* );
void			write_full_index( ostream& );
void			write_meta_name_index( ostream&, off_t* );
void			write_partial_index();
void			write_stop_word_index( ostream&, off_t* );
void			write_word_index( ostream&, off_t* );

#define	INDEX
#include "do_file.c"

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
//		options below, the arguments are the names of the files and
//		directories to be indexed.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA, 1997.  pp. 116-118.
//
//*****************************************************************************
{
	me = ::strrchr( argv[0], '/' );		// determine base name...
	me = me ? me + 1 : argv[0];		// ...of executable

	////////// Max-out various system resources ///////////////////////////

#ifdef	RLIMIT_AS				/* SVR4 */
	//
	// Max-out out out total memory potential.
	//
	max_out_limit( RLIMIT_AS );
#endif
#ifdef	RLIMIT_CPU				/* SVR4, 4.3+BSD */
	//
	// Max-out the amount of CPU time we can run since indexing can take a
	// while.
	//
	max_out_limit( RLIMIT_CPU );
#endif
#ifdef	RLIMIT_DATA				/* SVR4, 4.3+BSD */
	//
	// Max-out our heap allocation potential.
	//
	max_out_limit( RLIMIT_DATA );
#endif
	/////////// Process command-line options //////////////////////////////

	static option_stream::spec const opt_spec[] = {
		"help",			0, '?',
		"config",		1, 'c',
#ifdef	MOD_HTML
		"no-class",		1, 'C',
#endif
		"pattern",		1, 'e',
		"no-pattern",		1, 'E',
		"file-max",		1, 'f',
		"files-reserve",	1, 'F',
		"files-grow",		1, 'G',
#ifdef	MOD_HTML
		"dump-html",		0, 'H',
#endif
		"index-file",		1, 'i',
		"incremental",		0, 'I',
#ifndef	PJL_NO_SYMBOLIC_LINKS
		"follow-links",		0, 'l',
#endif
		"meta",			1, 'm',
		"no-meta",		1, 'M',
		"percent-max",		1, 'p',
		"no-recurse",		0, 'r',
		"stop-file",		1, 's',
		"dump-stop",		0, 'S',
		"title-lines",		1, 't',
		"temp-dir",		1, 'T',
		"verbosity",		1, 'v',
		"version",		0, 'V',
		0
	};

	char const*	config_file_name_arg = ConfigFile_Default;
#ifdef	MOD_HTML
	bool		dump_html_elements_opt = false;
#endif
	bool		dump_stop_words_opt = false;
	char const*	files_grow_arg = 0;
	char const*	files_reserve_arg = 0;
#ifndef	PJL_NO_SYMBOLIC_LINKS
	bool		follow_symbolic_links_opt = false;
#endif
	bool		incremental_opt = false;
	IndexFile	index_file_name;
	char const*	index_file_name_arg = 0;
	char const*	num_title_lines_arg = 0;
	bool		recurse_subdirectories_opt = false;
	StopWordFile	stop_word_file_name;
	char const*	stop_word_file_name_arg = 0;
	TempDirectory	temp_directory;
	char const*	temp_directory_arg = 0;
	char const*	verbosity_arg = 0;
	char const*	word_file_max_arg = 0;
	char const*	word_percent_max_arg = 0;

	option_stream opt_in( argc, argv, opt_spec );
	for ( option_stream::option opt; opt_in >> opt; )
		switch ( opt ) {

			case '?': // Print help.
				cerr << usage;

			case 'c': // Specify config. file.
				config_file_name_arg = opt.arg();
				break;
#ifdef	MOD_HTML
			case 'C': // Specify CLASS name(s) not to index.
				exclude_class_names.insert(
					to_lower( opt.arg() )
				);
				break;
#endif
			case 'e': { // Filename pattern(s) to index.
				if ( !::strtok( opt.arg(), ":" ) ) {
					error()	<< "no indexer module name"
						<< endl;
					::exit( Exit_Usage );
				}
				indexer *const
					i = indexer::find_indexer( opt.arg() );
				if ( !i ) {
					error()	<< '"' << opt.arg()
						<< "\": no such indexing module"
						<< endl;
					::exit( Exit_Usage );
				}
				for ( char *pat; pat = ::strtok( 0, "," ); )
					include_patterns.insert( pat, i );
				break;
			}

			case 'E': { // Filename pattern(s) not to index.
				char *a = opt.arg();
				for ( char *pat; pat = ::strtok( a, "," ); ) {
					exclude_patterns.insert( pat );
					a = 0;
				}
				break;
			}

			case 'f': // Specify the word/file file maximum.
				word_file_max_arg = opt.arg();
				break;

			case 'F': // Specify files to reserve space for.
				files_reserve_arg = opt.arg();
				break;

			case 'G': // Specify files to reserve space for growth.
				files_grow_arg = opt.arg();
				break;
#ifdef	MOD_HTML
			case 'H': // Dump recognized HTML and XHTML elements.
				dump_html_elements_opt = true;
				break;
#endif
			case 'i': // Specify index file overriding the default.
				index_file_name_arg = opt.arg();
				break;

			case 'I': // specify incremental indexing.
				incremental_opt = true;
				break;
#ifndef	PJL_NO_SYMBOLIC_LINKS
			case 'l': // Follow symbolic links during indexing.
				follow_symbolic_links_opt = true;
				break;
#endif
			case 'm': // Specify meta name(s) to index.
				include_meta_names.parse_value( opt.arg() );
				break;

			case 'M': // Specify meta name(s) not to index.
				exclude_meta_names.insert(
					to_lower( opt.arg() )
				);
				break;

			case 'p': // Specify the word/file percentage.
				word_percent_max_arg = opt.arg();
				break;

			case 'r': // Specify whether to index recursively.
				recurse_subdirectories_opt = true;
				break;

			case 's': // Specify stop-word list.
				stop_word_file_name_arg = opt.arg();
				break;

			case 'S': // Dump stop-word list.
				dump_stop_words_opt = true;
				break;

			case 't': // Specify number of title lines.
				num_title_lines_arg = opt.arg();
				break;

			case 'T': // Specify temp. directory.
				temp_directory_arg = opt.arg();
				break;

			case 'v': // Specify verbosity level.
				verbosity_arg = opt.arg();
				break;

			case 'V': // Display version and exit.
				cout << "SWISH++ " << version << endl;
				::exit( Exit_Success );

			default: // Bad option.
				cerr << usage;
		}
	argc -= opt_in.shift(), argv += opt_in.shift();

	//
	// First, parse the config. file (if any); then override variables
	// with options specified on the command line.
	//
	conf_var::parse_file( config_file_name_arg );

	if ( files_grow_arg )
		files_grow = files_grow_arg;
	if ( files_reserve_arg )
		files_reserve = files_reserve_arg;
#ifndef	PJL_NO_SYMBOLIC_LINKS
	if ( follow_symbolic_links_opt )
		follow_symbolic_links = true;
#endif
	if ( incremental_opt )
		incremental = true;
	if ( index_file_name_arg )
		index_file_name = index_file_name_arg;
	if ( num_title_lines_arg )
		num_title_lines = num_title_lines_arg;
	if ( recurse_subdirectories_opt )
		recurse_subdirectories = false;
	if ( stop_word_file_name_arg )
		stop_word_file_name = stop_word_file_name_arg;
	if ( temp_directory_arg )
		temp_directory = temp_directory_arg;
	if ( verbosity_arg )
		verbosity = verbosity_arg;
	if ( word_file_max_arg )
		word_file_max = word_file_max_arg;
	if ( word_percent_max_arg )
		word_percent_max = word_percent_max_arg;

	temp_file_name_prefix = temp_directory;
	if ( *temp_file_name_prefix.rbegin() != '/' )
		temp_file_name_prefix += '/';
	temp_file_name_prefix += string( itoa( ::getpid() ) ) + string( "." );

	/////////// Dump stuff if requested ///////////////////////////////////

#ifdef	MOD_HTML
	if ( dump_html_elements_opt ) {
		element_map const &elements = element_map::instance();
		::copy( elements.begin(), elements.end(),
			ostream_iterator< element_map::value_type >( cout,"\n" )
		);
		::exit( Exit_Success );
	}
#endif
	if ( dump_stop_words_opt ) {
		stop_words = new stop_word_set();
		::copy( stop_words->begin(), stop_words->end(),
			ostream_iterator< char const* >( cout, "\n" )
		);
		::exit( Exit_Success );
	}

	/////////// Index specified directories and files /////////////////////

	bool const using_stdin = *argv && (*argv)[0] == '-' && !(*argv)[1];
	if ( !using_stdin &&
		include_patterns.empty() && exclude_patterns.empty()
	)
		error()	<< "filename patterns must be specified "
			"when not using standard input\n" << usage;

	if ( !argc )
		cerr << usage;

	if ( incremental ) {
		load_old_index( index_file_name );
		index_file_name += ".new";
	} else
		stop_words = new stop_word_set( stop_word_file_name );

	ofstream out( index_file_name, ios::out | ios::binary );
	if ( !out ) {
		error()	<< "can not write index to \""
			<< index_file_name << '"' << endl;
		::exit( Exit_No_Write_Index );
	}

	time_t time = ::time( 0 );		// Go!

	if ( using_stdin ) {
		//
		// Read file/directory names from standard input.
		//
		char file_name[ PATH_MAX + 1 ];
		while ( cin.getline( file_name, PATH_MAX ) ) {
			if ( !file_exists( file_name ) ) {
				if ( verbosity > 3 )
					cout << "  " << file_name
					     << " (skipped: does not exist)\n";
				continue;
			}
			if ( is_directory() )
				do_directory( file_name );
			else
				do_file( file_name );
		}
	} else {
		//
		// Read file/directory names from command line.
		//
		for ( ; *argv; ++argv ) {
			if ( !file_exists( *argv ) ) {
				if ( verbosity > 3 )
					cout << "  " << *argv
					     << " (skipped: does not exist)\n";
				continue;
			}
			if ( is_directory() )
				do_directory( *argv );
			else
				do_file( *argv );
		}
	}

	if ( partial_index_file_names.empty() ) {
		rank_full_index();
		write_full_index( out );
	} else {
		if ( words.size() ) {
			//
			// Since we created partial indicies, write any
			// remaining words to their own partial index so the
			// merge code doesn't have a special case.
			//
			write_partial_index();
		}
		merge_indicies( out );
	}

	out.close();

	if ( verbosity ) {
		time = ::time( 0 ) - time;	// Stop!
		cout	<< '\n' << me << ": done:\n  "
			<< setfill('0')
			<< setw(2) << (time / 60) << ':'
			<< setw(2) << (time % 60)
			<< " (min:sec) elapsed time\n  "
			<< num_examined_files << " files, "
			<< file_info::list_.size() << " indexed\n  "
			<< num_total_words << " words, "
			<< num_indexed_words << " indexed, "
			<< num_unique_words << " unique\n\n";
	}

	::exit( Exit_Success );
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool is_too_frequent( char const *word, int file_count )
//
// DESCRIPTION
//
//	Checks to see if the word is too frequent by either exceeding the
//	maximum number or percentage of files it can be in.
//
// PARAMETERS
//
//	word		The word to be checked.
//
//	file_count	The number of files the word occurs in.
//
// RETURN VALUE
//
//	Returns true only if the word is too frequent.
//
//*****************************************************************************
{
	if ( file_count > word_file_max ) {
		if ( verbosity > 2 )
			cout	<< "\n  \"" << word
				<< "\" discarded (" << file_count << " files)"
				<< flush;
		return true;
	}
	int const wfp = file_count * 100 / file_info::list_.size();
	if ( wfp >= word_percent_max ) {
		if ( verbosity > 2 )
			cout	<< "\n  \"" << word
				<< "\" discarded (" << wfp << "%)" << flush;
		return true;
	}
	return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void load_old_index( char const *index_file_name )
//
// DESCRIPTION
//
//	Load the stop-word, file, and meta-name indicies from an existing index
//	file.
//
// PARAMETERS
//
//	index_file_name		The name of the index file to load.
//
//*****************************************************************************
{
	mmap_file const index_file( index_file_name );
	if ( !index_file ) {
		cerr	<< error << "could not read index from \""
			<< index_file_name << '"' << endl;
		::exit( Exit_No_Read_Index );
	}

	stop_words = new stop_word_set( index_file );

	index_segment old_files( index_file, index_segment::file_index );
	if ( files_reserve <= old_files.size() ) {
		//
		// Add the FilesGrow configuration variable to the FilesReserve
		// configuration variable to allow room for growth.
		//
		files_reserve = files_grow( old_files.size() );
	}
	FOR_EACH( index_segment, old_files, fi )
		file_info::parse( *fi );

	index_segment old_meta_names(
		index_file, index_segment::meta_name_index
	);
	FOR_EACH( index_segment, old_meta_names, meta_name ) {
		unsigned char const *p =
			reinterpret_cast<unsigned char const*>( *meta_name );
		while ( *p++ ) ;		// skip past meta name
		meta_names[ ::strdup( *meta_name ) ] = parse_bcd( p );
	}

	partial_index_file_names.push_back( index_file_name );
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline int rank( int file_index, int occurences_in_file, double factor )
//
// DESCRIPTION
//
//	Compute the rank of a word in a file.  This equation was taken from the
//	one used in SWISH-E whose author thinks (?) it is the one taken from
//	WAIS.  I can't find this equation in the refernece cited below,
//	although that reference does list a different equation.  But, if it
//	ain't broke, don't fix it.
//
// PARAMETERS
//
//	file_index		Which file we're dealing with.
//
//	occurences_in_file	The number of times the word occurs in a given
//				file.
//
//	factor			This should be precomputed to be the value of
//				10000.0 divided by the total number of
//				occurrences across all files.  This number is
//				constant for a given word, hence the
//				precomputation.
//
// RETURN VALUE
//
//	Returns a rank greater than zero.
//
// SEE ALSO
//
//	Gerard Salton.  "Automatic Text Processing: the transformation,
//	analysis, and retrieval of information by computer."  Addison-Wesley,
//	Reading, MA.  pp. 279-280.
//
//*****************************************************************************
{
	int r = int(
		( ::log( occurences_in_file ) + 10 ) * factor
		/ file_info::list_[ file_index ]->num_words()
	);
	return r > 0 ? r : 1;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void merge_indicies( ostream &o )
//
// DESCRIPTION
//
//	Perform an n-way merge of the partial word index files.  It first
//	determines the number of unique words in all the partial indicies, then
//	merges them all together and performs ranking at the same time.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//*****************************************************************************
{
	vector< mmap_file > index( partial_index_file_names.size() );
	vector< index_segment > words( partial_index_file_names.size() );
	vector< index_segment::const_iterator >
		word( partial_index_file_names.size() );
	register int i, j;

	////////// Reopen all the partial indicies ////////////////////////////

	::atexit( &remove_temp_files );
	i = 0;
	FOR_EACH( vector<string>, partial_index_file_names, file_name ) {
		index[ i ].open( file_name->c_str() );
		if ( !index[ i ] ) {
			error()	<< "can not reopen temp. file \""
				<< *file_name << '"' << endl;
			::exit( Exit_No_Open_Temp );
		}
		words[ i ].set_index_file(
			index[ i ], index_segment::word_index
		);
		++i;
	}

	////////// Must determine the number of unique words first ////////////

	if ( verbosity > 1 )
		cout << me << ": determining unique words..." << flush;

	for ( i = 0; i < partial_index_file_names.size(); ++i ) {
		// Start off assuming that all the words are unique.
		num_unique_words += words[ i ].size();
		word[ i ] = words[ i ].begin();
	}
	while ( true ) {

		// Find at least two non-exhausted indicies noting the first.
		register int n = 0;
		for ( j = 0; j < partial_index_file_names.size(); ++j )
			if ( word[ j ] != words[ j ].end() )
				if ( !n++ )
					i = j;
				else if ( n >= 2 )
					break;
		if ( n < 2 )			// couldn't find at least 2
			break;

		// Find the lexographically least word.
		for ( j = i + 1; j < partial_index_file_names.size(); ++j )
			if ( word[ j ] != words[ j ].end() )
				if ( ::strcmp( *word[ j ], *word[ i ] ) < 0 )
					i = j;

		file_list const list( word[ i ] );
		int file_count = list.size();

		// See if there are any duplicates and eliminate them.
		for ( j = i + 1; j < partial_index_file_names.size(); ++j )
			if ( word[ j ] != words[ j ].end() )
				if ( !::strcmp( *word[ j ], *word[ i ] ) ) {
					//
					// The two words are the same: add the
					// second word's file count to that of
					// the first.
					//
					--num_unique_words;
					file_list const list( word[ j ] );
					file_count += list.size();
					++word[ j ];
				}

		if ( is_too_frequent( *word[ i ], file_count ) ) {
			//
			// The word occurs too frequently: consider it a stop
			// word.
			//
			stop_words->insert( *word[ i ] );
			--num_unique_words;
		}

		++word[ i ];
	}

	////////// Write index file header ////////////////////////////////////

	long const num_files = file_info::list_.size();
	long const num_stop_words = stop_words->size();
	long const num_meta_names = meta_names.size();
	off_t *const word_offset = new off_t[ num_unique_words ];
	off_t *const stop_word_offset = num_stop_words ?
		new off_t[ num_stop_words ] : 0;
	off_t *const file_offset = new off_t[ num_files ];
	off_t *const meta_name_offset = num_meta_names ?
		new off_t[ num_meta_names ] : 0;

	o.write( &num_unique_words, sizeof( num_unique_words ) );
	streampos const word_offset_pos = o.tellp();
	o.write( word_offset, num_unique_words * sizeof( word_offset[0] ) );

	o.write( &num_stop_words, sizeof( num_stop_words ) );
	streampos const stop_word_offset_pos = o.tellp();
	if ( num_stop_words )
		o.write( stop_word_offset,
			num_stop_words * sizeof( stop_word_offset[0] )
		);

	o.write( &num_files, sizeof( num_files ) );
	streampos const file_offset_pos = o.tellp();
	o.write( file_offset, num_files * sizeof( file_offset[0] ) );

	o.write( &num_meta_names, sizeof( num_meta_names ) );
	streampos const meta_name_offset_pos = o.tellp();
	if ( num_meta_names )
		o.write( meta_name_offset,
			num_meta_names * sizeof( meta_name_offset[0] )
		);

	////////// Merge the indicies /////////////////////////////////////////

	if ( verbosity > 1 )
		cout << '\n' << me << ": merging partial indicies..." << flush;

	for ( i = 0; i < partial_index_file_names.size(); ++i )
		word[ i ] = words[ i ].begin();		// reset all iterators
	int word_index = 0;
	while ( true ) {

		////////// Find the next word /////////////////////////////////

		// Find at least two non-exhausted indicies noting the first.
		register int n = 0;
		for ( j = 0; j < partial_index_file_names.size(); ++j ) {
			for ( ; word[ j ] != words[ j ].end(); ++word[ j ] )
				if ( !stop_words->contains( *word[ j ] ) )
					break;
			if ( word[ j ] != words[ j ].end() )
				if ( !n++ )
					i = j;
				else if ( n >= 2 )
					break;
		}
		if ( n < 2 )			// couldn't find at least 2
			break;

		// Find the lexographically least word.
		for ( j = i + 1; j < partial_index_file_names.size(); ++j )
			if ( word[ j ] != words[ j ].end() )
				if ( ::strcmp( *word[ j ], *word[ i ] ) < 0 )
					i = j;

		word_offset[ word_index++ ] = o.tellp();
		o << *word[ i ] << '\0';

		////////// Calc. total occurrences in all indicies ////////////

		int total_occurrences = 0;
		for ( j = i; j < partial_index_file_names.size(); ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( ::strcmp( *word[ j ], *word[ i ] ) )
				continue;
			file_list const list( word[ j ] );
			FOR_EACH( file_list, list, file )
				total_occurrences += file->occurrences_;
		}
		double const factor = 10000.0 / total_occurrences;

		////////// Copy all index info and compute ranks //////////////

		for ( j = i; j < partial_index_file_names.size(); ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( ::strcmp( *word[ j ], *word[ i ] ) )
				continue;
			file_list const list( word[ j ] );
			FOR_EACH( file_list, list, file ) {
				o << bcd( file->index_ );
				if ( !file->meta_ids_.empty() )
					file->write_meta_ids( o );
				o << bcd( file->occurrences_ ) << bcd( rank(
					file->index_, file->occurrences_, factor
				) );
			}

			if ( j != i ) ++word[ j ];
		}
		o << '\xFF';

		++word[ i ];
	}

	////////// Copy remaining words from last non-exhausted index /////////

	for ( j = 0; j < partial_index_file_names.size(); ++j ) {
		if ( word[ j ] == words[ j ].end() )
			continue;

		for ( ; word[ j ] != words[ j ].end(); ++word[ j ] ) {
			if ( stop_words->contains( *word[ j ] ) )
				continue;

			word_offset[ word_index++ ] = o.tellp();
			o << *word[ j ] << '\0';

			////////// Calc. total occurrences in all indicies ////

			int total_occurrences = 0;
			file_list const list( word[ j ] );
			FOR_EACH( file_list, list, file )
				total_occurrences += file->occurrences_;
			double const factor = 10000.0 / total_occurrences;

			////////// Copy all index info and compute ranks //////

			FOR_EACH( file_list, list, file ) {
				o << bcd( file->index_ );
				if ( !file->meta_ids_.empty() )
					file->write_meta_ids( o );
				o << bcd( file->occurrences_ ) << bcd( rank(
					file->index_, file->occurrences_, factor
				) );
			}
			o << '\xFF';
		}
	}

	write_stop_word_index( o, stop_word_offset );
	write_file_index( o, file_offset );
	write_meta_name_index( o, meta_name_offset );

	////////// Go back and write the computed offsets /////////////////////

	o.seekp( word_offset_pos );
	o.write( word_offset, num_unique_words * sizeof( word_offset[0] ) );
	if ( num_stop_words ) {
		o.seekp( stop_word_offset_pos );
		o.write( stop_word_offset,
			num_stop_words * sizeof( stop_word_offset[0] )
		);
	}
	o.seekp( file_offset_pos );
	o.write( file_offset, num_files * sizeof( file_offset[0] ) );
	if ( num_meta_names ) {
		o.seekp( meta_name_offset_pos );
		o.write( meta_name_offset,
			num_meta_names * sizeof( meta_name_offset[0] )
		);
	}

	delete[] word_offset;
	delete[] stop_word_offset;
	delete[] file_offset;
	delete[] meta_name_offset;

	if ( verbosity > 1 )
		cout << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	void rank_full_index()
//
// DESCRIPTION
//
//	Compute the rank of all files for all words in the index.  This
//	function is used only when partial indicies are not generated.  Also
//	removes words that occur too frequently.
//
//*****************************************************************************
{
	if ( words.empty() )
		return;

	if ( verbosity > 1 )
		cout << '\n' << me << ": ranking index..." << flush;

	TRANSFORM_EACH( word_map, words, w ) {
		word_info &info = w->second;

		if ( is_too_frequent( w->first.c_str(), info.files_.size() ) ) {
			//
			// The word occurs too frequently: consider it a stop
			// word.
			//
			stop_words->insert( ::strdup( w->first.c_str() ) );
			words.erase( w );
			continue;
		}

		//
		// Compute the rank for this word in every file it's in.
		//
		double const factor = 10000.0 / info.occurrences_;
		TRANSFORM_EACH( word_info::file_list, info.files_, file )
			file->rank_ = rank(
				file->index_, file->occurrences_, factor
			);
	}

	if ( verbosity > 1 )
		cout <<	'\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	void remove_temp_files( void )
//
// DESCRIPTION
//
//	Remove the temporary partial index files.  This function is called via
//	atexit(3).
//
// NOTE
//
//	This function is declared extern "C" since it is called via the C
//	library function atexit(3) and, because it's a C function, it expects C
//	linkage.
//
//*****************************************************************************
{
	for ( int i = 0; i < num_temp_files; ++i ) {
		string const temp_file_name = temp_file_name_prefix + itoa( i );
		::unlink( temp_file_name.c_str() );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_file_index( ostream &o, register off_t *offset )
//
// DESCRIPTION
//
//	Write the file index to the given ostream recording the offsets as it
//	goes.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//	offset	A pointer to a built-in vector where to record the offsets.
//
//*****************************************************************************
{
	register int file_index = 0;
	FOR_EACH( file_info::list_type, file_info::list_, fi ) {
		offset[ file_index++ ] = o.tellp();
		o	<< (*fi)->file_name() << '\0' << bcd( (*fi)->size() )
			<< bcd( (*fi)->num_words() ) << (*fi)->title() << '\0';
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_meta_name_index( ostream &o, register off_t *offset )
//
// DESCRIPTION
//
//	Write the meta name index to the given ostream recording the offsets as
//	it goes.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//	offset	A pointer to a built-in vector where to record the offsets.
//
//*****************************************************************************
{
	register int meta_index = 0;
	FOR_EACH( meta_map, meta_names, m ) {
		offset[ meta_index++ ] = o.tellp();
		o << m->first << '\0' << bcd( m->second );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_full_index( ostream &o )
//
// DESCRIPTION
//
//	Write the index to the given ostream.  The index file is written in
//	such a way so that it can be mmap'd and used instantly with no parsing
//	or other processing.  The format of an index file is:
//
//		long	num_unique_words;
//		off_t	word_offset[ num_unique_words ];
//		long	num_stop_words;
//		off_t	stop_word_offset[ num_stop_words ];
//		long	num_files;
//		off_t	file_offset[ num_files ];
//		long	num_meta_names;
//		off_t	meta_name_offset[ num_meta_names ];
//			(word index)
//			(stop-word index)
//			(file index)
//			(meta-name index)
//
//	The word index is a list of all the words indexed in alphabetical
//	order.  Each entry is of the form:
//
//		word\0{data}...\xFF
//
//	that is: a null-terminated word followed by one or more data entries
//	followed by an \xFF byte where a data entry is:
//
//		I[\xEE{M}...\xEE]OR
//
//	that is: a file-index (I) followed by zero or more meta-IDs (M)
//	surrounded by \xEE bytes followed by the number of occurrences in the
//	file (O) followed by a rank (R) followed by an \xFF byte.  The integers
//	are in BCD (binary coded decimal) not raw binary.  A BCD integer that
//	has an odd number of digits is terminated by a low-order nybble with
//	the value \xA; an integer that has an even number of digits is
//	terminated by a a byte with the value \xAA.  (These values were chosen
//	because they are invalid BCD.  All other values \xA0 through \xFE are
//	reserved for future use.)  The file-index is an index into the file
//	offset table; the meta-IDs, if present, are unique integers identifying
//	which meta name(s) a word is associated with in the meta-name index.
//
//	The stop-word index is a list of all the stop words (either the built-
//	in list or the list supplied from a file, plus those that exceed either
//	the file limit or word percentage) in alphabetical order.  Each entry
//	is of the form:
//
//		word\0
//
//	where the word is terminated by a null byte.
//
//	The file index is a list of file information.  Each entry is of the
//	form:
//
//		path_name\0{S}{W}file_title\0
//
//	that is: a null-terminated pathname followed by the file's size in
//	bytes (S) followed by the number of words in the file (W) followed by
//	the file's null-terminated title.  The integers are in BCD format.
//	Each file_offset points to the first character in a path name.
//
//	The meta-name index is a list of all the meta names (in alphabetical
//	order) encountered during indexing of all the files.  Each meta-name is
//	followed by its numeric ID that is simply a unique integer to identify
//	it.  Each entry is of the form:
//
//		meta_name\0{I}
//
//	that is: a null-terminated meta-name followed by the ID (I) in BCD
//	format.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//*****************************************************************************
{
	if ( !( num_unique_words = words.size() ) )
		return;

	if ( verbosity > 1 )
		cout << me << ": writing index..." << flush;

	long const num_stop_words = stop_words->size();
	long const num_files = file_info::list_.size();
	long const num_meta_names = meta_names.size();
	off_t *const word_offset = new off_t[ num_unique_words ];
	off_t *const stop_word_offset = num_stop_words ?
		new off_t[ num_stop_words ] : 0;
	off_t *const file_offset = new off_t[ num_files ];
	off_t *const meta_name_offset = num_meta_names ?
		new off_t[ num_meta_names ] : 0;

	// Write dummy data as a placeholder until the offsets are computed.
	o.write( &num_unique_words, sizeof( num_unique_words ) );
	streampos const word_offset_pos = o.tellp();
	o.write( word_offset, num_unique_words * sizeof( word_offset[0] ) );

	o.write( &num_stop_words, sizeof( num_stop_words ) );
	streampos const stop_word_offset_pos = o.tellp();
	if ( num_stop_words )
		o.write( stop_word_offset,
			num_stop_words * sizeof( stop_word_offset[0] )
		);

	o.write( &num_files, sizeof( num_files ) );
	streampos const file_offset_pos = o.tellp();
	o.write( file_offset, num_files * sizeof( file_offset[0] ) );

	o.write( &num_meta_names, sizeof( num_meta_names ) );
	streampos const meta_name_offset_pos = o.tellp();
	if ( num_meta_names )
		o.write( meta_name_offset,
			num_meta_names * sizeof( meta_name_offset[0] )
		);

	write_word_index( o, word_offset );
	write_stop_word_index( o, stop_word_offset );
	write_file_index( o, file_offset );
	write_meta_name_index( o, meta_name_offset );

	// Go back and write the computed offsets.
	o.seekp( word_offset_pos );
	o.write( word_offset, num_unique_words * sizeof( word_offset[0] ) );
	if ( num_stop_words ) {
		o.seekp( stop_word_offset_pos );
		o.write( stop_word_offset,
			num_stop_words * sizeof( stop_word_offset[0] )
		);
	}
	o.seekp( file_offset_pos );
	o.write( file_offset, num_files * sizeof( file_offset[0] ) );
	if ( num_meta_names ) {
		o.seekp( meta_name_offset_pos );
		o.write( meta_name_offset,
			num_meta_names * sizeof( meta_name_offset[0] )
		);
	}

	delete[] word_offset;
	delete[] stop_word_offset;
	delete[] file_offset;
	delete[] meta_name_offset;

	if ( verbosity > 1 )
		cout << '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_partial_index()
//
// DESCRIPTION
//
//	Write a partial index to a temporary file.  The format of a partial
//	index file is:
//
//		long	num_words;
//		off_t	word_offset[ num_words ];
//			(word index)
//
//	The partial word index is in the same format as the complete index.
//
//*****************************************************************************
{
	string const temp_file_name =
		temp_file_name_prefix + itoa( num_temp_files++ );
	ofstream o( temp_file_name.c_str(), ios::out | ios::binary );
	if ( !o ) {
		error()	<< "can not write temp. file \""
			<< temp_file_name << '"' << endl;
		::exit( Exit_No_Write_Temp );
	}
	partial_index_file_names.push_back( temp_file_name );

	if ( verbosity > 1 )
		cout << '\n' << me << ": writing partial index..." << flush;

	long const num_words = words.size();
	off_t *const word_offset = new off_t[ num_words ];

	// Write dummy data as a placeholder until the offsets are computed.
	o.write( &num_words, sizeof( num_words ) );
	streampos const word_offset_pos = o.tellp();
	o.write( word_offset, num_words * sizeof( word_offset[0] ) );

	write_word_index( o, word_offset );

	// Go back and write the computed offsets.
	o.seekp( word_offset_pos );
	o.write( word_offset, num_words * sizeof( word_offset[0] ) );

	delete[] word_offset;
	words.clear();

	if ( verbosity > 1 )
		cout << "\n\n";
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_stop_word_index( ostream &o, register off_t *offset )
//
// DESCRIPTION
//
//	Write the stop-word index to the given ostream recording the offsets as
//	it goes.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//	offset	A pointer to a built-in vector where to record the offsets.
//
//*****************************************************************************
{
	register int word_index = 0;
	FOR_EACH( stop_word_set, *stop_words, word ) {
		offset[ word_index++ ] = o.tellp();
		o << *word << '\0';
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_word_index( ostream &o, register off_t *offset )
//
// DESCRIPTION
//
//	Write the word index to the given ostream recording the offsets as it
//	goes.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//	offset	A pointer to a built-in vector where to record the offsets.
//
//*****************************************************************************
{
	register int word_index = 0;
	FOR_EACH( word_map, words, w ) {
		offset[ word_index++ ] = o.tellp();
		o << w->first << '\0';

		word_info const &info = w->second;
		FOR_EACH( word_info::file_list, info.files_, file ) {
			o << bcd( file->index_ );
			if ( !file->meta_ids_.empty() )
				file->write_meta_ids( o );
			o << bcd( file->occurrences_ ) << bcd( file->rank_ );
		}

		o << '\xFF';
	}
}

//*****************************************************************************
//
//	Miscellaneous function(s)
//
//*****************************************************************************

ostream& usage( ostream &o ) {
	o << "usage: " << me << " [options] dir ... file ...\n"
	"options: (unambiguous abbreviations may be used for long options)\n"
	"========\n"
	"-?     | --help            : Print this help message\n"
	"-c f   | --config-file f   : Name of configuration file [default: " << ConfigFile_Default << "]\n"
#ifdef	MOD_HTML
	"-C c   | --no-class c      : Class name not to index [default: none]\n"
#endif
	"-e m:p | --pattern m:p     : Module and file pattern to index [default: none]\n"
	"-E p   | --no-pattern p    : File pattern not to index [default: none]\n"
	"-f n   | --word-files n    : Word/file maximum [default: infinity]\n"
	"-F n   | --files-reserve n : Reserve space for number of files [default: " << FilesReserve_Default << "]\n"
	"-G n   | --files-grow n    : Number or percentage to grow by [default: " << FilesGrow_Default << "]\n"
#ifdef	MOD_HTML
	"-H     | --dump-html       : Dump built-in recognized HTML/XHTML elements, exit\n"
#endif
	"-i f   | --index-file f    : Name of index file to use [default: " << IndexFile_Default << "]\n"
	"-I     | --incremental     : Add files to index [default: replace]\n"
#ifndef	PJL_NO_SYMBOLIC_LINKS
	"-l     | --follow-links    : Follow symbolic links [default: no]\n"
#endif
	"-m m   | --meta m          : Meta name to index [default: all]\n"
	"-M m   | --no-meta m       : Meta name not to index [default: none]\n"
	"-p n   | --word-percent n  : Word/file percentage [default: 100]\n"
	"-r     | --no-recurse      : Don't index subdirectories [default: do]\n"
	"-s f   | --stop-file f     : Stop-word file to use instead of built-in default\n"
	"-S     | --dump-stop       : Dump built-in stop-words, exit\n"
	"-t n   | --title-lines n   : Lines to look for <TITLE> [default: " << TitleLines_Default << "]\n"
	"-T d   | --temp-dir d      : Directory for temporary files [default: " << TempDirectory_Default << "]\n"
	"-v n   | --verbosity n     : Verbosity level [0-4; default: 0]\n"
	"-V     | --version         : Print version number, exit\n";
	::exit( Exit_Usage );
	return o;			// just to make the compiler happy
}
