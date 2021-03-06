/*
**      SWISH++
**      src/SocketFile.h
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

#ifndef SocketFile_H
#define SocketFile_H

// local
#include "config.h"
#include "conf_string.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %SocketFile is-a conf&lt;string&gt; containing the name of the Unix domain
 * socket file to use when 'search' is running as a daemon.
 *
 * This is the same search's \c -u command-line option.
 */
class SocketFile : public conf<std::string> {
public:
  SocketFile() : conf<std::string>( "SocketFile", SocketFile_Default ) { }
  CONF_STRING_ASSIGN_OPS( SocketFile )
};

extern SocketFile socket_file_name;

///////////////////////////////////////////////////////////////////////////////

#endif /* SocketFile_H */
/* vim:set et sw=2 ts=2: */
