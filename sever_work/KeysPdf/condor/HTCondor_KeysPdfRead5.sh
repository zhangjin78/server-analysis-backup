#!/bin/sh
#---
#$1: dst version
#$2: charge
#$3: charge version
#$4: N distribution to fit, -1: use default
#$5: X-, Y-, or XYQ
#$6: l1 (1) or l2 (2) template #2019.11.10
#$7: span
#$7&8: first and last Rig to run

if [[ $# -lt 3 ]]; then
	echo "Need at least 3 parameter: <dst version> <charge> <charge version>"
	exit -1
fi

#------
wdir="/home/ams/yachen" #XXX update to your directory

#------
queuename=workday

dstver=$1
AmsVer="B1130"

charge=$2
qver=$3
NVar=$4
XY=$5
layer=$6

script_ver=analysis6
Version=L1QTempFit4 #XXX change to your version

#---
execname=multithreading_KeysPdfRead5.py

#---
odir=${wdir}/result/${script_ver}/amsd${dstver}n_${AmsVer}_${Version}/total

echo "odir=${odir}"
echo "charge=${charge}"
echo "qver=${qver}"
echo "JobFlavour is ${queuename}"

#---
if [[ $NonIteraction -ne 1 ]]; then
	read -p "Continue? (Y)" yn
	if [ "${yn}" != "Y" ] && [ "${yn}" != "y" ]; then
		echo "quit"
		exit 0
	fi
fi

#--create directory if not exist
[ ! -e  ${odir} ] && mkdir -p ${odir}
[ ! -e  ${odir}/output ] && mkdir -p ${odir}/output
[ ! -e  ${odir}/error ] && mkdir -p ${odir}/error
[ ! -e  ${odir}/log ] && mkdir -p ${odir}/log
[ ! -e  ${odir}/code ] && mkdir -p ${odir}/code/
[ ! -e  ${odir}/submit ] && mkdir -p ${odir}/submit/
[ ! -e  ${odir}/submit/arg ] && mkdir -p ${odir}/submit/arg/

#--
mycc=KeysPdfRead_batch5 #script name
cpcfile="${mycc}.C"

cdir=${odir}/code #copy directory
codefile=${cdir}/codefile.txt

[ -e ${codefile} ] && rm ${codefile}

ncdirFile=`ls ${cdir} | wc -l`

for ifile in $cpcfile ; do
	#if [[ $CodeUpdate -eq 1 ]] || [[ $ncdirFile -eq 0 ]]; then
	#	cp -v ${wdir}/analisi/KeysPdf/$ifile ${cdir}
	#fi
	cp -v ${wdir}/analisi/KeysPdf/$ifile ${cdir}
	echo $ifile >> ${codefile}
done

NCORE=32

#---
span=1
[[ $# -ge 7 ]] && [[ $7 -ne -1 ]] && span=$7

#---
fRig=0
lRig=39 #XXX change the range of rigidity bin accordingly
if [ $# -ge 9 ] && [ $8 -ne -1 ]; then
	fRig=$8
	lRig=$9
fi

#------creating argument lists & submitting script
echo "start creating submitting script"
submitfilen=${odir}/submit/${oname}.sub

echo "universe = vanilla\n" 
echo "universe = vanilla\n" > ${submitfilen}

echo "executable	= ${execname}" 
echo "executable	= ${execname}" > ${submitfilen}
echo "output		= log/\$(file).out" 
echo "output		= log/\$(file).out" >> ${submitfilen}
echo "error		= log/\$(file).err" 
echo "error		= log/\$(file).err" >> ${submitfilen}
echo "log		= log/\$(file).log" 
echo "log		= log/\$(file).log"  >> ${submitfilen}
echo "request_cpus	= ${NCORE}"
echo "request_cpus	= ${NCORE}" >> ${submitfilen}


echo "" >> ${submitfilen}
echo "initialdir = ${odir}"  
echo "initialdir = ${odir}"  >> ${submitfilen}
echo "" >> ${submitfilen}
echo "+JobFlavour = \"${queuename}\"" 
echo "+JobFlavour = \"${queuename}\"" >> ${submitfilen}

for ((irig=${fRig}; irig<${lRig}; irig++))
do
	jobname=R${irig}_q${charge}_amsd${dstver}n_${mycc}_NVar${NVar}_xy${XY}_layer${layer}_core${NCORE}
	echo "jobname = ${jobname}"
	
	#------create argument files
	echo "${cdir}" "${mycc}" "${codefile}" "${irig}" "${lRig}" "${odir}" "${charge}" "${qver}" "${NVar}" "${XY}" "${layer}" "${span}" "${NCORE}" > ${odir}/submit/arg/${jobname}
	#echo "${cdir}" "${mycc}" "${codefile}" "${indir}" "${i}" "${lfile}" "${odir}" "${nevent}" "${GeoSafety}" "${NCORE}" > ${odir}/submit/arg/${jobname} #2021.09.02
	
	#------updating submitting script
	echo "" >> ${submitfilen}
	echo "file=${jobname}" >> ${submitfilen}
	echo "queue arguments from ${odir}/submit/arg/\$(file)" >> ${submitfilen}
	
	#remove existing log file
	if [[ -e ${odir}/log/${jobname}.log ]]; then
		rm ${odir}/log/${jobname}.log
	fi
	
	irig=$((irig+NCORE-1))
done

echo "Finish creating argument files and submit scripts"
echo " Run \"condor_submit ${submitfilen}\" ?"
if [[ $NonIteraction -ne 1 ]]; then
	read -p "Continue? (Y)" yn
	if [[ "${yn}" == "Y" ]] || [[ "${yn}" == "y" ]]; then
		condor_submit ${submitfilen}
	fi
else
	condor_submit ${submitfilen}
fi
date
