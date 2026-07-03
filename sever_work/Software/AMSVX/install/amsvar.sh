export INTEL_LICENSE_FILE=$Offline/intel/licenses
export ROOTSYS=$Offline/root/$OS/527.icc64
export PATH=$ROOTSYS/bin:/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/bin/intel64:$PATH:.:/usr/sbin
export LD_LIBRARY_PATH=.:/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/idb/lib/intel64/:/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/lib/intel64/:$ROOTSYS/lib:/opt/intel/compiler80/lib
# Get Redhat version
if [[ -f /etc/redhat-release ]]; then
    RH_Version=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`
    if [[ "$RH_Version" == "6" ]]; then
        export LD_LIBRARY_PATH=.:/cvmfs/ams.cern.ch/opt/xrootd-icc64-12/lib64:/cvmfs/ams.cern.ch/opt/intel/composer_xe_2013_sp1.1.106/compiler/lib/intel64/:$ROOTSYS/lib:/opt/intel/compiler80/lib:$ROOTSYS/lib
        export PATH=$ROOTSYS/bin:/cvmfs/ams.cern.ch/opt/intel/composer_xe_2013_sp1.1.106/compiler/bin/intel64:$PATH:./usr/sbin
    fi
fi

export CVSROOT=$Offline/CVS
#export CVSROOT :ext:`whoami`@lxplus.cern.ch:/afs/cern.ch/ams/Offline/CVS
export CVS_RSH=ssh
export ProductionRunsDir=/s0dat0/Data/Deframing
export AMSDataDir=$Offline/AMSDataDirRW
export AMSDataDirRW=$Offline/AMSDataDirRW
export EDITOR=emacs
export CASTORSTATIC=1
export CVSEDITOR=emacs
export RunsDir=$Offline/RunsDir
export AMSDataNtuplesDir=$Offline/ntuples
export AMSMCNtuplesDir=$Offline/amsmcntuplesdir
export ProductionLogDirLocal=$Offline/logs.local
export RunsSummaryFile=$Offline/runs_STS91.log
export SlowRateDataDir=$Offline/SlowRateDataDir
export AMSGeoDir=$Offline/vdev/display/
export amsed=$Offline/vdev/exe/linuxicc/amsed
export amsedc=$Offline/vdev/exe/linuxicc/amsedc
export amsedPG=$Offline/vdev/exe/linux/amsedPG
export amsedcPG=$Offline/vdev/exe/linux/amsedcPG

export offmon=$Offline/vdev/exe/linuxicc/offmon      
export offmonc=$Offline/vdev/exe/linuxicc/offmonc      

export AMSDynAlignment=$AMSDataDirRW/ExtAlig/AlignmentFiles/

export AMSICC=1
export AMSP=1

# Enable Aachen TrdQt interface
export AMS_ACQT_INTERFACE=1
export ACROOTSOFTWARE=$AMSDataDir/v5.00/TRD
export ACROOTLOOKUPS=$ACROOTSOFTWARE/acroot/data/
export QTDIR_STATIC=$Offline/AMSsoft/AMSQtCore/qt-4.8.3/
export QTDIR_STATIC_32=$Offline/AMSsoft/AMSQtCore/qt-4.8.3-32bit/
