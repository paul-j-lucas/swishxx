/*
**      SWISH++
**      src/stem_word.cpp
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
#include <mutex>
#endif /* WITH_SEARCH_DAEMON */

using namespace std;

///////////////////////////////////////////////////////////////////////////////

struct rule_list {
  short           id;
  char const     *old_suffix, *new_suffix;
  unsigned short  old_suffix_len, new_suffix_len;
  short           min_stem_size;
  bool          (*condition)(char const*);
};

static char *word_end; // iterator at end of word being stemmed
// Access to this global variable is protected by the cache_lock mutex in
// stem_word().

// local functions
static bool ends_with_cvc( char const* );
static int  word_size( char const* );

////////// local functions ////////////////////////////////////////////////////

/**
 * Checks whether a word should have an 'e' added.  This is used only by rule
 * #122.
 *
 * @param word    The word to be checked.
 * @return Returns \c true only if the word meets the conditions for having an
 * 'e' added.
 */
static bool add_e( char const *word ) {
  return word_size( word ) == 1 && ends_with_cvc( word );
}

/**
 * Checks af the given word ends with in a consonant-vowel-consonant triple and
 * the second consonant is not w, x, or y.  Some of the rewrite rules apply
 * only to a stem with this characteristic.
 *
 * @param word The word to be checked.
 * @return Returns \c true only if the word ends with a consonant-vowel-
 * consonant triple as described above.
 */
static bool ends_with_cvc( char const *word ) {
  if ( word_end - word < 3 )
    return false;

  char const *c = word_end;
  return !(is_vowel( *--c ) || *c == 'w' || *c == 'x' || *c == 'y' ) &&
          (is_vowel( *--c ) || *c == 'y') && !is_vowel( *--c );
}

/**
 * Checks if the given word contains a vowel [aeiou].  A 'y' is also considered
 * a vowel, but only if preceeded by a consonant.
 *
 * @param word The word to be checked.
 * @return Returns \c true only if the word has a vowel in it.
 */
static bool has_vowel( char const *word ) {
  if ( !*word )
    return false;
  return is_vowel( *word ) || ::strpbrk( word + 1, "aeiouy" );
}

/**
 * Checks whether a word should have an 'e' removed.  This is used only by rule
 * #502.
 *
 * @param word The word to check.
 * @return Returns \c true only if the word meets the conditions for having an
 * 'e' removed.
 */
static bool remove_e( char const *word ) {
  return word_size( word ) == 1 && !ends_with_cvc( word );
}

/**
 * Replaces the suffix of a word if a rule to do so can be found.
 *
 * @param word The word to be stemmed.
 * @param rule A pointer to the first rule in the ruleset to apply.
 * @return Returns the ID for the rule applied, zero otherwise.
 */
static int replace_suffix( char *word, rule_list const *rule ) {
# ifdef DEBUG_stem_word
  cerr << "---> replace_suffix( \"" << word << "\" )\n";
# endif

  for ( ; rule->id; ++rule ) {
    char *const suffix = word_end - rule->old_suffix_len;
    if ( suffix < word )
      continue;

#   ifdef DEBUG_stem_word
    cerr << "---> suffix=" << suffix << ", old=" << rule->old_suffix << "\n";
#   endif
    if ( ::strcmp( suffix, rule->old_suffix ) )
      continue;

    char const ch = *suffix;            // chop off ...
    *suffix = '\0';                     // ... the old suffix
    if ( word_size( word ) > rule->min_stem_size &&
       ( !rule->condition || (*rule->condition)( word ) ) ) {
      ::strcpy( suffix, rule->new_suffix );
#     ifdef DEBUG_stem_word
      cerr << "---> replaced word=" << word << "\n";
#     endif
      word_end = suffix + rule->new_suffix_len;
      break;
    }
    *suffix = ch;                       // no match: put back
  } // for
  return rule->id;
}

/**
 * Counts the number vowel-consonant pairs (syllables, sort of) in a word
 * disregarding initial consonants and final vowels.  The letter 'y' counts as
 * a consonant both at the beginning of a word and when it has a vowel in front
 * of it; otherwise (when it follows a consonant), it is treated as a vowel.
 *
 * Examples:
 *    word      size
 *    --------  ----
 *    cat       1
 *    any       1
 *    amount    2
 *    anything  3
 *
 * @param word The word to be counted.
 * @return Returns the size of the word as described.
 */
static int word_size( char const *word ) {
  int size = 0;
  enum state_type { st_initial, st_vowel, st_consonant };
  state_type state = st_initial;

  for ( ; *word; ++word ) {
    switch ( state ) {

      case st_initial:
        state = is_vowel( *word ) ? st_vowel : st_consonant;
        break;

      case st_vowel:
        state = is_vowel( *word ) ? st_vowel : st_consonant;
        if ( state == st_consonant ) {
          //
          // Got a vowel-consonant pair, which is what we are counting.
          //
          ++size;
        }
        break;

      case st_consonant:
        //
        // A 'y' is considered a vowel here since it follows a consonant.
        //
        state = is_vowel( *word ) || *word == 'y' ? st_vowel : st_consonant;
        break;
    } // switch
  } // for
  return size;
}

////////// member functions ///////////////////////////////////////////////////

