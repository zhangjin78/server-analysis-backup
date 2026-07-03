#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
//#define USELAYERS
//#define USELADDERS
//#define USESENSORS
//#define USEFINAL
//#define AFTERSPLINE
#define USEFAST
#define USEDATA
//#define USECLEANA
//#define USEHIGHE
//#define USESURVIVEL9
//#define USENORIGWEIGHT
//#define USET2CHISCUT
//#define USET1CHISCUT
#define USEEXTALIGN
#define AFTERSPLINE
#include "ParSelectAlignExtMill.C"

int run_Sum1_AlignMill(char *infile="root://castorpublic.cern.ch///castor/cern.ch/user/q/qyan/tofsum1_1_B584/1312161387_2011080101.root?svcClass=amsuser",char *outfile="ParEff3_2MC.root",char *outdir=0,char *runlogdir="/afs/cern.ch/work/q/qyan/runlist/"){
   AMSChain *ch=0;
   TChain *ch_1=new TChain("amstreea");
#ifdef USEDATA
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_P7N0/1591290818_10.root");
   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_BTB1130_P7N0/1505567802_10.root");//1505578915
#else
   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1200_05_100V3N0/604410948_5.root");
   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_PrMCB1200_05_100V3N0/604411771_5.root");
#endif
//   ch_1->AddFile("root://eosams//eos/ams/group/mit/alignment_ROOT5HPB_He4MCB1200l1_2_4000400N0/604423287_5.root");
#ifdef AFTERSPLINE
//   int nalign=3;
   int nalign=4;
#else
//   int nalign=5;
   int nalign=4;
#endif
//   int nalign=1;
   for(int ialign=0;ialign<nalign;ialign++){
#ifdef NOMCSMEAR
     if(ialign!=0)continue;
#endif
/*#ifdef AFTERSPLINE
     if(ialign<=1)continue;//start with ialign>=2
#endif*/
     int ua=ialign;
     if(ua==nalign-1)ua*=-1;
     int stat=Sum1_AMS_CINT(ch,ch_1,outfile,outdir,runlogdir,-1,ua);
     if(stat)return stat;//fatal error stop loop
//      Sum1_AMS_CINT(ch,ch_1,outfile,runlogdir,1000000,ialign);
   }
   return 0; 
}
int main(char *infile,char *outfile){
//   run_Sum1_AlignMill(infile,outfile,runlogdir);
   char ofile[1000];
   sprintf(ofile,"ParEff3Ext_");
   strcat(ofile,"MSPAN");
#ifdef USEDATA
   strcat(ofile,"DATA");
#endif
#if defined (USEHIGHE)
   strcat(ofile,"HE");
#elif defined (USELOWE)
   strcat(ofile,"LE");
#endif
#ifdef USESURVIVEL9
   strcat(ofile,"SURVL9");
#endif
#ifdef USENORIGWEIGHT
   strcat(ofile,"NORW");
#endif
#if defined (USET2CHISCUT)
   strcat(ofile,"T2Chis");
#elif defined (USET1CHISCUT)
   strcat(ofile,"TChis");
#endif
   strcat(ofile,".root");
   char *outdir=0;
   char *runlogdir=0;
   return run_Sum1_AlignMill(infile,ofile,outdir,runlogdir);
}
