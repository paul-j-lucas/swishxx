/*
**      SWISH++
**      src/ResultsMax.h
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

#ifndef ResultsMax_H
#define ResultsMax_H

// local
#include "config.h"
#include "conf_unsigned.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %ResultsMax is-a conf_unsigned containing the maximum number of results to
 * return from a search.
 *
 * This is the same as search's -m command-line option.
 */
class ResultsMax : public conf<unsigned> {
public:
  ResultsMax() : conf<unsigned>( "ResultsMax", ResultsMax_Default ) { }
  CONF_INT_ASSIGN_OPS( ResultsMax )
};

extern ResultsMax max_results;

///////////////////////////////////////////////////////////////////////////////

#endif /* ResultsMax_H */
/* vim:set et sw=2 ts=2: */
