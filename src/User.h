/*
**      SWISH++
**      src/User.h
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

#ifndef User_H
#define User_H

// local
#include "config.h"
#include "conf_string.h"
#include "swishxx-config.h"

// standard
#include <sys/types.h>                  /* for uid_t */
#include <unistd.h>                     /* for geteuid() */

///////////////////////////////////////////////////////////////////////////////

/**
 * A User is-a conf<string> containing the login name of the user we
 * should run as after initialization (if we're root to begin with).
 *
 * This is the same as search's \c -U command-line option.
 */
class User : public conf<std::string> {
public:
  User() : conf<std::string>( "User", User_Default ), uid_( ::geteuid() ) { }
  CONF_STRING_ASSIGN_OPS( User );

  bool change_to_uid() const;
  uid_t uid() const { return uid_; }

private:
  uid_t uid_;

  // inherited
  virtual void  parse_value( char *line );
};

extern User user;

///////////////////////////////////////////////////////////////////////////////

#endif /* User_H */
/* vim:set et sw=2 ts=2: */
