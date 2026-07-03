unset DEBUGFLAG 
export GDBMAN=1
export GDBINFO=1
export VERBOSE=1
export LANGUAGE=US
export LANG=C
export LC_ALL=en_US

export ROOTSYS=$Offline/root/Linux/root6-04-08-icc16
source $ROOTSYS/bin/thisroot.sh
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh  /afs/cern.ch/sw/lcg/external
export INTELSW=/cvmfs/projects.cern.ch
export INTELDIR=$INTELSW/intelsw/psxe/linux/x86_64/2019
export INTELVER=compilers_and_libraries_2019.0.117/linux
source /cvmfs/projects.cern.ch/intelsw/psxe/linux/all-setup.sh
source /cvmfs/projects.cern.ch/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.sh   intel64
export GENFIT=1
