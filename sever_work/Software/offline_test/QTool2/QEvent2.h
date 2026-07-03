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
#include <float.h>
//#include <stdio.h>
//#include <limits.h>
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
   int  AddBranch_RTI(TChain *ch);
   int  AddBranch_MC (TChain *ch);
   int  AddBranch_Particle(TChain *ch);
   int  AddBranch_Trk (TChain *ch);
   int  AddBranch_Tof (TChain *ch);
   int  AddBranch_Trd (TChain *ch);
   int  AddBranch_Rich(TChain *ch);
   int  AddBranch_Ecal(TChain *ch);
   int  AddBranch_Anti(TChain *ch);
   int  AddBranch_TrkParticle(TChain *ch);
   bool IsGoodRTI(int opt=2);
//----
   inline virtual int   Getamsdn();
   inline virtual unsigned int GetJINJLength(int ijinj);
//---Hit Pattern
   inline virtual int   HasTkLHitXY(int ilay);// 2-XY 1-Y 0-NO
   inline virtual int   GetTkInNHit(int ixy=1,int ispan=0);
   inline virtual int   GetiPlane(int ilay);
   inline virtual int   HasTkPlaneXY(int iplane);// 2-XY 1-Y 0-NO
   inline virtual int   GetTkInNPlane(int ixy=1);
//---Geometry
   inline virtual int   Interpolate(double zpl,double pnt[3]);
   inline virtual int   InterpolateL(int ilay,double pnt[3],double dir[3]);
   inline virtual int   InterpolateMC(double zpl,double pnt[3]);
   inline virtual bool  IsPassTkL(int ilay);
   inline virtual bool  IsPassECAL();
   inline virtual bool  IsPassTRD();
   inline virtual bool  IsPassRich();
//----
   inline virtual float GetSABeta();
//---Charge
   inline virtual float GetMean(vector<float> signal,float &rms,int opt);
   inline virtual float GetSATOFLQ(int ilay);
   inline virtual float GetTOFLQ(int ilay);
   inline virtual int   GetTOFLZ(int ilay);
   inline virtual float GetTOFUDQ(int iud=0);
   inline virtual int   GetTOFUDZ(int iud=0);
   inline virtual bool  IsGoodTkLQStat(int ilay,int opt=1);//opt=0 loose, opt=1 tight
   inline virtual bool  IsGoodTOFLQStat(int ilay);
   static bool          IsGoodRichTile(int utile,int opt);//Good-RichTile
   inline virtual bool  IsGoodRichTile(int opt=0);
   inline virtual bool  IsGoodRichGeom();
   inline virtual int   GetTOFNGoodQL(int iud=-1);
   inline virtual int   GetTOFNLayer(int isbeta=0);
   inline virtual float GetSABZQUnbias(int ilay,int calib=0);
   inline virtual float GetTkBZQUnbias(int ilay,int ixy=2,int calib=0);
   inline virtual float GetTkLQ(int ilay,int ixy=2,int calib=0);
   inline virtual int   GetTkLZ(int ilay,int ixy=2,int calib=0);
   inline virtual float GetTkInQ(int ixy,float &qrms,int opt=0,int calib=0);
   inline virtual float GetTkInQ(int ixy=2,int calib=0);
   inline virtual float GetTkInQRMS(int ixy=2,int calib=0);
   inline virtual int   GetTkInZ(int ixy=2,int calib=0);
   inline virtual float GetRichQ();
//---Hit and Rigidity
   inline virtual int   GetSpanArrayI(int ispan=0);
   inline virtual float GetRigidity(int ispan=0,int ialgo=0,int ival=2,int iz=0);//ialgo=0 VC, ialgo=1 Alcaraz
   inline virtual float GetERigidity(int ispan=0,int ialgo=0,int opt=0);
   inline virtual float GetChis(int ispan=0,int ixy=1,int ialgo=0,int ival=2);//X+Y+XY
//   inline virtual float GetChisExt(int ispan=0,int ixy=1,int ialgo=0,int ival=2);
//   inline virtual float GetPVal(int ispan=0,int ixy=1,int ialgo=0);
   inline virtual float GetL1ChisY(int ialgo=0,int ival=2);
   inline virtual float GetL9ChisY(int ialgo=0,int ival=2);
   inline virtual float GetL19ChisY(int ialgo=0,int ival=2);
   inline virtual float GetTkLRes(int ilay,int ixy=1,int nobias=1);//um
   inline virtual float GetDiffPGMD(int ilay,int ixy=1);//um
   inline virtual float GetEcalEnergy(int ie=0);//energye+enerya+energyd
   inline virtual float GetRichBeta();
   static float GetMass(float urig,int uz,float ubeta);
   static float GetMass2(float urig,int uz,float ubeta);
   static float GetEMass(float urig,float evrig,int uz,float ubeta,float ebeta);//d(1/r) dbeta
   static float GetEMass2(float urig,float evrig,int uz,float ubeta,float ebeta);//d(1/r) dbeta
   static float GetRigidityBeta(float urig,int uz,float umass);
   static int   GetTrMCClusterZA(int mcpar,int &mcz,int &mca);
//---
   virtual bool  Select_Trigger(int nanti=4);
   virtual bool  Select_RTI(int opt=2);//opt=2, opt=1 without rtinexl, opt=0 without zenith
   virtual bool  Select_Particle();
   virtual bool  Select_TkGeom(int igeom=2,int opt=0);
   virtual bool  Select_TkInNHit(int opt=0,int ixy=1);//opt=0 loose, opt=1 tight, opt=2 ttight
   virtual bool  Select_Tk(int igeom=2);
   virtual bool  Select_TofBeta();
   virtual bool  Select_Tof();
   virtual bool  Select_TkInQ(int opt=0,int zref=0,int calib=0);
   virtual bool  Select_TofUQ(int opt=0,int zref=0);//opt=0 loose, opt=1 tight
   virtual bool  Select_TofDQ(int opt=0,int zref=0);
   virtual bool  Select_TofLQ(int ilay,int opt=0,int zref=0);
   virtual bool  Select_TkExtQ(int ispan=0,int opt=0,int zref=0,int calib=0);//0-In,1-L1,2-L1L9,3-L9
   virtual bool  Select_RichQ();
   virtual bool  Select_TkL1ChisY(int ialgo=0,int ival=2);
   virtual bool  Select_TkL19ChisY(int ialgo=0,int ival=2);
   virtual bool  Select_TkRig(int ispan=0,int ialgo=0,int ival=2,float ychis=-1,float ychisex=0);
   virtual bool  Select_Tk2nd(int opt=0);
   virtual bool  Select_Rich(int opt=1);
   virtual bool  Select_TrdK();
   virtual bool  Select_Ecal(int opt=0);//opt=0 loose, opt=1 tight
public:
//---Head
   bool         isreal;
   unsigned int run;
   unsigned int event;
   int          amsdn;
   int          version;
   unsigned int time[2];
   unsigned int errorb;
//---Trigger
   int          daqtyerr;
   unsigned int daqjrmerr;
   unsigned int daqjlen[24];
   unsigned int daqsdlen;
   unsigned int daqsdlenu;
   int          nlevel1; 
   int          physbpatt;
   int          physbpatt1;
   int          physbpatt2;
   int          jmembpatt;
   int          jmembpatt1;
   int          tofflag[2];
   int          antipatt;
//---RTI
   int          irti;
   float        zenith;
   float        thetas;
   float        phis;
   float        rads;
   float        glat;//galatic latti
   float        glong;//galatic longti
   bool         issaa;
   float        rtilf;
   float        rtinev;
   float        rtinerr;
   float        rtintrig;
   float        rtinpar;
   int          rtigood;
   float        mcutoff[4][2];
   float        mcutoffi[4][2];
   float        rtinexl[2][2];
//---MC
   float        mmom;
   float        mch;
   int          mpar;
   int          mtof_pass;
   float        mevcoo[3];
   float        mevdir[3];
   float        mfcoo[3];
   float        mpare[2];
   int          mparp[2];
   float        mparc[2];
   float        msume;
   float        msumc;
   float        mtrdmom[20];
   int          mtrdpar[20];
   float        mtrdcoo[20][3];
   float        mtrmom[9];
   int          mtrpar[9];
   int          mtrz[9];
   float        mtrcoo[9][3];
   float        mtrcoo1[9][3];
   int          mtrpri[9];
   float        mtofdep[4];
   int          mtofpri[4];
   int          mtofbar[4];
//---
   float        mevcoo1[21][3];
   float        mevdir1[21][3];
   float        mevmom1[21];
//---L1L9Particle
   float        tk_l1q;
   float        tk_l9q;
   float        tk_l1qxy[2];
   float        tk_l9qxy[2];
   int          tk_l1qs;
   int          tk_l9qs;
   int          ibetahs;
   int          itrdtracks;
   int          tof_nhits;
   int          tof_hsumhus;
   float        betahs;
   float        betahsmc;
   float        tof_chiscs;
   float        tof_chists;
   float        tof_qls[4];
   float        trd_rqs;
   float        ecal_pos[3];
   float        ecal_ens;
   float        ecal_dis;
   float        tk_l1mds;
   float        tk_pos1s[9][3];
   float        tk_l1qvs;
   float        tk_l9qvs;
   float        tk_exqvn[2][2];
   float        tk_l1qvr;
   float        tk_l9qvr;
//---Particle
   int          nbetah;
   int          ntrack;
   int          ntrrawcl;
   int          ntrcl;
   int          ntrhit;
   int          ntrdtrack;
   int          nrich;
   int          nrichh;
   int          nshow;
   int          ntofclh;
   int          ntrdseg;
   int          ntrdrawh;
   int          ibetah;
   int          itrtrack;
   int          itrdtrack;
   int          irich;
   int          ishow;
   float        ec_pos[3];
   float        cutoffp[2];
   float        cutoffpi[2];
//---2ndParticle
   int          betah2p;
   float        betah2q;
   float        betah2r;
   int          betah2hb[2];
   float        betah2ch;
//---Trk
   int          tk_rtype;
   int          tk_hitb[2];
   float        tk_q[2];
   float        tk_qrms[2];
   int          tk_rz[2][2];
   float        tk_rpz[2][2];
   float        tk_ql[9];
   float        tk_ql2[9][2];
   float        tk_qin[2][3];
   float        tk_qrmn[2][3];
   float        tk_qln[2][9][3];
   int          tk_qls[9];
   int          tk_qlsn;
   int          tk_lid[9];
   int          tk_cad[9];
   float        tk_cof[9][2];
   float        tk_adc[9][2][11];//L+XY+ii
//   float        tk_xco[9][2][11];//L+XY+ii
   float        tk_cdis[9];
   float        tk_crsr[9];
   float        tk_exql[2][3];
   float        tk_exqlt[2][2][3];
   float        tk_exqln[2][2];
   int          tk_exqls[2];
   float        tk_exdis[2];
   int          tk_exlid[2];
   int          tk_excad[2];
   float        tk_rigidity1[3][3][7];//Algo+PGCIEMAT+Span
   float        tk_rigidityk[4][7];//Kalman/Z+Span
   float        tk_erigidity1[3][7];//Algo+Span
   float        tk_chis1[3][3][7][3];////Algo+PGCIEMAT+Span+XYXY
//   float        tk_chisl19[2][2][3][2];
//   float        tk_pval[3][5][3];
   float        tk_res[9][2];
   float        tk_res1[9][2];
   float        tk_rigidityi[9];
   float        tk_chisi[9];
   float        tk_res1a[9][2];//Residual Span+Layer+XY
   float        tk_rigidityia[9];
   float        tk_chisia[9];
   float        tk_cdif[2][3];
   float        tk_pos[9][3];
   float        tk_dir[9][3];
   float        tk_hitc[9][3];
//   float        tk_hitcl[9][2];
   float        tk_oq[2];
   float        tk_oel[9];
   int          tk_ohitl[9];
   float        tk_rigidityr[4][3][7];//Z+Algo+Span
   float        tk_posr[4][3][7][3];//Z+Algo+Span+P
   float        tk_dirr[4][3][7][3];//Z+Algo+Span+P
//---Tof
   int          tof_btype;
   int          tof_bpatt;
   float        tof_beta;
   float        tof_betah;
   float        tof_betahmc;
   int          tof_hsumh;
   int          tof_hsumhu;
   int          tof_nclhl[4];
   int          tof_barid[4];
   float        tof_pos[4][3];
   float        tof_chisc;
   float        tof_chist;
   float        tof_chisc_n;
   float        tof_chist_n;
   float        tof_q[2];
   float        tof_qrms[2];
   int          tof_z;
   float        tof_pz;
   float        tof_ql[4];
   float        tof_qlr[3][4];
//   float        tof_qlc[4][4];
   float        tof_qlc[6][4];
   float        tof_tl[4];
   int          tof_qs;
   float        tof_oq[4][2];
   int          tof_ob[4][2];
   int          tof_oncl;
//---Trd
   int          nitrdseg;
   float        trd_rq;
   float        trd_qk[5];//A+U+D
   int          trd_qnhk[3];
   float        trd_qrmsk[3];
   int          trd_statk;
   int          trd_nhitk;
   int          trd_onhitk;
   float        trd_lik[3];
   float        trd_like[3];
//---Rich
   int          rich_itrtrack;
   float        rich_beta[3];
   float        rich_pb;//Prob To Ring
   float        rich_udis;
   int          rich_used;
   int          rich_stat;
   float        rich_q[2];
   float        rich_width;
   float        rich_npe[3];
   bool         rich_good;
   bool         rich_clean;
   bool         rich_NaF;
   float        rich_PMTChargeConsistency;
   float        rich_BetaConsistency;
   float        rich_pos[3];
   int          rich_tile;
   float        rich_distb;
//---Ecal
   unsigned int ecal_stat;
   float        ecal_en[4];
   float        ecal_enc[3];
   float        ecal_bdt[3];
   float        show_dis; 
   float        ecal_q;
   int          ecal_nhit[18];
   float        ecal_el[18];
   float        ecal_eh[18];
//---Anti
   int          anti_nhit;
//---TrkParticle
   int          itrtracks;
   int          tk_hitbs[2];
   float        tk_qis[2];
   float        tk_qirmss[2];
   float        tk_qlss[2][9];
   float        tk_rigiditys[5];
   float        tk_chiss[5][3];
   float        tk_pos2s[9][3];
   float        tk_dir2s[2][3];
   int          tk_ibetahs;
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
    ch->SetBranchAddress("daqtyerr",    &daqtyerr);
    ch->SetBranchAddress("daqjrmerr",   &daqjrmerr);
    if((amsdn%100)>=60)ch->SetBranchAddress("daqjlen",     daqjlen);
    ch->SetBranchAddress("daqsdlen",    &daqsdlen);
    ch->SetBranchAddress("daqsdlenu",   &daqsdlenu);
    ch->SetBranchAddress("nlevel1",     &nlevel1);
    ch->SetBranchAddress("physbpatt",   &physbpatt);
    ch->SetBranchAddress("physbpatt1",  &physbpatt1);
    ch->SetBranchAddress("physbpatt2",  &physbpatt2);
    ch->SetBranchAddress("jmembpatt",   &jmembpatt);
    ch->SetBranchAddress("jmembpatt1",  &jmembpatt1);
    ch->SetBranchAddress("tofflag",      tofflag);
    ch->SetBranchAddress("antipatt",    &antipatt);
    return 0;
}

int QEvent::AddBranch_RTI(TChain *ch){
    ch->SetBranchAddress("irti",      &irti);
    ch->SetBranchAddress("zenith",    &zenith);
    ch->SetBranchAddress("issaa",     &issaa);
    ch->SetBranchAddress("rtilf",     &rtilf);
    ch->SetBranchAddress("rtinev",    &rtinev);
    ch->SetBranchAddress("rtinerr",   &rtinerr);
    ch->SetBranchAddress("rtintrig",  &rtintrig);
    ch->SetBranchAddress("rtinpar",   &rtinpar);
    ch->SetBranchAddress("rtigood",   &rtigood);
    ch->SetBranchAddress("mcutoff",    mcutoff);
    ch->SetBranchAddress("mcutoffi",   mcutoffi);
    if((amsdn%100)>=68)ch->SetBranchAddress("rtinexl",     rtinexl);//P8
    return 0; 
}

int QEvent::AddBranch_MC(TChain *ch){
    ch->SetBranchAddress("mmom",      &mmom);
    ch->SetBranchAddress("mch",       &mch);
    ch->SetBranchAddress("mpar",      &mpar);
    ch->SetBranchAddress("mtof_pass", &mtof_pass);
    ch->SetBranchAddress("mevcoo",     mevcoo);
    ch->SetBranchAddress("mevdir",     mevdir);
    ch->SetBranchAddress("mfcoo",      mfcoo);
    ch->SetBranchAddress("mpare",      mpare);
    ch->SetBranchAddress("mparp",      mparp);
    ch->SetBranchAddress("mparc",      mparc);
    ch->SetBranchAddress("msume",     &msume);
    ch->SetBranchAddress("msumc",     &msumc);
    ch->SetBranchAddress("mtrdmom",    mtrdmom);
    ch->SetBranchAddress("mtrdpar",    mtrdpar);
    ch->SetBranchAddress("mtrdcoo",    mtrdcoo);
    ch->SetBranchAddress("mtrmom",     mtrmom);
    ch->SetBranchAddress("mtrpar",     mtrpar);
    ch->SetBranchAddress("mtrz",       mtrz);
    ch->SetBranchAddress("mtrcoo",     mtrcoo);
    ch->SetBranchAddress("mtrcoo1",    mtrcoo1);
    ch->SetBranchAddress("mtrpri",     mtrpri);
    ch->SetBranchAddress("mtofdep",    mtofdep);
    ch->SetBranchAddress("mtofpri",    mtofpri);
    ch->SetBranchAddress("mtofbar",    mtofbar);
//---
    ch->SetBranchAddress("mevcoo1",    mevcoo1);
    ch->SetBranchAddress("mevdir1",    mevdir1);
    ch->SetBranchAddress("mevmom1",    mevmom1);
    return 0;
}


int QEvent::AddBranch_Particle(TChain *ch){
//---L1L9Particle
    ch->SetBranchAddress("tk_l1q",     &tk_l1q);
    ch->SetBranchAddress("tk_l9q",     &tk_l9q);
    ch->SetBranchAddress("tk_l1qxy",    tk_l1qxy);
    ch->SetBranchAddress("tk_l9qxy",    tk_l9qxy);
    ch->SetBranchAddress("tk_l1qs",    &tk_l1qs);
    ch->SetBranchAddress("tk_l9qs",    &tk_l9qs);
    ch->SetBranchAddress("ibetahs",    &ibetahs);
    ch->SetBranchAddress("itrdtracks", &itrdtracks);
    ch->SetBranchAddress("tof_nhits",  &tof_nhits);
    ch->SetBranchAddress("tof_hsumhus",  &tof_hsumhus);
    ch->SetBranchAddress("betahs",     &betahs);
    if((amsdn%100)>=58&&(amsdn%100)<=68){
      ch->SetBranchAddress("betahsmc",   &betahsmc);
    }
    ch->SetBranchAddress("tof_chiscs", &tof_chiscs);
    ch->SetBranchAddress("tof_chists", &tof_chists);
    ch->SetBranchAddress("tof_qls",     tof_qls);
    ch->SetBranchAddress("ecal_pos",    ecal_pos);
    ch->SetBranchAddress("ecal_ens",   &ecal_ens);
    ch->SetBranchAddress("ecal_dis",   &ecal_dis);
    ch->SetBranchAddress("tk_l1mds",   &tk_l1mds);
    ch->SetBranchAddress("tk_pos1s",    tk_pos1s);
    ch->SetBranchAddress("tk_l1qvs",   &tk_l1qvs);
    ch->SetBranchAddress("tk_l9qvs",   &tk_l9qvs);
    ch->SetBranchAddress("tk_exqvn",    tk_exqvn);
    ch->SetBranchAddress("tk_l1qvr",   &tk_l1qvr);
    ch->SetBranchAddress("tk_l9qvr",   &tk_l9qvr);
    ch->SetBranchAddress("trd_rqs",    &trd_rqs);
//---Particle
    ch->SetBranchAddress("nbetah",    &nbetah);
    ch->SetBranchAddress("ntrack",    &ntrack);
    ch->SetBranchAddress("ntrrawcl",  &ntrrawcl);
    ch->SetBranchAddress("ntrcl",     &ntrcl);
    ch->SetBranchAddress("ntrhit",    &ntrhit);
    ch->SetBranchAddress("ntrdtrack", &ntrdtrack);
    ch->SetBranchAddress("nrich",     &nrich);
    ch->SetBranchAddress("nrichh",    &nrichh);
    ch->SetBranchAddress("nshow",     &nshow);
    ch->SetBranchAddress("ntofclh",   &ntofclh);
    ch->SetBranchAddress("ntrdseg",   &ntrdseg);
    ch->SetBranchAddress("ntrdrawh",  &ntrdrawh);
    ch->SetBranchAddress("ibetah",    &ibetah);
    ch->SetBranchAddress("itrtrack",  &itrtrack);
    ch->SetBranchAddress("itrdtrack", &itrdtrack);
    ch->SetBranchAddress("irich",     &irich);
    ch->SetBranchAddress("ishow",     &ishow);
    ch->SetBranchAddress("ec_pos",     ec_pos);
    ch->SetBranchAddress("cutoffp",    cutoffp);
    ch->SetBranchAddress("cutoffpi",    cutoffpi);
//---2ndParticle
    ch->SetBranchAddress("betah2p",   &betah2p);
    ch->SetBranchAddress("betah2q",   &betah2q);
    ch->SetBranchAddress("betah2r",   &betah2r);
    ch->SetBranchAddress("betah2hb",   betah2hb);
    ch->SetBranchAddress("betah2ch",  &betah2ch);
    return 0; 
}

int QEvent::AddBranch_Trk(TChain *ch){
    ch->SetBranchAddress("tk_rtype",    &tk_rtype);
    ch->SetBranchAddress("tk_hitb",      tk_hitb); 
    ch->SetBranchAddress("tk_q",         tk_q);
    ch->SetBranchAddress("tk_qrms",      tk_qrms);
    ch->SetBranchAddress("tk_rz",        tk_rz);
    ch->SetBranchAddress("tk_rpz",       tk_rpz);
    ch->SetBranchAddress("tk_ql",        tk_ql);
    ch->SetBranchAddress("tk_ql2",       tk_ql2);
    if((amsdn%100)>=55){
      ch->SetBranchAddress("tk_qin",       tk_qin);
      ch->SetBranchAddress("tk_qrmn",      tk_qrmn);
      ch->SetBranchAddress("tk_qln",       tk_qln);
    }
    ch->SetBranchAddress("tk_qls",       tk_qls);
    if((amsdn%100)>=64){
      ch->SetBranchAddress("tk_qlsn",     &tk_qlsn);
    }
    ch->SetBranchAddress("tk_lid",       tk_lid);
    ch->SetBranchAddress("tk_cad",       tk_cad);
    ch->SetBranchAddress("tk_cof",       tk_cof);
    if(qversion/10%10>=1){//Tracker ADC study
      ch->SetBranchAddress("tk_adc",       tk_adc);
//      ch->SetBranchAddress("tk_xco",       tk_xco);
    }
    ch->SetBranchAddress("tk_exql",      tk_exql);
    if((amsdn%100)>=59)ch->SetBranchAddress("tk_exqln",     tk_exqlt);
    else               ch->SetBranchAddress("tk_exqln",     tk_exqln);
    ch->SetBranchAddress("tk_exqls",     tk_exqls);
    ch->SetBranchAddress("tk_exdis",     tk_exdis);
    ch->SetBranchAddress("tk_exlid",     tk_exlid);
    ch->SetBranchAddress("tk_excad",     tk_excad);
    ch->SetBranchAddress("tk_rigidity1", tk_rigidity1);
    ch->SetBranchAddress("tk_rigidityk", tk_rigidityk);
    ch->SetBranchAddress("tk_erigidity1",tk_erigidity1);
    ch->SetBranchAddress("tk_chis1",     tk_chis1);
//    if((amsdn%100)>=65)ch->SetBranchAddress("tk_chisl19",     tk_chisl19);
//    ch->SetBranchAddress("tk_pval",      tk_pval);
    ch->SetBranchAddress("tk_rigidityi", tk_rigidityi);
    ch->SetBranchAddress("tk_chisi",     tk_chisi);
    ch->SetBranchAddress("tk_res",       tk_res);
    ch->SetBranchAddress("tk_res1",      tk_res1);
    if(qversion/10%10>=2){//Tracker ADC study
      ch->SetBranchAddress("tk_rigidityia", tk_rigidityia);
      ch->SetBranchAddress("tk_chisia",     tk_chisia);
      ch->SetBranchAddress("tk_res1a",      tk_res1a);
    }
    ch->SetBranchAddress("tk_cdif",      tk_cdif);
    ch->SetBranchAddress("tk_pos",       tk_pos); 
    ch->SetBranchAddress("tk_dir",       tk_dir);
    ch->SetBranchAddress("tk_hitc",      tk_hitc);
//    if(qversion/10%10>=1)ch->SetBranchAddress("tk_hitcl",      tk_hitcl);
    ch->SetBranchAddress("tk_oq",        tk_oq);
    ch->SetBranchAddress("tk_oel",       tk_oel);
    ch->SetBranchAddress("tk_ohitl",     tk_ohitl);
    if(qversion/100%10>=1){
      ch->SetBranchAddress("tk_rigidityr",      tk_rigidityr);
      ch->SetBranchAddress("tk_posr",           tk_posr);
      ch->SetBranchAddress("tk_dirr",           tk_dirr);
    }
    return 0;
}

int QEvent::AddBranch_Tof(TChain *ch){
    ch->SetBranchAddress("tof_btype",    &tof_btype);
    ch->SetBranchAddress("tof_bpatt",    &tof_bpatt);
    ch->SetBranchAddress("tof_beta",     &tof_beta);
    ch->SetBranchAddress("tof_betah",    &tof_betah);
    if((amsdn%100)>=58){
      ch->SetBranchAddress("tof_betahmc",    &tof_betahmc);
    }
    ch->SetBranchAddress("tof_hsumh",    &tof_hsumh);
    ch->SetBranchAddress("tof_hsumhu",   &tof_hsumhu);
    ch->SetBranchAddress("tof_nclhl",     tof_nclhl);
    ch->SetBranchAddress("tof_barid",     tof_barid);
    ch->SetBranchAddress("tof_pos",       tof_pos);
    ch->SetBranchAddress("tof_chisc",    &tof_chisc);
    ch->SetBranchAddress("tof_chist",    &tof_chist);
    ch->SetBranchAddress("tof_chisc_n",  &tof_chisc_n);
    ch->SetBranchAddress("tof_chist_n",  &tof_chist_n);
    ch->SetBranchAddress("tof_q",         tof_q);
    ch->SetBranchAddress("tof_qrms",      tof_qrms);
    ch->SetBranchAddress("tof_z",        &tof_z);
    ch->SetBranchAddress("tof_pz",       &tof_pz);
    ch->SetBranchAddress("tof_ql",        tof_ql);
    ch->SetBranchAddress("tof_qlr",       tof_qlr);
    if(qversion/10%10>=1)ch->SetBranchAddress("tof_qlc",       tof_qlc);//TOF ADC study
    ch->SetBranchAddress("tof_tl",        tof_tl);
    ch->SetBranchAddress("tof_qs",       &tof_qs);
    ch->SetBranchAddress("tof_oq",        tof_oq);
    ch->SetBranchAddress("tof_ob",        tof_ob);
    ch->SetBranchAddress("tof_oncl",     &tof_oncl);
    return 0;
}

int QEvent::AddBranch_Trd(TChain *ch){
    ch->SetBranchAddress("nitrdseg",    &nitrdseg);
    ch->SetBranchAddress("trd_rq",      &trd_rq);
    ch->SetBranchAddress("trd_qk",       trd_qk);
    ch->SetBranchAddress("trd_qnhk",     trd_qnhk);
    ch->SetBranchAddress("trd_qrmsk",    trd_qrmsk);
    ch->SetBranchAddress("trd_statk",   &trd_statk);
    ch->SetBranchAddress("trd_nhitk",   &trd_nhitk);
    ch->SetBranchAddress("trd_onhitk",  &trd_onhitk);
    ch->SetBranchAddress("trd_lik",      trd_lik);
    ch->SetBranchAddress("trd_like",     trd_like);
    return 0;
}

