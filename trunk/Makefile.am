## Process this file with automake to produce Makefile.in
##
## Copyright (C) 2003--2005 Carnegie Mellon University
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
##
## $Id: Makefile.am,v 4.1 2005-02-01 14:25:41 lorens Exp $

bin_PROGRAMS = ymer
ymer_SOURCES = ymer.cc rng.c rngdc.c rng.h rational.cc rational.h expressions.cc expressions.h formulas.cc formulas.h distributions.cc distributions.h modules.cc modules.h models.cc models.h states.cc states.h sampling.cc odd.cc odd.h hybrid.cc hybrid.h symbolic.cc mixed.cc comm.h parser.yy tokenizer.ll

ymer_LDADD =  -lcudd -lepd -lmtr -lst -lutil @LIBOBJS@
MAINTAINERCLEANFILES = parser.cc tokenizer.cc config.h.in~
EXTRA_DIST = getopt.c getopt1.c getopt.h examples scripts

AM_CPPFLAGS = -I@CUDDDIR@
AM_CFLAGS = -malign-double
AM_CXXFLAGS = -malign-double
AM_LDFLAGS = -L@CUDDDIR@

AM_YFLAGS = -d
