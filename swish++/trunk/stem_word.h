/*
**      SWISH++
**      stem_word.h
**
**      Copyright (C) 1998  Paul J. Lucas
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

#ifndef stem_word_H
#define stem_word_H

// local
#include "less.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct less_stem : std::less< char const* >
//
// DESCRIPTION
//
//      A less_stem is-a less< char const* > that compares C-style strings but
//      possibly stems (suffix strips) the words before comparison.
//
//*****************************************************************************
{
    less_stem( bool stem ) : stem_func_( stem ? stem_word : no_stem ) { }

    result_type
    operator()( first_argument_type a, second_argument_type b ) const {
        return std::strcmp( stem_func_( a ), stem_func_( b ) ) < 0;
    }
private:
    char const*         (*const stem_func_)( char const *word );
    static char const*  no_stem( char const *word ) { return word; }
    static char const*  stem_word( char const *word );
};

#endif  /* stem_word_H */
/* vim:set et sw=4 ts=4: */
