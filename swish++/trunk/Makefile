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

ROOT=		.
include		$(ROOT)/config/config.mk

CFLAGS=		$(CCFLAGS)
#		Various debug flags; add to CFLAGS for debugging.
#		-DDEBUG_is_ok_word
#		-DDEBUG_parse_class
#		-DDEBUG_parse_query
#		-DDEBUG_stem_word

########## You shouldn't have to change anything below this line. #############

CTARGET=	index search extract

I_LINK=		$(CCLINK) -lm
I_OBJS=		conf_var$(CCOEXT) \
		conf_bool$(CCOEXT) \
		conf_int$(CCOEXT) \
		conf_set$(CCOEXT) \
		conf_string$(CCOEXT) \
		directory$(CCOEXT) \
		elements$(CCOEXT) \
		entities$(CCOEXT) \
		file_vector$(CCOEXT) \
		file_info$(CCOEXT) \
		filter$(CCOEXT) \
		FilterExtension$(CCOEXT) \
		word_index$(CCOEXT) \
		file_list$(CCOEXT) \
		html$(CCOEXT) \
		stop_words$(CCOEXT) \
		util$(CCOEXT) \
		index$(CCOEXT)

S_LINK=		$(CCLINK)
S_OBJS=		conf_var$(CCOEXT) \
		conf_bool$(CCOEXT) \
		conf_int$(CCOEXT) \
		conf_string$(CCOEXT) \
		file_vector$(CCOEXT) \
		file_index$(CCOEXT) \
		word_index$(CCOEXT) \
		file_list$(CCOEXT) \
		token$(CCOEXT) \
		stem_word$(CCOEXT) \
		util$(CCOEXT) \
		search$(CCOEXT)

E_LINK=		$(CCLINK)
E_OBJS=		conf_var$(CCOEXT) \
		conf_bool$(CCOEXT) \
		conf_int$(CCOEXT) \
		conf_set$(CCOEXT) \
		conf_string$(CCOEXT) \
		directory$(CCOEXT) \
		entities$(CCOEXT) \
		file_vector$(CCOEXT) \
		filter$(CCOEXT) \
		FilterExtension$(CCOEXT) \
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

conf_bool$(CCOEXT): \
	conf_bool$(CCHEXT) \
		conf_var$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT) \
	exit_codes$(CCHEXT) \
	util$(CCHEXT)


conf_int$(CCOEXT): \
	conf_int$(CCHEXT) \
		conf_var$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT) \
	exit_codes$(CCHEXT) \
	util$(CCHEXT)

conf_set$(CCOEXT): \
	conf_set$(CCHEXT) \
		conf_var$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT) \
		my_set$(CCHEXT)

conf_string$(CCOEXT): \
	conf_string$(CCHEXT) \
		conf_var$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT) \
	exit_codes$(CCHEXT)

conf_var$(CCOEXT): \
	conf_var$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H) \
		less$(CCHEXT) \
	util$(CCHEXT)

directory$(CCOEXT): \
	directory$(CCHEXT) \
		FollowLinks$(CCHEXT) \
			conf_bool$(CCHEXT) \
				conf_var$(CCHEXT) \
					fake_ansi$(CCHEXT) \
						$(PLATFORM_H) \
					less$(CCHEXT) \
	RecurseSubdirs$(CCHEXT) \
	util$(CCHEXT) \
	Verbosity$(CCHEXT) \
		conf_int$(CCHEXT)

elements$(CCOEXT): \
	elements$(CCHEXT) \
		my_set$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT)

entities$(CCOEXT): \
	entities$(CCHEXT) \
		less$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H)

extract$(CCOEXT): \
	config$(CCHEXT) \
	directory$(CCHEXT) \
		FollowLinks$(CCHEXT) \
			conf_bool$(CCHEXT) \
				conf_var$(CCHEXT) \
					fake_ansi$(CCHEXT) \
						$(PLATFORM_H) \
					less$(CCHEXT) \
	do_file$(CCCEXT) \
	ExcludeExtension$(CCHEXT) \
		conf_set$(CCHEXT) \
			my_set$(CCHEXT) \
	exit_codes$(CCHEXT) \
	file_vector$(CCHEXT) \
	FilterExtension$(CCHEXT) \
		filter$(CCHEXT) \
	IncludeExtension$(CCHEXT) \
	$(PLATFORM_H) \
	postscript$(CCHEXT) \
	RecurseSubdirs$(CCHEXT) \
	StopWordFile$(CCHEXT) \
		conf_string$(CCHEXT) \
	stop_words$(CCHEXT) \
	util$(CCHEXT) \
	Verbosity$(CCHEXT) \
		conf_int$(CCHEXT) \
	version$(CCHEXT)

file_index$(CCOEXT): \
	file_index$(CCHEXT) \
		file_vector$(CCHEXT) \
	util$(CCHEXT) \
		config$(CCHEXT) \
		$(PLATFORM_H)

file_info$(CCOEXT): \
	config$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_info$(CCHEXT) \
	FilesReserve$(CCHEXT) \
		conf_int$(CCHEXT) \
			conf_var$(CCHEXT) \
				less$(CCHEXT)

file_list$(CCOEXT): \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_list$(CCHEXT) \
		word_index$(CCHEXT) \
			file_vector$(CCHEXT) \
		word_info$(CCHEXT) \
			html$(CCHEXT) \
			my_set$(CCHEXT) \
				less$(CCHEXT)

