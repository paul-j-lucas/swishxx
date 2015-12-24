/*
**      SWISH++
**      src/mod/man/mod_man.h
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

#ifndef mod_man_H
#define mod_man_H

// local
#include "encoded_char.h"
#include "indexer.h"
#include "pjl/mmap_file.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class man_indexer : public indexer
//
// DESCRIPTION
//
//      An man_indexer is-an indexer for indexing Unix manual pages.
//
//*****************************************************************************
{
public:
    man_indexer() : indexer( "man" ) { }

    virtual char const* find_title( PJL::mmap_file const& ) const;
    virtual void        index_words(
                            encoded_char_range const&,
                            int meta_id = Meta_ID_None
                        );
private:
    void                parse_man_macro( char const *&pos, char const *end );
};

#endif /* mod_man_H */
/* vim:set et sw=4 ts=4: */
