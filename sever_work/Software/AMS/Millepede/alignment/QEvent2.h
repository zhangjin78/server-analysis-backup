///--Author Q.Yan qyan@cern.ch
///--Tool For Analysis Events
#ifndef _QEvent_
#define _QEvent_
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TString.h"
#include "TChain.h"
//#include <stdio.h>
//#include <limits.h>
#include <float.h>
#include "QAnalysisdbc.h"

using namespace qanalysisconst;

class QEvent{
public:
   static int qversion;
public:
   QEvent();
   void SetBranch(TChain *ch,int opt=0);
//---
   int  AddBranch_Header (TChain *ch);
   int  AddBranch_Trigger(TChain *ch);
   int  AddBranch_MC (TChain *ch);
   int  AddBranch_Particle(TChain *ch);
   int  AddBranch_Trk (TChain *ch);
   int  AddBranch_Tof (TChain *ch);
   int  AddBranch_Rich (TChain *ch);
//---Hit Pattern
   inline virtual int   HasTkLHitXY(int ilay);// 2-XY 1-Y 0-NO
   inline virtual int   GetTkInNHit(int ixy=1);
   inline virtual int   GetiPlane(int ilay);
   inline virtual int   HasTkPlaneXY(int iplane);// 2-XY 1-Y 0-NO
   inline virtual int   GetTkInNPlane(int ixy=1);
//---Geometry
   inline virtual int   Interpolate(double zpl,double pnt[3]);
   inline virtual int   InterpolateL(int ilay,double pnt[3],double dir[3]);
   inline virtual bool  IsPassTkL(int ilay);
//---Charge
   inline virtual float GetMean(vector<float> signal,float &rms,int opt);
   inline virtual float GetTOFLQ(int ilay);
   inline virtual int   GetTOFLZ(int ilay);
   inline virtual float GetTOFUDQ(int iud=0);
   inline virtual int   GetTOFUDZ(int iud=0);
   inline virtual bool  IsGoodTkLQStat(int ilay,int opt=1);//opt=0 loose, opt=1 tight
   inline virtual bool  IsGoodTOFLQStat(int ilay);
   inline virtual int   GetTOFNGoodQL(int iud=-1);
   inline virtual int   GetTOFNLayer(int isbeta=0);
   inline virtual float GetTkLQ(int ilay,int ixy=2);
   inline virtual int   GetTkLZ(int ilay,int ixy=2);
   inline virtual float GetTkInQ(int ixy,float &qrms,int opt=0);
   inline virtual float GetTkInQ(int ixy=2);
   inline virtual float GetTkInQRMS(int ixy=2);
   inline virtual int   GetTkInZ(int ixy=2);
//---Hit and Rigidity
   inline virtual float GetRigidity();//ialgo=0 VC, ialgo=1 Alcaraz
   inline virtual float GetERigidity();
   inline virtual float GetChis(int ixy=1);//X+Y+XY
//   inline virtual float GetTkLRes(int ilay,int ixy=1);//um
//---
   virtual bool  Select_Trigger();
   virtual bool  Select_Particle();
   virtual bool  Select_TkGeom(int igeom=2,int opt=0);
   virtual bool  Select_TkInNHit(int opt=0,int ixy=1);//opt=0 loose, opt=1 tight, opt=2 ttight
   virtual bool  Select_Tk(int igeom=2);
   virtual bool  Select_TofBeta();
   virtual bool  Select_Tof();
   virtual bool  Select_TkInQ(int opt=0,int zref=0);
   virtual bool  Select_TofUQ(int opt=0,int zref=0);//opt=0 loose, opt=1 tight
   virtual bool  Select_TofDQ(int opt=0,int zref=0);
   virtual bool  Select_TofLQ(int ilay,int opt=0,int zref=0);
   virtual bool  Select_TkExtQ(int ispan=0,int opt=0,int zref=0);//0-In,1-L1,2-L1L9,3-L9
public:
//---Head
   bool         isreal;
   unsigned int run;
   unsigned int event;
   int          amsdn;
   int          version;
   unsigned int time[2];
   unsigned int errorb;
   float        thetam;//magnetic theta
   float        phim;
//---Trigger
   int          nlevel1; 
   int          physbpatt;
   int          physbpatt1;
   int          physbpatt2;
   int          jmembpatt;
   int          jmembpatt1;
//---MC
   float        mmom;
   float        mch;
   int          mpar;
   float        mevcoo[3];
   float        mevdir[3];
   float        mtrmom[9];
   float        mtrcoo1[9][3];
   float        mtrdir1[9][3];
   float        mevmom1[21];
//---Particle
   int          nbetah;
   int          ntrack;
   int          ntrdtrack;
   int          ntofclh;
   int          ibetah;
   int          itrtrack;
   int          irich;
//---Trk
   int          tk_rtype;
   int          tk_hitb[2];
   float        tk_q[2];
   float        tk_qrms[2];
   float        tk_ql[9];
   float        tk_ql2[9][2];
   int          tk_qls[9];
   int          tk_lid[9];
   float        tk_lch[9][2];
   int          tk_cad[9];
   float        tk_rigidity1;//Algo+PGCIEMAT+Span
   float        tk_erigidity1;//Algo+Span
   float        tk_chis1[3];////Algo+PGCIEMAT+Span+XYXY
//   float        tk_res[9][2];
   float        tk_pos[9][3];
   float        tk_dir[9][3];
   float        tk_hitc[9][3];
//---Tof
   int          tof_btype;
   int          tof_bpatt;
   float        tof_betah;
   int          tof_hsumh;
   int          tof_hsumhu;
   float        tof_chisc_n;
   float        tof_chist_n;
   float        tof_ql[4];
   int          tof_qs;
//---Rich
   float        rich_beta;
   float        rich_pb;
   int          rich_hit;
   int          rich_stat;
};

