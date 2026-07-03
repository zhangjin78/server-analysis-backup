#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
//#define USEP4ISS
//#define USEOLDDST
#define USEMHZ
#define USENEWTKQ
#include "ParSelect5_T2T2.C"

int run_Sum1_AMS_Par5T(char *infile="root://castorpublic.cern.ch///castor/cern.ch/user/q/qyan/tofsum1_1_B584/1312161387_2011080101.root?svcClass=amsuser",char *outfile="ParEff3_2O.root",const char *runlogdir="/afs/cern.ch/work/q/qyan/runlist/"){
   AMSChain *ch=0;
   TChain *ch_1=new TChain("amstreea");
//   ch_1->AddFile("root://eosams.cern.ch//eos/ams/group/mit/amsd64n_ROOT5HPB_Si28MCB1220l1_14_28000402N0/73500345_30.root");
   ch_1->AddFile("root://eosams.cern.ch//eos/ams/group/mit/amsd68n_ROOT5HB_Si28MCB1236l1_14_28000602N0/80239404_10.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/analysis_amsd40n_TMTFNTotORDHB_C12MCB1075rdl19_12_48000203N0/67615406_10.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/analysis_amsd40n_TMTFNTotORDHB_C12MCB1075rdl19_12_48000203N0/1209015290_10.root");
//   ch_1->Print();
   return Sum1_AMS_CINT(ch,ch_1,outfile,runlogdir,40000); 
//   return Sum1_AMS_CINT(ch,ch_1,outfile);
   
}
int main(int argc,char ** argv){
   return run_Sum1_AMS_Par5T(argv[1],argv[2],argv[3]);
}
