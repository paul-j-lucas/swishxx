##
#	SWISH++
#	GNUmakefile
#
#	Copyright (C) 2000  Paul J. Lucas
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

ROOT=		.
include		$(ROOT)/config/config.mk

CFLAGS=		$(CCFLAGS)
#		Various debug flags; add to CFLAGS for debugging.
#		-DDEBUG_is_ok_word
#		-DDEBUG_parse_query
#		-DDEBUG_stem_word
#		-DDEBUG_threads

########## You shouldn't have to change anything below this line. #############

C_TARGET:=	index search extract

I_SRCS:=	bcd.c \
		conf_var.c \
		conf_bool.c \
		conf_int.c \
		conf_set.c \
		conf_string.c \
		directory.c \
		elements.c \
		entities.c \
		ExcludeFile.c \
		file_vector.c \
		file_info.c \
		filter.c \
		FilesGrow.c \
		FilterFile.c \
		IncludeFile.c \
		index_segment.c \
		file_list.c \
		itoa.c \
		html.c \
		option_stream.c \
		stop_words.c \
		util.c \
		word_info.c \
		word_util.c \
		index.c
I_OBJS:=	$(I_SRCS:.c=.o)
I_LINK:=	$(CCLINK) -lm

S_SRCS:=	bcd.c \
		conf_var.c \
		conf_bool.c \
		conf_int.c \
		conf_string.c \
		file_info.c \
		file_vector.c \
		index_segment.c \
		file_list.c \
		option_stream.c \
		token.c \
		stem_word.c \
		util.c \
		word_util.c \
		search.c
ifndef WIN32
S_SRCS+=	thread_pool.c \
		search_thread.c \
		search_daemon.c
endif
S_OBJS:=	$(S_SRCS:.c=.o)
S_LINK:=	$(CCLINK) $(SOCKET_LIB) $(PTHREAD_LIB)

E_SRCS:=	conf_var.c \
		conf_bool.c \
		conf_int.c \
		conf_set.c \
		conf_string.c \
		directory.c \
		entities.c \
		ExcludeFile.c \
		file_vector.c \
		filter.c \
		FilterFile.c \
		IncludeFile.c \
		index_segment.c \
		option_stream.c \
		postscript.c \
		stop_words.c \
		util.c \
		word_util.c \
		extract.c
E_OBJS:=	$(E_SRCS:.c=.o)
E_LINK:=	$(CCLINK)

PERL_TARGET:=	httpindex
ifndef WIN32
PERL_TARGET+=	searchc
endif
LIB_TARGET:=	WWW.pm

BUILD_TARGET:=	$(C_TARGET) $(PERL_TARGET)

##
# Build rules
##

.PHONY: all
all: $(BUILD_TARGET)

extract: $(E_OBJS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(E_OBJS) $(E_LINK)

index: $(I_OBJS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(I_OBJS) $(I_LINK)

search: $(S_OBJS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(S_OBJS) $(S_LINK)

platform.h:
ifdef WIN32
	cp $@.win32 $@
else
	@$(MAKE) -C config
endif

.%.d : %.c platform.h
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< | sed "s/\([^:]*\):/\1 $@ : /g" > $@; [ -s $@ ] || $(RM) $@'

ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
ifneq ($(MAKECMDGOALS),dist)
include	 $(I_SRCS:%.c=.%.d) $(E_SRCS:%.c=.%.d) $(S_SRCS:%.c=.%.d)
endif
endif

ps pdf txt:
	@$(MAKE) -C man $@

##
# Install rules
##

install: install_bin install_lib install_man

install_bin: $(BUILD_TARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(C_TARGET) $(I_BIN)
	cd $(I_BIN) && $(STRIP) $(C_TARGET)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(PERL_TARGET) $(I_BIN)

install_lib: $(I_LIB)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(LIB_TARGET) $(I_LIB)

install_man:
	@$(MAKE) -C man install

$(I_BIN) $(I_LIB):
	$(MKDIR) $@

uninstall:
	cd $(I_BIN) && $(RM) $(BUILD_TARGET)
	cd $(I_LIB) && $(RM) $(LIB_TARGET)
	@$(MAKE) -C man $@

##
# Utility rules
##

clean:
	$(RM) *.o .*.d core swish++.index
	@$(MAKE) -C man $@

dist distclean: clean
	$(RM) $(BUILD_TARGET) platform.h
	@$(MAKE) -C man $@