//----
int QEvent::qversion=0;

///----
QEvent::QEvent(){
}

///----
int QEvent::AddBranch_Header(TChain *ch){
    ch->SetBranchAddress("isreal",  &isreal);
    ch->SetBranchAddress("run",     &run);
    ch->SetBranchAddress("event",   &event);
    ch->SetBranchAddress("version", &version);
    ch->SetBranchAddress("amsdn",   &amsdn);
    ch->SetBranchAddress("time",     time);
    ch->SetBranchAddress("errorb",  &errorb);
    return 0; 
}

int QEvent::AddBranch_Trigger(TChain *ch){
    ch->SetBranchAddress("nlevel1",     &nlevel1);
    ch->SetBranchAddress("physbpatt",   &physbpatt);
    ch->SetBranchAddress("physbpatt1",  &physbpatt1);
    ch->SetBranchAddress("physbpatt2",  &physbpatt2);
    ch->SetBranchAddress("jmembpatt",   &jmembpatt);
    ch->SetBranchAddress("jmembpatt1",  &jmembpatt1);
    return 0;
}

int QEvent::AddBranch_MC(TChain *ch){
    ch->SetBranchAddress("mmom",      &mmom);
    ch->SetBranchAddress("mch",       &mch);
    ch->SetBranchAddress("mpar",      &mpar);
    ch->SetBranchAddress("mevcoo",     mevcoo);
    ch->SetBranchAddress("mevdir",     mevdir);
    ch->SetBranchAddress("mtrmom",     mtrmom);
    ch->SetBranchAddress("mtrcoo1",    mtrcoo1);
    ch->SetBranchAddress("mtrdir1",    mtrdir1);
    ch->SetBranchAddress("mevmom1",    mevmom1);
    return 0;
}

int QEvent::AddBranch_Particle(TChain *ch){
    ch->SetBranchAddress("nbetah",    &nbetah);
    ch->SetBranchAddress("ntrack",    &ntrack);
    ch->SetBranchAddress("ntrdtrack", &ntrdtrack);
    ch->SetBranchAddress("ntofclh",   &ntofclh);
    ch->SetBranchAddress("ibetah",    &ibetah);
    ch->SetBranchAddress("itrtrack",  &itrtrack);
    ch->SetBranchAddress("irich",     &irich);
    return 0; 
}

int QEvent::AddBranch_Trk(TChain *ch){
    ch->SetBranchAddress("tk_rtype",    &tk_rtype);
    ch->SetBranchAddress("tk_hitb",      tk_hitb); 
    ch->SetBranchAddress("tk_q",         tk_q);
    ch->SetBranchAddress("tk_qrms",      tk_qrms);
    ch->SetBranchAddress("tk_ql",        tk_ql);
    ch->SetBranchAddress("tk_ql2",       tk_ql2);
    ch->SetBranchAddress("tk_qls",       tk_qls);
    ch->SetBranchAddress("tk_lid",       tk_lid);
    ch->SetBranchAddress("tk_lch",       tk_lch);
    ch->SetBranchAddress("tk_cad",       tk_cad);
    ch->SetBranchAddress("tk_rigidity1", &tk_rigidity1);
    ch->SetBranchAddress("tk_erigidity1",&tk_erigidity1);
    ch->SetBranchAddress("tk_chis1",     tk_chis1);
//    ch->SetBranchAddress("tk_res",       tk_res);
    ch->SetBranchAddress("tk_pos",       tk_pos); 
    ch->SetBranchAddress("tk_dir",       tk_dir);
    ch->SetBranchAddress("tk_hitc",      tk_hitc);
    return 0;
}

