/*
**      SWISH++
**      src/ThreadTimeout.h
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

#ifndef ThreadTimeout_H
#define ThreadTimeout_H

// local
#include "config.h"
#include "conf_int.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %ThreadTimeout is-a conf&lt;int&gt; containing the number of seconds until
 * an idle spare thread times out and destroys itself.
 *
 * This is the same as index's \c -o command-line option.
 */
class ThreadTimeout : public conf<int> {
public:
  ThreadTimeout() : conf<int>( "ThreadTimeout", ThreadTimeout_Default ) { }
  CONF_INT_ASSIGN_OPS( ThreadTimeout )
};

extern ThreadTimeout thread_timeout;

///////////////////////////////////////////////////////////////////////////////

#endif /* ThreadTimeout_H */
/* vim:set et sw=2 ts=2: */
