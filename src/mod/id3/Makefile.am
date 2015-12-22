##
#	SWISH++
#	src/id3/Makefile.am
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

lib_LIBRARIES = libid3.a

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_builddir)/lib -I$(top_srcdir)/src -L$(top_srcdir)/lib -L$(top_srcdir)/src/pjl

libid3_a_SOURCES = id3v1.cpp id3v2.cpp mod_id3.cpp
libid3_a_LIBADD = $(top_builddir)/lib/libgnu.a

# vim:set noet sw=8 ts=8:
