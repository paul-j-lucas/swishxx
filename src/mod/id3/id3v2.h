/*
**      SWISH++
**      src/mod/id3/id3v2.h
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

#ifndef id3v2_h
#define id3v2_h

// local
#include "word_util.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * Contain the needed bits of information from an ID3v2 tag header.  This is
 * NOT meant to map the physical byte storage of the tag in the file.
 */
struct id3v2_header {
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
    Version_Min = 0x0200,               // the minimum and ...
    Version_Max = 0x0400                // ... maximum versions we can parse
  };

  unsigned short  version_;
  unsigned short  flags_;
  unsigned int    tag_size_;

  int all_frames_size();

  bool parse( char const*&, char const* );
};

/**
 * Contains the needed bits of information from an ID3v2 tag frame.  This is
 * NOT meant to map the physical byte storage of the frame in the file.
 */
struct id3v2_frame {
  typedef unsigned int size_type;

  enum Sizes {
    Version_2_2_Size    =  6,
    Version_2_3_Size    = 10            // also 2.4.0
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

  typedef void (id3v2_frame::*parser_ptr)();

  char            id_[5];
  size_type       size_;
  unsigned short  flags_;
  int             meta_id_;
  char const     *content_begin_;
  char const     *content_end_;
  char            unsynchronized_buf_[ 4096 ];
  char            uncompressed_buf_[ 4096 ];

  /**
   * Finds the parser for for a frame given its ID.
   *
   * @param frame_id The 3- or 4-character frame ID.  Unlike the ID3v2
   * specification, it must be in lower case.
   * @return Returns said parser or null if the frame ID is invalid.
   */
  static parser_ptr find_parser( char const* );

  /**
   * Parses a COMM ID3v2 frame and index the text in it.  This function is also
   * used to parse USER frames.
   *
   * See also:
   *    Martin Nilsson.  "4.10. Comments," ID3 tag version 2.4.0 - Native
   *    Frames, November 2000.
   *      <http://www.id3.org/>
   */
  void parse_comm();

  /**
   * Parses an ID3v2 frame header.
   *
   * See also:
   *    Martin Nilsson.  "3.2. ID3v2 frames overview," ID3 tag version 2, March
   *    1998.
   *      <http://www.id3.org/>
   *
   *    "3.3. ID3v2 frame overview," ID3 tag version 2.3.0, February
   *    1999.
   *      <http://www.id3.org/>
   *
   *    ---.  "4. ID3v2 frame overview," ID3 tag version 2.4.0 - Main
   *    Structure, November 2000.
   *      <http://www.id3.org/>
   *
   * @param c A pointer to the start of the frame.
   * @param header The ID3v2 tag header.
   * @return Returns a header_result.
   */
  header_result parse_header( char const *&c, id3v2_header const &header );

  /**
   * Parses a SYLT ID3v2 frame.
   *
   * See also:
   *    Martin Nilsson.  "4.9. Synchronized lyrics/text," ID3 tag version 2.4.0
   *    - Native Frames, November 2000.
   *      <http://www.id3.org/>
   */
  void parse_sylt();

  /**
   * Parses a TCON (genre) ID3v2 frame.  In ID3v2.x, genres are either strings
   * or references to ID3v1.x numeric IDs.
   *
   * In ID3v2.3, an ID3v1.x reference is of the form "(n)" where 'n' is one or
   * more decimal digits.  It can be optionally followed by a "refinement,"
   * e.g., "(4)Eurodisco".  Multiple references can be given, e.g., "(n)(n)".
   *
   * In ID3v2.4, an ID3v1.x reference is of the form "n" (without parentheses)
   * where 'n' is one or more decimal digits.  Multiple genres/references are
   * null separated.
   *
   * See also:
   *    Martin Nilsson.  "4.2.1. Text information frames - details," ID3 tag
   *    version 2.3.0, February 1999.
   *      <http://www.id3.org/>
   *
   *    ---.  "4.2.3. TCON Content type," ID3 tag version 2.4.0 - Native
   *    Frames, November 2000.
   *      <http://www.id3.org/>
   */
  void parse_tcon();

  /**
   * Parses a TEXT ID3v2 frame.
   *
   * See also:
   *    Martin Nilsson.  "4.2. Text information frames," ID3 tag version 2.4.0
   *    - Native Frames, November 2000.
   *      <http://www.id3.org/>
   */
  void parse_text();
};

/**
 * Checks the file for the presence of an ID3v2 tag.
 *
 * @param c A pointer to the beginning of the file.  If a tag is found, this is
 * moved 3 bytes past the "ID3".
 * @param end A pointer to one past the end of the file.
 * @return Returns \c true only if the file contains an ID3v2 tag.
 */
inline bool has_id3v2_tag( char const *&c, char const *end ) {
  return end - c > id3v2_header::Size && move_if_match( c, end, "ID3" );
}

////////// id3v2_header inlines ///////////////////////////////////////////////

inline int id3v2_header::all_frames_size() {
  return tag_size_ - (version_ > 0x0200 ?
    id3v2_frame::Version_2_3_Size : id3v2_frame::Version_2_2_Size
  );
}

///////////////////////////////////////////////////////////////////////////////

#endif /* id3v2_h */
/* vim:set et sw=2 ts=2: */
