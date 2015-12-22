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
AC_PROG_INSTALL
AC_PROC_LIBTOOL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([zlib], [uncompress])

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
AC_CHECK_FUNCS([select socket strchr strrchr])

# Miscellaneous.
AC_LANG(C++)
AX_CFLAGS_WARN_ALL
AX_CHECK_COMPILE_FLAG([-Wextra],[CFLAGS="$CFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wc++-compat],[CFLAGS="$CFLAGS -Wc++-compat"])
AX_CHECK_COMPILE_FLAG([-Wredundant-decls],[CFLAGS="$CFLAGS -Wredundant-decls"])
AX_CHECK_COMPILE_FLAG([-Wwrite-strings],[CFLAGS="$CFLAGS -Wwrite-strings"])

# Generate files.
AH_TOP([#ifndef swishpp_config_H
#define swishpp_config_H])
AH_BOTTOM([#endif /* swishpp_config_H */])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
    Makefile
    man/Makefile
    man/man1/Makefile
    man/man4/Makefile
    man/man8/Makefile
    src/Makefile
    src/charsets/Makefile
    src/encodings/Makefile
    src/mod/html/Makefile
    src/mod/id3/Makefile
    src/mod/latex/Makefile
    src/mod/mail/Makefile
    src/mod/man/Makefile
    src/mod/rtf/Makefile
    src/pjl/Makefile
])
AC_OUTPUT

# vim:set et sw=2 ts=2:
