#  $Id: Makefile,v 1.195 2023/12/13 10:43:35 bshan Exp $
# PZ reviewing the display Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc
#
#
# verbose output
ifdef  VERBOSE
VERBOSE=1
else
VERBOSE=0
endif
AT_0:=@
AT_1:=
AT=$(AT_$(VERBOSE))

ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch )
VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
VERSION6:=$(shell $(ROOTSYS)/bin/root-config --version | cut -b1-1)
ifeq ($(VERSION6),6)
VERSION6=6
else
VERSION6=
endif
VERSION614:=$(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ifeq ($(VERSION614),6.14)
VERSION614=614
else
VERSION614=
endif

ROOTINCDIR   := $(shell $(ROOTSYS)/bin/root-config --incdir)
ROOTLIBDIR   := $(shell $(ROOTSYS)/bin/root-config --libdir)

ifndef NAGDIR
  NAGDIR  = $(Offline)/CERN/NagLib
endif

# List of SLC6 changes
#
ifeq ($(wildcard $(ROOTINCDIR)/Compression.h),)
    LZMAL =
else
    DEFINES  +=  -D__LZMA__
    LZMAL = -llzma
endif

ifndef SLC6system
SLC6system=
ifneq "$(wildcard /etc/redhat-release)" ""
  RH_Version := $(shell cat /etc/redhat-release|awk -F'[^0-9]+' '{ print $$2 }')
  ifeq ($(RH_Version), $(filter $(RH_Version), 6 7 8 9))
    SLC6system := $(RH_Version)
  endif
endif
endif

#
#[ae-2012.08.17] - can't take from ../install/Makefile
#
ifdef VERSION6
XRDLIB = -lXrdClient   -lXrdUtils
ifneq ($(ARCH),macosx64)
ifndef SLC6system
SLC6system=1
endif
endif
ifdef VERSION614
LZMAL+=  -ltbb -llz4 -lxxhash -lclang -lcling -lllvm  -lz -lgsl -lblas -lgfortran -lquadmath 
else
LZMAL+= -llvm -lz -lgsl -lgslcblas
endif
endif

ifndef AMSDEPLIB
AMSDEPLIB := /cvmfs/ams.cern.ch/opt/
endif

#M.Duranti - 27/07/2013
__XROOTDPATHTEMP__ := $(shell root-config --config | awk 'BEGIN { FS = "--" } ; { for(i=1;i<=NF;i++){ print $$i"\n" } }' | grep "with-xrootd" )
__XROOTDPATH__ := $(subst with-xrootd=,,$(__XROOTDPATHTEMP__))

ifneq ($(strip $(__XROOTDPATH__)),)
MENOELLEXROOTD=-L$(strip $(__XROOTDPATH__))/lib/
endif

#
#[ae-2012.08.02] - some static libs we need take from slc5, initially we have there only those ones:
#    libcrypt.a  libexpat.a  libfontconfig.a  libshift.a
#
ifdef SLC6system
AMSLIBSUFFIX=slc6
else
AMSLIBSUFFIX=slc4
endif

IMPORTS_SLC5_LIBS=
ifdef SLC6system
  IMPORTS_SLC5_LIBS= -L$(AMSDEPLIB)/lib64
endif
#
#[ae-2012.08.02] - change local /usr/X11R6 to afs location
ifndef
  IMPORTS_X11R6= $(AMSDEPLIB)/X11R6
endif
#


ifndef AMSWD
AMSWD := ..
endif

ifdef LIBJPEGDIR
LIBJPEGFLAG=-L$(LIBJPEGDIR)
endif


PLATFORM      =  Linux
VERSIONP=$(VERSION)_$(RH_Version)

PREFIX        = $(AMSWD)/bin/$(ARCH)$(VERSIONP)/display/
DEPDIR        = $(PREFIX)/dep

AMSLIB        = ntuple_$(AMSLIBSUFFIX)
AMSLIBSO      = ntuple_$(AMSLIBSUFFIX).so
AMSPRG        = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/amsed
AMSPRGC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/amsedc
SLC4=y
#GCC_VER_MAJOR=$(shell gcc --version | grep gcc | cut -b11 )
#GCC_VER_MIDDLE=$(shell gcc --version | grep gcc |cut -b13 )
#GCC_VER_MINOR=$(shell gcc --version | grep gcc |cut -b15 )
GCC_VER_MAJOR=$(shell gcc --version | head -1 | cut '-d ' -f3 | cut -c1 )
GCC_VER_MIDDLE=$(shell gcc --version | head -1 | cut '-d ' -f3 | cut -c3 )
GCC_VER_MINOR=$(shell gcc --version | head -1 | cut '-d ' -f3 | cut -c5 )
XRDLIBP        :=

ifeq ($(GCC_VER_MAJOR),4)
VERGCC :=1
ifeq ($(GCC_VER_MIDDLE),4)
VERGCCH := 1
else ifeq ($(GCC_VER_MIDDLE),5)
VERGCCH := 1
else ifeq ($(GCC_VER_MIDDLE),6)
VERGCCH := 1
else ifeq ($(GCC_VER_MIDDLE),7)
VERGCCH := 1
else ifeq ($(GCC_VER_MIDDLE),8)
VERGCCH := 1
else ifeq ($(GCC_VER_MIDDLE),9)
VERGCCH := 1
endif
endif

# GCC >= 5
ifneq ($(GCC_VER_MAJOR),4)
VERGCC := 1
VERGCCH := 1
endif

ifeq ($(VERSION),5.27)
ifndef SLC6system
  XRDLIB = -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -lXrdNetUtil
else
  XRDLIB = -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys
endif
endif

ifeq ($(VERSION),5.34)
  XRDLIB = -lXrdClient   -lXrdUtils
endif
ifdef VERSION6
  XRDLIB = -lXrdClient   -lXrdUtils
endif

ifdef PGTRACK
PREFIX        = $(AMSWD)/bin/$(ARCH)$(VERSIONP)/display/PG/
AMSLIB        = ntuple_$(AMSLIBSUFFIX)_PG
AMSPRG        = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/amsedPG
AMSPRGC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/amsedcPG
AMSPRGCC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/amseddPG
endif


ifdef NOCASTOR
CPPFLAGS+= -D__NOCASTOR__
CASTORLIB=
endif

# display compiles only with customized ROOT, at the moment v5.21
NEW         :=_new

VER          := $(shell $(ROOTSYS)/bin/root-config --version )
LFORT = -L$(Offline)/AMSsoft.32/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -liomp5

ifeq ($(ARCH),linuxicc)

CXX           =  /cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/bin/ia32/icc
CPPFLAGS      =    -m32 -D__LINUXGNU__ -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = /cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/bin/ia32/icpc -axsse4.2,ssse3 -openmp -static-intel
LDFLAGS       =
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
ifdef PGTRACK
LFORT = -L$(Offline)/AMSsoft.32/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -liomp5
else
LFORT = -L$(Offline)/AMSsoft.32/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -liomp5
endif

LNAG = -L$(NAGDIR) -lnagifort32
ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lTMVA -lASImage -lXMLIO -lMLP -lTreePlayer -lGeom -lVMC  -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LFORT)
ifndef NOCASTOR
ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
ROOTLIBS     += -lNetx
endif
ROOTLIBSA     =   -L$(ROOTLIBDIR) -lRoot -pthread  -lAfterImage   $(ROOTLIBDIR)/libXrdClient.a -lpcre -lncurses \
	              -L$(IMPORTS_X11R6)/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                  -lXpm -lXmu -lXt -lXext -lX11 $(LIBJPEGFLAG) -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys $(LFORT)

