/*
**      SWISH++
**      src/search.h
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

#ifndef search_H
#define search_H

// local
#include "config.h"
#include "pjl/option_stream.h"

// standard
#include <iostream>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %search_options is the set of command-line options to the \c search
 * executable.  These are all packaged into a structure because, in addition to
 * parsing command-line options initially, \c search, if run as a daemon, needs
 * to parse "command-line" options per request via a socket.  The options are
 * therefore packaged together into an object so each thread can have its own
 * set of options, i.e., be thread-safe.
 *
 * Structure member names are such that those that end in \c _arg are the value
 * of the argument of the option (which implies that the option was given) and
 * those that end in \c _opt are Boolean options and, if true, mean that the
 * option was given.
 */
struct search_options {
  char const *config_file_name_arg;
  bool        dump_entire_index_opt;
  int         dump_match_arg;
  bool        dump_meta_names_opt;
  bool        dump_stop_words_opt;
  int         dump_window_size_arg;
  bool        dump_word_index_opt;
  char const *index_file_name_arg;
  char const *max_results_arg;
  bool        print_help_opt;
  bool        print_version_opt;
  char const *results_format_arg;
  char const *result_separator_arg;
  int         skip_results_arg;
  bool        stem_words_opt;
  char const *word_files_max_arg;
  char const *word_percent_max_arg;
#ifdef WITH_WORD_POS
  int         words_near_arg;
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
  char const *daemon_type_arg;
  char const *group_arg;
#ifdef __APPLE__
  bool        launchd_opt;
#endif /* __APPLE__ */
  int         max_threads_arg;
  int         min_threads_arg;
  char const *pid_file_name_arg;
  bool        search_background_opt;
  char const *socket_address_arg;
  char const *socket_file_name_arg;
  int         socket_queue_size_arg;
  int         socket_timeout_arg;
  int         thread_timeout_arg;
  char const *user_arg;
#endif /* WITH_SEARCH_DAEMON */

  /**
   * Constructs (initialzes) a search_options by parsing options from \a argv
   * according to the given option specification.
   *
   * @param argc A pointer to the number of arguments.  The value is
   * decremented by the number of options and their arguments.
   * @param argv A pointer to the \c argv vector.  This pointer is incremented
   * by the number of options and their arguments.
   * @param spec The set of options to allow and their parameters.
   * @param err The ostream to print errors to.
   */
  search_options( int *argc, char ***argv,
                  PJL::option_stream::spec const spec[],
                  std::ostream &err = std::cerr );

  /**
   * Returns whether this %search_options contains all valid options.
   */
  explicit operator bool() const {
    return !bad_;
  }

private:
  bool bad_;                            // true only if there's a bad option
};

/**
 * Services a request either from the command line or from a client via a
 * socket.
 *
 * @param argv The post-option-parsed set of command line arguments, i.e., all
 * the options have been stripped.
 * @param opt The set of options specified for this request.
 * @param out The ostream to send results to.
 * @param err The ostream to send errors to.
 */
bool service_request( char *argv[], search_options const &opts,
                      std::ostream &out = std::cout,
                      std::ostream &err = std::cerr );

/**
 * Emits the usage message to the given ostream.
 *
 * @param o The ostream to emit to.
 * @return Returns \a o.
 */
std::ostream& usage( std::ostream &o );

///////////////////////////////////////////////////////////////////////////////
#endif /* search_H */
/* vim:set et sw=2 ts=2: */
