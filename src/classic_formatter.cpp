/*
**      SWISH++
**      src/classic_formatter.cpp
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

// local
#include "classic_formatter.h"
#include "file_info.h"
#include "index_segment.h"
#include "ResultSeparator.h"

extern index_segment directories;

///////////////////////////////////////////////////////////////////////////////

classic_formatter::~classic_formatter() {
  // do nothing
}

void classic_formatter::pre( stop_word_set const &stop_words ) const {
  if ( !stop_words.empty() ) {
    out_ << "# ignored:";
    for ( auto const &word : stop_words )
      out_ << ' ' << word;
    out_ << '\n';
  }
  out_ << "# results: " << results_ << '\n';
}

void classic_formatter::result( int rank, file_info const &fi ) const {
  out_ << rank << result_separator
       << directories[ fi.dir_index() ] << '/' << fi.file_name()
       << result_separator << fi.size()
       << result_separator << fi.title() << '\n';
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
