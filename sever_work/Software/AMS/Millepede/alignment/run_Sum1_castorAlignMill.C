#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
//#define USENOSMEAR
//#define USELAYERS
//#define USELADDERS
//#define USESENSORS
//#define DROPBADHIT
//#define AFTERALIGN
//#define USENOMS
//#define TESTRIG
//#define AFTERALIGN2
#define USEMAXSPAN
#define LOOSECUT
//#define USENOBEAMMAT
//#define USETESTBEAMA
//#define SAMEL19ERROR
#include "ParSelectAlignMill.C"
int run_Sum1_castorAlignMill(char *infile="/afs/cern.ch/work/q/qyan/log/log1/bcanalysis4_13_69_ev9_1330391865_2012022801_20file",char *outfile="test_2.root",char *outdir=0,char *runlogdir="/afs/cern.ch/work/q/qyan/runlist/"){
   AMSChain *ch=0;
//   TChain *ch_1=new TChain("amstree");
   TChain *ch_1=new TChain("amstreea");
//   readfile(infile,(*ch_1),-1,1);
   readfile(infile,(*ch_1),-1,1);
//   ch_1->Print();
   return Sum1_AMS_CINT(ch,ch_1,outfile,outdir,runlogdir); 
}
int main(int argc,char ** argv){
   if(argc>=5)return run_Sum1_castorAlignMill(argv[1],argv[2],argv[3],argv[4]);
   else       return run_Sum1_castorAlignMill(argv[1],argv[2],argv[3]);
}
