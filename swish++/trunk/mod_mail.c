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
#include <cctype>
#include <cstring>
#include <map>
#include <string>

// local
#include "config.h"
#include "ExcludeMeta.h"
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
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

struct header_value {
	char const *header;
	char const *value_begin, *value_end;
};

extern ExcludeMeta	exclude_meta_names;
extern IncludeMeta	include_meta_names;
extern TitleLines	num_title_lines;

mail_indexer::stack_type mail_indexer::boundary_stack_;

bool			header_cmp(
				file_vector::const_iterator &pos,
				file_vector::const_iterator end,
				char const *tag
			);
bool			parse_header(
				file_vector::const_iterator &pos,
				file_vector::const_iterator end,
				header_value *hv
			);

//*****************************************************************************
//
// SYNOPSIS
//
	bool boundary_cmp(
		register file_vector::const_iterator c,
		register file_vector::const_iterator end,
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
//	c		The iterator to use.  It is presumed to be positioned
//			at the first character on a line.  If the boundary
//			string matches, it is repositioned at the first
//			character past boundary string; otherwise, it is not
//			touched.
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
	inline bool is_newline(
		file_vector::const_iterator c,
		file_vector::const_iterator end
	)
//
// DESCRIPTION
//
//	Checks to see if the character (or character sequence) is a newline
//	(or "internet newline," i.e., a CR-LF pair).
//
// PARAMETERS
//
//	c	The iterator to use: it is assumed not to be at "end".
//
//	end	The iterator marking the end of the range to check.
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
	char const* mail_indexer::find_title( file_vector const &file ) const
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
//	David H. Croker.  "RFC 822: Standard for the Format of ARPA Internet
//	Text Messages," Department of Electrical Engineering, University of
//	Delaware, August 1982.
//
//*****************************************************************************
{
	int lines = 0;

	register file_vector::const_iterator c = file.begin();
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
		file_vector::const_iterator const
			nl = find_newline( c, file.end() );
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
		file_vector::const_iterator &c,
		register file_vector::const_iterator end,
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
//	c	The iterator to use.  It is presumed to be positioned at the
//		first character after the '<'.  If the tag name matches, it is
//		repositioned at the first character past the name; otherwise,
//		it is not touched.
//
//	end	The iterator marking the end of the file.
//
//	header	The string to compare against; it must be in lower case.
//
// RETURN VALUE
//
//	Returns true only if the header matches.
//
//*****************************************************************************
{
	register file_vector::const_iterator d = c;
	while ( *header && d != end && *header++ == to_lower( *d++ ) ) ;
	return *header ? false : c = d;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void mail_indexer::index_enriched(
		register encoded_char_range::const_iterator &c
	)
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
//	c	The iterator marking the beginning of the text to index.
//
//	end	The iterator marking the end of the text to index.
//
// SEE ALSO
//
//	Tim Berners-Lee.  "RFC 1563: The text/enriched MIME Content-type,"
//	Network Working Group of the Internet Engineering Task Force, January
//	1994.
//
//*****************************************************************************
{
	char		buf[ Word_Hard_Max_Size + 1 ];
	register char*	word;
	bool		in_word = false;
	int		len;

	while ( !c.at_end() ) {
		register file_vector::value_type ch = iso8859_to_ascii( *c++ );

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
		register file_vector::const_iterator &c,
		register file_vector::const_iterator end
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
//	c	The iterator to use.  It must be positioned at the first
//		character in the file.  It is left after the last header.
//
//	end	The iterator marking the end of the file.
//
// RETURN VALUE
//
//	Returns the type and encoding of the message based on the headers.
//
// SEE ALSO
//
//	David H. Croker.  "RFC 822: Standard for the Format of ARPA Internet
//	Text Messages," Department of Electrical Engineering, University of
//	Delaware, August 1982.
//
//*****************************************************************************
{
	typedef map< char const*, header_type > header_map;

	static header_map h_map;
	if ( h_map.empty() ) {
		//
		// These are the headers we're interested in.  Any header not
		// listed here is ignored.
		//
		h_map[ "bcc" ]			= Index_Header;
		h_map[ "cc" ]			= Index_Header;
		h_map[ "comments" ]		= Index_Header;
		h_map[ "content-description" ]	= Index_Header;
		h_map[ "content-transfer-encoding"] = Content_Transfer_Encoding;
		h_map[ "content-type" ]		= Content_Type;
		h_map[ "from" ]			= Index_Header;
		h_map[ "keywords" ]		= Index_Header;
		h_map[ "resent-to" ]		= Index_Header;
		h_map[ "subject" ]		= Index_Header;
		h_map[ "to" ]			= Index_Header;
	}

	//
	// We assume text/plain and 7-bit US-ASCII as stated in RFC 2045, sec.
	// 5.2., "Content-Type Defaults":
	//
	//	Default RFC 822 messages without a MIME Content-Type header are
	//	taken by this protocol to be plain text in the US-ASCII
	//	character set, which can be explicitly specified as:
	//
	//		Content-type: text/plain; charset=us-ascii
	//
	//	This default is assumed if no Content-Type header field is
	//	specified.
	//
	message_type type( Text_Plain, Seven_Bit );

	header_value hv;
	while ( parse_header( c, end, &hv ) ) {
		header_map::const_iterator const i = h_map.find( hv.header );
		if ( i == h_map.end() ) {
			//
			// We're not interested in any header not specifically
			// accounted for above.
			//
			continue;
		}

		switch ( i->second ) {

			case Index_Header: {
				int const meta_id = find_meta( hv.header );
				if ( meta_id == No_Meta_ID )
					break;
				//
				// Index the words in the value of the header
				// marking them as being associated with the
				// name of header.
				//
				encoded_char_range::const_iterator e(
					hv.value_begin, hv.value_end, Seven_Bit
				);
				indexer::index_words( e, meta_id );
				/*
				indexer::index_words(
					hv.value_begin, hv.value_end,
					Seven_Bit, meta_id
				);
				*/
				break;
			}

			case Content_Transfer_Encoding: {
				char const *const v = to_lower(
					hv.value_begin, hv.value_end
				);
				if ( ::strstr( v, "quoted-printable" ) )
					type.second = Quoted_Printable;
				else if ( ::strstr( v, "base64" ) )
					type.second = Base64;
				else if ( ::strstr( v, "binary" ) )
					type.second = Binary;
				break;
			}

			case Content_Type: {
				char const *const v = to_lower(
					hv.value_begin, hv.value_end
				);
				//
				// See if it's the text/"something"
				//
				if ( ::strstr( v, "text/plain" ) ) {
					type.first = Text_Plain;
					break;
				}
				if ( ::strstr( v, "text/enriched" ) ) {
					type.first = Text_Enriched;
					break;
				}
				if ( ::strstr( v, "message/rfc822" ) ) {
					type.first = Message_RFC822;
					break;
				}
#ifdef	MOD_HTML
				if ( ::strstr( v, "text/html" ) ) {
					type.first = Text_HTML;
					break;
				}
#endif
				//
				// See if it's a multipart/"something," i.e.,
				// alternative, mixed, or parallel.  If not, we
				// don't know what to do with it so it's not
				// indexable.
				//
				if ( !::strstr( v, "multipart/" ) ) {
					type.first = Not_Indexable;
					continue;
				}

				//
				// If so, we have to extract the boundary
				// string.
				//
				char const *b = ::strstr( v, "boundary=" );
				if ( !b || !*(b+=9) )
					continue;	// weird case
				//
				// Erase everything (including any surrounding
				// quotes) except the boundary string from the
				// value.
				//
				string boundary(
					hv.value_begin + (b-v), hv.value_end
				);
				if ( boundary[0] == '"' )
					boundary.erase( 0, 1 );
				if ( boundary[ boundary.length() - 1 ] == '"' )
					boundary.erase( boundary.size()-1, 1 );

				//
				// Push the boundary onto the stack.
				//
				boundary_stack_.push_back( boundary );
				type.first = Multipart;
			}
		}
	}

	return type;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void mail_indexer::index_multipart(
		register file_vector::const_iterator &c,
		register file_vector::const_iterator end
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	multipart data.
//
// PARAMETERS
//
//	c	The iterator marking the beginning of the text to index.
//
//	end	The iterator marking the end of the text to index.
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
	register file_vector::const_iterator nl;
	//
	// Find the beginning boundary string.
	//
	while ( (nl = find_newline( c, end )) != end ) {
		file_vector::const_iterator const d = c;
		c = skip_newline( nl, end );
		if ( boundary_cmp( d, nl, boundary_stack_.back().c_str() ) )
			break;
	}
	if ( nl == end )
		return;

	while ( c != end ) {
		file_vector::const_iterator const part_begin = c;
		//
		// Find the ending boundary string.
		//
		file_vector::const_iterator part_end = end;
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
		encoded_char_range::const_iterator part( part_begin, part_end );
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
		register encoded_char_range::const_iterator &c
	)
//
// DESCRIPTION
//
//	Index the words in a vCard attachment.
//
// PARAMETERS
//
//	c	The iterator to use.
//
//	end	The ...
//
// SEE ALSO
//
//	Frank Dawson and Tim Howes.  "RFC 2426: vCard MIME Directory Profile,"
//	Network Working Group of the Internet Engineering Task Force, September
//	1998.
//
//*****************************************************************************
{
	// m!^adr(?:;\w+(?:=.+)?)*:(.+)$!im;
	// m!^label(?:;\w+(?:=.+)?)*:(.+)$!im;

	/*
		BEGIN:vCard
		VERSION:3.0
		FN:Paul J. Lucas
		ORG:Internet Pictures Corporation
		ADR;TYPE=WORK,PARCEL:;;163 Everett Street
		 ;Palo Alto;CA
		X-WHATEVER:FOOBAR
		END:vCard
	*/

	// handle folder values

/*
	int meta_id;

	if ( (meta_id = find_meta( "fn" )) != No_Meta_ID )
		indexer::index_words( pos, end, encoding, meta_id );
*/

	// m!^email;(?:type=)?internet,pref:(.+)$!

	// m!^email;(?:type=)?internet:(.+)$!

	// m!^categories:(.+)!i
	// m!^class:(.+)!i
	// m!^fn:(.+)!i
	// m!^nickname:(.+)!i
	// m!^note:(.+)!i
	// m!^org:(.+)!i
	// m!^role:(.+)!i
	// m!^title:(.+)!i

	// m!^tel;(?:type=)?.*work.*:(.+)$!

}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void mail_indexer::index_words(
		encoded_char_range::const_iterator &c,
		int
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.
//
// PARAMETERS
//
//	c	The iterator marking the beginning of the text to index.
//
//	end	The iterator marking the end of the text to index.
//
//*****************************************************************************
{
	message_type const type( index_headers( c.pos(), c.end_pos() ) );
	//
	// Create a new iterator having the same range but the Content-
	// Transfer-Encoding given in the headers.
	//
	encoded_char_range::const_iterator c2(
		c.pos(), c.end_pos(), type.second
	);

	switch ( type.first ) {

		case Text_Plain:
		case Message_RFC822:
			if ( type.second != Binary )
				indexer::index_words( c2 );
			break;

		case Text_Enriched:
			index_enriched( c2 );
			break;
#ifdef	MOD_HTML
		case Text_HTML: {
			static indexer *const
				html = indexer::find_indexer( "HTML" );
			html->index_words( c2 );
			break;
		}
#endif
		case Text_vCard:
			index_vcard( c2 );
			break;

		case Multipart:
			index_multipart( c.pos(), c.end_pos() );
			boundary_stack_.pop_back();
			return;
	}
	c.pos( c2.pos() );
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
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool parse_header(
		register file_vector::const_iterator &c,
		register file_vector::const_iterator end,
		header_value *hv
	)
//
// DESCRIPTION
//
//	This function parses a single header and its value.
//
// PARAMETERS
//
//	c	The iterator to use.  It must be positioned at the first
//		character in a header.  It is left after the last character in
//		the value.
//
//	end	The iterator marking the end of the file.
//
// SEE ALSO
//
//	David H. Croker.  "RFC 822: Standard for the Format of ARPA Internet
//	Text Messages," Department of Electrical Engineering, University of
//	Delaware, August 1982.
//
//*****************************************************************************
{
	static bool did_last_header;
	if ( did_last_header )
		return did_last_header = false;

	file_vector::const_iterator nl = find_newline( c, end );
	if ( nl == end )
		return false;
	//
	// Parse a header by looking for the terminating ':'.
	//
	file_vector::const_iterator const header_begin = c;
	while ( c != nl && *c != ':' )
		++c;
	if ( c == nl )				// didn't find it: weird
		return false;
	file_vector::const_iterator const header_end = c;

	//
	// Parse a value.
	//
	if ( ++c == end )			// skip past the ':'
		return false;
	hv->value_begin = c;
	while ( 1 ) {
		if ( (c = skip_newline( nl, end )) == end )
			break;
		//
		// See if the value is folded across multiple lines: if the
		// first character on the next line isn't whitespace, then the
		// value isn't folded (it's the next header).
		//
		if ( !isspace( *c ) )
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
				// also the end of this value.
				//
				goto last_header;
			}
		} while ( ++c != end && isspace( *c ) );

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
	hv->value_end = nl;

	//
	// Canonicalize the name of the header to lower case.
	//
	hv->header = to_lower( header_begin, header_end );

	return true;
}

#endif	/* MOD_MAIL */
