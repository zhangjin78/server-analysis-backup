///--Author Q.Yan qyan@cern.ch
///--An Example of Nuclei Event Selection and Exposure Time Calculation by using QTool
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
#ifdef USEPASS6DST
#include "QEvent.h"
#else
#include "QEvent2.h"
#endif
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
//----Si-Ni
#elif defined (USESHZ) || defined (USESHZD)
    static const int SCharge=14;
    static const int MCharge=28;
//----Li-Ni
#elif defined (USEMHZ)
    static const int SCharge=3;
    static const int MCharge=28;
//----P(He)-O
#else
#if defined (USEPROTON)
    static const int SCharge=1;
#else
    static const int SCharge=2;
#endif
//    static const int MCharge=8;
    static const int MCharge=10;
#endif
    HistoMan hman1;
 public:
    unsigned int timebe[2];
    char *runlogname;
 public:
    QAnalysisN(TChain *ch):QAnalysis(ch){timebe[0]=timebe[1]=0;runlogname=0;}
    int BookHistos(const char *ofile);
    int SelectEvent(QEvent *ev0=0,int pos=0);
    int FillExpoTime(const char *runlogdir);
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
   double hebinv[1000];
   const int binv=getamsbin(hebinv,1,1)-1; 
//--fine bin
   double hebinf[1000];
   const int binf=getamsbin(hebinf,2)-1;
   double hebinfv[1000];
   const int binfv=getamsbin(hebinfv,2,1)-1;
//--new bin
   const int newbinvers=21;//B/C bins
//   const int newbinvers=1;
   double hebin2[1000];
   int bin2=getamsbin(hebin2,newbinvers)-1;
   double hebin2v[1000];
   int bin2v=getamsbin(hebin2v,newbinvers,1)-1;
//--new bin3
   const int newbinvers3=31;
   double hebin3[1000];
   int bin3=getamsbin(hebin3,newbinvers3)-1;
   double hebin3v[1000];
   int bin3v=getamsbin(hebin3v,newbinvers3,1)-1;
//---cos bin
   const int bincos=6;
   double cosbin[1000];
   getcosbin(cosbin,0,20,bincos);
   const int bincos2=50;//0-50deg 50bins
   double cosbin2[1000];
   getcosbin(cosbin2,0,50,bincos2);
//---kinetic-energy bin
   double hebink[1000],hebinkr[1000];
   double hebinkv[1000],hebinkrv[1000];
   double hebink2[1000],hebink2r[1000];//new bin
   double hebink2v[1000],hebink2rv[1000];
   double hebink3[1000],hebink3r[1000];//nnew bin
   double hebink3v[1000],hebink3rv[1000];
   converterbin(hebin,bin,0.5,hebink,0);//zva=0.5 Rig->Kin
   converterbin(hebinv,binv,0.5,hebinkv,0,1);
   converterbin(hebin2,bin2,0.5,hebink2,0);//zva=0.5 Rig->Kin
   converterbin(hebin2v,bin2v,0.5,hebink2v,0,1);
   converterbin(hebin3,bin3,0.5,hebink3,0);//zva=0.5 Rig->Kin
   converterbin(hebin3v,bin3v,0.5,hebink3v,0,1);
//--RawEvent
    hman1.Add(new TH1D("mcev_rrawp","mcev_rrawp",2,0,2)); 
//--Exposure
    hman1.Add(new TH1D("ExpoTime","ExpoTime",bin,hebin));
    hman1.Add(new TH1D("ExpoTimePr","ExpoTimePr",bin,hebin));//25deg
    hman1.Add(new TH1D("ExpoTime2Pr","ExpoTime2Pr",bin2,hebin2));
    hman1.Add(new TH1D("ExpoTime3Pr","ExpoTime3Pr",bin3,hebin3));
    hman1.Add(new TH1D("ExpoTimeHe","ExpoTimeHe",bin,hebin));//30deg
    hman1.Add(new TH1D("ExpoTime2He","ExpoTime2He",bin2,hebin2));
    hman1.Add(new TH1D("ExpoTime3He","ExpoTime3He",bin3,hebin3));
//---
    for(int ich=SCharge;ich<=MCharge;ich++){
      double zva=0.5;
      if     (ich==1) zva=1.;//Proton
      else if(ich==3) zva=3./(0.5*6+0.5*7);//50%Li6+50%Li7
//      else if(ich==4)zva=4./(0.7*7+0.3*9);//70%Be7+30%Be9
      else if(ich==5) zva=5./(0.7*11+0.3*10);//70%B11+30%B10
      else if(ich==7) zva=7./(0.5*14+0.5*15);//50%N14+50%N15
      else if(ich==9) zva=9./19.;//F19
      else if(ich==11)zva=11./23.;//Na23
      else if(ich==13)zva=13./27.;//Al27
      else if(ich==26)zva=26./56.;//Fe56
      else if(ich==28)zva=28./58.;//Ni58
      int uz=ich;
//-------Z/A=0.5(again) Z dependence
      converterbin(hebink,bin,zva,hebinkr,1);//zva=0.5 Kin->Rig
      converterbin(hebinkv,binv,zva,hebinkrv,1,1);
      converterbin(hebink2,bin2,zva,hebink2r,1);//zva=0.5 Kin->Rig
      converterbin(hebink2v,bin2v,zva,hebink2rv,1,1);
      converterbin(hebink3,bin3,zva,hebink3r,1);//zva=0.5 Kin->Rig
      converterbin(hebink3v,bin3v,zva,hebink3rv,1,1);
//--------
      sprintf(histn,"ExpoTime_q%d",ich);
      hman1.Add(new TH1D(histn,histn,bin,hebinkr));
      sprintf(histn,"ExpoTime2_q%d",ich);
      hman1.Add(new TH1D(histn,histn,bin2,hebink2r));
      sprintf(histn,"ExpoTime3_q%d",ich);
      hman1.Add(new TH1D(histn,histn,bin3,hebink3r));
      for(int ir=0;ir<5;ir++){
        sprintf(histn,"all_l1r%d_q%d",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin,hebin));
        sprintf(histn,"all2_l1r%d_q%d",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin,hebin)); 
        sprintf(histn,"all2_l1r%d_q%dtheta",ir,ich);
        hman1.Add(new TH2D(histn,histn,bin,hebin,bincos,cosbin));
        sprintf(histn,"all2_l1r%d_q%dtheta2",ir,ich);
        hman1.Add(new TH2D(histn,histn,bin,hebin,bincos2,cosbin2));
        sprintf(histn,"all2_l1r%d_q%df",ir,ich);
        hman1.Add(new TH1D(histn,histn,binf,hebinf));
        sprintf(histn,"all2_l1r%d_q%dv",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all2_l1r%d_q%dnhitix",ir,ich);
        hman1.Add(new TH2D(histn,histn,bin,hebin,9,0,9));
        sprintf(histn,"all2_l1r%d_q%dnhitiy",ir,ich);
        hman1.Add(new TH2D(histn,histn,bin,hebin,9,0,9));
        sprintf(histn,"all2_l1r%d_q%dvb",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all3_l1r%d_q%dvb",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin2v,hebin2v));
        sprintf(histn,"all4_l1r%d_q%dvb",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin3v,hebin3v));
        sprintf(histn,"all2_l1r%d_q%dvtheta",ir,ich);
        hman1.Add(new TH2D(histn,histn,binv,hebinv,bincos,cosbin));
        sprintf(histn,"all2_l1r%d_q%dvtheta2",ir,ich);
        hman1.Add(new TH2D(histn,histn,binv,hebinv,bincos2,cosbin2));
        sprintf(histn,"all2_l1r%d_q%dvk",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinkrv));
        sprintf(histn,"all3_l1r%d_q%dvk",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin2v,hebink2rv));
        sprintf(histn,"all4_l1r%d_q%dvk",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin3v,hebink3rv));
        sprintf(histn,"all2_l1r%d_q%dvs",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all2_l1r%d_q%dvsb",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all3_l1r%d_q%dvsb",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin2v,hebin2v));
        sprintf(histn,"all4_l1r%d_q%dvsb",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin3v,hebin3v));
        sprintf(histn,"all2_l1r%d_q%dvs1",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all2_l1r%d_q%dvs2",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all2_l1r%d_q%dvs3",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinv));
        sprintf(histn,"all2_l1r%d_q%dvsk",ir,ich);
        hman1.Add(new TH1D(histn,histn,binv,hebinkrv));
        sprintf(histn,"all3_l1r%d_q%dvsk",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin2v,hebink2rv));
        sprintf(histn,"all4_l1r%d_q%dvsk",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin3v,hebink3rv));
        sprintf(histn,"all2_l1r%d_q%dfv",ir,ich);
        hman1.Add(new TH1D(histn,histn,binfv,hebinfv));
        sprintf(histn,"all2_l1r%d_q%d_1",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin,hebin));
        sprintf(histn,"all2_l1r%d_q%ds",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin,hebin));
        sprintf(histn,"all2_l1r%d_q%ds1",ir,ich);
        hman1.Add(new TH1D(histn,histn,bin,hebin));
        double qlv=SCharge-3;
        double qhv=MCharge+3;
        int qbinv=(qhv<=12)?470:470+(qhv-12)*30;
        for(int ib=0;ib<2;ib++){
          sprintf(histn,"tk_ql0_q%d_atrdmc_r%db%d",ich,ir,ib);
          hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
          sprintf(histn,"tk_ql0_q%d_atrdmc_r%db%dw",ich,ir,ib);
          hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
        }
        sprintf(histn,"tk_ql0_q%d_atrdu_r%dd",ich,ir);
        hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
        sprintf(histn,"tk_ql0_q%d_atrdu_r%ddv",ich,ir);
        hman1.Add(new TH2D(histn,histn,binv,hebinv,qbinv,0,qhv));
        sprintf(histn,"tk_ql0_q%d_atrdut_r%dd",ich,ir);
        hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
        sprintf(histn,"tk_ql0_q%d_atrdut_r%ddv",ich,ir);
        hman1.Add(new TH2D(histn,histn,binv,hebinv,qbinv,0,qhv));
        for(int il=0;il<2;il++){
          for(int is=0;is<4;is++){
            char app[10]="";
            if(is>=1)sprintf(app,"s%d",is);
            if(il!=0){if(is>=2)continue;}
            sprintf(histn,"tk_ql%d_q%d_atrds_r%dd%s",il,ich,ir,app);//U+D
            hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            sprintf(histn,"tk_ql%d_q%d_atrdd_r%dd%s",il,ich,ir,app);//U+D
            hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            for(int ib=0;ib<2;ib++){
              sprintf(histn,"tk_ql%d_q%d_atrds_r%db%d%s",il,ich,ir,ib,app);//U+D
              hman1.Add(new TH1D(histn,histn,bin,hebin));
              sprintf(histn,"tk_ql%d_q%d_atrdd_r%db%d%s",il,ich,ir,ib,app);//U+D
              hman1.Add(new TH1D(histn,histn,bin,hebin));
            }
            sprintf(histn,"tk_ql%d_q%d_atrds_r%dd2%s",il,ich,ir,app);//U+D
            hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            if(is!=0)continue;
            for(int ixy=0;ixy<2;ixy++){
              sprintf(histn,"tk_ql%d_q%d_atrds_r%ddxy%d",il,ich,ir,ixy);//U+D
              hman1.Add(new TH2D(histn,histn,bin,hebin,qbinv,0,qhv));
            }
         }
       }
