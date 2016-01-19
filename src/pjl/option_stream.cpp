/*
**      PJL C++ Library
**      option_stream.cpp
**
**      Copyright (C) 1999-2016  Paul J. Lucas
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
#include <cassert>
#include <cstdlib>                      /* for abort(3) */
#include <cstring>
#include <iterator>
#include <iostream>

#define GAVE_OPTION(OPT)  opts_given_[ static_cast<unsigned char>( OPT ) ]

using namespace std;

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

char const *const option_stream::arg_lone = "<LONE-ARGUMENT>";

option_stream::option_stream( int argc, char *argv[], spec const specs[],
                              ostream &err ) :
  argc_( argc ), argv_( argv ), specs_( specs ), err_( err ), argi_( 0 ),
  next_c_( nullptr ), state_( goodbit )
{
  ::memset( &opts_given_, 0, sizeof( opts_given_ ) );
}

/**
 * Checks that no options were given that are among the two given mutually
 * exclusive sets of short options.
 * Prints an error message and exits if any such options are found.
 */
void option_stream::check_mutually_exclusive() {
  spec const *opt_spec;
  char bad_opt;

  for ( opt_spec = specs_; opt_spec->short_name; ++opt_spec ) {
    if ( GAVE_OPTION( opt_spec->short_name ) && opt_spec->me_short_names ) {
      if ( opt_spec->me_short_names == arg_lone ) {
        for ( auto me_spec = specs_; me_spec->short_name; ++me_spec )
          if ( me_spec->short_name != opt_spec->short_name &&
               GAVE_OPTION( me_spec->short_name ) ) {
            bad_opt = me_spec->short_name;
            goto error;
          }
      } else {
        for ( auto me_opt = opt_spec->me_short_names; *me_opt; ++me_opt )
          if ( GAVE_OPTION( *me_opt ) ) {
            bad_opt = *me_opt;
            goto error;
          }
      }
    }
  } // for
  return;

error:
  error()
    << "--" << opt_spec->long_name << "/-" << opt_spec->short_name << " and "
    << "--" << get_long_opt( bad_opt ) << "/-" << bad_opt
    << " options are mutually exclusive" << endl;
}

/**
 * For each option in \a opts that was given, checks that at least one of
 * \a req_opts was also given.
 * If not, prints an error message and exits.
 */
void option_stream::check_required() {
  for ( auto opt = specs_; opt->short_name; ++opt ) {
    if ( GAVE_OPTION( opt->short_name ) &&
         opt->req_short_names && *opt->req_short_names ) {
      for ( auto req_opt = opt->req_short_names; *req_opt; ++req_opt )
        if ( GAVE_OPTION( *req_opt ) )
          return;
      bool const reqs_multiple = opt->req_short_names[1];
      error()
        << "--" << opt->long_name << "/-" << opt->short_name << " requires "
        << (reqs_multiple ? "one of " : "") << "the -" << opt->req_short_names
        << " option" << (reqs_multiple ? "s" : "")
        << " to be given also" << endl;
    }
  } // for
}

/**
 * Sets the failbit and writes an error message preamble.
 *
 * @return Returns the error stream.
 */
ostream& option_stream::error() {
  class program_name {
  public:
    program_name( char const *argv0 ) : argv0_( basename( argv0 ) ) { }

    operator char const*() const {
      return argv0_;
    }

  private:
    static char const* basename( char const *path ) {
      char const *const slash = ::strrchr( path, '/' );
      return slash ? slash + 1 : path;
    }

    char const *const argv0_;
  };
  static program_name const me( argv_[0] );

  state_ |= failbit;
  err_ << me << ": error: ";
  return err_;
}

/**
 * Gets the corresponding name of the long option for the given short option.
 *
 * @param short_opt The short option to get the corresponding long option for.
 * @return Returns the said option.
 */
char const* option_stream::get_long_opt( char short_opt ) const {
  for ( auto s = specs_; s->long_name; ++s )
    if ( s->short_name == short_opt )
      return s->long_name;
  assert( false );
}

///////////////////////////////////////////////////////////////////////////////

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
      os.error() << '"';
      ::copy( arg, end, ostream_iterator<char>( os.err_, "" ) );
      os.err_ << "\" is ambiguous\n";
      goto option_error;
    } // for

    if ( !found ) {
      os.error() << '"';
      ::copy( arg, end, ostream_iterator<char>( os.err_, "" ) );
      os.err_ << "\" unrecognized\n";
      goto option_error;
    }

    //
    // Got a long option: now see about its argument.
    //
    arg = nullptr;
    switch ( found->arg_type ) {
      case option_stream::arg_none:
        if ( *end == '=' ) {
          os.error() << '"' << found->long_name << "\" takes no argument\n";
          goto option_error;
        }
        break;
      case option_stream::arg_req:
      case option_stream::arg_opt:
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
    os.error() << '"' << *arg << "\" unrecognized\n";
    goto option_error;
  }

  //
  // Found the short option: now see about its argument.
  //
  switch ( found->arg_type ) {

    case option_stream::arg_none:
      //
      // Set next_c_ in case the user gave a grouped set of short options (see
      // the comment near the beginning) so we can pick up where we left off on
      // the next call.
      //
      os.next_c_ = arg + 1;
      arg = nullptr;
      break;

    case option_stream::arg_req:
    case option_stream::arg_opt:
      //
      // Reset next_c_ since an option with either a required or optional
      // argument terminates a grouped set of options.
      //
      os.next_c_ = nullptr;

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
  if ( (arg && *arg) || found->arg_type != option_stream::arg_req ) {
    o.short_name_ = found->short_name;
    o.arg_ = arg;
    os.opts_given_[ static_cast<unsigned char>( o.short_name_ ) ] = true;
    return os;
  }
  os.error() << '"';
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
  os.state_ |= option_stream::eofbit;
  if ( !os.fail() )
    os.check_mutually_exclusive();
  if ( !os.fail() )
    os.check_required();
  return os;
bad_spec:
  os.error() << "invalid option argument spec: " << found->arg_type << '\n';
  ::abort();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

//#define TEST_OPTION_STREAM
#ifdef TEST_OPTION_STREAM

using namespace PJL;

int main( int argc, char *argv[] ) {
  static option_stream::spec const spec[] = {
    { "n-no-arg",   0, 'n', "", "" },
    { "m-no-arg",   0, 'm', "", "" },
    { "r-req-arg",  1, 'r', "", "" },
    { "o-opt-arg",  2, 'o', "", "" },
    { nullptr, 0, '\0', "", "" },
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
