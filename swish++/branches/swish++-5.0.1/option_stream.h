/*
**	PJL C++ Library
**	option_stream.h
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

#ifndef	option_stream_H
#define	option_stream_H

// standard
#include <iostream>

#if 0
extern char const* dtoa( double );

#include "itoa.h"
#include "util.h"
#endif

#ifndef	PJL_NO_NAMESPACES
namespace PJL {
#else
#define	PJL /* nothing */
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	class option_stream
//
// DESCRIPTION
//
//	Given the traditional argc and argv for command-line arguments, extract
//	options from them following the stream model.
//
// RATIONALE
//
//	Why not use the standard getopt() or GNU's getopt_long()?  Because
//	neither are thread-safe and I needed a thread-safe version.
//
//*****************************************************************************
{
public:
	enum {
		no_arg	= 0,
		req_arg	= 1,
		opt_arg	= 2
	};
	struct spec {
		//
		// A spec gives a specfication for an option: its long name,
		// its argument type, and it's short option equivalent.  A
		// null-terminated array of specs is given to the option_stream
		// constructor.
		//
		// The arg_type takes on one of the above enum values, but is
		// not declared as an enum to allow the integer values to be
		// used as a shorthand.
		//
		// Regardless of whether the user enters a long or a short
		// option, it is the short option character that is returned to
		// the caller in order to be able to use a simple switch
		// statement to do different things depending on the option.
		//
		// If, for a given long option there is to be no short option
		// equivalent, then the caller has to make up bogus ones that
		// the user will never type.  Either the ASCII control or high
		// characters can be user for this.  The null character may not
		// be used.
		//
#if 0
		//
		// Ignore the class in this section.  I'm not sure I want to
		// implement default values for options that can have optional
		// arguments.  I left the code here in case I ever decide I
		// want to implement it.
		//
		class default_value {
		public:
			default_value( char v ) : val_( new char[2] ) {
				val_[0] = v;
				val_[1] = '\0';
			}

			default_value( char const *v = "" ) :
				val_( new_strdup( v ) ) { }

			default_value( long v ) :
				val_( new_strdup( ltoa( v ) ) ) { }

			default_value( double v ) :
				val_( new_strdup( dtoa( v ) ) ) { }

			default_value( default_value const &v ) :
				val_( new_strdup( v.val_ ) ) { }

			~default_value() { delete[] val_; }

			operator char*() const { return val_; }
		private:
			char *const val_;

			default_value& operator=( default_value const& );
		};
#endif

		char const	*long_name;
		short		arg_type;
		unsigned char	c;
#if 0
		default_value	def_arg;
#endif
	};

	option_stream(
		int argc, char *argv[], spec const[], std::ostream& = cerr
	);

	int	shift() const		{ return index_; }
	operator bool() const 		{ return !end_; }

	class option {
		//
		// An option is what is extracted from an option_stream.  Its
		// operator char() gives which option it is and arg() gives its
		// argument, if any.  For options that do not have an argument,
		// arg() returns the null pointer.
		//
		// An option may be copied in which case it has a private copy
		// of its argument.
		//
	public:
		option( char c = '\0', char *a = 0 ) :
			c_( c ), arg_( a ), am_copy_( false ) { }
		option( option const &o ) {
			if ( &o != this ) copy( o );
		}
		~option() {
			if ( am_copy_ ) delete[] arg_;
		}
		option& operator=( option const &o ) {
			if ( &o != this ) {
				if ( am_copy_ ) delete[] arg_;
				copy( o );
			}
			return *this;
		}

		char*	arg() const	{ return arg_; }
		operator char() const	{ return c_; }

		friend option_stream& operator>>( option_stream&, option& );
	private:
		char	c_;
		char*	arg_;
		bool	am_copy_;

		void	copy( option const& );
	};

	friend option_stream& operator>>( option_stream&, option& );
private:
	int		argc_;
	char**		argv_;
	spec const*	specs_;
	std::ostream&	err_;

	int		index_;
	char*		next_c_;
	bool		end_;
};

#ifndef	PJL_NO_NAMESPACES
}
#endif

#endif	/* option_stream_H */
