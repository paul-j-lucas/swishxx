/*
**      SWISH++
**      src/Group.cpp
**
**      Copyright (C) 2001-2015  Paul J. Lucas
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
#include "config.h"
#include "Group.h"
#include "exit_codes.h"
#include "util.h"

// standard
#include <sys/types.h>                  /* needed by FreeBSD systems */
#include <cstdlib>                      /* for exit(3) */
#include <grp.h>                        /* for getgrnam(3) */

using namespace std;

///////////////////////////////////////////////////////////////////////////////

bool Group::change_to_gid() const {
  if ( ::geteuid() == 0 /* root */ && gid_ != ::getgid() )
    return ::setgid( gid_ ) == 0;
  return true;
}

void Group::parse_value( char *line ) {
  conf<string>::parse_value( line );
  auto const group_name = operator char const*();
  if ( auto g = ::getgrnam( group_name ) ) {
    gid_ = g->gr_gid;
  } else {
    error() << '"' << group_name << "\" does not exist" << endl;
    ::exit( Exit_No_Group );
  }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
