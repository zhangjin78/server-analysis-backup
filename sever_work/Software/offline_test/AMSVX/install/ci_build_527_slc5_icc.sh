#!/usr/bin/env bash

BUILD_DIR=$1
MAKE_ARG_1=$2
MAKE_ARG_2=$3
MAKE_ARG_3=$4

source amsvar.sh

export AMSWD=..

unset ADDBUILDNO

unset AMSLIB 
unset CERNDIR    
unset CERNICC     
unset CERNSRCDIR  

unset SLC6system
export CXXVERS=`g++ -dumpversion | tr -d "."`

export PREFIX=.
export TESTFLAG 
unset G4AMS
unset MARCH
export ECALBDT=1

echo
echo "******** export PGTRACK 1 ********"
echo

export PGTRACK=1
source  $PREFIX/amsvar$TESTFLAG.icc64.sh
source  $PREFIX/g4i$TESTFLAG.icc64.sh
 
cd ./$BUILD_DIR
gmake $MAKE_ARG_1 $MAKE_ARG_2 $MAKE_ARG_3
retcode=$?
cd -
exit $retcode
