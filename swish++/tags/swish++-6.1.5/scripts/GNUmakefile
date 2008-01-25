##
#	SWISH++
#	scripts/GNUmakefile
#
#	Copyright (C) 2004  Paul J. Lucas
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

TARGET:=	$(patsubst %.in,%,$(wildcard *.in))

.PHONY: all
all: $(TARGET)

ROOT:=		..
include		$(ROOT)/config/config.mk

##
# Install rules
##

install: install_bin

install_bin: $(TARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(TARGET) $(I_BIN)

$(I_BIN):
	$(MKDIR) $@

uninstall:
	cd $(I_BIN) && $(RM) $(TARGET)

##
# Utility rules
##

clean:

distclean: clean
	$(RM) $(TARGET)
