#  $Id: g4i10.1.sh,v 1.5 2024/03/10 16:54:22 qyan Exp $
export GEANT4NEW=1
unset CLHEP_BASE_DIR
# Get Redhat version
if [[ -f /etc/redhat-release ]]; then
    RH_Version=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`
fi
#export VGM_INSTALL=$G4INSTALL/vgmams 

if [[ "$($ROOTSYS/bin/root-config --arch)" == linuxx8664icx ]]; then
    export G4LIB_BUILD_GDML=1
    export G4USE_STD11=1
#    export G4SYSTEM=Linux-icc
    export G4SYSTEM=Linux-icx
    if [[ "$G4MULTITHREADED" ]]; then
#        export G4SYSTEM=Linux-icc-mt-mic
        export G4SYSTEM=Linux-icx-mt
    fi
fi

if [[ "$($ROOTSYS/bin/root-config --arch)" == linuxx8664icc ]]; then
    export G4LIB_BUILD_GDML=1
    export G4SYSTEM=Linux-icc
    if [[ $RH_Version -gt 5 ]]; then
        export G4SYSTEM=Linux-icc
        if [[ "$G4MULTITHREADED" ]]; then
            export G4SYSTEM=Linux-icc-mt-mic
            if [[ "$KNLBUILDAMS" ]]; then
                export G4SYSTEM=Linux-icc-mt-mico
            fi
        fi
    fi
fi

if [[ "$($ROOTSYS/bin/root-config --arch)" == "linuxx8664gcc" ]]; then
    export G4LIB_BUILD_GDML=1
    export G4SYSTEM=Linux-g++
    if [[ "$DEBUGFLAG" ]]; then
        export G4SYSTEM=Linux-g++.debug
    fi
    if [[ $RH_Version -gt 5 ]]; then
        export G4SYSTEM=Linux-g++
        if [[ "$DEBUGFLAG" ]]; then
            export G4SYSTEM=Linux-g++.debug
        fi
    fi
fi

if [[ "$($ROOTSYS/bin/root-config --arch)" == "linuxia64ecc" ]]; then
    export G4SYSTEM=Linux-ecc
    if [[ "$G4MULTITHREADED" ]]; then
        export G4SYSTEM=Linux-ecc-mt
    fi
    export ICC_ROOT=/opt/intel/Compiler/11.1/080
    source $ICC_ROOT/bin/iccvars.sh ia64
    export PATH=$ROOTSYS/bin:$ICC_ROOT/bin/ia64:$PATH:.:/usr/sbin
    export LD_LIBRARY_PATH=.:$ICC_ROOT/idb/lib/ia64/:$ICC_ROOT/lib/ia64/:$ROOTSYS/lib/:$ROOTSYS/lib/:/afs/cern.ch/user/b/bshan/work/public/xrootd-3.2.7-ia64-icc:$LD_LIBRARY_PATH
fi

export G4USE_STL=1
export OGLHOME=/usr
export G4WORKDIR=/tmp
export G4INSTALL=$Offline/geant4.10.01.p01-ams
#export G4INSTALL=/afs/cern.ch/ams/Offline/geant4.10.01.p01-ams
if [[ "$G4POLARGAMMA" ]]; then
    echo "using gamma polar MC, geant4.10.01 "
    export G4INSTALL=$Offline/geant4.10.01-ams-polar
fi
export G4LIB=$G4INSTALL/lib
export G4_NO_VERBOSE=1
export G4VIS_BUILD_DAWNFILE_DRIVER=1
export G4VIS_BUILD_DAWN_DRIVER=1
export G4VIS_USE_DAWN=1 
export G4VIS_USE_DAWNFILE=1
export G4VIS_BUILD_OPENGLX_DRIVER=1  # Uses OpenGL and Motif
export G4VIS_BUILD_OPENGLXM_DRIVER=1  # Uses OpenGL and Motif
export G4VIS_USE_OPENGLX=1
#export G4VIS_USE_OPENGLXM=1
#export G4VIS_BUILD_OPENGLQT_DRIVER=1
unset G4VIS_USE_OPENGLXM
export G4VIS_BUILD_RAYTRACERX_DRIVER=1
#export G4VIS_USE_RAYTRACER=1
export G4VIS_USE_HEPREP=1
export G4VIS_BUILD_HEPREPFILE_DRIVER=1
export G4VIS_USE_HEPREPFILE=1
export G4UI_BUILD=1
#export G4VIS_BUILD_OPACS_DRIVER=1
#export G4VIS_USE_OPACS=1
#export G4DAWNFILE_VIEWER=david
export G4VIS_BUILD_ASCIITREE_DRIVER=1
export G4VIS_USE_ASCIITREE=1
export G4LEVELGAMMADATA=$G4INSTALL/data/PhotonEvaporation3.1
export G4RADIOACTIVEDATA=$G4INSTALL/data/RadioactiveDecay4.2
export G4LEDATA=$G4INSTALL/data/G4EMLOW6.41
export NeutronHPCrossSections=$G4INSTALL/data/G4NDL4.5
export G4NEUTRONXSDATA=$G4INSTALL/data/G4NEUTRONXS1.4
export G4PIIDATA=$G4INSTALL/data/G4PII1.3
export G4ELASTIC=$G4INSTALL/data/G4ELASTIC1.1
export G4SAIDXSDATA=$G4INSTALL/data/G4SAIDDATA1.1
export G4DPMJET2_5DATA=$G4INSTALL/data/DPMJET/GlauberData
export G4DPMJET3_DATA=$G4INSTALL/data/DPMJET/dpmjet3_0/tabs
export G4AUTODETECT=1
