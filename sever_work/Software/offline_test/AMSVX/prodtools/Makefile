#
# slc5 build version, no 32-bit support 
#

SHELL=/bin/sh

ifndef AMSDEPLIB
  AMSDEPLIB=/afs/cern.ch/ams/local2/opt
endif

# for build_all only linuxx8664gcc compilation supported
ifndef DEVEL
  DEVEL=$(shell pwd)
  ifndef AMSWD
    AMSWD = $(DEVEL)/..
  endif
  BIN=$(AMSWD)/bin/$(MARCH)/prodtools
  EXE=$(AMSWD)/exe/$(MARCH)
  ifneq ($(MARCH),linuxx8664gcc)
    no_linuxx8664gcc=1
  endif
endif

include Makefile.linuxx8664gcc

OTHER_CCFLAGS = -Wno-unused-function
OTHER_CXXFLAGS = -Wno-unused-function -fpermissive
OTHER_LDFLAGS = -Wl,--allow-multiple-definition -v
ifdef DEBUGFLAG
  OTHER_CCFLAGS += $(_DEBUG)
  OTHER_CXXFLAGS += $(_DEBUG)
  OTHER_LDFLAGS += $(_DEBUG)
else
  OTHER_CCFLAGS += $(_RELEASE)
  OTHER_CXXFLAGS += $(_RELEASE)
  OTHER_LDFLAGS += $(_RELEASE)
endif

QTDEV=$(AMSDEPLIB)/qt-everywhere-opensource-src-4.8.5/_static64
QTLIB=$(QTDEV)/lib/libQtGui.a $(QTDEV)/lib/libQtCore.a $(QTDEV)/lib/libQtNetwork.a -lXt -lXext -lX11 -lXrender -lpng -lfontconfig

ORA=/afs/cern.ch/project/oracle/@sys/10205
ifdef DEBUGFLAG
  # devel ora 11 on local machine, for debugging
  ORALIB:=-L/usr/lib64/oracle/11.2.0.4.0/client/lib64 -lclntsh -locci
else
  # cern ora is here, for production
  ORALIB:=-L$(ORA)/lib -lclntsh -locci 
endif

BDB=$(AMSDEPLIB)/BerkeleyDB.6.2.23
BDBLIB:=$(BDB)/lib/libdb.a $(BDB)/lib/libdb_cxx.a $(BDB)/lib/libdb_stl.a
ifdef BDB_VERSION
  OTHER_CCFLAGS += -DBDB_VERSION=$(BDB_VERSION)
  OTHER_CXXFLAGS += -DBDB_VERSION=$(BDB_VERSION)
else
  OTHER_CCFLAGS += -DBDB_VERSION=0
  OTHER_CXXFLAGS += -DBDB_VERSION=0
endif
BOOSTLIB := /usr/lib64/libboost_system.a /usr/lib64/libboost_filesystem.a

INCLUDE:=$(INCLUDE) -I$(DEVEL)/include -I$(AMSWD)/include -I$(QTDEV)/include -I$(ORA)/rdbms/public -I$(BDB)/include
LIBS:=$(LIBS) $(ORALIB) $(BDBLIB)

MONITOR = $(BIN)/moc_mainwindow.o $(BIN)/moc_warning.o $(BIN)/moc_viewer.o  $(BIN)/moc_filter.o $(BIN)/moc_editcolumn.o $(BIN)/moc_selfile.o \
          $(BIN)/main.o $(BIN)/srviface.o $(BIN)/uiwrapper.o $(BIN)/shmutils.o $(BIN)/qrc_application.o \
          $(idl_lib)
DBSERVER = $(BIN)/dbserver.o $(BIN)/poad.o $(idl_lib)
DBVIEWER = $(BIN)/moc_mainw_dbvw.o $(BIN)/dbviewer.o $(BIN)/uiwrapper_dbvw.o $(idl_lib)

vpath %.cpp ./monitorUI ./dbView
vpath %.ui  ./monitorUI/rc ./dbView/rc
vpath %.cxx ./dbServer
vpath %.cpp $(BIN)

ifndef no_linuxx8664gcc
all: $(EXE)/dbServer$(BDB_VERSION) $(EXE)/monitorUI $(EXE)/dbqView 
#all: $(EXE)/dbServer$(BDB_VERSION)
else
all:
	@echo "only linuxx8664gcc compilation supported for prodtools, nothing can be done here"
endif

$(EXE)/monitorUI: $(BIN)/Make-dep $(MONITOR)
		@echo Linking target $@
		$(LINK_CMD) -o $(EXE)/monitorUI $(MONITOR) $(LIBS) $(QTLIB) $(BOOSTLIB) -lrt

$(EXE)/dbServer$(BDB_VERSION): $(BIN)/Make-dep $(DBSERVER)
		@echo "$$"
		@echo Making target $@
		$(LINK_CMD) -o $(EXE)/dbServer$(BDB_VERSION) $(DBSERVER) $(LIBS) $(BOOSTLIB) -lrt
		  
$(EXE)/dbqView: $(BIN)/Make-dep $(DBVIEWER)
		@echo "$$"
		@echo Making target $@
		$(LINK_CMD) -o $(EXE)/dbqView $(DBVIEWER) $(LIBS) $(QTLIB) $(BOOSTLIB) -lrt

