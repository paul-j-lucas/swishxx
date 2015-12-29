/*
**      SWISH++
**      src/Group.h
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

#ifndef Group_H
#define Group_H

// local
#include "conf_string.h"
#include "swishxx-config.h"

// standard
#include <sys/types.h>
#include <unistd.h>                     /* for gid_t */

///////////////////////////////////////////////////////////////////////////////

/**
 * A %Group is-a conf&lt;string&gt; containing the group name of the user we
 * should run as after initialization (if we're root to begin with).
 *
 * This is the same as search's \c -G command-line option.
 */
class Group : public conf<std::string> {
public:
    Group() : conf<std::string>( "Group", Group_Default ), gid_( ::getegid() ) {
    }
    CONF_STRING_ASSIGN_OPS( Group );

    bool change_to_gid() const;
    gid_t gid() const { return gid_; }

private:
    // inherited
    virtual void parse_value( char *line );

    gid_t gid_;
};

extern Group group;

///////////////////////////////////////////////////////////////////////////////

#endif /* Group_H */
/* vim:set et sw=2 ts=2: */
