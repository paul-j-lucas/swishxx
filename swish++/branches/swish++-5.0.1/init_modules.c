/*
**	SWISH++
**	indexer_modules.c
**
**	Copyright (C) 2000  Paul J. Lucas
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
#ifdef	MOD_HTML
#include "mod_html.h"
#endif
#ifdef	MOD_MAIL
#include "mod_mail.h"
#endif
#ifdef	MOD_MAN
#include "mod_man.h"
#endif

#ifndef	PJL_NO_NAMESPACES
using namespace std;
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	/* static */ void indexer::init_modules()
//
// DESCRIPTION
//
//	This function is a place to bundle together the construction of the
//	singleton instances of indexer modules.  The base indexer() constructor
//	will add pointers to them into the static mod_name-to-instance map.
//
//*****************************************************************************
{
#ifdef	MOD_HTML
	static HTML_indexer	HTML;
#endif
#ifdef	MOD_MAIL
	static mail_indexer	mail;
#endif
#ifdef	MOD_MAN
	static man_indexer	man;
#endif
	// The plain text indexer is initialized in indexer::map_ref().
}
