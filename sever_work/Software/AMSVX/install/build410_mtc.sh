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

source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.csh  /afs/cern.ch/sw/lcg/external
unsetenv DEBUGFLAG 
setenv GENFIT 1
setenv GDBMAN 1
setenv GDBINFO 1
setenv VERBOSE 1
setenv LANGUAGE US
setenv LANG C
setenv LC_ALL en_US
source $Offline/vdev/install/amsvar
source $ROOTSYS/../root-v5-34-9-icc64.17-slc6/amsvar
source $INTELSW/intelsw/psxe/linux/all-setup.csh
source $INTELSW/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.csh   intel64
setenv INTELDIR $INTELSW/intelsw/psxe/linux/x86_64/2019
source $INTELDIR/vtune_amplifier_2019.0.2.570779/amplxe-vars.csh
setenv INTELVER compilers_and_libraries_2019.0.117
setenv G4MULTITHREADED 1
source $Offline/vdev/install/g4i10.1
setenv DPMJET3 6
setenv PGTRACK 1
setenv G4AMS 1
#setenv INTEL_LICENSE_FILE 28518@licenintelcomp
gmake -j 40

gmake producer
#gmake producer6


