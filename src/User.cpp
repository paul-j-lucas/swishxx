/*
**      SWISH++
**      src/User.cpp
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
#include "exit_codes.h"
#include "User.h"
#include "util.h"

// standard
#include <cstdlib>                      /* for exit(3) */
#include <pwd.h>                        /* for getpwnam(3) */

using namespace std;

///////////////////////////////////////////////////////////////////////////////

bool User::change_to_uid() const {
  if ( ::geteuid() == 0 /* root */ && uid_ != ::getuid() )
    return ::setuid( uid_ ) == 0;
  return true;
}

void User::parse_value( char *line ) {
  conf<string>::parse_value( line );
  char const *const user_name = operator char const*();
  if ( auto p = ::getpwnam( user_name ) ) {
    uid_ = p->pw_uid;
  } else {
    error() << '"' << name() << "\" value of \"" << user_name
            << "\" does not exist" << endl;
    ::exit( Exit_No_User );
  }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
