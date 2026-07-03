#  $Id: Makefile,v 1.200 2025/05/16 20:07:36 choutko Exp $
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
  ifeq ($(shell echo "$$(( $(RH_Version) > 5 ))"), 1)
    SLC6system := $(RH_Version)
  else
    INTELDIR=/cvmfs/ams.cern.ch/opt/intel/Compiler
    INTELVER=11.1/073
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
ifdef GCCLIB
LDFLAGS+= -L$(GCCLIB)
endif
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

#ifeq ($(ARCH),linuxx8664icc)
ifeq ($(ARCH),$(filter $(ARCH), linuxx8664icc linuxx8664icx))

########################################################################
# Try to find INTELBIN
########################################################################
ICV:=icc
ICPV:=icpc
IFORV:=ifort
ifndef NOICX
  ifneq ("$(findstring oneAPI,$(INTELDIR))","")
    ICV:=icx
#    ICV:=icpx
    ICPV:=icpx
    IFORV:=ifx
  endif
endif

ifeq ("$(INTELBIN)", "")
  ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/bin/$(ICPV))","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/bin
  else ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/linux/bin/intel64/$(ICPV))","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/linux/bin/intel64
  else ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/compiler/bin/intel64/$(ICPV))","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/compiler/bin/intel64
  else ifneq ("$(wildcard $(INTELDIR)/$(INTELVER)/bin/intel64/$(ICPV))","")
    INTELBIN = $(INTELDIR)/$(INTELVER)/bin/intel64
  endif
endif

########################################################################
# Manual overrides for compilers
########################################################################

ifndef ICC
  ifeq ("$(INTELBIN)", "")
    $(error Could not find Intel C++ compiler!  $(INTELDIR)/$(INTELVER))
  endif
  ICC:=$(INTELBIN)/$(ICV)
else
  ICV:=$(notdir $(ICC))
endif

ifndef ICPC
  ifeq ("$(INTELBIN)", "")
    $(error Could not find Intel C compiler!  $(INTELDIR)/$(INTELVER))
  endif
  ICPC:=$(INTELBIN)/$(ICPV)
else
  ICPV:=$(notdir $(ICPC))
endif

########################################################################
# Compiler flags
########################################################################

# [ae]-icc23.el9 find compiler versions
ifneq ("$(findstring icpx,$(ICPV))", "")
   ICC_VERSION:=$(shell $(ICC) --version | head -1 | cut '-d ' -f5 | cut -b3-4)
else
   ICC_VERSION:=$(shell $(ICC) --version | head -1 | cut '-d ' -f4 | cut -b3-4)
endif
GCC_MAJOR_VERSION=$(shell gcc --version | head -1 |  cut '-d ' -f3 | cut -d '.' -f1)
GCC_MIDDLE_VERSION=$(shell gcc --version | head -1 | cut '-d ' -f3 | cut -d '.' -f2)
ifneq ("$(findstring icpx,$(ICPV))", "")
  INTELOPT:=-axSSE4.2,SSE4.1,SSSE3,AVX,CORE-AVX2,CORE-AVX512 -fp-model precise -qopenmp
else
  INTELOPT:=-axsse4.2,ssse3,AVX,CORE-AVX2 -qopenmp -fp-model source -fast-transcendentals
endif

# set NOCXXSTD if GCC backend is older than GCC 4.8
ifeq ($(GCC_MAJOR_VERSION),4)
ifneq ($(GCC_MIDDLE_VERSION),$(filter $(GCC_MIDDLE_VERSION),8 9))
  NOCXXSTD=1
endif
endif

# omp
ifeq ($(shell echo "$$(( $(ICC_VERSION) > 15 ))"), 1)
  OPENMP_FLAG = -qopenmp
else
  OPENMP_FLAG = -openmp
endif
ifdef AMSPARALLEL
  PARALLEL=$(OPENMP_FLAG) -D__AMSPARALLEL__