int QEvent::AddBranch_Tof(TChain *ch){
    ch->SetBranchAddress("tof_btype",    &tof_btype);
    ch->SetBranchAddress("tof_bpatt",    &tof_bpatt);
    ch->SetBranchAddress("tof_betah",    &tof_betah);
    ch->SetBranchAddress("tof_hsumh",    &tof_hsumh);
    ch->SetBranchAddress("tof_hsumhu",   &tof_hsumhu);
    ch->SetBranchAddress("tof_chisc_n",  &tof_chisc_n);
    ch->SetBranchAddress("tof_chist_n",  &tof_chist_n);
    ch->SetBranchAddress("tof_ql",        tof_ql);
    ch->SetBranchAddress("tof_qs",       &tof_qs);
    return 0;
}

int QEvent::AddBranch_Rich(TChain *ch){
    ch->SetBranchAddress("rich_beta",    &rich_beta);
    ch->SetBranchAddress("rich_pb",      &rich_pb);
    ch->SetBranchAddress("rich_hit",     &rich_hit);
    ch->SetBranchAddress("rich_stat",    &rich_stat);
    return 0;
}

void QEvent::SetBranch(TChain *ch,int opt){
   if(opt==0){//Header
      AddBranch_Header(ch);
   }
   else {//Other
     AddBranch_Trigger(ch);
     if(!isreal)AddBranch_MC(ch); 
     AddBranch_Particle(ch);
     AddBranch_Trk(ch);
     AddBranch_Tof(ch);
     AddBranch_Rich(ch);
   }
}


int QEvent::HasTkLHitXY(int ilay){
   bool lhxy[2]={0};
   for(int ixy=0;ixy<2;ixy++){if((tk_hitb[ixy]&(1<<ilay))>0)lhxy[ixy]=1;}
   if     (lhxy[0]&&lhxy[1])return 2;//X+Y
   else if(lhxy[1])         return 1;//Y
   else                     return 0;//No
}

int QEvent::GetTkInNHit(int ixy){
   int nhiti[2]={0};
   int bl=0+1;
   for(int ilay=bl;ilay<9-1;ilay++){
      if(HasTkLHitXY(ilay)==2)nhiti[0]++;//X
      if(HasTkLHitXY(ilay)>=1)nhiti[1]++;//Y
   }
   return nhiti[ixy];
}

int QEvent::GetiPlane(int ilay){
   int iplane=-1;
   if     (ilay>=0&&ilay<=1)iplane=ilay;
   else if(ilay>=2&&ilay<=8)iplane=2+(ilay-2)/2;
   return iplane;
}

int QEvent::HasTkPlaneXY(int iplane){//0(L1),1(L2),2(L3-4),3(L5-6),4(L7-8),5(L9)
   int ilay[2]={-1,-1},hasxy[2]={-1,-1};
   if     (iplane==0||iplane==1)ilay[0]=ilay[1]=iplane;
   else if(iplane>=2&&iplane<=4){ilay[0]=2*(iplane-1);ilay[1]=ilay[0]+1;}
   else if(iplane==5)ilay[0]=ilay[1]=8;
   hasxy[0]=HasTkLHitXY(ilay[0]);
   hasxy[1]=HasTkLHitXY(ilay[1]);
   return (hasxy[0]>=hasxy[1])? hasxy[0]: hasxy[1];
}

int QEvent::GetTkInNPlane(int ixy){
   int np[2]={0};
   for(int ipl=1;ipl<5;ipl++){
     int haspl=HasTkPlaneXY(ipl);
     if(haspl==2)np[0]++;//X
     if(haspl>=1)np[1]++;//Y
   }
   return np[ixy];
}

