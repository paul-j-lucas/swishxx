/*
**      SWISH++
**      charsets/utf8.c
**
**      Copyright (C) 2002  Paul J. Lucas
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

#ifdef  CHARSET_utf8

// local
#include "encoded_char.h"
#include "unicode.h"

//*****************************************************************************
//
// SYNOPSIS
//
    encoded_char_range::value_type charset_utf8(
        encoded_char_range::pointer /* begin */,
        encoded_char_range::pointer &c,
        encoded_char_range::pointer end
    )
//
// DESCRIPTION
//
//      Convert a UTF-8-encoded character sequence to its ASCII equivalent.
//
// PARAMETERS
//
//      begin   An pointer marking the beginning of the entire encoded range.
//
//      c       A pointer marking the position of the character to decode.
//              It is left after the decoded character.
//
//      end     A pointer marking the end of the entire encoded range.
//
// RETURN VALUE
//
//      Returns the decoded character or ' ' upon error.
//
// SEE ALSO
//
//      The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
//      section 2.3, Addison-Wesley, 2000.
//
//      Francois Yergeau.  "RFC 2279: UTF-8, a transformation format of ISO
//      10646," Network Working Group of the Internet Engineering Task Force,
//      January 1998.
//
//*****************************************************************************
{
    //
    // If the byte value is in the ASCII range, we can simply return the
    // character.
    //
    if ( static_cast<unsigned char>( *c ) <= 127u )
        return *c++;

    //
    // Make sure we're at the first byte of the UTF-8 character; if not, "sync"
    // by skipping characters until we're at a first byte.  Only the first byte
    // has the bit pattern 11xxxxxx so it's easy to find.
    //
    while ( (static_cast<unsigned char>( *c ) & 0xC0u) != 0xC0u ) {
        if ( c == end ) {
            //
            // We ran into "end" before being able to sync: this is weird.
            // Return something innocuous like a space since we have to return
            // something.
            //
            return ' ';
        }
        ++c;
    }
    if ( (static_cast<unsigned char>( *c ) & 0xFEu) == 0xFEu ) {
        //
        // The octets FE and FF are explicity forbidden: skip over the
        // offending byte and return something innocuous like a space since we
        // have to return something.
        //
        ++c;
        return ' ';
    }

    //
    // Using a static table to know how many bytes are in the UTF-8 character
    // is the fastest way.
    //
    static char const trailing_bytes_table[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C0-CF
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D0-DF
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // E0-EF
        3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, // F0-FF
    };
    int const trailing_bytes = trailing_bytes_table[
        static_cast<unsigned char>( *c )
    ];

    register ucs4 u = 0;
    switch ( trailing_bytes ) {
        case 5: u += static_cast<unsigned char>( *c++ ); u <<= 6;
            if ( c == end ) break;
        case 4: u += static_cast<unsigned char>( *c++ ); u <<= 6;
            if ( c == end ) break;
        case 3: u += static_cast<unsigned char>( *c++ ); u <<= 6;
            if ( c == end ) break;
        case 2: u += static_cast<unsigned char>( *c++ ); u <<= 6;
            if ( c == end ) break;
        case 1: u += static_cast<unsigned char>( *c++ ); u <<= 6;
            if ( c == end ) break;
        case 0: u += static_cast<unsigned char>( *c++ );
    }

    static unsigned long const offset_table[] = {
        0x0u, 0x3080u, 0xE2080u, 0x3C82080u, 0xFA082080u, 0x82082080u
    };
    u -= offset_table[ trailing_bytes ];

    return unicode_to_ascii( u );
}

#endif  /* CHARSET_utf8 */
/* vim:set et sw=4 ts=4: */
