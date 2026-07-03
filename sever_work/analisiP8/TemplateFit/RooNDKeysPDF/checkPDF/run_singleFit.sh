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
    for ((ilayer=1; ilayer<=2; ilayer++))
    do
        for ((irho=2; irho<=2; irho++))
        do
            for ((isig=3; isig<=3; isig++))
            do
                root -l -b -q ${cdir}/${mycc}.C'('${ich}', '${iRig}', '${ilayer}', '${irho}', '${isig}')'
            done
        done
    done
    # root -l -b -q ${cdir}/${mycc}.C'('${ich}', '${iRig}')'
done

cd $curdir

echo "***Job Done***"
date