#!/bin/bash
#
# Copyright (C) 2003--2005 Carnegie Mellon University
# Copyright (C) 2011--2015 Google Inc
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

readonly YMER=${YMER:-./ymer}

function die() {
  echo $1
  echo FAIL
  exit 1
}

function const() {
  case $(basename $1) in
    brp.pm)
      echo ' --const=N=2,MAX=3'
      ;;
    cell.sm)
      echo ' --const=N=2'
      ;;
    cluster.sm)
      echo ' --const=N=2'
      ;;
    embedded.sm)
      echo ' --const=MAX_COUNT=2'
      ;;
    fms.sm)
      echo ' --const=n=2'
      ;;
    kanban.sm)
      echo ' --const=t=2'
      ;;
    knacl.sm)
      echo ' --const=N1=2,N2=3,N3=4'
      ;;
    mc.sm)
      echo ' --const=N1=2,N2=3'
      ;;
    nacl.sm)
      echo ' --const=N1=2,N2=3'
      ;;
    tandem.sm)
      echo ' --const=c=2'
      ;;
  esac
}

if [[ -z "$1" ]]; then 
  die 'usage: parser_regtest.sh <PRISM examples dir>'
fi

models=$(find $1 -regex '.*[ps]m')
for m in ${models}; do
  echo PARSING MODEL ${m}
  HEAPCHECK=normal GLOG_logtostderr=1 ${YMER}$(const ${m}) ${m} || die ${m}
done

echo PASS
