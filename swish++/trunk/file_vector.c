/*
**	PJL C++ Library
**	file_vector.c
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
#include <fcntl.h>				/* for open(2), O_RDONLY, ... */
#ifndef	WIN32
#include <sys/mman.h>				/* for mmap(2) */
#endif
#include <sys/stat.h>				/* for stat(2) */
#include <unistd.h>				/* for close(2) */

// local
#include "fake_ansi.h"
#include "file_vector.h"			/* for *_CAST */

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void file_vector_base::close()
//
// DESCRIPTION
//
//	Munmaps and closes a file previously opened and mmapped by
//	file_vector_base::open().
//
// SEE ALSO
//
//	close(2), file_vector_base::open(3), munmap(2)
//
//*****************************************************************************
{
#ifdef	WIN32
	if ( addr_ )
		::UnmapViewOfFile( addr_ );
	if ( map_ )
		::CloseHandle( map_ );
	if ( fd_ )
		::CloseHandle( fd_ );
#else
	if ( addr_ )
		::munmap( STATIC_CAST( char* )( addr_ ), size_ );
	if ( fd_ )
		::close( fd_ );
#endif
	init();
}

//*****************************************************************************
//
// SYNOPSIS
//
	void file_vector_base::init()
//
// DESCRIPTION
//
//	Initialize an instance of file_vector_base.
//
//*****************************************************************************
{
	size_ = 0;
	fd_ = 0;
#ifdef	WIN32
	map_ = 0;
#endif
	addr_ = 0;
	error_ = 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool file_vector_base::open( char const *path, ios::open_mode mode )
//
// DESCRIPTION
//
//	Open and mmap a file.
//
// RETURN VALUE
//
//	Returns true only if the file was mmapped successfully.
//
// SEE ALSO
//
//	mmap(2), open(2), stat(2)
//
//*****************************************************************************
{
	close();

#ifdef	WIN32

	fd_ = ::CreateFile( path,
		mode & ios::out ? GENERIC_WRITE : GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
	if ( fd_ == INVALID_HANDLE_VALUE ) {
		fd_ = 0;
		error_ = 1;
		return false;
	}

	if ( ( size_ = ::GetFileSize( fd_, 0 ) ) == 0xFFFFFFFF ) {
		error_ = 2;
		return false;
	}

	map_ = ::CreateFileMapping( fd_, NULL,
		mode & ios::out ? PAGE_READWRITE : PAGE_READONLY,
		0, 0, NULL
	);
	if ( !map_ ) {
		error_ = 3;
		return false;
	}

	addr_ = ::MapViewOfFile( map_,
		mode & ios::out ? FILE_MAP_WRITE : FILE_MAP_READ,
		0, 0, 0
	);
	if ( !addr_ ) {
		error_ = 4;
		return false;
	}
#else
	struct stat stat_buf;
	if ( ::stat( path, &stat_buf ) == -1 ) {
		error_ = 1;
		return false;
	}

	int flags = 0;
	if ( mode & ios::in  )	flags |= O_RDONLY;
	if ( mode & ios::out )	flags |= O_WRONLY;

	if ( ( fd_ = ::open( path, flags ) ) == -1 ) {
		fd_ = 0;
		error_ = 2;
		return false;
	}

	int prot = PROT_NONE;
	if ( mode & ios::in  )	prot |= PROT_READ;
	if ( mode & ios::out )	prot |= PROT_WRITE;

	if ( stat_buf.st_size ) {
		addr_ = ::mmap( 0, stat_buf.st_size, prot, MAP_SHARED, fd_, 0 );
		if ( addr_ == REINTERPRET_CAST( caddr_t )( -1 ) ) {
			addr_ = 0;
			error_ = 3;
			return false;
		}
	}

	size_ = stat_buf.st_size;

#endif	/* WIN32 */

	return true;
}
