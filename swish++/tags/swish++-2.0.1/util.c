/*
**	SWISH++
**	util.c
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

// standard
#include <cctype>

// local
#include "config.h"
#include "fake_ansi.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void get_index_info(
		file_vector<char> const &file, int i,
		long *n, off_t const **offset
	)
//
// DESCRIPTION
//
//	Get the ith number/offset pair from an index file.
//
// SEE ALSO
//
//	index.c		write_full_index() -- format of index file
//
//*****************************************************************************
{
	file_vector<char>::const_iterator c = file.begin();
	long const *p = REINTERPRET_CAST( long const* )( c );
	*n = p[ 0 ];
	while ( i-- > 0 ) {
		c += sizeof( *n ) + *n * sizeof( off_t );
		p = REINTERPRET_CAST( long const* )( c );
		*n = p[ 0 ];
	}
	*offset = REINTERPRET_CAST( off_t const* )( &p[ 1 ] );
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool is_ok_word( char const *word )
//
// DESCRIPTION
//
//	Determine whether a given word should be indexed or not using several
//	heuristics.
//
//	First, a word is checked to see if it looks like an acronym.  A word
//	is considered an acronym only if it starts with a capital letter and
//	is composed exclusively of capital letters, digits, and punctuation
//	symbols, e.g., "AT&T."  If a word looks like an acronym, it is OK and
//	no further checks are done.
//
//	Second, there are several other checks that are applied.  A word is
//	not indexed if it:
//
//	1. Starts with a capital letter, is of mixed case, and contains more
//	   than a third capital letters, e.g., "BizZARE."
//
//	2. Contains a capital letter other than the first, e.g, "weIrd."
//
//	3. Is less that Word_Min_Size characters and is not an acronym.
//
//	4. Contains less than Word_Min_Vowels.
//
//	5. Contains more than Word_Max_Consec_Same of the same character
//	   consecutively (not including digits).
//
//	6. Contains more than Word_Max_Consec_Consonants consecutive
//	   consonants.
//
//	7. Contains more than Word_Max_Consec_Vowels consecutive vowels.
//
//	8. Contains more than Word_Max_Consec_Puncts consecutive punctuation
//	   characters.
//
// PARAMETERS
//
//	word	The word to be checked.
//
// RETURN VALUE
//
//	Returns true only if the word should be indexed.
//
// EXAMPLES
//
//	AT&T	OK
//	cccp	not OK -- no vowels
//	CCCP	OK -- acronym
//	eieio	not OK -- too many consec. vowels
//
//*****************************************************************************
{
	register char const *c;

#	ifdef DEBUG_is_ok_word
	cerr << '\t' << word << ' ';
#	endif

	////////// Survey the characters in the word //////////////////////////

	int digits = 0;
	int puncts = 0;
	int uppers = 0;
	int vowels = 0;
	for ( c = word; *c; ++c ) {
		if ( isdigit( *c ) ) {
			++digits;
			continue;
		};
		if ( ispunct( *c ) ) {
			++puncts;
			continue;
		}
		if ( isupper( *c ) )
			++uppers;
		if ( is_vowel( tolower( *c ) ) )
			++vowels;
	}
	int const len = c - word;

	if ( isupper( *word ) ) {		// starts with a capital letter
		if ( uppers + digits + puncts == len ) {
#			ifdef DEBUG_is_ok_word
			cerr << "(potential acronym)" << endl;
#			endif
			return true;
		}
		if ( ( uppers + digits ) * 100 / len > 33 ) {
#			ifdef DEBUG_is_ok_word
			cerr << "(too many intermediate uppers)" << endl;
#			endif
			return false;
		}
	} else if ( uppers ) {			// contains a capital letter
#		ifdef DEBUG_is_ok_word
		cerr << "(intermediate uppers)" << endl;
#		endif
		return false;
	}

	if ( len < Word_Min_Size ) {
#		ifdef DEBUG_is_ok_word
		cerr << "(len < Word_Min_Size)" << endl;
#		endif
		return false;
	}

	if ( vowels < Word_Min_Vowels ) {
#		ifdef DEBUG_is_ok_word
		cerr << "(vowels < Word_Min_Vowels)" << endl;
#		endif
		return false;
	}

	////////// Perform consecutive-character checks ///////////////////////

	int consec_consonants = 0;
	int consec_vowels = 0;
	int consec_same = 0;
	int consec_puncts = 0;
	register char last_c = '\0';

	for ( c = word; *c; ++c ) {

		if ( isdigit( *c ) ) {
			consec_consonants = 0;
			consec_vowels = 0;
			consec_puncts = 0;
			last_c = '\0';	// consec_same doesn't apply to digits
			continue;
		}

		if ( ispunct( *c ) ) {
			if ( ++consec_puncts > Word_Max_Consec_Puncts ) {
#				ifdef DEBUG_is_ok_word
				cerr << "(exceeded consec puncts)" << endl;
#				endif
				return false;
			}
			consec_consonants = 0;
			consec_vowels = 0;
			continue;
		}

		if ( *c == last_c ) {
			if ( ++consec_same > Word_Max_Consec_Same ) {
#				ifdef DEBUG_is_ok_word
				cerr << "(exceeded consec same)" << endl;
#				endif
				return false;
			}
		} else {
			consec_same = 0;
			last_c = *c;
		}

		if ( is_vowel( tolower( *c ) ) ) {
			if ( ++consec_vowels > Word_Max_Consec_Vowels ) {
#				ifdef DEBUG_is_ok_word
				cerr << "(exceeded consec vowels)" << endl;
#				endif
				return false;
			}
			consec_consonants = 0;
			consec_puncts = 0;
			continue;
		}

		if ( ++consec_consonants > Word_Max_Consec_Consonants ) {
#			ifdef DEBUG_is_ok_word
			cerr << "(exceeded consec consonants)" << endl;
#			endif
			return false;
		}
		consec_vowels = 0;
		consec_puncts = 0;
	}

#	ifdef DEBUG_is_ok_word
	cerr << endl;
#	endif
	return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
	char const *ltoa( register long n )
//
// DESCRIPTION
//
//	Convert a long integer to a string.  The string returned is from an
//	internal pool of string buffers.  The time you get into trouble is if
//	you hang on to more then Num_Buffers strings.  This doesn't normally
//	happen in practice, however.
//
// RETURN VALUE
//
//	A pointer to the string.
//
// SEE ALSO
//
//	Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language,
//	2nd ed."  Addison-Wesley, Reading, MA.  pp. 63-64.
//
//*****************************************************************************
{
	static char_buffer_pool<25,5> buf;
	register char	*s = buf.next();
	bool const	is_neg = n < 0;

	if ( is_neg ) n = -n;
	do {					// generate digits in reverse
		*s++ = n % 10 + '0';
	} while ( n /= 10 );
	if ( is_neg ) *s++ = '-';
	*s = '\0';

	// now reverse the string
	for ( register char *t = buf.current(); t < s; ++t ) {
		char const tmp = *--s; *s = *t; *t = tmp;
	}

	return buf.current();
}

//*****************************************************************************
//
// SYNOPSIS
//
	char *to_lower( register char const *s )
//
// DESCRIPTION
//
//	Return a pointer to a string converted to lower case; the original
//	string is untouched.  The string returned is from an internal pool of
//	string buffers.  The time you get into trouble is if you hang on to
//	more then Num_Buffers strings.  This doesn't normally happen in
//	practice, however.
//
// PARAMETERS
//
//	s	The string to be converted.
//
// RETURN VALUE
//
//	A pointer to the lower-case string.
//
//*****************************************************************************
{
	static char_buffer_pool<128,5> buf;
	register char *p = buf.next();

	while ( *p++ = to_lower( *s++ ) ) ;

	return buf.current();
}

//*****************************************************************************
//
// SYNOPSIS
//
	char *to_lower(
		register char const *begin, register char const *end
	)
//
// DESCRIPTION
//
//	Return a pointer to a string converted to lower case; the original
//	string is untouched.  The string returned is from an internal pool of
//	string buffers.  The time you get into trouble is if you hang on to
//	more then Num_Buffers strings.  This doesn't normally happen in
//	practice, however.
//
// PARAMETERS
//
//	begin	The iterator positioned at the first character of the string.
//
//	end	The iterator postitioned one past the last character of the
//		string.
//
// RETURN VALUE
//
//	A pointer to the lower-case string.
//
//*****************************************************************************
{
	static char_buffer_pool<128,5> buf;
	register char *p = buf.next();

	while ( begin != end )
		*p++ = to_lower( *begin++ );
	*p = '\0';

	return buf.current();
}
