# Process this file with autoconf to produce a configure script.

##
#     SWISH++
#     configure.ac
#
#     Copyright (C) 1998-2015  Paul J. Lucas
#
#     This program is free software; you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation; either version 2 of the Licence, or
#     (at your option) any later version.
#     
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#     
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

AC_INIT([SWISH++], [7.0], [paul@lucasmail.org],,[https://github.com/paul-j-lucas/swishxx])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
gl_EARLY
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([z], [uncompress])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sysexits.h])
AC_CHECK_HEADERS([unistd.h])
AC_HEADER_ASSERT
gl_INIT

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MMAP
AC_CHECK_FUNCS([madvise select socket strchr strrchr])

# Program feature: Search Daemon (--disable-daemon)
AC_MSG_CHECKING([whether to enable the search daemon])
AC_ARG_ENABLE([daemon],
  [AS_HELP_STRING([--disable-daemon], [disable search daemon])],
  [],
  [enable_daemon="yes"]
)
AC_MSG_RESULT([$enable_daemon])
AS_IF([test x$enable_daemon = xyes],
  [AC_DEFINE([WITH_SEARCH_DAEMON],[1],[with search daemon])
   AC_DEFINE([MULTI_THREADED],[1],[multithreaded])]
)

# Program feature: Base64 Decoding (--disable-base64)
AC_MSG_CHECKING([whether to enable Base64 support])
AC_ARG_ENABLE([base64],
  [AS_HELP_STRING([--disable-base64], [disable Base64 support])],
  [],
  [enable_base64="yes" enable_decoding="yes"]
)
AC_MSG_RESULT([$enable_base64])
AS_IF([test x$enable_base64 = xyes],
  [AC_DEFINE([WITH_BASE64],[1],[with Base64 support])
   AC_DEFINE([WITH_DECODING],[1],[with decoding])]
)

# Program feature: Quoted-Printable Decoding (--disable-quoted-printable)
AC_MSG_CHECKING([whether to enable quoted-printable support])
AC_ARG_ENABLE([quoted-printable],
  [AS_HELP_STRING([--disable-quoted-printable], [disable quoted-printable support])],
  [],
  [enable_quoted_printable="yes" enable_decoding="yes"]
)
AC_MSG_RESULT([$enable_quoted_printable])
AS_IF([test x$enable_quoted_printable = xyes],
  [AC_DEFINE([WITH_QUOTED_PRINTABLE],[1],[with quoted-printable support])
   AC_DEFINE([WITH_DECODING],[1],[with decoding])]
)

# Program feature: UTF-7 Decoding (--disable-utf7)
AC_MSG_CHECKING([whether to enable UTF-7 support])
AC_ARG_ENABLE([utf7],
  [AS_HELP_STRING([--disable-utf7], [disable UTF-7 support])],
  [],
  [enable_utf7="yes" enable_decoding="yes"]
)
AC_MSG_RESULT([$enable_utf7])
AS_IF([test x$enable_utf7 = xyes],
  [AC_DEFINE([WITH_UTF7],[1],[with UTF-7 support])
   AC_DEFINE([WITH_DECODING],[1],[with decoding])]
)

# Program feature: UTF-8 Decoding (--disable-utf8)
AC_MSG_CHECKING([whether to enable UTF-8 support])
AC_ARG_ENABLE([utf8],
  [AS_HELP_STRING([--disable-utf8], [disable UTF-8 support])],
  [],
  [enable_utf8="yes" enable_decoding="yes"]
)
AC_MSG_RESULT([$enable_utf8])
AS_IF([test x$enable_utf8 = xyes],
  [AC_DEFINE([WITH_UTF8],[1],[with UTF-8 support])
   AC_DEFINE([WITH_DECODING],[1],[with decoding])]
)

# Program feature: UTF-16 Decoding (--disable-utf16)
AC_MSG_CHECKING([whether to enable UTF-16 support])
AC_ARG_ENABLE([utf16],
  [AS_HELP_STRING([--disable-utf16], [disable UTF-16 support])],
  [],
  [enable_utf16="yes" enable_decoding="yes"]
)
AC_MSG_RESULT([$enable_utf16])
AS_IF([test x$enable_utf16 = xyes],
  [AC_DEFINE([WITH_UTF16],[1],[with UTF-16 support])
   AC_DEFINE([WITH_DECODING],[1],[with decoding])]
)

# Program feature: Word Position Data (--disable-word-pos)
AC_MSG_CHECKING([whether to enable word position data])
AC_ARG_ENABLE([word_pos],
  [AS_HELP_STRING([--disable-word-pos], [disable word position data])],
  [],
  [enable_word_pos="yes"]
)
AC_MSG_RESULT([$enable_word_pos])
AS_IF([test x$enable_word_pos = xyes],
  [AC_DEFINE([WITH_WORD_POS],[1],[with word position data])]
)

# Program feature: HTML Indexing (--disable-html)
AC_MSG_CHECKING([whether to enable HTML indexing])
AC_ARG_ENABLE([html],
  [AS_HELP_STRING([--disable-html], [disable HTML indexing])],
  [],
  [enable_html="yes"]
)
AC_MSG_RESULT([$enable_html])
AS_IF([test x$enable_html = xyes],
  [AC_DEFINE([WITH_HTML],[1],[with HTML indexing])]
)

# Program feature: ID3 Indexing (--disable-id3)
AC_MSG_CHECKING([whether to enable ID3 (music file) indexing])
AC_ARG_ENABLE([id3],
  [AS_HELP_STRING([--disable-id3], [disable ID3 indexing])],
  [],
  [enable_id3="yes"]
)
AC_MSG_RESULT([$enable_id3])
AS_IF([test x$enable_id3 = xyes],
  [AC_DEFINE([WITH_ID3],[1],[with ID3 indexing])]
)

# Program feature: LaTeX Indexing (--disable-latex)
AC_MSG_CHECKING([whether to enable LaTeX indexing])
AC_ARG_ENABLE([latex],
  [AS_HELP_STRING([--disable-latex], [disable LaTex indexing])],
  [],
  [enable_latex="yes"]
)
AC_MSG_RESULT([$enable_latex])
AS_IF([test x$enable_latex = xyes],
  [AC_DEFINE([WITH_LATEX],[1],[with LaTeX indexing])]
)

# Program feature: Mail Indexing (--disable-mail)
AC_MSG_CHECKING([whether to enable mail indexing])
AC_ARG_ENABLE([mail],
  [AS_HELP_STRING([--disable-mail], [disable mail indexing])],
  [],
  [enable_mail="yes"]
)
AC_MSG_RESULT([$enable_mail])
AS_IF([test x$enable_mail = xyes],
  [AC_DEFINE([WITH_MAIL],[1],[with mail indexing])]
)

# Program feature: Man(ual page) Indexing (--disable-man)
AC_MSG_CHECKING([whether to enable man(ual page) indexing])
AC_ARG_ENABLE([man],
  [AS_HELP_STRING([--disable-man], [disable man indexing])],
  [],
  [enable_man="yes"]
)
AC_MSG_RESULT([$enable_man])
AS_IF([test x$enable_man = xyes],
  [AC_DEFINE([WITH_MAN],[1],[with man indexing])]
)

# Program feature: RTF Indexing (--disable-rtf)
AC_MSG_CHECKING([whether to enable RTF (Rich Text Format) indexing])
AC_ARG_ENABLE([rtf],
  [AS_HELP_STRING([--disable-rtf], [disable RTF indexing])],
  [],
  [enable_rtf="yes"]
)
AC_MSG_RESULT([$enable_rtf])
AS_IF([test x$enable_rtf = xyes],
  [AC_DEFINE([WITH_RTF],[1],[with RTF indexing])]
)

# Makefile conditionals.
AM_CONDITIONAL([MULTI_THREADED], [test x$enable_daemon = xyes])
AM_CONDITIONAL([WITH_BASE64], [test x$enable_base64 = xyes])
AM_CONDITIONAL([WITH_DECODING], [test x$enable_decoding = xyes])
AM_CONDITIONAL([WITH_HTML], [test x$enable_html = xyes])
AM_CONDITIONAL([WITH_ID3], [test x$enable_id3 = xyes])
AM_CONDITIONAL([WITH_LATEX], [test x$enable_latex = xyes])
AM_CONDITIONAL([WITH_MAIL], [test x$enable_mail = xyes])
AM_CONDITIONAL([WITH_MAN], [test x$enable_man = xyes])
AM_CONDITIONAL([WITH_QUOTED_PRINTABLE], [test x$enable_quoted_printable = xyes])
AM_CONDITIONAL([WITH_RTF], [test x$enable_rtf = xyes])
AM_CONDITIONAL([WITH_SEARCH_DAEMON], [test x$enable_daemon = xyes])
AM_CONDITIONAL([WITH_UTF16], [test x$enable_utf16 = xyes])
AM_CONDITIONAL([WITH_UTF7], [test x$enable_utf7 = xyes])
AM_CONDITIONAL([WITH_UTF8], [test x$enable_utf8 = xyes])

# Miscellaneous.
AC_LANG(C++)
AX_CFLAGS_WARN_ALL
AX_CHECK_COMPILE_FLAG([-Wextra],[CFLAGS="$CFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wc++-compat],[CFLAGS="$CFLAGS -Wc++-compat"])
AX_CHECK_COMPILE_FLAG([-Wredundant-decls],[CFLAGS="$CFLAGS -Wredundant-decls"])
AX_CHECK_COMPILE_FLAG([-Wwrite-strings],[CFLAGS="$CFLAGS -Wwrite-strings"])

# Generate files.
AH_TOP([#ifndef config_H
#define config_H])
AH_BOTTOM([#endif /* config_H */])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
    Makefile
    lib/Makefile
    src/Makefile
    src/pjl/Makefile
    src/charsets/Makefile
    src/encodings/Makefile
    src/mod/Makefile
    src/mod/html/Makefile
    src/mod/id3/Makefile
    src/mod/latex/Makefile
    src/mod/mail/Makefile
    src/mod/man/Makefile
    src/mod/rtf/Makefile
    test/Makefile
    man/Makefile
    man/man1/Makefile
    man/man4/Makefile
    man/man8/Makefile
])
AC_OUTPUT

# vim:set et sw=2 ts=2:
