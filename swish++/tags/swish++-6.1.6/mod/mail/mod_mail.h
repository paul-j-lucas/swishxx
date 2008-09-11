/*
**      SWISH++
**      mod/mail/mod_mail.h
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

#ifdef  MOD_mail

#ifndef mod_mail_H
#define mod_mail_H

// standard
#include <string>
#include <vector>

// local
#include "auto_vec.h"
#include "charsets/charsets.h"
#include "encoded_char.h"
#include "encodings/encodings.h"
#include "filter.h"
#include "indexer.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class mail_indexer : public indexer
//
// DESCRIPTION
//
//      A mail_indexer is-an indexer for indexing mail (and news) files.  The
//      following message formats are handled: plain text, enriched text,
//      quoted-printable, HTML, MIME, vCard.  Header names and vCard types are
//      treated as meta names.
//
// SEE ALSO
//
//      David H. Crocker.  "RFC 822: Standard for the Format of ARPA Internet
//      Text Messages," Department of Electrical Engineering, University of
//      Delaware, August 1982.
//
//      Frank Dawson and Tim Howes.  "RFC 2426: vCard MIME Directory Profile,"
//      Network Working Group of the Internet Engineering Task Force, September
//      1998.
//
//      Nathaniel S. Borenstein and Ned Freed.  "RFC 2045: Multipurpose
//      Internet Mail Extensions (MIME) Part One: Format of Internet Message
//      Bodies," RFC 822 Extensions Working Group of the Internet Engineering
//      Task Force, November 1996.
//
//      Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "HTML 4.0
//      Specification," World Wide Web Consortium, April 1998.
//
//*****************************************************************************
{
public:
    mail_indexer() : indexer( "mail" ) { }

    virtual char const* find_title( PJL::mmap_file const& ) const;
    virtual void        index_words(
                            encoded_char_range const&, int = Meta_ID_None
                        );
private:
    //
    // The boundary stack keeps track of all the boundary strings for MIME
    // messages since they can nest.
    //
    // Note: I can't use an actual STL stack since I need to be able to clear
    // the entire stack and, unfortunately, clear() isn't supported for stacks,
    // an oversight in STL, IMHO.
    //
    typedef std::vector<std::string> boundary_stack_type;
    static boundary_stack_type boundary_stack_;

    enum content_type {
        ct_unknown,                     // a type we don't know how to index
        ct_external_filter,

        ct_text_plain,
        ct_text_enriched,
        ct_text_html,
        ct_text_vcard,
        ct_message_rfc822,
        ct_multipart
    };

#ifdef  __SUNPRO_CC
    //
    // Sun's CC compiler doesn't implement the latest ANSI/ISO C++ standard
    // with regard to section 11.8.
    //
    struct message_type;
    friend struct message_type;
#endif
    struct message_type {
        //
        // A message_type contains information about the mail/news message
        // including whether or not it ought to be filtered.
        //
        content_type                        content_type_;
        encoded_char_range::charset_type    charset_;
        encoded_char_range::encoding_type   encoding_;
        mutable filter*                     filter_;

        message_type();
        message_type( message_type const& );
        ~message_type()                     { delete filter_; }
    };

    struct key_value {
        PJL::auto_vec<char> key;
        char const *value_begin, *value_end;
    };

    static bool         boundary_cmp( char const*, char const*, char const* );
    message_type        index_headers( char const*&, char const* );
    void                index_multipart( char const*&, char const* );
    void                index_vcard( char const*&, char const* );
    static void         new_file();
    static bool         parse_header( char const*&, char const*, key_value* );

    static bool         did_last_header_;
};

////////// Inlines ////////////////////////////////////////////////////////////

inline mail_indexer::message_type::message_type() :
    //
    // We assume text/plain and 7-bit US-ASCII as stated in RFC 2045, sec.
    // 5.2., "Content-Type Defaults":
    //
    //      Default RFC 822 messages without a MIME Content-Type header are
    //      taken by this protocol to be plain text in the US-ASCII
    //      character set, which can be explicitly specified as:
    //
    //      Content-type: text/plain; charset=us-ascii
    //
    //      This default is assumed if no Content-Type header field is
    //      specified.
    //
    content_type_( ct_text_plain ),
    charset_( US_ASCII ), encoding_( Seven_Bit ),
    filter_( 0 )
{
    // do nothing else
}

inline mail_indexer::message_type::message_type( message_type const &mt ) :
    content_type_( mt.content_type_ ),
    charset_( mt.charset_ ), encoding_( mt.encoding_ ),
    filter_( mt.filter_ )
{
    // On copy, null the pointer to the filter in the source message_type since
    // the destructor will delete it and deleting it more than once would be
    // bad.  We don't copy the filter since there isn't a need to: the last
    // message_type object to get the filter will be the one to delete it.
    //
    mt.filter_ = 0;
}

#endif  /* mod_mail_H */

#endif  /* MOD_mail */
/* vim:set et sw=4 ts=4: */
