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
#include "AbsKalmanFitter.h"
#include "KalmanFitStatus.h"
#include "KalmanFittedStateOnPlane.h"
#include "KalmanFitterInfo.h"
//#include "/afs/cern.ch/user/q/qyan/ihepsub/readfile1.C"
#include "/afs/cern.ch/user/q/qyan/work/tofanalysis/vdev/testn/readfile3.C"
//#define _USEFAST_
#ifdef _HEINNERPRESCALE_
#define _HEL1PRESCALE_
#endif
#ifdef _IONINNERPRESCALE_
#define _IONL1PRESCALE_
#endif
#if defined (_USENOLINEARCOR_) || defined (_USECALIB_)
#define _USETKADC_ 
#define _GOODTKTOFPAR_
#define _PRHESAVEUNBIASE_
#endif

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
#ifdef _USENEWL1L9G_
   const double cirr[]={62+2,62+2,46+2,46+2,46+2,46+2,46+2,46+2,43+2};//New L1L9 Geometry(+-2cm)
   const double ciry[]={47+2,40+2,44+2,44+2,36+2,36+2,44+2,44+2,29+2};
#else
   const double cirr[]={62,62,46,46,46,46,46,46,43};
   const double ciry[]={47,40,44,44,36,36,44,44,29};
#endif
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


class AMSAnalysis{
  public:
   AMSAnalysis():fout(0),pev(0){tout[0]=tout[1]=tout[2]=0;};
   ~AMSAnalysis(){};
   void BookFile(char *ofile);
   int  LoadHeader();
   int  LoadHeaderEv();
   int  LoadHeaderMC(int mcz=0,int vers=0,int mparid=0);
   int  InitTrkReFit(float tkiq,float tkfz,int &refit,float &mass,int &chrg,int algo=1);
//---
   int  GetTreeSize(TTree *tree);
   int  AddBranch_Header (TTree *tree);
   int  AddBranch_Trigger(TTree *tree);
   int  AddBranch_RTI(TTree *tree);
   int  AddBranch_MC (TTree *tree);
   int  AddBranch_Particle(TTree *tree);
   int  AddBranch_Trk (TTree *tree);
   int  AddBranch_Tof (TTree *tree);
   int  AddBranch_Trd (TTree *tree);
   int  AddBranch_Rich(TTree *tree);
   int  AddBranch_Ecal(TTree *tree);
   int  AddBranch_Anti(TTree *tree);
   int  AddBranch_TrkParticle(TTree *tree);
//---
   int  Save();
   void InitEvent(AMSEventR *ev);   
   int  Select();//1 Write-Pre 2 Write-All
   bool Select_Header();
   bool Select_Trigger();
   bool Select_MC(int opt=0);
   bool Select_RTI();
   bool Select_Particle();
//----*
   bool Select_L1L9Particle();
   bool Select_MCL1L9Particle();
   bool Select_2ndParticle();
   bool Select_LinkParticle();
//----
   bool Select_Trk(int sel=0);//pre selction 1 or 0
   bool Select_Tof();
   bool Select_Trd();
   bool Select_Rich();
   bool Select_Ecal();  
   bool Select_Anti();
   bool Select_TrkParticle();
   int  Fill(int it);
//--
   TFile *fout;
   TTree *tout[3];

   AMSEventR *pev;
   int iparindex;
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
   int          ndaqev;
   int          ndaqerr;
   unsigned int daqlen;
   int          daqtyerr;
   unsigned int daqjrmerr;
   unsigned int daqjlen[24];
   unsigned int daqsdlen;
   unsigned int daqsdlenu;
   float        livetime;
   int          physbpatt;
   int          jmembpatt;
   int          physbpatt1;
   int          jmembpatt1;
   int          physbpatt2;
   int          jmembpatt2;
   int          ecalflag;
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
   float        mcutoff[4][2];
   float        mcutoffi[4][2];
   float        rtilf;
   float        rtinev;
   float        rtinerr;
   float        rtintrig;
   float        rtinpar;
   int          rtigood;
   float        rticdif[2][3]; 

//--MC
   int          mpar;
   float        mmom;
   float        mch;
   int          mtof_pass;
   float        mevcoo[3];
   float        mevdir[3];
   float        mfcoo[3];
   float        mfdir[3];
   float        mpare[2];
   int          mparp[2];
   float        mparc[2];
   float        msume;
   float        msumc;
   float        mtrdmom[20];
   int          mtrdpar[20];
   float        mtrdcoo[20][3];
   int          ntrmccl;
   float        mtrmom[9];
   int          mtrpar[9];
   float        mtrcoo[9][3];
   float        mtrcoo1[9][3];
   int          mtrpri[9];
   int          mtrz[9];
   int          ntofmccl;
   float        mtofdep[4];
   int          mtofpri[4];
   int          mtofbar[4];
//---
   float        mevcoo1[21][3];
   float        mevdir1[21][3];
   float        mevmom1[21];
//---
   float        tof_qlsb[4];
   float        tof_qlb[4];

//---L1L9Particle
   int          l1i;
   int          l9i;
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
   float        trd_thetas;
   float        trd_phis;
   float        trd_coos[3];
   float        trd_rqs;
   float        ecal_pos[3];
   float        ecal_ens;
   float        ecal_dis;
   float        tk_l1mds;
   float        tk_l9mds;
   float        tk_pos1s[9][3];
   float        tk_dir1s[3];
   float        tk_l1qvs;
   float        tk_l9qvs;
   float        tk_exqvn[2][2];//YJ+HL
   float        tk_l1qvr;
   float        tk_l9qvr;
   float        cutoffps; 

//---Particle
   int          nparticle;
   int          nbetah;
   int          nbeta;
   int          ncharge;
   int          ntrack;
   int          ntrrawcl;
   int          ntrcl;
   int          ntrhit;
   int          ntrdtrack;
   int          nshow;
   int          necalhit;
   int          ntofclh;
   int          ntrdseg;
   int          ntrdrawh;
   int          ntrdcl;
   int          nrich;
   int          nrichb;
   int          nrichh;
   int          btstat;
   int          ibeta;
   int          ibetah;
   int          itrtrack;
   int          itrdtrack;
   int          icharge;
   int          ishow;
   int          irich;
   int          irichb;
   float        show_dis;
   float        ec_pos[3];
   float        ec_dir[3];
   float        cutoffp[2];
   float        cutoffpi[2];
//---2nd Particle
  int          betah2p;//Pattern
  float        betah2q;//Q
  float        betah2r;//Rigidity
  int          betah2hb[2];//Hit
  float        betah2ch;//Chis
   
//---Track
   bool         tk_l1l9par;
   bool         tk_bgpar;
   bool         tk_bhgpar;
   bool         tk_ubpar;
   bool         tk_ngpar;
   int          tk_rtype;
   int          tk_nhiti[2];
   int          tk_hitb[2];//XY
   float        tk_iq;
   float        tk_1q;
   float        tk_9q;
   float        tk_q[2];
   float        tk_qr;//norig and beta correction
   float        tk_qrms[2];
   int          tk_qhit[2];
   int          tk_rz[2][2];
   float        tk_rpz[2][2];
   float        tk_ql[9];//Lay1+2+..9
   float        tk_ql2[9][2];
   int          tk_qls[9];
   int          tk_qlsn;
   float        tk_qin[2][3];
   float        tk_qrmn[2][3];
   float        tk_qln[2][9][3];//Lay1+2+..9
   float        tk_qlqs[9][2][5];
   int          tk_lid[9];
   int          tk_cad[9];
   int          tk_stripx[9];
   float        tk_adc[9][2][11];//L+XY+ii
   float        tk_xco[9][2][11];//L+XY+ii
   float        tk_cof[9][2];//L+XY
   float        tk_iso[9][2];//Y stripe isolation
//---External Two Layers
   float        tk_exql[2][3];
   float        tk_exqln[2][2][3];//YJ+LU
   int          tk_exqls[2];
   float        tk_exdis[2];
   int          tk_exlid[2];
   int          tk_excad[2];
//---Full+Inner+InnerL0+InnerL8+InnerUP+InnerDo
   float        tk_fzm[2];
   float        tk_rigidity1[3][3][7];//Algo+PGCIEMAT+Span
   float        tk_rigidityk[4][7];//Kalman/Z+Span
   float        tk_erigidity1[3][7];//Algo+Span
   float        tk_chis1[3][3][7][3];////Algo+PGCIEMAT+Span+XYA
   float        tk_pval[3][5][3];//Algo+Span+XYA
   float        tk_dir0[3][3][4];//Algo+PGCIEMAT+Span+X
   float        tk_bcor;
   float        tk_tcor;//1/Rc-1/Rr
   float        tk_res[9][2];//Residual Layer+XY
   float        tk_res1[9][2];//Residual Layer+XY
   float        tk_rigidityi[9];
   float        tk_chisi[9];
   float        tk_cdif[2][3];//PG-CIEMAT Layer19+XYZ
   float        tk_pos[9][3];
   float        tk_dir[9][3];
   float        tk_hitc[9][3];
   float        tk_hitcl[9][2];
   float        tk_oq[2];
   float        tk_oel[9]; 
   int          tk_ohitl[9];
//-----Kalman
   float        tk_rigidityr[4][3][7];//Z+Algo+Span
   float        tk_posr[4][3][7][3];//Z+Algo+Span+P
   float        tk_dirr[4][3][7][3];//Z+Algo+Span+P

//---TOF
   int          tof_btype;
   int          tof_bpatt;
   float        tof_beta;
   float        tof_betah;
   float        tof_ebetah;
   float        tof_ebetah_n;
   float        tof_betahmc;
   int          tof_hsumh;
   int          tof_hsumhu;
   float        tof_chist;
   float        tof_chisc;
   float        tof_chist_n;
   float        tof_chisc_n;
   float        tof_q[2];
   float        tof_qrms[2];
   int          tof_z;
   float        tof_pz;
   int          tof_zr;
   float        tof_pzr;
   int          tof_zud[2];
   float        tof_pud[2];
   int          tof_pass;
   int          tof_qs;//QStat 
   int          tof_nclhl[4];
   int          tof_barid[4];
   float        tof_pos[4][3];
   float        tof_ql[4];
   float        tof_ql1[4];
   float        tof_qlc[6][4];
   float        tof_qlpm[4][2][4];
   float        tof_qlr[3][4];//All+Anode+Dynode From BetaH
   float        tof_tl[4];
   unsigned int tof_sbit[4][2];
   float        tof_oq[4][2];//Two Max Non-BetaH ClusterH
   int          tof_ob[4][2];
   int          tof_oncl;

//--TrdTrack
   int          nitrdseg;
   float        distrd[3];///0x ,1y, 2theta
   float        trd_rq;
   int          trd_rz;
   float        trd_rpz;
   int          trd_pass;
   int          trd_statk;
   int          trd_nhitk;
   float        trd_lik[3];//e/p e/He p/He
   float        trd_like[3];
   int          trd_onhitk;
   float        trd_oampk;
   float        trd_qk[5];//A+U+D+Non-Dedx
   int          trd_qnhk[3];
   float        trd_ipch;
   float        trd_qrmsk[3];
   float        trd_amplk[20];//raw amps (ADC-chan)
   float        trd_pathlk[20];
   float        trd_ampsk;//tot.adc, path_length corrected
   int          trd_amphitk;//good hits for above
   float        trd_pathsk;//tot path_length

//--Rich
   int          rich_itrtrack;
   float        rich_beta[3];//getbeta+default+refit
   float        rich_ebeta;
   float        rich_ebeta1; 
   float        rich_pb;//Prob To Ring
   float        rich_udis;
   int          rich_hit;
   int          rich_used;
   int          rich_usedm;
   int          rich_stat;
   float        rich_q[2];//q2 from ring+q from ChargeR
   float        rich_width;
   float        rich_npe[3];//Collect and expect
   bool         rich_good;
   bool         rich_clean;
   bool         rich_NaF;
   int          rich_pmt;
   int          rich_pmt1;
   float        rich_PMTChargeConsistency;
   float        rich_BetaConsistency;
   float        rich_pos[3];
   float        rich_theta;
   float        rich_phi;
   float        rich_n[2];
   int          rich_tile;
   float        rich_distb;
   int          rich_cstat;
   float        rich_betap;
   float        rich_likp;
   float        rich_pbp;
   int          rich_usedp;
   float        rich_qp;
   int          rich_statp;
   float        rich_pzp[3];//3most probZ
   int          rich_rz;
   float        rich_rq;
   float        rich_rpz;

//---Ecal
   unsigned int ecal_stat;
   float        ecal_en[4];//EnergyE EnergyA EnergyD EnergyC
   float        ecal_enc[3];//GetCorrect-Energy(vdev-add)
   float        ecal_rearl;//EnergyE RearLeak
   float        ecal_bdt[3];//BDT+BDTA+BDTChis
   float        ecal_q;
   int          ecal_nhit[18];//Hit Number for each Layer
   float        ecal_el[18];//Energy for each Layer
   float        ecal_eh[18];//Max Cell Energy for each Layer

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
   float        tk_betahs;
   float        tk_tofchiscs;
   float        tk_tofchists;
   int          tk_tofsumhus;
   float        tk_tofqls[4];
};



void AMSAnalysis::BookFile(char *ofile){
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
    sstree[2]=AddBranch_RTI(tout[it]);
    sstree[3]=AddBranch_Particle(tout[it]);
    if(it==0)continue;
    sstree[4]=AddBranch_Trk(tout[it]);
    sstree[5]=AddBranch_Tof(tout[it]);
    sstree[7]=AddBranch_Trd(tout[it]);
    sstree[8]=AddBranch_Rich(tout[it]);
    sstree[9]=AddBranch_Ecal(tout[it]);
    sstree[10]=AddBranch_Anti(tout[it]);
    sstree[11]=AddBranch_TrkParticle(tout[it]);
 }

}

