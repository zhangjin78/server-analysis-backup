#!/bin/tcsh
setenv GDBMAN 1
setenv GDBINFO 1
setenv LANGUAGE US
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
setenv VERBOSE 1
setenv Offline /afs/cern.ch/ams/Offline
source $Offline/vdev/install/amsvar.root6
setenv G4MULTITHREADED 1
source $Offline/vdev/install/g4i10.1
setenv PGTRACK 1
setenv G4AMS 1
gmake -j 30

gmake producer
gmake producer6


