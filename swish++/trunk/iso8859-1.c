/*
**	SWISH++
**	iso8859-1.c
**
**	Copyright (C) 2002  Paul J. Lucas
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

//
// All characters are from the ISO 8859-1 character set mapped to 7-bit ASCII.
//
// This file has to be at the top-level rather than in the charsets
// subdirectory where it ought to be because "search" and "extract" use it
// stand-alone.
//
extern char const iso8859_1_map[] = {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  0
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', '!', '"', '#', '$', '%', '&', '\'',//  32
	'(', ')', '*', '+', ',', '-', '.', '/',	//  |
	'0', '1', '2', '3', '4', '5', '6', '7',	//  |
	'8', '9', ':', ';', '<', '=', '>', '?',	//  |
	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',	//  |
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',	//  |
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',	//  |
	'X', 'Y', 'Z', '[', '\\',']', '^', '_', //  |
	'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',	//  |
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',	//  |
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',	//  |
	'x', 'y', 'z', '{', '|', '}', '~', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	// 128
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  |
	' ', '!', ' ', '#', ' ', ' ', '|', ' ',	// 160
	' ', ' ', ' ', '"', ' ', '-', ' ', ' ',	//  |
	' ', ' ', '2', '3', '\'',' ', ' ', '.', //  |
	' ', '1', ' ', '"', ' ', ' ', ' ', '?',	//  |
	'A', 'A', 'A', 'A', 'A', 'A', 'E', 'C',	// 192
	'E', 'E', 'E', 'E', 'I', 'I', 'I', 'I',	//  |
	'D', 'N', 'O', 'O', 'O', 'O', 'O', ' ',	//  |
	'O', 'U', 'U', 'U', 'U', 'Y', ' ', 's',	//  |
	'a', 'a', 'a', 'a', 'a', 'a', 'e', 'c',	//  |
	'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i',	//  |
	'd', 'n', 'o', 'o', 'o', 'o', 'o', ' ',	//  |
	'o', 'u', 'u', 'u', 'u', 'y', ' ', 'y',	// 255
};
