#!/bin/bash

date
pwd
cdir=$1
mycc=$2
ich=$3
iRig=$4

curdir=`pwd`
echo $curdir
wdir="work_dir"
echo "outdir=${cdir}"

echo ${ich} ${iRig}

root -l -b -q ${cdir}/${mycc}.C'('${ich}', '${iRig}')'

cd $curdir

echo "***Job Done***"
date