//------
       if(ir==2||ir==4){
         for(int ib=0;ib<2;ib++){
           for(int it=0;it<2;it++){
             for(int is=0;is<=28;is++){
               sprintf(histn,"tql0_q%dr%ds%db%dt%d",ich,ir,is,ib,it);
               hman1.Add(new TH1D(histn,histn,bin,hebin));
             }
           }
           for(int ig=0;ig<2;ig++){
             for(int it=0;it<2;it++){
               for(int is=0;is<=14;is++){
                 sprintf(histn,"tql0_r%ds%db%dg%dt%dd2",ir,is,ib,ig,it);
                 int nb=10;
                 float bl=0,bh=10;
                 if(is>=6)nb=20;
                 if(is==6)bh=20;
                 if(is==5){nb=80;bh=160;}
                 if(is==7||is==8||is==9){nb=50;bh=7;}
                 if(is==10){nb=150;bh=1500;}
                 if(is==11||is==12){nb=50;bl=-5;bh=5;}
                 if(is==13){nb=150;bh=1500;}
                 if(is==14){nb=150,bh=1500;}
                 if(ich==SCharge)hman1.Add(new TH2F(histn,histn,30,0,30,nb,bl,bh));
               }
             }
           }
         }
       }
//-----
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

//---Fill Offline RTI
#ifdef USERTIOffline
//   if(ev->isreal)QRTIManager::GetHead(-1)->Fill_RTI(ev,ev->time[0]);
#endif
//---RTI
  if(!ev->Select_RTI())return 1;

#ifdef USEAlignCUT
   if(ev->isreal){
      if(!QRTIManager::GetHead(-1)->Select_RTIdL1L9(ev->time[0]))return 1;
    }
#endif

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
   int  tk_z1=ev->GetTkInZ(2,tkqcalib);
//   if(tk_z1>=9)tk_z1=ev->GetTOFUDZ(0);
//---L1Q
   int  tk_l1z=ev->GetTkLZ(0,2,tkqcalib);  
   float tkl1qx=ev->GetTkLQ(0,0,tkqcalib);
   float tkl1qy=ev->GetTkLQ(0,1,tkqcalib);
   float tkl1q=ev->GetTkLQ(0,2,tkqcalib);
   double tkl1qxyr=(tkl1qx-tkl1qy)/(tkl1qx+tkl1qy);//(1+f)/(1-f)
   float tkl9q=ev->GetTkLQ(8,2,tkqcalib);
//-----
   bool cutl1qxyr=((tk_z1<=8)?1:(fabs(tkl1qxyr)<0.2));
   bool cutl1pre=(lh[0]&&lhqs[0]&&cutl1qxyr);
#ifdef USETEMPNOLQCUT
   bool cutl1hbz=ev->Select_TkExtQ(1,6,tk_z1,tkqcalib);//BZ
#else
   bool cutl1hbz=ev->Select_TkExtQ(1,2,tk_z1,tkqcalib);//BZ default
#endif
   bool cutl1hzt=ev->Select_TkExtQ(1,1,tk_z1,tkqcalib);//Final
   bool cutl1hbzmc=(tkl1q>2.5&&lh[0]&&lhqs[0]);
   bool issurvl1=(ev->mevmom1[0]!=-1000||ev->mevmom1[1]!=-1000);
//--Normal Cut
   bool parqcutu=(ev->Select_TkInQ(0,tk_z1,tkqcalib)&&ev->Select_TofUQ(0,tk_z1));
   bool parqcutud=(parqcutu&&ev->Select_TofDQ(0,tk_z1));
   bool dchiscut=ev->Select_TkL1ChisY(ualgo,uval);
   bool dchiscutl19=ev->Select_TkL19ChisY(ualgo,uval);
   bool partkcut[5]={0};
   bool exqcut[5]={0};
   bool exqcutt[5]={0};//Tig
   float rigidity[5]={0};
   float chisy[5]={0};
   for(int ir=0;ir<5;ir++){
      int isp=0;
      if     (ir<=2)isp=ir;
      else if(ir==3)isp=0;
      else if(ir==4)isp=1;
      int igeom=isp;
      if(ir<=2)igeom=2;//L1IL9G
      rigidity[ir]=ev->GetRigidity(isp,ualgo,uval);
      chisy[ir]=ev->GetChis(isp,1,ualgo,uval);
      double rawrig=rigidity[ir];
/*#ifdef USENEWSCALE
      int pgcm=2;
//      cout<<"rawrig="<<rawrig<<",";
      double rawrig1=rawrig;
      rawrig=AMSEventR::GetCorrectedRigidity(rigidity[ir],ev->time[0],1,1,2);//rig->raw
      rigidity[ir]=AMSEventR::GetCorrectedRigidity(rawrig,ev->time[0],0,3,pgcm);//raw->rig
//      cout<<"newrig="<<rigidity[ir]<<" drig="<<(1./rawrig1-1./rigidity[ir])*1e3<<endl;
#endif*/
#ifdef USECORGEOMRIG
      rawrig=rigidity[ir];
      if(!(ev->Getamsdn()%100>=68&&uval==2)){//V6-rig will not apply this correction
        rigidity[ir]=AMSEventR::GetCorrectedGeomRigidity(rawrig,igeom,0);
      }
//      cout<<"igeom="<<igeom<<" rawrig="<<rawrig<<" nowrig="<<rigidity[ir]<<endl;
#endif
      partkcut[ir]=(ev->Select_TkGeom(igeom)&&ev->Select_TkRig(isp,ualgo,uval));
      if(isp==2)partkcut[ir]=(partkcut[ir]&&ev->Select_TkExtQ(3,0,tk_z1,tkqcalib));
      exqcut[ir]=ev->Select_TkExtQ(isp,0,tk_z1,tkqcalib);
      exqcutt[ir]=ev->Select_TkExtQ(isp,1,tk_z1,tkqcalib);//Tighy L1Q&&Stat
   }
   int nhitix=ev->GetTkInNHit(0);
   int nhitiy=ev->GetTkInNHit(1);

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
   float tofbq[2]={0};
   for(int ilay=0;ilay<4;ilay++){
     if(ev->tof_oq[ilay][0]>tofbq[1]){tofbq[1]=ev->tof_oq[ilay][0];}
     if(ev->tof_oq[ilay][1]>tofbq[0]){tofbq[0]=ev->tof_oq[ilay][1];}
     if(ev->tof_ob[ilay][0]==-1||ev->tof_barid[ilay]==-1)continue;
     if(abs(ev->tof_barid[ilay]-ev->tof_ob[ilay][0])>1){
         if(ev->tof_oq[ilay][0]>tofbq[0]){tofbq[0]=ev->tof_oq[ilay][0];}
       }
   }
   double ww0=0,ww1=1;
   double riglh[2]={4,20};
   double crig=fabs(rigidity[0]);
   if(fabs(crig)<=riglh[0]){ww0=1;ww1=0;}
   else if(fabs(crig)<riglh[1]){
     double lcrig=log(crig), lriglh0=log(riglh[0]), lriglh1=log(riglh[1]);
     ww0=(lriglh1-lcrig)/(lriglh1-lriglh0);
     ww1=(lcrig-lriglh0)/(lriglh1-lriglh0);
   }
