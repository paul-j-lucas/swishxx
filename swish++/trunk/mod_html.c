/*
**	SWISH++
**	mod_html.c
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

#ifdef	MOD_HTML

// standard
#include <cctype>
#include <cstring>

// local
#include "config.h"
#include "encoded_char.h"
#include "entities.h"
#include "ExcludeClass.h"
#include "mod_html.h"
#include "platform.h"
#include "TitleLines.h"
#include "util.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

HTML_indexer::stack_type	HTML_indexer::element_stack_;

bool	is_html_comment( encoded_char_range::const_iterator &pos );
bool	skip_html_tag( encoded_char_range::const_iterator &pos );
bool	tag_cmp( encoded_char_range::const_iterator &pos, char const *tag );

//*****************************************************************************
//
// SYNOPSIS
//
	char entity_to_ascii( register encoded_char_range::const_iterator &c )
//
// DESCRIPTION
//
//	Convert either a numeric or character entity reference to its ASCII
//	character equivalent (if it has one).  A numeric reference is a
//	character sequence having the form:
//
//		&#d;
//		&#xh;
//		&#Xh;
//
//	where 'd' is a sequence of 1 or more decimal digits [0-9] and 'h' is a
//	sequence of 1 or more hexadecimal digits [0-9A-Fa-f].  A character
//	entity reference is a character sequence having the form:
//
//		&ref;
//
//	The numeric and character entities converted are listed in the
//	entities.c file.
//
// PARAMETERS
//
//	c	This iterator is to be positioned at the character past the
//		'&'; if an entity is found, it is left after the ';'.
//
// RETURN VALUE
//
//	Returns the ASCII equivalent of the entity or ' ' (space) if either
//	there is no equivalent or the entity is malformed.
//
// EXAMPLE
//
//	The references in "r&eacute;sum&#233;" will be converted to the letter
//	'e' resulting in the "resume" string.
//
// SEE ALSO
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "On SGML and HTML: SGML
//	constructs used in HTML: Entities," HTML 4.0 Specification, section
//	3.2.3, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/intro/sgmltut.html#h-3.2.3
//
//	International Standards Organization.  "ISO 8859-1: Information
//	Processing -- 8-bit single-byte coded graphic character sets -- Part 1:
//	Latin alphabet No. 1," 1987.
//
//	International Standards Organization.  "ISO 8879: Information
//	Processing -- Text and Office Systems -- Standard Generalized Markup
//	Language (SGML)," 1986.
//
//*****************************************************************************
{
	////////// See if it's a numeric character reference //////////////////

	bool const is_num = (!c.at_end() && *c == '#');
	bool const is_hex = (is_num && !(++c).at_end() &&
		(*c == 'x' || *c == 'X'));
	if ( is_hex ) ++c;

	////////// Find the terminating ';' ///////////////////////////////////

	char entity_buf[ Entity_Max_Size + 1 ];
	int entity_len = 0;

	while ( !c.at_end() && *c != ';' ) {
		if ( ++entity_len > Entity_Max_Size )
			return ' ';			// give up looking
		entity_buf[ entity_len - 1 ] = *c++;
	}
	if ( c.at_end() )				// didn't find it
		return ' ';
	++c;						// put past ';'

	entity_buf[ entity_len ] = '\0';

	////////// Look up character entity reference /////////////////////////

	if ( !is_num )
		return char_entity_map::instance()[ entity_buf ];

	////////// Parse a numeric character reference ////////////////////////

	register unsigned n = 0;
	for ( register char const *e = entity_buf; *e; ++e ) {
		if ( is_hex ) {
			if ( !isxdigit( *e ) )		// bad hex num
				return ' ';
			n = (n << 4) | ( isdigit( *e ) ?
				*e - '0' : tolower( *e ) - 'a' + 10
			);
		} else {
			if ( !isdigit( *e ) )		// bad dec num
				return ' ';
			n = n * 10 + *e - '0';
		}
	}

	return	n < sizeof( iso8859_map ) / sizeof( iso8859_map[0] ) ?
		iso8859_to_ascii( n ) : ' ';
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool find_attribute( encoded_char_range &e, char const *attribute )
//
// DESCRIPTION
//
//	Given an attribute's name, find its value within an HTML or XHTML
//	element's start tag, e.g., find the value of the NAME attribute within
//	the META element.  Case is irrelevant.
//
//	The HTML 4.0 specification is vague in stating whether whitespace is
//	legal around the '=' character separating an attribute's name from its
//	value; hence, this function is lenient in that it will consider:
//
//		NAME = "Author"
//	and:
//		NAME="Author"
//
//	equivalent.
//
// PARAMETERS
//
//	e		The range spanning where to look.  If the attribute is
//			found, this iterator is repositioned to be at the first
//			character of the value; otherwise, it is not touched.
//
//	attribute	The name of the attribute to find; it must be in lower
//			case.
//
// RETURN VALUE
//
//	Returns true only if the attribute was found.
//
// CAVEAT
//
//	This function only handles attributes with explicit values, i.e., those
//	followed by an '=' and value, and not Boolean attributes.  But, since
//	it was written with the intent of only parsing attributes in META tags,
//	namely NAME and CONTENT, it's OK.
//
// SEE ALSO
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "On SGML and HTML: SGML
//	constructs used in HTML: Attributes," HTML 4.0 Specification, section
//	3.2.2, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/intro/sgmltut.html#h-3.2.2
//
//*****************************************************************************
{
	encoded_char_range::const_iterator c = e.begin();
	while ( !c.at_end() ) {
		if ( !isalpha( *c ) ) {
			++c;
			continue;
		}
		//
		// We just found the start of a potentially matching attribute
		// name: now try to find its end.
		//
		register char const *a = attribute;
		while ( !c.at_end() && to_lower( *c ) == *a )
			++c, ++a;
		while ( !c.at_end() && ( isalpha( *c ) || *c == '-' ) )
			++c;
		while ( !c.at_end() && isspace( *c ) )
			++c;
		if ( c.at_end() )
			break;
		if ( *c != '=' )		// we do only NAME=VALUE form
			continue;
		while ( !(++c).at_end() && isspace( *c ) )
			;
		if ( c.at_end() )
			break;
		//
		// Determine the span of the attribute's value: if it started
		// with a quote, it's terminated only by the matching closing
		// quote; if not, it's terminated by a whitespace character (or
		// running into 'end').
		//
		// This is more lenient than the HTML 4.0 specification in that
		// it allows non-quoted values to contain characters other than
		// the set [A-Za-z0-9.-], i.e., any non-whitespace character.
		//
		char const quote = ( *c == '"' || *c == '\'' ) ? *c : 0;
		if ( quote && (++c).at_end() )
			break;
		encoded_char_range::const_pointer const new_begin = c.pos();
		for ( ; !c.at_end(); ++c )
			if ( quote ) {		// stop at matching quote only
				if ( *c == quote )
					break;
			} else if ( isspace( *c ) )
				break;		// stop at whitespace

		if ( !*a ) {			// attribute name matched...
			e.begin_pos( new_begin );
			e.end_pos( c );		// ...and got entire value  :)
			return true;
		}
		if ( c.at_end() )
			break;
		++c;				// attribute name didn't match
	}
	return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	char const* HTML_indexer::find_title( mmap_file const &file ) const
//
// DESCRIPTION
//
//	Scan through the first num_title_lines lines in an HTML or XHTML file
//	looking for <TITLE>...</TITLE> tags to extract the title.
//
// PARAMETERS
//
//	file	The file presumed to be an HTML or XHTML file.
//
// RETURN VALUE
//
//	Returns the title string or null if no title can be found.
//
// SEE ALSO
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "The global structure of
//	an HTML document: The document head: The TITLE element," HTML 4.0
//	Specification, section 7.4.2, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/intro/sgmltut.html#h-7.4.2
//
//*****************************************************************************
{
	static char const *const title_tag[] = {	// tag_index
		"title",				//	0
		"/title"				//	1
	};
	int tag_index = 0;
	int lines = 0;

	//
	// <TITLE>This is a title</TITLE>
	//        |              |
	//        after          before
	//
	// Mark the positions after the closing '>' of the start tag and before
	// the opening '<' of the end tag.  What's in between is the title.
	//
	mmap_file::const_iterator after, before;

	encoded_char_range::const_iterator c( file.begin(), file.end() );
	while ( !c.at_end() ) {
		if ( *c == '\n' && ++lines > num_title_lines ) {
			//
			// Didn't find <TITLE> within first num_title_lines
			// lines of file: forget it.
			//
			return 0;
		}

		if ( *c != '<' ) {			// not a tag: forget it
			++c;
			continue;
		}

		//
		// Found the start of an HTML or XHTML tag: mark the position
		// before it in case it turns out to be the </TITLE> tag.
		//
		before = c.pos();

		if ( is_html_comment( ++c ) )
			continue;

		//
		// Is the HTML or XHTML tag a TITLE tag?
		//
		bool const found_title_tag =
			tag_cmp( c, title_tag[ tag_index ] );
		skip_html_tag( c );			// skip until '>'
		if ( !found_title_tag )			// wrong tag
			continue;

		if ( tag_index == 1 )			// found entire title
			return tidy_title( after, before );

		//
		// Found the <TITLE> tag: mark the position after it and begin
		// looking for the </TITLE> tag.
		//
		after = c.pos();
		++tag_index;
	}

	//
	// The file has less than num_title_lines lines and no <TITLE> was
	// found.
	//
	return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void HTML_indexer::index_words(
		encoded_char_range const &e, int meta_id
	)
//
// DESCRIPTION
//
//	Index the words between the given iterators.  The text is assumed to be
//	HTML or XHTML.
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
		register char ch = *c++;
		//
		// If the character is an '&' (the start of a entity
		// reference), convert the entity reference to ASCII;
		// otherwise, convert the ISO 8859 character to ASCII.
		//
		ch = ch == '&' ? entity_to_ascii( c ) : iso8859_to_ascii( ch );

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

		if ( ch == '<' && meta_id == No_Meta_ID ) {
			//
			// If the character is a '<' (the start of an HTML or
			// XHTML tag), and we're not in the midst of indexing
			// the value of a META element's CONTENT attribute,
			// then parse the HTML or XHTML tag.
			//
			parse_html_tag( c );
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
	bool is_html_comment( register encoded_char_range::const_iterator &c )
//
// DESCRIPTION
//
//	Checks to see if the current HTML or XHTML element is the start of a
//	comment. If so, skip it scanning for the closing "-->" character
//	sequence.  The HTML specification permits whitespace between the "--"
//	and the ">" (for some strange reason).  Unlike skipping an ordinary
//	HTML or XHTML tag, quotes are not significant and no attempt must be
//	made either to "balance" them or to ignore what is in between them.
//
//	This function is more lenient than the HTML 4.0 specification in that
//	it allows for a string of hyphens within a comment since this is common
//	in practice; the specification considers this to be an error.
//
// PARAMETERS
//
//	c	The iterator to use.  It is presumed to be positioned at the
//		first character after the '<'.  If the tag is the start of a
//		comment, it is repositioned at the first character past the
//		tag, i.e., past the '>'; otherwise, it is not touched.
//
// RETURN VALUE
//
//	Returns true only if the current element is the beginning of a comment.
//
// SEE ALSO
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs.  "On SGML and HTML: SGML
//	constructs used in HTML: Comments," HTML 4.0 Specification, section
//	3.2.4, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/intro/sgmltut.html#h-3.2.4
//
//*****************************************************************************
{
	if ( tag_cmp( c, "!--" ) ) {
		while ( !c.at_end() ) {
			if ( *c++ != '-' )
				continue;
			while ( !c.at_end() && *c == '-' )
				++c;
			while ( !c.at_end() && isspace( *c ) )
				++c;
			if ( !c.at_end() && *c++ == '>' )
				break;
		}
		return true;
	}
	return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	void HTML_indexer::parse_html_tag(
		register encoded_char_range::const_iterator &c
	)
//
// DESCRIPTION
//
//	This function does everything skip_html_tag() does but additionally
//	does extra parsing for certain HTML or XHTML elements:
//
//	1. If the tag contains a CLASS attribute whose value is among the set
//	   of class names specified as those not to index, then all the text up
//	   to the tag that ends the element will not be indexed.
//
//	   For an element that has an optional end tag, "the tag that ends the
//	   element" is either the element's end tag or a tag of another element
//	   that implicitly ends it; for an element that does not have an end
//	   tag, "the tag that ends the element" is the element's start tag.
//
//	2. If the tag contains a TITLE attribute, it indexes the words of its
//	   value.
//
//	3. If the tag is an AREA, IMG, or INPUT element and contains an ALT
//	   attribute, it indexes the words of its value.
//
//	4. If the tag is a META element, it parses its NAME and CONTENT
//	   attributes indexing the words of the latter associated with the
//	   former.
//
//	5. If the tag is an OBJECT element and contains a STANDBY attribute, it
//	   indexes the words of its value.
//
//	6. If the tag is a TABLE element and contains a SUMMARY attribute, it
//	   indexes the words of its value.
//
// PARAMETERS
//
//	c	The iterator to use.  It must be positioned at the character
//		after the '<'; it is repositioned at the first character after
//		the '>'.
//
// SEE ALSO
//
//	Dave Raggett, Arnaud Le Hors, and Ian Jacobs. "The global structure of
//	an HTML document: The document head: The title attribute," HTML 4.0
//	Specification, section 7.4.3, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/REC-html40/struct/global.html#adef-title
//
//	---.  "The global structure of an HTML document: The document head:
//	Meta data," HTML 4.0 Specification, section 7.4.4, World Wide Web
//	Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/struct/global.html#h-7.4.4
//
//	---.  "The global structure of an HTML document: The document body:
//	Element identifiers: the id and class attributes," HTML 4.0
//	Specification, section 7.5.2, World Wide Web Consortium, April 1998.
//		http://www.w3.org/TR/PR-html40/struct/global.html#h-7.5.2
//
//	---. "Tables: Elements for constructing tables: The TABLE element,"
//	HTML 4.0 Specification, section 11.2.1, World Wide Web Consortium,
//	April 1998.
//		http://www.w3.org/TR/REC-html40/struct/tables.html#adef-summary
//
//	---. "Objects, Images, and Applets: Generic inclusion: the OBJECT
//	element," HTML 4.0 Specification, section 13.3, World Wide Web
//	Consortium, April 1998.
//		http://www.w3.org/TR/REC-html40/struct/objects.html#adef-standby
//
//	---. "Objects, Images, and Applets: How to specify alternate text,"
//	HTML 4.0 Specification, section 13.8, World Wide Web Consortium, April
//	1998.
//		http://www.w3.org/TR/REC-html40/struct/objects.html#h-13.8
//
//*****************************************************************************
{
	if ( c.at_end() )
		return;

	encoded_char_range::const_iterator name = c;
	if ( skip_html_tag( c ) || *name == '!' )
		return;
	name.end_pos( c.prev_pos() );
	bool const is_end_tag = *name == '/';

	////////// Deal with elements of a class not to index /////////////////

	if ( !exclude_class_names.empty() ) {		// else, don't bother
		char tag_buf[ Tag_Name_Max_Size + 2 ];	// 1 for '/', 1 for null
		{ // local scope
		//
		// Copy only the tag name by stopping at a whitespace character
		// (or running into the end of the tag); also convert it to
		// lower case.  (We don't call to_lower() in util.c so as not
		// to waste time copying the entire tag with its attributes
		// since we only want the tag name.)
		//
		register char *to = tag_buf;
		encoded_char_range::const_iterator from = name;
		while ( !from.at_end() && !isspace( *from ) ) {
			//
			// Check to see if the name is too long to be a valid
			// one for an HTML element: if it is, invalidate it by
			// writing "garbage" into the name so something like
			// "BLOCKQUOTED" (an invalid name) won't match
			// "BLOCKQUOTE" (a valid name) when one letter shorter
			// and throw off element closures.
			//
			if ( to - tag_buf >= Tag_Name_Max_Size + is_end_tag ) {
				to = tag_buf;
				*to++ = '\1';
				break;
			}
			*to++ = to_lower( *from++ );
		}
		*to = '\0';
		} // local scope

		////////// Close open element(s) //////////////////////////////

		while ( !element_stack_.empty() &&
			element_stack_.back().first->second.close_tags.contains(
				tag_buf
			)
		) {
			// This element closes the currently open element.
			//
			if ( element_stack_.back().second ) {
				//
				// The currently open element is a member of
				// one of the classes not being being indexed:
				// resume indexing.
				//
				resume_indexing();
			}
			char const *const
				start_tag = element_stack_.back().first->first;
			element_stack_.pop_back();

			//
			// We have to stop closing elements if we encounter the
			// start tag matching the end tag.
			//
			if ( !::strcmp( tag_buf + 1, start_tag ) )
				break;
		}

		if ( is_end_tag ) {
			//
			// The tag is an end tag: it doesn't have attributes.
			//
			return;
		}

		////////// Look for CLASS attribute ///////////////////////////

		bool is_no_index_class = false;

		encoded_char_range class_att = name;
		if ( find_attribute( class_att, "class" ) ) {
			//
			// CLASS attribute values can contain multiple classes
			// separated by whitespace: we must iterate over all of
			// them to see if one of them is among the set not to
			// index.
			//
			char *names = to_lower( class_att );
			register char const *name;
			while ( name = ::strtok( names, " \f\n\r\t\v" ) ) {
				if ( exclude_class_names.contains( name ) ) {
					is_no_index_class = true;
					break;
				}
				names = 0;
			}
		}

		element_map const &elements = element_map::instance();
		element_map::const_iterator const e = elements.find( tag_buf );
		if ( e != elements.end() ) {
			//
			// We found the element in our internal table: now do
			// different stuff depending upon whether its end tag
			// is forbidden or not.
			//
			if ( e->second.end_tag != element::forbidden ) {
				//
				// The element's end tag isn't forbidden, so we
				// have to keep track of it on the stack and
				// whether the text between the start and end
				// tags should be indexed or not.
				//
				// Note that we have to keep track of all HTML
				// or XHTML elements even if they are not
				// members of a class not to be indexed because
				// they could be nested inside of an element
				// that is, e.g.:
				//
				//	<DIV CLASS=ignore>
				//	  <SPAN CLASS=other>Hello</SPAN>
				//	</DIV>
				//
				element_stack_.push_back( make_pair(
					&*e, is_no_index_class
				) );
				if ( is_no_index_class ) {
					//
					// A class name in the value of this
					// element's CLASS attribute is among
					// the set not to index: suspend
					// indexing.
					//
					suspend_indexing();
				}
			}
			if ( is_no_index_class ) {
				//
				// Regardless of this element's end tag, it is
				// to be ignored, so we can stop parsing it.
				//
				return;
			}
		} else {
			// We didn't find the element in our internal table:
			// ignore it.  We really should do something better
			// because this could potentially mess up the proper
			// closing of elements, but, since we know nothing
			// about this element, there's nothing better that can
			// be done.
		}
	}

	if ( is_end_tag ) {
		//
		// The tag is an end tag: it doesn't have attributes.
		//
		return;
	}

	////////// Look for a TITLE attribute /////////////////////////////////

	encoded_char_range title_att = name;
	if ( find_attribute( title_att, "title" ) )
		index_words( title_att );

	////////// Look for an ALT attribute //////////////////////////////////

	if (	tag_cmp( name, "area" ) ||
		tag_cmp( name, "img" ) ||
		tag_cmp( name, "input" )
	) {
		encoded_char_range alt_att = name;
		if ( find_attribute( alt_att, "alt" ) )
			index_words( alt_att );
		return;
	}

	////////// Parse a META element ///////////////////////////////////////

	if ( tag_cmp( name, "meta" ) ) {
		encoded_char_range name_att    = name;
		encoded_char_range content_att = name;
		if (	find_attribute( name_att, "name" ) &&
			find_attribute( content_att, "content" )
		) {
			//
			// Canonicalize the value of the NAME attribute to
			// lower case.
			//
			char const *const name = to_lower( name_att );

			//
			// Do not index the words in the value of the CONTENT
			// attribute if either the value of the NAME attribute
			// is among the set of meta names to exclude or not
			// among the set to include.
			//
			int const meta_id = find_meta( name );
			if ( meta_id == No_Meta_ID )
				return;

			//
			// Index the words in the value of the CONTENT
			// attribute marking them as being associated with the
			// value of NAME attribute.
			//
			index_words( content_att, meta_id );
		}
		return;
	}

	////////// Look for a STANDBY attribute ///////////////////////////////

	if ( tag_cmp( name, "object" ) ) {
		encoded_char_range standby_att = name;
		if ( find_attribute( standby_att, "standby" ) )
			index_words( standby_att );
		return;
	}

	////////// Look for a SUMMARY attribute ///////////////////////////////

	if ( tag_cmp( name, "table" ) ) {
		encoded_char_range summary_att = name;
		if ( find_attribute( summary_att, "summary" ) )
			index_words( summary_att );
		return;
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ void HTML_indexer::new_file()
//
// DESCRIPTION
//
//	Clear the element stack prior to indexing a new file.  This function is
//	out-of-line due to being virtual.
//
//*****************************************************************************
{
	element_stack_.clear();
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool skip_html_tag( register encoded_char_range::const_iterator &c )
//
// DESCRIPTION
//
//	Scan for the closing '>' of an HTML or XHTML tag skipping all
//	characters until it's found.  It takes care to ignore any '>' inside
//	either single or double quotation marks.  It properly handles HTML and
//	XHTML comments.
//
// PARAMETERS
//
//	c	The iterator to use.  It is presumed to start at any position
//		after the '<' and before the '>'; it is left at the first
//		character after the '>'.
//
// RETURN VALUE
//
//	Returns true only if the tag is an HTML or XHTML comment.
//
//*****************************************************************************
{
	if ( is_html_comment( c ) )
		return true;

	register char quote = '\0';
	while ( !c.at_end() ) {
		if ( quote ) {			// ignore everything...
			if ( *c++ == quote )	// ...until matching quote
				quote = '\0';
			continue;
		}
		if ( *c == '\"' || *c == '\'' ) {
			quote = *c++;		// start ignoring stuff
			continue;
		}
		if ( *c++ == '>' )		// found it  :)
			break;
	}

	return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool tag_cmp(
		encoded_char_range::const_iterator &c,
		register char const *tag
	)
//
// DESCRIPTION
//
//	Compares the tag name starting at the given iterator to the given
//	string.  Case is irrelevant.
//
// PARAMETERS
//
//	c	The iterator to use.  It is presumed to be positioned at the
//		first character after the '<'.  If the tag name matches, it is
//		repositioned at the first character past the name; otherwise,
//		it is not touched.
//
//	tag	The string to compare against; it must be in lower case.
//
// RETURN VALUE
//
//	Returns true only if the tag matches.
//
//*****************************************************************************
{
	encoded_char_range::const_iterator d = c;
	while ( *tag && !d.at_end() && *tag++ == to_lower( *d++ ) ) ;
	if ( *tag )
		return false;
	c = d;
	return true;
}

#endif	/* MOD_HTML */
