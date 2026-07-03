#!/bin/sh
#---
#$1: charge
#-----

if [[ $# -lt 1 ]]; then
	echo "Need at least 1 parameter: <charge>"
	exit -1
fi

#------
# wdir="/afs/cern.ch/user/c/chenya" #XXX update to your directory
wdir="/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF" #XXX update to your directory

#------
#queuename=workday
queuename=testmatch

dstver=69
AmsVer="B1130"

charge=$1
qver=1
NVar=-1
XY=2
layer=1
span=1
fRig=0
# lRig=36 #XXX change the range of rigidity bin accordingly
lRig=23 #XXX test the bin

#---
execname=protest_KeysPdfRead5_sdiat.sh

#---
# odir=${wdir}/SubJob/amsd${dstver}n_${AmsVer}_NVar${NVar}_RunQ${charge}_RebinRho_L1QRebin
odir=${wdir}/SubJob/amsd${dstver}n_${AmsVer}_NVar${NVar}_RunQ${charge}_RebinRho_L1QRebin_Migration_Z1516rZ14
# odir=${wdir}/SubJob/amsd${dstver}n_${AmsVer}_NVar${NVar}_RunQ${charge}_RebinRho_L1QRebin_ShiftL2
# odir=${wdir}/SubJob/amsd${dstver}n_${AmsVer}_NVar${NVar}_RunQ${charge}_RebinRho
# odir=${wdir}/SubJob/amsd${dstver}n_${AmsVer}_NVar${NVar}_RunQ${charge}_rho2p0_sigma1p0

echo "odir=${odir}"
echo "charge=${charge}"
echo "qver=${qver}"
# echo "JobFlavour is ${queuename}"

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
mycc=KeysPdfRead_Sur #script name
# mycc=KeysPdfRead_L1TempFitL2PDF #script name
cpcfile="${mycc}.C"

cdir=${odir}/code #copy directory
codefile=${cdir}/codefile.txt
cp -v -r ${wdir}/include ${cdir}
cp -v ${wdir}/$cpcfile ${cdir}
cp -v ${wdir}/$execname ${odir}

[ -e ${codefile} ] && rm ${codefile}

ncdirFile=`ls ${cdir} | wc -l`

for ifile in $cpcfile ; do
	if [[ $CodeUpdate -eq 1 ]] || [[ $ncdirFile -eq 0 ]]; then
		cp -v ${wdir}/$ifile ${cdir}
	fi
	echo $ifile >> ${codefile}
done


# if [ $# -ge 7 ]; then
# 	span=$7
# fi

# if [ $# -ge 9 ] && [ $8 -ne -1 ]; then
# 	fRig=$8
# 	lRig=$9
# fi

#------creating argument lists & submitting script
echo "start creating submitting script"
oname=q${charge}_amsd${dstver}n_${mycc}_NVar${NVar}_xy${XY}_layer${layer}
submitfilen=${odir}/submit/${oname}.sub

echo "universe = vanilla\n" 
echo "universe = vanilla\n" > ${submitfilen}

echo "executable	= ${execname}" 
echo "executable	= ${execname}" > ${submitfilen}
echo "output		= output/\$(file).out" 
echo "output		= output/\$(file).out" >> ${submitfilen}
echo "error		= error/\$(file).err" 
echo "error		= error/\$(file).err" >> ${submitfilen}
echo "log		= log/\$(file).log" 
echo "log		= log/\$(file).log"  >> ${submitfilen}
# echo "request_memory	= 1280" 
# echo "request_memory	= 1280"  >> ${submitfilen}
# echo "MY.WantOS = \"el7\"" >> ${submitfilen}


echo "" >> ${submitfilen}
echo "initialdir = ${odir}"  
echo "initialdir = ${odir}"  >> ${submitfilen}
echo "" >> ${submitfilen}
echo "+JobFlavour = \"${queuename}\"" 
echo "+JobFlavour = \"${queuename}\"" >> ${submitfilen}


for ((irig=${fRig}; irig<${lRig}; irig++))
do
	jobname=R${irig}_q${charge}_amsd${dstver}n_${mycc}_NVar${NVar}_xy${XY}_layer${layer}
	echo "jobname = ${jobname}"
	
	#------create argument files
	echo "${cdir}" "${mycc}" "${codefile}" "${irig}" "${odir}" "${charge}" "${qver}" "${NVar}" "${XY}" "${layer}" "${span}" > ${odir}/submit/arg/${jobname}
	#echo "${cdir}" "${mycc}" "${codefile}" "${indir}" "${i}" "${lfile}" "${odir}" "${nevent}" "${GeoSafety}" "${NCORE}" > ${odir}/submit/arg/${jobname} #2021.09.02
	
	#------updating submitting script
	echo "" >> ${submitfilen}
	echo "file=${jobname}" >> ${submitfilen}
	echo "queue arguments from ${odir}/submit/arg/\$(file)" >> ${submitfilen}
	
	#remove existing log file
	if [[ -e ${odir}/log/${jobname}.log ]]; then
		rm ${odir}/log/${jobname}.log
	fi
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
