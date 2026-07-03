# $Id: g4i.icc64.sh,v 1.3 2023/11/29 20:36:26 bshan Exp $
export GEANT4NEW=1

if [[ "`$ROOTSYS/bin/root-config --arch`" == "linux" ]]; then
    export G4SYSTEM=Linux-g++
    export CLHEP_BASE_DIR=$Offline/CLHEP.2.0.4.7.gcc32
fi

if [[ -f /etc/redhat-release ]]; then
    RH_Version=`cat /etc/redhat-release | awk -F'[^0-9]+' '{ print $2 }'`
fi

if [[ "`$ROOTSYS/bin/root-config --arch`" == "linuxx8664icc" ]]; then
    export G4LIB_BUILD_GDML=1
    export G4SYSTEM=Linux-icc
    export CLHEP_BASE_DIR=$Offline/CLHEP.2.0.4.7.icc64
    if [[ "$RH_Version" == "6" ]]; then
        export G4SYSTEM=Linux-icc.slc6
    fi
fi

if [[ "`$ROOTSYS/bin/root-config --arch`" == "linuxx8664gcc" ]]; then
    export G4LIB_BUILD_GDML=1
    export G4SYSTEM=Linux-g++.64
    if [[ "$DEBUGFLAG" ]]; then
        export G4SYSTEM=Linux-g++.64.debug
    fi

    export CLHEP_BASE_DIR=$Offline/CLHEP.2.0.4.7.gcc64
    if [[ "$RH_Version" == "6" ]]; then
        export G4SYSTEM=Linux-g++.slc6
    fi
fi

if [[ "`$ROOTSYS/bin/root-config --arch`" == "linuxia64ecc" ]]; then
    export G4SYSTEM=Linux-icc.ia64
    export CLHEP_BASE_DIR=$Offline/CLHEP/2.0.4.5/sles_ia64_icc41
fi

export G4USE_STL=1
export OGLHOME=/usr
export G4WORKDIR=/tmp
export G4INSTALL=$Offline/geant4.9.4.p04
export G4LIB=$G4INSTALL/lib
export G4_NO_VERBOSE=1
export G4VIS_BUILD_DAWNFILE_DRIVER=1
export G4VIS_BUILD_DAWN_DRIVER=1
export G4VIS_USE_DAWN=1 
export G4VIS_USE_DAWNFILE=1
export G4VIS_BUILD_OPENGLX_DRIVER=1 # Uses OpenGL and Motif
export G4VIS_USE_OPENGLX=1
export G4VIS_BUILD_RAYX_DRIVER=1
export G4VIS_USE_RAYX=1
export G4UI_BUILD=1
#export G4VIS_BUILD_OPACS_DRIVER=1
#export G4VIS_USE_OPACS=1
#export G4DAWNFILE_VIEWER=david
export G4VIS_BUILD_ASCIITREE_DRIVER=1
export G4VIS_USE_ASCIITREE=1
export G4LEVELGAMMADATA=$G4INSTALL/data/PhotonEvaporation2.1
export G4RADIOACTIVEDATA=$G4INSTALL/data/RadioactiveDecay3.3
export G4LEDATA=$G4INSTALL/data/G4EMLOW6.19
export NeutronHPCrossSections=$G4INSTALL/data/G4NDL3.14
export G4NEUTRONXSDATA=$G4INSTALL/data/G4NEUTRONXS1.0
export G4PIIDATA=$G4INSTALL/data/G4PII1.2
export G4ELASTIC=$G4INSTALL/data/G4ELASTIC1.1
export G4DPMJET2_5DATA=$G4INSTALL/data/DPMJET/GlauberData
export G4DPMJET3_DATA=$G4INSTALL/data/tabs/
