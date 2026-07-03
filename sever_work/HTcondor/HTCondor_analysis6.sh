#!/bin/bash
#----------
#scrpit to create argument lists & submit scrpit for HTCondor
#----------
#$1: dst version
#$2: ISS (0) or MC (1)
#$3: for ISS ($2=0): trigger period
#    for MC ($2=1): MC focus
#$4: version to analysis
#$5: MC charge
#$6: MC Mass
#$7: geomagnetic cutoff safety factor #2020.03.09
#$8: update the code or not: 1: update, 0: use already stored
#$9&{10}: first and last list to run, for new Trigger ISS, start from 104 for 100, 209 for 50 or simply "grep "/1456" *"
#${11}: process how many events in one list, -1 for all

wdir="/afs/cern.ch/work/j/jzhang2/AMS"

hversion=14
version=analysis6
mycc=run_${version} #script name
#cpcfile="${mycc}.C ${version}.C readfile5.C SelEvent${hversion}.h QSplineFit.C ChargeCalN9.C TrRes5.C RigEstiCal3.h Efficiency11.h RigReso2.h HistoMan.C ReadRigCalib.h B800MCCor.C EffVal3.h EstiBins.h weight.h file_io.h"
#cpcfile="${mycc}.C ${version}.C readfile5.C SelEvent${hversion}.h QSplineFit.C ChargeCalN9.C TrRes6.h RigEstiCal3.h Efficiency11.h RigReso2.h HistoMan.C ReadRigCalib.h B800MCCor.C EffVal3.h EstiBins.h weight.h file_io.h"
cpcfile="${mycc}.C ${version}.C gbatch_rootlogon.C readfile5.C SelEvent${hversion}.h QSplineFit.C ChargeCalN9.C TrRes6.h RigEstiCal3.h Efficiency11.h RigReso2.h HistoMan.C ReadRigCalib.h B800MCCor.C EffVal3.h EstiBins.h weight.h file_io.h" #2020.09.06: gbatch_rootlogon.C

#nrunlist=30
#queuename=workday
#2020.09.06
nrunlist=20
#queuename=workday
queuename=tomorrow
#queuename=testmatch
if [[ $2 -eq 1 ]]; then
	nrunlist=100
	queuename=workday
	#queuename=longlunch
	[[ $5 -eq 2 ]] && nrunlist=20
fi

#----dst version
dstver=$1
amssoft_ver="B1130"
prod_ver=amsd${dstver}n

dst=${prod_ver}

MCfocus=( "l1" "l19" )

if [[ $2 -eq 0 ]]; then
	if [[ $3 -gt 0 ]] && [[ $3 -lt 3 ]]; then
		dst=${dst}_trig${3}
	elif [[ $3 -eq 3 ]]; then
		dst=${dst}_EcalCheck
	fi
