/*
**	SWISH++
**	conf_set.h
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

#ifndef conf_set_H
#define conf_set_H

// local
#include "conf_var.h"
#include "my_set.h"

//*****************************************************************************
//
// SYNOPSIS
//
	class conf_set : public conf_var, public string_set
//
// DESCRIPTION
//
//	A conf_set is-a conf_var and-a string_set for containing a set of
//	configuration variable values.
//
//*****************************************************************************
{
protected:
	conf_set( char const *name ) : conf_var( name ) { }
private:
	virtual void	parse_value( char const *var_name, char *line );
	virtual void	reset() { clear(); }
};

#endif	/* conf_set_H */
