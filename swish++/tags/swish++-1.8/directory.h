/*
**	SWISH++
**	directory.h
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

#ifndef directory_H
#define directory_H

// standard
#include <sys/stat.h>
#include <sys/types.h>

// local
#include "fake_ansi.h"

extern bool		follow_symbolic_links;
extern struct stat	stat_buf;		// somplace to do a stat(2) in

void		do_directory( char const *path );

//
// File test functions.  Those that do not take an argument operate on the last
// file stat'ed.
//
inline bool	is_directory() {
			return ( stat_buf.st_mode & S_IFMT ) == S_IFDIR;
		}

inline bool	is_directory( char const *path ) {
			return	::stat( path, &stat_buf ) != -1
				&& is_directory();
		}

inline bool	is_directory( string const &path ) {
			return is_directory( path.c_str() );
		}

inline bool	is_plain_file() {
			return ( stat_buf.st_mode & S_IFMT ) == S_IFREG;
		}

inline bool	is_plain_file( char const *path ) {
			return	::stat( path, &stat_buf ) != -1
				&& is_plain_file();
		}

inline bool	is_plain_file( string const &path ) {
			return is_plain_file( path.c_str() );
		}

inline bool	is_symbolic_link() {
			return ( stat_buf.st_mode & S_IFLNK ) == S_IFLNK;
		}

inline bool	is_symbolic_link( char const *path ) {
			return	::lstat( path, &stat_buf ) != -1
				&& is_symbolic_link();
		}

inline bool	is_symbolic_link( string const &path ) {
			return is_symbolic_link( path.c_str() );
		}

#endif	/* directory_H */
