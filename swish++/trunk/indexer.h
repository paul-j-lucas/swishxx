/*
**	SWISH++
**	indexer.h
**
**	Copyright (C) 2000  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef indexer_H
#define indexer_H

// standard
#include <map>

// local
#include "encoded_char.h"
#include "mmap_file.h"
#include "util.h"
#include "word_util.h"

enum {
	No_Meta_ID		= -1,
	Meta_ID_Not_Found	= -2
};

//*****************************************************************************
//
// SYNOPSIS
//
	class indexer
//
// DESCRIPTION
//
//	An indexer the base class for all other indexers.  By itself, it can
//	only index plain text files.
//
//	The model used is that singleton instances of indexers are created once
//	at program initialization time and NOT that indexers are created and
//	destroyed for every file indexed.
//
//*****************************************************************************
{
public:
	static indexer*		find_indexer( char const *mod_name );
	//			Given a module name (case is irrelevant),
	//			return its indexer.

	virtual char const*	find_title( PJL::mmap_file const& ) const;
	//			By default, a file has no title, so the file's
	//			base name becomes its title.  If a particular
	//			file type can have something better for a
	//			title, the derived indexer class should
	//			override this function.

	void			index_file( PJL::mmap_file const& );
	//			This is the main entry point: this is called to
	//			index the given file.

	virtual void		index_words(
					encoded_char_range const&,
					int meta_id = No_Meta_ID
				);
	//			Index words in a file between [begin,end) and
	//			associate them with the given meta ID.  The
	//			default indexes a run of plain text.  A derived
	//			indexer will override this.
protected:
	indexer( char const *mod_name );

	virtual void		new_file();
	//			This function is called when a new file is
	//			about to be indexed.  The default does nothing.
	//			A derived indexer class that needs to do some
	//			initialization should override this function.

	static void		index_word( char*, int len, int = No_Meta_ID );
	//			Once a word has been parsed, this is the
	//			function to be called from within index_words()
	//			to index it, potentially.  This is not virtual
	//			intentionally for performance.

	static int		find_meta( char const *meta_name );
	//			Look up a meta name to get its associated ID;
	//			if it doesn't exist, add it.  However, if the
	//			name is either among the set of meta names to
	//			exclude or not among the set to include, forget
	//			it.

	static void		suspend_indexing();
	static void		resume_indexing();
	//			These control whether index_word() above will
	//			actually index words.  This is useful not to
	//			indexed selected portions of files while still
	//			going through the motions of collecting word
	//			statistics.  Suspend/resume calls may nest.

	static char const*	tidy_title( char const *begin, char const *end);
private:
	typedef std::map< std::string, indexer* > map_type;

	indexer( indexer const& );		// forbid initialization
	indexer& operator=( indexer const& );	// forbid assignment

	static int		suspend_indexing_count_;

	static void		init_modules();	// defined in init_modules.c
	static map_type&	map_ref();
};

////////// Inline functions ///////////////////////////////////////////////////

inline indexer* indexer::find_indexer( char const *mod_name ) {
	return map_ref()[ to_lower( mod_name ) ];
}

inline void indexer::index_file( PJL::mmap_file const &file ) {
	suspend_indexing_count_ = 0;
	new_file();
	encoded_char_range const e( file.begin(), file.end() );
	index_words( e );
}

inline void indexer::suspend_indexing() { ++suspend_indexing_count_; }
inline void indexer::resume_indexing () { --suspend_indexing_count_; }

#endif	/* indexer_H */
