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
	tests/index-text-v1.test \
	tests/index-text-v2.test \
	tests/index-text-v3.test \
	tests/index-TitleLines-a.test \
	tests/index-v5.test \
	tests/index-va.test \
	tests/index-WordFilesMax-1.test \
	tests/index-WordFilesMax-a.test \
	tests/index-WordPercentMax-0.test \
	tests/index-WordPercentMax-102.test \
	tests/index-WordPercentMax-a.test \
	tests/search-Fbad.test \
	tests/search-ma.test \
	tests/search-ResultsFormat-bad.test \
	tests/search-text-01.test \
	tests/search-text-and-01.test \
	tests/search-text-and-02.test \
	tests/search-text-d-01.test \
	tests/search-text-D.test \
	tests/search-text-Fclassic.test \
	tests/search-text-Fxml.test \
	tests/search-text-m0.test \
	tests/search-text-m3.test \
	tests/search-text-not-01.test \
	tests/search-text-or-01.test \
	tests/search-text-ResultSeparator-01.test \
	tests/search-text-ResultSeparator-02.test \
	tests/search-text-ResultsFormat-classic.test \
	tests/search-text-ResultsFormat-xml.test \
	tests/search-text-R.test \
	tests/search-text-s-01.test \
	tests/search-text-S.test \
	tests/search-text-w7,4.test \
	tests/search-text-w7.test \
	tests/search-text-wild-01.test

if WITH_WORD_POS
TESTS+=	tests/search-text-near-01.test \
	tests/search-text-near-02.test
endif

if WITH_HTML
TESTS+=	tests/index-H.test \
	tests/index-html-v1.test \
	tests/index-html-v2.test \
	tests/index-html-v3.test \
	tests/search-html-D.test \
	tests/search-html-no_index-01.test \
	tests/index-html-no_index.test \
	tests/search-html-no_index-02.test
endif

if WITH_MAN
TESTS+=	tests/index-man-v1.test \
	tests/index-man-v2.test \
	tests/index-man-v3.test \
	tests/search-man-D.test \
	tests/search-man-meta-01.test \
	tests/search-man-M.test
endif

if WITH_RTF
TESTS+=	tests/index-rtf-v1.test \
	tests/index-rtf-v2.test \
	tests/index-rtf-v3.test \
	tests/search-rtf-D.test \
	tests/search-rtf-rquote-01.test \
	tests/search-rtf-M.test
endif

AM_TESTS_ENVIRONMENT = BUILD_SRC=$(top_builddir)/src; export BUILD_SRC ;
TEST_EXTENSIONS = .sh .test
SH_LOG_DRIVER = $(srcdir)/run_test.sh
TEST_LOG_DRIVER = $(srcdir)/run_test.sh

EXTRA_DIST = run_test.sh tests data expected
dist-hook:
	cd $(distdir)/tests && rm -f *.log *.trs

# vim:set noet sw=8 ts=8:
