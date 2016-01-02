/*
**      SWISH++
**      src/SocketAddress.cpp
**
**      Copyright (C) 2000-2015  Paul J. Lucas
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
#include "exit_codes.h"
#include "SocketAddress.h"
#include "util.h"

// standard
#include <iostream>
#include <netdb.h>                      /* for gethostbyname(3), etc */
#include <sys/types.h>                  /* needed by FreeBSD systems */
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>                      /* for exit(3) */

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void SocketAddress::convert_host( char const *hostname_or_ip ) {
  if ( *hostname_or_ip == '*' ) {
    addr_.s_addr = htonl( INADDR_ANY );
    return;
  }

  struct hostent const *const host = ::gethostbyname( hostname_or_ip );
  if ( !host ) {
    error() << '"' << hostname_or_ip << "\" is invalid\n";
    ::exit( Exit_No_Host_or_IP );
  }

  //
  // The h_addr_list member is defined as "char**" even though it is really
  // "struct in_addr**" (this seems really brain damaged), so we have to do
  // an ugly cast.
  //
  addr_ = *reinterpret_cast<struct in_addr*>( host->h_addr_list[0] );
}

void SocketAddress::parse_value( char *line ) {
  conf<string>::parse_value( line );
  char *const colon = ::strchr( line, ':' );
  if ( colon ) {
    *colon = '\0';
    convert_host( line );
    port_ = atou( colon + 1, "port" );
  } else {
    port_ = atou( line, "port" );
  }

  if ( !port_ )
    ::exit( Exit_Config_File );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
