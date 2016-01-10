/*
**      SWISH++
**      src/mod/html/mod_html.cpp
**
**      Copyright (C) 1998-2015  Paul J. Lucas
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
#include "charsets/unicode.h"
#include "encoded_char.h"
#include "entities.h"
#include "ExcludeClass.h"
#include "html_config.h"
#include "iso8859-1.h"
#include "mod_html.h"
#include "TitleLines.h"
#include "util.h"
#include "word_util.h"

// standard
#include <cctype>
#include <cstdlib>                      /* for exit(3) */
#include <cstring>
#include <utility>                      /* for pair<> */
#include <vector>

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

// extern variables
ExcludeClass exclude_class_names;

//
// The element_stack keeps track of all the HTML or XHTML elements we encounter
// until they are closed.  The first member of the pair is a pointer to the
// element_map's value_type or the key/value pair of the map where the key is
// the element name and the value is a pointer to the element.  The second
// member of the pair is a flag indicating whether the words between the start
// and end tags of that element are not to be indexed (true = "don't index").
//
// Note: I can't use an actual STL stack since I need to be able to clear the
// entire stack and, unfortunately, clear() isn't supported for stacks...an
// oversight in STL, IMHO.
//
typedef vector<pair<element_map::value_type const*,bool>> stack_type;

// local variables
static bool       dump_html_elements_opt;
static stack_type element_stack;

////////// local functions ////////////////////////////////////////////////////

inline void new_file() {
  element_stack.clear();
}

/**
 * Converts aither a numeric or character entity reference to its ASCII
 * character equivalent (if it has one).  A numeric reference is a character
 * sequence having the form:
 *
 *    &#d;
 *    &#xh;
 *    &#Xh;
 *
 * where 'd' is a sequence of 1 or more decimal digits [0-9] and 'h' is a
 * sequence of 1 or more hexadecimal digits [0-9A-Fa-f].  A character entity
 * reference is a character sequence having the form:
 *
 *    &ref;
 *
 * The numeric and character entities converted are listed in the \c entities.c
 * file.
 *
 * Example:
 *    The references in "r&eacute;sum&#233;" will be converted to the letter
 *    'e' resulting in the "resume" string.
 *
 * See also:
 *    Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "On SGML and HTML: SGML
 *    constructs used in HTML: Entities," HTML 4.0 Specification, section
 *    3.2.3, World Wide Web Consortium, April 1998.
 *      <http://www.w3.org/TR/PR-html40/intro/sgmltut.html#h-3.2.3>
 *
 *    International Standards Organization.  "ISO 8859-1: Information
 *    Processing -- 8-bit single-byte coded graphic character sets -- Part 1:
 *    Latin alphabet No. 1," 1987.
 *
 *    International Standards Organization.  "ISO 8879: Information Processing
 *    -- Text and Office Systems -- Standard Generalized Markup Language
 *    (SGML)," 1986.
 *
 * @param c This iterator is to be positioned at the character past the '&'; if
 * an entity is found, it is left after the ';'.
 * @return Returns the ASCII equivalent of the entity or ' ' (space) if either
 * there is no equivalent or the entity is malformed.
 */
static char entity_to_ascii( encoded_char_range::const_iterator &c ) {

  ////////// See if it's a numeric character reference ////////////////////////

  bool const is_num = (!c.at_end() && *c == '#');
  bool const is_hex = (is_num && !(++c).at_end() && (*c == 'x' || *c == 'X'));
  if ( is_hex ) ++c;

  ////////// Find the terminating ';' /////////////////////////////////////////

  char entity_buf[ Entity_Max_Size + 1 ];
  int entity_len = 0;

  while ( !c.at_end() && *c != ';' ) {
    if ( ++entity_len > Entity_Max_Size )
      return ' ';                       // give up looking
    entity_buf[ entity_len - 1 ] = *c++;
  } // while
  if ( c.at_end() )                     // didn't find it
    return ' ';
  ++c;                                  // put past ';'

  entity_buf[ entity_len ] = '\0';

  ////////// Look up character entity reference ///////////////////////////////

  if ( !is_num ) {
    static char_entity_map const &ch = char_entity_map::instance();
    return ch[ entity_buf ];
  }

  ////////// Parse a numeric character reference //////////////////////////////

  unsigned n = 0;
  for ( auto e = entity_buf; *e; ++e ) {
    if ( is_hex ) {
      if ( !is_xdigit( *e ) )           // bad hex num
        return ' ';
      n = (n << 4) | (is_digit( *e ) ? *e - '0' : tolower( *e ) - 'a' + 10);
    } else {
      if ( !is_digit( *e ) )            // bad dec num
        return ' ';
      n = n * 10 + *e - '0';
    }
  } // for

  return unicode_to_ascii( n );
}

