## Process this file with autoconf to produce a configure script.
##
## Copyright (C) 2003--2005 Carnegie Mellon University
## Copyright (C) 2011--2015 Google Inc
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
AC_INIT(Ymer, 4.1, hlsyounes@gmail.com)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(ymer.cc)
AM_INIT_AUTOMAKE([subdir-objects])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AM_PROG_LEX
AC_PROG_LIBTOOL
AC_PROG_YACC

AC_DEFUN([AX_PROG_CXX_CXX11],
    [AC_MSG_CHECKING([for $CXX option to accept ISO C++11])
     AC_CACHE_VAL(ax_cv_prog_cxx_cxx11,
         [ax_cv_prog_cxx_cxx11=no
          ax_save_CXX=$CXX
          AC_LANG(C++)
          AC_LANG_CONFTEST([AC_LANG_PROGRAM(
              [[enum class Enum { VALUE };]],
              [[Enum test = Enum::VALUE;]])])
          for ax_arg in "" -std=c++11 -std=c++0x
          do
            CXX="$ax_save_CXX $ax_arg"
            AC_COMPILE_IFELSE([],
                [ax_cv_prog_cxx_cxx11=$ax_arg
                 break])
          done
          rm -f conftest.$ac_ext conftest.$ac_objext
          CXX=$ax_save_CXX])
     case "x$ax_cv_prog_cxx_cxx11" in
       x|xno)
         AC_MSG_RESULT([none needed]) ;;
       *)
         AC_MSG_RESULT([$ax_cv_prog_cxx_cxx11])
         CXX="$CXX $ax_cv_prog_cxx_cxx11" ;;
     esac])

AX_PROG_CXX_CXX11

# Whether to use tcmalloc.
AC_ARG_WITH([tcmalloc],
            [AS_HELP_STRING([--with-tcmalloc],
                            [a malloc replacement @<:@default=no@:>@])],
            [],
            [with_tcmalloc=no])
AS_IF([test "x$with_tcmalloc" != xno],
      [AC_CHECK_LIB(
          [tcmalloc], [tc_malloc], ,
          [AS_IF([test "x$with_tcmalloc" != xcheck],
                 [AC_MSG_FAILURE(
                     [--with-tcmalloc was given, but test for tcmalloc failed])
                 ])])])

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
AC_CHECK_FUNCS([floor gethostbyname gettimeofday memset pow rint select socket sqrt strcasecmp strerror getopt_long])

AC_CONFIG_FILES(Makefile)
AC_CONFIG_SUBDIRS([glog gsl gtest])
AC_OUTPUT
