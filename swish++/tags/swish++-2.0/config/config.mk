##
#	SWISH++
#	config/config.mk
#
#	Copyright (C) 1998  Paul J. Lucas
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
# 
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
# 
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the Free Software
#	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

##
#	Note: If you later redefine any of these variables, you *MUST* first
#	do a "make distclean" before you do another "make".
##

###############################################################################
#
#	General stuff
#
###############################################################################

MAKE=		make
#		The 'make' software you are using; usually "make".

RM=		rm -fr
#		The command to remove files recursively and ignore errors;
#		usually "rm -fr".

PERL=		/usr/local/bin/perl
#		The full path to the perl 5 executable; usually "/bin/perl" or
#		"/usr/local/bin/perl".  You need this only if you intend on
#		using httpindex(1).

SHELL=		/bin/sh
#		The shell to spawn for subshells; usually "/bin/sh".

STRIP=		strip
#		The command to strip symbolic informaiton from executables;
#		usually "strip".  You can leave this defined even if your OS
#		doesn't have it or any equivalent since any errors from this
#		command are ignored in the Makefiles.

###############################################################################
#
#	C++ compiler
#
###############################################################################

CC=		g++
#		The C++ compiler you are using; usually "CC" or "g++".

CCFLAGS=	-O3 #-g -pg
#		Flags for C++ compiler:
#		-g	Include symbol-table information in object file.
#		-O	Turn optimization on.  Some implementations allow a
#			digit after the O for optimization level.
#		-pg	Turn profiling on.

CCHEXT=		.h
#		C++ declaration file name extension; usually ".h", ".H", ".hh",
#		or ".hpp".

CCCEXT=		.c
#		C++ definition file name extension; usually ".c", ".C", ".cc",
#		or ".cpp".

CCOEXT=		.o
#		C++ object file name extension; usually ".o", ".obj".

CCLOPT=		-L
#		Flag for locating libraries; usually "-L" but some systems,
#		for example UTS, use "-Wl,-L".

###############################################################################
#
#	Installation
#
###############################################################################

INSTALL=	$(ROOT)/install-sh
#		Install command; usually "$(ROOT)/install-sh".

I_ROOT=		/home/www/swish++
#		The top-level directory of where SWISH++ will be installed.

I_BIN=		$(I_ROOT)/bin
#		Where executables are installed; usually "$(I_ROOT)/bin".

I_LIB=		$(I_ROOT)/lib
#		Where libraries are installed; "$(I_ROOT)/lib".

I_MAN=		$(I_ROOT)/man
#		Where manual pages are installed; "$(I_ROOT)/man".

I_OWNER=	-o bin
#		The owner of the installed files.

I_GROUP=	-g bin
#		The group of the installed files.

I_MODE=		-m 644
#		File permissions for regular files (non executables).

I_XMODE=	-m 755
#		File permissions for eXecutables.

MKDIR=		$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) -d
#		Command used to create a directory.

###############################################################################
#
#	Manual page formatting
#
###############################################################################

DISTILL=	distill
#		Command for converting PostScript to PDF; usually "distill".
#		This is used by the developer only; you don't need this since
#		PDF versions of the manual pages are included in the SWISH++
#		distribution.

NROFF=		nroff
#		Command for formatting [nt]roff input for a terminal screen;
#		usually "nroff".

NROFF_FLAGS=	-man
#		Flags for nroff to format manual pages; usually "-man".

TBL=		tbl
#		Command for processing 'tbl' input to [nt]roff; usually "tbl".

TROFF=		troff
#		Command for formatting [nt]roff input for a phototypesetter;
#		usually "troff".

TROFF_FLAGS=	-man -Tpost
#		Flags for troff to format manual pages; usually "-man -Tpost".

TO_PS=		/usr/lib/lp/postscript/dpost
#		Command to convert troff output to PostScript.

TO_TXT=		col -b
#		Command to strip all non-text characters from nroff output;
#		usually "col -b".

########## You shouldn't have to change anything below this line. #############

# ROOT is defined by the Makefile including this.

CONFIG=		$(ROOT)/config
MAN=		$(ROOT)/man

PLATFORM_H=	platform$(CCHEXT)
PLATFORM_H_TARGET=$(ROOT)/$(PLATFORM_H)
