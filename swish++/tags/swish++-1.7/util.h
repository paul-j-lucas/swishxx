/*
**	SWISH++
**	util.h
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

#ifndef	util_H
#define	util_H

// standard
#include <cctype>
#include <cstring>

// local
#include "config.h"
#include "file_vector.h"

#define	FOR_EACH(T,C,I) \
	for ( T::const_iterator I = (C).begin(); I != (C).end(); ++I )

#define	TRANSFORM_EACH(T,C,I) \
	for ( T::iterator I = (C).begin(); I != (C).end(); ++I )

extern void	get_index_info(
			file_vector<char> const &file, int i,
			long *n, off_t const **offset
		);

inline bool	is_html_ext( char const *ext ) {
			//
			// This is faster than calling strcmp() multiple times.
			//
			int const i = ext[0] == 's';
			return	 ext[ i ] == 'h' &&
				 ext[i+1] == 't' &&
				 ext[i+2] == 'm' &&
				(ext[i+3] == 'l' && !ext[i+4] ||
				!ext[i+3] && !i);
		}

inline bool	is_word_char( char c ) {
			return c > 0 &&
#			if OPTIMIZE_WORD_CHARS
			( isalnum( c ) ||
			//
			// If you change Word_Chars in config.h from the
			// default set but would like to keep the optimization,
			// edit the line below to compare 'c' against every
			// non-alphanumeric character in your set of
			// Word_Chars.
			//
				c == '&' || c == '\'' || c == '-' || c == '_'
			);
#			else
			std::strchr( Word_Chars, tolower( c ) ) != 0;
#			endif
		}

inline bool	is_word_begin_char( char c ) {
#			if OPTIMIZE_WORD_BEGIN_CHARS
			//
			// If you change Word_Begin_Chars in config.h from the
			// default set but would like to keep the optimization,
			// edit the line below to compare 'c' against every
			// character in your set of Word_Begin_Chars.
			//
			return isalnum( c );
#			else
			return std::strchr( Word_Begin_Chars, tolower( c ) ) != 0;
#			endif
		}
inline bool	is_word_end_char( char c ) {
#			if OPTIMIZE_WORD_END_CHARS
			//
			// Same deal as with OPTIMIZE_WORD_BEGIN_CHARS.
			//
			return isalnum( c );
#			else
			return std::strchr( Word_End_Chars, tolower( c ) ) != 0;
#			endif
		}

extern bool	is_ok_word( char const *word );

			// ensure function semantics: 'c' is expanded once
inline char		to_lower( char c )	{ return tolower( c ); }
extern char*		to_lower( char const *s );
extern char*		to_lower( char const *begin, char const *end );
inline char		to_upper( char c )	{ return toupper( c ); }

extern char const*	ltoa( long );
inline char const*	itoa( int    n )	{ return ::ltoa( n ); }
inline char const*	ntoa( short  n )	{ return ::ltoa( n ); }
inline char const*	ntoa( int    n )	{ return ::ltoa( n ); }
inline char const*	ntoa( long   n )	{ return ::ltoa( n ); }

#endif	/* util_H */
