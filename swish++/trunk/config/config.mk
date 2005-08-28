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
#	here, first try none; then, if that doesn't work, pick Linux and see if
#	that works.
#
###############################################################################

#FREE_BSD:=1
LINUX:=1
#MAC_OS_X:=1
#SOLARIS:=1
#WIN32:=1

###############################################################################
#
#	SWISH++ stuff
#
###############################################################################

MOD_LIST:=	html id3 latex mail man rtf
#		The indexing modules you want built into index(1).  If you want
#		to index mail files and you want to be able to index HTML
#		attachments, then you need to build-in "html"; similarly, if
#		you want to be able to index RTF attachments, then you need to
#		build-in "rtf".

CHARSET_LIST:=	utf7 utf8 utf16
#		The character sets you want index(1) to be able to decode.
#		Note that "us-ascii" and "iso8859-1" are implicitly included.
#
#		If you have no intention of indexing mail or news files, then
#		you do not need utf7.
#
#		If you have no intention of indexing either mail, news, or MP3
#		files and being able to index ID3 tags containing Unicode
#		characters, then you do not need either utf8 or utf16.

ENCODING_LIST:=	base64 quoted_printable
#		The Content-Transfer-Encodings you want index(1) to be able to
#		decode only when MOD_LIST contains "mail".

## READ THIS ->	Including any character sets or encodings requires more
#		processing PER CHARACTER and therefore will be slower for ALL
#		files (not just those that are encoded).  So if you don't need
#		any character sets (other than ISO 8859-1) or encodings, do NOT
#		compile them in.

FEATURE_LIST:=	word_pos
#		The set of optional features you want built into SWISH++:
#
#		1. word_pos: Store word positions during indexing needed to do
#		   "near" searches.  Storing said data approximately doubles
#		   the size of the generated index.

# Leave the following lines alone!
CHARSET_DEFS:=	$(foreach charset,$(CHARSET_LIST),-DCHARSET_$(charset))
ENCODING_DEFS:=	$(foreach encoding,$(ENCODING_LIST),-DENCODING_$(encoding))
FEATURE_DEFS:=	$(foreach feature,$(FEATURE_LIST),-DFEATURE_$(feature))
MOD_DEFS:=	$(foreach mod,$(MOD_LIST),-DMOD_$(mod))

# These too!
ifneq ($(CHARSET_DEFS),"")
DECODING:=	-DIMPLEMENT_DECODING
endif
ifneq ($(ENCODING_DEFS),"")
DECODING:=	-DIMPLEMENT_DECODING
endif

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
PTHREAD_LINK:=	-pthread
else
PTHREAD_LINK:=	-lpthread
endif
#		Library to link against for POSIX threads if building with the
#		search daemon ability.

ifdef LINUX
SEARCH_DAEMON+=	-D_XOPEN_SOURCE=500
#		Linux needs this to define more POSIX thread functions.
endif

ifdef SOLARIS
SOCKET_LINK:=	-lsocket -lnsl
#		Library to link against for sockets if building with the search
#		daemon ability.
endif

endif # SEARCH_DAEMON
endif # WIN32

ifeq ($(findstring id3,$(MOD_LIST)),id3)
ZLIB_LINK:=	-lz
#		Library to link against for zlib compression if building with
#		the ID3 module.
endif

###############################################################################
#
#	General stuff
#
###############################################################################

AR:=		ar rv
#		The command (plus arguments) to create archive libraries;
#		usually "ar rv".

RM:=		rm -fr
#		The command to remove files recursively and ignore errors;
#		usually "rm -fr" for Unix or "erase" for Windows.

PERL:=		/usr/local/bin/perl
#		The full path to the Perl 5 executable; usually "/bin/perl" or
#		"/usr/local/bin/perl" for Unix or "\Perl\bin\perl" for
#		Windows.

RANLIB:=	ranlib
#		The command to generate library tables-of-contents; usually
#		"ranlib".  If your OS doesn't need this done to libraries, you
#		can still leave this here since errors from this command are
#		ignored in the makefiles.

SHELL:=		/bin/sh
#		The shell to spawn for subshells; usually "/bin/sh".

STRIP:=		strip
#		The command to strip symbolic information from executables;
#		usually "strip".  You can leave this defined even if your OS
#		doesn't have it or any equivalent since any errors from this
#		command are ignored in the makefiles.

###############################################################################
#
#	C++ compiler
#
###############################################################################

CC:=		g++
#		The C++ compiler you are using; usually "CC" or "g++".

#DEBUG:=		true
ifdef DEBUG
OPTIM:=		-g
else
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

ifeq ($(findstring g++,$(CC)),g++)
OPTIM+=		-fomit-frame-pointer
endif
endif # DEBUG

CCFLAGS:=	-I. $(CHARSET_DEFS) $(ENCODING_DEFS) $(DECODING) $(MOD_DEFS) \
		$(FEATURE_DEFS) $(SEARCH_DAEMON) $(OPTIM)
#		Flags for the C++ compiler.

ifdef LINUX
CCFLAGS+=	-D_BSD_SOURCE
endif

ifeq ($(findstring g++,$(CC)),g++)
CCFLAGS+=	-fno-exceptions
#		Since SWISH++ doesn't use exceptions, turn off code generation
#		for them to save space in the executables.
ifneq ($(findstring word_pos,$(FEATURE_LIST)),word_pos)
CCFLAGS+=	-fno-rtti
#		SWISH++ uses RTTI only for the word_pos feature, so, if the
#		feature isn't being compiled in, turn off code generation for
#		RTTI to save space in the executables.
endif
endif

ifeq ($(findstring g++,$(CC)),g++)
#CCFLAGS+=	-fmessage-length=0 -W -Wcast-align -Wcast-qual -Wnon-virtual-dtor -Wpointer-arith -Wreorder -Wswitch -Wtraditional -Wuninitialized -Wunreachable-code -Wunused #-Winline -Wshadow
endif
#		Warning flags specific to g++.  Unless you are modifying the
#		source code, you should leave this commented out.

ifdef SOLARIS
TEMPLATE_REPOSITORY:=	SunWS_cache
#		The name of the directory used as C++ template repository.
#		Using Sun's CC, this is usually "SunWS_cache".  If another
#		compiler is being used that doesn't use such a repository
#		(e.g., g++), it's harmless to leave this as-is.
endif

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

I_ETC:=		$(I_ROOT)/etc
#		Where .conf files are installed; usually "$(I_ROOT)/etc".

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

ifeq ($(findstring g++,$(CC)),g++)
MAKEDEPEND=	$(CC) -MM $(CFLAGS)
else
MAKEDEPEND=	$(PERL) $(ROOT)/config/makedepend.pl $(CFLAGS)
endif

.%.d : %.c $(ROOT)/platform.h
	$(SHELL) -ec '$(MAKEDEPEND) $< | sed "s!\([^:]*\):!\1 $@ : !g" > $@; [ -s $@ ] || $(RM) $@'

ifneq ($(findstring platform,$(TARGET)),platform)
$(ROOT)/platform.h $(ROOT)/config/platform.mk:
	@$(MAKE) -C $(ROOT)/config
endif

.SUFFIXES: .in
% :: %.in
	$(PERL) $(ROOT)/config/config.pl $< < $(ROOT)/config/config.mk

# vim:set noet sw=8 ts=8:
