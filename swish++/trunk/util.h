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
#include <cerrno>
#include <climits>
#include <cstring>
#include <iostream>
#include <string>
#include <ctime>			/* needed by sys/resource.h */
#include <sys/time.h>			/* needed by FreeBSD systems */
#include <sys/types.h>			/* needed by FreeBSD systems */
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>			/* for _exit(2), geteuid(2) */

//
// POSIX.1 is, IMHO, brain-damaged in the way it makes you determine the
// maximum path-name length, so we'll simply pick a sufficiently large constant
// such as 1024.  In practice, this is the actual value used on many SVR4 as
// well as 4.3+BSD systems.
//
// See also: W. Richard Stevens.  "Advanced Programming in the Unix
// Environment," Addison-Wesley, Reading, MA, 1993.  pp. 34-42.
//
#ifdef	PATH_MAX
#undef	PATH_MAX
#endif
int const		PATH_MAX = 1024;

// local
#include "exit_codes.h"
#include "fake_ansi.h"			/* for std */
#include "platform.h"			/* for PJL_NO_SYMBOLIC_LINKS */

extern char const*	me;

//*****************************************************************************
//
// SYNOPSIS
//
	template< int Buf_Size, int N > class char_buffer_pool
//
// DESCRIPTION
//
//	A char_buffer_pool maintains a small set ("pool") of size N of
//	available character buffers, each of size Buf_Size, and issues them in
//	a round-robin manner.
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
	char	buf_[ N ][ Buf_Size ];
	int	next_buf_index_;
	char	*cur_buf_;
};

//*****************************************************************************
//
// SYNOPSIS
//
	inline char const* find_newline( char const *c, char const *end )
//
// DESCRIPTION
//
//	Finds the next newline (CR or LF) in a character sequence.
//
// PARAMETERS
//
//	c	The iterator to use.
//
//	end	The iterator marking the end of the range to check.
//
// RETURN VALUE
//
//	Returns an iterator positioned either at the first character of a CR-LF
//	if found, or at "end" if not.
//
//*****************************************************************************
{
	while ( c != end && *c != '\n' && *c != '\r' )
		++c;
	return c;
}

//*****************************************************************************
//
// SYNOPSIS
//
	template< class T > void max_out_limit( T resource )
//
// DESCRIPTION
//
//	Set the limit for the given resource to its maximum value.  If we're
//	running as root, set it to infinity.
//
// PARAMETERS
//
//	resource	The ID for the resource as given in sys/resources.h.
//
// NOTE
//
//	This can't be an ordinary function since the type "resource" isn't int
//	on some systems (e.g., Linux).
//
// SEE ALSO
//
//	W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
//	Addison-Wesley, Reading, MA, 1993. pp. 180-184.
//
//*****************************************************************************
{
	struct rlimit r;
#ifdef	RLIM_INFINITY	/* hey, you never know: it might be undefined */
	if ( ::geteuid() == 0 )
		r.rlim_max = RLIM_INFINITY;
	else
#endif
		::getrlimit( resource, &r );
	r.rlim_cur = r.rlim_max;
	::setrlimit( resource, &r );
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline char const* pjl_basename( char const *file_name )
//
// DESCRIPTION
//
//	Determine the base name of a given file name.
//
// PARAMETERS
//
//	file_name	The file_name.
//
// RETURN VALUE
//
//	Returns a pointer to the base name of the file name.  Note that the
//	pointer points within file_name, i.e., the two will share storage.
//
//*****************************************************************************
{
	char const *const slash = ::strrchr( file_name, '/' );
	return slash ? slash + 1 : file_name;
}

//*****************************************************************************
//
// SYNOPSIS
//
	inline char const* skip_newline( char const *c, char const *end )
//
// DESCRIPTION
//
//	Skips the next newline (CR and/or LF) in a character sequence.
//
// PARAMETERS
//
//	c	The iterator positioned at either a CR or LF.
//
//	end	The iterator marking the end of the range.
//
// RETURN VALUE
//
//	Returns an iterator positioned either at a character after a newline if
//	found, or untouched if not.
//
//*****************************************************************************
{
	if ( c != end && *c == '\r' )
		++c;
	if ( c != end && *c == '\n' )
		++c;
	return c;
}

//*****************************************************************************
//
//	File test functions.  Those that do not take an argument operate on the
//	last file stat'ed.
//
//*****************************************************************************

extern struct stat	stat_buf;		// someplace to do a stat(2) in

inline bool	file_exists( char const *path ) {
			return ::stat( path, &stat_buf ) != -1;
		}
inline bool	file_exists( std::string const &path ) {
			return file_exists( path.c_str() );
		}

inline off_t	file_size() { return stat_buf.st_size; }

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
			return S_ISREG( stat_buf.st_mode );
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

inline int		pjl_abs( int n ) { return n >= 0 ? n : -n; }

inline std::ostream&	error( std::ostream &o = std::cerr ) {
				return o << me << ": error: ";
			}
inline std::ostream&	error_string( std::ostream &o = std::cerr ) {
				return o << ": " << std::strerror( errno )
					 << std::endl;
			}

#define	internal_error	std::cerr << (me ? me : "SWISH++") << ", \"" \
			<< __FILE__ << "\", line " << __LINE__ \
			<< ": internal error: "

#define	NUM_ELEMENTS(a)	(sizeof (a) / sizeof( (a)[0] ))

inline std::ostream&	report_error( std::ostream &o = std::cerr ) {
				o << "; please report this error" << std::endl;
				::_exit( Exit_Internal_Error );
				return o;	// just to make compiler happy
			}

inline char*		new_strdup( char const *s ) {
				return std::strcpy(
					new char[ std::strlen( s ) + 1 ], s
				);
			}

		// ensure function semantics: 'c' is expanded once
inline bool	is_alnum( char c ) {
			return isalnum( static_cast<unsigned char>( c ) );
		}
inline bool	is_alpha( char c ) {
			return isalpha( static_cast<unsigned char>( c ) );
		}
inline bool	is_digit( char c ) {
			return isdigit( static_cast<unsigned char>( c ) );
		}
inline bool	is_punct( char c ) {
			return ispunct( static_cast<unsigned char>( c ) );
		}
inline bool	is_space( char c ) {
			return isspace( static_cast<unsigned char>( c ) );
		}
inline bool	is_upper( char c ) {
			return isupper( static_cast<unsigned char>( c ) );
		}
inline bool	is_xdigit( char c ) {
			return isxdigit( static_cast<unsigned char>( c ) );
		}

inline char	to_lower( char c ) { return tolower( c ); }
extern char*	to_lower( char const* );
extern char*	to_lower_r( char const* );
extern char*	to_lower( char const *begin, char const *end );
extern char*	to_lower_r( char const *begin, char const *end );

#define	FOR_EACH(T,C,I) \
	for ( T::const_iterator I = (C).begin(); I != (C).end(); ++I )

#define	FOR_EACH_IN_PAIR(T,P,I) \
	for ( T::const_iterator I = (P).first; I != (P).second; ++I )

#define	TRANSFORM_EACH(T,C,I) \
	for ( T::iterator I = (C).begin(); I != (C).end(); ++I )

#endif	/* util_H */
/* vim:set noet sw=8 ts=8: */
