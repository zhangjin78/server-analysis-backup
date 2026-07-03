#AMS setup csh: Qi Yan
setenv UCC icc64
setenv UROOT ROOT5
setenv AMSWD /afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ
setenv AMSSRC $AMSWD

#---SLC5 or SLC6
setenv USLC slc6
if  `cat /etc/redhat-release | grep -c "SLC release 5"` then
setenv USLC slc5
endif
cat /etc/redhat-release
echo "USLC=$USLC UCC=$UCC"

#setenv Offline /afs/cern.ch/ams/Offline
#setenv AMSOPT /afs/cern.ch/ams/local2/opt
setenv Offline /cvmfs/ams.cern.ch/Offline
setenv AMSOPT /cvmfs/ams.cern.ch/opt

if "$UROOT"x == "ROOT6"x then
 setenv XRDLIB $AMSOPT/xrootd-icc64-12
 setenv ROOTSYS $Offline/root/Linux/root6-04-08-icc16 
else if $USLC == "slc5" then
 setenv XRDLIB $AMSOPT/xrootd-icc64-11
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-$UCC-$USLC
else if `echo $UCC | grep -c "icc"` then
 setenv XRDLIB $AMSOPT/xrootd-icc64-12
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-icc64.14-slc6
# setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-icc64.17-slc6
else
 setenv XRDLIB $AMSOPT/xrootd-gcc64-44
 setenv ROOTSYS $Offline/root/Linux/root-v5-34-9-gcc64-slc6
endif
endif
echo "$Offline"
echo "$ROOTSYS"
#-----
#--ICC Lib
setenv INTEL_LICENSE_FILE $Offline/intel/licenses

if $USLC == "slc5"  then
  setenv LD_LIBRARY_PATH .:$XRDLIB/lib64:$AMSOPT/intel/Compiler/11.1/073/idb/lib/intel64/:$AMSOPT/intel/Compiler/11.1/073/lib/intel64:/opt/intel/compiler80/lib:$ROOTSYS/lib
  set path=($ROOTSYS/bin $AMSOPT/intel/Compiler/11.1/073/bin/intel64 $path . /usr/sbin  )
else
  setenv LD_LIBRARY_PATH .:$XRDLIB/lib64:$AMSOPT/intel/composer_xe_2013_sp1.3.174/compiler/lib/intel64:/opt/intel/compiler80/lib:$ROOTSYS/lib
  set path=($ROOTSYS/bin $AMSOPT/intel/composer_xe_2013_sp1.3.174/compiler/bin/intel64 $path . /usr/sbin  )
endif

#----------
if "$UROOT"x == "ROOT6"x then
  source /afs/cern.ch/ams/Offline/vdev/install/amsvar.root6
endif
#setenv GEANT4NEW 1
#setenv G4INSTALL $Offline/geant4.9.6.p03
#setenv G4INSTALL $Offline/geant4.9.6.p02
#setenv BASE_ARCH_DIR $Offline/AMSsoft/linux_${USLC}_${UCC}
#setenv CLHEP_BASE_DIR $BASE_ARCH_DIR/CLHEP/
#setenv CERN $BASE_ARCH_DIR
#setenv CERN_LEVEL 2005/
#setenv CERN_ROOT $CERN/$CERN_LEVEL
#setenv CERNDIR $CERN/$CERN_LEVEL


#---G4SYSTEM
if $USLC == "slc5"  then
setenv UUSLC ""
else 
setenv UUSLC .$USLC
endif

if `echo $UCC | grep -c "icc"` then
setenv G4SYSTEM Linux-icc$UUSLC
#setenv G4SYSTEM Linux-icc.slc6
else
setenv G4SYSTEM Linux-g++$UUSLC
endif

#------
#setenv G4USE_STL 1
#setenv OGLHOME /usr
#setenv G4WORKDIR /tmp
#setenv G4LIB $G4INSTALL/lib
#setenv G4_NO_VERBOSE 1

#setenv G4DPMJET 1
#setenv G4DATA /afs/ams.cern.ch/user/qyan/Offline
#setenv G4LEVELGAMMADATA  $G4INSTALL/data/PhotonEvaporation2.3
#setenv G4RADIOACTIVEDATA  $G4INSTALL/data/RadioactiveDecay3.6
#setenv G4LEDATA $G4INSTALL/data/G4EMLOW6.32
#setenv NeutronHPCrossSections $G4INSTALL/data/G4NDL4.2
#setenv G4NEUTRONXSDATA $G4INSTALL/data/G4NEUTRONXS1.2
#setenv G4PIIDATA $G4INSTALL/data/G4PII1.3
#setenv G4ELASTIC $G4INSTALL/data/G4ELASTIC1.1
#setenv G4DPMJET2_5DATA $G4INSTALL/data/DPMJET/GlauberData
#setenv G4SAIDXSDATA $G4INSTALL/data/G4SAIDDATA1.1
#setenv G4POLARGAMMA 1
source $Offline/vdev/install/g4i10.1
#unsetenv G4POLARGAMMA
#source $Offline/vdev/install/g4i10.3 
#source $Offline/vdev/install/g4i.icc64
setenv AMSDataDir $Offline/AMSDataDir
setenv AMSGeoDir $Offline/vdev/display/
setenv amsedcPG $Offline/vdev/exe/linuxx8664icc5.34/amsedcPG
#
# Data for elastic scattering processes,
setenv GENFIT 1
setenv DPMJET3 6
setenv G4MULTITHREADED 1
setenv CASTORSTATIC 1
setenv AMSICC 1
setenv AMSP 1
setenv G4AMS 1
setenv PGTRACK 1
setenv CVS_RSH ssh
#limit core 0
limit data unlimited
limit stack unlimited
# Enable Aachen TrdQt interface
#setenv AMS_ACQT_INTERFACE 1
#setenv ACROOTSOFTWARE $AMSDataDir/v5.00/TRD
#setenv ACROOTLOOKUPS $ACROOTSOFTWARE/acroot/data/
#setenv QTDIR_STATIC $Offline/AMSsoft/AMSQtCore/qt-4.8.3/
#setenv QTDIR_STATIC_32 $Offline/AMSsoft/AMSQtCore/qt-4.8.3-32bit/
if ! `echo $UCC | grep -c "gcc"` then
#--------
setenv INTELSW /cvmfs/projects.cern.ch
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.csh  /afs/cern.ch/sw/lcg/external
source $INTELSW/intelsw/psxe/linux/all-setup.csh
#--------
source $INTELSW/intelsw/psxe/linux/x86_64/2017/compilers_and_libraries/linux/bin/compilervars.csh   intel64
setenv INTELDIR $INTELSW/intelsw/psxe/linux/x86_64/2017
#setenv INTELVER compilers_and_libraries_2017.4.196
setenv INTELVER compilers_and_libraries_2017.2.174
#--------
#source $INTELSW/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.csh   intel64
#setenv INTELDIR $INTELSW/intelsw/psxe/linux/x86_64/2019
#setenv INTELVER compilers_and_libraries_2019.4.243
#setenv INTELVER compilers_and_libraries_2019.3.199
endif
