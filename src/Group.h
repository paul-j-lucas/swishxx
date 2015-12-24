/*
**      SWISH++
**      src/Group.h
**
**      Copyright (C) 2001  Paul J. Lucas
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
#include "swishpp-config.h"

// standard
#include <sys/types.h>
#include <unistd.h>                             /* for gid_t */

//*****************************************************************************
//
// SYNOPSIS
//
        class Group : public conf<std::string>
//
// DESCRIPTION
//
//      A Group is-a conf<string> containing the group name of the user we
//      should run as after initialization (if we're root to begin with).
//
//      This is the same as search's -G command-line option.
//
//*****************************************************************************
{
public:
    Group();
    CONF_STRING_ASSIGN_OPS( Group );

    bool            change_to_gid() const;
    gid_t           gid() const { return gid_; }
private:
    virtual void    parse_value( char *line );

    gid_t           gid_;
};

extern Group group;

////////// Inlines ////////////////////////////////////////////////////////////

inline Group::Group() :
    conf<std::string>( "Group", Group_Default ), gid_( ::getegid() )
{
}

inline bool Group::change_to_gid() const {
    if ( ::geteuid() == 0 /* root */ && gid_ != ::getgid() )
        return ::setgid( gid_ ) == 0;
    return true;
}

#endif /* Group_H */
/* vim:set et sw=4 ts=4: */
