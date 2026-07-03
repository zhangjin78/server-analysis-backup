///--Author Q.Yan qyan@cern.ch
///--An Example for MC Accpetance Study by using QTool
#include <TH1.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "HistoMan.h"
#include "TProfile2D.h"
#include "TVirtualFitter.h"
#include "root_RVSP.h"
#include "TDatime.h"
#include "readfile3.C"
#include "bin.C"
#include "B800MCCor.C"
#ifdef USEOLDDST
#include "QEvent.h"
#else
#include "QEvent2.h"
#endif
#include "QAnalysis.h"
#include "fluxweight.C"

#define MYOUT
#define USETTK
//#define USEFAST
using namespace qanalysisconst;

class QAnalysisN: public QAnalysis{
 public:
//----Cl-Fe
#if defined (USESHZ) || defined (USESHZD)
    static const int SCharge=14;
    static const int MCharge=28;
//----O-S
#elif defined (USEMHZ)
    static const int SCharge=8;
    static const int MCharge=18;
//----P(He)-O
#else 
#if defined (USEPROTON)
    static const int SCharge=1;
#else
    static const int SCharge=2;
#endif
    static const int MCharge=8;
#endif
    HistoMan hman1;
 public:
    QAnalysisN(TChain *ch):QAnalysis(ch){}
    int BookHistos(const char *ofile);
    int InitEvent(QEvent *ev0=0);//First
    int SelectEvent(QEvent *ev0=0,int pos=0);//Second
    int Save(){hman1.Save();return 0;}
};

TDatime T0(2011,05,00,00,00,00);
int X0=T0.Convert();