else
  PARALLEL=
endif
ifdef AMSP
  PARALLEL+= $(OPENMP_FLAG) 
endif

# set NOCXXSTD if ICC version is older than ICC 14
#ifneq ($(ICC_VERSION),$(filter $(ICC_VERSION),14 15 16 17 18 19 20 21))
ifeq ($(shell echo "$$(( $(ICC_VERSION) < 14 ))"), 1)
  NOCXXSTD=1
endif
ifeq ($(ICC_VERSION),$(filter $(ICC_VERSION),111))
  CXX00=1
endif

ifndef NOCXXSTD
  CXXSTD=-std=c++11
else
ifdef CXX00
  CXXSTD=-std=c++0x
else
  CXXSTD=
endif
endif

########################################################################
# Setup compiler variables
########################################################################

ifneq ("$(findstring icpx,$(ICPV))","")
  CXX=$(ICC) $(INTELOPT) $(CXXSTD) $(PARALLEL)
else
  CXX=$(ICC) $(INTELOPT) $(CXXSTD) -diag-disable cpu-dispatch $(PARALLEL)
endif
CC=$(ICC) $(INTELOPT) $(PARALLEL)
LD=$(ICPC) $(INTELOPT) $(PARALLEL)

########################################################################
# Libraries
########################################################################

#ifeq ($(VERSION),5.34)
ifeq ($(VERSION),$(filter $(VERSION), 5.27 5.34 6.04 6.14))
ifeq ($(shell echo "$$(( $(RH_Version) < 8 ))"), 1)
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-17/lib64
  PNGLIB= -lpng
  TIFFLIB= 
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-4.8.2.el9/lib64
  PNGLIB= -lpng16
  TIFFLIB= -ltiff
endif
endif

########################################################################
# Compilers FLAGS
########################################################################

CFLAGSB= -Wno-deprecated -fPIC
# Disable warnings triggered by ROOT

ifneq ("$(findstring icpx,$(ICPV))","")
CFLAGSB += -Wabi -Wunknown-pragmas -Wno-writable-strings
#-Wno-unknown-pragmas -Werror=unknown-pragmas
else
CFLAGSB += -Wabi-tag

# remark #383: value copied to temporary, reference to temporary used
CFLAGSB += -wd383

# remark #444: destructor for base class "std::iterator<... (declared at line 104 of "/usr/include/c++/4.4.7/bits/stl_iterator_base_types.h") is not virtual
CFLAGSB += -wd444

# remark #1418: external function definition with no prior declaration
CFLAGSB += -wd1418

# remark #522: function "TArrayD::At" redeclared "inline" after being called
CFLAGSB += -wd522

# remark #2259: non-pointer conversion from "int" to "Short_t={short}" may lose significant bits
CFLAGSB += -wd2259

# remark #981: operands are evaluated in unspecified order
CFLAGSB += -wd981

# remark #869: parameter "o" was never referenced
CFLAGBS += -wd869

# remark #193: zero used for undefined preprocessing identifier "_FILE_OFFSET_BITS"
CFLAGSB += -wd193

# Disable warnings triggered by us.

# remark #177: variable "charge" was declared but never referenced
# Similar to Wunused-function in GCC (needed for cfortran.h static functions)
CFLAGSB += -wd177

# remark #174: expression has no effect
CFLAGSB += -wd174

# remark #869: parameter "i" was never referenced
CFLAGSB += -wd869

# remark #185: dynamic initialization in unreachable code
CFLAGSB += -wd185

# remark #310: old-style parameter list (anachronism)
CFLAGSB += -wd310

# remark #271: trailing comma is nonstandard
CFLAGSB += -wd271

# remark #1572: floating-point equality and inequality comparisons are unreliable
CFLAGSB += -wd1572

# remark #111: statement is unreachable
CFLAGSB += -wd111

# remark #181: argument is incompatible with corresponding format string conversion
CFLAGSB += -wd181