ifndef NO_NAG
ROOTLIBS += $(LNAG)
ROOTLIBSA += $(LNAG)
endif

LIBS          =  -Bstatic -L$(AMSWD)/lib/linuxicc/ -l$(AMSLIB) -Bstatic $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm  $(XRDLIBP) $(XRDLIB) -Bdynamic -ldl
LIBSA         =  -L$(AMSWD)/lib/linuxicc/ -l$(AMSLIB) $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm        $(XRDLIBP) $(XRDLIB)      -ldl
LIBSD         =  -L$(AMSWD)/lib/linuxicc/ -l$(AMSLIB) $(ROOTLIBS) $(LIBJPEGFLAG) -ljpeg -lpng -lz

endif

ifeq ($(ARCH),linuxx8664icc)

ifndef XRDSYS
ifeq ($(VERSION),5.34)
ifndef SLC6system
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-11/lib64
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-12/lib64
endif
endif
ifdef VERSION6
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-12/lib64
endif
else
  XRDLIBP += -Bdynamic -L$(XRDSYS)/lib
endif



ifndef SLC6system
  ifndef INTELVER
    INTELVER=/cvmfs/ams.cern.ch/opt/intel/Compiler/11.1/073/
  endif
  CPPFLAGS= -D__LINUXGNU__ -axssse4.1,ssse3
