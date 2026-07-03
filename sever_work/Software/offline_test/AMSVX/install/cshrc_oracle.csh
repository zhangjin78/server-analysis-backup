#
# Script to set up the users environment to use the Oracle tools
#
# Version for csh  
#
#
if ( $?ORACLE_CERN == 0 ) then
        echo "cshrc_oracle.csh: Error: ORACLE_CERN not set."
        echo "             Please contact ORACLE Support"
else 
  if ( -d $ORACLE_CERN/@sys ) then
   setenv ORACLE_MOUNT $ORACLE_CERN/@sys
#
# By default we set it to 'prod', we admit as input parameter 'test' or 
# 'prod'
#
if ( $#argv == 0  ) then
   setenv ORACLE_HOME $ORACLE_MOUNT/prod
 else
   setenv ORACLE_HOME $ORACLE_MOUNT/prod
   if ( "$1" == "test" ) then 
     setenv ORACLE_HOME $ORACLE_MOUNT/test
   else if ( "$1" == "prod" ) then 
     setenv ORACLE_HOME $ORACLE_MOUNT/prod
   else if ( "$1" == "old" ) then 
     setenv ORACLE_HOME $ORACLE_MOUNT/10205
   else if ( "$1" != "" ) then 
     setenv ORACLE_HOME $ORACLE_MOUNT/$1
   endif
 endif

#if ( $#argv == 0  ) then
#  setenv ORACLE_HOME $ORACLE_MOUNT/prod
# else
#  if ( -r $ORACLE_MOUNT/$1 ) then
#    setenv ORACLE_HOME $ORACLE_MOUNT/$1
#  else
#    echo 'Can not find Oracle distribution ' "$ORACLE_HOME/$1"
#    echo 'Please check that the value you entered is valid'
#endif

setenv TNS_ADMIN $ORACLE_CERN/admin

#
# We add the script directory to the PATH, the script directory contains
# shell scripts
#
set oracle_cern=$ORACLE_CERN
echo $PATH | grep $oracle_cern/script >! /dev/null
if ( $status ) \
        setenv PATH `echo $PATH | sed 's/\(.*\):$/\1/'`:$ORACLE_CERN/script
 
#
# We add the generic sql directory centrally maintained which contains
# useful .sql scripts for the users
#

if ($?ORACLE_PATH == 0) then
   setenv ORACLE_PATH ${HOME}:${ORACLE_CERN}/sql:.
else
   echo $ORACLE_PATH | grep $oracle_cern/sql > /dev/null
   if ( $status ) then
     setenv ORACLE_PATH ${ORACLE_PATH}:${HOME}:${ORACLE_CERN}/sql:.
   endif
endif
setenv ORACLE_LPPROG xprint

source $ORACLE_CERN/script/caliases

#
# Same settings in Nice/Unix. To solve also problem with 8.x clients the
# get century instead of year (20 vs 2000)
#
setenv NLS_DATE_FORMAT 'DD-MON-FXYYYY'

#
# We set those variables that depend on the value of ORACLE_HOME (ex 
# LD_LIBRARY_PATH)
#
source $ORACLE_CERN/script/oracle_Cenv.csh
#
# This else/endif comes from the test of ORACLE_MOUNT as a valid oracle platform
# we just skip the setting in that case instead of exitting. This caused
# some disturbances to the HepIx scripts.
 else
  echo "cshrc_oracle.csh : Error: Invalid value for ORACLE_MOUNT."
  echo "                          Please contact Oracle Support"
 endif
# endif to close the initial test for ORACLE_CERN
endif 
