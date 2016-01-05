/*
**      SWISH++
**      src/index.cpp
**
**      Copyright (C) 1998  Paul J. Lucas
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
#include "AssociateMeta.h"
#include "ChangeDirectory.h"
#include "enc_int.h"
#include "ExcludeFile.h"
#include "ExcludeMeta.h"
#include "exit_codes.h"
#include "file_info.h"
#include "file_list.h"
#include "FilesGrow.h"
#include "FilesReserve.h"
#include "FilterFile.h"
#ifndef PJL_NO_SYMBOLIC_LINKS
#include "FollowLinks.h"
#endif
#include "IncludeFile.h"
#include "IncludeMeta.h"
#include "Incremental.h"
#include "indexer.h"
#include "IndexFile.h"
#include "index_segment.h"
#include "meta_map.h"
#include "pjl/itoa.h"
#include "pjl/mmap_file.h"
#include "pjl/option_stream.h"
#include "RecurseSubdirs.h"
#include "StopWordFile.h"
#include "stop_words.h"
#ifdef WITH_WORD_POS
#include "StoreWordPositions.h"
#endif /* WITH_WORD_POS */
#include "TempDirectory.h"
#include "TitleLines.h"
#include "util.h"
#include "Verbosity.h"
#include "WordFilesMax.h"
#include "word_info.h"
#include "word_markers.h"
#include "WordPercentMax.h"
#include "WordThreshold.h"
#include "word_util.h"

// standard
#include <cmath>                        /* for log(3) */
#include <cstdlib>                      /* for getenv(3), exit(3) */
#include <cstring>
#include <fstream>
#include <iomanip>                      /* for setfill(), setw() */
#include <iostream>
#include <iterator>
#include <memory>                       /* for unique_ptr */
#include <string>
#include <time.h>
#include <sys/time.h>                   /* needed by FreeBSD systems */
#include <sys/resource.h>               /* for RLIMIT_* */
#include <sys/types.h>
#include <unistd.h>                     /* for unlink(2) */
#include <vector>

using namespace PJL;
using namespace std;

/**
 * A scaling factor used in rank calculation.  Empirically, this was found to
 * be a good number: higher, the results don't get better; lower, the results
 * get more striated.
 */
static long const Rank_Factor = 10000000;

AssociateMeta         associate_meta;
ExcludeFile           exclude_patterns;   // do not index these
IncludeFile           include_patterns;   // do index these
ExcludeMeta           exclude_meta_names; // meta names not to index
IncludeMeta           include_meta_names; // meta names to index
FilesGrow             files_grow;
FilterFile            file_filters;
Incremental           incremental;
char const*           me;                 // executable name
meta_map              meta_names;
static int            num_examined_files;
static int            num_temp_files;
TitleLines            num_title_lines;
unsigned long         num_total_words;    // over all files indexed
unsigned long         num_indexed_words;  // over all files indexed
static unsigned long  num_unique_words;   // over all files indexed
static vector<string> partial_index_file_names;
RecurseSubdirs        recurse_subdirectories;
string                temp_file_name_prefix;
Verbosity             verbosity;          // how much to print
word_map              words;              // the index being generated
WordFilesMax          word_files_max;
WordPercentMax        word_percent_max;
WordThreshold         word_threshold;

#ifdef WITH_WORD_POS
StoreWordPositions    store_word_positions;
int                   word_pos;           // ith word in file
#endif /* WITH_WORD_POS */

// local functions
static void           load_old_index( char const *index_file_name );
static void           merge_indicies( ostream& );
static void           rank_full_index();
extern "C" void       remove_temp_files( void );
static ostream&       usage( ostream& = cerr );
static void           write_dir_index( ostream&, off_t* );
static void           write_file_index( ostream&, off_t* );
static void           write_full_index( ostream& );
static void           write_meta_name_index( ostream&, off_t* );
static void           write_partial_index();
static void           write_stop_word_index( ostream&, off_t* );
static void           write_word_index( ostream&, off_t* );

#define SWISHXX_INDEX
#include "do_file.cpp"
#include "directory.cpp"

////////// inline functions ///////////////////////////////////////////////////

/**
 * In the latest g++ implementation of the ANSI C++ standard library,
 * \c ostream::write() now apparantly requires a \c char* rather than a
 * \c void*.  This function is to do the case in one place because I'm lazy.
 *
 * @param o The ostream to write to.
 * @param buf The buffer to be written.
 * @param len The number of bytes in \a buf.
 */
inline void my_write( ostream &o, void const *buf, size_t len ) {
  o.write( reinterpret_cast<char const*>( buf ), len );
  assert_stream( o );
}

////////// main ///////////////////////////////////////////////////////////////

