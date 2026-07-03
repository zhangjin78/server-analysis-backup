unset DEBUGFLAG 
export GDBMAN=1
export GDBINFO=1
export VERBOSE=1
export LANGUAGE=US
export LANG=C
export LC_ALL=en_US

if [ -z "$Offline" -o "${Offline}" == "" ]; then
    export Offline=/cvmfs/ams.cern.ch/Offline
fi

# Get the release version of RedHat Linux
if [[ -f /etc/redhat-release ]]; then
    RedHatRelease=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`
fi
if [[ $RedHatRelease -gt 7 ]]; then
    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/11.3.0/x86_64-el9-gcc11-opt/setup.sh /afs/cern.ch/sw/lcg/external
    export ROOTSYS=$Offline/root/Linux/root6-14-04-icc64.24-el9
    export XRDLIB=/cvmfs/ams.cern.ch/opt/xrootd-4.8.2.el9
    export INTELSW=/cvmfs/projects.cern.ch
    export INTELDIR=$INTELSW/intelsw/oneAPI/linux/x86_64/2024
    export INTELVER=compiler/latest
    export CMPLR_ROOT=$INTELDIR/$INTELVER
    export PKG_CONFIG_PATH=$CMPLR_ROOT/lib/pkgconfig
    export PATH=$CMPLR_ROOT/bin:$PATH
    export LD_LIBRARY_PATH=$XRDLIB/lib64:$CMPLR_ROOT/lib:$CMPLR_ROOT/opt/compiler/lib:${LD_LIBRARY_PATH}
    export CMAKE_PREFIX_PATH=$CMPLR_ROOT/lib/cmake/IntelDPCPP
    export NLSPATH=$CMPLR_ROOT/lib/intel64_lin/locale/%l_%t/%N
else
    # Choose the correct version of gcc
    if [[ "$RedHatRelease" == "7" ]]; then
        os=cc7
    else
        os=slc6
    fi
    source /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9.3/x86_64-$os/setup.sh  /afs/cern.ch/sw/lcg/external
    export ROOTSYS=$Offline/root/Linux/root6-14-04-icc19
    export INTELSW=/cvmfs/projects.cern.ch
    source $INTELSW/intelsw/psxe/linux/all-setup.sh
    source $INTELSW/intelsw/psxe/linux/x86_64/2019/compilers_and_libraries/linux/bin/compilervars.sh   intel64
    export INTELDIR=$INTELSW/intelsw/psxe/linux/x86_64/2019
    source $INTELDIR/vtune_amplifier_2019.0.2.570779/amplxe-vars.sh
    export INTELVER=compilers_and_libraries_2019.0.117
    export INTELVER=compilers_and_libraries_2019.3.199
    export PATH=/afs/cern.ch/sw/lcg/external/gdb/7.8/x86_64-slc6-gcc48-opt/bin:$PATH
fi
export AMSDataDir=/cvmfs/ams.cern.ch/Offline/AMSDataDir
export AMSGeoDir=$Offline/vdev/display/
source $ROOTSYS/bin/thisroot.sh
export GENFIT=1
