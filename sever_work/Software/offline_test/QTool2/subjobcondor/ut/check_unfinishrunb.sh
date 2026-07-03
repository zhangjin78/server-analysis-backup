#!/bin/bash
#Author Qi Yan

#----oname
oname=amsd68n_B1236P8_template30_testabc2

#----indir
indir=/eos/ams/group/mit/amsd68n_TMTFNTotHB_B1236P8

#----otxtdir
otxtdir=/afs/cern.ch/work/q/qyan/otxt
finishfile="${otxtdir}/${oname}/Run_*.cvs"

#----output
echo "oname=${oname}"
alltxt=${oname}_all.txt
finishtxt=${oname}_finish.txt
unfinishtxt=${oname}_unfinish.txt

#----eos indir: all files
if [[ "${indir}" =~ "eos" ]] ; then
echo "indireos=${indir}"
eos ls $indir | grep root | sed 's/^.*\///g' | sort | uniq > ${alltxt}
#----afs indir: all files
else
echo "indirafs=${indir}"
ls $indir | grep root | sed 's/^.*\///g' | sort | uniq > ${alltxt}
fi

#----finish files
cat ${finishfile} | sed 's/^.*\///g' | sort | uniq  > ${finishtxt}

#----unfinish files
indir2=`echo $indir | sed 's#\/#\\\/#g'`
comm -23  ${alltxt} ${finishtxt} | sed 's/^/'${indir2}'\//g' | tee ${unfinishtxt}
[ ! -s "${unfinishtxt}" ] && echo "all jobs finished !!!"
