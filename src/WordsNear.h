/*
**      SWISH++
**      src/WordsNear.h
**
**      Copyright (C) 2004-2015  Paul J. Lucas
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

#ifdef WITH_WORD_POS

#ifndef WordsNear_H
#define WordsNear_H

// local
#include "conf_unsigned.h"
#include "config.h"

// standard
#include <climits>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %WordsNear is-a conf&lt;unsigned&gt; containing the maximum number of words
 * apart two words can be to be considered "near" each other.
 *
 * This is the same as search's \c -n command-line option.
 */
class WordsNear : public conf<unsigned> {
public:
  WordsNear() :
    conf<unsigned>(
      "WordsNear", WordsNear_Default, 1, std::numeric_limits<value_type>::max()
    )
  {
  }
  CONF_INT_ASSIGN_OPS( WordsNear )
};

extern WordsNear words_near;

#endif /* WordsNear_H */

///////////////////////////////////////////////////////////////////////////////

#endif /* WITH_WORD_POS */
/* vim:set et sw=2 ts=2: */