int main( int argc, char *argv[] ) {
  me = ::strrchr( argv[0], '/' );       // determine base name...
  me = me ? me + 1 : argv[0];           // ...of executable

  ////////// Max-out various system resources ///////////////////////////////

#ifdef RLIMIT_AS                        /* SVR4 */
  //
  // Max-out out out total memory potential.
  //
  max_out_limit( RLIMIT_AS );
#endif /* RLIMIT_AS */
#ifdef RLIMIT_CPU                       /* SVR4, 4.3+BSD */
  //
  // Max-out the amount of CPU time we can run since indexing can take a
  // while.
  //
  max_out_limit( RLIMIT_CPU );
#endif /* RLIMIT_CPU */
#ifdef RLIMIT_DATA                      /* SVR4, 4.3+BSD */
  //
  // Max-out our heap allocation potential.
  //
  max_out_limit( RLIMIT_DATA );
#endif /* RLIMIT_DATA */
#ifdef RLIMIT_FSIZE
  //
  // Max-out the file-size creation potential.
  //
  max_out_limit( RLIMIT_FSIZE );
#endif /* RLIMIT_FSIZE */
#ifdef RLIMIT_NOFILE                    /* SVR4 */
  //
  // Max-out the number of file descriptors we can have open to be able to
  // merge as many partial indicies as possible.
  //
  max_out_limit( RLIMIT_NOFILE );
#elif defined( RLIMIT_OFILE )           /* 4.3+BSD name for NOFILE */
  max_out_limit( RLIMIT_OFILE );
#endif /* RLIMIT_OFILE */

  /////////// Process command-line options ////////////////////////////////////

  static option_stream::spec const opt_spec[] = {
    { "help",           0, '?' },
    { "no-assoc-meta",  0, 'A' },
    { "config-file",    1, 'c' },
    { "chdir",          1, 'd' },
    { "pattern",        1, 'e' },
    { "no-pattern",     1, 'E' },
    { "file-max",       1, 'f' },
    { "files-reserve",  1, 'F' },
    { "files-grow",     1, 'g' },
    { "index-file",     1, 'i' },
    { "incremental",    0, 'I' },
#ifndef PJL_NO_SYMBOLIC_LINKS
    { "follow-links",   0, 'l' },
#endif
    { "meta",           1, 'm' },
    { "no-meta",        1, 'M' },
    { "percent-max",    1, 'p' },
#ifdef WITH_WORD_POS
    { "no-pos-data",    0, 'P' },
#endif /* WITH_WORD_POS */
    { "no-recurse",     0, 'r' },
    { "stop-file",      1, 's' },
    { "dump-stop",      0, 'S' },
    { "title-lines",    1, 't' },
    { "temp-dir",       1, 'T' },
    { "verbosity",      1, 'v' },
    { "version",        0, 'V' },
    { "word-threshold", 1, 'W' },

    { nullptr, 0, '\0' }
  };

  ChangeDirectory change_directory;
  char const     *change_directory_arg = nullptr;
  char const     *config_file_name_arg = ConfigFile_Default;
  bool            dump_stop_words_opt = false;
  char const     *files_grow_arg = nullptr;
  char const     *files_reserve_arg = nullptr;
#ifndef PJL_NO_SYMBOLIC_LINKS
  bool            follow_symbolic_links_opt = false;
#endif
  bool            incremental_opt = false;
  IndexFile       index_file_name;
  char const     *index_file_name_arg = nullptr;
  bool            no_associate_meta_opt = false;
  bool            no_word_pos_opt = false;
  char const     *num_title_lines_arg = nullptr;
  bool            print_version = false;
  bool            recurse_subdirectories_opt = false;
  StopWordFile    stop_word_file_name;
  char const     *stop_word_file_name_arg = nullptr;
  TempDirectory   temp_directory;
  char const     *temp_directory_arg = nullptr;
  char const     *verbosity_arg = nullptr;
  char const     *word_files_max_arg = nullptr;
  char const     *word_percent_max_arg = nullptr;
  char const     *word_threshold_arg = nullptr;

  auto const all_options( indexer::all_mods_options( opt_spec ) );
  option_stream opt_in( argc, argv, all_options.get() );

  for ( option_stream::option opt; opt_in >> opt; ) {
    switch ( opt ) {

      case '?': // Print help.
        cerr << usage;

      case 'A': // Don't associate meta names.
        no_associate_meta_opt = true;
        break;

      case 'c': // Specify config. file.
        config_file_name_arg = opt.arg();
        break;

      case 'd': // Change to directory.
        change_directory_arg = opt.arg();
        break;

      case 'e': // Filename pattern(s) to index.
        if ( !::strtok( opt.arg(), ":" ) ) {
          error() << "no indexer module name\n";
          ::exit( Exit_Usage );
        }
        if ( indexer *const i = indexer::find_indexer( opt.arg() ) ) {
          for ( char *pat; (pat = ::strtok( nullptr, "," )); )
            include_patterns.insert( pat, i );
          break;
        }
        error() << '"' << opt.arg() << "\": no such indexing module\n";
        ::exit( Exit_Usage );

      case 'E': // Filename pattern(s) not to index.
        for ( char *a = opt.arg(), *pat; (pat = ::strtok( a, "," )); ) {
          exclude_patterns.insert( pat );
          a = nullptr;
        } // for
        break;

      case 'f': // Specify the word/file file maximum.
        word_files_max_arg = opt.arg();
        break;

      case 'F': // Specify files to reserve space for.
        files_reserve_arg = opt.arg();
        break;

      case 'g': // Specify files to reserve space for growth.
        files_grow_arg = opt.arg();
        break;

      case 'i': // Specify index file overriding the default.
        index_file_name_arg = opt.arg();
        break;

      case 'I': // specify incremental indexing.
        incremental_opt = true;
        break;

#ifndef PJL_NO_SYMBOLIC_LINKS
      case 'l': // Follow symbolic links during indexing.
        follow_symbolic_links_opt = true;
        break;
#endif
      case 'm': // Specify meta name(s) to index.
        include_meta_names.parse_value( opt.arg() );
        break;

      case 'M': // Specify meta name(s) not to index.
        exclude_meta_names.insert( to_lower( opt.arg() ) );
        break;

      case 'p': // Specify the word/file percentage.
        word_percent_max_arg = opt.arg();
        break;

#ifdef WITH_WORD_POS
      case 'P': // Don't store word position data.
        no_word_pos_opt = true;
        break;
#endif /* WITH_WORD_POS */

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
        print_version = true;
        break;

      case 'W': // Word threshold.
        word_threshold_arg = opt.arg();
        break;

      default: // Any indexing module claim the option?
        if ( !indexer::any_mod_claims_option( opt ) )
          cerr << usage;
    } // switch
  } // for
  argc -= opt_in.shift(), argv += opt_in.shift();

  if ( print_version ) {
    cout << PACKAGE_STRING << endl;
    ::exit( Exit_Success );
  }

  //
  // First, parse the config. file (if any); then override variables with
  // options specified on the command line.
  //
  conf_var::parse_file( config_file_name_arg );

  if ( files_grow_arg )
    files_grow = files_grow_arg;
  if ( files_reserve_arg )
    files_reserve = files_reserve_arg;
#ifndef PJL_NO_SYMBOLIC_LINKS
  if ( follow_symbolic_links_opt )
    follow_symbolic_links = true;
#endif
  if ( incremental_opt )
    incremental = true;
  if ( index_file_name_arg )
    index_file_name = index_file_name_arg;
  if ( no_associate_meta_opt )
    associate_meta = false;
#ifdef WITH_WORD_POS
  if ( no_word_pos_opt )
    store_word_positions = false;
#endif /* WITH_WORD_POS */
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
  if ( word_files_max_arg )
    word_files_max = word_files_max_arg;
  if ( word_percent_max_arg )
    word_percent_max = word_percent_max_arg;
  if ( word_threshold_arg )
    word_threshold = word_threshold_arg;

  indexer::all_mods_post_options();

  /////////// Dump stuff if requested /////////////////////////////////////////

  if ( dump_stop_words_opt ) {
    stop_words = new stop_word_set();
    ::copy( stop_words->begin(), stop_words->end(),
      ostream_iterator<char const*>( cout, "\n" )
    );
    ::exit( Exit_Success );
  }

  /////////// Index specified directories and files ///////////////////////////

  temp_file_name_prefix = temp_directory;
  if ( *temp_file_name_prefix.rbegin() != '/' )
    temp_file_name_prefix += '/';
  temp_file_name_prefix += string( itoa( ::getpid() ) ) + string( "." );

  bool const using_stdin = *argv && (*argv)[0] == '-' && !(*argv)[1];
  if ( !using_stdin && include_patterns.empty() && exclude_patterns.empty() )
    error() << "filename patterns must be specified "
               "when not using standard input\n" << usage;

  if ( !argc )
    cerr << usage;

  if ( incremental ) {
    load_old_index( index_file_name );
    index_file_name += ".new";
  } else {
    stop_words = new stop_word_set( stop_word_file_name );
    //
    // In the case where several files (and no directories) are indexed, there
    // would be no directory; however, every file must be in a directory, so
    // add the directory "." here and now to the list of directories.
    //
    check_add_directory( "." );
  }

  ofstream out( index_file_name, ios::out | ios::binary );
  if ( !out ) {
    error() << "can not write index to \"" << index_file_name << "\"\n";
    ::exit( Exit_No_Write_Index );
  }

  if ( change_directory_arg )
    change_directory = change_directory_arg;

  time_t time = ::time( nullptr );      // Go!

  if ( using_stdin ) {
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
        do_check_add_file( file_name );
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
        do_check_add_file( *argv );
    } // for
  }

  if ( partial_index_file_names.empty() ) {
    rank_full_index();
    write_full_index( out );
  } else {
    if ( words.size() ) {
      //
      // Since we created partial indicies, write any remaining words to their
      // own partial index so the merge code doesn't have a special case.
      //
      write_partial_index();
    }
    merge_indicies( out );
  }

  out.close();

  if ( verbosity ) {
    time = ::time( nullptr ) - time;    // Stop!

    bool testing = false;
    if ( char const *const swishxx_test = ::getenv( "SWISHXX_TEST" ) )
      parse( swishxx_test, &testing );

    cout << '\n' << me << ": done:\n";
    if ( !testing ) {
      cout << "  " << setfill('0')
           << setw(2) << (time / 60) << ':'
           << setw(2) << (time % 60) << " (min:sec) elapsed time\n";
    }
    cout << "  ";
    if ( !testing )
      cout << num_examined_files << " files, ";
    cout << file_info::num_files() << " indexed\n  "
         << num_total_words << " words, "
         << num_indexed_words << " indexed, "
         << num_unique_words << " unique\n\n";
  }

  ::exit( Exit_Success );
}

