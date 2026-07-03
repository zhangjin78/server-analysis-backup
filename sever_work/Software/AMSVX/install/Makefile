#  
#  $Id: Makefile,v 1.455 2024/01/08 18:45:45 qyan Exp $

# definition of cppflags and inc paths within Makefile
# avoid multiple listings of files, using where possible the generation
#      of objects names from the source names
#
# tentative classification of source files
# 
# renaming of some variable to avoid conficts with the gmake 
#          implicit variables (ex CPP is C pre processor)
#
# many comments added

#rl october 2003

# removed direct references to /afs within the MAKEFILE
#   created variables for these references (CERNICC AMSLIB AMSICCLIB)
#     so that it is easier to create a local version of the Mopt by
#     defining these variables once at the TOP of the OS specific Makefile
#    rather than making changes in many places
#    also makes it easier to change if the library paths change
#
# removed most rules for compiling from sources
#      created default rules which will work for ~80% of the source files
#      makes it easier to add a program as the user
#      doesn't  have to add a rule to compile
#
# removed ifdefs for Operating system and included an OS specific file 
#     instead.  This makes it easier to edit files for a specific OS
#    it also automatically provides for new a OS (if necessary)
#
#   added a variable SFF = ../F which is used in the default rules
#             for compiling .F .f files
#
# made some formatting changes to make it easier to read (I think)
#
#---------------------------
# this Makefile calls OS specific Makefiles which set specific
# flags/variables for that OS.


#rl variables that must be defined 
#  CERN_LEVEL 
#  CERNDIR     root directory of the cern tree e.g /cern/pro
#  CERNICC     root directory of cern tree for intel compiler on Linux 
#  CERNSRCDIR  root directory for CERN SOURCES 
#  ROOTSYS     root directory for ROOT (defined externally via amsvar)
#  AMSLIB AMSICCLIB  directory for where ams libraries (amsutil.a)
#  NAGDIR            directory for NAG 

# [ae-2015.11.21] this change affects only 64-bit compilation staff
# Modified CORBA libraries used, see Makefile.linuxx8664icc
# No need to compile gbatch-orbitxxx.exe.lxplus6, 
# gbatch-orbitxxx.exe should work on lxplus6/7 hosts
ifndef IS_CORBA64
  IS_CORBA64 := 1
endif

# List of SLC6 changes
#
ifndef SLC6system
  ifneq ("$(wildcard /etc/redhat-release)","")
    RH_Version := $(shell cat /etc/redhat-release|awk -F'[^0-9]+' '{ print $$2 }')
  else ifneq ("$(wildcard /etc/os-release)","")
    RH_Version := $(shell grep ^VERSION= /etc/os-release|awk -F'[^0-9]+' '{ print $$2 }')
  endif
  ifeq ($(RH_Version), $(filter $(RH_Version), 6 7 8 9))
    SLC6system := $(RH_Version)
  endif
endif

ifndef SLC64system
	SLC64system := $(SLC6system)
endif


SHELL=/bin/sh

ifneq ("$(wildcard $(AMSWD)/.git)","")
  GITSHA_HEAD = $(shell git --git-dir=${AMSWD}/.git rev-parse HEAD | tr -d '\n')
  GITCLEAN = $(shell git --git-dir=${AMSWD}/.git --work-tree=${AMSWD} diff-index --name-only HEAD | tr -d '\n')
  ifneq ("$(GITCLEAN)","")
    GITSHA = $(GITSHA_HEAD)-dirty
  else
    GITSHA = $(GITSHA_HEAD)
  endif
else
  GITSHA = CVS
endif

ifdef GLIBCXX_USE_CXX11
  DEFINES += -D_GLIBCXX_USE_CXX11_ABI=1 
else
  DEFINES += -D_GLIBCXX_USE_CXX11_ABI=0 
endif


ROOTINCDIR := $(shell $(ROOTSYS)/bin/root-config --incdir)
ROOTLIBDIR := $(shell $(ROOTSYS)/bin/root-config --libdir)

#-----------------------DEFAULTS------------------------------
### defaults can be redefined in the OS specific Makefile (Makefile.[OS])
## these defaults are for  Linux at CERN 
ifdef G4AMS
LINKLIBZ=1
endif
ifeq ($(wildcard $(ROOTINCDIR)/Compression.h),) 
    LZMAL =
else 
    DEFINES  +=  -D__LZMA__ 
    LZMAL =  -llzma  
    ifdef LINKLIBZ 
ifeq ($(wildcard $(ROOTLIBDIR)/libz.a),)
	LZMAL +=      -lz 
else
	LZMAL += $(ROOTLIBDIR)/libz.a
endif
LZMAL += -lexpat
    endif
endif


ifndef AMSWD
AMSWD := ..
endif
#
ifndef AMSDEPLIB
  AMSDEPLIB := /cvmfs/ams.cern.ch/opt
  ifeq "$(wildcard $(AMSDEPLIB))" ""
    AMSDEPLIB := /afs/cern.ch/ams/local2/opt
    $(info "Redefined: AMSDEPLIB=$(AMSDEPLIB)")
  endif
endif



############## PRE PROCESSOR FLAGS (non ARCH dependent) 
ifdef PGTRACK
  PG:=PG
  DEFINES+= -D_PGTRACK_
endif
ifdef ECALBDT
  DEFINES+= -D_ECALBDT_
endif
ifdef ECALBDTCHI2
  DEFINES+= -D_ECALBDTCHI2_
endif
ifdef CASTORSTATIC 
  DEFINES+= -DCASTORSTATIC
endif
ifdef UID2DST
  DEFINES+= -DUID2DST
endif


################  OUTPUT DIRS  
# C++      sources dir 
SCC = ../CC/
# Fortran  sources dir 
SFF = ../F/
ifdef SLC6system
  AMSLIBSUFFIX=slc6
else
  AMSLIBSUFFIX=slc4
endif


############## ROOT section
# Operating system dependence, ARCH comes from the ROOT arch 
DEFINES  += -D__WRITEROOT__
INCDIRS  += -I../include -I$(ROOTINCDIR)

ifndef  ROOTSYSRUN
  ROOTSYSRUN=$(ROOTSYS)
else
  LD_LIBRARY_PATH=$(ROOTSYSRUN)/lib
endif

ifeq ($(origin MARCH), undefined) 
  MARCH   := $(shell $(ROOTSYS)/bin/root-config --arch)
endif

VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
VERSIONP = $(VERSION)
VERSION6      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-1)

ifeq ($(VERSION6),6)
  VERSION6=6
  SLC64ROOT6system:=1
  DEFINES+= -DVERSION6
else
  VERSION6= 
endif

VERSION614      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ifeq ($(VERSION614),6.14)
  VERSION614=614
  NO_LLVM=1
  DEFINES+= -DVERSION614
else
  VERSION614=
endif

# Uncomment here if you have lib/libMinuit2.so and its objs in lib/libRoot.a
ifdef MINUIT2
  DEFINES+= -D_MINUIT2_
endif

ifdef KNLBUILDAMS
  MARCH     :=linuxx8664iccknl
endif	

LCLANG= -static
LROOT= -lRoot
LGEOM= -lGeom
ifdef GENFIT
  LROOT= -lRootgf
  LGEOM= -lGeomgf
endif

ROOTCINT=rootcint
ifdef VERSION6
  LCLANG=  -static
  ifndef NO_LLVM
    LCLANG += -llvm
    LCLANGD += -llvm
  endif
  ifdef VERSION614
     LCLANG+= -ltbb -llz4 -lxxhash -lclang -lcling -lllvm  -lblas
  endif
  LCLANG += -lz -lgsl
  LCLANGD += -lz  -lgsl  -rdynamic
  ROOTCINT=rootcling
endif


############## ROOTDICT
ifdef VERSION6
  ROOTDICT_CPPFLAGS:=$(DEFINES)
else
  RD:=$(DEFINES)
  ROOTDICT_CPPFLAGS:=$(filter-out -std=c++11,$(RD))
endif


############## GenFit section (Qi Yan)
GENFITDIR=../GenFit
GENFITSCC=$(GENFITDIR)/src/
ifdef GENFIT
  DEFINES +=-DUSEGENFIT
  INCDIRS +=-I$(GENFITDIR)/include
endif


############## DPMJET section (Qi Yan)
ifdef G4AMS
  DPMDIR=../DPMJET
  DPMSCC=$(DPMDIR)/src/
  DPMFF= $(DPMDIR)/dpmjet2_5/
ifdef G4MULTITHREADED
  DPMFF= $(DPMDIR)/dpmjet2_5_mt/
endif
  DEFINES +=-DG4_USE_DPMJET \
        -DG4DPMJET25GDSHDEBU
  INCDIRS +=-I$(DPMDIR)/include
#----DPM3
  DPM3DIR=../DPMJET/dpmjet3_0
  DPM3CC=$(DPM3DIR)
  ifeq ($(DPMJET3),6)
    DPM3FF=$(DPM3DIR)/3.0-6-mt
#---DUSEDPMUS=3(0 underscore), =2(2 underscore), =1 or other (1 underscore)
    DEFINES +=-DG4_USE_DPMJET3 -DUSEDPMUS=3
#    DEFINES +=-DG4_USE_DPMJET3
    INCDIRS +=-I$(DPM3CC)
  else ifeq ($(DPMJET3),7)
    DPM3FF=$(DPM3DIR)/3.2017-1-mt
    DEFINES +=-DG4_USE_DPMJET3 -DUSEDPMUS=3
    INCDIRS +=-I$(DPM3CC)
  else
    DPM3FF=
  endif
endif



############## G4 section 
ifndef VISLIBS
  VISLIBS:=
endif
ifndef UILIBS
  UILIBS:=
endif
G4LIBDIR=$(G4INSTALL)/lib.p02/geant4/$(G4SYSTEM)
ifdef G4LIB
  G4LIBDIR = $(G4LIB)/$(G4SYSTEM)
endif
G4INCLUDEDIR=$(G4INSTALL)/include/geant4
ifdef G4INCLUDE
  G4INCLUDEDIR = $(G4INCLUDE)
endif
G4LIBS=
G4LIBSI=
G4=
ifdef NOOSTLS
        NOOSTLS=:noostls
else
        NOOSTLS=
endif
ifdef JUQUEEN
        G4:=JQ
NOOSTLS=:noostls
ifdef NOOSTLS
        G4:=JQ
endif
endif
ifdef G4AMS
  G4VIS_USE:= 1
  G4:=G4

#G4 multithreads
  ifdef G4MULTITHREADED
    G4:=G4MT
    DEFINES+= -DG4MULTITHREADED
    ifdef G4POLARGAMMA
      G4:=G4MTPolar
      DEFINES+= -D__G4POLARGAMMA__
    endif
    ifdef JUQUEEN
      G4:=G4MTJQ
    endif
    ifdef JUQUEEND
      G4:=G4MTJQD
    endif
  endif

#We need to restore externally defined CC. Geant doesn't respect it
#Save the environment value
  CC_env := $(CC)
  CXX_env := $(CXX)
  CPPFLAGS_env:=$(CPPFLAGS)
  CXXFLAGS_env:=$(CXXFLAGS)
  LDFLAGS_env:=$(LDFLAGS)
  AR_env:=$(AR)

#this would go to auto asisgn to CPPFLAGS, CXXFLAGS, LDFLAGS, AR, ...
  CPPFLAGS:=
  include $(G4INSTALL)/config/architecture.gmk
  ifdef G4VIS_USE
    include $(G4INSTALL)/config/G4UI_USE.gmk
    include $(G4INSTALL)/config/G4VIS_USE.gmk
  endif
  DEFINESG4:=$(filter -D%, $(CPPFLAGS))
  INCDIRSG4:=$(filter -I%, $(CPPFLAGS))

#We need to restore externally defined CC. Geant doesn't respect it
#Restore environment value
  CC := $(CC_env)
  CXX := $(CXX_env)
  CPPFLAGS:=$(CPPFLAGS_env)
  CXXFLAGS:=$(CXXFLAGS_env)
  LDFLAGS:=$(LDFLAGS_env)
  AR:=$(AR_env)

#adding defines, includes, libs
  DEFINES+= -D__NAMESPACE__ -D__G4AMS__ $(DEFINESG4)
# remove duplicates
  DEFINES:=$(shell echo "$(DEFINES)" | tr ' ' '\n' | awk '!a[$$0]++' | tr '\n' ' ')

  INCDIRS+= -I$(CLHEP_BASE_DIR)/include -I$(G4INCLUDEDIR) $(INCDIRSG4)
# remove duplicates
  INCDIRS:=$(shell echo "$(INCDIRS)" | tr ' ' '\n' | awk '!a[$$0]++' | tr '\n' ' ')

  G4LIBS= -L$(G4LIBDIR) -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4analysis -lG4global

  ifdef G4LIB_BUILD_CLHEP
    G4LIBS+=  -lG4clhep
  else
    G4LIBS+= -L$(CLHEP_BASE_DIR)/lib  -lCLHEP
   endif

  G4LIBSI= -L$(G4LIBDIR) $(VISLIBS) $(G4LIBS) -lG4interfaces -lGL -lGLU
endif


############## CALL OS SPECIFIC FILE: include a file called Makefile.$(ARCH)
#import DEFINES, CFLAGS, CXX, CXX_ORBIT, LD, LDFLAGS, LDFLAGSDYNAMIC, LDFLAGSSTATIC, ...
AR:=ar
local_makefile = Makefile.$(MARCH)
include $(local_makefile)


