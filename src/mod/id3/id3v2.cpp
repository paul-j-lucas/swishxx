/*
**      SWISH++
**      src/mod/id3/id3v2.cpp
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

// local
#include "config.h"
#include "AssociateMeta.h"
#include "charsets/charsets.h"
#include "encoded_char.h"
#include "id3v1.h"
#include "id3v2.h"
#include "indexer.h"
#include "pjl/less.h"
#include "util.h"

// standard
#include <cstdlib>
#include <cstring>
#include <map>
#ifdef DEBUG_id3v2
#include <iostream>
#include <iomanip>
#endif /* DEBUG_id3v2 */

// zlib
#ifdef HAVE_ZLIB
#include <zlib.h>
#endif /* HAVE_ZLIB */

using namespace std;

//
// What the ID3v2 specification calls an "encoding" we call a "character set."
//
typedef encoded_char_range::charset_type text_encoding;

static unsigned         parse_int( char const*&, int );
static text_encoding    parse_text_encoding( char const*&, int = 0 );
static void             resynchronize( char*, int, char const*, unsigned* );
static unsigned         unsynchsafe( char const*&, int = 4 );

//*****************************************************************************
//
// SYNOPSIS
//
        inline id3v1_genre const *find_genre( char const *s, int len )
//
// DESCRIPTION
//
//      Finds the text label for a genre given a string containing its ID.
//
// PARAMETERS
//
//      s       The pointer to the string.
//
//      len     The length of 's'.
//
// RETURN VALUE
//
//      Returns said text label or null if the string doesn't represent an
//      integer ID or the ID is out of range.
//
//*****************************************************************************
{
    return ::strspn( s, "0123456789" ) < len ? 0 : find_genre( ::atoi( s ) );
}

//*****************************************************************************
//
// SYNOPSIS
//
        id3v2_frame::parser id3v2_frame::find_parser( char const *frame_id )
