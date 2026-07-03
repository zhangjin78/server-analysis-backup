#!/bin/sh
#---
#$1: dst version
#$2: charge
#$3: L1Q selected range
#-----
#$4: N distribution to fit, -1: use default
#$5: X-, Y-, or XYQ(2)
#$6: l1 (1) or l2 (2) template #2019.11.10
#$7: span
#$8&9: first and last Rig to run

if [[ $# -lt 3 ]]; then
	echo "Need at least 3 parameter: <dst version> <charge> <charge version>"
	exit -1
fi

#------
# wdir="/afs/cern.ch/user/c/chenya" #XXX update to your directory
wdir="/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit2_Nfit" #XXX update to your directory

#------
#queuename=workday
queuename=testmatch

dstver=$1
AmsVer="B1130"

charge=$2
# qver=$3
# NVar=$4
qver=1
NVar=5
XY=$5
layer=$6
XY=2
layer=1
L1QSel=$3 #L1Q Sel Range -1[Z-0.5,Z+0.5] 0[Z-0.25,Z+0.5] 1[Z-0.5,Z+0.25] 2[Z-0.25,Z+0.25]

Version=q$2 #XXX change to your version
#---
execname=protest_KeysPdfRead5_sdiat.sh
#--
if [[ $layer -eq 1 ]]; then
	mycc=KeysPdfRead_Sur
else
	mycc=KeysPdfRead_AfterShift
fi
# mycc=KeysPdfRead_Sur #script name
# mycc=KeysPdfRead_AfterShift #script name
cpcfile="${mycc}.C"

#---
odir=${wdir}/amsd${dstver}n_${AmsVer}_${Version}_NVar${NVar}_L${layer}_L1Q${L1QSel}

echo "odir=${odir}"
echo "charge=${charge}"
echo "qver=${qver}"
echo "run file=${cpcfile}"
# echo "JobFlavour is ${queuename}"
if [[ $L1QSel -eq 0 ]]; then
	echo "L1Q cut range : [Z-0.25,Z+0.5]"
elif [[ $L1QSel -eq 1 ]]; then
	echo "L1Q cut range : [Z-0.5,Z+0.25]"
elif [[ $L1QSel -eq 2 ]]; then
	echo "L1Q cut range : [Z-0.25,Z+0.25]"
else
	echo "L1Q cut range : [Z-0.5,Z+0.5]"
fi

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

span=1
if [ $# -ge 7 ]; then
	span=$7
fi

#---
fRig=0
lRig=36 #XXX change the range of rigidity bin accordingly
if [ $# -ge 9 ] && [ $8 -ne -1 ]; then
	fRig=$8
	lRig=$9
fi

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
echo "MY.WantOS = \"el7\"" >> ${submitfilen}


echo "" >> ${submitfilen}
echo "initialdir = ${odir}"  
echo "initialdir = ${odir}"  >> ${submitfilen}
echo "" >> ${submitfilen}
echo "+JobFlavour = \"${queuename}\"" 
echo "+JobFlavour = \"${queuename}\"" >> ${submitfilen}


for ((irig=${fRig}; irig<${lRig}; irig++))
do
	jobname=R${irig}_q${charge}_amsd${dstver}n_${mycc}_NVar${NVar}_xy${XY}_layer${layer}_L1Q${L1QSel}
	echo "jobname = ${jobname}"
	
	#------create argument files
	echo "${cdir}" "${mycc}" "${codefile}" "${irig}" "${odir}" "${charge}" "${qver}" "${NVar}" "${XY}" "${layer}" "${span}" "${L1QSel}" > ${odir}/submit/arg/${jobname}
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
