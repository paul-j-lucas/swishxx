/*
**      SWISH++
**      src/util.h
**
**      Copyright (C) 1998-2005  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef util_H
#define util_H

// local
#include "exit_codes.h"
#include "pjl/less.h"
#include "pjl/omanip.h"

// standard
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>                   /* needed by FreeBSD systems */
#include <sys/types.h>                  /* needed by FreeBSD systems */
#include <time.h>                       /* needed by sys/resource.h */
#include <unistd.h>                     /* for _exit(2), geteuid(2) */
#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////

//
// POSIX.1 is, IMHO, brain-damaged in the way it makes you determine the
// maximum path-name length, so we'll simply pick a sufficiently large constant
// such as 1024.  In practice, this is the actual value used on many SVR4 as
// well as 4.3+BSD systems.
//
// See also: W. Richard Stevens.  "Advanced Programming in the Unix
// Environment," Addison-Wesley, Reading, MA, 1993.  pp. 34-42.
//
#ifdef PATH_MAX
#undef PATH_MAX
#endif /* PATH_MAX */
int const PATH_MAX = 1024;

extern char const  *me;
extern struct stat  stat_buf;           // someplace to do a stat(2) in

///////////////////////////////////////////////////////////////////////////////

/**
 * Parses a C string for an unsigned integer.
 * If an unsigned integer can't be parsed, an error message is printed
 * and the program exits.
 *
 * @param s The C string to parse.
 * @param label If given, the error message will will include this.
 * @return Returns the parsed unsigned integer.
 */
unsigned atou( char const *s, char const *label = nullptr );

/**
 * Finds the next newline (CR or LF) in a character sequence.
 *
 * @param c The iterator to use.
 * @param end The iterator marking the end of the range to check.
 * @return Returns an iterator positioned either at the first character of a
 * CR-LF if found, or at "end" if not.
 */
inline char const* find_newline( char const *c, char const *end ) {
  while ( c != end && *c != '\n' && *c != '\r' )
    ++c;
  return c;
}

/**
 * Sets the limit for the given resource to its maximum value.  If we're
 * running as root, set it to infinity.
 *
 * See also:
 *    W. Richard Stevens.  "Advanced Programming in the Unix Environment,"
 *    Addison-Wesley, Reading, MA, 1993. pp. 180-184.
 *
 * @param resource The ID for the resource as given in \c sys/resources.h.
 */
template<typename T>
void max_out_limit( T resource ) {
  struct rlimit r;
#ifdef RLIM_INFINITY
  if ( ::geteuid() == 0 )
    r.rlim_max = RLIM_INFINITY;
  else
#endif /* RLIM_INFINITY */
    ::getrlimit( resource, &r );
  r.rlim_cur = r.rlim_max;
  ::setrlimit( resource, &r );
}

/**
 * Parses a string into a \c bool.
 *
 * @param s The string to parse. Valid values are:
 * 0, f, false, n, no, off, 1, on, t, true, y, yes.
 * @return Returns \c true and sets \a result only if \a s is valid.
 */
bool parse( char const *s, bool *result );

/**
 * Determines the base name of a given file name.
 *
 * @param file_name The file_name.
 * @return Returns a pointer to the base name of the file name.  Note that the
 * pointer points within \a file_name, i.e., the two will share storage.
 */
inline char const* pjl_basename( char const *file_name ) {
  char const *const slash = ::strrchr( file_name, '/' );
  return slash ? slash + 1 : file_name;
}

/**
 * Skips the next newline (CR and/or LF) in a character sequence.
 *
 * @param c The iterator positioned at either a CR or LF.
 * @param end The iterator marking the end of the range.
 * @return Returns an iterator positioned either at a character after a newline
 * if found, or untouched if not.
 */
inline char const* skip_newline( char const *c, char const *end ) {
  if ( c != end && *c == '\r' )
    ++c;
  if ( c != end && *c == '\n' )
    ++c;
  return c;
}

////////// set functions //////////////////////////////////////////////////////

/**
 * Less verbose way of determining whether a set contains a particlar element.
 *
 * @tparam T The element type.
 * @param s The set to check.
 * @param key The key to check for.
 * @return Returns \c true only if \a s contains \a key.
 */
template<typename T,typename K>
inline bool contains( std::set<T> const &s, K const &key ) {
  return s.find( key ) != s.end();
}

/**
 * Less verbose way of determining whether an unordered_set contains a
 * particlar element.
 *
 * @tparam T The element type.
 * @param s The set to check.
 * @param key The key to check for.
 * @return Returns \c true only if \a s contains \a key.
 */
template<typename T>
inline bool contains( std::unordered_set<T> const &s, T const &key ) {
  return s.find( key ) != s.end();
}

/**
 * Specialization for C strings.
 */
inline bool contains( std::set<char const*> const &s, char const *key ) {
  return s.find( key ) != s.end();
}

/**
 * Specialization for C strings.
 */
inline bool contains( std::unordered_set<char const*> const &s,
                      char const *key ) {
  return s.find( key ) != s.end();
}

