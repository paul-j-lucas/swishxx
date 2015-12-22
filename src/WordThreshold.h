/*
**      SWISH++
**      src/WordThreshold.h
**
**      Copyright (C) 2001  Paul J. Lucas
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

#ifndef WordThreshold_H
#define WordThreshold_H

// local
#include "config.h"
#include "conf_int.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class WordThreshold : public conf<int>
//
// DESCRIPTION
//
//      A WordThreshold is-a conf<int> containing the word count past which
//      partial indicies are generated and merged since all the words are too
//      big to fit into memory at the same time.  See also the comment in
//      config.h for WordThreshold_Default.
//
//      Only the super-user can specify a value larger than the compiled-in
//      default.
//
//      This is the same as index's -W command-line option.
//
//*****************************************************************************
{
public:
    WordThreshold() :
        conf<int>( "WordThreshold", WordThreshold_Default, 100 ) { }
    CONF_INT_ASSIGN_OPS( WordThreshold );
private:
    virtual void    parse_value( char *line );
};

extern WordThreshold word_threshold;

#endif  /* WordThreshold_H */
/* vim:set et sw=4 ts=4: */
