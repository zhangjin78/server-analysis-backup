# set up all AMSsoft environment variables in csh, created by Qi Yan

# Offline directory
setenv Offline /cvmfs/ams.cern.ch/Offline
setenv AMSOPT /cvmfs/ams.cern.ch/opt
#setenv Offline /afs/cern.ch/ams/Offline
#setenv AMSOPT /afs/cern.ch/ams/local2/opt

# AMSSoft
if ( ! $?AMSWD ) then
  setenv AMSWD $Offline/vdev
endif
setenv AMSSRC $AMSWD
setenv AMSDataDir $Offline/AMSDataDir

# icc or gcc
if (! $?UCC) then
  setenv UCC icc64
  #setenv UCC gcc64
endif

# root5 or root6
if (! $?UROOT) then
  setenv UROOT 5
  #setenv UROOT 6
endif

# Get Redhat version
setenv USLC `cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`

# Get gcc and icc version with enforcement
if ($?VGCC) then
  set UGCC=$VGCC
else
  set UGCC=0
endif
if ($?VICC) then
  set UICC=$VICC
else
  set UICC=0
endif

# Refine gcc version
if (`echo $UCC | grep -c "gcc"`) then
  if ($UGCC == 0) then
    if ($USLC < 7) then
       set UGCC=493
    else if ($USLC == 7) then
       set UGCC=530
    else
       set UGCC=1130
    endif
  endif
# Refine icc version
else 
  if ($UICC == 0) then
    if ($USLC == 5) then
      set UICC=2011
    else if ($USLC < 8) then
      set UICC=2019
    else
      set UICC=2023
    endif
  endif
  if ($UGCC == 0) then
    if ($UICC == 2011) then
       set UGCC=493
    else if ($UICC == 2019) then
       set UGCC=493
    else
       set UGCC=1130
    endif 
  endif
endif


# choose XRDLIB and ROOT
if ($UROOT == 6 && $USLC < 9) then
 setenv XRDLIB $AMSOPT/xrootd-icc64-12
# setenv ROOTSYS $Offline/root/Linux/root6-04-08-icc16 
# setenv ROOTSYS /afs/cern.ch/ams/Offline/root/Linux/root6-04-08-icc16
 setenv ROOTSYS $Offline/root/Linux/root6-14-04-icc19
# setenv ROOTSYS /afs/cern.ch/ams/Offline/root/Linux/root6-14-04-icc19
else if ($UROOT == 6) then
 setenv XRDLIB $AMSOPT/xrootd-4.8.2.el9
 setenv ROOTSYS $Offline/root/Linux/root6-14-04-icc23-slc9
else if ($USLC == 5) then
 setenv XRDLIB $AMSOPT/xrootd-icc64-11
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-$UCC-slc5
else if `echo $UCC | grep -c "gcc"` then
 setenv XRDLIB $AMSOPT/xrootd-gcc64-44
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-gcc64-slc6
#  setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-icc64.17-slc6
else if ($USLC < 9) then
 setenv XRDLIB $AMSOPT/xrootd-icc64-17
# setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-icc64.14-slc6
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-icc64.17-slc6
else
 setenv XRDLIB $AMSOPT/xrootd-4.8.2.el9
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-icc64.23-slc9
endif

# setup output
echo "Offline=$Offline"
echo "AMSWD=$AMSWD"
echo "USLC=$USLC UCC=$UCC UICC=$UICC UGCC=$UGCC UROOT=$UROOT ROOTSYS=$ROOTSYS"
cat /etc/redhat-release

# ROOT and XRDLIB Lib
setenv LD_LIBRARY_PATH .:${ROOTSYS}/lib:${XRDLIB}/lib64
set path=($ROOTSYS/bin . $path  /usr/sbin)
if ($UROOT == 6) then
  source $ROOTSYS/bin/thisroot.csh
endif

# Aux Lib
#if ($USLC > 6)  then
#  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/cvmfs/ams.cern.ch/opt/castor.cc7/lib
#endif
if ($USLC == 9)  then
  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/cvmfs/ams.cern.ch/opt/lib64_EL9
endif
#source /afs/cern.ch/project/oracle/script/setoraenv.csh

# Compiler Lib
setenv INTEL_LICENSE_FILE $Offline/intel/licenses
# gcc Lib
if ($UGCC == 493) then
  source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.csh /afs/cern.ch/sw/lcg/external
else if ($UGCC == 530) then
  source /cvmfs/sft.cern.ch/lcg/contrib/gcc/5.3.0/x86_64-centos7-gcc53-opt/setup.csh /afs/cern.ch/sw/lcg/external
else if ($UGCC  >  0 ) then
#  source /cvmfs/sft.cern.ch/lcg/contrib/gcc/8.3.0/x86_64-centos7-gcc8-opt/setup.csh /afs/cern.ch/sw/lcg/external
  source /cvmfs/sft.cern.ch/lcg/contrib/gcc/11.3.0/x86_64-el9-gcc11-opt/setup.csh /afs/cern.ch/sw/lcg/external
