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
#include <algorithm>
#include <climits>
#include <cmath>				/* for log(3) */
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#ifdef	WIN32
int const	MAXNAMLEN = 255;
#else
#include <dirent.h>				/* for MAXNAMLEN */
#endif

// local
#include "config.h"
#include "directory.h"
#include "ExcludeClass.h"
#include "ExcludeExtension.h"
#include "ExcludeMeta.h"
#include "exit_codes.h"
#include "FilesReserve.h"
#include "file_info.h"
#include "file_list.h"
#include "file_vector.h"
#include "FilterExtension.h"
#include "html.h"
#include "IncludeExtension.h"
#include "IncludeMeta.h"
#include "index.h"
#include "IndexFile.h"
#include "meta_map.h"
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
#include "WordPercentMax.h"
#include "word_index.h"
#include "word_info.h"

extern "C" {
	extern char*	optarg;
	extern int	optind, opterr;
}

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

ExcludeExtension exclude_extensions;		// do not index these
IncludeExtension include_extensions;		// do index these
int		exclude_class_count;		// don't index words if > 0
ExcludeClass	exclude_class_names;		// class names not to index
ExcludeMeta	exclude_meta_names;		// meta names not to index
IncludeMeta	include_meta_names;		// meta names to index
FilterExtension	filters;
char const*	me;				// executable name
meta_map	meta_names;
FilesReserve	num_files_reserve;
TitleLines	num_title_lines;
long		num_total_words;		// over all files indexed
long		num_indexed_words;		// over all files indexed
long		num_unique_words;		// over all files indexed
int		num_examined_files;
int		num_temp_files;
RecurseSubdirs	recurse_subdirectories;
string		temp_file_prefix;
Verbosity	verbosity;			// how much to print
word_map	words;				// the index being generated
WordFilesMax	word_file_max;
WordPercentMax	word_percent_max;

