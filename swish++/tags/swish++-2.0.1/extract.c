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
#include <dirent.h>				/* for MAXNAMLEN */

// local
#include "config.h"
#include "directory.h"
#include "ext_proc.h"
#include "fake_ansi.h"
#include "file_vector.h"
#include "my_set.h"
#include "postscript.h"
#include "stop_words.h"
#include "util.h"
#include "version.h"

extern "C" {
	extern char*	optarg;
	extern int	optind, opterr;
}

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

string_set	include_extensions;		// file extensions to index
string_set	exclude_extensions;		// file extensions not to index
bool		in_postscript;
char const*	me;				// executable name
int		num_files;
bool		recurse_subdirectories = true;
int		verbosity;			// how much to print

void		do_file( char const *path );
bool		extract_word( char *word, int len, ofstream& );
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
//		and directories to extract.
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

	char const *stop_word_file_name = 0;

	::opterr = 1;
	char const opts[] = "e:E:lrs:v:V";
	for ( int opt; (opt = ::getopt( argc, argv, opts )) != EOF; )
		switch ( opt ) {

			case 'e': // Specify filename extension to extract.
				include_extensions.insert( ::optarg );
				break;

			case 'E': // Specify filename extension not to extract.
				exclude_extensions.insert( ::optarg );
				break;

			case 'l': // Follow symbolic links during extraction.
				follow_symbolic_links = true;
				break;

			case 'r': // Specify whether to extract recursively.
				recurse_subdirectories = false;
				break;

			case 's': // Specify stop-word list.
				stop_word_file_name = ::optarg;
				break;

			case 'v': // Specify verbosity level.
				verbosity = ::atoi( ::optarg );
				if ( verbosity < 0 )
					verbosity = 0;
				else if ( verbosity > 4 )
					verbosity = 4;
				break;

			case 'V': // Display version and exit.
				cout << "SWISH++ " << version << endl;
				::exit( 0 );

			case '?': // Bad option.
				usage();
		}
	argc -= ::optind, argv += ::optind;

	if ( !argc )
		usage();

	bool const using_stdin = *argv && (*argv)[0] == '-' && !(*argv)[1];
	if ( !using_stdin &&
		include_extensions.empty() && exclude_extensions.empty()
	) {
		cerr << me << ": extensions must be specified "
			"when not using standard input " << endl;
		usage();
	}

	stop_words = new stop_word_set( stop_word_file_name );

	////////// Extract text from specified files //////////////////////////

	time_t time = ::time( 0 );		// Go!

	if ( using_stdin ) {
		//
		// Read file/directory names from standard input.
		//
		char file_name[ MAXNAMLEN + 1 ];
		while ( cin.getline( file_name, MAXNAMLEN ) )
			if ( is_directory( file_name ) )
				do_directory( file_name );
			else
				do_file( file_name );
	} else {
		//
		// Read file/directory names from command line.
		//
		while ( *argv ) {
			if ( is_directory( *argv ) )
				do_directory( *argv );
			else
				do_file( *argv );
			++argv;
		}
	}

	if ( verbosity ) { 
		time = ::time( 0 ) - time;	// Stop!
		cout	<< setfill('0')
			<< "\nExtraction done:\n  "
			<< setw(2) << (time / 60) << ':'
			<< setw(2) << (time % 60)
			<< " elapsed time\n  "
			<< num_files << " files\n\n"
			<< setfill(' ');
	}

	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void do_file( char const *file_name )
//
// DESCRIPTION
//
//	Extract the text.  This algorithm is loosely based on what the Unix
//	strings(1) command does except it goes a bit further to discard
//	things like Encapsulated PostScript and raw hex data.
//
// PARAMETERS
//
//	file_name	The file to extract text from.
//
//*****************************************************************************
{
	char const *const slash = ::strrchr( file_name, '/' );
	char const *const base_name = slash ? slash + 1 : file_name;

	if ( verbosity > 3 )			// print base name of file
		cout << "  " << base_name << flush;

	////////// Determine if we should process the file ////////////////////

	if ( !is_plain_file() ) {
		if ( verbosity > 3 )
			cout << " (skipped: not plain file)" << endl;
		return;
	}
	if ( is_symbolic_link( file_name ) && !follow_symbolic_links ) {
		if ( verbosity > 3 )
			cout << " (skipped: symbolic link)" << endl;
		return;
	}

	//
	// Check to see if the file name has an extension by looking for a '.'
	// in it and making sure that it is not the last character.  If the
	// extension contains a '/', then it's really not an extension; rather,
	// it's a file name like: "/a.bizarre/file".
	//
	char const *ext = ::strrchr( file_name, '.' );
	if ( !ext || !*++ext || ::strchr( ext, '/' ) ) {
		if ( verbosity > 3 )
			cout << " (skipped: no extension)" << endl;
		return;
	}

	//
	// Determine if the file needs to be preprocessed first.
	//
	static ext_proc_map const ext_procs;
	ext_proc_map::value_type const *const proc = ext_procs[ ext ];

	if ( proc ) {
		static char ext_buf[ 10 ];
		//
		// Get the "real" filename extension, e.g., get "txt" out of
		// "foo.txt.gz".
		//
		register char const *p;
		for ( p = ext - 2; p > file_name; --p )
			if ( *p == '.' ) {
				::copy( p + 1, ext - 1, ext_buf );
				ext_buf[ ext - p - 2 ] = '\0';
				break;
			}
		if ( *p != '.' ) {
			//
			// File doesn't have a "real" extension, e.g., it's
			// something like "bar.gz".
			//
			if ( verbosity > 3 )
				cout << " (skipped: no real extension)" << endl;
			return;
		}
		static string fixed_file_name;
		fixed_file_name = string( file_name, ext - 1 );
		file_name = fixed_file_name.c_str();
		ext = ext_buf;
	}

	//
	// Skip the file if either the set of unacceptable extensions contains
	// the candidate or the set of acceptable extensions doesn't.
	//
	if ( exclude_extensions.find( ext ) ) {
		if ( verbosity > 3 )
			cout << " (skipped: extension excluded)" << endl;
		return;
	}
	if ( !include_extensions.empty() && !include_extensions.find( ext ) ) {
		if ( verbosity > 3 )
			cout << " (skipped: extension not included)" << endl;
		return;
	}

	//
	// Check to see if the .txt file already exists; if so, skip it.
	//
	string const file_name_txt = string( file_name ) + ".txt";
	if ( ::stat( file_name_txt.c_str(), &stat_buf ) != -1 ) {
		if ( verbosity > 3 )
			cout << " (skipped: .txt file already exists)" << endl;
		return;
	}
	ofstream txt( file_name_txt.c_str() );
	if ( !txt ) {
		if ( verbosity > 3 )
			cout << " (skipped: can not create .txt file)" << endl;
		return;
	}

	if ( proc && !process_file( proc->undo, file_name ) ) {
		if ( verbosity > 3 )
			cout	<< " (skipped: could not " << proc->undo
				<< " file)" << endl;
		return;
	}

	file_vector<char> file( file_name );	// try to open the file
	if ( !file ) {
		if ( verbosity > 3 )
			cout << " (skipped: can not open)" << endl;
		return;
	}

	if ( verbosity == 3 )			// print base name of file
		cout << "  " << base_name << flush;

	////////// Parse the file /////////////////////////////////////////////

	++num_files;

	char buf[ Word_Hard_Max_Size + 1 ];
	register char *word;
	int len;
	int num_words = 0;
	bool in_word = false;
	in_postscript = false;

	register file_vector<char>::const_iterator c = file.begin();
	while ( c != file.end() ) {
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
			while ( c != file.end() && is_word_char( *c++ ) ) ;
			continue;
		}

		if ( in_word ) {
			in_word = false;
			num_words += extract_word( word, len, txt );
		}
	}
	if ( in_word )
		num_words += extract_word( word, len, txt );

	if ( verbosity > 2 )
		cout << " (" << num_words << " words)" << endl;

	if ( proc )			// restore file to the way it was
		process_file( proc->redo, file_name );
}

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
	if ( postscript_comments.find( word ) ) {
		in_postscript = true;
		return false;
	}
	static postscript_operator_set const postscript_operators;
	if ( postscript_operators.find( word ) )
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

	if ( !is_ok_word( word ) || stop_words->find( to_lower( word ) ) )
		return false;

	out << word << '\n';
	return true;
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
	"  -e ext          : Extension to extract [default: none]\n"
	"  -E ext          : Extension not to extract [default: none]\n"
	"  -l              : Follow symbolic links [default: no]\n"
	"  -r              : Do not recursively extract subdirectories [default: false]\n"
	"  -s stop_file    : Stop-word file to use instead of compiled-in default\n"
	"  -v verbosity    : Verbosity level [0-4; default: 0]\n"
	"  -V              : Print version number and exit\n";
	::exit( 1 );
}
