/*
**      SWISH++
**      src/encodings/base64.cpp
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

#ifdef WITH_BASE64

// local
#include "encoded_char.h"
#include "util.h"

// standard
#include <cstring>

using namespace std;

namespace {

    class base64_decoder : public encoded_char_range::decoder {
    public:
        const_pointer prev_c_;
    private:
        virtual void reset() {
            prev_c_ = 0;
        }
    };

} // namespace

//*****************************************************************************
//
// SYNOPSIS
//
        encoded_char_range::value_type encoding_base64(
            encoded_char_range::const_pointer begin,
            encoded_char_range::const_pointer &c,
            encoded_char_range::const_pointer end
        )
//
// DESCRIPTION
//
//      Convert a base64-encoded character sequence to its single-character
//      equivalent.  Ideally, we want to decode only a single character, but
//      Base64 encoding forces us to decode 3 characters at a time because they
//      are encoded as a unit into 4 bytes.  This makes this code a major pain
//      and slow because characters have to be able to be decoded with random
//      access, i.e., wherever the pointer is positioned.
//
//      An approach other than the one implemented here would have been to
//      decode the entire range into a buffer in one shot, but this could use a
//      lot of memory if the range is large (and the indexer already uses lots
//      of memory).  Additionally, mapping the iterator position from encoded
//      space to decoded space would have been tricky and just as much of a
//      pain.
//
//      Anywhere a space is returned it's because we've encountered an error
//      condition and the function has to return "something" and a space is
//      innocuous.
//
// PARAMETERS
//
//      begin   A pointer marking the beginning of the entire encoded range.
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
//      Bodies," Section 6.8, "Base64 Content-Transfer-Encoding," RFC 822
//      Extensions Working Group of the Internet Engineering Task Force,
//      November 1996.
//
// NOTE
//
//      This code is based on the decode_base64() function as part of "encdec
//      1.1" by Jörgen Hägg <jh@efd.lth.se>, 1993.
//
//*****************************************************************************
{
    int const Bits_Per_Char = 6;    // by definition of Base64 encoding

    static encoded_char_range::value_type buf[ 3 ]; // group-of-4 -> 3 chars
    static base64_decoder decoder;

    //
    // See if the pointer is less than a buffer's-worth away from the previous
    // pointer: if so, simply return the already-decoded character.
    //
    encoded_char_range::difference_type delta = c - decoder.prev_c_;
    if ( delta >= 0 && delta < sizeof buf ) {
        //
        // We advance the pointer 1 position for the first 2 characters but 2
        // positions for the 3rd since we have to skip over the 4th character
        // used in the encoded version of the characters.
        //
return_decoded_char:
        if ( ++c != end && delta == 2 )
            ++c;
        return buf[ delta ];
    }

    //
    // If we're positioned at a newline, skip over it.
    //
    encoded_char_range::const_pointer line_begin = skip_newline( c, end );
    if ( line_begin == end ) {
        //
        // We ran into the end: return something innocuous like a space since
        // we have to return something.
        //
reached_end:    c = end;
        return ' ';
    }

    if ( line_begin == c && line_begin > begin ) {
        //
        // Both line_begin hasn't moved (meaning we didn't just skip over a
        // newline) and we're not at the beginning of the encoded char range:
        // we need to "sync" by finding the beginning of the line to know where
        // the groups-of-4 encoded characters start.
        //
        while ( line_begin > begin && !is_space( *line_begin ) )
            --line_begin;
        if ( line_begin > begin )
            ++line_begin;
    } else {
        //
        // Either line_begin moved or we're at "begin": in either case, we're
        // at the beginning of a line.  Just skip "c" over the newline also.
        //
        c = line_begin;
    }

    //
    // Calculate where the start of the group-of-4 encoded characters is.
    //
    delta = c - line_begin;
    encoded_char_range::difference_type const delta4 = delta & ~3u;
    encoded_char_range::const_pointer const group = line_begin + delta4;

    if ( group + 1 == end || group + 2 == end || group + 3 == end ) {
        //
        // Well-formed Base64-encoded text should always been in groups of 4
        // characters.  This text isn't: stop.
        //
        goto reached_end;
    }

    //
    // Determine the number of characters actually encoded into the 4 by
    // looking for padding characters ('=').
    //
    int const num_chars = group[2] == '=' ? 1 : group[3] == '=' ? 2 : 3;

    //
    // Calculate a combined value of the encoded 6-bit characters.
    //
    unsigned value = 0;
    int i;
    for ( i = 0; i <= num_chars; ++i ) {
        static char const alphabet[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/=";
        //
        // Find the character in the Base64 alphabet.
        //
        if ( char const *const a = ::strchr( alphabet, group[i] ) )
            value += static_cast<unsigned>( a - alphabet )
                     << ((3 - i) * Bits_Per_Char);
        else {
            //
            // From RFC 2045, section 6.8:
            //
            //      Any characters outside of the base64 alphabet are to be
            //      ignored in base64-encoded data.
            //
            /* do nothing */;
        }
    }

    //
    // Now that we have a combined value, break it back apart but in 8-bit
    // chunks, i.e., ordinary characters.
    //
    for ( i = 2; i >= 0; --i ) {
        buf[ i ] = value & 255;
        value >>= 8;
    }

    //
    // Pretend to have decoded a single character and that it took only a
    // single byte to do it.  Additionally, remember the position of the
    // pointer marking the beginning of the range of characters that have been
    // decoded.  If we subsequently are asked to decode a character in the
    // range [i,i+3), we can simply return the character.
    //
    decoder.prev_c_ = c;
    delta -= delta4;
    goto return_decoded_char;
}
#endif /* WITH_BASE64 */
/* vim:set et sw=4 ts=4: */
