/*
**	SWISH++
**	stop_words.c
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

// standard
#include <cctype>

// local
#include "config.h"
#include "exit_codes.h"
#include "index_segment.h"
#include "mmap_file.h"
#include "platform.h"
#include "stop_words.h"
#include "util.h"
#include "word_util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace PJL;
using namespace std;
#endif

extern char const*	me;

stop_word_set*		stop_words;		// pointer to global set

//*****************************************************************************
//
// SYNOPSIS
//
	stop_word_set::stop_word_set( char const *file_name )
//
// DESCRIPTION
//
//	Construct (initialize) a stop_word_set either from the built-in list of
//	stop-words or from a list given in a file.
//
// PARAMETERS
//
//	file_name	The name of the file containing a list of stop-words.
//			If it's either a null pointer or the string it points
//			to is a null string, then the built-in list is used.
//
//*****************************************************************************
{
	static char const *const default_stop_word_table[] = {

		// A

		"about",
		"above",
		"according",
		"across",
		"actually",
		"adj",
		"after", "afterwards",
		"again", "against",
		"all",
		"almost",
		"alone",
		"along",
		"already",
		"alright", "allright",	// not real words, but common
		"also",
		"although",
		"always",
		"among", "amongst",
		"and",
		"another",
		"any", "anyhow", "anyone", "anything", "anyway", "anywhere",
		"are", "aren", "aren't",
		"around",

		// B

		"bad",
		"became",
		"because",
		"become", "becomes", "becoming",
		"been",
		"before", "beforehand",
		"began",
		"begin", "beginning", "begins",
		"behind",
		"being",
		"below",
		"beside", "besides",
		"best",
		"better",
		"between",
		"beyond",
		"billion", "billions",
		"both",
		"but",

		// C

		"came",
		"can", "can't", "cannot",
		"caption", "captions",
		"come", "comes", "coming",
		"could", "couldn", "couldn't",

		// D

		"did", "didn", "didn't",
		"does", "doesn", "doesn't",
		"don", "don't",
		"down",
		"during",

		// E

		"each",
		"eight", "eighteen", "eighty",
		"either",
		"else", "elsewhere",
		"end", "ended", "ending", "ends",
		"enough",
		"etc",
		"even",
		"ever", "every", "everyone", "everything", "everywhere",
		"except",

		// F

		"far", "farther",
		"few", "fewer",
		"fifteen", "fifty",
		"first", "firstly",
		"five",
		"for",
		"former", "formerly",
		"forty",
		"found",
		"four", "fourteen", "fourty",
		"from",
		"further",

		// G

		"good",
		"great", "greater", "greatest",

		// H

		"had",
		"half",
		"has", "hasn", "hasn't",
		"have", "haven", "haven't", "having",
		"hence",
		"her",
		"here", "hereafter", "hereby", "herein", "hereupon",
		"hers", "herself",
		"him", "himself",
		"his",
		"how",
		"however",
		"hundred", "hundreds",

		// I

		"i.e.",
		"inc",
		"indeed",
		"instead",
		"into",
		"isn", "isn't",
		"its",
		"itself",

		// J

		"just",

		// L

		"last",
		"later",
		"latter",
		"least",
		"less", "lesser",
		"let", "lets", "let's", "letting",
		"like", "liked", "likes",
		"likely",
		"likewise",
		"ltd",

		// M

		"made",
		"make", "makes", "making",
		"many",
		"may",
		"maybe",
		"meantime",
		"meanwhile",
		"might",
		"million", "millions",
		"miss",
		"more",
		"moreover",
		"most", "mostly",
		"mrs",
		"much",
		"must", "mustn't",
		"my",
		"myself",

		// N

		"namely",
		"neither",
		"never", "nevertheless",
		"next",
		"nine", "nineteen", "ninety",
		"nobody",
		"none",
		"nonetheless",
		"noone",
		"nor",
		"not",
		"nothing",
		"now",
		"nowhere",

		// O

		"off",
		"often",
		"once",
		"one",
		"only",
		"onto",
		"other", "others", "otherwise",
		"our", "ours", "ourselves",
		"out",
		"over",
		"overall",
		"own",

		// P

		"per",
		"perhaps",

		// R

		"rather",
		"really",
		"recent", "recently",

		// S

		"same",
		"saw",
		"second", "seconds",
		"see", "seen",
		"seem", "seemed", "seeming", "seems",
		"seven", "seventeen", "seventy",
		"several",
		"shall",
		"she",
		"should", "shouldn", "shouldn't",
		"since",
		"six", "sixteen", "sixty",
		"some", "somehow", "someone", "something",
		"sometime", "sometimes",
		"somewhere",
		"still",
		"stop",
		"such",

		// T

		"take", "taken", "taking",
		"ten", "tens",
		"than",
		"that",
		"the",
		"their",
		"them", "themselves",
		"then",
		"thence",
		"there", "thereafter", "thereby", "therefore",
		"therein", "thereupon",
		"these",
		"they",
		"third", "thirteen", "thirty",
		"this",
		"thorough",
		"those",
		"though",
		"thousand", "thousands",
		"three",
		"through", "throughout",
		"thru",
		"thus", "thusly",
		"together",
		"too",
		"took",
		"toward", "towards",
		"trillion", "trillions",
		"twelve",
		"twenty",
		"two",

		// U

		"under",
		"unless",
		"unlike", "unlikely",
		"until",
		"upon",
		"us",
		"use", "used", "uses", "using",
		"usual", "usually",

		// V

		"varied", "various", "vary",
		"very",
		"via",

		// W

		"want",
		"was", "wasn", "wasn't",
		"way",
		"well",
		"were", "weren", "weren't",
		"what", "whatever",
		"when", "whence", "whenever",
		"where", "whereafter", "whereas", "whereby",
		"wherein", "whereupon", "wherever",
		"whether",
		"which",
		"while",
		"whither",
		"who", "whoever",
		"whole",
		"whom", "whomever",
		"whose",
		"why",
		"will",
		"with", "within", "without",
		"won",
		"won't",
		"worse",
		"would", "wouldn", "wouldn't",

		// Y

		"yes",
		"yet",
		"you", "you'll", "your", "you're", "yours",
		"yourself", "yourselves",

		// Z

		"zero",

		0
	};

	if ( !file_name || !*file_name ) {		// no file: use default
		for ( register char const *const *
			w = default_stop_word_table; *w; ++w
		)
			insert( *w );
		return;
	}

	mmap_file const file( file_name );
	if ( !file ) {
		error() << "could not open \"" << file_name << "\"\n";
		::exit( Exit_No_Read_Stopwords );
	}

	char	word[ Word_Hard_Max_Size + 1 ];
	int	len;
	bool	in_word = false;

	register mmap_file::const_iterator c = file.begin();
	while ( true ) {
		if ( c != file.end() ) {
			register char const ch = tolower(
				iso8859_to_ascii( *c++ )
			);

			////////// Collect a word /////////////////////////////

			if ( is_word_char( ch ) ) {
				if ( !in_word ) {
					// start a new word
					word[ 0 ] = ch;
					len = 1;
					in_word = true;
					continue;
				}
				if ( len < Word_Hard_Max_Size ) {
					// continue same word
					word[ len++ ] = ch;
					continue;
				}
				in_word = false;	// too big: skip chars
				while ( c != file.end() && is_word_char(*c++) );
				continue;
			}

			if ( ch == '#' )		// discard comment
				while ( c != file.end() && *c++ != '\n' ) ;

			if ( !in_word )
				continue;
		} else
			if ( !in_word )
				break;

		////////// Got a word /////////////////////////////////////////

		in_word = false;
		if ( len < Word_Hard_Min_Size )
			continue;

		word[ len ] = '\0';
		insert( ::strdup( word ) );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	stop_word_set::stop_word_set( mmap_file const &index_file )
//
// DESCRIPTION
//
//	Construct (initialize) a stop_word_set from an existing index file.
//
// PARAMETERS
//
//	index_file	The index file to read the stop-words from.
//
//*****************************************************************************
{
	index_segment stop_words( index_file, index_segment::stop_word_index );
	FOR_EACH( index_segment, stop_words, stop_word )
		insert( ::strdup( *stop_word ) );
}