//-----
   double cz=(tk_z1>=3)?tk_z1:3;
///
   double mtofoq=1.078+0.01305*pow(cz,1.535);
   double stofoq=0.3691+2.26e-5*pow(cz,2.753);
   bool cuttofiso2=(tofbq[0]<mtofoq+stofoq*1.5);
   bool cuttofiso2a=(tofbq[0]<mtofoq+stofoq*(-0.5*ww0+1.5*ww1));//should use
///
   double mtofoq3=1.017+0.04674*pow(cz,1.326);
   double stofoq3=0.381+0.001083*pow(cz,1.895);
   bool cuttofiso3=(tofbq[1]<mtofoq3+stofoq3*1.5);//maxisoq
   bool cuttofiso3a=(tofbq[1]<mtofoq3+stofoq3*(-0.5*ww0+1.5*ww1));//maxisoq
///
   bool cutttof=(tofhu&&tofgpt&&cuttofchis&&cuttofqz&&cuttofiso2a&&cuttofiso3a);
///
   bool cuttofqz2u=(ev->Select_TofUQ(1,tk_z1)&&fabs(ev->GetTOFUDQ(0)-tk_z1)<0.5);
   bool cuttofqz2d=(ev->Select_TofDQ(1,tk_z1)&&fabs(ev->GetTOFUDQ(1)-tk_z1)<0.5);
   bool cutttof2=(tofgpt&&cuttofqz2u&&cuttofqz2d);
   bool cutttofa=(tofhu&&tofgpt&&cuttofchis&&cuttofqz2u&&cuttofqz2d&&cuttofiso2a);
//---Tracker
   bool cutnbetah=1;
   if     (tk_z1<=5)cutnbetah=(ev->nbetah==1);
   else if(tk_z1<=7)cutnbetah=(ev->nbetah<=2);
   else if(tk_z1<=8)cutnbetah=(ev->nbetah<=3);
   bool cutntk=(ev->ntrack<=3&&cutnbetah);
   double mtkrcl=52.19+2.435*pow(cz,1.824)+10;
   double stkrcl=7.781+1.242*pow(cz,1.524);
   bool cutntk2=(ev->ntrrawcl<mtkrcl+stkrcl*1.);//rawcl
///   
   bool cuttk2nd=ev->Select_Tk2nd();
   bool cuttk2nd1=ev->Select_Tk2nd(1);
   bool cuttk2nd2=ev->Select_Tk2nd(2);
///
   double mtkoq=0.04912*(cz-3)+0.877;
   double stkoq=0.18;
   double vtkoq=mtkoq+stkoq*1.5;
   if(vtkoq<1.5)vtkoq=1.5;
   bool cuttkoq=(ev->betah2q<vtkoq);//2ndtk q
   bool cutttk=(tk_z1>8)?(cuttk2nd&&cuttkoq&&cutntk2):(cuttk2nd&&cuttkoq&&cutntk2&&cutntk);
   bool cutttka=(cuttk2nd&&cuttkoq);
//---Trd
   double mtrdh=-2.312+16.75*pow(cz,1.192)+15.;
   double strdh=-67.45+58.61*pow(cz,0.2888);//sigma
   bool cutntrd2=(ev->ntrdrawh<mtrdh+strdh*1.);//rawhit should use
///
   double mtrds=-10.22+5.302*pow(cz,0.8582)+4.;
   double strds=0.1872*(cz-3)+4.74;
   bool cutntrd3=(ev->ntrdseg<mtrds+strds*1.);//segment
///
   double mtrdq=-0.02952*(cz-3)-0.014;
   double strdq=0.01422*(cz-3)+0.923;
   bool cuttrdq=(ev->trd_qk[4]-tk_z1<mtrdq+strdq*0.8);//trd2nd q
   bool cuttrdq2=(tk_z1>=6)?1:(ev->trd_qk[3]-tk_z1<0.5);//trd q
///
   bool cutttrd=(tk_z1>8)?(cuttrdq&&cutntrd2&&cutntrd3):(cuttrdq&&cuttrdq2&&cutntrd2&&cutntrd3&&ev->ntrdtrack==1);
   bool cutttrda=(cuttrdq&&cutntrd2);

//--
   bool cuttktofq=(cuttk2nd&&ev->GetTOFUDQ(0)>tk_z1-0.5&&ev->GetTOFUDQ(0)<tk_z1+1);

