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

CPP_TARGET:=	index search extract
PERL_TARGET:=	httpindex splitmail
ifndef WIN32
PERL_TARGET+=	searchc
endif
TARGET:=	$(CPP_TARGET) $(PERL_TARGET)

.PHONY: all
all: $(TARGET)

ROOT:=		.
include		$(ROOT)/config/config.mk

CFLAGS:=	$(CCFLAGS)
#		Various debug flags; add to CFLAGS for debugging.
#		-DDEBUG_is_ok_word
#		-DDEBUG_parse_query
#		-DDEBUG_stem_word
#		-DDEBUG_threads

########## You shouldn't have to change anything below this line. #############

MOD_LIBS:=	$(foreach mod,$(MOD_LIST),mod/$(mod)/libmod_$(mod).a)
MOD_LIB_PATHS:=	$(foreach mod,$(MOD_LIST),-Lmod/$(mod))
MOD_LINK:=	$(foreach mod,$(MOD_LIST),-lmod_$(mod))

I_SOURCES:=	bcd.c \
		mmap_file.c \
		conf_var.c \
		conf_bool.c \
		conf_filter.c \
		conf_int.c \
		conf_percent.c \
		conf_set.c \
		conf_string.c \
		directory.c \
		encoded_char.c \
		ExcludeFile.c \
		file_info.c \
		file_list.c \
		filter.c \
		IncludeFile.c \
		IncludeMeta.c \
		indexer.c \
		index_segment.c \
		init_modules.c \
		itoa.c \
		option_stream.c \
		stop_words.c \
		util.c \
		word_info.c \
		word_util.c \
		index.c
ifdef WIN32
I_SOURCES+=	fnmatch.c		# see the comment in pattern_map.h
endif
I_OBJECTS:=	$(I_SOURCES:.c=.o)
I_LINK:=	$(MOD_LINK) -lm

S_SOURCES:=	bcd.c \
		mmap_file.c \
		conf_var.c \
		conf_bool.c \
		conf_int.c \
		conf_string.c \
		index_segment.c \
		file_list.c \
		option_stream.c \
		query.c \
		token.c \
		stem_word.c \
		util.c \
		word_util.c \
		search.c
ifdef SEARCH_DAEMON
##
# See the comment in config/config.mk regarding the search daemon ability.
##
S_SOURCES+=	conf_enum.c \
		fdbuf.c \
		thread_pool.c \
		search_thread.c \
		search_daemon.c \
		SearchDaemon.c \
		SocketAddress.c
endif
S_OBJECTS:=	$(S_SOURCES:.c=.o)
S_LINK:=	$(SOCKET_LIB) $(PTHREAD_LIB)

E_SOURCES:=	mmap_file.c \
		conf_var.c \
		conf_bool.c \
		conf_filter.c \
		conf_int.c \
		conf_set.c \
		conf_string.c \
		directory.c \
		ExcludeFile.c \
		ExtractFile.c \
		filter.c \
		index_segment.c \
		option_stream.c \
		postscript.c \
		stop_words.c \
		util.c \
		word_util.c \
		extract.c
ifdef WIN32
E_SOURCES+=	fnmatch.c		# see the comment in pattern_map.h
endif
E_OBJECTS:=	$(E_SOURCES:.c=.o)
E_LINK:=	

LIB_TARGET:=	WWW.pm

##
# Build rules
##

extract: $(E_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(E_LINK)

index: $(I_OBJECTS) $(MOD_LIBS)
	$(CC) $(CFLAGS) $(MOD_LIB_PATHS) -o $@ $(I_OBJECTS) $(I_LINK)

init_modules.c: mod/*/mod_*.h init_modules-sh
	./init_modules-sh > $@ || $(RM) $@

search: $(S_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(S_LINK)

$(MOD_LIBS): FORCE
	@$(MAKE) -C $(dir $@) DEBUG="$(DEBUG)"

.PHONY: FORCE

# Don't do the "include" if the goal contains the word "clean," i.e., either
# the "clean" or "distclean" goal.
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
# Also, don't do the "include" if the goal is the "dist" goal.
ifneq ($(MAKECMDGOALS),dist)
include $(I_SOURCES:%.c=dep/%.d) $(E_SOURCES:%.c=dep/%.d) $(S_SOURCES:%.c=dep/%.d)
endif
endif

ps pdf txt:
	@$(MAKE) -C man $@

##
# Install rules
##

install: install_bin install_lib install_man

install_bin: $(TARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(CPP_TARGET) $(I_BIN)
	cd $(I_BIN) && $(STRIP) $(CPP_TARGET)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(PERL_TARGET) $(I_BIN)

install_lib: $(I_LIB)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(LIB_TARGET) $(I_LIB)

install_man:
	@$(MAKE) -C man install

$(I_BIN) $(I_LIB):
	$(MKDIR) $@

uninstall:
	cd $(I_BIN) && $(RM) $(TARGET)
	cd $(I_LIB) && $(RM) $(LIB_TARGET)
	@$(MAKE) -C man $@

##
# Utility rules
##

MAKE_SUBDIRS=	for dir in man mod/*; \
		do [ -e $$dir/*akefile ] && $(MAKE) -C $$dir $1; \
		done

clean:
	$(RM) *.o core swish++.index
	@$(call MAKE_SUBDIRS,$@)

distclean: clean
	$(RM) $(TARGET) dep init_modules.c platform.h
	@$(call MAKE_SUBDIRS,$@)

dist: clean
	$(RM) $(TARGET) init_modules.c platform.h
	@$(call MAKE_SUBDIRS,$@)
