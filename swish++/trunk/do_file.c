/*
**	SWISH++
**	do_file.c
**
**	Copyright (C) 1998  Paul J. Lucas
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

/*
**	Note that this file is #include'd into index.c and extract.c because
**	it generates different code depending on which one it's compiled into.
*/

// standard
#include <vector>

// local
#include "encoded_char.h"

//*****************************************************************************
//
// SYNOPSIS
//
#ifdef	INDEX
	void do_file( char const *file_name, int dir_index )
#else
	void do_file( char const *file_name )
#endif
//
// DESCRIPTION
//
//	Either index or extract text from the given file, but only if its
//	extension is among (not among) the specified set.  It will not follow
//	symbolic links unless either the FollowLinks config. file variable or
//	the -l command-line option was given.
//
//	For extraction, the algorithm is loosely based on what the Unix
//	strings(1) command does except it goes a bit further to discard things
//	like Encapsulated PostScript and raw hex data.
//
// PARAMETERS
//
//	file_name	The file to process.
//
//*****************************************************************************
{
	char const *const orig_base_name = pjl_basename( file_name );

	++num_examined_files;
	if ( verbosity > 3 )			// print base name of file
		cout << "  " << orig_base_name << flush;

	////////// Simple checks to see if we should process the file /////////

	if ( !is_plain_file() ) {
		//
		// We're able to use the zero-argument form of is_plain_file()
		// because the stat_buf is cached by the call to file_exists()
		// in both index.c and extract.c just before the call to
		// do_file().
		//
		if ( verbosity > 3 )
			cout << " (skipped: not plain file)\n";
		return;
	}

#ifdef	INDEX
	//
	// Record the size of the original (non-filtered) file here before we
	// call is_symbolic_link() below.  This is the size that is stored in
	// the index.
	//
	off_t const orig_file_size = file_size();
#endif	/* INDEX */

#ifndef	PJL_NO_SYMBOLIC_LINKS
	if ( is_symbolic_link( file_name ) && !follow_symbolic_links ) {
		//
		// Despite the above comment for is_plain_file(), we have to
		// use the one-argument form is is_symbolic_link() because we
		// need to call lstat(2) rather than stat(2).
		//
		if ( verbosity > 3 )
			cout << " (skipped: symbolic link)\n";
		return;
	}
#endif	/* PJL_NO_SYMBOLIC_LINKS */

#ifdef	INDEX
	//
	// If incrementally indexing, it's possible that we've encountered the
	// file before.
	//
	if ( incremental && file_info::seen_file( file_name ) ) {
		if ( verbosity > 3 )
			cout << " (skipped: encountered before)\n";
		return;
	}
#endif	/* INDEX */

	////////// Perform filter name substitution(s) ////////////////////////

	typedef vector< filter > filter_list_type;
	filter_list_type filter_list;
#ifdef	INDEX
	char const *const orig_file_name = file_name;
#endif

	while ( true ) {
		//
		// Determine if the file needs to be filtered and, if so, set
		// the filename to what it would become if it were filtered.
		//
		FilterFile::const_pointer const f = file_filters[ file_name ];
		if ( !f )
			break;
		filter_list.push_back( *f );
		file_name = filter_list.back().substitute( file_name );
	}
	char const *const base_name = pjl_basename( file_name );

	//
	// Skip the file if it matches one of the set of unacceptable patterns.
	//
	if ( exclude_patterns.matches( base_name ) ) {
		if ( verbosity > 3 )
			cout << " (skipped: file excluded)\n";
		return;
	}

	//
	// See if the filename pattern is included.
	//
#ifdef	INDEX
	IncludeFile::const_iterator const
#else
	ExtractFile::const_iterator const
#endif
		include_pattern = include_patterns.find( base_name );
	//
	// Skip the file if the set of acceptable patterns doesn't contain the
	// candidate, but only if there was at least one acceptable pattern
	// specified.
	//
	bool const found_pattern = include_pattern != include_patterns.end();
	if ( !include_patterns.empty() && !found_pattern ) {
		if ( verbosity > 3 )
			cout << " (skipped: file not included)\n";
		return;
	}

#ifdef	EXTRACT
	ostream *out;
	ofstream extracted_file;
	if ( extract_as_filter ) {
		//
		// We're running as a filter: write to standard output.
		//
		out = &cout;
	} else {
		//
		// We're not running as a filter: check to see if the extracted
		// file already exists; if so, skip extraction entirely.
		//
		char extracted_file_name[ PATH_MAX + 1 ];
		::strcpy( extracted_file_name, file_name );
		::strcat( extracted_file_name, extract_extension );
		if ( file_exists( extracted_file_name ) ) {
			if ( verbosity > 3 )
				cout	<< " (skipped: " << extract_extension
					<< " file already exists)\n";
			return;
		}

		extracted_file.open( extracted_file_name );
		if ( !extracted_file ) {
			if ( verbosity > 3 )
				cout	<< " (skipped: can not create "
					<< extract_extension << " file)\n";
			return;
		}
		out = &extracted_file;
	}
#endif	/* EXTRACT */

	//
	// Execute the filter(s) on the file.
	//
	FOR_EACH( filter_list_type, filter_list, f )
		if ( !( file_name = f->exec() ) ) {
			if ( verbosity > 3 )
				cout << " (skipped: could not filter)\n";
			return;
		}

	//
	// We can (finally!) open the (possibly post-filtered) file.
	//
	mmap_file const file( file_name );
	file.behavior( mmap_file::sequential );
	if ( !file ) {
		if ( verbosity > 3 )
			cout << " (skipped: can not open)\n";
		return;
	}

	if ( verbosity == 3 )			// print base name of file
		cout << "  " << orig_base_name << flush;

#ifdef	INDEX
	if ( file.empty() ) {
		//
		// Don't waste a file_info entry on it.
		//
		if ( verbosity > 2 )
			cout << " (0 words)\n";
		return;
	}

	////////// Index the file /////////////////////////////////////////////

#ifdef	MOD_mail
	encoded_char_range::decoder::reset_all();
#endif
	static indexer *const text = indexer::find_indexer( "text" );
	indexer *const i = found_pattern ? include_pattern->second : text;
	file_info *const fi = new file_info(
		orig_file_name, dir_index, orig_file_size, i->find_title( file )
	);
	i->index_file( file );

	if ( verbosity > 2 )
		cout << " (" << fi->num_words() << " words)\n";

	if ( words.size() >= Word_Threshold )
		write_partial_index();
#endif	/* INDEX */

#ifdef	EXTRACT
	////////// Extract the file ///////////////////////////////////////////

	++num_extracted_files;
	extract_words( file.begin(), file.end(), *out );
#endif	/* EXTRACT */
}
