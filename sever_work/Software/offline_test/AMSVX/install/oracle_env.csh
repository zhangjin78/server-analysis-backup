#+################################################################################
# File:            oracle_env.csh
# Description:     Call the environment startup scripts for Oracle.
# Authors:         A. Taddei (CERN/IT/DIS)
# Site Contact:    Setup Support <Setup.Support@cern.ch>
# Version:         3.7
# Last Change:     Wed Dec 19 12:20:57 2001
#-################################################################################


setenv ORACLE_CERN /afs/cern.ch/project/oracle

set version=prod
set OSVER=`cat /etc/redhat-release`
if ("$OSVER" =~ "Scientific Linux CERN SLC release 5*") then
    set version=prod
else if ("$OSVER" =~ "Scientific Linux CERN SLC release 6*") then
    set version=prod
else if  ("$OSVER" =~ "CentOS Linux release 7*") then
    set version=19300
endif

if ( -r $ORACLE_CERN/script/cshrc_oracle.csh ) then
    source $Offline/vdev/install/cshrc_oracle.csh $version
endif

