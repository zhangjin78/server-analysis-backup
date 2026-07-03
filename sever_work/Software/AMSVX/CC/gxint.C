//  $Id: gxint.C,v 1.15 2016/06/08 13:13:08 bshan Exp $
// Author V. Choutko 24-may-1996
 
#include <iostream>
#include "cern.h"
#include "gvolume.h"
#include <signal.h>
#include <unistd.h>
#include "MPIEmulator.h"

const int NWPAW=2000000;
struct PAWC_DEF{
float q[NWPAW];
};
const int NWGEAN=12000000;
struct GCBANK_DEF{
float q[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
//
void (handler)(int);
MPIEmulator mpi;
int main(int argc, char * argv[]){
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
    if (mpi.MPI_Init(argc, argv) != 0) {
        cerr << "MPIEmulator::MPI_Init returned error, exiting..." << endl;
        return 1;
    }

   AMSgvolume::debug=0;
  GPAW(NWGEAN,NWPAW);
return 0;
}

void (handler)(int sig){
  if(sig==SIGFPE)cerr <<" FPE intercepted"<<endl;
  else if (sig==SIGTERM || sig==SIGINT){
    cerr <<" SIGTERM intercepted"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
  }
  else if(sig==SIGQUIT){
    cerr <<" Process suspended"<<endl;
     pause();
  }
  else if(sig==SIGCONT){
    cerr <<" Process resumed"<<endl;
  }
}
