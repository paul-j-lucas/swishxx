/*
**      PJL C++ Library
**      option_stream.cpp
**
**      Copyright (C) 1999-2015  Paul J. Lucas
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
#include "option_stream.h"

// standard
#include <algorithm>                    /* for copy() */
#include <cstdlib>                      /* for abort(3) */
#include <cstring>
#include <iterator>
#include <iostream>

#define ERROR os.err_ << os.argv_[0] << ": error: option "

using namespace std;

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

option_stream::option_stream( int argc, char *argv[], spec const specs[],
                              ostream &err ) :
  argc_( argc ), argv_( argv ), specs_( specs ), err_( err ), argi_( 0 ),
  next_c_( 0 ), end_( false )
{
  // do nothing else
}

option_stream& operator>>( option_stream &os, option_stream::option &o ) {
  char *arg;
  option_stream::spec const *s, *found = nullptr;
  bool was_short_option = false;

  o.short_name_ = '\0';

  if ( os.next_c_ && *os.next_c_ ) {
    //
    // We left off within a grouped set of short options taking no arguments,
    // i.e., instead of -a -b -c, the user did -abc and next_c_ points to a
    // character past 'a'.
    //
    arg = os.next_c_;
    goto short_option;
  }

  if ( ++os.argi_ >= os.argc_ )         // no more arguments
    goto the_end;
  arg = os.argv_[ os.argi_ ];
  if ( !arg || *arg != '-' || !*++arg ) // no more options
    goto the_end;

  if ( *arg == '-' ) {
    if ( !*++arg ) {                    // "--": end of options
      ++os.argi_;
      goto the_end;
    }
    was_short_option = false;

    //
    // Got the start of a long option: find the last character of its name.
    //
    char *end;
    for ( end = arg; *end && *end != '='; ++end )
      ;

    //
    // Now look through the options table for a match.
    //
    for ( s = os.specs_; s->long_name; ++s ) {
      size_t const len = static_cast<size_t>( end - arg );
      if ( ::strncmp( arg, s->long_name, len ) != 0 )
        continue;
      if ( ::strlen( s->long_name ) == len ) {
        found = s;                      // exact match
        break;
      }
      if ( !found ) {
        found = s;                      // partial match
        continue;
      }
      ERROR << '"';
      ::copy( arg, end, ostream_iterator<char>(os.err_, "") );
      os.err_ << "\" is ambiguous\n";
      goto option_error;
    } // for

    if ( !found ) {
      ERROR << '"';
      ::copy( arg, end, ostream_iterator<char>(os.err_, "") );
      os.err_ << "\" unrecognized\n";
      goto option_error;
    }

    //
    // Got a long option: now see about its argument.
    //
    arg = nullptr;
    switch ( found->arg_type ) {
      case option_stream::os_arg_none:
        if ( *end == '=' ) {
          ERROR << '"' << found->long_name << "\" takes no argument\n";
          goto option_error;
        }
        break;
      case option_stream::os_arg_req:
      case option_stream::os_arg_opt:
        if ( *end == '=' ) {
          arg = ++end;
          break;
        }
        goto next;
      default:
        goto bad_spec;
    } // switch

    goto check_arg;
  }

short_option:
  was_short_option = true;
  //
  // Got a single '-' for a short option: look for it in the specs.
  //
  for ( s = os.specs_; s->short_name; ++s ) {
    if ( *arg == s->short_name ) {
      found = s;
      break;
    }
  } // for
  if ( !found ) {
    ERROR << '"' << *arg << "\" unrecognized\n";
    goto option_error;
  }

  //
  // Found the short option: now see about its argument.
  //
  switch ( found->arg_type ) {

    case option_stream::os_arg_none:
      //
      // Set next_c_ in case the user gave a grouped set of short options (see
      // the comment near the beginning) so we can pick up where we left off on
      // the next call.
      //
      os.next_c_ = arg + 1;
      arg = nullptr;
      break;

    case option_stream::os_arg_req:
    case option_stream::os_arg_opt:
      //
      // Reset next_c_ since an option with either a required or optional
      // argument terminates a grouped set of options.
      //
      os.next_c_ = 0;

      if ( !*++arg ) {
        //
        // The argument, if any, is given in the next argv.
        //
next:   if ( ++os.argi_ >= os.argc_ )
          break;
        arg = os.argv_[ os.argi_ ];
        if ( *arg == '-' ) {
          //
          // The next argv looks like an option so assume it is one and that
          // there is no argument for this option.
          //
          arg = nullptr;
        }
      }
      break;

    default:
      goto bad_spec;
  } // switch

check_arg:
  if ( (arg && *arg) || found->arg_type != option_stream::os_arg_req ) {
    o.short_name_ = found->short_name;
    o.arg_ = arg;
    return os;
  }
  ERROR << '"';
  if ( was_short_option )
    os.err_ << found->short_name;
  else
    os.err_ << found->long_name;
  os.err_ << "\" requires an argument\n";
  // fall through
option_error:
  o.arg_ = nullptr;
  return os;
the_end:
  os.end_ = true;
  return os;
bad_spec:
  ERROR << "invalid option argument spec: " << found->arg_type << '\n';
  ::abort();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

//#define TEST_OPTION_STREAM
#ifdef TEST_OPTION_STREAM

using namespace PJL;

int main( int argc, char *argv[] ) {
  static option_stream::spec const spec[] = {
    "n-no-arg",   0, 'n',
    "m-no-arg",   0, 'm',
    "r-req-arg",  1, 'r',
    "o-opt-arg",  2, 'o',
    nullptr,
  };
  option_stream opt_in( argc, argv, spec );
  option_stream::option opt;
  while ( opt_in >> opt ) {
    switch ( opt ) {
      case 'm':
        cout << "got --m-no-arg\n";
        break;
      case 'n':
        cout << "got --n-no-arg\n";
        break;
      case 'r':
        cout << "got --r-req-arg=" << opt.arg() << '\n';
        break;
      case 'o':
        cout << "got --o-opt-arg=";
        cout << (opt.arg() ? opt.arg() : "(null)") << '\n';
        break;
      default:
        cout << "got weird=" << (int)(char)opt << '\n';
    }
  } // while
  cout << "shift=" << opt_in.shift() << '\n';

  argc -= opt_in.shift();
  argv += opt_in.shift();

  cout << "argc=" << argc << endl;
  cout << "first non-option=" << (argv[0] ? argv[0] : "(null)") << endl;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

#endif /* TEST_OPTION_STREAM */
/* vim:set et sw=2 ts=2: */
