/*
**      SWISH++
**      src/SearchDaemon.h
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

#ifdef SEARCH_DAEMON

#ifndef SearchDaemon_H
#define SearchDaemon_H

// local
#include "conf_enum.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class SearchDaemon : public conf_enum
//
// DESCRIPTION
//
//      A SearchDaemon is-a conf_enum containing the whether we're a search
//      daemon and, if so, which kind: TCP, Unix domain, or both.
//
//      This is the same as search's -b command-line option.
//
//*****************************************************************************
{
public:
    SearchDaemon() : conf_enum( "SearchDaemon", legal_values_ ) { }
    CONF_ENUM_ASSIGN_OPS( SearchDaemon )
private:
    static char const *const legal_values_[];
};

extern SearchDaemon daemon_type;

#endif /* SearchDaemon_H */

#endif /* SEARCH_DAEMON */
/* vim:set et sw=4 ts=4: */