/**
 * Given an attribute's name, find its value within an HTML or XHTML element's
 * start tag, e.g., find the value of the NAME attribute within the META
 * element.  Case is irrelevant.
 *
 * The HTML 4.0 specification is vague in stating whether whitespace is legal
 * around the '=' character separating an attribute's name from its value;
 * hence, this function is lenient in that it will consider:
 *
 *    NAME = "Author"
 *
 * and:
 *
 *    NAME="Author"
 *
 * equivalent.
 *
 * Caveat:
 *    This function only handles attributes with explicit values, i.e., those
 *    followed by an '=' and value, and not Boolean attributes.  But, since it
 *    was written with the intent of only parsing attributes in META tags,
 *    namely NAME and CONTENT, it's OK.
 *
 * See also:
 *    Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "On SGML and HTML: SGML
 *    constructs used in HTML: Attributes," HTML 4.0 Specification, section
 *    3.2.2, World Wide Web Consortium, April 1998.
 *      <http://www.w3.org/TR/PR-html40/intro/sgmltut.html#h-3.2.2>
 *
 * @param e The range spanning where to look.  If the attribute is found, the
 * range is rebounded to span the value; otherwise, it is not touched.
 * @param attribute The name of the attribute to find; it must be in lower
 * case.
 * @return Returns \c true only if the attribute was found.
 */
static bool find_attribute( encoded_char_range &e, char const *attribute ) {
  for ( auto c = e.begin(); !c.at_end(); ) {
    if ( !is_alpha( *c ) ) {
      ++c;
      continue;
    }
    //
    // We just found the start of a potentially matching attribute name: now
    // try to find its end.
    //
    char const *a = attribute;
    while ( !c.at_end() && to_lower( *c ) == *a )
      ++c, ++a;
    while ( !c.at_end() && ( is_alpha( *c ) || *c == '-' ) )
      ++c;
    while ( !c.at_end() && is_space( *c ) )
      ++c;
    if ( c.at_end() )
      break;
    if ( *c != '=' )                    // we do only NAME=VALUE form
      continue;
    while ( !(++c).at_end() && is_space( *c ) )
      ;
    if ( c.at_end() )
      break;
    //
    // Determine the span of the attribute's value: if it started with a quote,
    // it's terminated only by the matching closing quote; if not, it's
    // terminated by a whitespace character (or running into 'end').
    //
    // This is more lenient than the HTML 4.0 specification in that it allows
    // non-quoted values to contain characters other than the set
    // [A-Za-z0-9.-], i.e., any non-whitespace character.
    //
    char const quote = ( *c == '"' || *c == '\'' ) ? *c : 0;
    if ( quote && (++c).at_end() )
      break;
    auto const new_begin = c.pos();
    for ( ; !c.at_end(); ++c ) {
      if ( quote ) {                    // stop at matching quote only
        if ( *c == quote )
          break;
      } else if ( is_space( *c ) )
        break;                          // stop at whitespace
    } // for

    if ( !*a ) {                        // attribute name matched...
      e.begin_pos( new_begin );
      e.end_pos( c );                   // ...and got entire value  :)
      return true;
    }
    if ( c.at_end() )
      break;
    ++c;                                // attribute name didn't match
  } // for

  return false;
}

/**
 * Checks to see if the current HTML or XHTML element is the start of a
 * comment. If so, skip it scanning for the closing \c "--&gt;" character
 * sequence.  The HTML specification permits whitespace between the \c "--" and
 * the \c "&gt;" (for some strange reason).  Unlike skipping an ordinary HTML
 * or XHTML tag, quotes are not significant and no attempt must be made either
 * to "balance" them or to ignore what is in between them.
 *
 * This function is more lenient than the HTML 4.0 specification in that it
 * allows for a string of hyphens within a comment since this is common in
 * practice; the specification considers this to be an error.
 *
 * @param c The iterator to use.  It is presumed to be positioned at the first
 * character after the \c '&lt;'.  If the tag is the start of a comment, it is
 * repositioned at the first character past the tag, i.e., past the \c '&gt;';
 * otherwise, it is not touched.
 * @return Returns \c true only if the current element is the beginning of a
 * comment.
 */
