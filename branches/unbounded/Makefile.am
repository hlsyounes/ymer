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

SUBDIRS = gsl

noinst_LTLIBRARIES =

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
cudd_cudd_libcudd_la_CPPFLAGS = -I$(srcdir)/cudd/epd -I$(srcdir)/cudd/mtr \
    -I$(srcdir)/cudd/st -I$(srcdir)/cudd/util

noinst_LTLIBRARIES += cudd/epd/libepd.la
cudd_epd_libepd_la_SOURCES = cudd/epd/epd.c
cudd_epd_libepd_la_LIBADD = cudd/util/libutil.la
cudd_epd_libepd_la_CPPFLAGS = -I$(srcdir)/cudd/util

noinst_LTLIBRARIES += cudd/mtr/libmtr.la
cudd_mtr_libmtr_la_SOURCES = cudd/mtr/mtrBasic.c cudd/mtr/mtrGroup.c
cudd_mtr_libmtr_la_LIBADD = cudd/st/libst.la cudd/util/libutil.la
cudd_mtr_libmtr_la_CPPFLAGS = -I$(srcdir)/cudd/st -I$(srcdir)/cudd/util

noinst_LTLIBRARIES += cudd/st/libst.la
cudd_st_libst_la_SOURCES = cudd/st/st.c
cudd_st_libst_la_LIBADD = cudd/util/libutil.la
cudd_st_libst_la_CPPFLAGS = -I$(srcdir)/cudd/util

noinst_LTLIBRARIES += cudd/util/libutil.la
cudd_util_libutil_la_SOURCES = cudd/util/cpu_time.c cudd/util/cpu_stats.c \
    cudd/util/getopt.c cudd/util/safe_mem.c cudd/util/strsav.c \
    cudd/util/texpand.c cudd/util/ptime.c cudd/util/prtime.c \
    cudd/util/pipefork.c cudd/util/pathsearch.c cudd/util/stub.c \
    cudd/util/tmpfile.c cudd/util/datalimit.c

bin_PROGRAMS = ymer
ymer_SOURCES = main.cc rng.h rng.c rngdc.c verbosity.h refcount.h expressions.h expressions.cc formulas.h formulas.cc distributions.h distributions.cc modules.h modules.cc models.h models.cc comm.h comm.cc simulator.h simulator.cc verify.h sampling.cc sampling.h trajectories.h learning.cc learning.h mtbdd.cc mtbdd.h parser.yy lexer.ll
ymer_LDADD = cudd/cudd/libcudd.la
ymer_LDADD += gsl/libgsl.la gsl/cblas/libgslcblas.la

EXTRA_DIST = cudd examples scripts

AM_CPPFLAGS = -I$(srcdir)/cudd/cudd -I$(srcdir)/cudd/mtr \
    -I$(srcdir)/cudd/epd -Igsl
AM_YFLAGS = -d