///--Fill
  for(int uir=0;uir<5;uir++){
    if(tk_z1<SCharge||tk_z1>MCharge)continue;
    if(!partkcut[uir])continue;
    if(uir==2){if(!parqcutud)continue;}
    else      {if(!parqcutu)continue;}
    float urig=rigidity[uir];
    if(exqcut[uir]){
      sprintf(histn,"all_l1r%d_q%d",uir,tk_z1);
      hman1.Fill(histn,urig,ww);
    }
#ifdef USEP7CUT
    icffv=(tk_z1<=1)?0:3;
#else
    icffv=(tk_z1<=1)?0:1;
#endif
//---Kinetic-cutoff bin by bin(or new bin)
    for(int ih=0;ih<3;ih++){ 
      for(int ik=0;ik<2;ik++){
        bool cutoffpz=1;
        if(isbinbybin&&ev->isreal){//Bin by bin
          if(ik==0&&ih==0)sprintf(histn,"ExpoTimeHe");
          if(ik==0&&ih==1)sprintf(histn,"ExpoTime2He");
          if(ik==0&&ih==2)sprintf(histn,"ExpoTime3He");
          if(ik==1&&ih==0)sprintf(histn,"ExpoTime_q%d",tk_z1);
          if(ik==1&&ih==1)sprintf(histn,"ExpoTime2_q%d",tk_z1);
          if(ik==1&&ih==2)sprintf(histn,"ExpoTime3_q%d",tk_z1);
          TH1 *htn=(TH1 *)hman1.Get(histn);
          double bincutoff=QCutoffManager::GetHead()->GetBinCutoff(htn,ev->mcutoffi[icffv][1],mcutthu);
          cutoffpz=(fabs(urig)>=bincutoff);
        }
        if(cutoffpz&&exqcutt[uir]){//External Q
          if(ik==0)sprintf(histn,"all%d_l1r%d_q%dvb",2+ih,uir,tk_z1);
          else     sprintf(histn,"all%d_l1r%d_q%dvk",2+ih,uir,tk_z1);
          hman1.Fill(histn,1./urig,ww);
          if(((uir==1||uir==4)&&dchiscut)||(uir==2&&dchiscutl19)){
           if(ik==0)sprintf(histn,"all%d_l1r%d_q%dvsb",2+ih,uir,tk_z1);
           else     sprintf(histn,"all%d_l1r%d_q%dvsk",2+ih,uir,tk_z1);
           hman1.Fill(histn,1./urig,ww);
          }
        }
      }
    }
    if(isbinbybin&&ev->isreal){//Bin by bin
       double bincutoff=QCutoffManager::GetHead()->GetBinCutoff(ht,ev->mcutoffi[icffv][1],mcutthu);
       if(fabs(urig)<bincutoff)continue;
    }
//----External Q
    if(exqcut[uir]){//L1Q>InZ&&L1Q<InZ+1.5 
       sprintf(histn,"all2_l1r%d_q%d",uir,tk_z1);
       hman1.Fill(histn,urig,ww);
       sprintf(histn,"all2_l1r%d_q%ds",uir,tk_z1);
       if(lhqs[0])hman1.Fill(histn,urig,ww);//L1Q>InZ&&L1Q<InZ+1.5&&QStat
       sprintf(histn,"all2_l1r%d_q%ds1",uir,tk_z1);
       if(ev->IsGoodTkLQStat(0,1))hman1.Fill(histn,urig,ww);//L1Q>InZ&&L1Q<InZ+1.5&&QStat1
    }
//----External TightQ
    if(exqcutt[uir]){//L1Q>InZ&&L1Q<InZ+0.65&&QStat(Z>=3)
      double ptheta=acos(fabs(ev->tk_dir[0][2]))*180./3.1415926;
      sprintf(histn,"all2_l1r%d_q%dtheta",uir,tk_z1);
      hman1.Fill(histn,urig,ptheta,ww);
      sprintf(histn,"all2_l1r%d_q%dtheta2",uir,tk_z1);
      hman1.Fill(histn,urig,ptheta,ww);
      sprintf(histn,"all2_l1r%d_q%df",uir,tk_z1);
      hman1.Fill(histn,urig,ww);
      sprintf(histn,"all2_l1r%d_q%dv",uir,tk_z1);
      hman1.Fill(histn,1./urig,ww);
      sprintf(histn,"all2_l1r%d_q%dnhitix",uir,tk_z1);
      hman1.Fill(histn,urig,nhitix,ww);
      sprintf(histn,"all2_l1r%d_q%dnhitiy",uir,tk_z1);
      hman1.Fill(histn,urig,nhitiy,ww);
#ifdef USEZ26OUT
/*      if(tk_z1==1&&urig>=20&&(uir==0)){
        if(uir==0)cout<<"InnZ1"<<" qid="<<qid<<" run="<<ev->run<<" event="<<ev->event<<endl;
      }*/
      if(tk_z1==2&&fabs(urig)>=500&&(uir==4)){
        cout<<"L1IZ2"<<" qid="<<qid<<" run="<<ev->run<<" event="<<ev->event<<endl;
      }
/*      if(tk_z1==26&&urig>=20&&(uir==3||uir==4)){
        if(uir==3)cout<<"InnZ26"<<" qid="<<qid<<" run="<<ev->run<<" event="<<ev->event<<endl;
        if(uir==4)cout<<"L1IZ26"<<" qid="<<qid<<" run="<<ev->run<<" event="<<ev->event<<endl;
      }*/
/*      if(tk_z1==2&&urig>=20&&(uir==3||uir==4)){
        if(uir==3)cout<<"InnZ26"<<" qid="<<qid<<" run="<<ev->run<<" event="<<ev->event<<endl;
        if(uir==4)cout<<"L1IZ26"<<" qid="<<qid<<" run="<<ev->run<<" event="<<ev->event<<endl;
      }*/
#endif
      sprintf(histn,"all2_l1r%d_q%dvtheta",uir,tk_z1);
      hman1.Fill(histn,1./urig,ptheta,ww);
      sprintf(histn,"all2_l1r%d_q%dvtheta2",uir,tk_z1);
      hman1.Fill(histn,1./urig,ptheta,ww);
      sprintf(histn,"all2_l1r%d_q%dvs",uir,tk_z1);
      if     (uir==0||uir==3){if(cuttktofq)hman1.Fill(histn,1./urig,ww);}
      else if(((uir==1||uir==4)&&dchiscut)||(uir==2&&dchiscutl19))hman1.Fill(histn,1./urig,ww);
#ifdef USEEVENTOUT
      if(tk_z1==6&&urig>300){
       int sw=12;
       cout.precision(6);
       static const char *span[]={"L1IL9","L1I"};
       if((uir==2)||(uir==4&&dchiscut)){
          cout<<std::setw(sw)<<span[uir==2?0:1]<<std::setw(sw)<<ev->run<<std::setw(sw)<<ev->event<<std::setw(sw)<<urig<<std::setw(sw)<<chisy[uir]<<endl;
       }
      }
#endif
      if(uir==2){
        for(int imr=0;imr<3;imr++){
          sprintf(histn,"all2_l1r%d_q%dvs1",imr,tk_z1);
          float urigm=ev->GetRigidity(imr,ualgo,uval);
          hman1.Fill(histn,1./urigm,ww);//Innner
          if(partkcut[imr]){
             sprintf(histn,"all2_l1r%d_q%dvs2",imr,tk_z1);
             hman1.Fill(histn,1./urigm,ww);//Innner
             sprintf(histn,"all2_l1r%d_q%dvs3",imr,tk_z1);
             hman1.Fill(histn,1./urig,ww);//Innner
          }
        }
      }
      sprintf(histn,"all2_l1r%d_q%dfv",uir,tk_z1);
      hman1.Fill(histn,1./urig,ww);
    }
//---MC(L1Q)
    if(!ev->isreal&&cutl1hbzmc){
      int ub=(issurvl1)?0:1;
      sprintf(histn,"tk_ql0_q%d_atrdmc_r%db%d",tk_z1,uir,ub);
      hman1.Fill(histn,mrig,tkl1q);
      sprintf(histn,"tk_ql0_q%d_atrdmc_r%db%dw",tk_z1,uir,ub);
      hman1.Fill(histn,urig,tkl1q,ww);
    }
//---L1BZQ
    if(!cutl1hbz)continue;//L1Q>InZ
//----L1BZ
    if(lhqs[0]) {
      sprintf(histn,"all2_l1r%d_q%d_1",uir,tk_z1);//L1Q>InZ&&QStat 
      hman1.Fill(histn,urig,ww);
      sprintf(histn,"tk_ql0_q%d_atrdu_r%dd",tk_z1,uir);
      hman1.Fill(histn,urig,tkl1q,ww);
      sprintf(histn,"tk_ql0_q%d_atrdu_r%ddv",tk_z1,uir);
      hman1.Fill(histn,1./urig,tkl1q,ww);
      if(((uir==1||uir==4)&&dchiscut)||(uir==2&&dchiscutl19)){
        sprintf(histn,"tk_ql0_q%d_atrdut_r%dd",tk_z1,uir);
        hman1.Fill(histn,urig,tkl1q,ww);
        sprintf(histn,"tk_ql0_q%d_atrdut_r%ddv",tk_z1,uir);
        hman1.Fill(histn,1./urig,tkl1q,ww);
      }
      bool dchiscutex=1;
      if(uir==1||uir==4)dchiscutex=dchiscut;
      else if   (uir==2)dchiscutex=dchiscutl19;
      for(int is=0;is<4;is++){
        if(is==0&&ev->isreal){if(!(cutttof&&cutttk&&cutttrd))continue;}
        else if(is==1)       {if(!(cutttof2&&cuttk2nd))continue;}
        else if(is==2)       {if(!(cutttofa&&cutttka&&cutttrda&&cuttofiso3a&&cutntk2&&cutntrd3))continue;}
        else if(is==3)       {if(!(cutttofa&&cutttka&&cutttrda))continue;}
        if(!dchiscutex)continue;
        char app[10]="";
        if(is>=1)sprintf(app,"s%d",is);
        sprintf(histn,"tk_ql0_q%d_atrds_r%dd%s",tk_z1,uir,app);//U+D
        hman1.Fill(histn,urig,tkl1q,ww);
        sprintf(histn,"tk_ql0_q%d_atrds_r%db0%s",tk_z1,uir,app);//U+D
        hman1.Fill(histn,urig,ww);
        sprintf(histn,"tk_ql0_q%d_atrds_r%db1%s",tk_z1,uir,app);//U+D
        if(cutl1hzt)hman1.Fill(histn,urig,ww);
      }
//----L0 template efficiency
      int ib=-1;
      double bvcut=(tk_z1<=13)?1.:1.5;
      if(tkl1q>tk_z1+bvcut)ib=1;//back
      else                 ib=0;//signal
      if(dchiscutex&&(uir==2||uir==4)&&ib>=0){
        for(int it=0;it<2;it++){
          if(it==1){if(!(cutttrda&&cutttofa&&cutttka))continue;}
          for(int is=0;is<=28;is++){
            bool pcut=1;
///trd
            if     (is==1)pcut=(ev->ntrdtrack==1);
            else if(is==2)pcut=cutntrd2;
            else if(is==3)pcut=cutntrd3;
            else if(is==4)pcut=cuttrdq;
            else if(is==5)pcut=cuttrdq2;
            else if(is==6)pcut=cutttrda;
            else if(is==7)pcut=cutttrd;
//tof
            else if(is==8)pcut=tofhu;
            else if(is==9)pcut=tofgpt;
            else if(is==10)pcut=cuttofchis;
            else if(is==11)pcut=cuttofqz;
            else if(is==12)pcut=cuttofiso2;
            else if(is==13)pcut=cuttofiso2a;
            else if(is==14)pcut=cuttofiso3;
            else if(is==15)pcut=cuttofiso3a;
            else if(is==16)pcut=cutttof2;
            else if(is==17)pcut=cutttofa;
            else if(is==18)pcut=cutttof;
//tracker
            else if(is==19)pcut=cuttk2nd;
            else if(is==20)pcut=cuttk2nd1;
            else if(is==21)pcut=cuttk2nd2;
            else if(is==22)pcut=(ev->ntrack<=3);
            else if(is==23)pcut=cutnbetah;
            else if(is==24)pcut=cutntk2;
            else if(is==25)pcut=cuttkoq;
            else if(is==26)pcut=cutttka;
            else if(is==27)pcut=cutttk;
            else if(is==28)pcut=(cutttofa&&cutttka&&cutttrda);
//----
            if(!pcut)continue;
            sprintf(histn,"tql0_q%dr%ds%db%dt%d",tk_z1,uir,is,ib,it);//U+D
            hman1.Fill(histn,urig,ww);
          }
        }
        int ig=-1;
        if     (fabs(urig)>20)ig=0;
        else if(fabs(urig)<7)ig=1;
        for(int it=0;it<2;it++){
          if(ig<0)continue;//rig-range
          if(it==1){if(!(cutttrda&&cutttofa&&cutttka))continue;}
          for(int is=0;is<=14;is++){
            float uvar=ev->ntrack;
            if(is==1)uvar=ev->nbetah;
            else if(is==2)uvar=ev->ntrdtrack;
            else if(is==3)uvar=ev->tof_hsumhu;
            else if(is==4)uvar=nqud[0]+nqud[1];
            else if(is==5){uvar=ev->ntrdseg;}
            else if(is==6){if(!cutttof2)continue;uvar=ev->tof_nclhl[0]+ev->tof_nclhl[1]-2;}
            else if(is==7)uvar=ev->betah2q;
            else if(is==8){if(!cutttof2)continue;uvar=tofbq[0];}
            else if(is==9){if(!cutttof2)continue;uvar=tofbq[1];}
            else if(is==10){uvar=ev->ntrdrawh;}
            else if(is==11){uvar=(ev->trd_qk[0]!=0)?ev->trd_qk[4]-tk_z1:-4;}
            else if(is==12){uvar=(ev->trd_qk[0]!=0)?ev->trd_qk[3]-tk_z1:-4;}
            else if(is==13){uvar=ev->ntrrawcl;}
            else if(is==14){uvar=ev->nrichh;}
            sprintf(histn,"tql0_r%ds%db%dg%dt%dd2",uir,is,ib,ig,it);//U+D
            hman1.Fill(histn,tk_z1,uvar);
          }
        }
      }
//----
    }
  }
     
  

