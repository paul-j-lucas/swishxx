/*
**      SWISH++
**      src/indexer.h
**
**      Copyright (C) 2000-2015  Paul J. Lucas
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

// local
#include "encoded_char.h"
#include "pjl/mmap_file.h"
#include "pjl/option_stream.h"
#include "util.h"
#include "word_util.h"

// standard
#include <iostream>
#include <map>
#include <memory>
#include <string>

enum {
  Meta_ID_None        = -1,
  Meta_ID_Not_Found   = -2
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An %indexer the base class for all other indexers.  By itself, it can only
 * index plain text files.
 *
 * The model used is that singleton instances of indexers are created once at
 * program initialization time and NOT that indexers are created and destroyed
 * for every file indexed.
 */
class indexer {
public:
  virtual ~indexer();

  /**
   * See if any indexing module claims a given option.
   *
   * @param opt The option to claim.
   * @return Returns \c true only if claimed.
   */
  static bool any_mod_claims_option( PJL::option_stream::option const &opt );

  /**
   * Combines the option specification of the main indexing options plus any
   * additional ones of indexing modules.
   *
   * @param main_spec The option specification for the main part of index(1).
   * @return Returns said combined option specification.  The pointer should be
   * deleted via \c delete[].
   */
  static std::unique_ptr<PJL::option_stream::spec[]>
                  all_mods_options( PJL::option_stream::spec const* );

  /**
   * Give all %indexer modules a chance to do things just after command-line
   * options have been processed.
   */
  static void all_mods_post_options();

  /**
   * Prints additional usage messages, if any, for all indexing modules.
   *
   * @param o The ostream to write to.
   */
  static void all_mods_usage( std::ostream &o );

  /**
   * Gets the %indexer module having the given name.
   *
   * @param mod_name The name of the module to find.  Case is irrelevant.
   * @return Returns a pointer to the relevant %indexer or null if none.
   */
  static indexer* find_indexer( char const *mod_name );

  /**
   * Looks up a meta name to get its associated ID; if it doesn't exist, add
   * it.  However, if the name is either among the set of meta names to exclude
   * or not among the set to include, forget it.
   *
   * @param meta_name The meta-name to find.
   * @return Returns the associated meta-ID or Meta_ID_None.
   */
  static int find_meta( char const *meta_name );

  /**
   * By default, a file has no title, so the file's base name becomes its
   * title.  If a particular file type can have something better for a title,
   * the derived %indexer class should override this function.
   *
   * @param file The file being indexed.
   * @return Returns a pointer to a title or null if none.
   */
  virtual char const* find_title( PJL::mmap_file const &file ) const;

  /**
   * Indexes the fiven file.
   *
   * @param file The file to index.
   */
  void index_file( PJL::mmap_file const &file );

  /**
   * Once a word has been parsed, this is the function to be called from within
   * index_words() to index it, potentially.  This is not \c virtual
   * intentionally for performance.
   *
   * @param word The candidate word to be indexed.
   * @param len The length of the word since it is not null-terminated.
   * @param meta_id The numeric ID of the meta name the word, if indexed, is to
   * be associated with.
   */
  static void index_word( char *word, size_t len, int meta_id = Meta_ID_None );

  /**
   * Indexes words in a file between [begin,end) and associates them with the
   * given meta ID.  The default indexes a run of plain text.  A derived
   * %indexer will override this.
   *
   * @param e The encoded text to index.
   * @param meta_id The numeric ID of the meta name the words index are to be
   * associated with.
   */
  virtual void index_words( encoded_char_range const &e,
                            int meta_id = Meta_ID_None );

  /**
   * Gets the plain text %indexer.
   *
   * @return Returns said %indexer.
   */
  static indexer* text_indexer();

protected:
  indexer( char const *mod_name );

  /**
   * Suspend indexing words.  This is useful not to indexed selected portions
   * of files while still going through the motions of collecting word
   * statistics.  Suspend/resume calls may nest.
   */
  static void suspend_indexing();

  /**
   * Resume indexing words.
   */
  static void resume_indexing();

  /**
   * See if an indexing module claims an option.  The default doesn't.  A
   * derived %indexer that does should override this function.
   *
   * @param opt The option to claim.
   * @return Returns \c true only if claimed.
   */
  virtual bool claims_option( PJL::option_stream::option const &opt );

  /**
   * Gets a module-specific option specification.  A derived %indexer that has
   * its own command-line options should override this function.
   *
   * @return Returns an option specification for a module or null if none.
   */
  virtual PJL::option_stream::spec const* option_spec() const;

  /**
   * See if an indexing module needs to do anything just after command-line
   * options have been processed.  The default does nothing.  A derived
   * %indexer that needs to should override this function.
   */
  virtual void post_options();

  /**
   * "Tidy up" a title string by trimming leading and trailing whitespace as
   * well as converting all non-space whitespace characters to spaces.  A
   * derived %indexer that overrides \c find_title() should call this on the
   * result to tidy it up.
   *
   * Additionally, if the length of the title exceeds Title_Max_Size, then the
   * title is truncated and the last 3 characters of the truncated title are
   * replaced with an ellipsis ("...").
   *
   * @param begin The pointer to the beginning of the title.
   * @param end The pointer to one past the end of the title.
   * @return Returns said title.
   */
  static char* tidy_title( char const *begin, char const *end );

  /**
   * Print a module-specific usage message.  The default prints nothing.  A
   * derived %indexer that has its own command-line options should override
   * this function.
   *
   * @param o The ostream to print to.
   */
  virtual void usage( std::ostream &o ) const;

private:
  /**
   * This needs to be a string rather than a \c char* because the module names
   * have to be converted to lower case strings (since they have to be stored
   * separately from the original module names, i.e., the original \c char*
   * can't be used), strings might as well be used.
   */
  typedef std::map<std::string,indexer*> map_type;

  indexer( indexer const& ) = delete;
  indexer& operator=( indexer const& ) = delete;

  static int        suspend_indexing_count_;
  static indexer*   text_indexer_;

  static void       init_modules();     // generated by init_modules-sh
  static map_type&  map_ref();
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

///////////////////////////////////////////////////////////////////////////////

#endif /* indexer_H */
/* vim:set et sw=2 ts=2: */
