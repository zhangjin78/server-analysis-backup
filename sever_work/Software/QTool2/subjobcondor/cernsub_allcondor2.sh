#!/bin/bash
#Author Qi Yan
#-----(please change to yours)
#export AMSWD=/afs/cern.ch/work/q/qyan/AMSVDEV/AMS308
export AMSWD=/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ
#----
mycc=$1
cdir=$2
runopt1=$3
maketool=$4
txtdir=$5
odir=$6
oname=$7
indir=$8
mode=$9
nrun=${10}
subqueue=${11}
request=${12}
begnam=${13}
endnam=${14}
begday=${15}
endday=${16}
nday=${17}


echo "1=$1"
echo "2=$2"
echo "3=$3"
echo "4=$4"
echo "5=$5"
echo "6=$6"
echo "7=$7"
echo "8=$8"
echo "9=$9"
echo "10=${10}"
echo "11=${11}"
echo "12=${12}"
echo "13=${13}"
echo "14=${14}"
echo "15=${15}"
echo "16=${16}"
echo "17=${17}"
#exit

#---control value
subname=/afs/cern.ch/user/q/qyan/public/DSTvdev/QTool2/subjobcondor/protest3.sh
#---------cfile
cpcfile="${mycc}.C"

#---------txtdir+txtnam
txtdir=${txtdir}/${oname}
if [ ! -e  ${txtdir} ] ; then
  mkdir -p ${txtdir}
else
  rm ${txtdir}/*.in
fi
txtnam=${oname}

#---------odir+logdir
[ -z "$runopt1" ] && runopt1="''"
odir=${odir}/${oname}
ldir=${odir}/log
#----!!!condor did not support eos as logdir
if [[ "${ldir}" =~ "/eos/" ]] ; then
  ldir=${txtdir}/logeos
  echo "ldir=${ldir}"
fi
#-----
[ ! -e  ${odir}/${oname} ] && mkdir -p ${odir}/${oname}
[ ! -e  ${ldir} ] && mkdir -p ${ldir}

#---------maketool
if [ -z "$maketool" ] ; then
  maketool="''"
elif [[ "$mycc" =~ ".exe" ]] ; then
  mycc=(`echo ${mycc} | cut -f 1 -d "."`)  
  ${maketool} ${cdir}/${mycc} clean
  ${maketool} ${cdir}/${mycc}
  mycc=${mycc}.exe
  mv ${cdir}/${mycc} ${odir}
  cpcfile=${odir}/${mycc}
  echo "mycc=${mycc} cpcfile=${cpcfile}"
fi

#-------jobkey
[[ $begday != 0 ]] && begnam=$(date +%s -d "${begday} UTC")
[[ $endday != 0 ]] && endnam=$(date +%s -d "${endday} UTC")

#-------writetxt0
if echo "$indir" | grep castor ; then
   infile=(`nsls ${indir}`)
elif  echo "$indir" | grep eos ; then
   infile=(`eos ls ${indir}`)
elif  echo "$indir" | grep ".txt" ; then
   infile=(`cat ${indir}`)
else
   infile=(`ls ${indir}`)
fi

#------writetxt
itxt=0
declare -a txtfil
declare -a indirfiles

contrun=$(($nrun+1))
befrun=0
netime=0
echo "nfile=${#infile[@]}"

for  ((i=0; i<${#infile[@]}; i++ ))
do
#  ifile=${infile[i]}
  ifile=`basename ${infile[i]}`
  indirfile=${indir}/${ifile}
#  if ! echo "$ifile" | grep -q .root ; then
  if [[ ! "${ifile}" =~ ".root" ]] ; then
     continue
  fi
  if [[ "$(dirname ${infile[i]})" =~ "/" ]] ; then
    indirfile=${infile[i]}
  fi
#  run=`echo $ifile| cut -f 1 -d "."`
  run=`echo $ifile| cut -f 1 -d "_" | cut -f 1 -d "."`
#-----Add
  if [[ "$netime" == "0" ]] ; then
     netime=${run}
     echo "run0=$netime"
  fi
#-----
  [[ ${begnam} != 0 && ${run} < ${begnam} ]] && befrun=${run} && netime=${begnam} && continue
  [[ ${endnam} != 0 && ${run} > ${endnam} ]] && befrun=${run} && netime=${begnam} && continue  
#date mode
  if [ $mode -eq 0 ]; then
     if [ $run -ge $netime ]; then
#-----Add
       while [ `echo "scale=0; ${netime}+86400*${nday}/1" |bc -l` -le $run ]
       do
         netime=$( echo "scale=0; ${netime}+86400*${nday}/1"|bc -l)
       done
#-----
       day=`date +%Y%m%d%H -u -d "1970-01-01 UTC ${netime} sec"`
       txtfil[$itxt]="${txtdir}/${run}_${day}_${nday}day.in"
#       [ -e ${txtfil[$itxt]} ] && rm ${txtfil[$itxt]}
       itxt=$(($itxt+1))
       netime=$( echo "scale=0; ${netime}+86400*${nday}/1"|bc -l)
     fi
     if [ ! -z ${indirfiles[$(($itxt-1))]} ]; then
        indirfiles[$(($itxt-1))]+="\n"
     fi
     indirfiles[$(($itxt-1))]+="${indirfile}"
#     echo -e "${indirfile}" >> ${txtfil[$(($itxt-1))]}   
#file mode
  elif [ $mode -eq 1 ]; then
#      if [ $contrun -ge $nrun ] && [ "$run" -ne $befrun ] ; then
       if [ $contrun -ge $nrun ] && [[ "$run" != "$befrun" ]] ; then
         txtfil[$itxt]="${txtdir}/${run}_${nrun}file.in"
#         [ -e ${txtfil[$itxt]} ] && rm ${txtfil[$itxt]}
         itxt=$(($itxt+1))
         contrun=0
      fi
      if [ ! -z ${indirfiles[$(($itxt-1))]} ]; then
         indirfiles[$(($itxt-1))]+="\n"
      fi
      indirfiles[$(($itxt-1))]+="${indirfile}"
#      echo -e "${indirfile}" >> ${txtfil[$(($itxt-1))]}
      if [[ "$run" != "$befrun" ]] ; then 
        contrun=$((${contrun}+1))
      fi
      befrun=$run
  else
     exit
  fi
done


echo  "write ntxt=${#txtfil[@]}"
for  ((j=0; j<${#txtfil[@]}; j++ ))
do
  echo -e "${indirfiles[j]}" > ${txtfil[j]}
done

#exit


#-------generate condor
function create_submit() {
  echo "########################"
  echo "#"
  echo "# Submit description file for Q.Yan"
  echo "#"
  echo "########################"
  echo "universe = vanilla"
  echo "+MaxRuntime  = ${subqueue}"
  echo '+AccountingGroup = "group_u_AMS.dev"'
#  echo '+AccountingGroup = "group_u_AMS.prod"'
#  echo '+AccountingGroup = "group_u_AMS.u_va"'
  echo '+AMSPublic = True'
#  echo "+JobFlavour = ${subqueue}"
  echo "RequestCpus = ${request}"
  echo "request_disk = 2GB"
#  echo "request_memory = ${memory}GB"
  echo "executable = ${subname}"
#  echo 'requirements = (OpSysAndVer =?= "CentOS7")'
#  echo 'MY.WantOS = "el7"'
#  echo 'requirements = OpSys == "LINUX" && (Arch != "DUMMY")'
#  echo 'requirements = (OpSysAndVer =?= "CentOS7" || OpSysAndVer =?= "SLCern6")'
  echo "arguments = ${cdir} ${mycc} ${cpcfile} \$Fnx(indata) ${odir}/${oname}/\$Fn(indata).root ${runopt1} ${maketool}"
  echo 'ID = $(ClusterId).$(ProcId)'
  echo "output                =${ldir}/\$Fn(indata).\$(ID).out"
  echo "error                 =${ldir}/\$Fn(indata).\$(ID).err"
  echo "log                   =${ldir}/\$Fn(indata).\$(ID).log"
  echo "should_transfer_files = YES"
#  echo "should_transfer_files = NO"
  echo "when_to_transfer_output = ON_EXIT"
  echo 'transfer_input_files    = $(indata)'
  echo 'transfer_output_files   = ""'
  echo "queue indata matching files ${txtdir}/*.in"
}

create_submit >  ${oname}.sh

#-------submit condor
condor_submit ${oname}.sh -batch-name ${oname}
