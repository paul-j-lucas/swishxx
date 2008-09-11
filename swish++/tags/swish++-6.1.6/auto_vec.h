/*
**      PJL C++ Library
**      auto_vec.h
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

#ifndef auto_vec_H
#define auto_vec_H

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        template< typename T > class auto_vec
//
// DESCRIPTION
//
//      A auto_vec<T> will delete the vector of objects it points to upon
//      destruction.  This is very similar to STL's auto_ptr<T>, but for
//      vectors.
//
// SEE ALSO
//
//      Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//      Addison-Wesley, Reading, MA, 1997.  pp. 367-368.
//
//*****************************************************************************
{
public:
    explicit auto_vec( T *p = 0 ) : m_p( p )    { }
    ~auto_vec()                                 { delete[] m_p; }

    auto_vec<T>& operator=( T *p ) {
        if ( p != m_p ) {
            delete[] m_p;
            m_p = p;
        }
        return *this;
    }

    T&          operator*()                     { return *m_p; }
    T const&    operator*() const               { return *m_p; }

    T&          operator[]( int i )             { return m_p[ i ]; }
    T const&    operator[]( int i ) const       { return m_p[ i ]; }

                operator T*()                   { return m_p; }
                operator T const*() const       { return m_p; }
private:
    T *m_p;

    auto_vec( auto_vec<T> const& );             // forbid copy
    auto_vec& operator=( auto_vec< T >& );      // forbid assignment
};

} // namespace PJL

#endif  /* auto_vec_H */
/* vim:set et sw=4 ts=4: */
