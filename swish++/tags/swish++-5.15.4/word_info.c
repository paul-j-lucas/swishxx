/*
**	SWISH++
**	word_info.c
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

// local
#include "enc_int.h"
#include "word_info.h"
#include "word_markers.h"
#include "util.h"				/* for FOR_EACH */

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void word_info::file::write_meta_ids( ostream &o ) const
//
// DESCRIPTION
//
//	Write out the set of META IDs for a word in a given file.
//
// PARAMETERS
//
//	o	The ostream to write to.
//
//*****************************************************************************
{
	o << Meta_Name_List_Marker;
	FOR_EACH( meta_set, meta_ids_, meta_id )
		o << enc_int( *meta_id );
	o << Stop_Marker;
}