elif [[ $2 -eq 1 ]]; then
	if [[ $5 -eq 2 ]]; then
		sQ=He
		Mass=4
	elif [[ $5 -eq 3 ]]; then
		sQ=Li
	elif [[ $5 -eq 4 ]]; then
		sQ=Be
	elif [[ $5 -eq 5 ]]; then
		sQ=B
	elif [[ $5 -eq 6 ]]; then
		sQ=C
		Mass=12
	elif [[ $5 -eq 7 ]]; then
		sQ=N
	elif [[ $5 -eq 8 ]]; then
		sQ=O
		Mass=16
	elif [[ $5 -eq 9 ]]; then
		sQ=F
		Mass=19
	elif [[ $5 -eq 10 ]]; then
		sQ=Ne
		Mass=20
	elif [[ $5 -eq 11 ]]; then
		sQ=Na
		Mass=23
	elif [[ $5 -eq 12 ]]; then
		sQ=Mg
		Mass=24
	elif [[ $5 -eq 13 ]]; then
		sQ=Al
		Mass=27
	elif [[ $5 -eq 14 ]]; then
		sQ=Si
		Mass=28
	elif [[ $5 -eq 16 ]]; then
		sQ=S
		Mass=32
	elif [[ $5 -eq 26 ]]; then
		sQ=Fe
		Mass=56
	fi
	
	[[ $# -ge 6 ]] && [[ $6 -gt 0 ]] && Mass=$6
	sQ=${sQ}${Mass}
	
	dst=${dst}MC${sQ}${MCfocus[$3]}
fi

AnalVer=$4
if [[ "$AnalVer" == "0" ]]; then
	#--ChargeCalN8
	#aver=L1Q2ndCheck6 #2020.03.05
	#aver=L8L9Sur3_LQSelEff #2020.04.12
	#aver=QDis #2020.06.02: Iron charge distribution
	#aver=LQTemp4 #2020.11.21
	aver=L8L9Sur_LQSel #2020.12.14
elif [[ "$AnalVer" == "1" ]]; then
	#--Rigidity estimator
	aver=RigEstiCal
elif [[ "$AnalVer" == "2" ]]; then
	#--Efficiency
	#aver=newBin3 #2020.02.06: all efficiencies with cutoff
	#aver=Eff #all efficiencies, reweigh with single power law
	#aver=TkQEffCheck
	#aver=Eff2
	#aver=Eff3
	#aver=Eff4
	#aver=Eff_HeBin
	#aver=Eff_FluxWeigh
	#aver=Eff5 #2020.07.30
	#aver=Eff_8hyr #2020.07.30
	#aver=Eff6 #2020.07.31
	#aver=Eff7 #2020.07.31
	#aver=Eff8 #2020.08.03
	#aver=Eff8_FluxWeigh #2020.08.06
	#aver=Eff8_FluxWeigh_8hyr #2020.08.06
	#aver=Eff8_FluxWeigh_5yr #2020.08.18
	#aver=EffCorRig #2020.09.09
	#aver=EffCorRig4Q8_FluxWeigh #2020.09.09
	#aver=EffCorRig4NeSi_FluxWeigh #2020.09.09
	#aver=TkEffSamCheck #2020.09.10#
	#aver=EffCorRig_FNa #2020.10.01
	#aver=EffCorRig_FNa2 #2020.10.06
	#aver=EffCorRig_FNa3 #2020.10.08
	#aver=EffCorRig_FNa3_nearby #2020.10.12
	#aver=EffCorRig_FNa3_FluxWeigh #2020.10.18
	#aver=EffCorRig_FNa3_FluxWeigh_nearby #2020.10.18
	#aver=FNa3_FluxWeigh #2020.10.19
	#aver=FNa3_FluxWeigh_EffSec #2020.10.19
	#aver=FNa4_FluxWeigh_EffSec #2020.10.19
	#aver=FNa5_FluxWeigh_EffSec #2020.11.09
	#aver=FNaAl_FluxWeigh #2020.11.15
	#aver=FNaAl_FluxWeigh_nearby #2020.11.15
	#aver=FNaAl_FluxWeigh_EffSec #2020.11.15
	#aver=FNaAlTightFV_FluxWeigh #2020.11.16
	#aver=FNaAl2_FluxWeigh_EffSec #2020.11.20
	#aver=FNaAl3_FluxWeigh_nearby #2020.11.27
	#aver=FNaAl4_FluxWeigh #2020.11.28
	#aver=EffPri_FluxWeigh #2020.12.09
	#aver=EffPriOT_FluxWeigh #2020.12.09
	#aver=EffPriNT_FluxWeigh #2020.12.09
	#aver=FNaAl4NT_FluxWeigh #2020.11.28
	#aver=FNaAl4OT_FluxWeigh #2020.11.28
	#2020.12.12
	#if [[ $dstver -eq 63 ]]; then
	#	#aver=EffPriOT_FluxWeigh
	#	aver=EffPriNT_FluxWeigh
	#elif [[ $dstver -eq 64 ]]; then
	#	#aver=FNaAl4OT_FluxWeigh
	#	aver=FNaAl4NT_FluxWeigh
	#fi
	#aver=LiBN #2020.12.23
	#aver=LiBN2 #2020.12.25
	#aver=LiBN3 #2021.01.17
	#aver=LiBN3_nearby #2021.01.18
	#aver=LiBN3_FluxWeigh #2021.02.09
	#aver=LiBN4_FluxWeigh #2021.02.09
	#aver=LiBN4_nearby #2021.02.09
	#aver=RichEff #2022.05.29
	#aver=RichEff2 #2022.05.30: use unbiased RichGeom
	#aver=HZBin_10yr_FluxWeigh #2022.06.26
	#aver=RichEff3 #2022.07.08: new mixture for Be, store the rich eff vs genrig + use MC truth to select unfragmented sample
	# aver=HZBin_10yr_FluxWeigh #2022.09.14: reweigh using MIT 10yr He flux
	aver=Pass8_11p5_F19_Eff  #2023.09.11
elif [[ "$AnalVer" == "3" ]]; then
	#--Tracker residual
	#aver=TrRes2 #with tracker fiducial exam for chis and fiducial volume
	aver=TrRes #2020.08.10
elif [[ "$AnalVer" == "4" ]]; then
	#--TkEff validarion
	aver=EffVal3_TkEff_passecal #store for Beta and Cutoff requiring passing Ecal
elif [[ "$AnalVer" == "5" ]]; then
	#--Trd Nuclear Interaction Control
	aver=TrdInterCon4 #TRDINTERCON, TRDVAR2D
elif [[ "$AnalVer" == "6" ]]; then
	#--Rigidity resolution
	#aver=RigReso #2019.11.11
	#aver=RigBetaReso #2020.12.26
	#aver=RigBetaReso2 #2020.12.31
	#aver=RigReso2 #2021.03.13
	#aver=RigReso3 #2021.03.13
	#aver=RigReso4 #2021.06.27
	#aver=MassTemp #2021.09.06
	aver=MassTemp3 #2021.09.14
	#aver=LiBNISO #2021.09.15
	#aver=LiBNISO2 #2021.09.17
elif [[ "$AnalVer" == "7" ]]; then
	#--Acceptance
	#aver=TOI5 #2020.02.07: reweight with flux times exposure time
	#aver=SurProb #2020.03.18: survival probability up to different layers
	#aver=TOI6 #2020.10.05: survival probability up to different layers
	#aver=TOI7 #2020.10.18
	#aver=TOI8 #2020.10.28
	#aver=TOI9 #2020.11.26
	#aver=TOI9_NewReweigh #2020.11.28
	#aver=TOI9_LowQ #2021.02.01
	#aver=SurProb #2021.02.15
	#aver=TOI10 #2021.02.21
	#aver=TOI11 #2021.03.27
	#aver=TOI12 #2021.04.18
	aver=TOI13 #2021.05.23
elif [[ "$AnalVer" == "8" ]]; then
	#--Unbiased hit check
	aver=UBHitCheck2
elif [[ "$AnalVer" == "9" ]]; then
	#--MC generaeted events
	aver=NGen
elif [[ "$AnalVer" == "10" ]]; then
	#--Event Count
	#aver=L1L2SUR7 #2020.02.14: apply rigidity cutoff as well
	#aver=L1L2SUR8 #2020.03.11: separate different trigger period
	#aver=UnderCutoff_EstRigCheck #2020.04.18: under cutoff check and estimated rig check
	#aver=UnderCutoff_EstRigCheck2 #2020.04.20
	#aver=L1L2SUR8Q8 #2020.04.20
	#aver=Q15Q16EvCount #2020.04.26
	#aver=EvCount #2020.08.15
	#aver=QYEvCount6 #2020.08.26
	#aver=EvCountCorRig #2020.09.06
	#aver=L1L2SUR2 #2020.11.02
	#aver=LiBN3_EventCount #2021.01.19
	#aver=BetaRig #2021.04.01
	#aver=InnerAcc #2021.10.22
	#aver=RichIndex8 #2021.11.03
	#aver=MassDis_CO #2021.11.04
	#aver=MassDisCor_CO #2021.11.05
	#aver=RichVarCheck #2022.08.02
	#aver=RichVarCheck2 #2022.08.17: store 2d distribution in more rigidity range
	#aver=RichVarCheck3 #2022.09.10
	#aver=RichGeomCheck #2022.09.17
	aver=RichGeomRefl #2022.09.21
#2020.02.18
elif [[ $AnalVer -eq 11 ]]; then
	#--L8L9Sur
	#aver=L8L9Sur2 #2020.02.20: correctly applied the S3/S5 MIPs cut
	aver=L8L9Sur3 #2020.03.26: separate for two trigger setting periods
	dst=${dst}_ECalCheck
else
	echo "Please choose the corrected version to analysis. Exit"
	exit
fi
if [[ $iFAiter -gt 0 ]] && [[ $AnalVer -eq 2 ]] && [[ $2 -eq 1 ]]; then
	aver=${aver}_Unfold${iFAiter}
fi

#if [[ $5 -eq 8 ]] || [[ $5 -eq 10 ]] || [[ $5 -eq 12 ]] || [[ $5 -eq 16 ]]; then
#	aver=${aver}Q$5 
#fi

#----geomagnetic cutoff safety factor
GeoSafety=1
if [[ $# -ge 7 ]]; then
	GeoSafety=$7
fi

[[ ${GeoSafety} -ne 1 ]] && aver=${aver}_GeoSafety${GeoSafety}

#----code update
CodeUpdate=0 #in default to be 0, will only update if the cdir is empty
[[ $# -ge 8 ]] && CodeUpdate=$8

#----process root-file or list dir  ######3
if [[ $2 -eq 0 ]]; then
	indir=${wdir}/runlist/ISS/${amssoft_ver}/${dst}_${nrunlist}
elif [[ $2 -eq 1 ]]; then
	indir=${wdir}/runlist/MC/${amssoft_ver}/${dst}_${nrunlist}
fi

oname=${dst}_${amssoft_ver}_${aver}

odir=${wdir}/result/${version}/${oname}

ldir=${odir}/log

#---
execname=protest_analysis6.csh
#execname=protest_analysis6_randy.csh

ffile=0

lfile=`ls ${indir} | wc -l`
#set ffile and lfile separately
if [[ $# -ge 9 ]] && [[ $9 -gt -1 ]]; then
	ffile=$9
fi
if [[ $# -ge 10 ]]; then
	#read only the run in given list
	if [[ $9 -eq -2 ]]; then
		echo "reading error list: ${odir}/${10}"
		#errorRunlist=(`less ${10}`)
		errorRunlist=(`less ${odir}/${10}`)
		lfile=${#errorRunlist[@]}
		echo "error run list: ${10}"
		echo "total file needs rerun: $lfile"
		#2020.07.10: read the no. of log or out file
		for (( ifile = 0; ifile < lfile; ifile++ ))
		do
			#echo ${errorRunlist[ifile]}
			#echo ${errorRunlist[ifile]} | grep -o "_[0-9]*" | grep -o "[0-9]*"
			errorRunlist[ifile]=`echo ${errorRunlist[ifile]} | grep -o "_[0-9]*" | grep -o "[0-9]*"`
			echo ${errorRunlist[ifile]}
		done
	elif [[ ${10} -ne -1 ]]; then
		lfile=${10}
	fi
fi

nevent=-1
if [[ $# -ge 11 ]] && [[ ${11} -ne -1 ]]; then
	nevent=${11}
fi

echo "process dst from ${ffile} to ${lfile}"
echo "process ${nevent} events in one dst (-1 for all)"
echo "input dir is ${indir}"
echo "output dir is ${odir}"
echo "JobFlavour is ${queuename}"
echo "Geomagnetic cutoff safety factor: ${GeoSafety}"
echo "update code? ${CodeUpdate}"

#2021.02.01
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
[ ! -e  ${odir}/output2 ] && mkdir -p ${odir}/output2 #2022.09.18
[ ! -e  ${odir}/error ] && mkdir -p ${odir}/error
[ ! -e  ${odir}/log ] && mkdir -p ${odir}/log
[ ! -e  ${odir}/code ] && mkdir -p ${odir}/code/
[ ! -e  ${odir}/submit ] && mkdir -p ${odir}/submit/
[ ! -e  ${odir}/submit/arg ] && mkdir -p ${odir}/submit/arg/

cdir=${odir}/code #copy directory
codefile=${cdir}/codefile.txt

[ -e ${codefile} ] && rm ${codefile}

ncdirFile=`ls ${cdir} | wc -l`

for ifile in $cpcfile ; do
	if [[ $CodeUpdate -eq 1 ]] || [[ $ncdirFile -eq 0 ]]; then
		cp -v ${wdir}/analisiP8/script/$ifile ${cdir}
	fi
	echo $ifile >> ${codefile}
done

#------creating argument lists & submitting script
echo "start creating submitting script"
submitfilen=${odir}/submit/${oname}.sub

echo "executable = ${execname}" 
echo "executable = ${execname}" > ${submitfilen}
echo "output     = output/\$(file).out" 
echo "output     = output/\$(file).out" >> ${submitfilen}
echo "error      = error/\$(file).err" 
echo "error      = error/\$(file).err" >> ${submitfilen}
echo "log        = log/\$(file).log" 
echo "log        = log/\$(file).log"  >> ${submitfilen}

echo "" >> ${submitfilen}
echo "initialdir = ${odir}"  
echo "initialdir = ${odir}"  >> ${submitfilen}
echo "" >> ${submitfilen}
#echo "+MaxRunTime = 1000" 
#echo "+MaxRunTime = 1000" >> ${submitfilen}
echo "+JobFlavour = \"${queuename}\"" 
echo "+JobFlavour = \"${queuename}\"" >> ${submitfilen}
echo "+AMSPublic = True" 
echo "+AMSPublic = True" >> ${submitfilen}

for ((i = ffile; i < lfile; i++))
do
	#ifile1=${indir}/${i}
	#[[ $9 -eq -2 ]] && ifile1=${indir}/${errorRunlist[i]}
	#2022.09.19
	ifile0=${i}
	[[ $9 -eq -2 ]] && ifile0=${errorRunlist[i]}
	ifile1=${indir}/${ifile0}
	echo ${ifile1}
	
	#jobname="${dst}_${i}"
	#[[ $3 -eq -2 ]] && jobname="recover_${dst}_${errorRunlist[i]}"
	jobname="${dst}_${aver}_${i}"
	#[[ $9 -eq -2 ]] && jobname="recover_${dst}_${aver}_${errorRunlist[i]}"
	if [[ $9 -eq -2 ]]; then
		echo "Recover jobs: "
		jobname="${dst}_${aver}_${errorRunlist[i]}"
	fi
	echo "jobname = ${jobname}"
	
	#------create argument files

#	if [ -e ${odir}/submit/arg/${jobname} ]; then
##		echo "arguments file ${odir}/submit/arg/${jobname} already exists, removing..."
#		rm ${odir}/submit/arg/${jobname}
#	fi

#	echo "Creating arguments file argfile = ${jobname} in directory ${odir}/submit/arg"
#	echo "${cdir}" "${mycc}" "${cpcfile}" "${ifile1}" "${odir}" "${nevent}" > ${odir}/submit/arg/${jobname}
#	echo "${cdir}" "${mycc}" "${codefile}" "${ifile1}" "${odir}" "${nevent}" > ${odir}/submit/arg/${jobname}
#""	echo "${cdir}" "${mycc}" "${codefile}" "${ifile1}" "${odir}" "${nevent}" "${GeoSafety}" > ${odir}/submit/arg/${jobname} #2020.06.02
#	echo "${cdir}" "${mycc}" "${codefile}" "${ifile1}" "${odir}" "${nevent}" "${GeoSafety}" "${2}" > ${odir}/submit/arg/${jobname} #2020.09.06
	echo "${cdir}" "${mycc}" "${codefile}" "${ifile1}" "${odir}" "${nevent}" "${GeoSafety}" "${2}" "${ifile0}" > ${odir}/submit/arg/${jobname} #2020.09.19
	
	#------updating submitting script
	echo "" >> ${submitfilen}
#	echo "file=${jobname}"
	echo "file=${jobname}" >> ${submitfilen}
#	echo "queue arguments from ${odir}/submit/arg/\$(file)" 
	echo "queue arguments from ${odir}/submit/arg/\$(file)" >> ${submitfilen}
	
	#remove existing log file #2021.04.02
	if [[ -e ${odir}/log/${jobname}.log ]]; then
		#echo "${odir}/log/${jobname}.log"
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
