/*
**      PJL C++ Library
**      simple_pool.h
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

#ifndef simple_pool_H
#define simple_pool_H

// standard
#include <vector>

namespace PJL {

template< class T > class simple_pool_object;

//*****************************************************************************
//
// SYNOPSYS
//
        template< typename T > class simple_pool
//
// DESCRIPTION
//
//      A simple_pool is an object that is used to keep track of dynamically
//      allocated objects derived from simple_pool_object for the purpose of
//      ensuring that they are all deleted when the pool is deleted.  It's a
//      "poor man's garbage collector."
//
//*****************************************************************************
{
public:
    ~simple_pool() {
        list_type::const_iterator i;
        for ( i = list_.begin(); i != list_.end(); ++i )
            delete static_cast<T*>( *i );
    }
private:
    typedef std::vector<void*> list_type;
    list_type list_;

    void add( void *obj ) {
        list_.push_back( obj );
    }

    friend class simple_pool_object<T>;
};

//*****************************************************************************
//
// SYNOPSYS
//
        template< typename Derived > class simple_pool_object
//
// DESCRIPTION
//
//      A simple_pool_object is the base class for another that should be
//      "pooled" such that it, and others like it in the same pool, are deleted
//      when the pool is deleted.
//
//*****************************************************************************
{
public:
    typedef simple_pool_object<Derived> pool_object_type;
    typedef simple_pool<Derived> pool_type;

    virtual ~simple_pool_object() { }

    pool_type& pool() const { return *pool_; }

protected:
    simple_pool_object( pool_type *pool = 0 ) : pool_( pool ) {
        if ( pool_ )
            pool_->add( this );
    }
    simple_pool_object( pool_type &pool ) : pool_( &pool ) {
        pool_->add( this );
    }

private:
    pool_type *const pool_;
};

} // namespace PJL

#endif  /* simple_pool_H */
/* vim:set et sw=4 ts=4: */
