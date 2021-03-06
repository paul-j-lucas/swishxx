/*
**      SWISH++
**      src/mod/html/elements.cpp
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
#include "elements.h"

///////////////////////////////////////////////////////////////////////////////

element_map::element_map() {
  typedef char const *const cpcc;
  //
  // Declare character pointers having values equal to element's end_tag_type
  // enum so that we can have a simple array of strings below where each "row"
  // has varying length.  It's kind of a hack, but it works.
  //
  cpcc F = reinterpret_cast<char const*>( element::et_forbidden );
  cpcc O = reinterpret_cast<char const*>( element::et_optional  );
  cpcc R = reinterpret_cast<char const*>( element::et_required  );

  //
  // Elements that have forbidden end tags (obviously) have no tags listed for
  // them in the rows below.
  //
  // Elements that have optional end tags have a null-terminated list of tags
  // that close them, either explicitly (their own end tag) or implicitly (a
  // tag from some other element).
  //
  // Elements that have required end tags simply list their single end tag.
  // Yes, it's easy, given a start tag, to know what it's end tag is (the same
  // tag, but with a leading '/'); however, the code in parse_html_tag() in
  // mod_html.c is made simpler by explicitly giving the end tag here rather
  // than having to construct a temporary string prepending a '/' to the tag.
  //
  static char const *const end_tag_table[] = {

  //  element       end tag tags that close it  status
  //  ------------  ------- ------------------  ---------------
      "a",          R,      "/a",
      "abbr",       R,      "/abbr",
      "access",     R,      "/acces",           // XHTML 2.0
      "acronym",    R,      "/acronym",
      "action",     R,      "/action",          // XHTML 2.0
      "address",    R,      "/address",
      "applet",     R,      "/applet",          // deprecated
      "area",       F,
      "area",       F,
      "article",    R,      "/article",
      "aside",      R,      "/aside",
      "audio",      R,      "/audio",
      "b",          R,      "/b",
      "base",       F,
      "basefont",   F,                          // deprecated
      "bdi",        R,      "/bdi",
      "bdo",        R,      "/bdo",
      "big",        R,      "/big",
      "blink",      R,      "/blink",           // nonstandard
      "blockcode",  R,      "/blockcode",       // XHTML 2.0
      "blockquote", R,      "/blockquote",
      "body",       O,      "/body", "/html", nullptr,
      "br",         F,
      "button",     R,      "/button",
      "canvas",     R,      "/canvas",
      "caption",    R,      "/caption",
      "center",     R,      "/center",          // deprecated
      "cite",       R,      "/cite",
      "code",       R,      "/code",
      "col",        F,

      "colgroup",   O,      "colgroup", "/colgroup",
                            "tbody", "tfoot", "thead",
                            "tr",
                            "/table",
                            nullptr,

      "data",       R,      "/data",
      "datalist",   R,      "/datalist",
      "dd",         O,      "dd", "/dd", "/dl", "dt", "/dt", nullptr,
      "del",        R,      "/del",
      "dfn",        R,      "/dfn",
      "dir",        R,      "/dir",             // deprecated
      "di",         R,      "/di",              // XHTML 2.0
      "div",        R,      "/div",
      "dl",         R,      "/dl",
      "dt",         O,      "dt", "/dt", "/dl", nullptr,
      "em",         R,      "/em",
      "embed",      R,      "/embed",           // nonstandard
      "fieldset",   R,      "/fieldset",
      "figcaption", R,      "/figcaption",
      "figure",     R,      "/figure",
      "font",       R,      "/font",            // deprecated
      "footer",     R,      "/footer",
      "form",       R,      "/form",
      "frame",      F,
      "frameset",   R,      "/frameset",
      "group",      R,      "/group",
      "h",          R,      "/h",               // XHTML 2.0
      "h1",         R,      "/h1",
      "h2",         R,      "/h2",
      "h3",         R,      "/h3",
      "h4",         R,      "/h4",
      "h5",         R,      "/h5",
      "h6",         R,      "/h6",
      "head",       O,      "body", "/body", "/head", "/html", nullptr,
      "header",     R,      "/header",
      "heading",    R,      "/heading",         // XHTML 2.0
      "hr",         F,
      "html",       O,      "/html", nullptr,
      "i",          R,      "/i",
      "iframe",     R,      "/iframe",
      "ilayer",     R,      "/ilayer",          // nonstandard
      "img",        F,
      "input",      F,
      "ins",        R,      "/ins",
      "isindex",    F,                          // deprecated
      "kbd",        R,      "/kbd",
      "keygen",     F,                          // nonstandard
      "l",          R,      "/l",               // XHTML 2.0
      "label",      R,      "/label",
      "layer",      R,      "/layer",           // nonstandard
      "legend",     R,      "/legend",
      "li",         O,      "li", "/li", "/ol", "/ul", nullptr,
      "line",       R,      "/line",            // XHTML 2.0
      "link",       F,
      "map",        R,      "/map",
      "mark",       R,      "/mark",
      "menu",       R,      "/menu",            // deprecated
      "meta",       F,
      "meter",      R,      "/meter",
      "multicol",   R,      "/multicol",        // nonstandard
      "name",       R,      "/name",            // XHTML 2.0
      "nav",        R,      "/nav",
      "nl",         R,      "/nl",              // XHTML 2.0
      "nobr",       F,                          // nonstandard
      "noembed",    R,      "/noembed",         // nonstandard
      "noframes",   R,      "/noframes",
      "nolayer",    R,      "/nolayer",         // nonstandard
      "noscript",   R,      "/noscript",
      "object",     R,      "/object",
      "ol",         R,      "/ol",
      "optgroup",   R,      "/optgroup",

      "option",     O,      "/optgroup",
                            "option", "/option",
                            "/select",
                            nullptr,

      "output",     R,      "/output",

      "p",          O,      "address", "/address",
                            "applet", "/applet",
                            "blockquote", "/blockquote",
                            "/body",
                            "br",
                            "caption", "/caption",
                            "center", "/center",
                            "dd", "/dd",
                            "div", "/div",
                            "dl", "/dl",
                            "dt", "/dt",
                            "embed", "/embed",
                            "form", "/form",
                            "frame", "/frame",
                            "frameset", "/frameset",
                            "h1", "/h1",
                            "h2", "/h2",
                            "h3", "/h3",
                            "h4", "/h4",
                            "h5", "/h5",
                            "h6", "/h6",
                            "hr",
                            "/html",
                            "layer", "/layer",
                            "li", "/li",
                            "map", "/map",
                            "multicol", "/multicol",
                            "noembed", "/noembed",
                            "noframes", "/noframes",
                            "nolayer", "/nolayer",
                            "noscript", "/noscript",
                            "object", "/object",
                            "ol", "/ol",
                            "p", "/p",
                            "plaintext",
                            "pre", "/pre",
                            "script", "/script",
                            "select", "/select",
                            "style", "/style",
                            "table", "/table",
                            "tbody", "/tbody"
                            "td", "/td",
                            "tfoot", "/tfoot"
                            "th", "/th",
                            "thead", "/thead"
                            "tr", "/tr",
                            "ul", "/ul",
                            "xmp", "/xmp",
                            nullptr,

      "param",      F,
      "plaintext",  F,                          // deprecated
      "pre",        R,      "/pre",
      "progress",   R,      "/progress",
      "q",          R,      "/q",               // deprecated
      "quote",      R,      "/quote",           // XHTML 2.0

      "rb",         R,      "/rb",
      "rbc",        R,      "/rbc",
      "rp",         R,      "/rp",
      "rt",         R,      "/rt",
      "rtc",        R,      "/rtc",
      "ruby",       R,      "/ruby",            // ruby elements

      "s",          R,      "/s",               // deprecated
      "samp",       R,      "/samp",
      "script",     R,      "/script",
      "section",    R,      "/section",         // XHTML 2.0
      "select",     R,      "/select",
      "server",     R,      "/server",          // nonstandard
      "small",      R,      "/small",
      "source",     F,
      "spacer",     F,                          // nonstandard
      "span",       R,      "/span",
      "strike",     R,      "/strike",          // deprecated
      "strong",     R,      "/strong",
      "style",      R,      "/style",
      "sub",        R,      "/sub",
      "summary",    R,      "/summary",         // XHTML 2.0
      "sup",        R,      "/sup",
      "table",      R,      "/table",
      "tbody",      O,      "tbody", "/tbody", nullptr,

      "td",         O,      "tbody", "/tbody",
                            "td", "/td",
                            "tfoot", "/tfoot",
                            "th",
                            "/table",
                            "tr", "/tr",
                            nullptr,

      "template",   R,      "/template",
      "textarea",   R,      "/textarea",
      "tfoot",      O,      "tbody", "/tfoot", "thead", nullptr,

      "th",         O,      "tbody", "/tbody",
                            "td",
                            "tfoot", "/tfoot",
                            "th", "/th",
                            "/table",
                            "tr", "/tr",
                            nullptr,

      "thead",      O,      "tbody", "tfoot", "/thead", nullptr,
      "time",       R,      "/time",
      "title",      R,      "/title",

      "tr",         O,      "tbody", "/tbody",
                            "tfoot", "/tfoot",
                            "/thead",
                            "tr", "/tr",
                            "/table",
                            nullptr,

      "track",      F,
      "tt",         R,      "/tt",
      "u",          R,      "/u",               // deprecated
      "ul",         R,      "/ul",
      "var",        R,      "/var",
      "video",      R,      "/video",
      "wbr",        F,                          // nonstandard
      "xmp",        R,      "/xmp",             // deprecated

      nullptr
  };

  for ( auto p = end_tag_table; *p; ++p ) {
    //
    // The ugly double, old-style casts below are necessary because C++ has
    // become overly pedantic about casting to enums.
    //
    auto const v = (element::end_tag_type const)(long const)(p[1]);
    auto &e = insert( value_type( *p++, element( v ) ) ).first->second;

    switch ( v ) {

      case element::et_forbidden:
        // do nothing
        break;

      case element::et_optional:
        //
        // Elements that have optional end tags have a null-terminated list of
        // closing tags.
        //
        while ( *++p )
          e.close_tags.insert( *p );
        break;

      case element::et_required:
        //
        // Elements that have required and tags have a single closing tag.
        //
        e.close_tags.insert( *++p );
        break;
    } // switch
  } // for
}

element_map const& element_map::instance() {
  static element_map const m;
  return m;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
