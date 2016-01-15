/*
**      SWISH++
**      src/meta_id.h
**
**      Copyright (C) 1998-2016  Paul J. Lucas
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

#ifndef meta_id_H
#define meta_id_H

// local
#include "pjl/less.h"

// standard
#include <map>

///////////////////////////////////////////////////////////////////////////////

typedef short meta_id_type;

/**
 * A %meta_name_id_map_type is used to map a META name to its unique integer
 * ID. IDs start at zero.
 */
typedef std::map<char const*,meta_id_type> meta_name_id_map_type;

extern meta_name_id_map_type meta_name_id_map;

///////////////////////////////////////////////////////////////////////////////

#endif /* meta_id_H */
/* vim:set et sw=2 ts=2: */
