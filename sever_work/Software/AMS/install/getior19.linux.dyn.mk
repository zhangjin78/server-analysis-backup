ORACLE_HOME=/afs/cern.ch/project/oracle/linux64/19090
include $(ORACLE_HOME)/precomp/lib/env_precomp.mk
#
PCINCLUDE=/usr/include/c++/4.1.1/backward,/usr/include/c++/4.1.1
OBJS=getior.dyn.o
EXE=../exe/linux/getior.dyn.exe
BIN=../bin/linux/getior.dyn.c
PCCSRC=../CC/getior.pc

cppbuild: 
	$(PROC) $(PROCPPFLAGS) INCLUDE=$(PCINCLUDE) iname=$(PCCSRC) oname=$(BIN)
	$(CPLUSPLUS) -v -w -g -c -fpermissive -I/usr/include  -I../include  -I/usr/local/include/g++  $(INCLUDE) $(BIN) -I/afs/cern.ch/user/b/bshan/work/backward
	$(CPLUSPLUS) -v -dynamic -g -w -o $(EXE) $(OBJS) -L$(LIBHOME)  $(STATICCPPLDLIBS) -pthread # -L$(LIBHOME)/stubs 

$(CPPSAMPLES): cppdemo2
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ cppbuild
#
# The macro definition fill in some details or override some defaults from 
# other files.
#
OTTFLAGS=$(PCCFLAGS)
CLIBS= $(TTLIBS_QA) $(STATICCPPLDLIBS)
PRODUCT_LIBHOME=
MAKEFILE=./getior.linux.mk
PROCPLSFLAGS= sqlcheck=full userid=$(USERID) 
PROCPPFLAGS= code=cpp $(CPLUS_SYS_INCLUDE)
USERID=scott/tiger
NETWORKHOME=$(ORACLE_HOME)/network/
PLSQLHOME=$(ORACLE_HOME)/plsql/
INCLUDE_PC= $(I_SYM)$(PRECOMPHOME)syshdr $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
INCLUDE= $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