//*****************************************************************************
//
//      File test functions.  Those that do not take an argument operate on the
//      last file stat'ed.
//
//*****************************************************************************

inline bool file_exists( char const *path ) {
  return ::stat( path, &stat_buf ) != -1;
}

inline bool file_exists( std::string const &path ) {
  return file_exists( path.c_str() );
}

inline off_t file_size() {
  return stat_buf.st_size;
}

inline bool is_directory() {
  return S_ISDIR( stat_buf.st_mode );
}

inline bool is_directory( char const *path ) {
  return file_exists( path ) && is_directory();
}

inline bool is_directory( std::string const &path ) {
  return is_directory( path.c_str() );
}

inline bool is_plain_file() {
  return S_ISREG( stat_buf.st_mode );
}

inline bool is_plain_file( char const *path ) {
  return file_exists( path ) && is_plain_file();
}

inline bool is_plain_file( std::string const &path ) {
  return is_plain_file( path.c_str() );
}

#ifndef PJL_NO_SYMBOLIC_LINKS
inline bool is_symbolic_link() {
  return S_ISLNK( stat_buf.st_mode & S_IFLNK );
}

inline bool is_symbolic_link( char const *path ) {
  return ::lstat( path, &stat_buf ) != -1 && is_symbolic_link();
}

inline bool is_symbolic_link( std::string const &path ) {
  return is_symbolic_link( path.c_str() );
}
#endif  /* PJL_NO_SYMBOLIC_LINKS */

//*****************************************************************************
//
//      Miscelleneous.
//
//*****************************************************************************

inline unsigned pjl_abs( int n ) {
  return static_cast<unsigned>( n >= 0 ? n : -n );
}

inline std::ostream& error( std::ostream &o = std::cerr ) {
  return o << me << ": error: ";
}

inline std::ostream& error_string( std::ostream &o, int err_code ) {
  return o << ": " << std::strerror( err_code ) << std::endl;
}

inline std::ostream& error_string( std::ostream &o = std::cerr ) {
  return error_string( o, errno );
}

inline PJL::omanip<int> error_string( int err_code ) {
  return PJL::omanip<int>( error_string, err_code );
}

#define internal_error \
  std::cerr << (me ? me : "SWISH++") << ", \"" \
  << __FILE__ << "\", line " << __LINE__ << ": internal error: "

#define NUM_ELEMENTS(a) (sizeof (a) / sizeof( (a)[0] ))

inline std::ostream& report_error( std::ostream &o = std::cerr ) {
  o << "; please report this error" << std::endl;
  ::_exit( Exit_Internal_Error );
  return o;                             // just to make compiler happy
}

inline std::ostream& assert_stream( std::ostream &o ) {
  if ( !o ) {
    error() << "writing index failed: " << error_string;
    ::exit( Exit_No_Write_Index );
  }
  return o;
}

inline char* new_strdup( char const *s ) {
  return std::strcpy( new char[ std::strlen( s ) + 1 ], s );
}

inline char to_lower( char c ) {
  return tolower( c );
}

/**
 * Converts a C string to lower case; the original string is untouched.  The
 * string returned is from an internal pool of string buffers.  The time you
 * get into trouble is if you hang on to more then Num_Buffers strings.  This
 * doesn't normally happen in practice, however.
 *
 * @param s The string to be converted.
 * @return Returns a pointer to the lower-case string.
 */
char* to_lower( char const* );

/**
 * Converts a C string to lower case; the original string is untouched.  The
 * string returned is from an internal pool of string buffers.  The time you
 * get into trouble is if you hang on to more then Num_Buffers strings.  This
 * doesn't normally happen in practice, however.
 *
 * @param begin The iterator positioned at the first character of the string.
 * @param end The iterator postitioned one past the last character of the
 * string.
 * @return Returns a pointer to the lower-case string.
 */
char* to_lower( char const *begin, char const *end );

/**
 * Converts a C string to lower case using the given buffer; the original
 * string is untouched.
 *
 * @param buf The buffer into which the lower case string is to be put.
 * @parma s The string to be converted.
 * @return Returns \a buf.
 */
char* to_lower( char *buf, char const *s );

/**
 * Converts a C string to lower case; the original string is untouched.  The
 * string returned is dynamically allocated so the caller is responsible for
 * deleting it.
 *
 * @param s The string to be converted.
 * @return Returns a pointer to the lower-case string.
 */
char* to_lower_r( char const* );

/**
 * Converts a C string to lower case; the original string is untouched.  The
 * string returned is dynamically allocated so the caller is responsible for
 * deleting it.
 *
 * @param begin The iterator positioned at the first character of the string.
 * @param end The iterator postitioned one past the last character of the
 * string.
 * @return Returns a pointer to the lower-case string.
 */
char* to_lower_r( char const *begin, char const *end );

#define FOR_EACH(C,I) \
  for ( auto &&I = (C).begin(); I != (C).end(); ++I )

#define FOR_EACH_IN_PAIR(P,I) \
  for ( auto I = (P).first; I != (P).second; ++I )

///////////////////////////////////////////////////////////////////////////////

#endif /* util_H */
/* vim:set et sw=2 ts=2: */
