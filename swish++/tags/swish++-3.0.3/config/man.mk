##
#	SWISH++
#	config/man.mk
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

##
# Build rules
##

.SUFFIXES:
.SUFFIXES: .$(SECT) .pdf .ps .txt

.$(SECT).txt:
	$(TBL) $< | $(NROFF) $(NROFF_FLAGS) | $(TO_TXT) > $@

.$(SECT).ps:
	$(TBL) $< | $(TROFF) $(TROFF_FLAGS) | $(TO_PS) > $@

.ps.pdf:
	$(DISTILL) $<

text:: $(TARGET_TXT)
pdf:: $(TARGET_PDF)
ps:: $(TARGET_PS)

all:: text pdf ps

##
# Install rules
##

install:: $(I_MAN)/man$(SECT)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(PAGES) $?

$(I_MAN)/man$(SECT):
	$(MKDIR) $@

deinstall::
	cd $(I_MAN)/man$(SECT) && $(RM) $(PAGES)

##
# Utility rules
##

clean distclean::
	$(RM) $(TARGET_TXT) $(TARGET_PDF) $(TARGET_PS)

dist::
	$(MAKE) text pdf
	$(RM) $(TARGET_PS)
