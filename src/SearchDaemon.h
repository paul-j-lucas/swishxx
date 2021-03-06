/*
**      SWISH++
**      src/SearchDaemon.h
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

#ifndef SearchDaemon_H
#define SearchDaemon_H

// local
#include "conf_enum.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %SearchDaemon is-a conf_enum containing the whether we're a search daemon
 * and, if so, which kind: TCP, Unix domain, or both.
 *
 * This is the same as search's \c -b command-line option.
 */
class SearchDaemon : public conf_enum {
public:
  SearchDaemon() : conf_enum( "SearchDaemon", legal_values_ ) { }
  CONF_ENUM_ASSIGN_OPS( SearchDaemon )

private:
  static char const *const legal_values_[];
};

extern SearchDaemon daemon_type;

///////////////////////////////////////////////////////////////////////////////

#endif /* SearchDaemon_H */
/* vim:set et sw=2 ts=2: */
