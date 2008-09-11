/*
**      SWISH++
**      mod/id3/id3v2.h
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

#ifdef  MOD_id3

#ifndef id3v2_h
#define id3v2_h

// local
#include "word_util.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct id3v2_header
//
// DESCRIPTION
//
//      This struct is used to contain the needed bits of information from an
//      ID3v2 tag header.  This is NOT meant to map the physical byte storage
//      of the tag in the file.
//
//*****************************************************************************
{
    enum Sizes {
        Size = 10
    };

    enum Flags {
        Flag_Unsynchronized = 0x80,
        Flag_Extended       = 0x40,
        Flag_Experimental   = 0x20,
        Flag_Footer_Present = 0x10
    };

    enum Versions {
        Version_Min = 0x0200,   // the minimum and ...
        Version_Max = 0x0400    // ... maximum versions we can parse
    };

    unsigned short  version_;
    unsigned short  flags_;
    unsigned int    tag_size_;

    int     all_frames_size();
    bool    parse( char const*&, char const* );
};

//*****************************************************************************
//
// SYNOPSIS
//
        struct id3v2_frame
//
// DESCRIPTION
//
//      This struct is used to contain the needed bits of information from an
//      ID3v2 tag frame.  This is NOT meant to map the physical byte storage of
//      the frame in the file.
//
//*****************************************************************************
{
    typedef unsigned int size_type;

    enum Sizes {
        Version_2_2_Size    =  6,
        Version_2_3_Size    = 10                // also 2.4.0
    };

    enum Flags {
        Flag_Tag_Alter      = 0x4000,
        Flag_File_Alter     = 0x2000,
        Flag_Read_Only      = 0x1000,
        Flag_Grouping       = 0x0040,
        Flag_Compressed     = 0x0008,
        Flag_Encrypted      = 0x0004,
        Flag_Unsynchronized = 0x0002,
        Flag_Data_Length    = 0x0001
    };

    enum header_result {
        hr_failure,
        hr_success,
        hr_end_of_frames
    };

    typedef void (id3v2_frame::*parser)();

    char            id_[5];
    size_type       size_;
    unsigned short  flags_;
    int             meta_id_;
    char const*     content_begin_;
    char const*     content_end_;
    char            unsynchronized_buf_[ 4096 ];
    char            uncompressed_buf_[ 4096 ];

    static parser   find_parser( char const* );
    header_result   parse_header( char const*&, id3v2_header const& );
    void            parse_comm();
    void            parse_sylt();
    void            parse_tcon();
    void            parse_text();
};

//*****************************************************************************
//
// SYNOPSIS
//
        inline bool has_id3v2_tag( char const *&c, char const *end )
//
// DESCRIPTION
//
//      Check the file for the presence of an ID3v2 tag.
//
// PARAMETERS
//
//      c       A pointer to the beginning of the file.  If a tag is found,
//              this is moved 3 bytes past the "ID3".
//
//      end     A pointer to one past the end of the file.
//
// RETURN VALUE
//
//      Returns true only if the file contains an ID3v2 tag.
//
//*****************************************************************************
{
    return end - c > id3v2_header::Size && move_if_match( c, end, "ID3" );
}

////////// inlines ////////////////////////////////////////////////////////////

inline int id3v2_header::all_frames_size() {
    return tag_size_ - (version_ > 0x0200 ?
        id3v2_frame::Version_2_3_Size : id3v2_frame::Version_2_2_Size
    );
}

#endif  /* id3v2_h */

#endif  /* MOD_id3 */
/* vim:set et sw=4 ts=4: */