//
// DESCRIPTION
//
//      Finds the parser for for a frame given its ID.
//
// PARAMETERS
//
//      frame_id    The 3- or 4-character frame ID.  Unlike the ID3v2
//                  specification, it must be in lower case.
//
// RETURN VALUE
//
//      Returns said parser or null if the frame ID is invalid.
//
//*****************************************************************************
{
    struct frame_parser {
        char const*         frame_id;
        id3v2_frame::parser parser;
    };
    static frame_parser const frame_parser_table[] = {
        //
        // ID3v2.4 frame IDs (that we care about indexing).
        //
        "comm", &id3v2_frame::parse_comm,       // Comments
        "sylt", &id3v2_frame::parse_sylt,       // Synchronised lyrics
        "talb", &id3v2_frame::parse_text,       // Album title
        "tcom", &id3v2_frame::parse_text,       // Composer
        "tcon", &id3v2_frame::parse_tcon,       // Content type (genre)
        "tcop", &id3v2_frame::parse_text,       // Copyright message
        "tenc", &id3v2_frame::parse_text,       // Encoded by
        "text", &id3v2_frame::parse_text,       // Lyricist
        "tipl", &id3v2_frame::parse_text,       // Involved people list
        "tit1", &id3v2_frame::parse_text,       // Content group desc.
        "tit2", &id3v2_frame::parse_text,       // Title
        "tit3", &id3v2_frame::parse_text,       // Subtitle
        "tmcl", &id3v2_frame::parse_text,       // Musician credit list
        "tmoo", &id3v2_frame::parse_text,       // Mood
        "toal", &id3v2_frame::parse_text,       // Original album title
        "toly", &id3v2_frame::parse_text,       // Original lyricist
        "tope", &id3v2_frame::parse_text,       // Original artist
        "town", &id3v2_frame::parse_text,       // File owner/licensee
        "tpe1", &id3v2_frame::parse_text,       // Lead artist/performer
        "tpe2", &id3v2_frame::parse_text,       // Band/orchestra
        "tpe3", &id3v2_frame::parse_text,       // Conductor
        "tpe4", &id3v2_frame::parse_text,       // Modified by
        "tpub", &id3v2_frame::parse_text,       // Publisher
        "tsst", &id3v2_frame::parse_text,       // Set subtitle
        "txxx", &id3v2_frame::parse_text,       // User-defined text
        "user", &id3v2_frame::parse_comm,       // Terms of use frame
        "uslt", &id3v2_frame::parse_comm,       // Unsynchronised lyrics
        //
        // ID3v2.3 frame IDs.
        //
        "ipls", &id3v2_frame::parse_text,       // becomes TMCL
        //
        // ID3v2.2 frame IDs.
        //
        "com",  &id3v2_frame::parse_comm,       // becomes COMM
        "ipl",  &id3v2_frame::parse_text,       // becomes TMCL
        "slt",  &id3v2_frame::parse_sylt,       // becomes SYLT
        "tal",  &id3v2_frame::parse_text,       // becomes TALB
        "tcm",  &id3v2_frame::parse_text,       // becomes TCOM
        "tco",  &id3v2_frame::parse_tcon,       // becomes TCON
        "tcr",  &id3v2_frame::parse_text,       // becomes TCOP
        "ten",  &id3v2_frame::parse_text,       // becomes TENC
        "toa",  &id3v2_frame::parse_text,       // becomes TOPE
        "tol",  &id3v2_frame::parse_text,       // becomes TOLY
        "tot",  &id3v2_frame::parse_text,       // becomes TOAL
        "tp1",  &id3v2_frame::parse_text,       // becomes TPE1
        "tp2",  &id3v2_frame::parse_text,       // becomes TPE2
        "tp3",  &id3v2_frame::parse_text,       // becomes TPE3
        "tp4",  &id3v2_frame::parse_text,       // becomes TPE4
        "tpb",  &id3v2_frame::parse_text,       // becomes TPUB
        "tt1",  &id3v2_frame::parse_text,       // becomes TIT1
        "tt2",  &id3v2_frame::parse_text,       // becomes TIT2
        "tt3",  &id3v2_frame::parse_text,       // becomes TIT3
        "txt",  &id3v2_frame::parse_text,       // becomes TEXT
        "txx",  &id3v2_frame::parse_text,       // becomes TXXX
        "ult",  &id3v2_frame::parse_comm,       // becomes USLT

        0
    };

    //
    // Note that the declaration of std::map has a default "Compare" template
    // parameter of "less< key_type >" and, since we've included less.h above
    // that defines "less< char const* >", C-style string comparisons work
    // properly.
    //
    typedef map< char const*, id3v2_frame::parser > map_type;
    static map_type m;

    if ( m.empty() )
        for ( frame_parser const *f = frame_parser_table; f->frame_id; ++f )
            m[ f->frame_id ] = f->parser;

    map_type::const_iterator const found = m.find( frame_id );
    return found != m.end() ? found->second : 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
        id3v2_frame::header_result id3v2_frame::parse_header(
            char const *&c, id3v2_header const &header
        )
//
// DESCRIPTION
//
//      Parse an ID3v2 frame header.
//
// PARAMETERS
//
//      c       A pointer to the start of the frame.
//
//      header  The ID3v2 tag header.
//
// RETURN VALUE
//
//      Returns a header_result.
//
// SEE ALSO
//
//      Martin Nilsson.  "3.2. ID3v2 frames overview," ID3 tag version 2, March
//      1998.
//          http://www.id3.org/
//
//      ---.  "3.3. ID3v2 frame overview," ID3 tag version 2.3.0, February
//      1999.
//          http://www.id3.org/
//
//      ---.  "4. ID3v2 frame overview," ID3 tag version 2.4.0 - Main
//      Structure, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    ////////// Check for padding or footer ////////////////////////////////////

    if ( (c[0] ==  0  && c[1] ==  0  && c[2] ==  0) ||
         (c[0] == '3' && c[1] == 'D' && c[2] == 'I')
    )
        return hr_end_of_frames;

    ////////// Parse frame ID, size, and flags ////////////////////////////////

    id_[0] = to_lower( *c++ );
    id_[1] = to_lower( *c++ );
    id_[2] = to_lower( *c++ );
    id_[3] = header.version_ > 0x0200 ? to_lower( *c++ ) : '\0';
    id_[4] = '\0';

    size_ = header.version_ > 0x0200 ? unsynchsafe( c ) : parse_int( c, 3 );
    flags_ = header.version_ > 0x0200 ? parse_int( c, 2 ) : 0;

#ifdef DEBUG_id3v2
    cerr << "frame_id='" << id_ << '\'' << endl;
    cerr << "frame_size=" << dec << size_ << endl;
    cerr << "frame_flags=0x" << hex << flags_ << endl;
#endif

    content_begin_ = c;
    c += size_;                                 // move to start of next frame

    ////////// Handle frame encryption ////////////////////////////////////////

    if ( flags_ & Flag_Encrypted ) {
        //
        // We currently don't support encrypted frames.
        //
        return hr_failure;
    }

    ////////// Handle meta IDs ////////////////////////////////////////////////

    meta_id_ = Meta_ID_None;
    if ( associate_meta ) {
        //
        // Do not index the words in the value of the field if either the name
        // of the field (canonicalized to lower case) is among the set of meta
        // names to exclude or not among the set to include.
        //
        if ( (meta_id_ = indexer::find_meta( id_ )) == Meta_ID_None )
            return hr_failure;
    }

    ////////// Handle unsynchronization ///////////////////////////////////////

    size_type content_size = size_;
    if ( (flags_ & Flag_Unsynchronized)
         || header.flags_ & id3v2_header::Flag_Unsynchronized
    ) {
#ifdef DEBUG_id3v2
        cerr << "resynchronizing frame..." << endl;
#endif
        resynchronize(
            unsynchronized_buf_, sizeof unsynchronized_buf_, c, &content_size
        );
        content_begin_ = unsynchronized_buf_;
    }

    ////////// Handle compression /////////////////////////////////////////////

    if ( flags_ & Flag_Compressed ) {
#ifdef HAVE_ZLIB
#ifdef DEBUG_id3v2
        cerr << "uncompressing frame..." << endl;
#endif
        unsigned long buf_len = sizeof uncompressed_buf_;
        int const result = uncompress(
            reinterpret_cast<Bytef*>( uncompressed_buf_ ), &buf_len,
            reinterpret_cast<Bytef const*>( content_begin_ ), content_size
        );
        if ( result != Z_OK )
            return hr_failure;
        content_begin_ = uncompressed_buf_;
        content_size = buf_len;
#else
        //
        // Since we don't have zlib, we can't uncompress the frame to index it:
        // c'est la vie.
        //
        return hr_failure;
#endif /* HAVE_ZLIB */
    }

    content_end_ = content_begin_ + content_size;
    return hr_success;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void id3v2_frame::parse_comm()
//
// DESCRIPTION
//
//      Parse a COMM ID3v2 frame and index the text in it.  This function is
//      also used to parse USER frames.
//
// SEE ALSO
//
//      Martin Nilsson.  "4.10. Comments," ID3 tag version 2.4.0 - Native
//      Frames, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    text_encoding const encoding = parse_text_encoding( content_begin_, 3 );
    if ( encoding == CHARSET_UNKNOWN )
        return;
    content_begin_ += 3;                        // skip language
    encoded_char_range const e( content_begin_, content_end_, encoding );
    indexer::text_indexer()->index_words( e, meta_id_ );
}

