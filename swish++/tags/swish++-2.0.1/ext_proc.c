/*
**	SWISH++
**	ext_proc.c
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
#include <cstdlib>
#include <sys/wait.h>
#ifndef WEXITSTATUS
#	define WEXITSTATUS(stat_val) ( (unsigned)(stat_val) >> 8 )
#endif
#ifndef WIFEXITED
#	define WIFEXITED(stat_val) ( ( (stat_val) & 255 ) == 0 )
#endif
#include <unistd.h>

// local
#include "config.h"
#include "ext_proc.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

extern char const	*me;

//*****************************************************************************
//
// SYNOPSIS
//
	ext_proc_map::ext_proc_map()
//
// DESCRIPTION
//
//	Construct (initialize) an ext_proc_map.  The entries in
//	ext_proc_table[] MUST be full paths.
//
// SEE ALSO
//
//	extract.c	do_file()
//
//*****************************************************************************
{
	struct ext_proc {
		char const *extension;
		char const *undo;
		char const *redo;
	};
	static ext_proc const ext_proc_table[] = {
		"Z",	"/usr/bin/uncompress",		"/usr/bin/compress",
		"gz",	"/usr/local/bin/gunzip",	"/usr/local/bin/gzip",
		0
	};

	for ( register ext_proc const *e = ext_proc_table; e->extension; ++e )
		map_[ e->extension ] = value_type( e->undo, e->redo );
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool process_file( char const *path_name, char const *file_name )
//
// DESCRIPTION
//
//	Process a file with another command by forking, execing, and waiting
//	for it to complete.
//
// RETURN VALUE
//
//	Returns true only if the file was processed successfully.
//
// SEE ALSO
//
//	exec(2), fork(2), wait(2), wstat(5)
//
//*****************************************************************************
{
	static pid_t const pid_error = STATIC_CAST( pid_t )( -1 );
	pid_t child_pid;
	int attempt_count = 0;

	while ( ( child_pid = ::fork() ) == pid_error ) {
		//
		// Try to fork a few times before giving up in case the system
		// is temporarily busy.
		//
		if ( ++attempt_count > Fork_Attempts ) {
			cerr << me << ": could not fork" << endl;
			::exit( 2 );
		}
		::sleep( Fork_Sleep );
	}

	if ( !child_pid ) {				// child process
		::execl( path_name, path_name, file_name, 0 );
		cerr << me << ": can not exec " << path_name << endl;
		::exit( 3 );
	}

	int child_stat;
	if ( ::wait( &child_stat ) == pid_error ) {
		cerr << me << ": error waiting for child process" << endl;
		::exit( 4 );
	}
	if ( WIFEXITED( child_stat ) && WEXITSTATUS( child_stat ) ) {
		cerr << me << ": child process terminated abnormally" << endl;
		::exit( 5 );
	}
	return true;
}
