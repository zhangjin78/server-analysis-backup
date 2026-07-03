MARCH:=`root-config --arch`5.34
CXX:=`root-config --cxx`
NAGDIR:=/afs/cern.ch/ams/Offline/CERN/NagLib

SRC_U= analyze.C 


OBJS_U=$(SRC_U:%.C=%.o)

CPPFLAGS=-D_PGTRACK_ -I.  -I$(AMSSRC)/include -I$(ROOTSYS)/include
CXXFLAGS= -g -Wno-write-strings  $(CPPFLAGS)

main: static
static: analyze
shared: analyze_shared

analyze:$(SRC_U)
	$(CXX) -o  $@  -static-libstdc++ -static-libgfortran -static-libgcc  $(CXXFLAGS) $^   -L$(AMSWD)/lib/$(MARCH) -lntuple_slc6_PG  `root-config --libs` -lMLP -lTreePlayer -lTMVA -lMLP -lRFIO -lNetx -L$(NAGDIR) -lnaggcc64  -lGeom -lEG -L/afs/cern.ch/ams/Offline/CERN/2005.gcc64.44/lib -lmathlib -lpacklib  -Wl,-Bstatic -lgfortran -lm -lquadmath
analyze_shared:  $(SRC_U) 
	$(CXX) -o $@ $(CXXFLAGS) $^  -L$(AMSWD)/lib/$(MARCH)/ntuple_slc6_PG.so `root-config --libs` 

clean:
	rm -fv *.o
	rm -fv analyze analyze_shared

