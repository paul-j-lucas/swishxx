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
#	OS selection
#
#	Uncomment whichever line represents your OS.  If your OS isn't listed
#	here, just pick Linux and see if that works.
#
###############################################################################

#FREE_BSD:=	-DFreeBSD
LINUX:=		-DLinux
#SOLARIS:=	-DSolaris
#WIN32:=		-DWIN32

# Leave the following line alone!
OS:=		$(FREE_BSD) $(LINUX) $(SOLARIS) $(WIN32)

###############################################################################
#
#	SWISH++ stuff
#
###############################################################################

MOD_HTML:=	-DMOD_HTML
MOD_MAIL:=	-DMOD_MAIL
MOD_MAN:=	-DMOD_MAN
MOD_LIST:=	$(MOD_HTML) $(MOD_MAIL) $(MOD_MAN)
#		The indexing modules you want built into index(1).

ifndef WIN32
#		The search daemon ability is not currently supported for
#		Windows.  The only way it will ever be is if somebody
#		volunteers to port the socket and multithreading code.

SEARCH_DAEMON:=	-DSEARCH_DAEMON -DMULTI_THREADED -D_REENTRANT
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

ifdef SEARCH_DAEMON

ifdef FREE_BSD
PTHREAD_LIB:=	-pthread
else
PTHREAD_LIB:=	-lpthread
endif
#		Library to link against for POSIX threads if building with the
#		search daemon ability.

ifdef SOLARIS
SOCKET_LIB:=	-lsocket -lnsl
#		Library to link against for sockets if building with the search
#		daemon ability.
endif

endif # SEARCH_DAEMON
endif # WIN32

###############################################################################
#
#	General stuff
#
###############################################################################

RM:=		rm -fr
#		The command to remove files recursively and ignore errors;
#		usually "rm -fr" for Unix or "erase" for Windows.

PERL:=		/usr/local/bin/perl
#		The full path to the Perl 5 executable; usually "/bin/perl" or
#		"/usr/local/bin/perl" for Unix or "\Perl\bin\perl" for
#		Windows.  You need this only if you intend on using
#		httpindex(1) or searchc(1).

SHELL:=		/bin/sh
#		The shell to spawn for subshells; usually "/bin/sh".

STRIP:=		strip
#		The command to strip symbolic information from executables;
#		usually "strip".  You can leave this defined even if your OS
#		doesn't have it or any equivalent since any errors from this
#		command are ignored in the Makefiles.

###############################################################################
#
#	C++ compiler
#
###############################################################################

CC:=		g++
#		The C++ compiler you are using; usually "CC" or "g++".

OPTIM:=		-O2
#		The optimization level.  Many compilers allow a digit after the
#		O to specify the level of optimization; if so, set yours to the
#		highest number your compiler allows without eliciting problems
#		in the optimizer.
#
#		Using g++, -O3 under Cynwin under Windows produces bad code;
#		-O3 with 3.0 causes the optimizer to take ridiculously long and
#		use most of the CPU and memory.
#
#		If SWISH++ doesn't work correctly with optimization on, but it
#		works just fine with it off, then there is a bug in your
#		compiler's optimizer.

CCFLAGS:=	$(MOD_LIST) $(SEARCH_DAEMON) $(OS) $(OPTIM)
#		Flags for the C++ compiler.
ifeq ($(findstring g++,$(CC)),g++)
CCFLAGS+=	-fno-exceptions
#		Since SWISH++ doesn't use exceptions, turn off code generation
#		for them to save space in the executables.
endif

ifeq ($(findstring g++,$(CC)),g++)
#CCFLAGS+=	-W -Wcast-align -Wcast-qual -Wpointer-arith -Wswitch -Wtraditional -Wuninitialized -Wunused #-Winline -Wshadow
endif
#		Warning flags specific to g++.  Unless you are modifying the
#		source code, you should leave this commented out.

###############################################################################
#
#	Installation
#
###############################################################################

INSTALL:=	$(ROOT)/install-sh
#		Install command; usually "$(ROOT)/install-sh".

I_ROOT:=	/usr/local
#		The top-level directory of where SWISH++ will be installed.

I_BIN:=		$(I_ROOT)/bin
#		Where executables are installed; usually "$(I_ROOT)/bin".

I_LIB:=		$(I_ROOT)/lib
#		Where libraries are installed; usually "$(I_ROOT)/lib".

I_MAN:=		$(I_ROOT)/man
#		Where manual pages are installed; usually "$(I_ROOT)/man".

I_OWNER:=	-o bin
#		The owner of the installed files.

I_GROUP:=	-g bin
#		The group of the installed files.

I_MODE:=	-m 644
#		File permissions for regular files (non executables).

I_XMODE:=	-m 755
#		File permissions for eXecutables and directories.

MKDIR:=		$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) -d
#		Command used to create a directory.

########## You shouldn't have to change anything below this line. #############

# $(ROOT) is defined by the Makefile including this.

.SUFFIXES: .in

% :: %.in
	$(PERL) $(ROOT)/config/config.pl $< < $(ROOT)/config/config.mk

##
# The end.
##
