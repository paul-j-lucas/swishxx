##
#	SWISH++
#	config/mod.mk
#
#	Copyright (C) 2001  Paul J. Lucas
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

.PHONY: all
all: $(TARGET)

include		$(ROOT)/config/config.mk

INCLUDES:=	-I$(ROOT)
CFLAGS:=	$(CCFLAGS) $(DEBUGFLAGS) $(INCLUDES)
SOURCES:=	$(wildcard *.c)

##
# Build rules
##

$(TARGET): $(SOURCES:.c=.o)
	$(RM) $@
	$(AR) $@ $^
	-$(RANLIB) $@

# Don't do the "include" if the goal contains the word "clean," i.e., either
# the "clean" or "distclean" goal.
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
-include $(SOURCES:%.c=.%.d)
endif

##
# Utility rules
##

clean:
	$(RM) *.o $(TEMPLATE_REPOSITORY)

distclean: clean
	$(RM) $(TARGET) .*.d

# vim:set noet sw=8 ts=8:
