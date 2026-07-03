#!/bin/bash
#source root_setting.sh
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh
# source /home/ams/jzhang/AMS/Flux/Sulfur/setenv.sh

cdir=$1
mycc=$2
irig=$3
echo "1=$1"
echo "2=$2"
echo "3=$3"

#----
curdir=`pwd`
echo $curdir
wdir="work_dir"
echo "outdir=${outdir}"

#----cp file
# cpcfile=`cat ${codefile}`
# for cfile in $cpcfile ; do
# 	cp ${cdir}/$cfile ${tmpDir}
# done

date
pwd

# root -l -b -q ${cdir}/${mycc}.C'('${irig}')'

fCh=11
lCh=17

for ((ich=${fCh}; ich<=${lCh}; ich++))
do
	root -l -b -q ${cdir}/${mycc}.C'('${ich}', '${irig}')'
done

cd ${curdir}

echo "***Job Done***"
date
