/*
**	SWISH++
**	manager_ptr.h
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

#ifdef	SEARCH_DAEMON

#ifndef	managed_ptr_H
#define	managed_ptr_H

//*****************************************************************************
//
// SYNOPSIS
//
	template< class T > class managed_ptr
//
// DESCRIPTION
//
//	A managed_ptr<T> will delete the object it points to upon destruction.
//
//*****************************************************************************
{
public:
	managed_ptr( T *p ) : p_( p ) { }
	~managed_ptr() { delete p_; }

	operator T*() const { return p_; }
private:
	T *const p_;
};

//*****************************************************************************
//
// SYNOPSIS
//
	template< class T > class managed_vec
//
// DESCRIPTION
//
//	A managed_ptr<T> will delete the vector of objects it points to upon
//	destruction.
//
//*****************************************************************************
{
public:
	managed_vec( T *p ) : p_( p ) { }
	~managed_vec() { delete[] p_; }

	operator T*() const { return p_; }
private:
	T *const p_;
};

#endif	/* managed_ptr_H */

#endif	/* SEARCH_DAEMON */
