#!/usr/bin/env tcsh
#Building tool for AMS analysis executable using makefile, created by Qi Yan

setenv SRC "$1"
setenv SRC `echo "${SRC}" | sed 's/\.C$//'`
setenv SRC `echo "${SRC}" | sed 's/\.exe$//'`

setenv EXEAP ""
setenv FLAGAP ""
setenv CLEAN ""
foreach arg ("$2" "$3" "$4")
  if ("${arg}" == "clean") then
    setenv CLEAN "${arg}"
  else if ("${arg}" =~ "*-D*") then
    setenv FLAGAP "${FLAGAP} ${arg}"
  else
    setenv EXEAP "${EXEAP}${arg}"
  endif
end


if (! $?AMSWD) then
    setenv AMSWD ../..
endif
source $AMSWD/install/amsroot.csh


make -f Makefile534_2SL6 SRC_U="${SRC}" EXEAP_U="${EXEAP}" FLAGAP_U="${FLAGAP}" ${CLEAN}
