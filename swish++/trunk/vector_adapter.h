/*
**      PJL C++ Library
**      vector_adapter.h
**
**      Copyright (C) 2004  Paul J. Lucas
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

#ifndef vector_adapter_H
#define vector_adapter_H

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        template< class T > class vector_adapter
//
// DESCRIPTION
//
//      A vector_adapter is a simple template to make a built-in C++ vector
//      have the API of the STL vector.
//
//*****************************************************************************
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef value_type const* const_pointer;
    typedef value_type const& const_reference;
    typedef int size_type;
    typedef pointer iterator;
    typedef const_pointer const_iterator;

    vector_adapter( size_type size ) :
        size_( size ), v_( new value_type[ size ] ) { }
    ~vector_adapter()                           { delete[] v_; }

    reference       operator[]( int i )         { return v_[i]; }
    const_reference operator[]( int i ) const   { return v_[i]; }

    iterator        begin()                     { return v_; }
    iterator        end()                       { return v_ + size_; }
    const_iterator  begin() const               { return v_; }
    const_iterator  end() const                 { return v_ + size_; }
private:
    size_type const size_;
    value_type *const v_;
};

} // namespace PJL

#endif  /* vector_adapter_H */
/* vim:set et sw=4 ts=4: */
