#!/bin/bash

die() {
  echo $1
  exit 1
}

if [[ -z "$1" ]]; then 
  die 'usage: parser_regtest.sh <PRISM examples dir>'
fi

models=$(find $1 -regex '.*[psn]m')
for m in ${models}; do
  echo PARSING MODEL ${m}
  HEAPCHECK=normal ./parser --logtostderr ${m} || die ${m}
done
