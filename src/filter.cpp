/*
**      SWISH++
**      src/filter.cpp
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
#include "filter.h"
#include "swishxx-config.h"
#include "util.h"

// standard
#include <cassert>
#include <cstdlib>                      /* for system(3) */
#include <cstring>
#include <unistd.h>                     /* for sleep(3) */

using namespace std;

////////// local functions ////////////////////////////////////////////////////

/**
 * Escapes all characters in a filename for passing to a shell.
 *
 * @param s The string containing the filename to escape.  It is modified
 * in-place.
 */
static void escape_filename( string &s ) {
  string::size_type pos = 0;
  while ( (pos = s.find_first_of( ShellFilenameEscapeChars, pos ))
           != string::npos ) {
    s.insert( pos, 1, '\\' );
    pos += 2;
  } // while
}

/**
 * Unescapes all \c '\' characters in a filename for not passing to a shell.
 *
 * @param s The string containing the filename to unescape.  It is modified
 * in-place.
 */
static void unescape_filename( string &s ) {
  string::size_type pos = 0;
  while ( (pos = s.find( '\\', pos )) != string::npos )
    s.erase( pos++, 1 );
}

////////// member functions ///////////////////////////////////////////////////

char const* filter::exec() const {
  assert( !command_.empty() );

  int exit_code;
  int attempt_count = 0;
  while ( (exit_code = ::system( command_.c_str() )) == -1 ) {
    //
    // Try a few times before giving up in case the system is temporarily busy.
    //
    if ( ++attempt_count > Fork_Attempts )
      return nullptr;
    ::sleep( Fork_Sleep );
  }
  return exit_code ? nullptr : target_file_name_.c_str();
}

char const *filter::substitute( char const *file_name ) {
  string esc_file_name( file_name );
  escape_filename( esc_file_name );
  //
  // Determine the base name of the file in case we need it for 'b' or 'B'
  // substitutions.
  //
  auto const base_name = pjl_basename( esc_file_name.c_str() );

  //
  // For this kind of string manipulation, the C++ string class is much easier
  // to use than the C str*() functions.
  //
  auto target_pos = string::npos;

  command_ = command_template_;
  string::size_type pos = 0;
  while ( (pos = command_.find_first_of( "%@", pos )) != string::npos ) {
    if ( pos + 1 >= command_.length() ) {
      //
      // The % or @ is the last character in the command so it can't be
      // substituted.  This is weird, but be lenient by assuming the user knows
      // what s/he's doing and simply stop rather than return an error.
      //
      break;
    }
    if ( command_[ pos ] == command_[ pos + 1 ] ) {
      //
      // We encountered either a %% or @@ to represent a literal % or @,
      // respectively.  Simply erase one of them and skip any substitution.
      //
      command_.erase( pos++, 1 );
      continue;
    }
    if ( command_[ pos ] == '@' ) {
      //
      // We found the substitution that represents the target filename: make a
      // note.  Note that we don't have to check to see if we've already set
      // target_pos (meaning there was more than one @ substitution) because
      // that illegal situation would have been caught by
      // FilterFile::parse_value().
      //
      target_pos = pos;
      command_.erase( pos, 1 );
      continue;
    }

    //
    // Perform a substitution.
    //
    switch ( command_[ pos + 1 ] ) {

      case 'b':                         // basename of filename
        command_.replace( pos, 2, base_name );
        pos += ::strlen( base_name );
        break;

      case 'B': {                       // basename minus last extension
        string no_ext( base_name );
        no_ext.erase( no_ext.rfind( '.' ) );
        command_.replace( pos, 2, no_ext );
        pos += no_ext.length();
        break;
      }

      case 'e': {                       // filename extension
        string ext( esc_file_name );
        ext.erase( 0, ext.rfind( '.' ) );
        command_.replace( pos, 2, ext );
        pos += ext.length();
        break;
      }

      case 'E': {                       // second-to-last filename extension
        string ext( esc_file_name );
        string::size_type const x = ext.rfind( '.' );
        if ( x != string::npos ) {
          ext.erase( x );
          ext.erase( 0, ext.rfind( '.' ) );
          command_.replace( pos, 2, ext );
          pos += ext.length();
        }
        break;
      }

      case 'f':                         // entire filename
        command_.replace( pos, 2, esc_file_name );
        pos += esc_file_name.length();
        break;

      case 'F': {                       // filename minus last extension
        string no_ext( esc_file_name );
        no_ext.erase( no_ext.rfind( '.' ) );
        command_.replace( pos, 2, no_ext );
        pos += no_ext.length();
        break;
      }
    } // switch
  } // while

  //
  // This should never happen: the command template should have been checked by
  // FilterFile::parse_line() for the existence of an @.
  //
  assert( target_pos != string::npos );

  //
  // Find the first character that delimits the target file name (that is not
  // escaped).
  //
  pos = target_pos;
  while ( (pos = command_.find_first_of( ShellFilenameDelimChars, pos ))
          != string::npos ) {
    if ( command_[ pos - 1 ] == '\\' )
      ++pos;
    else
      break;
  } // while

  target_file_name_ = string( command_, target_pos, pos );
  //
  // Having shell meta-characters and whitespace automatically escaped was good
  // for executing the filter(s), but it's not good for actually opening the
  // file since no shell is involved.  Therefore, we must now unescape the
  // final file-name.
  //
  unescape_filename( target_file_name_ );
  return target_file_name_.c_str();
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
