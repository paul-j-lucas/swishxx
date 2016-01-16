/*
**      SWISH++
**      src/extract.cpp
**
**      Copyright (C) 1998-2015  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "config.h"
#include "ExcludeFile.h"
#include "exit_codes.h"
#include "ExtractExtension.h"
#include "ExtractFile.h"
#include "ExtractFilter.h"
#include "FilterFile.h"
#ifndef PJL_NO_SYMBOLIC_LINKS
#include "FollowLinks.h"
#endif
#include "pjl/mmap_file.h"
#include "pjl/option_stream.h"
#include "postscript.h"
#include "RecurseSubdirs.h"
#include "StopWordFile.h"
#include "stop_words.h"
#include "util.h"
#include "Verbosity.h"
#include "word_util.h"

// standard
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <fstream>
#include <iomanip>                      /* for setfill(), setw() */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <time.h>

using namespace PJL;
using namespace std;

ExcludeFile       exclude_patterns;     // do not extract these
ExtractFile       include_patterns;     // do extract these
ExtractFilter     extract_as_filter;
ExtractExtension  extract_extension;
FilterFile        file_filters;
static bool       in_postscript;
char const*       me;                   // executable name
int               num_examined_files;
int               num_extracted_files;
RecurseSubdirs    recurse_subdirectories;
Verbosity         verbosity;            // how much to print

////////// local functions ////////////////////////////////////////////////////

static bool extract_word( char *word, int len, ostream& );
static void extract_words( mmap_file::const_iterator begin,
                           mmap_file::const_iterator end, ostream& );
static ostream& usage( ostream& = cerr );

#define SWISHXX_EXTRACT
#include "do_file.cpp"
#include "directory.cpp"

////////// main ///////////////////////////////////////////////////////////////