int QEvent::AddBranch_Rich(TChain *ch){
    ch->SetBranchAddress("rich_itrtrack",&rich_itrtrack);
    ch->SetBranchAddress("rich_beta",    rich_beta);
    ch->SetBranchAddress("rich_pb",     &rich_pb);
    ch->SetBranchAddress("rich_udis",   &rich_udis);
    ch->SetBranchAddress("rich_used",   &rich_used);
    ch->SetBranchAddress("rich_stat",   &rich_stat);
    ch->SetBranchAddress("rich_q",       rich_q);
    ch->SetBranchAddress("rich_width",  &rich_width);
    ch->SetBranchAddress("rich_npe",     rich_npe);
    ch->SetBranchAddress("rich_good",   &rich_good);
    ch->SetBranchAddress("rich_clean",  &rich_clean);
    ch->SetBranchAddress("rich_NaF",    &rich_NaF);
    ch->SetBranchAddress("rich_PMTChargeConsistency",&rich_PMTChargeConsistency);
    ch->SetBranchAddress("rich_BetaConsistency",&rich_BetaConsistency);
    ch->SetBranchAddress("rich_pos",     rich_pos);
    ch->SetBranchAddress("rich_tile",   &rich_tile);
    ch->SetBranchAddress("rich_distb",  &rich_distb);
    return 0;
}

int QEvent::AddBranch_Ecal(TChain *ch){
    ch->SetBranchAddress("ecal_stat",  &ecal_stat);
    ch->SetBranchAddress("ecal_en",     ecal_en);
    ch->SetBranchAddress("ecal_enc",    ecal_enc);
    ch->SetBranchAddress("ecal_bdt",    ecal_bdt);
    ch->SetBranchAddress("show_dis",   &show_dis);
    ch->SetBranchAddress("ecal_q",     &ecal_q);
    ch->SetBranchAddress("ecal_nhit",   ecal_nhit);
    ch->SetBranchAddress("ecal_el",     ecal_el);
    ch->SetBranchAddress("ecal_eh",     ecal_eh);
    return 0;
}

int QEvent::AddBranch_Anti(TChain *ch){
    ch->SetBranchAddress("anti_nhit",  &anti_nhit);
    return 0;
}

int QEvent::AddBranch_TrkParticle(TChain *ch){
   if(qversion%10>=3){
    ch->SetBranchAddress("itrtracks",   &itrtracks);
    ch->SetBranchAddress("tk_hitbs",     tk_hitbs);
    ch->SetBranchAddress("tk_qis",       tk_qis);//All+Inner
    ch->SetBranchAddress("tk_qirmss",    tk_qirmss);
    ch->SetBranchAddress("tk_qlss",      tk_qlss);
    ch->SetBranchAddress("tk_rigiditys", tk_rigiditys);
    ch->SetBranchAddress("tk_chiss",     tk_chiss);
    ch->SetBranchAddress("tk_pos2s",     tk_pos2s);
    ch->SetBranchAddress("tk_dir2s",     tk_dir2s);
    ch->SetBranchAddress("tk_ibetahs",  &tk_ibetahs);
   }
    return 0;
}

void QEvent::SetBranch(TChain *ch,int opt){
   if(opt==0){//Header
      AddBranch_Header(ch);
   }
   else {//Other
     AddBranch_Trigger(ch);
     AddBranch_RTI(ch);
     if(!isreal)AddBranch_MC(ch);
     AddBranch_Particle(ch);
     AddBranch_Trk(ch);
     AddBranch_Tof(ch);
     AddBranch_Trd(ch);
     AddBranch_Rich(ch);
     AddBranch_Ecal(ch);
     AddBranch_Anti(ch);
     AddBranch_TrkParticle(ch);
   }
}

int QEvent::Getamsdn(){
  return amsdn;
}

unsigned int QEvent::GetJINJLength(int ijinj){
  unsigned int jinjb[2]={0xff4d00,0xb2ff};//2-JINJ bit
  unsigned int daqjlens[2]={0,0};
  for(int ib=0;ib<24;ib++){
    if       (run<1456503197)daqjlens[0]+=daqjlen[ib];
    else if(jinjb[0]&(1<<ib))daqjlens[0]+=daqjlen[ib];//1-JINJ
    else if(jinjb[1]&(1<<ib))daqjlens[1]+=daqjlen[ib];//2-JINJ
  }
  if(ijinj>=2){return (daqjlens[0]>daqjlens[1])?daqjlens[0]:daqjlens[1];}
  else         return daqjlens[ijinj];
}

int QEvent::HasTkLHitXY(int ilay){
   bool lhxy[2]={0};
   for(int ixy=0;ixy<2;ixy++){if((tk_hitb[ixy]&(1<<ilay))>0)lhxy[ixy]=1;}
   if     (lhxy[0]&&lhxy[1])return 2;//X+Y
   else if(lhxy[1])         return 1;//Y
   else                     return 0;//No
}

