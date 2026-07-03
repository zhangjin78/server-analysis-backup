#!/bin/csh
source $AMSWD/install/amsroot.csh

icc  -I/$ROOTSYS/include  -no-multibyte-chars -I$AMSWD/include -D__root__new  -D__WRITEROOT__ -D__ROOTSHAREDLIBRARY__ -qopenmp -O3 -axsse4.2,ssse3,AVX,CORE-AVX2 $1.C -c -o $1.o
icpc  -static-libstdc++ -static-libgcc -static-intel -qopenmp -qopenmp-link static  -axssse3  -static-intel  -static-libgcc -O $1.o -Bstatic -L$AMSWD/lib/linuxx8664icc5.34 -lntuple_slc6_PG    -L$ROOTSYS/lib -lRoot -L/afs/cern.ch/ams/Offline/CERN/NagLib -lnag64 -L/afs/cern.ch/ams/Offline/CERN/2005.icc16/lib -lmathlib -lpacklib -lgfortran  -pthread   -lpcre  -L/afs/cern.ch/ams/local2/opt/lib64 -lcrypto  -llzma -lfreetype -Bdynamic -L$XRDLIB/lib64 -lXrdClient  -lXrdUtils -Wl,--allow-multiple-definition   -Bstatic -lifcore -lshift    -Bdynamic    -ldl -lrt -o $AMSWD/exe/linux/$1_534
chmod +x $AMSWD/exe/linux/$1_534
