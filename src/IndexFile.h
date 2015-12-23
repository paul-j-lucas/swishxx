/*
**      SWISH++
**      src/IndexFile.h
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

#ifndef IndexFile_H
#define IndexFile_H

// local
#include "config.h"
#include "conf_string.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class IndexFile : public conf<std::string>
//
// DESCRIPTION
//
//      An IndexFile is-a conf<std::string> containing the name of the index
//      file to use.
//
//      This is the same as index's or search's -i command-line option.
//
//*****************************************************************************
{
public:
    IndexFile() : conf<std::string>( "IndexFile", IndexFile_Default ) { }
    CONF_STRING_ASSIGN_OPS( IndexFile )
};

#endif /* IndexFile_H */
/* vim:set et sw=4 ts=4: */