int main( int argc, char *argv[] ) {
  me = ::strrchr( argv[0], '/' );       // determine base name...
  me = me ? me + 1 : argv[0];           // ...of executable

#ifdef RLIMIT_CPU                       /* SVR4, 4.2+BSD */
  //
  // Max-out the amount of CPU time we can run since extraction can take a
  // while.
  //
  max_out_limit( RLIMIT_CPU );
#endif /* RLIMIT_CPU */

  /////////// Process command-line options ////////////////////////////////////

  static option_stream::spec const opt_spec[] = {
    { "help",         0, '?' },
    { "config",       1, 'c' },
    { "pattern",      1, 'e' },
    { "no-pattern",   1, 'E' },
    { "filter",       0, 'f' },
#ifndef PJL_NO_SYMBOLIC_LINKS
    { "follow-links", 0, 'l' },
#endif
    { "no-recurse",   0, 'r' },
    { "stop-file",    1, 's' },
    { "dump-stop",    0, 'S' },
    { "verbose",      1, 'v' },
    { "version",      0, 'V' },
    { "extension",    1, 'x' },

    { nullptr, 0, '\0' }
  };

  char const   *config_file_name_arg = ConfigFile_Default;
  bool          dump_stop_words_opt = false;
  bool          extract_as_filter_opt = false;
  char const   *extract_extension_arg = nullptr;
#ifndef PJL_NO_SYMBOLIC_LINKS
  bool          follow_symbolic_links_opt = false;
#endif
  bool          recurse_subdirectories_opt = false;
  StopWordFile  stop_word_file_name;
  char const   *stop_word_file_name_arg = nullptr;
  char const   *verbosity_arg = nullptr;

  option_stream opt_in( argc, argv, opt_spec );
  for ( option_stream::option opt; opt_in >> opt; ) {
    switch ( opt ) {

      case '?': // Print help.
        cerr << usage;

      case 'c': // Specify config. file.
        config_file_name_arg = opt.arg();
        break;

      case 'e': { // Filename pattern(s) to extract.
        char *a = opt.arg();
        for ( char *pat; (pat = ::strtok( a, "," )); ) {
          include_patterns.insert( pat, 0 );
          a = nullptr;
        } // for
        break;
      }

      case 'E': { // Filename pattern(s) not to extract.
        char *a = opt.arg();
        for ( char *pat; (pat = ::strtok( a, "," )); ) {
          exclude_patterns.insert( pat );
          a = nullptr;
        } // for
        break;
      }

      case 'f': // Run as a filter.
        extract_as_filter_opt = true;
        break;

#ifndef PJL_NO_SYMBOLIC_LINKS
      case 'l': // Follow symbolic links during extraction.
        follow_symbolic_links_opt = true;
        break;
#endif

      case 'r': // Specify whether to extract recursively.
        recurse_subdirectories_opt = true;
        break;

      case 's': // Specify stop-word list.
        stop_word_file_name_arg = opt.arg();
        break;

      case 'S': // Dump stop-word list.
        dump_stop_words_opt = true;
        break;

      case 'v': // Specify verbosity level.
        verbosity_arg = opt.arg();
        break;

      case 'V': // Display version and exit.
        cout << PACKAGE_STRING << endl;
        ::exit( Exit_Success );

      case 'x': // Specify filename extension to append.
        extract_extension_arg = opt.arg();
        break;

      default: // Bad option.
        cerr << usage;
    } // switch
  } // for
  argc -= opt_in.shift(), argv += opt_in.shift();

  //
  // First, parse the config. file (if any); then override variables specified
  // on the command line with options.
  //
  conf_var::parse_file( config_file_name_arg );

  if ( extract_as_filter_opt )
    extract_as_filter = true;
  if ( extract_as_filter ) {
    //
    // When running as a filter, patterns aren't used.  We clear them here in
    // case some were set via an IncludeFile directive in a configuration file.
    // That let's us get away with not having to special-case the code in
    // do_file().
    //
    exclude_patterns.clear();
    include_patterns.clear();
  }
  if ( extract_extension_arg )
    extract_extension = extract_extension_arg;
  if ( *extract_extension != '.' )      // prepend '.' if needed
    extract_extension = string( "." ) + (char const*)extract_extension;
#ifndef PJL_NO_SYMBOLIC_LINKS
  if ( follow_symbolic_links_opt )
    follow_symbolic_links = true;
#endif
  if ( recurse_subdirectories_opt )
    recurse_subdirectories = false;
  if ( stop_word_file_name_arg )
    stop_word_file_name = stop_word_file_name_arg;
  if ( verbosity_arg )
    verbosity = verbosity_arg;

  /////////// Deal with stop-words ////////////////////////////////////////////

  stop_words = new stop_word_set( stop_word_file_name );
  if ( dump_stop_words_opt ) {
    ::copy( stop_words->begin(), stop_words->end(),
      ostream_iterator<char const*>( cout, "\n" )
    );
    ::exit( Exit_Success );
  }

  /////////// Extract specified directories and files ///////////////////////

  bool const using_stdin = *argv && (*argv)[0] == '-' && !(*argv)[1];
  if ( !( extract_as_filter || using_stdin ) &&
       include_patterns.empty() && exclude_patterns.empty() ) {
    error() << "filename patterns must be specified when not"
               " a filter nor\nusing standard input\n" << usage;
  }

  if ( !argc )
    cerr << usage;

  ////////// Extract text from specified files ////////////////////////////////

  time_t time = ::time( 0 );            // Go!

  if ( extract_as_filter ) {
    //
    // Do a single file.
    //
    if ( !file_exists( *argv ) ) {
      error() << *argv << " does not exist\n";
      ::exit( Exit_No_Such_File );
    }
    do_file( *argv );
  } else if ( using_stdin ) {
    //
    // Read file/directory names from standard input.
    //
    char file_name[ PATH_MAX + 1 ];
    while ( cin.getline( file_name, PATH_MAX ) ) {
      if ( !file_exists( file_name ) ) {
        if ( verbosity > 3 )
          cout << "  " << file_name << " (skipped: does not exist)\n";
        continue;
      }
      if ( is_directory() )
        do_directory( new_strdup( file_name ) );
      else
        do_file( file_name );
    } // while
  } else {
    //
    // Read file/directory names from command line.
    //
    for ( ; *argv; ++argv ) {
      if ( !file_exists( *argv ) ) {
        if ( verbosity > 3 )
          cout << "  " << *argv << " (skipped: does not exist)\n";
        continue;
      }
      if ( is_directory() )
        do_directory( *argv );
      else
        do_file( *argv );
    } // for
  }

  if ( verbosity ) {
    time = ::time( 0 ) - time;          // Stop!
    cout << '\n' << me << ": done:\n  "
         << setfill('0')
         << setw(2) << (time / 60) << ':'
         << setw(2) << (time % 60)
         << " (min:sec) elapsed time\n  "
         << num_examined_files << " files, "
         << num_extracted_files << " extracted\n\n";
  }

  ::exit( Exit_Success );
}

/**
 * Potentially extracts the given word.
 *
 * @param word The candidate word to be extracted.
 * @param len The length of the word since it is not null-terminated.
 * @param out The ostream to write the word to.
 * @return Returns true only if the word was extracted.
 */
