#!/bin/bash
#source root_setting.sh
# source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh
# source /home/ams/jzhang/AMS/Flux/Sulfur/setenv.sh

cdir=$1
mycc=$2
rig=$3
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
charge=24

root -l -b -q ${cdir}/${mycc}.C'('${charge}', '${rig}')'

# for ((irig=${fRig}; irig<${lRig}; irig++))
# do
# 	echo "selected rig bin ${irig}"
# 	root -l -b -q ${cdir}/${mycc}.C++'('${irig}', '${charge}')'
# 	# root -l -b -q ${cdir}/${mycc}.C++'('${irig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')' &
# done

cd ${curdir}

echo "***Job Done***"
date
