/*
**      PJL C++ Library
**      pjl_set.h
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

#ifndef pjl_set_H
#define pjl_set_H

// standard
#include <set>
#include <string>

// local
#include "pjl/less.h"

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename T> class pjl_set : public std::set<T>
//
// DESCRIPTION
//
//      A pjl_set is-a set but with the addition of a contains() member
//      function, one that returns a simpler bool result indicating whether a
//      given element is in the set.  (This is called a lot and I hate lots of
//      typing.)
//
//*****************************************************************************
{
public:
    bool contains( T const &s ) const { return this->find( s ) != this->end(); }
};

//*****************************************************************************
//
// SYNOPSIS
//
        template<> class pjl_set<char const*> : public std::set<char const*>
//
// DESCRIPTION
//
//      Specialize pjl_set for C-stle strings so as not to have a reference
//      (implemented as a pointer) to a char const*.
//
//*****************************************************************************
{
public:
    bool contains( char const *s ) const { return find( s ) != this->end(); }
};

typedef pjl_set<char const*> char_ptr_set;
typedef pjl_set<std::string> string_set;

} // namespace PJL

#endif /* pjl_set_H */
/* vim:set et sw=4 ts=4: */
