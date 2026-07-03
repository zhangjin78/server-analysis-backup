#!/usr/bin/env bash
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh


# do not modify anything under and above this line
#
mkdir -p ../exe/linux && g++ -static ../CC/tle.C -o ../exe/linux/tle -L/cvmfs/ams.cern.ch/Offline/vdev/lib/linuxx8664gcc5.34 -lntuple_slc6_PG