# remark #82: storage class is not first
CFLAGSB += -wd82

# remark #424: extra ";" ignored
CFLAGSB += -wd424

# remark #1419: external declaration in primary source file
CFLAGSB += -wd1419

# remark #1599: declaration hides variable "f1" (declared at line 669)
CFLAGSB += -wd1599

# remark #593: variable "S5totL" was set but never used
CFLAGSB += -wd593
endif
# end of ifneq ("$(findstring icpx,$(ICPV))","")

CFLAGS=       $(CFLAGSB)

CPPFLAGS      =  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2
CXXFLAGS      = $(CFLAGS) $(OPT) $(CPPFLAGS)

ifdef SLC6system
  CPPFLAGS += -D__LINUXNEW__
else
  CPPFLAGS+= -D__LINUXGNU__
endif
ifdef VERSION6
  CPPFLAGS += -DVERSION6
endif
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
#ifneq ($(ICC_VERSION),$(filter $(ICC_VERSION),21))
#  LD            = $(INTELVER)/bin/intel64/icpc  $(CFLAGS) -openmp -static-intel -static-libgcc -static-libstdc++
#else
#  LD            = $(INTELDIR)/$(INTELVER)/bin/intel64/icpc  $(CFLAGS) -qopenmp -static-intel -static-libgcc -static-libstdc++
#endif
STATIC=-static-intel -static-libgcc -static-libstdc++  -Bstatic
LDFLAGS       = $(STATIC)
CERNDIRLIB = $(Offline)/CERN/2005/lib
LFORT =  -L$(CERNDIRLIB) -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELDIR)/$(INTELVER)/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s
ifndef PGTRACK
  ifndef SLC6system
    LFORT += -liomp5
  endif
endif

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
    LFORT= -L$(CERNDIR)/lib -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELDIR)/$(INTELVER)/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s
  else
    LFORT= -L$(CERNDIR)/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -L$(INTELDIR)/$(INTELVER)/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s
  endif
else
  LDFLAGS =
  LFORT =  -L$(CERNDIR)/lib -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELDIR)/$(INTELVER)/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s
  ifndef SLC6system
    LFORT += -liomp5
  endif
endif
LNAG = -L$(NAGDIR) -lnag64
LDS     =       $(LD) -static
LDD     =       $(LD) -rdynamic

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMathMore -lgsl -lgslcblas -lMinuit -lTMVA -lASImage $(TIFFLIB) -lXMLIO -lMLP -lTreePlayer  -lGeom -lVMC -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LFORT)
ifdef VERSION614
ROOTLIBS+= -lCling
endif
ifndef NOCASTOR
ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
ROOTLIBS += -lNetx
endif
ROOTLIBSA     = -L$(ROOTLIBDIR) -lRoot $(LZMAL) -pthread  -lAfterImage  -lpcre -lcrypto \
                    $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat  -lncurses \
                -lXpm -lXmu -lXt -lXext -lX11 -ljpeg $(PNGLIB) -ltiff
ROOTLIBSA +=  $(LNAG) $(LFORT)

ifndef NO_NAG
ROOTLIBS += $(LNAG)
ROOTLIBSA += $(LNAG)
endif

LIBS          =  -Bstatic   $(ROOTLIBSA)  $(XRDLIBP) $(XRDLIB) -Bdynamic -lshift -lm    -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  $(XRDLIBP) $(XRDLIB) -lm -Bstatic
LIBSD         =  -L$(AMSWD)/lib/$(ARCH)$(VERSIONP)/ -l$(AMSLIB) $(ROOTLIBS)  $(LIBJPEGFLAG) -ljpeg -lpng -lz  $(XRDLIBP) $(XRDLIB)
ifdef GCCLIB
LIBSA+= -L$(GCCLIB)
LIBS+= -L$(GCCLIB)
LIBSD+=-L$(GCCLIB)
endif

endif
# end of linuxx8664icc

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
