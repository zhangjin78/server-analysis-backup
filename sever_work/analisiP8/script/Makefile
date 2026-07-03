MARCH:=`root-config --arch`
CXX:=`root-config --cxx`

WD=$(shell pwd)

SRC_U:=$(WD)/run_analysis6_make.C #XXX
EXE:=$(WD)/run_analysis6

#----
VERSION6 := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-1)
#----
VERSION  := $(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)

ifeq ($(VERSION),5.27)
VERSIONP=
else
VERSIONP = $(VERSION)
endif

#--AMS software (gBatch)
#AMSWD=${Offline}/vdev
#AMSSRC=${Offline}/vdev

#--C PreProcessor Flags, include dirs
INDIR=/afs/cern.ch/user/c/chenyam/analisi
#CPPFLAGS=-D_PGTRACK_ -DUSEGENFIT -I$(WD)/include -Ilib  -I$(AMSSRC)/include -I$(ROOTSYS)/include -I$(AMSSRC)/GenFit/include
CPPFLAGS=-D_PGTRACK_ -DUSEGENFIT -I$(INDIR) -I$(INDIR)/include -Ilib  -I$(AMSSRC)/include -I$(ROOTSYS)/include -I$(AMSSRC)/GenFit/include

CXXFLAGS=-O3 -no-multibyte-chars -qopenmp -lifcore -lifcoremt $(CPPFLAGS) -D_IONL1PRESCALE_ -D_USELAYERRES_ -static-libstdc++ -std=c++11 \
		 `root-config --libs` -lMinuit -lTMVA -lNetx -lGeom -lMathMore -lEG -lRFIO -L/$(AMSWD)/lib/$(MARCH)$(VERSIONP) \
		 -lntuple_slc6_PG  -L/afs/cern.ch/ams/Offline/CERN/NagLib -lnag64 -L/afs/cern.ch/ams/Offline/CERN/2005/lib -lmathlib

ifeq ($(VERSION6),6)
CXXFLAGS+= -DVERSION6
endif

.PHONY:main
main: static
static: $(EXE)

$(EXE): $(SRC_U)
	$(CXX) $^ $(CXXFLAGS) -o $@ 

clean:
	rm $(EXE)
