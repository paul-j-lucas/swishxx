/*
**      PJL C++ Library
**      char_buffer_pool.h
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

#ifndef char_buffer_pool_H
#define char_buffer_pool_H

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * A %char_buffer_pool maintains a small set ("pool") of size N of available
 * character buffers, each of size Buf_Size, and issues them in a round-robin
 * manner.
 *
 * This is used by functions to return a character string without having to
 * allocate memory dynamically nor have previously returned strings
 * overwritten.
 */
template<int Buf_Size,int N>
class char_buffer_pool {
public:
  char_buffer_pool() : next_buf_index_( 0 ), cur_buf_( buf_[ 0 ] ) { }

  char* current() const {
    return cur_buf_;
  }

  char* next() {
    cur_buf_ = buf_[ next_buf_index_ ];
    next_buf_index_ = (next_buf_index_ + 1) % N;
    return cur_buf_;
  }

private:
  char buf_[ N ][ Buf_Size ];
  int next_buf_index_;
  char *cur_buf_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* char_buffer_pool_H */
/* vim:set et sw=2 ts=2: */
