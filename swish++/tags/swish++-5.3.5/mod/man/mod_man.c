/*
**	SWISH++
**	mod/man/mod_man.c
**
**	Copyright (C) 2001  Paul J. Lucas
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

#ifdef	mod_man

// standard
#include <cctype>
#include <cstring>

// local
#include "AssociateMeta.h"
#include "config.h"
#include "encoded_char.h"
#include "mod_man.h"
#include "platform.h"
#include "TitleLines.h"
#include "util.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

bool	is_man_comment( char const *&pos, char const *end );
bool	macro_cmp( char const *&pos, char const *end, char const *macro );
void	parse_backslash( char const *&pos, char const *end );

//*****************************************************************************
//
// SYNOPSIS
//
	char const* man_indexer::find_title( mmap_file const &file ) const
//
// DESCRIPTION
//
//	Scan through the first num_title_lines lines in a Unix manual page file
//	looking for the ".SH NAME" macro to extract the title.
//
// PARAMETERS
//
//	file	The file presumed to be a Unix manual page file.
//
// RETURN VALUE
//
//	Returns the title string or null if no title can be found.
//
//*****************************************************************************
{
	int	lines = 0;
	bool	newline = false;

	mmap_file::const_iterator c = file.begin();
	while ( c != file.end() ) {
		if ( newline && ++lines > num_title_lines ) {
			//
			// Didn't find ".SH NAME" within first num_title_lines
			// lines of file: forget it.
			//
			return 0;
		}

		//
		// Find the start of a macro, i.e., a line that begins with a
		// '.' (dot).
		//
		if ( !newline || *c != '.' ) {		// not macro: forget it
			newline = *c++ == '\n';
			continue;
		}

		//
		// Found a macro: is it a comment?  If so, skip it.
		//
		if ( is_man_comment( ++c, file.end() ) ) {
			newline = true;
			continue;
		}

		//
		// Is the macro ".SH NAME"?
		//
		if ( !macro_cmp( c, file.end(), "SH" ) )
			continue;
		while ( c != file.end() && is_space( *c ) )
			++c;
		if ( !macro_cmp( c, file.end(), "NAME" ) )
			continue;

		//
		// Found ".SH NAME": skip the newline to get to the beginning
		// of the title on the next line.  The end of the title is the
		// end of that next line.
		//
		c = skip_newline( find_newline( c, file.end() ), file.end() );
		char *const title = tidy_title(
			c, find_newline( c, file.end() )
		);

		//
		// Go through the title and process backslashed character
		// sequences in case there are things like \fBword\fP in it so
		// they can be stripped out.
		//
		register char *d = title;
		for ( c = title; *c; ++c ) {
			if ( *c == '\\' )
				parse_backslash( ++c, file.end() );
			*d++ = *c;
		}
		*d = '\0';

		return title;
	}

	//
	// The file has less than num_title_lines lines and no .SH NAME was
	// found.
	//
	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void man_indexer::index_words(
		encoded_char_range const &e, int meta_id
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	a Unix manual page.
//
// PARAMETERS
//
//	e		The encoded text to index.  Note that since manual
//			pages are never (?) encoded, we can use the faster
//			underlying ordinary character pointers.
//
//	meta_id		The numeric ID of the meta name the words index are to
//			to be associated with.
//
//*****************************************************************************
{
	char	word[ Word_Hard_Max_Size + 1 ];
	bool	in_word = false;
	int	len;
	bool	newline = false;

	char const* c = e.begin_pos();
	while ( 1 ) {
		register char const ch = iso8859_to_ascii( *c );

		////////// Collect a word /////////////////////////////////////

		if ( is_word_char( ch ) ) {
			if ( !in_word ) {
				// start a new word
				word[ 0 ] = ch;
				len = 1;
				in_word = true;
				goto next_c;
			}
			if ( len < Word_Hard_Max_Size ) {
				// continue same word
				word[ len++ ] = ch;
				goto next_c;
			}
			in_word = false;	// too big: skip chars
			while ( ++c != e.end_pos() && is_word_char( *c ) ) ;
			goto next_c;
		}

		if ( ch == '\\' ) {
			//
			// Got a backslash sequence: skip over it and treat it
			// as though it weren't even there.
			//
			parse_backslash( c, e.end_pos() );
			goto next_c;
		}

		if ( in_word ) {
			//
			// We ran into a non-word character, so index the word
			// up to, but not including, it.
			//
			in_word = false;
			index_word( word, len, meta_id );
		}

		if ( newline && ch == '.' && meta_id == No_Meta_ID ) {
			//
			// If we're at the first character on a line and the
			// character is a '.' (the start of a macro), parse it.
			//
			parse_man_macro( c, e.end_pos() );
		}
next_c:
		if ( c == e.end_pos() )
			break;
		newline = *c++ == '\n';
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
	bool is_man_comment( char const *&c, char const *end )
//
// DESCRIPTION
//
//	Checks to see if the current macro is the start of a comment. If so,
//	skip it by scanning for the newline.
//
// PARAMETERS
//
//	c	The iterator to use.  It is presumed to be positioned at the
//		first character after the '.'.  If the macro is the start of a
//		comment, it is repositioned at the first character past the
//		macro, i.e., past the newline; otherwise, it is not touched.
//
//	end	The iterator marking the end of the file.
//
// RETURN VALUE
//
//	Returns true only if the current macro is the beginning of a comment.
//
//*****************************************************************************
{
	if ( macro_cmp( c, end, "\\\"" ) ) {
		c = skip_newline( find_newline( c, end ), end );
		return true;
	}
	return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool macro_cmp(
		char const *&c, char const *end, register char const *macro
	)
//
// DESCRIPTION
//
//	Compares the macro name starting at the given iterator to the given
//	string.
//
// PARAMETERS
//
//	c	The iterator to use.  It is presumed to be positioned at the
//		first character after the '.'.  If the macro name matches, it
//		is repositioned at the first character past the name;
//		otherwise, it is not touched.
//
//	end	The iterator marking the end of the file.
//
//	macro	The string to compare against; it must be in lower case.
//
// RETURN VALUE
//
//	Returns true only if the macro matches.
//
//*****************************************************************************
{
	register char const *d = c;
	while ( *macro && d != end && *macro == *d )
		++macro, ++d;
	if ( *macro )			// didn't match before null
		return false;
	c = d;
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void parse_backslash( register char const *&c, char const *end )
//
// DESCRIPTION
//
//	Parse a backslash followed by "something" and treat it as if it weren't
//	even there.
//
// PARAMETERS
//
//	c	The iterator to use.  It must be positioned at the character
//		after the '\'; it is repositioned at the first character after
//		the baclskash sequence.
//
//	end	The iterator marking the end of the file.
//
// SEE ALSO
//
//	Joseph F. Ossanna & Brian W. Kernighan.  "Troff User's Manual,"
//	Computing Science Technical Report No. 54, AT&T Bell Laboratories,
//	Murry Hill, NJ, November 1992.
//
//*****************************************************************************
{
	if ( c == end )
		return;
	char const ch = *c++;
	if ( c == end )
		return;

	switch ( ch ) {

		case '*':		// \fx or \f(xx
		case 'f':
		case 'g':
		case 'n':
			if ( *c++ != '(' || c == end )
				break;
			// no break;

		case '(':		// \(xx
			if ( ++c != end )
				++c;
			break;

		case 'b':		// \b'xxxxx'
		case 'C':
		case 'D':
		case 'h': case 'H':
		case 'l':
		case 'L':
		case 'N':
		case 'o':
		case 'S':
		case 'v':
		case 'w':
		case 'x': case 'X':
			if ( *c != '\'' )
				break;
			++c;
			while ( c != end && *c++ != '\'' ) ;
			break;

		case 's':		// \sN, \s+N, \s-N
			if ( (*c == '+' || *c == '-') && ++c == end )
				break;
			// no break;

		case '\n':		// escaped newline ...
			++c;		// ... make it disappear
			break;

		case '-':		// \-
			--c;		// transform an en-dash to just a '-'
			break;

		default:
			/* do nothing except skip over the '\' */ ;
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void man_indexer::parse_man_macro(
		register char const *&c, char const *end
	)
