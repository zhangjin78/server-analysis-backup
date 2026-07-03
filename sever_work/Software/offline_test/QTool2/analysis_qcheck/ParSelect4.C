///--Author Q.Yan qyan@cern.ch
///--An Example to Check Tracker Charge Distribution by using QTool
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
#include "fluxweight.C"
#include "B800MCCor.C"
#include "QEvent2.h"
#include "QRTIManager.h"
#include "QAnalysis.h"
#include "QCutoffManager.h"

#define USETTK

using namespace qanalysisconst;

class QAnalysisN: public QAnalysis{
 public:
//----All
#if defined (USEALL)
    static const int SCharge=1;
    static const int MCharge=26;
//----Cl-Fe
#elif defined (USESHZ) || defined (USESHZD)
    static const int SCharge=14;
    static const int MCharge=28;
//----F-S
#elif defined (USEMHZ)
    static const int SCharge=8;
    static const int MCharge=16;
//----P(He)-O
#else
#if defined (USEPROTON)
    static const int SCharge=1;
#else
    static const int SCharge=2;
#endif
//    static const int MCharge=8;
    static const int MCharge=9;
#endif
    HistoMan hman1;
 public:
    unsigned int timebe[2];
    char *runlogname;
 public:
    QAnalysisN(TChain *ch):QAnalysis(ch){timebe[0]=timebe[1]=0;runlogname=0;}
    int BookHistos(const char *ofile);
    int SelectEvent(QEvent *ev0=0,int pos=0);
//    int FillExpoTime(const char *runlogdir);
    int Save(){hman1.Save();hman1.Clear();return 0;}
 public:
    float GetMean(vector<float> signal,float &rms);
};

//----
float QAnalysisN::GetMean(vector<float> signal,float &rms){
  int   n    = 0;
  float mean = 0;
  rms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2);
    n++;
  }
  mean /= n;
  rms  /= n;
  rms = sqrt(rms - mean*mean);
  return mean;
}

//------BookHisto
int QAnalysisN::BookHistos(const char *ofile){

  hman1.Clear();
  hman1.Enable();
  hman1.Setname(ofile);

//---
   char histn[1000];
//--normal bin
   double hebin[1000];
   const int bin=getamsbin(hebin,1)-1;
//--new bin
/*   const int newbinvers=21;
//   const int newbinvers=1;
   double hebin2[1000];
   int bin2=getamsbin(hebin2,newbinvers)-1;
//--new bin3
   const int newbinvers3=31;
   double hebin3[1000];
   int bin3=getamsbin(hebin3,newbinvers3)-1;*/
//--RawEvent
    hman1.Add(new TH1D("mcev_rrawp","mcev_rrawp",2,0,2)); 
//--Exposure
    hman1.Add(new TH1D("ExpoTime","ExpoTime",bin,hebin));
/*    hman1.Add(new TH1D("ExpoTimePr","ExpoTimePr",bin,hebin));//25deg
    hman1.Add(new TH1D("ExpoTime2Pr","ExpoTime2Pr",bin2,hebin2));
    hman1.Add(new TH1D("ExpoTime3Pr","ExpoTime3Pr",bin3,hebin3));
    hman1.Add(new TH1D("ExpoTimeHe","ExpoTimeHe",bin,hebin));//30deg
    hman1.Add(new TH1D("ExpoTime2He","ExpoTime2He",bin2,hebin2));
    hman1.Add(new TH1D("ExpoTime3He","ExpoTime3He",bin3,hebin3));*/
//---
    for(int ich=SCharge;ich<=MCharge;ich++){
      for(int ir=0;ir<5;ir++){
        double qlv=SCharge-3;
        double qhv=MCharge+3;
        int qbinv=(qhv<=12)?470:470+(qhv-12)*30;
        sprintf(histn,"tk_ql0_q%d_atrdu_r%dd",ich,ir);
        hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
        for(int il=0;il<9-1;il++){
          for(int is=0;is<2;is++){
            sprintf(histn,"tk_ql%d_q%d_atrds_r%ds%d",il,ich,ir,is);
            hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            sprintf(histn,"tk_ql%d_q%d_atrdd_r%ds%d",il,ich,ir,is);
            hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            for(int ixy=0;ixy<2;ixy++){
              sprintf(histn,"tk_ql%d_q%d_atrds_r%ddxy%ds%d",il,ich,ir,ixy,is);
              hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            }
         }
       }
       for(int ic=0;ic<6;ic++){
         sprintf(histn,"all_iqr%d_q%d_qis%d",ir,ich,ic);
         hman1.Add(new TH2F(histn,histn,bin,hebin,200,qlv,qhv));
       }
     }
   }

  return 0;
}


//-------------------Select Event
int QAnalysisN::SelectEvent(QEvent *ev,int pos){

   int tkqcalib=0;
#ifdef USENEWTKQ
   tkqcalib=1;
//   tkqcalib=2;
#endif
   int ualgo=0;
#if defined (USEGBL)
   ualgo=1;
#elif defined (USEKALMAN)
   ualgo=2;
#endif
//--------PG/CIEMAT/COMB
#if defined (USEPGONLY)
   uval=0;
#elif defined (USECMONLY)
   uval=1;
//#else
//   uval=2;//by default
#endif
//--MCEvent TotalEvent //run+event
   static int prrun=-1,prev=-1;
   static int prcev=0,ccev;
   if(ev->event<prev||ev->run!=prrun||pos==0){
     cout<<"prrun="<<prrun<<" prev="<<prev<<" nrun="<<ev->run<<" nev="<<ev->event;
     if(prev!=-1){
        ccev=(prev-prcev+1);//Total
        if(prcev<100)ccev=prev;//old
        cout<<" cev="<<ccev;
        hman1.Fill("mcev_rrawp",0,double(ccev));
     }
     cout<<endl;
     prcev=ev->event;
   }
   prrun=ev->run;prev=ev->event;//Latest

//---Weight factor
   int   tk_zm=ev->mch;
   double ww=1;
   static int versww=1;
   if(abs(tk_zm)==1||abs(tk_zm)==2||abs(tk_zm)==3||abs(tk_zm)==4||abs(tk_zm)==5||abs(tk_zm)==6||abs(tk_zm)==7||abs(tk_zm)==8)versww=3;//New-RigScale
//----For MC Reweighted
   double mrig=ev->mmom/ev->mch;
   if(!ev->isreal)ww=GetISSTotW(mrig,-1,1,isbinbybin,abs(tk_zm),versww);

//---Trigger
#ifdef USESHZD
  if(!ev->Select_Trigger(7))return 1;
#else
  if(!ev->Select_Trigger())return 1;
#endif

//---RTI
  if(!ev->Select_RTI())return 1;

//---Particle(TOF+Tk) 
  if(!ev->Select_Particle())return 1;

//---TOFBeta
  if(!ev->Select_TofBeta())return 1;

//---InnerNHit
#ifndef USETTK
  if(!ev->Select_TkInNHit())return 1;
#else
  if(!ev->Select_TkInNHit(2))return 1;
#endif

   TH1D* ht=(TH1D *)hman1.Get("ExpoTime");
   char histn[1000];

///---Preselection
   bool lh[9]={0};
   bool lhqs[9]={0};
   for(int il=0;il<9;il++){
     lh[il]=(ev->HasTkLHitXY(il)==2);
     lhqs[il]=ev->IsGoodTkLQStat(il,0);
   }
//---InnerZ
   float tk_q1=ev->GetTkInQ(2,tkqcalib);
   int  tk_z1=ev->GetTkInZ(2,tkqcalib);
//---L1Q
   int  tk_l1z=ev->GetTkLZ(0,2,tkqcalib);  
   float tkl1qx=ev->GetTkLQ(0,0,tkqcalib);
   float tkl1qy=ev->GetTkLQ(0,1,tkqcalib);
   float tkl1q=ev->GetTkLQ(0,2,tkqcalib);
   float tkl9q=ev->GetTkLQ(8,2,tkqcalib);
   bool cuttkl1qs=(lhqs[0]&&lh[0]);
//--Normal Cut
   bool parqcutu=(ev->Select_TkInQ(0,tk_z1,tkqcalib)&&ev->Select_TofUQ(0,tk_z1));
   bool parqcutud=(parqcutu&&ev->Select_TofDQ(0,tk_z1));
   bool partkcut[5]={0};
   float rigidity[5]={0};
   for(int ir=0;ir<5;ir++){
      int isp=0;
      if     (ir<=2)isp=ir;
      else if(ir==3)isp=0;
      else if(ir==4)isp=1;//L1IG
      int igeom=isp;
      if(ir<=2)igeom=2;//L1IL9G
      rigidity[ir]=ev->GetRigidity(isp,ualgo,uval);
      partkcut[ir]=(ev->Select_TkGeom(igeom)&&ev->Select_TkRig(isp,ualgo,uval,-1,-1));
      if(isp==2)partkcut[ir]=(partkcut[ir]&&ev->Select_TkExtQ(3,0,tk_z1,tkqcalib));
   }

///---L1 Template
//--Tof
   bool tofhu=(ev->tof_hsumhu==4);
   int nqud[2]={0};
   for(int ilay=0;ilay<4;ilay++){//TOF Hit
      if(ev->IsGoodTOFLQStat(ilay)){nqud[ilay/2]++;}
   }
   bool tofgpt=(nqud[0]+nqud[1]==4);//TOF
   bool cuttofchis=((ev->tof_chisc_n<=10)&&(ev->tof_chist_n<=10));//TOFChisCT
   int  utofz=int(ev->GetTOFLQ(0)+0.5);
   bool cuttofqz=(utofz==tk_z1);
   for(int ilay=0;ilay<4;ilay++){
     double tofqcut=(utofz>=9)?0.5:0.4;
     cuttofqz=(cuttofqz&&fabs(ev->GetTOFLQ(ilay)-utofz)<tofqcut);
   }
//---
   float tofbq=0;
   for(int ilay=0;ilay<4;ilay++){
     if(ev->tof_oq[ilay][1]>tofbq){tofbq=ev->tof_oq[ilay][1];}
     if(ev->tof_ob[ilay][0]==-1||ev->tof_barid[ilay]==-1)continue;
     if(abs(ev->tof_barid[ilay]-ev->tof_ob[ilay][0])>1){
         if(ev->tof_oq[ilay][0]>tofbq){tofbq=ev->tof_oq[ilay][0];}
       }
   }
   bool cuttofiso=(tofbq<2);
   bool cutttof=(tofhu&&tofgpt&&cuttofchis&&cuttofqz&&cuttofiso);
//---Tracker
   bool cuttntk=(ev->ntrack<=3&&ev->nbetah==1);
   bool cuttk2nd=ev->Select_Tk2nd();
   bool cutttk1=(cuttk2nd&&cuttntk);
//---Trd
   bool cutttrd=(ev->ntrdtrack==1);
//--
   bool cuttktofq=(cuttk2nd&&ev->GetTOFUDQ(0)>tk_z1-0.5&&ev->GetTOFUDQ(0)<tk_z1+1);
//-----Loose
   bool cutttof2u=(ev->Select_TofUQ(1,tk_z1)&&(ev->GetTOFNGoodQL(0)==2)&&fabs(ev->GetTOFUDQ(0)-tk_z1)<0.5);
   bool cutttof2d=(ev->Select_TofDQ(1,tk_z1)&&(ev->GetTOFNGoodQL(1)==2)&&fabs(ev->GetTOFUDQ(1)-tk_z1)<0.5);
   bool cutttof2=(cutttof2u&&cutttof2d);

///--L0 template
  for(int uir=0;uir<5;uir++){
    if(tk_z1<SCharge||tk_z1>MCharge)continue;
    if(!partkcut[uir])continue;
    if(uir==2){if(!parqcutud)continue;}
    else      {if(!parqcutu)continue;}
    float urig=rigidity[uir];
/*    icffv=(tk_z1<=1)?0:1;
    if(isbinbybin&&ev->isreal){//Bin by bin
       double bincutoff=QCutoffManager::GetHead()->GetBinCutoff(ht,ev->mcutoffi[icffv][1],mcutthu);
       if(fabs(urig)<bincutoff)continue;
    }*/
    if(!cuttkl1qs)continue;//L0Hit
    sprintf(histn,"tk_ql0_q%d_atrdu_r%dd",tk_z1,uir);//L0 normal selection
    hman1.Fill(histn,urig,tkl1q,ww);
    for(int is=0;is<2;is++){
      if(is==0&&ev->isreal){if(!(cutttof&&cutttk1&&cutttrd))continue;}//L0 template tight
      else if(is==1)       {if(!(cutttof2&&cuttk2nd))continue;}//L0 template loose
      sprintf(histn,"tk_ql0_q%d_atrds_r%ds%d",tk_z1,uir,is);
      hman1.Fill(histn,urig,tkl1q,ww);
      for(int ixy=0;ixy<2;ixy++){
        sprintf(histn,"tk_ql0_q%d_atrds_r%ddxy%ds%d",tk_z1,uir,ixy,is);
        hman1.Fill(histn,urig,((ixy==0)?tkl1qx:tkl1qy),ww);
      }
    }
  }
     
  

///---L1-L7 template
///--TkGeometry+InnerHit
  int prez=ev->GetTOFUDZ(0);
  bool tkcut[5]={0};
  for(int ir=0;ir<5;ir++){
    int isp=0;
    if     (ir<=2)isp=ir;
    else if(ir==3)isp=0;
    else if(ir==4)isp=1;
    tkcut[ir]=(ev->Select_TkGeom(isp)&&ev->Select_TkRig(isp,ualgo,uval,-1,-1)&&lh[0]);
    if(ir<=2)tkcut[ir]=(tkcut[ir]&&ev->Select_TkGeom(2));
    if(isp==2){
      if(prez>=9){
        double l19qsigma=0.28+0.08*(prez-9);
        if(tkqcalib>=1)l19qsigma=0.0167*pow(double(prez),1.15)+0.1;
        tkcut[ir]=(tkcut[ir]&&tkl9q>prez-1.7*l19qsigma&&tkl9q<prez+2.5*l19qsigma&&lh[8]);
      }
      else tkcut[ir]=(tkcut[ir]&&tkl9q>prez-0.5&&tkl9q<prez+1.5&&lh[8]);
    }
  }
  bool cuttofqut=(ev->Select_TofUQ(1,prez)&&(ev->GetTOFNGoodQL(0)==2)&&fabs(ev->GetTOFUDQ(0)-prez)<0.5);
  bool cuttofqdt=(ev->Select_TofDQ(1,prez)&&(ev->GetTOFNGoodQL(1)==2)&&fabs(ev->GetTOFUDQ(1)-prez)<0.5);

///--L1-L7 templates
  bool cuttkqi[9]={0};
  bool cuttkqia[9]={0},cuttkqia2[9]={0};
  float tk_qi[9]={0},tk_qrmsi[9]={0};
  for(int il=0+1;il<9-1;il++){
    float tkqln=ev->GetTkLQ(il-1,2,tkqcalib);
    float tkqlp=ev->GetTkLQ(il+1,2,tkqcalib);
    float tkqlcutnp=(prez>=9)?0.5:0.45;
    bool  cutqln=(lh[il-1]&&lhqs[il-1]&&fabs(tkqln-prez)<tkqlcutnp);
    bool  cutqlp=(il==7)?1:(lh[il+1]&&lhqs[il+1]&&fabs(tkqlp-prez)<tkqlcutnp);//the latest inner layer do not use L9
    bool  cutqln2=cutqln;
    bool  cutqlp2=cutqlp;
    if(prez>=9){
      double l19qsigma=0.28+0.08*(prez-9);
      if(tkqcalib>=1)l19qsigma=0.0167*pow(double(prez),1.15)+0.1;
      cutqln2=(lh[il-1]&&lhqs[il-1]&&(tkqln>prez-1.5*l19qsigma)&&(tkqln<prez+0.5));
      cutqlp2=(il==7)?1:(lh[il+1]&&lhqs[il+1]&&(tkqlp>prez-0.5)&&(tkqlp<prez+1.5*l19qsigma)); //the latest inner layer do not use L9
    }
    vector<float>signal;
    for(int il1=0+1;il1<9-1;il1++){
      if(il1==il||!lh[il1])continue;
      float tkql1=ev->GetTkLQ(il1,2,tkqcalib);
      signal.push_back(tkql1);
    }
    tk_qi[il]=GetMean(signal,tk_qrmsi[il]);
    bool cutqii=(signal.size()>=3&&fabs(tk_qi[il]-prez)<0.35&&tk_qrmsi[il]<0.45);
    if(prez>=9){
      cutqii=(signal.size()>=3&&fabs(tk_qi[il]-prez)<0.5&&(tk_qrmsi[il]<0.57+0.1*(prez-9)));
    }
    cuttkqia[il] =(cutqii&&cutqln&&cutqlp);
    cuttkqia2[il]=(cutqii&&cutqln2&&cutqlp2);
  }


  for(int uir=0;uir<5;uir++){
    if(prez<SCharge||prez>MCharge)continue;
    if(!tkcut[uir]||!cuttofqut)continue;
    float urig=rigidity[uir];
/*    icffv=(prez<=1)?0:1;
//--ISS binbybin
    if(isbinbybin&&ev->isreal){//Bin by bin
      double bincutoff=QCutoffManager::GetHead()->GetBinCutoff(ht,ev->mcutoffi[icffv][1],mcutthu);
      if(fabs(urig)<bincutoff)continue;
    }*/
    for(int il=0+1;il<9-1;il++){
       if(!lh[il]||!lhqs[il])continue;//TkU+D QCut
       float tkqlx=ev->GetTkLQ(il,0,tkqcalib);
       float tkqly=ev->GetTkLQ(il,1,tkqcalib);
       float tkql =ev->GetTkLQ(il,2,tkqcalib);
//------
       for(int is=0;is<2;is++){
         if(is==0){if(!cuttkqia[il]) continue;}
         else     {if(!cuttkqia2[il])continue;}
         sprintf(histn,"tk_ql%d_q%d_atrds_r%ds%d",il,prez,uir,is);//L1-L7 template
         hman1.Fill(histn,urig,tkql,ww);
         for(int ixy=0;ixy<2;ixy++){
           sprintf(histn,"tk_ql%d_q%d_atrds_r%ddxy%ds%d",il,prez,uir,ixy,is);//L1 template
           hman1.Fill(histn,urig,((ixy==0)?tkqlx:tkqly),ww);
         }
         if(cuttofqdt){
           sprintf(histn,"tk_ql%d_q%d_atrdd_r%ds%d",il,prez,uir,is);//L1-L7 template with lower TOFQ cut
           hman1.Fill(histn,urig,tkql,ww);
         }
       }
//-------
     }
   }


///InnerQ shape
  int tofz=ev->GetTOFUDZ(0);
  bool cuttofch=1;
  for(int ilay=0;ilay<4;ilay++){
     double tofqcut=(tofz>=9)?0.5:0.4;
     if(fabs(ev->GetTOFLQ(ilay)-tofz)>tofqcut)cuttofch=0;
  }
  bool sumttof=(tofhu&&tofgpt&&cuttofchis&&cuttofch);
  if(tofz>=17)sumttof=(tofhu&&tofgpt&&cuttofch);
//-----
  bool cuttofqutl=(ev->Select_TofUQ(1,tofz)&&(ev->GetTOFNGoodQL(0)==2)&&fabs(ev->GetTOFUDQ(0)-tofz)<0.5);
  bool cuttofqdtl=(ev->Select_TofDQ(1,tofz)&&(ev->GetTOFNGoodQL(1)==2)&&(ev->GetTOFUDQ(1)>tofz-0.5));
///L1Q Match with TOFQ
  bool cuttkl1qtof=cuttkl1qs;
  bool cuttkl1qtof2=(cuttkl1qs&&(tkl1q>tofz-0.2) &&(tkl1q<tofz+0.27));
  bool cuttkl1qtof3=(cuttkl1qs&&(tkl1q>tofz-0.28)&&(tkl1q<tofz+0.35));
  if(tofz>=9){
    double l19qsigma=0.28+0.08*(tofz-9);
    if(tkqcalib>=1)l19qsigma=0.0167*pow(double(tofz),1.15)+0.1;
    cuttkl1qtof2=(cuttkl1qs&&(tkl1q>tofz-0.5)&&(tkl1q<tofz+1.3*l19qsigma));
    cuttkl1qtof3=(cuttkl1qs&&(tkl1q>tofz-1.3*l19qsigma)&&(tkl1q<tofz+1.7*l19qsigma));
  }
  for(int uir=0;uir<5;uir++){
    if(tofz<SCharge||tofz>MCharge)continue;
    if(!partkcut[uir])continue;
//---- 
 /*   icffv=(tofz<=1)?0:1;  
    if(isbinbybin&&ev->isreal){//Bin by bin
       double bincutoff=QCutoffManager::GetHead()->GetBinCutoff(ht,ev->mcutoffi[icffv][1],mcutthu);
       if(fabs(rigidity[uir])<bincutoff)continue;
    }*/
    for(int ic=0;ic<6;ic++){
      if(ic<3){if(!sumttof)continue;}//0,1,2(tightTOF)
      else    {if(!(cuttofqutl&&cuttofqdtl))continue;} //3,4,5 (looseTOF)
//--L0 Charge cut
      bool cuttkl1=cuttkl1qtof;//0,3(no L0)
      if     ((ic%3)==1)cuttkl1=cuttkl1qtof2;//1,4(normal L0)
      else if((ic%3)==2)cuttkl1=cuttkl1qtof3;//2,5(loose L0)
      if(!cuttkl1)continue;
//-----
      sprintf(histn,"all_iqr%d_q%d_qis%d",uir,tofz,ic);
      hman1.Fill(histn,rigidity[uir],tk_q1);
    }
//--    
  }
  

  return 0;
}

/*int QAnalysisN::FillExpoTime(const char *runlogdir){

//---MC return
  if(qrunmanager->qisreal==0)return -1;
//---
  vector<unsigned int> runlist=qrunmanager->qprocessrunlist;
  for(vector<unsigned int>::iterator iter=runlist.begin(); iter!=runlist.end(); iter++ ){
    unsigned int runid=(*iter);
    cout<<"prun="<<runid<<endl;
  }
  if(runlogdir)cout<<"runlogdir="<<runlogdir<<endl;
  qrunmanager->SetFDir(runlogdir,runlogname);
  vector<unsigned int> runlistn=runlist;
 
//---After
  AMSSetupR::RTI::UseLatest();//Pass8 RTI
  char histn[100];
  const int nh0=6;
  TH1D *thn[nh0]={0};
  thn[0]=(TH1D *)hman1.Get("ExpoTimePr");
  thn[1]=(TH1D *)hman1.Get("ExpoTime2Pr");
  thn[2]=(TH1D *)hman1.Get("ExpoTime3Pr");
  thn[3]=(TH1D *)hman1.Get("ExpoTimeHe");
  thn[4]=(TH1D *)hman1.Get("ExpoTime2He");
  thn[5]=(TH1D *)hman1.Get("ExpoTime3He");
  for(vector<unsigned int>::iterator iter=runlistn.begin(); iter!=runlistn.end(); iter++ ){
    unsigned int runid=(*iter);
    unsigned int time[2];
    AMSEventR::GetRTIRunTime(runid,time);
    cout<<"nrun="<<runid<<" time="<<time[0]<<" "<<time[1]<<endl;
    for(unsigned int t=time[0];t<=time[1];t++){
//--RTI Selection
      AMSSetupR::RTI a;
      if(AMSEventR::GetRTI(a,t)!=0)continue; //Time no information
      if(!QRTIManager::Select_RTI(a))continue;
//--Time Cal
      double nt=a.lf*a.nev/(a.nev+a.nerr);
//----Time Cal2+Cal3
      for(int ih=0;ih<nh0;ih++){
        TH1D *thu=thn[ih];
        for(int ibr=1;ibr<=thu->GetNbinsX();ibr++){//Above CutOff Time++
         icffv=(ih<=2)?0:1;
         double urig=mcutthu*a.cfi[icffv][1];//1.2*Cutoff (25+30 degree)
         if(thu->GetBinLowEdge(ibr)>=urig)thu->AddBinContent(ibr,nt);
        }
        thu->SetEntries(thu->GetEntries()+1);
     }
//--- 
    }//end time 
  }//end nrun


//----Write/record processed runs
  qrunmanager->InsertRunList(runlistn);//write runs
  return (qrunmanager->qprocessfile).size();
}
*/

int Sum1_AMS_CINT(AMSChain *ch,TChain *ch1,const char *outfile,const char *runlogdir=0,Long64_t num=-1, QAnalysisN *analysis2=0){

   QAnalysisN *analysis=analysis2;
   if(analysis==0)analysis=new QAnalysisN(ch1);
//----Book Histo
   analysis->BookHistos(outfile);
//----Exclude Bad Run
#if defined (USEP4ISS)
   analysis->SelectGoodRun(1);
#elif defined (USEBEFORENEWEXT)
   analysis->SelectGoodRun(7);
#elif defined (USEAFTERNEWEXT)
//   analysis->SelectGoodRun(70);
   analysis->SelectGoodRun(170);
#elif defined (USESHZD)
   analysis->SelectGoodRun(90);
#elif defined (USESHZDWPHO)
   analysis->SelectGoodRun(190);
#elif defined (WPHO)
   analysis->SelectGoodRun(100);//including polarization-photon runs
#else
   analysis->SelectGoodRun();
#endif
///---Process
   int nstat=analysis->ProcessEvents(num);//nstat=1|=128: assign fatal error
   if(nstat==0){
//----Process Time
//     analysis->FillExpoTime(runlogdir);
//----Write/record processed files
     WriteList(runlogdir,analysis->GetQRunManager()->qprocessfile);//write files
//----Histo Save
     analysis->Save();
   }
   else {cerr<<"Sum1_AMS_CINT::error nstat="<<nstat<<endl;}
   return nstat;
}


