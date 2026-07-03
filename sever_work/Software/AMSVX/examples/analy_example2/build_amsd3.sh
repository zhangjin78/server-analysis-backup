#!/usr/bin/env bash
#Author Qi Yan

export CLEAN=""
if [[ "$1" == "clean" ]];  then
    export CLEAN=$1
fi


if [[ -z "$AMSWD" ]]; then
    export AMSWD=`cd ../.. && pwd`
fi
source $AMSWD/install/amsroot.sh


#----SRC
#export SRC=analysis_amsd66n
#export SRC=analysis_amsd67n
export SRC=analysis_amsd68n
#----APP
#set EXEAPP=V10
EXEAPP=V11
#----
export USEPRHEION=1
unset USEEVENTORDER
unset USEPRL1
unset USEHEL1
unset USEHEINNER
unset USENEWL1L9G
unset USEMCTKRAW
unset USEONEEV
unset USEADDTKHIT
unset USESAVENEG
unset USENOLINEARCOR
unset USECALIB
#-------
#export USEIONINNER=1
#export USENOLINEARCOR=1
#export USECALIB=1
#export USEMCTKRAW=1
#export USEONEEV=1
#export USEADDTKHIT=1
#export USEHEL1=1
export USEHEINNER=1
export USESAVENEG=1
export USEPRL1=1
#echo
#echo "build Pass4..."
#echo
#unset USEPASS7
#make -f Makefile534_1SL6 SRC_U=$SRC EXEAPP_U=$EXEAPP $CLEAN

echo
echo "build Pass8ORD..."
echo
export  USEEVENTORDER=1
export  USEPASS7=1
make -f Makefile534_1SL6 SRC_U=$SRC EXEAPP_U=$EXEAPP $CLEAN

exit

echo
echo "build Pass8..."
echo
unset USEEVENTORDER
export   USEPASS7=1
make -f Makefile534_1SL6 SRC_U=$SRC EXEAPP_U=$EXEAPP $CLEAN
