/*
**	PJL C++ Library
**	option_stream.c
**
**	Copyright (C) 1999  Paul J. Lucas
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
#include <cstring>
#include <iostream>

// local
#include "option_stream.h"

#define	ERROR	os.err_ << os.argv_[0] << ": error: option "

//*****************************************************************************
//
// SYNOPSIS
//
	void option_stream::option::copy( option const &o )
//
// DESCRIPTION
//
//	Copy an option_stream to ourselves.  This is a utility function for the
//	copy constructor and assignment operator.
//
// PARAMETERS
//
//	o	The option_stream to copy from.
//
//*****************************************************************************
{
	c_ = o.c_;
	::strcpy( arg_ = new char[ ::strlen( o.arg_ ) + 1 ], o.arg_ );
	am_copy_ = true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	option_stream::option_stream(
		int argc, char *argv[], spec const specs[]
	)
//
// DESCRIPTION
//
//	Construct (initialize) an option_stream.
//
// PARAMETERS
//
//	argc	The number of arguments.
//
//	argv	A vector of the arguments; argv[argc] is null.
//
//	specs	A vector of option specifications, i.e., the options that are
//		allowed and their argument parameters.
//
//*****************************************************************************
	: argc_( argc ), argv_( argv ), specs_( specs ), err_( cerr ),
	  end_( false ), index_( 0 ), next_c_( 0 )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
	option_stream::option_stream(
		int argc, char *argv[], spec const specs[], ostream &err
	)
//
// DESCRIPTION
//
//	Construct (initialize) an option_stream.
//
// PARAMETERS
//
//	argc	The number of arguments.
//
//	argv	A vector of the arguments; argv[argc] is null.
//
//	specs	A vector of option specifications, i.e., the options that are
//		allowed and their argument parameters.
//
//	err	The ostream to emit error messages to.
//
//*****************************************************************************
	: argc_( argc ), argv_( argv ), specs_( specs ), err_( err ),
	  end_( false ), index_( 0 ), next_c_( 0 )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
	option_stream& operator>>( option_stream &os, option_stream::option &o )
//
// DESCRIPTION
//
//	Parse and extract an option from an option stream (argv values).
//	Options begin with either a '-' for short options or a "--" for long
//	options.  Either a '-' or "--" by itself explicitly ends the options;
//	however, the difference is that '-' is returned as the first non-option
//	whereas "--" is skipped entirely.
//
//	When there are no more options, the option_stream converts to bool as
//	false.  The option_stream's shift() member is the number of options
//	parsed which the caller can use to adjust argc and argv.
//
//	Short options can take an argument either as the remaining characters
//	of the same argv or in the next argv unless the next argv looks like an
//	option by beginning with a '-').
//
//	Long option names can be abbreviated so long as the abbreviation is
//	unambiguous.  Long options can take an argument either directly after a
//	'=' in the same argv or in the next argv (but without an '=') unless
//	the next argv looks like an option by beginning with a '-').
//
// PARAMETERS
//
//	os	The option_stream to extract options from
//
//	o	The option to deposit into.
//
// RETURN VALUE
//
//	Returns the passed-in option_stream.
//
//*****************************************************************************
{
	register char *arg;
	register option_stream::spec const *s, *found = 0;

	o.c_ = '\0';

	if ( os.next_c_ && *os.next_c_ ) {
		//
		// We left off within a grouped set of short options taking no
		// arguments, i.e., instead of -a -b -c, the user did -abc and
		// next_c_ points to a character past 'a'.
		//
		arg = os.next_c_;
		goto short_option;
	}

	if ( ++os.index_ >= os.argc_ )		// no more arguments
		goto the_end;
	arg = os.argv_[ os.index_ ];
	if ( !arg || *arg != '-' || !*++arg )	// no more options
		goto the_end;

	if ( *arg == '-' ) {
		if ( !*++arg ) {		// "--": end of options
			++os.index_;
			goto the_end;
		}

		//
		// Got the start of a long option: find the last character of
		// its name.
		//
		char *end;
		for ( end = arg; *end && *end != '='; ++end ) ;

		//
		// Now look through the options table for a match.
		//
		for ( s = os.specs_; s->long_name; ++s ) {
			int const len = end - arg;
			if ( ::strncmp( arg, s->long_name, len ) )
				continue;
			if ( ::strlen( s->long_name ) == len ) {
				found = s;	// exact match
				break;
			}
			if ( !found ) {
				found = s;	// partial match
				continue;
			}
			ERROR << '"';
			::copy( arg, end, ostream_iterator<char>(os.err_, "") );
			os.err_ << "\" is ambiguous\n";
			return os;
		}
		if ( !found ) {
			ERROR << '"';
			::copy( arg, end, ostream_iterator<char>(os.err_, "") );
			os.err_ << "\" unrecognized\n";
			return os;
		}

		//
		// Got a long option: now see about its argument.
		//
		arg = 0;
		switch ( found->arg_type ) {

			case option_stream::no_arg:
				if ( *end == '=' ) {
					ERROR	<< '"' << found->long_name
						<< "\" takes no argument\n";
					goto set_arg;
				}
				break;

			case option_stream::req_arg:
			case option_stream::opt_arg:
				if ( *end == '=' ) {
					arg = ++end;
					break;
				}
				if ( ++os.index_ >= os.argc_ )
					break;
				arg = os.argv_[ os.index_ ];
				if ( *arg == '-' ) {
					//
					// The next argv looks like an option
					// so assume it is one and that there
					// is no argument for this option.
					//
					arg = 0;
				}
				break;

			default:
				goto bad_spec;
		}

		if ( !arg && found->arg_type == option_stream::req_arg )
			ERROR	<< '"' << found->long_name
				<< "\" requires an argument\n";
		else
			o.c_ = found->c;

		goto set_arg;
	}

short_option:
	//
	// Got a single '-' for a short option: look for it in the specs.
	//
	for ( s = os.specs_; s->c; ++s )
		if ( *arg == s->c ) {
			found = s;
			break;
		}
	if ( !found ) {
		ERROR << '"' << *arg << "\" unrecognized\n";
		return os;
	}

	//
	// Found the short option: now see about its argument.
	//
	switch ( found->arg_type ) {

		case option_stream::no_arg:
			//
			// Set next_c_ in case the user gave a grouped set of
			// short options (see the comment near the beginning)
			// so we can pick up where we left off on the next
			// call.
			//
			os.next_c_ = arg + 1;
			arg = 0;
			break;

		case option_stream::req_arg:
		case option_stream::opt_arg:
			//
			// Reset next_c_ since an option with either a required
			// or optional argument terminates a grouped set of
			// options.
			//
			os.next_c_ = 0;

			if ( !*++arg ) {
				//
				// The argument, if any, is given in the next
				// argv.
				//
				if ( ++os.index_ >= os.argc_ )
					break;
				arg = os.argv_[ os.index_ ];
				if ( *arg == '-' ) {
					//
					// The next argv looks like an option
					// so assume it is one and that there
					// is no argument for this option.
					//
					arg = 0;
				}
			}
			break;

		default:
			goto bad_spec;
	}

	if ( !arg && found->arg_type == option_stream::req_arg )
		ERROR << '"' << found->c << "\" requires an argument\n";
	else
		o.c_ = found->c;

set_arg:
	o.arg_ = arg;
	return os;
the_end:
	os.end_ = true;
	return os;
bad_spec:
	ERROR << "invalid option argument spec: " << found->arg_type << "\n";
	::abort();
}

//*****************************************************************************

/*#define TEST_OPTION_STREAM /**/
#ifdef	TEST_OPTION_STREAM

int main( int argc, char *argv[] ) {
	static option_stream::spec const spec[] = {
		"no-arg",	0, 'n',
		"req-arg",	1, 'r',
		"opt-arg",	2, 'o',
		0,
	};
	option_stream opt_in( argc, argv, spec );
	option_stream::option opt;
	while ( opt_in >> opt ) {
		switch ( opt ) {
			case 'n':
				cout << "got --no-arg\n";
				break;
			case 'r':
				cout << "got --req-arg=" << opt.arg() << '\n';
				break;
			case 'o':
				cout << "got --opt-arg=";
				cout << (opt.arg() ? opt.arg() : "(null)") << endl;
				break;
			default:
				cout << "got weird=" << (int)(char)opt << endl;
		}
	}
	cout << "shift=" << opt_in.shift() << '\n';

	argc -= opt_in.shift();
	argv += opt_in.shift();

	cout << "argc=" << argc << endl;
	cout << "first non-option=" << (argv[0] ? argv[0] : "(null)") << endl;
}

#endif	/* TEST_OPTION_STREAM */
