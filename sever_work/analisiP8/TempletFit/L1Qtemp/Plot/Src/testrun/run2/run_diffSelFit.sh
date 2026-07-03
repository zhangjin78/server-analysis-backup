#!/bin/bash

#----
curdir=`pwd`
echo $curdir
date
pwd
cd $curdir

curdir=`pwd`
echo $curdir

charge=14
iRig=20
fsel=0
esel=6

for ((isel=${fsel}; isel<${esel}; isel++))
do
    echo "selection: ${isel}"
    root -l -b -q ./diffSelFit.C'('${charge}', '${iRig}', '${isel}')'
done

echo "***Job Done***"
date
