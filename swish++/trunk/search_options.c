/*
**	SWISH++
**	search_options.c
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

/*
**	Note that this file is #include'd into search.c and search_thread.c
**	because it generates different code depending on which one it's
**	compiled into.
*/

static option_stream::spec const opt_spec[] = {
	"help",			0, '?',
	"dump-words",		0, 'd',
	"dump-index",		0, 'D',
	"word-files",		1, 'f',
	"results",		1, 'm',
	"dump-meta",		0, 'M',
	"word-percent",		1, 'p',
	"skip-results",		1, 'r',
	"stem-words",		0, 's',
	"dump-stop",		0, 'S',
	"version",		0, 'V',
	"window",		1, 'w',
#ifndef	SEARCH_DAEMON_OPTIONS_ONLY
	//
	// Once running as a daemon, 'search' no longer accepts the following
	// options.
	//
	"config-file",		1, 'c',
	"index-file",		1, 'i',
#ifdef	SEARCH_DAEMON
	"daemon-type",		1, 'b',
	"pid-file",		1, 'P',
	"socket-timeout",	1, 'o',
	"thread-timeout",	1, 'O',
	"queue-size",		1, 'q',
	"min-threads",		1, 't',
	"max-threads",		1, 'T',
	"socket-address",	1, 'a',
	"socket-file",		1, 'u',
#endif	/* SEARCH_DAEMON */
#endif	/* SEARCH_DAEMON_OPTIONS_ONLY */
	0
};
