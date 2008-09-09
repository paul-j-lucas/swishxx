/*
**      SWISH++
**      encodings/quoted_printable.c
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

#ifdef  ENCODING_quoted_printable

// standard
#include <cstring>

// local
#include "encoded_char.h"
#include "platform.h"
#include "util.h"

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        encoded_char_range::value_type encoding_quoted_printable(
            encoded_char_range::const_pointer,
            encoded_char_range::const_pointer &c,
            encoded_char_range::const_pointer end
        )
//
// DESCRIPTION
//
//      Convert a quoted-printable character sequence to its single-character
//      equivalent.  However, if it's just a "soft line break," return the
//      character after it, i.e., make it seem as if the soft line break were
//      not there.
//
//      Anywhere a space is returned it's because we've encountered an error
//      condition and the function has to return "something" and a space is
//      innocuous.
//
// PARAMETERS
//
//      c       A pointer marking the position of the character to decode.  It
//              is left after the decoded character.
//
//      end     A pointer marking the end of the entire encoded range.
//
// RETURN VALUE
//
//      Returns the decoded character or ' ' upon error.
//
// SEE ALSO
//
//      Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose
//      Internet Mail Extensions (MIME) Part One: Format of Internet Message
//      Bodies," Section 6.7, "Quoted-Printable Content-Transfer-Encoding," RFC
//      822 Extensions Working Group of the Internet Engineering Task Force,
//      November 1996.
//
//*****************************************************************************
{
    //
    // Check to see if the character at the current position is an '=': if not,
    // the character is an ordinary character; if so, the character is a
    // quoted-printable encoded character and needs to be decoded.
    //
    if ( *c != '=' )
        return *c++;
    if ( ++c == end )
        return ' ';

    encoded_char_range::value_type h1;
    while ( true ) {
        h1 = *c++;
        if ( h1 == '\r' ) {
            //
            // The '=' was the last character on a line so this is supposed to
            // be a "soft line break": we therefore have to skip over it
            // entirely making things appear as though it's not even there by
            // returning the character after the break.
            //
            if ( c == end  || *c == '\n' && ++c == end )
                return ' ';
            if ( *c != '=' )
                return *c++;
            //
            // The character after the soft line break just so happens to be
            // another '=' so we have to start all over again.
            //
            if ( ++c == end )
                return ' ';
            continue;
        }
        if ( h1 == '\n' ) {
            //
            // Although "soft line breaks" are supposed to be represented by
            // CR-LF pairs, we're being robust here and allowing just an LF by
            // itself.
            //
            if ( c == end )
                return ' ';
            if ( *c != '=' )
                return *c++;
            if ( ++c == end )
                return ' ';
            continue;
        }
        break;
    }
    if ( !is_xdigit( h1 ) || c == end ) {
        //
        // If it's not a hexadecimal digit or it's the last character, it's
        // malformed.
        //
        return ' ';
    }
    encoded_char_range::value_type const h2 = *c++;
    if ( !is_xdigit( h2 ) ) {
        //
        // This shouldn't happen in proper quoted-printable text.
        //
        return ' ';
    }

    return static_cast<encoded_char_range::value_type>(
        //
        // We're being robust by ensuring the hexadecimal characters are upper
        // case.
        //
        ( is_digit( h1 ) ? h1 - '0' : toupper( h1 ) - 'A' + 10 ) << 4 |
        ( is_digit( h2 ) ? h2 - '0' : toupper( h2 ) - 'A' + 10 )
    );
}
#endif  /* ENCODING_quoted_printable */
/* vim:set et sw=4 ts=4: */