static bool is_html_comment( encoded_char_range::const_iterator &c ) {
  if ( move_if_match( c, "!--" ) ) {
    while ( !c.at_end() ) {
      if ( *c++ == '-' && !c.at_end() && *c == '-' ) {
        auto const d = c;
        while ( !(++c).at_end() && is_space( *c ) )
          ;
        if ( c.at_end() || *c++ == '>' )
          break;
        c = d;
      }
    } // while
    return true;
  }
  return false;
}

/**
 * Scans for the closing \c '&gt;' of an HTML or XHTML tag skipping all
 * characters until it's found.  It takes care to ignore any \c '&lt;' inside
 * either single or double quotation marks.  It properly handles HTML and XHTML
 * comments.
 *
 * @param c The iterator to use.  It is presumed to start at any position after
 * the \c '&lt;' and before the \c '&gt;'; it is left at the first character
 * after the \c '&gt;'.
 * @return Returns \c true only if the tag is an HTML or XHTML comment.
 */
static bool skip_html_tag( encoded_char_range::const_iterator &c ) {
  if ( is_html_comment( c ) )
    return true;

  char quote = '\0';
  while ( !c.at_end() ) {
    if ( quote ) {                      // ignore everything...
      if ( *c++ == quote )              // ...until matching quote
        quote = '\0';
      continue;
    }
    if ( *c == '\"' || *c == '\'' ) {
      quote = *c++;                     // start ignoring stuff
      continue;
    }
    if ( *c++ == '>' )                  // found it  :)
      break;
  } // while

  return false;
}

////////// member functions ///////////////////////////////////////////////////

bool HTML_indexer::claims_option( option_stream::option const &opt ) {
  switch ( opt ) {
    case 'C': // Specify CLASS name(s) not to index.
      exclude_class_names.insert( to_lower( opt.arg() ) );
      return true;
    case 'H': // Dump recognized HTML and XHTML elements.
      dump_html_elements_opt = true;
      return true;
    default:
      return false;
  } // switch
}

char const* HTML_indexer::find_title( mmap_file const &file ) const {
  static char const *const title_tag[] = {  // tag_index
    "title",                                //  0
    "/title"                                //  1
  };

  new_file();

  int tag_index = 0;
  unsigned lines = 0;

  //
  // <TITLE>This is a title</TITLE>
  //        |              |
  //        after          before
  //
  // Mark the positions after the closing '>' of the start tag and before the
  // opening '<' of the end tag.  What's in between is the title.
  //
  mmap_file::const_iterator after, before;

  encoded_char_range::const_iterator c( file.begin(), file.end() );
  while ( !c.at_end() ) {
    if ( *c == '\n' && ++lines > num_title_lines ) {
      //
      // Didn't find <TITLE> within first num_title_lines lines of file: forget
      // it.
      //
      return nullptr;
    }

    if ( *c != '<' ) {                  // not a tag: forget it
      ++c;
      continue;
    }

    //
    // Found the start of an HTML or XHTML tag: mark the position before it in
    // case it turns out to be the </TITLE> tag.
    //
    before = c.pos();

    if ( is_html_comment( ++c ) )
      continue;

    //
    // Is the HTML or XHTML tag a TITLE tag?
    //
    bool const found_title_tag =
      move_if_match( c, title_tag[ tag_index ], true );
    skip_html_tag( c );                 // skip until '>'
    if ( !found_title_tag )             // wrong tag
      continue;

    if ( tag_index == 1 )               // found entire title
      return tidy_title( after, before );

    //
    // Found the <TITLE> tag: mark the position after it and begin looking for
    // the </TITLE> tag.
    //
    after = c.pos();
    ++tag_index;
  } // while

  //
  // The file has less than num_title_lines lines and no <TITLE> was found.
  //
  return nullptr;
}

