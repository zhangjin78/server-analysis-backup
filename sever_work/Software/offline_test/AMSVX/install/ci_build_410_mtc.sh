#!/usr/bin/env bash

# do not modify anything under and above this line
#

BUILD_DIR=$1
MAKE_ARG_1=$2
MAKE_ARG_2=$3
MAKE_ARG_3=$4

export LANGUAGE=US
export LANG=C
export GENFIT=1
export LC_ALL=en_US
unset DEBUGFLAG
export VERBOSE=1

# icc building ..
export AMSWD=..
export LANGUAGE=US
export GDBMAN=1
export GDBINFO=1
source amsvar.sh
source $ROOTSYS/../root-v5-34-9-icc64.17-slc6/amsvar.sh
export G4MULTITHREADED=1
source g4i10.1.sh
export DPMJET3=6
export PGTRACK=1
export G4AMS=1
cd ./$BUILD_DIR
gmake $MAKE_ARG_1 $MAKE_ARG_2 $MAKE_ARG_3
retcode=$?
cd -
exit $retcode
