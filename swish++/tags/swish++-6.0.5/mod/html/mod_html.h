/*
**      SWISH++
**      mod/html/mod_html.h
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

#ifdef  MOD_html

#ifndef mod_html_H
#define mod_html_H

// local
#include "elements.h"
#include "encoded_char.h"
#include "fake_ansi.h"                          /* for std */
#include "indexer.h"

//*****************************************************************************
//
// SYNOPSIS
//
        class HTML_indexer : public indexer
//
// DESCRIPTION
//
//      An HTML_indexer is-an indexer for indexing HTML or XHTML files.
//
// SEE ALSO
//
//      Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "HTML 4.0
//      Specification," World Wide Web Consortium, April 1998.
//          http://www.w3.org/TR/PR-html40/
//
//*****************************************************************************
{
public:
    HTML_indexer() : indexer( "HTML" ) { }

    virtual char const* find_title( PJL::mmap_file const& ) const;
    virtual void        index_words(
                            encoded_char_range const&,
                            int meta_id = Meta_ID_None
                        );
private:
    virtual bool    claims_option( PJL::option_stream::option const& );
    void            parse_html_tag( encoded_char_range::const_iterator& );
    virtual PJL::option_stream::spec const* option_spec() const;
    virtual void    post_options();
    virtual void    usage( std::ostream& ) const;
};

#endif  /* mod_html_H */

#endif  /* mod_html */
/* vim:set et sw=4 ts=4: */