############## CERNDIR (ARCH dependence, defined in Makefile.$(MARCH))
INCDIRS+= -I$(CERNDIR)/include


# combine compiler flags with pre-processor flags
CFLAGSI:=$(DEFINES) $(INCDIRS)
CFLAGS+= $(CFLAGSI)
CFLAGSTOFSIM+= $(CFLAGSI)
CFLAGSn+= $(CFLAGSI)
CFLAGSDAQ+= $(CFLAGSI)

#------------------------------------------------------------------------------
ifdef PGTRACK
FFLAGSDPM:=$(filter-out -O2,$(FFLAGS))
BINMT=$(AMSWD)/bin/$(MARCH)$(VERSIONP)_$(RH_Version)/mt_PG$(G4)/
BIN=$(AMSWD)/bin/$(MARCH)$(VERSIONP)_$(RH_Version)/PG$(G4)/
EXE=$(AMSWD)/exe/$(MARCH)$(VERSIONP)_$(RH_Version)/
LIB=$(AMSWD)/lib/$(MARCH)$(VERSIONP)_$(RH_Version)/
SHLD=$(LIB)/ntuple_$(AMSLIBSUFFIX)_PG.so
NTLD=$(LIB)/libntuple_$(AMSLIBSUFFIX)_PG.a
SHLD_LINK=$(LIB)/libntuple_$(AMSLIBSUFFIX)_PG_dynamic.so
else
PG=
BINMT=$(AMSWD)/bin/$(MARCH)$(VERSIONP)/mt/$(G4)/
BIN=$(AMSWD)/bin/$(MARCH)$(VERSIONP)/$(G4)/
EXE=$(AMSWD)/exe/$(MARCH)$(VERSIONP)/
LIB=$(AMSWD)/lib/$(MARCH)$(VERSIONP)/
SHLD=$(LIB)/ntuple_$(AMSLIBSUFFIX).so
NTLD=$(LIB)/libntuple_$(AMSLIBSUFFIX).a
SHLD_LINK=$(LIB)/libntuple_$(AMSLIBSUFFIX)_dynamic.so
endif

GENFITBIN=$(BIN)GenFit/
DPMBIN=$(BIN)DPMJET/
DPM3CCBIN=$(BIN)DPMJET3/
DPM3FFBIN=$(BIN)DPMJET3/

IDLFLAGS=-D__CORBA__ -I$(BIN)


# verbose output
ifdef  VERBOSE
VERBOSE=1
else
VERBOSE=0
endif
AT_0:=@
AT_1:=
AT=$(AT_$(VERBOSE))


#------------------------------------------------------------------------------

# Setup rules to compile source from $SFF or $SCC and put objects in $BIN
# default rules for object files 
# object files that have special flags are still defined below 
#
#    rules to compile o from F,f,C,c using the AMS directories
#                  $<  =  the prerequsite (the source code)
#                  $@  =  the target (the object file)

$(BIN)%.o : $(SFF)%.F
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(AT)$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@


$(BIN)%_s.o : $(SFF)%.F
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(AT)$(FF) -c $(FFLAGS) -D__ROOTSHAREDLIBRARY__  -I$(CERNDIR)/include $< -o $@


$(BIN)%.o : $(SFF)%.f
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(AT)$(FF) -c $(FFLAGS) -I$(CERNDIR)/include $< -o $@


$(BIN)%.o : $(SCC)%.C
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $< -o $@


$(BIN)%_s.o: $(SCC)%.C ../include/root_methods$(PG).h
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__ROOTSHAREDLIBRARY__   $<  -o $@


$(BIN)%.o : $(SCC)%.c
	@echo Compiling  $< ...
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(AT)$(CC) -c $(CFLAGS) $< -o $@

#------------------------------------------------------------------------------
# gbatch source files


