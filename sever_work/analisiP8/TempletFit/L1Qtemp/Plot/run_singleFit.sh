#!/bin/bash

date
pwd
cdir=$1
mycc=$2
fcharge=$3
lcharge=$4
iRig=$5

curdir=`pwd`
echo $curdir
wdir="work_dir"
echo "outdir=${cdir}"

echo ${fcharge} ${lcharge} ${iRig}

for ((ich=${fcharge}; ich<=${lcharge}; ich++))
do
    echo "run Charge: ${ich}"
    echo "selected rig bin ${iRig}"
    # root -l -b -q /afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/singleFit.C'('${ich}', '${iRig}')'
    root -l -b -q ${cdir}/${mycc}.C'('${ich}', '${iRig}')'
done

cd $curdir

echo "***Job Done***"
date