int QEvent::Interpolate(double zpl,double pnt[3]){
    pnt[0]=pnt[1]=0;pnt[2]=zpl;
    if(itrtrack<0)return 1;
    int lmin=4,lmax=4;
    double ww=1;
    if     (zpl>=tk_pos[0][2]){lmin=lmax=0;ww=1;}
    else if(zpl<=tk_pos[8][2]){lmin=lmax=8;ww=1;}
    else   { 
      for(int ilay=0;ilay<9-1;ilay++){
        if(zpl<=tk_pos[ilay][2]&&zpl>=tk_pos[ilay+1][2]){
          ww=(zpl-tk_pos[ilay+1][2])/(tk_pos[ilay][2]-tk_pos[ilay+1][2]);
          lmin=ilay;lmax=ilay+1;
          break;
        }
      }
    }
    if(!(ww>=0&&ww<=1)){cout<<"Error_Interpolate="<<ww<<endl;return 2;}
    for(int ixy=0;ixy<2;ixy++){
      double xymin=tk_pos[lmin][ixy]+(zpl-tk_pos[lmin][2])*tk_dir[lmin][ixy]/tk_dir[lmin][2];
      double xymax=tk_pos[lmax][ixy]+(zpl-tk_pos[lmax][2])*tk_dir[lmax][ixy]/tk_dir[lmax][2];
      pnt[ixy]=ww*xymin+(1-ww)*xymax;
//      cout<<"zpl="<<zpl<<" ww="<<ww<<" xymin="<<xymin<<" xymax="<<xymax<<" pnt="<<pnt[ixy]<<endl;
    }
    return 0;
}

int QEvent::InterpolateL(int ilay,double pnt[3],double dir[3]){
   for(int ixy=0;ixy<3;ixy++){
     pnt[ixy]=tk_pos[ilay][ixy];
     dir[ixy]=tk_dir[ilay][ixy];
   }
   return 0;
}

bool QEvent::IsPassTkL(int ilay){
   bool mlh=0;
   float coox=tk_pos[ilay][0];
   float cooy=tk_pos[ilay][1];
   float disr=coox*coox+cooy*cooy;
   if(ilay==9-1)mlh=(fabs(coox)<cirr[ilay]&&fabs(cooy)<ciry[ilay]);
   else         mlh=(sqrt(disr)<cirr[ilay]&&fabs(cooy)<ciry[ilay]);
   return mlh;
}

float QEvent::GetMean(vector<float> signal,float &rms,int opt){
  int   n    = 0;
  float mean = 0;
  rms  = 0;
  float sigmax=-FLT_MAX,sigmin=FLT_MAX;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    float psignal=signal.at(ii);
    if (psignal<0) continue;
    mean += psignal;
    rms  += pow(psignal,2);
    if(psignal>sigmax)sigmax=psignal;
    if(psignal<sigmin)sigmin=psignal;
    n++;
  }
  if(opt==2&&n>=3){mean=mean-sigmax-sigmin;rms=rms-pow(sigmax,2)-pow(sigmin,2);n=n-2;}//remove 2hits
  if(n>=1){
    mean /= n;
    rms  /= n;
    rms = sqrt(rms - mean*mean);
  }
  return mean;
}

float QEvent::GetTOFLQ(int ilay){
   return tof_ql[ilay];
}

int QEvent::GetTOFLZ(int ilay){
   return int(GetTOFLQ(ilay)+0.5);
}

float QEvent::GetTOFUDQ(int iud){
  float tofqud[2]={0};
  tofqud[0]=(GetTOFLQ(0)+GetTOFLQ(1))/2.;
  tofqud[1]=(GetTOFLQ(2)+GetTOFLQ(3))/2.;
  return tofqud[iud];   
}

int QEvent::GetTOFUDZ(int iud){
  return int(GetTOFUDQ(iud)+0.5);
}

bool QEvent::IsGoodTkLQStat(int ilay,int opt){
   bool lhqs=0;
   if(opt==1)lhqs=((tk_qls[ilay]&0x1001FD)==0);
   else      lhqs=((tk_qls[ilay]&0x10013D)==0);//Y 8-bit/X 0,2,3,4,5,8
   return lhqs;
}

bool QEvent::IsGoodTOFLQStat(int ilay){
   bool ltofqs=(tof_qs/int(pow(10.,3-ilay))%10==1);
   return ltofqs;
}

int QEvent::GetTOFNGoodQL(int iud){
   int ngoodql=0;
   int bl=0,el=3;
   if     (iud==0){bl=0;el=1;}
   else if(iud==1){bl=2;el=3;}
   for(int ilay=bl;ilay<=el;ilay++){//TOF Hit
      if(IsGoodTOFLQStat(ilay))ngoodql++;
   }
   return ngoodql;
}

int QEvent::GetTOFNLayer(int isbeta){
   if(isbeta>=1)return tof_hsumhu;
   else         return tof_hsumh;
}

float QEvent::GetTkLQ(int ilay,int ixy){
   if(ixy==0||ixy==1){
      return  tk_ql2[ilay][ixy];
   }
   return tk_ql[ilay];
}

int QEvent::GetTkLZ(int ilay,int ixy){
   return int(GetTkLQ(ilay,ixy)+0.5);
}

float QEvent::GetTkInQ(int ixy,float &qrms,int opt){
   float qm=tk_q[1];
   qrms=tk_qrms[1];
   int bl=0+1;
   if(ixy==0||ixy==1){
     vector<float>signal;
     for(int ilay=bl;ilay<9-1;ilay++){
       if(ixy==0){if(HasTkLHitXY(ilay)!=2)continue;}//X
       else      {if(HasTkLHitXY(ilay)==0)continue;}//Y
       signal.push_back(GetTkLQ(ilay,ixy));  
     }
     qm=GetMean(signal,qrms,opt);
   }
   return qm;
}

float QEvent::GetTkInQ(int ixy){
   float qm,qrms;
   qm=GetTkInQ(ixy,qrms,0);
   return qm;
}

float QEvent::GetTkInQRMS(int ixy){
   float qm,qrms;
   qm=GetTkInQ(ixy,qrms,0);
   return qrms;
}

int QEvent::GetTkInZ(int ixy){
   float tkqin=GetTkInQ(ixy); 
   if(ixy==2&&tkqin>9)tkqin=(tkqin-9.)/1.03+9.;//special biase correction for pass6
   return int(tkqin+0.5);
}

float QEvent::GetRigidity(){
   return tk_rigidity1;
}

float QEvent::GetERigidity(){
   return tk_erigidity1;
}

float QEvent::GetChis(int ixy){
   return tk_chis1[ixy];
}


/*float QEvent::GetTkLRes(int ilay,int ixy){
  return tk_res[ilay][ixy]*10000.;
}*/


bool QEvent::Select_Trigger(){
    int physbpattp=(isreal)?physbpatt2:physbpatt1;//Trigger ISS/Trigger MC
    bool trigp=((physbpattp&0x3EL)!=0);
    return trigp;
} 

bool QEvent::Select_Particle(){
    bool parcut=(itrtrack>=0&&ibetah>=0&&tof_btype<10);
    return parcut;
}

bool QEvent::Select_TkGeom(int igeom,int opt){
   int mnhitf=0;
   for(int il=0+1;il<9-1;il++){
     if(IsPassTkL(il)){mnhitf++;}
   }
   bool geomcut=(mnhitf>=5);
   if(opt>=1){
      bool L1=(IsPassTkL(1));
      bool L23=(IsPassTkL(2)||IsPassTkL(3));
      bool L45=(IsPassTkL(4)||IsPassTkL(5));
      bool L67=(IsPassTkL(6)||IsPassTkL(7));
      if(opt==1)geomcut=(L1&&L23&&L45&&L67);
      else      geomcut=(L1&&L23&&L45&&L67&&geomcut);
   }
   if(igeom>=0)geomcut=(geomcut&&IsPassTkL(0));
   if(igeom>=2)geomcut=(geomcut&&IsPassTkL(8));
   return geomcut;
} 

bool QEvent::Select_TkInNHit(int opt,int ixy){
    bool inhitcut=(GetTkInNHit(ixy)>=5);
    int hitxy=(ixy==1)?1:2;
    if(opt>=1){
      bool L1=(HasTkLHitXY(1)>=hitxy);
      bool L23=(HasTkLHitXY(2)>=hitxy||HasTkLHitXY(3)>=hitxy);
      bool L45=(HasTkLHitXY(4)>=hitxy||HasTkLHitXY(5)>=hitxy);
      bool L67=(HasTkLHitXY(6)>=hitxy||HasTkLHitXY(7)>=hitxy);
      if     (opt==1)inhitcut=(L1&&L23&&L45&&L67);
      else if(opt==6)inhitcut=(L23&&L45&&L67&&inhitcut);//InInner 
      else           inhitcut=(L1&&L23&&L45&&L67&&inhitcut);
    }
    return inhitcut;
}

bool QEvent::Select_Tk(int igeom){
//----Tk Geometry
   if(!Select_TkGeom(igeom))return 0;
//---Tk InnerNhit
   if(!Select_TkInNHit())return 0;
//---    
   return 1;
}

bool QEvent::Select_TofBeta(){
    bool cutbeta=(tof_betah>0.4);
    return cutbeta;
}

bool QEvent::Select_Tof(){
   return Select_TofBeta();
}

