/*
**	PJL C++ Library
**	auto_vec.h
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

#ifndef	auto_vec_H
#define	auto_vec_H

// local
#include "fake_ansi.h"			/* for explicit */

#ifndef	PJL_NO_NAMESPACES
namespace PJL {
#else
#define	PJL /* nothing */
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	template< class T > class auto_vec
//
// DESCRIPTION
//
//	A auto_vec<T> will delete the vector of objects it points to upon
//	destruction.  This is very similar to STL's auto_ptr<T>, but for
//	vectors.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA, 1997.  pp. 367-368.
//
//*****************************************************************************
{
public:
	explicit auto_vec( T *p = 0 ) : p_( p )		{ }
	~auto_vec()					{ delete[] p_; }

	auto_vec<T>& operator=( T *p ) {
		delete[] p_;
		p_ = p;
		return *this;
	}

	T&		operator*()			{ return *p_; }
	T const&	operator*() const		{ return *p_; }

	T&		operator[]( int i )		{ return p_[ i ]; }
	T const&	operator[]( int i ) const	{ return p_[ i ]; }

			operator T*()			{ return p_; }
			operator T const*() const	{ return p_; }
private:
	T *p_;

	auto_vec( auto_vec<T> const& );		// forbid copy
	auto_vec& operator=( auto_vec< T >& );	// forbid assignment
};

#ifndef	PJL_NO_NAMESPACES
}
#endif

#endif	/* auto_vec_H */
/* vim:set noet sw=8 ts=8: */