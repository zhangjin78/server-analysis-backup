#!/bin/bash
#source root_setting.sh
#source root_setting_cvmfs.sh #use root in cvmfs instead
#echo "HOME=$HOME"

export ROOTSYS=/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.22.06/x86_64-centos7-gcc48-opt
export PATH=$ROOTSYS/bin:${PATH}
echo "PATH=$PATH"

cdir=$1
mycc=$2
codefile=$3
rig=$4
outdir=$5
charge=$6
qver=$7
NVar=$8
XY=$9
layer=${10}
span=${11}

echo "1=$1"
echo "2=$2"
echo "3=$3"
echo "4=$4"
echo "5=$5"
echo "6=$6"
echo "7=$7"
echo "8=$8"
echo "9=$9"
echo "10=${10}"
echo "11=${11}"

#----
curdir=`pwd`
echo $curdir
wdir="work_dir"
[ ! -e $wdir ] && mkdir ${curdir}/${wdir}
echo "outdir=${outdir}"
echo "curdir=$curdir"

#----cp file
cpcfile=`cat ${codefile}`
for cfile in $cpcfile ; do
	cp ${cdir}/$cfile  ${curdir}/${wdir}
done

date
cd ${wdir}
pwd

ls -lrt

#root -l -b -q ${cdir}/${mycc}.C++'('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'
#root -l -b -q /home/ams/yachen/.rootlogon.C ${mycc}.C'('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'
root -l -b -q ${mycc}.C'('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'

ls

cd ${curdir}

echo "***Job Done***"
date
