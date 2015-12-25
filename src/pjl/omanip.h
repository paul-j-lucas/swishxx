/*
**      PJL C++ Library
**      omanip.h
**
**      Copyright (C) 2001-2015  Paul J. Lucas
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

#ifndef omanip_H
#define omanip_H

// standard
#include <iostream>

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * An %omanip is a class for assisting in the creating of ostream manipulators
 * by storing a pointer to a function and its argument to be called later via
 * \c operator<<().
 *
 * See also:
 *    Angelika Langer and Klaus Kreft.  "Standard C++ IOStreams and Locales."
 *    Addison-Wesley, Reading, MA., pp. 179-191.
 */
template<typename Arg>
class omanip {
public:
  typedef std::ostream& (*func_type)( std::ostream&, Arg );

  omanip( func_type f, Arg const &arg ) : f_( f ), arg_( arg ) { }

  friend std::ostream& operator<<( std::ostream &o, omanip<Arg> const &om ) {
    return (*om.f_)( o, om.arg_ );
  }

private:
  func_type const f_;
  Arg const arg_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* omanip_H */
/* vim:set et sw=2 ts=2: */
