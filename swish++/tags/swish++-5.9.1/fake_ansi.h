/*
**	PJL C++ Library
**	fake_ansi.h -- Fake some ANSI C++ comittee resolutions
**
**	Copyright (C) 2001  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the Licence, or
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

#ifndef	fake_ansi_H
#define	fake_ansi_H

// standard
#include <cstddef>			/* for ptrdiff_t */

// local
#include "platform.h"

////////// namespaces /////////////////////////////////////////////////////////

#ifdef	PJL_NO_NAMESPACES
#	define namespace		/* nothing */
#	define using			/* nothing */
#	define std			/* nothing */
#endif

//
// The HP STL headers are broken for g++ 2.95.x and earlier because
// stl_config.h incorrectly defines __STL_NO_NAMESPACES when it shouldn't
// (causing __STL_USE_NAMESPACES to be undefined) and stl_iterator.h defines
// "struct iterator" only when __STL_USE_NAMESPACES is defined.  This is fixed
// in 2.96, but for earlier versions we define std::iterator here ourselves:
// the definition is copied/pasted from stl_iterator.h.
//
#ifdef	PJL_GCC_295
namespace std {
	template <class _Category, class _Tp, class _Distance = ptrdiff_t,
		  class _Pointer = _Tp*, class _Reference = _Tp&>
	struct iterator {
		typedef _Category  iterator_category;
		typedef _Tp        value_type;
		typedef _Distance  difference_type;
		typedef _Pointer   pointer;
		typedef _Reference reference;
	};
}
#endif

////////// some new keywords //////////////////////////////////////////////////

#ifdef	PJL_NO_EXPLICIT
#	define explicit			/* nothing */
#endif

#endif	/* fake_ansi_H */