//
// DESCRIPTION
//
//	Parse a macro.  If it is "SH" (section heading), parse the name of the
//	heading and make that a meta name.  Then scan for the next .SH to mark
//	the end of the range and index all the words in between as being
//	associated with the section heading meta name.
//
// PARAMETERS
//
//	c	The iterator to use.  It must be positioned at the character
//		after the '.'; it is repositioned.
//
//	end	The iterator marking the end of the file.
//
//*****************************************************************************
{
	if ( c == end || !macro_cmp( ++c, end, "SH" ) )
		return;
	char const *const nl = find_newline( c, end );
	if ( !nl )
		return;
	while ( c != nl && !is_word_char( *c ) ) {
		//
		// Skip non-word characters after the 'H' in "SH" and before
		// the first word of the section heading.
		//
		++c;
	}
	char const *const begin = c;

	////////// Parse the name of the section heading. /////////////////////

	char	word[ Word_Hard_Max_Size + 1 ];
	int	len = 0;

	while ( c != nl ) {
		register char ch = *c++;
		if ( ch == ' ' )
			ch = '-';
		if ( !is_word_char( ch ) )
			break;
		if ( len < Word_Hard_Max_Size ) {
			word[ len++ ] = tolower( ch );
			continue;
		}
		while ( c != nl && is_word_char( *c++ ) ) ;	// too big
		return;
	}
	while ( len > 0 && word[ len - 1 ] == '-' )
		--len;
	if ( len < Word_Min_Size )
		return;
	word[ len ] = '\0';

	////////// Find the next .SH macro. ///////////////////////////////////

	bool newline = false;
	while ( c != end ) {
		if ( !newline || *c != '.' ) {	// not macro: forget it
			newline = *c++ == '\n';
			continue;
		}
		newline = *c++ == '\n';

		//
		// Found a macro: is it a comment?  If so, skip it.
		//
		if ( is_man_comment( c, end ) ) {
			newline = true;
			continue;
		}

		//
		// Is the macro ".SH"?  If so, back up the iterator before the
		// '.' so the indexing will stop at the right place.
		//
		if ( macro_cmp( c, end, "SH" ) ) {
			c -= 4;			// 3 for ".SH" + 1 before that
			break;
		}
	}

	////////// Index what's in between. ///////////////////////////////////

	int meta_id;
	if ( associate_meta ) {
		//
		// Potentially index the words in the section where they are
		// associated with the name of the section as a meta name.
		//
		if ( (meta_id = find_meta( word )) == No_Meta_ID )
			return;
	} else
		meta_id = No_Meta_ID;
	//
	// Index the words in between the two .SH macros marking them as being
	// associated with the value of the current section heading name.
	//
	index_words( encoded_char_range( begin, c ), meta_id );
}

#endif	/* mod_man */
