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
export AMSP=1
export PGTRACK=1
unset G4MULTITHREADED
unset G4AMS
export AMSWD=..
export LANGUAGE=US
source amsvar.root614.sh
cd ./$BUILD_DIR
gmake $MAKE_ARG_1 $MAKE_ARG_2 $MAKE_ARG_3
retcode=$?
cd -
exit $retcode
