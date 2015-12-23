/*
**      SWISH++
**      src/charsets/charsets.h
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

#ifndef charsets_H
#define charsets_H

// local
#include "encoded_char.h"

encoded_char_range::charset_type const US_ASCII         = 0;
encoded_char_range::charset_type const ISO_8859_1       = 0;
encoded_char_range::charset_type const CHARSET_UNKNOWN  =
    reinterpret_cast<encoded_char_range::charset_type>( ~0 );

#ifdef WITH_UTF7
encoded_char_range::value_type charset_utf7(
    encoded_char_range::const_pointer begin,
    encoded_char_range::const_pointer &pos,
    encoded_char_range::const_pointer end
);
#endif

#ifdef WITH_UTF8
encoded_char_range::value_type charset_utf8(
    encoded_char_range::const_pointer begin,
    encoded_char_range::const_pointer &pos,
    encoded_char_range::const_pointer end
);
#endif

#ifdef WITH_UTF16
encoded_char_range::value_type charset_utf16be(
    encoded_char_range::const_pointer begin,
    encoded_char_range::const_pointer &pos,
    encoded_char_range::const_pointer end
);
encoded_char_range::value_type charset_utf16le(
    encoded_char_range::const_pointer begin,
    encoded_char_range::const_pointer &pos,
    encoded_char_range::const_pointer end
);
#endif

#endif /* charsets_H */
/* vim:set et sw=4 ts=4: */
