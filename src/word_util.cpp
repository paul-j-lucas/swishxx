/*
**      SWISH++
**      src/word_util.cpp
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
#include "word_util.h"

// standard
#include <cctype>
#ifdef DEBUG_is_ok_word
#include <iostream>
#endif

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        bool is_ok_word( char const *word )
//
// DESCRIPTION
//
//      Determine whether a given word should be indexed or not using several
//      heuristics.
//
//      First, a word is checked to see if it looks like an acronym.  A word is
//      considered an acronym only if it starts with a capital letter and is
//      composed exclusively of capital letters, digits, and punctuation
//      symbols, e.g., "AT&T."  If a word looks like an acronym, it is OK and
//      no further checks are done.
//
//      Second, there are several other checks that are applied.  A word is not
//      indexed if it:
//
//      1. Is less that Word_Min_Size characters and is not an acronym.
//
//      2. Contains less than Word_Min_Vowels.
//
//      3. Contains more than Word_Max_Consec_Same of the same character
//         consecutively (not including digits).
//
//      4. Contains more than Word_Max_Consec_Consonants consecutive
//         consonants.
//
//      5. Contains more than Word_Max_Consec_Vowels consecutive vowels.
//
//      6. Contains more than Word_Max_Consec_Puncts consecutive punctuation
//         characters.
//
// PARAMETERS
//
//      word    The word to be checked.
//
// RETURN VALUE
//
//      Returns true only if the word should be indexed.
//
// EXAMPLES
//
//      AT&T    OK
//      cccp    not OK -- no vowels
//      CCCP    OK -- acronym
//      eieio   not OK -- too many consec. vowels
//
//*****************************************************************************
{
    char const *c;

#   ifdef DEBUG_is_ok_word
    cerr << '\t' << word << ' ';
#   endif

    ////////// Survey the characters in the word //////////////////////////////

    int digits = 0;
    int puncts = 0;
    int uppers = 0;
    int vowels = 0;
    for ( c = word; *c; ++c ) {
        if ( is_digit( *c ) ) {
            ++digits;
            continue;
        };
        if ( is_punct( *c ) ) {
            ++puncts;
            continue;
        }
        if ( is_upper( *c ) )
            ++uppers;
        if ( is_vowel( tolower( *c ) ) )
            ++vowels;
    }
    ptrdiff_t const len = c - word;

    if ( is_upper( *word ) && uppers + digits + puncts == len ) {
#       ifdef DEBUG_is_ok_word
        cerr << "(potential acronym)\n";
#       endif
        return true;
    }

    if ( len < Word_Min_Size ) {
#       ifdef DEBUG_is_ok_word
        cerr << "(len < Word_Min_Size)\n";
#       endif
        return false;
    }

    if ( vowels < Word_Min_Vowels ) {
#       ifdef DEBUG_is_ok_word
        cerr << "(vowels < Word_Min_Vowels)\n";
#       endif
        return false;
    }

    ////////// Perform consecutive-character checks ///////////////////////////

    int consec_consonants = 0;
    int consec_vowels = 0;
    int consec_same = 0;
    int consec_puncts = 0;
    char last_c = '\0';

    for ( c = word; *c; ++c ) {

        if ( is_digit( *c ) ) {
            consec_consonants = 0;
            consec_vowels = 0;
            consec_puncts = 0;
            last_c = '\0';              // consec_same doesn't apply to digits
            continue;
        }

        if ( is_punct( *c ) ) {
            if ( ++consec_puncts > Word_Max_Consec_Puncts ) {
#               ifdef DEBUG_is_ok_word
                cerr << "(exceeded consec puncts)\n";
#               endif
                return false;
            }
            consec_consonants = 0;
            consec_vowels = 0;
            continue;
        }

        if ( *c == last_c ) {
            if ( ++consec_same > Word_Max_Consec_Same ) {
#               ifdef DEBUG_is_ok_word
                cerr << "(exceeded consec same)\n";
#               endif
                return false;
            }
        } else {
            consec_same = 0;
            last_c = *c;
        }

        if ( is_vowel( tolower( *c ) ) ) {
            if ( ++consec_vowels > Word_Max_Consec_Vowels ) {
#               ifdef DEBUG_is_ok_word
                cerr << "(exceeded consec vowels)\n";
#               endif
                return false;
            }
            consec_consonants = 0;
            consec_puncts = 0;
            continue;
        }

        if ( ++consec_consonants > Word_Max_Consec_Consonants ) {
#           ifdef DEBUG_is_ok_word
            cerr << "(exceeded consec consonants)\n";
#           endif
            return false;
        }
        consec_vowels = 0;
        consec_puncts = 0;
    }

#   ifdef DEBUG_is_ok_word
    cerr << "\n";
#   endif
    return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool move_if_match(
            char const *&c, char const *end, char const *s,
            bool ignore_case
        )
//
// SYNOPSIS
//
//      Compares a string starting at the given iterator to another.
//
// PARAMETERS
//
//      c               The iterator to use.  If the string matches, it is
//                      repositioned at the first character past the string;
//                      otherwise it is not touched.
//
//      end             The iterator marking the end of the range to search.
//
//      s               The string to compare against.  If ignore_case is true,
//                      then this string must be in lower case.
//
//      ignore_case     True if case should be ignored.
//
// RETURN VALUE
//
//      Returns true only if the string matches.
//
//*****************************************************************************
{
    char const *d = c;
    if ( ignore_case )
        while ( *s && d != end && *s == to_lower( *d ) )
            ++s, ++d;
    else
        while ( *s && d != end && *s == *d )
            ++s, ++d;
    if ( *s )                           // didn't match before null
        return false;
    c = d;
    return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool move_if_match(
            encoded_char_range::const_iterator &c, char const *s,
            bool ignore_case
        )
//
// SYNOPSIS
//
//      Compares a string starting at the given iterator to another.
//
// PARAMETERS
//
//      c               The iterator to use.  If the string matches, it is
//                      repositioned at the first character past the string;
//                      otherwise it is not touched.
//
//      s               The string to compare against.  If ignore_case is true,
//                      then this string must be in lower case.
//
//      ignore_case     True if case should be ignored.
//
// RETURN VALUE
//
//      Returns true only if the string matches.
//
//*****************************************************************************
{
    encoded_char_range::const_iterator d = c;
    if ( ignore_case )
        while ( *s && !d.at_end() && *s == to_lower( *d ) )
            ++s, ++d;
    else
        while ( *s && !d.at_end() && *s == *d )
            ++s, ++d;
    if ( *s )                           // didn't match before null
        return false;
    c = d;
    return true;
}
/* vim:set et sw=4 ts=4: */