else
  ifndef INTELVER
    INTELVER=/cvmfs/ams.cern.ch/opt/intel/composer_xe_2013_sp1.1.106/
  endif
  CPPFLAGS= -D__LINUXNEW__ -axAVX -msse3
ifdef VERSION6
  ifndef INTELVER
    INTELVER=compilers_and_libraries_2016.2.181/linux/
  endif

  CPPFLAGS= -D__LINUXNEW__ -DVERSION6 -std=c++11
  INTELOPTS=-axsse4.2,ssse3,AVX -fp-model source -fast-transcendentals
endif
endif
ifeq ("$(INTELBIN)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/compiler/bin/intel64)","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/compiler/bin/intel64
  endif
endif


ifeq ("$(INTELBIN)", "")
	ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/linux/bin/intel64)","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/linux/bin/intel64
  endif
endif

ifeq ("$(INTELBIN)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/bin/intel64)","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/bin/intel64
  endif
endif

ifeq ("$(INTELBIN)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/bin)","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/bin
  endif
endif

ifeq ("$(INTELLIB)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/compiler/lib/intel64)","")
    INTELLIB = $(INTELDIR)/$(INTELVER)/compiler/lib/intel64
  endif
endif

ifeq ("$(INTELLIB)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/lib/intel64)","")
    INTELLIB = $(INTELDIR)/$(INTELVER)/lib/intel64
  endif
endif
ifeq ("$(INTELLIB)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/linux/compiler/lib/intel64)","")
    INTELLIB = $(INTELDIR)/$(INTELVER)/linux/compiler/lib/intel64
  endif
endif

ifeq ("$(INTELLIB)", "")
  $(error Could not find intel compiler! $(INTELDIR)/$(INTELVER))
endif

ifndef ICPC
  ICPC=$(INTELBIN)/icpc
endif

CXX = $(ICPC)

ifeq ("$(findstring oneAPI,$(CXX))", "")   # [ae]-icc21.el9
   ICC_VERSION=$(shell $(CXX) --version | head -1 | cut '-d ' -f3 | cut -b1-2)
else
   ICC_VERSION=$(shell $(CXX) --version | head -1 | cut '-d ' -f3 | cut -b3-4)
endif

ifeq ($(ICC_VERSION),$(filter $(ICC_VERSION),16 17 18 19 20 21 22 23))
     OPENMP_FLAG = -qopenmp
else
     OPENMP_FLAG = -openmp
endif

CPPFLAGS     += -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
CXXFLAGS      = -Wno-deprecated $(OPENMP_FLAG)  $(OPT) $(CPPFLAGS) $(INTELOPTS)
OPT           = -O3
OPTC          = -O3
ifdef DEBUGFLAG
OPT           = -g
OPTC          = -g
endif
NOOPT         = -g
NOOPTC        = -g
LD            = $(ICPC)  $(CPPFLAGS) $(OPENMP_FLAG) -static-intel -static-libgcc -static-libstdc++

ifeq ($(ICC_VERSION),$(filter $(ICC_VERSION),21))
  CERN_LEVEL = 2023.el9/new
else
  CERN_LEVEL = 2005
endif
ifndef CERNDIR
  CERNDIR = $(Offline)/CERN/$(CERN_LEVEL)
endif

ifdef PGTRACK
  LDFLAGS =
  ifneq ($(ICC_VERSION),$(filter $(ICC_VERSION),21)) # [ae]-icc21.el9 
    LFORT= -L$(CERNDIR)/lib -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELLIB) -lifport -lifcoremt -limf  -lipgo -lirc_s
  else
    LFORT= -L$(CERNDIR)/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -L$(INTELLIB) -lifport -lifcoremt -limf  -lipgo -lirc_s
  endif
else
  LDFLAGS =
  LFORT =  -L$(CERNDIR)/lib -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELLIB) -lifport -lifcoremt -limf  -lipgo -lirc_s
  ifndef SLC6system
    LFORT += -liomp5
  endif
endif
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic

LNAG = -L$(NAGDIR) -lnag64
ROOTLIBS  =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMathMore -lgsl -lgslcblas -lMinuit -lTMVA -lASImage -lXMLIO -lMLP -lTreePlayer  -lGeom -lVMC -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LFORT)
ifdef VERSION614
ROOTLIBS+= -lCling
endif
ifndef NOCASTOR
ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
ROOTLIBS += -lNetx
endif
ROOTLIBSA =  -L$(ROOTLIBDIR) -lRoot $(LZMAL) -lcrypto -pthread  -lAfterImage  -lpcre -lncurses \
	         $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
             -lXpm -lXmu -lXt -lXext -lX11 $(LIBJPEGFLAG) -ljpeg -lpng -ltiff $(LFORT)

ifndef NO_NAG
ROOTLIBS += $(LNAG)
ROOTLIBSA += $(LNAG)
endif

ROOTLIBSA += -lncurses $(LFORT)

#LIBS          =   -Bstatic $(ROOTLIBSA)  -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm   -Bdynamic -ldl
LIBS          =   -Bstatic $(ROOTLIBSA) $(LZMAL)  -lm   $(XRDLIBP) $(XRDLIB) -Bdynamic    -lshift -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm  $(XRDLIBP) $(XRDLIB)
LIBSD         =  -L$(AMSWD)/lib/$(ARCH)$(VERSIONP)/ -l$(AMSLIB) $(ROOTLIBS)  $(LIBJPEGFLAG) -ljpeg -lpng -lz  $(XRDLIBP) $(XRDLIB)

endif

ifeq ($(ARCH),macosx64)

OSXVER := $(shell if [ `sw_vers -productVersion | sed 's/\.//g'` == "109" ]; then echo 1090; elif [ `sw_vers -productVersion | sed 's/\.//g'` == "1010" ]; then echo 10100; elif [ `sw_vers -productVersion | sed 's/\.//g'` == "1011" ]; then echo 10110; elif [ `sw_vers -productVersion | sed 's/\.//g'` == "1012" ]; then echo 10120; elif [ `sw_vers -productVersion | sed 's/\.//g'` == "1013" ]; then echo 10130; else echo `sw_vers -productVersion | sed 's/\.//g'`; fi)

ROOTINCDIR := $(shell root-config --incdir)

CXX = $(shell $(ROOTSYS)/bin/root-config --cxx)
CPPFLAGS      =  -D__APPLE__ -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -D__OSXVER__=$(OSXVER) -I. -I../include -I$(ROOTINCDIR)  -I/usr/X11R6/include/ -I/usr/X11R6/include/freetype2

ifdef VERSION6
 CPPFLAGS += -std=c++11
endif

CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O3
NOOPT         = -g

LD = $(shell $(ROOTSYS)/bin/root-config --cxx)
LDFLAGS       = -g -L$(GFORTRAN_DIR)/lib -L${GFORTRAN_DIR}/lib/gcc/7
LDS	= 	$(LD)
LDS	= 	$(LD)
LDD	= 	$(LD)

ROOTLIBSA       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer
ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lTMVA -lASImage -lXMLIO -lMLP -lTreePlayer -lGeom -lVMC -lEG 

ifndef NO_NAG
ROOTLIBS += $(LNAG)
ROOTLIBSA += $(LNAG)
endif
ifndef NOCASTOR
ROOTLIBSA += -lRCastor
ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
ROOTLIBSA += -lNetx
ROOTLIBS += -lNetx
endif

CERN_LEVEL = 2006
ifndef CERNDIR
  CERNDIR= $(Offline)/MacOS/CERN-mac/$(CERN_LEVEL)
endif

LNAG = -L$(NAGDIR) -lnagmac64
LFORT = -L$(CERNDIR)/lib -lgeant321 -lpacklib -lmathlib -lgfortran
LIBS          =  -L$(AMSWD)/lib/macosx64$(VERSIONP)/  -l$(AMSLIB) $(ROOTLIBS) $(LFORT)
LIBSA         =  -L$(AMSWD)/lib/macosx64$(VERSIONP)/ -l$(AMSLIB) $(ROOTLIBSA) $(LFORT)
LIBSD         =  -L$(AMSWD)/lib/macosx64$(VERSIONP)/ -l$(AMSLIB) $(ROOTLIBS)  $(LFORT)
endif
ifeq ($(ARCH),linux)

CXX           =  g++ -m32
CPPFLAGS      =  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -g
NOOPT         = -g

LD            = g++ -m32
LDFLAGS       = -g
LDS	= 	$(LD) -static
LD            = g++ -m32
LDFLAGS       = -g
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
ifdef PGTRACK
LFORT= -lgfortran
else
LFORT =  -L$(Offline)/AMSsoft.32/linux_slc5_gcc32/2005/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -lgfortran
endif

LNAG = -L$(NAGDIR) -lnaggfortran32
ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lTMVA -lASImage -lXMLIO -lMLP -lTreePlayer -lGeom -lVMC -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LFORT)
ifndef NOCASTOR
ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
ROOTLIBS += -lNetx
endif
ROOTLIBSA      = -L$(ROOTLIBDIR) -lRoot -pthread  -lAfterImage $(ROOTLIBDIR)/libXrdClient.a -lpcre -lncurses \
	          -L$(IMPORTS_X11R6)/lib -lXft -lfontconfig -lXrender -lfreetype  /usr/lib/libexpat.a   \
                  -lXpm -lXmu -lXt -lXext -lX11 $(LIBJPEGFLAG) -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdOuc -lXrdSys $(LFORT)

ifndef NO_NAG
ROOTLIBS += $(LNAG)
ROOTLIBSA += $(LNAG)
endif

LIBS          =   -Bstatic $(ROOTLIBSA)   -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm -Bdynamic -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm    -ldl
LIBSD          =   -L$(AMSWD)/lib/linux/ -l$(AMSLIB) $(ROOTLIBS) $(LIBJPEGFLAG) -ljpeg -lpng -lz

ifdef VERGCCH
  CPPFLAGS+= -D__LINUXNEW__
else
  CPPFLAGS+= -D__LINUXGNU__
endif
endif

ifeq ($(ARCH),linuxx8664gcc)

ifndef XRDSYS
ifeq ($(VERSION),5.34)
ifndef SLC6system
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-41/lib64
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-44/lib64
endif
endif
ifdef VERSION6
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-44/lib64
endif
else
  XRDLIBP += -Bdynamic -L$(XRDSYS)/lib
endif

CPPFLAGS=
ifdef VERSION6
  CPPFLAGS += -DVERSION6
endif

ifeq ($(GCC_VER_MAJOR),4)
ifneq ($(GCC_VER_MIDDLE),8)
ifneq ($(GCC_VER_MIDDLE),9)
  NOCXXSTD=1
endif
endif
endif

ifndef NOCXXSTD
  CXXSTD = -std=c++11
else
  CXXSTD =
endif

CXX           =  g++
CPPFLAGS     +=  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS) $(CXXSTD)
OPT           = -g
NOOPT         = -g

LD            = g++
LDFLAGS       = -g
LDS	= 	$(LD) -static
LD            = g++
LDFLAGS       = -g
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic

ifndef SLC6system
  CERN_LEVEL = 2005.gcc64
else
  CERN_LEVEL = 2005.gcc64.44
endif

ifndef CERNDIR
  CERNDIR = $(Offline)/CERN/$(CERN_LEVEL)
endif

LFORT =  -L$(CERNDIR)/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -lgfortran
LNAG = -L$(NAGDIR) -lnaggcc64 -lgfortran
ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMathMore -lgsl -lgslcblas -lMinuit -lTMVA -lASImage -lXMLIO -lMLP -lTreePlayer  -lGeom -lVMC -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LFORT)
ifndef NOCASTOR
ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
ROOTLIBS     += -lNetx
endif

ROOTLIBSA      = -L$(ROOTLIBDIR) -lRoot -pthread  -lAfterImage  -lpcre $(LZMAL) -lcrypto  -lncurses \
	             $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lexpat -lfontconfig -lXrender -lfreetype  /usr/lib64/libexpat.a   \
                 -lXpm -lXmu -lXt -lXext -lX11 $(LIBJPEGFLAG) -ljpeg -lpng -ltiff $(LFORT)
ifndef NO_NAG
ROOTLIBS += $(LNAG)
ROOTLIBSA += $(LNAG)
endif

LIBS          =  -Wl,--allow-multiple-definition -Bstatic $(ROOTLIBSA)    $(XRDLIBP) $(XRDLIB) -lm -Bdynamic    -lshift  -ldl

LIBSA         =  $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive -lm      $(XRDLIBP) $(XRDLIB) -ldl
LIBSD         =  -L$(AMSWD)/lib/$(ARCH)$(VERSIONP)/ -l$(AMSLIB) $(ROOTLIBS)  $(LIBJPEGFLAG) -ljpeg -lpng -lz $(XRDLIBP) $(XRDLIB)

ifdef VERGCCH
  CPPFLAGS+= -D__LINUXNEW__
else
  CPPFLAGS+= -D__LINUXGNU__
endif

# note: ARCH linuxx8664gcc ends here.
endif

ifdef GLIBCXX_USE_CXX11
  CPPFLAGS += -D_GLIBCXX_USE_CXX11_ABI=1
else
  CPPFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif

ifdef PGTRACK
CPPFLAGS+= -D_PGTRACK_
endif

ifdef CASTORSTATIC
CPPFLAGS+= -DCASTORSTATIC
ROOTLIBS+= -lRFIO
endif

# Other ARCH are not supported for the moment !!!!!!!!!!

#ifeq ($(ARCH),macosx)
#CXX           =  g++  -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
#OPT           = -O2
#OPTC          = -O0
#NOOPT         = -g
#LD            = g++
#LDFLAGS       = -g
#ROOTLIBS      =  -L$(ROOTLIBDIR) -lRoot -lfreetype -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11