/**
 * Checks to see if the word is too frequent by either exceeding the maximum
 * number or percentage of files it can be in.
 *
 * @param word The word to be checked.
 * @param file_count The number of files the word occurs in.
 * @return Returns \c true only if the word is too frequent.
 */
bool is_too_frequent( char const *word, unsigned file_count ) {
  if ( file_count > word_files_max ) {
    if ( verbosity > 2 )
      cout << "\n  \"" << word << "\" discarded (" << file_count << " files)"
           << flush;
    return true;
  }
  unsigned const wfp =
    static_cast<unsigned>( file_count * 100 / file_info::num_files() );
  if ( wfp >= word_percent_max ) {
    if ( verbosity > 2 )
      cout << "\n  \"" << word << "\" discarded (" << wfp << "%)" << flush;
    return true;
  }
  return false;
}

/**
 * Load the stop-word, file, directory, and meta-name indicies from an existing
 * index file.
 *
 * @param index_file_name The name of the index file to load.
 */
void load_old_index( char const *index_file_name ) {
  mmap_file const index_file( index_file_name );
  if ( !index_file ) {
    error() << "could not read index from \"" << index_file_name
            << '"' << error_string( index_file.error() );
    ::exit( Exit_No_Read_Index );
  }

  ////////// Load old stop words //////////////////////////////////////////////

  stop_words = new stop_word_set( index_file );

  ////////// Load old directories /////////////////////////////////////////////

  index_segment old_dirs( index_file, index_segment::isi_dir );
  for ( auto const &d : old_dirs )
    check_add_directory( new_strdup( d ) );

  ////////// Load old files ///////////////////////////////////////////////////

  index_segment old_files( index_file, index_segment::isi_file );
  if ( files_reserve <= old_files.size() ) {
    //
    // Add the FilesGrow configuration variable to the FilesReserve
    // configuration variable to allow room for growth.
    //
    files_reserve = files_grow( static_cast<int>( old_files.size() ) );
  }
  FOR_EACH( old_files, f ) {
    unsigned char const *u = reinterpret_cast<unsigned char const*>( *f );
    int const dir_index = dec_int( u );
    char const *const file_name = reinterpret_cast<char const*>(u);
    while ( *u++ ) ;                    // skip past filename
    size_t const size = dec_int( u );
    int const num_words = dec_int( u );
    char const *const title = reinterpret_cast<char const*>( u );

    string const dir_str( old_dirs[ dir_index ] );
    string const path( dir_str + '/' + file_name );

    new file_info( path.c_str(), dir_index, size, title, num_words);
  } // for

  ////////// Load old meta names //////////////////////////////////////////////

  index_segment old_meta_names( index_file, index_segment::isi_meta_name );
  FOR_EACH( old_meta_names, m ) {
    unsigned char const* p = reinterpret_cast<unsigned char const*>( *m );
    while ( *p++ ) ;                    // skip past meta name
    meta_names[ new_strdup( *m ) ] = dec_int( p );
  }

  partial_index_file_names.push_back( index_file_name );
}

