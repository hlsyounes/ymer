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

SUBDIRS = glog gtest

noinst_LTLIBRARIES =

CUDD_DEFINES = -DBSD -DHAVE_IEEE_754

# The main CUDD library.
noinst_LTLIBRARIES += cudd/cudd/libcudd.la
cudd_cudd_libcudd_la_SOURCES = cudd/cudd/cuddAPI.c cudd/cudd/cuddAddAbs.c \
    cudd/cudd/cuddAddApply.c cudd/cudd/cuddAddFind.c cudd/cudd/cuddAddIte.c \
    cudd/cudd/cuddAddInv.c cudd/cudd/cuddAddNeg.c cudd/cudd/cuddAddWalsh.c \
    cudd/cudd/cuddAndAbs.c cudd/cudd/cuddAnneal.c cudd/cudd/cuddApa.c \
    cudd/cudd/cuddApprox.c cudd/cudd/cuddBddAbs.c cudd/cudd/cuddBddCorr.c \
    cudd/cudd/cuddBddIte.c cudd/cudd/cuddBridge.c cudd/cudd/cuddCache.c \
    cudd/cudd/cuddCheck.c cudd/cudd/cuddClip.c cudd/cudd/cuddCof.c \
    cudd/cudd/cuddCompose.c cudd/cudd/cuddDecomp.c cudd/cudd/cuddEssent.c \
    cudd/cudd/cuddExact.c cudd/cudd/cuddExport.c cudd/cudd/cuddGenCof.c \
    cudd/cudd/cuddGenetic.c cudd/cudd/cuddGroup.c cudd/cudd/cuddHarwell.c \
    cudd/cudd/cuddInit.c cudd/cudd/cuddInteract.c cudd/cudd/cuddLCache.c \
    cudd/cudd/cuddLevelQ.c cudd/cudd/cuddLinear.c cudd/cudd/cuddLiteral.c \
    cudd/cudd/cuddMatMult.c cudd/cudd/cuddPriority.c cudd/cudd/cuddRead.c \
    cudd/cudd/cuddRef.c cudd/cudd/cuddReorder.c cudd/cudd/cuddSat.c \
    cudd/cudd/cuddSign.c cudd/cudd/cuddSolve.c cudd/cudd/cuddSplit.c \
    cudd/cudd/cuddSubsetHB.c cudd/cudd/cuddSubsetSP.c cudd/cudd/cuddSymmetry.c \
    cudd/cudd/cuddTable.c cudd/cudd/cuddUtil.c cudd/cudd/cuddWindow.c \
    cudd/cudd/cuddZddCount.c cudd/cudd/cuddZddFuncs.c cudd/cudd/cuddZddGroup.c \
    cudd/cudd/cuddZddIsop.c cudd/cudd/cuddZddLin.c cudd/cudd/cuddZddMisc.c \
    cudd/cudd/cuddZddPort.c cudd/cudd/cuddZddReord.c cudd/cudd/cuddZddSetop.c \
    cudd/cudd/cuddZddSymm.c cudd/cudd/cuddZddUtil.c
cudd_cudd_libcudd_la_LIBADD = cudd/epd/libepd.la cudd/mtr/libmtr.la \
    cudd/st/libst.la cudd/util/libutil.la
cudd_cudd_libcudd_la_CPPFLAGS = $(CUDD_DEFINES) -I$(srcdir)/cudd/epd \
    -I$(srcdir)/cudd/mtr -I$(srcdir)/cudd/st -I$(srcdir)/cudd/util

# CUDD library for extended double precision arithmetic.
noinst_LTLIBRARIES += cudd/epd/libepd.la
cudd_epd_libepd_la_SOURCES = cudd/epd/epd.c
cudd_epd_libepd_la_LIBADD = cudd/util/libutil.la
cudd_epd_libepd_la_CPPFLAGS = $(CUDD_DEFINES) -I$(srcdir)/cudd/util

# CUDD library for multiway-branch tree manipulation.
noinst_LTLIBRARIES += cudd/mtr/libmtr.la
cudd_mtr_libmtr_la_SOURCES = cudd/mtr/mtrBasic.c cudd/mtr/mtrGroup.c
cudd_mtr_libmtr_la_LIBADD = cudd/st/libst.la cudd/util/libutil.la
cudd_mtr_libmtr_la_CPPFLAGS = $(CUDD_DEFINES) -I$(srcdir)/cudd/st \
    -I$(srcdir)/cudd/util

# CUDD library for symbol tables.
noinst_LTLIBRARIES += cudd/st/libst.la
cudd_st_libst_la_SOURCES = cudd/st/st.c
cudd_st_libst_la_LIBADD = cudd/util/libutil.la
cudd_st_libst_la_CPPFLAGS = $(CUDD_DEFINES) -I$(srcdir)/cudd/util

# CUDD utility library.
noinst_LTLIBRARIES += cudd/util/libutil.la
cudd_util_libutil_la_SOURCES = cudd/util/cpu_time.c cudd/util/cpu_stats.c \
    cudd/util/getopt.c cudd/util/safe_mem.c cudd/util/strsav.c \
    cudd/util/texpand.c cudd/util/ptime.c cudd/util/prtime.c \
    cudd/util/pipefork.c cudd/util/pathsearch.c cudd/util/stub.c \
    cudd/util/tmpfile.c cudd/util/datalimit.c
cudd_util_libutil_la_CPPFLAGS = $(CUDD_DEFINES)

# Library for dynamic creation of Mersenne Twister generators.
noinst_LTLIBRARIES += dcmt/lib/libdcmt.la
dcmt_lib_libdcmt_la_SOURCES = dcmt/lib/dci.h dcmt/lib/check32.c \
    dcmt/lib/prescr.c dcmt/lib/mt19937.c dcmt/lib/eqdeg.c dcmt/lib/seive.c \
    dcmt/lib/genmtrand.c dcmt/lib/init.c
dcmt_lib_libdcmt_la_CPPFLAGS = -I$(srcdir)/dcmt/include

#
# Ymer libraries.
#

# String utility library.
noinst_LTLIBRARIES += src/libstrutil.la
src_libstrutil_la_SOURCES = src/strutil.h

# Decision diagram utility library.
noinst_LTLIBRARIES += src/libddutil.la
src_libddutil_la_SOURCES = src/ddutil.h src/ddutil.cc
src_libddutil_la_LIBADD = cudd/cudd/libcudd.la glog/libglog.la

# Random number generator library.
noinst_LTLIBRARIES += src/librng.la
src_librng_la_SOURCES = src/rng.h src/rng.cc
src_librng_la_LIBADD = dcmt/lib/libdcmt.la

# Typed-value library.
noinst_LTLIBRARIES += src/libtyped-value.la
src_libtyped_value_la_SOURCES = src/typed-value.h src/typed-value.cc
src_libtyped_value_la_LIBADD = glog/libglog.la

# Expression library.
noinst_LTLIBRARIES += src/libexpression.la
src_libexpression_la_SOURCES = src/expression.h src/expression.cc
src_libexpression_la_LIBADD = src/libddutil.la src/libtyped-value.la \
    glog/libglog.la

# Compiled expression library.
noinst_LTLIBRARIES += src/libcompiled-expression.la
src_libcompiled_expression_la_SOURCES = src/compiled-expression.h \
    src/compiled-expression.cc

# Compiled distribution library.
noinst_LTLIBRARIES += src/libcompiled-distribution.la
src_libcompiled_distribution_la_SOURCES = src/compiled-distribution.h \
    src/compiled-distribution.cc
src_libcompiled_distribution_la_LIBADD = src/libcompiled-expression.la