////---Template
///--TkGeometry+InnerHit
//  int prez=tk_l1z;
  int prez=ev->GetTOFUDZ(0);
  bool tkcut[5]={0};
  for(int ir=0;ir<5;ir++){
    int isp=0;
    if     (ir<=2)isp=ir;
    else if(ir==3)isp=0;
    else if(ir==4)isp=1;
    tkcut[ir]=(ev->Select_TkGeom(isp)&&ev->Select_TkRig(isp,ualgo,uval)&&lh[0]);
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

///--Inner Template without This Layer
  bool cuttkqi[9]={0};
  bool cuttkqia[9]={0},cuttkqia2[9]={0};
  float tk_qi[9]={0},tk_qrmsi[9]={0};
  for(int il=0+1;il<9-1;il++){
    float tkqln=ev->GetTkLQ(il-1,2,tkqcalib);
    float tkqlp=ev->GetTkLQ(il+1,2,tkqcalib);
    float tkqlcutnp=(prez>=9)?0.5:0.45;
    bool  cutqln=(lh[il-1]&&lhqs[il-1]&&fabs(tkqln-prez)<tkqlcutnp);
    bool  cutqlp=(lh[il+1]&&lhqs[il+1]&&fabs(tkqlp-prez)<tkqlcutnp);
    bool  cutqln2=cutqln;
    bool  cutqlp2=cutqlp;
    if(prez>=9){
      double l19qsigma=0.28+0.08*(prez-9);
      if(tkqcalib>=1)l19qsigma=0.0167*pow(double(prez),1.15)+0.1;
      cutqln2=(lh[il-1]&&lhqs[il-1]&&(tkqln>prez-1.5*l19qsigma)&&(tkqln<prez+0.5));
      cutqlp2=(lh[il+1]&&lhqs[il+1]&&(tkqlp>prez-0.5)&&(tkqlp<prez+1.5*l19qsigma)); 
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


//---All Sel
  for(int uir=0;uir<5;uir++){
    if(prez<SCharge||prez>MCharge)continue;
    if(!tkcut[uir]||!cuttofqut)continue;
    float urig=rigidity[uir];
#ifdef USEP7CUT
    icffv=(prez<=1)?0:3;
#else
    icffv=(prez<=1)?0:1;
#endif
//--ISS binbybin
    if(isbinbybin&&ev->isreal){//Bin by bin
      double bincutoff=QCutoffManager::GetHead()->GetBinCutoff(ht,ev->mcutoffi[icffv][1],mcutthu);
      if(fabs(urig)<bincutoff)continue;
    }
    for(int il=0+1;il<2;il++){
       if(!lh[il]||!lhqs[il])continue;//TkU+D QCut
       float tkqlx=ev->GetTkLQ(il,0,tkqcalib);
       float tkqly=ev->GetTkLQ(il,1,tkqcalib);
       float tkql =ev->GetTkLQ(il,2,tkqcalib);
       double tkqlxyr=(tkqlx-tkqly)/(tkqlx+tkqly);//(1+f)/(1-f)
       bool cutqlxyr=1;
       bool cutbz =(prez<=2)?(tkql>prez-0.4) :(tkql>prez-0.46-(prez-3)*0.16);
       bool cutbac=(prez<=5)?(tkql<prez+0.65):(tkql<prez+0.65+(prez-5)*0.03);
       if(prez>=9){
         cutqlxyr=(fabs(tkqlxyr)<0.2);
         double l19qsigma=0.28+0.08*(prez-9);
         if(tkqcalib>=1)l19qsigma=0.0167*pow(double(prez),1.15)+0.1;
         cutbz =(tkql>prez-3.5*l19qsigma);
         cutbac=(tkql<prez+2.0*l19qsigma);
       }
       double dqlxysigma=3.80718e-20*pow(tkql,13.5362)+0.546714;
       bool cutqlxyrt=(fabs(tkqlx-tkqly)<1.5*dqlxysigma);
//------
       for(int is=0;is<2;is++){
         if(is==0){if(!cutqlxyr||!cuttkqia[il]) continue;}
         else     {if(!cutqlxyr||!cuttkqia2[il])continue;}
         char app[10]="";
         if(is>=1)sprintf(app,"s%d",is);
         sprintf(histn,"tk_ql%d_q%d_atrds_r%dd%s",il,prez,uir,app);//U+D
         hman1.Fill(histn,urig,tkql,ww);
         sprintf(histn,"tk_ql%d_q%d_atrds_r%db0%s",il,prez,uir,app);//U+D
         if(cutbz)hman1.Fill(histn,urig,ww);
         sprintf(histn,"tk_ql%d_q%d_atrds_r%db1%s",il,prez,uir,app);//U+D
         if(cutbz&&cutbac)hman1.Fill(histn,urig,ww);
         if(cuttofqdt){
           sprintf(histn,"tk_ql%d_q%d_atrdd_r%dd%s",il,prez,uir,app);//U+D
           hman1.Fill(histn,urig,tkql,ww);
           sprintf(histn,"tk_ql%d_q%d_atrdd_r%db0%s",il,prez,uir,app);//U+D
           if(cutbz)hman1.Fill(histn,urig,ww);
           sprintf(histn,"tk_ql%d_q%d_atrdd_r%db1%s",il,prez,uir,app);//U+D
           if(cutbz&&cutbac)hman1.Fill(histn,urig,ww);
         }
//------
         sprintf(histn,"tk_ql%d_q%d_atrds_r%dd2%s",il,prez,uir,app);//U+D
         if(cutqlxyrt)hman1.Fill(histn,urig,tkql,ww);
         if(is!=0)continue;
         for(int ixy=0;ixy<2;ixy++){
           sprintf(histn,"tk_ql%d_q%d_atrds_r%ddxy%d",il,prez,uir,ixy);//U+D
           hman1.Fill(histn,urig,((ixy==0)?tkqlx:tkqly),ww);
         }
       }
//-------
     }
   }

  return 0;
}

int QAnalysisN::FillExpoTime(const char *runlogdir){

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
/*  vector<unsigned int> runlistn;
  qrunmanager->GetNewRunList(runlist,runlistn);
  if(runlistn.size()!=runlist.size()){cout<<"runlist diff="<<runlist.size()<<","<<runlistn.size()<<endl;}*/
 
//---After
#ifdef USEP7CUT
  AMSSetupR::RTI::UseLatest(7);//Pass7 RTI
#else
  AMSSetupR::RTI::UseLatest();//Pass8 RTI
#endif
  char histn[100];
  const int nh0=6;
  TH1D *thn[nh0]={0};
  thn[0]=(TH1D *)hman1.Get("ExpoTimePr");
  thn[1]=(TH1D *)hman1.Get("ExpoTime2Pr");
  thn[2]=(TH1D *)hman1.Get("ExpoTime3Pr");
  thn[3]=(TH1D *)hman1.Get("ExpoTimeHe");
  thn[4]=(TH1D *)hman1.Get("ExpoTime2He");
  thn[5]=(TH1D *)hman1.Get("ExpoTime3He");
  const int nh=3;
  TH1D *thz[nh][MCharge+1]={{0}};
  for(int ich=SCharge;ich<=MCharge;ich++){
    for(int ih=0;ih<nh;ih++){
      if     (ih==0)sprintf(histn,"ExpoTime_q%d",ich);
      else if(ih==1)sprintf(histn,"ExpoTime2_q%d",ich);
      else          sprintf(histn,"ExpoTime3_q%d",ich);
      thz[ih][ich]=(TH1D *)hman1.Get(histn);//Z-depends cutoff
    }
  }
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
#ifdef USEAlignCUT
      if(!QRTIManager::Select_RTIdL1L9(t))continue;
#endif
//--Time Cal
      double nt=a.lf*a.nev/(a.nev+a.nerr);
//----Time Cal2+Cal3
      for(int ih=0;ih<nh0;ih++){
        TH1D *thu=thn[ih];
        for(int ibr=1;ibr<=thu->GetNbinsX();ibr++){//Above CutOff Time++
#ifdef USEP7CUT
         icffv=(ih<=2)?0:3;
#else
         icffv=(ih<=2)?0:1;
#endif
         double urig=mcutthu*a.cfi[icffv][1];//1.2*Cutoff (25+30 degree)
         if(thu->GetBinLowEdge(ibr)>=urig)thu->AddBinContent(ibr,nt);
        }
        thu->SetEntries(thu->GetEntries()+1);
     }
//----Time Cal2+Cal3
     for(int ih=0;ih<nh;ih++){
      for(int ich=SCharge;ich<=MCharge;ich++){
        TH1D *thu=thz[ih][ich];
        if(!thu)continue;
        for(int ibr=1;ibr<=thu->GetNbinsX();ibr++){//Above CutOff Time++
#ifdef USEP7CUT
          icffv=(ich<=1)?0:3;
#else
          icffv=(ich<=1)?0:1;
#endif
          double urig=mcutthu*a.cfi[icffv][1];//1.2*Cutoff (Positive 25+30 degree)
          if(thu->GetBinLowEdge(ibr)>=urig)thu->AddBinContent(ibr,nt);
        }
        thu->SetEntries(thu->GetEntries()+1);
       }
     }
//--- 
    }//end time 
  }//end nrun


//----Write/record processed runs
  qrunmanager->InsertRunList(runlistn);//write runs
  return (qrunmanager->qprocessfile).size();
}


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
     analysis->FillExpoTime(runlogdir);
//----Write/record processed files
     WriteList(runlogdir,analysis->GetQRunManager()->qprocessfile);//write files
//----Histo Save
     analysis->Save();
   }
   else {cerr<<"Sum1_AMS_CINT::error nstat="<<nstat<<endl;}
   return nstat;
}