static bool extract_word( char *word, int len, ostream &out ) {
  if ( len < Word_Hard_Min_Size )
    return false;

  word[ len ] = '\0';

  ////////// Look for Encapsulated PostScript code and skip it ////////////////

  if ( in_postscript ) {
    if ( !::strcmp( word, "%%Trailer" ) )
      in_postscript = false;
    return false;
  }
  static auto const &postscript_comments = postscript::comments();
  if ( contains( postscript_comments, word ) ) {
    in_postscript = true;
    return false;
  }
  static auto const &postscript_operators = postscript::operators();
  if ( contains( postscript_operators, word ) )
    return false;

  ////////// Strip chars not in Word_Begin_Chars/Word_End_Chars ///////////////

  for ( int i = len - 1; i >= 0; --i ) {
    if ( is_word_end_char( word[ i ] ) )
      break;
    --len;
  } // for
  if ( len < Word_Hard_Min_Size )
    return false;

  word[ len ] = '\0';

  while ( *word ) {
    if ( is_word_begin_char( *word ) || *word == '%' )
      break;
    --len, ++word;
  } // while
  if ( len < Word_Hard_Min_Size )
    return false;

  ////////// Discard what looks like ASCII hex data ///////////////////////////

  if ( len > Word_Hex_Max_Size &&
       (int)::strspn( word, "0123456789abcdefABCDEF" ) == len ) {
    return false;
  }

  ////////// Stop-word checks /////////////////////////////////////////////////

  if ( !is_ok_word( word ) || contains( *stop_words, to_lower( word ) ) )
    return false;

  out << word << '\n';
  return true;
}

/**
 * Extracts the words between the given iterators.
 *
 * @param c The iterator marking the beginning of the text to extract.
 * @param end The iterator marking the end of the text to extract.
 * @param out The ostream to write the words to.
 */
static void extract_words( mmap_file::const_iterator c,
                           mmap_file::const_iterator end, ostream &out ) {
  char  word[ Word_Hard_Max_Size + 1 ];
  int   len;
  int   num_words = 0;
  bool  in_word = false;

  in_postscript = false;

  while ( c != end ) {
    char const ch = *c++;

    ////////// Collect a word /////////////////////////////////////////////////

    if ( is_word_char( ch ) || ch == '%' ) {
      if ( !in_word ) {                 // start a new word
        word[ 0 ] = ch;
        len = 1;
        in_word = true;
        continue;
      }
      if ( len < Word_Hard_Max_Size ) { // continue same word
        word[ len++ ] = ch;
        continue;
      }
      in_word = false;                  // too big: skip chars
      while ( c != end && is_word_char( *c++ ) ) ;
      continue;
    }

    if ( in_word ) {
      //
      // We ran into a non-word character, so extract the word up to, but not
      // including, it.
      //
      in_word = false;
      num_words += extract_word( word, len, out );
    }
  } // while
  if ( in_word ) {
    //
    // We ran into 'end' while still accumulating characters into a word, so
    // just extract what we've got.
    //
    num_words += extract_word( word, len, out );
  }

  if ( verbosity > 2 )
    cout << " (" << num_words << " words)" << endl;
}

ostream& usage( ostream &o ) {
  o << "usage: " << me << " [options] dir ... file ...\n"
  "options: (unambiguous abbreviations may be used for long options)\n"
  "========\n"
  "-?   | --help          : Print this help message\n"
  "-c f | --config-file f : Name of configuration file [default: " << ConfigFile_Default << "]\n"
  "-e p | --pattern p     : Filename pattern to extract [default: none]\n"
  "-E p | --no-pattern p  : Filename pattern not to extract [default: none]\n"
  "-f   | --filter        : Filter one file to standard output [default: no]\n"
#ifndef PJL_NO_SYMBOLIC_LINKS
  "-l   | --follow-links  : Follow symbolic links [default: no]\n"
#endif
  "-r   | --no-recurse    : Don't extract subdirectories [default: do]\n"
  "-s f | --stop-file f   : Stop-word file to use instead of built-in default\n"
  "-S   | --dump-stop     : Dump stop-words, exit\n"
  "-v v | --verbosity v   : Verbosity level [0-4; default: 0]\n"
  "-V   | --version       : Print version number, exit\n"
  "-x e | --extension e   : Extension to append to filename [default: txt]\n";
  ::exit( Exit_Usage );
  return o;                             // just to make compiler happy
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
