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
#include <string>
#include <sys/stat.h>

// local
#include "config.h"
#include "file_vector.h"
#include "platform.h"				/* for PJL_NO_SYMBOLIC_LINKS */

extern struct stat	stat_buf;		// somplace to do a stat(2) in

//*****************************************************************************
//
// SYNOPSIS
//
	template< int Size, int N > class char_buffer_pool
//
// DESCRIPTION
//
//	A char_buffer_pool maintains a small set ("pool") of size N of
//	available character buffers, each of size Size, and issues them in a
//	round-robin manner.
//
//	This is used by functions to return a character string without having
//	to allocate memory dynamically nor have previously returned strings
//	overwritten.
//
//*****************************************************************************
{
public:
	char_buffer_pool() : next_buf_index_( 0 ), cur_buf_( buf_[ 0 ] ) { }

	char*	current() const { return cur_buf_; }
	char*	next() {
			cur_buf_ = buf_[ next_buf_index_ ];
			next_buf_index_ = (next_buf_index_ + 1) % N;
			return cur_buf_;
		}
private:
	char	buf_[ N ][ Size ];
	int	next_buf_index_;
	char	*cur_buf_;
};

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_html_ext( char const *ext )
//
// DESCRIPTION
//
//	Checks the given filename extension to see if it is one used for HTML
//	files: html, htm, or shtml.  Case is significant.
//
// PARAMETERS
//
//	ext	The filename extension to be checked.
//
// RETURN VALUE
//
//	Returns true only if the filename extension is one used for HTML
//	files.
//
//*****************************************************************************
{
	int const i = ext[0] == 's';		// This is faster than calling
	return	 ext[ i ] == 'h' &&		// strcmp() multiple times.
		 ext[i+1] == 't' &&
		 ext[i+2] == 'm' &&
		(ext[i+3] == 'l' && !ext[i+4] ||
		!ext[i+3] && !i);
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_vowel( char c )
//
// DESCRIPTION
//
//	Determine whether a character is a lower-case vowel [aeiou].
//
// PARAMETERS
//
//	c	The character to be checked.
//
// RETURN VALUE
//
//	Returns true only if the character is a vowel.
//
//*****************************************************************************
{
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_word_char( char c )
//
// DESCRIPTION
//
//	Check whether a given character is a "word character," one that is
//	valid to be in a word.
//
// PARAMETERS
//
//	c	The character to be checked.
//
// RETURN VALUE
//
//	Returns true only if the character is a "word character."
//
//*****************************************************************************
{
	return c > 0 &&
#	if OPTIMIZE_WORD_CHARS
	( isalnum( c ) ||
		//
		// If you change Word_Chars in config.h from the default set
		// but would like to keep the optimization, edit the line below
		// to compare 'c' against every non-alphanumeric character in
		// your set of Word_Chars.
		//
		c == '&' || c == '\'' || c == '-' || c == '_'
	);
#	else
	std::strchr( Word_Chars, tolower( c ) ) != 0;
#	endif
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_word_begin_char( char c )
//
// DESCRIPTION
//
//	Check whether a given character is a "word beginning character," one
//	that is valid to begin a word.
//
// PARAMETERS
//
//	c	The character to be checked.
//
// RETURN VALUE
//
//	Returns true only if the character is a "word beginning character."
//
//*****************************************************************************
{
#	if OPTIMIZE_WORD_BEGIN_CHARS
	//
	// If you change Word_Begin_Chars in config.h from the default set but
	// would like to keep the optimization, edit the line below to compare
	// 'c' against every character in your set of Word_Begin_Chars.
	//
	return isalnum( c );
#	else
	return std::strchr( Word_Begin_Chars, tolower( c ) ) != 0;
#	endif
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline bool is_word_end_char( char c )
//
// DESCRIPTION
//
//	Check whether a given character is a "word ending character," one
//	that is valid to end a word.
//
// RETURN VALUE
//
//	Returns true only if the character is a "word ending character."
//
//*****************************************************************************
{
#	if OPTIMIZE_WORD_END_CHARS
	//
	// Same deal as with OPTIMIZE_WORD_BEGIN_CHARS.
	//
	return isalnum( c );
#	else
	return std::strchr( Word_End_Chars, tolower( c ) ) != 0;
#	endif
}

//*****************************************************************************
//
//	File test functions.  Those that do not take an argument operate on
//	the last file stat'ed.
//
//*****************************************************************************

inline bool	file_exists( char const *path ) {
			return ::stat( path, &stat_buf ) != -1;
		}
inline bool	file_exists( std::string const &path ) {
			return file_exists( path.c_str() );
		}

inline bool	is_directory() {
			return S_ISDIR( stat_buf.st_mode );
		}
inline bool	is_directory( char const *path ) {
			return file_exists( path ) && is_directory();
		}
inline bool	is_directory( std::string const &path ) {
			return is_directory( path.c_str() );
		}

inline bool	is_plain_file() {
			return S_ISREG( stat_buf.st_mode & S_IFMT );
		}
inline bool	is_plain_file( char const *path ) {
			return file_exists( path ) && is_plain_file();
		}
inline bool	is_plain_file( std::string const &path ) {
			return is_plain_file( path.c_str() );
		}

#ifndef	PJL_NO_SYMBOLIC_LINKS
inline bool	is_symbolic_link() {
			return S_ISLNK( stat_buf.st_mode & S_IFLNK );
		}
inline bool	is_symbolic_link( char const *path ) {
			return	::lstat( path, &stat_buf ) != -1
				&& is_symbolic_link();
		}
inline bool	is_symbolic_link( std::string const &path ) {
			return is_symbolic_link( path.c_str() );
		}
#endif	/* PJL_NO_SYMBOLIC_LINKS */

//*****************************************************************************
//
//	Miscelleneous.
//
//*****************************************************************************

#define	ERROR		cerr << me << ": error: "

extern void		get_index_info(
				file_vector<char> const &file, int i,
				long *n, off_t const **offset
			);

extern bool		is_ok_word( char const *word );

inline char*		new_strdup( char const *s ) {
				return ::strcpy( new char[ ::strlen(s)+1 ], s );
			}

extern void		parse_config_file( char const *file_name );

			// ensure function semantics: 'c' is expanded once
inline char		to_lower( char c )	{ return tolower( c ); }
extern char*		to_lower( char const *s );
extern char*		to_lower( char const *begin, char const *end );

extern char const*	ltoa( long );
inline char const*	itoa( int n )		{ return ::ltoa( n ); }

#define	FOR_EACH(T,C,I) \
	for ( T::const_iterator I = (C).begin(); I != (C).end(); ++I )

#define	TRANSFORM_EACH(T,C,I) \
	for ( T::iterator I = (C).begin(); I != (C).end(); ++I )

#endif	/* util_H */
