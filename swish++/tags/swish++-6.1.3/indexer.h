/*
**      SWISH++
**      indexer.h
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

#ifndef indexer_H
#define indexer_H

// standard
#include <iostream>
#include <map>
#include <string>

// local
#include "encoded_char.h"
#include "mmap_file.h"
#include "option_stream.h"
#include "util.h"
#include "word_util.h"

enum {
    Meta_ID_None        = -1,
    Meta_ID_Not_Found   = -2
};

//*****************************************************************************
//
// SYNOPSIS
//
        class indexer
//
// DESCRIPTION
//
//      An indexer the base class for all other indexers.  By itself, it can
//      only index plain text files.
//
//      The model used is that singleton instances of indexers are created once
//      at program initialization time and NOT that indexers are created and
//      destroyed for every file indexed.
//
//*****************************************************************************
{
public:
    virtual ~indexer();

    static bool     any_mod_claims_option( PJL::option_stream::option const& );
    //              See if any indexing module claims a given option.

    static PJL::option_stream::spec*
                    all_mods_options( PJL::option_stream::spec const* );
    //              Returns a combined option specification of the main
    //              indexing options plus any additional ones of indexing
    //              modules.

    static void     all_mods_post_options();
    //              Give all indexer modules a chance to do things just after
    //              command-line options have been processed.

    static void     all_mods_usage( std::ostream& );
    //              Print additional usage messages, if any, for all indexing
    //              modules.

    static indexer* find_indexer( char const *mod_name );
    //              Given a module name (case is irrelevant), return its
    //              indexer.

    static int      find_meta( char const *meta_name );
    //              Look up a meta name to get its associated ID; if it doesn't
    //              exist, add it.  However, if the name is either among the
    //              set of meta names to exclude or not among the set to
    //              include, forget it.

    virtual
    char const*     find_title( PJL::mmap_file const& ) const;
    //              By default, a file has no title, so the file's base name
    //              becomes its title.  If a particular file type can have
    //              something better for a title, the derived indexer class
    //              should override this function.

    void            index_file( PJL::mmap_file const& );
    //              This is the main entry point: this is called to index the
    //              given file.

    static void     index_word( char*, int len, int = Meta_ID_None );
    //              Once a word has been parsed, this is the function to be
    //              called from within index_words() to index it, potentially.
    //              This is not virtual intentionally for performance.

    virtual void    index_words(
                        encoded_char_range const&, int meta_id = Meta_ID_None
                    );
    //              Index words in a file between [begin,end) and associate
    //              them with the given meta ID.  The default indexes a run of
    //              plain text.  A derived indexer will override this.

    static indexer* text_indexer();
    //              Return the plain text indexer.
protected:
    indexer( char const *mod_name );

    static void     suspend_indexing();
    static void     resume_indexing();
    //              These control whether index_word() above will actually
    //              index words.  This is useful not to indexed selected
    //              portions of files while still going through the motions of
    //              collecting word statistics.  Suspend/resume calls may nest.

    virtual bool    claims_option( PJL::option_stream::option const& );
    //              See if an indexing module claims an option.  The default
    //              doesn't.  A derived indexer that does should override this
    //              function.

    virtual PJL::option_stream::spec const*
                    option_spec() const;
    //              Return a module-specific option specification.  The default
    //              returns none.  A derived indexer that has its own
    //              command-line options should override this function.

    virtual void    post_options();
    //              See if an indexing module needs to do anything just after
    //              command-line options have been processed.  The default does
    //              nothing.  A derived indexer that needs to should override
    //              this function.

    static char*    tidy_title( char const *begin, char const *end );
    //              "Tidy up" a title string by trimming leading and trailing
    //              whitespace as well as converting all non-space whitespace
    //              characters to spaces.  A derived indexer that overrides
    //              find_title() should call this on the result to tidy it up.

    virtual void    usage( std::ostream& ) const;
    //              Print a module-specific usage message.  The default prints
    //              nothing.  A derived indexer that has its own command-line
    //              options should override this function.
private:
    //
    // This needs to be a string rather than a char* because the module names
    // have to be converted to lower case strings (since they have to be stored
    // separately from the original module names, i.e., the original char*'s
    // can't be used), strings might as well be used.
    //
    typedef std::map< std::string, indexer* > map_type;

    indexer( indexer const& );                  // forbid initialization
    indexer& operator=( indexer const& );       // forbid assignment

    static int          suspend_indexing_count_;
    static indexer*     text_indexer_;

    static void         init_modules();         // generated by init_modules-sh
    static map_type&    map_ref();
};

////////// Inline functions ///////////////////////////////////////////////////

inline indexer* indexer::find_indexer( char const *mod_name ) {
    return map_ref()[ to_lower( mod_name ) ];
}

inline void indexer::index_file( PJL::mmap_file const &file ) {
    suspend_indexing_count_ = 0;
    encoded_char_range const e( file.begin(), file.end() );
    index_words( e );
}

inline indexer* indexer::text_indexer() {
    return text_indexer_;
}

inline void indexer::suspend_indexing() {
    ++suspend_indexing_count_;
}

inline void indexer::resume_indexing () {
    if ( suspend_indexing_count_ )
        --suspend_indexing_count_;
}

#endif  /* indexer_H */
/* vim:set et sw=4 ts=4: */