bool QEvent::Select_TkInQ(int opt,int zref){
   float tkqinrms=0;
   float tkqin=GetTkInQ(2,tkqinrms,0);//X+Y
   tkqin=(tkqin-9.)/1.03+9.;//special biase correction for pass6
   int tk_z1=(zref>=1)? zref: int(tkqin+0.5);
   bool cuttkql=  (tkqin>tk_z1-0.3);
   bool cuttkqh=  (tkqin<tk_z1+0.4);
   bool cuttkqrms=(tkqinrms<0.4);
   if(tk_z1>=3){
     cuttkql=  (tkqin>tk_z1-0.45);
     cuttkqh=  (tkqin<tk_z1+0.45);
     cuttkqrms=(tkqinrms<0.55);
   }
   if(tk_z1>=9){
     double sigma=0.15+0.028*(tk_z1-9);
     double zran=3.*sigma;
     if(zref<=0&&zran>0.5)zran=0.5;//0.5 c.u.
     cuttkql=(tkqin>tk_z1-zran);
     cuttkqh=(tkqin<tk_z1+zran);
     double rmean=0.0051*pow(double(tk_z1),1.5)+0.065;
     double rmsig=0.00012*pow(double(tk_z1),2.18)+0.053;
/*     cuttkqrms=(opt>=0)? (tkqinrms<0.57+0.1*(tk_z1-9)) :1;
     if(calib>=1)cuttkqrms=1;*/
     cuttkqrms=1;
/*     if(calib==1){
       double rmean=8.87974e-07*pow(double(tk_z1),4.46102)+0.235398;
       if(tk_z1>=16)rmean=2.44994e-14*pow(double(tk_z1),8.93812)+0.460895;
     }
     if(calib==2){
       double rmean=8.87974e-07*pow(double(tk_z1),4.46102)+0.235398;
       1.94727e-09,6.54957,0.263074
       if(tk_z1>=16)rmean=2.44994e-14*pow(double(tk_z1),8.93812)+0.460895;
     }*/
   }
   bool cuttkiq=(cuttkql&&cuttkqh&&cuttkqrms); 
   return cuttkiq; 
} 

bool QEvent::Select_TofUQ(int opt,int zref){
   float tofqu=GetTOFUDQ(0);
   int tk_z1=(zref>=1)? zref: GetTkInZ();
   bool cuttofqul=(tofqu>tk_z1-0.5);
   bool cuttofquh=(tofqu<tk_z1+1.5);
   bool cuttofqrms=1;
   if(opt>=1){
     cuttofqul=(GetTOFLQ(0)>tk_z1-0.5&&GetTOFLQ(1)>tk_z1-0.5);
     cuttofquh=(tofqu<tk_z1+1.5);
   }
   if(opt==4){
     cuttofqul=(tofqu>tk_z1-0.5);
     cuttofquh=(tofqu<tk_z1+0.5);
   }
   if(tk_z1>=3){
     cuttofqul=(tofqu>tk_z1-0.6);
     cuttofquh=(tofqu<tk_z1+1.5);
     if(opt>=1){
       cuttofqul=(GetTOFLQ(0)>tk_z1-0.6&&GetTOFLQ(1)>tk_z1-0.6);
       cuttofquh=(tofqu<tk_z1+1.5);
     }
     if(opt==4){
      cuttofqul=(tofqu>tk_z1-0.5);
      cuttofquh=(tofqu<tk_z1+0.5);
     }
   }
   if(tk_z1>=9){
     double sigma=0.25+0.009*(tk_z1-9);
     double rsigma=0.41+0.013*(tk_z1-9);
     cuttofqul=(tofqu>tk_z1-2.5*sigma);//2.5sigma
     if(tk_z1>=17)cuttofqul=(tofqu>tk_z1-3*sigma);//3sigma
     cuttofquh=(tofqu<tk_z1+1.5);
     cuttofqrms=1;
     if(opt>=1){
       cuttofqul=(tofqu>tk_z1-2*sigma);//2sigma
       cuttofquh=(tofqu<tk_z1+2*sigma);
       cuttofqrms=(fabs(GetTOFLQ(0)-GetTOFLQ(1))<2*rsigma);//2sigma
     }
     if(opt==4){
       for(int il=0;il<2;il++){
        cuttofqul=(cuttofqul&&GetTOFLQ(il)>tk_z1-0.5);
        cuttofquh=(cuttofquh&&GetTOFLQ(il)<tk_z1+0.5);
       }
     }
   }
   bool cuttofq=(cuttofqul&&cuttofquh&&cuttofqrms);
   return cuttofq;
}

