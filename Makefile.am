##
#	SWISH++
#	Makefile.am
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

ACLOCAL_AMFLAGS =	-I m4
SUBDIRS =		lib src man test

EXTRA_DIST =		bootstrap \
			m4/gnulib-cache.m4 \
			README.md \
			README.Solaris

dist-hook:
	find $(distdir) -name .DS_Store -exec rm -f {} \;

update-gnulib:
	gnulib-tool --add-import --no-vc-files

# vim:set noet sw=8 ts=8:
