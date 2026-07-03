#!/bin/bash
#set up all AMSsoft environment variables in bash, created by Qi Yan

# Offline directory
export Offline=/cvmfs/ams.cern.ch/Offline
export AMSOPT=/cvmfs/ams.cern.ch/opt
#export Offline=/afs/cern.ch/ams/Offline
#export AMSOPT=/afs/cern.ch/ams/local2/opt

# AMSSoft
if [[ -z "$AMSWD" ]]; then
    export AMSWD=$Offline/vdev
fi
export AMSSRC=$AMSWD
export AMSDataDir=$Offline/AMSDataDir

# icc or gcc
if [[ -z "$UCC" ]]; then
    export UCC=icc64
    #export UCC=gcc64
fi

# root5 or root6
if [[ -z "$UROOT" ]]; then
    export UROOT=5
    #export UROOT=6
fi

# Get Redhat version
export USLC=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`

# Get gcc and icc version with enforcement
if [[ -n "$VGCC" ]]; then
    UGCC=$VGCC
else
    UGCC=0
fi
if [[ -n "$VICC" ]]; then
    UICC=$VICC
else
    UICC=0
fi

# Refine gcc version
if [[ "$UCC" == *gcc* ]]; then
   if [[ "$UGCC" == "0" ]]; then
      if [[ "$USLC" < "7" ]]; then
         UGCC=493
      elif [[ "$USLC" == "7" ]]; then
         UGCC=530
      else
         UGCC=1130
      fi
   fi
# Refine icc version
else
   if [[ "$UICC" == "0" ]]; then
      if [[ "$USLC" == "5" ]]; then
         UICC=2011
      elif [[ "$USLC" < "8" ]]; then
         UICC=2019
      else
         UICC=2023
      fi
   fi
   if [[ "$UGCC" == "0" ]]; then
      if [[ "$UICC" == "2011" ]]; then
         UGCC=493
      elif [[ "$UICC" == "2019" ]]; then
         UGCC=493
      else
         UGCC=1130
      fi
   fi
fi


# choose XRDLIB and ROOT
if [[ "$UROOT" == "6" ]]; then
    export XRDLIB=$AMSOPT/xrootd-icc64-12
    export ROOTSYS=$Offline/root/Linux/root6-04-08-icc16
elif [[ "$USLC" == "5" ]]; then
    export XRDLIB=$AMSOPT/xrootd-icc64-11
    export ROOTSYS=$Offline/root/Linux/root-v5-34-9-$UCC-slc5
elif [[ "$UCC" == *gcc* ]]; then
    export XRDLIB=$AMSOPT/xrootd-gcc64-44
    export ROOTSYS=$Offline/root/Linux/root-v5-34-9-gcc64-slc6
#    export ROOTSYS=$Offline/root/Linux/root-v5-34-9-icc64.17-slc6
else
    export XRDLIB=$AMSOPT/xrootd-icc64-17
#    export ROOTSYS=$Offline/root/Linux/root-v5-34-9-icc64.14-slc6
    export ROOTSYS=$Offline/root/Linux/root-v5-34-9-icc64.17-slc6
fi

# setup output
echo "Offline=$Offline"
echo "AMSWD=$AMSWD"
echo "USLC=$USLC UCC=$UCC UICC=$UICC UGCC=$UGCC UROOT=$UROOT ROOTSYS=$ROOTSYS"
cat /etc/redhat-release

# ROOT and XRDLIB Lib
export LD_LIBRARY_PATH=.:${ROOTSYS}/lib:${XRDLIB}/lib64
export PATH=$ROOTSYS/bin:.:$PATH:/usr/sbin
if [[ "$UROOT" == "6" ]]; then
    source $ROOTSYS/bin/thisroot.sh
fi

# Aux Lib
if [[ "$USLC" > "6" ]]; then
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/cvmfs/ams.cern.ch/opt/castor.cc7/lib
fi
if [[ "$USLC" == "9" ]]; then
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/cvmfs/ams.cern.ch/opt/lib64_EL9
fi
#source /afs/cern.ch/project/oracle/script/setoraenv.sh

# Compiler Lib
export INTEL_LICENSE_FILE=$Offline/intel/licenses
# gcc Lib
if [[ "$UGCC" == "493" ]]; then
    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh /afs/cern.ch/sw/lcg/external
elif [[ "$UGCC" == "530" ]]; then
    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/5.3.0/x86_64-centos7-gcc53-opt/setup.sh /afs/cern.ch/sw/lcg/external
elif [[ "$UGCC"  >  "0" ]]; then
#    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/8.3.0/x86_64-centos7-gcc8-opt/setup.sh /afs/cern.ch/sw/lcg/external
    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/11.3.0/x86_64-el9-gcc11-opt/setup.sh /afs/cern.ch/sw/lcg/external
#    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/12.1.0/x86_64-el9-gcc12-opt/setup.sh /afs/cern.ch/sw/lcg/external
fi
# gcc Lib
if [[ "$UICC" == "2011" ]]; then
    export LD_LIBRARY_PATH=${AMSOPT}/intel/Compiler/11.1/073/idb/lib/intel64:${AMSOPT}/intel/Compiler/11.1/073/lib/intel64:/opt/intel/compiler80/lib:${LD_LIBRARY_PATH}
    export PATH=$AMSOPT/intel/Compiler/11.1/073/bin/intel64:$PATH
elif [[ "$UICC" == "2019" ]]; then
    export LD_LIBRARY_PATH=${AMSOPT}/intel/composer_xe_2013_sp1.3.174/compiler/lib/intel64:/opt/intel/compiler80/lib:${LD_LIBRARY_PATH}
    export PATH=$AMSOPT/intel/composer_xe_2013_sp1.3.174/compiler/bin/intel64:$PATH
#--------
    export INTELSW=/cvmfs/projects.cern.ch
    source $INTELSW/intelsw/psxe/linux/all-setup.sh
#--------
#    source $INTELSW/intelsw/psxe/linux/x86_64/2017/compilers_and_libraries/linux/bin/compilervars.csh   intel64
#    export INTELDIR=$INTELSW/intelsw/psxe/linux/x86_64/2017
#    export INTELVER=compilers_and_libraries_2017.4.196
#    export INTELVER=compilers_and_libraries_2017.2.174
    source $INTELSW/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.sh   intel64
    export INTELDIR=$INTELSW/intelsw/psxe/linux/x86_64/2019
#    export INTELVER=compilers_and_libraries_2019.4.243
    export INTELVER=compilers_and_libraries_2019.3.199
#    export INTELVER=compilers_and_libraries_2019.0.117
elif [[ "$UICC" > "0" ]]; then
    export INTELSW=/cvmfs/projects.cern.ch
    export INTELDIR=$INTELSW/intelsw/oneAPI/linux/x86_64/2023
    export INTELVER=compiler/latest/linux
    source $INTELDIR/$INTELVER/../env/vars.sh
fi


# Configuration 
export VERBOSE=1
export GENFIT=1
export DPMJET3=6
export G4MULTITHREADED=1
export CASTORSTATIC=1
export AMSICC=1
export AMSP=1
export G4AMS=1
export PGTRACK=1
if [[ "$UCC" == *gcc* ]]; then
#   unset AMSICC
    unset G4MULTITHREADED
    export CXX=g++
    export CC=gcc
    export FC=gfortran
else
    export CXX=icc
    export CC=icc
    export FC=ifort
fi


# Display
export AMSGeoDir=$Offline/vdev/display/
export amsedcPG=$Offline/vdev/exe/linuxx8664icc5.34/amsedcPG
export offmoncPG=$Offline/vdev/exe/linuxx8664icc5.34/offmoncPG


# Enable Aachen TrdQt interface
#export AMS_ACQT_INTERFACE=1
#export ACROOTSOFTWARE=$AMSDataDir/v5.00/TRD
#export ACROOTLOOKUPS=$ACROOTSOFTWARE/acroot/data/
#export QTDIR_STATIC=$Offline/AMSsoft/AMSQtCore/qt-4.8.3/
#export QTDIR_STATIC_32=$Offline/AMSsoft/AMSQtCore/qt-4.8.3-32bit/


# Geant4
#export G4POLARGAMMA=1
#unset G4POLARGAMMA
source $AMSWD/install/g4i10.1.sh
#source $AMSWD/install/g4i10.3.sh
#source $AMSWD/install/g4i.icc64.sh


# Aux Configureation 
export CVSROOT=/afs/cern.ch/exp/ams/Offline/CVS
export CVS_RSH=ssh
if [[ -z "$EOS_MGM_URL" ]]; then
  export EOS_MGM_URL="root://eosams.cern.ch"
fi
export QToolWD=/afs/cern.ch/user/q/qyan/public/DSTvdev/QTool2


#ulimit -c 0
#ulimit -d 700000
#ulimit -s 128000
