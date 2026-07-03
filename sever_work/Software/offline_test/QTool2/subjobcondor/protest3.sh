#!/bin/bash
#Author Qi Yan

cdir=$1
mycc=$2
cpcfile=$3
ifile=$4
ofile=$5 
runopt1=$6
maketool=$7
runopt1=$(eval echo ${runopt1})
maketool=$(eval echo ${maketool})

echo "1=$1"
echo "2=$2"
echo "3=$3"
echo "4=$4"
echo "5=$5"
echo "6=$6"
echo "7=$7"
#exit

#--------
#-----(please change to yours)
source /afs/cern.ch/user/q/qyan/public/DSTvdev/amsvar_all.sh
export AMSDataDir=/cvmfs/ams.cern.ch/Offline/AMSDataDir


#--ifile qry
runlist=(`cat ${ifile}`)
for (( m=0; m<${#runlist[@]}; m++ ))
do
  if  ! echo "${runlist[m]}" | grep castor ; then
    continue
  fi 
  if  ! stager_qry -M ${runlist[m]} | grep STAGED ; then
    stager_get -M ${runlist[m]}
  fi
done


#----cp file
curdir=`pwd`
tmpofile=${curdir}/`basename ${ofile}`
odir=`dirname ${ofile}`

#------
#cd ${cdir}
#if [ -e ${curdir}/${mycc} ] ; then
#  curdir="`dirname ${ofile}`"/"`basename ${ifile}`"
#  mkdir -p $curdir
#fi
echo "pwd=$curdir"

#---cpcfile
mcpcfile=6
for ((icp=1; icp<=${mcpcfile}; icp++ ))
do
  if [[ ${icp} -ge 4 && "${cpcfile}" =~ "/eos/" ]] ; then
    eos cp ${cpcfile} ${curdir}
  else
    cp ${cpcfile} ${curdir}
  fi
  cpstat=$?
  [[ "$cpstat" == "0" ]] && break
  echo "${icp}Copy-Failed!!! cpcfile=${cpcfile} curdir=${curdir} cpstat=${cpstat}"
  sleep 3m
done
echo "cpcfile=${cpcfile}"


#---process
cd ${curdir}
mprocess=3
mcptry=5
for  ((ipro=1; ipro<=${mprocess}; ipro++ ))
do
  substat=0
  if [ -n "$maketool" ] ; then
    if [ ! -f "${mycc}" ] ; then
       ${maketool} ${mycc}
    fi
    chmod +x ${mycc}
    exemycc=${mycc}
    [[ ! "${exemycc}" =~ "/" ]] && exemycc="./${exemycc}"
    ${exemycc}  "$ifile" "$tmpofile" "$runopt1"
    substat=$?
  elif [ -n "$runopt1" ] ; then
    root -b -q ${mycc}+'("'$ifile'","'$tmpofile'","'$runopt1'")'
  else 
    root -b -q ${mycc}+'("'$ifile'","'$tmpofile'")'
  fi
#---cp
  if  [ -f "$tmpofile" ] ; then
    tmpofile1=$tmpofile
#    tmpofile1=`echo "$tmpofile" | sed 's/.root//'`
    for ((icp=1; icp<=${mcptry}; icp++ ))
    do
      if [[ "${ofile}" =~ "/eos/" ]] ; then
        eos cp ${tmpofile1}* ${odir}
      elif [[ "${ofile}" =~ "/castor/" ]] ; then
        rfcp ${tmpofile1}* ${odir}
      else
        cp ${tmpofile1}* ${odir}
      fi
      substat=$?
      [[ "$substat" == "0" ]] && break
      echo "${icp}Copy-Failed!!! tmp=${tmpofile} odir=${odir} cpstat=${substat}"
      sleep 60s
    done
    rm ${tmpofile}
  fi
#---status
  if [[ "$substat" == "2" ]] ; then
    echo "Zero Process substat=${substat} ${tmpofile}"
    break
  elif [[ "$substat" == "0" ]] ; then
    echo "Normal Process substat=${substat} ${tmpofile}"
    break
  else
    echo "Error Process substat=${substat} ${tmpofile}"
  fi
#----
  echo "${ipro}Processing-Failed!!! ${tmpofile}"
  sleep 20m
done 

echo "JobOK"
