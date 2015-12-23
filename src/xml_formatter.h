/*
**      SWISH++
**      src/xml_formatter.h
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

#ifndef xml_formatter_H
#define xml_formatter_H

// local
#include "results_formatter.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class xml_formatter : public results_formatter
//
// DESCRIPTION
//
//      An xml_formatter is-a results_formatter for formatting search results
//      in XML.
//
//*****************************************************************************
{
public:
    xml_formatter( std::ostream &o, int results ) :
        results_formatter( o, results ) { }
    virtual ~xml_formatter();

    virtual void pre( stop_word_set const& ) const;
    virtual void result( int rank, file_info const& ) const;
    virtual void post() const;
};

#endif /* xml_formatter_H */
/* vim:set et sw=4 ts=4: */
