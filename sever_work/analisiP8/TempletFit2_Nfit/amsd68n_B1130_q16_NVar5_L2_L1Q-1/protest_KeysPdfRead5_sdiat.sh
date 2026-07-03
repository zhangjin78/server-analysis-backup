#!/bin/bash
#source root_setting.sh
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh
# source /home/ams/jzhang/AMS/Flux/Sulfur/setenv.sh

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
L1QSel=${12}

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
echo "12=${12}"

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

root -l -b -q ${cdir}/${mycc}.C'('${rig}', '${charge}', '${qver}', '${NVar}', '${L1QSel}')'
# root -l -b -q ${cdir}/${mycc}.C'++('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'

# for ((irig=${fRig}; irig<${lRig}; irig++))
# do
# 	echo "selected rig bin ${irig}"
# 	root -l -b -q ${cdir}/${mycc}.C++'('${irig}', '${charge}')'
# 	# root -l -b -q ${cdir}/${mycc}.C++'('${irig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')' &
# done

cd ${curdir}

echo "***Job Done***"
date
