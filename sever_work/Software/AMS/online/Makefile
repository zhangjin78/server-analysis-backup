#  $Id: Makefile,v 1.94 2024/05/30 07:06:28 choutko Exp $
# PZ reviewing the online  Makefile on Aug 2009
#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc

#    WIN32 ans MACOSX arch commented as not supported in GBATCH for the moment
#    LINUX arch supported is SLC4 with gcc or  icc

# List of SLC6 changes [ae]-icc21.el9
#
ARCH         := $(shell $(ROOTSYS)/bin/root-config --arch )
VERSION      := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ROOTLIBDIR   := $(shell $(ROOTSYS)/bin/root-config --libdir)
ROOTINCDIR   := $(shell $(ROOTSYS)/bin/root-config --incdir)

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
  # ifeq ($(RH_Version), $(filter $(RH_Version), 6 7 8 9))
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
ifndef AMSDEPLIB
  AMSDEPLIB := /cvmfs/ams.cern.ch/opt/
endif
ifdef SLC6system
AMSLIBSUFFIX=slc6
else
AMSLIBSUFFIX=slc4
endif
#
#[ae-2012.08.02] - some static libs we need take from slc5, initially we have there only those ones: 
#    libcrypt.a  libexpat.a  libfontconfig.a  libshift.a     
#
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

XRDLIBP        := 
PLATFORM      =  Linux

VERSIONP=$(VERSION)_$(RH_Version)


PREFIX        = $(AMSWD)/bin/$(ARCH)$(VERSIONP)/online/
DEPDIR        = $(PREFIX)/dep

AMSLIB        = ntuple_$(AMSLIBSUFFIX)
AMSPRG        = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmon
AMSPRGC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmonc
SLC4=y
ifdef PGTRACK
PREFIX        = $(AMSWD)/bin/$(ARCH)$(VERSIONP)/online/PG/
AMSLIB        = ntuple_$(AMSLIBSUFFIX)_PG
AMSPRG        = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmonPG
AMSPRGC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmoncPG
AMSPRGCC       = $(AMSWD)/exe/$(ARCH)$(VERSIONP)/offmondPG
else
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

# display compiles only with customized ROOT, at the moment v5.21
NEW         :=_new

VER          := $(shell $(ROOTSYS)/bin/root-config --version )

ifndef NAGDIR
  NAGDIR  = $(Offline)/CERN/NagLib
endif

ifeq ($(ARCH),linuxicc)

CXX           =  $(AMSDEPLIB)/intel/Compiler/11.1/073/bin/ia32/icc 
CPPFLAGS      =   -m32  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -I. \
		 -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2   
CXXFLAGS      = -Wno-deprecated -openmp -axssse3 $(OPT) $(CPPFLAGS)
OPT           = -O3
OPTC          = -O3
NOOPT         = -g
NOOPTC        = -g
LD            = $(AMSDEPLIB)/intel/Compiler/11.1/073/bin/ia32/icpc -m32 -axssse3 -openmp

LDFLAGS       = 
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
ifdef PGTRACK
LFORT = -L$(Offline)/AMSsoft.32/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L$(AMSDEPLIB)/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -liomp5 
else
LFORT = -L$(Offline)/AMSsoft.32/linux_slc5_icc32/2005/lib -lgeant321 -lpacklib -lmathlib -lkernlib  -L$(AMSDEPLIB)/intel/Compiler/11.1/073/lib/ia32 -lifport -lifcoremt -limf  -lipgo -lirc_s -liomp5 
endif
LNAG = -L$(NAGDIR) -lnagifort32

#ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF $(LNAG) $(LFORT)

ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lTMVA -lASImage -lXMLIO -lMLP -lTreePlayer -lGeom -lVMC  -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LFORT)


ifndef NOCASTOR
	ROOTLIBS += -lRCastor
endif
ifndef NOXROOTD
	ROOTLIBS     += -lNetx
endif



