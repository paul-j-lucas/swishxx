/*
**      SWISH++
**      src/token.cpp
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

// local
#include "config.h"
#include "iso8859-1.h"
#include "token.h"
#include "util.h"
#include "word_util.h"

// standard
#include <cctype>
#include <cstring>

using namespace std;

//*****************************************************************************
//
// SNYOPSIS
//
        token_stream& operator>>( token_stream &ts, token &t )
//
// DESCRIPTION
//
//      Extract a token from the given input stream.
//
// PARAMETERS
//
//      ts  The token stream to extract the token from.
//
//      t   The newly extracted token.
//
// RETURN VALUE
//
//      Returns the first argument as is standard practice.
//
// SEE ALSO
//
//      Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//      Addison-Wesley, Reading, MA, 1997.  p. 621.
//
//*****************************************************************************
{
    token *const put_back = ts.put_back();
    if ( put_back ) {
        //
        // If there was a token previously put back, use it.
        //
        t = *put_back;
        return ts;
    }
    t.type_ = token::tt_none;
    bool in_word = false;
    char c;

    while ( ts.get( c ) ) {
        c = iso8859_1_to_ascii( c );

        if ( is_word_char( c ) ) {
            if ( !in_word ) {
                // start a new word
                t.buf_[ 0 ] = c;
                t.len_ = 1;
                in_word = true;
                continue;
            }
            if ( t.len_ < Word_Hard_Max_Size ) {
                // continue same word
                t.buf_[ t.len_++ ] = c;
                continue;
            }
            in_word = false;                    // too big: skip chars
            while ( ts.get( c ) && is_word_char( c ) ) ;
            continue;
        }

        if ( in_word ) {
            if ( c == '*' )
                t.type_ = token::tt_word_star;
            else
                ts.putback( c );
            break;
        }

        switch ( c ) {
            case '(':
                t.type_ = token::tt_lparen;
                return ts;
            case ')':
                t.type_ = token::tt_rparen;
                return ts;
            case '=':
                t.type_ = token::tt_equal;
                return ts;
        }
    }

    if ( in_word ) {
        t.buf_[ t.len_ ] = '\0';
        to_lower( t.lower_buf_, t.buf_ );
        if ( t.type_ )
            return ts;

        //
        // Check to see if the word is an operator.  For only 4 comparisons,
        // linear search is good enough.
        //
        if ( !::strcmp( t.lower_buf_, "and" ) )
            t.type_ = token::tt_and;
        else if ( !::strcmp( t.lower_buf_, "or"  ) )
            t.type_ = token::tt_or;
#ifdef WITH_WORD_POS
        else if ( !::strcmp( t.lower_buf_, "near" ) )
            t.type_ = token::tt_near;
#endif /* WITH_WORD_POS */
        else if ( !::strcmp( t.lower_buf_, "not" ) )
            t.type_ = token::tt_not;
        else
            t.type_ = token::tt_word;
    }

    return ts;
}
/* vim:set et sw=4 ts=4: */
