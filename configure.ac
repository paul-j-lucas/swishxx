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

AC_INIT([swish++], [7.0], [paul@lucasmail.org],,[https://github.com/paul-j-lucas/swish++])
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

# Program features.
AC_MSG_CHECKING([whether to enable the search daemon])
AC_ARG_ENABLE([daemon],
  [AS_HELP_STRING([--disable-daemon], [disable search daemon])],
  [],
  [enable_daemon="yes"]
)
AC_MSG_RESULT([$enable_daemon])
AS_IF([test "x$enable_daemon" = "xyes"],
  [AC_DEFINE([WITH_SEARCH_DAEMON],[1],[with search daemon]),
   AC_DEFINE([MULTI_THREADED],[1],[multithreaded])]
)

AC_MSG_CHECKING([whether to enable Base64 support])
AC_ARG_ENABLE([base64],
  [AS_HELP_STRING([--disable-base64], [disable Base64 support])],
  [],
  [enable_base64="yes"]
)
AC_MSG_RESULT([$enable_base64])
AS_IF([test "x$enable_base64" = "xyes"],
  [AC_DEFINE([WITH_BASE64],[1],[with Base64 support])]
)

AC_MSG_CHECKING([whether to enable quoted-printable support])
AC_ARG_ENABLE([quoted-printable],
  [AS_HELP_STRING([--disable-quoted-printable], [disable quoted-printable support])],
  [],
  [enable_quoted_printable="yes"]
)
AC_MSG_RESULT([$enable_quoted_printable])
AS_IF([test "x$enable_quoted_printable" = "xyes"],
  [AC_DEFINE([WITH_QUOTED_PRINTABLE],[1],[with quoted-printable support])]
)

AC_MSG_CHECKING([whether to enable UTF-7 support])
AC_ARG_ENABLE([utf7],
  [AS_HELP_STRING([--disable-utf7], [disable UTF-7 support])],
  [],
  [enable_utf7="yes"]
)
AC_MSG_RESULT([$enable_utf7])
AS_IF([test "x$enable_utf7" = "xyes"],
  [AC_DEFINE([WITH_UTF7],[1],[with UTF-7 support])]
)

AC_MSG_CHECKING([whether to enable UTF-8 support])
AC_ARG_ENABLE([utf8],
  [AS_HELP_STRING([--disable-utf8], [disable UTF-8 support])],
  [],
  [enable_utf8="yes"]
)
AC_MSG_RESULT([$enable_utf8])
AS_IF([test "x$enable_utf8" = "xyes"],
  [AC_DEFINE([WITH_UTF8],[1],[with UTF-8 support])]
)

AC_MSG_CHECKING([whether to enable UTF-16 support])
AC_ARG_ENABLE([utf16],
  [AS_HELP_STRING([--disable-utf16], [disable UTF-16 support])],
  [],
  [enable_utf16="yes"]
)
AC_MSG_RESULT([$enable_utf16])
AS_IF([test "x$enable_utf16" = "xyes"],
  [AC_DEFINE([WITH_UTF16],[1],[with UTF-16 support])]
)

AC_MSG_CHECKING([whether to enable word position data])
AC_ARG_ENABLE([word_pos],
  [AS_HELP_STRING([--disable-word-pos], [disable word position data])],
  [],
  [enable_word_pos="yes"]
)
AC_MSG_RESULT([$enable_word_pos])
AS_IF([test "x$enable_word_pos" = "xyes"],
  [AC_DEFINE([WITH_WORD_POS],[1],[with word position data])]
)

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
    man/Makefile
    man/man1/Makefile
    man/man4/Makefile
    man/man8/Makefile
])
AC_OUTPUT

# vim:set et sw=2 ts=2:
