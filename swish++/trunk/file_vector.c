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
#include <cerrno>
#include <fcntl.h>			/* for open(2), O_RDONLY, etc */
#ifndef	WIN32
#include <ctime>			/* needed by sys/resource.h */
#include <sys/time.h>			/* needed by FreeBSD systems */
#include <sys/mman.h>			/* for mmap(2) */
#include <sys/resource.h>		/* for get/setrlimit(2) */
#endif
#include <sys/stat.h>			/* for stat(2) */
#include <unistd.h>			/* for close(2) */
#if	defined( MULTI_THREADED ) && defined( RLIMIT_VMEM )
#include <pthread.h>
#endif

// local
#include "file_vector.h"
#include "platform.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

#ifdef	RLIMIT_VMEM			/* SVR4 */
//*****************************************************************************
//
// SYNOPSIS
//
	extern "C" void max_out_limits()
//
// DESCRIPTION
//
//	Max-out our memory-mapped address space since we can potentially
//	mmap(2) very large files.
//
// NOTE
//
//	This function is declared extern "C" since it is called via the C
//	library function pthread_once() (if MULTI_THREADED is defined) and,
//	because it's a C function, it expects C linkage.
//
//*****************************************************************************
{
	struct rlimit r;
	::getrlimit( RLIMIT_VMEM, &r );
	r.rlim_cur = r.rlim_max;
	::setrlimit( RLIMIT_VMEM, &r );
}
#endif	/* RLIMIT_VMEM */

//*****************************************************************************
//
// SYNOPSIS
//
	void file_vector::close()
//
// DESCRIPTION
//
//	Munmaps and closes a file previously opened and mmapped by
//	file_vector::open().
//
// SEE ALSO
//
//	close(2), file_vector::open(3), munmap(2)
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
		::munmap( static_cast<char*>( addr_ ), size_ );
	if ( fd_ )
		::close( fd_ );
#endif
	init();
}

//*****************************************************************************
//
// SYNOPSIS
//
	void file_vector::init()
//
// DESCRIPTION
//
//	Initialize an instance of file_vector.
//
//*****************************************************************************
{
#ifdef	RLIMIT_VMEM				/* SVR4 */
	//
	// This OS defines a separate resource limit for memory-mapped address
	// space as opposed to data, stack, or heap space.  Anyway, we want to
	// max it out so we can mmap(2) very large files.
	//
#ifdef	MULTI_THREADED
	static pthread_once_t max_out = PTHREAD_ONCE_INIT;
	::pthread_once( &max_out, max_out_limits );
#else
	static bool maxed_out;
	if ( !maxed_out ) {
		max_out_limits();
		maxed_out = true;
	}
#endif	/* MULTI_THREADED */
#endif	/* RLIMIT_VMEM */

	size_ = 0;
	fd_ = 0;
#ifdef	WIN32
	map_ = 0;
#endif
	addr_ = 0;
	errno_ = 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool file_vector::open( char const *path, ios::open_mode mode )
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
		errno_ = 1;
		return false;
	}

	if ( ( size_ = ::GetFileSize( fd_, 0 ) ) == 0xFFFFFFFF ) {
		errno_ = 2;
		return false;
	}

	map_ = ::CreateFileMapping( fd_, NULL,
		mode & ios::out ? PAGE_READWRITE : PAGE_READONLY,
		0, 0, NULL
	);
	if ( !map_ ) {
		errno_ = 3;
		return false;
	}

	addr_ = ::MapViewOfFile( map_,
		mode & ios::out ? FILE_MAP_WRITE : FILE_MAP_READ,
		0, 0, 0
	);
	if ( !addr_ ) {
		errno_ = 4;
		return false;
	}
#else
	struct stat stat_buf;
	if ( ::stat( path, &stat_buf ) == -1 ) {
		errno_ = errno;
		return false;
	}

	int flags = 0;
	if ( mode & ios::in  )	flags |= O_RDONLY;
	if ( mode & ios::out )	flags |= O_WRONLY;

	if ( ( fd_ = ::open( path, flags ) ) == -1 ) {
		fd_ = 0;
		errno_ = errno;
		return false;
	}

	int prot = PROT_NONE;
	if ( mode & ios::in  )	prot |= PROT_READ;
	if ( mode & ios::out )	prot |= PROT_WRITE;

	if ( !( size_ = stat_buf.st_size ) ) {
#ifdef	ENODATA
		errno_ = ENODATA;
#else
		//
		// For BSD systems, we're forced to use something other than
		// ENODATA since it's not available.  Unfortunately, there
		// isn't something good to use: EINVAL is the least bad.
		//
		errno_ = EINVAL;
#endif
		return false;
	}

	addr_ = ::mmap( 0, stat_buf.st_size, prot, MAP_SHARED, fd_, 0 );
	if ( addr_ == MAP_FAILED ) {
		addr_ = 0;
		errno_ = errno;
		return false;
	}

#endif	/* WIN32 */

	return true;
}