# Compiled model library.
noinst_LTLIBRARIES += src/libcompiled-model.la
src_libcompiled_model_la_SOURCES = src/compiled-model.h src/compiled-model.cc
src_libcompiled_model_la_LIBADD = src/libddutil.la \
    src/libcompiled-distribution.la src/libcompiled-expression.la \
    glog/libglog.la

#
# Ymer binaries.
#

bin_PROGRAMS = ymer
ymer_SOURCES = ymer.cc formulas.cc formulas.h distributions.cc distributions.h modules.cc modules.h models.cc models.h states.cc states.h sampling.cc odd.cc odd.h hybrid.cc hybrid.h symbolic.cc mixed.cc comm.h parser.yy tokenizer.ll
ymer_LDADD = cudd/cudd/libcudd.la src/libstrutil.la src/libddutil.la \
    src/librng.la src/libtyped-value.la src/libexpression.la \
    src/libcompiled-model.la

#
# Ymer tests.
#

check_LTLIBRARIES =

# Library with main function for tests.
check_LTLIBRARIES += src/libtest-main.la
src_libtest_main_la_SOURCES = src/test-main.cc
src_libtest_main_la_LIBADD = glog/libglog.la gtest/lib/libgtest.la

check_PROGRAMS =

# Test for string utility library.
check_PROGRAMS += src/strutil_test
src_strutil_test_SOURCES = src/strutil_test.cc
src_strutil_test_LDADD = src/libstrutil.la src/libtest-main.la

# Test for decision diagram utility library.
check_PROGRAMS += src/ddutil_test
src_ddutil_test_SOURCES = src/ddutil_test.cc
src_ddutil_test_LDADD = src/libddutil.la src/libtest-main.la

# Test for random number generator library.
check_PROGRAMS += src/rng_test
src_rng_test_SOURCES = src/rng_test.cc
src_rng_test_LDADD = src/librng.la src/libtest-main.la

# Test for typed-value library.
check_PROGRAMS += src/typed-value_test
src_typed_value_test_SOURCES = src/typed-value_test.cc
src_typed_value_test_LDADD = src/libtyped-value.la src/libstrutil.la \
    src/libtest-main.la

# Test for expression library.
check_PROGRAMS += src/expression_test
src_expression_test_SOURCES = src/expression_test.cc
src_expression_test_LDADD = src/libexpression.la src/libstrutil.la \
    src/libtest-main.la

# Test for compiled expression library.
check_PROGRAMS += src/compiled-expression_test
src_compiled_expression_test_SOURCES = src/compiled-expression_test.cc
src_compiled_expression_test_LDADD = src/libcompiled-expression.la \
    src/libtest-main.la

# Test for compiled distribution library.
check_PROGRAMS += src/compiled-distribution_test
src_compiled_distribution_test_SOURCES = src/compiled-distribution_test.cc
src_compiled_distribution_test_LDADD = src/libcompiled-distribution.la \
    src/libcompiled-expression.la src/libtest-main.la

# Test for compiled model library.
check_PROGRAMS += src/compiled-model_test
src_compiled_model_test_SOURCES = src/compiled-model_test.cc
src_compiled_model_test_LDADD = src/libcompiled-model.la src/libtest-main.la

# Note: heap checking is enabled only if tests were linked with tcmalloc.
TESTS_ENVIRONMENT = HEAPCHECK=normal GLOG_logtostderr=1
TESTS = $(check_PROGRAMS)

EXTRA_DIST = cudd examples scripts

CLEANFILES = core

AM_CPPFLAGS = -I$(srcdir)/cudd/cudd -I$(srcdir)/cudd/epd -I$(srcdir)/cudd/mtr \
    -I$(srcdir)/dcmt/include -I$(srcdir)/glog/src -I$(builddir)/glog/src \
    -I$(srcdir)/gtest/include
AM_CXXFLAGS = -Wall -Werror
AM_YFLAGS = -d

ACLOCAL_AMFLAGS = -I m4
