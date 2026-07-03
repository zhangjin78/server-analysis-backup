//---Author Q.Yan qyan@cern.ch: compression data used for alignment
#include <signal.h>
#include <TH1.h>
#include <TH2F.h>
#include <TF1.h>
#include <TLeaf.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include "TkSens.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include "root_RVSP.h"
#include "amschain.h"
#include "Tofrec02_ihep.h"
#include "TofTrack.h"
#include "TrdKCluster.h"
#include "TrdSCalib.h"
#include "bcorr.h"
#include <time.h>
#include "Track.h"
#include "TrInnerDzDB.h"
#include "AbsKalmanFitter.h"
#include "KalmanFitStatus.h"
#include "KalmanFittedStateOnPlane.h"
#include "KalmanFitterInfo.h"
#include "TkGeomN.h"
//#include "/afs/cern.ch/user/q/qyan/ihepsub/readfile1.C"
#include "readfile3.C"

namespace tofconst{
  const int NECALL=18;
  const int NTKL=9;
  const int NTKS=2;
  const int NTKDIS=4;
  const float TKDIS[NTKDIS]={1,2,4,6};//dis=cm
  const int NTOFL=4;
  const int NTOFBM=10;
  const int NTOFS=2;
  const int NBAR[NTOFL]={8,8,10,8};
  const int TRANS[NTOFL]={0,1,1,0};//trans pos
  const float TOFZ[NTOFL]={65.2,62.1,-62.1,-65.2};//central cm  
  const float Sci_pt[NTOFL][NTOFBM]={
      -45.5,  -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 45.5,  0., 0.,
      -47.,   -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 47.,   0., 0.,
-55., -40.25, -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 40.25, 55.,
      -47.25, -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 47.25, 0., 0.,
   };
   const float qxlow[]={0.9, 1.9, 2.85, 3.8, 4.7, 5.6, 6.65, 7.6};
   const float qxhig[]={1.15,2.2, 3.2,  4.2, 5.2, 6.2, 7.2, 8.2};
   const float tk_pz[NTKL]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
//--Static information
#ifndef _EVENTSCALE_ 
   const int prscale=1;
   const int prunbias=20;//1/20 for unbias
#else
   const int prscale=10;
   const int prunbias=200;//1/20 for unbias
#endif
   int    prunbiasmc=10;
   unsigned int prtime=0;
   int    sstat[100]={0};
   int    sstree[100]={0};
   double cput[100]={0};
}
using namespace tofconst;


class AMSAlignTree{
  public:
   AMSAlignTree():fout(0),pev(0){tout[0]=tout[1]=tout[2]=0;};
   ~AMSAlignTree(){};
   void BookFile(char *ofile);
   int  LoadHeader();
   int  LoadHeaderEv();
   int  LoadHeaderMC();
   int  InitTrkReFit(float tkiq,float tkfz,int &refit,float &mass,int &chrg,int algo=1);
//---
   int  GetTreeSize(TTree *tree);
   int  AddBranch_Header (TTree *tree);
   int  AddBranch_Trigger(TTree *tree);
   int  AddBranch_MC (TTree *tree);
   int  AddBranch_Particle(TTree *tree);
   int  AddBranch_Trk (TTree *tree);
   int  AddBranch_Tof (TTree *tree);
   int  AddBranch_Rich(TTree *tree);
//---
   int  Save();
   void InitEvent(AMSEventR *ev);   
   int  Select();//1 Write-Pre 2 Write-All
   bool Select_Header();
   bool Select_Trigger();
   bool Select_MC();
   bool Select_Particle();
//----
   bool Select_Trk();//pre selction 1 or 0
   bool Select_Tof();
   bool Select_Rich();
   int  Fill(int it);
//--
   TFile *fout;
   TTree *tout[3];

   AMSEventR *pev;
//--Sum Control
   int          unbiasetag0;
   int          unbiasetag;
   int          issmearext;
   int          istkaddhit;
//--Header
   bool         isreal;
   unsigned int run;
   unsigned int event;
   int          amsdn;
   int          version;
   unsigned int errorb;
   unsigned int time[2];
   float        yaw;
   float        pitch;
   float        roll;
   float        thetam;//magnetic theta
   float        phim;
   bool         isbadrun;

//---Trigger
   int          nlevel1;
   float        livetime;
   int          physbpatt;
   int          jmembpatt;
   int          physbpatt1;
   int          jmembpatt1;
   int          physbpatt2;
   int          jmembpatt2;

//---MC
   int          mpar;
   float        mmom;
   float        mch;
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
   int          irich;
   int          itrtrack;
   
//---Track
   int          tk_rtype;
   int          tk_hitb[2];//XY
   float        tk_q[2];
   float        tk_qrms[2];
   float        tk_ql[9];//Lay1+2+..9
   float        tk_ql2[9][2];
   int          tk_qls[9];
   int          tk_lid[9];
   float        tk_lch[9][2];
   int          tk_cad[9];
   int          tk_stripx[9];
//---Full+Inner+InnerL0+InnerL8+InnerUP+InnerDo
   float        tk_fzm[2];
   float        tk_rigidity1;//PGCIEMAT+Span
   float        tk_erigidity1;//Algo+Span
   float        tk_chis1[3];////PGCIEMAT+Span+XYA
//   float        tk_res[9][2];//Residual Layer+XY
   float        tk_pos[9][3];
   float        tk_dir[9][3];
   float        tk_hitc[9][3];
//---TOF
   int          tof_btype;
   int          tof_bpatt;
   float        tof_betah;
   int          tof_hsumh;
   int          tof_hsumhu;
   float        tof_chist_n;
   float        tof_chisc_n;
   int          tof_qs;//QStat 
   float        tof_ql[4];
//---Rich
   float        rich_beta;
   float        rich_pb;
   int          rich_hit;
   int          rich_stat;
};



