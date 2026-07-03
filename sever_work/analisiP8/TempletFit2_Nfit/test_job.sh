#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh
# source /home/ams/jzhang/AMS/Flux/Sulfur/setenv.sh

#----

root -l -b /afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/KeysPdfRead_Sur.C'(1,14)'
# root -l -b -q /home/ams/jzhang/AMS/Flux/Sulfur/plot/SurvivalProbability/L1L2Sur/L1BK/TempletFit/KeysPdfRead_Sur.C'++('${rig}', '${charge}')'


echo "***Job Done***"
date
