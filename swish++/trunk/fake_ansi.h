/*
**	PJL C++ Library
**	fake_ansi.h -- Fake some ANSI C++ comittee resolutions
**
**	Copyright (C) 1998  Paul J. Lucas
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

#include "platform.h"

/* namespaces */

#ifdef	PJL_NO_NAMESPACES
#	define namespace		/* nothing */
#	define using			/* nothing */
#	define std			/* nothing */
#endif

/* some new keywords */

#ifdef	PJL_NO_EXPLICIT
#	define explicit			/* nothing */
#endif

#endif	/* fake_ansi_H */
