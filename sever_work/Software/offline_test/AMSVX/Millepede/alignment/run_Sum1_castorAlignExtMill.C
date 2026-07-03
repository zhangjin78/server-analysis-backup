#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
//#define USENEVG
//#define USEHIGHE
//#define USECLEAN2
#define USEFAST
#define USEEXTALIGN
#define USEFINAL
#define AFTERSPLINE
//#define AFTERSPLINE2
#define USEFIXPAR2
//#define USEFIXPAR3
#include "ParSelectAlignExtMill.C"
int run_Sum1_castorAlignExtMill(char *infile="/afs/cern.ch/work/q/qyan/log/log1/bcanalysis4_13_69_ev9_1330391865_2012022801_20file",char *outfile="test_2.root",char *outdir=0,char *runlogdir=0){
   AMSChain *ch=0;
//   TChain *ch_1=new TChain("amstree");
   TChain *ch_1=new TChain("amstreea");
//   readfile(infile,(*ch_1),-1,1);
   int rmode=10;
   readfile(infile,(*ch_1),-1,1,0,runlogdir,rmode);
//   ch_1->Print();
#if defined (AFTERSPLINE) || defined (AFTERSPLINE2)
//   int nalign=3;
   int nalign=4;
#else
//   int nalign=5;
   int nalign=4;
#endif
   for(int ialign=0;ialign<nalign;ialign++){
#ifdef NOMCSMEAR
     if(ialign!=0)continue;
#endif
     int ua=ialign;
     if(ua==nalign-1)ua*=-1;
     int stat=Sum1_AMS_CINT(ch,ch_1,outfile,outdir,runlogdir,-1,ua);
     if(stat)return stat;//fatal error stop loop
   }
   return 0;
}
int main(int argc,char ** argv){
   if     (argc>=5)return run_Sum1_castorAlignExtMill(argv[1],argv[2],argv[3],argv[4]);
   else if(argc>=4)return run_Sum1_castorAlignExtMill(argv[1],argv[2],argv[3]);
   else            return run_Sum1_castorAlignExtMill(argv[1],argv[2]);
}