#LIBS          =  $(ROOTLIBS) $(RH9add) -lm -ldl
#LIBSA          =  $(ROOTLIBS) $(RH9adda) -lm -ldl
#endif
#ifeq ($(ARCH),win32gcc)
#PREFIX        = $(AMSWD)/display/cygwin/
#CXX           =  g++ -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
#OPT           = -O2
#OPTC          = -O0
#NOOPT         = -g
#NOOPTC         = -g
#LD            = g++
#LDFLAGS       = -g
#ROOTLIBS      =  -L$(ROOTLIBDIR) -lRoot -lfreetype -lcrypt -L/usr/X11R6/lib  -lXpm.dll -lX11.dll $(RH9add) -L/usr/lib -lshift
#ROOTLIBSA      =  -L$(ROOTLIBDIR) -lRoot -lfreetype -lcrypt -L/usr/X11R6/lib  -lXpm.dll -lX11.dll $(RH9adda) -L/usr/lib -lshift
#LIBS          =  $(ROOTLIBS) -lm
#LIBSA          =  $(ROOTLIBSA) -lm
#endif
#ifeq ($(ARCH),win32)
#PREFIX   = $(AMSWD)/display/wingdk/
#CXX        =  $(ROOTSYS)/build/win/cxx.sh
#CXXFLAGS =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -O2 -Z7 -G5 -GR -GX -MD -DWIN32 -DGDK_WIN32  -D_WINDOWS -DWINVER=0x0400 -nologo -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -FIw32pragma.h -D_X86_=1 -D_DLL -DVISUAL_CPLUSPLUS -I../include  -I$(ROOTINCDIR)  /TP
#OPT           =
#OPTC          =
#NOOPT         = -g
#NOOPTC         = -g
#LD = $(ROOTSYS)/build/win/ld.sh
#LDFLAGS       =  -nologo -incremental:yes   -nodefaultlib:libc.lib -nodefaultlib:dfor.lib
#LIBS          =  $(ROOTLIBDIR)/libCore.lib $(ROOTLIBDIR)/libCint.lib $(ROOTLIBDIR)/libHist.lib $(ROOTLIBDIR)/libHistPainter.lib $(ROOTLIBDIR)/libGui.lib  $(ROOTLIBDIR)/libMatrix.lib $(ROOTLIBDIR)/libGeom.lib $(ROOTLIBDIR)/libPhysics.lib $(ROOTLIBDIR)/libGpad.lib $(ROOTLIBDIR)/libGraf.lib $(ROOTLIBDIR)/libGraf3d.lib $(ROOTLIBDIR)/libTree.lib  $(ROOTLIBDIR)/libRint.lib advapi32.lib
#ALIBS          = lib/libroot.a lib/libCore.lib lib/libfreetype.lib advapi32.lib
#endif




##### MACROS #####


SRCS           = main.cxx TGRunEventDialog.cxx ControlFrame.cxx \
                 AMSNtupleV.cxx   TView3DAMS.cxx    Help.cxx

SRCSN          = AMSR_GeometrySetter.cxx  AMSDisplay$(NEW).cxx \
                 TMarker3DCl$(NEW).cxx    AMSR_Axis.cxx

HDRS          = TGRunEventDialog.h    ControlFrame.h \
		AMSNtupleV.h         TView3DAMS.h     \
                AMSR_GeometrySetter.h AMSDisplay.h   \
                TMarker3DCl.h          AMSR_Axis.h

DEPS=  $(SRCS:%.cxx=$(DEPDIR)/%.d)
DEPS+= $(SRCSN:%.cxx=$(DEPDIR)/%.d)


DICT          = $(PREFIX)/AMSCint_$(ARCH)_$(SLC4)$(NEW).cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:$(PREFIX)/%.cxx=$(PREFIX)/%.o)
DICTPCM       = $(DICT:.cxx=_rdict.pcm)




OBJS          = $(SRCS:%.cxx=$(PREFIX)%.o)
OBJSN         = $(SRCSN:%.cxx=$(PREFIX)%.o)

