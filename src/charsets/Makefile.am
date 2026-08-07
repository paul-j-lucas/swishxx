##
#	SWISH++
#	src/charsets/Makefile.am
#
#	Copyright (C) 1998-2026  Paul J. Lucas
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

lib_LIBRARIES =	libcharsets.a

AM_CXXFLAGS =	$(SWISHXX_CXXFLAGS)
AM_CPPFLAGS =	-I$(top_srcdir)/lib -I$(top_builddir)/lib -I$(top_srcdir)/src

TIDY_FLAGS =	-Xtidy --config=$(top_srcdir)/src/include-tidy.toml \
		-Xtidy --verbose=f \
		-Xtidy --error=never
TIDY_CXXFLAGS =	$(CXXFLAGS) $(AM_CXXFLAGS) $(CPPFLAGS) $(AM_CPPFLAGS) -xc++

libcharsets_a_SOURCES =

if WITH_UTF7
libcharsets_a_SOURCES+= utf7.cpp
endif

if WITH_UTF8
libcharsets_a_SOURCES+= utf8.cpp
endif

if WITH_UTF16
libcharsets_a_SOURCES+= utf16.cpp
endif

tidy-local:
	@for file in *.h *.cpp; do \
	  [ ! -f "$$file" ] || \
	    include-tidy $(TIDY_FLAGS) $(TIDY_CXXFLAGS) "$$file" || exit $$?; \
	done

# vim:set noet sw=8 ts=8:
