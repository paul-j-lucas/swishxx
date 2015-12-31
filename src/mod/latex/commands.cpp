/*
**      SWISH++
**      src/mod/latex/commands.cpp
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

// local
#include "commands.h"

///////////////////////////////////////////////////////////////////////////////

command_map::command_map() {
  //
  // A command maps to an action which is what to do about it when encountered
  // in a document.  In most cases, the action is simply text to be
  // substituted.  In cases where the action is either '{' or '[', the
  // character is to be "balanced" by looking for the closing '}' or ']' and
  // the words in between are indexed seperately.
  //
  static command const command_table[] = {

  //  \name             action
  //  ----------------  -------
      "\"",             "",
      "#",              "#",
      "$",              "$",
      "%",              "%",
      "&",              "&",
      "'",              "",
      ".",              "",
      "=",              "",
      "@",              "",
      "\\",             "\n",
      "^",              "",
      "_",              "_",
      "`",              "",
      "{",              " ",
      "}",              " ",

      "aa",             "a",
      "AA",             "A",
      "ae",             "ae",
      "AE",             "AE",
      "b",              "",
      "c",              "",
      "d",              "",
      "H",              "",
      "i",              "i",
      "j",              "j",
      "LaTeX",          "LaTeX",
      "l",              "l",
      "L",              "L",
      "OE",             "OE",
      "ss",             "ss",
      "t",              "",
      "TeX",            "TeX",
      "u",              "",
      "v",              "",

      "author",         "{",
      "caption",        "{",
      "chapter",        "{",
      "emph",           "{",
      "fbox",           "{",
      "footnote",       "{",
      "item",           "[",
      "mbox",           "{",
      "paragraph",      "{",
      "part",           "{",
      "section",        "{",
      "sout",           "{",
      "subparagraph",   "{",
      "subsection",     "{",
      "subsubsection",  "{",
      "textbf",         "{",
      "textit",         "{",
      "textmd",         "{",
      "textnormal",     "{",
      "textrm",         "{",
      "textsc",         "{",
      "textsf",         "{",
      "textsl",         "{",
      "texttt",         "{",
      "textup",         "{",
      "title",          "{",
      "uline",          "{",
      "underline",      "{",
      "uwave",          "{",
      "xout",           "{",

      nullptr
  };

  for ( auto c = command_table; c->name; ++c )
    operator[]( c->name ).action = c->action;
}

command_map const& command_map::instance() {
  static command_map const m;
  return m;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
