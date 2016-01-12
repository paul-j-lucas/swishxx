/*
**      SWISH++
**      src/word_util.h
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

#ifndef word_util_H
#define word_util_H

// local
#include "encoded_char.h"
#include "swishxx-config.h"
#include "util.h"

// standard
#include <cctype>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////

/**
 * Determines whether a character is a lower-case vowel [aeiou].
 *
 * @param c The character to be checked.
 * @return Returns \c true only if the character is a vowel.
 */
inline bool is_vowel( char c ) {
  return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

/**
 * Checks whether a given character is a "word character," one that is valid to
 * be in a word.
 *
 * @param c The character to be checked.
 * @return Returns \c true only if the character is a "word character."
 */
inline bool is_word_char( char c ) {
  return c > 0 &&
#if OPTIMIZE_WORD_CHARS
  ( isalnum( c ) ||
    //
    // If you change Word_Chars in config.h from the default set but would like
    // to keep the optimization, edit the line below to compare 'c' against
    // every non-alphanumeric character in your set of Word_Chars.
    //
    c == '&' || c == '\'' || c == '-' || c == '_'
  );
#else
  std::strchr( Word_Chars, tolower( c ) ) != 0;
#endif /* OPTIMIZE_WORD_CHARS */
}

/**
 * Checks whether a given character is a "word beginning character," one that
 * is valid to begin a word.
 *
 * @param c The character to be checked.
 * @return Returns \c true only if the character is a "word beginning
 * character."
 */
inline bool is_word_begin_char( char c ) {
#if OPTIMIZE_WORD_BEGIN_CHARS
  //
  // If you change Word_Begin_Chars in config.h from the default set but would
  // like to keep the optimization, edit the line below to compare 'c' against
  // every character in your set of Word_Begin_Chars.
  //
  return isalnum( c );
#else
  return std::strchr( Word_Begin_Chars, tolower( c ) ) != 0;
#endif /* OPTIMIZE_WORD_BEGIN_CHARS */
}

/**
 * Checks whether a given character is a "word ending character," one that is
 * valid to end a word.
 *
 * @param c The character to be checked.
 * @return Returns \c true only if the character is a "word ending character."
 */
inline bool is_word_end_char( char c ) {
#if OPTIMIZE_WORD_END_CHARS
  //
  // Same deal as with OPTIMIZE_WORD_BEGIN_CHARS.
  //
  return isalnum( c );
#else
  return std::strchr( Word_End_Chars, tolower( c ) ) != 0;
#endif /* OPTIMIZE_WORD_END_CHARS */
}

/**
 * Determines whether a given word should be indexed or not using several
 * heuristics.
 *
 * First, a word is checked to see if it looks like an acronym.  A word is
 * considered an acronym only if it starts with a capital letter and is
 * composed exclusively of capital letters, digits, and punctuation symbols,
 * e.g., "AT&T."  If a word looks like an acronym, it is OK and no further
 * checks are done.
 *
 * Second, there are several other checks that are applied.  A word is not
 * indexed if it:
 *
 * 1. Is less that Word_Min_Size characters and is not an acronym.
 *
 * 2. Contains less than Word_Min_Vowels.
 *
 * 3. Contains more than Word_Max_Consec_Same of the same character
 *    consecutively (not including digits).
 *
 * 4. Contains more than Word_Max_Consec_Consonants consecutive consonants.
 *
 * 5. Contains more than Word_Max_Consec_Vowels consecutive vowels.
 *
 * 6. Contains more than Word_Max_Consec_Puncts consecutive punctuation
 *    characters.
 *
 * @param word The word to be checked.
 * @return Returns \c true only if the word should be indexed.
 *
 * Examples:
 * \code
 *    AT&T    OK
 *    cccp    not OK -- no vowels
 *    CCCP    OK -- acronym
 *    eieio   not OK -- too many consec. vowels
 * \endcode
 */
bool is_ok_word( char const *word );

/**
 * Compares a string starting at the given iterator to another.
 *
 * @param c The iterator to use.  If the string matches, it is repositioned at
 * the first character past the string; otherwise it is not touched.
 * @param end The iterator marking the end of the range to search.
 * @param s The string to compare against.  If ignore_case is true, then this
 * string must be in lower case.
 * @param ignore_case True if case should be ignored.
 * @return Returns \c true only if the string matches.
 */
bool move_if_match( char const *&c, char const *end, char const *s,
                    bool ignore_case = false );

/**
 * Compares a string starting at the given iterator to another.
 *
 * @param c The iterator to use.  If the string matches, it is repositioned at
 * the first character past the string; otherwise it is not touched.
 * @param end The iterator marking the end of the range to search.
 * @param s The string to compare against.  If ignore_case is true, then this
 * string must be in lower case.
 * @param ignore_case True if case should be ignored.
 * @return Returns \c true only if the string matches.
 */
bool move_if_match( encoded_char_range::const_iterator &c, char const *s,
                    bool ignore_case = false );

///////////////////////////////////////////////////////////////////////////////

#endif /* word_util_H */
/* vim:set et sw=2 ts=2: */
