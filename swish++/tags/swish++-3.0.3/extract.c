/*
**	SWISH++
**	extract.c
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
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef	WIN32
int const	MAXNAMLEN = 255;
#else
#include <dirent.h>				/* for MAXNAMLEN */
#endif

// local
#include "config.h"
#include "directory.h"
#include "ExcludeExtension.h"
#include "exit_codes.h"
#include "file_vector.h"
#include "FilterExtension.h"
#include "IncludeExtension.h"
#include "platform.h"
#include "postscript.h"
#include "RecurseSubdirs.h"
#include "StopWordFile.h"
#include "stop_words.h"
#include "util.h"
#include "Verbosity.h"
#include "version.h"

extern "C" {
	extern char*	optarg;
	extern int	optind, opterr;
}

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

ExcludeExtension exclude_extensions;		// do not extract these
IncludeExtension include_extensions;		// do extract these
FilterExtension	filters;
bool		in_postscript;
char const*	me;				// executable name
int		num_examined_files;
int		num_extracted_files;
RecurseSubdirs	recurse_subdirectories;
Verbosity	verbosity;			// how much to print

void		do_file( char const *path );
bool		extract_word( char *word, int len, ofstream& );
void		extract_words(
			file_vector<char>::const_iterator begin,
			file_vector<char>::const_iterator end,
			ofstream&
		);
void		usage();

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
//		and directories to be extracted.
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
	bool		recurse_subdirectories_opt = false;
	StopWordFile	stop_word_file_name;
	char const*	stop_word_file_name_arg = 0;
	char const*	verbosity_arg = 0;

	char const opts[] =
#ifndef	PJL_NO_SYMBOLIC_LINKS
		"l"
#endif
		"c:e:E:rs:Sv:V";

	::opterr = 1;
	for ( int opt; (opt = ::getopt( argc, argv, opts )) != EOF; )
		switch ( opt ) {

			case 'c': // Specify config. file.
				config_file_name_arg = ::optarg;
				break;

			case 'e': // Specify filename extension to extract.
				include_extensions.insert( ::optarg );
				break;

			case 'E': // Specify filename extension not to extract.
				exclude_extensions.insert( ::optarg );
				break;
#ifndef	PJL_NO_SYMBOLIC_LINKS
			case 'l': // Follow symbolic links during extraction.
				follow_symbolic_links_opt = true;
				break;
#endif
			case 'r': // Specify whether to extract recursively.
				recurse_subdirectories_opt = true;
				break;

			case 's': // Specify stop-word list.
				stop_word_file_name_arg = ::optarg;
				break;

			case 'S': // Dump stop-word list.
				dump_stop_words_opt = true;
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
	if ( recurse_subdirectories_opt )
		recurse_subdirectories = false;
	if ( stop_word_file_name_arg )
		stop_word_file_name = stop_word_file_name_arg;
	if ( verbosity_arg )
		verbosity = verbosity_arg;

	/////////// Deal with stop-words //////////////////////////////////////

	stop_words = new stop_word_set( stop_word_file_name );
	if ( dump_stop_words_opt ) {
		::copy( stop_words->begin(), stop_words->end(),
			ostream_iterator< char const* >( cout, "\n" )
		);
		::exit( Exit_Success );
	}

	/////////// Extract specified directories and files ///////////////////

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

	////////// Extract text from specified files //////////////////////////

	time_t time = ::time( 0 );		// Go!

	if ( using_stdin ) {
		//
		// Read file/directory names from standard input.
		//
		char file_name[ MAXNAMLEN + 1 ];
		while ( cin.getline( file_name, MAXNAMLEN ) ) {
			if ( !file_exists( *argv ) ) {
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

	if ( verbosity ) { 
		time = ::time( 0 ) - time;	// Stop!
		cout	<< '\n' << me << ": done:\n  "
			<< setfill('0')
			<< setw(2) << (time / 60) << ':'
			<< setw(2) << (time % 60)
			<< " (min:sec) elapsed time\n  "
			<< num_examined_files << " files, "
			<< num_extracted_files << " extracted\n\n"
			<< setfill(' ');
	}

	::exit( Exit_Success );
}

#define	EXTRACT
#include "do_file.c"

//*****************************************************************************
//
// SYNOPSIS
//
	bool extract_word(
		register char *word, register int len, ofstream &out
	)
//
// DESCRIPTION
//
//	Potentially extract the given word.
//
// PARAMETERS
//
//	word	The candidate word to be extracted.
//
//	len	The length of the word since it is not null-terminated.
//
// RETURN VALUE
//
//	Returns true only if the word was extracted.
//
//*****************************************************************************
{
	if ( len < Word_Hard_Min_Size )
		return false;

	word[ len ] = '\0';

	////////// Look for Encapsulated PostScript code and skip it //////////

	if ( in_postscript ) {
		if ( !::strcmp( word, "%%Trailer" ) )
			in_postscript = false;
		return false;
	}
	static postscript_comment_set const postscript_comments;
	if ( postscript_comments.contains( word ) ) {
		in_postscript = true;
		return false;
	}
	static postscript_operator_set const postscript_operators;
	if ( postscript_operators.contains( word ) )
		return false;

	////////// Strip chars not in Word_Begin_Chars/Word_End_Chars /////////

	for ( register int i = len - 1; i >= 0; --i ) {
		if ( is_word_end_char( word[ i ] ) )
			break;
		--len;
	}
	if ( len < Word_Hard_Min_Size )
		return false;

	word[ len ] = '\0';

	while ( *word ) {
		if ( is_word_begin_char( *word ) || *word == '%' )
			break;
		--len, ++word;
	}
	if ( len < Word_Hard_Min_Size )
		return false;

	////////// Discard what looks like ASCII hex data /////////////////////

	if ( len > Word_Hex_Max_Size &&
		::strspn( word, "0123456789abcdefABCDEF" ) == len
	)
		return false;

	////////// Stop-word checks ///////////////////////////////////////////

	if ( !is_ok_word( word ) || stop_words->contains( to_lower( word ) ) )
		return false;

	out << word << '\n';
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void extract_words(
		register file_vector<char>::const_iterator c,
		register file_vector<char>::const_iterator end,
		ofstream &out
	)
//
// DESCRIPTION
//
//	Extract the words between the given iterators.
//
// PARAMETERS
//
//	c	The iterator marking the beginning of the text to extract.
//
//	end	The iterator marking the end of the text to extract.
//
//*****************************************************************************
{
	char buf[ Word_Hard_Max_Size + 1 ];
	register char *word;
	int len;
	int num_words = 0;
	bool in_word = false;
	in_postscript = false;

	while ( c != end ) {
		register char ch = *c++;

		////////// Collect a word /////////////////////////////////////

		if ( is_word_char( ch ) || ch == '%' ) {
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
			num_words += extract_word( word, len, out );
		}
	}
	if ( in_word )
		num_words += extract_word( word, len, out );

	if ( verbosity > 2 )
		cout << " (" << num_words << " words)" << endl;
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
	"  -e ext          : Extension to extract [default: none]\n"
	"  -E ext          : Extension not to extract [default: none]\n"
#ifndef	PJL_NO_SYMBOLIC_LINKS
	"  -l              : Follow symbolic links [default: no]\n"
#endif
	"  -r              : Do not recursively extract subdirectories [default: do]\n"
	"  -s stop_file    : Stop-word file to use instead of compiled-in default\n"
	"  -S              : Dump stop-words and exit\n"
	"  -v verbosity    : Verbosity level [0-4; default: 0]\n"
	"  -V              : Print version number and exit\n";
	::exit( Exit_Usage );
}