void HTML_indexer::index_words( encoded_char_range const &e, int meta_id ) {
  char  word[ Word_Hard_Max_Size + 1 ];
  bool  in_word = false;
  int   len;

  encoded_char_range::const_iterator c = e.begin();
  while ( !c.at_end() ) {
    char ch = iso8859_1_to_ascii( *c++ );
    //
    // If the character is an '&' (the start of a entity reference), convert
    // the entity reference to ASCII.
    //
    if ( ch == '&' )
      ch = entity_to_ascii( c );

    ////////// Collect a word /////////////////////////////////////////////////

    if ( is_word_char( ch ) ) {
      if ( !in_word ) {                 // start a new word
        word[ 0 ] = ch;
        len = 1;
        in_word = true;
        continue;
      }
      if ( len < Word_Hard_Max_Size ) { // continue same word
        word[ len++ ] = ch;
        continue;
      }
      in_word = false;                  // too big: skip chars
      while ( !c.at_end() && is_word_char( iso8859_1_to_ascii( *c++ ) ) )
        ;
      continue;
    }

    if ( in_word ) {
      //
      // We ran into a non-word character, so index the word up to, but not
      // including, it.
      //
      in_word = false;
      index_word( word, len, meta_id );
    }

    if ( ch == '<' && meta_id == Meta_ID_None ) {
      //
      // If the character is a '<' (the start of an HTML or XHTML tag), and
      // we're not in the midst of indexing the value of a META element's
      // CONTENT attribute, then parse the HTML or XHTML tag.
      //
      parse_html_tag( c );
    }
  } // while

  if ( in_word ) {
    //
    // We ran into 'end' while still accumulating characters into a word, so
    // just index what we've got.
    //
    index_word( word, len, meta_id );
  }
}

