/*
**      SWISH++
**      src/ThreadsMax.h
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

#ifndef ThreadsMax_H
#define ThreadsMax_H

// local
#include "config.h"
#include "conf_unsigned.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %ThreadsMax is-a conf&lt;unsigned&gt; containing the maximum number of
 * threads that will exist at any one time to service clients
 *
 * This is the same as search's \c -T command-line option.
 */
class ThreadsMax : public conf<unsigned> {
public:
  ThreadsMax() : conf<unsigned>( "ThreadsMax", ThreadsMax_Default, 1 ) { }
  CONF_INT_ASSIGN_OPS( ThreadsMax )
};

extern ThreadsMax max_threads;

///////////////////////////////////////////////////////////////////////////////

#endif /* ThreadsMax_H */
/* vim:set et sw=2 ts=2: */