void AMSAlignTree::BookFile(char *ofile){
  fout=new TFile(ofile,"RECREATE");

//--book tree
  char trnam[1000];
  for(int it=0;it<3;it++){
    if     (it==0)sprintf(trnam,"amstreeb");
    else if(it==1)sprintf(trnam,"amstreea");
    else if(it==2)sprintf(trnam,"amstreeu");
    tout[it]=new TTree(trnam,trnam);
    sstree[0]=AddBranch_Header(tout[it]);
    sstree[1]=AddBranch_Trigger(tout[it]);
    sstree[3]=AddBranch_Particle(tout[it]);
    if(it==0)continue;
    sstree[4]=AddBranch_Trk(tout[it]);
    sstree[5]=AddBranch_Tof(tout[it]);
    sstree[6]=AddBranch_Rich(tout[it]);
 }

}

int AMSAlignTree::GetTreeSize(TTree *utree){
   int treelen=0;
   TObjArray* leaves=utree->GetListOfLeaves();
   int nleaves=leaves->GetEntriesFast();
   for (int i = 0; i < nleaves; i++) {
     TLeaf* leaf = (TLeaf*) leaves->UncheckedAt(i);
     int len = leaf->GetLen();
     treelen +=len;
   }
   return treelen;
}


int AMSAlignTree::AddBranch_Header(TTree *utree){//12
    int lenb=GetTreeSize(utree);
    utree->Branch("run",      &run,      "run/i");
    utree->Branch("isbadrun", &isbadrun, "isbadrun/O");
    utree->Branch("isreal",   &isreal,   "isreal/O");
    utree->Branch("event",    &event,    "event/i");
    utree->Branch("version",  &version,  "version/I");
    utree->Branch("amsdn",    &amsdn,    "amsdn/I");
    utree->Branch("errorb",   &errorb,   "errorb/i");
    utree->Branch("time",      time,     "time[2]/i");
    utree->Branch("yaw",      &yaw,      "yaw/F");
    utree->Branch("pitch",    &pitch,    "pitch/F");
    utree->Branch("roll",     &roll,     "roll/F");
    utree->Branch("thetam",   &thetam,   "thetam/F");
    utree->Branch("phim",     &phim,     "phim/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAlignTree::AddBranch_Trigger(TTree *utree){//14
    int lenb=GetTreeSize(utree);
    utree->Branch("nlevel1",    &nlevel1,    "nlevel1/I");
    utree->Branch("livetime",   &livetime,   "livetime/F");
    utree->Branch("physbpatt",  &physbpatt,  "physbpatt/I");
    utree->Branch("jmembpatt",  &jmembpatt,  "jmembpatt/I");
    utree->Branch("physbpatt1", &physbpatt1, "physbpatt1/I");
    utree->Branch("jmembpatt1", &jmembpatt1, "jmembpatt1/I");
    utree->Branch("physbpatt2", &physbpatt2, "physbpatt2/I");
    utree->Branch("jmembpatt2", &jmembpatt2, "jmembpatt2/I");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}


int AMSAlignTree::AddBranch_MC(TTree *utree){
    int lenb=GetTreeSize(utree);
    utree->Branch("mpar",     &mpar,     "mpar/I");
    utree->Branch("mmom",     &mmom,     "mmom/F");
    utree->Branch("mch",      &mch,      "mch/F");
    utree->Branch("mevcoo",    mevcoo,   "mevcoo[3]/F");
    utree->Branch("mevdir",    mevdir,   "mevdir[3]/F");
    utree->Branch("mtrmom",    mtrmom,   "mtrmom[9]/F");
    utree->Branch("mtrcoo1",   mtrcoo1,  "mtrcoo1[9][3]/F");
    utree->Branch("mtrdir1",   mtrdir1,  "mtrdir1[9][3]/F");
    utree->Branch("mevmom1",   mevmom1,  "mevmom1[21]/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}


int AMSAlignTree::AddBranch_Particle(TTree *utree){
    int lenb=GetTreeSize(utree);
   //--Particle //22
    utree->Branch("nbetah",     &nbetah,     "nbetah/I");
    utree->Branch("ntrack",     &ntrack,     "ntrack/I");
    utree->Branch("ntrdtrack",  &ntrdtrack,  "ntrdtrack/I");
    utree->Branch("ntofclh",    &ntofclh,    "ntofclh/I");
    utree->Branch("ibetah",     &ibetah,     "ibetah/I");
    utree->Branch("irich",      &irich,      "irich/I");
    utree->Branch("itrtrack",   &itrtrack,   "itrtrack/I");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAlignTree::AddBranch_Trk(TTree *utree){//52+380-54
    int lenb=GetTreeSize(utree);
    utree->Branch("tk_rtype",     &tk_rtype,       "tk_rtype/I");
    utree->Branch("tk_hitb",       tk_hitb,        "tk_hitb[2]/I");
    utree->Branch("tk_q",          tk_q,           "tk_q[2]/F");//All+Inner
    utree->Branch("tk_qrms",       tk_qrms,        "tk_qrms[2]/F");
    utree->Branch("tk_ql",         tk_ql,          "tk_ql[9]/F");//Temp Only Layer1+2
    utree->Branch("tk_ql2",        tk_ql2,         "tk_ql2[9][2]/F");
    utree->Branch("tk_qls",        tk_qls,         "tk_qls[9]/I");//TK-QL Status
    utree->Branch("tk_lid",        tk_lid,         "tk_lid[9]/I");//TK-Ladder-Id
    utree->Branch("tk_lch",        tk_lch,         "tk_lch[9][2]/F");
    utree->Branch("tk_cad",        tk_cad,         "tk_cad[9]/I");
    utree->Branch("tk_stripx",     tk_stripx,      "tk_stripx[9]/I");
//    utree->Branch("tk_res",        tk_res,         "tk_res[9][2]/F");
    utree->Branch("tk_fzm",        tk_fzm,         "tk_fzm[2]/F");
    utree->Branch("tk_rigidity1", &tk_rigidity1,   "tk_rigidity1/F");
    utree->Branch("tk_erigidity1",&tk_erigidity1,  "tk_erigidity1/F");
    utree->Branch("tk_chis1",      tk_chis1,       "tk_chis1[3]/F");
    utree->Branch("tk_pos",        tk_pos,         "tk_pos[9][3]/F");
    utree->Branch("tk_dir",        tk_dir,         "tk_dir[9][3]/F");
    utree->Branch("tk_hitc",       tk_hitc,        "tk_hitc[9][3]/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAlignTree::AddBranch_Tof(TTree *utree){//45
    int lenb=GetTreeSize(utree);
    utree->Branch("tof_btype",   &tof_btype,   "tof_btype/I");
    utree->Branch("tof_bpatt",   &tof_bpatt,   "tof_bpatt/I");
    utree->Branch("tof_betah",   &tof_betah,   "tof_betah/F");
    utree->Branch("tof_hsumh",   &tof_hsumh,   "tof_hsumh/I"); //BetaH
    utree->Branch("tof_hsumhu",  &tof_hsumhu,  "tof_hsumhu/I");//
    utree->Branch("tof_chist_n", &tof_chist_n, "tof_chist_n/F");
    utree->Branch("tof_chisc_n", &tof_chisc_n, "tof_chisc_n/F");
    utree->Branch("tof_ql",       tof_ql,      "tof_ql[4]/F");
    utree->Branch("tof_qs"  ,    &tof_qs,      "tof_qs/I");//Q-Status
    int lene=GetTreeSize(utree);
    return lene-lenb;
}


int AMSAlignTree::AddBranch_Rich(TTree *utree){//25
    int lenb=GetTreeSize(utree);
    utree->Branch("rich_beta",    &rich_beta,  "rich_beta/F");
    utree->Branch("rich_pb",      &rich_pb,    "rich_pb/F");
    utree->Branch("rich_hit",     &rich_hit,   "rich_hit/I");
    utree->Branch("rich_stat",    &rich_stat,  "rich_stat/I");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}


int AMSAlignTree::LoadHeader(){

///--RTI
     AMSSetupR::RTI::UseLatest(7);

///--Track
     TkDBc::UseFinal();
     TRMCFFKEY_DEF::ReadFromFile = 0;
     TRFITFFKEY_DEF::ReadFromFile = 0;
     TrInnerDzDB::ForceFromTDV = 1;
     cout<<"NewN TkVersion Used"<<endl;

    return 0;
}


int AMSAlignTree::LoadHeaderEv(){

//---Center of Gravity
    TRCLFFKEY.ClusterCofGOpt=1;
    if(isreal){TrLinearEtaDB::SetLinearCluster();}
    return 0;
}


int  AMSAlignTree::LoadHeaderMC(){

   pev->SetDefaultMCTuningParameters();
   return 0;
}


int AMSAlignTree::Select(){

//---IsReal
   isreal=(pev->nMCEventg()==0);
   static bool hasev=0;
   if(hasev==0){
      for(int it=0;it<3;it++)if(isreal==0){sstree[2]+=AddBranch_MC(tout[it]);}
      hasev=1;
   }

//----
   clock_t tb,te;
   sstat[0]++;

//---Datacard
   LoadHeaderEv();

//---Header 0
   tb=clock();
   bool selhead=Select_Header();
   te=clock();
   cput[0]+=te-tb;
   if(!selhead)return 0;
   sstat[1]++;

//---Trigger 1
   tb=te;
   bool seltrig=Select_Trigger();
   te=clock();
   cput[1]+=(te-tb);
   if(!seltrig)return 0;
   sstat[2]++;

   if(isreal==0){
     Select_MC();
     LoadHeaderMC();
   }

//---Particle 2
   tb=te;
   bool selpart=Select_Particle();
   te=clock();
   cput[2]+=(te-tb); 
   if(!selpart)return 0;
   sstat[3]++;
 
//--Preselection Track 3
   tb=te;
   bool seltrk=Select_Trk();
   te=clock();
   cput[3]+=(te-tb);
   if(!seltrk)return 0;
   sstat[4]++;

//--Strict Selection Tof 4
   tb=te;
   bool seltof=Select_Tof();
   te=clock();
   cput[4]+=(te-tb);
   if(!seltof)return 0;
   sstat[5]++;

//--Rich 6
   tb=te;
   bool selrich=Select_Rich();
   te=clock();
   cput[5]+=(te-tb);
//   if(!selrich)return 1;
   sstat[6]++;

   return true;
}

//---Header
bool  AMSAlignTree::Select_Header(){

   run=pev->Run();
   event=pev->Event();
   version=pev->Version();
//   cout<<"version="<<version<<endl;
   errorb=pev->fHeader.Error;
   time[0]=pev->fHeader.Time[0];
   time[1]=pev->fHeader.Time[1];
//--ISS
   yaw=   pev->fHeader.Yaw;
   pitch= pev->fHeader.Pitch;
   roll=  pev->fHeader.Roll;
//--Magnetic
   thetam=pev->fHeader.ThetaM;
   phim=pev->fHeader.PhiM;
//---only real data
   if(isreal==0)return true;

   isbadrun=pev->isBadRun(run);
   return true;
}

//---Trigger
bool AMSAlignTree::Select_Trigger(){

    nlevel1=pev->nLevel1();
    livetime=pev->LiveTime();
    if(nlevel1>0){
      Level1R *lv=pev->pLevel1(0);
//      livetime=  lv->LiveTime;
      physbpatt= lv->PhysBPatt;
      jmembpatt= lv->JMembPatt;
//Add MC
      if(isreal==0){
         jmembpatt1=physbpatt1=0;
         lv->RebuildTrigPatt(jmembpatt1,physbpatt1);
       }
//Add ISS
      else {
         jmembpatt2=physbpatt2=0; 
         lv->RestorePhysBPat();
         physbpatt2=lv->PhysBPatt;
         jmembpatt2=lv->JMembPatt;
      }
//---
    }
    return true;
}


//---MC
bool AMSAlignTree::Select_MC(){

   if(isreal)return true;

   int mnpart=pev->nMCEventg();
   MCEventgR *mcev0=pev->pMCEventg(0);
   MCEventgR *mcev=pev->GetPrimaryMC();
//----
   mpar=mcev->Particle;
   mmom=mcev->Momentum;
   mch= mcev->Charge;
//----Coo+Dir
   for(int ico=0;ico<3;ico++){
      mevcoo[ico]=mcev->Coo[ico]; mevdir[ico]=mcev->Dir[ico];
   }
//--TrackMCCluster MaxE
   for(int ilay=0;ilay<9;ilay++){
      mtrmom[ilay]=0;
      for(int ic=0;ic<3;ic++){mtrcoo1[ilay][ic]=0;mtrdir1[ilay][ic]=0;}
    }
    for(int itr=0;itr<pev->nTrMCCluster();itr++){
      TrMCClusterR *trmc=pev->pTrMCCluster(itr);
      int il=fabs(trmc->GetTkId()/100);
      if     (il==8)il=1;
      else if(il<8)il=il+1;
      il--;
      if(trmc->GetMomentum()>mtrmom[il]){
        mtrmom[il]=trmc->GetMomentum();
        for(int ic=0;ic<3;ic++){mtrcoo1[il][ic]=trmc->GetXgl()[ic];mtrdir1[il][ic]=trmc->GetDir()[ic];}
      }
    }

//---MCEvengN Interaction Point Find Max Kine
   for(int iz=0;iz<21;iz++)mevmom1[iz]=-1000;
   for(int ipar=1;ipar<mnpart;ipar++){
      MCEventgR *mcev=pev->pMCEventg(ipar);
      int iskip=mcev->Nskip;
      if(iskip>=-1020&&iskip<=-1000){
         iskip=fabs(iskip)-1000;
         mevmom1[iskip]=mcev->Momentum;
      }
    }

   return true;
}

//---Particle
bool AMSAlignTree::Select_Particle(){

//---Load Status
   ntrack=pev->nTrTrack();
   ntrdtrack=pev->nTrdTrack();
   ntofclh=pev->nTofClusterH();
   nbetah=pev->nBetaH();

//---PreScale 
   if(pev->nTrTrack()<1||pev->nBetaH()<1||pev->nParticle()<1)return false;//require Tk

//----
   ibetah=pev->pParticle(0)->iBetaH();
   irich=pev->pParticle(0)->iRichRing();
   if(ibetah<0)return false;
   BetaHR *betah=pev->pBetaH(ibetah);
   itrtrack=betah->iTrTrack();
   if(itrtrack<0||betah->GetBuildType()!=1||betah->GetSumHit()<3||fabs(betah->GetBeta())<0.4)return false;
   TrTrackR *trk=pev->pTrTrack(itrtrack);
   if(trk->GetNhitsI(1)<5||trk->GetNhitsI(0)<3)return false;//>=5HitY, >=3HitX
//-----
  return true;
}


//---Init Trk ReFit
int AMSAlignTree::InitTrkReFit(float tkiq,float tkfz,int &refit,float &mass,int &chrg,int algo){

//--Fiting Charge and Mass 
    chrg=int(tkiq+0.5);
    if(chrg<1)chrg=1;
    mass=TrFit::Mproton;//Proton
    if(chrg>=2)mass=TrFit::Mhelium/2*chrg;//Helium+Ion
    if(!isreal&&version<1106)TRFITFFKEY.ErcHeY=0.65;//OldVersion MC
    else                     TRFITFFKEY.ErcHeY=0;//Newweight (NewMC||ISS)
//--ISS Setting
    if(isreal){
       TRFITFFKEY.Zshift=2;
//       refit=3;//Force ReFit
       if(tkfz<0||chrg==int(tkfz+0.5))refit=0;
       else                           refit=3;
//       if(refit==3)cout<<"refit="<<refit<<" chrg="<<tkiq<<","<<tkfz<<endl;
    }
//--MC Smearing
    else  {
      if(issmearext==0){//once
        TrExtAlignDB::SmearExtAlign();//MC Smear Ext-Layer
        TRCLFFKEY.UseSensorAlign = 0;
        issmearext=1;
      }
      TRFITFFKEY.Zshift=-1;
      refit=3;//Force ReFit
    }

    return 0;
}

//---Trk
bool AMSAlignTree::Select_Trk(){
    int mfit=-1;
    TrTrackR *trk=pev->pTrTrack(itrtrack);
    if(!trk)return false;
    float umass=0; int fid=0;
    BetaHR *betah=pev->pBetaH(ibetah);
    float obeta=betah?betah->GetBeta():1;
    float frig=trk->GetRigidity(fid);

//---New charge All 
    for(int isp=0;isp<2;isp++){
//--Float Z
       mean_t tkqa;
       if(isp==0)tkqa=trk->GetQ_all(obeta,fid);
       else      tkqa=trk->GetInnerQ_all(obeta,fid);
       tk_q[isp]   =tkqa.Mean;
       tk_qrms[isp]=tkqa.RMS;
     }

//---New Charge Layer     
     for(int il=0;il<9;il++){
       tk_lid[il]=0;
       tk_qls[il]=0;
       tk_ql2[il][0]=tk_ql2[il][1]=0;
       tk_ql[il]=trk->GetLayerJQ(il+1,obeta,fid);
       TrRecHitR* tkhit =trk->GetHitLJ(il+1);
       if(!tkhit)continue;
       tk_lid[il]=tkhit->GetTkId();
//       cout<<"tkid="<<tk_lid[il]<<endl;
       tk_qls[il]=tkhit->GetQStatus();
       tk_ql2[il][0]=tkhit->GetQ(0,obeta,frig);//XQ
       tk_ql2[il][1]=tkhit->GetQ(1,obeta,frig);//YQ
     }
    

//---Tk quality
    int algo=1;// 1:Choutko 2:Alcaraz
    int patt=3;// 3:Inner 5:L1I 6:IL9 7:L1IL9
    int refit=3;
    float mass=TrFit::Mproton;
    int chrg=1;

//---Hit Pattern
    tk_rtype=trk->GetRecType();
    tk_hitb[0]=trk->GetBitPatternXYJ(); tk_hitb[1]=trk->GetBitPatternJ();
    tk_fzm[0]=trk->GetAdvancedFitCharge(); tk_fzm[1]=trk->GetAdvancedFitMass();
    float tkfz=tk_fzm[0];
    if(ibetah<0){tkfz=-1;}//ibetah=-1 reassigned tkfz

//---
    float nrig=0;
    float enrig=9999;
    float nchis[3]={9999,9999,9999};
    float ntkres[9][3];
    for(int il=0;il<9;il++){
      for(int ixy=0;ixy<3;ixy++){ntkres[il][ixy]=9999;tk_hitc[il][ixy]=0;}
      for(int ixy=0;ixy<2;ixy++){tk_lch[il][ixy]=-1;};
    }
//---Init Track ReFit
    InitTrkReFit(tk_q[1],tkfz,refit,mass,chrg,algo);
    refit=refit%10;//Ex-PG
    TrFit trfit;
    mfit=trk->iTrTrackPar(trfit,algo,patt,refit,mass,chrg);
    if(mfit<0)return false;
//--
    if(mfit>=0){
      nrig=trk->GetRigidity(mfit);
      enrig=trk->GetErrRinv(mfit);
      nchis[0]=trk->GetNormChisqX(mfit);
      nchis[1]=trk->GetNormChisqY(mfit);
      nchis[2]=trk->GetChisq(mfit);
      for(int il=0;il<NTKL;il++){
        AMSPoint pnt=trk->GetResidualJ(il+1,mfit);
        ntkres[il][0]=pnt[0];
        ntkres[il][1]=pnt[1];
        pnt=trk->GetHitCooLJ(il+1,0);//PG Alignment
        TrRecHitR *tkhit=trk->GetHitLJ(il+1);
        if(!tkhit)continue;
        int imult=tkhit->GetResolvedMultiplicity();//need refine
        for(int ixy=0;ixy<3;ixy++)tk_hitc[il][ixy]=pnt[ixy];
//------
        int z=0;
        if(chrg>0){z=(chrg>1)?int(chrg+0.5):1;}
        TrClusterR *clx=tkhit->GetXCluster();
        TrClusterR *cly=tkhit->GetYCluster();
        double ladchanx=-10,ladchany=-10;
        double dzondxy[2]={0};
        TkLadderN *ladder=TkTrackN::GetHead()->GetLadder(tkhit->GetTkId());
        if(clx){
          ladchanx=clx->GetXCofG(TrClusterR::DefaultUsedStrips,imult,TrClusterR::DefaultBestResidualOpt,z,-2,-2,2);
          if(ladchanx>ladder->nchans[0]-0.5){//ladder channel overflow
            int umult;
            double daqchanx=ladder->GetDAQChanX(ladchanx,umult);
            int mmult=ladder->GetMaxMult(daqchanx);
            if(imult>mmult)imult=mmult;//protect imult overflow
            ladchanx=clx->GetXCofG(TrClusterR::DefaultUsedStrips,imult,TrClusterR::DefaultBestResidualOpt,z,-2,-2,2);//try again
          }
//          if(dzshiftondxy)dzondxy[0]=clx->GetdZShiftondXY(z,dxdz,dydz);
        }
        else {
          float dummyX=tkhit->GetDummyX();//need refine
          int mmult=ladder->GetMaxMult(dummyX+640);
          if(imult>mmult)imult=mmult;//protect imult overflow
          ladchanx=TkLadderN::GetLadderChanX(dummyX+640,imult);
        }
        if(cly){
          ladchany=cly->GetXCofG(TrClusterR::DefaultUsedStrips,imult,TrClusterR::DefaultBestResidualOpt,z,-2,-2,2);
//        if(dzshiftondxy)dzondxy[1]=cly->GetdZShiftondXY(z,dxdz,dydz);
        }
//        cout<<"ladchan1="<<setprecision(8)<<ladchanx<<","<<setprecision(8)<<ladchany<<endl;
        tk_lch[il][0]=ladchanx;
        tk_lch[il][1]=ladchany;
//        cout<<"ladchan2="<<setprecision(8)<<tk_lch[il][0]<<","<<setprecision(8)<<tk_lch[il][1]<<endl;
//------
      }
    }
//--Select Copy To Data
    tk_rigidity1=nrig;
    tk_erigidity1=enrig;
    for(int ixy=0;ixy<3;ixy++){//Algo+PG(CIEMAT+All)+Span
      tk_chis1[ixy]=nchis[ixy];
    }
//    for(int il=0;il<NTKL;il++){for(int ixy=0;ixy<2;ixy++)tk_res[il][ixy]=ntkres[il][ixy];}
  
//--Track Pos
    for(int ilay=0;ilay<NTKL;ilay++){
      AMSPoint postr;AMSDir dirtr;
      trk->InterpolateLayerJ(ilay+1,postr,dirtr);
      for(int ic=0;ic<3;ic++){tk_pos[ilay][ic]=postr[ic];tk_dir[ilay][ic]=dirtr[ic];}
   }

//---Others/Calibration
    for(int il=0;il<9;il++){
       tk_cad[il]=tk_stripx[il]=-1;
       TrRecHitR* tkhit =trk->GetHitLJ(il+1);
       if(!tkhit)continue;
       int k7=TkDBc::Head->FindTkId(tkhit->GetTkId())->IsK7()?1:0;
       int imul=tkhit->GetResolvedMultiplicity();
       if(imul<0||imul>=9)imul=9;//impossible
       tk_cad[il]=(k7*10+imul)*100000000;
       for(int ixy=0;ixy<2;ixy++){
         TrClusterR *cl=(ixy==0)?tkhit->GetXCluster():tkhit->GetYCluster();
         if(!cl)continue;
         int opt=(isreal)? (TrClusterR::kAsymEta|TrClusterR::kOverflow): TrClusterR::kAsym;//QY p/n new asymmetry for ISS
         int cstrip,sstrip;//seed/second strip index
         int noverflow=cl->GetSeedSecondIndex(cstrip,sstrip,opt);//number of ADC overflow strips nearby the seed strip
         tk_cad[il]+=int(pow(10000.,ixy))*cl->GetAddress(cstrip);//1000000000*K7+100000000*Mult+10000*y(address)+x(address)
         if(ixy==0){//X
           int sensor;
           int mcstrip=cl->GetSeedIndex();//muliplicity defined by old-cof algorithm with old-seed
           if(cl->GetResolvedMultiplicity(cstrip,imul,mcstrip)==-3)imul--;//QY K7 new seed address overflow
           int sensor_cad=cl->GetSensorAddress(sensor,cstrip,imul,mcstrip);//sesor/local sensor-address for seed
           int striptype=cl->GetSeedSecondPairType(imul,mcstrip,opt)%1000;//type of seed-second strips location
           int ik=0;
           if     (striptype==102||striptype==202)ik=1;//niters=1(2)/K5/type=2
           else if(striptype==113)                ik=2;//niters=1   /K7/type=3(sensor-address: 64-159)
           else if(striptype==114)                ik=3;//niters=1   /K7/type=4(sensor-address: 0-63,160-223)
           else if(striptype== 14)                ik=4;//niters=0   /K7/type=4(sensor-address: 0-63,160-223)
           tk_stripx[il]=ik+sensor*10+sensor_cad*1000+noverflow*1000000;
         }
//---adc
      }
   }

   return true;
}


bool AMSAlignTree::Select_Tof(){

//---
   int fid=0;
   float frig=(itrtrack>=0)?pev->pTrTrack(itrtrack)->GetRigidity(fid):0;//default
//--
   BetaHR *betah=pev->pBetaH(ibetah);
      
   if(!betah)return false;
   tof_btype=betah->GetBuildType();
   tof_bpatt=betah->GetBetaPattern();

   tof_betah=betah->GetBeta();
//---Normalized
   TofBetaPar normbetapar=betah->GetNormTofBetaPar();
   tof_chist_n=normbetapar.Chi2T; 
   tof_chisc_n=normbetapar.Chi2C;
//----
   tof_hsumh=betah->GetSumHit();
   tof_hsumhu=betah->GetUseHit();

//--TOFCharge All
   tof_qs=0;
   TofChargeHR *tofhc=betah->pTofChargeH();
   int nlay=0;

//---TOFQL new
   tofhc->ReFit(tof_betah,TofChargeHR::DefaultQOptIonW,frig); 
//---TOFQL
   for(int ilay=0;ilay<NTOFL;ilay++){
//---Clear
     tof_ql[ilay]=0;
//---QL
     if(!tofhc->TestExistHL(ilay))continue;
     tof_ql[ilay] =tofhc->GetQL(ilay,22);//New TOFL
     if(tofhc->IsGoodQPathL(ilay)){tof_qs+=int(pow(10.,3-ilay));}
   }


  return true; 
}


bool AMSAlignTree::Select_Rich(){

//---
  RichRingR *ring=pev->pRichRing(irich); 
  if(!ring)return false;
  if(ring->iTrTrack()!=itrtrack){irich=-1;return false;}
  rich_beta=ring->getBeta();
  rich_pb=ring->getProb();
  rich_hit=ring->getHits();
  rich_stat=0;
  if(ring->IsNaF())rich_stat+=1;
  if(ring->IsClean())rich_stat+=10;
  if(ring->IsGood())rich_stat+=100;
}



void AMSAlignTree::InitEvent(AMSEventR *ev){
//---amsdn
   amsdn=164; //vers*100+amsd
//---Header
   run=0;event=0;version=0;errorb=0;time[0]=0;time[1]=0;isbadrun=0;
//---Trigger
   nlevel1=-1;
//--Particle
   nbetah=ntrack=ntofclh=-1;
//---
   itrtrack=ibetah=irich=-1;
//---TOF
   tof_betah=-3;
   tof_hsumh=tof_hsumhu=-1;
//---Track
   issmearext=0;
   tk_q[0]=tk_q[1]=-1;
   tk_rtype=tk_hitb[0]=tk_hitb[1]=0;
   tk_fzm[0]=tk_fzm[1]=0;
//----Rich
   rich_beta=-3;
   rich_stat=0;

  pev=ev;
}

int AMSAlignTree::Fill(int it){
  if     (it==0){return tout[0]->Fill();}
  else if(it==1){return tout[1]->Fill();}
  else          {return tout[2]->Fill();}
}

int AMSAlignTree::Save(){
    int fsave=0;
    fout->cd();
    for(int it=0;it<3;it++)tout[it]->Write();
    if(fout->IsZombie())fsave=1;
    fout->Close();
    return fsave;
}

int stop=0;
void shandler(int sig){
  stop=1;
  return;
}

int main(int argc,char ** argv){//in_file ofile num
    char ifile[1000];
    char ofile[1000];
    int rmode=0;//readmode 0 txt 1 root
    int num=-1;
    int nstat=9;
    int nrun=-1;
    cout<<"argc="<<argc<<endl;
    if(argc>=3){
      sprintf(ifile,"%s",argv[1]);
      sprintf(ofile,"%s",argv[2]);
      cout<<"ifile="<<ifile<<endl;
      cout<<"ofile="<<ofile<<endl;
    }
    else {
        printf("Usage: %s  <infile_txt> <ofile.root> <max_events_number>\n",argv[0]);
        exit(-1);
    }

    if (argc>=4){
       rmode=atoi(argv[3]);
       cout<<"rmode="<<rmode<<endl;
    }
    if(argc>=5){
       num=atoi(argv[4]);
       if(num==0)num=-1;
       cout<<"num="<<num<<endl;
    }
//    char *otxt="./bac/";
    char *otxt=0;
    unsigned int srun=0;
    if(argc>=6){
      if(rmode==2){srun=stoul(argv[5]);   cout<<"srun="<<srun<<endl;}
      else        {otxt=argv[5];          cout<<"otxt="<<otxt<<endl;}
    }

    char *ofilecp=0;
    unsigned int sevent=0;
    if(argc>=7){
      if(rmode==2){sevent=stoul(argv[6]); cout<<"sevent="<<sevent<<endl;}
      else        {ofilecp=argv[6];       cout<<"ofilecp="<<ofilecp<<endl;}
    }

    signal(SIGTERM, shandler);
    signal(SIGINT, shandler);

    AMSChain ams;
    int nfile=-1;
    int nread=-1;
    if(rmode==1||rmode==2){
        ams.Add(ifile);
        nread=1;
    }
    else{
       nread=readfile(ifile,ams,nfile,0,otxt,rmode);
       if(nread<0){//read-error
         printf("error files read:%s \n",ifile);
         return 5;
       }
    }//read file

//---
    AMSAlignTree *analysis =new AMSAlignTree();
    analysis->BookFile(ofile);
    analysis->LoadHeader();
///---Baichi
    Long64_t num2= ((num==-1)||(num>ams.GetEntries()))?ams.GetEntries():num;
    cout<<"num2="<<num2<<endl;
    int nevout=300000;
    static unsigned int prrun=0;
    static Long64_t nevtree=0;//Local Tree Events
    static string bf="";//bad Tree
    static string pf="";//Pre Tree
    AMSEventR::fRunList.clear();//Clear AMSEventR:fRunList
/*#ifdef _USEONEEV_
    srun=604311619;
    sevent=751735;
#endif*/
    if(srun!=0&&sevent!=0)num2=1;
    for(Long64_t entry=0; entry<num2; entry++){
        AMSEventR *ev=NULL;
        if(num2==1)ev=(AMSEventR *)ams.GetEventFast(srun,sevent);
#if defined (_USEEVENTORDER_)
        else       ev=(AMSEventR *)ams.GetOrderedEntry(entry);
#else
        else       ev=(AMSEventR *)ams.GetEvent(entry);
#endif
        if(ev==NULL)continue;
        if(entry>=ams.GetEntries())break;
        if(ev->Run()!=prrun){cout<<"ProcessInfo NewRun="<<ev->Run()<<endl;prrun=ev->Run();}
        if(entry%nevout==0)cout<<"ProcessInfo Entry="<<entry<<" Run="<<ev->Run()<<" Event="<<ev->Event()<<endl; 
        int records=ev->RecordRTIRun();//Run to AMSEventR:fRunList
//----Check Run in AMSEventR:fRunList
        TTree *evtree=ev->Tree();
        string nf=evtree->GetCurrentFile()->GetName();
        if (nf==bf)continue;//BadRunList
        if (nf!=pf){//New Tree
          nevtree=0;
          if(records==2){//Exist in RunList
            cout<<"Error ExistFile in RunList="<<nf<<endl;
            pf=bf=nf;//Exist to  BadRunList
            continue;
          }
          else {
            pf=nf;
          }
        }
        else {
          nevtree++;
        }
//---
       if(num2==1)cout<<"run="<<ev->Run()<<" event="<<ev->Event()<<endl;
//       cout<<"run="<<ev->Run()<<" event="<<ev->Event()<<endl;
//---Set Unbiase
       analysis->unbiasetag0=-1;//Pre Unbiase-Tag
       analysis->unbiasetag=0;//ISS
       analysis->istkaddhit=0;//AddLostHit-Tag
//---Set Pre Unbiase
       if(entry%prscale==0)analysis->unbiasetag0=0;
//       cout<<"current event="<<ev->Tree()->GetReadEvent()<<" "<<ev->Tree()->GetEntries()<<" cev"<<nevtree<<endl;
       if((evtree->GetReadEvent()==0)||(evtree->GetReadEvent()==evtree->GetEntries()-1)||(nevtree==0)||(nevtree==evtree->GetEntries()-1)){//tree bein+end |entry begin+end 
         analysis->unbiasetag0=1;
       }
       if(analysis->unbiasetag0>=0){
         analysis->InitEvent(ev);
         int ksel=analysis->Select();
         if(ksel==1){
           int ktree=analysis->Fill(1);
           if(ktree<0){cout<<"Error Write Tree1"<<endl;nstat=6;goto ENDList;}
         }
       }
//----Set ISS|MC Unbiase
        if(entry%prunbias==0)analysis->unbiasetag=1;
        if(analysis->unbiasetag){
          analysis->InitEvent(ev);
          int ksel=analysis->Select();
          if(ksel==1){
            int ktree=analysis->Fill(2);
            if(ktree<0){cout<<"Error Write Tree2"<<endl;nstat=6;goto ENDList;}
          }
        }
        prtime=ev->UTime();
//---
        if(stop==1){cout<<"Force stop"<<endl;nstat=4;goto ENDList;}//force stop
    }


//--Write To List
  {
    map <string,unsigned int >runlistn;
    for(map<unsigned int,AMSSetupR::RunI>::iterator it=AMSEventR::fRunList.begin();it!=AMSEventR::fRunList.end();it++){//Get Run by Run
       AMSSetupR::RunI runi=it->second; //run information
       for(int k=0;k<runi.fname.size();k++)runlistn[runi.fname[k]]=runi.run;
    }
    nrun=runlistn.size();
    AMSEventR::fRunList.clear();
    int fsave=analysis->Save();
    if(fsave){cout<<"Error Saving File="<<ofile<<endl;nstat=6;}
    else if(nrun>0){
//---CopyToEOS
      int cpstat=cpofile(ofile,ofilecp,0);
//---Recording to txt if CopyToEOS success
      if(cpstat==0){WriteList(otxt,runlistn); nstat=0;}//normal
      else         {nstat=3;}//copy-error
    }
    else {nstat=2;}//no-run
    if((nrun!=nread)&&(nstat==0||nstat==2))nstat+=10;//nrun!=nread
  }

///--List Status
ENDList:
   const char *sname[6]={
      "Head  ", "Trig  ", "Part    ","Trk    ","Tof     ","Rich    ",
    };
///---
    cout<<"<<---------Begin Static-------------->>"<<endl;
    cout<<"   Event("<<sstat[0]<<")"<<"   CPU"<<"   TreeSize"<<endl;
    for(int icut=0;icut<6;icut++){
      cout<<sname[icut]<<"   "<<sstat[icut+1]<<"   "<<(double(cput[icut]/CLOCKS_PER_SEC))<<"   "<<sstree[icut]<<endl;
     }
    cout<<"    Process Run="<<nrun<<" Stat="<<nstat<<endl;
    if(nrun!=nread)cout<<"    Read Run="<<nread<<endl;
    cout<<"<<---------End Static AMSJOB-------------->>"<<endl;

//-----
   return nstat;//nstat=0 normal, nstat=2 no-run, nstat=3 copy-error, nstat=4 stop-error, nstat=5 read-error,nstat=6 save-error,nstat=9 initialization-error,nstat+10 nrun!=nread
   

}
