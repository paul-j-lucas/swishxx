/*
**      SWISH++
**      src/swishxx-config.h
**
**      Copyright (C) 1998-2015  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef swishxx_config_H
#define swishxx_config_H

// local
#include "config.h"

////////// Word determination /////////////////////////////////////////////////

/**
 * The minimum length a word must be in order even to bother doing more
 * aggressive checks on it to determine if it should be indexed.
 */
int const   Word_Hard_Min_Size          = 3;

/**
 * The maximum length a word must be in order even to bother doing more
 * aggressive checks on it to determine if it should be indexed.
 */
int const   Word_Hard_Max_Size          = 25;

/**
 * The minimum length a non-acronym word must be in order to be considered for
 * indexing.
 */
int const   Word_Min_Size               = 4;

/**
 * The minimum number of vowels a word must have in order to be indexed.
 */
int const   Word_Min_Vowels             = 1;

/**
 * The maximum length a string composed entirely of hexadecimal digits i.e.,
 * ASCII hex data, can be before it is discarded.  Note that the word "cafe" is
 * a legitimate English word composed entirely of hexedecimal digits.  This
 * parameter is used only by \b extract(1) in \fc extract.cpp.
 */
int const   Word_Hex_Max_Size           = 4;

// I don't think there is a word in English that has more than...

int const   Word_Max_Consec_Consonants  = 7;
//          ...this many consecutive consonants (like "symphysis")

int const   Word_Max_Consec_Vowels      = 5;
//          ...this many consecutive vowels (like "queueing")

int const   Word_Max_Consec_Same        = 2;
//          ...this many of the same alphabetic character consecutively

int const   Word_Max_Consec_Puncts      = 1;
//          ...this many punctuation character in a row

/**
 * Characters that may be in a word.  Note that \c '&' is here so acronyms like
 * "AT&T" are treated as one word.  Unlike SWISH-E, \c ';' does not need to be
 * here to recognize and convert character entity references.
 */
char const  Word_Chars[] = "&'-0123456789abcdefghijklmnopqrstuvwxyz_";

/**
 * If you are using the default set of characters, that is the alphanumerics
 * and \c "&'-_" characters, then having this macro set to 1 will optimize the
 * \c is_word_char() function yielding about a 10% performance improvement;
 * alternatively, you can also edit that function to keep the optimization if
 * you are not using the default set of characters.  See \c word_util.h for
 * details.
 */
#define     OPTIMIZE_WORD_CHARS         1

/**
 * Characters that may begin a word; should be a subset of the above.
 */
char const  Word_Begin_Chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";

/**
 * Same deal as with OPTIMIZE_WORD_CHARS.
 */
#define     OPTIMIZE_WORD_BEGIN_CHARS   1

/**
 * Characters that may end a word; usually the same as the above.
 */
char const  Word_End_Chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";

/**
 * Same deal as with OPTIMIZE_WORD_CHARS.
 */
#define     OPTIMIZE_WORD_END_CHARS     1

#ifdef WITH_SEARCH_DAEMON
////////// Search server daemon parameters ////////////////////////////////////

/**
 * Default name of the Unix domain socket file; this can be overridden either
 * in a config. file or on the command line.
 */
char const  SocketFile_Default[]        = "/tmp/search.socket";

/**
 * Default port number of the TCP socket; this can be overridden either in a
 * config. file or on the command line.
 */
int const   SocketPort_Default          = 1967;

/**
 * Maximum number of queued connections for a socket.  From [Stevens 1998], p.
 * 96:
 *
 * > Historically, sample code always shows a backlog of 5, as that was the
 * > maximum value supported by 4.2BSD.  This was adequate in the 1980s when
 * > busy servers would handle only a few hundred connections per day.  But
 * > with the growth of the World Wide Web (WWW), where busy servers handle
 * > millions of connections per day, this small number is completely
 * > inadequate.  Busy HTTP servers must specify a much larger backlog, and
 * > newer kernels must support larger values.
 *
 * Unfortunately, Stevens doesn't say what a good value is.  The default 511
 * value is taken from \c httpd.h in Apache:
 *
 * > It defaults to 511 instead of 512 because some systems store it as an
 * > 8-bit datatype; 512 truncated to 8-bits is 0, while 511 is 255 when
 * > truncated.
 *
 * If it's good enough for Apache, it's good enough for us.  This can be
 * overridden either in a config. file or on the command line.
 */
int const   SocketQueueSize_Default     = 511;

/**
 * The number of seconds a client has to complete a search request before being
 * disconnected.  This is to prevent a client from connecting, not completing a
 * request, and causing the thread servicing the request to wait forever.  This
 * can be overridden either in a config. file or on the command line.
 */
int const   SocketTimeout_Default       = 10;   // seconds

/**
 * The minimum number of simultanous threads; this can be overridden either in
 * a config. file or on the command line.
 */
int const   ThreadsMin_Default          = 5;

/**
 * The maximum number of simultanous threads; this can be overridden either in
 * a config. file or on the command line.
 */
