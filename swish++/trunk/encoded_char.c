/*
**	SWISH++
**	encoded_char.c
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

#ifdef MOD_MAIL

// standard
#include <cctype>
#include <cstring>

// local
#include "encoded_char.h"
#include "file_vector.h"
#include "platform.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ encoded_char_range::value_type
	encoded_char_range::const_iterator::decode_base64(
		file_vector::const_iterator begin,
		file_vector::const_iterator &c,
		file_vector::const_iterator end
	)
//
// DESCRIPTION
//
//	Convert a base64-encoded character sequence to its single-character
//	equivalent.  Ideally, we want to decode only a single character, but
//	Base64 encoding forces us to decode 3 characters at a time because they
//	are encoded as a unit into 4 bytes.  This makes this code a major pain
//	and slow because characters have to be able to be decoded with random
//	access, i.e., wherever the iterator is positioned.
//
//	An approach other than the one implemented here would have been to
//	decode the entire range into a buffer in one shot, but this could use a
//	lot of memory if the range is large (and the indexer already uses lots
//	of memory).  Additionally, mapping the iterator position from encoded
//	space to decoded space would have been tricky and just as much of a
//	pain.
//
//	Anywhere a space is returned it's because we've encountered an error
//	condition and the function has to return "something" and a space is
//	innocuous.
//
// SEE ALSO
//
//	Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose
//	Internet Mail Extensions (MIME) Part One: Format of Internet Message
//	Bodies," Section 6.8, "Base64 Content-Transfer-Encoding," RFC 822
//	Extensions Working Group of the Internet Engineering Task Force,
//	November 1996.
//
// NOTE
//
//	This code is based on the decode_base64() function as part of "encdec
//	1.1" by Jörgen Hägg <jh@efd.lth.se>, 1993.
//
//*****************************************************************************
{
	int const Bits_Per_Char = 6;	// by definition of Base64 encoding

	static value_type buf[ 3 ];	// group-of-4 decodes to 3 chars
	static file_vector::const_iterator prev_c;

	//
	// See if the iterator is less than a buffer's-worth away from the
	// previous iterator: if so, simply return the already-decoded
	// character.
	//
	file_vector::difference_type delta = c - prev_c;
	if ( delta >= 0 && delta < sizeof buf ) {
		//
		// We advance the iterator 1 position for the first 2
		// characters but 2 positions for the 3rd since we have to skip
		// over the 4th character used in the encoded versions of the
		// characters.
		//
		c += 1 + (delta == 2);
		return buf[ delta ];
	}

	//
	// If we're positioned at a newline, skip over it.
	//
	register file_vector::const_iterator line_begin = c;
	if ( (line_begin = skip_newline( c, end )) == end ) {
		c = end;
		return ' ';
	}

	if ( line_begin == c && line_begin > begin ) {
		//
		// Both line_begin hasn't moved (meaning we didn't just skip
		// over a newline) and we're not at the beginning of the
		// encoded char range: we need to "sync" by finding the
		// beginning of the line to know where the groups-of-4 encoded
		// characters start.
		//
		while ( line_begin > begin && !isspace( *line_begin ) )
			--line_begin;
		if ( line_begin > begin )
			++line_begin;
	} else {
		// Either line_begin moved or we're at "begin": in either case,
		// we're at the beginning of a line.  Just skip "c" over the
		// newline also.
		//
		c = line_begin;
	}

	//
	// Calculate where the start of the group-of-4 encoded characters is.
	//
	delta = c - line_begin;
	file_vector::difference_type const delta4 = delta & ~3u;
	file_vector::const_iterator  const group  = line_begin + delta4;

	if ( group + 1 == end || group + 2 == end || group + 3 == end ) {
		//
		// Well-formed Base64-encoded text should always been in groups
		// of 4 characters.  This text isn't: stop.
		//
		c = end;
		return ' ';
	}

	//
	// Determine the number of characters actually encoded into the 4 by
	// looking for padding characters ('=').
	//
	int const num_chars = group[2] == '=' ? 1 : group[3] == '=' ? 2 : 3;

	//
	// Calculate a combined value of the 4 encoded 6-bit characters.
	//
	register unsigned value = 0;
	register int i;
	for ( i = 0; i <= num_chars; ++i ) {
		static char const alphabet[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/=";
		//
		// Find the character in the Base64 alphabet.
		//
		char const *const a = ::strchr( alphabet, c[i] );
		if ( a )
			value += (a - alphabet) << ((3 - i) * Bits_Per_Char);
		else {
			// From RFC 2045, section 6.8:
			//
			//	Any characters outside of the base64 alphabet
			//	are to be ignored in base64-encoded data.
			//
			/* do nothing */;
		}
	}

	//
	// Now that we have a combined value, break it back apart but in 8-bit
	// chunks, i.e., ordinary characters.
	//
	for ( i = 2; i >= 0; --i ) {
		buf[ i ] = value & 255;
		value >>= 8;
	}

	//
	// Pretend to have decoded a single character and that it took only a
	// single byte to do it.  Additionally, remember the position of the
	// iterator marking the beginning of the range of characters that have
	// been decoded.  If we subsequently are asked to decode a character in
	// the range [i,i+3), we can simply return the character.
	//
	prev_c = c;
	delta -= delta4;
	c += 1 + (delta == 2);
	return buf[ delta ];
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ encoded_char_range::value_type
	encoded_char_range::const_iterator::decode_quoted_printable_aux(
		file_vector::const_iterator &c,
		file_vector::const_iterator end
	)
//
// DESCRIPTION
//
//	Convert a quoted-printable character sequence to its single-character
//	equivalent.  However, if it's just a "soft line break," return the
//	character after it, i.e., make it seem as if the soft line break were
//	not there.
//
//	Anywhere a space is returned it's because we've encountered an error
//	condition and the function has to return "something" and a space is
//	innocuous.
//
// SEE ALSO
//
//	Ned Freed and Nathaniel S. Borenstein.  "RFC 2045: Multipurpose
//	Internet Mail Extensions (MIME) Part One: Format of Internet Message
//	Bodies," Section 6.7, "Quoted-Printable Content-Transfer-Encoding," RFC
//	822 Extensions Working Group of the Internet Engineering Task Force,
//	November 1996.
//
//*****************************************************************************
{
	value_type h1;
	while ( 1 ) {
		h1 = *c++;
		if ( h1 == '\r' ) {
			//
			// The '=' was the last character on a line so this is
			// supposed to be a "soft line break": we therefore
			// have to skip over it entirely making things appear
			// as though it's not even there by returning the
			// character after the break.
			//
			if ( c == end  || *c == '\n' && ++c == end )
				return ' ';
			if ( *c != '=' )
				return *c++;
			//
			// The character after the soft line break just so
			// happens to be another '=' so we have to start all
			// over again.
			//
			if ( ++c == end )
				return ' ';
			continue;
		}
		if ( h1 == '\n' ) {
			//
			// Although "soft line breaks" are supposed to be
			// represented by CR-LF pairs, we're being robust here
			// and allowing just an LF by itself.
			//
			if ( c == end )
				return ' ';
			if ( *c != '=' )
				return *c++;
			if ( ++c == end )
				return ' ';
			continue;
		}
		break;
	}
	if ( !isxdigit( h1 ) || c == end ) {
		//
		// If it's not a hexadecimal digit or it's the last character,
		// it's malformed.
		//
		return ' ';
	}
	value_type const h2 = *c++;
	if ( !isxdigit( h2 ) ) {
		//
		// This shouldn't happen in proper quoted-printable text.
		//
		return ' ';
	}

	return static_cast<value_type>(
		//
		// We're being robust by ensuring the hexadecimal characters
		// are upper case.
		//
		( isdigit( h1 ) ? h1 - '0' : toupper( h1 ) - 'A' + 10 ) << 4 |
		( isdigit( h2 ) ? h2 - '0' : toupper( h2 ) - 'A' + 10 )
	);
}
#endif	/* MOD_MAIL */
