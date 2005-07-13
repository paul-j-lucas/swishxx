/*
**      SWISH++
**      conf_filter.h
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

#ifndef conf_filter_H
#define conf_filter_H

// standard
#include <string>

// local
#include "conf_var.h"
#include "filter.h"
#include "pattern_map.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class conf_filter : public conf_var
//
// DESCRIPTION
//
//      A conf_filter is-a conf_var for mapping a filename pattern to a filter
//      (being a Unix process called via command-line).  Certain filename
//      patterns need to be filtered first, e.g., uncompressed.
//
//*****************************************************************************
{
public:
    typedef char const* key_type;
    typedef filter value_type;
    typedef value_type const* const_pointer;

    const_pointer operator[]( key_type key ) const {
        map_type::const_iterator const i = map_.find( key );
        return i != map_.end() ? &i->second : 0;
    }
    const_pointer operator[]( std::string const &key ) const {
        return operator[]( key.c_str() );
    }
protected:
    conf_filter( char const *name ) : conf_var( name ) { }

    typedef pattern_map< value_type > map_type;
    map_type map_;

    virtual void    parse_value( char *line );
    virtual void    reset() { map_.clear(); }
};

#endif  /* conf_filter_H */
/* vim:set et sw=4 ts=4: */