void		do_file( char const *path );
void		index_word( char *word, int len, int meta_id );
void		merge_indicies( ostream& );
void		rank_full_index();
extern "C" void	remove_temp_files();
void		usage();
void		write_file_index( ostream&, off_t* );
void		write_full_index( ostream& );
void		write_meta_name_index( ostream&, off_t* );
void		write_partial_index();
void		write_stop_word_index( ostream&, off_t* );
void		write_word_index( ostream&, off_t* );

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
//		the options below, the arguments are the names of the files
//		and directories to be indexed.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA.  pp. 116-118.
//
//*****************************************************************************
{
	me = ::strrchr( argv[0], '/' );		// determine base name...
	me = me ? me + 1 : argv[0];		// ...of executable

	/////////// Process command-line options //////////////////////////////

	char const*	config_file_name_arg = ConfigFile_Default;
	bool		dump_stop_words_opt = false;
#ifndef	PJL_NO_SYMBOLIC_LINKS
	bool		follow_symbolic_links_opt = false;
#endif
	IndexFile	index_file_name;
	char const*	index_file_name_arg = 0;
	char const*	num_files_reserve_arg = 0;
	char const*	num_title_lines_arg = 0;
	bool		recurse_subdirectories_opt = false;
	StopWordFile	stop_word_file_name;
	char const*	stop_word_file_name_arg = 0;
	TempDirectory	temp_directory;
	char const*	temp_directory_arg = TempDirectory_Default;
	char const*	verbosity_arg = 0;
	char const*	word_file_max_arg = 0;
	char const*	word_percent_max_arg = 0;

	char const opts[] =
#ifndef	PJL_NO_SYMBOLIC_LINKS
		"l"
#endif
		"c:C:e:E:f:F:i:m:M:p:rs:St:T:v:V";

	::opterr = 1;
	for ( int opt; (opt = ::getopt( argc, argv, opts )) != EOF; )
		switch ( opt ) {

			case 'c': // Specify config. file.
				config_file_name_arg = ::optarg;
				break;

			case 'C': // Specify CLASS name not to index.
				exclude_class_names.insert(
					::strdup( to_lower( ::optarg ) )
				);
				break;

			case 'e': // Specify filename extension(s) to index.
				include_extensions.insert( ::optarg );
				break;

			case 'E': // Specify filename extension(s) not to index.
				exclude_extensions.insert( ::optarg );
				break;

			case 'f': // Specify the word/file file maximum.
				word_file_max_arg = ::optarg;
				break;

			case 'F': // Specify files to reserve space for.
				num_files_reserve_arg = ::optarg;
				break;

			case 'i': // Specify index file overriding the default.
				index_file_name_arg = ::optarg;
				break;
#ifndef	PJL_NO_SYMBOLIC_LINKS
			case 'l': // Follow symbolic links during indexing.
				follow_symbolic_links_opt = true;
				break;
#endif
			case 'm': // Specify meta name(s) to index.
				include_meta_names.insert(
					::strdup( to_lower( ::optarg ) )
				);
				break;

			case 'M': // Specify meta name(s) not to index.
				exclude_meta_names.insert(
					::strdup( to_lower( ::optarg ) )
				);
				break;

			case 'p': // Specify the word/file percentage.
				word_percent_max_arg = ::optarg;
				break;

			case 'r': // Specify whether to index recursively.
				recurse_subdirectories_opt = true;
				break;

			case 's': // Specify stop-word list.
				stop_word_file_name_arg = ::optarg;
				break;

			case 'S': // Dump stop-word list.
				dump_stop_words_opt = true;
				break;

			case 't': // Specify number of title lines.
				num_title_lines_arg = ::optarg;
				break;

			case 'T': // Specify temp. directory.
				temp_directory_arg = ::optarg;
				break;

			case 'v': // Specify verbosity level.
				verbosity_arg = ::optarg;
				break;

			case 'V': // Display version and exit.
				cout << "SWISH++ " << version << endl;
				::exit( Exit_Success );

			case '?': // Bad option.
				usage();
		}
	argc -= ::optind, argv += ::optind;

	//
	// First, parse the config. file (if any); then override variables
	// specified on the command line with options.
	//
	parse_config_file( config_file_name_arg );

#ifndef	PJL_NO_SYMBOLIC_LINKS
	if ( follow_symbolic_links_opt )
		follow_symbolic_links = true;
#endif
	if ( index_file_name_arg )
		index_file_name = index_file_name_arg;
	if ( num_files_reserve_arg )
		num_files_reserve = num_files_reserve_arg;
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

	temp_file_prefix = temp_directory;
	if ( *temp_file_prefix.rbegin() != '/' )
		temp_file_prefix += '/';
	temp_file_prefix += string( itoa( ::getpid() ) ) + string( "." );

	/////////// Deal with stop-words //////////////////////////////////////

	stop_words = new stop_word_set( stop_word_file_name );
	if ( dump_stop_words_opt ) {
		::copy( stop_words->begin(), stop_words->end(),
			ostream_iterator< char const* >( cout, "\n" )
		);
		::exit( Exit_Success );
	}

	/////////// Index specified directories and files /////////////////////

	bool const using_stdin = *argv && (*argv)[0] == '-' && !(*argv)[1];
	if ( !using_stdin &&
		include_extensions.empty() && exclude_extensions.empty()
	) {
		ERROR << "extensions must be specified "
			"when not using standard input " << endl;
		usage();
	}
	if ( !argc )
		usage();

	ofstream out( index_file_name, ios::out | ios::binary );
	if ( !out ) {
		ERROR	<< "can not write index to \""
			<< index_file_name << '"' << endl;
		::exit( Exit_No_Write_Index );
	}

	time_t time = ::time( 0 );		// Go!

	if ( using_stdin ) {
		//
		// Read file/directory names from standard input.
		//
		char file_name[ MAXNAMLEN + 1 ];
		while ( cin.getline( file_name, MAXNAMLEN ) ) {
			if ( !file_exists( file_name ) ) {
				if ( verbosity > 3 )
					cout	<< " (skipped: does not exist)"
						<< endl;
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
					cout	<< " (skipped: does not exist)"
						<< endl;
				continue;
			}
			if ( is_directory() )
				do_directory( *argv );
			else
				do_file( *argv );
		}
	}

	if ( num_temp_files ) {
		if ( words.size() ) {
			//
			// If we created any partial indicies, write the
			// remaining words to their own partial index so the
			// merge code doesn't have a special case.
			//
			write_partial_index();
		}
		merge_indicies( out );
	} else {
		rank_full_index();
		write_full_index( out );
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
			<< file_info::set_.size() << " indexed\n  "
			<< num_total_words << " words, "
			<< num_indexed_words << " indexed, "
			<< num_unique_words << " unique\n\n"
			<< setfill(' ');
	}

	::exit( Exit_Success );
}

#define	INDEX
#include "do_file.c"

