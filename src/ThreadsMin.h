/*
**      SWISH++
**      src/ThreadsMin.h
**
**      Copyright (C) 1998  Paul J. Lucas
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

#ifdef WITH_SEARCH_DAEMON

#ifndef ThreadsMin_H
#define ThreadsMin_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class ThreadsMin : public conf<int>
//
// DESCRIPTION
//
//      A ThreadsMin is-a conf<int> containing the minimum number of threads
//      that will exist at any one time to service clients
//
//      This is the same as search's -t command-line option.
//
//*****************************************************************************
{
public:
    ThreadsMin() : conf<int>( "ThreadsMin", ThreadsMin_Default, 1 ) { }
    CONF_INT_ASSIGN_OPS( ThreadsMin )
};

extern ThreadsMin min_threads;

#endif /* ThreadsMin_H */

#endif /* WITH_SEARCH_DAEMON */
/* vim:set et sw=4 ts=4: */
