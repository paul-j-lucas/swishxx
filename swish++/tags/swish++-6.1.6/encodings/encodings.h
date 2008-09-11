/*
**      SWISH++
**      encodings/encodings.h
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

#ifndef encodings_H
#define encodings_H

// local
#include "encoded_char.h"

encoded_char_range::encoding_type const Seven_Bit = 0;
encoded_char_range::encoding_type const Eight_Bit = 0; // treaded like Seven_Bit
encoded_char_range::encoding_type const Binary =
    reinterpret_cast<encoded_char_range::encoding_type>( ~0 );

#ifdef  ENCODING_base64
encoded_char_range::value_type encoding_base64(
    encoded_char_range::const_pointer begin,
    encoded_char_range::const_pointer &pos,
    encoded_char_range::const_pointer end
);
#endif

#ifdef  ENCODING_quoted_printable
encoded_char_range::value_type encoding_quoted_printable(
    encoded_char_range::const_pointer begin,
    encoded_char_range::const_pointer &pos,
    encoded_char_range::const_pointer end
);
#endif

#endif  /* encodings_H */
/* vim:set et sw=4 ts=4: */
