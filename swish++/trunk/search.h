/*
**	SWISH++
**	search.h
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

#ifndef	search_H
#define	search_H

// standard
#include <iostream>

// local
#include "fake_ansi.h"			/* for std */
#include "option_stream.h"

//*****************************************************************************
//
// SYNOPSIS
//
	struct search_options
//
// DESCRIPTION
//
//	A search_options is the set of command-line options to the "search"
//	executable.  These are all packaged into a structure because, in
//	addition to parsing command-line options initially, "search," if run as
//	a daemon, needs to parse "command-line" options per request via a
//	socket.  The options are therefore packaged together into an object so
//	each thread can have its own set of options, i.e., be thread-safe.
//
//	Structure member names are such that those that end in "_arg" are the
//	value of the argument of the option (which implies that the option was
//	given) and those that end in "_opt" are Boolean options and, if true,
//	mean that the option was given.
//
//*****************************************************************************
{
	char const*	config_file_name_arg;
	bool		dump_entire_index_opt;
	int		dump_match_arg;
	bool		dump_meta_names_opt;
	bool		dump_stop_words_opt;
	int		dump_window_size_arg;
	bool		dump_word_index_opt;
	char const*	index_file_name_arg;
	char const*	max_results_arg;
	char const*	result_separator_arg;
	int		skip_results_arg;
	bool		stem_words_opt;
	char const*	word_file_max_arg;
	char const*	word_percent_max_arg;
#ifdef	SEARCH_DAEMON
	char const*	daemon_type_arg;
	int		max_threads_arg;
	int		min_threads_arg;
	char const*	pid_file_name_arg;
	char const*	socket_address_arg;
	char const*	socket_file_name_arg;
	int		socket_queue_size_arg;
	int		socket_timeout_arg;
	int		thread_timeout_arg;
#endif
	search_options(
		int *argc, char ***argv,
		PJL::option_stream::spec const[],
		std::ostream& = std::cerr
	);

	operator bool() const			{ return !bad_; }
private:
	bool	bad_;
};

void		service_request(
			char *argv[],
			search_options const&,
			std::ostream& = std::cout, std::ostream& = std::cerr
		);

std::ostream&	usage( std::ostream& );

#endif	/* search_H */
