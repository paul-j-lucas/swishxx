##
#	SWISH++
#	src/html/Makefile.am
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

lib_LIBRARIES = libhtml.a

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_builddir)/lib -I$(top_srcdir)/src

libhtml_a_SOURCES = elements.cpp entities.cpp mod_html.cpp
libhtml_a_LIBADD = $(top_builddir)/lib/libgnu.a

# vim:set noet sw=8 ts=8:
