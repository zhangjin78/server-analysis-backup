#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
//#define USELAYERS
//#define USELADDERS
//#define USESENSORS
//#define AFTERALIGN
//#define TESTRIG
//#define AFTERALIGN2
#define USEMAXSPAN
//#define USETESTBEAMA
//#define USENOBEAMMAT
#include "ParSelectAlignMill.C"

int run_Sum1_AlignMill(char *infile="root://castorpublic.cern.ch///castor/cern.ch/user/q/qyan/tofsum1_1_B584/1312161387_2011080101.root?svcClass=amsuser",char *outfile="ParEff3_2MC.root",char *outdir=0,char *runlogdir="/afs/cern.ch/work/q/qyan/runlist/"){
   AMSChain *ch=0;
   TChain *ch_1=new TChain("amstreea");
#if defined (USENOBEAMMAT)
     ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_PR180N0AV3N1/1282333817_5.root");
#elif defined (USETESTBEAMA)
     ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1221_400testaN1/1679463509_40.root");
#else
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_PR400N0/1281464767_5.root");
     ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_PR400N0AV3/1281464767_5.root");
     ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_PR400N0AV3/1281355854_5.root");
     ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_PR400N0AV3/1281706805_5.root");
#endif
//   ch_1->AddFile("aligtree.root");
//   ch_1->AddFile("aligtree_prmc054000.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1200_05_4000400N0/1879299177_5.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1200_05_4000400N0/604405138_5.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1200_05_4000400N0/604405097_5.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1200_05_4000400N0/604405301_5.root");
//   ch_1->AddFile("pr400test.root");
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_He4MCB1200l1_2_4000400N0/604423287_5.root");
   return Sum1_AMS_CINT(ch,ch_1,outfile,outdir,runlogdir);
//   return Sum1_AMS_CINT(ch,ch_1,outfile,outdir,runlogdir,1000);
   
}
int main(char *infile,char *outfile){
   char ofile[1000];
   sprintf(ofile,"ParEff3_");   
#if defined (USELAYERS)
   strcat(ofile,"Layer");
#elif defined (USELADDERS)
   strcat(ofile,"Ladder");
#else
   strcat(ofile,"Data");
#endif
#if defined (USENOBEAMMAT)
   strcat(ofile,"NOBM");
#elif defined (USETESTBEAMA)
   strcat(ofile,"MCTB");
#endif
#ifdef AFTERALIGN
   strcat(ofile,"AfAlign");
#endif
#ifdef USEMAXSPAN
   strcat(ofile,"MSPAN");
#endif
   strcat(ofile,".root");
   char *outdir=0;
   char *runlogdir=0;
   return run_Sum1_AlignMill(infile,ofile,outdir,runlogdir);
}
