/*
**	SWISH++
**	mod_mail.c
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

#ifdef	MOD_MAIL

// standard
#include <algorithm>			/* for copy() */
#include <cctype>
#include <cstring>
#include <string>
#include <unistd.h>			/* for unlink(2) */

// local
#include "AssociateMeta.h"
#include "auto_vec.h"
#include "config.h"
#include "FilterAttachment.h"
#include "IncludeMeta.h"
#include "less.h"
#include "meta_map.h"
#ifdef	MOD_HTML
#include "mod_html.h"
#endif
#include "mod_mail.h"
#include "platform.h"
#include "TitleLines.h"
#include "util.h"
#include "Verbosity.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

mail_indexer::stack_type	mail_indexer::boundary_stack_;
bool				mail_indexer::did_last_header;

bool				header_cmp(
					char const *&pos, char const *end,
					char const *tag
				);

FilterAttachment		attachment_filters;

//*****************************************************************************
//
// SYNOPSIS
//
	bool boundary_cmp(
		register char const *c, register char const *end,
		register char const *boundary
	)
//
// DESCRIPTION
//
//	Compares the boundary, prefixed by "--", string starting at the given
//	iterator to the given string.
//
// PARAMETERS
//
//	c		The pointer presumed to be positioned at the first
//			character on a line.
//
//	end		The iterator marking the end of the file.
//
//	boundary	The boundary string to compare against.
//
// RETURN VALUE
//
//	Returns true only if the boundary string matches.
//
//*****************************************************************************
{
	if ( c == end || *c != '-' || ++c == end || *c++ != '-' )
		return false;
	while ( *boundary && c != end && *boundary++ == *c++ ) ;
	return !*boundary;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_newline( char const *c, char const *end )
//
// DESCRIPTION
//
//	Checks to see if the character (or character sequence) is a newline
//	(or "internet newline," i.e., a CR-LF pair).
//
// PARAMETERS
//
//	c	The pointer to start comparing at; it is assumed not to be at
//		"end".
//
//	end	The pointer to the end of the range to check.
//
// RETURN VALUE
//
//	Returns true only if it's a newline.
//
//*****************************************************************************
{
	return ( c[0] == '\r' && c+1 != end && c[1] == '\n' ) || *c == '\n';
}

//*****************************************************************************
//
// SYNOPSIS
//
	char const* mail_indexer::find_title( mmap_file const &file ) const
//
// DESCRIPTION
//
//	Scan through the first num_title_lines lines in a mail file looking for
//	"Subject: ..." to extract it as the title.  Every non-space whitespace
//	character in the title is converted to a space; leading and trailing
//	spaces are removed.
//
//	If the length of the title exceeds Title_Max_Size, then the title is
//	truncated and the last 3 characters of the truncated title are replaced
//	with an elipsis ("...").
//
// PARAMETERS
//
//	file	The file presumed to be a mail file.
//
// RETURN VALUE
//
//	Returns the Subject as the title string or null if Subject can be
//	found.
//
// EXAMPLE
//
//	Given:
//
//		Subject: This is a title
//
//	returns:
//
//		This is a title
//
// CAVEAT
//
//	This function won't currently return a Subject whose value is folded
//	across multiple lines.  This most likely won't occur in practice.
//
// SEE ALSO
//
//	David H. Crocker.  "RFC 822: Standard for the Format of ARPA Internet
//	Text Messages," Department of Electrical Engineering, University of
//	Delaware, August 1982.
//
//*****************************************************************************
{
	int lines = 0;

	mmap_file::const_iterator c = file.begin();
	while ( c != file.end() ) {
		if ( is_newline( c, file.end() ) || ++lines > num_title_lines ){
			//
			// We either ran out of headers or didn't find the
			// Subject header within first num_title_lines lines of
			// file: forget it.
			//
			break;
		}

		//
		// Find the newline ending the header and its value and see if
		// it's the Subject header.
		//
		char const *const nl = find_newline( c, file.end() );
		if ( nl == file.end() )
			break;
		if ( header_cmp( c, nl, "subject:" ) )
			return tidy_title( c, nl );	// found the Subject

		c = skip_newline( nl, file.end() );
	}

	//
	// The file has less than num_title_lines lines and no Subject was
	// found.
	//
	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool header_cmp(
		char const *&c, register char const *end,
		register char const *header
	)
//
// DESCRIPTION
//
//	Compares the header name starting at the given iterator to the given
//	string.  Case is irrelevant.
//
// PARAMETERS
//
//	c	The pointer presumed to be positioned at the first character
//		after the '<'.  If the tag name matches, it is repositioned at
//		the first character past the name; otherwise, it is not
//		touched.
//
//	end	The pointer to the end of the file.
//
//	header	The string to compare against; it must be in lower case.
//
// RETURN VALUE
//
//	Returns true only if the header matches.
//
//*****************************************************************************
{
	register char const *d = c;
	while ( *header && d != end && *header == to_lower( *d ) )
		++header, ++d;
	if ( *header )
		return false;
	c = d;
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void index_via_filter( filter *f, encoded_char_range const &e )
//
// DESCRIPTION
//
//	Call an external filter program to convert the encoded character range
//	into plain text that we know how to index.
//
// PARAMETERS
//
//	f	The filter to use.
//
//	e	The encoded character range to filter and index.
//
//*****************************************************************************
{
	extern string temp_file_name_prefix;
	//
	// Create a temporary file containing the decoded bytes of an
	// attachment.
	//
	string const temp_file_name = temp_file_name_prefix + "att";
	ofstream temp_file( temp_file_name.c_str(), ios::out | ios::binary );
	if ( !temp_file ) {
could_not_filter:
		if ( verbosity > 3 )
			cout << " (could not filter attachment)";
		return;
	}
	::copy( e.begin(), e.end(),
		ostream_iterator< encoded_char_range::value_type >( temp_file )
	);
	temp_file.close();

	//
	// Substitute the temporary file's name into the filter command,
	// execute the filter creating a text file, and delete the temporary
	// file containing the original attachment.
	//
	f->substitute( temp_file_name );
	char const *const new_file_name = f->exec();
	::unlink( temp_file_name.c_str() );

	if ( new_file_name ) {
		//
		// The filter worked, so now index the post-filtered file that
		// is assumed to be plain text.
		//
		static indexer *const text = indexer::find_indexer( "text" );
		mmap_file const file( new_file_name );
		if ( file && !file.empty() )
			text->index_file( file );
	} else
		goto could_not_filter;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void mail_indexer::index_enriched( encoded_char_range const &e )
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	text/enriched content type.
//
//	The code is copied-and-pasted from indexer::index_words() because,
//	despite the code redundancy (which isn't all that much), this is much
//	faster than, say, calling an overrideable virtual function for every
//	single character.
//
// PARAMETERS
//
//	e	The encoded character range to index.
//
// SEE ALSO
//
//	Tim Berners-Lee.  "RFC 1563: The text/enriched MIME Content-type,"
//	Network Working Group of the Internet Engineering Task Force, January
//	1994.
//
//*****************************************************************************
{
	char	word[ Word_Hard_Max_Size + 1 ];
	bool	in_word = false;
	int	len;

	encoded_char_range::const_iterator c = e.begin();
	while ( !c.at_end() ) {
		register char ch = iso8859_to_ascii( *c++ );

		////////// Collect a word /////////////////////////////////////

		if ( is_word_char( ch ) ) {
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

		if ( in_word ) {
			//
			// We ran into a non-word character, so index the word
			// up to, but not including, it.
			//
			in_word = false;
			index_word( word, len );
		}

		if ( ch == '<' ) {
			//
			// This is probably the start of command; if so, skip
			// everything until the terminating '>'.
			//
			if ( c.at_end() )
				break;
			if ( (ch = *c++) == '<' )
				continue;	// a literal '<': ignore it
			while ( !c.at_end() && *c++ != '>' ) ;
		}
	}
	if ( in_word ) {
		//
		// We ran into 'end' while still accumulating characters into a
		// word, so just index what we've got.
		//
		index_word( word, len );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	mail_indexer::message_type mail_indexer::index_headers(
		register char const *&c, register char const *end
	)
//
// DESCRIPTION
//
//	This function indexes the words found in the values of the headers as
//	being associated with the headers as meta names.  Additionally, it
//	determines the message type and encoding via the Content-Type and
//	Content-Transfer-Encoding headers.
//
// PARAMETERS
//
//	c	The pointer that must be positioned at the first character in
//		the file.  It is left after the last header.
//
//	end	The pointer to the end of the file.
//
// RETURN VALUE
//
//	Returns the type and encoding of the message based on the headers.
//
// SEE ALSO
//
//	David H. Crocker.  "RFC 822: Standard for the Format of ARPA Internet
//	Text Messages," Department of Electrical Engineering, University of
//	Delaware, August 1982.
//
//	Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose
//	Internet Mail Extensions (MIME) Part One: Format of Internet Message
//	Bodies," RFC 822 Extensions Working Group of the Internet Engineering
//	Task Force, November 1996.
//
//*****************************************************************************
{
	message_type type;

	key_value kv;
	while ( parse_header( c, end, &kv ) ) {

		////////// Deal with Content-Transfer-Encoding ////////////////

		if ( !::strcmp( kv.key, "content-transfer-encoding" ) ) {
			auto_vec< char > const value(
				to_lower_r( kv.value_begin, kv.value_end )
			);
			if ( ::strstr( value, "quoted-printable" ) )
				type.encoding_ = Quoted_Printable;
			else if ( ::strstr( value, "base64" ) )
				type.encoding_ = Base64;
			else if ( ::strstr( value, "binary" ) )
				type.encoding_ = Binary;
			continue;
		}

		////////// Deal with Content-Type /////////////////////////////

		if ( !::strcmp( kv.key, "content-type" ) ) {
			auto_vec< char > const value(
				to_lower_r( kv.value_begin, kv.value_end )
			);

			//
			// Extract the MIME type.
			//
			char const *s = value;
			while ( *s && is_space( *s ) ) ++s;
			if ( !*s )		// all whitespace: weird
				continue;
			string const mime_type( s, ::strcspn( s, "; \n\r\t" ) );

			//
			// See if there's a filter for the MIME type: if so,
			// set up to use it.  Note that a filter can override
			// our built-in handling of certain MIME types.
			//
			if ( FilterAttachment::const_pointer const
				f = attachment_filters[ mime_type ]
			) {
				type.content_type_ = External_Filter;
				//
				// Just in case there were two Content-Type
				// headers (weird, yes; but we have to be
				// robust) and we previously set the filter,
				// delete the old filter first so there won't
				// be a memory leak.
				//
				delete type.filter_;
				type.filter_ = new filter( *f );
				continue;
			}

			//
			// See if it's the text/"something" or "message/rfc822".
			//
			if ( mime_type == "text/plain" )
				type.content_type_ = Text_Plain;
			else if ( mime_type == "text/enriched" )
				type.content_type_ = Text_Enriched;
#ifdef	MOD_HTML
			else if ( mime_type == "text/html" )
				type.content_type_ = Text_HTML;
#endif
			else if ( ::strstr( value, "vcard" ) )
				type.content_type_ = Text_vCard;
			else if ( mime_type == "message/rfc822" )
				type.content_type_ = Message_RFC822;

			//
			// See if it's multipart/"something", i.e., mixed,
			// alternative, or parallel: we have to extract the
			// boundary string.
			//
			else if ( ::strstr( value, "multipart/" ) ) {
				char const *b = ::strstr( value, "boundary=" );
				if ( !b || !*(b += 9) ) {
					type.content_type_ = Not_Indexable;
					continue;	// weird case
				}
				//
				// Erase everything (including any surrounding
				// quotes) except the boundary string from the
				// value.
				//
				string boundary(
					kv.value_begin + (b - value),
					kv.value_end
				);
				if ( boundary[0] == '"' )
					boundary.erase( 0, 1 );
				if ( boundary[ boundary.length() - 1 ] == '"' )
					boundary.erase( boundary.size()-1, 1 );
				//
				// Push the boundary onto the stack.
				//
				boundary_stack_.push_back( boundary );
				type.content_type_ = Multipart;
				continue;
			}

			//
			// It's not a Content-Type we know anything about, so
			// it's not indexable.
			//
			type.content_type_ = Not_Indexable;
		}

		////////// Index the value of the header //////////////////////

		int meta_id;
		if ( associate_meta ) {
			//
			// Potentially index the words in the value of the
			// header where they are associated with the name of
			// the header as a meta name.
			//
			if ( (meta_id = find_meta( kv.key )) == No_Meta_ID )
				continue;
		} else
			meta_id = No_Meta_ID;

		encoded_char_range const e(
			kv.value_begin, kv.value_end, Seven_Bit
		);
		indexer::index_words( e, meta_id );
	}

	return type;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void mail_indexer::index_multipart(
		register char const *&c, register char const *end
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	multipart data.
//
// PARAMETERS
//
//	c	The pointer to beginning of the text to index.
//
//	end	The poitner to the end of the text to index.
//
// SEE ALSO
//
//	Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose
//	Internet Mail Extensions (MIME) Part One: Format of Internet Message
//	Bodies," RFC 822 Extensions Working Group of the Internet Engineering
//	Task Force, November 1996.
//
//*****************************************************************************
{
	register char const *nl;
	//
	// Find the beginning boundary string.
	//
	while ( (nl = find_newline( c, end )) != end ) {
		char const *const d = c;
		c = skip_newline( nl, end );
		if ( boundary_cmp( d, nl, boundary_stack_.back().c_str() ) )
			break;
	}
	if ( nl == end )
		return;

	while ( c != end ) {
		char const *const part_begin = c;
		//
		// Find the ending boundary string.
		//
		char const *part_end = end;
		while ( (nl = find_newline( c, end )) != end ) {
			part_end = c;
			c = skip_newline( nl, end );
			if ( boundary_cmp(
				part_end, nl, boundary_stack_.back().c_str()
			) )
				break;
		}

		//
		// Index the words between the boundaries.
		//
		encoded_char_range const part( part_begin, part_end );
		index_words( part );

		//
		// See if the boundary string is the final one, i.e., followed
		// by "--".
		//
		if ( part_end == end || nl[-1] == '-' && nl[-2] == '-' )
			break;
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	void mail_indexer::index_vcard(
		register char const *&c, char const *end
	)
//
// DESCRIPTION
//
//	Index the words in a vCard attachment.  The vCard "types" are made into
//	meta names.
//
// PARAMETERS
//
//	c	The pointer to the start of the vCard.
//
//	end	The pointer to the end of the vCard.
//
// CAVEAT
//
//	Nested vCards via the AGENT type are not handled properly, i.e., the
//	nested vCards are not treated as a vCards.
//
// SEE ALSO
//
//	Frank Dawson and Tim Howes.  "RFC 2426: vCard MIME Directory Profile,"
//	Network Working Group of the Internet Engineering Task Force, September
//	1998.
//
//*****************************************************************************
{
	key_value kv;
	while ( parse_header( c, end, &kv ) ) {
		//
		// Reuse parse_header() to parse vCard types, but trim them at
		// semicolons.
		//
		int const meta_id = find_meta( ::strtok( kv.key, ";" ) );
		if ( meta_id == No_Meta_ID )
			continue;
		//
		// Index the words in the value of the type marking them as
		// being associated with the name of the type.
		//
		encoded_char_range const e(
			kv.value_begin, kv.value_end, Eight_Bit
		);
		indexer::index_words( e, meta_id );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void mail_indexer::index_words(
		encoded_char_range const &e, int
	)
//
// DESCRIPTION
//
//	Index the words over the entire range that the iterator covers.
//
// PARAMETERS
//
//	e	The encoded character range to be indexed.
//
//*****************************************************************************
{
	encoded_char_range::const_iterator c = e.begin();
	message_type const type( index_headers( c.pos(), c.end_pos() ) );

	if ( type.content_type_ == Not_Indexable || type.encoding_ == Binary ) {
		//
		// The attachment is something we can't index so just skip over
		// it.
		//
		return;
	}

	//
	// Create a new encoded_char_range having the same range but the
	// Content-Transfer-Encoding given in the headers.
	//
	encoded_char_range const e2( c.pos(), c.end_pos(), type.encoding_ );

	switch ( type.content_type_ ) {

		case External_Filter:
			index_via_filter( type.filter_, e2 );
			break;

		case Text_Plain:
			indexer::index_words( e2 );
			break;

		case Text_Enriched:
			index_enriched( e2 );
			break;
#ifdef	MOD_HTML
		case Text_HTML: {
			static indexer *const
				html = indexer::find_indexer( "HTML" );
			html->index_words( e2 );
			break;
		}
#endif
		case Text_vCard:
			index_vcard( c.pos(), c.end_pos() );
			break;

		case Message_RFC822:
			index_words( e2 );
			break;

		case Multipart:
			index_multipart( c.pos(), c.end_pos() );
			boundary_stack_.pop_back();
			return;
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void mail_indexer::new_file()
//
// DESCRIPTION
//
//	Clear the boundary stack prior to indexing a new file.  This function
//	is out-of-line due to being virtual.
//
//*****************************************************************************
{
	boundary_stack_.clear();
	did_last_header = false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ bool mail_indexer::parse_header(
		register char const *&c, register char const *end,
		key_value *kv
	)
//
// DESCRIPTION
//
//	This function parses a single header and its value.  It properly
//	handles values that are folded across multiple lines.
//
// PARAMETERS
//
//	c	The pointer that must be positioned at the first character in a
//		header.  It is left after the last character in the value, or,
//		if there are no more headers, after the blank line marking the
//		end of the headers.
//
//	end	The pointer to the end of the range.
//
//	kv	A pointer to the key_value to be modified only if a header is
//		parsed.  The key is always converted to lower case.
//
// SEE ALSO
//
//	David H. Crocker.  "RFC 822: Standard for the Format of ARPA Internet
//	Text Messages," Department of Electrical Engineering, University of
//	Delaware, August 1982.
//
//*****************************************************************************
{
	if ( did_last_header )
		return did_last_header = false;

	char const *header_begin, *header_end, *nl;

	while ( 1 ) {
		if ( (nl = find_newline( c, end )) == end )
			return false;
		//
		// Parse a header by looking for the terminating ':'.
		//
		header_begin = c;
		while ( c != nl && *c != ':' )
			++c;
		//
		// We have to check for the special case of the "From" header
		// that doesn't have the ':', i.e., the one in the envelope,
		// not the letter, and ignore it.
		//
		if ( c >= header_begin + 4 /* 4 == strlen( "From" ) */ &&
			!::strncmp( header_begin, "From ", 5 )
		) {
			if ( (c = skip_newline( nl, end )) == end )
				return false;
			continue;
		}

		if ( c == nl )			// didn't find it: weird
			return false;
		header_end = c;
		break;
	}

	//
	// Parse a value.
	//
	if ( ++c == end )			// skip past the ':'
		return false;
	kv->value_begin = c;
	while ( 1 ) {
		if ( (c = skip_newline( nl, end )) == end )
			break;
		//
		// See if the value is folded across multiple lines: if the
		// first character on the next line isn't whitespace, then the
		// value isn't folded (it's the next header).
		//
		if ( !is_space( *c ) )
			goto more_headers;
		//
		// The first character on the next line is whitespace: see how
		// much of the rest of the next line is also whitepace.
		//
		do {
			if ( *c == '\r' || *c == '\n' ) {
				//
				// The entire next line is whitespace: consider
				// it the end of all the headers and therefore
				// also the end of this value.  Also skip the
				// blank line.
				//
				c = skip_newline( c, end );
				goto last_header;
			}
		} while ( ++c != end && is_space( *c ) );

		//
		// The next line has at least one non-leading non-whitespace
		// character; therefore, it is a continuation of the current
		// header's value: reposition "nl" and start over.
		//
		if ( (nl = find_newline( c, end )) == end )
			break;
	}
last_header:
	did_last_header = true;
more_headers:
	kv->value_end = nl;
	//
	// Canonicalize the name of the header to lower case.
	//
	kv->key = to_lower_r( header_begin, header_end );
	return true;
}

#endif	/* MOD_MAIL */