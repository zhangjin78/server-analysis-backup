#!/bin/tcsh
#source root_setting.sh
set HOME="/afs/cern.ch/user/c/chenya"
#source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh #2022.09.17: comment out

set cdir=$1
set mycc=$2
set codefile=$3
set ifile=$4
set odir=$5
set nevent=$6
set GeoSafety=$7
set DATAMC=$8 #2020.09.06
set fileno=$9 #2022.09.18

echo "1=$1"
echo "2=$2"
echo "3=$3"
echo "4=$4"
echo "5=$5"
echo "6=$6"
echo "7=$7"
echo "8=$8" #2020.09.06
echo "9=$9" #2022.09.18

#----cp file
#tmpDir=`mktemp -d`
#cd ${tmpDir}
#echo 'we are in '${PWD}
#echo 'tmpDir='${tmpDir}

set curdir=`pwd`
set wdir="work_dir"
mkdir $curdir/$wdir

#------set the AMS gbatch enviroment
if ($DATAMC == 0) then
	#set curdir=`pwd`
	#cd /afs/cern.ch/user/c/chenya/public/gbatch/gbatch_install/
	#source ams_compile_setup_jiawei.csh
	#cd $curdir
	#2022.09.17
	source /afs/cern.ch/user/c/chenya/RichRec/amsroot534.csh
	setenv LD_LIBRARY_PATH /cvmfs/sft.cern.ch/lcg/external/castor/2.1.13-6/x86_64-slc6-gcc48-opt/usr/lib64:$LD_LIBRARY_PATH
endif

#------
echo "odir=${odir}"
set cpcfile=`cat ${codefile}`
foreach cfile ( $cpcfile )
	cp -v ${cdir}/$cfile ${curdir}/$wdir
end

#---process
date
cd $wdir
pwd

#---calling castor
#root -l -b -q ${mycc}.C++'("'${ifile}'", "'$curdir/${wdir}'", '${nevent}')'
#2020.09.06
if ($DATAMC == 0) then
	#root -l -b -q gbatch_rootlogon.C ${mycc}.C++'("'${ifile}'", "'$curdir/${wdir}'", '${nevent}')' > ${odir}/output2/${fileno}.out
	root -l -b -q gbatch_rootlogon.C ${mycc}.C++'("'${ifile}'", "'$curdir/${wdir}'", '${nevent}')'
else
	#root -l -b -q ${mycc}.C++'("'${ifile}'", "'$curdir/${wdir}'", '${nevent}')' > ${odir}/output2/${fileno}.out
	root -l -b -q ${mycc}.C++'("'${ifile}'", "'$curdir/${wdir}'", '${nevent}')'
endif

echo "copy to ${odir}"
cp *_*_*.root ${odir}

ls

cd ${curdir}
rm -vrf ${wdir}

rmdir ${wdir}
echo 'Cleanup done!'
echo "***Job Done***"
date
