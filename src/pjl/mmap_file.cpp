/*
**      PJL C++ Library
**      mmap_file.cpp
**
**      Copyright (C) 1998-2015  Paul J. Lucas
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
#include "mmap_file.h"

// standard
#include <cerrno>
#include <fcntl.h>                      /* for open(2), O_RDONLY, etc */
#include <time.h>                       /* needed by sys/resource.h */
#include <sys/time.h>                   /* needed by FreeBSD systems */
#include <sys/resource.h>               /* for get/setrlimit(2) */
#include <sys/stat.h>                   /* for stat(2) */
#include <unistd.h>                     /* for close(2) */
#if defined( MULTI_THREADED ) && defined( RLIMIT_VMEM )
#include <pthread.h>
#endif

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

#ifdef RLIMIT_VMEM                      /* SVR4 */
/**
 * Max-out our memory-mapped address space since we can potentially mmap(2)
 * very large files.
 *
 * This function is declared extern "C" since it is called via the C library
 * function pthread_once() (if MULTI_THREADED is defined) and, because it's a C
 * function, it expects C linkage.
 */
extern "C" void max_out_limits() {
  struct rlimit r;
  ::getrlimit( RLIMIT_VMEM, &r );
  r.rlim_cur = r.rlim_max;
  ::setrlimit( RLIMIT_VMEM, &r );
}
#endif /* RLIMIT_VMEM */

int mmap_file::behavior( behavior_type behavior ) const {
#ifndef __APPLE__
  //
  // As of Mac OS X 10.2.1 (Darwin 6.1), madvise(2) seems broken, so don't
  // bother.
  //
#ifdef HAVE_MADVISE
  if ( ::madvise( static_cast<caddr_t>( addr_ ), size_, behavior ) == -1 )
    return errno_ = errno;
#endif /* HAVE_MADVISE */
#endif /* __APPLE__ */
  return 0;
}

void mmap_file::close() {
  if ( addr_ )
    ::munmap( static_cast<char*>( addr_ ), size_ );
  if ( fd_ )
    ::close( fd_ );
  init();
}

void mmap_file::init() {
#ifdef RLIMIT_VMEM                     /* SVR4 */
  //
  // This OS defines a separate resource limit for memory-mapped address space
  // as opposed to data, stack, or heap space.  Anyway, we want to max it out
  // so we can mmap(2) very large files.
  //
#ifdef MULTI_THREADED
  static pthread_once_t max_out = PTHREAD_ONCE_INIT;
  ::pthread_once( &max_out, max_out_limits );
#else
  static bool maxed_out;
  if ( !maxed_out ) {
    max_out_limits();
    maxed_out = true;
  }
#endif /* MULTI_THREADED */
#endif /* RLIMIT_VMEM */

  size_ = 0;
  fd_ = 0;
  addr_ = nullptr;
  errno_ = 0;
}

bool mmap_file::open( char const *path, ios::openmode mode ) {
  close();

  struct stat stat_buf;
  if ( ::stat( path, &stat_buf ) == -1 ) {
    errno_ = errno;
    return false;
  }

  int flags = 0;
  if ( mode & ios::in  )  flags |= O_RDONLY;
  if ( mode & ios::out )  flags |= O_WRONLY;

  if ( (fd_ = ::open( path, flags )) == -1 ) {
    fd_ = 0;
    errno_ = errno;
    return false;
  }

  int prot = PROT_NONE;
  if ( mode & ios::in  )  prot |= PROT_READ;
  if ( mode & ios::out )  prot |= PROT_WRITE;

  if ( !(size_ = stat_buf.st_size) ) {
#ifdef ENODATA
    errno_ = ENODATA;
#else
    //
    // For BSD systems, we're forced to use something other than ENODATA since
    // it's not available.  Unfortunately, there isn't something good to use:
    // EINVAL is the least bad.
    //
    errno_ = EINVAL;
#endif /* ENODATA */
    return false;
  }

  addr_ = ::mmap( nullptr, size_, prot, MAP_SHARED, fd_, 0 );
  if ( addr_ == MAP_FAILED ) {
    addr_ = nullptr;
    errno_ = errno;
    return false;
  }

  return behavior( bt_normal ) == 0;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
