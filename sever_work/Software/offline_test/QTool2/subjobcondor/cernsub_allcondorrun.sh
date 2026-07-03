#!/bin/sh
#Author Qi Yan

#----cfile
mycc=run_Sum1_AMS_4_castorA.exe
#mycc=run_Sum1_AMS_5_castorT2A.exe

#----cdir
cdir=/afs/cern.ch/user/q/qyan/public/DSTvdev/QTool2/analysis_eventsel
#cdir=/afs/cern.ch/user/q/qyan/public/DSTvdev/QTool2/analysis_mcacc

#----runkey
runopt1=
#----maketool
#maketool=
maketool=/afs/cern.ch/user/q/qyan/public/DSTvdev/QTool2/subjobcondor/build_amsd2sh

#----oname
oname=amsd68n_B1236P8_template30_testabc2
#oname=amsd68n_PrMCB1236_054000602N0acc51

#---onamep
onamep=0
#onamep=amsd68n_B1236P8_template30_testabc

#---indir
#indir=/eos/ams/group/mit/amsd64n_TMTFNTotHPB_B1130P7
indir=/eos/ams/group/mit/amsd68n_TMTFNTotHB_B1236P8
#indir=/eos/ams/group/mit/amsd68n_ROOT5HB_PrMCB1236l1_05_4000602N0
#----txtdir
#txtdir=/afs/ams.cern.ch/user/qyan/log
txtdir=/afs/cern.ch/work/q/qyan/log/log1
#----odir
#odir=/afs/ams.cern.ch/user/qyan/Data
odir=/eos/ams/group/mit/qyan/Data
#----otxtdir
otxtdir=/afs/cern.ch/work/q/qyan/otxt
if [ -n "$onamep" ] ; then
  otxtdir1=${otxtdir}/${oname}
  [ ! -e  ${otxtdir1} ] && mkdir -p ${otxtdir1}
  [[ "$onamep" != "0" ]] &&  cp ${otxtdir}/${onamep}/Run_* ${otxtdir1}
  runopt1=${otxtdir1}
fi

#---jobkey
#mode=0 mean datemode #mode=1 mean filemode
mode=1
#mode=0 choose 0.5 1 2 3 day mode=1 choose nrun 1 job
#nday=0.5
#nrun=10
#nrun=5
#nrun=7
#nrun=25
nrun=30
#nrun=40
#nrun=30
#subqueue=workday
#subqueue=86400
subqueue=172800
#subqueue=259200
#espresso     = 20 minutes
#microcentury = 1 hour
#longlunch    = 2 hours
#workday      = 8 hours
#tomorrow     = 1 day
#testmatch    = 3 days
#nextweek     = 1 week
#+JobFlavour = "longlunch"
#request=4
#request=2
request=1
#4 CPUs, 8gb of memory and 20gb of disk
#------
#begnam!=0||begday!=0 matching name using selection
begnam=0
endnam=0
#begnam=1208500000
#endnam=1208531587
#begnam=1476604811
#endnam=1476605731
#matching date using selection
begday=0
endday=0
#begday=20110620
#endday=20110920
#endday=23:59:59 20110619

/afs/cern.ch/user/q/qyan/public/DSTvdev/QTool2/subjobcondor/cernsub_allcondor2.sh "$mycc" "$cdir" "$runopt1" "$maketool" "$txtdir" "$odir" ${oname} "$indir" "$mode" "$nrun" "$subqueue" "$request" "$begnam" "$endnam" "$begday" "$endday"
