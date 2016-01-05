/*
**      SWISH++
**      src/util.cpp
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
#include "pjl/char_buffer_pool.h"
#include "util.h"

// standard
#include <cassert>
#include <cerrno>
#include <cstdlib>                      /* for strtoul(3) */
#include <cstring>

using namespace PJL;
using namespace std;

char_buffer_pool<128,5> lower_buf;
struct stat             stat_buf;       // someplace to do a stat(2) in

///////////////////////////////////////////////////////////////////////////////

unsigned atou( char const *s, char const *label ) {
  assert( s );
  while ( isspace( *s ) )               // skip whitespace
    ++s;
  if ( *s && *s != '-' ) {              // strtoul(3) wrongly allows '-'
    char *end = nullptr;
    errno = 0;
    unsigned long const n = ::strtoul( s, &end, 0 );
    if ( !errno && !*end )
      return n;
  }
  error() << '"' << s << "\": invalid unsigned integer";
  if ( label )
    cerr << " for " << label;
  cerr << endl;
  ::exit( Exit_Config_File );
}

bool parse( char const *s, bool *result ) {
  assert( s );
  s = to_lower( s );
  if ( (!s[1] && (*s == '0' || *s == 'f' || *s == 'n')) ||
       ::strcmp( s, "false" ) == 0 ||
       ::strcmp( s, "no" ) == 0 ||
       ::strcmp( s, "off" ) == 0 ) {
    *result = false;
    return true;
  }
  if ( (!s[1] && (*s == '1' || *s == 't' || *s == 'y')) ||
       ::strcmp( s, "true" ) == 0 ||
       ::strcmp( s, "yes" ) == 0 ||
       ::strcmp( s, "on" ) == 0 ) {
    *result = true;
    return true;
  }
  return false;
}

char *to_lower( char *buf, char const *s ) {
  assert( buf );
  assert( s );
  char *p = buf;
  while ( (*p++ = to_lower( *s++ )) != '\0' )
    ;
  return buf;
}

char *to_lower( char const *s ) {
  assert( s );
  for ( auto p = lower_buf.next(); (*p++ = to_lower( *s++ )) != '\0'; )
    ;
  return lower_buf.current();
}

char *to_lower( char const *begin, char const *end ) {
  assert( begin );
  assert( end );
  char *p = lower_buf.next();
  while ( begin != end )
    *p++ = to_lower( *begin++ );
  *p = '\0';
  return lower_buf.current();
}

char *to_lower_r( char const *s ) {
  assert( s );
  char *const buf = new char[ ::strlen( s ) + 1 ];
  for ( char *p = buf; (*p++ = to_lower( *s++ )); )
    ;
  return buf;
}

char *to_lower_r( char const *begin, char const *end ) {
  assert( begin );
  assert( end );
  char *const buf = new char[ end - begin + 1 ];
  char *p = buf;
  while ( begin != end )
    *p++ = to_lower( *begin++ );
  *p = '\0';
  return buf;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