/**
 * Calculates the rank of a word in a file.  This equation was taken from the
 * one used in SWISH-E whose author thinks (?) it is the one taken from WAIS.
 * I can't find this equation in the refernece cited below, although that
 * reference does list a different equation.  But, if it ain't broke, don't fix
 * it.
 *
 * See also:
 *    Gerard Salton.  "Automatic Text Processing: the transformation, analysis,
 *    and retrieval of information by computer."  Addison-Wesley, Reading, MA.
 *    pp. 279-280.
 *
 * @param file_index Which file we're dealing with.
 * @param occurences_in_file The number of times the word occurs in a given
 * file.
 * @param factor This should be precomputed to be the value of Rank_Factor
 * divided by the total number of occurrences across all files.  This number is
 * constant for a given word, hence the precomputation.
 * @return Returns a rank greater than zero.
 */
inline int rank_word( int file_index, int occurences_in_file, double factor ) {
  int const r = int(
    ( ::log( occurences_in_file ) + 10 ) * factor
    / file_info::ith_info( file_index )->num_words()
  );
  return r > 0 ? r : 1;
}

/**
 * Perform an n-way merge of the partial word index files.  It first determines
 * the number of unique words in all the partial indicies, then merges them all
 * together and performs ranking at the same time.
 *
 * @param o The ostream to write the index to.
 */