INCS= $(wildcard ../include/*.h)    


# Base AMS Classes
AMS_BASE=     $(SCC)amsdbc.C  $(SCC)astring.C     $(SCC)id.C  $(SCC)point.C  $(SCC)edep.C 

AMS_BASEO=$(AMS_BASE:$(SCC)%.C=$(BIN)%.o)
COMMON_FILES+= $(AMS_BASE) $(BIN)predict.o

# Base GBATCH Classes
GBATCH_BASE = $(SCC)amsgobj.C $(SCC)link.C $(SCC)commonsi.C   $(SCC)node.C   $(SCC)snode.C \
	      $(SCC)cont.C  $(SCC)upool.C $(SCC)uzstat.C $(SCC)apool.C $(SCC)ams2tgeo.C  

COMMON_FILES+=$(GBATCH_BASE)

# MPIEmulator
COMMON_FILES += $(SCC)system_bg.C  $(SCC)MPIEmulator.C


ifdef G4AMS
#Geant 4 interface

COMMON_FILES +=   $(SCC)TRD_SimUtil.C  $(SCC)g4physics_ion.C \
	$(SCC)TRD_GammaXTRadiator.C $(SCC)TRD_VXTenergyLoss.C $(SCC)geant4.C \
	$(SCC)g4physics.C $(SCC)g4util.C $(SCC)g4xray.C  $(SCC)AMSPhysicsList_HadronIon.C $(SCC)g4strangelet.C $(SCC)G4ExcitationHandler.C $(SCC)g4matscan.C
# $(SCC)G4PhysicsVector.C $(SCC)G4OpBoundaryProcess.C
#$(SCC)G4HadronicProcess.C
#$(SCC)G4PhysicsVector.C 
#$(SCC)G4MagIntegratorDriver.C $(SCC)G4MTRunManager.C $(SCC)G4RunManager.C $(SCC)G4EventManager.C $(SCC)G4WorkerRunManager.C

COMMON_FILES += $(SCC)g4rich.C $(SCC)g4tof.C $(SCC)TofSimUtil.C
COMMON_FILES += $(SCC)TrSimUtil.C
#COMMON_FILES += $(SCC)G4CrossSectionDataSetRegistry.C #$(SCC)G4PiNuclearCrossSection.C $(SCC)G4INCLXXPionBuilder.C
COMMON_FILES += $(SCC)G4HadronPhysicsQGSP_BIC_AMS.C

else
#Geant3/AMS custom  files
COMMON_FILES += $(SFF)sbcomp.F  $(SFF)gskpho.F  $(SFF)flinit.F  $(SFF)gbirk.F $(SFF)hadevv.F $(SFF)grfile.F  $(SFF)gstord.F
endif

#Geant3 Interface
COMMON_FILES +=	$(SCC)gvolume.C $(SCC)amsgeom.C   $(SFF)Geant3_utils.F  $(SFF)trd.F \
		$(SCC)gmat.C    $(SCC)mceventg.C  $(SCC)mccluster.C     \
		$(SFF)egzav.F    $(SFF)rzerowrapper.F 


#Utils
COMMON_FILES +=  $(SCC)commons.C  $(SCC)cern.C    $(SFF)sgif.F   $(SCC)rnormx.C  $(SFF)geocoor.F     \
		 $(SCC)status.C   $(SCC)user.C    $(SCC)timelx.c $(SCC)crclib.C

ifndef NO_NAG
  COMMON_FILES += $(SFF)e04ccf.f
endif

ifndef PGTRACK
COMMON_FILES += $(SFF)mtx2.F     $(SFF)fint_my.F 
endif

#TrkLH
ifdef PGTRACK
COMMON_FILES += $(SCC)TrkLH.C
endif

#TrkCC
ifdef PGTRACK
COMMON_FILES += $(SCC)TrkCC.C
endif

#Tracker
ifdef PGTRACK
TRACK_BASE= $(SCC)TkCoo.C 	\
        $(SCC)tkdcards.C     $(SCC)TkLadder.C $(SCC)TkObject.C \
	$(SCC)TkPlane.C      $(SCC)TkSens.C $(SCC)TkFeet.C \
	$(SCC)TrCluster.C \
	$(SCC)TrLadCal.C      \
	$(SCC)patt.C \
        $(SCC)TrLadPar.C     $(SCC)TrParDB.C \
	$(SCC)TrTasCluster.C $(SCC)TrTasPar.C \
	$(SCC)TrAlignFit.C \
	  $(SCC)TrTrackSelection.C \
	$(SCC)tktof.C \
        $(SCC)TrPdf.C        $(SCC)TrPdfDB.C  $(SCC)TrCharge.C 

TRACK_ADD= $(SCC)tkgeom.C   $(SCC)trdaq.C   $(SCC)TkDBc.C $(SCC)TrTrack.C  \
	   $(SCC)trrec.C    $(SCC)MagField.C $(SCC)TrFit.C $(SCC)TrRecon.C $(SCC)TrReconQ.C $(SCC)TrSim.C $(SCC)TrMCCluster.C $(SCC)TrSimSensor.C $(SCC)TrSimCluster.C $(SCC)GausBF.C \
	   $(SCC)bcorr.C    $(SFF)fint_my.F $(SCC)TrCalDB.C $(SCC)TrTasDB.C $(SCC)TrGainDB.C $(SCC)TrChargeLossDB.C $(SCC)TrOccDB.C $(SCC)tkpatt.C $(SCC)MonoSpline.C \
	   $(SCC)TrEDepDB.C $(SCC)TrMipDB.C $(SCC)TrLinearDB.C $(SCC)TrLikeDB.C  $(SCC)HistoMan.C $(SCC)HistoProofMan.C \
           $(SCC)gufld.C $(SFF)rkms.F $(SCC)TrInnerDzDB.C $(SCC)TrExtAlignDB.C \
	   $(SCC)DynAlignment.C $(SCC)TrRecHit.C $(SCC)TrHistoMan.C $(SCC)Vertex.C $(SCC)TrMass.C $(SCC)Betalhd.C\
           $(SCC)MSplineH.C $(SCC)Fit2DH.C $(SCC)TrAsymmDBH.C $(SCC)TrBetaDBH.C $(SCC)TrChargeDBH.C $(SCC)TrChargeLossDBH.C $(SCC)TrClusterChargeH.C $(SCC)TrGainCHDBH.C $(SCC)TrGainDBH.C $(SCC)TrRecHitChargeH.C $(SCC)TrRigidityDBH.C $(SCC)TrTempDBH.C $(SCC)TrThetaDBH.C $(SCC)TrTrackChargeH.C $(SCC)TrChargeYJ.C

TRACK_ADD+=$(SCC)OrbGen.C $(SCC)pfunction.C $(SCC)cgmmap.C
else

#Tracker
TRACK_BASE +=   $(SCC)trid.C       $(SCC)trmccluster.C $(SCC)trrec.C  $(SCC)trcalib.C $(SCC)tralig.C  \
	 	$(SFF)tkfit.F      $(SCC)tkdbc.C   $(SFF)trafit.F  $(SFF)tcircf.F  \
		$(SFF)tszfit.F     $(SCC)trfit.C $(SFF)txyfit.F  $(SCC)trrawcluster.C \
		$(SFF)tptcir.F     $(SCC)gsen.C    $(SCC)TrPdf.C \
		$(SFF)rkms.F       $(SFF)tkmccl.F $(SCC)tkgeom.C $(SCC)vtx.C
TRACK_ADD=      $(SFF)mfield.F     $(SCC)bcorr.C  $(SCC)DynAlignment.C $(SCC)TrMass.C $(SCC)Betalhd.C
endif
COMMON_FILES +=$(TRACK_ADD) 
COMMON_FILES +=$(TRACK_BASE)

TRACK_BASEO=$(TRACK_BASE:$(SCC)%.C=$(BIN)%.o)
COMMON_FILES += $(SCC)VCon_gbatch.C $(SCC)TrElem.C $(SCC)VCon.C  
ifdef PGTRACK
COMMON_FILES +=  $(SCC)TrRawCluster.C
endif
#TRD 
COMMON_FILES += $(SCC)TrdMcDatacards.C $(SCC)trdid.C $(SCC)trdsim.C $(SCC)trdrec.C $(SCC)TrdHRecon.C $(SCC)TrdHCalib.C $(SCC)TrdHCharge.C $(SCC)TrdTFit.C $(SCC)TrdHSegment.C $(SCC)TrdHTrack.C $(SCC)trdhrec.C  $(SCC)trddbc.C $(SCC)trdcalib.C  

#TRD + AC Calibration + TrdKLikelihood Methods + TrdMTrack
ifdef PGTRACK
TRD_ADD= $(SCC)TrdSCalib.C $(SCC)TrdKCalib.C $(SCC)TrdKHit.C $(SCC)TrdMTrack.C $(SCC)TrdKPDF.C $(SCC)TrdKCluster.C $(SCC)TrdKToyMC.C $(SCC)GammaFit.C
TOF_ADD= $(SCC)TofGeometry.C $(SCC)TofRecon.C $(SCC)TofTrack.C
endif
COMMON_FILES +=$(TRD_ADD) 
COMMON_FILES +=$(TOF_ADD)

#TOF + ACC
COMMON_FILES += $(SCC)tofrec02.C   $(SCC)tofuser02.C \
		$(SCC)tofsim02.C   $(SCC)tofdbc02.C   $(SCC)tofcalib02.C \
		$(SCC)tofid.C      $(SCC)antidbc02.C  $(SCC)antirec02.C \
		$(SCC)anticalib02.C $(SCC)Tofsim02.C $(SCC)Tofdbc.C \
		$(SCC)Tofrec02_ihep.C $(SCC)tofhit.C $(SCC)Tofcharge_ihep.C \

#Tracker
COMMON_FILES += $(SCC)AMSPlane.C $(SCC)TkDBcN.C $(SCC)TkGeomN.C

#ECAL
COMMON_FILES += $(SCC)ecaldbc.C    $(SCC)ecid.C       $(SCC)ecalrec.C \
		$(SCC)ecalcalib.C  $(SCC)ecalBDT.C $(SCC)ecalBDTCHI2.C $(SCC)EcalChi2CY.C $(SCC)EcalH.C $(SCC)EcalKDbc.C $(SCC)EcalFastRec.C $(SCC)Ecal3DRec.C $(SCC)EcalKRec.C $(SCC)EcalShowerProfileFit.C $(SCC)EcalShowerDirectionFit.C $(SCC)DeadSideCorr.C

COMMON_FILES += $(SCC)EcalKX3DRec.C $(SCC)EcalKXCalib.C $(SCC)EcalKXFastRec.C $(SCC)EcalKXRec.C # new ECAL 3D reconstruction

#RICH
COMMON_FILES += $(SCC)richdbc.C           $(SCC)richrec.C        $(SCC)richid.C      $(SCC)richgeom.C  \
		$(SCC)richlip.C           $(SFF)richpatt_lip.F      $(SFF)richrec_lip.F \
		$(SFF)richpmtlib.F        $(SFF)gtckov.F         $(SFF)graylc.F      $(SFF)cerenkov.F \
		$(SCC)richradid.C         $(SFF)ggckov.F         $(SCC)richcal.C     $(SCC)richdaq.C \
		$(SFF)richbetalike_lip.F \
		$(SFF)richbetastand_lip.F $(SFF)richchg_lip.F \
		$(SFF)richutil_lip.F \
		$(SCC)RichCharge.C $(SCC)RichConfig.C $(SCC)RichBeta.C $(SCC)RichTools.C  \
		$(SCC)richRefit.C

# High level reconstruction
COMMON_FILES += $(SCC)beta.C     $(SCC)charge.C   $(SCC)particle.C   \
		$(SCC)astro.C    $(SCC)io.C       

#Trigger
COMMON_FILES += $(SCC)trigger102.C $(SCC)trigger302.C  


#DAQ Interface
COMMON_FILES +=  $(SCC)daqs2block.C $(SCC)daqecblock.C 

#SlowControlDB
COMMON_FILES +=  $(SCC)SlowControlDB.C

ifdef WEBACCESS
#AMSDataDir Web access interface
COMMON_FILES +=  $(SCC)fopen.c 
endif

#HBOOK interface
#COMMON_FILES += $(SCC)ntuple.C

#SunPosition
 COMMON_FILES +=  $(SCC)Sunposition.C

#Coordinate Transforamtion
 COMMON_FILES +=  $(SCC)FrameTrans.C

#Geomagnetic Trace back
 COMMON_FILES +=  $(SCC)GeoMagField.C $(SCC)GeoMagCoord.C $(SCC)GeoMagTrace.C

#GeoMagneticCutoff 
 COMMON_FILES += $(SCC)GM_SubLibrary.C

#LxMCcutoff
 COMMON_FILES += $(SCC)LxMCcutoff.C

#BackTracing
COMMON_FILES += $(SCC)BackTracingMIB.C
COMMON_FILES += $(SFF)BackTracing.F

# Unfolding
#COMMON_FILES += $(SCC)UnfoldingTools.C
COMMON_FILES += $(SCC)SpFold.C

# AntiPG
COMMON_FILES += $(SCC)AntiPG.C

#MVA
COMMON_FILES += $(SCC)MVAmanager.C $(SCC)ConfigParser.C \
		$(SCC)OutHistoManager.C  $(SCC)OutTreeManager.C

#COMMON_FILES += $(SCC)ConfigParser.C


#Root Interface
COMMON_FILES += $(SCC)root.C $(SCC)root_setup.C   

ifdef PGTRACK
ROOTDICTFILES += ../include/CintUInt8.h ../include/TkPlane.h ../include/TkSens.h \
		../include/TrCalDB.h ../include/TrGainDB.h ../include/TrChargeLossDB.h ../include/TrOccDB.h ../include/MonoSpline.h \
		../include/TrEDepDB.h ../include/TrMipDB.h ../include/TrLinearDB.h ../include/TrLikeDB.h \
		../include/tkpatt.h         ../include/TkCoo.h  ../include/TkSens.h \
		../include/TrRecon.h 	  ../include/TkDBc.h  ../include/MagField.h \
		../include/TrLadCal.h  ../include/TrReconQ.h \
		../include/TkLadder.h  ../include/TkFeet.h   ../include/TkObject.h ../include/TrLadPar.h  ../include/TrAlignFit.h \
		../include/TrCluster.h    ../include/TrRecHit.h ../include/TrTrack.h \
		../include/TrTasCluster.h ../include/TrTasPar.h ../include/TrTasDB.h \
		../include/TrFit.h ../include/GausBF.h \
		../include/TrMCCluster.h \
                ../include/trrec.h ../include/tkdcards.h ../include/TrdMcDatacards.h \
		../include/point.h ../include/edep.h ../include/root_RV.h ../include/root.h \
		../include/TrSim.h ../include/TrSimSensor.h ../include/TrSimCluster.h \
		../include/TrPdf.h ../include/TrPdfDB.h ../include/TrCharge.h ../include/TrChargeYJ.h ../include/TrMass.h ../include/Betalhd.h \
		../include/TrHistoMan.h ../include/HistoMan.h  ../include/HistoProofMan.h \
		../include/TrTrackSelection.h \
		../include/root_setup.h ../include/TrExtAlignDB.h ../include/TrInnerDzDB.h  ../include/pfunction.h \
                ../include/MSplineH.h ../include/Fit2DH.h ../include/TrAsymmDBH.h ../include/TrBetaDBH.h ../include/TrChargeDBH.h ../include/TrChargeLossDBH.h ../include/TrClusterChargeH.h ../include/TrGainCHDBH.h ../include/TrGainDBH.h ../include/TrRecHitChargeH.h ../include/TrRigidityDBH.h ../include/TrTempDBH.h ../include/TrThetaDBH.h ../include/TrTrackChargeH.h
else
ROOTDICTFILES +=  ../include/point.h ../include/edep.h ../include/root_RV.h ../include/root.h  ../include/amschain.h ../include/root_setup.h ../include/TrPdf.h ../include/trfit.h 
endif

ROOTDICTFILES += ../include/VCon.h ../include/TrElem.h ../include/trigger102_setup.h  

ROOTDICTFILES += ../include/TrdRawHit.h ../include/TrdHSegment.h ../include/TrdHTrack.h ../include/TrdHRecon.h ../include/TrdHCalib.h ../include/TrdHCharge.h ../include/TrdTFit.h ../include/SlowControlDB.h

ROOTDICTFILES += ../include/DynAlignment.h ../include/RichTools.h ../include/RichCharge.h ../include/RichConfig.h ../include/Tofrec02_ihep.h  ../include/Tofcharge_ihep.h ../include/EcalChi2CY.h ../include/EcalH.h ../include/EcalKDbc.h ../include/EcalFastRec.h ../include/Ecal3DRec.h ../include/EcalKRec.h ../include/GeoMagField.h ../include/GeoMagCoord.h ../include/GeoMagTrace.h ../include/GM_SubLibrary.h ../include/LxMCcutoff.h 

# new ECAL 3D reconstruction
#ROOTDICTFILES += ../include/EcalKX3DRecDB.h ../include/EcalKX3DRec.h ../include/EcalKXCalib.h ../include/EcalKXFastRec.h ../include/EcalKXRec.h

ROOTDICTFILES += ../include/BackTracingMIB.h

ifdef PGTRACK
ROOTDICTFILES +=  ../include/TrdKCalib.h ../include/TrdKHit.h ../include/TrdMTrack.h ../include/TrdKPDF.h ../include/TrdKCluster.h ../include/TrdKToyMC.h ../include/GammaFit.h
ROOTDICTFILES += ../include/TofGeometry.h  ../include/TofRecon.h  ../include/TofTrack.h ../include/bcorr.h
endif

ROOTDICTFILES += ../include/UnfoldingTools.h ../include/SpFold.h

# AntiPG
ROOTDICTFILES += ../include/AntiPG.h

# MVA
ROOTDICTFILES +=../include/MVAmanager.h ../include/ConfigParser.h  ../include/OutTreeManager.h  ../include/OutHistoManager.h

#TrkLH
ifdef PGTRACK
ROOTDICTFILES += ../include/TrkLH.h
endif

#TrkCC
ifdef PGTRACK
ROOTDICTFILES += ../include/TrkCC.h
endif


#ROOTDICTFILES += ../include/linkdef.h


#------------------------------------------------------------------------------
# File list for standard gbatch compilation
#------------------------------------------------------------------------------

FILES= 	$(COMMON_FILES) \
	$(SCC)gbatch.C  $(SCC)geant.C  	$(SCC)geant3.C      \
	$(SCC)job.C     $(SCC)event.C   $(SCC)daqevt.C $(SFF)rndm.F   \
	$(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)richnodisp_lip.F
ifeq ($(MARCH),linuxppcbgxlc)
    SPECIAL_FILES=$(SCC)Tofsim02.C 
    FILES:=$(filter-out $(SPECIAL_FILES),$(FILES))
    SPECIAL_FILES_PPC:=$(SPECIAL_FILES:.C=_ppc.C)
    FILES+=$(SPECIAL_FILES_PPC)
	WF=-WF,
else
	WF=
	CXXG=$(CXX)
	CXXFLAGSG=$(CXXFLAGS)
    CFLAGSG=$(CFLAGS)	
endif

ifndef PGTRACK
FILES +=$(SFF)TrackFit_utils.F
endif
ifeq ($(MARCH),linuxppcbgxlc)
ROOTDICTO= $(BIN)rootdict1.o $(BIN)rootdict2.o $(BIN)rootdict3.o $(BIN)rootdict4.o $(BIN)rootdict5.o $(BIN)rootdict6.o $(BIN)rootdict7.o $(BIN)rootdict8.o $(BIN)rootdict9.o
ROOTDICTOS= $(BIN)rootdict1_s.o $(BIN)rootdict2_s.o $(BIN)rootdict3_s.o $(BIN)rootdict4_s.o $(BIN)rootdict5_s.o $(BIN)rootdict6_s.o $(BIN)rootdict7_s.o $(BIN)rootdict8_s.o $(BIN)rootdict9_s.o
else
ifeq ($(MARCH),linuxx8664icc1)
ROOTDICTO= $(BIN)rootdict1.o $(BIN)rootdict2.o $(BIN)rootdict3.o $(BIN)rootdict4.o $(BIN)rootdict5.o $(BIN)rootdict6.o $(BIN)rootdict7.o $(BIN)rootdict8.o $(BIN)rootdict9.o
ROOTDICTOS= $(BIN)rootdict1_s.o $(BIN)rootdict2_s.o $(BIN)rootdict3_s.o $(BIN)rootdict4_s.o $(BIN)rootdict5_s.o $(BIN)rootdict6_s.o $(BIN)rootdict7_s.o $(BIN)rootdict8_s.o $(BIN)rootdict9_s.o
else
ROOTDICTO=$(BIN)rootdict.o
ROOTDICTOS=$(BIN)rootdict_s.o
endif
endif
OBJECTS:= $(BIN)mtx.o $(ROOTDICTO)


MYOBJS1:=$(FILES:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTS+= $(MYOBJS:$(SFF)%.f=$(BIN)%.o)
#new added
ifndef NOCTYPE
  OBJECTS+= $(BIN)ctype.o
endif

DPMFFLIST= $(wildcard $(DPMFF)/*.f)
DPMFFLIST1= $(notdir $(DPMFFLIST))
DPMFFOBJ=$(patsubst %.f, $(DPMBIN)%.o, $(DPMFFLIST1))

#########################################################################
## please put those objs which need special command to generate here ####
#########################################################################
ifeq ($(MARCH),linuxia64ecc) 
SPECIAL_OBJS=$(BIN)trigger302.o
SPECIAL_OBJS1= $(BIN)trddbc.o
OBJECTS:=$(filter-out $(SPECIAL_OBJS),$(OBJECTS)) 
OBJECTS:=$(filter-out $(SPECIAL_OBJS1),$(OBJECTS)) 
SPOPT=-O0
SPOPT1=-O0
ifdef G4MULTITHREADED
SPECIAL_OBJF=$(DPMBIN)dpm25nuc1g4.o  
SPECIAL_OBJF1=$(DPMBIN)dpm25nuc2.o
SPOPTF1=
SPOPT=-O0
DPMFFOBJ:=$(filter-out $(SPECIAL_OBJF),$(DPMFFOBJ))
DPMFFOBJ:=$(filter-out $(SPECIAL_OBJF1),$(DPMFFOBJ))

endif
else
ifdef G4MULTITHREADED
SPECIAL_OBJF=$(DPMBIN)dpm25lap.o
SPOPT=-O2
DPMFFOBJ:=$(filter-out $(SPECIAL_OBJF),$(DPMFFOBJ))
endif
endif
#------------------------------------------------------------------------------
# File list for readdaq  gbatch compilation

FILESR= $(COMMON_FILES)     $(SCC)readdaq.C $(SCC)daqevt.C  $(SFF)richnodisp_lip.F

ifndef PGTRACK
FILESR +=$(SFF)TrackFit_utils.F
endif

MYOBJS1:=$(FILESR:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.c=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.C=$(BIN)%.o) 
OBJECTR:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

OBJECTR+=	$(BIN)geant.o  $(BIN)geant3.o $(BIN)job.o \
		$(BIN)event.o   $(BIN)timeid.o $(BIN)ntuple.o \
		$(BIN)rootdict.o
#new added
ifndef NOCTYPE
  OBJECTR+= $(BIN)ctype.o
endif

#------------------------------------------------------------------------------
# File list for interactive gbatch compilation

ifdef  G4AMS
FILESI= $(COMMON_FILES)  \
	$(SCC)g4visman.C $(SFF)gi.F $(SCC)gi2.C  \
	$(SCC)job.C $(SCC)event.C $(SCC)daqevt.C \
        $(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)richdisp_lip.F
else
FILESI= $(COMMON_FILES)  \
	$(SCC)gxint.C $(SFF)gi.F $(SCC)gi2.C  \
	$(SCC)job.C $(SCC)event.C $(SCC)daqevt.C \
        $(SCC)timeid.C  $(SCC)ntuple.C  $(SFF)richdisp_lip.F
endif
ifndef PGTRACK
FILESI += $(SFF)TrackFit_utils.F
endif

MYOBJS1:=$(FILESI:$(SFF)%.F=$(BIN)%.o)
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTSI:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 

#$(info "Building OBJECTSI using $(OBJECTSI)")
#$(error Exiting the Makefile)

ifdef  G4AMS
OBJECTSI+= $(BIN)mtx.o  $(ROOTDICTO) $(BIN)geant4i.o $(BIN)geanti.o  $(BIN)gbatchi.o
else
OBJECTSI+= $(BIN)mtx.o  $(ROOTDICTO)   $(BIN)geanti.o 	$(BIN)geant3i.o  
endif
#new added
ifndef NOCTYPE
  OBJECTSI+= $(BIN)ctype.o
endif

#------------------------------------------------------------------------------
# File list for client(production) gbatch compilation

FILESC= $(COMMON_FILES)     $(SCC)producer.C $(SCC)client.C \
	$(SCC)ins_dummy.C $(SFF)rndm.F $(SFF)richnodisp_lip.F

ifeq ($(MARCH),linuxppcbgxlc)
    SPECIAL_FILES=$(SCC)Tofsim02.C
    FILESC:=$(filter-out $(SPECIAL_FILES),$(FILESC))
    SPECIAL_FILES_PPC:=$(SPECIAL_FILES:.C=_ppc.C)
    FILESC+= $(SPECIAL_FILES_PPC)
endif

MYOBJS1:=$(FILESC:$(SFF)%.F=$(BIN)%.o) 
MYOBJS2:=$(MYOBJS1:$(SCC)%.C=$(BIN)%.o) 
MYOBJS:=$(MYOBJS2:$(SCC)%.c=$(BIN)%.o) 
OBJECTSC:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o) 
OBJECTSCL:=$(MYOBJS:$(SFF)%.f=$(BIN)%.o)

ifdef  G4AMS
OBJECTSC:=$(filter-out $(BIN)geant4.o,$(OBJECTSC)) 
OBJECTSI:=$(filter-out $(BIN)geant4.o,$(OBJECTSI))
OBJECTSC+= $(BIN)geant4_c.o
OBJECTSCL+= $(BIN)geant4_l.o
OBJECTSCL:=$(filter-out $(BIN)geant4.o,$(OBJECTSCL))
endif
ifndef PGTRACK
OBJECTSC += $(BIN)TrackFit_utils_c.o
OBJECTSCL += $(BIN)TrackFit_utils_c.o
endif
OBJECTSC+=  $(BIN)gbatch_c.o  $(BIN)geant_c.o  $(BIN)geant3_c.o  $(BIN)job_c.o \
            $(BIN)event_c.o   $(BIN)timeid_c.o $(BIN)ntuple_c.o  $(BIN)daqevt_c.o  $(ROOTDICTO)
OBJECTSCL+= $(BIN)gbatch_l.o  $(BIN)geant_l.o  $(BIN)geant3_l.o  $(BIN)job_l.o \
            $(BIN)event_l.o   $(BIN)timeid_l.o $(BIN)ntuple_l.o  $(BIN)daqevt_l.o  $(ROOTDICTO)
OBJECTSCL:=$(filter-out $(BIN)producer.o,$(OBJECTSCL))
OBJECTSCL:=$(filter-out $(BIN)client.o,$(OBJECTSCL))
OBJECTSCL+= $(BIN)producer_l.o $(BIN)client_l.o
#new added
ifndef NOCTYPE
  OBJECTSC+= $(BIN)ctype.o
  OBJECTSCL+= $(BIN)ctype.o
endif

#---Qi Yan
ifdef G4AMS
$(DPMBIN)%.o : $(DPMFF)%.f
	@echo Compiling  $< ...
	@if ! [ -d $(DPMBIN) ] ; then mkdir -p $(DPMBIN); fi
	$(AT)$(FF) -c $(FFLAGS)  -I$(CERNDIR)/include $< -o $@


$(DPMBIN)%.o : $(DPMSCC)%.cc
	@echo Compiling  $< ...
	@if ! [ -d $(DPMBIN) ] ; then mkdir -p $(DPMBIN); fi
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $< -o $@

DPMSCCLIST= $(wildcard $(DPMSCC)/*.cc)
DPMSCCLIST1= $(notdir $(DPMSCCLIST))
DPMSCCOBJ=$(patsubst %.cc, $(DPMBIN)%.o, $(DPMSCCLIST1))

DPMFFLIST= $(wildcard $(DPMFF)/*.f)
DPMFFLIST1= $(notdir $(DPMFFLIST))
DPMFFOBJ=$(patsubst %.f, $(DPMBIN)%.o, $(DPMFFLIST1))
OBJECTS+= $(DPMSCCOBJ) $(DPMFFOBJ)
OBJECTSI+= $(DPMSCCOBJ) $(DPMFFOBJ)
OBJECTSC+= $(DPMSCCOBJ) $(DPMFFOBJ)
OBJECTSCL+= $(DPMSCCOBJ) $(DPMFFOBJ)
#----DPM3
ifneq (${DPM3FF},)
$(DPM3FFBIN)%.o: $(DPM3FF)/%.f
	@echo Compiling  $< ...
	@if ! [ -d $(DPM3FFBIN) ] ; then mkdir -p $(DPM3FFBIN); fi
#	$(AT)$(FF) -c $(FFLAGS)  -I$(CERNDIR)/include $< -o $@
	$(AT)$(FF) -c $(FFLAGSUS3)  -I$(CERNDIR)/include $< -o $@

$(DPM3CCBIN)%.o: $(DPM3CC)/%.cc $(DPM3CC)/*.h
	@echo Compiling  $< ...
	@if ! [ -d $(DPM3CCBIN) ] ; then mkdir -p $(DPM3CCBIN); fi
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $< -o $@

DPM3FFLIST= $(wildcard $(DPM3FF)/*.f)
DPM3FFLIST1= $(notdir $(DPM3FFLIST))
DPM3FFOBJ=$(patsubst %.f, $(DPM3FFBIN)%.o, $(DPM3FFLIST1))
DPM3CCLIST= $(wildcard $(DPM3CC)/*.cc)
DPM3CCLIST1= $(notdir $(DPM3CCLIST))
DPM3CCOBJ=$(patsubst %.cc, $(DPM3CCBIN)%.o, $(DPM3CCLIST1))
OBJECTS+=  $(DPM3CCOBJ) $(DPM3FFOBJ)
OBJECTSI+= $(DPM3CCOBJ) $(DPM3FFOBJ)
OBJECTSC+= $(DPM3CCOBJ) $(DPM3FFOBJ)
OBJECTSCL+= $(DPM3CCOBJ) $(DPM3FFOBJ)
endif
#----
endif

#------------------------------------------------------------------------------
ifdef GENFIT
$(GENFITBIN)%.o : $(GENFITSCC)%.cc
	@echo Compiling  $< ...
	@if ! [ -d $(GENFITBIN) ] ; then mkdir -p $(GENFITBIN); fi
	$(AT)$(CXXG) -c $(CXXFLAGSG) $(CFLAGSG) $< -o $@

GENFITSCCLIST= $(wildcard $(GENFITSCC)/*.cc)
GENFITSCCLIST1= $(notdir $(GENFITSCCLIST))
GENFITSCCOBJ=$(patsubst %.cc, $(GENFITBIN)%.o, $(GENFITSCCLIST1))
OBJECTS+= $(GENFITSCCOBJ) 
OBJECTSI+= $(GENFITSCCOBJ) 
OBJECTSC+= $(GENFITSCCOBJ) 
OBJECTSCL+= $(GENFITSCCOBJ) 
#ifdef VERSION6
ROOTDICTO+= $(GENFITBIN)rootgenfitdict.o
ROOTDICTOS+= $(GENFITBIN)rootgenfitdict.o
OBJECTR+= $(GENFITBIN)rootgenfitdict.o 
OBJECTS+= $(GENFITBIN)rootgenfitdict.o
OBJECTSI+= $(GENFITBIN)rootgenfitdict.o
OBJECTSC+= $(GENFITBIN)rootgenfitdict.o
OBJECTSCL+= $(GENFITBIN)rootgenfitdict.o
#endif
endif

#------------------------------------------------------------------------------
#new added
ifndef NOCTYPE
  OBJECTSC6:=$(filter-out $(BIN)ctype.o,$(OBJECTSC))
  OBJECTSC6+= $(BIN)ctype_l6.o
endif
#------------------------------------------------------------------------------
#----   Files list for the ORBiT/CORBA client/server 
#------------------------------------------------------------------------------

ifndef UID2DST
  IDLFILE=server
else
  IDLFILE=serverrc
endif

SRC=$(BIN)
INCLUDE=../include/

INCSCPP= $(SRC)$(IDLFILE)-cpp-stubs.h $(SRC)$(IDLFILE)-cpp-skels.h $(SRC)$(IDLFILE)-cpp-common.h

SRCS= $(SRC)$(IDLFILE)-stubs.c $(SRC)$(IDLFILE)-skels.c $(SRC)$(IDLFILE)-common.c
OBJECTCO=$(BIN)$(IDLFILE)-stubs.o $(BIN)$(IDLFILE)-skels.o $(BIN)$(IDLFILE)-common.o


SRCSCPP= $(SRC)$(IDLFILE)-cpp-stubs.cc $(SRC)$(IDLFILE)-cpp-skels.cc $(SRC)$(IDLFILE)-cpp-common.cc
OBJECTCL= $(BIN)$(IDLFILE)-cpp-stubs.o                           $(BIN)$(IDLFILE)-cpp-common.o
OBJECTSE=  $(BIN)$(IDLFILE)-cpp-skels.o   $(OBJECTCL)

OBJECTSEAUX=    $(BIN)id.o         $(BIN)node.o     $(BIN)astring.o \
		$(BIN)client.o     $(BIN)amsdbc.o   $(BIN)timeid_server.o \
		$(BIN)apool.o      $(BIN)snode.o    $(BIN)dbserver.o \
		$(BIN)system_bg.o $(BIN)commonsi.o

#--------------------------------------------------------------------------------------------------

SRCSMT= $(SRCMT)$(IDLFILE)-stubs.c $(SRCMT)$(IDLFILE)-skels.c $(SRCMT)$(IDLFILE)-common.c
OBJECTCOMT=$(BINMT)$(IDLFILE)-stubs.o $(BINMT)$(IDLFILE)-skels.o $(BINMT)$(IDLFILE)-common.o

SRCSCPPMT= $(SRCMT)$(IDLFILE)-cpp-stubs.cc $(SRCMT)$(IDLFILE)-cpp-skels.cc $(SRCMT)$(IDLFILE)-cpp-common.cc
OBJECTCLMT= $(BINMT)$(IDLFILE)-cpp-stubs.o                             $(BINMT)$(IDLFILE)-cpp-common.o
OBJECTSEMT=  $(BINMT)$(IDLFILE)-cpp-skels.o   $(OBJECTCLMT)

OBJECTSEAUXMT=	$(BIN)id.o 	   $(BIN)node.o      $(BIN)astring.o  \
		$(BIN)client_mt.o  $(BIN)amsdbc.o    $(BIN)timeid.o    \
		$(BIN)apool.o      $(BIN)snode.o     $(BIN)dbserver_mt.o

#new added
ifndef NOCTYPE
  OBJECTSEAUX+= $(BIN)ctype.o
  OBJECTSEAUXMT+= $(BIN)ctype.o
endif

#------------------------------------------------------------------------------
#----   TARGETS
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
#  Gbatch standalone batch processing

STANDALONE:=$(EXE)gbatch$(G4)$(PG).exe 
STANDALONED:=$(EXE)gbatchd$(G4)$(PG).exe

gbatch: amswd $(STANDALONE)
gbatchd: amswd $(STANDALONED)

$(STANDALONE): $(OBJECTS) $(SPECIAL_OBJS) $(SPECIAL_OBJS1) $(SPECIAL_OBJF) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...
	@if [ -f $(BIN)/rootdict_rdict.pcm ]; then cp -f $(BIN)/rootdict_rdict.pcm $(LIB); cp -f $(BIN)/rootdict_rdict.pcm $(EXE); fi
	@if [ -f $(GENFITBIN)/rootgenfitdict_rdict.pcm ]; then cp -f $(GENFITBIN)/rootgenfitdict_rdict.pcm $(LIB); cp -f $(GENFITBIN)/rootgenfitdict_rdict.pcm $(EXE); fi
	$(AT)$(LD) $(FLAGSSTATIC)  -o $@ $(OBJECTS) $(SPECIAL_OBJS) $(SPECIAL_OBJS1) $(TIMEL) $(LDFLAGSSTATIC)

$(STANDALONED): $(OBJECTS) $(SPECIAL_OBJS) $(SPECIAL_OBJS1) $(SPECIAL_OBJF) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@if [ ! -d $(LIB) ] ; then mkdir -p $(LIB); echo "Creating the $(LIB) directory ..."; fi
	@if [ -f $(BIN)/rootdict_rdict.pcm ]; then cp -f $(BIN)/rootdict_rdict.pcm $(LIB); cp -f $(BIN)/rootdict_rdict.pcm $(EXE); fi
	@if [ -f $(GENFITBIN)/rootgenfitdict_rdict.pcm ]; then cp -f $(GENFITBIN)/rootgenfitdict_rdict.pcm $(LIB); cp -f $(GENFITBIN)/rootgenfitdict_rdict.pcm $(EXE); fi
	@echo Linking $@ ...
	$(AT)$(LD) $(FLAGSDYNAMIC)  -o $@ $(OBJECTS) $(SPECIAL_OBJS) $(SPECIAL_OBJS1) $(TIMEL) $(LDFLAGSDYNAMIC)


$(SPECIAL_OBJS):$(SPECIAL_OBJS: %.o=%.C)
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGS) $(SPOPT)  $(SCC)trigger302.C -o $(BIN)/trigger302.o

$(SPECIAL_OBJS1):$(SPECIAL_OBJS1: %.o=%.C)
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGS) $(SPOPT1)  $(SCC)trddbc.C -o $(BIN)/trddbc.o

$(SPECIAL_OBJF):$(DPMFF)dpm25nuc1g4.f
	$(AT)$(FF) -c $(FFLAGS) $(SPOPT)   $(DPMFF)dpm25nuc1g4.f -o $(DPMBIN)dpm25nuc1g4.o

$(SPECIAL_OBJF):$(DPMFF)dpm25lap.f
	$(AT)$(FF) -c $(FFLAGS) $(SPOPT)   $(DPMFF)dpm25lap.f -o $(DPMBIN)dpm25lap.o

$(SPECIAL_OBJF1):$(DPMFF)dpm25nuc2.f
	$(AT)$(FF) -c $(FFLAGS) $(SPOPTF1)   $(DPMFF)dpm25nuc2.f -o $(DPMBIN)dpm25nuc2.o


$(SCC)Tofsim02_ppc.C:$(SCC)Tofsim02.C
	sed "s/phmap\./phmap[AMSEvent::get_thread_num()]\./g" $(SCC)Tofsim02.C >$(SCC)Tofsim02_ppc.C

$(SCC)Tofcharge_ihep_ppc.C:$(SCC)Tofcharge_ihep.C
	sed -e "s/chargepar\./chargepar[AMSEvent::get_thread_num()]\./g" -e "s/chargepar=/chargepar[AMSEvent::get_thread_num()]=/g" $(SCC)Tofcharge_ihep.C >$(SCC)Tofcharge_ihep_ppc.C


$(SCC)RichCharge_ppc.C:$(SCC)RichCharge.C
	sed -e "s/m_tag[NTAG]\./m_tag[NTAG][AMSEvent::get_thread_num()]\./g" -e "s/v_utime\[l/v_utime[AMSEvent::get_thread_num()]\[l/g" -e "s/v_utime\./v_utime[AMSEvent::get_thread_num()]\./g" -e "s/v_pmt_dts\./v_pmt_dts[AMSEvent::get_thread_num()]\./g" -e "s/v_temp\[dts\]/v_temp\[dts\][AMSEvent::get_thread_num()]/g" -e "s/v_pmt_dts\[i/v_pmt_dts\[AMSEvent::get_thread_num()\]\[i/g" -e "s/v_pmt_dts\[p/v_pmt_dts\[AMSEvent::get_thread_num()\]\[p/g" -e "s/v_pmt_rep\[p/v_pmt_rep[AMSEvent::get_thread_num()]\[p/g" -e "s/v_pmt_rep\./v_pmt_rep[AMSEvent::get_thread_num()]\./g" $(SCC)RichCharge.C >$(SCC)RichCharge_ppc.C


 
readdaq:amswd  $(EXE)readdaq$(PG)

$(EXE)readdaq$(PG):  $(OBJECTR)
	@echo Linking $@ ...
	$(AT)$(LD) $(FLAGSSTATIC)  -o  $@  $(OBJECTR) $(TIMEL) $(LDFLAGSSTATIC)

#------------------------------------------------------------------------------
#  Gbatch standalone interactive session

ifndef G4AMS
INTERACTIVE:=$(EXE)gxint$(G4)$(PG).exe 
gxint :	amswd $(INTERACTIVE)


$(INTERACTIVE):	$(OBJECTSI) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...
	$(AT)$(LD) -o $@  $(OBJECTSI) $(LDFLAGSI)


$(BIN)geanti.o:      $(SCC)geant.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGSI) $(SCC)geant.C -o $(BIN)/geanti.o

$(BIN)geant3i.o:      $(SCC)geant3.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGSI) $(SCC)geant3.C -o $(BIN)/geant3i.o
else
INTERACTIVE:=$(EXE)gxint$(G4)$(PG).exe 
gxint :	amswd $(INTERACTIVE)


$(INTERACTIVE):	$(OBJECTSI) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...
	$(AT)$(LD) -o $@  $(OBJECTSI) $(LDFLAGSI)


$(BIN)geant4i.o:      $(SCC)geant4.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGSI) -DG4VIS_USE $(SCC)geant4.C -o $(BIN)/geant4i.o

$(BIN)geanti.o:      $(SCC)geant.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGSI) $(SCC)geant.C -o $(BIN)/geanti.o
$(BIN)gbatchi.o:      $(SCC)gbatch.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c  $(CXXFLAGS) $(CFLAGSI) $(SCC)gbatch.C -o $(BIN)/gbatchi.o
endif
#---------------------------------------------------------------------------------
# Client TARGET (producer)

CLIENT=$(EXE)gbatch-orbit$(G4)$(PG).exe
CLIENTL=$(EXE)gbatch-noorbit$(G4)$(PG).exe
CLIENTL6=$(EXE)gbatch-noorbit$(G4)$(PG).exe.lxplus6
CLIENT6=$(EXE)gbatch-orbit$(G4)$(PG).exe.lxplus6
producerlite: amswd $(CLIENTL)
#                   Orbit-C      Orbit-C++   gbatch
$(CLIENTL):          $(OBJECTSCL) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...$(OBJECTSCL)
	$(AT)$(LD) $(FLAGSSTATIC) -o $@  $(OBJECTSCL) $(TIMEL) $(LDFLAGSSTATIC)

producer: amswd	$(CLIENT)

#                   Orbit-C      Orbit-C++   gbatch
$(CLIENT):          $(OBJECTCO) $(OBJECTCL) $(OBJECTSC) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...
	$(AT)$(LD) $(FLAGSSTATIC) -o $@  $(OBJECTCO)  $(OBJECTCL) $(OBJECTSC) $(LDFLAGSSTATICIDL) $(TIMEL) $(LDFLAGSSTATIC)


producer6: amswd	$(CLIENT6)

#                   Orbit-C      Orbit-C++   gbatch
$(CLIENT6):          $(OBJECTCO) $(OBJECTCL) $(OBJECTSC6) $(BIN)predict.o
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...
	$(AT)$(LD) $(FLAGSSTATIC) -o $@  $(OBJECTCO)  $(OBJECTCL) $(OBJECTSC6) $(LDFLAGSSTATICIDL) $(TIMEL) $(LDFLAGSSTATIC)


#---------------------------------------------------------------------------------
#  Server TARGET (server)

SERVER=$(EXE)amsprodserver$(G4)$(PG).exe

test: $(OBJECTCO)
	echo $^

server: amswd	$(SERVER)
#               Orbit-C   Orbit-C++-Ser
$(SERVER):    $(OBJECTCO) $(OBJECTSE)   $(OBJECTSEAUX) $(BIN)server.o 
	@if [ ! -d $(EXE) ] ; then mkdir -p $(EXE); echo "Creating the $(EXE) directory ..."; fi
	@echo Linking $@ ...
	$(AT)$(LD) -o $(SERVER) $(BIN)server.o  $(OBJECTCO) $(OBJECTSE) $(OBJECTSEAUX) $(LDFLAGSSTATICIDL) 


#------------------------------------------------------------------------------
#   Libraries to read the ROOT output

fava:
	echo $(CC)
	echo $(CXX)
	echo $(FF)
	echo $(LD)

lib: shared static 
ifdef PGTRACK
SHLIBO:=$(AMS_BASEO) $(TRACK_BASEO) $(BIN)amschain_s.o $(BIN)node.o $(BIN)snode.o  $(BIN)tkpatt_s.o \
        $(BIN)rnormx_s.o $(BIN)TrFit_s.o $(BIN)TrRecon_s.o $(BIN)TrReconQ_s.o $(BIN)TrSim_s.o $(BIN)TrMCCluster_s.o $(BIN)TrSimCluster_s.o $(BIN)TrSimSensor_s.o $(BIN)TkDBc_s.o $(BIN)TrTrack_s.o $(BIN)GausBF_s.o \
	$(BIN)root_rs.o $(BIN)root_setup_s.o  $(BIN)SlowControlDB_s.o $(BIN)TrCalDB_s.o $(BIN)TrTasDB_s.o $(BIN)TrGainDB_s.o $(BIN)TrChargeLossDB_s.o $(BIN)TrOccDB_s.o $(BIN)MonoSpline_s.o \
	$(BIN)TrEDepDB_s.o $(BIN)TrMipDB_s.o $(BIN)TrLinearDB_s.o $(BIN)TrLikeDB_s.o $(BIN)TrMass_s.o $(BIN)Betalhd_s.o\
        $(ROOTDICTOS) $(BIN)TrdHCalib_s.o $(BIN)Vertex_s.o   $(BIN)EcalFastRec_s.o $(BIN)MagField_s.o  $(BIN)HistoMan_s.o $(BIN)TrHistoMan_s.o $(BIN)TrRecHit_s.o $(BIN)HistoProofMan_s.o\
        $(BIN)VCon_s.o $(BIN)TrdHSegment_s.o $(BIN)TrdHTrack.o  $(BIN)TrdMcDatacards.o $(BIN)TrdSCalib_s.o  $(BIN)TrdKCalib_s.o $(BIN)TrdKToyMC_s.o  $(BIN)TrdMTrack_s.o  $(BIN)TrdKCluster_s.o $(BIN)TrdKHit_s.o $(BIN)TrdKPDF_s.o $(BIN)TrdHCharge.o $(BIN)TrdTFit.o $(BIN)TrdHRecon_s.o $(BIN)VCon_root_s.o $(BIN)TrElem.o  $(BIN)ecalBDT.o $(BIN)ecalBDTCHI2.o $(BIN)commonsi.o $(BIN)EcalChi2CY_s.o $(BIN)EcalH_s.o $(BIN)EcalKDbc.o  $(BIN)Ecal3DRec.o $(BIN)EcalKRec.o $(BIN)EcalShowerProfileFit_s.o $(BIN)EcalShowerDirectionFit_s.o $(BIN)DeadSideCorr_s.o $(BIN)GammaFit_s.o \
	$(BIN)timeid_s.o $(BIN)TrExtAlignDB_s.o  $(BIN)TrInnerDzDB_s.o $(BIN)DynAlignment_s.o $(BIN)RichCharge_s.o $(BIN)RichConfig_s.o $(BIN)RichBeta_s.o $(BIN)RichTools_s.o $(BIN)pfunction_s.o $(BIN)OrbGen_s.o $(BIN)cgmmap.o $(BIN)Sunposition.o $(BIN)FrameTrans.o $(BIN)GeoMagField.o $(BIN)GeoMagCoord.o $(BIN)GeoMagTrace.o   $(BIN)GM_SubLibrary.o $(BIN)LxMCcutoff.o \
	$(BIN)rkms_s.o $(BIN)gufld.o $(BIN)util.o $(BIN)predict.o $(BIN)gitsha.o \
        $(BIN)MSplineH_s.o $(BIN)Fit2DH_s.o $(BIN)TrAsymmDBH_s.o $(BIN)TrBetaDBH_s.o $(BIN)TrChargeDBH_s.o $(BIN)TrChargeLossDBH_s.o $(BIN)TrClusterChargeH_s.o $(BIN)TrGainCHDBH_s.o $(BIN)TrGainDBH_s.o $(BIN)TrRecHitChargeH_s.o $(BIN)TrRigidityDBH_s.o $(BIN)TrTempDBH_s.o $(BIN)TrThetaDBH_s.o $(BIN)TrTrackChargeH_s.o ${BIN}TrChargeYJ_s.o
SHLIBO+=$(BIN)TofGeometry_s.o  $(BIN)TofRecon_s.o  $(BIN)TofTrack_s.o $(BIN)Tofdbc_s.o $(BIN)Tofrec02_ihep_s.o $(BIN)Tofcharge_ihep_s.o $(BIN)AMSPlane_s.o $(BIN)TkDBcN_s.o $(BIN)TkGeomN_s.o $(BIN)bcorr_s.o
# new ECAL 3D reconstruction
SHLIBO+=$(BIN)EcalKX3DRec_s.o $(BIN)EcalKXCalib_s.o $(BIN)EcalKXFastRec_s.o $(BIN)EcalKXRec_s.o

ifeq ($(MARCH),linuxppcbgxlc)
  SHLIBO+=$(BIN)apool.o
endif

SHLIBO+=$(BIN)BackTracingMIB.o
SHLIBO+=$(BIN)BackTracing.o
SHLIBO+=$(BIN)UnfoldingTools.o $(BIN)SpFold.o
SHLIBO+=$(BIN)AntiPG_s.o
SHLIBO+=$(BIN)TrkLH_s.o
SHLIBO+=$(BIN)TrkCC_s.o
SHLIBO+=$(BIN)TrRawCluster_s.o
SHLIBO+=${BIN}TrkBDTlapp_s.o

SHLIBO+=$(BIN)MVAmanager.o
SHLIBO+=$(BIN)ConfigParser.o
SHLIBO+=$(BIN)OutTreeManager.o
SHLIBO+=$(BIN)OutHistoManager.o
ifndef NO_NAG
  SHLIBO+= $(BIN)e04ccf.o
endif

#
# RICH Off reconstruction preliminary version
#
SHLIBO+=$(BIN)richdbcOff_s.o 
SHLIBO+=$(BIN)richtrrecOff.o
SHLIBO+=$(BIN)richradidOff.o
SHLIBO+=$(BIN)richidOff.o
SHLIBO+=$(BIN)richrecOff_s.o
SHLIBO+=$(BIN)cerenkov.o $(BIN)richpmtlib.o
SHLIBO+=$(BIN)richRefit_s.o


ifndef NO_MATHLIB
  SOFLAGS += -L$(CERNDIR)/lib -lmathlib
endif


SHLIBBO:=$(SHLIBO)
else
SHLIBBO := $(BIN)point.o $(BIN)edep.o $(BIN)amschain_s.o $(BIN)root_rs.o $(ROOTDICTOS) $(BIN)id.o $(BIN)VCon_s.o $(BIN)TrdHSegment_s.o $(BIN)TrdHTrack.o  $(BIN)TrdHCalib.o $(BIN)TrdHCharge.o $(BIN)TrdTFit.o $(BIN)TrdHRecon_s.o $(BIN)VCon_root_s.o $(BIN)TrElem.o $(BIN)root_setup_s.o $(BIN)tracking.o $(BIN)SlowControlDB.o $(BIN)ecalBDT.o $(BIN)ecalBDTCHI2.o $(BIN)EcalChi2CY_s.o $(BIN)EcalH_s.o $(BIN)EcalKDbc.o $(BIN)EcalFastRec.o $(BIN)Ecal3DRec.o $(BIN)EcalKRec.o $(BIN)EcalShowerProfileFit_s.o $(BIN)EcalShowerDirectionFit_s.o $(BIN)DeadSideCorr_s.o $(BIN)timeid_s.o $(BIN)amsdbc.o  $(BIN)astring.o $(BIN)commonsi.o $(BIN)TrPdf.o $(BIN)TrFitF.o $(BIN)tkfit.o $(BIN)rkms.o $(BIN)trfit_s.o $(BIN)tkfit.o $(BIN)/mfield_s.o $(BIN)/fint_my.o $(BIN)/bcorr_s.o  $(BIN)/trafit.o $(BIN)/txyfit.o  $(BIN)/tszfit.o $(BIN)/tptcir.o $(BIN)/tcircf.o $(BIN)/DynAlignment_s.o $(BIN)RichCharge_s.o $(BIN)RichConfig_s.o $(BIN)RichBeta_s.o $(BIN)RichTools_s.o $(BIN)Sunposition.o $(BIN)FrameTrans.o $(BIN)Tofdbc_s.o $(BIN)Tofrec02_ihep_s.o  $(BIN)Tofcharge_ihep_s.o $(BIN)AMSPlane_s.o $(BIN)TkDBcN_s.o $(BIN)TkGeomN_s.o $(BIN)GeoMagField.o $(BIN)GeoMagCoord.o $(BIN)GeoMagTrace.o  $(BIN)GM_SubLibrary.o $(BIN)LxMCcutoff.o $(BIN)predict.o  $(BIN)BackTracingMIB.o $(BIN)BackTracing.o $(BIN)UnfoldingTools.o $(BIN)SpFold.o $(BIN)gitsha.o
SHLIBBO+=$(BIN)AntiPG_s.o
#SHLIBBO+=$(BIN)EcalKX3DRec.o $(BIN)EcalKXCalib.o $(BIN)EcalKXFastRec.o $(BIN)EcalKXRec.o
SHLIBO := $(BIN)point.o $(BIN)edep.o $(BIN)amschain_s.o $(BIN)root_rs.o $(ROOTDICTOS) $(BIN)id.o $(BIN)VCon_s.o $(BIN)TrdHSegment_s.o $(BIN)TrdHTrack.o $(BIN)TrdHCalib.o $(BIN)TrdHCharge.o $(BIN)TrdTFit.o $(BIN)TrdHRecon_s.o $(BIN)VCon_root_s.o $(BIN)TrElem.o $(BIN)root_setup_s.o $(BIN)tracking.o $(BIN)SlowControlDB.o $(BIN)ecalBDT.o $(BIN)ecalBDTCHI2.o $(BIN)EcalChi2CY_s.o $(BIN)EcalH_s.o $(BIN)EcalKDbc.o $(BIN)EcalFastRec.o $(BIN)Ecal3DRec.o $(BIN)EcalKRec.o $(BIN)EcalShowerProfileFit_s.o $(BIN)EcalShowerDirectionFit_s.o $(BIN)DeadSideCorr_s.o $(BIN)timeid_s.o $(BIN)amsdbc.o  $(BIN)astring.o $(BIN)commonsi.o $(BIN)TrPdf.o  $(BIN)/trfit.o $(BIN)bcorr_s.o $(BIN)DynAlignment_s.o $(BIN)RichCharge_s.o $(BIN)RichConfig_s.o $(BIN)RichBeta_s.o $(BIN)RichTools_s.o $(BIN)Sunposition.o $(BIN)FrameTrans.o $(BIN)Tofdbc_s.o $(BIN)Tofrec02_ihep_s.o $(BIN)Tofcharge_ihep_s.o $(BIN)AMSPlane_s.o $(BIN)TkDBcN_s.o $(BIN)TkGeomN_s.o $(BIN)GeoMagField.o $(BIN)GeoMagCoord.o $(BIN)GeoMagTrace.o $(BIN)GM_SubLibrary.o $(BIN)predict.o  $(BIN)BackTracingMIB.o $(BIN)BackTracing.o $(BIN)LxMCcutoff.o
#SHLIBO+=$(BIN)EcalKX3DRec.o $(BIN)EcalKXCalib.o $(BIN)EcalKXFastRec.o $(BIN)EcalKXRec.o
SHLIBO+=$(BIN)UnfoldingTools.o $(BIN)SpFold.o $(BIN)gitsha.o
SHLIBO+=$(BIN)AntiPG_s.o

SHLIBO+=$(BIN)MVAmanager.o
SHLIBO+=$(BIN)ConfigParser.o
SHLIBO+=$(BIN)OutTreeManager.o
SHLIBO+=$(BIN)OutHistoManager.o
ifndef NO_NAG
  SHLIBO+= $(BIN)e04ccf.o
endif

endif

ifdef GENFIT
SHLIBO+= $(GENFITSCCOBJ)
SHLIBBO+= $(GENFITSCCOBJ)
endif


shared : amswd $(SHLD) $(SHLD_LINK)

$(SHLD):  $(SHLIBO)   ../include/root_RV.h ../include/root.h  
	@if [ ! -d $(LIB) ] ; then mkdir -p $(LIB); echo "Creating the $(LIB) directory ..."; fi
	@if [ -f $(BIN)/rootdict_s_rdict.pcm ]; then cp -f $(BIN)/rootdict_s_rdict.pcm $(LIB); fi
	@if [ -f $(GENFITBIN)/rootgenfitdict_rdict.pcm ]; then cp -f $(GENFITBIN)/rootgenfitdict_rdict.pcm $(LIB); fi
	@echo Creating shared library  $(SHLD) ...
	$(AT)$(LD) -O   $(SHLIBO) $(SOFLAGS) $(SHLDFLAGS)  -o  $(SHLD)

$(SHLD_LINK): $(SHLD)
	@if [ ! -d $(LIB) ] ; then mkdir -p $(LIB); echo "Creating the $(LIB) directory ..."; fi
	@echo Creating symlink  $(SHLD_LINK) ...
	$(AT)ln -sf $(shell basename $(SHLD)) $(SHLD_LINK)

static : amswd $(NTLD)

$(NTLD): $(SHLIBBO)
	@if [ ! -d $(LIB) ] ; then mkdir -p $(LIB); echo "Creating the $(LIB) directory ..."; fi
	@echo Creating static Library  $(NTLD) ...
	$(AT)$(AR) cr   $(NTLD) $(SHLIBBO)

#-------------------------------------------------------------------------------
# these files do not use the default rules

ifndef VGM_INSTALL
$(BIN)geant.o: 	 $(SCC)geant.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__BATCH__  $< -o $@
else
$(BIN)geant.o: 	 $(SCC)geant.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D_USEVGM_ -I$(VGM_INSTALL)/include -D__BATCH__  $< -o $@
endif

$(BIN)geant3.o:	 $(SCC)geant3.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGSTOFSIM) -D__BATCH__  $< -o $@

$(BIN)ctype.o: $(SCC)ctype.c
ifeq  "$(shell uname)"  "Linux"
	@echo Compiling  $< ... 
	$(AT)$(CC) -c $(CFLAGS) $< -o$@
else 
	cp $(SCC)ctype.c $(BIN)ctype.o
endif

$(BIN)ctype_l6.o: $(SCC)ctype.c
	@echo Compiling  $< ...
	$(AT)$(CC) -c -D__LXPLUS6__ $< -o$@
$(BIN)predict.o:  $(SCC)predict.c
	@echo Compiling  $< ...
	$(AT)$(CC) -c $(CFLAGS) $< -o$@

../include/root_methods$(PG).h:      ../perl/root.perl
	@echo Generating $@ ...
	$(AT)perl ../perl/root.perl

ifdef PGTRACK
$(BIN)rootdict1.C:  $(ROOTDICTFILES) ../include/linkdef1.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict1.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict.C:  $(ROOTDICTFILES) ../include/linkdef.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)
ifdef VERSION6
	$(AT)sed "s/vector<int>\[2\]/vector<int>/g" $(BIN)rootdict.C >$(BIN)rootdict_d.C
	$(AT)mv  $(BIN)rootdict_d.C $(BIN)rootdict.C
endif

$(BIN)rootdict2.C:  $(ROOTDICTFILES) ../include/linkdef2.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict2.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict3.C:  $(ROOTDICTFILES) ../include/linkdef3.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict3.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict4.C:  $(ROOTDICTFILES) ../include/linkdef4.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict4.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict5.C:  $(ROOTDICTFILES) ../include/linkdef5.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict5.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict6.C:  $(ROOTDICTFILES) ../include/linkdef6.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict6.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict7.C:  $(ROOTDICTFILES) ../include/linkdef7.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict7.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict8.C:  $(ROOTDICTFILES) ../include/linkdef8.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict8.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)

$(BIN)rootdict9.C:  $(ROOTDICTFILES) ../include/linkdef9.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict9.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)
	@echo Generating dictionary ... OK

$(BIN)rootdict_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^))
	@echo Generating dictionary ... OK
ifdef VERSION6
	$(AT)sed "s/vector<int>\[2\]/vector<int>/g" $(BIN)rootdict_s.C >$(BIN)rootdict_s_d.C 
	$(AT)mv  $(BIN)rootdict_s_d.C $(BIN)rootdict_s.C 
endif
$(BIN)rootdict1_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef1.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict1_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^))
	@echo Generating dictionary ... OK
$(BIN)rootdict2_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef2.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict2_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict3_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef3.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict3_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict4_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef4.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict4_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict5_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef5.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict5_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict6_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef6.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict6_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict7_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef7.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict7_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict8_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef8.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict8_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
$(BIN)rootdict9_s.C:   ../include/VCon_root.h ../include/root_RVSP.h ../include/amschain.h $(ROOTDICTFILES) ../include/root_methods$(PG).h ../include/linkdef9.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)    -f $(BIN)rootdict9_s.C -c -p  $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^)) 
	@echo Generating dictionary ... OK
else
$(BIN)rootdict.C: ../include/point.h ../include/edep.h ../include/root_RV.h ../include/root.h ../include/root_setup.h ../include/trigger102_setup.h ../include/trfit.h ../include/DynAlignment.h ../include/RichTools.h ../include/RichCharge.h ../include/RichConfig.h  ../include/Tofrec02_ihep.h ../include/Tofcharge_ihep.h ../include/EcalChi2CY.h ../include/EcalH.h ../include/EcalKDbc.h ../include/EcalFastRec.h ../include/Ecal3DRec.h ../include/EcalKRec.h ../include/GeoMagField.h ../include/GeoMagCoord.h ../include/GeoMagTrace.h ../include/LxMCcutoff.h ../include/SpFold.h ../include/bcorr.h ../include/linkdef.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)      -f $(BIN)rootdict.C -c -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(notdir $^)
ifdef VERSION6
	$(AT)sed "s/vector<int>\[2\]/vector<int>/g" $(BIN)rootdict.C >$(BIN)rootdict_d.C 
	$(AT)mv  $(BIN)rootdict_d.C $(BIN)rootdict.C 
endif
	@echo Generating dictionary ... OK

$(BIN)rootdict_s.C: ../include/point.h ../include/edep.h ../include/VCon_root.h ../include/root_RVS.h ../include/root.h ../include/amschain.h  ../include/tracking.h ../include/trfit.h ../include/DynAlignment.h ../include/RichTools.h ../include/RichCharge.h ../include/RichConfig.h ../include/Tofrec02_ihep.h ../include/Tofcharge_ihep.h ../include/EcalChi2CY.h ../include/EcalH.h ../include/EcalKDbc.h ../include/EcalFastRec.h ../include/Ecal3DRec.h ../include/EcalKRec.h ../include/GeoMagField.h ../include/GeoMagCoord.h ../include/GeoMagTrace.h ../include/LxMCcutoff.h ../include/bcorr.h ../include/UnfoldingTools.h ../include/SpFold.h ../include/AntiPG.h ../include/TrkLH.h ../include/TrkCC.h ../include/root_methods$(PG).h ../include/linkdef.h
	@echo Generating dictionary ...
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(BIN)rootdict_s.C -c -I$(realpath $(ROOTINCDIR)) -I$(realpath ../include) $(filter-out root_methods$(PG).h,$(notdir $^))
ifdef VERSION6
	$(AT)sed "s/vector<int>\[2\]/vector<int>/g" $(BIN)rootdict_s.C >$(BIN)rootdict_s_d.C 
	$(AT)mv  $(BIN)rootdict_s_d.C $(BIN)rootdict_s.C 
endif
	@echo Generating dictionary ... OK

endif

$(BIN)commonsi_time.C:      $(SCC)commonsi.C
	@echo Generating $@ ...
	$(AT)sed "s_buildtime=[^buildtime=]*;_buildtime=$(shell date +%s);_" $(SCC)commonsi.C > $(BIN)commonsi_time.C

$(BIN)rootdict.o:         $(BIN)rootdict.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict.C -o $(BIN)rootdict.o

$(BIN)rootdict1.o:         $(BIN)rootdict1.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict1.C -o $(BIN)rootdict1.o

$(BIN)rootdict2.o:         $(BIN)rootdict2.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict2.C -o $(BIN)rootdict2.o

$(BIN)rootdict3.o:         $(BIN)rootdict3.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict3.C -o $(BIN)rootdict3.o

$(BIN)rootdict4.o:         $(BIN)rootdict4.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict4.C -o $(BIN)rootdict4.o

$(BIN)rootdict5.o:         $(BIN)rootdict5.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict5.C -o $(BIN)rootdict5.o

$(BIN)rootdict6.o:         $(BIN)rootdict6.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict6.C -o $(BIN)rootdict6.o

$(BIN)rootdict7.o:         $(BIN)rootdict7.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict7.C -o $(BIN)rootdict7.o

$(BIN)rootdict8.o:         $(BIN)rootdict8.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict8.C -o $(BIN)rootdict8.o

$(BIN)rootdict9.o:         $(BIN)rootdict9.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(BIN)rootdict9.C -o $(BIN)rootdict9.o

$(BIN)commonsi_s.o:	$(BIN)commonsi_time.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__ROOTSHAREDLIBRARY__ $(BIN)commonsi_time.C -o $(BIN)commonsi_s.o

$(BIN)commonsi.o:	$(BIN)commonsi_time.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(BIN)commonsi_time.C -o $(BIN)commonsi.o

$(BIN)gitsha.o: $(SCC)gitsha.C $(BIN)gitsha.txt
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -DGBATCHGITSHA=$(GITSHA) $< -o $@

$(BIN)daqevt.o:         $(SCC)daqevt.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGSDAQ) -D__AMS02DAQ__ $(SCC)daqevt.C -o $(BIN)daqevt.o

$(BIN)daqevt_c.o:         $(SCC)daqevt.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGSDAQ) -D__AMS02DAQ__ $(IDLFLAGS) $(SCC)daqevt.C -o $(BIN)daqevt_c.o

$(BIN)daqevt_l.o:         $(SCC)daqevt.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGSDAQ) -D__AMS02DAQ__ $(IDLFLAGS) $(SCC)daqevt.C -D__CORBA_LIGHT__ -o $(BIN)daqevt_l.o


$(BIN)rootdict_s.o:         $(BIN)rootdict_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict_s.C -o $(BIN)rootdict_s.o


$(BIN)rootdict1_s.o:         $(BIN)rootdict1_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict1_s.C -o $(BIN)rootdict1_s.o


$(BIN)rootdict2_s.o:         $(BIN)rootdict2_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict2_s.C -o $(BIN)rootdict2_s.o


$(BIN)rootdict3_s.o:         $(BIN)rootdict3_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict3_s.C -o $(BIN)rootdict3_s.o


$(BIN)rootdict4_s.o:         $(BIN)rootdict4_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict4_s.C -o $(BIN)rootdict4_s.o


$(BIN)rootdict5_s.o:         $(BIN)rootdict5_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict5_s.C -o $(BIN)rootdict5_s.o


$(BIN)rootdict6_s.o:         $(BIN)rootdict6_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict6_s.C -o $(BIN)rootdict6_s.o


$(BIN)rootdict7_s.o:         $(BIN)rootdict7_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict7_s.C -o $(BIN)rootdict7_s.o


$(BIN)rootdict8_s.o:         $(BIN)rootdict8_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict8_s.C -o $(BIN)rootdict8_s.o


$(BIN)rootdict9_s.o:         $(BIN)rootdict9_s.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  $(BIN)rootdict9_s.C -o $(BIN)rootdict9_s.o


$(BIN)root_rs.o:    	 $(SCC)root.C ../include/root_methods$(PG).h
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS)  -D__ROOTSHAREDLIBRARY__ $(SCC)root.C -o $(BIN)root_rs.o


ifdef GENFIT
$(GENFITBIN)rootgenfitdict.o:  $(GENFITBIN)rootgenfitdict.C
	@echo Compiling  $< ...
	@if ! [ -d $(GENFITBIN) ] ; then mkdir -p $(GENFITBIN); fi
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CFLAGSDICT) $(GENFITBIN)rootgenfitdict.C -o $(GENFITBIN)rootgenfitdict.o
ifdef VERSION6
$(GENFITBIN)rootgenfitdict.C:  $(GENFITDIR)/include/*.h ../include/fittersLinkDef6.h
else
$(GENFITBIN)rootgenfitdict.C:  $(GENFITDIR)/include/MilleBinary.h ../include/fittersLinkDef.h
endif
	@echo Generating dictionary ...
	@if ! [ -d $(GENFITBIN) ] ; then mkdir -p $(GENFITBIN); fi
	$(AT)$(ROOTSYSRUN)/bin/$(ROOTCINT)   -f $(GENFITBIN)rootgenfitdict.C -c  -p $(ROOTDICT_CPPFLAGS) -I$(realpath $(ROOTINCDIR)) -I$(realpath $(GENFITDIR)/include) -I$(realpath ../include) $(notdir  $^)
endif

$(BIN)ntuple.o:    	 $(SCC)ntuple.C

ifdef AMSMEMMONITOR
	@echo Compiling  $< ...
	$(AT)$(CXX) -c -D__MEMMONITOR__ $(CXXFLAGS) $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
else
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple.o
endif
$(BIN)ntuple_l.o:   $(SCC)ntuple.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) -D__CORBA_LIGHT__ $(SCC)ntuple.C -o $(BIN)ntuple_l.o

$(BIN)ntuple_c.o:   $(SCC)ntuple.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) $(SCC)ntuple.C -o $(BIN)ntuple_c.o
$(BIN)TrackFit_utils_c.o:   $(SFF)TrackFit_utils.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGSn) $(IDLFLAGS) $(SFF)TrackFit_utils.F -o $(BIN)TrackFit_utils_c.o
$(BIN)TrackFit_utils.o:	$(SFF)TrackFit_utils.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGSn)  $(SFF)TrackFit_utils.F -o $(BIN)TrackFit_utils.o

$(BIN)Geant3_utils.o:   $(SFF)Geant3_utils.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS) $(FFLAGSSAVE)  -I$(CERNDIR)/include $(SFF)Geant3_utils.F -o $(BIN)Geant3_utils.o
$(BIN)trd.o:   $(SFF)trd.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS) $(FFLAGSSAVE) -I$(CERNDIR)/include $(SFF)trd.F -o $(BIN)trd.o
$(BIN)rkms.o:   $(SFF)rkms.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS) $(FFLAGSOPT) -I$(CERNDIR)/include $(SFF)rkms.F -o $(BIN)rkms.o
$(BIN)BackTracing.o:   $(SFF)BackTracing.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS) $(FFLAGSSAVE) -I$(CERNDIR)/include $(SFF)BackTracing.F -o $(BIN)BackTracing.o

ifeq  "$(shell uname)"  "SunOS" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	@echo Compiling  $< ...
	$(AT)f77 -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "HP-UX" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	@echo Compiling  $< ...
	$(AT)f77 -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "Darwin" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "Linux" 

$(BIN)mtx.o:                   $(SFF)mtx77.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS)      $(SFF)mtx77.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "AIX" 

$(BIN)mtx.o:                   $(SFF)mtx.F
	@echo Compiling  $< ...
	$(AT)xlf90 -c $(FFLAGS)      $(SFF)mtx.F -o $(BIN)mtx.o
endif
ifeq  "$(shell uname)"  "AIXold" 

$(BIN)mtx.o:                   $(SFF)mtx.F
	@echo Compiling  $< ...
	$(AT)xlf90 -c $(FFLAGS)      $(SFF)mtx.F -o $(BIN)mtx.o
endif

ifeq  "$(shell uname)"  "OSF1" 

$(BIN)mtx.o:                   $(SFF)mtx.F
	@echo Compiling  $< ...
	$(AT)f90 -c -free $(FFLAGS)      $(SFF)mtx.F -o $(BIN)mtx.o
endif


#===============================================================================
#  Rules to customize  and compile some  GEANT3 files
#===============================================================================

$(SFF)flinit.F:      $(CERNSRCDIR)/src/geant321/fiface/flinit.F
	@echo Generating $@ ...
	$(AT)sed  "s/      OPEN(UNIT=LUNERR,FILE='flukaerr.dat',STATUS='UNKNOWN')/#if defined __IFC || defined __INTEL_COMPILER\n       OPEN(UNIT=LUNERR,FILE='\/dev\/null',shared,STATUS='UNKNOWN')\n#else\n       OPEN(UNIT=LUNERR,FILE='\/dev\/null',STATUS='UNKNOWN')\n#endif/g"  $(CERNSRCDIR)/src/geant321/fiface/flinit.F > $(SFF)flinit.F


$(SFF)gbirk.F:      $(CERNSRCDIR)/src/geant321/gphys/gbirk.F
	@echo Generating $@ ...
	$(AT)sed  "s/  99  RETURN/             IF(ABS(CHARGE).GE.2..AND.MODEL.EQ.1)C=0\n      IF(ABS(CHARGE).GE.2)THEN\n       GAMASS=GETOT+AMASS\n       BET2=GEKIN*GAMASS\/(GETOT*GETOT)\n        BET=SQRT(BET2)\n       W1=1.034-0.1777*EXP(-0.08114*CHARGE)\n       W2=BET\/ABS(CHARGE)**(2.\/3.)\n       W3=121.4139*W2+0.0378*SIN(190.7165*W2)\n       CHARG1=CHARGE*(1.-W1*EXP(-W3))\n       IF(CHARG1.LT.0)CHARG1=1\n       CHARG2=CHARG1**2\n       DEDXCM=DEDXCM*CHARG2\n      ENDIF\n       IF(MODEL.EQ.0)THEN\n        C=C*DENS**2     \n        RKB=RKB*DENS \n       EDEP=DESTEP\/(1.+C*ATAN(RKB\/C*DEDXCM))\n      ELSE\n       EDEP=DESTEP\/(1.+RKB*DEDXCM+C*DEDXCM**2)\n      ENDIF\n  99  RETURN/g"  $(CERNSRCDIR)/src/geant321/gphys/gbirk.F > $(SFF)gbirk.F


$(SFF)grfile.F:      $(CERNSRCDIR)/src/geant321/giopa/grfile.F
	@echo Generating $@ ...
	$(AT)sed  "s/NQUOT=1000/NQUOT=32000/g"  /$(CERNSRCDIR)/src/geant321/giopa/grfile.F > $(SFF)grfile.F

$(BIN)grfile.o:      $(SFF)grfile.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)grfile.F -o $(BIN)grfile.o

$(SFF)gstord.F:      $(CERNSRCDIR)/src/geant321/gtrak/gstord.F
	@echo Generating $@ ...
	$(AT)sed  "s/DO 30/       if(jpolar.lt.jstak .and.jplast+NWPOLA.gt.jstak)then\n          write(*,*)' jpolar overwrite??? ',jplast,jstak\n           return\n            endif\n       DO 30/"   $(CERNSRCDIR)/src/geant321/gtrak/gstord.F > $(SFF)gstord.F

$(BIN)gstord.o:      $(SFF)gstord.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gstord.F -o $(BIN)gstord.o

$(SFF)gskpho.F:      $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F
	@echo Generating $@ ...
	$(AT)sed  "s/IQ(JSTAK+1)+N2-N1+1-IQ(JPOLAR-1)\/3/IQ(JSTAK+2)-IQ(JPOLAR-1)\/3/g;s/N2-N1/NGPHOT\*10/g"  $(CERNSRCDIR)/src/geant321/gtrak/gskpho.F > $(SFF)gskpho.F

#$(BIN)gskpho.o:      $(SFF)gskpho.F
#	@echo Compiling  $< ...
#	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gskpho.F -o $(BIN)gskpho.o

$(BIN)ggckov.o:      $(SFF)ggckov.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)ggckov.F -o $(BIN)ggckov.o

$(BIN)graylc.o:      $(SFF)graylc.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)graylc.F -o $(BIN)graylc.o

$(BIN)gtckov.o:      $(SFF)gtckov.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(SFF)gtckov.F -o $(BIN)gtckov.o

$(SFF)hadevv.F:      $(CERNSRCDIR)/src/geant321/fluka/hadevv.F
	@echo Generating $@ ...
	$(AT)sed  "0,/AMCH2 .LT. AMB8/ s/AMCH2 .LT. AMB8/AMCH2 .LT. AMB8 .AND. AMB8 .LT. UMO*0.5/"  $(CERNSRCDIR)/src/geant321/fluka/hadevv.F > $(SFF)hadevv.F.tmp
	$(AT)sed  "s/ACH1.LE./ACH1.LT./;s/ACH2.LE./ACH2.LT./"  $(SFF)hadevv.F.tmp > $(SFF)hadevv.F
	$(AT)rm -rf $(SFF)hadevv.F.tmp

$(BIN)hadevv.o:      $(SFF)hadevv.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) $(WF)-DCERNLIB_DOUBLE -I$(CERNDIR)/include $(SFF)hadevv.F -o $(BIN)hadevv.o

$(SFF)sbcomp.F:      $(CERNSRCDIR)/src/geant321/peanut/sbcomp.F
	@echo Generating  $@ ...
	$(AT)sed  "s/STOP 'BIMPCT'/write(*,*)'****SBCOMP BIMPCT STOP ****',R0TRAJ,BIMPCT,R1TRAJ/g"  $(CERNSRCDIR)/src/geant321/peanut/sbcomp.F > $(SFF)sbcomp.F

$(BIN)sbcomp.o:      $(SFF)sbcomp.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) -I$(CERNDIR)/include $(WF)-DCERNLIB_DOUBLE $(SFF)sbcomp.F -o $(BIN)sbcomp.o


$(BIN)flinit.o:      $(SFF)flinit.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) $(WF)-DCERNLIB_UNIX -I$(CERNDIR)/include $(SFF)flinit.F -o $(BIN)flinit.o

$(BIN)gbirk.o:      $(SFF)gbirk.F
	@echo Compiling  $< ...
	$(AT)$(FFOLD) -c $(FFLAGS) $(WF)-DCERNLIB_UNIX -I$(CERNDIR)/include $(SFF)gbirk.F -o $(BIN)gbirk.o

$(BIN)richpmtlibo.o:	$(SFF)richpmtlib.F
	@echo Compiling  $< ...
	$(AT)$(FF) -c $(FFLAGS) $(WF)-DCERNLIB_UNIX -I$(CERNDIR)/include $(SFF)richpmtlib.F -o $(BIN)richpmtlib.o




#---------------------------------------------------------------------------------
# Additional Rules for client/server 

#!!!!!
$(OBJECTSE): $(BIN)%.o : $(SRC)%.cc
	$(CXX_ORBIT) -c $< -o $@

$(OBJECTCO): $(BIN)%.o : $(SRC)%.c
	$(CC_ORBIT)  -c $< -o $@ 


$(SRC)%-stubs.c $(SRC)%-skels.c $(SRC)%-common.c : $(INCLUDE)$(IDLFILE).idl
	$(IDL) $(INCLUDE)$(IDLFILE).idl
	mv *.h $(SRC)
	mv *.c $(SRC)

$(SRC)%-cpp-stubs.cc $(SRC)%-cpp-skels.cc $(SRC)%-cpp-common.cc : $(SRCS) $(INCLUDE)$(IDLFILE).idl
	$(IDLCPP) $(INCLUDE)$(IDLFILE).idl
	mv *.h $(SRC)
	mv *.cc $(SRC)


$(BIN)gbatch_c.o:         $(SCC)gbatch.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) -D__ROOTA__ $(SCC)gbatch.C $(IDLFLAGS)  -o $(BIN)gbatch_c.o

$(BIN)geant_c.o: 	 $(SCC)geant.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGSI) -D__BATCH__ $(IDLFLAGS) $(SCC)geant.C -o $(BIN)geant_c.o


$(BIN)job_c.o: 	 $(SCC)job.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) $(SCC)job.C -o $(BIN)job_c.o

$(BIN)geant3_c.o: 	 $(SCC)geant3.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) -D__BATCH__ $(IDLFLAGS) $(SCC)geant3.C -o $(BIN)geant3_c.o

$(BIN)geant4_c.o:        $(SCC)geant4.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) -D__BATCH__ $(IDLFLAGS) -UG4VIS_USE $(SCC)geant4.C -o $(BIN)geant4_c.o

$(BIN)geant4.o: 	 $(SCC)geant4.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__BATCH__ -UG4VIS_USE $(SCC)geant4.C -o $(BIN)geant4.o


$(BIN)event_c.o: 	 $(SCC)event.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) $(SCC)event.C -o $(BIN)event_c.o

$(BIN)timeid_c.o:        $(SCC)timeid.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGSTOFSIM) $(IDLFLAGS) $(SCC)timeid.C -o $(BIN)timeid_c.o

$(BIN)gbatch_l.o:         $(SCC)gbatch.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__CORBA_LIGHT__ -D__ROOTA__ $(SCC)gbatch.C $(IDLFLAGS)  -o $(BIN)gbatch_l.o

$(BIN)geant_l.o:         $(SCC)geant.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGSI) -D__CORBA_LIGHT__ -D__BATCH__ $(IDLFLAGS) $(SCC)geant.C -o $(BIN)geant_l.o


$(BIN)job_l.o:   $(SCC)job.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) -D__CORBA_LIGHT__ $(SCC)job.C -o $(BIN)job_l.o

$(BIN)geant3_l.o:        $(SCC)geant3.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__BATCH__ $(IDLFLAGS) -D__CORBA_LIGHT__ $(SCC)geant3.C -o $(BIN)geant3_l.o

$(BIN)geant4_l.o:        $(SCC)geant4.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) -D__BATCH__ $(IDLFLAGS) -D__CORBA_LIGHT__ -UG4VIS_USE $(SCC)geant4.C -o $(BIN)geant4_l.o

$(BIN)event_l.o:         $(SCC)event.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) -D__CORBA_LIGHT__ $(SCC)event.C -o $(BIN)event_l.o

$(BIN)timeid_l.o:        $(SCC)timeid.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGSTOFSIM) $(IDLFLAGS) -D__CORBA_LIGHT__ $(SCC)timeid.C -o $(BIN)timeid_l.o

$(BIN)timeid.o:        $(SCC)timeid.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c $(CXXFLAGS) $(CFLAGSTOFSIM) $(SCC)timeid.C -o $(BIN)timeid.o

$(BIN)timeid_server.o:        $(SCC)timeid.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c $(CXXFLAGS) $(CFLAGS) -D__CORBASERVER__ $(SCC)timeid.C -o $(BIN)timeid_server.o

$(BIN)producer.o: 	 $(SCC)producer.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c -w $(CXXFLAGS) $(CFLAGSI) $(IDLFLAGS) $(SCC)producer.C -o $(BIN)producer.o

$(BIN)producer_l.o:        $(SCC)producer.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c -w $(CXXFLAGS) $(CFLAGSI)  -D__CORBA_LIGHT__ $(IDLFLAGS) $(SCC)producer.C -o $(BIN)producer_l.o

$(BIN)client.o: 	 $(SCC)client.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -fpermissive -c -w $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) $(SCC)client.C -o $(BIN)client.o

$(BIN)client_l.o:          $(SCC)client.C
	@echo Compiling  $< ...
	$(AT)$(CXX) -c -w $(CXXFLAGS) $(CFLAGS) -D__CORBA_LIGHT__ $(SCC)client.C $(IDLFLAGS) -o $(BIN)client_l.o

$(BIN)server.o: 	 $(SCC)server.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT)  -c -w $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) $(OPENMP_FLAG) $(SCC)server.C -o $(BIN)server.o

$(BIN)dbserver.o: 	 $(SCC)dbserver.C
	@echo Compiling  $< ...
	$(AT)$(CXX_ORBIT) -c -w $(CXXFLAGS) $(CFLAGS) $(IDLFLAGS) $(SCC)dbserver.C -o $(BIN)dbserver.o

$(BIN)VCon_s.o: ../include/VCon_root.h

##################### ADDTIONAL (PHONY) TARGETS ##############################################
#checks if the directory AMSWD exist 
amswd:
	@if ! [ -d $(AMSWD) ]; then echo FATAL: the directory pointer by AMSWD = $(AMSWD) is missing !!!; exit 1;fi

depend:         $(BIN)Make-depend

touch_depend:
	touch $(BIN)Make-depend

$(BIN)gitsha.txt: FORCE
	@if [ ! -f $(BIN)gitsha.txt ] || [ `cat $(BIN)gitsha.txt` != "$(GITSHA)" ]; then echo $(GITSHA) > $(BIN)gitsha.txt; fi

FORCE:

$(BIN)Make-depend:    $(INCS)
		@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
		@if ! [ -e $(BIN)Make-depend ] ; then touch $(BIN)Make-depend; fi
		@echo Running makedepend ...
		$(AT)makedepend $(DEFINES) -f $(BIN)Make-depend  -Y../include  $(FILES) -p$(BIN) > /dev/null 2>&1
		$(AT)cat  $(BIN)Make-depend | sed -e 'sx/../CC/x/x1' > $(BIN)Make-depend.new
		$(AT)rm -f $(BIN)Make-depend
		$(AT)mv $(BIN)Make-depend.new $(BIN)Make-depend

special-links:
	@if ! [ -d $(AMSWD)/$(MARCH) ] ; then mkdir $(AMSWD)/$(MARCH); fi
	@ln -s $(AMSWD)/exe/$(MARCH) $(AMSWD)/$(MARCH)/exe
	@ln -s $(AMSWD)/lib/$(MARCH) $(AMSWD)/$(MARCH)/lib
	@ln -s $(AMSWD)/bin/$(MARCH) $(AMSWD)/$(MARCH)/bin

clean : 
	rm -f $(BIN)*.o 
	rm -f $(BIN)Make-depend 
	rm -f $(BIN)*.C
	rm -f $(BIN)*.c
	rm -f $(BIN)*.cc
	rm -f $(BIN)*.h
	rm -f $(BIN)*.pcm
	rm -f $(LIB)*.pcm
	rm -f $(EXE)*.pcm
	rm -f $(STANDALONE) $(STANDALONED) $(INTERACTIVE) $(CLIENT) $(SERVER)
	rm -f $(NTLD) $(SHLD) $(SHLD_LINK)
	rm -f $(DPMBIN)*.o
	rm -f $(DPM3FFBIN)*.o
	rm -f $(DPM3CCBIN)*.o
	rm -f $(GENFITBIN)*
        
cleanALL : 
	rm -rf `find $(AMSWD)/bin -type f | grep -v 'CVS'`
	rm -rf `find $(AMSWD)/lib -type f | grep -v 'CVS'`
	rm -rf `find $(AMSWD)/exe -type f | grep -v 'CVS'`
	#rm -rf $(AMSWD)/$(MARCH)
	rm -f ../include/root_methods*.h

cleanlibs:
	rm -f $(NTLD) $(SHLD)
	rm -f $(BIN)/root*.o
	cd ../ruby; gmake clean
	cd ../python; gmake clean
	cd ../install
all:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE)  producer -i
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend
allg4:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE)  producer -i
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend
ifndef SLC6system
endif
fast64:
	$(MAKE) -j -f Makefastntrd64

allp:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE)  producer -i
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j -C../display
	$(MAKE) -j -C../display osdepend
	$(MAKE) -j server
allpold:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE) -j gxint
	$(MAKE)  producer -i
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend
	$(MAKE) -j server
allold:
	$(MAKE) -j 
	$(MAKE) -j lib 
	$(MAKE)  producer -i
	$(MAKE) -j -C../online
	$(MAKE) -j -C../online osdepend


ifneq ("$(wildcard /etc/redhat-release)","")
ifneq "$(shell cat /etc/redhat-release | grep -c 'Scientific Linux CERN SLC release 4')" "1"
	@echo "Please use SLC4 distribution to produce Linux 2.4 executables"
	#@rm -f $(STANDALONE) $(INTERACTIVE) $(CLIENT) $(SERVER)
endif
endif


##### DEPENDENCIES #####
ifneq ($(MAKECMDGOALS),touch_depend)
-include $(BIN)Make-depend
endif

# DO NOT DELETE
