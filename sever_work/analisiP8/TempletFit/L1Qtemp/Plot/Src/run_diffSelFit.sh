#!/bin/bash

#----
curdir=`pwd`
echo $curdir
date
pwd

charge=14
iRig=20
fsel=0
esel=5

for ((isel=${fsel}; isel<${esel}; isel++))
do
    echo "selection: ${isel}"
    root -l -b -q /afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/Src/diffSelFit.C'('${charge}', '${iRig}', '${isel}')'
done

echo "***Job Done***"
date