void merge_indicies( ostream &o ) {
  vector<mmap_file> index( partial_index_file_names.size() );
  vector<index_segment> words( partial_index_file_names.size() );
  vector<index_segment::const_iterator> word( partial_index_file_names.size() );
  size_t i, j;

  ////////// Reopen all the partial indicies //////////////////////////////////

  ::atexit( &remove_temp_files );
  i = 0;
  for ( auto const &file_name : partial_index_file_names ) {
    index[i].open( file_name.c_str() );
    if ( !index[i] ) {
      error() << "can not reopen temp. file \"" << file_name << '"'
              << error_string( index[i].error() );
      ::exit( Exit_No_Open_Temp );
    }
    words[i].set_index_file( index[i], index_segment::isi_word );
    ++i;
  } // for

  ////////// Must determine the number of unique words first //////////////////

  if ( verbosity > 1 )
    cout << me << ": determining unique words..." << flush;

  for ( i = 0; i < partial_index_file_names.size(); ++i ) {
    // Start off assuming that all the words are unique.
    num_unique_words += words[i].size();
    word[i] = words[i].begin();
  } // for

  while ( true ) {

    // Find at least two non-exhausted indicies noting the first.
    int n = 0;
    for ( j = 0; j < partial_index_file_names.size(); ++j ) {
      if ( word[j] != words[j].end() ) {
        if ( !n++ )
          i = j;
        else if ( n >= 2 )
          break;
      }
    } // for
    if ( n < 2 )                        // couldn't find at least 2
      break;

    // Find the lexographically least word.
    for ( j = i + 1; j < partial_index_file_names.size(); ++j )
      if ( word[j] != words[j].end() )
        if ( ::strcmp( *word[j], *word[i] ) < 0 )
          i = j;

    file_list const list( word[i] );
    int file_count = list.size();

    // See if there are any duplicates and eliminate them.
    for ( j = i + 1; j < partial_index_file_names.size(); ++j ) {
      if ( word[j] != words[j].end() && !::strcmp( *word[j], *word[i] ) ) {
        //
        // The two words are the same: add the second word's file count to that
        // of the first.
        //
        --num_unique_words;
        file_list const list( word[j] );
        file_count += list.size();
        ++word[j];
      }
    } // for

    if ( is_too_frequent( *word[i], file_count ) ) {
      //
      // The word occurs too frequently: consider it a stop word.
      //
      stop_words->insert( *word[i] );
      --num_unique_words;
    }

    ++word[i];
  } // while

  ////////// Write index file header //////////////////////////////////////////

#define SWISHXX_WRITE_HEADER
#include "index_header.cpp"
#undef SWISHXX_WRITE_HEADER

  ////////// Merge the indicies ///////////////////////////////////////////////

  if ( verbosity > 1 )
    cout << '\n' << me << ": merging partial indicies..." << flush;

  for ( i = 0; i < partial_index_file_names.size(); ++i )
    word[i] = words[i].begin();         // reset all iterators
  int word_index = 0;
  while ( true ) {

    ////////// Find the next word /////////////////////////////////////////////

    // Find at least two non-exhausted indicies noting the first.
    int n = 0;
    for ( j = 0; j < partial_index_file_names.size(); ++j ) {
      for ( ; word[j] != words[j].end(); ++word[j] )
        if ( !stop_words->contains( *word[j] ) )
            break;
      if ( word[j] != words[j].end() ) {
        if ( !n++ )
          i = j;
        else if ( n >= 2 )
          break;
      }
    } // for
    if ( n < 2 )                        // couldn't find at least 2
      break;

    // Find the lexographically least word.
    for ( j = i + 1; j < partial_index_file_names.size(); ++j )
      if ( word[j] != words[j].end() )
        if ( ::strcmp( *word[j], *word[i] ) < 0 )
          i = j;

    word_offset[ word_index++ ] = o.tellp();
    o << *word[i] << '\0' << assert_stream;

    ////////// Calc. total occurrences in all indicies ////////////////////////

    int total_occurrences = 0;
    for ( j = i; j < partial_index_file_names.size(); ++j ) {
      if ( word[j] == words[j].end() )
        continue;
      if ( ::strcmp( *word[j], *word[i] ) )
        continue;
      for ( auto const &file : file_list( word[j] ) )
        total_occurrences += file.occurrences_;
    } // for
    double const factor = (double)Rank_Factor / total_occurrences;

    ////////// Copy all index info and compute ranks //////////////////////////

    bool continues = false;
    for ( j = i; j < partial_index_file_names.size(); ++j ) {
      if ( word[j] == words[j].end() )
        continue;
      if ( ::strcmp( *word[j], *word[i] ) )
        continue;
      for ( auto const &file : file_list( word[j] ) ) {
        if ( continues )
          o << Word_Entry_Continues_Marker << assert_stream;
        else
          continues = true;

        o << enc_int( file.index_ )
          << enc_int( file.occurrences_ )
          << enc_int( rank_word( file.index_, file.occurrences_, factor ) )
          << assert_stream;
        if ( !file.meta_ids_.empty() )
          file.write_meta_ids( o );
#ifdef WITH_WORD_POS
        if ( !file.pos_deltas_.empty() )
          file.write_word_pos( o );
#endif /* WITH_WORD_POS */
      } // for

      if ( j != i )
        ++word[j];
    } // for
    o << Stop_Marker << assert_stream;

    ++word[i];
  } // while

  ////////// Copy remaining words from last non-exhausted index ///////////////

  for ( j = 0; j < partial_index_file_names.size(); ++j ) {
    if ( word[j] == words[j].end() )
      continue;

    for ( ; word[j] != words[j].end(); ++word[j] ) {
      if ( stop_words->contains( *word[j] ) )
        continue;

      word_offset[ word_index++ ] = o.tellp();
      o << *word[j] << '\0' << assert_stream;

      ////////// Calc. total occurrences in all indicies //////////////////////

      int total_occurrences = 0;
      file_list const list( word[j] );
      for ( auto const &file : list )
        total_occurrences += file.occurrences_;
      double const factor = (double)Rank_Factor / total_occurrences;

      ////////// Copy all index info and compute ranks ////////////////////////

      bool continues = false;
      for ( auto const &file : list ) {
        if ( continues )
          o << Word_Entry_Continues_Marker << assert_stream;
        else
          continues = true;

        o << enc_int( file.index_ )
          << enc_int( file.occurrences_ )
          << enc_int( rank_word( file.index_, file.occurrences_, factor ) )
          << assert_stream;

        if ( !file.meta_ids_.empty() )
          file.write_meta_ids( o );
#ifdef WITH_WORD_POS
        if ( !file.pos_deltas_.empty() )
          file.write_word_pos( o );
#endif /* WITH_WORD_POS */
      } // for
      o << Stop_Marker << assert_stream;
    } // for
  } // for

  write_stop_word_index( o, stop_word_offset );
  write_dir_index      ( o, dir_offset );
  write_file_index     ( o, file_offset );
  write_meta_name_index( o, meta_name_offset );

  ////////// Go back and write the computed offsets /////////////////////////

#define SWISHXX_REWRITE_HEADER
#include "index_header.cpp"
#undef SWISHXX_REWRITE_HEADER

  if ( verbosity > 1 )
    cout << '\n';
}

