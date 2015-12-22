/*
**      SWISH++
**      src/conf_enum.c
**
**      Copyright (C) 2000  Paul J. Lucas
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

// standard
#include <cstdlib>                      /* for exit(3) */
#include <iostream>

// local
#include "conf_enum.h"
#include "exit_codes.h"
#include "pjl/auto_vec.h"
#include "util.h"

using namespace PJL;
using namespace std;

extern char const*  me;

//*****************************************************************************
//
// SYNOPSIS
//
        conf_enum::conf_enum(
            char const *name, char const *const legal_values[]
        ) :
//
// DESCRIPTION
//
//      Construct (initialize) a conf_enum.
//
// PARAMETERS
//
//      name            The name of the configuration variable.
//
//      legal_values    The set of legal values.
//
//*****************************************************************************
    conf<std::string>( name, legal_values[0] ),
    legal_values_( legal_values )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool conf_enum::is_legal( char const *value, ostream &err ) const
//
// DESCRIPTION
//
//      Checks to see if a given value is legal, i.e., among the pre-determined
//      set of legal values.
//
// PARAMETERS
//
//      value   The value to be checked.
//
//      err     The ostream to write an error message to, if any.
//
// RETURN VALUE
//
//      Returns true only if the value is legal.
//
//*****************************************************************************
{
    if ( *value ) {
        auto_vec<char> const lower( to_lower_r( value ) );
        for ( char const *const *v = legal_values_; *v; ++v )
            if ( !::strcmp( lower, *v ) )
                return true;
    }
    err << error << '"' << name() << "\" is not one of: ";
    bool comma = false;
    for ( char const *const *v = legal_values_; *v; ++v ) {
        if ( comma )
            err << ", ";
        else
            comma = true;
        err << *v;
    }
    err << '\n';
    return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ void conf_enum::parse_value( char *line )
//
// DESCRIPTION
//
//      Parse an enum value from the line of text.  It must be one of the legal
//      values.
//
// PARAMETERS
//
//      line    The line of text to be parsed.
//
//*****************************************************************************
{
    if ( !is_legal( line ) )
        ::exit( Exit_Config_File );
    auto_vec<char> lower( to_lower_r( line ) );
    conf<string>::parse_value( lower );
}
/* vim:set et sw=4 ts=4: */
