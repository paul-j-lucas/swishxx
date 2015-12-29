/*
**      SWISH++
**      src/results_formatter.h
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

#ifndef results_formatter_H
#define results_formatter_H

// local
class file_info;
#include "query.h"

// standard
#include <iostream>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %results_formatter is an abstract base class for classes that format
 * search result output.
 */
class results_formatter {
public:
  virtual ~results_formatter();

  /**
   * Outputs search-result "meta" information before the results themselves:
   * the set of stop words found in the query (if any) and the number of
   * results.
   *
   * @param stop_words The stop words to format.
   */
  virtual void pre( stop_word_set const &stop_words ) const = 0;

  /**
   * Outputs an individual search result's information: it's rank, path, and
   * title.
   *
   * @param rank The result's rank.
   * @param finfo The file_info to format.
   */
  virtual void result( int rank, file_info const &finfo ) const = 0;

  /**
   * Outputs any trailing information.
   */
  virtual void post() const;

protected:
  results_formatter( std::ostream &o, int results ) :
    out_( o ), results_( results ) { }

  std::ostream&   out_;
  int const       results_;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* results_formatter_H */
/* vim:set et sw=2 ts=2: */
