#!/bin/tcsh
#source root_setting.sh
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh

set cdir=$1
set mycc=$2
set codefile=$3
set rig=$4
set outdir=$5
set charge=$6
set qver=$7
set NVar=$8
set XY=$9
set layer=${10}
set span=${11}

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
set curdir=`pwd`
echo $curdir
set wdir="work_dir"
echo "outdir=${outdir}"

#----cp file
set cpcfile=`cat ${codefile}`
foreach cfile ( $cpcfile )
	cp -v ${cdir}/$cfile ${curdir}/$wdir
end

date
cd $wdir
pwd

#root -l -b -q ${cdir}/${mycc}.C++'('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'
#root -l -b -q ${cdir}/${mycc}.C'('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'
root -l -b -q ${mycc}.C'('${rig}', '${charge}', '${qver}', '${NVar}', '${XY}', '${layer}', '${span}')'
ls

cd ${curdir}

echo "***Job Done***"
date
