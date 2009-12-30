/*
**      SWISH++
**      src/WordsNear.h
**
**      Copyright (C) 2004  Paul J. Lucas
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

#ifdef  FEATURE_word_pos

#ifndef WordsNear_H
#define WordsNear_H

// standard
#include <climits>

// local
#include "conf_int.h"
#include "config.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class WordsNear : public conf<int>
//
// DESCRIPTION
//
//      A WordsNear is-a conf<int> containing the maximum number of words apart
//      two words can be to be considered "near" each other.
//
//      This is the same as search's -n command-line option.
//
//*****************************************************************************
{
public:
    WordsNear() : conf<int>( "WordsNear", WordsNear_Default, 1, INT_MAX ) { }
    CONF_INT_ASSIGN_OPS( WordsNear )
};

extern WordsNear words_near;

#endif  /* WordsNear_H */

#endif  /* FEATURE_word_pos */
/* vim:set et sw=4 ts=4: */
