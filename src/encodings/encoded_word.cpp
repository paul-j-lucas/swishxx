/*
**      SWISH++
**      src/encodings/encoded_word.cpp
**
**      Copyright (C) 2016  Paul J. Lucas
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
#include "encoded_char.h"
#include "pjl/less.h"
#include "util.h"

// standard
#include <cstring>

using namespace PJL;
using namespace std;

/**
 * From 2047, 2. Syntax of encoded-words:
 *
 *    An 'encoded-word' may not be more than 75 characters long, including
 *    'charset', 'encoding', 'encoded-text', and delimiters.
 */
static unsigned const Encoded_Word_Max_Len = 75;

inline bool is_token_char( encoded_char_range::value_type c ) {
  return isalnum( c ) || strchr( "!#$%&'*+-\\^_`{|}~", c );
}

///////////////////////////////////////////////////////////////////////////////

#if 0
static unordered_char_ptr_map<encoding_type> const encoding_map{
  { "us-ascii", us_ascii },
  { "iso-8859-1", iso_8859_1 }
};
#endif

namespace {

  enum encoding_type {
    encoding_B, // Base64
    encoding_Q  // Quoted-Printable
  };

  class encoded_word_decoder : public encoded_char_range::decoder {
  public:
    encoding_type enc_type_;
    bool encoded_;

    bool is_encoded_word( encoded_char_range::const_pointer &c,
                          encoded_char_range::const_pointer end );

  private:
    virtual void reset();
  };

  bool encoded_word_decoder::
  is_encoded_word( encoded_char_range::const_pointer &c,
                   encoded_char_range::const_pointer end ) {
    encoded_char_range::const_pointer const end_max = c + Encoded_Word_Max_Len;
    if ( end > end_max )
      end = end_max;

    if ( ++c == end || *c != '?' || ++c == end )
      return false;
    auto charset_begin = c;
    while ( is_token_char( *c ) )
      if ( ++c == end )
        return false;
    if ( *c != '?' )
      return false;
    string const charset( to_lower( charset_begin, c ) );
#if 0
    auto const found = encoding_map.find( charset.c_str() );
    if ( found == encoding_map.end() )
      found->second;
#endif
    if ( ++c == end )
      return false;
    switch ( toupper( *c ) ) {
      case 'B': enc_type_ = encoding_B;
      case 'Q': enc_type_ = encoding_Q;
      default : return false;
    } // switch
    if ( ++c == end || *c != '?' || ++c == end )
      return false;
    return true;
  }

  void encoded_word_decoder::reset() {
    encoded_ = false;
  }

} // namespace

///////////////////////////////////////////////////////////////////////////////

encoded_char_range::value_type
encoding_encoded_word( encoded_char_range::const_pointer,
                       encoded_char_range::const_pointer &c,
                       encoded_char_range::const_pointer end ) {
  static encoded_word_decoder decoder;

  for ( ;; ) {
    if ( c == end )
      return ' ';

    if ( !decoder.encoded_ ) {
      if ( *c == '=' && decoder.is_encoded_word( c, end ) )
        continue;
      return *c++;
    }

    switch ( decoder.enc_type_ ) {

      case encoding_B:
        // TODO
        break;

      case encoding_Q: {
        char const ch = *c++;
        switch ( ch ) {

          case '=': {                 // =XX
            if ( c == end )
              return ' ';
            char const h1 = *c++;
            if ( c == end )
              return ' ';
            char const h2 = *c++;
            return static_cast<encoded_char_range::value_type>(
              //
              // We're being robust by ensuring the hexadecimal characters
              // are upper case.
              //
              ( isdigit( h1 ) ? h1 - '0' : toupper( h1 ) - 'A' + 10 ) << 4 |
              ( isdigit( h2 ) ? h2 - '0' : toupper( h2 ) - 'A' + 10 )
            );
          }

          case '?':
            if ( c == end )
              return ' ';
            if ( *c == '=' ) {
              decoder.encoded_ = false;
              ++c;
            }
            break;

          case '_':
            return ' ';

          default:
            return ch;
        } // switch
        break;
      }

    } // switch
  } // for
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
