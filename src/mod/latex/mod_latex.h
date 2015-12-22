/*
**      SWISH++
**      src/mod/latex/mod_latex.h
**
**      Copyright (C) 2002  Paul J. Lucas
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

#ifdef  MOD_latex

#ifndef mod_latex_H
#define mod_latex_H

// local
#include "encoded_char.h"
#include "indexer.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class LaTeX_indexer : public indexer
//
// DESCRIPTION
//
//      A LaTeX_indexer is-an indexer for indexing LaTeX files.
//
// SEE ALSO
//
//      Leslie Lamport.  "LaTeX: A Document Preparation System, 2nd ed."
//      Addison-Wesley, Reading, MA, 1994.
//
//*****************************************************************************
{
public:
    LaTeX_indexer() : indexer( "LaTeX" ) { }

    virtual char const* find_title( PJL::mmap_file const& ) const;
    virtual void        index_words(
                            encoded_char_range const&,
                            int meta_id = Meta_ID_None
                        );
private:
    char const* parse_latex_command( encoded_char_range::const_iterator& );
};

#endif  /* mod_latex_H */

#endif  /* MOD_latex */
/* vim:set et sw=4 ts=4: */
