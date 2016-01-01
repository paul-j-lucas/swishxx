/*
**      SWISH++
**      src/conf_int.cpp
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

// local
#include "config.h"
#include "conf_int.h"
#include "exit_codes.h"
#include "util.h"

// standard
#include <cstdlib>                      /* for atoi(3), exit(3) */
#include <cstring>
#include <iostream>
#include <memory>                       /* for unique_ptr */

using namespace PJL;
using namespace std;

extern char const*  me;

///////////////////////////////////////////////////////////////////////////////

conf<int>::conf( char const *name, int default_value, int min, int max ) :
  conf_var( name ),
  default_value_( default_value ), min_( min ), max_( max ),
  value_( default_value )
{
  // do nothing else
}

conf<int>& conf<int>::operator=( int new_value ) {
  if ( new_value >= min_ && new_value <= max_ ) {
    value_ = new_value;
    return *this;
  }

  error() << '"' << name() << "\" value \""
          << new_value << "\" not in range [" << min_ << '-';

  if ( max_ == numeric_limits<int>::max() )
    cerr << "infinity";
  else
    cerr << max_;

  cerr << "]\n";
  ::exit( Exit_Config_File );
}

void conf<int>::parse_value( char *line ) {
  if ( !line || !*line ) {
    error() << '"' << name() << "\" has no value\n";
    ::exit( Exit_Config_File );
  }
  unique_ptr<char[]> const lower( to_lower_r( line ) );
  if ( !::strcmp( lower.get(), "infinity" ) ) {
    operator=( numeric_limits<int>::max() );
    return;
  }
  int const n = ::atoi( line );
  if ( n || *line == '0' ) {
    operator=( n );
    return;
  }

  error() << '"' << name() << "\" has a non-numeric value\n";
  ::exit( Exit_Config_File );
}

void conf<int>::reset() {
  value_ = default_value_;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
