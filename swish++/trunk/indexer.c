/*
**	SWISH++
**	indexer.c
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

// standard
#include <cstring>

// local
#include "config.h"
#include "encoded_char.h"
#include "file_info.h"
#include "ExcludeMeta.h"
#include "IncludeMeta.h"
#include "indexer.h"
#include "meta_map.h"
#include "stop_words.h"
#include "util.h"
#include "word_info.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

extern long	num_indexed_words;
extern long	num_total_words;
extern word_map	words;

int		indexer::suspend_indexing_count_ = 0;

//*****************************************************************************
//
// SYNOPSIS
//
	indexer::indexer( char const *mod_name )
//
// DESCRIPTION
//
//	Construct an indexer module by adding its name to the map of indexers.
//
// PARAMETERS
//
//	name	The name of the indexer module.
//
//*****************************************************************************
{
	indexer *&i = map_ref()[ to_lower( mod_name ) ];
	if ( i ) {
		cerr	<< "indexer::indexer(\"" << mod_name << "\"): "
			"registered more than once" << endl;
		::abort();
	}
	i = this;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ int indexer::find_meta( char const *meta_name )
//
// DESCRIPTION
//
//	Look up a meta name to get its associated unique integer ID; if the
//	meta name didn't exist, add it, or perhaps a reassigned name of it.
//	However, if the name is either among the set of meta names to exclude
//	or not among the set to include, forget it.
//
// PARAMETERS
//
//	meta_name	The meta name to find.
//
// RETURN VALUE
//
//	Returns the ID of the given meta name or No_Meta_ID if the meta name
//	is either excluded or not included.
//
//*****************************************************************************
{
	if ( exclude_meta_names.contains( meta_name ) )
		return No_Meta_ID;

	if ( !include_meta_names.empty() ) {
		//
		// There were meta names explicitly given: see if the meta name
		// is among them.  If not, forget it; if so, possible reassign
		// the name.
		//
		IncludeMeta::const_iterator const
			m = include_meta_names.find( meta_name );
		if ( m == include_meta_names.end() )
			return No_Meta_ID;
		meta_name = m->second;
	}

	//
	// Look up the meta name to get its associated unique integer ID.
	//
	meta_map::const_iterator const i = meta_names.find( meta_name );
	if ( i != meta_names.end() )
		return i->second;
	//
	// New meta name: add it.  Do this in two statements intentionally
	// because C++ doesn't guarantee that the RHS of assignment is
	// evaluated first.
	//
	int const meta_id = meta_names.size();
	return meta_names[ ::strdup( meta_name ) ] = meta_id;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	char const* indexer::find_title( mmap_file const& ) const
//
// DESCRIPTION
//
//	Define the default find_title() function that simply returns null
//	indicating that the file has no meaningful title (like plain text
//	files don't).
//
// RETURN VALUE
//
//	Returns null.
//
//*****************************************************************************
{
	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void indexer::index_word(
		register char *word, register int len, int meta_id
	)
//
// DESCRIPTION
//
//	Potentially index the given word.
//
// PARAMETERS
//
//	word		The candidate word to be indexed.
//
//	len		The length of the word since it is not null-terminated.
//
//	meta_id		The numeric ID of the META NAME the word, if indexed,
//			is to be associated with.
//
//*****************************************************************************
{
	++num_total_words;

	if ( len < Word_Hard_Min_Size )
		return;

	if ( suspend_indexing_count_ > 0 ) {
		//
		// A derived indexer class has called suspend_indexing(), so
		// do nothing more.
		//
		// This facility is currently used by HTML_indexer to indicate
		// that the word is within an HTML or XHTML element's begin/end
		// tags whose begin tag's CLASS attribute value is among the
		// set of class names not to index, so do nothing.
		//
		return;
	}

	////////// Strip chars not in Word_Begin_Chars/Word_End_Chars /////////

	for ( register int i = len - 1; i >= 0; --i ) {
		if ( is_word_end_char( word[ i ] ) )
			break;
		--len;
	}
	if ( len < Word_Hard_Min_Size )
		return;

	word[ len ] = '\0';

	while ( *word ) {
		if ( is_word_begin_char( *word ) )
			break;
		--len, ++word;
	}
	if ( len < Word_Hard_Min_Size )
		return;

	////////// Stop-word checks ///////////////////////////////////////////

	if ( !is_ok_word( word ) )
		return;

	char const *const lower_word = to_lower( word );
	if ( stop_words->contains( lower_word ) )
		return;

	////////// Add the word ///////////////////////////////////////////////

	file_info::inc_words();
	++num_indexed_words;

	word_info &wi = words[ lower_word ];
	++wi.occurrences_;

	if ( !wi.files_.empty() ) {
		//
		// We've seen this word before: determine whether we've seen it
		// before in THIS file, and, if so, increment the number of
		// occurrences and associate with the current meta name, if
		// any.
		//
		word_info::file &last_file = wi.files_.back();
		if ( last_file.index_ == file_info::current_index() ) {
			++last_file.occurrences_;
			if ( meta_id != No_Meta_ID )
				last_file.meta_ids_.insert( meta_id );
			return;
		}
	}

	// First time word occurred in current file.
	wi.files_.push_back(
		word_info::file( file_info::current_index(), meta_id )
	);
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void indexer::index_words(
		encoded_char_range const &e, int meta_id
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	plain text.
//
// PARAMETERS
//
//	e		The encoded text to index.
//
//	meta_id		The numeric ID of the META NAME the words index are to
//			to be associated with.
//
//*****************************************************************************
{
	char		buf[ Word_Hard_Max_Size + 1 ];
	register char*	word;
	bool		in_word = false;
	int		len;

	encoded_char_range::const_iterator c = e.begin();
	while ( !c.at_end() ) {
		register mmap_file::value_type ch = iso8859_to_ascii( *c++ );

		////////// Collect a word /////////////////////////////////////

		if ( is_word_char( ch ) ) {
			if ( !in_word ) {
				// start a new word
				word = buf;
				word[ 0 ] = ch;
				len = 1;
				in_word = true;
				continue;
			}
			if ( len < Word_Hard_Max_Size ) {
				// continue same word
				word[ len++ ] = ch;
				continue;
			}
			in_word = false;	// too big: skip chars
			while ( !c.at_end() && is_word_char( *c++ ) ) ;
			continue;
		}

		if ( in_word ) {
			//
			// We ran into a non-word character, so index the word
			// up to, but not including, it.
			//
			in_word = false;
			index_word( word, len, meta_id );
		}
	}
	if ( in_word ) {
		//
		// We ran into 'end' while still accumulating characters into a
		// word, so just index what we've got.
		//
		index_word( word, len, meta_id );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ indexer::map_type& indexer::map_ref()
//
// DESCRIPTION
//
//	Define and initialize (exactly once) a static data member for indexer
//	and return a reference to it.  The reason for this function is to
//	guarantee that the map is initialized before its first use across all
//	translation units, something that would not guaranteed if it were a
//	static data member initialized at file scope.
//
//	We also initialize the map with pointers to the singleton instances of
//	all derived class indexers.
//
// RETURN VALUE
//
//	Returns a reference to a static instance of an initialized map_type.
//
// SEE ALSO
//
//	Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++
//	Reference Manual."  Addison-Wesley, Reading, MA, 1990.  p. 19.
//
//*****************************************************************************
{
	static map_type m;
	static bool init;
	if ( !init ) {
		init = true;		// must set this before init_modules()
		init_modules();		// defined in init_modules.c
		static indexer text( "text" );
	}
	return m;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void indexer::new_file()
//
// DESCRIPTION
//
//	This function is called whenever a new file is about to be indexed.
//	This function is out-of-line due to being virtual.
//
//*****************************************************************************
{
	// do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ char const* indexer::tidy_title(
		char const *begin, char const *end
	)
//
// DESCRIPTION
//
//	"Tidy up" a title string by trimming leading and trailing whitespace as
//	well as converting all non-space whitespace characters to spaces.
//
//	Additionally, if the length of the title exceeds Title_Max_Size, then
//	the title is truncated and the last 3 characters of the truncated title
//	are replaced with an elipsis ("...").
//
// PARAMETERS
//
//	begin	The pointer to the beginning of the title.
//
//	end	The pointer to the end of the title.
//
// RETURN VALUE
//
//	Returns the title.
//
//*****************************************************************************
{
	// Remove leading spaces
	while ( begin < end && isspace( *begin ) )
		++begin;

	// Remove trailing spaces
	while ( begin < --end && isspace( *end ) ) ;
	++end;

	static char title[ Title_Max_Size + 1 ];
	int len = end - begin;
	if ( len > Title_Max_Size ) {
		::strncpy( title, begin, Title_Max_Size );
		::strcpy( title + Title_Max_Size - 3, "..." );
		len = Title_Max_Size;
	} else {
		::copy( begin, end, title );
	}
	title[ len ] = '\0';

	// Normalize all whitespace chars to space chars.
	for ( register char *p = title; *p; ++p )
		if ( isspace( *p ) )
			*p = ' ';

	return title;
}
