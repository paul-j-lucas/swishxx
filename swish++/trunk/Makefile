##
#	SWISH++
#	Makefile
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

########## You shouldn't have to change anything below this line. #############

ROOT=		.
include		$(ROOT)/config/config.mk

CTARGET=	index search extract
CFLAGS=		$(CCFLAGS) -DFEATURE_CLASS # -DDEBUG_parse_class -DDEBUG_parse_query -DDEBUG_is_ok_word -DDEBUG_stem_word

I_LINK=		-lm
I_OBJS=		directory$(CCOEXT) \
		elements$(CCOEXT) \
		entities$(CCOEXT) \
		file_vector$(CCOEXT) \
		file_info$(CCOEXT) \
		word_index$(CCOEXT) \
		file_list$(CCOEXT) \
		html$(CCOEXT) \
		stop_words$(CCOEXT) \
		util$(CCOEXT) \
		index$(CCOEXT)

S_LINK=		
S_OBJS=		file_vector$(CCOEXT) \
		file_index$(CCOEXT) \
		word_index$(CCOEXT) \
		file_list$(CCOEXT) \
		token$(CCOEXT) \
		search$(CCOEXT) \
		stem_word$(CCOEXT) \
		util$(CCOEXT)

E_LINK=
E_OBJS=		directory$(CCOEXT) \
		entities$(CCOEXT) \
		ext_proc$(CCOEXT) \
		file_vector$(CCOEXT) \
		postscript$(CCOEXT) \
		stop_words$(CCOEXT) \
		util$(CCOEXT) \
		extract$(CCOEXT)

LTARGET=	WWW.pm
VARS=		I_BIN="$(I_BIN)" I_LIB="$(I_LIB)" PERL="$(PERL)"

PTARGET=	httpindex

BTARGET=	$(CTARGET) $(PTARGET)

##
# Build rules
##

all:: $(BTARGET)

extract: $(E_OBJS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(E_OBJS) $(E_LINK)

index: $(I_OBJS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(I_OBJS) $(I_LINK)

search: $(S_OBJS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(S_OBJS) $(S_LINK)

directory$(CCOEXT): \
	directory$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H)

elements$(CCOEXT): \
	elements$(CCHEXT) \
		my_set$(CCHEXT) \
			fake_ansi$(CCHEXT) \
			less$(CCHEXT)

entities$(CCOEXT): \
	entities$(CCHEXT) \
		less$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H)

ext_proc$(CCOEXT): \
	config$(CCHEXT) \
	ext_proc$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H) \
		less$(CCHEXT)

extract$(CCOEXT): \
	config$(CCHEXT) \
	directory$(CCHEXT) \
	ext_proc$(CCHEXT) \
		less$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_vector$(CCHEXT) \
	my_set$(CCHEXT) \
	postscript$(CCHEXT) \
	stop_words$(CCHEXT) \
	util$(CCHEXT) \
	version$(CCHEXT)

file_index$(CCOEXT): \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_index$(CCHEXT) \
		file_vector$(CCHEXT) \
	util$(CCHEXT) \
		file_vector$(CCHEXT)

file_info$(CCOEXT): \
	config$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_info$(CCHEXT)

file_list$(CCOEXT): \
	file_list$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H) \
		word_index$(CCHEXT) \
			file_vector$(CCHEXT) \
		word_info$(CCHEXT) \
			html$(CCHEXT) \
			my_set$(CCHEXT) \
				less$(CCHEXT)

file_vector$(CCOEXT): \
	file_vector$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H)

html$(CCOEXT): \
	config$(CCHEXT) \
	elements$(CCHEXT) \
		my_set$(CCHEXT) \
			less$(CCHEXT) \
				fake_ansi$(CCHEXT) \
					$(PLATFORM_H) \
	entities$(CCHEXT) \
	html$(CCHEXT) \
		file_vector$(CCHEXT) \
	index$(CCHEXT) \
	meta_map$(CCHEXT) \
	util$(CCHEXT)

index$(CCOEXT): \
	config$(CCHEXT) \
	directory$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_info$(CCHEXT) \
	file_list$(CCHEXT) \
	file_vector$(CCHEXT) \
	index$(CCHEXT) \
		html$(CCHEXT) \
	meta_map$(CCHEXT) \
		less$(CCHEXT) \
	my_set$(CCHEXT) \
	stop_words$(CCHEXT) \
	util$(CCHEXT) \
	version$(CCHEXT) \
	word_info$(CCHEXT) \
	word_index$(CCHEXT)

postscript$(CCOEXT): \
	postscript$(CCHEXT) \
		my_set$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT)

search$(CCOEXT): \
	config$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_index$(CCHEXT) \
		file_vector$(CCHEXT) \
	file_list$(CCHEXT) \
	html$(CCHEXT) \
	less$(CCHEXT) \
	my_set$(CCHEXT) \
	stem_word$(CCHEXT) \
	token$(CCHEXT) \
	util$(CCHEXT) \
	version$(CCHEXT) \
	word_index$(CCHEXT)

stem_word$(CCOEXT): \
	util$(CCHEXT) \
	config$(CCHEXT) \
	file_vector$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H)

stop_words$(CCOEXT): \
	config$(CCHEXT) \
	file_vector$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H) \
	stop_words$(CCHEXT) \
		my_set$(CCHEXT) \
			less$(CCHEXT) \
	util$(CCHEXT)

token$(CCOEXT): \
	config$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	token$(CCHEXT) \
	util$(CCHEXT) \
		file_vector$(CCHEXT)

util$(CCOEXT): \
	config$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	util$(CCHEXT) \
		file_vector$(CCHEXT)

word_index$(CCOEXT): \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	util$(CCHEXT) \
		file_vector$(CCHEXT) \
	word_index$(CCHEXT)

$(PLATFORM_H):
	@cd config && $(MAKE)

httpindex: httpindex.in
	$(PERL) $(CONFIG)/config.pl $@ $(VARS)

##
# Install rules
##

install: install_bin install_lib install_man

install_bin: $(BTARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(CTARGET) $(I_BIN)
	cd $(I_BIN) && $(STRIP) $(CTARGET)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(PTARGET) $(I_BIN)

install_lib: $(I_LIB)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(LTARGET) $(I_LIB)

install_man::
	@cd man && $(MAKE) install

$(I_BIN) $(I_LIB):
	$(MKDIR) $@

uninstall::
	cd $(I_BIN) && $(RM) $(BTARGET)
	cd $(I_LIB) && $(RM) $(LTARGET)
	@cd man && $(MAKE) $@

##
# Utility rules
##

clean::
	$(RM) *$(CCOEXT) core the.index

dist distclean:: clean
	$(RM) $(BTARGET) $(PLATFORM_H)
	@cd man && $(MAKE) $@
