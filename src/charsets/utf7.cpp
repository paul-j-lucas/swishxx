/*
**      SWISH++
**      src/charsets/utf7.cpp
**
**      Copyright (C) 2002-2015  Paul J. Lucas
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
#include "encoded_char.h"
#include "unicode.h"

// standard
#include <cstring>
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

namespace {

  class utf7_decoder : public encoded_char_range::decoder {
  public:
    int buf_count_;
    const_pointer prev_c_;
  private:
    virtual void reset();
  };

  void utf7_decoder::reset() {
    buf_count_ = 0;
    prev_c_ = 0;
  }

} // namespace

///////////////////////////////////////////////////////////////////////////////

encoded_char_range::value_type
charset_utf7( encoded_char_range::const_pointer begin,
              encoded_char_range::const_pointer &c,
              encoded_char_range::const_pointer end ) {
  //
  // This code is based on the decode_base64() function as part of "encdec 1.1"
  // by Jörgen Hägg <jh@efd.lth.se>, 1993.
  //
  int const Bits_Per_Char = 6;          // by definition of Base64 encoding

  static char const set_B[] =           // modified Base64 alphabet
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";                     // '=' is omitted intentionally

  static encoded_char_range::value_type buf[ 3 ]; // group-of-4 -> 3 chars
  static utf7_decoder decoder;

  ////////// Return previously decoded character //////////////////////////////

return_decoded_char:
  //
  // See if the pointer is less than a buffer's-worth away from the previous
  // pointer: if so, simply return the already-decoded character.
  //
  encoded_char_range::difference_type const delta = c - decoder.prev_c_;
  if ( delta >= 0 && delta < decoder.buf_count_ ) {
    if ( ++c != end && delta == decoder.buf_count_ - 1 ) {
      if ( ++c != end && *c == '-' ) {
        //
        // From RFC 2152, Rule 2:
        //
        //    As a special case, if the sequence terminates with the character
        //    '-' then that character is absorbed ...
        //
        ++c;
      }
    }
    return buf[ delta ];
  }

  ////////// Determine whether current character is encoded ///////////////////

  //
  // We need to "sync" by looking backwards and finding the first character
  // that is not in set B (meaning it's not encoded) and then going forwards
  // again to know whether the character at the current postition is encoded or
  // not.
  //
  bool encoded = false;
  auto const orig_c = c;
  while ( true ) {
    if ( ::strchr( set_B, *c ) ) {
      //
      // The character is in set B: hence, we don't know whether it's encoded
      // or not.
      //
      if ( c == begin ) {
        //
        // We ran into "begin" before being able to sync: this is weird. Set
        // the position one past the original position to try to skip this
        // weirdness and return something innocuous like a space since we have
        // to return something.
        //
        c = orig_c + 1;
        return ' ';
      }
      --c;
      continue;
    }

    //
    // The character isn't in set B: hence, it's not encoded.  Now work our way
    // back to the original position looking for '+' and '-' characters to keep
    // track of whether the character at the original position is encoded or
    // not.
    //
    bool just_saw_plus = false;
    while ( c < orig_c ) {
      switch ( *c++ ) {
        case '+':
          just_saw_plus = true;
          encoded = true;
          break;
        case '-':
          encoded = false;
          // no break;
        default:
          just_saw_plus = false;
      } // switch
    } // while

    //
    // OK, we're back at the original position knowing whether the character
    // here is encoded or not.
    //
    switch ( *c ) {
      case '+':
        if ( !encoded ) {
          //
          // This '+' isn't encoded so it's the start of an encoded sequence.
          //
          if ( ++c == end ) {
            //
            // We unexpectedly ran into the "end": try to do something sensible
            // and simply return the plus as if it weren't encoded after all.
            //
            return '+';
          }
          encoded = true;
        }
        break;
      case '-':
        encoded = false;
        if ( just_saw_plus ) {
          //
          // Ibid.:
          //
          //    ... as a special case, the sequence "+-" may be used to encode
          //    the character '+'.
          //
          ++c;
          return '+';
        } else if ( encoded ) {
          //
          // Ibid.:
          //
          //    As a special case, if the sequence terminates with the
          //    character '-' then that character is absorbed ...
          //
          // Therefore, return the next character unless we ran into "end" in
          // which case return something innocuous like a space since we have
          // to return something.
          //
          return ++c == end ? ' ' : *c++;
        }
    } // switch

    if ( !encoded ) {
      //
      // The character isn't encoded: return it as-is.
      //
      return *c++;
    }
    break;
  } // while

  ////////// Decode a UTF-7 character /////////////////////////////////////////

  //
  // Calculate a combined value of the encoded 6-bit characters.
  //
  ucs4 value = 0;
  int i;
  for ( i = 0; i <= 3; ++i ) {
    //
    // Find the character in set B.
    //
    if ( auto const a = ::strchr( set_B, *c ) ) {
      value += (a - set_B) << ((3 - i) * Bits_Per_Char);
      ++c;
    } else {
      //
      // We encountered a character not in set B: stop.  Ibid:
      //
      //    ... octets are to be interpreted as elements of the Modified Base64
      //    alphabet until a character not in that alphabet is encountered.
      //
      break;
    }
  } // for

  i = 4 - i;
  if ( i <= 2 ) {
    //
    // Now that we have a combined value, break it back apart but in 8-bit
    // chunks, i.e., ordinary characters.
    //
    value >>= 8 * i;
    for ( int j = 2 - i; j >= 0; --j ) {
      buf[ j ] = value & 255;
      value >>= 8;
    }
    decoder.buf_count_ = 3 - i;
  } else {
    //
    // The encoded sequence was bad, e.g. +6.
    //
    decoder.buf_count_ = 0;
  }

  //
  // Remember the position of the pointer marking the beginning of the range of
  // characters that have been decoded.  If we subsequently are asked to decode
  // a character in the range [i,i+buf_count), we can simply return the
  // character.
  //
  decoder.prev_c_ = c = orig_c + 1;
  goto return_decoded_char;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
