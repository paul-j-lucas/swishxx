/*
**      SWISH++
**      src/conf_var.cpp
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
#include "conf_var.h"
#include "exit_codes.h"
#include "pjl/mmap_file.h"
#include "swishxx-config.h"
#include "util.h"

// standard
#include <cctype>
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <iostream>
#include <memory>                       /* for unique_ptr */

using namespace std;
using namespace PJL;

extern char const   *me;

int conf_var::current_config_file_line_no_ = 0;

///////////////////////////////////////////////////////////////////////////////

/**
 * Constructs a configuration variable by mapping a variable name to an
 * instance of a %conf_var (really, an instance of some derived class).  Only a
 * single instance of any given variable may exist.
 *
 * @param var_name The name of the variable.
 */
conf_var::conf_var( char const *var_name ) :
  name_( var_name )
{
  conf_var *&var = map_ref()[ to_lower( name_ ) ];
  if ( var ) {
    internal_error
      << "conf_var::conf_var(): \"" << name_
      << "\" registered more than once" << report_error;
  }
  var = this;
}

conf_var::~conf_var() {
  // do nothing
}

/**
 * Defines and initializes (exactly once) a static data member for conf_var and
 * return a reference to it.  The reason for this function is to guarantee that
 * the map is initialized before its first use across all translation units,
 * something that would not guaranteed if it were a static data member
 * initialized at file scope.
 *
 * We also load the map with all configuration variable names (in lower case so
 * variables in config files will be case-insensitive) so we can tell the
 * difference between a variable that doesn't exist (and that we will complain
 * about to the user) and one that simply isn't used in a particular executable
 * (and will be silently ignored).
 *
 * @return Returns a reference to a static instance of an initialized map_type.
 */
conf_var::map_type& conf_var::map_ref() {
  static map_type m;
  static bool init;
  if ( !init ) {
    init = true;                        // must set this before init_mod_vars()
    init_mod_vars();                    // defined in init_mod_vars.c
    static char const *const var_name_table[] = {
      "associatemeta",
      "changedirectory",
      "excludefile",
      "excludemeta",
      "extractextension",
      "extractfile",
      "extractfilter",
      "filesgrow",
      "filesreserve",
      "filterfile",
      "followlinks",
      "includefile",
      "includemeta",
      "incremental",
      "indexfile",
      "recursesubdirs",
      "resultsformat",
      "resultseparator",
      "resultsmax",
      "stemwords",
      "stopwordfile",
      "tempdirectory",
      "titlelines",
      "verbosity",
      "wordfilesmax",
      "wordpercentmax",
      "wordthreshold",
#ifdef WITH_WORD_POS
      "storewordpositions",
      "wordsnear",
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
      "group",
#ifdef __APPLE__
      "launchdcooperation",
#endif /* __APPLE__ */
      "pidfile",
      "searchbackground",
      "searchdaemon",
      "socketaddress",
      "socketfile",
      "socketqueuesize",
      "sockettimeout",
      "threadsmax",
      "threadsmin",
      "threadtimeout",
      "user",
#endif /* WITH_SEARCH_DAEMON */
      0,
    };
    for ( char const *const *v = var_name_table; *v; ++v )
      m[ *v ] = 0;
  }
  return m;
}

/**
 * Emits the standard message preamble to the given ostream, that is the
 * program name, a line number if a configuration file is being parsed, and a
 * label.
 *
 * @param o The ostream to emit the message to.
 * @param label The label to emit, usually "error" or "warning".
 * @return Returns \a o.
 */
ostream& conf_var::msg( ostream &o, char const *label ) {
  o << me;
  if ( current_config_file_line_no_ ) {
    //
    // This is set only by parse_line() during the parsing of a configuration
    // file.
    //
    o << ": config file line " << current_config_file_line_no_;
    current_config_file_line_no_ = 0;
  }
  return o << ": " << label << ": ";
}

void conf_var::parse_const_value( char const *line ) {
  unique_ptr<char[]> line_copy( new_strdup( line ) );
  parse_value( line_copy.get() );
}

void conf_var::parse_file( char const *file_name ) {
  mmap_file const conf_file( file_name );
  if ( !conf_file ) {
    if ( !::strcmp( file_name, ConfigFile_Default ) ) {
      //
      // The configuration file couldn't be opened; however, the file name is
      // the default, so assume that none is being used and simply return.
      //
      return;
    }
    cerr << "could not read configuration from \"" << file_name
         << '"' << error_string( conf_file.error() );
    ::exit( Exit_Config_File );
  }
  conf_file.behavior( mmap_file::bt_sequential );

  int line_no = 0;
  mmap_file::const_iterator c = conf_file.begin();

  while ( c != conf_file.end() ) {
    //
    // Find the end of the line.
    //
    mmap_file::const_iterator const nl = find_newline( c, conf_file.end() );
    if ( nl == conf_file.end() )
        break;

    ++line_no;
    //
    // See if the line is entirely whitespace optionally followed by a comment
    // starting with '#': if so, skip it.  If we don't end up skipping it,
    // leading whitespace will have been skipped.
    //
    for ( ; c != nl; ++c ) {
      if ( is_space( *c ) )
        continue;
      if ( *c == '#' )
        goto next_line;
      break;
    } // for
    if ( c != nl ) {
      //
      // The line has something on it worth parsing further: copy it (less
      // leading and trailing whitespace) to a modifyable buffer and
      // null-terminate it to make that task easier.
      //
      char buf[ 256 ];
      ptrdiff_t len = nl - c;
      ::strncpy( buf, c, len );
      while ( len > 0 ) {
        if ( is_space( buf[ len - 1 ] ) )
          --len;
        else
          break;
      } // while
      buf[ len ] = '\0';
      parse_line( buf, line_no );
    } // for

next_line:
    c = skip_newline( nl, conf_file.end() );
  } // while
}

/**
 * Parses a non-comment or non-blank line from a the configuration file, the
 * first word of which is the variable name.  Look up the variable in our map
 * and delegate the parsing of the rest of the line to an instance of a derived
 * class that knows how to parse its own line format.
 *
 * @param line A line from a configuration file to be parsed.
 * @param line_no The line number of the line.
 */
void conf_var::parse_line( char *line, int line_no ) {
  current_config_file_line_no_ = line_no;
  ::strtok( line, " \r\t" );            // just the variable name
  map_type::const_iterator const i = map_ref().find( to_lower( line ) );
  if ( i == map_ref().end() ) {
    warning() << '"' << line << "\" unrecognized; ignored\n";
    return;
  }
  if ( i->second ) {
    //
    // Chop off trailing newline and remove leading whitespace from value.
    //
    char *value = ::strtok( 0, "\r\n" );
    while ( *value && is_space( *value ) )
      ++value;
    i->second->parse_value( value );
  } // else
  //
  // This config. variable is not used by the current executable: silently
  // ignore it.
  //
  current_config_file_line_no_ = 0;
}

void conf_var::register_var( char const *name ) {
  map_type::const_iterator const i = map_ref().find( name );
  if ( i != map_ref().end() ) {
    internal_error
      << "conf_var::register_var(): \"" << name
      << "\" registered more than once" << report_error;
  }
  map_ref()[ name ] = 0;
}

void conf_var::reset_all() {
  map_type &m = map_ref();
  TRANSFORM_EACH( map_type, m, i )
    if ( i->second )
      i->second->reset();
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