ROOTLIBSA     =  -L$(ROOTLIBDIR) -lRoot -pthread  -lAfterImage $(ROOTLIBDIR)/libXrdClient.a -lpcre -lncurses \
	             -L$(IMPORTS_X11R6)/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                 -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet -lXrdNetUtil -lXrdOuc -lXrdSys $(LNAG) $(LFORT)

LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =  $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm -Bdynamic -ldl 

endif
# end of linuicc

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
  INTELOPT:=-axSSE4.2,SSE4.1,SSSE3,AVX,CORE-AVX2,CORE-AVX512 -fp-model precise
else
  INTELOPT:=-axsse4.2,ssse3,AVX,CORE-AVX2 -fp-model source -fast-transcendentals
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
  PARALLEL+= $(OPENMP_FLAG) -D__AMSP__
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
ifeq ($(VERSION),$(filter $(VERSION), 5.27 5.34))
ifeq ($(shell echo "$$(( $(RH_Version) < 8 ))"), 1)
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-icc64-17/lib64
  PNGLIB= -lpng
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-4.8.2.el9/lib64
  PNGLIB= -lpng16
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
LFORT =  -L$(CERNDIRLIB) -lgeant321  -lminuit -lpacklib -lmathlib -lkernlib  -L$(INTELVER)/lib/intel64 -lifport -lifcoremt -limf  -lipgo -lirc_s
ifndef PGTRACK
  ifndef SLC6system
    LFORT += -liomp5
  endif
endif
LNAG = -L$(NAGDIR) -lnag64
LDS	= 	$(LD) -Bstatic
LDD	= 	$(LD) -Bdynamic

#ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lHistPainter -lMinuit -lGX11 -lGX11TTF $(LNAG) $(LFORT)
ROOTLIBS      =  $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lTMVA -lASImage -lHistPainter -lXMLIO -lMLP -lTreePlayer -lGeom -lVMC  -lEG -lGX11 -lGX11TTF $(CASTORLIB) $(LNAG) $(LFORT)


ifndef NOCASTOR
	        ROOTLIBS += -lRCastor
	endif
	ifndef NOXROOTD
	        ROOTLIBS     += -lNetx
	endif



ROOTLIBSA     = -L$(ROOTLIBDIR) -lRoot $(LZMAL) -pthread  -lAfterImage  -lpcre -lcrypto \
	            $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat  -lncurses \
                -lXpm -lXmu -lXt -lXext -lX11 -ljpeg $(PNGLIB) -ltiff   
ROOTLIBSA +=  $(LNAG) $(LFORT)

LIBS          =  -Bstatic   $(ROOTLIBSA)  $(XRDLIBP) $(XRDLIB) -Bdynamic -lshift -lm    -ldl 
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  $(XRDLIBP) $(XRDLIB) -lm -Bstatic -ldl 

LIBSD          =   -Bdynamic  $(ROOTLIBS)  $(XRDLIBP) $(XRDLIB)   -lm    -ldl



endif
# end of linuxx8664icc

ifeq ($(ARCH),linux)

CXX           =  g++ -m32 
CPPFLAGS      =  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2 
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -g
OPTC          = -g
NOOPT         = -g
LD            = g++ -m32
LDFLAGS       = -g 
LDS	= 	$(LD) -static
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
ifdef PGTRACK
LFORT =  -L$(Offline)/AMSsoft.32/linux_slc5_gcc32/2005/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -lgfortran
else
LFORT =  -L$(Offline)/AMSsoft.32/linux_slc5_gcc32/2005/lib -lgeant321  -lpacklib -lmathlib -lkernlib  -lgfortran
endif

LNAG = -L$(NAGDIR) -lnaggfortran32
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lGX11 -lHistPainter -lMinuit  -lGX11TTF $(LNAG) $(LFORT)
ROOTLIBSA     =  -L$(ROOTLIBDIR) -lRoot -pthread  -lAfterImage $(ROOTLIBDIR)/libXrdClient.a -lpcre -lncurses \
	             -L$(IMPORTS_X11R6)/lib -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                 -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   -lXrd -lXrdNet  -lXrdNetUtil -lXrdOuc -lXrdSys $(LNAG) $(LFORT)


LIBS          =  -Bstatic   $(ROOTLIBSA)  -Bdynamic -lshift -lm    -ldl
LIBSA         =   $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   -Bdynamic          -ldl 

endif
ifeq ($(ARCH),linuxx8664gcc)
ifeq ($(VERSION),5.34)
ifndef SLC6system
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-41/lib64
else
  XRDLIBP += -Bdynamic -L$(AMSDEPLIB)/xrootd-gcc64-44/lib64
endif
endif

CXX           =  g++
CPPFLAGS      =  -D__root_$(NEW) -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I. -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2 
ifdef SLC6system
  CPPFLAGS += -D__LINUXNEW__
endif
CXXFLAGS      = -Wno-deprecated $(OPT) $(CPPFLAGS)
OPT           = -O2
OPTC          = -O0
NOOPT         = -g
LD            = g++ 
LDFLAGS       = -g 
ifdef PGTRACK
  ifndef SLC6system
    CERNDIRLIB = $(Offline)/CERN/2005.gcc64/lib
  else  
    CERNDIRLIB = $(Offline)/CERN/2005.gcc64.44/lib
  endif  
  LFORT = -L$(CERNDIRLIB) -lgeant321 -lpacklib -lmathlib -lkernlib  -lgfortran
else
  ifndef SLC6system
    CERNDIRLIB = $(Offline)/CERN/2005.gcc64/lib
  else  
    CERNDIRLIB = $(Offline)/CERN/2005.gcc64.44/lib
  endif  
  LFORT = -L$(CERNDIRLIB) -lgeant321 -lpacklib -lmathlib -lkernlib  -lgfortran
endif
LDS	= 	$(LD) -static
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic

LNAG = -L$(NAGDIR) -lnaggcc64
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --glibs) -lASImage  -lNetx -lGX11 -lHistPainter -lMinuit  -lGX11TTF $(LNAG) $(LFORT)
ROOTLIBSA     = -L$(ROOTLIBDIR) -lRoot $(LZMAL) -pthread  -lAfterImage  -lpcre -lcrypto  -lncurses \
	            $(IMPORTS_SLC5_LIBS) -L$(IMPORTS_X11R6)/lib64 -lXft -lfontconfig -lXrender -lfreetype  -lexpat   \
                -lXpm -lXmu -lXt -lXext -lX11 -ljpeg -lpng -ltiff   $(XRDLIB) $(XRDLIBP) $(LNAG) $(LFORT)

LIBS          = -Bstatic   $(ROOTLIBSA)   $(XRDLIBP) $(XRDLIB) -Bdynamic -lshift -lm    -ldl
LIBSA         = $(ROOTLIBSA) -Wl,-whole-archive,--allow-multiple-definition -lshift -Wl,-no-whole-archive  -lm   $(XRDLIBP) $(XRDLIB)  -Bdynamic          -ldl 

endif


#ifeq ($(ARCH),macosx)
#CXX           =  g++  -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2 
#OPTC          = -O0
#NOOPT         = -g
#LD            = g++
#ROOTLIBS      =  -L$(ROOTLIBDIR) -lRoot -lfreetype -lcrypto -L/usr/X11R6/lib  -lXpm -lXmu -lXt -lXext -lX11  

