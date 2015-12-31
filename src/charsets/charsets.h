/*
**      SWISH++
**      src/charsets/charsets.h
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

#ifndef charsets_H
#define charsets_H

// local
#include "config.h"
#include "encoded_char.h"

///////////////////////////////////////////////////////////////////////////////

encoded_char_range::charset_type const US_ASCII         = nullptr;
encoded_char_range::charset_type const ISO_8859_1       = nullptr;
encoded_char_range::charset_type const CHARSET_UNKNOWN  =
  reinterpret_cast<encoded_char_range::charset_type>( ~0 );

#ifdef WITH_UTF7
/**
 * Converts a UTF-7-encoded character sequence to its ASCII equivalent.
 *
 * See also:
 *    The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
 *    section 2.3, Addison-Wesley, 2000.
 *
 *    Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose Internet
 *    Mail Extensions (MIME) Part One: Format of Internet Message Bodies,"
 *    Section 6.8, "Base64 Content-Transfer-Encoding," RFC 822 Extensions
 *    Working Group of the Internet Engineering Task Force, November 1996.
 *
 *    David Goldsmith and Mark Davis.  "RFC 2152: UTF-7, a mail-safe
 *    transformation format of Unicode," Network Working Group of the Internet
 *    Engineering Task Force, May 1997.
 *
 * @param begin A pointer marking the beginning of the entire encoded range.
 * @param pos A pointer marking the position of the character to decode.  It is
 * left after the decoded character.
 * @param end A pointer marking the end of the entire encoded range.
 * @return Returns the decoded character or ' ' upon error.
 */
encoded_char_range::value_type charset_utf7(
  encoded_char_range::const_pointer begin,
  encoded_char_range::const_pointer &pos,
  encoded_char_range::const_pointer end
);
#endif /* WITH_UTF7 */

#ifdef WITH_UTF8
/**
 * Converts a UTF-8-encoded character sequence to its ASCII equivalent.
 *
 * See also:
 *    The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
 *    section 2.3, Addison-Wesley, 2000.
 *
 *    Francois Yergeau.  "RFC 2279: UTF-8, a transformation format of ISO
 *    10646," Network Working Group of the Internet Engineering Task Force,
 *    January 1998.
 *
 * @param begin A pointer marking the beginning of the entire encoded range.
 * @param pos A pointer marking the position of the character to decode.  It is
 * left after the decoded character.
 * @param end A pointer marking the end of the entire encoded range.
 * @return Returns the decoded character or ' ' upon error.
 */
encoded_char_range::value_type charset_utf8(
  encoded_char_range::const_pointer begin,
  encoded_char_range::const_pointer &pos,
  encoded_char_range::const_pointer end
);
#endif /* WITH_UTF8 */

#ifdef WITH_UTF16
/**
 * Converts a UTF-16 big-endian encoded character sequence to its ASCII
 * equivalent.
 *
 * See also:
 *    The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
 *    section 2.3, Addison-Wesley, 2000.
 *
 * @param begin A pointer marking the beginning of the entire encoded range.
 * @param pos A pointer marking the position of the character to decode.  It is
 * left after the decoded character.
 * @param end A pointer marking the end of the entire encoded range.
 */
encoded_char_range::value_type charset_utf16be(
  encoded_char_range::const_pointer begin,
  encoded_char_range::const_pointer &pos,
  encoded_char_range::const_pointer end
);

/**
 * Converts a UTF-16 little-endian encoded character sequence to its ASCII
 * equivalent.
 *
 * See also:
 *    The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
 *    section 2.3, Addison-Wesley, 2000.
 *
 * @param begin A pointer marking the beginning of the entire encoded range.
 * @param pos A pointer marking the position of the character to decode.  It is
 * left after the decoded character.
 * @param end A pointer marking the end of the entire encoded range.
 */
encoded_char_range::value_type charset_utf16le(
  encoded_char_range::const_pointer begin,
  encoded_char_range::const_pointer &pos,
  encoded_char_range::const_pointer end
);
#endif /* WITH_UTF16 */

///////////////////////////////////////////////////////////////////////////////

#endif /* charsets_H */
/* vim:set et sw=2 ts=2: */