bool QEvent::Select_TofDQ(int opt,int zref){
   float tofqd=GetTOFUDQ(1);
   int tk_z1=(zref>=1)? zref: GetTkInZ();
   bool cuttofqdl=(tofqd>tk_z1-0.5);
   bool cuttofqdh=1;
   bool cuttofqrms=1;
   if(opt>=1){
     cuttofqdl=(GetTOFLQ(2)>tk_z1-0.5&&GetTOFLQ(3)>tk_z1-0.5);
     cuttofqdh=1;
   }
   if(opt==4){
     cuttofqdl=(tofqd>tk_z1-0.5);
     cuttofqdh=(tofqd<tk_z1+0.5);
   }
   if(tk_z1>=3){
     cuttofqdl=(tofqd>tk_z1-0.6);
     cuttofqdh=1;
     if(opt>=1){
       cuttofqdl=(GetTOFLQ(2)>tk_z1-0.6&&GetTOFLQ(3)>tk_z1-0.6);
       cuttofqdh=1;
     }
     if(opt==4){
       cuttofqdl=(tofqd>tk_z1-0.5);
       cuttofqdh=(tofqd<tk_z1+0.5);
     }
   }
   if(tk_z1>=9){
     double sigma=0.25+0.009*(tk_z1-9);
     double rsigma=0.41+0.013*(tk_z1-9);
     cuttofqdl=(tofqd>tk_z1-2.5*sigma);//2.5sigma
     if(tk_z1>=17)cuttofqdl=(tofqd>tk_z1-3*sigma);//3sigma
     cuttofqdh=cuttofqrms=1;
     if(opt>=1){
       cuttofqdl=(tofqd>tk_z1-2*sigma);//2sigma
       cuttofqdh=(tofqd<tk_z1+2*sigma);
       cuttofqrms=(fabs(GetTOFLQ(2)-GetTOFLQ(3))<2*rsigma);//2sigma
     }
     if(opt==4){
       for(int il=2;il<4;il++){
        cuttofqdl=(cuttofqdl&&GetTOFLQ(il)>tk_z1-0.5);
        cuttofqdh=(cuttofqdh&&GetTOFLQ(il)<tk_z1+0.5);
       }
     }
   }
   bool cuttofq=(cuttofqdl&&cuttofqdh&&cuttofqrms);
   return cuttofq;
}

bool QEvent::Select_TofLQ(int ilay,int opt,int zref){
   int tk_z1=(zref>=1)? zref: GetTkInZ();
   float toflq=GetTOFLQ(ilay);
   bool cuttofql=(toflq>tk_z1-0.5);
   bool cuttofqh=(toflq<tk_z1+0.5);
   if(tk_z1>=9){
     double sigma=0.32+0.01*(tk_z1-9);
     cuttofql=(toflq>tk_z1-1.5*sigma);//1.5*1sigma
     cuttofqh=(toflq<tk_z1+1.5*sigma);//1.5*1sigma
   }
   if(opt>=1){
     cuttofql=(cuttofql&&(toflq>tk_z1-0.5));
     cuttofqh=(cuttofqh&&(toflq<tk_z1+0.5));   
   }
   bool cuttofq=(cuttofql&&cuttofqh);
   return cuttofq;
}

bool QEvent::Select_TkExtQ(int ispan,int opt,int zref){
//---L1L9 Pattern
   bool l1h=(HasTkLHitXY(0)==2);
   bool l9h=(HasTkLHitXY(8)==2);
   bool l1hs=IsGoodTkLQStat(0,0);
   bool l9hs=IsGoodTkLQStat(8,0);
//---L1L9 Charge
   int tk_z1=(zref>=1)? zref: GetTkInZ(2);
   float tkql1x=GetTkLQ(0,0);
   float tkql1y=GetTkLQ(0,1);
   float tkql1 =GetTkLQ(0,2);
   float tkql9x=GetTkLQ(8,0);
   float tkql9y=GetTkLQ(8,1);
   float tkql9 =GetTkLQ(8,2);
//---
   bool cutl1ch=l1h;
   bool cutl9ch=l9h;
   bool cutql1l=1,cutql1h=1,cutql9l=1,cutql9h=1;
   if(tk_z1<=2){
     cutql1l=(tkql1>tk_z1-0.4);
     cutql1h=(tkql1<tk_z1+1.0);
     cutql9l=(tkql9>tk_z1-0.4);
     cutql9h=(tkql9<tk_z1+1.0);
     if(opt==2){//BZHit Cut
       cutql1h=1;
       cutql9h=1;
     } 
     else if(opt==4){//Background Free Cut
       cutql1h=(tkql1<tk_z1+0.5);
       cutql9h=(tkql9<tk_z1+0.7);
     }
     else if(opt==5){//Strong Background Free Cut
       cutql1l=(tkql1>tk_z1-0.3);
       cutql1h=(tkql1<tk_z1+0.4);
       cutql9l=(tkql9>tk_z1-0.3);
       cutql9h=(tkql9<tk_z1+0.5);
     }
     else if(opt==6){//Loose selection on Hit
       cutql1l=1;
       cutql1h=1;
       cutql9l=1;
       cutql9h=1;
     }
   }
   else if(tk_z1<=8){
     cutql1l=(tkql1>tk_z1-0.46-(tk_z1-3)*0.16);
     cutql1h=(tkql1<tk_z1+1.5);
     cutql9l=(tkql9>tk_z1-0.5);
     cutql9h=(tkql9<tk_z1+1.5+(tk_z1-3)*0.06);
     if(opt==1){//Final Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       if(tk_z1<=5)cutql1h=(tkql1<tk_z1+0.65);
       else        cutql1h=(tkql1<tk_z1+0.65+(tk_z1-5)*0.03);
     }
     else if(opt==2){//BZHit Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1h=1;
       cutql9h=1;
     }
     else if(opt==3){//BZHit Cut && not cutql1h
       cutql1h=1;
     }
     else if(opt==4){//Background Free Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1h=(tkql1<tk_z1+0.5);
       cutql9h=(tkql9<tk_z1+1); 
     }
     else if(opt==5){//Strong Background Free Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1l=(tkql1>tk_z1-0.46-(tk_z1-3)*0.01);
       cutql1h=(tkql1<tk_z1+0.4);
       cutql9l=(tkql9>tk_z1-0.3);
       cutql9h=(tkql9<tk_z1+0.5);
     }
     else if(opt==6){//Loose selection on Hit
       cutl1ch=(cutl1ch&&l1hs);//XY&&XYQMatch&&QS
       cutql1l=1;
       cutql1h=1;
       cutql9l=1;
       cutql9h=1;
     }
   }
   else {//Z>=9
     double tkql1xyr=(tkql1x-tkql1y)/(tkql1x+tkql1y);//(1+f)/(1-f)
     double tkql9xyr=(tkql9x-tkql9y)/(tkql9x+tkql9y);
//----selection
     cutl1ch=(cutl1ch&&fabs(tkql1xyr)<0.2);
     cutl9ch=(cutl9ch&&fabs(tkql9xyr)<0.2);
//------
     double sigma=0.28+0.08*(tk_z1-9);
     cutql1l=(tkql1>tk_z1-3.5*sigma);//>3.5 Sigma
     cutql1h=(tkql1<tk_z1+3.5*sigma);//<3.5 Sigma
     cutql9l=(tkql9>tk_z1-1.7*sigma);//>1.7 Sigma
     cutql9h=(tkql9<tk_z1+3.5*sigma);//<3.5 Sigma
     if(opt<0) {//Loose Cut
       cutql9l=(tkql9>tk_z1-2.5*sigma);//>2.5 Sigma
     }
     else if(opt==1){//Final Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1h=(tkql1<tk_z1+2.0*sigma);//<2.0 Sigma
     }
     else if(opt==2){//BZHit Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1h=1;
       cutql9h=1;
     }
     else if(opt==3){//BZHit Cut && not cutql1h
       cutql1h=1;
     }
     else if(opt==4){//Background Free Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1h=(tkql1<tk_z1+1.7*sigma);//<1.7 Sigma
       cutql9h=(tkql9<tk_z1+2.5*sigma);//<2.5 Sigma 
     }
     else if(opt==5){//Strong Background Free Cut
       cutl1ch=(cutl1ch&&l1hs);//QS
       cutql1l=(tkql1>tk_z1-1.5*sigma);//>1.5 Sigma
       cutql1h=(tkql1<tk_z1+1.5*sigma);//<1.5 Sigma
       cutql9l=(tkql9>tk_z1-1.5*sigma);//>1.5 Sigma
       cutql9h=(tkql9<tk_z1+2.0*sigma); //<2.5 Sigma 
     }
     else if(opt==6){//Loose selection on Hit
       cutl1ch=(cutl1ch&&l1hs);//XY&&XYQMatch&&QS
       cutql1l=1;
       cutql1h=1;
       cutql9l=1;
       cutql9h=1;
     }
   }
//-----summary
   cutl1ch=(cutl1ch&&cutql1l&&cutql1h);
   cutl9ch=(cutl9ch&&cutql9l&&cutql9h);
//-----
   bool cuttkextq[7]={0};
   cuttkextq[0]=1;
   cuttkextq[1]=(l1h&&cutl1ch);//Pattern+Charge+ChisY
   cuttkextq[2]=(l1h&&l9h&&cutl1ch&&cutl9ch);
   cuttkextq[3]=(l9h&&cutl9ch);
   cuttkextq[4]=cuttkextq[5]=cuttkextq[6]=1;
   return cuttkextq[ispan];
}


#endif