//*****************************************************************************
//
// SYNOPSIS
//
	void index_word( register char *word, register int len, int meta_id )
//
// DESCRIPTION
//
//	Potentially index the given word.
//
// PARAMETERS
//
//	word		The candidate word to be indexed.
//
//	len		The length of the word since it is not null-terminated.
//
//	meta_id		Specifies the numeric ID of the META NAME the word, if
//			indexed, is to be associated with.
//
//*****************************************************************************
{
	++num_total_words;

	if ( len < Word_Hard_Min_Size )
		return;

	if ( exclude_class_count ) {
		//
		// Word is within an HTML element's begin/end tags whose begin
		// tag's CLASS attribute value is among the set of class names
		// not to index.
		//
		return;
	}

	////////// Strip chars not in Word_Begin_Chars/Word_End_Chars /////////

	for ( register int i = len - 1; i >= 0; --i ) {
		if ( is_word_end_char( word[ i ] ) )
			break;
		--len;
	}
	if ( len < Word_Hard_Min_Size )
		return;

	word[ len ] = '\0';

	while ( *word ) {
		if ( is_word_begin_char( *word ) )
			break;
		--len, ++word;
	}
	if ( len < Word_Hard_Min_Size )
		return;

	////////// Stop-word checks ///////////////////////////////////////////

	if ( !is_ok_word( word ) )
		return;

	char const *const lower_word = to_lower( word );
	if ( stop_words->find( lower_word ) )
		return;

	////////// Add the word ///////////////////////////////////////////////

	++file_info::current_file().num_words_;
	++num_indexed_words;

	word_info &wi = words[ lower_word ];
	++wi.occurrences_;

	if ( !wi.files_.empty() ) {
		//
		// We've seen this word before: determine whether we've seen it
		// before in THIS file, and, if so, increment the number of
		// occurrences and associate with the current meta name, if
		// any.
		//
		word_info::file &last_file = wi.files_.back();
		if ( last_file.index_ == file_info::current_index() ) {
			++last_file.occurrences_;
			if ( meta_id != No_Meta_ID )
				last_file.meta_ids_.insert( meta_id );
			return;
		}
	}

	// First time word occurred in current file.
	wi.files_.push_back(
		word_info::file( file_info::current_index(), meta_id )
	);
}

//*****************************************************************************
//
// SYNOPSIS
//
	void index_words(
		register file_vector<char>::const_iterator c,
		register file_vector<char>::const_iterator end,
		bool is_html, int meta_id
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.
//
// PARAMETERS
//
//	c		The iterator marking the beginning of the text to
//			index.
//
//	end		The iterator marking the end of the text to index.
//
//	is_html		If true, treat the text as HTML: specifically, look for
//			'<' (the start of an HTML tag) and '&' (the start of
//			an entity reference) characters and process them
//			accordingly.
//
//	meta_id		Specifies the numeric ID of the META NAME the words
//			indexed are to be associated with.
//
//*****************************************************************************
{
	static bool new_file = true;
	char buf[ Word_Hard_Max_Size + 1 ];
	register char *word;
	int len;
	bool in_word = false;

	while ( c != end ) {
		register char ch = *c++;

		if ( is_html && ch == '&' )
			ch = convert_entity( c, end );

		////////// Collect a word /////////////////////////////////////

		if ( is_word_char( ch ) ) {
			if ( !in_word ) {
				// start a new word
				word = buf;
				word[ 0 ] = ch;
				len = 1;
				in_word = true;
				continue;
			}
			if ( len < Word_Hard_Max_Size ) {
				// continue same word
				word[ len++ ] = ch;
				continue;
			}
			in_word = false;	// too big: skip chars
			while ( c != end && is_word_char( *c++ ) ) ;
			continue;
		}

		if ( in_word ) {
			in_word = false;
			index_word( word, len, meta_id );
		}

		if ( is_html && ch == '<' && meta_id == No_Meta_ID ) {
			parse_html_tag( c, end, new_file );
			new_file = false;
		}
	}
	if ( in_word )
		index_word( word, len, meta_id );

	new_file = true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline int rank( int file_index, int occurences_in_file, double factor )
//
// DESCRIPTION
//
//	Compute the rank of a word in a file.  This equation was taken from
//	the one used in SWISH-E whose author thinks (?) it is the one taken
//	from WAIS.  I can't find this equation in the refernece cited below,
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
//	Returns the rank.
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
		/ file_info::set_[ file_index ]->num_words_
	);
	return r > 0 ? r : 1;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline int word_file_percentage( int file_count )
//
// DESCRIPTION
//
//	Compute the percentage of the number of files a word occurs in.
//
// RETURN VALUE
//
//	Returns said percentage.
//
//*****************************************************************************
{
	return file_count * 100 / file_info::set_.size();
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
//	determines the number of unique words in all the partial indicies,
//	then merges them all together and performs ranking at the same time.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//*****************************************************************************
{
	vector< file_vector<char> > index( num_temp_files );
	vector< word_index > words( num_temp_files );
	vector< word_index::const_iterator > word( num_temp_files );
	register int i, j;

	////////// Reopen all the partial indicies ////////////////////////////

	::atexit( &remove_temp_files );
	for ( i = 0; i < num_temp_files; ++i ) {
		string const temp_file = temp_file_prefix + itoa( i );
		index[ i ].open( temp_file.c_str() );
		if ( !index[ i ] ) {
			ERROR	<< "can not reopen temp. file \""
				<< temp_file << '"' << endl;
			::exit( Exit_No_Open_Temp );
		}
		words[ i ].set_index_file( index[ i ] );
	}

	////////// Must determine the number of unique words first ////////////

	if ( verbosity > 1 )
		cout << me << ": determining unique words..." << flush;

	for ( i = 0; i < num_temp_files; ++i ) {
		// Start off assuming that all the words are unique.
		num_unique_words += words[ i ].size();
		word[ i ] = words[ i ].begin();
	}
	while ( 1 ) {
		// Find at least two non-exhausted indicies noting the first.
		int n = 0;
		for ( j = 0; j < num_temp_files; ++j )
			if ( word[ j ] != words[ j ].end() && !n++ )
				i = j;
		if ( n < 2 )			// couldn't find at least 2
			break;

		// Find the lexographically least word.
		for ( j = i + 1; j < num_temp_files; ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( ::strcmp( *word[ j ], *word[ i ] ) < 0 )
				i = j;
		}

		file_list list( word[ i ] );
		int file_count = list.size();

		// See if there are any duplicates and eliminate them.
		for ( j = i + 1; j < num_temp_files; ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( !::strcmp( *word[ i ], *word[ j ] ) ) {
				--num_unique_words;
				file_list list( word[ j ] );
				file_count += list.size();
				++word[ j ];
			}
		}

		// Remove words that occur too frequently.
		if ( file_count > word_file_max ) {
			if ( verbosity > 2 )
				cout	<< "\n  \"" << *word[ i ]
					<< "\" discarded (" << file_count
					<< " files)" << flush;
			goto remove;
		} else {
			int const wfp = word_file_percentage( file_count );
			if ( wfp >= word_percent_max ) {
				if ( verbosity > 2 )
					cout	<< "\n  \"" << *word[ i ]
						<< "\" discarded (" << wfp
						<< "%)" << flush;
				goto remove;
			}
		}

		++word[ i ];
		continue;

	remove:	stop_words->insert( *word[ i ] );
		--num_unique_words;
	}

	////////// Write index file header ////////////////////////////////////

	long const num_files = file_info::set_.size();
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

	for ( i = 0; i < num_temp_files; ++i )		// reset all iterators
		word[ i ] = words[ i ].begin();
	int word_index = 0;
	while ( 1 ) {

		////////// Find the next word /////////////////////////////////

		// Find at least two non-exhausted indicies.
		int n = 0;
		for ( j = 0; j < num_temp_files; ++j )
			if ( word[ j ] != words[ j ].end() && !n++ )
				i = j;
		if ( n < 2 )
			break;

		// Find the lexographically least word.
		for ( j = i + 1; j < num_temp_files; ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( ::strcmp( *word[ j ], *word[ i ] ) < 0 )
				i = j;
		}

		if ( stop_words->find( *word[ i ] ) )
			continue;

		word_offset[ word_index++ ] = o.tellp();
		o << *word[ i ] << '\0';

		////////// Determine total occurrences in all indicies ////////

		int total_occurrences = 0;
		for ( j = i; j < num_temp_files; ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( ::strcmp( *word[ i ], *word[ j ] ) )
				continue;

			file_list list( word[ j ] );
			FOR_EACH( file_list, list, file )
				total_occurrences += file->occurrences_;
		}

		////////// Copy all index info and compute ranks //////////////

		double const factor = 10000.0 / total_occurrences;

		for ( j = i; j < num_temp_files; ++j ) {
			if ( word[ j ] == words[ j ].end() )
				continue;
			if ( ::strcmp( *word[ i ], *word[ j ] ) )
				continue;

			file_list list( word[ j ] );
			FOR_EACH( file_list, list, file ) {
				o << file->index_;
				FOR_EACH( file_list::value_type::meta_set,
					file->meta_ids_, meta_id
				)
					o << '\2' << *meta_id;
				o << '\1' << rank(
					file->index_, file->occurrences_, factor
				) << '\1';
			}

			if ( j != i ) ++word[ j ];
		}
		o << '\0';

		++word[ i ];
	}

	////////// Copy remaining words from last non-exhausted index /////////

	for ( j = 0; j < num_temp_files; ++j ) {
		if ( word[ j ] == words[ j ].end() )
			continue;

		if ( stop_words->find( *word[ i ] ) )
			continue;

		////////// Determine total occurrences in all indicies ////////

		int total_occurrences = 0;
		file_list list( word[ j ] );
		FOR_EACH( file_list, list, file )
			total_occurrences += file->occurrences_;
		double const factor = 10000.0 / total_occurrences;

		////////// Copy all index info and compute ranks //////////////

		while ( word[ j ] != words[ j ].end() ) {

			word_offset[ word_index++ ] = o.tellp();
			o << *word[ j ] << '\0';

			file_list list( word[ j ] );
			FOR_EACH( file_list, list, file ) {
				o << file->index_;
				FOR_EACH( file_list::value_type::meta_set,
					file->meta_ids_, meta_id
				)
					o << '\2' << *meta_id;
				o << '\1' << rank(
					file->index_, file->occurrences_, factor
				) << '\1';
			}

			++word[ j ];
		}
		o << '\0';
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
		cout << endl;
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
		cout <<	"\nRanking index..." << flush;

	TRANSFORM_EACH( word_map, words, w ) {
		word_info &info = w->second;
		int const file_count = info.files_.size();

		//
		// Remove words that occur too frequently.
		//
		if ( file_count > word_file_max ) {
			if ( verbosity > 2 )
				cout	<< "\n  \"" << w->first
					<< "\" discarded (" << file_count
					<< " files)" << flush;
			goto remove;
		}

		{ // local scope so we can "goto" past initialization
		int const wfp = word_file_percentage( file_count );
		if ( wfp >= word_percent_max ) {
			if ( verbosity > 2 )
				cout	<< "\n  \"" << w->first
					<< "\" discarded (" << wfp
					<< "%)" << flush;
			goto remove;
		}
		}

		//
		// Compute the rank for this word in every file it's in.
		//
		{ // local scope so we can "goto" past initialization
		double const factor = 10000.0 / info.occurrences_;
		TRANSFORM_EACH( word_info::file_set, info.files_, file )
			file->rank_ = rank(
				file->index_, file->occurrences_, factor
			);
		continue;
		}

	remove:	stop_words->insert( ::strdup( w->first.c_str() ) );
		words.erase( w );
	}

	if ( verbosity > 1 )
		cout <<	endl;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void remove_temp_files()
//
// DESCRIPTION
//
//	Remove the temporary partial index files.  This function is called
//	via atexit(3).
//
// SEE ALSO
//
//	atexit(3)
//
//*****************************************************************************
{
	for ( int i = 0; i < num_temp_files; ++i ) {
		string const temp_file = temp_file_prefix + itoa( i );
		::unlink( temp_file.c_str() );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_file_index( ostream &o, off_t *offset )
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
	register int i = 0;
	FOR_EACH( file_info::set_type, file_info::set_, fi ) {
		offset[ i++ ] = o.tellp();
		o << **fi << '\0';
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_meta_name_index( ostream &o, off_t *offset )
//
// DESCRIPTION
//
//	Write the meta name index to the given ostream recording the offsets
//	as it goes.
//
// PARAMETERS
//
//	o	The ostream to write the index to.
//
//	offset	A pointer to a built-in vector where to record the offsets.
//
//*****************************************************************************
{
	register int i = 0;
	FOR_EACH( meta_map, meta_names, m ) {
		offset[ i++ ] = o.tellp();
		o << m->first << '\0' << m->second << '\0';
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
//	such a way so that it can be mmap'd and used instantly with no
//	parsing or other processing.  The format of an index file is:
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
//		word\0{data}...\0
//
//	that is: a null-terminated word followed by one or more data entries
//	followed by a null byte where a data entry is:
//
//		file_index[\2meta_id]\1rank\1
//
//	that is: a file-index followed by zero or more meta-IDs (each
//	preceeded by the ASCII 2 character) followed by a rank (both
//	preceeded and followed by the ASCII 1 character).  The integers are
//	in ASCII, not binary.  Currently, only ASCII characters 1 and 2 (the
//	binary values 1 and 2, not the digits 1 and 2 having ASCII codes of
//	49 and 50 decimal, respectively) are used.  (Other low ASCII
//	characters are reserved for future use.)  The file-index is an index
//	into the file_offset table; the meta-IDs, if present, are unique
//	integers identifying which meta name(s) a word is associated with in
//	the meta-name index.
//
//	The stop-word index is a list of all the stop words (either the
//	built-in list or the list supplied from a file, plus those that
//	exceed either the file limit or word percentage) in alphabetical
//	order.  Each entry is of the form:
//
//		word\0
//
//	where the word is terminated by a null character.
//
//	The file index is a list of file information.  Each entry is of the
//	form:
//
//		path_name file_size file_title\0
//
//	where the parts are separated by a single space and are teminated by
//	a null character.  (Any spaces after the second one are part of the
//	title.)  For example:
//
//		/path/my.html 1234 Some Title
//
//	Each file_offset points to the first character in a path name.
//
//	The meta-name index is a list of all the meta names (in alphabetical
//	order) encountered during indexing of all the files.  Each meta-name
//	is followed by its numeric ID that is simply a unique integer to
//	identify it.  Each entry is of the form:
//
//		meta-name\0ID\0
//
//	where both the meta-name and its numeric ID are terminated by a null
//	character.
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
	long const num_files = file_info::set_.size();
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
		cout << endl;
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
//	The partial word index is in the same format as the complete index
//	except that "rank" is "occurrences."
//
//*****************************************************************************
{
	string const temp_file = temp_file_prefix + itoa( num_temp_files++ );
	ofstream o( temp_file.c_str(), ios::out | ios::binary );
	if ( !o ) {
		ERROR	<< "can not write temp. file \""
			<< temp_file << '"' << endl;
		::exit( Exit_No_Write_Temp );
	}

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
		cout << '\n' << endl;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void write_stop_word_index( ostream &o, off_t *offset )
//
// DESCRIPTION
//
//	Write the stop-word index to the given ostream recording the offsets
//	as it goes.
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
	void write_word_index( ostream &o, off_t *offset )
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
		FOR_EACH( word_info::file_set, info.files_, file ) {
			o << file->index_;
			FOR_EACH( word_info::file::meta_set, file->meta_ids_, meta_id )
				o << '\2' << *meta_id;
			o << '\1' << file->occurrences_ << '\1';
		}

		o << '\0';
	}
}

//*****************************************************************************
//
//	Miscellaneous function(s)
//
//*****************************************************************************

void usage() {
	cerr <<	"usage: " << me << " [options] dir ... file ...\n"
	" options:\n"
	" --------\n"
	"  -c config_file  : Name of configuration file [default: " << ConfigFile_Default << "]\n"
	"  -C class_name   : Class name not to index [default: none]\n"
	"  -e ext          : Extension to index [default: none]\n"
	"  -E ext          : Extension not to index [default: none]\n"
	"  -f file_max     : Word/file maximum [default: infinity]\n"
	"  -F file_reserve : Reserve space for number of files [default: " << FilesReserve_Default << "]\n"
	"  -i index_file   : Name of index file to use [default: " << IndexFile_Default << "]\n"
#ifndef	PJL_NO_SYMBOLIC_LINKS
	"  -l              : Follow symbolic links [default: no]\n"
#endif
	"  -m meta_name    : Meta name to index [default: all]\n"
	"  -M meta_name    : Meta name not to index [default: none]\n"
	"  -p percent_max  : Word/file percentage [default: 100]\n"
	"  -r              : Do not recursively index subdirectories [default: do]\n"
	"  -s stop_file    : Stop-word file to use instead of compiled-in default\n"
	"  -S              : Dump default stop-words and exit\n"
	"  -t title_lines  : Lines to look for <TITLE> [default: " << TitleLines_Default << "]\n"
	"  -v verbosity    : Verbosity level [0-4; default: 0]\n"
	"  -V              : Print version number and exit\n";
	::exit( Exit_Usage );
}
