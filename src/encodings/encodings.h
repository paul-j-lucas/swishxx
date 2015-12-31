/*
**      SWISH++
**      src/encodings/encodings.h
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

#ifndef encodings_H
#define encodings_H

// local
#include "encoded_char.h"

///////////////////////////////////////////////////////////////////////////////

encoded_char_range::encoding_type const Seven_Bit = nullptr;
encoded_char_range::encoding_type const Eight_Bit = nullptr;
encoded_char_range::encoding_type const Binary =
  reinterpret_cast<encoded_char_range::encoding_type>( ~0 );

#ifdef WITH_BASE64
/**
 * Converts a base64-encoded character sequence to its single-character
 * equivalent.  Ideally, we want to decode only a single character, but Base64
 * encoding forces us to decode 3 characters at a time because they are encoded
 * as a unit into 4 bytes.  This makes this code a major pain and slow because
 * characters have to be able to be decoded with random access, i.e., wherever
 * the pointer is positioned.
 *
 * An approach other than the one implemented here would have been to decode
 * the entire range into a buffer in one shot, but this could use a lot of
 * memory if the range is large (and the indexer already uses lots of memory).
 * Additionally, mapping the iterator position from encoded space to decoded
 * space would have been tricky and just as much of a pain.
 *
 * Anywhere a space is returned it's because we've encountered an error
 * condition and the function has to return "something" and a space is
 * innocuous.
 *
 * See also:
 *    Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose Internet
 *    Mail Extensions (MIME) Part One: Format of Internet Message Bodies,"
 *    Section 6.8, "Base64 Content-Transfer-Encoding," RFC 822 Extensions
 *    Working Group of the Internet Engineering Task Force, November 1996.
 *
 * @param begin A pointer marking the beginning of the entire encoded range.
 * @param c A pointer marking the position of the character to decode.  It is
 * left after the decoded character.
 * @param end A pointer marking the end of the entire encoded range.
 * @return Returns the decoded character or ' ' upon error.
 */
encoded_char_range::value_type encoding_base64(
  encoded_char_range::const_pointer begin,
  encoded_char_range::const_pointer &pos,
  encoded_char_range::const_pointer end
);
#endif /* WITH_BASE64 */

#ifdef WITH_QUOTED_PRINTABLE
/**
 * Converts a quoted-printable character sequence to its single-character
 * equivalent.  However, if it's just a "soft line break," return the character
 * after it, i.e., make it seem as if the soft line break were not there.
 *
 * Anywhere a space is returned it's because we've encountered an error
 * condition and the function has to return "something" and a space is
 * innocuous.
 *
 * See also:
 *    Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose Internet
 *    Mail Extensions (MIME) Part One: Format of Internet Message Bodies,"
 *    Section 6.7, "Quoted-Printable Content-Transfer-Encoding," RFC 822
 *    Extensions Working Group of the Internet Engineering Task Force, November
 *    1996.
 *
 * @param begin A pointer marking the beginning of the entire encoded range.
 * @param c A pointer marking the position of the character to decode.  It is
 * left after the decoded character.
 * @param end A pointer marking the end of the entire encoded range.
 * @return Returns the decoded character or ' ' upon error.
 */
encoded_char_range::value_type encoding_quoted_printable(
  encoded_char_range::const_pointer begin,
  encoded_char_range::const_pointer &pos,
  encoded_char_range::const_pointer end
);
#endif /* WITH_QUOTED_PRINTABLE */

///////////////////////////////////////////////////////////////////////////////
#endif /* encodings_H */
/* vim:set et sw=2 ts=2: */
