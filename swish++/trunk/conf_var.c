/*
**	SWISH++
**	conf_var.c
**
**	Copyright (C) 1998  Paul J. Lucas
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
#include <cctype>
#include <cstring>
#include <iostream>

// local
#include "config.h"
#include "conf_var.h"
#include "exit_codes.h"
#include "mmap_file.h"
#include "platform.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
using namespace PJL;
#endif

extern char const	*me;

int conf_var::current_config_file_line_no_ = 0;

//*****************************************************************************
//
// SYNOPSIS
//
	conf_var::conf_var( char const *var_name )
//
// DESCRIPTION
//
//	Construct a configuration variable by mapping a variable name to an
//	instance of a conf_var (really, an instance of some derived class).
//	Only a single instance of any given variable may exist.
//
// PARAMETERS
//
//	var_name	The name of the variable.
//
//*****************************************************************************
	: name_( var_name )
{
	conf_var *&var = map_ref()[ to_lower( name_ ) ];
	if ( var )
		internal_error
			<< "conf_var::conf_var(): \"" << name_
			<< "\" registered more than once\n" << report_error;
	var = this;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ conf_var::~conf_var()
//
// DESCRIPTION
//
//	Destructs a conf_var.  It is out-of-line only because it's virtual
//	(so its address is taken and put into the vtbl).
//
//*****************************************************************************
{
	// do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ conf_var::map_type& conf_var::map_ref()
//
// DESCRIPTION
//
//	Define and initialize (exactly once) a static data member for conf_var
//	and return a reference to it.  The reason for this function is to
//	guarantee that the map is initialized before its first use across all
//	translation units, something that would not guaranteed if it were a
//	static data member initialized at file scope.
//
//	We also load the map with all configuration variable names (in lower
//	case so variables in config files will be case-insensitive) so we can
//	tell the difference between a variable that doesn't exist (and that we
//	will complain about to the user) and one that simply isn't used in a
//	particular executable (and will be silently ignored).
//
// RETURN VALUE
//
//	Returns a reference to a static instance of an initialized map_type.
//
// SEE ALSO
//
//	Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++
//	Reference Manual."  Addison-Wesley, Reading, MA, 1990.  p. 19.
//
//*****************************************************************************
{
	static map_type m;
	static bool init;
	if ( !init ) {
		init = true;		// must set this before init_modules()
		init_mod_vars();	// defined in init_mod_vars.c
		static char const *const var_name_table[] = {
			"associatemeta",
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
#ifdef	SEARCH_DAEMON
			"group",
			"pidfile",
			"searchdaemon",
			"socketaddress",
			"socketfile",
			"socketqueuesize",
			"sockettimeout",
			"threadsmax",
			"threadsmin",
			"threadtimeout",
			"user",
#endif
			0,
		};
		for ( register char const *const *v = var_name_table; *v; ++v )
			m[ *v ] = 0;
	}
	return m;
}

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& conf_var::msg( ostream &o, char const *label )
//
// DESCRIPTION
//
//	Emit the standard message preamble to the given ostream, that is the
//	program name, a line number if a configuration file is being parsed,
//	and a label.
//
// PARAMETERS
//
//	o	The ostream to write the message to.
//
//	label	The label to emit, usually "error" or "warning".
//
// RETURN VALUE
//
//	Returns the ostream passed in.
//
//*****************************************************************************
{
	o << me;
	if ( current_config_file_line_no_ ) {
		//
		// This is set only by parse_line() during the parsing of a
		// configuration file.
		//
		o << ": config file line " << current_config_file_line_no_;
		current_config_file_line_no_ = 0;
	}
	return o << ": " << label << ": ";
}

//*****************************************************************************
//
// SYNOPSIS
//
	void conf_var::parse_const_value( char const *line )
//
// DESCRIPTION
//
//	Parse a line that can't be modified by simply copying it and calling
//	parse_value() on the copy.
//
//*****************************************************************************
{
	char *const line_copy = new_strdup( line );
	parse_value( line_copy );
	delete[] line_copy;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void conf_var::parse_file( char const *file_name )
//
// DESCRIPTION
//
//	Parse the lines in a configuration file setting variables accordingly.
//
// PARAMETERS
//
//	file_name	The name of the configuration file to parse.
//
//*****************************************************************************
{
	mmap_file const conf_file( file_name );
	if ( !conf_file ) {
		if ( !::strcmp( file_name, ConfigFile_Default ) ) {
			//
			// The configuration file couldn't be opened; however,
			// the file name is the default, so assume that none is
			// being used and simply return.
			//
			return;
		}
		cerr	<< "could not read configuration from \""
			<< file_name << "\"\n";
		::exit( Exit_Config_File );
	}

	register int line_no = 0;
	register mmap_file::const_iterator c = conf_file.begin();

	while ( c != conf_file.end() ) {
		//
		// Find the end of the line.
		//
		mmap_file::const_iterator const
			nl = find_newline( c, conf_file.end() );
		if ( nl == conf_file.end() )
			break;

		++line_no;
		//
		// See if the line is entirely whitespace optionally followed
		// by a comment starting with '#': if so, skip it.  If we don't
		// end up skipping it, leading whitespace will have been
		// skipped.
		//
		for ( ; c != nl; ++c ) {
			if ( is_space( *c ) )
				continue;
			if ( *c == '#' )
				goto next_line;
			break;
		}
		if ( c != nl ) {
			//
			// The line has something on it worth parsing further:
			// copy it (less leading and trailing whitespace) to a
			// modifyable buffer and null-terminate it to make that
			// task easier.
			//
			char buf[ 256 ];
			ptrdiff_t len = nl - c;
			::strncpy( buf, c, len );
			while ( len > 0 )
				if ( is_space( buf[ len - 1 ] ) )
					--len;
				else
					break;
			buf[ len ] = '\0';
			parse_line( buf, line_no );
		}
next_line:
		c = skip_newline( nl, conf_file.end() );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void conf_var::parse_line( char *line, int line_no )
//
// DESCRIPTION
//
//	Parse a non-comment or non-blank line from a the configuration file,
//	the first word of which is the variable name.  Look up the variable in
//	our map and delegate the parsing of the rest of the line to an instance
//	of a derived class that knows how to parse its own line format.
//
// PARAMETERS
//
//	line		A line from a configuration file to be parsed.
//
//	line_no		The line number of the line.
//
//*****************************************************************************
{
	current_config_file_line_no_ = line_no;
	::strtok( line, " \r\t" );		// just the variable name
	map_type::const_iterator const i = map_ref().find( to_lower( line ) );
	if ( i == map_ref().end() ) {
		warning() << '"' << line << "\" unrecognized; ignored\n";
		return;
	}
	if ( i->second ) {
		//
		// Chop off trailing newline and remove leading whitespace from
		// value.
		//
		register char *value = ::strtok( 0, "\r\n" );
		while ( *value && is_space( *value ) )
			++value;
		i->second->parse_value( value );
	} // else
	//
	//	This config. variable is not used by the current executable:
	//	silently ignore it.
	//
	current_config_file_line_no_ = 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void conf_var::register_var( char const *name )
//
// DESCRIPTION
//
//	Register a variable name for so that it can be used in a config. file.
//	Derived indexing modules use this function to register module-specific
//	variables.
//
// PARAMETERS
//
//	name	The name of the variable.
//
//*****************************************************************************
{
	map_type::const_iterator const i = map_ref().find( name );
	if ( i != map_ref().end() )
		internal_error
			<< "conf_var::register_var(): \"" << name
			<< "\" registered more than once\n" << report_error;
	map_ref()[ name ] = 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void conf_var::reset_all()
//
// DESCRIPTION
//
//	Reset all configuration variables to their default values.
//
//*****************************************************************************
{
	map_type &m = map_ref();
	TRANSFORM_EACH( map_type, m, i )
		if ( i->second )
			i->second->reset();
}
