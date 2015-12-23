##
#	SWISH++
#	src/Makefile.am
#
#	Copyright (C) 1998-2015  Paul J. Lucas
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the Licence, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

SUBDIRS = pjl charsets encodings mod

bin_PROGRAMS = index search extract

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_builddir)/lib -L$(top_srcdir)/lib

index_SOURCES = enc_int.cpp conf_var.cpp conf_bool.cpp conf_filter.cpp conf_int.cpp conf_percent.cpp conf_set.cpp conf_string.cpp encoded_char.cpp ExcludeFile.cpp file_info.cpp file_list.cpp filter.cpp IncludeFile.cpp IncludeMeta.cpp indexer.cpp index_segment.cpp init_modules.cpp init_mod_vars.cpp iso8859-1.cpp stop_words.cpp TempDirectory.cpp util.cpp word_info.cpp WordThreshold.cpp word_util.cpp index.cpp

index_LDADD = $(top_builddir)/lib/libgnu.a $(top_builddir)/src/charsets/libcharsets.a $(top_builddir)/src/encodings/libencodings.a $(top_builddir)/src/pjl/libpjl.a

search_SOURCES = enc_int.cpp conf_var.cpp conf_bool.cpp conf_enum.cpp conf_int.cpp conf_string.cpp index_segment.cpp init_mod_vars.cpp file_info.cpp file_list.cpp iso8859-1.cpp query_node.cpp query.cpp ResultsFormat.cpp results_formatter.cpp classic_formatter.cpp xml_formatter.cpp token.cpp stem_word.cpp util.cpp word_info.cpp word_util.cpp search.cpp Group.cpp search_thread.cpp search_daemon.cpp SearchDaemon.cpp SocketAddress.cpp User.cpp

search_LDADD = $(top_builddir)/lib/libgnu.a $(top_builddir)/src/charsets/libcharsets.a $(top_builddir)/src/encodings/libencodings.a $(top_builddir)/src/pjl/libpjl.a

extract_SOURCES = conf_var.cpp conf_bool.cpp conf_filter.cpp conf_int.cpp conf_set.cpp conf_string.cpp ExcludeFile.cpp ExtractFile.cpp filter.cpp index_segment.cpp init_mod_vars.cpp iso8859-1.cpp postscript.cpp stop_words.cpp util.cpp word_util.cpp extract.cpp

extract_LDADD = $(top_builddir)/lib/libgnu.a $(top_builddir)/src/charsets/libcharsets.a $(top_builddir)/src/encodings/libencodings.a $(top_builddir)/src/pjl/libpjl.a

INIT_MODULES_SH = $(top_srcdir)/config/init_modules-sh

init_modules.cpp: mod/*/mod_*.h $(INIT_MODULES_SH)
	$(INIT_MODULES_SH) > $@ || { rm -f $@; exit 1; }

INIT_MOD_VARS_SH = $(top_srcdir)/config/init_mod_vars-sh

init_mod_vars.cpp: mod/*/vars $(INIT_MOD_VARS_SH)
	$(INIT_MOD_VARS_SH) > $@ || { rm -f $@; exit 1; }

# vim:set noet sw=8 ts=8:
