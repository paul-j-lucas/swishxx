/*
**	SWISH++
**	mod/latex/mod_latex.c
**
**	Copyright (C) 2002  Paul J. Lucas
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

#ifdef	MOD_latex

// local
#include "commands.h"
#include "config.h"
#include "encoded_char.h"
#include "latex_config.h"
#include "mod_latex.h"
#include "platform.h"
#include "TitleLines.h"
#include "util.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	bool command_cmp(
		encoded_char_range::const_iterator &c,
		register char const *command
	)
//
// DESCRIPTION
//
//	Compares the command name starting at the given iterator to the given
//	string.  Case is irrelevant.
//
// PARAMETERS
//
//	c		The iterator to use.  It is presumed to be positioned
//			at the first character after the '\'.  If the command
//			name matches, it is repositioned at the first character
//			past the name; otherwise, it is not touched.
//
//	command		The string to compare against; it must be in lower case.
//
// RETURN VALUE
//
//	Returns true only if the comand matches.
//
//*****************************************************************************
{
	encoded_char_range::const_iterator d = c;
	while ( *command && !d.at_end() && *command == *d )
		++command, ++d;
	if ( *command )
		return false;
	c = d;
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool find_match( encoded_char_range::const_iterator &c, char left )
//
// DESCRIPTION
//
//	Given a "left" character of a pair, find its matching "right" character
//	taking care to "balance" intervening pairs, e.g., given '{', find its
//	matching '}'.
//
// PARAMETERS
//
//	c	The iterator to use.  It is presumed to be positioned at any
//		character past the "left" character.  It is repositioned after
//		the matching character only if it was found.
//
//	left	The left-hand character to find the match for.  It must be
//		either '{' or '['.
//
// RETURN VALUE
//
//	Returns true only if the match was found.
//
//*****************************************************************************
{
	char const right = left == '{' ? '}' : ']';
	int nesting = 0;

	for ( encoded_char_range::const_iterator d = c; !d.at_end(); ++d )
		if ( *d == left )
			++nesting;
		else if ( *d == right && --nesting <= 0 ) {
			c = ++d;
			return true;
		}
	return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */
	char const* LaTeX_indexer::find_title( mmap_file const &file ) const
//
// DESCRIPTION
//
//	Scan through the first num_title_lines lines in a LaTeX file looking
//	for \title{...} to extract the title.
//
// PARAMETERS
//
//	file	The file presumed to be a LaTeX file.
//
// RETURN VALUE
//
//	Returns the title string or null if no title can be found.
//
//*****************************************************************************
{
	int lines = 0;

	encoded_char_range::const_iterator c( file.begin(), file.end() );
	while ( !c.at_end() ) {
		if ( *c == '\n' && ++lines > num_title_lines ) {
			//
			// Didn't find \title{...} within first num_title_lines
			// lines of file: forget it.
			//
			return 0;
		}

		//
		// Did we find \title{ yet?
		//
		if ( *c++ != '\\' || !command_cmp( c, "title" ) )
			continue;
		if ( c.at_end() )
			return 0;
		if ( *c != '{' )
			continue;
		if ( (++c).at_end() )
			return 0;

		//
		// Found the \title{ command: mark the postition after it and
		// look for the closing }.
		//
		mmap_file::const_iterator const start = c.pos();
		while ( !(++c).at_end() )
			if ( *c == '}' )
				return tidy_title( start, c.pos() );
	}

	//
	// The file has less than num_title_lines lines and no \title{ was
	// found.
	//
	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void LaTeX_indexer::index_words(
		encoded_char_range const &e, int meta_id
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	LaTeX
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
	char		word[ Word_Hard_Max_Size + 1 ];
	bool		in_word = false;
	int		len;
	char const*	substitution = 0;

	encoded_char_range::const_iterator c = e.begin();
	while ( !c.at_end() ) {
		register char ch;

		if ( substitution ) {
			if ( *substitution ) {
				//
				// parse_latex_command() previously returned
				// text to be substituted and indexed: take the
				// next character from said text.
				//
				ch = *substitution++;
				goto collect;
			}
			substitution = 0;	// reached end of substitution
		}
		ch = *c++;

		////////// Collect a word /////////////////////////////////////

collect:	if ( is_word_char( ch ) ) {
			if ( !in_word ) {
				// start a new word
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

		////////// See if it's a special character ////////////////////

		switch ( ch ) {

			case '%': {
				//
				// Skip a comment that starts at a '%' that
				// includes all the remaining characters on the
				// line and any leading whitespace on the next
				// line.
				//
				bool newline = false;
				while ( !(++c).at_end() )
					if ( *c == '\n' )
						newline = true;
					else if ( newline && !is_space( *c ) )
						break;
				continue;
			}

			case '~':
				ch = ' ';
				break;

			case '\\': {
				//
				// Parse a LaTeX command: it may return text to
				// be substituted for the command and indexed.
				//
				substitution = parse_latex_command( c );
				continue;
			}
		}

		////////// Index a word, maybe ////////////////////////////////

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
	char const* LaTeX_indexer::parse_latex_command(
		register encoded_char_range::const_iterator &c
	)
//
// DESCRIPTION
//
//	This function does everything skip_latex_command() does but
//	additionally does extra processing for LaTeX commands including text
//	substitution.
//
// PARAMETERS
//
//	c	The iterator to use.  It must be positioned at the character
//		after the '\'; it is repositioned at the first character after
//		the '>'.
//
//*****************************************************************************
{
	if ( c.at_end() )
		return 0;

	////////// Deal with elements of a class not to index /////////////////

	char command_buf[ Command_Name_Max_Size + 1 ];	// +1 for null
	{ // local scope
	//
	// Copy only the tag name by stopping at a whitespace character (or
	// running into the end of the tag).
	//
	register char *to = command_buf;
	encoded_char_range::const_iterator from = c;
	while ( !from.at_end() && !is_alnum( *from ) ) {
		//
		// Check to see if the name is too long to be a valid one for a
		// LaTeX command: if it is, invalidate it by writing "garbage"
		// into the name.
		//
		if ( to - command_buf >= Command_Name_Max_Size ) {
			to = command_buf;
			*to++ = '\1';
			break;
		}
		*to++ = *from++;
	}
	*to = '\0';
	} // local scope

	////////// Look-up command ////////////////////////////////////////////

	static command_map const &commands = command_map::instance();
	command_map::const_iterator const cmd = commands.find( command_buf );
	if ( cmd == commands.end() )
		return 0;
	//
	// We found the command in our internal table: now do different stuff
	// depending upon the action.
	//
	switch ( *cmd->second.action ) {
		case '{':
		case '[': {
			//
			// Find the matching '}' or ']' and index the words in
			// between.
			//
			encoded_char_range::const_iterator end = c;
			if ( find_match( end, *cmd->second.action ) ) {
				index_words( encoded_char_range( c, end ) );
				c = end;
			}
			return 0;
		}
		default: {
			//
			// Substitute the text of the command's "action" as the
			// text to be indexed.
			//
			return cmd->second.action;
		}
	}
}

#endif	/* MOD_latex */
