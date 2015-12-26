/*
**      SWISH++
**      src/word_util.h
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

#ifndef word_util_H
#define word_util_H

// local
#include "encoded_char.h"
#include "swishpp-config.h"
#include "util.h"

// standard
#include <cctype>
#include <cstring>

//*****************************************************************************
//
// SYNOPSIS
//
        inline bool is_vowel( char c )
//
// DESCRIPTION
//
//      Determine whether a character is a lower-case vowel [aeiou].
//
// PARAMETERS
//
//      c   The character to be checked.
//
// RETURN VALUE
//
//      Returns true only if the character is a vowel.
//
//*****************************************************************************
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

//*****************************************************************************
//
// SYNOPSIS
//
        inline bool is_word_char( char c )
//
// DESCRIPTION
//
//      Check whether a given character is a "word character," one that is
//      valid to be in a word.
//
// PARAMETERS
//
//      c   The character to be checked.
//
// RETURN VALUE
//
//      Returns true only if the character is a "word character."
//
//*****************************************************************************
{
    return c > 0 &&
#if OPTIMIZE_WORD_CHARS
    ( is_alnum( c ) ||
        //
        // If you change Word_Chars in config.h from the default set but would
        // like to keep the optimization, edit the line below to compare 'c'
        // against every non-alphanumeric character in your set of Word_Chars.
        //
        c == '&' || c == '\'' || c == '-' || c == '_'
    );
#else
    std::strchr( Word_Chars, tolower( c ) ) != 0;
#endif /* OPTIMIZE_WORD_CHARS */
}

//*****************************************************************************
//
// SYNOPSIS
//
        inline bool is_word_begin_char( char c )
//
// DESCRIPTION
//
//      Check whether a given character is a "word beginning character," one
//      that is valid to begin a word.
//
// PARAMETERS
//
//      c   The character to be checked.
//
// RETURN VALUE
//
//      Returns true only if the character is a "word beginning character."
//
//*****************************************************************************
{
#if OPTIMIZE_WORD_BEGIN_CHARS
    //
    // If you change Word_Begin_Chars in config.h from the default set but
    // would like to keep the optimization, edit the line below to compare 'c'
    // against every character in your set of Word_Begin_Chars.
    //
    return is_alnum( c );
#else
    return std::strchr( Word_Begin_Chars, tolower( c ) ) != 0;
#endif /* OPTIMIZE_WORD_BEGIN_CHARS */
}

//*****************************************************************************
//
// SYNOPSIS
//
        inline bool is_word_end_char( char c )
//
// DESCRIPTION
//
//      Check whether a given character is a "word ending character," one that
//      is valid to end a word.
//
// RETURN VALUE
//
//      Returns true only if the character is a "word ending character."
//
//*****************************************************************************
{
#if OPTIMIZE_WORD_END_CHARS
    //
    // Same deal as with OPTIMIZE_WORD_BEGIN_CHARS.
    //
    return is_alnum( c );
#else
    return std::strchr( Word_End_Chars, tolower( c ) ) != 0;
#endif /* OPTIMIZE_WORD_END_CHARS */
}

//*****************************************************************************
//
//  Miscelleneous.
//
//*****************************************************************************

extern bool is_ok_word( char const *word );
extern bool move_if_match(
                char const *&c, char const *end, char const *s,
                bool ignore_case = false
            );
extern bool move_if_match(
                encoded_char_range::const_iterator &c, char const *s,
                bool ignore_case = false
            );

#endif /* word_util_H */
/* vim:set et sw=4 ts=4: */
