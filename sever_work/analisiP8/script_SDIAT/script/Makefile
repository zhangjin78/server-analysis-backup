all:main_run_analysis6 

MARCH:=`root-config --arch`
CXX:=`root-config --cxx`

#AMSDir:=/cvmfs/ams.cern.ch/Offline/vdev
AMSDir:=$(AMSSRC)
XRDLIB:=/cvmfs/ams.cern.ch/opt/xrootd-gcc64-44

SRC_U:=$(wildcard *.C) 
OBJS_U:=$(basename $(SRC_U) )

CPPFLAGS=-D_PGTRACK_  -isystem$(AMSDir)/include -isystem$(ROOTSYS)/include -isystem./src -I.
CXXFLAGS= -ggdb -fopenmp $(CPPFLAGS)  -lshift -lgfortran 

EXTRALIBS += `root-config --libs` -lc 

%:%.C  main_run_analysis6.C 
	$(CXX) -fPIC -w -g -o $@  $^  -L./src/ -L$(AMSDir)/lib/$(MARCH)6.22 -lntuple_slc6_PG_dynamic $(CPPFLAGS) $(EXTRALIBS)  

clean:
	@rm -rvf main_run_analysis6 
