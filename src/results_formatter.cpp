/*
**      SWISH++
**      src/results_formatter.c
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

// local
#include "results_formatter.h"

//*****************************************************************************
//
// SYNOPSIS
//
        results_formatter::~results_formatter()
//
// DESCRIPTION
//
//      Destroy a results_formatter.
//
// NOTE
//
//      This is out-of-line only because it's virtual.
//
//*****************************************************************************
{
    // do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
        void results_formatter::post() const
//
// DESCRIPTION
//
//  Output any trailing information.
//
//*****************************************************************************
{
        // do nothing
}
/* vim:set et sw=4 ts=4: */