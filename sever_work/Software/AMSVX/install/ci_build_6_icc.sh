#!/usr/bin/env bash

BUILD_DIR=$1
MAKE_ARG_1=$2
MAKE_ARG_2=$3
MAKE_ARG_3=$4

# do not modify anything under and above this line
#
export LANGUAGE=US
export LANG=C
export GENFIT=1
export LC_ALL=en_US
unset DEBUGFLAG
export VERBOSE=1

# icc building ..
source $ROOTSYS/../root-v5-34-9-icc64.17-slc6/amsvar.sh
export PGTRACK=1
export AMSP=1
unset G4MULTITHREADED
unset G4AMS
export AMSWD=..
export LANGUAGE=US
source amsvar.root6.sh
cd ./$BUILD_DIR
gmake $MAKE_ARG_1 $MAKE_ARG_2 $MAKE_ARG_3
retcode=$?
cd -
exit $retcode