void HTML_indexer::parse_html_tag( encoded_char_range::const_iterator &c ) {
  if ( c.at_end() )
    return;

  auto name = c;
  if ( skip_html_tag( c ) || *name == '!' || *name == '?' ) {
    //
    // Skip comments, type declarations, and XML processing instructions.
    //
    return;
  }
  name.end_pos( c.prev_pos() );
  bool const is_end_tag = *name == '/';

  ////////// Deal with elements of a class not to index ///////////////////////

  if ( !exclude_class_names.empty() ) {     // else, don't bother
    char tag_buf[ Tag_Name_Max_Size + 2 ];  // 1 for '/', 1 for null
    { // local scope
    //
    // Copy only the tag name by stopping at a whitespace character (or running
    // into the end of the tag); also convert it to lower case.  (We don't call
    // to_lower() in util.c so as not to waste time copying the entire tag with
    // its attributes since we only want the tag name.)
    //
    char *to = tag_buf;
    for ( auto from = name; !from.at_end() && !is_space( *from ); ) {
      //
      // Check to see if the name is too long to be a valid one for an HTML
      // element: if it is, invalidate it by writing "garbage" into the name so
      // something like "BLOCKQUOTED" (an invalid name) won't match
      // "BLOCKQUOTE" (a valid name) when one letter shorter and throw off
      // element closures.
      //
      if ( to - tag_buf >= Tag_Name_Max_Size + is_end_tag ) {
        to = tag_buf;
        *to++ = '\1';
        break;
      }
      *to++ = to_lower( *from++ );
    } // for
    *to = '\0';
    } // local scope

    ////////// Close open element(s) //////////////////////////////////////////

    while ( !element_stack.empty() &&
            element_stack.back().first->second.close_tags.contains( tag_buf ) ){
      //
      // This element closes the currently open element.
      //
      if ( element_stack.back().second ) {
        //
        // The currently open element is a member of one of the classes not
        // being being indexed: resume indexing.
        //
        resume_indexing();
      }
      auto const start_tag = element_stack.back().first->first;
      element_stack.pop_back();

      //
      // We have to stop closing elements if we encounter the start tag
      // matching the end tag.
      //
      if ( !::strcmp( tag_buf + 1, start_tag ) )
        break;
    } // while

    if ( is_end_tag ) {
      //
      // The tag is an end tag: it doesn't have attributes.
      //
      return;
    }

    ////////// Look for CLASS attribute ///////////////////////////////////////

    bool is_no_index_class = false;

    encoded_char_range class_att = name;
    if ( find_attribute( class_att, "class" ) ) {
      //
      // CLASS attribute values can contain multiple classes separated by
      // whitespace: we must iterate over all of them to see if one of them is
      // among the set not to index.
      //
      char *names = to_lower( class_att );
      char const *name;
      while ( (name = ::strtok( names, " \f\n\r\t\v" )) ) {
        if ( exclude_class_names.contains( name ) ) {
          is_no_index_class = true;
          break;
        }
        names = nullptr;
      } // while
    }

    ////////// Look-up the HTML element ///////////////////////////////////////

    static auto const &elements = element_map::instance();
    auto const e = elements.find( tag_buf );
    if ( e != elements.end() ) {
      //
      // We found the element in our internal table: now do different stuff
      // depending upon whether its end tag is forbidden or not.
      //
      if ( e->second.end_tag != element::et_forbidden ) {
        //
        // The element's end tag isn't forbidden, so we have to keep track of
        // it on the stack and whether the text between the start and end tags
        // should be indexed or not.
        //
        // Note that we have to keep track of all HTML or XHTML elements even
        // if they are not members of a class not to be indexed because they
        // could be nested inside of an element that is, e.g.:
        //
        //    <DIV CLASS=ignore>
        //      <SPAN CLASS=other>Hello</SPAN>
        //    </DIV>
        //
        element_stack.push_back( make_pair( &*e, is_no_index_class ) );
        if ( is_no_index_class ) {
          //
          // A class name in the value of this element's CLASS attribute is
          // among the set not to index: suspend indexing.
          //
          suspend_indexing();
        }
      }
      if ( is_no_index_class ) {
        //
        // Regardless of this element's end tag, it is to be ignored,
        // so we can stop parsing it.
        //
        return;
      }
    } else {
      //
      // We didn't find the element in our internal table: ignore it.  We
      // really should do something better because this could potentially mess
      // up the proper closing of elements, but, since we know nothing about
      // this element, there's nothing better that can be done.
      //
    }
  }

  if ( is_end_tag ) {
    //
    // The tag is an end tag: it doesn't have attributes.
    //
    return;
  }

  ////////// Look for a TITLE attribute ///////////////////////////////////////

  encoded_char_range title_att = name;
  if ( find_attribute( title_att, "title" ) )
    index_words( title_att );

  ////////// Look for an ALT attribute ////////////////////////////////////////

  if ( move_if_match( name, "area", true ) ||
       move_if_match( name, "img", true ) ||
       move_if_match( name, "input", true ) ) {
    encoded_char_range alt_att = name;
    if ( find_attribute( alt_att, "alt" ) )
      index_words( alt_att );
    return;
  }

  ////////// Parse a META element /////////////////////////////////////////////

  if ( move_if_match( name, "meta", true ) ) {
    encoded_char_range name_att    = name;
    encoded_char_range content_att = name;
    if ( find_attribute( name_att, "name" ) &&
         find_attribute( content_att, "content" ) ) {
      int meta_id = Meta_ID_None;
      if ( associate_meta ) {
        //
        // Do not index the words in the value of the CONTENT attribute if
        // either the value of the NAME attribute (canonicalized to lower case)
        // is among the set of meta names to exclude or not among the set to
        // include.
        //
        meta_id = find_meta( to_lower( name_att ) );
        if ( meta_id == Meta_ID_None )
          return;
      }
      //
      // Index the words in the value of the CONTENT attribute marking them as
      // being associated with the value of NAME attribute.
      //
      index_words( content_att, meta_id );
    }
    return;
  }

  ////////// Look for a STANDBY attribute /////////////////////////////////////

  if ( move_if_match( name, "object", true ) ) {
    encoded_char_range standby_att = name;
    if ( find_attribute( standby_att, "standby" ) )
      index_words( standby_att );
    return;
  }

  ////////// Look for a SUMMARY attribute /////////////////////////////////////

  if ( move_if_match( name, "table", true ) ) {
    encoded_char_range summary_att = name;
    if ( find_attribute( summary_att, "summary" ) )
      index_words( summary_att );
    return;
  }
}

option_stream::spec const* HTML_indexer::option_spec() const {
  static option_stream::spec const opt_spec[] = {
    { "no-class",   1, 'C'  },
    { "dump-html",  0, 'H'  },
    { nullptr,      0, '\0' }
  };
  return opt_spec;
}

void HTML_indexer::post_options() {
  if ( dump_html_elements_opt ) {
    static auto const &elements = element_map::instance();
    ::copy( elements.begin(), elements.end(),
      ostream_iterator<element_map::value_type>( cout,"\n" )
    );
    ::exit( Exit_Success );
  }
}

void HTML_indexer::usage( ostream &o ) const {
  o <<
  "---------------------------- HTML options -------------------------------------\n"
  "-C c   | --no-class c       : Class name not to index [default: none]\n"
  "-H     | --dump-html        : Dump built-in recognized HTML/XHTML elements, exit\n";
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
