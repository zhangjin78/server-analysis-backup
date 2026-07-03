#!/bin/csh
source $AMSWD/install/amsroot.csh

g++  -I/$ROOTSYS/include   -I$AMSWD/include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__  -O3 $1.C -c -o $1.o
g++  -static-libstdc++ -static-libgfortran -static-libgcc  -O $1.o -Wl,-Bstatic -L$AMSWD/lib/linuxx8664gcc5.34 -lntuple_slc6_PG    -L$ROOTSYS/lib -lRoot -L/afs/cern.ch/ams/Offline/CERN/NagLib -lnaggcc64 -L/afs/cern.ch/ams/Offline/CERN/2005.gcc64.44/lib -lmathlib -lpacklib   -pthread   -lpcre  -L/afs/cern.ch/ams/local2/opt/lib64 -lcrypto  -llzma -lfreetype -Wl,-Bdynamic -L$XRDLIB/lib64 -lXrdClient  -lXrdUtils -Wl,--allow-multiple-definition   -Wl,-Bstatic  -lshift -lgfortran -lquadmath       -Wl,-Bdynamic    -ldl -lrt -o $AMSWD/exe/linux/$1_gcc534
 

chmod +x $AMSWD/exe/linux/$1_534
