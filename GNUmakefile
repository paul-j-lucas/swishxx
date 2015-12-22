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

CPP_TARGET:=	src/index src/search src/extract
BIN_TARGET:=	$(CPP_TARGET)
SCRIPTS:=	$(patsubst %.in,%,$(wildcard scripts/*.in))
INITD_TARGET:=	searchd
TARGET:=	$(BIN_TARGET) $(SCRIPTS)

.PHONY: all
all: $(TARGET)

ROOT:=		.
include		$(ROOT)/config/config.mk

########## You shouldn't have to change anything below this line. #############

SUBDIRS:=	config man scripts src

LIB_TARGET:=	WWW.pm

##
# Build rules
##

src: FORCE
	@$(MAKE) -C src

$(SCRIPTS): FORCE
	@$(MAKE) -C $(dir $@)

.PHONY: FORCE

ps pdf txt:
	@$(MAKE) -C man $@

##
# Install rules
##

INITD_DIR:= $(firstword $(shell ls -d /etc/init.d /etc/rc.d/init.d 2>/dev/null))
LEVEL_DIR=  $(firstword $(shell ls -d /etc/rc$1.d /etc/rc.d/rc$1.d 2>/dev/null))

install: install_bin install_lib install_man install_conf

install_bin: $(BIN_TARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(BIN_TARGET) $(I_BIN)
	cd $(I_BIN) && $(STRIP) $(CPP_TARGET)
	@$(MAKE) -C scripts $@

install_lib: $(I_LIB)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(LIB_TARGET) $(I_LIB)

install_man:
	@$(MAKE) -C man install

install_conf: $(I_ETC)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) swish++.conf $(I_ETC)

$(I_BIN) $(I_ETC) $(I_LIB):
	$(MKDIR) $@

install_sysv: scripts/$(INITD_TARGET)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $< $(INITD_DIR)
	$(RM) $(call LEVEL_DIR,1)/K99$(INITD_TARGET)
	$(RM) $(call LEVEL_DIR,2)/K99$(INITD_TARGET)
	$(RM) $(call LEVEL_DIR,3)/S99$(INITD_TARGET)
	$(RM) $(call LEVEL_DIR,5)/S99$(INITD_TARGET)
	$(RM) $(call LEVEL_DIR,6)/K99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(call LEVEL_DIR,1)/K99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(call LEVEL_DIR,2)/K99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(call LEVEL_DIR,3)/S99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(call LEVEL_DIR,5)/S99$(INITD_TARGET)
	ln -s ../init.d/$(INITD_TARGET) $(call LEVEL_DIR,6)/K99$(INITD_TARGET)

uninstall:
	cd $(I_BIN) && $(RM) $(BIN_TARGET)
	cd $(I_LIB) && $(RM) $(LIB_TARGET)
	$(RM) $(INITD_DIR)/$(INITD_TARGET) \
		$(call LEVEL_DIR,1)/K99$(INITD_TARGET) \
		$(call LEVEL_DIR,2)/K99$(INITD_TARGET) \
		$(call LEVEL_DIR,3)/S99$(INITD_TARGET) \
		$(call LEVEL_DIR,5)/S99$(INITD_TARGET) \
		$(call LEVEL_DIR,6)/K99$(INITD_TARGET)
	@$(MAKE) -C man $@
	@$(MAKE) -C scripts $@

##
# Utility rules
##

MAKE_SUBDIRS=	for dir in $(SUBDIRS); \
		do [ -f $$dir/*akefile ] && $(MAKE) -C $$dir $1; \
		done

clean:
	@$(call MAKE_SUBDIRS,$@)

distclean: clean
	@$(call MAKE_SUBDIRS,$@)

dist: distclean
	@if [ -d .svn ]; then \
	echo; echo "Won't 'make $@' in source tree!"; echo; exit 1; \
	fi
	$(RM) *.xcode*

# vim:set noet sw=8 ts=8:
