#!/bin/tcsh

setenv ORACLE_HOME /afs/cern.ch/project/oracle/@sys/10205
setenv LD_LIBRARY_PATH $ORACLE_HOME/lib:$LD_LIBRARY_PATH
#export ORBIT2_DEBUG=traces:inproc_traces:timings:types:messages:errors:objects:giop:refs
#setenv ORBIT2_DEBUG traces:inproc_traces:timings:types:messages:errors:objects:giop:refs
#./monitorUI $argv --ORBInitialMsgLimit=2000000 -p/f2users/aegorov2/AMS/perl
../exe/linuxx8664gcc/monitorUI $argv --ORBIIOPIPv4=1 --ORBInitialMsgLimit=2000000 -p/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/perl
