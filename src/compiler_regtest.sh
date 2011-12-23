#!/bin/bash

die() {
  echo $1
  exit 1
}

const() {
  case $1 in
    */brp/brp.pm)
      echo 'MAX=1,N=1'
      ;;
    */cell/cell.sm)
      echo 'N=1'
      ;;
    */cluster/cluster.sm)
      echo 'N=1'
      ;;
    */consensus/coin*.nm)
      echo 'K=0'
      ;;
    */embedded/embedded.sm)
      echo 'MAX_COUNT=0'
      ;;
    */firewire/*/deadline.nm)
      echo 'deadline=0,delay=0,fast=0.5'
      ;;
    */firewire/*/firewire.nm)
      echo 'delay=0,fast=0.5'
      ;;
    */fms/fms.sm)
      echo 'n=1'
      ;;
    */kanban/kanban.sm)
      echo 't=1'
      ;;
    */molecules/knacl.sm)
      echo 'N1=1,N2=1,N3=1'
      ;;
    */molecules/*.sm)
      echo 'N1=1,N2=1'
      ;;
    */phil_lss/phil_lss*.nm)
      echo 'K=1'
      ;;
    */tandem/tandem.sm)
      echo 'c=1'
      ;;
    */wlan/wlan*_collide.nm)
      echo 'COL=1,TRANS_TIME_MAX=5'
      ;;
    */wlan/wlan*_collide.nm)
      echo 'COL=1,TRANS_TIME_MAX=5'
      ;;
    */wlan/wlan*_time_bounded.nm)
      echo 'DEADLINE=1,TRANS_TIME_MAX=5'
      ;;
    */wlan/wlan*.nm)
      echo 'TRANS_TIME_MAX=5'
      ;;
    */zeroconf/zeroconf.nm)
      echo 'reset=false,N=1,K=1,loss=0.01,err=2.5'
      ;;
    */zeroconf/zeroconf_time_bounded.nm)
      echo 'reset=false,T=0,N=1,K=1,loss=0.01'
      ;;
  esac
}

if [[ -z "$1" ]]; then 
  die 'usage: compiler_regtest.sh <PRISM examples dir>'
fi

models=$(find $1 -regex '.*[psn]m')
for m in ${models}; do
  echo COMPILING MODEL ${m}
  HEAPCHECK=normal ./compiler --logtostderr --const=$(const ${m}) ${m} \
      --vmodule=compiler-main=1 || die ${m}
done
