/*
**	SWISH++
**	filter.c
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
#include <cstdlib>				/* for abort(3C), system(3S) */
#include <cstring>
#include <unistd.h>				/* for sleep(3C) */

// local
#include "config.h"
#include "filter.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	char const* filter::exec() const
//
// DESCRIPTION
//
//	Filter a file with a command by by calling system(3C).
//
// RETURN VALUE
//
//	Returns true only if the file was filtered successfully.
//
// SEE ALSO
//
//	system(3C)
//
//*****************************************************************************
{
	if ( command_.empty() ) {
		//
		// This should never happen: substitute() was never called on
		// this filter.  If this happens, the programmer goofed.
		//
		cerr << "filter::exec(): command is empty" << endl;
		::abort();
	}
	int exit_code;
	int attempt_count = 0;
	while ( ( exit_code = ::system( command_.c_str() ) ) == -1 ) {
		//
		// Try a few times before giving up in case the system is
		// temporarily busy.
		//
		if ( ++attempt_count > Fork_Attempts )
			return 0;
		::sleep( Fork_Sleep );
	}
	return exit_code ? 0 : target_file_.c_str();
}

//*****************************************************************************
//
// SYNOPSIS
//
	char const *filter::substitute( char const *file_name )
//
// DESCRIPTION
//
//	Substitute the filename (or parts thereof) into our command template
//	whereever % or @ occurs.
//
// PARAMETERS
//
//	file_name	The name of the file to be substituted into the
//			command.
//
//*****************************************************************************
{
	//
	// For this kind of string manipulation, the C++ string class is much
	// easier to use than the C str*() functions.
	//
	string	ext, no_ext;
	int	target_pos = -1;

	command_ = command_template_;
	register int pos;
	while ( ( pos = command_.find_first_of( "%@" ) ) != string::npos ) {
		if ( pos + 1 >= command_.length() )
			break;
		if ( command_[ pos ] == '@' )
			target_pos = pos;
		switch ( command_[ pos + 1 ] ) {

			case 'E': // filename minus last extension
				if ( no_ext.empty() ) {
					no_ext = file_name;
					no_ext.erase( no_ext.rfind( '.' ) );
				}
				command_.replace( pos, 2, no_ext );
				break;

			case 'e': // filename extension
				if ( ext.empty() ) {
					ext = file_name;
					ext.erase( 0, ext.find( '.' ) );
				}
				command_.replace( pos, 2, ext );
				break;

			case 'f': // entire filename
				command_.replace( pos, 2, file_name );
				break;
		}
	}

	if ( target_pos == -1 ) {
		//
		// This should never happen: the command template should have
		// been checked by FilterExtension::parse_line() for the
		// existence of an @.  If this happens, the programmer goofed.
		//
		cerr << "filter::substitute(): target_post == -1" << endl;
		::abort();
	}

	target_file_ = string(
		command_, target_pos,
		command_.find_first_of( " &<>|", target_pos )
	);
	return target_file_.c_str();
}
