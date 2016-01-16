/*
**      SWISH++
**      src/mod/html/elements.h
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

#ifndef element_map_H
#define element_map_H

// local
#include "pjl/less.h"

// standard
#include <iostream>
#include <map>
#include <set>

///////////////////////////////////////////////////////////////////////////////

class element_map;

/**
 * An %element contains the information we need about HTML elements.  We only
 * need information about:
 *
 * 1. Whether an element's end tag is forbidden, optional, or required.
 * 2. For elements with optional end tags, what tags, some possibly from other
 *    elements, close them.
 */
class element {
public:
  enum end_tag_type {
    et_forbidden,
    et_optional,
    et_required
  };

  std::set<char const*> close_tags;
  end_tag_type const end_tag;

private:
  explicit element( end_tag_type t ) : end_tag( t ) { }
  friend class element_map;
};

/**
 * An %element_map is-a map from the character strings for HTML elements to
 * instances of the element class declared above.  The only reason for having a
 * derived class rather than a \c typedef is so that we can have a custom
 * constructor that initializes itself.
 *
 * The constructor is private, however, to ensure that only \c instance() can
 * be called to initialize and access a single, static instance.
 */
class element_map : public std::map<char const*,element> {
public:
  static element_map const& instance();

private:
  /**
   * Constructs (initializes) an element_map.
   *
   * See also:
   *    Shane McCarron, et al.  "XHTML 2.0," World Wide Web Consortium,
   *    August 2002.
   *      <http://www.w3.org/TR/xhtml2/>
   *
   *    Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "Index of Elements,"
   *    HTML 4.0 Specification, World Wide Web Consortium, April 1998.
   *      <http://www.w3.org/TR/REC-html40/index/elements.html>
   *
   *    Marcin Sawicki, et al.  "Ruby Annotation," World Wide Web Consortium,
   *    April 6, 2001.
   *      <http://www.w3.org/TR/2001/PR-ruby-20010406/>
   *
   *    Netscape Communications Corporation.  "HTML Tag Reference," January
   *    1998.
   *      <http://developer.netscape.com/docs/manuals/htmlguid/index.htm>
   */
  element_map();
};

/**
 * Writes the key of an \c element_map::value_type (the HTML or XHTML element
 * name) to an \c ostream.  This is useful so an \c ostream_iterator can be
 * used to dump the entire \c element_map.
 *
 * @param o The ostream to write to.
 * @param e The element_map::value_type to write.
 */
inline std::ostream& operator<<( std::ostream &o,
                                 element_map::value_type const &e ) {
  return o << e.first;
}

///////////////////////////////////////////////////////////////////////////////

#endif /* element_map_H */
/* vim:set et sw=2 ts=2: */