#  source /cvmfs/sft.cern.ch/lcg/contrib/gcc/12.1.0/x86_64-el9-gcc12-opt/setup.csh /afs/cern.ch/sw/lcg/external
endif
# icc Lib
if ($UICC == 2011) then
  setenv LD_LIBRARY_PATH ${AMSOPT}/intel/Compiler/11.1/073/idb/lib/intel64/:${AMSOPT}/intel/Compiler/11.1/073/lib/intel64:/opt/intel/compiler80/lib:${LD_LIBRARY_PATH}
  set path=($AMSOPT/intel/Compiler/11.1/073/bin/intel64 $path)
else if ($UICC == 2019) then
  setenv LD_LIBRARY_PATH ${AMSOPT}/intel/composer_xe_2013_sp1.3.174/compiler/lib/intel64:/opt/intel/compiler80/lib:${LD_LIBRARY_PATH}
  set path=($AMSOPT/intel/composer_xe_2013_sp1.3.174/compiler/bin/intel64 $path)
#--------
  setenv INTELSW /cvmfs/projects.cern.ch
  source $INTELSW/intelsw/psxe/linux/all-setup.csh
#--------
#  source $INTELSW/intelsw/psxe/linux/x86_64/2017/compilers_and_libraries/linux/bin/compilervars.csh   intel64
#  setenv INTELDIR $INTELSW/intelsw/psxe/linux/x86_64/2017
#  setenv INTELVER compilers_and_libraries_2017.4.196
#  setenv INTELVER compilers_and_libraries_2017.2.174
  source $INTELSW/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.csh   intel64
  setenv INTELDIR $INTELSW/intelsw/psxe/linux/x86_64/2019
#  setenv INTELVER compilers_and_libraries_2019.4.243
  setenv INTELVER compilers_and_libraries_2019.3.199
#  setenv INTELVER compilers_and_libraries_2019.0.117
else if ($UICC > 0) then
  setenv INTELSW /cvmfs/projects.cern.ch
  setenv INTELDIR $INTELSW/intelsw/oneAPI/linux/x86_64/2023
  setenv INTELVER compiler/latest/linux
  setenv CMPLR_ROOT $INTELDIR/$INTELVER/..
  setenv PKG_CONFIG_PATH $CMPLR_ROOT/lib/pkgconfig
  setenv PATH $CMPLR_ROOT/linux/bin/intel64:$CMPLR_ROOT/linux/bin:${PATH}
  setenv LD_LIBRARY_PATH $CMPLR_ROOT/linux/lib:$CMPLR_ROOT/linux/lib/x64:$CMPLR_ROOT/linux/compiler/lib/intel64_lin:${LD_LIBRARY_PATH}
  setenv CMAKE_PREFIX_PATH $CMPLR_ROOT/linux/IntelDPCPP
  setenv NLSPATH $CMPLR_ROOT/linux/compiler/lib/intel64_lin/locale/%l_%t/%N
endif


# Configuration 
setenv VERBOSE 1
setenv GENFIT 1
setenv DPMJET3 6
setenv G4MULTITHREADED 1
#setenv CASTORSTATIC 1
setenv AMSICC 1
setenv AMSP 1
setenv G4AMS 1
setenv PGTRACK 1
if (`echo $UCC | grep -c "gcc"`) then
#  unsetenv AMSICC
  unsetenv G4MULTITHREADED
  setenv CXX g++
  setenv CC gcc
  setenv FC gfortran
else
  setenv CXX icc
  setenv CC icc
  setenv FC ifort
endif


# Display
setenv AMSGeoDir $Offline/vdev/display/
setenv amsedcPG $Offline/vdev/exe/linuxx8664icc5.34/amsedcPG
setenv offmoncPG $Offline/vdev/exe/linuxx8664icc5.34/offmoncPG


# Enable Aachen TrdQt interface
#setenv AMS_ACQT_INTERFACE 1
#setenv ACROOTSOFTWARE $AMSDataDir/v5.00/TRD
#setenv ACROOTLOOKUPS $ACROOTSOFTWARE/acroot/data/
#setenv QTDIR_STATIC $Offline/AMSsoft/AMSQtCore/qt-4.8.3/
#setenv QTDIR_STATIC_32 $Offline/AMSsoft/AMSQtCore/qt-4.8.3-32bit/


# Geant4
#setenv G4POLARGAMMA 1
#unsetenv G4POLARGAMMA
source $AMSWD/install/g4i10.1
#source $AMSWD/install/g4i10.3 
#source $AMSWD/install/g4i.icc64


# Aux Configureation 
setenv CVSROOT /afs/cern.ch/exp/ams/Offline/CVS
setenv CVS_RSH ssh
if (! $?EOS_MGM_URL ) then
  setenv EOS_MGM_URL "root://eosams.cern.ch"
endif
#setenv QToolWD /afs/cern.ch/work/q/qyan/AMSVDEV/QTool


#limit core 0
limit data unlimited
limit stack unlimited
limit vmemoryuse unlimited
limit memoryuse unlimited
limit descriptors unlimited
