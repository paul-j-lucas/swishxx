/*
**      SWISH++
**      src/mod/html/entities.cpp
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
#include "entities.h"

///////////////////////////////////////////////////////////////////////////////

char_entity_map::char_entity_map() :
  map_{
    { "amp",    '&' },
    { "apos",  '\'' },                    // apos is in XHTML
    { "Aacute", 'A' }, { "aacute", 'a' },
    { "Acirc",  'A' }, { "acirc",  'a' },
    { "AElig",  'A' }, { "aelig",  'a' },
    { "Agrave", 'A' }, { "agrave", 'a' },
    { "Aring",  'A' }, { "aring",  'a' },
    { "Atilde", 'A' }, { "atilde", 'a' },
    { "Auml",   'A' }, { "auml",   'a' },
    { "Ccedil", 'C' }, { "ccedil", 'c' },
    { "Eacute", 'E' }, { "eacute", 'e' },
    { "Ecirc",  'E' }, { "ecirc",  'e' },
    { "Egrave", 'E' }, { "egrave", 'e' },
    { "ETH",    'D' }, { "eth",    'd' },
    { "Euml",   'E' }, { "euml",   'e' },
    { "Iacute", 'I' }, { "iacute", 'i' },
    { "Icirc",  'I' }, { "icirc",  'i' },
    { "Igrave", 'E' }, { "igrave", 'i' },
    { "Iuml",   'I' }, { "iuml",   'i' },
    { "Ntilde", 'N' }, { "ntilde", 'n' },
    { "Oacute", 'O' }, { "oacute", 'o' },
    { "Ocirc",  'O' }, { "ocirc",  'o' },
    { "Ograve", 'O' }, { "ograve", 'o' },
    { "Oslash", 'O' }, { "oslash", 'o' },
    { "Otilde", 'O' }, { "otilde", 'o' },
    { "Ouml",   'O' }, { "ouml",   'o' },
    { "Scaron", 'S' }, { "scaron", 's' }, // in XHTML
    { "szlig",  's' },
    { "Uacute", 'U' }, { "uacute", 'u' },
    { "Ucirc",  'U' }, { "ucirc",  'u' },
    { "Ugrave", 'U' }, { "ugrave", 'u' },
    { "Uuml",   'U' }, { "uuml",   'u' },
    { "Yacute", 'Y' }, { "yacute", 'y' },
    { "Yuml",   'Y' }, { "yuml",   'y' }
  }
{
}

char_entity_map const& char_entity_map::instance() {
  static char_entity_map const m;
  return m;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
