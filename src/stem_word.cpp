/*
**      SWISH++
**      src/stem_word.cpp
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

/*
**      This code is a descendant of that originally written by B. Frakes and
**      C. Cox, 1986.
*/

// local
#include "config.h"
#include "stem_word.h"
#include "word_util.h"
#include "util.h"                       /* for new_strdup() */

// standard
#include <cctype>
#include <cstring>
#include <map>
#ifdef WITH_SEARCH_DAEMON
#include <pthread.h>
#endif /* WITH_SEARCH_DAEMON */

using namespace std;

struct rule_list {
    int         id;
    char const  *old_suffix, *new_suffix;
    int         old_suffix_len, new_suffix_len;
    int         min_stem_size;
    bool        (*condition)( char const *word );
};

static char *word_end; // iterator at end of word being stemmed
// Acess to this global variable is protected by the cache_lock mutex in
// stem_word().

static bool add_e( char const *word );
static bool ends_with_cvc( char const *word );
static bool has_vowel( char const *word );
static bool remove_e( char const *word );
static int  replace_suffix( char *word, rule_list const* );
static int  word_size( char const *word );

//*****************************************************************************
//
// SYNOPSIS
//
        bool add_e( char const *word )
//
// DESCRIPTION
//
//      Checks whether a word should have an 'e' added.  This is used only
//      by rule #122.
//
// PARAMETERS
//
//      word    The word to be checked.
//
// RETURN VALUE
//
//      Returns true only if the word meets the conditions for having an 'e'
//      added.
//
// NOTE
//
//      This function would be "inline" except that its address is taken in the
//      rule_list tables.
//
//*****************************************************************************
{
    return word_size( word ) == 1 && ends_with_cvc( word );
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool ends_with_cvc( char const *word )
//
// DESCRIPTION
//
//      Check if the given word ends with in a consonant-vowel-consonant triple
//      and the second consonant is not w, x, or y.  Some of the rewrite rules
//      apply only to a stem with this characteristic.
//
// PARAMETERS
//
//      word    The word to be checked.
//
// RETURN VALUE
//
//      Returns true only if the word ends with a consonant-vowel-consonant
//      triple as described above.
//
//*****************************************************************************
{
    if ( word_end - word < 3 )
        return false;

    char const *c = word_end;
    return !(is_vowel( *--c ) || *c == 'w' || *c == 'x' || *c == 'y' ) &&
            (is_vowel( *--c ) || *c == 'y') && !is_vowel( *--c );
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool has_vowel( char const *word )
//
// DESCRIPTION
//
//      Checks if the given word contains a vowel [aeiou].  A 'y' is also
//      considered a vowel, but only if preceeded by a consonant.
//
// PARAMETERS
//
//      word    The word to be checked.
//
// RETURN VALUE
//
//      Returns true only if the word has a vowel in it.
//
// NOTE
//
//      This function would be "inline" except that its address is taken in the
//      rule_list tables.
//
//*****************************************************************************
{
    if ( !*word )
        return false;
    return is_vowel( *word ) || ::strpbrk( word + 1, "aeiouy" );
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool remove_e( char const *word )
//
// DESCRIPTION
//
//      Checks whether a word should have an 'e' removed.  This is used only by
//      rule #502.
//
// RETURN VALUE
//
//      Returns true only if the word meets the conditions for having an 'e'
//      removed.
//
// NOTE
//
//      This function would be "inline" except that its address is taken in the
//      rule_list tables.
//
//*****************************************************************************
{
    return word_size( word ) == 1 && !ends_with_cvc( word );
}

//*****************************************************************************
//
// SYNOPSIS
//
        int replace_suffix( char *word, rule_list const *rule )
//
// DESCRIPTION
//
//      Replace the suffix of a word if a rule to do so can be found.
//
// PARAMETERS
//
//      word    The word to be stemmed.
//
//      rule    A pointer to the first rule in the ruleset to apply.
//
// RETURN VALUE
//
//      Returns the ID for the rule applied, zero otherwise.
//
//*****************************************************************************
{
#   ifdef DEBUG_stem_word
    cerr << "---> replace_suffix( \"" << word << "\" )\n";
#   endif

    for ( ; rule->id; ++rule ) {
        char *const suffix = word_end - rule->old_suffix_len;
        if ( suffix < word )
            continue;

#       ifdef DEBUG_stem_word
        cerr << "---> suffix=" << suffix
             << ", old=" << rule->old_suffix << "\n";
#       endif
        if ( ::strcmp( suffix, rule->old_suffix ) )
            continue;

        char const ch = *suffix;                // chop off ...
        *suffix = '\0';                         // ... the old suffix
        if ( word_size( word ) > rule->min_stem_size &&
             ( !rule->condition || (*rule->condition)( word ) )
        ) {
            ::strcpy( suffix, rule->new_suffix );
#           ifdef DEBUG_stem_word
            cerr << "---> replaced word=" << word << "\n";
#           endif
            word_end = suffix + rule->new_suffix_len;
            break;
        }
        *suffix = ch;                           // no match: put back
    }
    return rule->id;
}

//*****************************************************************************
//
// SYNOPSIS
//
        char const* less_stem::stem_word( char const *word )
//
// DESCRIPTION
//
//      Stem the given word by applying Porter's algorithm: run through several
//      sets of suffix replacement rules applying at most one per set.  A word
//      is stemmed only if it is composed entirely of letters.
//
// PARAMETERS
//
//      word    The word to be stemmed.  It is presumed to have already been
//              converted to lower case.
//
// RETURN VALUE
//
//      Returns the word stemmed.
//
// CAVEAT
//
//      This algorithm is (obviosuly) geared only for English.
//
// SEE ALSO
//
//      M.F. Porter. "An Algorithm For Suffix Stripping," Program, 14(3), July
//      1980, pp. 130-137.
//
//*****************************************************************************
{
    static rule_list const rules_1a[] = {
        101, "sses",    "ss",   4,  2,  -1, 0,
        102, "ies",     "i",    3,  1,  -1, 0,
        103, "ss",      "ss",   2,  2,  -1, 0,
        104, "s",       "",     1,  0,  -1, 0,
        0,
    };
    static rule_list const rules_1b[] = {
        105, "eed",     "ee",   3,  2,  0,  0,
        106, "ed",      "", 2,  0,  -1, has_vowel,
        107, "ing",     "", 3,  0,  -1, has_vowel,
        0,
    };
    static rule_list const rules_1b1[] = {
        108, "at",      "ate",  2,  3,  -1, 0,
        109, "bl",      "ble",  2,  3,  -1, 0,
        110, "iz",      "ize",  2,  3,  -1, 0,
        111, "bb",      "b",    2,  1,  -1, 0,
        112, "dd",      "d",    2,  1,  -1, 0,
        113, "ff",      "f",    2,  1,  -1, 0,
        114, "gg",      "g",    2,  1,  -1, 0,
        115, "mm",      "m",    2,  1,  -1, 0,
        116, "nn",      "n",    2,  1,  -1, 0,
        117, "pp",      "p",    2,  1,  -1, 0,
        118, "rr",      "r",    2,  1,  -1, 0,
        119, "tt",      "t",    2,  1,  -1, 0,
        120, "ww",      "w",    2,  1,  -1, 0,
        121, "xx",      "x",    2,  1,  -1, 0,
        122, "",        "e",    0,  1,  -1, add_e,
        0,
    };
    static rule_list const rules_1c[] = {
        123, "y",       "i",    1,  1,  -1, has_vowel,
        0,
    };
    static rule_list const rules_2[] = {
        201, "ational", "ate",  7,  3,  0,  0,
        202, "tional",  "tion", 6,  4,  0,  0,
        203, "enci",    "ence", 4,  4,  0,  0,
        204, "anci",    "ance", 4,  4,  0,  0,
        205, "izer",    "ize",  4,  3,  0,  0,
        206, "abli",    "able", 4,  4,  0,  0,
        207, "alli",    "al",   4,  2,  0,  0,
        208, "entli",   "ent",  5,  3,  0,  0,
        209, "eli",     "e",    3,  1,  0,  0,
        210, "ousli",   "ous",  5,  3,  0,  0,
        211, "ization", "ize",  7,  3,  0,  0,
        212, "ation",   "ate",  5,  3,  0,  0,
        213, "ator",    "ate",  4,  3,  0,  0,
        214, "alism",   "al",   5,  2,  0,  0,
        215, "iveness", "ive",  7,  3,  0,  0,
        216, "fulnes",  "ful",  6,  3,  0,  0,
        217, "ousness", "ous",  7,  3,  0,  0,
        218, "aliti",   "al",   5,  2,  0,  0,
        219, "iviti",   "ive",  5,  3,  0,  0,
        220, "biliti",  "ble",  6,  3,  0,  0,
        0,
    };
    static rule_list const rules_3[] = {
        301, "icate",   "ic",   5,  2,  0,  0,
        302, "ative",   "",     5,  0,  0,  0,
        303, "alize",   "al",   5,  2,  0,  0,
        304, "iciti",   "ic",   5,  2,  0,  0,
        305, "ical",    "ic",   4,  2,  0,  0,
        308, "ful",     "",     3,  0,  0,  0,
        309, "ness",    "",     4,  0,  0,  0,
        0,
    };
    static rule_list const rules_4[] = {
        401, "al",      "",     2,  0,  1,  0,
        402, "ance",    "",     4,  0,  1,  0,
        403, "ence",    "",     4,  0,  1,  0,
        405, "er",      "",     2,  0,  1,  0,
        406, "ic",      "",     2,  0,  1,  0,
        407, "able",    "",     4,  0,  1,  0,
        408, "ible",    "",     4,  0,  1,  0,
        409, "ant",     "",     3,  0,  1,  0,
        410, "ement",   "",     5,  0,  1,  0,
        411, "ment",    "",     4,  0,  1,  0,
        412, "ent",     "",     3,  0,  1,  0,
        413, "sion",    "s",    4,  1,  1,  0,
        414, "tion",    "t",    4,  1,  1,  0,
        415, "ou",      "",     2,  0,  1,  0,
        416, "ism",     "",     3,  0,  1,  0,
        417, "ate",     "",     3,  0,  1,  0,
        418, "iti",     "",     3,  0,  1,  0,
        419, "ous",     "",     3,  0,  1,  0,
        420, "ive",     "",     3,  0,  1,  0,
        421, "ize",     "",     3,  0,  1,  0,
        0,
    };
    static rule_list const rules_5a[] = {
        501, "e",       "",     1,  0,  1,  0,
        502, "e",       "",     1,  0,  -1, remove_e,
        0,
    };
    static rule_list const rules_5b[] = {
        503, "ll",      "l",    2,  1,  1,  0,
        0,
    };

    ////////// Ensure word is exclusively alphabetic //////////////////////////

    int const len = ::strlen( word );
    if ( ::strspn( word, "abcdefghijklmnopqrstuvwxyz" ) < len )
        return word;

    ////////// Stemming is really slow: look in a private cache ///////////////

    typedef map<char const*,char const*> stem_cache;
    static stem_cache cache;
#ifdef WITH_SEARCH_DAEMON
    static pthread_mutex_t cache_lock = PTHREAD_MUTEX_INITIALIZER;
    ::pthread_mutex_lock( &cache_lock );
#endif /* WITH_SEARCH_DAEMON */
    stem_cache::const_iterator const found = cache.find( word );
    if ( found != cache.end() ) {
#ifdef WITH_SEARCH_DAEMON
        ::pthread_mutex_unlock( &cache_lock );
#endif /* WITH_SEARCH_DAEMON */
        return found->second;
    }

    ////////// Stem the word //////////////////////////////////////////////////

#   ifdef DEBUG_stem_word
    cerr << "\n---> stem_word( \"" << word << "\" )\n";
#   endif

    char word_buf[ Word_Hard_Max_Size ];
    ::strcpy( word_buf, word );
    word_end = word_buf + len;

    replace_suffix( word_buf, rules_1a );
    int const rule = replace_suffix( word_buf, rules_1b );
    if ( rule == 106 || rule == 107 )
        replace_suffix( word_buf, rules_1b1 );
    replace_suffix( word_buf, rules_1c );
    replace_suffix( word_buf, rules_2  );
    replace_suffix( word_buf, rules_3  );
    replace_suffix( word_buf, rules_4  );
    replace_suffix( word_buf, rules_5a );
    replace_suffix( word_buf, rules_5b );

#   ifdef DEBUG_stem_word
    cerr << "\n---> stemmed word=" << word_buf << "\n";
#   endif

    char const *const new_word = new_strdup( word_buf );
    cache[ new_strdup( word ) ] = new_word;

#ifdef WITH_SEARCH_DAEMON
    ::pthread_mutex_unlock( &cache_lock );
#endif /* WITH_SEARCH_DAEMON */
    return new_word;
}

//*****************************************************************************
//
// SYNOPSIS
//
        int word_size( char const *word )
//
// DESCRIPTION
//
//      Count the number vowel-consonant pairs (syllables, sort of) in a word
//      disregarding initial consonants and final vowels.  The letter 'y'
//      counts as a consonant both at the beginning of a word and when it has a
//      vowel in front of it; otherwise (when it follows a consonant), it is
//      treated as a vowel.
//
// PARAMETERS
//
//      word    The word to be counted.
//
// EXAMPLES
//
//      word        size
//      --------    ----
//      cat         1
//      any         1
//      amount      2
//      anything    3
//
// RETURN VALUE
//
//      The size of the word as described.
//
//*****************************************************************************
{
    int size = 0;
    enum state_type { st_initial, st_vowel, st_consonant };
    state_type state = st_initial;

    for ( ; *word; ++word )
        switch ( state ) {

            case st_initial:
                state = is_vowel( *word ) ? st_vowel : st_consonant;
                break;

            case st_vowel:
                state = is_vowel( *word ) ? st_vowel : st_consonant;
                if ( state == st_consonant ) {
                    //
                    // Got a vowel-consonant pair, which is what we are
                    // counting.
                    //
                    ++size;
                }
                break;

            case st_consonant:
                //
                // A 'y' is considered a vowel here since it follows a
                // consonant.
                //
                state = is_vowel( *word ) || *word == 'y' ?
                    st_vowel : st_consonant;
                break;
        }
    return size;
}
/* vim:set et sw=4 ts=4: */
