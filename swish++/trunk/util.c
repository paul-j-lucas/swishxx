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
#include <cstring>

// local
#include "platform.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

char_buffer_pool<128,5>	lower_buf;
struct stat		stat_buf;	// someplace to do a stat(2) in

//*****************************************************************************
//
// SYNOPSIS
//
	char *to_lower( char *buf, char const *s )
//
// DESCRIPTION
//
//	Return a pointer to a string converted to lower case using the given
//	buffer; the original string is untouched.
//
// PARAMETERS
//
//	buf	The buffer into which the lower case string is to be put.
//
//	s	The string to be converted.
//
// RETURN
//
//	The buf pointer.
//
//*****************************************************************************
{
	register char *p = buf;
	while ( *p++ = to_lower( *s++ ) ) ;
	return buf;
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
	for ( register char *p = lower_buf.next(); *p++ = to_lower( *s++ ); ) ;
	return lower_buf.current();
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
	register char *p = lower_buf.next();
	while ( begin != end )
		*p++ = to_lower( *begin++ );
	*p = '\0';
	return lower_buf.current();
}

//*****************************************************************************
//
// SYNOPSIS
//
	char *to_lower_r( register char const *s )
//
// DESCRIPTION
//
//	Return a pointer to a string converted to lower case; the original
//	string is untouched.  The string returned is dynamically allocated so
//	the caller is responsible for deleting it.
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
	char *const buf = new char[ ::strlen( s ) + 1 ];
	for ( register char *p = buf; *p++ = to_lower( *s++ ); ) ;
	return buf;
}

//*****************************************************************************
//
// SYNOPSIS
//
	char *to_lower_r(
		register char const *begin, register char const *end
	)
//
// DESCRIPTION
//
//	Return a pointer to a string converted to lower case; the original
//	string is untouched.  The string returned is dynamically allocated so
//	the caller is responsible for deleting it.
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
	char *const buf = new char[ end - begin + 1 ];
	register char *p = buf;
	while ( begin != end )
		*p++ = to_lower( *begin++ );
	*p = '\0';
	return buf;
}
/* vim:set noet sw=8 ts=8: */
