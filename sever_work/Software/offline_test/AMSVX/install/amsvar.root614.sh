unset DEBUGFLAG 
export GDBMAN=1
export GDBINFO=1
export VERBOSE=1
export LANGUAGE=US
export LANG=C
export LC_ALL=en_US

# Get the release version of RedHat Linux
if [[ -f /etc/redhat-release ]]; then
    RedHatRelease=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`
fi
# Choose the correct version of gcc
if [[ "$RedHatRelease" == "7" ]]; then
    os=cc7
else
    os=slc6
fi
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-$os/setup.sh  /afs/cern.ch/sw/lcg/external

export ROOTSYS=$Offline/root/Linux/root6-14-04-icc19
source $ROOTSYS/bin/thisroot.sh
export INTELSW=/cvmfs/projects.cern.ch
source $INTELSW/intelsw/psxe/linux/all-setup.sh
source $INTELSW/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.sh   intel64
export INTELDIR=$INTELSW/intelsw/psxe/linux/x86_64/2019
source $INTELDIR/vtune_amplifier_2019.0.2.570779/amplxe-vars.sh
export INTELVER=compilers_and_libraries_2019.0.117
export INTELVER=compilers_and_libraries_2019.3.199
export PATH=/afs/cern.ch/sw/lcg/external/gdb/7.8/x86_64-slc6-gcc48-opt/bin:$PATH
export GENFIT=1
