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
#	SWISH++ stuff
#
###############################################################################

MOD_HTML=	-DMOD_HTML
MOD_MAIL=	-DMOD_MAIL
MOD_LIST=	$(MOD_HTML) $(MOD_MAIL)
#		The indexing modules you want built into index(1).

#WIN32=		-DWIN32
#		If uncommented, build SWISH++ for Windows.

ifndef WIN32
#		The search daemon ability is not currently supported for
#		Windows.  The only way it will ever be is if somebody
#		volunteers to port the socket and multithreading code.

SEARCH_DAEMON=	-DSEARCH_DAEMON -DMULTI_THREADED -D_REENTRANT
#		These definitions will build search(1) with the ability to run
#		in the background as a multi-threaded daemon process.  Comment
#		this out if you have no need for this feature.  Currently, the
#		daemon ability is supported only for Unix and not Windows.
#
#		In order to build with the search daemon ability, use of the C
#		global variable "errno" must be thread-safe.  This usually
#		means that "errno" is defined as a macro rather than an integer
#		variable.  Different operating systems have different ways to
#		enable this.  For example, Solaris requires that you #define
#		_REENTRANT; FreeBSD has it enabled automatically; for all other
#		operating systems, check your documentation.  (Start with
#		error(3) and intro(2).)

PTHREAD_LIB=	-lpthread
#		Library to link against for POSIX threads if building with the
#		search daemon ability.

SOCKET_LIB=	-lsocket
#		Library to link against for sockets if building with the search
#		daemon ability.  Comment this out on Linux systems.
endif

###############################################################################
#
#	General stuff
#
###############################################################################

MAKE=		make
#		The 'make' software you are using; usually "make".

RM=		rm -fr
#		The command to remove files recursively and ignore errors;
#		usually "rm -fr" for Unix or "erase" for Windows.

PERL=		/usr/local/bin/perl
#		The full path to the Perl 5 executable; usually "/bin/perl" or
#		"/usr/local/bin/perl" for Unix or "\Perl\bin\perl" for
#		Windows.  You need this only if you intend on using
#		httpindex(1) or searchc(1).

SHELL=		/bin/sh
#		The shell to spawn for subshells; usually "/bin/sh".

STRIP=		strip
#		The command to strip symbolic information from executables;
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

#GCC_WARNINGS=	-W -Wcast-align -Wcast-qual -Winline -Wpointer-arith -Wshadow -Wswitch -Wtraditional -Wuninitialized -Wunused
#		Warning flags specific to gcc/g++.  Unless you are modifying
#		the source code, you should leave this commented out.

CCFLAGS=	$(GCC_WARNINGS) $(MOD_LIST) $(SEARCH_DAEMON) $(WIN32) -O3
#		Additional flags for the C++ compiler:
#
#		-g	Include symbol-table information in object file.  (You
#			normally wouldn't want to do this unless you are
#			either helping me to debug a problem found on your
#			system or you are changing SWISH++ yourself.)
#
#		-O	Turn optimization on.  Some compilers allow a digit
#			after the O for optimization level; if so, set yours
#			to the highest number your compiler allows (without
#			eliciting bugs in its optimizer).  If SWISH++ doesn't
#			work correctly with optimization on, but it works just
#			fine with it off, then there is a bug in your
#			compiler's optimizer.
#
#		-pedantic
#			Makes g++ warn about anything that isn't strict ANSI
#			standard C++.
#
#		-pg	Turn profiling on.  (You normally wouldn't want to do
#			this unless you are changing SWISH++ and want to try
#			to performance-tune your changes.)  This option may be
#			g++-specific.  If you are not using g++, consult your
#			C++ compiler's documentation.

CCLINK=		#-liberty
#		Flags for the linker:
#		-liberty	May be required under Windows for getopt(3C).

###############################################################################
#
#	Installation
#
###############################################################################

INSTALL=	$(ROOT)/install-sh
#		Install command; usually "$(ROOT)/install-sh".

I_ROOT=		/usr/local/packages/swish++
#		The top-level directory of where SWISH++ will be installed.

I_BIN=		$(I_ROOT)/bin
#		Where executables are installed; usually "$(I_ROOT)/bin".

I_LIB=		$(I_ROOT)/lib
#		Where libraries are installed; usually "$(I_ROOT)/lib".

I_MAN=		$(I_ROOT)/man
#		Where manual pages are installed; usually "$(I_ROOT)/man".

I_OWNER=	-o bin
#		The owner of the installed files.

I_GROUP=	-g bin
#		The group of the installed files.

I_MODE=		-m 644
#		File permissions for regular files (non executables).

I_XMODE=	-m 755
#		File permissions for eXecutables and directories.

MKDIR=		$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) -d
#		Command used to create a directory.

########## You shouldn't have to change anything below this line. #############

# $(ROOT) is defined by the Makefile including this.

.SUFFIXES: .in

% :: %.in
	$(PERL) $(ROOT)/config/config.pl $< < $(ROOT)/config/config.mk

##
# The end.
##
