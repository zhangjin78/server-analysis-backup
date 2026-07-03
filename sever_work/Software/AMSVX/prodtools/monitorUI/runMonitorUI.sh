#!/bin/tcsh

setenv ORACLE_HOME /afs/cern.ch/project/oracle/@sys/10205
setenv LD_LIBRARY_PATH $ORACLE_HOME/lib:$LD_LIBRARY_PATH

# orbit debug mode, very special 
#setenv ORBIT2_DEBUG traces:inproc_traces:timings:types:messages:errors:objects:giop:refs

#./monitorUI $argv --ORBIIOPIPv4=1 --ORBInitialMsgLimit=2000000 -p/f2users/aegorov2/tmp/AMS/perl
#pwd
set K5REAUTH = ""
set me = `whoami`
if ($me == "ams") then
	set K5REAUTH = "/usr/bin/k5reauth -p ams -k $HOME/ams.keytab -- "
endif
$K5REAUTH ../exe/linuxx8664gcc/monitorUI $argv --ORBIIOPIPv4=1 --ORBIIOPIPv6=0 --ORBInitialMsgLimit=100000000 -p/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/perl
