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
OTHER_TARGET:=	httpindex searchc searchmonitor splitmail
BIN_TARGET:=	$(CPP_TARGET) $(OTHER_TARGET)
INITD_TARGET:=	searchd
TARGET:=	$(BIN_TARGET) $(INITD_TARGET)

.PHONY: all
all: $(TARGET)

ROOT:=		.
include		$(ROOT)/config/config.mk

DEBUGFLAGS:=
#		Various debug flags; add to DEBUGFLAGS for debugging.
#		-DDEBUG_is_ok_word
#		-DDEBUG_parse_query
#		-DDEBUG_stem_word
#		-DDEBUG_threads
CFLAGS:=	$(CCFLAGS) $(DEBUGFLAGS)

########## You shouldn't have to change anything below this line. #############

SUBDIRS:=	man mod/*

ifdef CHARSET_LIST
SUBDIRS+=		charsets
CHARSET_LIB:=		charsets/libcharsets.a
CHARSET_LIB_PATH:=	-Lcharsets
CHARSET_LINK:=		-lcharsets
endif

ifdef ENCODING_LIST
SUBDIRS+=		encodings
ENCODING_LIB:=		encodings/libencodings.a
ENCODING_LIB_PATH:=	-Lencodings
ENCODING_LINK:=		-lencodings
endif

MOD_LIBS:=		$(foreach mod,$(MOD_LIST),mod/$(mod)/libmod_$(mod).a)
MOD_LIB_PATHS:=		$(foreach mod,$(MOD_LIST),-Lmod/$(mod))
MOD_LINK:=		$(foreach mod,$(MOD_LIST),-lmod_$(mod))

I_SOURCES:=	bcd.c \
		mmap_file.c \
		conf_var.c \
		conf_bool.c \
		conf_filter.c \
		conf_int.c \
		conf_percent.c \
		conf_set.c \
		conf_string.c \
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
		init_mod_vars.c \
		iso8859-1.c \
		itoa.c \
		option_stream.c \
		stop_words.c \
		TempDirectory.c \
		util.c \
		word_info.c \
		word_util.c \
		index.c
ifdef WIN32
I_SOURCES+=	fnmatch.c		# see the comment in pattern_map.h
endif
I_OBJECTS:=	$(I_SOURCES:.c=.o)
I_LINK:=	$(MOD_LINK) $(ENCODING_LINK) $(CHARSET_LINK) $(PTHREAD_LIB) -lm

S_SOURCES:=	bcd.c \
		mmap_file.c \
		conf_var.c \
		conf_bool.c \
		conf_enum.c \
		conf_int.c \
		conf_string.c \
		index_segment.c \
		init_mod_vars.c \
		file_info.c \
		file_list.c \
		iso8859-1.c \
		option_stream.c \
		query.c \
		ResultsFormat.c \
		results_formatter.c \
		classic_formatter.c \
		xml_formatter.c \
		token.c \
		stem_word.c \
		util.c \
		word_util.c \
		search.c
ifdef SEARCH_DAEMON
##
# See the comment in config/config.mk regarding the search daemon ability.
##
S_SOURCES+=	fdbuf.c \
		Group.c \
		thread_pool.c \
		search_thread.c \
		search_daemon.c \
		SearchDaemon.c \
		SocketAddress.c \
		User.c
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
		ExcludeFile.c \
		ExtractFile.c \
		filter.c \
		index_segment.c \
		init_mod_vars.c \
		iso8859-1.c \
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

index: $(I_OBJECTS) $(CHARSET_LIB) $(ENCODING_LIB) $(MOD_LIBS)
	$(CC) $(CFLAGS) $(CHARSET_LIB_PATH) $(ENCODING_LIB_PATH) \
		$(MOD_LIB_PATHS) -o $@ $(I_OBJECTS) $(I_LINK)

init_modules.c: mod/*/mod_*.h init_modules-sh
	./init_modules-sh > $@ || $(RM) $@

init_mod_vars.c: mod/*/vars init_mod_vars-sh
	./init_mod_vars-sh > $@ || $(RM) $@

search: $(S_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(S_LINK)

$(CHARSET_LIB): FORCE
	@$(MAKE) -C $(dir $@) DEBUGFLAGS="$(DEBUGFLAGS)"

$(ENCODING_LIB): FORCE
	@$(MAKE) -C $(dir $@) DEBUGFLAGS="$(DEBUGFLAGS)"

$(MOD_LIBS): FORCE
	@$(MAKE) -C $(dir $@) DEBUGFLAGS="$(DEBUGFLAGS)"

.PHONY: FORCE

# Don't do the "include" if the goal contains the word "clean," i.e., either
# the "clean" or "distclean" goal.
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
include $(I_SOURCES:%.c=.%.d) $(E_SOURCES:%.c=.%.d) $(S_SOURCES:%.c=.%.d)
endif

ps pdf txt:
	@$(MAKE) -C man $@

##
# Install rules
##

INITD_DIR:= $(firstword $(shell ls -d /etc/init.d /etc/rc.d/init.d 2>/dev/null))
LEVEL_DIR:= $(firstword $(shell ls -d /etc/rc3.d /etc/rc.d/rc3.d 2>/dev/null))

install: install_bin install_lib install_man install_conf

install_bin: $(BIN_TARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(BIN_TARGET) $(I_BIN)
	cd $(I_BIN) && $(STRIP) $(CPP_TARGET)

install_lib: $(I_LIB)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(LIB_TARGET) $(I_LIB)

install_man:
	@$(MAKE) -C man install

install_conf:
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) swish++.conf $(I_ETC)

$(I_BIN) $(I_ETC) $(I_LIB):
	$(MKDIR) $@

install_sysv: $(INITD_TARGET)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(INITD_TARGET) $(INITD_DIR)
	$(RM) $(LEVEL_DIR)/[SK]99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(LEVEL_DIR)/S99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(LEVEL_DIR)/K99$(INITD_TARGET)

uninstall:
	cd $(I_BIN) && $(RM) $(BIN_TARGET)
	cd $(I_LIB) && $(RM) $(LIB_TARGET)
	$(RM) $(INITD_DIR)/$(INITD_TARGET) $(LEVEL_DIR)/[SK]99$(INITD_TARGET)
	@$(MAKE) -C man $@

##
# Utility rules
##

MAKE_SUBDIRS=	for dir in $(SUBDIRS); \
		do [ -f $$dir/*akefile ] && $(MAKE) -C $$dir $1; \
		done

clean:
	$(RM) *.o core swish++.index
	@$(call MAKE_SUBDIRS,$@)

distclean: clean
	$(RM) $(TARGET) .*.d init_modules.c platform.h
	@$(call MAKE_SUBDIRS,$@)
