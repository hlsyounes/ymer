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

echo -n poll5_sprt96...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --seed=0 src/testdata/poll5.sm <(echo 'P<0.96[ true U<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_sprt96.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n poll5_sprt98...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --seed=0 src/testdata/poll5.sm <(echo 'P<0.98[ true U<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_sprt98.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n poll5_hybrid...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --estimate-probabilities --engine=hybrid src/testdata/poll5.sm <(echo 'P>0[ true U<=10 (s=1 & a=0) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/poll5_hybrid.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n tandem7_sprt08...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --seed=0 --const=c=7 src/testdata/tandem.sm <(echo 'P<0.08[ true U<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_sprt08.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n tandem7_sprt12...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --seed=0 --const=c=7 src/testdata/tandem.sm <(echo 'P<0.12[ true U<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_sprt12.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n tandem7_hybrid...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --estimate-probabilities --engine=hybrid --const=c=7 src/testdata/tandem.sm <(echo 'P>0[ true U<=26 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandem7_hybrid.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n tandemW7_sprt18...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --seed=0 --const=c=7 src/testdata/tandemW.sm <(echo 'P<0.18[ true U<=227 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandemW7_sprt18.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

echo -n tandemW7_sprt22...
HEAPCHECK=normal GLOG_logtostderr=1 ./ymer --verbose=1 --seed=0 --const=c=7 src/testdata/tandemW.sm <(echo 'P<0.22[ true U<=227 (sc=c & sm=c) ]') 2>/dev/null | grep -v 'seconds.$' | diff src/testdata/tandemW7_sprt22.golden -
if [[ $? = 0 ]]; then
  echo pass
else
  echo fail
  pass=0
fi

if (( ${pass} )); then
  echo PASS
else
  echo FAIL
  exit 1
fi