file_vector$(CCOEXT): \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_vector$(CCHEXT)

filter$(CCOEXT): \
	config$(CCHEXT) \
	filter$(CCHEXT)

FilterExtension$(CCOEXT): \
	exit_codes$(CCHEXT) \
	FilterExtension$(CCHEXT) \
		conf_var$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
		filter$(CCHEXT) \
		less$(CCHEXT)

html$(CCOEXT): \
	config$(CCHEXT) \
	elements$(CCHEXT) \
	entities$(CCHEXT) \
		less$(CCHEXT) \
			fake_ansi$(CCHEXT) \
	ExcludeClass$(CCHEXT) \
		conf_set$(CCHEXT) \
			conf_var$(CCHEXT) \
				my_set$(CCHEXT) \
	ExcludeMeta$(CCHEXT) \
	IncludeMeta$(CCHEXT) \
	html$(CCHEXT) \
		file_vector$(CCHEXT) \
	index$(CCHEXT) \
	$(PLATFORM_H) \
	meta_map$(CCHEXT) \
	TitleLines$(CCHEXT) \
		conf_int$(CCHEXT) \
	util$(CCHEXT)

index$(CCOEXT): \
	config$(CCHEXT) \
	directory$(CCHEXT) \
		FollowLinks$(CCHEXT) \
			conf_bool$(CCHEXT) \
				conf_var$(CCHEXT) \
					fake_ansi$(CCHEXT) \
					less$(CCHEXT) \
	do_file$(CCCEXT) \
	ExcludeClass$(CCHEXT) \
		conf_set$(CCHEXT) \
			my_set$(CCHEXT) \
	ExcludeExtension$(CCHEXT) \
	ExcludeMeta$(CCHEXT) \
	exit_codes$(CCHEXT) \
	FilesReserve$(CCHEXT) \
		conf_int$(CCHEXT) \
	file_info$(CCHEXT) \
	file_list$(CCHEXT) \
		fake_ansi$(CCHEXT) \
	file_vector$(CCHEXT) \
	FilterExtension$(CCHEXT) \
		filter$(CCHEXT) \
	html$(CCHEXT) \
	IncludeExtension$(CCHEXT) \
	IncludeMeta$(CCHEXT) \
	index$(CCHEXT) \
	IndexFile$(CCHEXT) \
		conf_string$(CCHEXT) \
	meta_map$(CCHEXT) \
	$(PLATFORM_H) \
	RecurseSubdirs$(CCHEXT) \
	StopWordFile$(CCHEXT) \
	stop_words$(CCHEXT) \
	TempDirectory$(CCHEXT) \
	TitleLines$(CCHEXT) \
	util$(CCHEXT) \
	Verbosity$(CCHEXT) \
	version$(CCHEXT) \
	WordFilesMax$(CCHEXT) \
	WordPercentMax$(CCHEXT) \
	word_index$(CCHEXT) \
	word_info$(CCHEXT)

postscript$(CCOEXT): \
	postscript$(CCHEXT) \
		my_set$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT)

search$(CCOEXT): \
	config$(CCHEXT) \
	exit_codes$(CCHEXT) \
	file_index$(CCHEXT) \
		file_vector$(CCHEXT) \
	file_list$(CCHEXT) \
	html$(CCHEXT) \
	IndexFile$(CCHEXT) \
		conf_string$(CCHEXT) \
			conf_var$(CCHEXT) \
				fake_ansi$(CCHEXT) \
	less$(CCHEXT) \
	my_set$(CCHEXT) \
	$(PLATFORM_H) \
	ResultsMax$(CCHEXT) \
		conf_int$(CCHEXT) \
	stem_word$(CCHEXT) \
	StemWords$(CCHEXT) \
		conf_bool$(CCHEXT) \
	token$(CCHEXT) \
	util$(CCHEXT) \
	version$(CCHEXT) \
	word_index$(CCHEXT)

stem_word$(CCOEXT): \
	less$(CCHEXT) \
		fake_ansi$(CCHEXT) \
			$(PLATFORM_H) \
	util$(CCHEXT)

stop_words$(CCOEXT): \
	config$(CCHEXT) \
	exit_codes$(CCHEXT) \
	file_vector$(CCHEXT) \
	stop_words$(CCHEXT) \
		my_set$(CCHEXT) \
			fake_ansi$(CCHEXT) \
				$(PLATFORM_H) \
			less$(CCHEXT) \
	util$(CCHEXT)

token$(CCOEXT): \
	config$(CCHEXT) \
	$(PLATFORM_H) \
	fake_ansi$(CCHEXT) \
	token$(CCHEXT) \
	util$(CCHEXT) \
		file_vector$(CCHEXT)

util$(CCOEXT): \
	config$(CCHEXT) \
	conf_var$(CCHEXT) \
		less$(CCHEXT) \
	exit_codes$(CCHEXT) \
	fake_ansi$(CCHEXT) \
		$(PLATFORM_H) \
	file_vector$(CCHEXT) \
	util$(CCHEXT)

word_index$(CCOEXT): \
	util$(CCHEXT) \
		file_vector$(CCHEXT) \
		$(PLATFORM_H) \
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
