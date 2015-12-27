##
#	SWISH++
#	test/Makefile.am
#
#	Copyright (C) 2015  Paul J. Lucas
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the Licence, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

AUTOMAKE_OPTIONS = 1.12			# needed for TEST_LOG_DRIVER

TESTS =	tests/index-no_options.test \
	tests/index-f1.test \
	tests/index-fa.test \
	tests/index-p0.test \
	tests/index-p102.test \
	tests/index-pa.test \
	tests/index-S.test \
	tests/index-ta.test \
	tests/index-TitleLines-a.test \
	tests/index-txt-v1.test \
	tests/index-txt-v2.test \
	tests/index-txt-v3.test \
	tests/index-v5.test \
	tests/index-va.test \
	tests/index-WordFilesMax-1.test \
	tests/index-WordFilesMax-a.test \
	tests/index-WordPercentMax-0.test \
	tests/index-WordPercentMax-102.test \
	tests/index-WordPercentMax-a.test \
	tests/search-txt-01.test \
	tests/search-txt-D.test \
	tests/search-txt-Fbad.test \
	tests/search-txt-Fclassic.test \
	tests/search-txt-Fxml.test \
	tests/search-txt-m0.test \
	tests/search-txt-m3.test \
	tests/search-txt-ma.test \
	tests/search-txt-wild-01.test

if WITH_HTML
TESTS += tests/index-H.test
endif

AM_TESTS_ENVIRONMENT = BUILD_SRC=$(top_builddir)/src; export BUILD_SRC ;
TEST_EXTENSIONS = .sh .test
SH_LOG_DRIVER = $(srcdir)/run_test.sh
TEST_LOG_DRIVER = $(srcdir)/run_test.sh

EXTRA_DIST = run_test.sh tests data expected
dist-hook:
	cd $(distdir)/tests && rm -f *.log *.trs

# vim:set noet sw=8 ts=8:
