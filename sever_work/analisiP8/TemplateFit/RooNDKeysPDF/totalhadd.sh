#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.csh

wdir=/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/OutputFile

charge=14

for ((irho=1; irho<3; irho++))
do
    for ((isigma=1; isigma<4; isigma++))
    do
    namedir=LQTemp_CutHe_Z7TZ18_NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0
    if [[ "${charge}" == "26" ]]; then
        namedir=LQTemp_CutHe_Z24TZ28_NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0
    fi
    tempdir=r1_xy2_NVar5_rebin2_q${charge}
    cd ${wdir}/${namedir}/${tempdir}

    nameFile=NDKeys_NoShift_rho${irho}p0_sigma${isigma}p0_r1_xy2_NVar5_rebin2_q${charge}_L1QTemp
    # nameFile=NDKeys_NoShift_Front_rho${irho}p0_sigma${isigma}p0_r1_xy2_NVar5_rebin2_q${charge}_L1QTemp

    rm ${nameFile}.root
    hadd ${nameFile}.root L1QTempFit_L1QTemp/Root/*
    done
done