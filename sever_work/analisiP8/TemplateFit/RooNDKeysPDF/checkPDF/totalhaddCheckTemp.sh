#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh

wdir=/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/OutputFile/RebinRoot_TotalL1
# wdir=/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/OutputFile/RandomRoot

# charge=12
# for ((charge=12; charge<19; charge++))
for ((charge=24; charge<29; charge++))
do
    for ((irho=1; irho<3; irho++))
    do
        for ((isigma=1; isigma<4; isigma++))
        do
        # namedir=LQTemp_Rebin_Z7TZ18_NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0
        namedir=LQTemp_Rebin_Z24TZ28_NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0
        # namedir=LQTemp_CutHe_Z7TZ18_NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0
        tempdir=runQ${charge}
            for ((il=1; il<3; il++))
            do
                rootdir=${wdir}/${namedir}/${tempdir}
                cd ${rootdir}
                # nameFile=NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0_runQ${charge}_L${il}QTemp
                nameFile=NDKeys_Rebin_rho${irho}p0_sigma${isigma}p0_runQ${charge}_L${il}QTemp

                rm ${nameFile}.root
                hadd ${nameFile}.root ${rootdir}/L${il}/Root/*
            done
        
        done
    done
done