/*
**      SWISH++
**      src/mod/html/mod_html.h
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

#ifndef mod_html_H
#define mod_html_H

// local
#include "elements.h"
#include "indexer.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * An %HTML_indexer is-an indexer for indexing HTML or XHTML files.
 */
class HTML_indexer : public indexer {
public:
  HTML_indexer() : indexer( "HTML" ) { }

  // inherited
  char const* find_title( PJL::mmap_file const& ) const;
  void index_words( encoded_char_range const&, meta_id_type = Meta_ID_None );

private:
  /**
   * This function does everything \c skip_html_tag() does but additionally
   * does extra parsing for certain HTML or XHTML elements:
   *
   * 1. If the tag contains a CLASS attribute whose value is among the set of
   *    class names specified as those not to index, then all the text up to
   *    the tag that ends the element will not be indexed.
   *
   *    For an element that has an optional end tag, "the tag that ends the
   *    element" is either the element's end tag or a tag of another element
   *    that implicitly ends it; for an element that does not have an end tag,
   *    "the tag that ends the element" is the element's start tag.
   *
   * 2. If the tag contains a TITLE attribute, it indexes the words of its
   *    value.
   *
   * 3. If the tag is an AREA, IMG, or INPUT element and contains an ALT
   *    attribute, it indexes the words of its value.
   *
   * 4. If the tag is a META element, it parses its NAME and CONTENT attributes
   *    indexing the words of the latter associated with the former.
   *
   * 5. If the tag is an OBJECT element and contains a STANDBY attribute, it
   *    indexes the words of its value.
   *
   * 6. If the tag is a TABLE element and contains a SUMMARY attribute, it
   *    indexes the words of its value.
   *
   * @param c The iterator to use.  It must be positioned at the character
   * after the '<'; it is repositioned at the first character after the '>'.
   *
   * See also:
   *    Dave Raggett, Arnaud Le Hors, and Ian Jacobs. "The global structure of
   *    an HTML document: The document head: The title attribute," HTML 4.0
   *    Specification, section 7.4.3, World Wide Web Consortium, April 1998.
   *      <http://www.w3.org/TR/REC-html40/struct/global.html#adef-title>
   *
   *    ---.  "The global structure of an HTML document: The document head:
   *    Meta data," HTML 4.0 Specification, section 7.4.4, World Wide Web
   *    Consortium, April 1998.
   *      <http://www.w3.org/TR/PR-html40/struct/global.html#h-7.4.4>
   *
   *    ---.  "The global structure of an HTML document: The document body:
   *    Element identifiers: the id and class attributes," HTML 4.0
   *    Specification, section 7.5.2, World Wide Web Consortium, April 1998.
   *      <http://www.w3.org/TR/PR-html40/struct/global.html#h-7.5.2>
   *
   *    ---. "Tables: Elements for constructing tables: The TABLE element,"
   *    HTML 4.0 Specification, section 11.2.1, World Wide Web Consortium,
   *    April 1998.
   *      <http://www.w3.org/TR/REC-html40/struct/tables.html#adef-summary>
   *
   *    ---. "Objects, Images, and Applets: Generic inclusion: the OBJECT
   *    element," HTML 4.0 Specification, section 13.3, World Wide Web
   *    Consortium, April 1998.
   *      <http://www.w3.org/TR/REC-html40/struct/objects.html#adef-standby>
   *
   *    ---. "Objects, Images, and Applets: How to specify alternate text,"
   *    HTML 4.0 Specification, section 13.8, World Wide Web Consortium, April
   *    1998.
   *      <http://www.w3.org/TR/REC-html40/struct/objects.html#h-13.8>
   */
  void parse_html_tag( encoded_char_range::const_iterator& );

  // inherited
  bool claims_option( PJL::option_stream::option const& );
  PJL::option_stream::spec const* option_spec() const;
  void post_options();
  void usage( std::ostream& ) const;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* mod_html_H */
/* vim:set et sw=2 ts=2: */
