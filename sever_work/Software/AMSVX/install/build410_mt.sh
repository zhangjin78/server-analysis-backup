#!/bin/tcsh
if ($?AMSWD  )  then

else
setenv AMSWD ..
if (-e $AMSWD) then
    else
    mkdir -p $AMSWD
    endif
endif    
echo "$AMSWD"
unsetenv DEBUGFLAG 
setenv GDBMAN 1
setenv GDBINFO 1
setenv VERBOSE 1
setenv LANGUAGE US
setenv LANG C
setenv LC_ALL en_US
source $Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-icc64.14-slc6/amsvar
setenv G4MULTITHREADED 1
source $Offline/vdev/install/g4i10.1
source /afs/cern.ch/sw/lcg/external/gcc/4.9.3/x86_64-slc6/setup.csh  /afs/cern.ch/sw/lcg/external
source /afs/cern.ch/sw/IntelSoftware/linux/all-setup.csh
source /afs/cern.ch/sw/IntelSoftware/linux/x86_64/xe2016/compilers_and_libraries/linux/bin/compilervars.csh   intel64           
setenv INTELDIR /afs/cern.ch/sw/IntelSoftware/linux/x86_64/xe2016
setenv INTELVER compilers_and_libraries_2016.3.210
setenv PGTRACK 1
setenv G4AMS 1
gmake -j 40

gmake producer
gmake producer6