/**
 * Computes the rank of all files for all words in the index.  This function is
 * used only when partial indicies are not generated.  Also removes words that
 * occur too frequently.
 */
void rank_full_index() {
  if ( words.empty() )
    return;

  if ( verbosity > 1 )
    cout << '\n' << me << ": ranking index..." << flush;

  for ( auto w = words.begin(); w != words.end(); ) {
    word_info &info = w->second;

    if ( is_too_frequent( w->first.c_str(), info.files_.size() ) ) {
      //
      // The word occurs too frequently: consider it a stop word.
      //
      stop_words->insert( new_strdup( w->first.c_str() ) );
      words.erase( w++ );
      continue;
    } // for
    ++w;

    //
    // Compute the rank for this word in every file it's in.
    //
    double const factor = (double)Rank_Factor / info.occurrences_;
    for ( auto &file : info.files_ )
      file.rank_ = rank_word( file.index_, file.occurrences_, factor );
  } // for

  if ( verbosity > 1 )
    cout << '\n';
}

/**
 * Removes the temporary partial index files.  This function is called via
 * \c atexit(3).
 *
 * This function is declared <code>extern "C"</code> since it is called via the
 * C library function \c atexit(3) and, because it's a C function, it expects C
 * linkage.
 */
void remove_temp_files( void ) {
  for ( int i = 0; i < num_temp_files; ++i ) {
    string const temp_file_name = temp_file_name_prefix + itoa( i );
    ::unlink( temp_file_name.c_str() );
  } // for
}

/**
 * Writes the directory index to the given ostream recording the offsets as it
 * goes.
 *
 * @param o The ostream to write the index to.
 * @param offset A pointer to a built-in vector where to record the offsets.
 */
static void write_dir_index( ostream &o, off_t *offset ) {
  //
  // First, order the directories by their index using a temporary vector.
  //
  typedef vector<char const*> dir_list_type;
  dir_list_type dir_list( dir_set.size() );
  for ( auto const &dir : dir_set )
    dir_list[ dir.second ] = dir.first;
  //
  // Now write them out in order.
  //
  int dir_index = 0;
  for ( auto const &dir : dir_list ) {
    offset[ dir_index++ ] = o.tellp();
    o << dir << '\0' << assert_stream;
  } // for
}

/**
 * Writes the file index to the given ostream recording the offsets as it goes.
 *
 * @param o The ostream to write the index to.
 * @param offset A pointer to a built-in vector where to record the offsets.
 */
static void write_file_index( ostream &o, off_t *offset ) {
  int file_index = 0;
  for ( auto fi = file_info::begin(); fi != file_info::end(); ++fi ) {
    offset[ file_index++ ] = o.tellp();
    o << enc_int( (*fi)->dir_index() )
      << (*fi)->file_name() << '\0'
      << enc_int( (*fi)->size() )
      << enc_int( (*fi)->num_words() )
      << (*fi)->title() << '\0'
      << assert_stream;
  } // for
}

/**
 * Writes the index to the given ostream.  The index file is written in such a
 * way so that it can be mmap'd and used instantly with no parsing or other
 * processing.
 *
 * @param o The ostream to write the index to.
 */
static void write_full_index( ostream &o ) {
  if ( !( num_unique_words = words.size() ) )
    return;

  if ( verbosity > 1 )
    cout << me << ": writing index..." << flush;

#define SWISHXX_WRITE_HEADER
#include "index_header.cpp"
#undef SWISHXX_WRITE_HEADER

  write_word_index     ( o, word_offset );
  write_stop_word_index( o, stop_word_offset );
  write_dir_index      ( o, dir_offset );
  write_file_index     ( o, file_offset );
  write_meta_name_index( o, meta_name_offset );

#define SWISHXX_REWRITE_HEADER
#include "index_header.cpp"
#undef SWISHXX_REWRITE_HEADER

  if ( verbosity > 1 )
    cout << '\n';
}

/**
 * Writes the meta name index to the given ostream recording the offsets as it
 * goes.
 *
 * @param o The ostream to write the index to.
 * @param offset A pointer to a built-in vector where to record the offsets.
 */
static void write_meta_name_index( ostream &o, off_t *offset ) {
  int meta_index = 0;
  for ( auto const &m : meta_names ) {
    offset[ meta_index++ ] = o.tellp();
    o << m.first << '\0' << enc_int( m.second ) << assert_stream;
  } // for
}

/**
 * Writes a partial index to a temporary file.  The format of a partial index
 * file is:
 * \code
 *    long  num_words;
 *    off_t word_offset[ num_words ];
 *          (word index)
 * \endcode
 * The partial word index is in the same format as the complete index.
 */
static void write_partial_index() {
  string const temp_file_name =
    temp_file_name_prefix + itoa( num_temp_files++ );
  ofstream o( temp_file_name.c_str(), ios::out | ios::binary );
  if ( !o ) {
    error() << "can not write temp. file \"" << temp_file_name << "\"\n";
    ::exit( Exit_No_Write_Temp );
  }
  partial_index_file_names.push_back( temp_file_name );

  if ( verbosity > 1 )
    cout << '\n' << me << ": writing partial index..." << flush;

  long const num_words = words.size();
  off_t *const word_offset = new off_t[ num_words ];

  // Write dummy data as a placeholder until the offsets are computed.
  my_write( o, &num_words, sizeof( num_words ) );
  streampos const word_offset_pos = o.tellp();
  my_write( o, word_offset, num_words * sizeof( word_offset[0] ) );

  write_word_index( o, word_offset );

  // Go back and write the computed offsets.
  o.seekp( word_offset_pos );
  my_write( o, word_offset, num_words * sizeof( word_offset[0] ) );

  delete[] word_offset;
  words.clear();

  if ( verbosity > 1 )
    cout << "\n\n";
}

