/*
**      PJL C++ Library
**      auto_delete_pool.h
**
**      Copyright (C) 2004-2015  Paul J. Lucas
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

#ifndef auto_delete_pool_H
#define auto_delete_pool_H

// standard
#include <vector>

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

template<class T> class auto_delete_obj;

/**
 * An %auto_delete_pool is an object that is used to keep track of dynamically
 * allocated objects derived from auto_delete_obj for the purpose of ensuring
 * that they are all deleted when the pool is deleted.  It's a "poor man's
 * garbage collector."
 */
template<typename T>
class auto_delete_pool {
public:
  ~auto_delete_pool() {
    for ( list_type::const_iterator i = list_.begin(); i != list_.end(); ++i )
      delete static_cast<T*>( *i );
  }

private:
  typedef std::vector<void*> list_type;
  list_type list_;

  void add( void *obj ) {
    list_.push_back( obj );
  }

  friend class auto_delete_obj<T>;
};

/**
 * An %auto_delete_obj is the base class for another that should be "pooled"
 * such that it, and others like it in the same pool, are deleted
 */
template<typename Derived>
class auto_delete_obj {
public:
  typedef auto_delete_obj<Derived> pool_object_type;
  typedef auto_delete_pool<Derived> pool_type;

  virtual ~auto_delete_obj() {
  }

  pool_type* pool() const {
    return pool_;
  }

protected:
  auto_delete_obj( pool_type *pool = 0 ) : pool_( pool ) {
    if ( pool_ )
      pool_->add( this );
  }

  auto_delete_obj( pool_type &pool ) : pool_( &pool ) {
    pool_->add( this );
  }

private:
  pool_type *const pool_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* auto_delete_pool_H */
/* vim:set et sw=2 ts=2: */