ALLDICT       = $(DICT) $(DICTH)
ALLSRCS       = $(SRCS) $(SRCSN) $(DICT)
ALLOBJS       = $(OBJS) $(OBJSN) $(DICTO)





#ifeq ($(ARCH),win32)
#AMSPRG       = $(PREFIX)amsed.exe
#endif
#ifeq ($(ARCH),macosx)
#AMSPRG       = nothing
#LDS =  @echo "  Static Linking not supported in macosx!!! Use 'make osdepend' instead"
#LDD	= 	$(LD) -dynamic
#endif



##### TARGETS #####
standalone:  amslib $(AMSPRG)
osdepend:  $(AMSPRGC)
dynamic:   amslib $(AMSPRGCC)

amslib: $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
$(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a:
	$(MAKE) -C ../install lib

$(AMSPRG):    $(OBJSN)  $(OBJS) $(DICTO) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
		@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
		@echo Linking executable $(AMSPRG)
#		@echo $(__XROOTDPATH__)
#		@echo $(MENOELLEXROOTD)
#		@echo $(XRDLIB)
#		$(AT)$(LDS) $(LDFLAGS) $^ $(LIBSA) $(MENOELLEXROOTD) -lXrdClient -lXrd -lXrdNet -lXrdOuc -lXrdSys -o $(@)
		$(AT)$(LDS) $(LDFLAGS) $^ $(LIBSA) $(MENOELLEXROOTD) $(XRDLIB) -o $(@)
		@echo "$(AMSPRG) done"

$(AMSPRGC):	$(OBJSN)  $(OBJS) $(DICTO)  $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
		@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
		@echo Linking executable $(AMSPRGC)
		$(AT)$(LDD)   $(LDFLAGS) $^  $(LIBS) -o $(@)
		@echo "$(AMSPRGC) done"

ifeq ($(ARCH),macosx64)
$(AMSPRGCC):	$(OBJSN)  $(OBJS) $(DICTO)
		@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
		@if [ -f $(DICTPCM) ] ; then cp -f $(DICTPCM) $(AMSWD)/lib/$(ARCH)$(VERSIONP); fi
		@echo Linking executable $(AMSPRGCC)
		$(AT)$(LDD)    $(LDFLAGS) $^  $(LIBSD) -o $(@)
		$(AT)install_name_tool -change $(AMSWD)/lib/macosx64$(VERSIONP)//lib$(AMSLIB).so lib$(AMSLIB).so $(@)
		@echo "$(AMSPRGCC) done"
else
$(AMSPRGCC):	$(OBJSN)  $(OBJS) $(DICTO)
		@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
		@if [ -f $(DICTPCM) ] ; then cp -f $(DICTPCM) $(AMSWD)/lib/$(ARCH)$(VERSIONP); fi
		@echo Linking executable $(AMSPRGCC)
		$(AT)$(LDD)   $(LDFLAGS) $^  $(LIBSD) -o $(@)
		@echo "$(AMSPRGCC) done"
endif


$(DICT):        $(HDRS) LinkDef.h
		@echo "Generating dictionary ..."
		$(AT)$(ROOTSYS)/bin/rootcint -f $@ -c $(CPPFLAGS)  $(HDRS) LinkDef.h


dep:
	@echo  "Creating deps ..."



dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -vf $(ALLOBJS) $(ALLDICT) $(DEPS) $(AMSPRG) $(AMSPRGC)


##### RULES #####


$(DEPDIR)/%.d : %.cxx
	@echo "Generating Dep file $@ ..."
	@if ! [ -d $(DEPDIR) ] ; then mkdir -p $(DEPDIR); fi
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(OBJ)/$*.o -MF $@


$(PREFIX)%.o : $(PREFIX)%.cxx
	@echo Compiling $< ...
	$(AT)$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(PREFIX)%.o : %.cxx
	@echo Compiling $< ...
	$(AT)$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)




##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