int QEvent::GetTkInNHit(int ixy,int ispan){
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

int QEvent::InterpolateMC(double zpl,double pnt[3]){
   pnt[0]=pnt[1]=0;pnt[2]=zpl;
   if(isreal)return -1;
   for(int ixy=0;ixy<2;ixy++)pnt[ixy]=(zpl-mevcoo[2])*mevdir[ixy]/mevdir[2]+mevcoo[ixy];
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

bool QEvent::IsPassECAL(){
   bool ecalgeom=(fabs(ec_pos[0])<32&&fabs(ec_pos[1])<32);
   return ecalgeom;
}

bool QEvent::IsPassTRD(){
   bool trdgeom[2]={0};
   double pnt[3];
   for(int iud=0;iud<2;iud++){
     Interpolate(trd_pz[iud],pnt);
     float disr=pnt[0]*pnt[0]+pnt[1]*pnt[1];
//     cout<<"trdxy="<<pnt[0]<<" "<<pnt[1]<<endl;
     trdgeom[iud]=(sqrt(disr)<trd_cirr[iud]);
   }
   return (trdgeom[0]&&trdgeom[1]);
}

bool QEvent::IsPassRich(){
  float coox=rich_pos[0];
  float cooy=rich_pos[1];
  float disr=coox*coox+cooy*cooy;
  bool richgeom=(sqrt(disr)<59.161);//Algo Border
  return richgeom; 
}

float QEvent::GetSABeta(){
  float betahsu=betahs;
  if(!isreal&&((amsdn%100)>=58&&(amsdn%100)<=68))betahsu=betahsmc;
  return betahsu;
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

float QEvent::GetSATOFLQ(int ilay){
   float toflqs=tof_qls[ilay];
   double qq[2]={26,28};
   if(isreal&&toflqs>qq[0]){//special fixed for Z=28
     double hc=(ilay<2)?1.012:1.008;
     double lc=1;
     if(toflqs>=qq[1]){return toflqs*sqrt(hc);}
     else {
       double wwh=toflqs*toflqs-qq[0]*qq[0];
       double wwl=qq[1]*qq[1]-toflqs*toflqs;
       double nc=(fabs(wwh)*hc+fabs(wwl)*lc)/(fabs(wwh)+fabs(wwl));
       return toflqs*sqrt(nc);
     }
   }
   else {
     return toflqs;
   }
}

float QEvent::GetTOFLQ(int ilay){
   float toflq=tof_ql[ilay];
   double qq[2]={26,28};
   if(isreal&&toflq>qq[0]){//special fixed for Z=28
     double hc=(ilay<2)?1.012:1.008;
     double lc=1;
     if(toflq>=qq[1]){return toflq*sqrt(hc);}
     else {
       double wwh=toflq*toflq-qq[0]*qq[0];
       double wwl=qq[1]*qq[1]-toflq*toflq;
       double nc=(fabs(wwh)*hc+fabs(wwl)*lc)/(fabs(wwh)+fabs(wwl));
       return toflq*sqrt(nc);
     }
   }
   else {
     return toflq;
  }
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

bool QEvent::IsGoodRichTile(int utile,int opt){
   static const int nbadtile0=7;
   static int badtile0[nbadtile0]={3,7,12,20,87,100,108};
   static const int nbadtile=14;
   static int badtile[nbadtile]={3,7,12,20,38,39,51,62,73,82,83,87,100,108};
   int nbadtileu=(opt==0)?nbadtile0:nbadtile;
   int *badtileu=(opt==0)?badtile0:badtile;
   bool goodtile=1;
   for(int i=0;i<nbadtileu;i++){
     if     (i==0&&utile<badtileu[i]){goodtile=1;break;}
     else if(utile==badtileu[i]){goodtile=0;break;}
     else if(utile>badtileu[i]&&(i==nbadtileu-1||utile<badtileu[i+1])){goodtile=1;break;}
   }
   return goodtile;
}

bool QEvent::IsGoodRichTile(int opt){
   return IsGoodRichTile(rich_tile,opt);
}


bool QEvent::IsGoodRichGeom(){
   bool goodgeom=1;
   float coox=rich_pos[0];
   float cooy=rich_pos[1];
   if(rich_NaF){
     goodgeom=(rich_distb>0.4)?1:0;
     bool nafborder=(fabs(coox)>15&&fabs(cooy)>15);//4edge
     goodgeom=(goodgeom&&!nafborder);
   }
   else {
     goodgeom=(fabs(coox)>19||fabs(cooy)>19);
   }
   goodgeom=(goodgeom&&IsPassRich());
   return goodgeom;
}


float QEvent::GetSABZQUnbias(int ilay,int calib){
   int iilay=-1;
   if     (ilay==0)iilay=0;
   else if(ilay==8)iilay=1;
   if(iilay<0)return -1;
   float saqub=(iilay==0)?tk_l1qvs:tk_l9qvs;
   if(calib>=1)saqub=tk_exqvn[calib-1][iilay];
   return saqub;
}

float QEvent::GetTkBZQUnbias(int ilay,int ixy,int calib){
   int iilay=-1;
   if     (ilay==0)iilay=0;
   else if(ilay==8)iilay=1;
   if(iilay<0)return -1;
   float tkqub=tk_exql[iilay][ixy];
   if     (calib>=1&&(amsdn%100)>=59)tkqub=tk_exqlt[calib-1][iilay][ixy];
   else if(calib>=1&&ixy==2)tkqub=tk_exqln[calib-1][iilay];
   return tkqub;
}

float QEvent::GetTkLQ(int ilay,int ixy,int calib){
   if(calib>=1)return tk_qln[calib-1][ilay][ixy];
   if(ixy==0||ixy==1){
      return  tk_ql2[ilay][ixy];
   }
   return tk_ql[ilay];
}

int QEvent::GetTkLZ(int ilay,int ixy,int calib){
   return int(GetTkLQ(ilay,ixy,calib)+0.5);
}

float QEvent::GetTkInQ(int ixy,float &qrms,int opt,int calib){
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
   if(calib>=1){qm=tk_qin[calib-1][ixy];qrms=tk_qrmn[calib-1][ixy];}
   return qm;
}

float QEvent::GetTkInQ(int ixy,int calib){
   float qm,qrms;
   qm=GetTkInQ(ixy,qrms,0,calib);
   return qm;
}

float QEvent::GetTkInQRMS(int ixy,int calib){
   float qm,qrms;
   qm=GetTkInQ(ixy,qrms,0,calib);
   return qrms;
}

int QEvent::GetTkInZ(int ixy,int calib){
   float tkqin=GetTkInQ(ixy,calib); 
   if(calib==0&&ixy==2&&tkqin>9)tkqin=(tkqin-9.)/1.03+9.;//special biase correction for pass6
   return int(tkqin+0.5);
}

float QEvent::GetRichQ(){
   float qm=0;
   if(irich>=0)qm=sqrt(rich_q[0]);
   return qm;
}

int QEvent::GetSpanArrayI(int ispan){
   int indexs=-1;
   if     (ispan==0)indexs=1;//Inner
   else if(ispan==1)indexs=2;//Inner+L1
   else if(ispan==2)indexs=4;//Inner+L1+L9
   else if(ispan==3)indexs=3;//Inner+L9
   else if(ispan==4)indexs=5;//InnerUp   ||(L2-L6)
   else if(ispan==5)indexs=6;//InnerDown ||(L3-L8)
   else if(ispan==-1)indexs=0;//Max-Span
   return indexs;
}

float QEvent::GetRigidity(int ispan,int ialgo,int ival,int iz){
   int indexs=GetSpanArrayI(ispan);
   if(ialgo==2&&iz>=0){//Kalman/iz>=0
     return tk_rigidity1[ialgo][ival][indexs]+(tk_rigidityk[iz][indexs]-tk_rigidity1[ialgo][2][indexs]);//Filter to this position
   }
   else {return tk_rigidity1[ialgo][ival][indexs];}
}

float QEvent::GetERigidity(int ispan,int ialgo,int opt){
   int indexs=GetSpanArrayI(ispan);
   float erigidity=tk_erigidity1[ialgo][indexs];
   if(opt==1)erigidity*=GetRigidity(ispan,ialgo);//PG+CIEMAT
   return erigidity;
}

float QEvent::GetChis(int ispan,int ixy,int ialgo,int ival){
   int indexs=GetSpanArrayI(ispan);
   return tk_chis1[ialgo][ival][indexs][ixy];
}

/*float QEvent::GetPVal(int ispan,int ixy,int ialgo){
   int indexs=GetSpanArrayI(ispan);
   return tk_pval[ialgo][indexs][ixy];
}*/

/*float QEvent::GetChisExt(int ispan,int ixy,int ialgo,int ival){
  if(ispan>=1&&ispan<=3){
    int nmex[2][2]={{0}};
    for(int iex=0;iex<2;iex++){
      int lhex=HasTkLHitXY(iex==0?0:8);
      if(iex==0){
        if(ispan==1||ispan==2){if(lhex==0)return 99999; } 
      }
      else {
       
      }
      if(lhex==2)nmex[iex][0]++;//X
      if(lhex>=1)nmex[iex][1]++;//Y
    }
    int nmexa=0;
    if(ispan==1||ispan==2){if(nmex[0][1]==0)return 99999;nmexa+=(ixy==2)?nmex[0][0]+nmex[0][1]:nmex[0][ixy];}
    if(ispan==2||ispan==3){if(nmex[1][1]==0)return 99999;nmexa+=(ixy==2)?nmex[1][0]+nmex[1][1]:nmex[1][ixy];} 
    int l1h=HasTkLHitXY(0);
    int l9h=HasTkLHitXY(8);
    int nmex=0;
    if(ispan==1||ispan==2){if(l1h==0)return 99999; if()nmex++; if(l1h==2&&ixy==2)nmex++;}
    if(ispan==2||ispan==3){if(l9h==0)return 99999; nmex++; if(l9h==2&&ixy==2)nmex++;}
    int nmin=(ixy==2)?GetTkInNHit(0)+GetTkInNHit(1):GetTkInNHit(ixy);
    int npar[3]={2,3,5};//ixy
    int nmex=(ispan==2)?2:1)
    float chisin=GetChis(0,ixy,ialgo,ival)*(nmin[ixy]-npar[ixy]);
    float chisal=GetChis(1,ixy,ialgo,ival)*(inhity+1-3);
  }
  else return 0; 
}*/

float QEvent::GetL1ChisY(int ialgo,int ival){
  bool l1hy=(HasTkLHitXY(0)>=1);
  if(!l1hy)return 99999;
  int inhity=GetTkInNHit(1);
  float chisyin=GetChis(0,1,ialgo,ival)*(inhity-3);
  float chisyl1=GetChis(1,1,ialgo,ival)*(inhity+1-3);//L1I-I
  chisyl1=(chisyl1-chisyin);
  return chisyl1;
}

float QEvent::GetL9ChisY(int ialgo,int ival){
  bool l9hy=(HasTkLHitXY(8)>=1);
  if(!l9hy)return 99999;
  int inhity=GetTkInNHit(1);
  float chisyin=GetChis(0,1,ialgo,ival)*(inhity-3);
  float chisyl9=GetChis(3,1,ialgo,ival)*(inhity+1-3);//IL9-I
  chisyl9=(chisyl9-chisyin);
  return chisyl9;
}

float QEvent::GetL19ChisY(int ialgo,int ival){
  bool l1hy=(HasTkLHitXY(0)>=1);
  bool l9hy=(HasTkLHitXY(8)>=1);
  if(!l1hy||!l9hy)return 99999;
  int inhity=GetTkInNHit(1);
  float chisyin=GetChis(0,1,ialgo,ival)*(inhity-3); 
  float chisyl19=GetChis(2,1,ialgo,ival)*(inhity+2-3);//L19-I
  return (chisyl19-chisyin)/2.;//Chis/Ndof
}

float QEvent::GetTkLRes(int ilay,int ixy,int nobias){
  if(nobias)return tk_res1[ilay][ixy]*10000.;
  else      return tk_res[ilay][ixy]*10000.;
}

float QEvent::GetDiffPGMD(int ilay,int ixy){
  if     (ilay==0)return tk_cdif[0][ixy]*10000.;
  else if(ilay==8)return tk_cdif[1][ixy]*10000.; 
  else            return 0; 
}

float QEvent::GetEcalEnergy(int ie){
    return ecal_en[ie];
}

float QEvent::GetRichBeta(){
    return rich_beta[0];
}

float QEvent::GetMass(float urig,int uz,float ubeta){
   if(ubeta>1||ubeta==0)return -1;
   float umom=fabs(urig*uz);
   return umom*sqrt(1./ubeta/ubeta-1);
}

float QEvent::GetMass2(float urig,int uz,float ubeta){
   float umom=fabs(urig*uz);
   return umom*umom*(1./ubeta/ubeta-1);
}

float QEvent::GetEMass(float urig,float evrig,int uz,float ubeta,float ebeta){
   if(ubeta>1||ubeta==0)return -1;
   float emom=fabs(evrig*urig*urig*uz); 
   float umom=fabs(urig*uz);
   float gamma2=1./(1-ubeta*ubeta);
   float umass=GetMass(urig,uz,ubeta);
   float emass=umass*sqrt(emom/umom*emom/umom+ebeta/ubeta*ebeta/ubeta*gamma2*gamma2);
   return emass;
}

float QEvent::GetEMass2(float urig,float evrig,int uz,float ubeta,float ebeta){
   float emom=fabs(evrig*urig*urig*uz);
   float umom=fabs(urig*uz);
   float gamma2=1./(1-ubeta*ubeta);
   float umass2=GetMass2(urig,uz,ubeta);
   float emass2=2*umass2*sqrt(emom/umom*emom/umom+ebeta/ubeta*ebeta/ubeta*gamma2*gamma2);
   return emass2;  
}

float QEvent::GetRigidityBeta(float urig,int uz,float umass){
   float umom=fabs(urig*uz);
   return umom/sqrt(umom*umom+umass*umass);
}

int  QEvent::GetTrMCClusterZA(int mcpar,int &mcz,int &mca){
   mcz=mca=0;
   if(mcpar>=10000) {
     mcz=mcpar/100%100; 
     mca=mcpar%100;
   }
   if     (mcpar==14) {mcz=1;mca=1; }//P
   else if(mcpar==45) {mcz=1;mca=2; }//D
   else if(mcpar==46) {mcz=1;mca=3; }//T
   else if(mcpar==47) {mcz=2;mca=4; }//He4
   else if(mcpar==49) {mcz=2;mca=3; }//He3
   else if(mcpar==61) {mcz=3;mca=6; }//Li6
   else if(mcpar==62) {mcz=3;mca=7; }//Li7
   else if(mcpar==63) {mcz=4;mca=7; }//Be7
   else if(mcpar==64) {mcz=4;mca=9; }//Be9
   else if(mcpar==114){mcz=4;mca=10;}//Be10
   else if(mcpar==65) {mcz=5;mca=10;}//B10
   else if(mcpar==66) {mcz=5;mca=11;}//B11
   else if(mcpar==67) {mcz=6;mca=12;}//C12
   else if(mcpar==68) {mcz=7;mca=14;}//N14
   else if(mcpar==118){mcz=7;mca=15;}//N15
   else if(mcpar==69) {mcz=8;mca=16;}//O16
   else if(mcpar==70) {mcz=9;mca=19;}//F19
   else if(mcpar==71) {mcz=10;mca=20;}//Ne20
   else if(mcpar==72) {mcz=11;mca=23;}//Na23
   else if(mcpar==73) {mcz=12;mca=24;}//Mg24
   else if(mcpar==74) {mcz=13;mca=27;}//Al27
   else if(mcpar==75) {mcz=14;mca=28;}//Si28
   else if(mcpar==76) {mcz=15;mca=31;}//P31
   else if(mcpar==77) {mcz=16;mca=32;}//S32
   else if(mcpar==78) {mcz=17;mca=35;}//Cl35
   else if(mcpar==79) {mcz=18;mca=36;}//Ar36
   else if(mcpar==80) {mcz=19;mca=39;}//K39
   else if(mcpar==81) {mcz=20;mca=40;}//Ca40
   else if(mcpar==82) {mcz=21;mca=45;}//Sc45
   else if(mcpar==83) {mcz=22;mca=48;}//Ti48
   else if(mcpar==84) {mcz=23;mca=51;}//V51
   else if(mcpar==85) {mcz=24;mca=52;}//Cr52
   else if(mcpar==86) {mcz=25;mca=55;}//Mn55
   else if(mcpar==87) {mcz=26;mca=56;}//Fe56
   return 0;
}


bool QEvent::IsGoodRTI(int opt){
    bool isphorun=(run>=1620025528&&run<1635856717);
    bool cut[10]={0};
    cut[0]=(rtintrig/rtinev>0.98);
    cut[1]=(rtinpar/rtintrig>(isphorun?0.02:0.07/1600*rtintrig)&&rtinpar/rtintrig<0.25);
    cut[2]=(rtilf>(isphorun?0.35:0.5));
    cut[3]=(rtinerr>=0&&rtinerr/rtinev<0.1);
    cut[4]=(rtinpar>0&&rtinev<1800);
    cut[5]=(zenith<40);
    if(opt==0)cut[5]=1;
    cut[6]=((rtigood&0x3F)==0);
    cut[7]=(issaa==0);
    cut[8]=(irti>=0);
    cut[9]=1;
    if((amsdn%100)>=68&&opt!=1)cut[9]=(rtinexl[0][0]>700&&rtinexl[1][0]>500);
    bool tcut=(cut[0]&&cut[1]&&cut[2]&&cut[3]&&cut[4]&&cut[5]&&cut[6]&&cut[7]&&cut[8]&&cut[9]);
    return tcut;
}

bool QEvent::Select_Trigger(int nanti){
    int physbpattp=(isreal)?physbpatt2:physbpatt1;//Trigger ISS/Trigger MC
    bool trigp=((physbpattp&0x3EL)!=0);
//-------
    if(nanti==7){
      int nacctrig=0;
      for(int i=0;i<8;i++)if((antipatt&(1<<i))>0)nacctrig+=1;//count nsectors
      bool tofbztrig=(tofflag[1]==0);
      bool bz7trig=(tofbztrig&&nacctrig<=7);
      if(isreal)trigp=(trigp&&(run>=1456503197));
      else      trigp=(bz7trig||(physbpattp&0x3AL)!=0);
    }
    return trigp;
} 

bool QEvent::Select_RTI(int opt){
    bool rticut=1;
    if(isreal)rticut=IsGoodRTI(opt);
    return rticut;
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

bool QEvent::Select_TkInQ(int opt,int zref,int calib){
   float tkqinrms=0;
   float tkqin=GetTkInQ(2,tkqinrms,0,calib);//X+Y
   if(tkqin>9&&calib==0)tkqin=(tkqin-9.)/1.03+9.;//special biase correction for pass6
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
     if(calib>=1)sigma=0.0025*pow(double(tk_z1),1.414)+0.066;
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

bool QEvent::Select_TkExtQ(int ispan,int opt,int zref,int calib){
//---L1L9 Pattern
   bool l1h=(HasTkLHitXY(0)==2);
   bool l9h=(HasTkLHitXY(8)==2);
   bool l1hs=IsGoodTkLQStat(0,0);
   bool l9hs=IsGoodTkLQStat(8,0);
//---L1L9 Charge
   int tk_z1=(zref>=1)? zref: GetTkInZ(2,calib);
   float tkql1x=GetTkLQ(0,0,calib);
   float tkql1y=GetTkLQ(0,1,calib);
   float tkql1 =GetTkLQ(0,2,calib);
   float tkql9x=GetTkLQ(8,0,calib);
   float tkql9y=GetTkLQ(8,1,calib);
   float tkql9 =GetTkLQ(8,2,calib);
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
     if(calib>=1)sigma=0.0167*pow(double(tk_z1),1.15)+0.1;
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

bool QEvent::Select_RichQ(){
   float richq=GetRichQ();
   int tk_z1=GetTkInZ();
   bool cutrichq=(richq>tk_z1-0.8&&richq<tk_z1+1.);//Z=2
   return cutrichq;
}

bool QEvent::Select_TkL1ChisY(int ialgo,int ival){
   bool l1chiscut=(GetL1ChisY(ialgo,ival)<10);//Chiscut 
   return l1chiscut;
}

bool QEvent::Select_TkL19ChisY(int ialgo,int ival){
   bool l19chiscut=(GetL19ChisY(ialgo,ival)<10);//Chiscut 
   return l19chiscut;
}

bool QEvent::Select_TkRig(int ispan,int ialgo,int ival,float ychis,float ychisex){
//---L1L9 Pattern
   bool l1h=(HasTkLHitXY(0)==2);
   bool l9h=(HasTkLHitXY(8)==2);
//---Inner+L1L9 ChisY
   bool cutchist[7]={0};
   if(ychis<0)ychis=10;//default
   cutchist[0]=(GetChis(0,1,ialgo,ival)<ychis);//Inner
   cutchist[1]=(GetChis(1,1,ialgo,ival)<ychis);//L1+Inner
   cutchist[2]=(GetChis(2,1,ialgo,ival)<ychis);//L1+Inner+L9
   cutchist[3]=(GetChis(3,1,ialgo,ival)<ychis);//Inner+L9
   cutchist[4]=cutchist[5]=1;
   cutchist[6]=(GetChis(6,1,ialgo,ival)<ychis);//InInner
//---Span Cut
   bool scutrig[7]={0};
   scutrig[0]=(cutchist[0]);//ChisY
   scutrig[1]=(l1h&&cutchist[0]&&cutchist[1]);//Pattern+Charge+ChisY
   scutrig[2]=(l1h&&l9h&&cutchist[0]&&cutchist[2]);
   scutrig[3]=(l9h&&cutchist[0]&&cutchist[3]);
   scutrig[4]=(cutchist[4]);
   scutrig[5]=(cutchist[5]);
   scutrig[6]=(cutchist[6]);
//---External ChisY cut
   bool cutchisex=1;
   if(ychisex<0)ychisex=10;//default
   if(ychisex>0){
     if     (ispan==1)cutchisex=(GetL1ChisY(ialgo,ival)<ychisex);//L1+Inner
     else if(ispan==2)cutchisex=(GetL19ChisY(ialgo,ival)<ychisex);//L1+Inner+L9
     else if(ispan==3)cutchisex=(GetL9ChisY(ialgo,ival)<ychisex);//Inner+L9
   }

   return (scutrig[ispan]&&cutchisex);
}

bool QEvent::Select_Tk2nd(int opt){
   int nhit2i[2]={0};
   for(int ilay=0+1;ilay<9-1;ilay++){
     for(int ixy=0;ixy<2;ixy++){
      if((betah2hb[ixy]&(1<<ilay))>0)nhit2i[ixy]++;
     }
   }
   bool tkcut2=(nhit2i[0]>=3&&nhit2i[1]>=5);//secondary Tk
   if     (opt==1)tkcut2=(nhit2i[1]>=5);
   else if(opt==2)tkcut2=1;
   bool cuttk2nd=!(tkcut2&&betah2r/tof_betah>0&&fabs(betah2r)>0.5);
   return cuttk2nd;
}


bool QEvent::Select_Rich(int opt){
   bool richcut=(irich>=0&&rich_itrtrack==itrtrack);
   if(opt==0)return richcut;
   bool richgood=(rich_good&&rich_pb>0.01&&rich_used>=5&&rich_npe[0]/rich_npe[2]>0.3&&rich_width<3);
   int tk_z1=GetTkInZ();
//---Charge && Expect Phe
   if(tk_z1==1){
      richgood=(rich_good&&rich_pb>0.01&&rich_used>=3&&rich_npe[0]/rich_npe[2]>0.3&&rich_width<3);
   }
   if(tk_z1==2){
     bool cutchpmt=(rich_PMTChargeConsistency<10);
     bool cutphexp=rich_NaF?(rich_npe[1]>1):(rich_npe[1]>2);
     richgood=(richgood&&cutchpmt&&cutphexp);
   }
//---
   richcut=(richcut&&richgood&&IsPassRich());
   return richcut;
}

bool QEvent::Select_TrdK(){
   bool trdkcut=(trd_statk==1);
   return trdkcut;
}

bool QEvent::Select_Ecal(int opt){
   bool ecalcut=(nshow==1&&ishow>=0&&ecal_bdt[0]!=-3&&IsPassECAL()&&show_dis<10);
   bool catleakcut=((ecal_stat&(16384*2*2*2*2*2*2*2*2*2*2*2))==0);//No-CatLeak
   if(opt>=1)ecalcut=(ecalcut&&catleakcut);
   return ecalcut;
}
#endif