/**
 * Writes the stop-word index to the given ostream recording the offsets as it
 * goes.
 *
 * @param o The ostream to write the index to.
 * @param offset A pointer to a built-in vector where to record the offsets.
 */
static void write_stop_word_index( ostream &o, off_t *offset ) {
  int word_index = 0;
  for ( auto word : *stop_words ) {
    offset[ word_index++ ] = o.tellp();
    o << word << '\0' << assert_stream;
  }
}

/**
 * Writes the word index to the given ostream recording the offsets as it goes.
 *
 * @param o The ostream to write the index to.
 * @param offset A pointer to a built-in vector where to record the offsets.
 */
static void write_word_index( ostream &o, off_t *offset ) {
  int word_index = 0;
  for ( auto w : words ) {
    offset[ word_index++ ] = o.tellp();
    o << w.first << '\0' << assert_stream;
    bool continues = false;
    word_info const &info = w.second;
    for ( auto file : info.files_ ) {
      if ( continues )
        o << Word_Entry_Continues_Marker << assert_stream;
      else
        continues = true;
      o << enc_int( file.index_ )
        << enc_int( file.occurrences_ )
        << enc_int( file.rank_ )
        << assert_stream;
      if ( !file.meta_ids_.empty() )
        file.write_meta_ids( o );
#ifdef WITH_WORD_POS
      if ( !file.pos_deltas_.empty() )
        file.write_word_pos( o );
#endif /* WITH_WORD_POS */
    } // for

    o << Stop_Marker << assert_stream;
  } // for
}

/**
 * Writes the usage message to the given ostream.
 *
 * @param o The ostream to write to.
 * @return Returns \a o.
 */
static ostream& usage( ostream &o ) {
  o << "usage: " << me << " [options] dir ... file ...\n"
  "options: (unambiguous abbreviations may be used for long options)\n"
  "========\n"
  "-?     | --help             : Print this help message\n"
  "-A     | --no-assoc-meta    : Don't associate meta names [default: do]\n"
  "-c f   | --config-file f    : Name of configuration file [default: " << ConfigFile_Default << "]\n"
  "-e m:p | --pattern m:p      : Module and file pattern to index [default: none]\n"
  "-E p   | --no-pattern p     : File pattern not to index [default: none]\n"
  "-f n   | --word-files n     : Word/file maximum [default: infinity]\n"
  "-F n   | --files-reserve n  : Reserve space for number of files [default: " << FilesReserve_Default << "]\n"
  "-g n   | --files-grow n     : Number or percentage to grow by [default: " << FilesGrow_Default << "]\n"
  "-i f   | --index-file f     : Name of index file to use [default: " << IndexFile_Default << "]\n"
  "-I     | --incremental      : Add files/words to index [default: replace]\n"
#ifndef PJL_NO_SYMBOLIC_LINKS
  "-l     | --follow-links     : Follow symbolic links [default: don't]\n"
#endif
  "-m m   | --meta m           : Meta name to index [default: all]\n"
  "-M m   | --no-meta m        : Meta name not to index [default: none]\n"
  "-p n   | --word-percent n   : Word/file percentage [default: 100]\n"
#ifndef WITH_WORD_POS
  "-P     | --no-pos-data      : Don't store word position data [default: do]\n"
#endif /* WITH_WORD_POS */
  "-r     | --no-recurse       : Don't index subdirectories [default: do]\n"
  "-s f   | --stop-file f      : Stop-word file to use instead of built-in default\n"
  "-S     | --dump-stop        : Dump built-in stop-words, exit\n"
  "-t n   | --title-lines n    : Lines to look for titles [default: " << TitleLines_Default << "]\n"
  "-T d   | --temp-dir d       : Directory for temporary files [default: " << TempDirectory_Default << "]\n"
  "-v n   | --verbosity n      : Verbosity level [0-4; default: 0]\n"
  "-V     | --version          : Print version number, exit\n"
  "-W n   | --word-threshold n : Words to make partial indicies [default: " << WordThreshold_Default << "]\n";
  indexer::all_mods_usage( o );
  ::exit( Exit_Usage );
  return o;                             // just to make the compiler happy
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
