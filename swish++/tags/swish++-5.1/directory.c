/*
**	SWISH++
**	directory.c
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
#include <cstring>
#include <iostream>
#include <queue>
#include <sys/types.h>			/* needed by dirent.h */
#include <dirent.h>

// local
#include "directory.h"
#include "DirectoriesReserve.h"
#include "my_set.h"
#include "platform.h"
#include "RecurseSubdirs.h"
#include "util.h"
#include "Verbosity.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

extern void		do_file( char const *file_name );

#ifdef	WIN32
//
// The directory separator character ('/' for Unix) is apparantly transformed
// into '\' for Windows by the intermediate Windows port of POSIX functions.
// However, in the case where '/' is inserted into a string and that string is
// printed, the mere printing won't do the transformation.  Hence, this file
// contains the one place in all of the SWISH++ code where we need to use '\'
// explicitly when compiling under Windows.
//
char const		Dir_Sep_Char = '\\';
#else
char const		Dir_Sep_Char = '/';
#endif

DirectoriesReserve	directories_reserve;
dir_list_type		dir_list;

#ifndef	PJL_NO_SYMBOLIC_LINKS
FollowLinks		follow_symbolic_links;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void check_add_directory( char const *dir_path )
//
// DESCRIPTION
//
//	Check to see if the given directory has been added to the list of
//	directories encountered: if not, add it.
//
// PARAMETERS
//
//	dir_path	The full path of a directory.  The string must point to
//			storage that will last for the duration of the program.
//
//*****************************************************************************
{
	static char_ptr_set dir_set;
	if ( dir_set.insert( dir_path ).second ) {
		if ( dir_list.empty() )
			dir_list.reserve( directories_reserve );
		dir_list.push_back( dir_path );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void do_directory( char const *dir_path )
//
// DESCRIPTION
//
//	Call do_file() for every file in the given directory; it will queue
//	subdirectories encountered that do no start with '.' and call
//	do_directory() on them.  It will not follow symbolic links unless the
//	-l command-line option was given on the command line.
//
//	This function uses a queue and recurses only once so as not to have too
//	many directories open concurrently.  This has the effect of indexing in
//	a breadth-first order rather than depth-first.
//
// PARAMETERS
//
//	dir_path	The full path of the directory of the files and
//			subdirectories to index.  The string must point to
//			storage that will last for the duration of the program.
//
//*****************************************************************************
{
	typedef queue< char const* > dir_queue_type;
	static dir_queue_type dir_queue;
	static int recursion;

	if ( verbosity > 1 ) {
		if ( verbosity > 2 ) cout << '\n';
		cout << dir_path << flush;
	}

#ifndef	PJL_NO_SYMBOLIC_LINKS
	if ( is_symbolic_link( dir_path ) && !follow_symbolic_links ) {
		if ( verbosity > 3 )
			cout << " (skipped: symbolic link)";
		if ( verbosity > 1 )
			cout << '\n';
		return;
	}
#endif

	DIR *const dir_p = ::opendir( dir_path );
	if ( !dir_p ) {
		if ( verbosity > 3 )
			cout << " (skipped: can not open)";
		if ( verbosity > 1 )
			cout << '\n';
		return;
	}

	if ( verbosity > 1 ) {
		if ( verbosity > 2 ) cout << ':';
		cout << '\n';
	}

	check_add_directory( dir_path );
	//
	// Have a buffer for the full path to a file in a directory.  For each
	// file, simply strcpy() the file name into place one character past
	// the '/'.
	//
	char path[ PATH_MAX + 1 ];
	::strcpy( path, dir_path );
	char *file = path + ::strlen( path );
	*file++ = Dir_Sep_Char;

	struct dirent const *dir_ent;
	while ( dir_ent = ::readdir( dir_p ) ) {
		if ( *dir_ent->d_name == '.' )		// skip dot files
			continue;
		::strcpy( file, dir_ent->d_name );
		if ( is_directory( path ) && recurse_subdirectories )
			dir_queue.push( ::strdup( path ) );
		else {
			// Note that do_file() is called in the case where
			// 'path' is a directory and recurse_subdirectories is
			// false.  This is OK since do_file() checks for and
			// only does plain files.  It's also desirable to call
			// do_file() so we don't have to repeat the code to
			// print verbose information for 'path'.
			//
			do_file( path );
		}
	}

	::closedir( dir_p );
	if ( recursion )
		return;

	////////// Do all subdirectories //////////////////////////////////////

	while ( !dir_queue.empty() ) {
		char const *const dir_path = dir_queue.front();
		dir_queue.pop();
		++recursion;
		do_directory( dir_path );
		--recursion;
	}
}
