/*
**      SWISH++
**      src/mod/mail/vcard.cpp
**
**      Copyright (C) 2000  Paul J. Lucas
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

#ifdef MOD_mail

// local
#include "mod_mail.h"

// standard
#include <cstring>

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        void mail_indexer::index_vcard(
            char const *&c, char const *end
        )
//
// DESCRIPTION
//
//      Index the words in a vCard attachment.  The vCard "types" are made into
//      meta names.
//
// PARAMETERS
//
//      c       The pointer to the start of the vCard.
//
//      end     The pointer to the end of the vCard.
//
// CAVEAT
//
//      Nested vCards via the AGENT type are not handled properly, i.e., the
//      nested vCards are not treated as a vCards.
//
// SEE ALSO
//
//      Frank Dawson and Tim Howes.  "RFC 2426: vCard MIME Directory Profile,"
//      Network Working Group of the Internet Engineering Task Force, September
//  1998.
//
//*****************************************************************************
{
    key_value kv;
    while ( parse_header( c, end, &kv ) ) {
        //
        // Reuse parse_header() to parse vCard types, but trim them at
        // semicolons.
        //
        int const meta_id = find_meta( ::strtok( kv.key, ";" ) );
        if ( meta_id == Meta_ID_None )
            continue;
        //
        // Index the words in the value of the type marking them as being
        // associated with the name of the type.
        //
        encoded_char_range const e(
            kv.value_begin, kv.value_end, ISO_8859_1, Eight_Bit
        );
        indexer::index_words( e, meta_id );
    }
}

#endif /* MOD_mail */
/* vim:set et sw=4 ts=4: */