//*****************************************************************************
//
// SYNOPSIS
//
        void id3v2_frame::parse_sylt()
//
// DESCRIPTION
//
//      Parse a SYLT ID3v2 frame and index the text in it.
//
// SEE ALSO
//
//      Martin Nilsson.  "4.9. Synchronized lyrics/text," ID3 tag version 2.4.0
//      - Native Frames, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    static char const *const content_type_table[] = {
        "other",                                // 0 -- we don't use this one
        "lyrics",                               // 1
        "text-transcription",                   // 2
        "movement",                             // 3
        "events",                               // 4
        "chord",                                // 5
        "trivia",                               // 6
        "webpage-urls",                         // 7
        "image-urls",                           // 8
    };

    text_encoding const encoding = parse_text_encoding( content_begin_, 5 );
    if ( encoding == CHARSET_UNKNOWN )
        return;

    content_begin_ += 3;                        // skip language
    content_begin_ += 1;                        // skip time-stamp format

    if ( associate_meta && *content_begin_ &&
         *content_begin_ < NUM_ELEMENTS( content_type_table )
    ) {
        //
        // Do not index the words in the value of the content type if either
        // the name of the content descriptor is among the set of meta names to
        // exclude or not among the set to include.
        //
        meta_id_ = indexer::find_meta( content_type_table[ *content_begin_ ] );
        if ( meta_id_ == Meta_ID_None )
            return;
    }
    ++content_begin_;                           // skip content type

    encoded_char_range const e( content_begin_, content_end_, encoding );
    indexer::text_indexer()->index_words( e, meta_id_ );
}

//*****************************************************************************
//
// SYNOPSIS
//
        void id3v2_frame::parse_tcon()
//
// DESCRIPTION
//
//      Parse a TCON (genre) ID3v2 frame.  In ID3v2.x, genres are either
//      strings or references to ID3v1.x numeric IDs.
//
//      In ID3v2.3, an ID3v1.x reference is of the form "(n)" where 'n' is one
//      or more decimal digits.  It can be optionally followed by a
//      "refinement," e.g., "(4)Eurodisco".  Multiple references can be given,
//      e.g., "(n)(n)".
//
//      In ID3v2.4, an ID3v1.x reference is of the form "n" (without
//      parentheses) where 'n' is one or more decimal digits.  Multiple
//      genres/references are null separated.
//
// SEE ALSO
//
//      Martin Nilsson.  "4.2.1. Text information frames - details," ID3 tag
//      version 2.3.0, February 1999.
//          http://www.id3.org/
//
//      ---.  "4.2.3. TCON Content type," ID3 tag version 2.4.0 - Native
//      Frames, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    text_encoding const encoding = parse_text_encoding( content_begin_ );
    if ( encoding == CHARSET_UNKNOWN )
        return;

    char    word[ Word_Hard_Max_Size + 1 ];
    bool    in_word = false;
    int     len;

    char const *c = content_begin_;
    while ( c != content_end_ ) {
        char const ch = *c++;

        ////////// Collect a word /////////////////////////////////////////////

        if ( is_word_char( ch ) ) {
            if ( !in_word ) {
                // start a new word
                word[ 0 ] = ch;
                len = 1;
                in_word = true;
                continue;
            }
            if ( len < Word_Hard_Max_Size ) {
                // continue same word
                word[ len++ ] = ch;
                continue;
            }
            in_word = false;                    // too big: skip chars
            while ( c != content_end_ && is_word_char( *c++ ) ) ;
            continue;
        }

        if ( in_word ) {
            //
            // We ran into a non-word character, so index the word up to, but
            // not including, it.  But first see if the word is a number, i.e.,
            // an ID3v1.x genre ID: if so, look up the genre string and index
            // that instead.
            //
            in_word = false;
            word[ len ] = '\0';
            id3v1_genre const *const g = find_genre( word, len );
            if ( g ) {
                ::strcpy( word, g->name );
                len = g->length;
            }
            indexer::index_word( word, len, meta_id_ );
        }
    }
    if ( in_word ) {
        //
        // We ran into 'end' while still accumulating characters into a word,
        // so just index what we've got.
        //
        word[ len ] = '\0';
        if ( id3v1_genre const *const g = find_genre( word, len ) ) {
            ::strcpy( word, g->name );
            len = g->length;
        }
        indexer::index_word( word, len, meta_id_ );
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        void id3v2_frame::parse_text()
//
// DESCRIPTION
//
//      Parse a TEXT ID3v2 frame and index the text in it.
//
// SEE ALSO
//
//      Martin Nilsson.  "4.2. Text information frames," ID3 tag version 2.4.0
//      - Native Frames, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    text_encoding const encoding = parse_text_encoding( content_begin_ );
    if ( encoding == CHARSET_UNKNOWN )
        return;
    encoded_char_range const e( content_begin_, content_end_, encoding );
    indexer::text_indexer()->index_words( e, meta_id_ );
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool id3v2_header::parse( char const *&c, char const *end )
//
// DESCRIPTION
//
//      Parse an ID3v2 tag header.
//
// PARAMETERS
//
//      c       A pointer to the beginning of the file.
//
//      end     A pointer to one past the end of the file.
//
// RETURN VALUE
//
//      Returns true only if an ID3v2 tag header was parsed successfully.
//
// SEE ALSO
//
//      Martin Nilsson.  "3.2. ID3v2 Extender header," ID3 tag version 2.3.0,
//      February 2999.
//          http://www.id3.org/
//
//      ---.  "3.1. ID3v2 header," ID3 tag version 2.4.0 - Native Frames,
//      November 2000.
//          http://www.id3.org/
//
//      ---.  "3.2. Extender header," ID3 tag version 2.4.0 - Main Structure,
//      November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    if ( !has_id3v2_tag( c, end ) )
        return false;

    version_ = parse_int( c, 2 );
#ifdef DEBUG_id3v2
    cerr << "header_version=0x" << hex << version_ << endl;
#endif
    if ( version_ < Version_Min || version_ > Version_Max )
        return false;

    flags_ = *c++;
    tag_size_ = unsynchsafe( c );
#ifdef DEBUG_id3v2
    cerr << "header_flags=0x" << hex << flags_ << endl;
    cerr << "tag_size=" << dec << tag_size_ << endl;
#endif
    if ( flags_ & Flag_Extended ) {
        //
        // We don't care about anything in the extended header so just skip it.
        //
        unsigned const ext_size = version_ > 0x0300 ?
            unsynchsafe( c ) : parse_int( c, 4 );
#ifdef DEBUG_id3v2
        cerr << "ext_size=" << dec << ext_size << endl;
#endif
        c += ext_size;
    }

    return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
        unsigned parse_int( char const *&c, int bytes )
//
// DESCRIPTION
//
//      Parse a big-endian integer from a stream of bytes.
//
// PARAMETERS
//
//      c       The pointer to the first byte of the integer.
//
//      bytes   The number of bytes to parse.
//
// RETURN VALUE
//
//      Returns the value of the parsed integer.
//
//*****************************************************************************
{
    unsigned n = 0;
    while ( bytes-- > 0 )
        n = (n << 8) | static_cast<unsigned char>( *c++ );
    return n;
}

//*****************************************************************************
//
// SYNOPSIS
//
        text_encoding parse_text_encoding( char const *&c, int text_offset )
//
// DESCRIPTION
//
//      Parse a text encoding from an ID3v2 frame.
//
// PARAMETERS
//
//      c               A pointer to the ID3v2 text encoding ID.  It is bumped
//                      by one byte upon return, i.e., past the encoding ID.
//
//      text_offset     The offset from c where the encoded text starts.
//
// RETURN VALUE
//
//      Return said text encoding.
//
// SEE ALSO
//
//      Martin Nilsson.  "4. ID3v2 frame overview," ID3 tag version 2.4.0 -
//      Main Structure, November 2000.
//          http://www.id3.org/
//
//      The Unicode Consortium.  "Special Character and Noncharacter Values:
//      Byte Order Mark (BOM)," The Unicode Standard 3.0, section 2.7, Addison-
//      Wesley, 2000.
//
//      ---.  "Specials: Byte Order Mark (BOM)," The Unicode Standard 3.0,
//      section 13.6, Addison-Wesley, 2000.
//
//*****************************************************************************
{
    enum ID3v2_Text_Encoding {
        ID3v2_ISO_8859_1,
        ID3v2_UTF16,
        ID3v2_UTF16BE,
        ID3v2_UTF8
    };

    switch ( *c++ ) {
        case ID3v2_ISO_8859_1:
            return ISO_8859_1;
#ifdef WITH_UTF8
        case ID3v2_UTF8:
            return charset_utf8;
#endif /* WITH_UTF8 */
#ifdef WITH_UTF16
        case ID3v2_UTF16BE:
            return charset_utf16be;
        case ID3v2_UTF16: {
            //
            // Check for the BOM.
            //
            unsigned char const *const bom =
                reinterpret_cast<unsigned char const*>( c ) + text_offset;
            if ( bom[0] == 0xFEu && bom[1] == 0xFFu )
                return charset_utf16be;
            if ( bom[0] == 0xFFu && bom[1] == 0xFEu )
                return charset_utf16le;
            // no break;
        }
#endif /* WITH_UTF16 */
        default:
            return CHARSET_UNKNOWN;
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        void resynchronize(
            char *dest, int dest_len,
            char const *src, unsigned *src_len
        )
//
// DESCRIPTION
//
//      Resynchronize (de-unsynchronize) a run of bytes.  All that's involved
//      in resynchronizing is looking for the byte sequence $FF $00 and
//      removing the $00.
//
// PARAMETERS
//
//      dest        The destination bufer.
//
//      dest_len    The length of the dest buffer.
//
//      src         The source buffer.
//
//      src_len     The length of the source buffer.
//
// SEE ALSO
//
//      Martin Nilsson.  "6.1. The unsynchronisation scheme," ID3 tag version
//      2.4.0 - Main Structure, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    unsigned pos = 0;
    while ( pos++ < *src_len && dest_len-- )
        if ( static_cast<unsigned char>( *dest++ = *src++ ) == 0xFFu )
            if ( pos < *src_len && *src == 0x00 )
                ++src, --*src_len;
}

//*****************************************************************************
//
// SYNOPSIS
//
        unsigned unsynchsafe( char const *&c, int bytes )
//
// DESCRIPTION
//
//      Parse an unsynchsafe integer from a stream of bytes.
//
// PARAMETERS
//
//      c       The pointer to the first byte of the integer.
//
//      bytes   The number of bytes to parse.
//
// RETURN VALUE
//
//      Returns the "normal" value of the parsed integer.
//
// SEE ALSO
//
//      Martin Nilsson.  "6.2. Synchsafe integers," ID3 tag version 2.4.0 -
//      Main Structure, November 2000.
//          http://www.id3.org/
//
//*****************************************************************************
{
    int const bits = 7;
    int const mask = (1 << bits) - 1;

    unsigned n = 0;
    while ( bytes-- > 0 )
        n = (n << bits) | (*c++ & mask);
    return n;
}

/* vim:set et sw=4 ts=4: */
