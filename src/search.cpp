/*
**      SWISH++
**      src/search.cpp
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
#include "classic_formatter.h"
#include "config.h"
#include "exit_codes.h"
#include "file_info.h"
#include "file_list.h"
#include "indexer.h"
#include "IndexFile.h"
#include "index_segment.h"
#include "pjl/less.h"
#include "pjl/mmap_file.h"
#include "pjl/omanip.h"
#include "pjl/option_stream.h"
#include "pjl/pjl_set.h"
#include "query.h"
#include "ResultSeparator.h"
#include "ResultsFormat.h"
#include "results_formatter.h"
#include "ResultsMax.h"
#include "search.h"
#include "StemWords.h"
#include "token.h"
#include "util.h"
#include "WordFilesMax.h"
#include "WordPercentMax.h"
#ifdef WITH_WORD_POS
#include "WordsNear.h"
#endif /* WITH_WORD_POS */
#include "word_util.h"
#include "xml_formatter.h"
#ifdef WITH_SEARCH_DAEMON
#include "Group.h"
#ifdef __APPLE__
#include "LaunchdCooperation.h"
#endif /* __APPLE__ */
#include "PidFile.h"
#include "SearchBackground.h"
#include "SearchDaemon.h"
#include "SocketAddress.h"
#include "SocketFile.h"
#include "SocketQueueSize.h"
#include "SocketTimeout.h"
#include "ThreadsMax.h"
#include "ThreadsMin.h"
#include "ThreadTimeout.h"
#include "User.h"
#endif /* WITH_SEARCH_DAEMON */

// standard
#include <algorithm>                    /* for binary_search(), etc */
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <functional>                   /* for binary_function<> */
#include <iostream>
#include <iterator>
#include <memory>                       /* for unique_ptr */
#include <string>
#include <sys/resource.h>               /* for RLIMIT_* */
#include <sys/time.h>                   /* needed by FreeBSD systems */
#include <time.h>                       /* needed by sys/resource.h */
#include <utility>                      /* for pair<> */
#include <vector>

using namespace PJL;
using namespace std;

/**
 * A %search_result is an individual search result where the first \c int is a
 * file index and the second \c int is that file's rank.
 */
typedef pair<int,int> search_result;

/**
 * A %sort_by_rank is-a binary_function used to sort search results by rank in
 * descending order (highest rank first).
 */
struct sort_by_rank :
  binary_function<search_result const&,search_result const&,bool>
{
  result_type operator()( first_argument_type a, second_argument_type b ) {
    return a.second > b.second;
  }
};

//*****************************************************************************
//
//  Global declarations
//
//*****************************************************************************

index_segment       directories, files, meta_names, stop_words, words;
IndexFile           index_file_name;
ResultsMax          max_results;
char const*         me;                         // executable name
ResultSeparator     result_separator;
ResultsFormat       results_format;
StemWords           stem_words;
WordFilesMax        word_files_max;
WordPercentMax      word_percent_max;
#ifdef WITH_WORD_POS
WordsNear           words_near;
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
SearchDaemon        daemon_type;
Group               group;
#ifdef __APPLE__
LaunchdCooperation  launchd_cooperation;
#endif /* __APPLE__ */
ThreadsMax          max_threads;
ThreadsMin          min_threads;
PidFile             pid_file_name;
SearchBackground    search_background;
SocketAddress       socket_address;
SocketFile          socket_file_name;
SocketQueueSize     socket_queue_size;
SocketTimeout       socket_timeout;
ThreadTimeout       thread_timeout;
User                user;

void                become_daemon();
#endif /* WITH_SEARCH_DAEMON */
static void         dump_single_word( char const*, ostream& = cout );
static void         dump_word_window( char const*, int, int, ostream& = cout );
static ostream&     write_file_info( ostream&, char const* );

inline omanip<char const*> index_file_info( int index ) {
  return omanip<char const*>( write_file_info, files[ index ] );
}

////////// main ///////////////////////////////////////////////////////////////

int main( int argc, char *argv[] ) {
#include "search_options.cpp"           /* defines opt_spec */

  me = ::strrchr( argv[0], '/' );       // determine base name ...
  me = me ? me + 1 : argv[0];           // ... of executable

  /////////// Process command-line options ////////////////////////////////////

  search_options const opt( &argc, &argv, opt_spec );
  if ( !opt )
    ::exit( Exit_Usage );

  //
  // First, parse the config. file (if any); then override variables with
  // command-line options.
  //
  conf_var::parse_file( opt.config_file_name_arg );

  if ( opt.index_file_name_arg )
    index_file_name = opt.index_file_name_arg;
  if ( opt.max_results_arg )
    max_results = opt.max_results_arg;
  if ( opt.results_format_arg )
    results_format = opt.results_format_arg;
  if ( opt.result_separator_arg )
    result_separator = opt.result_separator_arg;
  if ( opt.stem_words_opt )
    stem_words = true;
  if ( opt.word_files_max_arg )
    word_files_max = opt.word_files_max_arg;
  if ( opt.word_percent_max_arg )
    word_percent_max = opt.word_percent_max_arg;
#ifdef WITH_WORD_POS
  if ( opt.words_near_arg )
    words_near = opt.words_near_arg;
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
  if ( opt.daemon_type_arg )
    daemon_type = opt.daemon_type_arg;
  if ( opt.group_arg )
    group = opt.group_arg;
#ifdef __APPLE__
  if ( opt.launchd_opt )
    launchd_cooperation = true;
#endif /* __APPLE__ */
  if ( opt.max_threads_arg )
    max_threads = opt.max_threads_arg;
  if ( opt.min_threads_arg )
    min_threads = opt.min_threads_arg;
  if ( opt.pid_file_name_arg )
    pid_file_name = opt.pid_file_name_arg;
  if ( opt.search_background_opt
#ifdef __APPLE__
       || launchd_cooperation
#endif /* __APPLE__ */
  )
    search_background = false;
  if ( opt.socket_address_arg )
    socket_address = opt.socket_address_arg;
  if ( opt.socket_file_name_arg )
    socket_file_name = opt.socket_file_name_arg;
  if ( opt.socket_queue_size_arg )
    socket_queue_size = opt.socket_queue_size_arg;
  if ( opt.socket_timeout_arg )
    socket_timeout = opt.socket_timeout_arg;
  if ( opt.thread_timeout_arg )
    thread_timeout = opt.thread_timeout_arg;
  if ( opt.user_arg )
    user = opt.user_arg;
#endif /* WITH_SEARCH_DAEMON */

  //
  // If there were no arguments, see if that's OK given the config. file
  // variables and command-line options.
  //
  bool const dump_something =
    opt.dump_entire_index_opt ||
    opt.dump_meta_names_opt   ||
    opt.dump_stop_words_opt   ||
    opt.dump_word_index_opt;
  if ( !(argc || dump_something
#ifdef WITH_SEARCH_DAEMON
         || daemon_type != "none"
#endif /* WITH_SEARCH_DAEMON */
  ) ) {
    cerr << usage;
    ::exit( Exit_Usage );
  }

  /////////// Load index file /////////////////////////////////////////////////

#ifdef RLIMIT_AS                       /* SVR4 */
#if defined( WITH_SEARCH_DAEMON ) && defined( __APPLE__ )
  if ( daemon_type != "none" && !launchd_cooperation )
#endif
    max_out_limit( RLIMIT_AS );         // max-out total avail. memory
#endif /* RLIMIT_AS */

  mmap_file const the_index( index_file_name );
  the_index.behavior( mmap_file::bt_random );
  if ( !the_index ) {
    error() << "could not read index from \"" << index_file_name
            << '"' << error_string( the_index.error() );
    ::exit( Exit_No_Read_Index );
  }
  words      .set_index_file( the_index, index_segment::isi_word      );
  stop_words .set_index_file( the_index, index_segment::isi_stop_word );
  directories.set_index_file( the_index, index_segment::isi_dir       );
  files      .set_index_file( the_index, index_segment::isi_file      );
  meta_names .set_index_file( the_index, index_segment::isi_meta_name );

#ifdef WITH_SEARCH_DAEMON
  ////////// Become a daemon //////////////////////////////////////////////////

  if ( !dump_something && daemon_type != "none" )
    become_daemon();                    // function does not return
#endif /* WITH_SEARCH_DAEMON */

  ////////// Perform the query ////////////////////////////////////////////////

  service_request( argv, opt );
  ::exit( Exit_Success );
}

////////// local functions ////////////////////////////////////////////////////

/**
 * Dumps the list of files a word is in and ranks therefore to standard output.
 *
 * @param word The word to have its index dumped.
 * @param out The ostream to dump to.
 */
static void dump_single_word( char const *word, ostream &out ) {
  unique_ptr<char[]> const lower_ptr( to_lower_r( word ) );
  char const *const lower_word = lower_ptr.get();
  less<char const*> const comparator;

  if ( !is_ok_word( word ) ||
    ::binary_search(
      stop_words.begin(), stop_words.end(), lower_word, comparator
    )
  ) {
    out << "# ignored: " << word << endl;
    return;
  }

  //
  // Look up the word.
  //
  word_range const range =
    ::equal_range( words.begin(), words.end(), lower_word, comparator );
  if ( range.first == words.end() || comparator( lower_word, *range.first ) ) {
    out << "# not found: " << word << endl;
    return;
  }

  file_list const list( range.first );
  for ( auto const &file : list ) {
    out << file.occurrences_ << ' '
        << file.rank_ << result_separator
        << index_file_info( file.index_ ) << '\n';
    if ( !out )
      return;
  } // for
  out << '\n';
}

/**
 * Dumps a "window" of words from the index around the given word to standard
 * output.
 *
 * @param word The word to dump.
 * @param window_size The number of lines the window is to contain at most.
 * @param match The number of characters to compare.
 * @param out The ostream to dump to.
 */
static void dump_word_window( char const *word, int window_size, int match,
                              ostream &out ) {
  unique_ptr<char[]> const lower_ptr( to_lower_r( word ) );
  char const *const lower_word = lower_ptr.get();
  less<char const*> const comparator;

  if ( !is_ok_word( word ) ||
    ::binary_search(
      stop_words.begin(), stop_words.end(), lower_word, comparator
    )
  ) {
    out << "# ignored: " << word << endl;
    return;
  }

  //
  // Look up the word.
  //
  word_range range =
    ::equal_range( words.begin(), words.end(), lower_word, comparator );
  if ( range.first == words.end() || comparator( lower_word, *range.first ) ){
    out << "# not found: " << word << endl;
    return;
  }

  //
  // Dump the window by first "backing up" half the window size, then going
  // forward.
  //
  int i = window_size / 2;
  while ( range.first != words.begin() && i-- > 0 )
    --range.first;
  for ( i = 0; range.first != words.end() && i < window_size; ++range.first ) {
    int const cmp = ::strncmp( *range.first, lower_word, match );
    if ( cmp < 0 )
      continue;
    if ( cmp > 0 )
      break;
    out << *range.first << '\n';
    if ( !out )
      return;
    ++i;
  } // for
  out << '\n';
}

/**
 * Parses a query, performs a search, and outputs the results.
 *
 * @param query The text of the query.
 * @param skip_results The number of initial results to skip.
 * @param max_results The maximum number of results to output.
 * @param results_format The results format.
 * @param out The ostream to print the results to.
 * @param err The ostream to print errors to.
 */
static bool search( char const *query, int skip_results, int max_results,
                    char const *results_format, ostream &out, ostream &err ) {
  token_stream    query_stream( query );
  search_results  results;
  stop_word_set   stop_words_found;

  if ( !(parse_query( query_stream, results, stop_words_found ) &&
         query_stream.eof()) ) {
    err << error << "malformed query\n";
#ifdef WITH_SEARCH_DAEMON
    if ( daemon_type != "none" )
      return false;
#endif /* WITH_SEARCH_DAEMON */
    ::exit( Exit_Malformed_Query );
  }

  ////////// Print the results ////////////////////////////////////////////////

  unique_ptr<results_formatter const> format;
  if ( to_lower( *results_format ) == 'x' /* must be "xml" */ )
    format.reset( new xml_formatter( out, results.size() ) );
  else
    format.reset( new classic_formatter( out, results.size() ) );

  format->pre( stop_words_found );
  if ( !out )
    return false;
  if ( skip_results < results.size() && max_results ) {
    typedef vector<search_result> sorted_results_type;
    sorted_results_type sorted;
    sorted.reserve( results.size() );
    ::copy( results.begin(), results.end(), ::back_inserter( sorted ) );
    ::sort( sorted.begin(), sorted.end(), sort_by_rank() );
    //
    // Compute the highest rank and the normalization factor.
    //
    int const highest_rank = sorted[0].second;
    double const normalize = 100.0 / highest_rank;
    //
    // Print the sorted results skipping some if requested to and not exceeding
    // the maximum.
    //
    for ( auto r = sorted.begin() + skip_results;
          r != sorted.end() && max_results-- > 0 && out; ++r ) {
      int rank = static_cast<int>( r->second * normalize );
      if ( !rank )
        rank = 1;
      format->result(
        rank,
        file_info( reinterpret_cast<unsigned char const*>( files[ r->first ] ) )
      );
      if ( !out )
        return false;
    } // for
  }
  format->post();
  return true;
}

search_options::search_options( int *argc, char ***argv,
                                option_stream::spec const opt_spec[],
                                ostream &err ) :
  bad_( false )
{
  config_file_name_arg  = ConfigFile_Default;
  dump_entire_index_opt = false;
  dump_match_arg        = 0;
  dump_meta_names_opt   = false;
  dump_stop_words_opt   = false;
  dump_window_size_arg  = 0;
  dump_word_index_opt   = false;
  index_file_name_arg   = 0;
  max_results_arg       = 0;
  results_format_arg    = 0;
  result_separator_arg  = 0;
  skip_results_arg      = 0;
  stem_words_opt        = false;
  word_files_max_arg    = 0;
  word_percent_max_arg  = 0;
#ifdef WITH_WORD_POS
  words_near_arg        = 0;
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
  daemon_type_arg       = 0;
  group_arg             = 0;
#ifdef __APPLE__
  launchd_opt           = 0;
#endif /* __APPLE__ */
  max_threads_arg       = 0;
  min_threads_arg       = 0;
  pid_file_name_arg     = 0;
  search_background_opt = false;
  socket_address_arg    = 0;
  socket_file_name_arg  = 0;
  socket_queue_size_arg = 0;
  socket_timeout_arg    = 0;
  thread_timeout_arg    = 0;
  user_arg              = 0;
#endif /* WITH_SEARCH_DAEMON */
  option_stream opt_in( *argc, *argv, opt_spec );
  for ( option_stream::option opt; opt_in >> opt; ) {
    switch ( opt ) {

#ifdef WITH_SEARCH_DAEMON
      case 'a': // TCP socket address.
        socket_address_arg = opt.arg();
        break;

      case 'b': // Run as a daemon.
        daemon_type_arg = opt.arg();
        break;

      case 'B': // Do not run daemon in the background.
        search_background_opt = true;
        break;
#endif /* WITH_SEARCH_DAEMON */

      case 'c': // Config. file.
        config_file_name_arg = opt.arg();
        break;

      case 'd': // Dump query word indices.
        dump_word_index_opt = true;
        break;

      case 'D': // Dump entire word index.
        dump_entire_index_opt = true;
        break;

      case 'f': // Word/files file maximum.
        word_files_max_arg = opt.arg();
        break;

      case 'F': // Results format.
        if ( results_format.is_legal( opt.arg(), err ) )
          results_format_arg = opt.arg();
        else
          bad_ = true;
        break;

#ifdef WITH_SEARCH_DAEMON
      case 'G': // Group.
        group_arg = opt.arg();
        break;
#endif /* WITH_SEARCH_DAEMON */

      case 'i': // Index file.
        index_file_name_arg = opt.arg();
        break;

      case 'm': // Max. number of results.
        max_results_arg = opt.arg();
        break;

      case 'M': // Dump meta-name list.
        dump_meta_names_opt = true;
        break;

#ifdef WITH_WORD_POS
      case 'n': // Words near.
        words_near_arg = ::atoi( opt.arg() );
        break;
#endif /* WITH_WORD_POS */

#ifdef WITH_SEARCH_DAEMON
      case 'o': // Socket timeout.
        socket_timeout_arg = ::atoi( opt.arg() );
        break;

      case 'O': // Thread timeout.
        thread_timeout_arg = ::atoi( opt.arg() );
        break;
#endif /* WITH_SEARCH_DAEMON */

      case 'p': // Word/file percentage.
        word_percent_max_arg = opt.arg();
        break;

#ifdef WITH_SEARCH_DAEMON
      case 'P': // PID file.
        pid_file_name_arg = opt.arg();
        break;

      case 'q': // Socket queue size.
        socket_queue_size_arg = ::atoi( opt.arg() );
        if ( socket_queue_size_arg < 1 )
          socket_queue_size_arg = 1;
        break;
#endif /* WITH_SEARCH_DAEMON */

      case 'r': // Number of initial results to skip.
        skip_results_arg = ::atoi( opt.arg() );
        if ( skip_results_arg < 0 )
          skip_results_arg = 0;
        break;

      case 'R': // Result separator.
        result_separator_arg = opt.arg();
        break;

      case 's': // Stem words.
        stem_words_opt = true;
        break;

      case 'S': // Dump stop-word list.
        dump_stop_words_opt = true;
        break;

#ifdef WITH_SEARCH_DAEMON
      case 't': // Minimum number of concurrent threads.
        min_threads_arg = ::atoi( opt.arg() );
        break;

      case 'T': // Maximum number of concurrent threads.
        max_threads_arg = ::atoi( opt.arg() );
        break;

      case 'u': // Unix domain socket file.
        socket_file_name_arg = opt.arg();
        break;

      case 'U': // User.
        user_arg = opt.arg();
        break;
#endif /* WITH_SEARCH_DAEMON */

      case 'V': // Display version and exit.
        err << PACKAGE_STRING << endl;
#ifdef WITH_SEARCH_DAEMON
        if ( daemon_type == "none" )
#endif /* WITH_SEARCH_DAEMON */
          ::exit( Exit_Success );
#ifdef WITH_SEARCH_DAEMON
        bad_ = true;
        return;
#endif /* WITH_SEARCH_DAEMON */

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

#if defined( WITH_SEARCH_DAEMON ) && defined( __APPLE__ )
      case 'X': // Cooperate with Mac OS X's launchd.
        launchd_opt = true;
        break;
#endif

      default: // Bad option.
        err << usage;
        bad_ = true;
        return;
      } // switch
    } // for

    *argc -= opt_in.shift(), *argv += opt_in.shift();
}

bool service_request( char *argv[], search_options const &opt, ostream &out,
                      ostream &err ) {
  if ( opt.dump_window_size_arg ) {
    while ( *argv && out )
      dump_word_window( *argv++,
        opt.dump_window_size_arg, opt.dump_match_arg, out
      );
    return true;
  }

  if ( opt.dump_word_index_opt ) {
    while ( *argv && out )
      dump_single_word( *argv++, out );
    return true;
  }

  if ( opt.dump_entire_index_opt ) {
    FOR_EACH( words, word ) {
      out << *word << '\n';
      file_list const list( word );
      FOR_EACH( list, file ) {
        out << "  " << file->occurrences_ << ' '
            << file->rank_ << result_separator
            << index_file_info( file->index_ )
            << '\n';
        if ( !out )
          return false;
      } // for
      out << '\n';
    } // for
    return true;
  }

  if ( opt.dump_stop_words_opt ) {
    FOR_EACH( stop_words, word ) {
      out << *word << '\n';
      if ( !out )
        return false;
    } // for
    return true;
  }

  if ( opt.dump_meta_names_opt ) {
    FOR_EACH( meta_names, meta_name ) {
      out << *meta_name << '\n';
      if ( !out )
        return false;
    } // for
    return false;
  }

  ////////// Perform the query ////////////////////////////////////////////////

  //
  // Paste the rest of the command line together into a single query string.
  //
  string query = *argv++;
  while ( *argv ) {
    query += ' ';
    query += *argv++;
  } // while

  return search(
    query.c_str(),
    opt.skip_results_arg,
    opt.max_results_arg ? ::atoi( opt.max_results_arg ) : max_results,
    opt.results_format_arg ? opt.results_format_arg : results_format,
    out, err
  );
}