int const   ThreadsMax_Default          = 100;

/**
 * The number of seconds until an idle spare thread times out and destroys
 * itself.  This can be overridden either in a config.  file or on the command
 * line.
 */
int const   ThreadTimeout_Default       = 30;   // seconds

/**
 * The user to switch to after initialization (if root to begin with).  This
 * can be overridden either in a config. file or on the command line.
 */
char const  User_Default[]              = "nobody";

/**
 * The group to switch to after initialization (if root to begin with).  This
 * can be overridden either in a config. file or on the command line.
 */
char const  Group_Default[]             = "nobody";
#endif /* WITH_SEARCH_DAEMON */

////////// Miscellaneous parameters ///////////////////////////////////////////

/**
 * Default name of the configuration file; this can be overridden on the
 * command line.
 */
char const  ConfigFile_Default[]        = "swish++.conf";

/**
 * Default extension to append to filenames during extraction.  This can be
 * overridden either in a config. file or on the command line.
 */
char const  ExtractExtension_Default[]  = "txt";

/**
 * Default number of files to grow reserved space for when incrementally
 * indexing.  This can be overridden either in a config.  file or on the
 * command line.
 */
int const   FilesGrow_Default           = 100;

/**
 * Default maximum number of files to reserve space for; see \c file_info.cpp
 * for details.  This can be overridden either in a config. file or on the
 * command line.
 */
int const   FilesReserve_Default        = 1000;

/**
 * Number of times to try to fork before giving up.  This parameter is used
 * only in \c filter.cpp.
 */
int const   Fork_Attempts               = 5;

/**
 * Number of seconds to sleep before retrying to fork.  This parameter is used
 * only in \c filter.cpp.
 */
int const   Fork_Sleep                  = 5;    // seconds

/**
 * Default name of the index file generated/searched; can be overridden either
 * in a config. file or on the command line.
 */
char const  IndexFile_Default[]         = "swish++.index";

/**
 * Default maximum number of search results; this can be overridden either in a
 * config. file or on the command line.
 */
int const   ResultsMax_Default          = 100;

/**
 * Characters in a Unix shell command that delimit file names.  Note that this
 * says "file" (not "path") names.
 */
char const  ShellFilenameDelimChars[]   = " \t&;<>|";

/**
 * Characters in a file name that must be escaped when passed to a Unix shell.
 * This is a superset of what are commonly referred to as "meta-characers"
 * because the space and tab characters are included.  Note again that this
 * says "file" (not "path") name.
 */
char const  ShellFilenameEscapeChars[]  = " !\"#$&'()*/;<>?[\\]^`{|}~";

#ifdef __CYGWIN__
char const  TempDirectory_Default[]     = "/temp";
#else
#/*#error      You have not set TempDirectory_Default for your system.*/
#/*#error      Comment out these lines after you have set it.*/

/**
 * Default directory to use for temporary files during indexing.  If your OS
 * mounts swap space via \c /tmp (e.g., Solaris), as indexing progresses and
 * more files get created in \c /tmp, you will have less swap space, indexing
 * will get slower, and you may run out of memory.  If this is the case, you
 * can either change this default here for all users (preferred) or override it
 * either in a config.  file or on the command line to use a directory on a
 * real filesystem, i.e., one on a physical disk, e.g., \c /var/tmp on some
 * OSs.  The directory must exist.
 */
char const  TempDirectory_Default[]     = "/tmp";
#endif /* __CYGWIN__ */

/**
 * Specifies the maximum number of lines into a file for its "title" (whatever
 * that means for a given file format); this can be overridden either in a
 * config. file or on the command line.
 */
int const   TitleLines_Default          = 50;

/**
 * Maximum length of a file "title" (whatever that means for a given file
 * format).
 */
int const   Title_Max_Size              = 200;

#ifdef WITH_WORD_POS
/**
 * The maximum number of words apart two words can be to be considered "near"
 * each other; this can be overridden either in a config. file or on the
 * command line.
 */
int const   WordsNear_Default           = 10;
#endif /* WITH_WORD_POS */

/**
 * Default maximum percentage of files a word may occur in before it is
 * discarded as being too frequent; this can be overridden either in a config.
 * file or on the command line.
 */
int const   WordPercentMax_Default      = 100;

#/*#error      You have not set WordThreshold_Default for your system.*/
#/*#error      Comment out these lines after you have set it.*/

/**
 * The word count past which partial indicies are generated and merged since
 * all the words are too big to fit into memory at the same time.  If you index
 * and your machine begins to swap like mad, lower this value.  The above works
 * OK in a 1GB machine.  A rule of thumb is to add 4000000 words for each
 * additional GB of RAM you have.  These numbers are for a SPARC machine
 * running Solaris.  Other machines running other operating systems use memory
 * differently.  You simply have to experiment.  Only the super-user can
 * increase this either in a config. file or on the command line.
 */
int const   WordThreshold_Default       = 4000000;

///////////////////////////////////////////////////////////////////////////////

#endif /* swishxx_config_H */
/* vim:set et sw=2 ts=2: */