$(BIN)/Make-dep: init
  ifneq "$(shell du -a  $(BIN)/Make-dep | grep -c  ake)" "1"
	touch $(BIN)/Make-dep
	@echo Runing makedepend
	makedepend ./dbServer/*.cxx ./monitorUI/*.cpp ./dbView/*.cpp -I./include -Y$(BIN) -p$(BIN)/ -f $(BIN)/Make-dep
	@cat $(BIN)/Make-dep | sed -e 's#'$(BIN)'/./monitorUI#'$(BIN)'#g' | sed -e 's#'$(BIN)'/./dbServer#'$(BIN)'#g' | sed -e 's#'$(BIN)'/./dbView#'$(BIN)'#g' > $(BIN)/Make-dep.new
	@rm -f $(BIN)/Make-dep
	@mv $(BIN)/Make-dep.new $(BIN)/Make-dep
  endif	 
  include $(BIN)/Make-dep

init:
	@if ! [ -f $(BIN)/server-cpp.h ]; then \
		mkdir -p $(BIN); \
		cd $(BIN); \
		echo Generating orbit code in $(BIN); \
		$(ORBIT_IDL) -l c $(AMSWD)/include/server.idl; \
		$(ORBIT_CPP) -l cpp $(AMSWD)/include/server.idl; \
		echo Generating qt code in $(BIN); \
		$(QTDEV)/bin/moc -o$(BIN)/moc_mainwindow.cpp $(DEVEL)/monitorUI/mainwindow.h; \
		$(QTDEV)/bin/moc -o$(BIN)/moc_warning.cpp $(DEVEL)/monitorUI/warning.h; \
		$(QTDEV)/bin/moc -o$(BIN)/moc_viewer.cpp $(DEVEL)/monitorUI/viewer.h; \
		$(QTDEV)/bin/moc -o$(BIN)/moc_filter.cpp $(DEVEL)/monitorUI/filter.h; \
		$(QTDEV)/bin/moc -o$(BIN)/moc_editcolumn.cpp $(DEVEL)/monitorUI/editcolumn.h; \
		$(QTDEV)/bin/moc -o$(BIN)/moc_selfile.cpp $(DEVEL)/monitorUI/selfile.h; \
		$(QTDEV)/bin/moc -o$(BIN)/moc_mainw_dbvw.cpp $(DEVEL)/dbView/mainw_dbvw.h; \
		cd $(DEVEL); \
	fi
	@mkdir -p $(EXE);

ifndef no_linuxx8664gcc
clean:
	@rm -rf $(BIN)
else
clean:
	@echo "only linuxx8664gcc compilation supported for prodtools, nothing can be done here"
endif

$(BIN)/moc_mainwindow.cpp: ./monitorUI/mainwindow.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_mainwindow.cpp $(DEVEL)/monitorUI/mainwindow.h
$(BIN)/moc_warning.cpp: ./monitorUI/warning.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_warning.cpp $(DEVEL)/monitorUI/warning.h
$(BIN)/moc_viewer.cpp: ./monitorUI/viewer.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_viewer.cpp $(DEVEL)/monitorUI/viewer.h
$(BIN)/moc_filter.cpp: ./monitorUI/filter.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_filter.cpp $(DEVEL)/monitorUI/filter.h
$(BIN)/moc_editcolumn.cpp: ./monitorUI/editcolumn.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_editcolumn.cpp $(DEVEL)/monitorUI/editcolumn.h
$(BIN)/moc_selfile.cpp: ./monitorUI/selfile.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_selfile.cpp $(DEVEL)/monitorUI/selfile.h
$(BIN)/moc_mainw_dbvw.cpp: ./dbView/mainw_dbvw.h
	$(QTDEV)/bin/moc -o$(BIN)/moc_mainw_dbvw.cpp $(DEVEL)/dbView/mainw_dbvw.h

ifdef RUNUIC
#./monitorUI/mainwindow.h: ./monitorUI/rc/mainwindow.ui 
#	$(QTDEV)/bin/uic -p -o ./monitorUI/mainwindow.h ./monitorUI/rc/mainwindow.ui
#./monitorUI/warning.h: ./monitorUI/rc/warning.ui 
#	$(QTDEV)/bin/uic -p -o ./monitorUI/warning.h ./monitorUI/rc/warning.ui
#./monitorUI/viewer.h: ./monitorUI/rc/viewer.ui 
#	$(QTDEV)/bin/uic -p -o ./monitorUI/viewer.h ./monitorUI/rc/viewer.ui
#./monitorUI/filter.h: ./monitorUI/rc/filter.ui 
#	$(QTDEV)/bin/uic -p -o ./monitorUI/filter.h ./monitorUI/rc/filter.ui
#./monitorUI/editcolumn.h: ./monitorUI/rc/editcolumn.ui 
#	$(QTDEV)/bin/uic -p -o ./monitorUI/editcolumn.h ./monitorUI/rc/editcolumn.ui
#./monitorUI/selfile.h: ./monitorUI/rc/selfile.ui 
#	$(QTDEV)/bin/uic -p -o ./monitorUI/selfile.h ./monitorUI/rc/selfile.ui
#
#./dbView/mainw_dbvw.h: ./dbView/rc/mainw_dbvw.ui 
#	$(QTDEV)/bin/uic -p -o ./dbView/mainw_dbvw.h ./dbView/rc/mainw_dbvw.ui
endif
