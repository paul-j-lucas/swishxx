/*
**      SWISH++
**      charsets/utf16.c
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

#ifdef  CHARSET_utf16

// local
#include "encoded_char.h"
#include "unicode.h"

//*****************************************************************************
//
// SYNOPSIS
//
        encoded_char_range::value_type charset_utf16be(
            encoded_char_range::pointer,
            encoded_char_range::pointer &c,
            encoded_char_range::pointer end
        )
//
// DESCRIPTION
//
//      Convert a UTF-16-big-endian encoded character sequence to its ASCII
//      equivalent.
//
// PARAMETERS
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
//      The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
//      section 2.3, Addison-Wesley, 2000.
//
//*****************************************************************************
{
    if ( c == end || c+1 == end )
        return ' ';
    ucs4 const u = (static_cast<ucs4>( c[0] ) << 8) | c[1];
    c += 2;
    return unicode_to_ascii( u );
}

//*****************************************************************************
//
// SYNOPSIS
//
        encoded_char_range::value_type charset_utf16le(
            encoded_char_range::pointer,
            encoded_char_range::pointer &c,
            encoded_char_range::pointer end
        )
//
// DESCRIPTION
//
//      Convert a UTF-16-little-endian encoded character sequence to its ASCII
//      equivalent.
//
// PARAMETERS
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
//      The Unicode Consortium.  "Encoding Forms," The Unicode Standard 3.0,
//      section 2.3, Addison-Wesley, 2000.
//
//*****************************************************************************
{
    if ( c == end || c+1 == end )
        return ' ';
    ucs4 const u = (static_cast<ucs4>( c[1] ) << 8) | c[0];
    c += 2;
    return unicode_to_ascii( u );
}

#endif  /* CHARSET_utf16 */
/* vim:set et sw=4 ts=4: */
