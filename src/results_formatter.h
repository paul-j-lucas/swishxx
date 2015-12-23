/*
**      SWISH++
**      src/results_formatter.h
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

#ifndef results_formatter_H
#define results_formatter_H

// local
class file_info;
#include "query.h"

// standard
#include <iostream>

//*****************************************************************************
//
// SYNOPSIS
//
        class results_formatter
//
// DESCRIPTION
//
//      A results_formatter is an abstract base class for classes that format
//      search result output.
//
//*****************************************************************************
{
public:
    virtual ~results_formatter();

    virtual void    pre( stop_word_set const& ) const = 0;
    //              Output search-result "meta" information before the results
    //              themselves: the set of stop words found in the query (if
    //              any) and the number of results.

    virtual void    result( int rank, file_info const& ) const = 0;
    //              Output an individual search result's information: it's
    //              rank, path, and title.

    virtual void    post() const;
    //              Output any trailing information.
protected:
    results_formatter( std::ostream &o, int results ) :
        out_( o ), results_( results ) { }

    std::ostream&   out_;
    int const       results_;
};

#endif /* results_formatter_H */
/* vim:set et sw=4 ts=4: */
