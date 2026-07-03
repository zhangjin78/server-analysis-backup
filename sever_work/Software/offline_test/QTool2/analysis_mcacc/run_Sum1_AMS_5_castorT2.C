#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#include "ParSelect5_T2T2.C"
int run_Sum1_AMS_5_castorT2(const char *infile="/afs/cern.ch/work/q/qyan/log/log1/amsmion_B636accts3546_1209015056_30file",const char *outfile="test_2.root",const char *runlogdir="/afs/cern.ch/work/q/qyan/runlist/"){
   AMSChain *ch=0;
   TChain *ch_1=new TChain("amstreea");
//   readfile(infile,(*ch_1),-1,1);
   int rmode=10;
   int nread=readfile(infile,(*ch_1),-1,1,0,runlogdir,rmode);//check in runlogdir list
   if(nread<=0)return 2;//zero runs
//   ch_1->Print();
   return Sum1_AMS_CINT(ch,ch_1,outfile,runlogdir); 
}
int main(int argc,char ** argv){
   return run_Sum1_AMS_5_castorT2(argv[1],argv[2],argv[3]);
}
