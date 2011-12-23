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
  HEAPCHECK=normal ./parser ${m} || die ${m}
done
