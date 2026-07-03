#!/bin/sh
#---
#------
# wdir="/afs/cern.ch/user/c/chenya" #XXX update to your directory
wdir="/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot" #XXX update to your directory

#------
#queuename=workday
queuename=testmatch

fCharge=12
lCharge=16

fRig=0
lRig=23

#---
execname=run_singleFit.sh

#---
odir=${wdir}/subjob/Z${fCharge}_Z${lCharge}_MCPDF_DataTemp_L2

echo "odir=${odir}"
# echo "JobFlavour is ${queuename}"

#--
# mycc=KeysPdfRead_Sur #script name
mycc=singleFit #script name
# mycc=diffSamFit #script name
cpcfile="${mycc}.C"

echo "run program is ${cpcfile}"

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

cdir=${odir}/code #copy directory
codefile=${cdir}/codefile.txt
cp -v -r ${wdir}/../include ${odir}
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


#------creating argument lists & submitting script
echo "start creating submitting script"
oname=run_Q${fCharge}ToQ${lCharge}
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
	jobname=run_Q${fCharge}ToQ${lCharge}_Rig${irig}
	echo "jobname = ${jobname}"
	
	#------create argument files
	echo "${cdir}" "${mycc}" "${fCharge}" "${lCharge}" "${irig}"  > ${odir}/submit/arg/${jobname}
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
