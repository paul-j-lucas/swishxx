/*
**	SWISH++
**	config.h
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

#ifndef	config_H
#define	config_H

////////// Word determination /////////////////////////////////////////////////

int const	Word_Hard_Min_Size		= 3;
int const	Word_Hard_Max_Size		= 25;
//		The minimum and maximum lengths a word must be in order even
//		to bother doing more aggressive checks on to determine if it
//		should be indexed.

int const	Word_Min_Size			= 4;
//		The minimum length a non-acronym word must be in order to be
//		considered for indexing.

int const	Word_Min_Vowels			= 1;
//		The minimum number of vowels a word must have in order to be
//		indexed.

int const	Word_Hex_Max_Size		= 4;
//		The maximum length a string composed entirely of hexadecimal
//		digits i.e., ASCII hex data, can be before it is discarded.
//		Note that the word "cafe" is a legitimate English word
//		composed entirely of hexedecimal digits.  This parameter is
//		used only by extract(1) in extract.c.

// I don't think there is a word in English that has more than...

int const	Word_Max_Consec_Consonants	= 5;
//		...5 consecutive consonants

int const	Word_Max_Consec_Vowels		= 4;
//		...4 consecutive vowels (like "queue")

int const	Word_Max_Consec_Same		= 2;
//		...2 of the same alphabetic character consecutively

int const	Word_Max_Consec_Puncts		= 1;
//		...1 punctuation character in a row

// Characters that are permissible in words: letters must be lower case and
// upper case letters would be redundant.
//
char const	Word_Chars[] = "&'-0123456789abcdefghijklmnopqrstuvwxyz_";
		// Characters that may be in a word.  Note that '&' is here so
		// acronyms like "AT&T" are treated as one word.  Unlike
		// SWISH-E, ';' does not need to be here to recognize and
		// convert character entity references.

#define		OPTIMIZE_WORD_CHARS 1
		// If you are using the default set of characters, that is the
		// alphanumerics and "&'-_" characters, then having this macro
		// set to 1 will optimize the is_word_char() function yielding
		// about a 10% performance improvement; alternatively, you can
		// also edit that function to keep the optimization if you are
		// not using the default set of characters.  See util.h for
		// details.

char const	Word_Begin_Chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
		// Characters that may begin a word; should be a subset of the
		// above.

#define		OPTIMIZE_WORD_BEGIN_CHARS 1
		// Same deal as with OPTIMIZE_WORD_CHARS.

char const	Word_End_Chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
		// Characters that may end a word; usually the same as the
		// above.

#define		OPTIMIZE_WORD_END_CHARS 1
		// Same deal as with OPTIMIZE_WORD_CHARS.

////////// HTML file parameters ///////////////////////////////////////////////

int const	Title_Lines_Default		= 12;
//		Specifies the maximum number of lines into a file to look at
//		for HTML <TITLE> tags; this can be overridden on the command
//		line.

int const	Title_Max_Size			= 200;
//		Maximum length of a title.

////////// Miscellaneous parameters ///////////////////////////////////////////

char const	Config_Filename_Default[]	= "swish++.conf";
//		Default name of the configuration file; this can be
//		overridden on the command line.

int const	Entity_Max_Size			= 7;
//		The maximum size of an entity reference, e.g., "&eacute;" NOT
//		counting the leading '&' or the trailing ';'.  You should have
//		no reason to change this.

int const	Files_Reserve_Default		= 1000;
//		Default maximum number of files to reserve space for; see
//		file_info::operator new() in file_info.c for details.  This can
//		be overridden on the command line.

int const	Fork_Attempts			= 5;
//		Number of times to try to fork before giving up.  This
//		parameter is used only by extract(1) in filter.c.

int const	Fork_Sleep			= 5;
//		Number of seconds to sleep before retrying to fork.  This
//		parameter is used only by extract(1) in filter.c.

char const	Index_Filename_Default[]	= "swish++.index";
//		Default name of the index file generated/searched; can be
//		overridden on the command line.

int const	Results_Max_Default		= 100;
//		Default maximum number of search results; this can be
//		overridden on the command line.

char const	Temp_Directory_Default[]	= "/tmp";
//		Default directory to use for temporary files during indexing.
//		If your OS mounts swap space on /tmp, as indexing progresses
//		and more files get created in /tmp, you will have less swap
//		space, indexing will get slower, and you may run out of memory.
//		If this is the case, you can either change this default here
//		for all users or override it on the command line to use a
//		directory on a real filesystem, i.e., one on a physical disk.
//		The directory must exist.

int const	Word_Threshold			= 250000;
//		The word count past which partial indicies are generated and
//		merged since the words are too big to fit into memory.  If
//		you index and your machine begins to swap like mad, lower
//		this value.  The above works OK in a 64MB machine.  A rule of
//		thumb is to add 250000 words for each additional 64MB of RAM
//		you have.

#endif	/* config_H */
