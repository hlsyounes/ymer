#!/bin/bash
#
# Copyright (C) 2003--2005 Carnegie Mellon University
# Copyright (C) 2011--2012 Google Inc
#
# This file is part of Ymer.
#
# Ymer is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Ymer is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ymer; if not, write to the Free Software Foundation,
# Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

pass=1

readonly YMER=${YMER:-./ymer}

function timestamp() {
  expr 1000 '*' $(date +%s) + $(date +%N) / 1000000
}

function expect_ok() {
  if [[ $? = 0 ]]; then
    echo pass "[$(expr $(timestamp) - ${1})ms]"
  else
    echo fail
    pass=0
  fi
}

echo -n poll5_sprt96...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --delta=0.005 src/testdata/poll5.sm <(echo 'P<0.96[ F<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_sprt96.golden -
expect_ok ${start}

echo -n poll5_sprt98...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --delta=0.005 src/testdata/poll5.sm <(echo 'P<0.98[ F<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_sprt98.golden -
expect_ok ${start}

echo -n poll5_ssp96...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --delta=0.005 --sampling-algorithm=ssp src/testdata/poll5.sm <(echo 'P<0.96[ F<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_ssp96.golden -
expect_ok ${start}

echo -n poll5_ssp98...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --delta=0.005 --sampling-algorithm=ssp src/testdata/poll5.sm <(echo 'P<0.98[ F<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_ssp98.golden -
expect_ok ${start}

echo -n poll5_estimate...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities src/testdata/poll5.sm <(echo 'P>0[ F<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_estimate.golden -
expect_ok ${start}

echo -n poll5_hybrid...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --estimate-probabilities --engine=hybrid src/testdata/poll5.sm <(echo 'P>0[ F<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_hybrid.golden -
expect_ok ${start}

echo -n tandem7_sprt08...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --const=c=7 src/testdata/tandem.sm <(echo 'P<0.08[ F<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_sprt08.golden -
expect_ok ${start}

echo -n tandem7_sprt12...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --const=c=7 src/testdata/tandem.sm <(echo 'P<0.12[ F<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_sprt12.golden -
expect_ok ${start}

echo -n tandem7_estimate...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --delta=0.05 --const=c=7 src/testdata/tandem.sm <(echo 'P>0[ F<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_estimate.golden -
expect_ok ${start}

echo -n tandem7_hybrid...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --estimate-probabilities --engine=hybrid --const=c=7 src/testdata/tandem.sm <(echo 'P>0[ F<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_hybrid.golden -
expect_ok ${start}

echo -n tandemW7_sprt18...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --const=c=7 src/testdata/tandemW.sm <(echo 'P<0.18[ F<=227 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandemW7_sprt18.golden -
expect_ok ${start}

echo -n tandemW7_sprt22...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --const=c=7 src/testdata/tandemW.sm <(echo 'P<0.22[ F<=227 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandemW7_sprt22.golden -
expect_ok ${start}

echo -n tandemW7_estimate...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --delta=0.05 --const=c=7 src/testdata/tandemW.sm <(echo 'P>0[ F<=227 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandemW7_estimate.golden -
expect_ok ${start}

# TODO(hlsyounes): Add tandemW7_hybrid regression test.  Requires phase type
# distributions in decision diagram model construction.

echo -n robot4_sprt74...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --memoization --delta=0.05 --const=n=4 src/testdata/robot.sm <(echo 'P>=0.74[ P>=0.5[ F<=9 c=1 ] U<=10 (x1=n & y1=n) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/robot4_sprt74.golden -
expect_ok ${start}

echo -n robot4_sprt86...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --memoization --delta=0.05 --const=n=4 src/testdata/robot.sm <(echo 'P>=0.86[ P>=0.5[ F<=9 c=1 ] U<=10 (x1=n & y1=n) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/robot4_sprt86.golden -
expect_ok ${start}

echo -n robot4_mixed74...
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --engine=mixed --delta=0.05 --const=n=4 src/testdata/robot.sm <(echo 'P>=0.74[ P>=0.5[ F<=9 c=1 ] U<=10 (x1=n & y1=n) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/robot4_mixed74.golden -
expect_ok ${start}

echo -n robot4_mixed86...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --engine=mixed --delta=0.05 --const=n=4 src/testdata/robot.sm <(echo 'P>=0.86[ P>=0.5[ F<=9 c=1 ] U<=10 (x1=n & y1=n) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/robot4_mixed86.golden -
expect_ok ${start}

echo -n robot4_estimate...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --engine=mixed --delta=0.05 --const=n=4 src/testdata/robot.sm <(echo 'P>=0.86[ P>=0.5[ F<=9 c=1 ] U<=10 (x1=n & y1=n) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/robot4_estimate.golden -
expect_ok ${start}

echo -n robot4_hybrid...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --estimate-probabilities --engine=hybrid --const=n=4 src/testdata/robot.sm <(echo 'P>0[ P>=0.5[ F<=9 c=1 ] U<=10 (x1=n & y1=n) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/robot4_hybrid.golden -
expect_ok ${start}

echo -n cell22_estimate...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --delta=0.05 --const=N=22 src/testdata/cell.sm <(echo 'P>0[ F<=1 n=N]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/cell22_estimate.golden -
expect_ok ${start}

echo -n cell22_hybrid...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --estimate-probabilities --engine=hybrid --const=N=22 src/testdata/cell.sm <(echo 'P>0[ F<=1 n=N]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/cell22_hybrid.golden -
expect_ok ${start}

echo -n knacl10_estimate...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --delta=0.05 --const=N1=10,N2=10,N3=10 src/testdata/knacl.sm <(echo 'P>0[ F[0.001,0.001] na=6]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/knacl10_estimate.golden -
expect_ok ${start}

# TODO(hlsyounes): Add knacl10_hybrid regression test.  Requires support for
# interval time bounds in hybrid engine.

echo -n poll14_unbounded_mixed...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --delta=0.05 --engine=mixed src/testdata/poll14.sm <(echo 'P>0[ !(s=2 & a=1) U (s=1 & a=1) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll14_unbounded_mixed.golden -
expect_ok ${start}

# TODO(hlsyounes) Add poll14_unbounded_mixed regression test.  Requires support
# for unbounded until in hybrid engine.

echo -n herman17_unbounded_mixed...
start=$(timestamp)
HEAPCHECK=normal GLOG_logtostderr=1 ${YMER} --seed=0 --estimate-probabilities --delta=0.01 --engine=mixed src/testdata/herman17.pm <(echo 'P>0[ F "stable" ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/herman17_unbounded_mixed.golden -
expect_ok ${start}

# TODO(hlsyounes) Add herman17_unbounded_hybrid regression test.  Requires
# support for DTMCs and unbounded until in hybrid engine.

if (( ${pass} )); then
  echo PASS
else
  echo FAIL
  exit 1
fi
