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
setenv GENFIT 1
setenv GDBMAN 1
setenv GDBINFO 1
setenv VERBOSE 1
setenv LANGUAGE US
setenv LANG C
setenv LC_ALL en_US
source $Offline/vdev/install/amsvar.root614
setenv G4MULTITHREADED 1
source /Offline/vdev/install/g4i10.3

setenv PGTRACK 1
setenv G4AMS 1
gmake -j 40

gmake producer
#gmake producer6