char const* less_stem::stem_word( char const *word ) {
  static rule_list const rules_1a[] = {
    { 101, "sses",    "ss",    4,  2,  -1, nullptr },
    { 102, "ies",     "i",     3,  1,  -1, nullptr },
    { 103, "ss",      "ss",    2,  2,  -1, nullptr },
    { 104, "s",       "",      1,  0,  -1, nullptr },
    { 0,   nullptr,   nullptr, 0,  0,   0, nullptr }
  };
  static rule_list const rules_1b[] = {
    { 105, "eed",     "ee",    3,  2,  0, nullptr   },
    { 106, "ed",      "",      2,  0, -1, has_vowel },
    { 107, "ing",     "",      3,  0, -1, has_vowel },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_1b1[] = {
    { 108, "at",      "ate",   2,  3, -1, nullptr   },
    { 109, "bl",      "ble",   2,  3, -1, nullptr   },
    { 110, "iz",      "ize",   2,  3, -1, nullptr   },
    { 111, "bb",      "b",     2,  1, -1, nullptr   },
    { 112, "dd",      "d",     2,  1, -1, nullptr   },
    { 113, "ff",      "f",     2,  1, -1, nullptr   },
    { 114, "gg",      "g",     2,  1, -1, nullptr   },
    { 115, "mm",      "m",     2,  1, -1, nullptr   },
    { 116, "nn",      "n",     2,  1, -1, nullptr   },
    { 117, "pp",      "p",     2,  1, -1, nullptr   },
    { 118, "rr",      "r",     2,  1, -1, nullptr   },
    { 119, "tt",      "t",     2,  1, -1, nullptr   },
    { 120, "ww",      "w",     2,  1, -1, nullptr   },
    { 121, "xx",      "x",     2,  1, -1, nullptr   },
    { 122, "",        "e",     0,  1, -1, add_e     },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_1c[] = {
    { 123, "y",       "i",     1,  1, -1, has_vowel },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_2[] = {
    { 201, "ational", "ate",   7,  3,  0, nullptr   },
    { 202, "tional",  "tion",  6,  4,  0, nullptr   },
    { 203, "enci",    "ence",  4,  4,  0, nullptr   },
    { 204, "anci",    "ance",  4,  4,  0, nullptr   },
    { 205, "izer",    "ize",   4,  3,  0, nullptr   },
    { 206, "abli",    "able",  4,  4,  0, nullptr   },
    { 207, "alli",    "al",    4,  2,  0, nullptr   },
    { 208, "entli",   "ent",   5,  3,  0, nullptr   },
    { 209, "eli",     "e",     3,  1,  0, nullptr   },
    { 210, "ousli",   "ous",   5,  3,  0, nullptr   },
    { 211, "ization", "ize",   7,  3,  0, nullptr   },
    { 212, "ation",   "ate",   5,  3,  0, nullptr   },
    { 213, "ator",    "ate",   4,  3,  0, nullptr   },
    { 214, "alism",   "al",    5,  2,  0, nullptr   },
    { 215, "iveness", "ive",   7,  3,  0, nullptr   },
    { 216, "fulnes",  "ful",   6,  3,  0, nullptr   },
    { 217, "ousness", "ous",   7,  3,  0, nullptr   },
    { 218, "aliti",   "al",    5,  2,  0, nullptr   },
    { 219, "iviti",   "ive",   5,  3,  0, nullptr   },
    { 220, "biliti",  "ble",   6,  3,  0, nullptr   },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_3[] = {
    { 301, "icate",   "ic",    5,  2,  0, nullptr   },
    { 302, "ative",   "",      5,  0,  0, nullptr   },
    { 303, "alize",   "al",    5,  2,  0, nullptr   },
    { 304, "iciti",   "ic",    5,  2,  0, nullptr   },
    { 305, "ical",    "ic",    4,  2,  0, nullptr   },
    { 308, "ful",     "",      3,  0,  0, nullptr   },
    { 309, "ness",    "",      4,  0,  0, nullptr   },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_4[] = {
    { 401, "al",      "",      2,  0,  1, nullptr   },
    { 402, "ance",    "",      4,  0,  1, nullptr   },
    { 403, "ence",    "",      4,  0,  1, nullptr   },
    { 405, "er",      "",      2,  0,  1, nullptr   },
    { 406, "ic",      "",      2,  0,  1, nullptr   },
    { 407, "able",    "",      4,  0,  1, nullptr   },
    { 408, "ible",    "",      4,  0,  1, nullptr   },
    { 409, "ant",     "",      3,  0,  1, nullptr   },
    { 410, "ement",   "",      5,  0,  1, nullptr   },
    { 411, "ment",    "",      4,  0,  1, nullptr   },
    { 412, "ent",     "",      3,  0,  1, nullptr   },
    { 413, "sion",    "s",     4,  1,  1, nullptr   },
    { 414, "tion",    "t",     4,  1,  1, nullptr   },
    { 415, "ou",      "",      2,  0,  1, nullptr   },
    { 416, "ism",     "",      3,  0,  1, nullptr   },
    { 417, "ate",     "",      3,  0,  1, nullptr   },
    { 418, "iti",     "",      3,  0,  1, nullptr   },
    { 419, "ous",     "",      3,  0,  1, nullptr   },
    { 420, "ive",     "",      3,  0,  1, nullptr   },
    { 421, "ize",     "",      3,  0,  1, nullptr   },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_5a[] = {
    { 501, "e",       "",      1,  0,  1, nullptr   },
    { 502, "e",       "",      1,  0, -1, remove_e  },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr   }
  };
  static rule_list const rules_5b[] = {
    { 503, "ll",      "l",     2,  1,  1,  nullptr  },
    { 0,   nullptr,   nullptr, 0,  0,  0, nullptr  }
  };

  ////////// Ensure word is exclusively alphabetic ////////////////////////////

  size_t const len = ::strlen( word );
  if ( ::strspn( word, "abcdefghijklmnopqrstuvwxyz" ) < len )
    return word;

  ////////// Stemming is really slow: look in a private cache /////////////////

  typedef map<char const*,char const*> stem_cache;
  static stem_cache cache;
#ifdef WITH_SEARCH_DAEMON
  static mutex cache_mutex;
  lock_guard<mutex> const lock( mutex );
#endif /* WITH_SEARCH_DAEMON */
  stem_cache::const_iterator const found = cache.find( word );
  if ( found != cache.end() )
    return found->second;

  ////////// Stem the word ////////////////////////////////////////////////////

# ifdef DEBUG_stem_word
  cerr << "\n---> stem_word( \"" << word << "\" )\n";
# endif

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

# ifdef DEBUG_stem_word
  cerr << "\n---> stemmed word=" << word_buf << "\n";
# endif

  char const *const new_word = new_strdup( word_buf );
  cache[ new_strdup( word ) ] = new_word;

  return new_word;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