#LIBS          =  $(ROOTLIBS) $(RH9add) -lm -ldl
#LIBSA          =  $(ROOTLIBS) $(RH9adda) -lm -ldl
#endif
#ifeq ($(ARCH),win32gcc)
#PREFIX        = $(AMSWD)/online/cygwin/
#CXX           =  g++ -Wno-deprecated
#CXXFLAGS      =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -I../include -I$(ROOTINCDIR) -I/usr/include/freetype2 
#OPT           = -O2
#OPTC          = -O0
#NOOPT         = -g
#NOOPTC         = -g
#LD            = g++
#LDFLAGS       = -g 
#ROOTLIBS      =  -L$(ROOTLIBDIR) -lRoot -lfreetype -lcrypt -lcrypto -L/usr/X11R6/lib  -lXpm.dll -lX11.dll  -L/usr/lib -lshift
#LIBS          =  $(ROOTLIBS) $(RH9add) -lm 
#LIBSA          =  $(ROOTLIBS) $(RH9adda) -lm
#endif
#ifeq ($(ARCH),win32)
#PREFIX   = $(AMSWD)/online/wingdk/
#CXX        =  $(ROOTSYS)/build/win/cxx.sh
#CXXFLAGS =   -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -O2 -Z7 -G5 -GR -GX -MD -DWIN32 -DGDK_WIN32  -D_WINDOWS -DWINVER=0x0400 -nologo -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -FIw32pragma.h -D_X86_=1 -D_DLL -DVISUAL_CPLUSPLUS -I../include  -I$(ROOTINCDIR)  /TP
#OPT           = 
#OPTC          = 
#NOOPT         = -g
#NOOPTC         = -g
#LD = $(ROOTSYS)/build/win/ld.sh
#LDFLAGS       =  -nologo    -nodefaultlib:libc.lib -nodefaultlib:dfor.lib 
#LIBS          =  $(ROOTLIBDIR)/libCore.lib $(ROOTLIBDIR)/libCint.lib $(ROOTLIBDIR)/libHist.lib $(ROOTLIBDIR)/libHistPainter.lib $(ROOTLIBDIR)/libGui.lib  $(ROOTLIBDIR)/libMatrix.lib $(ROOTLIBDIR)/libGeom.lib $(ROOTLIBDIR)/libPhysics.lib $(ROOTLIBDIR)/libGpad.lib $(ROOTLIBDIR)/libGraf.lib $(ROOTLIBDIR)/libGraf3d.lib $(ROOTLIBDIR)/libTree.lib  $(ROOTLIBDIR)/libRint.lib advapi32.lib
#ALIBS          = lib/libroot.a lib/libCore.lib lib/libfreetype.lib advapi32.lib
#endif

ifdef CASTOSTATIC
CPPFLAGS+= -DCASTORSTATIC
ROOTLIBS+=-lRFIO
endif
ifdef PGTRACK
PG := PG
CPPFLAGS += -D_PGTRACK_
else
PG :=
endif

##### MACROS #####



SRCS          = AMSDisplay.cxx     AMSHist.cxx       AMSAntiHist.cxx \
                AMSTrackerHist.cxx AMSRICHHist.cxx   AMSECALHist.cxx \
                AMSTRDHist.cxx     AMSTOFHist.cxx    AMSLVL1Hist.cxx \
                AMSLVL3Hist.cxx    AMSAxAMSHist.cxx  AMSGenHist.cxx \
                AMSNtuple.cxx      ControlFrame.cxx  AMSEverything$(PG).cxx

HDRS          = AMSDisplay.h       AMSHist.h         AMSAntiHist.h \
		AMSTrackerHist.h   AMSRICHHist.h     AMSECALHist.h \
                AMSTRDHist.h       AMSTOFHist.h      AMSLVL1Hist.h \
                AMSLVL3Hist.h      AMSAxAMSHist.h    AMSGenHist.h  \
                AMSNtuple.h        ControlFrame.h

DEPS=  $(SRCS:%.cxx=$(DEPDIR)/%.d)

DICT          = $(PREFIX)/AMSCint_$(ARCH)_$(SLC4)$(NEW).cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(DICT:$(PREFIX)/%.cxx=$(PREFIX)/%.o)


