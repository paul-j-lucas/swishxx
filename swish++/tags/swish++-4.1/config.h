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
//		The minimum and maximum lengths a word must be in order even to
//		bother doing more aggressive checks on to determine if it
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
//		Note that the word "cafe" is a legitimate English word composed
//		entirely of hexedecimal digits.  This parameter is used only by
//		extract(1) in extract.c.

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
//		Characters that may be in a word.  Note that '&' is here so
//		acronyms like "AT&T" are treated as one word.  Unlike SWISH-E,
//		';' does not need to be here to recognize and convert character
//		entity references.

#define		OPTIMIZE_WORD_CHARS		1
//		If you are using the default set of characters, that is the
//		alphanumerics and "&'-_" characters, then having this macro set
//		to 1 will optimize the is_word_char() function yielding about a
//		10% performance improvement; alternatively, you can also edit
//		that function to keep the optimization if you are not using the
//		default set of characters.  See word_util.h for details.

char const	Word_Begin_Chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
//		Characters that may begin a word; should be a subset of the
//		above.

#define		OPTIMIZE_WORD_BEGIN_CHARS	1
//		Same deal as with OPTIMIZE_WORD_CHARS.

char const	Word_End_Chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
//		Characters that may end a word; usually the same as the above.

#define		OPTIMIZE_WORD_END_CHARS		1
//		Same deal as with OPTIMIZE_WORD_CHARS.

////////// HTML parameters ////////////////////////////////////////////////////

int const	Entity_Max_Size			= 6;
//		The maximum size of an entity reference, e.g., "&eacute;" NOT
//		counting the leading '&' or the trailing ';'.  You should have
//		no reason to change this.

int const	Tag_Name_Max_Size		= 11;
//		The maximum size of an HTML tag name, e.g., "/BLOCKQUOTE".  You
//		might need to increase this if you are indexing HTML documents
//		that contain non-standard tags and at least one of them is
//		longer than the above.

int const	TitleLines_Default		= 12;
//		Specifies the maximum number of lines into a file to look at
//		for HTML <TITLE> tags; this can be overridden either in a
//		config. file or on the command line.

int const	Title_Max_Size			= 200;
//		Maximum length of a title between <TITLE>...</TITLE>.

#ifdef	SEARCH_DAEMON
////////// Search server daemon parameters ////////////////////////////////////

char const	SocketFile_Default[]		= "/tmp/swish++.socket";
//		Default name of the Unix domain socket file; this can be
//		overridden either in a config. file or on the command line.

int const	SocketQueueSize_Default		= 100;
//		Maximum number of queued connections for a socket.  From
//		[Stevens 1998], p. 96:
//
//			Historically, sample code always shows a backlog of 5,
//			as that was the maximum value supported by 4.2BSD.
//			This was adequate in the 1980s when busy servers would
//			handle only a few hundred connections per day.  But
//			with the growth of the World Wide Web (WWW), where busy
//			servers handle millions of connections per day, this
//			small number is completely inadequate.  Busy HTTP
//			servers must specify a much larger backlog, and newer
//			kernels must support larger values.
//
//		This can be overridden either in a config. file or on the
//		command line.

int const	SocketTimeout_Default		= 10; // seconds
//		The number of seconds a client has to complete a search request
//		before being disconnected.  This is to prevent a client from
//		connecting, not completing a request, and causing the thread
//		servicing the request to wait forever.

int const	ThreadsMin_Default		= 5;
//		The minimum number of simultanous threads; this can be
//		overridden either in a config. file or on the command line.

int const	ThreadsMax_Default		= 100;
//		The maximum number of simultanous threads; this can be
//		overridden either in a config. file or on the command line.

int const	ThreadTimeout_Default		= 30; // seconds
//		The number of seconds until an idle spare thread times out and
//		destroys itself.
#endif

////////// Miscellaneous parameters ///////////////////////////////////////////

char const	ConfigFile_Default[]		= "swish++.conf";
//		Default name of the configuration file; this can be overridden
//		on the command line.

int const	FilesReserve_Default		= 1000;
//		Default maximum number of files to reserve space for; see
//		file_info::operator new() in file_info.c for details.  This can
//		be overridden either in a config. file or on the command line.

int const	Fork_Attempts			= 5;
//		Number of times to try to fork before giving up.  This
//		parameter is used only in filter.c.

int const	Fork_Sleep			= 5;
//		Number of seconds to sleep before retrying to fork.  This
//		parameter is used only by extract(1) in filter.c.

char const	IndexFile_Default[]		= "swish++.index";
//		Default name of the index file generated/searched; can be
//		overridden either in a config. file or on the command line.

int const	ResultsMax_Default		= 100;
//		Default maximum number of search results; this can be
//		overridden either in a config. file or on the command line.

#error		You have not set TempDirectory_Default for your system.
#error		Delete these lines after you have set it.

char const	TempDirectory_Default[]		= "/tmp";
//		Default directory to use for temporary files during indexing.
//		If your OS mounts swap space on /tmp, as indexing progresses
//		and more files get created in /tmp, you will have less swap
//		space, indexing will get slower, and you may run out of memory.
//		If this is the case, you can either change this default here
//		for all users (preferred) or override it either in a config.
//		file or on the command line to use a directory on a real
//		filesystem, i.e., one on a physical disk, e.g., /var/tmp on
//		some OSs.  The directory must exist.

int const	WordPercentMax_Default		= 100;
//		Default maximum percentage of files a word may occur in before
//		it is discarded as being too frequent; this can be overridden
//		either in a config. file or on the command line.

#error		You have not set Word_Threshold for your system.
#error		Delete these lines after you have set it.

int const	Word_Threshold			= 250000;
//		The word count past which partial indicies are generated and
//		merged since all the words are too big to fit into memory at
//		the same time.  If you index and your machine begins to swap
//		like mad, lower this value.  The above works OK in a 64MB
//		machine.  A rule of thumb is to add 250000 words for each
//		additional 64MB of RAM you have.  These numbers are for a SPARC
//		machine running Solaris.  Other machines running other
//		operating systems use memory differently.  You simply have to
//		experiment.

#endif	/* config_H */
