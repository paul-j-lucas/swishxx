/*
**      PJL C++ Library
**      fdbuf.h
**
**      Copyright (C) 2001  Paul J. Lucas
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

#ifndef fdbuf_H
#define fdbuf_H

// standard
#include <iostream>
#include <streambuf>

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * An %fdbuf is-a streambuf that is used to be attached to a Unix file
 * descriptor.
 *
 * See also:
 *    Nicolai M. Josuttis.  "The C++ Starndard Library: A Tutorial and
 *    Reference," Addison-Wesley, 1999, pp. 672-676.
 *
 *    Angelika Langer and Klaus Kreft.  "Standard C++ IOStreams and Locales:
 *    Advanced Programmer's Guide and Reference," Addison-Wesley, 2000, pp.
 *    225-244.
 */
class fdbuf : public std::streambuf {
  enum { buf_size = 1024 };
public:
  explicit fdbuf( int fd = -1 )         { init( fd ); }
  ~fdbuf()                              { if ( fd_ > -1 ) sync(); }

  // default copy constructor is fine
  // default assignment is fine

  /**
   * In the case where an fdbuf object was constructed using the default
   * constructor, and therefore not attached to any file descriptor, this
   * function is used to attach an fdbuf to one at some later time.
   *
   * @param fd The file descriptor to attach to.
   */
  void attach( int fd );

protected:
  typedef int int_type;

  virtual int_type        overflow( int_type c );
  virtual int             sync();
  virtual int_type        underflow();
  std::streamsize         write_buf( char const*, std::streamsize );
  virtual std::streamsize xsputn( char const *buf, std::streamsize len );

private:
  int   fd_;
  char  rbuf_[ buf_size ];
  char  wbuf_[ buf_size ];

  void init( int fd );
};

/////////// Inlines ///////////////////////////////////////////////////////////

inline void fdbuf::attach( int fd ) {
  if ( fd_ > -1 ) sync();
  init( fd );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif  /* fdbuf_H */
/* vim:set et sw=2 ts=2: */