//------BookHisto
int QAnalysisN::BookHistos(const char *ofile){

  hman1.Clear();
  hman1.Enable();
  hman1.Setname(ofile);
//---
  TDatime T2(2019,01,01,00,00,00);
  int X2=T2.Convert();
  int dt=86400*3;
  int bint=(X2-X0)/dt+1;
//---
   char histn[1000];
//   float pmin=0.5,pmax=3000;
   float pmin=0.5,pmax=32000;
#ifdef USEP7CUT
   const int bin=130;
#else
   const int bin=200;
#endif
   double mon[bin+1];
   float de=(log(pmax)-log(pmin))/bin;
   for(int ibin=0;ibin<=bin;ibin++){
      mon[ibin]=log(pmin)+de*ibin;
      mon[ibin]=exp(mon[ibin]);
      cout<<"mon="<<mon[ibin]<<endl;
   }

   double hebin[1000];
   const int binhe=getamsbin(hebin,1)-1;
//---
   const int bincos=6;
   double cosbin[1000];
   getcosbin(cosbin,0,20,bincos);
//--Exposure
   sprintf(histn,"mcev_zm");
   hman1.Add(new TH1D(histn,histn,30,-2,28));
   sprintf(histn,"mcev_rrawp");
   hman1.Add(new TH1D(histn,histn,2,0,2));
   sprintf(histn,"mcev_rrawpm");
   hman1.Add(new TH1D(histn,histn,2,0,2));
   sprintf(histn,"mcev_rawp");
   hman1.Add(new TH1D(histn,histn,bin,mon));
   sprintf(histn,"mcev_rawp1");
   hman1.Add(new TH1D(histn,histn,bin,mon));
   double qlv=SCharge-3;
   double qhv=MCharge+3;
   for(int isp=0;isp<3;isp++){
     sprintf(histn,"mcev_rawp%d_time",isp);
     hman1.Add(new TH1D(histn,histn,bint,0,X2-X0));
     sprintf(histn,"mcev_rawp%d_timep",isp);
     hman1.Add(new TH1D(histn,histn,bint,0,X2-X0));
     sprintf(histn,"mcev_rawp%d_cite",isp);
     hman1.Add(new TH1D(histn,histn,32,0,32));
     sprintf(histn,"mcev_rawp%d_citep",isp);
     hman1.Add(new TH1D(histn,histn,32,0,32));
   }
   for(int igeo=0;igeo<3;igeo++){
     for(int ia=0;ia<6;ia++){
      sprintf(histn,"mcev_rawp_a%d_g%d",ia,igeo);
      hman1.Add(new TH1D(histn,histn,bin,mon));
      for(int ifrag=1;ifrag<=4;ifrag++){
       sprintf(histn,"mcev_f%d_a%d_g%d",ifrag,ia,igeo);
       hman1.Add(new TH1D(histn,histn,bin,mon)); 
     }
    }
  } 
//---
  for(int ib=0;ib<=8;ib++){
     sprintf(histn,"mcev_raw_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_trig0_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_stat0_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     for(int ibit=0;ibit<=12;ibit++){
        sprintf(histn,"mcev_error%d_b%d",ibit,ib);
        hman1.Add(new TH1D(histn,histn,bin,mon));
     }
     sprintf(histn,"mcev_errora_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tkmc_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tkmct_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tkmct1_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tkmct2_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_ntrrawcl_b%d",ib);
     hman1.Add(new TH2D(histn,histn,bin,mon,50,0,4000));
     sprintf(histn,"mcev_ntrcl_b%d",ib);
     hman1.Add(new TH2D(histn,histn,bin,mon,50,0,4000));
     sprintf(histn,"mcev_ntrhit_b%d",ib);
     hman1.Add(new TH2D(histn,histn,bin,mon,50,0,3200));
     sprintf(histn,"mcev_tofmc_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tktofmct2_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_par_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tk_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tkq_b%d_2d",ib);
     hman1.Add(new TH2D(histn,histn,bin,mon,500,qlv,qhv));
     sprintf(histn,"mcev_tkq_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tkq_b%ds0",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_tofq_b%d",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     sprintf(histn,"mcev_sp0_b%ds0",ib);
     hman1.Add(new TH1D(histn,histn,bin,mon));
     for(int isp=0;isp<3;isp++){
       sprintf(histn,"mcev_sp%d_b%d",isp,ib);
       hman1.Add(new TH1D(histn,histn,bin,mon));
       sprintf(histn,"mcev_trig1_sp%d_b%d",isp,ib);
       hman1.Add(new TH1D(histn,histn,bin,mon));  
       sprintf(histn,"mcev_tofq_sp%d_b%d",isp,ib);
       hman1.Add(new TH1D(histn,histn,bin,mon));
       sprintf(histn,"mcev_tofqud_sp%d_b%d",isp,ib);
       hman1.Add(new TH1D(histn,histn,bin,mon));
       sprintf(histn,"mcev_tofqud_tofsp%d_b%d",isp,ib);
       hman1.Add(new TH1D(histn,histn,bin,mon));
       sprintf(histn,"mcev_tofqud_tksp%d_b%d",isp,ib);
       hman1.Add(new TH1D(histn,histn,bin,mon));
     }
   }
 
//---
   for(int ich=SCharge;ich<=MCharge+1;ich++){
     sprintf(histn,"all_l9r_q%d_pid",ich);
     hman1.Add(new TH2D(histn,histn,bin,mon,100,0,100));
     for(int ir=0;ir<7;ir++){
//---
       for(int ib=0;ib<3;ib++){
         sprintf(histn,"all_rr%d_q%d_2dqx9_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,400,0,10));
         sprintf(histn,"all_rr%d_q%d_2dqy9_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,400,0,10));
         sprintf(histn,"all_rr%d_q%d_2dqxy9_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,400,0,10));
         sprintf(histn,"all_rr%d_q%d_2dtofqu_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,200,0,10));
         sprintf(histn,"all_rr%d_q%d_2dtofqd_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,200,0,10));
         sprintf(histn,"all_rr%d_q%d_2dchisl9_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,100,-10,10));
         sprintf(histn,"all_rr%d_q%d_2dtofql3_b%d",ir,ich,ib);
         hman1.Add(new TH2D(histn,histn,bin,mon,200,0,10));
       }
//---
       for(int imr=0;imr<7;imr++){
           sprintf(histn,"all_l1r%d_mr%d_q%d_g",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_s",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_t",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_ts",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1t",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1time",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bint,0,X2-X0));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1cite",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,32,0,32));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tm",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tt",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tt1",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           if(ir==4||ir==2){
             sprintf(histn,"all_l1r%d_mr%d_q%d_1ttheta",ir,imr,ich);//Trigger
             hman1.Add(new TH2D(histn,histn,bin,mon,bincos,cosbin));
             sprintf(histn,"all_l1r%d_mr%d_q%d_1tthetaw",ir,imr,ich);//Trigger
             hman1.Add(new TH2D(histn,histn,bin,mon,bincos,cosbin));
           }
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tsi",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1ts",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tts",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tts1",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tss",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_1tofs",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_3",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_3t",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_3ts",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_4",ir,imr,ich);//L1Inner Only
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_4t",ir,imr,ich);//L1Inner Only
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_4ts",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_4tss",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5t",ir,imr,ich);
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5tc",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5tw1",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,binhe,hebin));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5ts",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5tk",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_l1r%d_mr%d_q%d_5tsi",ir,imr,ich);//Trigger
           hman1.Add(new TH1D(histn,histn,bin,mon));
           sprintf(histn,"all_mr%d_q%d_zidl1",imr,ich);
           if(ir==4)hman1.Add(new TH2D(histn,histn,bin,mon,35,0,35));
           sprintf(histn,"all_mr%d_q%d_aidl1k",imr,ich);
           if(ir==4)hman1.Add(new TH2D(histn,histn,bin,mon,70,0,70));
           for(int ib=0;ib<2;ib++){
            sprintf(histn,"all_mr%d_q%d_zidl9b%d",imr,ich,ib);
            if(ir==4)hman1.Add(new TH2D(histn,histn,bin,mon,35,0,35));
            sprintf(histn,"all_mr%d_q%d_aidl9b%d",imr,ich,ib);
            if(ir==4)hman1.Add(new TH2D(histn,histn,bin,mon,70,0,70));
            sprintf(histn,"all_mr%d_q%d_zidinb%d",imr,ich,ib);
            if(ir==4)hman1.Add(new TH2D(histn,histn,bin,mon,35,0,35));
            sprintf(histn,"all_mr%d_q%d_aidinb%d",imr,ich,ib);
            if(ir==4)hman1.Add(new TH2D(histn,histn,bin,mon,70,0,70));
            sprintf(histn,"all_l1r%d_mr%d_q%d_5tql1b%d",ir,imr,ich,ib);//Trigger
            if((imr==2||imr==6)&&(ir==imr||ir==4))hman1.Add(new TH2D(histn,histn,bin,mon,300,2,10));
          }
//---
       }
       sprintf(histn,"all_l1r%d_q%d_n2dqi",ir,ich);
       hman1.Add(new TH2D(histn,histn,bin,mon,400,0,10));
       sprintf(histn,"all_l1r%d_q%d_tofbeta",ir,ich);
       hman1.Add(new TH2D(histn,histn,bin,mon,100,-2,2));
       sprintf(histn,"all_l1r%d_q%d_chis",ir,ich);
       hman1.Add(new TH2D(histn,histn,bin,mon,100,0,7));
//---
      for(int ilay=0;ilay<9;ilay++){
        for(int ic=0;ic<2;ic++){
         if(ir!=4)continue;
         sprintf(histn,"tkresyhit_l%d_r%d_q%d_c%d",ilay,ir,ich,ic);
         hman1.Add(new TH2D(histn,histn,bin,mon,100,-50,50));
         sprintf(histn,"tkresytrmc_l%d_r%d_q%d_c%d",ilay,ir,ich,ic);
         hman1.Add(new TH2F(histn,histn,bin,mon,100,-50,50));
         if(ilay==0||ilay==8){
           sprintf(histn,"tkresyscat_l%d_r%d_q%d_c%d",ilay,ir,ich,ic);
           hman1.Add(new TH2D(histn,histn,bin,mon,300,-200,200));
         }
         else {
           sprintf(histn,"tkresyscat_l%d_r%d_q%d_c%d",ilay,ir,ich,ic);
           hman1.Add(new TH2F(histn,histn,bin,mon,200,-50,50));
         }
        }
      }
//---
     }
   }

  return 0;
}

//-------------------Init Event
int QAnalysisN::InitEvent(QEvent *ev){

   float mrig=ev->mmom/ev->mch;
   int   tk_zm=ev->mch;
//---Correction for MC L1Q and L9Q(P/He)
   if(!ev->isreal&&abs(tk_zm)<=2){
     float corrig=fabs(mrig);
     mctkcor(ev->tk_ql[0],ev->tk_ql[8],corrig);
   }
   return 0;
}


//-------------------Select Event
int QAnalysisN::SelectEvent(QEvent *ev,int pos){

   int ialgo=0;
#if defined (USEGBL)
   ialgo=1;
#elif defined (USEKALMAN)
   ialgo=2;
#endif
   int  tkqcalib=0;
#ifdef USENEWTKQ
   tkqcalib=1;
//   tkqcalib=2;
#endif
   char histn[1000];
   float mrig=ev->mmom/ev->mch;
   int   tk_zm=ev->mch;
//---
   float mrigc=mrig;
   if(!ev->isreal&&tk_zm<0)mrigc=-mrigc;//Electron
 
//--MCEvent TotalEvent //run+event
   static int prrun=-1,prev=-1;
   static int prcev=0,ccev;
   if(ev->event<prev||ev->run!=prrun||pos==0){
     cout<<"prrun="<<prrun<<" prev="<<prev<<" nrun="<<ev->run<<" nev="<<ev->event;
     if(prev!=-1){
        if((pos==0)&&(ev->run==prrun)&&(ev->event>prev))prev=ev->event;//update with the last event
        ccev=(prev-prcev+1);//Total
        if(prcev<100)ccev=prev;//old
        cout<<" cev="<<ccev;
        if(!ev->isreal)hman1.Fill("mcev_rrawp",0,double(ccev));
     }
     cout<<endl;
     prcev=ev->event;
   }
   prrun=ev->run;prev=ev->event;//Latest

//-----MC-Focus
   if(!ev->isreal){
      double extrap1[3],extrap9[3];
      static double cross1[4]={63.14,48.4, 158.920,67.14};
      static double cross9[4]={46.62,34.5,-135.882,67.14};
      ev->InterpolateMC(cross1[2],extrap1);
      ev->InterpolateMC(cross9[2],extrap9);
      bool iscross1=(fabs(extrap1[0])<cross1[0]&&fabs(extrap1[1])<cross1[1]&&sqrt(extrap1[0]*extrap1[0]+extrap1[1]*extrap1[1])<cross1[3]);
      bool iscross9=(fabs(extrap9[0])<cross9[0]&&fabs(extrap9[1])<cross9[1]&&sqrt(extrap9[0]*extrap9[0]+extrap9[1]*extrap9[1])<cross9[3]);
//      cout<<"extra="<<extrap1[0]<<" "<<extrap1[1]<<" cross1="<<iscross1<<" mcdir="<<ev->mevdir[2]<<endl;
#ifdef USEMCFOCUSTH
      if(ev->mevdir[2]>-0.7)return -1;
#endif
#ifdef USEMCFOCUSL1
      if(!iscross1)return -1;
#endif
#ifdef USEMCFOCUSL9
      if(!iscross9)return -1;
#endif
   }

//---MCEvent Z
   hman1.Fill("mcev_zm",(tk_zm+0.5));

//---Tkco
    double tkcoo[9][3]={{0}};
    double tkdir[9][3]={{0}};
    for(int il=0;il<9;il++){
       int uil;
       if     (il==0)uil=0;
       else if(il==8)uil=18;
       else          uil=il+6;
       for(int ic=0;ic<3;ic++){
         tkcoo[il][ic]=ev->mevcoo1[uil][ic];
         tkdir[il][ic]=ev->mevdir1[uil][ic];
       } 
    }
//    cout<<"tkcoo="<<tkcoo[0][2]<<endl;
//--L1InnerGeom Event(All)
    bool mmlh[9]={0};
    bool mnlh[9]={0};
    bool mtlh[9]={0};
    for(int il=0;il<9;il++){
      float coox=(tk_pz[il]-ev->mevcoo[2])*ev->mevdir[0]/ev->mevdir[2]+ev->mevcoo[0];
      float cooy=(tk_pz[il]-ev->mevcoo[2])*ev->mevdir[1]/ev->mevdir[2]+ev->mevcoo[1];
      float disr=coox*coox+cooy*cooy;
      if(il==9-1)mmlh[il]=(fabs(coox)<cirr[il]&&fabs(cooy)<ciry[il]);
      else       mmlh[il]=(sqrt(disr)<cirr[il]&&fabs(cooy)<ciry[il]);
//---
      if(ev->mtrpri[il])mtlh[il]=1;
//---RealTk(AC)
      float disrn=sqrt(tkcoo[il][0]*tkcoo[il][0]+tkcoo[il][1]*tkcoo[il][1]);
      if(il==9-1)mnlh[il]=(fabs(tkcoo[il][0])<cirr[il]&&fabs(tkcoo[il][1])<ciry[il]);
      else       mnlh[il]=(sqrt(disrn)       <cirr[il]&&fabs(tkcoo[il][1])<ciry[il]);
   }
//---ECAL PZ
    float ecoox=(ecal_pz-ev->mevcoo[2])*ev->mevdir[0]/ev->mevdir[2]+ev->mevcoo[0];
    float ecooy=(ecal_pz-ev->mevcoo[2])*ev->mevdir[1]/ev->mevdir[2]+ev->mevcoo[1];
    bool mmecal=(fabs(ecoox)<ecalxy[0]&&fabs(ecooy)<ecalxy[1]);

//---Hit Number
   int mmnhitf=0;
   int mnnhitf=0;
   int mtnhitf=0;
   for(int il=0+1;il<9-1;il++){
      if(mmlh[il]){mmnhitf++;}
      if(mtlh[il]){mtnhitf++;}
      if(mnlh[il]){mnnhitf++;}
   }
//---
   bool MTL2=(mtlh[1]);
   bool MTL34=(mtlh[2]||mtlh[3]);
   bool MTL56=(mtlh[4]||mtlh[5]);
   bool MTL78=(mtlh[6]||mtlh[7]);
   bool MTLIn=(MTL2&&MTL34&&MTL56&&MTL78);

//---   
   bool mmcuthit=(mmnhitf>=5&&mmlh[0]&&mmlh[8]);//(All)
   bool mmcuthit1=(mmnhitf>=5&&mmlh[0]);//(All) L1Inner
   bool mtcuthit=(mtnhitf>=5&&mtlh[0]&&mtlh[7]);//(TrMC)
   bool mtcuthit1=(mtlh[0]&&mtlh[7]);//(TrMC)
   bool mtcuthit2=(mtlh[0]&&mtnhitf>=5);//(TrMC) L1+Inner 
   bool mncuthit=(mnnhitf>=5&&mnlh[0]&&mnlh[7]);//(AC)
//--Survive
  bool issurvl1=(ev->mevmom1[0]!=-1000||ev->mevmom1[1]!=-1000);
  bool issurvl2=(ev->mevmom1[7]!=-1000||ev->mevmom1[8]!=-1000);
  bool issurvl8=(ev->mevmom1[13]!=-1000||ev->mevmom1[14]!=-1000);
  bool issurvl9=(ev->mevmom1[18]!=-1000||ev->mevmom1[19]!=-1000);
  bool issurvl10=(ev->mevmom1[19]!=-1000);
  bool issurvl11=(ev->mevmom1[20]!=-1000);

   int citeid=-1;
   if(!ev->isreal)AMSEventR::GetCite(ev->run,citeid);
   int rtime=ev->time[0]-X0;
//---MC Rigidity
  if(mmcuthit&&!ev->isreal){//Ful-Span Geom
    sprintf(histn,"mcev_rawp");
    hman1.Fill(histn,mrigc);
    sprintf(histn,"mcev_rawp2_time");
    hman1.Fill(histn,rtime);
    sprintf(histn,"mcev_rawp2_timep");
    if(issurvl9)hman1.Fill(histn,rtime);
    sprintf(histn,"mcev_rawp2_cite");
    hman1.Fill(histn,citeid);
    sprintf(histn,"mcev_rawp2_citep");
    if(issurvl9)hman1.Fill(histn,citeid);
  }
  if(mmcuthit1&&!ev->isreal){//L1Inner-Geom
    sprintf(histn,"mcev_rawp1");
    hman1.Fill(histn,mrigc);
    sprintf(histn,"mcev_rawp1_time");
    hman1.Fill(histn,rtime);
    sprintf(histn,"mcev_rawp1_timep");
    if(issurvl8)hman1.Fill(histn,rtime);
    sprintf(histn,"mcev_rawp1_cite");
    hman1.Fill(histn,citeid);
    sprintf(histn,"mcev_rawp1_citep");
    if(issurvl8)hman1.Fill(histn,citeid);
  }

//---
  for(int igeo=0;igeo<3;igeo++){
    int uspan=0;
    if     (igeo==0){if(!(mmcuthit&&mmecal))continue;uspan=3;}
    else if(igeo==1){if(!mmcuthit)continue;uspan=2;}
    else            {if(!mmcuthit1)continue;uspan=1;}
    for(int ia=0;ia<6;ia++){
     sprintf(histn,"mcev_rawp_a%d_g%d",ia,igeo);
     if(!ev->isreal)hman1.Fill(histn,mrigc);
     int  mfrag=1;
//---
     if(ia==0){
       if(fabs(ev->mparp[0])!=ev->mpar&&ev->mparp[0]!=0){//Sign(Vertex)
         if(ev->mfcoo[2]>tk_pz[0])mfrag|=(1<<1);//L1 Above
         if(ev->mfcoo[2]>tk_pz[7])mfrag|=(1<<2);//Last Track Above
         if(ev->mfcoo[2]>tk_pz[8])mfrag|=(1<<3);//L9 Above
       }
     }
     else if(ia==1){//(AC)
        if(!mtcuthit){mfrag|=(1<<2);}//Frag Above Last Tracker
        if(!mtcuthit||ev->mevmom1[18]==-1000){mfrag|=(1<<3);} //Frag Last L9 Above 
     }
     else if(ia==2){//(AC)
        if(ev->mevmom1[0] ==-1000)mfrag|=(1<<1);//L1 Above
        if(ev->mevmom1[13]==-1000)mfrag|=(1<<2);//Last Track Above
        if(ev->mevmom1[18]==-1000)mfrag|=(1<<3);//L9 Above
        if(ev->mevmom1[7]==-1000)mfrag|=(1<<4);//L2 Above
     }
     else if(ia==3){
        if(!mncuthit){mfrag|=(1<<2);}//Frag Above Last Tracker(AC)
        if(!mncuthit||ev->mevmom1[18]==-1000){mfrag|=(1<<3);} //Frag Last L9 Above 
     }
     else if(ia==4){
        if(!mtcuthit1){mfrag|=(1<<2);}//Frag Above Last Tracker(TrMC)
        if(!mtcuthit1||ev->mevmom1[18]==-1000){mfrag|=(1<<3);} //Frag Last L9 Above 
     }
     else if(ia==5){
        if(!mtcuthit2){mfrag|=(1<<2);}//Frag Above Last Tracker(TrMC)
        if(!mtcuthit2||ev->mevmom1[18]==-1000){mfrag|=(1<<3);} //Frag Last L9 Above 
     }

//---
     for(int ifrag=1;ifrag<=4;ifrag++){
         if(((mfrag>>ifrag)&1)==0){//Non Frag
            sprintf(histn,"mcev_f%d_a%d_g%d",ifrag,ia,igeo);
            if(!ev->isreal)hman1.Fill(histn,mrigc);
         }
      }
    }//ia
//---
  }//igeo

//--
  int mtofnhitf=0;
  for(int ilay=0;ilay<4;ilay++){
    if(ev->mtofpri[ilay])mtofnhitf++;
  }

//---Particle
  bool cutparbeta=(ev->Select_Particle()&&ev->Select_TofBeta());
//---Tk Geom
#ifndef USETTK
  bool cuttkinhits=ev->Select_TkInNHit();
#else
  bool cuttkinhits=ev->Select_TkInNHit(2);
#endif
//---Tk InnerZ+Selection 
  float tk_q1=ev->GetTkInQ(2,tkqcalib);
  int tk_z1=ev->GetTkInZ(2,tkqcalib);
  bool sumnorigtk=(ev->Select_TkInQ(0,tk_z1,tkqcalib));
//   if(!ev->isreal&&tk_zm==26)sumnorigtk=1;//no inqcut
//---TOF
  bool cuttofqu=ev->Select_TofUQ(0,tk_z1);
  bool cuttofqd=ev->Select_TofDQ(0,tk_z1);
//---Trigger
#ifdef USESHZD
  bool trigp=(ev->Select_Trigger(7));
#else
  bool trigp=(ev->Select_Trigger());
#endif

//---
  bool isdaqok=((ev->daqtyerr&1)==0);
  bool iserror=(((ev->errorb>>0)&1)==0);
  bool iserrort=(ev->errorb==0);
#ifndef USEFAST
   for(int ib=0;ib<=8;ib++){
     int ugeom=2;
     if(ib==3||ib==4){if(!mmcuthit1)continue;ugeom=1;}//L1I
     else            {if(!mmcuthit)continue; ugeom=2;}//L1IL9
//---
     if     (ib==0){if(!issurvl9)continue;}
     else if(ib==1){if(issurvl9)continue;}
     else if(ib==2){
        if(!issurvl10)continue;//L9Surive && Bad DAQ
     }
     else if(ib==3){
        if(!issurvl8)continue;
     }
     else if(ib==4){
        if(issurvl8)continue;
     }
     else if(ib==5){
        if(!issurvl9||!(MTLIn&&mtnhitf>=5&&mtofnhitf==4))continue;//L9Surive && Bad DAQ
     }
     else if(ib==6){
       if(!issurvl9||!(mtnhitf>=5&&mtofnhitf==4))continue;
     }
     else if(ib==7){
       if(!issurvl9||!(mtlh[0]&&mtlh[8]&&MTLIn&&mtnhitf>=5&&mtofnhitf==4))continue;
     }
     else if(ib==8){
       if(!issurvl9||!(mtlh[0]&&mtlh[8]&&mtnhitf>=5&&mtofnhitf==4))continue;    
     }
     sprintf(histn,"mcev_raw_b%d",ib);
     hman1.Fill(histn,mrigc);
     if(trigp){
       sprintf(histn,"mcev_trig0_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    if(isdaqok){
       sprintf(histn,"mcev_stat0_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
//---
    for(int ibit=0;ibit<=12;ibit++){
       if(((ev->errorb>>ibit)&1)==0){
         sprintf(histn,"mcev_error%d_b%d",ibit,ib);
         hman1.Fill(histn,mrigc);
       }
    }
     if(ev->errorb==0){
        sprintf(histn,"mcev_errora_b%d",ib);
        hman1.Fill(histn,mrigc);
     }
//--
    if(mtnhitf>=5){
       sprintf(histn,"mcev_tkmc_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    if(MTLIn&&mtnhitf>=5){
       sprintf(histn,"mcev_tkmct_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    if(mtlh[0]&&MTLIn&&mtnhitf>=5){
       sprintf(histn,"mcev_tkmct1_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    if(mtlh[0]&&mtlh[8]&&MTLIn&&mtnhitf>=5){
       sprintf(histn,"mcev_tkmct2_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    sprintf(histn,"mcev_ntrrawcl_b%d",ib);
    hman1.Fill(histn,mrigc,(ev->ntrrawcl+0.5));
    sprintf(histn,"mcev_ntrcl_b%d",ib);
    hman1.Fill(histn,mrigc,(ev->ntrcl+0.5));
    sprintf(histn,"mcev_ntrhit_b%d",ib);
    hman1.Fill(histn,mrigc,(ev->ntrhit+0.5));
    if(mtofnhitf==4){
       sprintf(histn,"mcev_tofmc_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    if(mtlh[0]&&mtlh[8]&&MTLIn&&mtnhitf>=5&&mtofnhitf==4){
       sprintf(histn,"mcev_tktofmct2_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
//    if(ib==0)cout<<"mtofnhitf="<<mtofnhitf<<endl;
    if(!cutparbeta)continue;
    sprintf(histn,"mcev_par_b%d",ib);
    hman1.Fill(histn,mrigc);
//    if(!ev->Select_TkGeom(ugeom))continue;
    if(!cuttkinhits)continue;
    sprintf(histn,"mcev_tk_b%d",ib);
    hman1.Fill(histn,mrigc);
    sprintf(histn,"mcev_tkq_b%d_2d",ib);
    hman1.Fill(histn,mrigc,tk_q1);
    if(tk_q1>abs(tk_zm)-2.5){
      sprintf(histn,"mcev_tkq_b%ds0",ib);
      hman1.Fill(histn,mrigc);
      if(ev->Select_TkRig(0,ialgo)){
        sprintf(histn,"mcev_sp0_b%ds0",ib);
        hman1.Fill(histn,mrigc);
      }
    }
    if(!sumnorigtk||tk_z1!=abs(tk_zm)){
      continue;
    }
    sprintf(histn,"mcev_tkq_b%d",ib);
    hman1.Fill(histn,mrigc);
    if(cuttofqu){
       sprintf(histn,"mcev_tofq_b%d",ib);
       hman1.Fill(histn,mrigc);
    }
    for(int isp=0;isp<3;isp++){ 
        if(!ev->Select_TkRig(isp,ialgo)||!ev->Select_TkExtQ(isp,0,tk_z1,tkqcalib))continue;
        sprintf(histn,"mcev_sp%d_b%d",isp,ib);
        hman1.Fill(histn,mrigc);
        if(trigp){
          sprintf(histn,"mcev_trig1_sp%d_b%d",isp,ib); 
          hman1.Fill(histn,mrigc);
        }
        if(cuttofqu){
          sprintf(histn,"mcev_tofq_sp%d_b%d",isp,ib);
          hman1.Fill(histn,mrigc);
        }
//----After TOFQUD
        if(cuttofqu&&cuttofqd){
          sprintf(histn,"mcev_tofqud_sp%d_b%d",isp,ib);
          hman1.Fill(histn,mrigc);
          if(mtofnhitf==4){
            sprintf(histn,"mcev_tofqud_tofsp%d_b%d",isp,ib);
            hman1.Fill(histn,mrigc);
          }
          if(mtlh[0]&&mtlh[8]&&MTLIn&&mtnhitf>=5){
            sprintf(histn,"mcev_tofqud_tksp%d_b%d",isp,ib);
            hman1.Fill(histn,mrigc);
          }
        }
//---
      }
    }
#endif

//--MCEvent MinR MaxR
   static double mmir=100000,mmar=-100000;
//   cout<<"mrigc="<<mrigc<<" isreal="<<ev->isreal<<endl;
   if(mrigc<mmir&&!ev->isreal){
      TH1D* th=(TH1D *)hman1.Get("mcev_rrawpm");
      th->SetBinContent(1,mrigc);
      th->SetEntries(mrigc);
      mmir=mrigc;
      cout<<"mmir="<<mmir<<endl;
     }
    else if(mrigc>mmar&&!ev->isreal){
      TH1D* th=(TH1D *)hman1.Get("mcev_rrawpm");
      th->SetBinContent(2,mrigc);
      th->SetEntries(mrigc);
      mmar=mrigc;
      cout<<"mmar="<<mmar<<endl;
    }
//---End MC

//---Weight factor
  double ww=1;
  static int versww=1;
  if(abs(tk_zm)==1||abs(tk_zm)==2||abs(tk_zm)==3||abs(tk_zm)==4||abs(tk_zm)==5||abs(tk_zm)==6||abs(tk_zm)==7||abs(tk_zm)==8)versww=3;//New-RigScale
  if(!ev->isreal)ww=GetISSTotW(mrigc,-1,1,1,abs(tk_zm),versww);

//---Particle(TOF+Tk)+TOFBeta 
  if(!cutparbeta)return 1;
//---InnerZ 
  if(tk_z1<SCharge)return 1;
//---ISS
  if(ev->isreal)tk_zm=tk_z1;

//---InnerNHit
  if(!cuttkinhits)return 1;

//---L1Inner-Geometry || L1InnerL9-Geometry
/*  if(tk_z1>2){if(!ev->Select_TkGeom(1))return 1;}
  else       {if(!ev->Select_TkGeom(2))return 1;}*/
  if(tk_z1>1){if(!ev->Select_TkGeom(1))return 1;}
  else       {if(!ev->Select_TkGeom(2))return 1;}

///---Preselection
  bool lhxy[9][2]={{0}};
  bool lh[9]={1,1,1,1,1,1,1,1,1};
  float tkscr[9][2]={{0}};//L1+L9 Scattering
  float tkhpr[9][2]={{0}};//Hit-MCParCoo
  float tktmpr[9][2]={{0}};//TrMCCluster-MCParCoo
  for(int il=0;il<9;il++){
    lhxy[il][1] =(ev->HasTkLHitXY(il)>=1);//Y
    lh[il]=lhxy[il][0]=(ev->HasTkLHitXY(il)==2);//X+Y
//---Coordinate
    for(int ic=0;ic<2;ic++){
      int ul=(il==0)?il+1:il-1;
      tkscr[il][ic]=tkcoo[il][ic]-(tkdir[ul][ic]/tkdir[ul][2]*(tkcoo[il][2]-tkcoo[ul][2])+tkcoo[ul][ic]);
      tkscr[il][ic]*=10000.;//um
      if(lhxy[il][ic]){
        tkhpr[il][ic]=ev->tk_hitc[il][ic]-(tkdir[il][ic]/tkdir[il][2]*(ev->tk_hitc[il][2]-tkcoo[il][2])+tkcoo[il][ic]);
        tkhpr[il][ic]*=10000.;//um
//        if(tkcoo[il][2]!=-1000)cout<<"il="<<il<<" dz="<<((ev->mtrcoo1[il][2]-tkcoo[il][2])*10000)<<endl;
        tktmpr[il][ic]=ev->mtrcoo1[il][ic]-(tkdir[il][ic]/tkdir[il][2]*(ev->mtrcoo1[il][2]-tkcoo[il][2])+tkcoo[il][ic]);
        tktmpr[il][ic]*=10000.;//um
      }
    }
//---
  }

//---Inner Hit 
  int inlast=0;
  for(int il=0+1;il<9-1;il++){
     if(lhxy[il][1])inlast=il;
  }
   bool issurvintk=(tkcoo[inlast][2]!=-1000||tkcoo[inlast+1][2]!=-1000||issurvl8); 
   bool issurvintks=(mtlh[inlast]||mtlh[inlast+1]);
//--------
   int mtrzl1=(ev->mtrz[0]&0x3f);
   int mtrzl1p,mtral1;
   ev->GetTrMCClusterZA(ev->mtrpar[0],mtrzl1p,mtral1);
   int mtrzl9=(ev->mtrz[8]&0x3f);
   int mtrzl9p,mtral9;
   ev->GetTrMCClusterZA(ev->mtrpar[8],mtrzl9p,mtral9);
   int mtrzin=(ev->mtrz[inlast]&0x3f);
   int mtrzinp,mtrain;
   ev->GetTrMCClusterZA(ev->mtrpar[inlast],mtrzinp,mtrain);
///---Rigidity
  bool dchiscut=ev->Select_TkL1ChisY(ialgo);
  bool dchiscutl19=ev->Select_TkL19ChisY(ialgo);
  bool dchiscutl=(ev->GetL1ChisY(ialgo)<16);
  double dchisl9=ev->GetL9ChisY(ialgo);
  if(dchisl9<0)dchisl9=-sqrt(fabs(dchisl9));
  else         dchisl9=sqrt(fabs(dchisl9));
//---
  float rigidity[7]={0};
  bool scutrig[7]={0}; 
  double chisv[7]={0};
  for(int ir=0;ir<7;ir++){
    int isp=0;
    if     (ir<=2)isp=ir;//I+L1I+L1IL9(FSGeom)
    else if(ir==3)isp=0;//Inner(L1IGeom)
    else if(ir==6)isp=1;//L1Inner(L1IGeom)
    if(ir<=2||ir==3||ir==6){
      rigidity[ir]=ev->GetRigidity(isp,ialgo);
//      scutrig[ir]=(ev->Select_TkRig(isp,ialgo)&&ev->Select_TkGeom(isp)&&ev->Select_TkExtQ(isp,0,tk_z1,tkqcalib));
      scutrig[ir]=(ev->Select_TkRig(isp,ialgo)&&ev->Select_TkGeom(isp)&&ev->Select_TkExtQ(isp,3,tk_z1,tkqcalib));
      if(isp>=1&&isp<=2&&tk_z1>2)scutrig[ir]=(scutrig[ir]&&ev->IsGoodTkLQStat(0,0));//L1GoodQStat
      if(ir<=2)scutrig[ir]=(scutrig[ir]&&ev->Select_TkGeom(2));//FSGeom
      chisv[ir]=ev->GetChis(isp,1,ialgo);
      if(chisv[ir]>0)chisv[ir]=sqrt(chisv[ir]);
    }
    else if(ir==4||ir==5){
      rigidity[ir]=ev->mcutoffi[1][1];
      if(!ev->isreal)rigidity[ir]=mrig;
//      scutrig[ir]=(scutrig[0]);
      scutrig[ir]=(ev->Select_TkRig(isp,ialgo));
      chisv[ir]=chisv[0];
    }
  }
  float tkl1q=ev->GetTkLQ(0,2,tkqcalib);
  float tkl9q=ev->GetTkLQ(8,2,tkqcalib);
  float tkl9qx=ev->GetTkLQ(8,0,tkqcalib);
  float tkl9qy=ev->GetTkLQ(8,1,tkqcalib);
  bool cutl1hzt=ev->Select_TkExtQ(1,1,tk_z1,tkqcalib);//Normal L1Q cut
  bool cutl9hztt=ev->Select_TkExtQ(3,5,tk_z1,tkqcalib);//Strong L9Q cut
//--
  bool cuttofhztu=(ev->GetTOFUDQ(0)>tk_z1-0.5&&ev->GetTOFUDQ(0)<tk_z1+1);
  bool cuttofhztd=(ev->GetTOFUDQ(1)>tk_z1-0.5&&ev->GetTOFUDQ(1)<tk_z1+1);

//----NonRig Cut
   bool sumnorig=(sumnorigtk&&cuttofqu&&cuttofqd);
   bool sumnorigc=(sumnorigtk&&ev->Select_Tk2nd()&&cuttofhztu);//no dtof
   if(tk_z1>=9)sumnorigc=(sumnorigtk&&cuttofhztu);
   bool sumnorigl=(sumnorigtk&&cuttofqu);
//---All Sel
  for(int uir=0;uir<7;uir++){
//---
    double urig=rigidity[uir];
    int sign=1;
    if(!ev->isreal&&tk_zm<0){urig=-urig;sign=-1;}//Electron
///---InnerZ
    if(tk_z1<SCharge||tk_z1>MCharge+1)continue;
    if(tk_z1==abs(tk_zm)&&sumnorigtk){//TkMeasQ=MCQ
       sprintf(histn,"all_l1r%d_q%d_chis",uir,tk_z1);
       hman1.Fill(histn,urig,chisv[uir]);
    }
//---Chis
    if(!scutrig[uir])continue;
    double uww=(uir==4)?1:ww;
//----
     for(int imr=0;imr<7;imr++){
        if(!scutrig[imr])continue;
//--TkQ
        sprintf(histn,"all_l1r%d_mr%d_q%d",uir,imr,tk_z1);
        if(sumnorigtk)hman1.Fill(histn,urig,uww);
        if(sumnorigtk&&trigp){
          sprintf(histn,"all_l1r%d_mr%d_q%d_t",uir,imr,tk_z1);
          hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_ts",uir,imr,tk_z1);
          if(issurvl9)hman1.Fill(histn,urig,uww); 
        }
//--TkQ&&UTOF&&LTOF
        bool sumnorigu=(imr==2||uir==2)?sumnorig:sumnorigl;
        bool sumrigext=1;
        if(imr==1||imr==6)sumrigext=dchiscut;
#ifndef USEP7CUT
        else if   (imr==2)sumrigext=dchiscutl19;//pass8 cut
#endif
        sprintf(histn,"all_l1r%d_mr%d_q%d_1",uir,imr,tk_z1);
        if(sumnorigu)hman1.Fill(histn,urig,uww);
        sprintf(histn,"all_l1r%d_mr%d_q%d_g",uir,imr,tk_z1);
        if(sumnorigu&&ev->IsPassTkL(8))hman1.Fill(histn,urig,uww);//L9Geom
        if(sumnorigu&&trigp){
          sprintf(histn,"all_l1r%d_mr%d_q%d_1t",uir,imr,tk_z1);
          hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_1time",uir,imr,tk_z1);
          hman1.Fill(histn,rtime);
          sprintf(histn,"all_l1r%d_mr%d_q%d_1cite",uir,imr,tk_z1);
          hman1.Fill(histn,citeid);
          if(imr==2&&uir==2){
            for(int immr=0;immr<3;immr++){
              sprintf(histn,"all_l1r%d_mr%d_q%d_1tm",immr,imr,tk_z1);
              hman1.Fill(histn,rigidity[immr]*sign,uww);
            }
          }
          sprintf(histn,"all_l1r%d_mr%d_q%d_1tt",uir,imr,tk_z1);
          if(cutl9hztt)hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_1tt1",uir,imr,tk_z1);
          if(cuttofhztd)hman1.Fill(histn,urig,uww);
          if(uir==4||uir==2){
            double ptheta=acos(ev->tk_dir[0][2])*180./3.1415926;
            sprintf(histn,"all_l1r%d_mr%d_q%d_1ttheta",uir,imr,tk_z1);
            hman1.Fill(histn,urig,ptheta,uww);
            sprintf(histn,"all_l1r%d_mr%d_q%d_1tthetaw",uir,imr,tk_z1);
            hman1.Fill(histn,urig,ptheta,ww);
          }
          sprintf(histn,"all_l1r%d_mr%d_q%d_1tsi",uir,imr,tk_z1);
          if(issurvintk)hman1.Fill(histn,urig,uww);
          if(issurvl9){
            sprintf(histn,"all_l1r%d_mr%d_q%d_1ts",uir,imr,tk_z1);
            hman1.Fill(histn,urig,uww);
            sprintf(histn,"all_l1r%d_mr%d_q%d_1tts",uir,imr,tk_z1);
            if(cutl9hztt)hman1.Fill(histn,urig,uww);
            sprintf(histn,"all_l1r%d_mr%d_q%d_1tts1",uir,imr,tk_z1);
            if(cuttofhztd)hman1.Fill(histn,urig,uww);
          }
          sprintf(histn,"all_l1r%d_mr%d_q%d_1tss",uir,imr,tk_z1);
          if(mtlh[8])hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_1tofs",uir,imr,tk_z1);
          if(mtofnhitf==4)hman1.Fill(histn,urig,uww);
//----
          if(tk_z1==abs(tk_zm)&&uir==4&&imr==2){
             if(mtlh[8]==0&&mrigc>150&&mrigc<500){
/*            cout<<"run="<<ev->run<<" event="<<ev->event<<" MCRig="<<mrig<<" MaxParE-L9TrMCCl-Mom="<<ev->mtrmom[8];
            cout<<" L9TrMCCl-ParID="<<ev->mtrpar[8]<<" L9Q="<<ev->tk_ql[8]<<" 1st-Interaction-Vertex ZPos="<<ev->mfcoo[2]<<" 2nd-ParMom="<<ev->mpare[0]<<" 2nd-ParID="<<ev->mparp[0]<<" 2nd-ParCharge="<<ev->mparc[0]<<" MaxZ-ParId="<<ev->mparp[1]<<" MaxZ="<<ev->mparc[1]<<" Mom="<<ev->mpare[1]<<endl;*/
             }
             sprintf(histn,"all_l9r_q%d_pid",tk_z1);
             if(ev->mtrpar[8]>=0&&ev->mtrpar[8]<99)hman1.Fill(histn,urig,(ev->mtrpar[8]+0.5),uww);
             else                                  hman1.Fill(histn,urig,99.5,uww);
             if(mtlh[8]){
               for(int il=0;il<9;il++){
                 for(int ic=0;ic<2;ic++){
                   sprintf(histn,"tkresyhit_l%d_r%d_q%d_c%d",il,uir,tk_z1,ic);
                   if(lhxy[il][ic])hman1.Fill(histn,urig,tkhpr[il][ic]);
                   sprintf(histn,"tkresyscat_l%d_r%d_q%d_c%d",il,uir,tk_z1,ic);
                   hman1.Fill(histn,urig,tkscr[il][ic]);
                   sprintf(histn,"tkresytrmc_l%d_r%d_q%d_c%d",il,uir,tk_z1,ic);
                   if(lhxy[il][ic]){
//                     cout<<"il="<<il<<" ic="<<ic<<" dismc="<<tktmpr[il][ic]<<endl;
                     hman1.Fill(histn,urig,tktmpr[il][ic]);
                   }
                 }
               }
             }
           }
//----
       }
//--TkQ&&UTOF&&LTOF&&L1Q(L1Chis-L1I||L1IL9)
        sprintf(histn,"all_l1r%d_mr%d_q%d_5",uir,imr,tk_z1);
        if(sumnorigu&&cutl1hzt&&sumrigext)hman1.Fill(histn,urig,uww);
        if(sumnorigu&&cutl1hzt&&sumrigext&&trigp){
          sprintf(histn,"all_l1r%d_mr%d_q%d_5t",uir,imr,tk_z1);
          hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_5tc",uir,imr,tk_z1);
          if(cuttofhztu)hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_5tw1",uir,imr,tk_z1);
          hman1.Fill(histn,urig,ww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_5ts",uir,imr,tk_z1);
          if(issurvl1)hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_5tk",uir,imr,tk_z1);
          if(mtrzl1==tk_z1)hman1.Fill(histn,urig,uww);
          sprintf(histn,"all_l1r%d_mr%d_q%d_5tsi",uir,imr,tk_z1);
          if(issurvintk)hman1.Fill(histn,urig,uww);
          if(uir==4){
            sprintf(histn,"all_mr%d_q%d_zidl1",imr,tk_z1);
            hman1.Fill(histn,urig,(mtrzl1+0.5),uww);
            sprintf(histn,"all_mr%d_q%d_aidl1k",imr,tk_z1);
            if(mtrzl1==tk_z1)hman1.Fill(histn,urig,(mtral1+0.5),uww);
          }
          if(tk_z1==abs(tk_zm)&&uir==4){
            sprintf(histn,"all_mr%d_q%d_zidl9b%d",imr,tk_z1,(mtlh[8]?0:1));
            hman1.Fill(histn,urig,(mtrzl9+0.5),uww);
            sprintf(histn,"all_mr%d_q%d_aidl9b%d",imr,tk_z1,(mtlh[8]?0:1));
            if(mtrzl9==abs(tk_zm))hman1.Fill(histn,urig,(mtral9+0.5),uww);
            sprintf(histn,"all_mr%d_q%d_zidinb%d",imr,tk_z1,(mtlh[inlast]?0:1));
            hman1.Fill(histn,urig,(mtrzin+0.5),uww);
            sprintf(histn,"all_mr%d_q%d_aidinb%d",imr,tk_z1,(mtlh[inlast]?0:1));
            if(mtrzin==abs(tk_zm))hman1.Fill(histn,urig,(mtrain+0.5),uww);
          }
        }
        if(sumnorigu&&sumrigext&&trigp&&(imr==2||imr==6)&&(uir==imr||uir==4)){//L1Q Distribution-Background
          int ub=(issurvl1)?0:1;//L1Q=tk_zm
          sprintf(histn,"all_l1r%d_mr%d_q%d_5tql1b%d",uir,imr,tk_z1,ub);
          hman1.Fill(histn,urig,tkl1q,uww);
        }
//--TkQ&&UTOF&&2ndTk
        sprintf(histn,"all_l1r%d_mr%d_q%d_3",uir,imr,tk_z1);
        if(sumnorigc)hman1.Fill(histn,urig,uww);
        if(sumnorigc&&trigp){
           sprintf(histn,"all_l1r%d_mr%d_q%d_3t",uir,imr,tk_z1);
           hman1.Fill(histn,urig,uww);
           sprintf(histn,"all_l1r%d_mr%d_q%d_3ts",uir,imr,tk_z1);
           if(issurvintk)hman1.Fill(histn,urig,uww);
        }
//--TkQ&&UTOF&&L1Chis(LTOF&&L9Chis)
        if(((imr==1||imr==6)&&sumnorigl&&dchiscut)||(imr==2&&sumnorig&&dchiscutl19)){
          sprintf(histn,"all_l1r%d_mr%d_q%d_4",uir,imr,tk_z1);
          hman1.Fill(histn,urig,uww);
          if(trigp){
             sprintf(histn,"all_l1r%d_mr%d_q%d_4t",uir,imr,tk_z1);
             hman1.Fill(histn,urig,uww);
             sprintf(histn,"all_l1r%d_mr%d_q%d_4ts",uir,imr,tk_z1);
             if(issurvintk)hman1.Fill(histn,urig,uww);
             sprintf(histn,"all_l1r%d_mr%d_q%d_4tss",uir,imr,tk_z1);
             if(issurvintks)hman1.Fill(histn,urig,uww);
           }
        }
#ifndef USEFAST
//--TKQ&&TOFQ-2d
        if(uir==4&&sumnorigtk&&trigp){
          int ib=issurvl9?0:1;///Background
          sprintf(histn,"all_rr%d_q%d_2dqx9_b%d",imr,tk_z1,ib);
          if(lhxy[8][0])hman1.Fill(histn,urig,tkl9qx,uww);//XQ
          sprintf(histn,"all_rr%d_q%d_2dqy9_b%d",imr,tk_z1,ib);
          if(lhxy[8][1])hman1.Fill(histn,urig,tkl9qy,uww);//XQ
          sprintf(histn,"all_rr%d_q%d_2dqxy9_b%d",imr,tk_z1,ib);
          if(lh[8])hman1.Fill(histn,urig,tkl9q,uww);
//---
          sprintf(histn,"all_rr%d_q%d_2dqx9_b2",imr,tk_z1);
          if(lhxy[8][0])hman1.Fill(histn,urig,tkl9qx,uww);//XQ
          sprintf(histn,"all_rr%d_q%d_2dqy9_b2",imr,tk_z1);
          if(lhxy[8][1])hman1.Fill(histn,urig,tkl9qy,uww);//XQ
          sprintf(histn,"all_rr%d_q%d_2dqxy9_b2",imr,tk_z1);
          if(lh[8])hman1.Fill(histn,urig,tkl9q,uww);
//---
          sprintf(histn,"all_rr%d_q%d_2dchisl9_b%d",imr,tk_z1,ib);
          if(lh[8])hman1.Fill(histn,urig,dchisl9,uww);
          sprintf(histn,"all_rr%d_q%d_2dchisl9_b2",imr,tk_z1);
          if(lh[8])hman1.Fill(histn,urig,dchisl9,uww);
//---
          sprintf(histn,"all_rr%d_q%d_2dtofqu_b%d",imr,tk_z1,ib);
          hman1.Fill(histn,urig,ev->GetTOFUDQ(0),uww);
          sprintf(histn,"all_rr%d_q%d_2dtofqd_b%d",imr,tk_z1,ib);
          hman1.Fill(histn,urig,ev->GetTOFUDQ(1),uww);
          sprintf(histn,"all_rr%d_q%d_2dtofql3_b%d",imr,tk_z1,ib);
          hman1.Fill(histn,urig,ev->GetTOFLQ(2),uww);
//---
        }
#endif
//---
    }
//--Frag Diff L1Z InnerZ
#ifndef USEFAST
    sprintf(histn,"all_l1r%d_q%d_n2dqi",uir,abs(tk_zm));
    if(trigp&&(abs(tk_zm)>=SCharge&&abs(tk_zm)<=MCharge))hman1.Fill(histn,urig,tk_q1,uww);//InQ
    if(sumnorigtk&&tk_z1==abs(tk_zm)&&trigp){
       sprintf(histn,"all_l1r%d_q%d_tofbeta",uir,abs(tk_zm));
       hman1.Fill(histn,urig,ev->tof_betah,uww);//InQ
    }
#endif
  }

  return 0;
}


int Sum1_AMS_CINT(AMSChain *ch,TChain *ch1,const char *outfile,const char *runlogdir=0,Long64_t num=-1){

//--Init
   QEvent::qversion=1; 
//---
   QAnalysisN *analysis=new QAnalysisN(ch1);
   
//----Book Histo
   analysis->BookHistos(outfile);
//----Exclude Bad Run
#ifdef USEP4ISS
   analysis->SelectGoodRun(1);
#else
   analysis->SelectGoodRun();
#endif
///---Process
   int nstat=analysis->ProcessEvents(num);
//----Histo Save
   if(nstat==0){
//----Write/record processed files
     WriteList(runlogdir,analysis->GetQRunManager()->qprocessfile);//write files
//----Histo Save
     analysis->Save();
   }
   else        {cerr<<"Sum1_AMS_CINT::error nstat="<<nstat<<endl;}
   return nstat;
}