/**
 * Parses a file_info from an index file and write it to an ostream.
 *
 * @param o The ostream to write to.
 * @param p A pointer to the first character containing a file_info inside an
 * index file.
 * @return Returns \a o.
 */
static ostream& write_file_info( ostream &o, char const *p ) {
  file_info const fi( reinterpret_cast<unsigned char const*>( p ) );
  return o
      << directories[ fi.dir_index() ] << '/' << fi.file_name()
      << result_separator << fi.size() << result_separator
      << fi.title();
}

ostream& usage( ostream &o ) {
  o <<  "usage: " << me << " [options] query\n"
  "options: (unambiguous abbreviations may be used for long options)\n"
  "========\n"
  "-?   | --help             : Print this help message\n"
#ifdef WITH_SEARCH_DAEMON
  "-a a | --socket-address a : Socket address [default: *:" << SocketPort_Default << "]\n"
  "-b t | --daemon-type t    : Daemon type to run as [default: none]\n"
  "-B   | --no-background    : Don't run daemon in the background [default: do]\n"
#endif /* WITH_SEARCH_DAEMON */
  "-c f | --config-file f    : Name of configuration file [default: " << ConfigFile_Default << "]\n"
  "-d   | --dump-words       : Dump query word indices, exit\n"
  "-D   | --dump-index       : Dump entire word index, exit\n"
  "-f n | --word-files n     : Word/file maximum [default: infinity]\n"
  "-F f | --format f         : Results format [default: classic]\n"
#ifdef WITH_SEARCH_DAEMON
  "-G s | --group s          : Daemon group to run as [default: " << Group_Default << "]\n"
#endif /* WITH_SEARCH_DAEMON */
  "-i f | --index-file f     : Name of index file [default: " << IndexFile_Default << "]\n"
  "-m n | --max-results n    : Maximum number of results [default: " << ResultsMax_Default << "]\n"
  "-M   | --dump-meta        : Dump meta-name index, exit\n"
#ifdef WITH_WORD_POS
  "-n n | --near n           : Maximum number of words apart [default: " << WordsNear_Default << "]\n"
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
  "-o s | --socket-timeout s : Search client request timeout [default: " << SocketTimeout_Default << "]\n"
  "-O s | --thread-timeout s : Idle spare thread timeout [default: " << ThreadTimeout_Default << "]\n"
#endif /* WITH_SEARCH_DAEMON */
  "-p n | --word-percent n   : Word/file percentage [default: 100]\n"
#ifdef WITH_SEARCH_DAEMON
  "-P f | --pid-file f       : Name of file to record daemon PID in [default: none]\n"
  "-q n | --queue-size n     : Maximum queued socket connections [default: " << SocketQueueSize_Default << "]\n"
#endif /* WITH_SEARCH_DAEMON */
  "-r n | --skip-results n   : Number of initial results to skip [default: 0]\n"
  "-R s | --separator s      : Result separator string [default: \" \"]\n"
  "-s   | --stem-words       : Stem words prior to search [default: no]\n"
  "-S   | --dump-stop        : Dump stop-word index, exit\n"
#ifdef WITH_SEARCH_DAEMON
  "-t n | --min-threads n    : Minimum number of threads [default: " << ThreadsMin_Default << "] \n"
  "-T n | --max-threads n    : Maximum number of threads [default: " << ThreadsMax_Default << "] \n"
  "-u f | --socket-file f    : Name of socket file [default: " << SocketFile_Default << "]\n"
  "-U s | --user s           : Daemon user to run as [default: " << User_Default << "]\n"
#endif /* WITH_SEARCH_DAEMON */
  "-V   | --version          : Print version number, exit\n"
  "-w n[,m] | --window n[,m] : Dump window of words around query words [default: 0]\n"
#if defined( WITH_SEARCH_DAEMON ) && defined( __APPLE__ )
  "-X   | --launchd          : If a daemon, cooperate with Mac OS X's launchd\n"
#endif
  ;
  return o;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
