## Process this file with autoconf to produce a configure script.
##
## Copyright (C) 2003--2005 Carnegie Mellon University
## Copyright (C) 2011 Google Inc
##
## This file is part of Ymer.
##
## Ymer is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## Ymer is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Ymer; if not, write to the Free Software Foundation,
## Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

AC_PREREQ(2.57)
AC_INIT(Ymer, 3.0, hlsyounes@gmail.com)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(ymer.cc)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AM_PROG_LEX
AC_PROG_LIBTOOL
AC_PROG_YACC

# Set the language.
AC_LANG(C++)

# Checks for libraries.
AC_SEARCH_LIBS(gettext, intl)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h float.h libintl.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([floor gethostbyname gettimeofday memset pow rint select socket sqrt strcasecmp strerror])
AC_CHECK_FUNCS(getopt_long, , [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])

AC_CONFIG_FILES(Makefile)
AC_OUTPUT
