## Process this file with automake to produce Makefile.in
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

SUBDIRS = gflags glog

noinst_LTLIBRARIES =

#
# Ymer libraries.
#

# Expression library.
noinst_LTLIBRARIES += src/libexpression.la
src_libexpression_la_SOURCES = src/expression.h src/expression.cc
src_libexpression_la_LIBADD = glog/libglog.la

# Process algebra library.
noinst_LTLIBRARIES += src/libprocess-algebra.la
src_libprocess_algebra_la_SOURCES = src/process-algebra.h src/process-algebra.cc
src_libprocess_algebra_la_LIBADD = glog/libglog.la

# Type library.
noinst_LTLIBRARIES += src/libtype.la
src_libtype_la_SOURCES = src/type.h src/type.cc
src_libtype_la_LIBADD = glog/libglog.la

# Model library.
noinst_LTLIBRARIES += src/libmodel.la
src_libmodel_la_SOURCES = src/model.h src/model.cc
src_libmodel_la_LIBADD = src/libexpression.la src/libprocess-algebra.la \
    src/libtype.la glog/libglog.la

# Parse library.
noinst_LTLIBRARIES += src/libparse.la
src_libparse_la_SOURCES = src/parse.h src/parse.cc src/parser-state.h \
    src/grammar.yy src/tokens.ll
src_libparse_la_LIBADD = src/libexpression.la src/libmodel.la \
    src/libprocess-algebra.la glog/libglog.la
BUILT_SOURCES = grammar.h

#
# Ymer binaries.
#

noinst_PROGRAMS =

# Simple parser binary.
noinst_PROGRAMS += parser
parser_SOURCES = src/parser-main.cc
parser_LDADD = src/libparse.la gflags/libgflags.la glog/libglog.la

#
# Ymer tests.  Requires gtest library.
#
if HAVE_LIBGTEST

check_LTLIBRARIES =

# Library with main function for tests.
check_LTLIBRARIES += src/libtest-main.la
src_libtest_main_la_SOURCES = src/test-main.cc
src_libtest_main_la_LIBADD = gflags/libgflags.la glog/libglog.la -lgtest

check_PROGRAMS =

# Test for expression library.
check_PROGRAMS += src/expression_test
src_expression_test_SOURCES = src/expression_test.cc
src_expression_test_LDADD = src/libexpression.la src/libtest-main.la

# Test for process algebra library.
check_PROGRAMS += src/process-algebra_test
src_process_algebra_test_SOURCES = src/process-algebra_test.cc
src_process_algebra_test_LDADD = src/libprocess-algebra.la src/libtest-main.la

# Test for type library.
check_PROGRAMS += src/type_test
src_type_test_SOURCES = src/type_test.cc
src_type_test_LDADD = src/libtype.la src/libtest-main.la

# Test for model library.
check_PROGRAMS += src/model_test
src_model_test_SOURCES = src/model_test.cc
src_model_test_LDADD = src/libmodel.la src/libexpression.la \
    src/libprocess-algebra.la src/libtest-main.la

# Test for parse library.
check_PROGRAMS += src/parse_test
src_parse_test_SOURCES = src/parse_test.cc
src_parse_test_LDADD = src/libparse.la src/libexpression.la src/libmodel.la \
    src/libprocess-algebra.la src/libtest-main.la

# Note: heap checking is enabled only if tests were linked with tcmalloc.
TESTS_ENVIRONMENT = HEAPCHECK=normal
TESTS = $(check_PROGRAMS) src/parser_test.sh

endif  # HAVE_LIBGTEST

EXTRA_DIST = examples scripts

CLEANFILES = core

AM_CPPFLAGS = -I$(srcdir)/gflags/src -I$(srcdir)/glog/src
AM_CXXFLAGS = -std=c++0x
AM_YFLAGS = -d

ACLOCAL_AMFLAGS = -I m4
