#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#define USEGBL
#define USEALL
//#define USENEWTKQ
#include "ParSelect4.C"

int run_Sum1_AMS_Par4(char *infile="root://castorpublic.cern.ch///castor/cern.ch/user/q/qyan/tofsum1_1_B584/1312161387_2011080101.root?svcClass=amsuser",char *outfile="ParEff3_2.root",char *runlogdir="/afs/cern.ch/work/q/qyan/runlist/"){
   AMSChain *ch=0;
   TChain *ch_1=new TChain("amstreea");
//     ch_1->AddFile("root://eosams//eos/ams/group/mit/amsd54n_TMTFNTotHB_B1130P7/1498495489_10.root");
//     ch_1->AddFile("root://eosams//eos/ams/group/mit/amsd62n_TMTFNTotHB_B1130P7/1527384482_10.root");
//     ch_1->AddFile("root://eosams//eos/ams/group/mit/amsd64n_TMTFNTotHPB_B1130P7/1572341601_10.root");
     ch_1->AddFile("root://eosams.cern.ch//eos/ams/group/mit/amsd68n_TMTFNTotHB_B1236P8/1667846934_10.root");
//     ch_1->AddFile("root://eosams//eos/ams/user/q/qyan/analysis_amsd34n_TMTFNTotORD_C12MCB1048_115_6_12000N0/67267588_3.root");
//    ch_1->AddFile("root://eosams//eos/ams/user/q/qyan/analysis_amsd20n_TMTFNTotORD_L6MCB1005_6_6000N1/67117062_5.root");
//   ch_1->Print();
//   return Sum1_AMS_CINT(ch,ch_1,outfile,runlogdir,1000000); 
   return Sum1_AMS_CINT(ch,ch_1,outfile,runlogdir);
   //return Sum1_AMS_CINT(ch,ch_1,outfile,runlogdir,3000000);
   
}
int main(int argc,char ** argv){
   return run_Sum1_AMS_Par4(argv[1],argv[2],argv[3]);
}
