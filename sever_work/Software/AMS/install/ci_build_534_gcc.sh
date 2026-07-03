#!/usr/bin/env bash

export UCC=icc64
export UROOT=5
source amsvar_all.sh
export UCC=gcc64
USLC=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`
if [[ $USLC -lt 8 ]]; then
    export VGCC=8
else
    export VGCC=11
fi
./ci_build.sh $@