int AMSAnalysis::GetTreeSize(TTree *utree){
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


int AMSAnalysis::AddBranch_Header(TTree *utree){//12
    int lenb=GetTreeSize(utree);
    utree->Branch("run",      &run,      "run/i");
    utree->Branch("isbadrun", &isbadrun, "isbadrun/O");
    utree->Branch("isreal",   &isreal,   "isreal/O");
    utree->Branch("event",    &event,    "event/i");
    utree->Branch("version",  &version,  "version/I");
    utree->Branch("amsdn",    &amsdn,    "amsdn/I");
    utree->Branch("errorb",   &errorb,   "errorb/i");
    utree->Branch("time",      time,     "time[2]/i");
    utree->Branch("thetas",   &thetas,   "thetas/F");
    utree->Branch("phis",     &phis,     "phis/F");
    utree->Branch("rads",     &rads,     "rads/F");
    utree->Branch("yaw",      &yaw,      "yaw/F");
    utree->Branch("pitch",    &pitch,    "pitch/F");
    utree->Branch("roll",     &roll,     "roll/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Trigger(TTree *utree){//14
    int lenb=GetTreeSize(utree);
    utree->Branch("nlevel1",    &nlevel1,    "nlevel1/I");
    utree->Branch("ndaqev",     &ndaqev,     "ndaqev/I");
    utree->Branch("ndaqerr",    &ndaqerr,    "ndaqerr/I");
    utree->Branch("daqlen",     &daqlen,     "daqlen/i");
    utree->Branch("daqtyerr",   &daqtyerr,   "daqtyerr/I");
    utree->Branch("daqjrmerr",  &daqjrmerr,  "daqjrmerr/i");
    utree->Branch("daqjlen",     daqjlen,    "daqjlen[24]/i");
    utree->Branch("daqsdlen",   &daqsdlen,   "daqsdlen/i");
    utree->Branch("daqsdlenu",  &daqsdlenu,  "daqsdlenu/i");
    utree->Branch("livetime",   &livetime,   "livetime/F");
    utree->Branch("physbpatt",  &physbpatt,  "physbpatt/I");
    utree->Branch("jmembpatt",  &jmembpatt,  "jmembpatt/I");
    utree->Branch("physbpatt1", &physbpatt1, "physbpatt1/I");
    utree->Branch("jmembpatt1", &jmembpatt1, "jmembpatt1/I");
    utree->Branch("physbpatt2", &physbpatt2, "physbpatt2/I");
    utree->Branch("jmembpatt2", &jmembpatt2, "jmembpatt2/I");
    utree->Branch("ecalflag",   &ecalflag,   "ecalflag/I");
    utree->Branch("tofflag",     tofflag,    "tofflag[2]/I");
    utree->Branch("antipatt",   &antipatt,"   antipatt/I");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_RTI(TTree *utree){//22
    int lenb=GetTreeSize(utree);
    utree->Branch("irti",     &irti,     "irti/I");
    utree->Branch("zenith",   &zenith,   "zenith/F");
    utree->Branch("glong",    &glong,    "glong/F");
    utree->Branch("issaa",    &issaa,    "issaa/O");
    utree->Branch("glat",     &glat,     "glat/F");
    utree->Branch("thetam",   &thetam,   "thetam/F");
    utree->Branch("phim",     &phim,     "phim/F");
    utree->Branch("mcutoff",   mcutoff,  "mcutoff[4][2]/F");//Minus+Plus 
    utree->Branch("mcutoffi",  mcutoffi, "mcutoffi[4][2]/F");//Minus+Plus 
    utree->Branch("rtilf",    &rtilf,    "rtilf/F");
    utree->Branch("rtinev",   &rtinev,   "rtinev/F");
    utree->Branch("rtinerr",  &rtinerr,  "rtinerr/F");
    utree->Branch("rtintrig", &rtintrig, "rtintrig/F"); 
    utree->Branch("rtinpar",  &rtinpar,  "rtinpar/F"); 
    utree->Branch("rtigood",  &rtigood,  "rtigood/I");
    utree->Branch("rticdif",   rticdif,  "rticdif[2][3]/F");
    int lene=GetTreeSize(utree); 
    return lene-lenb;
}

int AMSAnalysis::AddBranch_MC(TTree *utree){
    int lenb=GetTreeSize(utree);
    utree->Branch("mpar",     &mpar,     "mpar/I");
    utree->Branch("mmom",     &mmom,     "mmom/F");
    utree->Branch("mch",      &mch,      "mch/F");
    utree->Branch("mtof_pass",&mtof_pass,"mtof_pass/I");
    utree->Branch("mevcoo",    mevcoo,   "mevcoo[3]/F");
    utree->Branch("mevdir",    mevdir,   "mevdir[3]/F");
    utree->Branch("mfcoo",     mfcoo,    "mfcoo[3]/F");
    utree->Branch("mfdir",     mfdir,    "mfdir[3]/F");
    utree->Branch("mpare",     mpare,    "mpare[2]/F");
    utree->Branch("mparp",     mparp,    "mparp[2]/I");
    utree->Branch("mparc",     mparc,    "mparc[2]/F");
    utree->Branch("msume",    &msume,    "msume/F");
    utree->Branch("msumc",    &msumc,    "msumc/F");
    utree->Branch("mtrdmom",   mtrdmom,  "mtrdmom[20]/F");
    utree->Branch("mtrdpar",   mtrdpar,  "mtrdpar[20]/I");
    utree->Branch("mtrdcoo",   mtrdcoo,  "mtrdcoo[20][3]/F");
    utree->Branch("ntrmccl",  &ntrmccl,  "ntrmccl/I");
    utree->Branch("mtrmom",    mtrmom,   "mtrmom[9]/F");
    utree->Branch("mtrpar",    mtrpar,   "mtrpar[9]/I");
    utree->Branch("mtrcoo",    mtrcoo,   "mtrcoo[9][3]/F");
    utree->Branch("mtrcoo1",   mtrcoo1,  "mtrcoo1[9][3]/F");
    utree->Branch("mtrpri",    mtrpri,   "mtrpri[9]/I");
    utree->Branch("mtrz",      mtrz,     "mtrz[9]/I");
    utree->Branch("ntofmccl", &ntofmccl, "ntofmccl/I");
    utree->Branch("mtofdep",   mtofdep,  "mtofdep[4]/F");
    utree->Branch("mtofpri",   mtofpri,  "mtofpri[4]/I");
    utree->Branch("mtofbar",   mtofbar,  "mtofbar[4]/I");
//---
    utree->Branch("mevcoo1",    mevcoo1,   "mevcoo1[21][3]/F");
    utree->Branch("mevdir1",    mevdir1,   "mevdir1[21][3]/F");
    utree->Branch("mevmom1",    mevmom1,   "mevmom1[21]/F");
//----
    utree->Branch("tof_qlsb",   tof_qlsb,  "tof_qlsb[4]/F");
    utree->Branch("tof_qlb",    tof_qlb,   "tof_qlb[4]/F");
//---
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Particle(TTree *utree){
    int lenb=GetTreeSize(utree);
    utree->Branch("tk_l1q",     &tk_l1q,     "tk_l1q/F");
    utree->Branch("tk_l9q",     &tk_l9q,     "tk_l9q/F");
    utree->Branch("tk_l1qxy",   tk_l1qxy,    "tk_l1qxy[2]/F");
    utree->Branch("tk_l9qxy",   tk_l9qxy,    "tk_l9qxy[2]/F");
    utree->Branch("tk_l1qs",    &tk_l1qs,    "tk_l1qs/I");
    utree->Branch("tk_l9qs",    &tk_l9qs,    "tk_l9qs/I");
    utree->Branch("ibetahs",    &ibetahs,    "ibetahs/I");
    utree->Branch("itrdtracks", &itrdtracks, "itrdtracks/I");
    utree->Branch("tof_nhits",  &tof_nhits,  "tof_nhits/I");
    utree->Branch("tof_hsumhus",&tof_hsumhus,"tof_hsumhus/I");
    utree->Branch("betahs",     &betahs,     "betahs/F");
    utree->Branch("betahsmc",   &betahsmc,   "betahsmc/F");
    utree->Branch("tof_chiscs", &tof_chiscs, "tof_chiscs/F");
    utree->Branch("tof_chists", &tof_chists, "tof_chists/F");
    utree->Branch("tof_qls",     tof_qls,    "tof_qls[4]/F");
    utree->Branch("trd_thetas", &trd_thetas, "trd_thetas/F");
    utree->Branch("trd_phis",   &trd_phis,   "trd_phis/F");
    utree->Branch("trd_coos",    trd_coos,   "trd_coos[3]/F");
    utree->Branch("trd_rqs",    &trd_rqs,    "trd_rqs/F");
    utree->Branch("ecal_pos",    ecal_pos,   "ecal_pos[3]/F");
    utree->Branch("ecal_ens",   &ecal_ens,   "ecal_ens/F");
    utree->Branch("ecal_dis",   &ecal_dis,   "ecal_dis/F");
    utree->Branch("tk_l1mds",   &tk_l1mds,   "tk_l1mds/F");
    utree->Branch("tk_l9mds",   &tk_l9mds,   "tk_l9mds/F");
    utree->Branch("tk_pos1s",    tk_pos1s,   "tk_pos1s[9][3]/F");
    utree->Branch("tk_dir1s",    tk_dir1s,   "tk_dir1s[3]/F");
    utree->Branch("tk_l1qvs",   &tk_l1qvs,   "tk_l1qvs/F");
    utree->Branch("tk_l9qvs",   &tk_l9qvs,   "tk_l9qvs/F");
    utree->Branch("tk_exqvn",    tk_exqvn,   "tk_exqvn[2][2]/F");
    utree->Branch("tk_l1qvr",   &tk_l1qvr,   "tk_l1qvr/F");
    utree->Branch("tk_l9qvr",   &tk_l9qvr,   "tk_l9qvr/F");
    utree->Branch("cutoffps",   &cutoffps,   "cutoffps/F");

   //--Particle //22
    utree->Branch("nparticle",  &nparticle,  "nparticle/I");
    utree->Branch("nbetah",     &nbetah,     "nbetah/I");
    utree->Branch("nbeta",      &nbeta,      "nbeta/I");
    utree->Branch("ntrack",     &ntrack,     "ntrack/I");
    utree->Branch("ntrrawcl",   &ntrrawcl,   "ntrrawcl/I");
    utree->Branch("ntrcl",      &ntrcl,      "ntrcl/I");
    utree->Branch("ntrhit",     &ntrhit,     "ntrhit/I");
    utree->Branch("ntrdtrack",  &ntrdtrack,  "ntrdtrack/I");
    utree->Branch("nrich",      &nrich,      "nrich/I");
    utree->Branch("nrichb",     &nrichb,     "nrichb/I");
    utree->Branch("nrichh",     &nrichh,     "nrichh/I");
    utree->Branch("nshow",      &nshow,      "nshow/I");
    utree->Branch("necalhit",   &necalhit,   "necalhit/I");
    utree->Branch("ntofclh",    &ntofclh,    "ntofclh/I");
    utree->Branch("ntrdseg",    &ntrdseg,    "ntrdseg/I");
    utree->Branch("ntrdrawh",   &ntrdrawh,   "ntrdrawh/I");
    utree->Branch("ntrdcl",     &ntrdcl,     "ntrdcl/I");
    utree->Branch("btstat",     &btstat,     "btstat/I"); //Status 1:Over cutoff, 2:Under cutoff, 3:Trapped
    utree->Branch("ibeta",      &ibeta,      "ibeta/I");
    utree->Branch("ibetah",     &ibetah,     "ibetah/I");
    utree->Branch("icharge",    &icharge,    "icharge/I");
    utree->Branch("itrtrack",   &itrtrack,   "itrtrack/I");
    utree->Branch("itrdtrack",  &itrdtrack,  "itrdtrack/I");
    utree->Branch("ishow",      &ishow,      "ishow/I");
    utree->Branch("irich",      &irich,      "irich/I");
    utree->Branch("irichb",     &irichb,     "irichb/I");
    utree->Branch("iparindex",  &iparindex,  "iparindex/I");
    utree->Branch("show_dis",   &show_dis,   "show_dis/F");
    utree->Branch("ec_pos",      ec_pos,     "ec_pos[3]/F");
    utree->Branch("ec_dir",      ec_dir,     "ec_dir[3]/F");
    utree->Branch("cutoffp",     cutoffp,    "cutoffp[2]/F");
    utree->Branch("cutoffpi",    cutoffpi,   "cutoffpi[2]/F");
    //--2ndParticle //6
    utree->Branch("betah2p",    &betah2p,    "betah2p/I");
    utree->Branch("betah2q",    &betah2q,    "betah2q/F");
    utree->Branch("betah2r",    &betah2r,    "betah2r/F");
    utree->Branch("betah2hb",    betah2hb,   "betah2hb[2]/I");
    utree->Branch("betah2ch",   &betah2ch,   "betah2ch/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Trk(TTree *utree){//52+380-54
    int lenb=GetTreeSize(utree);
    utree->Branch("tk_rtype",     &tk_rtype,       "tk_rtype/I");
    utree->Branch("tk_hitb",       tk_hitb,        "tk_hitb[2]/I");
    utree->Branch("tk_rz",         tk_rz,          "tk_rz[2][2]/I");//All+Inner
    utree->Branch("tk_rpz",        tk_rpz,         "tk_rpz[2][2]/F");//
    utree->Branch("tk_qr",        &tk_qr,          "tk_qr/F");
    utree->Branch("tk_q",          tk_q,           "tk_q[2]/F");//All+Inner
    utree->Branch("tk_qrms",       tk_qrms,        "tk_qrms[2]/F");
    utree->Branch("tk_qhit",       tk_qhit,        "tk_qhit[2]/I");
    utree->Branch("tk_ql",         tk_ql,          "tk_ql[9]/F");//Temp Only Layer1+2
    utree->Branch("tk_ql2",        tk_ql2,         "tk_ql2[9][2]/F");
    utree->Branch("tk_qls",        tk_qls,         "tk_qls[9]/I");//TK-QL Status
    utree->Branch("tk_qlsn",      &tk_qlsn,        "tk_qlsn/I");//TK_QL Status YJ
    utree->Branch("tk_qin",        tk_qin,         "tk_qin[2][3]/F");
    utree->Branch("tk_qrmn",       tk_qrmn,        "tk_qrmn[2][3]/F");
    utree->Branch("tk_qln",        tk_qln,         "tk_qln[2][9][3]/F");
    utree->Branch("tk_lid",        tk_lid,         "tk_lid[9]/I");//TK-Ladder-Id
    utree->Branch("tk_cad",        tk_cad,         "tk_cad[9]/I");
    utree->Branch("tk_cof",        tk_cof,         "tk_cof[9][2]/F");
#ifdef _USETKADC_
    utree->Branch("tk_qlqs",       tk_qlqs,        "tk_qlqs[9][2][5]/F");
    utree->Branch("tk_stripx",     tk_stripx,      "tk_stripx[9]/I");
    utree->Branch("tk_adc",        tk_adc,         "tk_adc[9][2][11]/F");
    utree->Branch("tk_xco",        tk_xco,         "tk_xco[9][2][11]/F");
#endif
    utree->Branch("tk_iso",        tk_iso,         "tk_iso[9][2]/F"); 
///--
    utree->Branch("tk_exql",       tk_exql,        "tk_exql[2][3]/F");
    utree->Branch("tk_exqln",      tk_exqln,       "tk_exqln[2][2][3]/F");
    utree->Branch("tk_exqls",      tk_exqls,       "tk_exqls[2]/I");//TK-QL Status
    utree->Branch("tk_exdis",      tk_exdis,       "tk_exdis[2]/F");
    utree->Branch("tk_exlid",      tk_exlid,       "tk_exlid[2]/I");
    utree->Branch("tk_excad",      tk_excad,       "tk_excad[2]/I");
///-All Span Rigidity
    utree->Branch("tk_res",        tk_res,         "tk_res[9][2]/F");
#ifdef _USELAYERRES_
    utree->Branch("tk_res1",       tk_res1,        "tk_res1[9][2]/F");
#endif
    utree->Branch("tk_fzm",        tk_fzm,         "tk_fzm[2]/F");
    utree->Branch("tk_rigidity1",  tk_rigidity1,   "tk_rigidity1[3][3][7]/F");
    utree->Branch("tk_rigidityk",  tk_rigidityk,   "tk_rigidityk[4][7]/F");
    utree->Branch("tk_erigidity1", tk_erigidity1,  "tk_erigidity1[3][7]/F");
    utree->Branch("tk_chis1",      tk_chis1,       "tk_chis1[3][3][7][3]/F");
    utree->Branch("tk_pval",       tk_pval,        "tk_pval[3][5][3]/F");
    utree->Branch("tk_dir0",       tk_dir0,        "tk_dir0[3][3][4]/F");
    utree->Branch("tk_rigidityi",  tk_rigidityi,   "tk_rigidityi[9]/F");
    utree->Branch("tk_chisi",      tk_chisi,       "tk_chisi[9]/F");
    utree->Branch("tk_bcor",      &tk_bcor,        "tk_bcor/F");
    utree->Branch("tk_tcor",      &tk_tcor,        "tk_tcor/F");
    utree->Branch("tk_cdif",       tk_cdif,        "tk_cdif[2][3]/F");
    utree->Branch("tk_pos",        tk_pos,         "tk_pos[9][3]/F");
    utree->Branch("tk_dir",        tk_dir,         "tk_dir[9][3]/F");
    utree->Branch("tk_hitc",       tk_hitc,        "tk_hitc[9][3]/F");
#ifdef _USETKADC_
    utree->Branch("tk_hitcl",      tk_hitcl,       "tk_hitcl[9][2]/F");
#endif
    utree->Branch("tk_oq",         tk_oq,          "tk_oq[2]/F");
    utree->Branch("tk_oel",        tk_oel,         "tk_oel[9]/F");
    utree->Branch("tk_ohitl",      tk_ohitl,       "tk_ohitl[9]/I");
#ifdef _USEKALMANFIT_
    utree->Branch("tk_rigidityr",  tk_rigidityr,   "tk_rigidityr[4][3][7]/F");
    utree->Branch("tk_posr",       tk_posr,        "tk_posr[4][3][7][3]/F");
    utree->Branch("tk_dirr",       tk_dirr,        "tk_dirr[4][3][7][3]/F");
#endif
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Tof(TTree *utree){//45
    int lenb=GetTreeSize(utree);
    utree->Branch("tof_btype",   &tof_btype,   "tof_btype/I");
    utree->Branch("tof_bpatt",   &tof_bpatt,   "tof_bpatt/I");
    utree->Branch("tof_beta",    &tof_beta,    "tof_beta/F");
    utree->Branch("tof_betah",   &tof_betah,   "tof_betah/F");
    utree->Branch("tof_ebetah",  &tof_ebetah,  "tof_ebetah/F");
    utree->Branch("tof_ebetah_n",&tof_ebetah_n,"tof_ebetah_n/F");
    utree->Branch("tof_betahmc", &tof_betahmc, "tof_betahmc/F");
    utree->Branch("tof_hsumh",   &tof_hsumh,   "tof_hsumh/I"); //BetaH
    utree->Branch("tof_hsumhu",  &tof_hsumhu,  "tof_hsumhu/I");//
    utree->Branch("tof_nclhl",    tof_nclhl,   "tof_nclhl[4]/I");
    utree->Branch("tof_barid",    tof_barid,   "tof_barid[4]/I");
    utree->Branch("tof_pos",      tof_pos,     "tof_pos[4][3]/F");
    utree->Branch("tof_pass",    &tof_pass,    "tof_pass/I");
    utree->Branch("tof_oq",       tof_oq,      "tof_oq[4][2]/F");
    utree->Branch("tof_ob",       tof_ob,      "tof_ob[4][2]/I");
    utree->Branch("tof_oncl",    &tof_oncl,    "tof_oncl/I");
    utree->Branch("tof_chist",   &tof_chist,   "tof_chist/F");
    utree->Branch("tof_chisc",   &tof_chisc,   "tof_chisc/F");
    utree->Branch("tof_chist_n", &tof_chist_n, "tof_chist_n/F");
    utree->Branch("tof_chisc_n", &tof_chisc_n, "tof_chisc_n/F");
    utree->Branch("tof_z",       &tof_z,       "tof_z/I");
    utree->Branch("tof_pz",      &tof_pz,      "tof_pz/F");
    utree->Branch("tof_zr",      &tof_zr,      "tof_zr/I");
    utree->Branch("tof_pzr",     &tof_pzr,     "tof_pzr/F");
    utree->Branch("tof_zud",      tof_zud,     "tof_zud[2]/I");
    utree->Branch("tof_pud",      tof_pud,     "tof_pud[2]/F");
    utree->Branch("tof_q",        tof_q,       "tof_q[2]/F");//Max+Trun TofChargeH+BetaH
    utree->Branch("tof_qrms",     tof_qrms,    "tof_qrms[2]/F");
    utree->Branch("tof_ql",       tof_ql,      "tof_ql[4]/F");
    utree->Branch("tof_ql1",      tof_ql1,     "tof_ql1[4]/F");
#ifdef _USETKADC_
    utree->Branch("tof_qlc",      tof_qlc,     "tof_qlc[6][4]/F");
    utree->Branch("tof_qlpm",     tof_qlpm,    "tof_qlpm[4][2][4]/F");
#endif
    utree->Branch("tof_qlr",      tof_qlr,     "tof_qlr[3][4]/F");
    utree->Branch("tof_tl",       tof_tl,      "tof_tl[4]/F");
    utree->Branch("tof_sbit",     tof_sbit,    "tof_sbit[4][2]/i");
    utree->Branch("tof_qs"  ,    &tof_qs,      "tof_qs/I");//Q-Status
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Trd(TTree *utree){//25
    int lenb=GetTreeSize(utree);
    utree->Branch("nitrdseg",    &nitrdseg,   "nitrdseg/I");
    utree->Branch("distrd",       distrd,     "distrd[3]/F");
    utree->Branch("trd_pass",    &trd_pass,   "trd_pass/I");
    utree->Branch("trd_statk",   &trd_statk,  "trd_statk/I");
    utree->Branch("trd_nhitk",   &trd_nhitk,  "trd_nhitk/I");
    utree->Branch("trd_lik",      trd_lik,    "trd_lik[3]/F");
    utree->Branch("trd_like",     trd_like,   "trd_like[3]/F");
    utree->Branch("trd_onhitk",  &trd_onhitk, "trd_onhitk/I");
    utree->Branch("trd_oampk",   &trd_oampk,  "trd_oampk/F");
    utree->Branch("trd_qk",       trd_qk,     "trd_qk[5]/F");//All+Up+Down
    utree->Branch("trd_qnhk",     trd_qnhk,   "trd_qnhk[3]/I");
    utree->Branch("trd_ipch",    &trd_ipch,   "trd_ipch/F");
    utree->Branch("trd_qrmsk",    trd_qrmsk,  "trd_qrmsk[3]/F");
    utree->Branch("trd_rq",      &trd_rq,     "trd_rq/F");
    utree->Branch("trd_rz",      &trd_rz,     "trd_rz/I");
    utree->Branch("trd_rpz",     &trd_rpz,    "trd_rpz/F");
    utree->Branch("trd_amplk",    trd_amplk,  "trd_amplk[20]/F");
    utree->Branch("trd_pathlk",   trd_pathlk, "trd_pathlk[20]/F");
    utree->Branch("trd_ampsk",   &trd_ampsk,  "trd_ampsk/F");
    utree->Branch("trd_pathsk",  &trd_pathsk, "trd_pathsk/F");
    utree->Branch("trd_amphitk", &trd_amphitk,"trd_amphitk/I");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Rich(TTree *utree){//25
    int lenb=GetTreeSize(utree);
    utree->Branch("rich_itrtrack",&rich_itrtrack,"rich_itrtrack/I");
    utree->Branch("rich_beta",     rich_beta,  "rich_beta[3]/F");
    utree->Branch("rich_ebeta",   &rich_ebeta, "rich_ebeta/F");
    utree->Branch("rich_ebeta1",   &rich_ebeta1, "rich_ebeta1/F");
    utree->Branch("rich_pb",      &rich_pb,    "rich_pb/F");
    utree->Branch("rich_udis",    &rich_udis,  "rich_udis/F");
    utree->Branch("rich_hit",     &rich_hit,  "rich_hit/I");
    utree->Branch("rich_used",    &rich_used,  "rich_used/I");
    utree->Branch("rich_usedm",   &rich_usedm, "rich_usedm/I");
    utree->Branch("rich_stat",    &rich_stat,  "rich_stat/I");
    utree->Branch("rich_q",        rich_q,     "rich_q[2]/F");
    utree->Branch("rich_width",   &rich_width, "rich_width/F");
    utree->Branch("rich_npe",      rich_npe,   "rich_npe[3]/F");
    utree->Branch("rich_good",    &rich_good,  "rich_good/O");
    utree->Branch("rich_clean",   &rich_clean, "rich_clean/O");
    utree->Branch("rich_NaF",     &rich_NaF,   "rich_NaF/O");
    utree->Branch("rich_pmt",     &rich_pmt,    "rich_pmt/I");
    utree->Branch("rich_pmt1",    &rich_pmt1,   "rich_pmt1/I");
    utree->Branch("rich_PMTChargeConsistency",&rich_PMTChargeConsistency,"rich_PMTChargeConsistency/F");
    utree->Branch("rich_BetaConsistency",&rich_BetaConsistency,"rich_BetaConsistency/F");
    utree->Branch("rich_pos",      rich_pos,    "rich_pos[3]/F");
    utree->Branch("rich_theta",   &rich_theta,  "rich_theta/F");
    utree->Branch("rich_phi",     &rich_phi,    "rich_phi/F");
    utree->Branch("rich_n",        rich_n,      "rich_n[2]/F");
    utree->Branch("rich_tile",    &rich_tile,   "rich_tile/I");
    utree->Branch("rich_distb",   &rich_distb,  "rich_distb/F");
    utree->Branch("rich_cstat",   &rich_cstat,  "rich_cstat/I");
//--Lip
    utree->Branch("rich_betap",   &rich_betap, "rich_betap/F");
    utree->Branch("rich_likp",    &rich_likp,  "rich_likp/F");
    utree->Branch("rich_pbp",     &rich_pbp,   "rich_pbp/F");
    utree->Branch("rich_usedp",   &rich_usedp, "rich_usedp/I");
    utree->Branch("rich_statp",   &rich_statp, "rich_statp/I");
    utree->Branch("rich_qp",      &rich_qp,    "rich_qp/F");
    utree->Branch("rich_pzp",      rich_pzp,   "rich_pzp[3]/F");
//--RICHZ
    utree->Branch("rich_rz",      &rich_rz,    "rich_rz/I");
    utree->Branch("rich_rq",      &rich_rq,    "rich_rq/F");
    utree->Branch("rich_rpz",     &rich_rpz,   "rich_rpz/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Ecal(TTree *utree){
   int lenb=GetTreeSize(utree);
   utree->Branch("ecal_stat",   &ecal_stat,    "ecal_stat/i"); 
   utree->Branch("ecal_en",      ecal_en,      "ecal_en[4]/F"); //EnergyE EnergyA EnergyD
   utree->Branch("ecal_enc",     ecal_enc,     "ecal_enc[3]/F");//GetCorrect-Energy(vdev-add)
   utree->Branch("ecal_rearl",  &ecal_rearl,   "ecal_rearl/F");
   utree->Branch("ecal_bdt",     ecal_bdt,     "ecal_bdt[3]/F");
   utree->Branch("ecal_q",      &ecal_q,       "ecal_q/F");
   utree->Branch("ecal_nhit",    ecal_nhit,    "ecal_nhit[18]/I");
   utree->Branch("ecal_el",      ecal_el,      "ecal_el[18]/F");
   utree->Branch("ecal_eh",      ecal_eh,      "ecal_eh[18]/F");
   int lene=GetTreeSize(utree);
   return lene-lenb;
}

int AMSAnalysis::AddBranch_Anti(TTree *utree){
   int lenb=GetTreeSize(utree);
   utree->Branch("anti_nhit",   &anti_nhit,    "anti_nhit/I");
   int lene=GetTreeSize(utree);
   return lene-lenb;
}

int AMSAnalysis::AddBranch_TrkParticle(TTree *utree){
   int lenb=GetTreeSize(utree);
   utree->Branch("itrtracks",   &itrtracks,    "itrtracks/I");
   utree->Branch("tk_hitbs",     tk_hitbs,     "tk_hitbs[2]/I");
   utree->Branch("tk_qis",       tk_qis,       "tk_qis[2]/F");//All+Inner
   utree->Branch("tk_qirmss",    tk_qirmss,    "tk_qirmss[2]/F");
   utree->Branch("tk_qlss",      tk_qlss,      "tk_qlss[2][9]/F");
   utree->Branch("tk_rigiditys", tk_rigiditys, "tk_rigiditys[5]/F");
   utree->Branch("tk_chiss",     tk_chiss,     "tk_chiss[5][3]/F");
   utree->Branch("tk_pos2s",     tk_pos2s,     "tk_pos2s[9][3]/F");
   utree->Branch("tk_dir2s",     tk_dir2s,     "tk_dir2s[2][3]/F");
   utree->Branch("tk_ibetahs",  &tk_ibetahs,   "tk_ibetahs/I");
   utree->Branch("tk_betahs",   &tk_betahs,    "tk_betahs/F");
   utree->Branch("tk_tofchiscs",&tk_tofchiscs, "tk_tofchiscs/F");
   utree->Branch("tk_tofchists",&tk_tofchists, "tk_tofchists/F");
   utree->Branch("tk_tofsumhus",&tk_tofsumhus, "tk_tofsumhus/I");
   utree->Branch("tk_tofqls",    tk_tofqls,    "tk_tofqls[4]/F");
   int lene=GetTreeSize(utree);
   return lene-lenb;
}


int AMSAnalysis::LoadHeader(){

///--RTI
#ifdef _USEPASS7_
     AMSSetupR::RTI::UseLatest(7);
#else
     AMSSetupR::RTI::UseLatest(6);
#endif

///--Track
     TkDBc::UseFinal();
     TRMCFFKEY_DEF::ReadFromFile = 0;
     TRFITFFKEY_DEF::ReadFromFile = 0;
     cout<<"NewN TkVersion Used"<<endl;

    return 0;
}


int AMSAnalysis::LoadHeaderEv(){

//---Center of Gravity
    TRCLFFKEY.ClusterCofGOpt=1;
    if(isreal){TrLinearEtaDB::SetLinearCluster();}
    return 0;
}


int  AMSAnalysis::LoadHeaderMC(int mcz,int vers,int mparid){

     TRFITFFKEY.magtemp = 0;
     TRFITFFKEY.Zshift=-1;
//---Load default tuning parameter
     TRMCFFKEY.MCtuneDmax = 100e-4;
     TRMCFFKEY.MCtuneDs[0]= 0;//InnerX
     TRMCFFKEY.MCtuneDs[1]= -1.000001;//InnerY
     TRMCFFKEY.MCtuneDy9  = 0;
/*     TRMCFFKEY.OuterSmearing[0][1] = -7.5e-4;//L1Y
     TRMCFFKEY.OuterSmearing[1][1] = -8.3e-4;//L9Y
     TRMCFFKEY.OuterSmearing[0][0] = 7.5e-4;//L1X
     TRMCFFKEY.OuterSmearing[1][0] = 8.5e-4;//L9X*/
     TRMCFFKEY.OuterSmearing[0][1] = -11.4e-4;//L1Y
     TRMCFFKEY.OuterSmearing[1][1] = -12.6e-4;//L9Y
     TRMCFFKEY.OuterSmearing[0][0] = 11.4e-4;//L1X
     TRMCFFKEY.OuterSmearing[1][0] = 12.6e-4;//L9X
     TRMCFFKEY.MCscat[0] = 0; 
     TRMCFFKEY.MCscat[1] = 0; 
     TRMCFFKEY.MCscat[2] = 0;
#ifdef _USEMCTKRAW_
     TRMCFFKEY.MCtuneDs[1]= -1.000001;
#else
     if (abs(mcz)==9){//F
        TRMCFFKEY.MCtuneDs[1]= -900.;
     }
     else if (abs(mcz)==10){//Neon
        TRMCFFKEY.MCtuneDs[1]= -1000.;
     }
     else if (abs(mcz)==12){//Magnesium
        TRMCFFKEY.MCtuneDs[1]= -1200.;
     }
     else if (abs(mcz)==13){//Al
        TRMCFFKEY.MCtuneDs[1]= -1300.;
     }
     else if (abs(mcz)==14){//Silicon
        TRMCFFKEY.MCtuneDs[1]= -1400.;
     }
     else if (abs(mcz)==16){//Sulfur
        TRMCFFKEY.MCtuneDs[1]= -1600.;
     }
     else if (abs(mcz)==26){//Ferrum
        TRMCFFKEY.MCtuneDs[1]= -2600.;
     }
     else if (abs(mcz)==8){//Oxygen
//       if(vers>=1106)TRMCFFKEY.MCtuneDs[1]= -1.000001;
       if     (vers>=1200)TRMCFFKEY.MCtuneDs[1]= -820.;
       else               TRMCFFKEY.MCtuneDs[1]= -810.;
     }
     else if (abs(mcz)==7){//Nitrogen
//       if(vers>=1106)TRMCFFKEY.MCtuneDs[1]= -1.000001;
       TRMCFFKEY.MCtuneDs[1]= -710.;
     }
     else if (abs(mcz)==6){//Carbon
//       if     (vers>=1106)TRMCFFKEY.MCtuneDs[1]= -1.000001;
       if     (vers>=1200)TRMCFFKEY.MCtuneDs[1]= -680.;
       else               TRMCFFKEY.MCtuneDs[1]= -670.;
     }
     else if (abs(mcz)==5){//Boron
//       if     (vers>=1106)TRMCFFKEY.MCtuneDs[1]= -1.000001;
       TRMCFFKEY.MCtuneDs[1]= -550.;
     }
     else if (abs(mcz)==4){//Beryllium
       TRMCFFKEY.MCtuneDs[1]= -410.;
     }
     else if (abs(mcz)==3){//Lithium
//       if(vers>=1106)TRMCFFKEY.MCtuneDs[1]= -1.000001;
       TRMCFFKEY.MCtuneDs[1]= -340.; 
     }
     else if (abs(mcz)==2){//Helium
//       if     (vers>=1106)TRMCFFKEY.MCtuneDs[1]= -1.000001;
       TRMCFFKEY.MCtuneDs[1]= -230.;
     }
     else if (abs(mcz)==1){
       bool ismcproton=(abs(mparid)==14||abs(mparid)==15);
       if(vers>=1106){
//         TRMCFFKEY.MCtuneDs[1]= -1.000001;
         TRMCFFKEY.MCtuneDs[1]= -110.;
       }
       else if(vers>=928&&ismcproton){
         TRMCFFKEY.MCtuneDs[0]= -9.0e-4;
         TRMCFFKEY.MCtuneDs[1]=  101.0;
       }
       if(vers<900&&ismcproton){//Proton
         TRMCFFKEY.MCtuneDy9  = 1.0e-4;  //<B900
         TRMCFFKEY.MCscat[0] = -15;      // This is NEW
         TRMCFFKEY.MCscat[1] = -440.06;  // This is NEW
         TRMCFFKEY.MCscat[2] = -440.05;  // This is NEW
       }
    }
#endif
//---TRDK
  TrdKCluster::ForceReadAlignment=0;
  TrdKCluster::ForceReadCalibration=0;
  TrdKCluster::ForceReadXePressure=0;
  TrdKCluster::SetDefaultMCXePressure(900);
  return 0;
}

int AMSAnalysis::Select(){

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

//----RTI or MC
   tb=te;
   bool selrti=isreal?Select_RTI():Select_MC(0);
   if(isreal==0)LoadHeaderMC(int(mch),version,mpar);
   te=clock();
   cput[2]+=(te-tb);
   if(!selrti)return 0;
   sstat[3]++;

//---Particle 2
   tb=te;
   bool selpart=Select_Particle();
   te=clock();
   cput[3]+=(te-tb); 
   if(!selpart)return 0;
   sstat[4]++;
 
//----MC2
   if(isreal==0){
     tb=te;
     selrti=Select_MC(1);
     te=clock();
     cput[2]+=(te-tb);
//     if(!selrti)return 0;
   } 
 
//--Preselection Track 3
   tb=te;
   bool seltrk1=Select_Trk(0);
   te=clock();
   cput[4]+=(te-tb);
//   if(!seltrk1)return 1;
   sstat[5]++;

//--Strict Selection Tof 4
   tb=te;
   bool seltof=Select_Tof();
   te=clock();
   cput[5]+=(te-tb);
//   if(!seltof)return 1;
   sstat[6]++;

//---Track 5
   tb=te;
   bool seltrk0=Select_Trk(1);
   te=clock();
   cput[6]+=(te-tb);
//   if(!seltrk0)return 1;
   sstat[7]++;

//---Trd 6
   tb=te;
   bool seltrd=Select_Trd();
   te=clock();
   cput[7]+=(te-tb);
//   if(!seltrd)return 1;
   sstat[8]++;

//---Rich 7
   tb=te;
   bool selrich=Select_Rich();
   te=clock();
   cput[8]+=(te-tb);
//   if(!selrich)return 1;
   sstat[9]++;   

//---Ecal 8
   tb=te;
   bool selecal=Select_Ecal();
   te=clock();
   cput[9]+=(te-tb);
//   if(!selecal)return 1;
   sstat[10]++;

//--Anti 9
   tb=te;
   bool selanti=Select_Anti();
   te=clock();
   cput[10]+=(te-tb);
//   if(!selanti)return 1;
   sstat[11]++;

//--TrkParticle 10
   tb=te;
   bool seltrkpart=Select_TrkParticle();
   te=clock();
   cput[11]+=(te-tb);
//   if(!seltrkpart)return 1;
   sstat[12]++;

   return true;
}

//---Header
bool  AMSAnalysis::Select_Header(){

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
bool AMSAnalysis::Select_Trigger(){

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
      tofflag[0]=lv->TofFlag1;
      tofflag[1]=lv->TofFlag2;
      ecalflag=lv->EcalFlag;
      antipatt=  lv->AntiPatt;
    }
//--DAQ
    ndaqev=pev->nDaqEvent();
    ndaqerr=0;
    daqlen=0;
    daqtyerr=0;
    if(pev->fStatus&(1<<30))daqtyerr=1;
    if(ndaqev>=1){
       DaqEventR *pdaq=pev->pDaqEvent(0);
       if(pdaq->HasHWError())ndaqerr++;
       daqlen=pdaq->Length;
       if(pdaq->L3RunError())daqtyerr|=(1<<1);
       if(pdaq->L3EventError())daqtyerr|=(1<<2);
       if(pdaq->L3ProcError())daqtyerr|=(1<<3);
       for(int ino=0;ino<=6;ino++){
         if(pdaq->L3NodeError(ino))daqtyerr|=(1<<(4+ino));
       }
       daqjrmerr=pdaq->JRoomError;
       for(int ii=0;ii<24;ii++)daqjlen[ii]=pdaq->JLength[ii];
       daqsdlen=pdaq->SDLength();
       daqsdlenu=pdaq->SDLengthU();
    }
    else {
       daqjrmerr=0;
       for(int ii=0;ii<24;ii++)daqjlen[ii]=0;
       daqsdlen=daqsdlenu=0;
    }
    return true;
}
//---MC
bool AMSAnalysis::Select_MC(int opt){

//---
 if(isreal)return true;
 int mnpart=pev->nMCEventg();

//---MCEveng0
  if(opt==0){

    ntrmccl=pev->nTrMCCluster();
    ntofmccl=pev->nTofMCCluster();
//----
    MCEventgR *mcev0=pev->pMCEventg(0);
    MCEventgR *mcev=pev->GetPrimaryMC();
//  if(mcev0!=mcev)cout<<"Error MCEvent(0)"<<" run="<<run<<" event="<<event<<endl;
//----
    mpar=mcev->Particle;
    mmom=mcev->Momentum;
    mch= mcev->Charge;

//----Coo+Dir
    for(int ico=0;ico<3;ico++){
       mevcoo[ico]=mcev->Coo[ico]; mevdir[ico]=mcev->Dir[ico];
    }

  }
//---MCEvengI
  else {
 
    AMSPoint pnt(mevcoo[0],mevcoo[1],mevcoo[2]);
    AMSDir dir(mevdir[0],mevdir[1],mevdir[2]);

//---MCEveng0 TOF Pass
    AMSPoint tofpnt;
    float disedge;double utm;
    mtof_pass=0;
    int ntofp=0;
    for(int ilay=0;ilay<4;ilay++){
      int tk_pass=ParticleR::IsPassTOF(ilay,pnt,dir,tofpnt,disedge);
      if(tk_pass>=0){mtof_pass+=int(pow(10.,3-ilay));ntofp++;}
    }

//---MCEvengN Interaction Point Find Max Kine
    for(int ic=0;ic<3;ic++){mfcoo[ic]=-1000;mfdir[ic]=-1000;}
    for(int is=0;is<2;is++){
      mpare[is]=mparp[is]=mparc[is]=0;
    }
    for(int ipar=1;ipar<mnpart;ipar++){
      MCEventgR *mcev=pev->pMCEventg(ipar);
      int parid=mcev->Particle;
//    if(parid<0)cout<<"parid="<<parid<<endl;
      if(fabs(parid)==1||fabs(parid)==2||fabs(parid)==3)continue;//gamma+positron+electron
      if(mcev->Coo[2]<-138||mcev->Nskip==-2||fabs(mcev->Particle)==fabs(mpar))continue;
      if(mcev->parentID!=1)continue;
      if(mcev->Momentum>mpare[0]){
        mpare[0]=mcev->Momentum;
        for(int ic=0;ic<3;ic++){
          mfcoo[ic]=mcev->Coo[ic];
          mfdir[ic]=mcev->Dir[ic];
        }
      }
    }
//Again
    msume=msumc=0;
    for(int iz=0;iz<21;iz++){
      mevmom1[iz]=-1000;
      for(int ic=0;ic<3;ic++){mevcoo1[iz][ic]=-1000;mevdir1[iz][ic]=-1000;}
    }
    for(int ipar=1;ipar<mnpart;ipar++){
      MCEventgR *mcev=pev->pMCEventg(ipar);
      if(mcev->Coo[2]==mfcoo[2]){
         msume+=mcev->Momentum;
         msumc+=mcev->Charge;
         if(mcev->Momentum==mpare[0]){mpare[0]=mcev->Momentum;mparp[0]=mcev->Particle;mparc[0]=mcev->Charge;}//MaxE
         if(fabs(mcev->Charge)>mparc[1]||(fabs(mcev->Charge)==mparc[1]&&mcev->Momentum==mpare[0])){
            mpare[1]=mcev->Momentum;mparp[1]=mcev->Particle;mparc[1]=mcev->Charge;
          }//MaxCh
      }
//--AC
      int iskip=mcev->Nskip;
      if(iskip>=-1020&&iskip<=-1000){
         iskip=fabs(iskip)-1000;
         mevmom1[iskip]=mcev->Momentum;
         for(int ic=0;ic<3;ic++){
           mevcoo1[iskip][ic]=mcev->Coo[ic];
           mevdir1[iskip][ic]=mcev->Dir[ic];
        }
      }
    }

//---TrdMCCluster MaxE
    for(int ilay=0;ilay<20;ilay++){mtrdmom[ilay]=0;mtrdpar[ilay]=0;for(int ic=0;ic<3;ic++)mtrdcoo[ilay][ic]=0;}
    for(int itrd=0;itrd<pev->nTrdMCCluster();itrd++){
      TrdMCClusterR *trdmc=pev->pTrdMCCluster(itrd);
      int ilay=trdmc->Layer;
      if(trdmc->Ekin>mtrdmom[ilay]){
        mtrdmom[ilay]=trdmc->Ekin;
        mtrdpar[ilay]=trdmc->ParticleNo;
        for(int ic=0;ic<3;ic++)mtrdcoo[ilay][ic]=trdmc->Xgl[ic];
      }
    }

//--TrackMCCluster MaxE
    for(int ilay=0;ilay<9;ilay++){
      mtrmom[ilay]=mtrpri[ilay]=mtrpar[ilay]=mtrz[ilay]=0;
      for(int ic=0;ic<3;ic++){mtrcoo[ilay][ic]=0;mtrcoo1[ilay][ic]=0;}
    }
    for(int itr=0;itr<pev->nTrMCCluster();itr++){
      TrMCClusterR *trmc=pev->pTrMCCluster(itr);
      int il=fabs(trmc->GetTkId()/100);
      if     (il==8)il=1;
      else if(il<8)il=il+1;
      il--;
      if(trmc->GetMomentum()>mtrmom[il]){
        mtrmom[il]=trmc->GetMomentum();
        mtrpar[il]=trmc->GetPart();
        for(int ic=0;ic<3;ic++){mtrcoo[il][ic]=trmc->GetStartPoint()[ic];mtrcoo1[il][ic]=trmc->GetXgl()[ic];}
        mtrpri[il]=trmc->IsPrimary()?1:0;
        mtrz[il]=trmc->Status;
      }
    }

//--TofMCCluster
    for(int ilay=0;ilay<4;ilay++){
      mtofdep[ilay]=mtofpri[ilay]=0;
      mtofbar[ilay]=-1;
    }
    double mtofe[4][10]={{0}};
    double mtofepri[4][10]={{0}};
    for(int imccl=0;imccl<pev->nTofMCCluster();imccl++){
      TofMCClusterR *tofmc=pev->pTofMCCluster(imccl);
      int ilay=tofmc->GetLayer();
      int ibar=tofmc->GetBar();
      mtofe[ilay][ibar]+=tofmc->EdepR*1000;//GeV->MeV
      if(tofmc->ParentNo==0){
        mtofpri[ilay]=1;
        mtofepri[ilay][ibar]+=tofmc->EdepR*1000;
      }
    }
    for(int ilay=0;ilay<4;ilay++){
      double maxdep=0;
      for(int ibar=0;ibar<10;ibar++){
        if(mtofpri[ilay]==1&&mtofepri[ilay][ibar]>maxdep){//Primary
          mtofbar[ilay]=ibar;
          maxdep=mtofdep[ilay]=mtofepri[ilay][ibar];
        }
        else if(mtofpri[ilay]==0&&mtofe[ilay][ibar]>maxdep){//No
          mtofbar[ilay]=ibar;
          maxdep=mtofdep[ilay]=mtofe[ilay][ibar];
        }
      }
    } 


  }

  return true;

}


//---RTI
bool AMSAnalysis::Select_RTI(){
    
    irti=-1;
    if(isreal==0)return true;
    AMSSetupR::RTI a;
    if(pev->GetRTI(a)!=0)return true;

//--Time Cut Provide By VC
     bool cut[10]={0};
     cut[0]=(a.ntrig/a.nev>0.98);
     cut[1]=(a.npart/a.ntrig>0.07/1600*a.ntrig&&a.npart/a.ntrig<0.25);
     cut[2]=(a.lf>0.5);
     cut[3]=(a.zenith<40);
     cut[3]=(a.nerr>=0&&a.nerr/a.nev<0.1);
     cut[4]=(a.npart>0&&a.nev<1800);
     bool tcut=(cut[0]&&cut[1]&&cut[2]&&cut[3]&&cut[4]);

//----RTI var
     zenith=a.zenith;
     thetas=a.theta;
     phis=a.phi;
     rads=a.r;
     glat=a.glat;
     glong=a.glong;
     issaa=a.IsInSAA();
//---cutoff
     for(int ifv=0;ifv<4;ifv++){
       for(int ipn=0;ipn<2;ipn++){mcutoff[ifv][ipn]=a.cf[ifv][ipn];mcutoffi[ifv][ipn]=a.cfi[ifv][ipn];}
     }
//---more 
     rtilf=a.lf;
     rtinev=a.nev;
     rtinerr=a.nerr;
     rtintrig=a.ntrig;
     rtinpar=a.npart;
     rtigood=a.good;
     irti=0;
//---PG-CIEMAT
     for(int uexl=0;uexl<2;uexl++){
       AMSPoint nxyz,dxyz;
       pev->GetRTIdL1L9(uexl,nxyz,dxyz,a.utime,60);
       for(int ixy=0;ixy<3;ixy++)rticdif[uexl][ixy]=dxyz[ixy]; 
     }
//     return tcut;
     return true;
}

//---Particle
bool AMSAnalysis::Select_Particle(){

//---Load Status
   nparticle=pev->nParticle();
   nbeta=pev->nBeta();
   ncharge=pev->nCharge();
   ntrack=pev->nTrTrack();
   ntrrawcl=pev->nTrRawCluster();
   ntrcl=pev->nTrCluster();
   ntrhit=pev->nTrRecHit();
   nshow=pev->nEcalShower();
   necalhit=pev->nEcalHit();
   nrich=pev->nRichRing();
   nrichb=pev->nRichRingB();
   nrichh=pev->nRichHit();
//---
   ntrdtrack=pev->nTrdTrack();
   ntrdseg=pev->nTrdSegment();
   ntrdrawh=pev->nTrdRawHit();
   ntrdcl=pev->nTrdCluster();
   ntofclh=-1;
   anti_nhit=pev->nAntiCluster();

//---PreScale 
#ifdef _GOODTKTOFPAR_
  if(pev->nTrTrack()<1)return false;//require Tk
#endif
//----TkIn Particle
  float tkiqm[3]={0};//L1orL9 +L1L9 
  bool l1hy=0;
  bool l9hy=0;
  bool ngpar=0;
  for(int itr=0;itr<pev->nTrTrack();itr++){
    TrTrackR *trk=pev->pTrTrack(itr);
    l1hy=((trk->GetBitPatternJ()&(1<<0))>0);//L1Hit
    l9hy=((trk->GetBitPatternJ()&(1<<8))>0);//L9Hit
    float ntkq=-1;
#if  defined (_HEINNERPRESCALE_) || defined (_IONINNERPRESCALE_)
    ntkq=trk->GetInnerQ();
    if(ntkq>tkiqm[0]){
       tkiqm[0]=ntkq;//Inner
    }
#endif
    if(!l1hy&&!l9hy)continue;//Both
    if(ntkq<0)ntkq=trk->GetInnerQ();
    if(ntkq>tkiqm[1]){//L1 or L9
       tkiqm[1]=ntkq;
     }
    if(l1hy&&l9hy&&ntkq>tkiqm[2]){//L1&&L9
       tkiqm[2]=ntkq;
    }
#ifdef _SAVENEGSCALE_
    int nhiti=0;
    for(int ilay=0+1;ilay<9-1;ilay++){
       if((trk->GetBitPatternJ()&(1<<ilay))>0)nhiti++;
    }
    if(nhiti>=5&&ntkq>0.6){
      int mfit=trk->iTrTrackPar(1,3,20);//VC+Inner+PGCIEMAT
      float tkrig=(mfit>=0)?trk->GetRigidity(mfit):0;
      float nchis=(mfit>=0)?trk->GetNormChisqY(mfit):9999;
      float obeta=0;
      for(int ibh=0;ibh<pev->nBetaH();ibh++){
        if(pev->pBetaH(ibh)->pTrTrack()==trk)obeta=pev->pBetaH(ibh)->GetBeta();
      }
      if(fabs(obeta)>0.4&&fabs(tkrig)>0.7&&tkrig/obeta<0&&nchis<20)ngpar=1;
    }
#endif
  }

  bool isintkbgpar=0;
//----Default
#if defined (_PRHEPRESCALE3_)
   isintkbgpar=(tkiqm[1]>1.5||tkiqm[2]>0);//L1Inner He,L1InnerL9 Pr
#elif defined (_HEL1L9PRESCALE_)
   isintkbgpar=(tkiqm[2]>1.5);//L1InnerL9 He+Full
#endif
//---P/He/Ion Inner+L1Inner
#if defined (_PRL1PRESCALE_)
   isintkbgpar=(isintkbgpar||tkiqm[1]>0);//L1Inner Pr+He+Ion 
#endif   
#if defined (_HEINNERPRESCALE_)
   isintkbgpar=(isintkbgpar||tkiqm[0]>1.5);//Inner He+Ion
#elif defined (_HEL1PRESCALE_)
   isintkbgpar=(isintkbgpar||tkiqm[1]>1.5);//L1Inner He+Ion
#endif
#if defined (_IONINNERPRESCALE_)
   isintkbgpar=(isintkbgpar||tkiqm[0]>2.5);//Inner Z>2
#elif defined (_IONL1PRESCALE_)
   isintkbgpar=(isintkbgpar||tkiqm[1]>2.5);//L1Inner Z>2
#endif
//----Unbiase
#ifdef _PRHESAVEUNBIASE_
   if(unbiasetag)isintkbgpar=(isintkbgpar||tkiqm[0]>0);//unbiased P+He
#endif
//-----
#ifdef _GOODTKTOFPAR_
  if(!isintkbgpar&&!unbiasetag0)return false;
#else
  bool isl1l9bgpar=Select_L1L9Particle();
  bool isl1l9mcpar=Select_MCL1L9Particle(); 
  if(!isintkbgpar&&!ngpar&&!isl1l9bgpar&&!isl1l9mcpar&&!unbiasetag0)return false;
#endif

//--TOFClusterH   
  if(ntofclh<0){
     TofRecH::BuildTofClusterH();
     ntofclh=pev->nTofClusterH();
   }
 
//--Paritcle
   TofRecH::BuildOpt=0;
   TofRecH::BuildBetaH();
   nbetah=pev->nBetaH();

//---Efficiency Mis-Match Continue to Search(For Z>=1)
  float tksq=0,tofsq=0;
  for(int ibh=0;ibh<nbetah;ibh++){
//---TOF sq
      BetaHR *betah=pev->pBetaH(ibh);
      float ntofsq=0;float ntksq=0;
      for(int ilay=0;ilay<4;ilay++){ 
         float tofql=betah->GetQL(ilay);
         if(tofql>0)ntofsq+=tofql;
      }
      TrTrackR *track=betah->pTrTrack();
      if(track){ntksq=track->GetInnerQ()*track->GetNhitsI();}//InnerQ*InnerNHits
//---First BetaH
     if(ibetah<0){ibetah=ibh;tofsq=ntofsq;}
//---Fist Track
     if(itrtrack<0&&betah->iTrTrack()>=0){//First Track Order
        ibetah=ibh; itrtrack=betah->iTrTrack();
        tofsq=ntofsq;tksq=ntksq;
     }
//--If Second Tracker Max TkQ
     else if(itrtrack>=0&&betah->iTrTrack()>=0&&ntksq>tksq){
        ibetah=ibh; itrtrack=betah->iTrTrack();
        tofsq=ntofsq;tksq=ntksq;
     }
//---If NoTrack Max TofQ
     else if(itrtrack<0&&betah->iTrTrack()<0&&ntofsq>tofsq){//Then Eneryg> Raw
        ibetah=ibh; itrtrack=betah->iTrTrack();
        tofsq=ntofsq;tksq=ntksq;
     }
//---end try
  }

///---Double Check(Recover) For Wrong-Tk
  if(nbetah>1&&itrtrack>=0&&ibetah>=0){
//---Check Good Candidate
     vector<int>ibzhg;
     vector<int>ibzhgex;//BZ L1(XY)||L9(XY)
     vector<int>ibzhgex2;//BZ L1(XY)&&L9(XY)
     vector<int>ibhg;
     vector<int>ibhgex;//L1(XY)||L9(XY)
     vector<int>ibhgex2;//L1(XY)&&L9(XY)
     int fid=0;
     for(int ibh=0;ibh<nbetah;ibh++){
       BetaHR *betah=pev->pBetaH(ibh);
       if(betah->iTrTrack()<0)continue;
       float obeta=betah->GetBeta();
       if(obeta<0.4)continue;
//---InnerNHit Suppress
       int ntk_hitb[2]={0};
       ntk_hitb[0]=betah->pTrTrack()->GetBitPatternXYJ();
       ntk_hitb[1]=betah->pTrTrack()->GetBitPatternJ();
       int nhiti[2]={0};
       for(int ilay=0+1;ilay<9-1;ilay++){
         for(int ixy=0;ixy<2;ixy++){
           if((ntk_hitb[ixy]&(1<<ilay))>0)nhiti[ixy]++;
         }
       }
       if(nhiti[1]<5)continue;//Inner-Not enough
//---Rigidity Suppress Wrong-Tk
       float tkrig=betah->pTrTrack()->GetRigidity();
       if(fabs(tkrig)<0.7)continue;
//--Charge Suppress Wrong-Tk
       float tkiq=betah->pTrTrack()->GetInnerQ(obeta,fid);
       if(tkiq<0.6)continue;
//--Inner Par
       ibhg.push_back(ibh);
       if(tkiq>2.5)ibzhg.push_back(ibh);//Inner Z>=3
//--L1L9 Par
       float tkl1q=0;
       bool l1hxy=((ntk_hitb[1]&(1<<0))>0&&(ntk_hitb[0]&(1<<0))>0);//L1HitXY
       if(l1hxy){
          tkl1q=betah->pTrTrack()->GetLayerJQ(1,obeta,fid);
          l1hxy=(l1hxy&&tkl1q>0.5);
       }
       float tkl9q=0;
       bool l9hxy=((ntk_hitb[1]&(1<<8))>0&&(ntk_hitb[0]&(1<<8))>0);//L9HitXY
       if(l9hxy){
          tkl9q=betah->pTrTrack()->GetLayerJQ(9,obeta,fid);
          l9hxy=(l9hxy&&tkl9q>0.5);
       }
       if(l1hxy||l9hxy) {
          ibhgex.push_back(ibh);
          if(tkiq>2.5&&(tkl1q>1.5||tkl9q>1.5))ibzhgex.push_back(ibh);//L1Inner Z>=3
       }
       if(l1hxy&&l9hxy) {
          ibhgex2.push_back(ibh);
          if(tkiq>1.5&&tkl1q>1.5&&tkl9q>1.5)ibzhgex2.push_back(ibh);//L1InnerL9 Z>=2
       }
//----
     }
     if     (ibzhgex2.size()>=1)ibhg=ibzhgex2;
     else if(ibzhgex.size()>=1) ibhg=ibzhgex;
     else if(ibzhg.size()>=1)   ibhg=ibzhg;
     else if(ibhgex2.size()>=1) ibhg=ibhgex2;
     else if(ibhgex.size()>=1)  ibhg=ibhgex;
//---Compare to Get Best Candidate
     if(ibhg.size()>=1){ibetah=ibhg[0]; itrtrack=pev->pBetaH(ibetah)->iTrTrack();}
     if(ibhg.size()>1){
       float tksq1=0,betam1=0;
       for(int ibhi=0;ibhi<ibhg.size();ibhi++){
          BetaHR *betah=pev->pBetaH(ibhg[ibhi]); 
          float obeta=betah->GetBeta();
          float ntksq1=betah->pTrTrack()->GetInnerQ(obeta,fid);
          if(ntksq1>tksq1){
            ibetah=ibhg[ibhi]; itrtrack=betah->iTrTrack();
            tksq1=ntksq1;betam1=betah->GetBeta();
         }
       }
     }
//----
  }


#ifdef _GOODTKTOFPAR_
  if(ibetah<0||itrtrack<0)return false;
#endif


//---2nd Particle
    Select_2ndParticle();

//--Link Particle
    Select_LinkParticle();


  return true;

}

bool  AMSAnalysis::Select_MCL1L9Particle(){

  if(isreal)return false; 
//--Hit Number
   bool mmlh[9]={0};
   for(int il=0;il<NTKL;il++){
      float coox=(tk_pz[il]-mevcoo[2])*mevdir[0]/mevdir[2]+mevcoo[0];
      float cooy=(tk_pz[il]-mevcoo[2])*mevdir[1]/mevdir[2]+mevcoo[1];
      float disr=coox*coox+cooy*cooy;
      if(il==9-1)mmlh[il]=(fabs(coox)<cirr[il]&&fabs(cooy)<ciry[il]);
      else       mmlh[il]=(sqrt(disr)<cirr[il]&&fabs(cooy)<ciry[il]);
   }
//---
   int mmnhitf=0;
   for(int il=0+1;il<9-1;il++){
      if(mmlh[il]){mmnhitf++;}
   }

   bool mcl1l9tkpar=(mmlh[0]&&mmlh[8]);//L1L9
   float mchcut=1.5;
#ifdef _PRL1PRESCALE_
   mchcut=0.5;
#endif 
#if defined (_PRL1PRESCALE_) || defined (_HEL1PRESCALE_) || defined (_IONL1PRESCALE_)
   if(!mcl1l9tkpar&&(fabs(mch)>mchcut)){//MC Z>2
       mcl1l9tkpar=(mmnhitf>=5&&mmlh[0]); //L1Inner
   }
#endif
  return mcl1l9tkpar;
}


bool AMSAnalysis::Select_L1L9Particle(){
 
 //--
   tk_l9qs=tk_l1qs=tk_l1qvs=tk_l9qvs=tk_l1qvr=tk_l9qvr=0;
   tk_l9q=tk_l1q=0;
   tk_l9qxy[0]=tk_l9qxy[1]=tk_l1qxy[0]=tk_l1qxy[1]=0;
   AMSPoint postr;AMSDir dirtr;
   for(int iyh=0;iyh<2;iyh++){tk_exqvn[iyh][0]=tk_exqvn[iyh][1]=0;}
//---Pre Scale
   if(ntrdtrack<1&&ntrack<1){return false;}

//----InnerTk Interpolate L1 L9 Geometry
  float tkiqm[3]={0};
  for(int itr=0;itr<pev->nTrTrack();itr++){
    bool tkl1g=0,tkl9g=0;
    for(int ilay=0;ilay<NTKL;ilay++){//L1Pos+L9Pos Geometry
        if(ilay!=0&&ilay!=NTKL-1)continue;
        pev->pTrTrack(itr)->InterpolateLayerJ(ilay+1,postr,dirtr);//L1+L9
        float disr=sqrt(postr[0]*postr[0]+postr[1]*postr[1]);
        if(ilay==0)tkl1g=(disr          <cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L1
        else       tkl9g=(fabs(postr[0])<cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L9
     }
     if(!tkl1g&&!tkl9g)continue;
     float ntkq=pev->pTrTrack(itr)->GetInnerQ();
     if(ntkq>tkiqm[1])tkiqm[1]=ntkq;
     if(tkl1g&&tkl9g&&ntkq>tkiqm[2])tkiqm[2]=ntkq;
  }

///---Ecal Events
  float ecalen=0;
  for(int ish=0;ish<pev->nEcalShower();ish++){
     EcalShowerR *show=pev->pEcalShower(ish);
     if(show->EnergyD>ecalen)ecalen=show->EnergyD; 
  }
   bool ecalpar=(ecalen/1000.>0.6);

   bool l1l9tkpar=0;//L1Inner
//------Default
#if defined (_PRHEPRESCALE3_)
   l1l9tkpar=(tkiqm[1]>1.5||tkiqm[2]>0);//L1Inner He,L1InnerL9 Pr
#elif defined (_HEL1L9PRESCALE_)
   l1l9tkpar=(tkiqm[2]>1.5);//L1InnerL9 He
#endif
//------L1Inner
#ifdef _PRL1PRESCALE_
   l1l9tkpar=(l1l9tkpar||tkiqm[1]>0);//L1Inner Pr
#endif
#ifdef _HEL1PRESCALE_
   l1l9tkpar=(l1l9tkpar||tkiqm[1]>1.5);//L1Inner He
#endif
#ifdef _IONL1PRESCALE_
   l1l9tkpar=(l1l9tkpar||tkiqm[1]>2.5);//L1Inner Z>2
#endif
  if(unbiasetag){
    l1l9tkpar=(tkiqm[1]>1.5||tkiqm[2]>0);//L1Inner He,L1InnerL9 Pr
//#ifdef _HEINNERPRESCALE_
    l1l9tkpar=(l1l9tkpar||tkiqm[1]>0);
//#endif
  }

//----TrdInterpolate L1 L9 Geometry
  int l1l9trdpar=0; 
  for(int itr=0;itr<pev->nTrdTrack();itr++){
    bool trdl1g=0,trdl9g=0;
    for(int ilay=0;ilay<NTKL;ilay++){//L1Pos+L9Pos Geometry
        if(ilay!=0&&ilay!=NTKL-1)continue;
        pev->pTrdTrack(itr)->Interpolate(tk_pz[ilay],postr,dirtr);//L1+L9
        float disr=sqrt(postr[0]*postr[0]+postr[1]*postr[1]);
        if(ilay==0)trdl1g=(disr          <cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L1
        else       trdl9g=(fabs(postr[0])<cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L9
     }
     if(trdl1g&&trdl9g){l1l9trdpar=2;break;}//L1InnerL9
     else if(trdl1g)   {l1l9trdpar=1;}//L1Inner
  }

//---PreScale
   bool tktrdscal=0;
#if defined (_HEL1L9PRESCALE_) || defined (_PRHEPRESCALE3_)
   tktrdscal=(l1l9tkpar||l1l9trdpar>=2);//Track|TRDL1InnerL9G
#endif
#if defined (_PRL1PRESCALE_) || defined (_HEL1PRESCALE_) || defined (_IONL1PRESCALE_)
   tktrdscal=(l1l9tkpar||l1l9trdpar>=1);//Track|TRDL1InnerG
#endif
  if(unbiasetag)tktrdscal=(l1l9tkpar||l1l9trdpar>=1);//Track|TRDL1InnerG
  if(!tktrdscal)return false;

//------
   float ubeta=1,urig=0,umass=0,udxdz=0,udydz=0;
   int umul=0; 
//---L19Hit
   for(int itkh=0;itkh<pev->nTrRecHit();itkh++){//MaxQ
     TrRecHitR* tkhit=pev->pTrRecHit(itkh);
     int layj=tkhit->GetLayerJ();
     if(layj==1||layj==9){//have to exclude noisy strip
       float nlqy=tkhit->GetQ(1,ubeta,urig,umass,umul,udxdz,udydz);//YQ
       float nlqx=tkhit->GetQ(0,ubeta,urig,umass,umul,udxdz,udydz);//XQ
       float nlq =tkhit->GetQ(2,ubeta,urig,umass,umul,udxdz,udydz);
       int   nqst=tkhit->GetQStatus();//have to define multiplicity here
       float &l19qy= (layj==1)? tk_l1qxy[1]: tk_l9qxy[1];//L19Y
       float &l19qx= (layj==1)? tk_l1qxy[0]: tk_l9qxy[0];//L19X
       float &l19q=  (layj==1)? tk_l1q:      tk_l9q;
       int   &l19qs= (layj==1)? tk_l1qs:     tk_l9qs;
       int   &l19i = (layj==1)? l1i:         l9i;//L19index
       if ((nlqy>1.2*l19qy)||(nlqy>0.99*l19qy&&nlqx>1.05*l19qx)){l19i=itkh;l19qy=nlqy;l19qx=nlqx;l19q=nlq;l19qs=nqst;}
     }
   }


  bool tkl1scal=0;
#ifdef _HEL1L9PRESCALE_ 
  tkl1scal=(l1i>=0&&tk_l1q>1.5&&l1l9trdpar>=2);
#endif
#ifdef _PRHEPRESCALE3_
  tkl1scal=(l1i>=0&&tk_l1q>1.5&&l1l9trdpar>=2);
  tkl1scal=(tkl1scal||(l1i>=0&&tk_l1q>0.6&&l1l9trdpar>=2&&ecalpar));//ECAL Events
#endif
//---------
#ifdef _PRL1PRESCALE_
  tkl1scal=(tkl1scal||(l1i>=0&&tk_l1q>0.5&&l1l9trdpar>=1));//PrL1InnerG
#endif
#ifdef _HEL1PRESCALE_
  tkl1scal=(tkl1scal||(l1i>=0&&tk_l1q>1.5&&l1l9trdpar>=1));//HeL1InnerG
#endif
#ifdef _IONL1PRESCALE_
  tkl1scal=(tkl1scal||(l1i>=0&&tk_l1q>2.5&&l1l9trdpar>=1));//Z>2 L1InnerG
#endif
  if(unbiasetag){tkl1scal=(tkl1scal||(l1i>=0&&tk_l1q>1.5&&l1l9trdpar>=1)||(l1i>=0&&tk_l1q>0.6&&l1l9trdpar>=2));}//(HeL1InnerG || PrL1InnerL9G)
  if(!tkl1scal)return l1l9tkpar;

//----Build TrdParticle
//   TofRecH::BuildOpt=31000;
   TofRecH::BuildOpt=1000;//TrdParticle
   TofRecH::ReBuild();

//-----BuildTofClusterH
   ntofclh=pev->nTofClusterH();
   int nbetahs=pev->nBetaH();

//--Select TrdBetaH
   float tofsqs=0;
   for(int ibh=0;ibh<nbetahs;ibh++){
      BetaHR *betah=pev->pBetaH(ibh);
      if(betah->iTrdTrack()<0)continue; 
      float ntofsq=0;
      for(int ilay=0;ilay<4;ilay++){
         float tofql=betah->GetQL(ilay);
         if(tofql>0)ntofsq+=tofql;
       }
       if(ntofsq>tofsqs){
        ibetahs=ibh;
        tofsqs=ntofsq;
      }
   }
     if(ibetahs<0){return l1l9tkpar;}
     BetaHR *betah=pev->pBetaH(ibetahs);
     itrdtracks=betah->iTrdTrack();
     tof_nhits=betah->GetSumHit();
     tof_hsumhus=betah->GetUseHit();
     TofBetaPar normbetapar=betah->GetNormTofBetaPar();
     tof_chists=normbetapar.Chi2T;
     tof_chiscs=normbetapar.Chi2C;
     betahs=betah->GetBeta();
     float betahsc=betahs;
     betahsc=betahsmc=(!isreal)?betah->GetMCBeta(1):betahs;
     float tofq=0; int ntofq=0;
     for(int ilay=0;ilay<4;ilay++){
        if(!isreal)tof_qlsb[ilay]=betah->GetQL(ilay,2,TofClusterHR::DefaultQOpt,111111,betahs,0);
        tof_qls[ilay]=betah->GetQL(ilay,2,TofClusterHR::DefaultQOpt,111111,betahsc,0);//beta correction only
        if(tof_qls[ilay]>0){tofq+=tof_qls[ilay]; ntofq++;}
     }
     if(ntofq>=1)tofq/=ntofq;

//---Ecal Match
     double ecalz=-143; double utm;
     betah->TInterpolate(ecalz,postr,dirtr,utm);
     for(int ic=0;ic<3;ic++){ecal_pos[ic]=postr[ic];}
     ecal_ens=-1000; ecal_dis=1000;
     for(int ish=0;ish<pev->nEcalShower();ish++){
        EcalShowerR *show=pev->pEcalShower(ish);
        betah->TInterpolate(show->Entry[2],postr,dirtr,utm);
        double necal_dis=pow(show->Entry[0]-postr[0],2)+pow(show->Entry[1]-postr[1],2);
        necal_dis=sqrt(necal_dis);
        if(necal_dis<ecal_dis){
           ecal_dis=necal_dis;
           ecal_ens=show->EnergyD;
        }
     }

//--L1L9Hit X+Y Match + L1L9Q
    tk_l1mds=tk_l9mds=1000;
    for(int uexl=0;uexl<2;uexl++){
       int ilay=(uexl==0)?0:8; 
       int iexl=(uexl==0)?l1i:l9i;
       float qex=(uexl==0)?tk_l1q:tk_l9q;
       if(iexl<0)continue; 
       TrRecHitR* tkhit=pev->pTrRecHit(iexl);
       float mexdis=1000;
       int resmul=-1;
       betah->TInterpolate(tk_pz[ilay],postr,dirtr,utm);
       TkSens sens(tkhit->GetTkId(),postr,dirtr,0);
       AMSDir sdir = sens.GetSensDir();
       float resdxdz = (sdir.z() != 0) ? sdir.x()/sdir.z() : 0;
       float resdydz = (sdir.z() != 0) ? sdir.y()/sdir.z() : 0;
       if(tkhit->GetXCluster()&&tkhit->GetYCluster()){//Only X+Y Cluster, Positon
          for(int imul=0;imul<tkhit->GetMultiplicity();imul++){
             AMSPoint mpos=tkhit->GetCoord(imul);
             double pexdis=(mpos[0]-postr[0])*(mpos[0]-postr[0])+(mpos[1]-postr[1])*(mpos[1]-postr[1]);
             pexdis=sqrt(pexdis);
             if(pexdis<mexdis){mexdis=pexdis;resmul=imul;}
          }
       }
//---Check Proton+Helium
       float cutexdis=10;//10cm
       if(mexdis>cutexdis&&qex<2.5&&tofq<2.5){//Low amplitude Refind L1|L9 hit
         float qexn=0,exprobn=0;
         for(int itkh=0;itkh<pev->nTrRecHit();itkh++){
           TrRecHitR* tkhit1=pev->pTrRecHit(itkh);
           if(tkhit1->GetLayerJ()!=ilay+1||tkhit1->GetYCluster()==0||tkhit1->GetXCluster()==0||itkh==iexl)continue;
           float mexdisn=1000;
           int resmuln=-1;
           for(int imul=0;imul<tkhit1->GetMultiplicity();imul++){
             AMSPoint mpos=tkhit1->GetCoord(imul);
             double pexdis=(mpos[0]-postr[0])*(mpos[0]-postr[0])+(mpos[1]-postr[1])*(mpos[1]-postr[1]);
             pexdis=sqrt(pexdis);
             if(pexdis<mexdisn){mexdisn=pexdis;resmuln=imul;}
           }
           if(mexdisn<cutexdis){
             float nlq=tkhit1->GetQ(1,ubeta,urig,umass,umul,udxdz,udydz);//MaxY
             float nlp=tkhit1->GetProb();//X
             bool qmatch=(tofq>1.5)? (nlq>1.5): (nlq>0.5);//charge match
             if     (qmatch&&(nlq>1.1*qexn))                {tkhit=tkhit1;mexdis=mexdisn;resmul=resmuln;qexn=nlq;exprobn=nlp;}
             else if(qmatch&&(nlq>0.99*qexn)&&(nlp>exprobn)){tkhit=tkhit1;mexdis=mexdisn;resmul=resmuln;qexn=nlq;exprobn=nlp;}
           }
         }
       }
//---
       if(resmul<0)resmul=0;
       if(uexl==0){
         tk_l1mds=mexdis;
         tk_l1qvr=tkhit->GetQ(2,betahs,urig,umass,umul,udxdz,udydz);
         tk_l1qvs=tkhit->GetQ(2,betahs,urig,umass,resmul,resdxdz,resdydz);
       }
       else  {
         tk_l9mds=mexdis;
         tk_l9qvr=tkhit->GetQ(2,betahs,urig,umass,umul,udxdz,udydz);
         tk_l9qvs=tkhit->GetQ(2,betahs,urig,umass,resmul,resdxdz,resdydz);
       }
       if((uexl==0&&tk_l1qvs>1.5)||(uexl==1&&tk_l9qvs>1.5)){
         tk_exqvn[0][uexl]=tkhit->GetQYJ(2,betahsc,urig,resmul,resdxdz,resdydz);
         tk_exqvn[1][uexl]=tkhit->GetQH(2,betahsc,urig,resmul,resdxdz,resdydz);
       }
    }

//---Interpolate TkL
     int nhiti=0;
     bool trdl1l9g[2]={0};
     for(int ilay=0;ilay<NTKL;ilay++){
       betah->TInterpolate(tk_pz[ilay],postr,dirtr,utm);
       for(int ic=0;ic<3;ic++){tk_pos1s[ilay][ic]=postr[ic];if(ilay==0)tk_dir1s[ic]=dirtr[ic];}
       float disr=sqrt(postr[0]*postr[0]+postr[1]*postr[1]);
       if(ilay!=0&&ilay!=NTKL-1){  
          if(disr<cirr[ilay]&&fabs(postr[1])<ciry[ilay])nhiti++;
       }
       else { 
          if(ilay==0)trdl1l9g[0]=(disr          <cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L1
          else       trdl1l9g[1]=(fabs(postr[0])<cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L9
       }
     }

//---TrdTrack
     TrdTrackR *trdtracks=pev->pTrdTrack(itrdtracks);
     if(trdtracks){
       trd_thetas=trdtracks->Theta;
       trd_phis=trdtracks->Phi;
       for(int ic=0;ic<3;ic++)trd_coos[ic]=trdtracks->Coo[ic];
       trdtracks->ComputeCharge(1);
       trd_rqs=trdtracks->Q;
    }

//--TrdParticle Cutoff
     if(isreal){
       double cfp=0;
       AMSDir dirp(tk_dir1s[0],tk_dir1s[1],tk_dir1s[2]);
       if(dirp[2]>0)dirp=dirp*(-1);
       if(betahs <0)dirp=dirp*(-1);
       pev->GetStoermerCutoff(cfp,1,dirp);
       cutoffps=cfp;//positive charge cutoff
     }
     else cutoffps=mmom/mch;

    bool l1trdpar=0;
#ifdef _HEL1L9PRESCALE_
     l1trdpar=(tk_l1qvs>1.5&&tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5)&&(tof_qls[2]>1.5||tof_qls[3]>1.5)&&trdl1l9g[1]);//L1IL9-He
#endif
#ifdef _PRHEPRESCALE3_
     l1trdpar=(tk_l1qvs>1.5&&tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5)&&(tof_qls[2]>1.5||tof_qls[3]>1.5)&&trdl1l9g[1]);//L1IL9-He
     l1trdpar=(l1trdpar||(tk_l1qvs>0.6&&tk_l1mds<10&&(tof_qls[0]>0.6||tof_qls[1]>0.6)&&(tof_qls[2]>0.6||tof_qls[3]>0.6)&&trdl1l9g[1]&&ecalpar));//L1IL9E-Pr
#endif
#ifdef _PRL1PRESCALE_
     l1trdpar=(l1trdpar||(tk_l1qvs>0.5&&tk_l1mds<10&&(tof_qls[0]>0.5&&tof_qls[1]>0.5)&&(tof_qls[2]>0.5&&tof_qls[3]>0.5)));//L1I-Pr
#endif
#ifdef _HEL1PRESCALE_
     l1trdpar=(l1trdpar||(tk_l1qvs>1.5&&tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5)&&(tof_qls[2]>1.5||tof_qls[3]>1.5)));//L1I-He
#endif
#ifdef _IONL1PRESCALE_
     l1trdpar=(l1trdpar||(tk_l1qvs>2.5&&tk_l1mds<10&&(tof_qls[0]>2.5||tof_qls[1]>2.5)&&(tof_qls[2]>2.5||tof_qls[3]>2.5)));//L1I-Ion
#endif
     if(unbiasetag){
       bool l1trdparbz=(tk_l1qvs>1.5&&tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5)&&(tof_qls[2]>1.5||tof_qls[3]>1.5));//L1I-He
       bool l1trdparpr=(tk_l1qvs>0.6&&tk_l1mds<10&&(tof_qls[0]>0.6||tof_qls[1]>0.6)&&(tof_qls[2]>0.6||tof_qls[3]>0.6)&&trdl1l9g[1]); //L1IL9-Pr
       l1trdpar=(l1trdpar||l1trdparbz||l1trdparpr);
     }
     if(!l1trdpar){return l1l9tkpar;}

     return true;
}


bool AMSAnalysis::Select_2ndParticle(){

//---Secondary TrTrack
      betah2r=0;
      betah2p=0;
      betah2q=0;
      betah2hb[0]=betah2hb[1]=0;
      betah2ch=10000;

//---1nd Not Exist
     if(ibetah<0||itrtrack<0||nbetah<=1)return false;
     BetaHR *betah=pev->pBetaH(ibetah);
     float sbeta=betah->GetBeta();

//---Require Tof+Tracker
     for(int ibh=0;ibh<nbetah;ibh++){
         if(ibh==ibetah)continue;//not selected
         BetaHR *betah2=pev->pBetaH(ibh);
         if(betah2->iTrTrack()<0||betah2->iTrTrack()==itrtrack)continue;//second Tracker
//---Non First BetaH Matched TofClusterH 
         int nclmatch=0;
         for(int ilay=0;ilay<NTOFL;ilay++){
           if(!betah2->TestExistHL(ilay))continue;
           if(betah2->GetClusterHL(ilay)==betah->GetClusterHL(ilay))continue;
           nclmatch++;
         }
//---Rigidity+HitB
         float nrig= betah2->pTrTrack()->GetRigidity();
         float nchis=betah2->pTrTrack()->GetChisq();
         float ntkq= betah2->pTrTrack()->GetInnerQ();
         int ntkhb[2];
         ntkhb[0]=betah2->pTrTrack()->GetBitPatternXYJ();
         ntkhb[1]=betah2->pTrTrack()->GetBitPatternJ();
//----
         int nhit2i[2][2]={0};
         for(int ilay=0+1;ilay<9-1;ilay++){
             for(int ixy=0;ixy<2;ixy++){
               if((betah2hb[ixy]&(1<<ilay))>0)nhit2i[0][ixy]++;
               if((ntkhb[ixy]&(1<<ilay))>0)nhit2i[1][ixy]++;
             }
         }
         bool goodtkpr=(nhit2i[0][0]>=3&&nhit2i[0][1]>=5&&betah2r/sbeta>0);
         bool goodtknw=(nhit2i[1][0]>=3&&nhit2i[1][1]>=5&&nrig/sbeta>0);
//-------
         int fw=0;
//----Good TkNW
         if(goodtknw){
            if     (!goodtkpr)               {fw=1;}//tk-first now found
            else if(fabs(nrig)>fabs(betah2r)){fw=1;}//already found Largest Rig
         }
         else if(!goodtkpr){
            if     (nclmatch>betah2p)               {fw=1;}
            else if(nclmatch==betah2p&&ntkq>betah2q){fw=1;}
         }
         if(fw!=1)continue;
         betah2r=nrig;
         betah2p=nclmatch;
         betah2q=ntkq;
         betah2hb[0]=ntkhb[0]; betah2hb[1]=ntkhb[1];
         betah2ch=nchis;
//---
     }
     
    if(betah2q==0)return false;
    return true;

}


bool AMSAnalysis::Select_LinkParticle(){

//--Particle index    
   if(ibetah>=0&&itrtrack>=0){
     for(int ipar=0;ipar<nparticle;ipar++){
       if(pev->pParticle(ipar)->iTrTrack()==itrtrack){iparindex=ipar;break;}
     }
   }
  else if(ibetah<0&&itrtrack<0){
     float tksq=0;
     for(int itr=0;itr<ntrack;itr++){
        TrTrackR *trk=pev->pTrTrack(itr);
        float ntksq=trk->GetQ();
        if(ntksq>tksq){itrtrack=itr;tksq=ntksq;}
     }
   }

//-----index 
   if(iparindex>=0){
     btstat=pev->pParticle(iparindex)->BT_status;
     ibeta=pev->pParticle(iparindex)->iBeta();
     itrdtrack=pev->pParticle(iparindex)->iTrdTrack();
     irich=pev->pParticle(iparindex)->iRichRing();
     irichb=pev->pParticle(iparindex)->iRichRingB();
     icharge=pev->pParticle(iparindex)->iCharge();
//-----
     trd_pass=(pev->pParticle(iparindex)->IsInsideTRD())?1:0;
  }
  else if(itrtrack>=0){
    for(int ir=0;ir<nrich;ir++){
      if(pev->pRichRing(ir)->iTrTrack()==itrtrack){irich=ir;break;}
    }
    for(int irb=0;irb<nrichb;irb++){
       if(pev->pRichRingB(irb)->iTrTrack()==itrtrack){irichb=irb;break;}
    }
    for(int ib=0;ib<nbeta;ib++){
       if(pev->pBeta(ib)->iTrTrack()==itrtrack){ibeta=ib;break;}
    }
    for(int ich=0;ich<ncharge;ich++){
      if(pev->pCharge(ich)->iBetaH()==ibetah){icharge=ich;break;}
    }
  }

  show_dis=1000;
  if(ibetah>=0){
//----Ecal Match 
    BetaHR *betah=pev->pBetaH(ibetah);
    double ecalz=-143; double utm;
    AMSPoint postr;AMSDir dirtr;
    betah->TInterpolate(ecalz,postr,dirtr,utm);
    for(int ic=0;ic<3;ic++){ec_pos[ic]=postr[ic];ec_dir[ic]=dirtr[ic];}
    for(int ish=0;ish<pev->nEcalShower();ish++){
        EcalShowerR *show=pev->pEcalShower(ish);
        betah->TInterpolate(show->Entry[2],postr,dirtr,utm);
        double necal_dis=pow(show->Entry[0]-postr[0],2)+pow(show->Entry[1]-postr[1],2);
        necal_dis=sqrt(necal_dis);
        if(necal_dis<show_dis){
           show_dis=necal_dis;
           ishow=ish; 
        }
    }
//---Particle Cutoff
    if(isreal){
      double cfp=0;
      float obeta=betah->GetBeta();
      betah->TInterpolate(tk_pz[obeta>0?0:8],postr,dirtr,utm);//L1||L8 direction
      if(dirtr[2]>0)dirtr=dirtr*(-1);
      if(obeta<0)dirtr=dirtr*(-1);
      for(int ipn=0;ipn<2;ipn++){
          int sign=(ipn==0)?-1:1;//negtive(postive) charge
          pev->GetStoermerCutoff(cfp,sign,dirtr);
          cutoffp[ipn]=cfp;
       }
     }
     else cutoffp[0]=cutoffp[1]=mmom/mch; 
 } 
 

  return true; 
 
}

//---Init Trk ReFit
int AMSAnalysis::InitTrkReFit(float tkiq,float tkfz,int &refit,float &mass,int &chrg,int algo){

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
#if defined (_USENOLINEARCOR_)
       TrClusterR::UnsetLinearityCorrection();//No Linearity Correction 
#endif
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
bool AMSAnalysis::Select_Trk(int sel){
    int mfit=-1;
    TrTrackR *trk=pev->pTrTrack(itrtrack);
    if(!trk)return false;
    float umass=0; int fid=0;
    BetaHR *betah=pev->pBetaH(ibetah);
    float obeta=betah?betah->GetBeta():1;
    float frig=trk->GetRigidity(fid);
//-------
    float obetac=(!isreal&&betah)?betah->GetMCBeta(1):obeta;

//---Charge
   if(sel==0){

//---New charge All 
     for(int isp=0;isp<2;isp++){
//--Float Z
        mean_t tkqa;
        if(isp==0)tkqa=trk->GetQ_all(obeta,fid);
        else      {tk_qr=trk->GetInnerQ(); tkqa=trk->GetInnerQ_all(obeta,fid);}
        tk_q[isp]   =tkqa.Mean;
        tk_qrms[isp]=tkqa.RMS;
        tk_qhit[isp]=tkqa.NPoints;
//---TkZ
       vector<like_t> likelihood;
       if(isp==0)trk->GetZ(likelihood,obeta,fid);
       else      trk->GetInnerZ(likelihood,obeta,fid);
       int nz=likelihood.size()>=2?2:likelihood.size();
       for(int ip=0;ip<nz;ip++){//Max + Second Max
        tk_rz[ip][isp]=likelihood[ip].Z;
        tk_rpz[ip][isp]=likelihood[ip].GetNormProb();
      }
//----YJ
       if(isp==1){
         for(int ixy=0;ixy<3;ixy++){
           if(tk_q[1]<=1.5)continue;
           TrQYJTrack tkqyj=trk->GetQYJ_all(ixy,obetac,fid);
           tk_qin[0][ixy]=tkqyj.InnerQ;
           tk_qrmn[0][ixy]=tkqyj.InnerQRMS;
           mean_t tkqha=trk->GetInnerQH_all(ixy,obetac,fid);
           tk_qin[1][ixy]=tkqha.Mean;
           tk_qrmn[1][ixy]=tkqha.RMS;
//           cout<<"ixy="<<ixy<<" innerQ="<<tk_q[isp]<<","<<tk_qin[0][ixy]<<","<<tk_qin[1][ixy]<<endl;
         }
       }
     }

//---New Charge Layer     
     for(int il=0;il<9;il++){
       tk_lid[il]=0;
       tk_qls[il]=0;
       if(il==0)tk_qlsn=0;
       tk_ql2[il][0]=tk_ql2[il][1]=0;
       tk_ql[il]=trk->GetLayerJQ(il+1,obeta,fid);
       for(int ixy=0;ixy<3;ixy++){
         tk_qln[0][il][ixy]=tk_qln[1][il][ixy]=0;
         if(tk_q[1]<=1.5)continue;
         TrQYJHit tkyjhit=trk->GetLayerQYJ_all(il+1,ixy,obetac,fid);
         tk_qln[0][il][ixy]=tkyjhit.Q;
         tk_qln[1][il][ixy]=trk->GetLayerJQH(il+1,ixy,obetac,fid);
         if(ixy>=2)continue;
         for(int is=0;is<5;is++)tk_qlqs[il][ixy][is]=tkyjhit.qcl[ixy]._qs[is];
       }
       TrRecHitR* tkhit =trk->GetHitLJ(il+1);
       if(!tkhit)continue;
       tk_lid[il]=tkhit->GetTkId();
       tk_qls[il]=tkhit->GetQStatus();
       if(il==0)tk_qlsn=tkhit->GetQStatusYJ();
       tk_ql2[il][0]=tkhit->GetQ(0,obeta,frig);//XQ
       tk_ql2[il][1]=tkhit->GetQ(1,obeta,frig);//YQ
     }
    

//---Tk quality
    int algo=1;// 1:Choutko 2:Alcaraz
    int patt=0;// 3:Inner 5:L1I 6:IL9 7:L1IL9
    int refit=3;
    float mass=TrFit::Mproton;
    int chrg=1;

//---Hit Pattern
    tk_rtype=trk->GetRecType();
    tk_hitb[0]=trk->GetBitPatternXYJ(); tk_hitb[1]=trk->GetBitPatternJ();
    for(int ilay=0+1;ilay<9-1;ilay++){
      for(int ixy=0;ixy<2;ixy++){
        if((tk_hitb[ixy]&(1<<ilay))>0)tk_nhiti[ixy]++;
      }
    }
    tk_fzm[0]=trk->GetAdvancedFitCharge(); tk_fzm[1]=trk->GetAdvancedFitMass();
    float tkfz=tk_fzm[0];
    if(ibetah<0){tkfz=-1;}//ibetah=-1 reassigned tkfz

//---
//    int uyh=-1;
    int uyh=0;
    tk_iq=tk_q[1];
    tk_1q=tk_ql[0];
    tk_9q=tk_ql[8];
/*    if(uyh>=0){
      tk_iq=tk_qin[uyh][2];
      tk_1q=tk_qln[uyh][0][2];
      tk_9q=tk_qln[uyh][8][2];
    }*/
    if(uyh>=0&&tk_qin[uyh][2]>8.5){
      tk_iq=tk_qin[uyh][2];
      tk_1q=tk_qln[uyh][0][2];
      tk_9q=tk_qln[uyh][8][2];
    }
    tk_l1l9par  =(tk_iq>0&&tk_1q>0&&tk_9q>0&&(tk_hitb[0]&(1<<0))>0&&(tk_hitb[0]&(1<<8))>0);
    tk_bgpar    =(tk_iq>1.5);
    tk_bhgpar   =(tk_iq>2.5);
    tk_ubpar    =(tk_iq>0&&unbiasetag>0);
    bool tk_mcz1=(tk_iq>0&&!isreal&&fabs(mch)==1);

//---Algo
    for(int ialgo=0;ialgo<3;ialgo++){
      if     (ialgo==0)algo=1;//Choutko
      else if(ialgo==1)algo=2;//Alcaraz
//      else if(ialgo==2)algo=13;//ChikanianF //without energy loss
      else if(ialgo==2)algo=6;//Kalman
//---Init Track ReFit
      InitTrkReFit(tk_iq,tkfz,refit,mass,chrg,algo);
//---
      for(int ipgc=0;ipgc<3;ipgc++){
        if     (ipgc==0)refit=refit%10;//Ex-PG
        else if(ipgc==1)refit=10+refit%10;//Ex-CIEMAT
        else if(ipgc==2)refit=20+refit%10;//PG+CIEMAT
//--
        for(int isp=0;isp<=6;isp++){
          if     (isp==0)patt=0;//Full Span
          else if(isp==1)patt=3;//Inner
          else if(isp==2)patt=5;//Inner+Layer1
          else if(isp==3)patt=6;//Inner+Layer9
          else if(isp==4)patt=7;//Force Full Span(change to 4)
          else if(isp==5)patt=1;//Inner Up
          else if(isp==6)patt=2;//Inner Down
//--Rigidity
          TrFit trfit;
          if(refit%10>=1&&(algo%10==3||algo%10==6)){//Chikanian/Kalman scale
            mfit=-2;
#ifndef _USEFAST_
            if(ipgc==2&&(tk_bgpar||tk_mcz1||tk_ubpar))mfit=-1;
#ifdef _SAVENEGSCALE_
            if(ipgc==2&&tk_ngpar)mfit=-1;
#endif
#endif
            if(algo%10==6&&(patt==1||patt==2))mfit=-2;
            if(mfit>=-1)mfit=trk->iTrTrackPar(trfit,algo,patt,refit,mass,chrg);
          }
          else mfit=trk->iTrTrackPar(trfit,algo,patt,refit,mass,chrg);
//--clear
          float nrig=0,nrigp[5]={0};
          float enrig=9999;
          float nchis[3]={9999,9999,9999};
          float npval[3]={-1,-1,-1};
          float ndir0[3]={0,0,-1};
          float ntkres[9][3];
          float dpgc[9][3];//CIMAT-PG-Diff
          for(int il=0;il<9;il++)
            for(int ixy=0;ixy<3;ixy++){ntkres[il][ixy]=9999;dpgc[il][ixy]=9999;}
//--
          if(mfit>=0){
            for(int iz=-1;iz<4;iz++){
              if(isreal)nrigp[iz+1]=trk->GetCorrectedRigidity(mfit,3,iz);
              else      nrigp[iz+1]=trk->GetRigidity(mfit,iz);
            }
            int izf=-1;
            nrig=nrigp[izf+1];//default
            if(tk_tcor==0)tk_tcor=1./trk->GetCorrectedRigidity(mfit,3,izf)-1./trk->GetRigidity(mfit,izf);
            float bcor=trk->GetBcorr(mfit);//positive
//---Add Magnetic correction
            if(isreal&&bcor==1){//Not applied
              float bcor1=1,bcor2=1;
              int bret1= MagnetVarp::btempcor(bcor1, 0, 1);
              int bret2= MagnetVarp::btempcor(bcor2, 0, 2);
              if      (bret1==0 && bret2==0)bcor=(bcor1+bcor2)/2;
              else if (bret1!=0 && bret2==0)bcor=bcor2;
              nrig*=bcor;
              bcor=-bcor;//put negtive in this case
            }
            if(tk_bcor==0)tk_bcor=bcor;
//----
            enrig=trk->GetErrRinv(mfit);
            nchis[0]=trk->GetNormChisqX(mfit);
            nchis[1]=trk->GetNormChisqY(mfit);
            nchis[2]=trk->GetChisq(mfit);
            for(int ixy=0;ixy<3;ixy++){npval[ixy]=trk->GetPVal(ixy,mfit);ndir0[ixy]=trk->GetDir(mfit)[ixy];}
            if((ialgo==0)&&(ipgc==2)&&(isp<=4)){if(ibetah>=0&&fabs(obeta)>0.4&&tk_nhiti[1]>=5&&tk_iq>0.6&&fabs(nrig)>0.7&&nrig/obeta<0&&nchis[1]<20)tk_ngpar=1;}
            for(int il=0;il<NTKL;il++){
              AMSPoint pnt=trk->GetResidualJ(il+1,mfit);
              ntkres[il][0]=pnt[0];
              ntkres[il][1]=pnt[1];
              if((ialgo==0)&&(ipgc==2)){for(int ixy=0;ixy<3;ixy++)dpgc[il][ixy]=(trk->GetPG_CIEMAT_diff(il+1))[ixy];}
              pnt=(trk->GetHitCooLJ(il+1,0)+trk->GetHitCooLJ(il+1,1))/2.;//PG+CIEMAT
              if((ialgo==0)&&(ipgc==2)&&(isp==0)){for(int ixy=0;ixy<3;ixy++)tk_hitc[il][ixy]=pnt[ixy];}
              TrRecHitR *tkhit=trk->GetHitLJ(il+1);
              if(tkhit)pnt=tkhit->GetLocalCoordinate(tkhit->GetResolvedMultiplicity());
              else     pnt.setp(0,0,0);
              if((ialgo==0)&&(ipgc==2)&&(isp==0)){for(int ixy=0;ixy<2;ixy++)tk_hitcl[il][ixy]=pnt[ixy];}
             }
#ifdef _USEKALMANFIT_
            const int nzl=4;
            double pzl[nzl]={195.00, 158.92, 53.06,-69.975};
//            double pzl[nzl]={195.00, 158.92, 53.06,-135.882};
            for(int iz=0;iz<nzl;iz++){
              if(ipgc!=2)break;
              AMSPoint postr;AMSDir dirtr;
              double rigtr=nrig;
              double zl=pzl[iz];
              if     (iz==0&&!isreal)zl=mevcoo[2];//TOI
              else if(iz==1&&!isreal)zl=mevcoo1[0][2];//L1
              else if(iz==2&&!isreal)zl=mevcoo1[7][2];//L2
              else if(iz==3&&!isreal)zl=mevcoo1[17][2];//LTOF and RICH
//              else if(iz==3&&!isreal)zl=mevcoo1[18][2];//L9
              if(zl==-1000)zl=pzl[iz];
              if(algo%10==6){
                trfit.InterpolateKalman(zl,postr,dirtr,rigtr);
                if(isreal)rigtr=pev->GetCorrectedRigidity(rigtr,0,3);//rigscale correction
              }
              else trk->Interpolate(zl,postr,dirtr,mfit);
              tk_rigidityr[iz][ialgo][isp]=rigtr;
              for(int ic=0;ic<3;ic++){tk_posr[iz][ialgo][isp][ic]=postr[ic];tk_dirr[iz][ialgo][isp][ic]=dirtr[ic];}
            }
#endif
          }
//--Select Copy To Data
          tk_rigidity1[ialgo][ipgc][isp]=nrig;
          if((algo%10==6)&&(ipgc==2)){for(int iz=0;iz<4;iz++)tk_rigidityk[iz][isp]=nrigp[iz+1];}//PG+CIEMAT
          if(ipgc==2)tk_erigidity1[ialgo][isp]=enrig;
          for(int ixy=0;ixy<3;ixy++){//Algo+PG(CIEMAT+All)+Span
            tk_chis1[ialgo][ipgc][isp][ixy]=nchis[ixy];
            if((ipgc==2)&&(isp<=4))tk_pval[ialgo][isp][ixy]=npval[ixy];
            if((ixy==0)&&(isp<4))tk_dir0[ialgo][ipgc][isp]=ndir0[ixy];
          }
          if((ialgo==0)&&(ipgc==2)&&(isp==1)){//PG+CIEMAT
            for(int il=0;il<NTKL;il++){for(int ixy=0;ixy<2;ixy++)tk_res[il][ixy]=ntkres[il][ixy];}
          }
          if((ialgo==0)&&(ipgc==2)&&(isp==0)){//PG+CIEMAT
            for(int uexl=0;uexl<2;uexl++){int il=(uexl==0)?0:8; for(int ixy=0;ixy<3;ixy++)tk_cdif[uexl][ixy]=dpgc[il][ixy];}
          }
#ifdef _USELAYERRES_
//----Exclude Residual using this layer(inner-layer)
          if((ialgo==0)&&(ipgc==2)&&(isp==0))
          {
            int intkpatt=0;
            for(int il=0+1;il<9-1;il++){if(trk->TestHitLayerJ(il+1))intkpatt+=9*int(pow(10.,il));}
            for(int il=0;il<9;il++){
//              int lpatt=99999990;//(L9)L8L7L6L5L4L3L2(L1)
              int lpatt=intkpatt;
              lpatt=lpatt-((lpatt/int(pow(10.,il)))%10)*int(pow(10.,il));
              int lmfit=trk->iTrTrackPar(algo,lpatt,(refit/10)*10+3,mass,chrg);//force refit
              float nrigi=0,nchisi=9999;
              float nresi[2]={9999,9999};
              if(lmfit>=0){
                if(isreal)nrigi=trk->GetCorrectedRigidity(lmfit,3);
                else      nrigi=trk->GetRigidity(lmfit);
                nchisi=trk->GetNormChisqY(lmfit);
                AMSPoint pnt=trk->GetResidualJ(il+1,lmfit);
                for(int ixy=0;ixy<2;ixy++)nresi[ixy]=pnt[ixy];
              }
              tk_rigidityi[il]=nrigi;
              tk_chisi[il]=nchisi;
              for(int ixy=0;ixy<2;ixy++)tk_res1[il][ixy]=nresi[ixy];
            }
         }
#endif
//----
      }//end isp
     }//end ipgc
    }//end ialgo
//----
   }
  
  else {

//--Track Pos
    for(int ilay=0;ilay<NTKL;ilay++){
      AMSPoint postr;AMSDir dirtr;
      trk->InterpolateLayerJ(ilay+1,postr,dirtr);
      for(int ic=0;ic<3;ic++){tk_pos[ilay][ic]=postr[ic];tk_dir[ilay][ic]=dirtr[ic];}
//---
      if(ilay!=0&&ilay!=NTKL-1)continue;
//----
      int uexl=(ilay==0)?0:1;
      int iexl=(ilay==0)?l1i:l9i;
      tk_exql[uexl][0]=tk_exql[uexl][1]=tk_exql[uexl][2]=0;
      for(int ixy=0;ixy<3;ixy++){tk_exqln[0][uexl][ixy]=tk_exqln[1][uexl][ixy]=0;}
      tk_exqls[uexl]=0;
      tk_exdis[uexl]=1000;
      tk_exlid[uexl]=0; tk_excad[uexl]=0; 
      if(iexl<0)continue; 
      TrRecHitR* tkhit=pev->pTrRecHit(iexl);
//----Multiplicity
      int resmul=-1;
      float mexdis=1000;
      for(int imul=0;imul<tkhit->GetMultiplicity();imul++){
         AMSPoint mpos=tkhit->GetCoord(imul);
         double pexdis=(mpos[0]-postr[0])*(mpos[0]-postr[0])+(mpos[1]-postr[1])*(mpos[1]-postr[1]);
         pexdis=sqrt(pexdis);
         if(pexdis<mexdis){mexdis=pexdis;resmul=imul;}
      }
      if(resmul<0)resmul=0;
      tk_exdis[uexl]=mexdis;
      TkSens sens(tkhit->GetTkId(),postr,dirtr,0);
      AMSDir sdir = sens.GetSensDir();
      float resdxdz = (sdir.z() != 0) ? sdir.x()/sdir.z() : 0;
      float resdydz = (sdir.z() != 0) ? sdir.y()/sdir.z() : 0;
//----
      for(int ixy=0;ixy<3;ixy++){
        tk_exql[uexl][ixy]=tkhit->GetQ(ixy,obeta,frig,umass,resmul,resdxdz,resdydz);//X+Y+XYQ 
        if(tk_q[1]<=1.5)continue;
        tk_exqln[0][uexl][ixy]=tkhit->GetQYJ(ixy,obetac,frig,resmul,resdxdz,resdydz);
        tk_exqln[1][uexl][ixy]=tkhit->GetQH(ixy,obetac,frig,resmul,resdxdz,resdydz);
      }
      tk_exqls[uexl]=tkhit->GetQStatus();
//---
      tk_exlid[uexl]=tkhit->GetTkId();
      for(int ixy=0;ixy<2;ixy++){
        TrClusterR *cl=(ixy==0)?tkhit->GetXCluster():tkhit->GetYCluster();
        if(!cl)continue;
        int opt=(isreal)? (TrClusterR::kAsymEta|TrClusterR::kOverflow): TrClusterR::kAsym;
        int cstrip,sstrip;//seed/second strip index
        int noverflow=cl->GetSeedSecondIndex(cstrip,sstrip,opt);//number of ADC overflow strips nearby the seed strip
        tk_excad[uexl]+=int(pow(10000.,ixy))*cl->GetAddress(cstrip);//1000000000*K7+100000000*Mult+10000*y(address)+x(address)
      }
   }

//---Others/Calibration
    for(int il=0;il<9;il++){
       tk_cad[il]=tk_stripx[il]=tk_iso[il][0]=tk_iso[il][1]=-1;
       tk_cof[il][0]=tk_cof[il][1]=0;
       for(int ii=0;ii<11;ii++){
         tk_adc[il][0][ii]=tk_adc[il][1][ii]=0;//5strips[2] largest
         tk_xco[il][0][ii]=tk_xco[il][1][ii]=0;//5strips[2] largest
       }
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
         tk_cof[il][ixy]=cl->GetLinearCofG(-1,imul,opt,int(tk_iq+0.5));//X+Y linearized center-gravity with respect to the seed [-0.5,0.5]
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
         double cadc=0,sadc=0,tadc=0,iadc=0;
         for(int ii=0;ii<cl->GetNelem();ii++){
           float nadc=cl->GetSignal(ii,opt);
           if(nadc<=0)continue;
           float nxco=cl->GetX(ii,imul);
           if(abs(ii-cstrip)<=5){tk_adc[il][ixy][ii-cstrip+5]+=nadc;tk_xco[il][ixy][ii-cstrip+5]=nxco;}
           else                 {tk_adc[il][ixy][ii-cstrip+5<0?0:10]+=nadc;}
           if     (ii==cstrip)cadc=nadc;
           else if(abs(ii-cstrip)==1){if(ii==sstrip)sadc=nadc;else tadc=nadc;}
           else if(abs(ii-cstrip)>=2)iadc+=nadc;//non-position isolate strip sum-up
         }
         if(cadc!=0){tadc=tadc/(cadc+sadc);iadc=iadc/(iadc+cadc+sadc);}
         if(ixy==1){
           tk_iso[il][0]=tadc;
           if(!cl->IsGoodVA(2))tk_iso[il][0]+=10;
           tk_iso[il][1]=iadc;
         }
      }
   }


//---2nd Trk
   tk_oq[0]=tk_oq[1]=-1;
   for(int itr=0;itr<ntrack;itr++){
      if(itr==itrtrack)continue;
      float otkq=pev->pTrTrack(itr)->GetInnerQ();
      if     (otkq>tk_oq[0]){tk_oq[1]=tk_oq[0];tk_oq[0]=otkq;}
      else if(otkq>tk_oq[1]){tk_oq[1]=otkq;}
   }

//--NonTrk Cluster
   float cdis=1.5;//1.5cm
   float cedep=0.05;//0.05MeV
   if(!tk_bgpar)cedep=0.03;//Proton
   float cdism=0.3;//0.3cm
//   mfit=trk->iTrTrackPar(1,0,20);//VC+MaxSpan+PGCIEMAT
   mfit=trk->iTrTrackPar(1,3,20);//VC+Inner+PGCIEMAT
   for(int il=0;il<NTKL;il++){
     tk_oel[il]=tk_ohitl[il]=0;
     if(mfit<0)continue;
//--TrkPos
     TrRecHitR  *tkhit=trk->GetHitLJ(il+1);
     AMSPoint postr;AMSDir dirtr;
     trk->InterpolateLayerJ(il+1,postr,dirtr,mfit);//Non-Hit
     if(tkhit)postr=tkhit->GetCoord(-1,0);//Has-Hit
//--TrkCl
     TrClusterR *tkcl[2]={0};
     float eltr[2]={0};
     for(int ixy=0;ixy<2;ixy++){
       if(!tkhit)continue;
       TrClusterR *cl=(ixy==0)?tkhit->GetXCluster():tkhit->GetYCluster();
       if(cl){tkcl[ixy]=cl;eltr[ixy]=cl->GetEdep();}
     } 
//--NonTrkCl
     float oeltr[2]={0};
     int   ohittr[2]={0};
     for(int icl=0;icl<pev->nTrCluster();icl++){
        TrClusterR *cl=pev->pTrCluster(icl);
        if(!cl||cl->GetLayerJ()!=il+1)continue;
        int ixy=cl->GetSide();
        if(cl==tkcl[ixy])continue;//Not Trk-Cluster
        float mdis=1000;
        for(int imul=0;imul<cl->GetMultiplicity();imul++){
          double dis=cl->GetGCoord(imul)-postr[ixy];
          if(fabs(dis)<mdis)mdis=fabs(dis);
        }
        if(mdis>10)continue;//Not >10cm
        float edep=cl->GetEdep();
        if     (eltr[ixy]>0&&mdis<cdis&&edep>cedep)oeltr[ixy]+=edep;//TrkHas-Layer
        else if(eltr[ixy]<=0&&mdis<cdism&&edep>cedep)oeltr[ixy]+=edep;//TrkMis-Layer
        if(edep>cedep)ohittr[ixy]++;//<10cm
     }
//--
     int uxy=1;//Y
     if(eltr[uxy]>0){tk_oel[il]=oeltr[uxy]/(eltr[uxy]+oeltr[uxy]);}//TrkHas-Layer Positive
     else           {tk_oel[il]=-oeltr[uxy];} //TrkMis-Layer Negtive
     tk_ohitl[il]=ohittr[uxy];
   }

//----IGRF cutoff(TkInner Extraplation)
   if(isreal){
#ifndef _USEFAST_
     mfit=trk->iTrTrackPar(1,3,20);//VC+Inner+PGCIEMAT
     if(ibetah>=0&&mfit>=0&&tk_bgpar&&(tk_ubpar||tk_ngpar)){//unbiase||rig/beta<0
       double cfp=0;
       AMSPoint postr;AMSDir dirtr;
       trk->InterpolateLayerJ(obeta>0?1:9,postr,dirtr,mfit);//Non-Hit
       if(dirtr[2]>0)dirtr=dirtr*(-1);
       if(obeta<0)dirtr=dirtr*(-1);
       for(int ipn=0;ipn<2;ipn++){
          int sign=(ipn==0)?-1:1;//negtive(postive) charge
          int cfstat=pev->GetIGRFCutoff(cfp,sign,dirtr);
          if(cfstat==0)cutoffpi[ipn]=cfp;
       }
      }
#endif
    }
    else cutoffpi[0]=cutoffpi[1]=mmom/mch;
//---
  }//end else
   return true;
}

bool AMSAnalysis::Select_Trd(){

    TrdTrackR *trdtrack=pev->pTrdTrack(itrdtrack);
    if(trdtrack){
       nitrdseg=trdtrack->NTrdSegment();
//---TrdTrack Match
       AMSDir dir;AMSPoint pos;
       TrTrackR *trk=pev->pTrTrack(itrtrack);
       if(trk){
         trk->Interpolate(trdtrack->Coo[2],pos,dir);
         distrd[0]=trdtrack->Coo[0]-pos.x();
         distrd[1]=trdtrack->Coo[1]-pos.y();
         AMSDir trddir(trdtrack->Theta,trdtrack->Phi);
         distrd[2]=acos(fabs(trddir.prod(dir)))*180/3.1415926;
       }
    }

//---TrdTrack-Default-Charge
    ChargeR *charge=pev->pCharge(icharge);
    if(charge){
      ChargeSubDR *zTrd=charge->getSubD("AMSChargeTRD");
      if(zTrd){
        trd_rz = TMath::Max(Int_t(zTrd->ChargeI[0]),1);
        trd_rq = zTrd->Q;
        trd_rpz= zTrd->getProb();
      }
    }

 
//-----TrdK
   for(int i=0;i<20;i++){trd_amplk[i]=trd_pathlk[i]=0;}
   TrTrackR *trk=pev->pTrTrack(itrtrack);
   if(!trk||tk_iq<=0)return false;
//---
   float ecalen=0;
   EcalShowerR *show= pev->pEcalShower(ishow);
   if(show){ecalen=show->EnergyE;}
   bool ecalpar=(ecalen>1);
//---
#ifndef _USEFAST_
   if(tk_bgpar||tk_l1l9par||tk_ubpar||ecalpar)trd_statk=-500;
#ifdef _SAVENEGSCALE_
   if(tk_ngpar)trd_statk=-500;
#endif
#endif
   if(trd_statk<=-1000)return false;
//---Track
   int fid=trk->iTrTrackPar(1,0,20); // Get any prefered fit code
   double trdlikr[3]={-1,-1,-1},trdlikre[3]={-1,-1,-1},trdlik[3]={-1,-1,-1}; //LikelihoodRatio :  e/P, e/H, P/H
   int NHits=0;  //number of hits in Likelihood Calculation
   float threshold=15;  //ADC above will be taken into account in Likelihood Calculation
   static unsigned int prtimek=0;//reduce huge output
   static bool prstatk=1;
   if(pev->UTime()==prtimek&&!prstatk){trd_statk=-200;return false;}
   prtimek=pev->UTime();prstatk=1;
   TrdKCluster trdcluster=TrdKCluster(pev,trk,fid);
//   return false;
   int IsReadAlignmentOK=trdcluster.IsReadAlignmentOK; //0: No Alignment, 1: Static Alignment Layer level,  2: Dynamic
   int IsReadCalibOK=trdcluster.IsReadCalibOK;  // 0: No Gain Calibration, 1: Gain Calibration Succeeded
   if(IsReadAlignmentOK==0||IsReadCalibOK==0){prstatk=0;trd_statk=-200;return false;} 
   trd_statk=trdcluster.GetLikelihoodRatio_TrTrack(threshold,trdlikr,NHits);//Track=1
   trdcluster.GetOffTrackHit_TrTrack(trd_onhitk,trd_oampk);
   if(trd_statk!=1){
      trd_statk=trdcluster.GetLikelihoodRatio_TRDRefit(threshold,trdlikr,NHits);
      if(trd_statk==1)trd_statk+=10;//TRD=11
      trdcluster.GetOffTrackHit_TRDRefit(trd_onhitk,trd_oampk);
   }
   trd_nhitk=NHits;//Number of surrounding fired tubes
   if(trd_statk%10==1){//trd_statk=1 || trd_statk=11
      if(ecalpar){
        if(trd_statk==1)trdcluster.GetLikelihoodRatio_TrTrack(threshold,trdlikre,NHits,ecalen,trdlik);
        else            trdcluster.GetLikelihoodRatio_TRDRefit(threshold,trdlikre,NHits,ecalen,trdlik);
      }
      for(int i=0;i<3;i++){trd_lik[i]=trdlikr[i];trd_like[i]=trdlikre[i];}
      AMSPoint pnt; AMSDir dir;
      int x= trdcluster.GetTrTrackExtrapolation(pnt,dir);
      if(x>=0){
       for(int i=0;i<trdcluster.NHits();i++){
         TrdKHit *hit=trdcluster.GetHit(i);
         if(!hit)continue;
         float tube_length=hit->Tube_Track_3DLength(&pnt,&dir);
         if(tube_length<=0.02)continue;
         float ampk = hit->TRDHit_Amp;
         if(ampk<=0)continue;
         if(ampk>32767)ampk=32767;
         trd_ampsk+=(ampk/tube_length);
         trd_pathsk+=tube_length;
         trd_amphitk++;
         int ilay=hit->TRDHit_Layer;
         if(ampk<=trd_amplk[ilay])continue;//MaxADC
         trd_amplk[ilay]=ampk;
         trd_pathlk[ilay]=tube_length;
       }
      }
    }
   ///TRD-Charge
     trdcluster.CalculateTRDCharge(0,tof_betah);
     trd_qk[0]=   trdcluster.GetTRDCharge();
     trd_qk[1]=   trdcluster.GetTRDChargeUpper();
     trd_qk[2]=   trdcluster.GetTRDChargeLower();
     trd_qrmsk[0]=trdcluster.GetTRDChargeError();
     trd_qnhk[0]= trdcluster.GetQNHit();
     trd_qnhk[1]= trdcluster.GetQNHitUpper();
     trd_qnhk[2]= trdcluster.GetQNHitLower();
     trd_ipch=    trdcluster.GetIPChi2();//impact Chis

///--End Fragmentation delta
     trdcluster.CalculateTRDCharge(1,tof_betah);
     trd_qk[3]=   trdcluster.GetTRDCharge();
     trd_qrmsk[1]=trdcluster.GetTRDChargeError();
     trdcluster.CalculateTRDCharge(2,tof_betah);
     trd_qk[4]=   trdcluster.GetTRDCharge();
     trd_qrmsk[2]=trdcluster.GetTRDChargeError();


    return true;
}

bool AMSAnalysis::Select_Tof(){

//---
     int fid=0;
     float frig=(itrtrack>=0)?pev->pTrTrack(itrtrack)->GetRigidity(fid):0;//default
//--
     BetaR *beta=pev->pBeta(ibeta);
     if(beta)tof_beta=beta->Beta; 
//---
     BetaHR *betah=pev->pBetaH(ibetah);
      
     if(!betah)return false;
     tof_btype=betah->GetBuildType();
     tof_bpatt=betah->GetBetaPattern();

     tof_betah=betah->GetBeta();
     tof_betahmc=(!isreal)?betah->GetMCBeta(1):tof_betah;
     tof_ebetah=betah->GetEBetaV();
     tof_chist=betah->GetChi2T();
     tof_chisc=betah->GetChi2C();
//---Normalized
     TofBetaPar normbetapar=betah->GetNormTofBetaPar();
     tof_chist_n=normbetapar.Chi2T; 
     tof_chisc_n=normbetapar.Chi2C;
     tof_ebetah_n=normbetapar.InvErrBeta; 
//----
     tof_hsumh=betah->GetSumHit();
     tof_hsumhu=betah->GetUseHit();

//---TOF Layer
     tof_pass=0;
     AMSPoint pnt; AMSDir dir; AMSPoint tofpnt;
     float disedge;double utm;
     for(int ilay=0;ilay<NTOFL;ilay++){
//---Clear
        tof_barid[ilay]=tof_tl[ilay]=-1;
        tof_oq[ilay][0]=tof_oq[ilay][1]=0;
        tof_ob[ilay][0]=tof_ob[ilay][1]=-1;
        tof_sbit[ilay][0]=tof_sbit[ilay][1]=0;
//---All
        tof_nclhl[ilay]=betah->GetAllFireHL(ilay);
        betah->TInterpolate (TOFGeom::GetMeanZ(ilay),pnt,dir,utm);
        for(int ic=0;ic<3;ic++)tof_pos[ilay][ic]=pnt[ic];
        int tk_pass=ParticleR::IsPassTOF(ilay,pnt,dir,tofpnt,disedge);
        if(tk_pass>=0){tof_pass+=int(pow(10.,3-ilay));}
//----TOFCL
        if(!betah->TestExistHL(ilay))continue;
        TofClusterHR *tofclh=betah->GetClusterHL(ilay);
        tof_barid[ilay]=tofclh->Bar;
        if(tofclh->IsGoodTime()){tof_tl[ilay]=tofclh->Time;}
        for(int is=0;is<2;is++)tof_sbit[ilay][is]=tofclh->SideBitPat[is];
     }

//---TOF ClusterH
    for(int iclh=0;iclh<ntofclh;iclh++){
       TofClusterHR *tofclh=pev->pTofClusterH(iclh);
       int ilay=tofclh->Layer;
       if(tofclh->Bar==tof_barid[ilay])continue;
       if(tofclh->GetQSignal()>tof_oq[ilay][0]){//>Max NonBetaH Cluster
          tof_oq[ilay][1]=tof_oq[ilay][0];
          tof_oq[ilay][0]=tofclh->GetQSignal();
          tof_ob[ilay][1]=tof_ob[ilay][0];
          tof_ob[ilay][0]=tofclh->Bar;
       }
       else if(tofclh->GetQSignal()>tof_oq[ilay][1]){//>Second Max NonBetaH Cluster
          tof_oq[ilay][1]=tofclh->GetQSignal();
          tof_ob[ilay][1]=tofclh->Bar;
       }
    }

//----
//   float obetac=tof_betah;
   float obetac=(!isreal)?tof_betahmc:tof_betah;
//--TOFCharge All
   tof_qs=0;
   TofChargeHR *tofhc=betah->pTofChargeH();
   int nlay=0;
   tof_zr=tofhc->GetZ(nlay,tof_pzr);

//----TOFQL old/MC
   if(!isreal){
      tofhc->ReFit(tof_betah,TofChargeHR::DefaultQOptIonW,frig);
      for(int ilay=0;ilay<NTOFL;ilay++){
        tof_qlb[ilay]=tofhc->TestExistHL(ilay)?tofhc->GetQL(ilay,22):0;
     }
   }

//---TOFQL new
   tofhc->ReFit(obetac,TofChargeHR::DefaultQOptIonW,frig); 
   tof_q[0]=tofhc->GetQ(nlay,tof_qrms[0],1111);//All
   tof_q[1]=tofhc->GetQ(nlay,tof_qrms[1],-2);//Truncate 
///---Z All
   tof_z   =tofhc->GetZ(nlay,tof_pz);
   tof_zud[0]=tofhc->GetZ(nlay,tof_pud[0],0,1100);
   tof_zud[1]=tofhc->GetZ(nlay,tof_pud[1],0,11);
//---TOFQL
   for(int ilay=0;ilay<NTOFL;ilay++){
//---Clear
     for(int ity=0;ity<3;ity++){tof_qlr[ity][ilay]=0;}
     tof_ql[ilay]=tof_ql1[ilay]=0;
//---BetaH Check
     tof_ql1[ilay]=betah->GetQL(ilay,2,TofClusterHR::DefaultQOptIonW,111111,obetac,frig);
     tof_qlr[0][ilay]=betah->GetQL(ilay,2,TofClusterHR::DefaultQOptIonW,111111,1,25);//no-beta(no-rigidity) correction
//---QL
     if(!tofhc->TestExistHL(ilay))continue;
     tof_ql[ilay] =tofhc->GetQL(ilay,22);//New TOFL
     tof_qlr[1][ilay]=tofhc->GetQL(ilay,1);//Anode
     tof_qlr[2][ilay]=tofhc->GetQL(ilay,0);//Dynode
     if(tofhc->IsGoodQPathL(ilay)){tof_qs+=int(pow(10.,3-ilay));}
   }

//---
   int ncls[4];
   tof_oncl=pev->GetNTofClustersInTime(betah, ncls); 

//---Calibration only
#ifdef _USETKADC_
  float obetac2=(!isreal)?tof_betahmc:tof_betah;
  for(int ic=0;ic<3;ic++){
    if     (ic==0)tofhc->ReFit(obetac2,(TofChargeHR::DefaultQOptIonW&(~TofRecH::kBetaCor)&(~TofRecH::kRigidityCor)),frig);
    else if(ic==1)tofhc->ReFit(obetac2,(TofChargeHR::DefaultQOptIonW|TofRecH::kBetaRigV1Cor),frig);
    else          tofhc->ReFit(obetac2,(TofChargeHR::DefaultQOptIonW|TofRecH::kBetaRigV2Cor),frig);
    for(int ilay=0;ilay<NTOFL;ilay++){
      for(int ity=0;ity<2;ity++){tof_qlc[ic*2+ity][ilay]=0;}
      if(ic==2){
        for(int is=0;is<2;is++){for(int ipm=0;ipm<4;ipm++)tof_qlpm[ilay][is][ipm]=0;}
      }
      if(!tofhc->TestExistHL(ilay))continue;
      tof_qlc[ic*2+0][ilay]=tofhc->GetQL(ilay,1);//Anode
      tof_qlc[ic*2+1][ilay]=tofhc->GetQL(ilay,0);//Dynode
//------
      if(ic==2){
        for(int is=0;is<2;is++){
          tof_qlpm[ilay][is][0]=betah->GetQLPM(ilay,1,is,0,(TofChargeHR::DefaultQOptIonW|TofRecH::kBetaRigV2Cor),frig);//Anode
          for(int ipm=0;ipm<3;ipm++){
             tof_qlpm[ilay][is][ipm+1]=betah->GetQLPM(ilay,0,is,ipm,(TofChargeHR::DefaultQOptIonW|TofRecH::kBetaRigV2Cor),frig);//Dynode
          }
        }
      }
//------
    }
  }
#endif
 

  return true; 
}

bool AMSAnalysis::Select_Rich(){
//---
    RichRingR *ring=pev->pRichRing(irich);
    if(ring){
      rich_itrtrack=ring->iTrTrack();
      rich_beta[0]=ring->getBeta();
      rich_beta[1]=ring->Beta; 
      rich_beta[2]=ring->BetaRefit;
      rich_ebeta=ring->ErrorBeta;
      rich_ebeta1=ring->getBetaError();
      rich_pb=ring->getProb();
      rich_udis=ring->UDist;
      rich_hit=ring->getHits();// 
      rich_used=ring->getUsedHits();
      rich_usedm=ring->getReflectedHits();
      rich_q[0]=ring->getCharge2Estimate();
      rich_q[1]=ring->NpExp>0?sqrt(ring->NpCol/ring->NpExp):-1;
      rich_width=ring->getWidth();
      rich_npe[0]=ring->getPhotoelectrons();
      rich_npe[1]=ring->getExpectedPhotoElectrons();
      rich_npe[2]=RichHitR::getCollectedPhotoElectrons();
      rich_stat=ring->Status;
      rich_good=ring->IsGood();
      rich_clean=ring->IsClean();
      rich_NaF=ring->IsNaF();
      rich_pmt=ring->getPMTs();
      rich_pmt1=RichHitR::getPMTs()-RichHitR::getPMTs(false);
      rich_PMTChargeConsistency=ring->getPMTChargeConsistency();
      rich_BetaConsistency=ring->getBetaConsistency();
      rich_pos[0]=ring->getTrackEmissionPoint()[0];
      rich_pos[1]=ring->getTrackEmissionPoint()[1];
      rich_pos[2]=ring->getTrackEmissionPoint()[2];
      rich_theta=ring->getTrackTheta();
      rich_phi=ring->getTrackPhi();
      rich_n[0]=ring->getIndexUsed();
      rich_n[1]=ring->getRawIndexUsed();
      rich_tile=ring->getTileIndex();
      rich_distb=ring->DistanceTileBorder();
      rich_cstat=ring->PmtCorrectionsFailed();
    }
//-----
     RichRingBR *ringb=pev->pRichRingB(irichb);
     if(ringb){
       rich_betap=ringb->Beta;
       rich_likp= ringb->Likelihood;
       rich_pbp=  ringb->ProbKolm;
       rich_usedp=ringb->Used;
       rich_qp=   ringb->ChargeRec; 
       rich_statp=ringb->Status;
       for(int iz=0;iz<3;iz++){rich_pzp[iz]=ringb->ChargeProb[iz];}
     }
   
//---Rich-Default-Charge
   ChargeR *charge=pev->pCharge(icharge);
   if(charge){
     ChargeSubDR *zRing=charge->getSubD("AMSChargeRich");
     if(zRing){
       rich_rz = TMath::Max(Int_t(zRing->ChargeI[0]),1);
       rich_rq = zRing->Q;
       rich_rpz= zRing->getProb();
     }
   }
 
 
  return true;
}

bool AMSAnalysis::Select_Ecal(){
   
   EcalShowerR *show= pev->pEcalShower(ishow);
   ecal_q=0;
   for(int ilay=0;ilay<NECALL;ilay++){ecal_el[ilay]=ecal_eh[ilay]=ecal_nhit[ilay]=0;}
   if(show){
     ecal_stat=show->Status;
     ecal_en[0]=show->EnergyE;
     ecal_en[1]=show->EnergyA;
     ecal_en[2]=show->EnergyD;
     ecal_en[3]=show->EnergyC;
     ecal_rearl=show->RearLeakPI;
     ecal_q=show->EcalChargeEstimator();
     for(int i2dcl=0;i2dcl<show->NEcal2DCluster();i2dcl++){//2dCl
       Ecal2DClusterR *e2dcl=show->pEcal2DCluster(i2dcl);
       for(int icl=0;icl<e2dcl->NEcalCluster();icl++){//1dCl
         EcalClusterR *ecl=e2dcl->pEcalCluster(icl);
         for(int ihit=0;ihit<ecl->NEcalHit();ihit++){//Nhit
           EcalHitR *ehit=ecl->pEcalHit(ihit);
           float edep=ehit->Edep;
           int ilay=ehit->Plane; 
           ecal_nhit[ilay]++;
           ecal_el[ilay]+=edep;
           if(edep>ecal_eh[ilay]){ecal_eh[ilay]=edep;}
         }
       }
     }
#ifndef _USEFAST_
     bool ecalpar=(ecal_en[0]>1);
     bool ecalbepar=(ecal_en[0]>1);//>1GV
     if(ecalpar){
        ecal_bdt[0]=show->GetEcalBDT();
        ecal_bdt[1]=show->GetEcalBDT(7,1);
//       ecal_bdt[1]=show->EcalStandaloneEstimatorV2();
//       if(ecalbepar)ecal_bdt[2]=show->GetEcalBDTCHI2();
     }
#endif
     ecal_enc[0]=show->GetCorrectedEnergy(2,2,0);//e+-/ps/oves(default)
     ecal_enc[1]=show->GetCorrectedEnergy(2,2,1);//e+-/ps/nves
     ecal_enc[2]=show->GetCorrectedEnergy(2,0,0);//e+-/vc/onves
   }

   return true;
}

bool AMSAnalysis::Select_Anti(){
  return true;
}


bool AMSAnalysis::Select_TrkParticle(){

//----Unbiased-Tracker(TOF efficiency study) Max-Momentum
  if(ntrack>=1)itrtracks=0;
  if(ntrack>1){
    float tkmom=0;
    for(int itr=0;itr<ntrack;itr++){
      TrTrackR *trk=pev->pTrTrack(itr);
      float ntkmom=fabs(trk->GetRigidity())*trk->GetQ();
      if(ntkmom>tkmom){itrtracks=itr;tkmom=ntkmom;}
    }
  }

//---TrTrackQ+Rigidity(good for Z>=2 particle)
  if(itrtracks<0)return false;
  TrTrackR *trk=pev->pTrTrack(itrtracks);
  for(int ibh=0;ibh<nbetah;ibh++){
    BetaHR *betah=pev->pBetaH(ibh);
    if(betah->iTrTrack()==itrtracks){tk_ibetahs=ibh;break;}
  }
  BetaHR *betah=pev->pBetaH(tk_ibetahs);

 for(int iq=0;iq<2;iq++){
    float obeta=1;int fid=0;
    if(iq==1&&betah)obeta=betah->GetBeta(); //Beta Correction
    mean_t tkqa=trk->GetInnerQ_all(obeta,fid);//Inner
    tk_qis[iq]   =tkqa.Mean;
    tk_qirmss[iq]=tkqa.RMS;
    for(int il=0;il<9;il++){//L1L9Q
      tk_qlss[iq][il]=trk->GetLayerJQ(il+1,obeta,fid);
    }
  }

//---Tk quality
  int algo=1;// 1:Choutko 2:Alcaraz
  int patt=0;// 3:Inner 5:L1I 6:IL9 7:L1IL9
  int refit=3;
  float mass=TrFit::Mproton;
  int chrg=1;

//---Init Track ReFit
  float tkfz=trk->GetAdvancedFitCharge();
  tkfz=-1;

  InitTrkReFit(tk_qis[0],tkfz,refit,mass,chrg,algo); 

//---Hit Pattern
  tk_hitbs[0]=trk->GetBitPatternXYJ(); tk_hitbs[1]=trk->GetBitPatternJ();

//---Algo
  for(int isp=0;isp<=4;isp++){
    if     (isp==0)patt=0;//Full Span
    else if(isp==1)patt=3;//Inner
    else if(isp==2)patt=5;//Inner+Layer1
    else if(isp==3)patt=6;//Inner+Layer9
    else if(isp==4)patt=7;//Force Full Span
    refit=20+refit%10;//PG+CIEMAT 
    int mfit=trk->iTrTrackPar(algo,patt,refit,mass,chrg); 
    tk_rigiditys[isp]=0;
    for(int ichis=0;ichis<3;ichis++){tk_chiss[isp][ichis]=9999;}
    if(mfit>=0){
      if(isreal)tk_rigiditys[isp]=trk->GetCorrectedRigidity(mfit,3);
      else      tk_rigiditys[isp]=trk->GetRigidity(mfit);
      tk_chiss[isp][0]=trk->GetNormChisqX(mfit);
      tk_chiss[isp][1]=trk->GetNormChisqY(mfit);
      tk_chiss[isp][2]=trk->GetChisq(mfit);
    }
  }

//--Track Pos
  for(int ilay=0;ilay<NTKL;ilay++){
     AMSPoint postr;AMSDir dirtr;
     trk->InterpolateLayerJ(ilay+1,postr,dirtr);
     int uexl=(ilay==0)?0:1;
     for(int ic=0;ic<3;ic++){tk_pos2s[ilay][ic]=postr[ic];if(ilay==0||ilay==NTKL-1)tk_dir2s[uexl][ic]=dirtr[ic];}
   }

//---TOF
  if(betah){
    tk_betahs=betah->GetBeta();
    TofBetaPar normbetapar=betah->GetNormTofBetaPar();
    tk_tofchists=normbetapar.Chi2T;
    tk_tofchiscs=normbetapar.Chi2C;
    tk_tofsumhus=betah->GetUseHit();
    TofChargeHR *tofhc=betah->pTofChargeH();
    float obetac=(!isreal)?betah->GetMCBeta(1):tk_betahs;//isreal 
    tofhc->ReFit(obetac,TofChargeHR::DefaultQOptIonW,tk_rigiditys[0]);
    for(int ilay=0;ilay<NTOFL;ilay++){
      tk_tofqls[ilay]=0;
      if(!tofhc->TestExistHL(ilay))continue;
      tk_tofqls[ilay]=tofhc->GetQL(ilay,22);//New TOFL
    }
  }

  return true;

}


void AMSAnalysis::InitEvent(AMSEventR *ev){
//---amsdn
   amsdn=164; //vers*100+amsd
//---Header
   run=0;event=0;version=0;errorb=0;time[0]=0;time[1]=0;isbadrun=0;
//---Trigger
   nlevel1=-1;
//---RTI
   irti=-1;
//---MC
   mpar=mch=0;
   ntrmccl=ntofmccl=-1;
//--Particle
   nparticle=nbetah=nbeta=ntrack=ntrrawcl=ntrcl=ntrhit=ntrdtrack=nrich=nrichb=nshow=necalhit=ntofclh=ncharge=-1;
   tk_l1q=0;
   trd_pass=-1;
//---
   l1i=l9i=itrtrack=ibetah=ibeta=iparindex=itrdtrack=ishow=irich=irichb=icharge=-1;
   cutoffp[0]=cutoffp[1]=cutoffpi[0]=cutoffpi[1]=0;
//---L1L9Par
   ibetahs=itrdtracks=itrtracks=tk_ibetahs=-1;
   tof_nhits=tof_hsumhus=tk_tofsumhus=-1;
   betahs=betahsmc=tk_betahs=-3;
   tk_qis[0]=tk_qis[1]=0;
   cutoffps=trd_thetas=trd_phis=trd_rqs=0;
//---TOF
   tof_z=0;
   tof_betah=tof_betahmc=tof_beta=-3;
   tof_hsumh=tof_hsumhu=-1;
//---Track
   issmearext=0;
   tk_l1l9par=tk_bgpar=tk_bhgpar=tk_ubpar=tk_ngpar=0;
   tk_iq=tk_1q=tk_9q=-1;
   tk_qr=tk_q[0]=tk_q[1]=tk_rz[0][0]=tk_rz[0][1]=tk_rz[1][0]=tk_rz[1][1]=-1;
   for(int ic=0;ic<2;ic++){
     for(int ixy=0;ixy<3;ixy++){tk_qin[ic][ixy]=tk_qrmn[ic][ixy]=0;}
   }
   tk_rtype=tk_hitb[0]=tk_hitb[1]=tk_nhiti[0]=tk_nhiti[1]=0;
   tk_bcor=tk_tcor=tk_fzm[0]=tk_fzm[1]=0;
//---Trd
   trd_statk=-1000;
   trd_nhitk=trd_onhitk=trd_oampk=-1;
   trd_lik[0]=trd_lik[1]=trd_lik[2]=trd_like[0]=trd_like[1]=trd_like[2]=-3;
   trd_ampsk=trd_amphitk=trd_pathsk=0;
   trd_qk[0]=0;
   trd_rq=trd_rz=-1;
//--Rich
   rich_itrtrack=-1;
   rich_beta[0]=rich_beta[1]=rich_betap=-3;
   rich_rz=rich_rq=-1;
//--Ecal
   ecal_stat=0;
   ecal_en[0]=ecal_en[1]=ecal_en[2]=ecal_en[3]=ecal_rearl=-3;
   ecal_enc[0]=ecal_enc[1]=ecal_enc[2]=-3;
   ecal_bdt[0]=ecal_bdt[1]=ecal_bdt[2]=-3;
//--Anti
   anti_nhit=-1;

  pev=ev;
}

int AMSAnalysis::Fill(int it){
  if     (it==0){return tout[0]->Fill();}
  else if(it==1){return tout[1]->Fill();}
  else          {return tout[2]->Fill();}
}

int AMSAnalysis::Save(){
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
    AMSAnalysis *analysis =new AMSAnalysis();
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
        if(ev->nMCEventg()){//MC
           if(fabs(analysis->mch)==1)prunbiasmc=1;//P/e+ prunbiasmc=1
           if(entry%prunbiasmc==0)analysis->unbiasetag=1;
        }
        else {
           if(entry%prunbias==0)analysis->unbiasetag=1;
        } 
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
   const char *sname[12]={
      "Head  ", "Trig  ","RTI|MC","Part    ","Trk0    ","Tof     ","Trk1    ","Trd     ","Rich    ","Ecal    ","Anti    ","TrkPart ",
    };
///---
    cout<<"<<---------Begin Static-------------->>"<<endl;
    cout<<"   Event("<<sstat[0]<<")"<<"   CPU"<<"   TreeSize"<<endl;
    for(int icut=0;icut<12;icut++){
      cout<<sname[icut]<<"   "<<sstat[icut+1]<<"   "<<(double(cput[icut]/CLOCKS_PER_SEC))<<"   "<<sstree[icut]<<endl;
     }
    cout<<"    Process Run="<<nrun<<" Stat="<<nstat<<endl;
    if(nrun!=nread)cout<<"    Read Run="<<nread<<endl;
    cout<<"<<---------End Static AMSJOB-------------->>"<<endl;

//-----
   return nstat;//nstat=0 normal, nstat=2 no-run, nstat=3 copy-error, nstat=4 stop-error, nstat=5 read-error,nstat=6 save-error,nstat=9 initialization-error,nstat+10 nrun!=nread
   

}
