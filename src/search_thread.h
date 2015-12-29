/*
**      SWISH++
**      src/search_thread.h
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

#ifndef search_thread_H
#define search_thread_H

// local
#include "pjl/thread_pool.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %search_thread is-a thread_pool::thread that performs a search based
 * on a query and returns the results.
 */
class search_thread : public PJL::thread_pool::thread {
public:
  search_thread( PJL::thread_pool &p ) : PJL::thread_pool::thread( p ) { }

  static unsigned socket_timeout;

private:
  // inherited
  thread* create( PJL::thread_pool &p ) const;
  void main( argument_type );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* search_thread_H */
/* vim:set et sw=2 ts=2: */
