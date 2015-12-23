/*
**      SWISH++
**      src/xml_formatter.cpp
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
#include "config.h"
#include "file_info.h"
#include "index_segment.h"
#include "util.h"
#include "xml_formatter.h"

// standard
#include <cstring>                      /* for strpbrk(3) */
#include <string>

using namespace std;

#define SWISH_PATH              "/pauljlucas/software/swish"
#define SWISH_NS_URI            "http://www.pauljlucas.org" SWISH_PATH
#define SWISH_PHYS_URI          "http://homepage.mac.com" SWISH_PATH
#define XSI_URI                 "http://www.w3.org/2001/XMLSchema-instance"

#define SEARCH_RESULTS          "SearchResults"
#define SEARCH_RESULTS_DTD      SEARCH_RESULTS ".dtd"
#define SEARCH_RESULTS_NS_URI   SWISH_NS_URI   "/" SEARCH_RESULTS
#define SEARCH_RESULTS_PHYS_URI SWISH_PHYS_URI "/" SEARCH_RESULTS
#define SEARCH_RESULTS_XSD      SEARCH_RESULTS ".xsd"

extern index_segment directories;

//*****************************************************************************
//
// SYNOPSIS
//
        static string escape( char const *s )
//
// DESCRIPTION
//
//      Escape all '&' and '<' characters in a given string by replacing them
//      with "&amp;" or "&lt;", respectively.
//
// PARAMETERS
//
//      s   The string to be escaped.
//
// RETURN VALUE
//
//      Returns a new string.
//
// SEE ALSO
//
//      Tim Bray, et al.  "Character Data and Markup," Extensible Markup
//      Language (XML) 1.0, section 2.4, February 10, 1998.
//
//*****************************************************************************
{
    string result = s;
    string::size_type i;

    for ( i = 0; (i = result.find( '&', i )) != string::npos; i += 5 )
        result.replace( i, 1, "&amp;" );
    for ( i = 0; (i = result.find( '<', i )) != string::npos; i += 4 )
        result.replace( i, 1, "&lt;" );

    return result;
}

//*****************************************************************************
//
// SYNOPSIS
//
        xml_formatter::~xml_formatter()
//
// DESCRIPTION
//
//      Destroy an xml_formatter.
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
        void xml_formatter::pre( stop_word_set const &stop_words ) const
//
// DESCRIPTION
//
//      Output search-result "meta" information before the results themselves:
//      the set of stop words found in the query (if any) and the number of
//      results.
//
// PARAMETERS
//
//      stop_words  The set of stop words.
//
//*****************************************************************************
{
    out_ << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>\n"
        "<!DOCTYPE SearchResults SYSTEM\n"
        " \"" SWISH_PHYS_URI "/" SEARCH_RESULTS_DTD "\">\n"
        "<SearchResults\n"
        " xmlns=\"" SEARCH_RESULTS_NS_URI "\"\n"
        " xmlns:xsi=\"" XSI_URI "\"\n"
        " xsi:schemaLocation=\"" SEARCH_RESULTS_PHYS_URI
        " " SEARCH_RESULTS_XSD "\">\n";

    if ( !stop_words.empty() ) {
        out_ << "  <IgnoredList>\n";
        FOR_EACH( stop_word_set, stop_words, word )
            out_ << "    <Ignored>" << *word << "</Ignored>\n";
        out_ << "  </IgnoredList>\n";
    }

    out_ << "  <ResultCount>" << results_ << "</ResultCount>\n";
    if ( results_ )
        out_ << "  <ResultList>\n";
}

//*****************************************************************************
//
// SYNOPSIS
//
        void xml_formatter::result( int rank, file_info const &fi ) const
//
// DESCRIPTION
//
//      Output an individual search result's information: it's rank, path,
//      size, and title.
//
// PARAMETERS
//
//      rank    The rank (1-100) of the result.
//
//      fi      The search result's file information.
//
//*****************************************************************************
{
    out_ << "    <File>\n"
        "      <Rank>" << rank << "</Rank>\n"
        "      <Path>"
                << directories[ fi.dir_index() ] << '/' << fi.file_name()
        <<    "</Path>\n"
        "      <Size>" << fi.size() << "</Size>\n"
        "      <Title>";
    if ( ::strpbrk( fi.title(), "&<" ) )
        out_ << escape( fi.title() );
    else
        out_ << fi.title();
    out_ <<         "</Title>\n"
        "    </File>\n";
}

//*****************************************************************************
//
// SYNOPSIS
//
        void xml_formatter::post() const
//
// DESCRIPTION
//
//      Output end tags of XML elements.
//
//*****************************************************************************
{
    if ( results_ )
        out_ << "  </ResultList>\n";
    out_ << "</SearchResults>\n";
}
/* vim:set et sw=4 ts=4: */
