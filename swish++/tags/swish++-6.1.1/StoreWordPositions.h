/*
**      SWISH++
**      StoreWordPositions.h
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

#ifndef StoreWordPositions_H
#define StoreWordPositions_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class StoreWordPositions : public conf<bool>
//
// DESCRIPTION
//
//      A StoreWordPositions is-a conf<bool> containing the Boolean value
//      indicating whether to store word positions during indexing needed to do
//      "near" searches later during searching.
//
//      This is the same as index's -P command-line option.
//
//*****************************************************************************
{
public:
    StoreWordPositions() : conf<bool>( "StoreWordPositions", true ) { }
    CONF_BOOL_ASSIGN_OPS( StoreWordPositions )
};

extern StoreWordPositions store_word_positions;

#endif  /* StoreWordPositions_H */

#endif  /* FEATURE_word_pos */
/* vim:set et sw=4 ts=4: */
