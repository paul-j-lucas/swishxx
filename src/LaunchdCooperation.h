/*
**      SWISH++
**      src/LaunchdCooperation.h
**
**      Copyright (C) 2005  Paul J. Lucas
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

#if defined( SEARCH_DAEMON ) && defined( __APPLE__ )

#ifndef LaunchdCooperation_H
#define LaunchdCooperation_H

// local
#include "conf_bool.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class LaunchdCooperation : public conf<bool>
//
// DESCRIPTION
//
//      A LaunchdCooperation is-a conf<bool> containing the Boolean value
//      indicating whether to cooperate with Mac OS X's launchd.
//
//      This is the same as search's -l command-line option.
//
//*****************************************************************************
{
public:
    LaunchdCooperation() : conf<bool>( "LaunchdCooperation", false ) { }
    CONF_BOOL_ASSIGN_OPS( LaunchdCooperation )
};

extern LaunchdCooperation launchd_cooperation;

#endif  /* LaunchdCooperation_H */

#endif  /* SEARCH_DAEMON && __APPLE__ */
/* vim:set et sw=4 ts=4: */