MAIN          = main.cxx
MAINH         = $(MAIN:.cxx=.h)
MAINO         = $(MAIN:%.cxx=$(PREFIX)%.o)
DEPS+=  $(MAIN:%.cxx=$(DEPDIR)/%.d)

MAINC          = mainc.cxx
MAINOC         = $(MAINC:.cxx=.o)
#DEPS+=  $(MAINC:%.cxx=$(DEPDIR)/%.d)

OBJS          = $(SRCS:%.cxx=$(PREFIX)%.o) 


ALLDICT       = $(DICT) $(DICTH) 8
ALLSRCS       = $(SRCS  $(ALLDICT) $(MAIN) $(MAINH) $(MAINC)
ALLOBJS       = $(OBJS) $(MAINO) 





#ifeq ($(ARCH),win32)
#AMSPRG       = $(PREFIX)offmon.exe
#endif
LDS	= 	$(LD) -static
LDD	= 	$(LD) -rdynamic
#ifeq ($(ARCH),macosx)
#AMSPRG       = nothing
#LDS =  @echo "  Static Linking not supported in macosx!!! Use 'make osdepend' instead"
#LDD	= 	$(LD) -dynamic
#endif



##### TARGETS #####
standalone:  amslib $(AMSPRG) 
osdepend:  amslib $(AMSPRGC)
dynamic:   amslib $(AMSPRGCC)

amslib: $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
$(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a:
	$(MAKE) -C ../install lib

  



$(AMSPRG):        $(OBJS)  $(DICTO) $(MAINO) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a
		@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
		@echo Linking executable $(AMSPRG)
		$(LDS)   $(LDFLAGS) $^  $(LIBSA) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a -o $(@) 
		@echo "$(AMSPRG) done"


$(AMSPRGC):       $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)
	@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
	@echo Linking executable $(AMSPRGC)
	$(LDD)  $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)   $(LIBS) $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a  $(LIBS) -o $(AMSPRGC)

$(AMSPRGCC):       $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)
	@if ! [ -d $(AMSWD)/exe/$(ARCH)$(VERSIONP) ] ; then mkdir -p $(AMSWD)/exe/$(ARCH)$(VERSIONP); fi
	@echo Linking executable $(AMSPRGCC)
	$(LDD)  $(LDFLAGS) $(ALLOBJS) $(DICTO) $(OBJSR) $(OBJSC)   $(AMSWD)/lib/$(ARCH)$(VERSIONP)/lib$(AMSLIB).a    $(LIBSD) -o $(AMSPRGCC)


$(DICT):        $(HDRS) LinkDef.h
		@echo "Generating dictionary ..."
		$(ROOTSYS)/bin/rootcint -f $(DICT) -c $(CPPFLAGS) $(ROOTH) $(HDRS) LinkDef.h





../include/root_methods$(P$(IMPORTS_SLC5_LIBS) G).h:      ../perl/root.perl
	perl ../perl/root.perl

AMSEverything$(PG).cxx:      ../perl/online.perl ../include/root_methods$(PG).h
	perl ../perl/online.perl

dist:
		rm -f $(AMSPRG).tar.gz ; \
		tar cvf $(AMSPRG).tar $(ALLSRCS) 00index.txt Make* ; \
		gzip $(AMSPRG).tar

clean:
		@rm -vf $(ALLOBJS) $(ALLDICT) $(DEPS) $(AMSPRG) $(AMSPRGC) AMSEverything$(PG).cxx



##### RULES #####


$(DEPDIR)/%.d : %.cxx
	@echo "Generating Dep file $@ ..."
	@if ! [ -d $(DEPDIR) ] ; then mkdir -p $(DEPDIR); fi
	@$(CXX) $(CPPFLAGS) -MM $< -MT $(OBJ)/$*.o -MF $@


$(PREFIX)%.o : $(PREFIX)%.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)

$(PREFIX)%.o : %.cxx 
	$(CXX) $(OPT) $(CXXFLAGS) -c $(<)  -o $(@)




##### DEPENDENCIES #####

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

