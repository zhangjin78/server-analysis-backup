//----Author Q.Yan qyan@cern.ch: interface to Millepede
#ifdef AFTERSPLINE2
#define AFTERSPLINE
#endif 
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
#include "TString.h"
#include "HistoMan.h"
#include "TProfile2D.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
#include "root_RVSP.h" //betahr
#include "TDatime.h"
#include "TkGeomN.h"
#include "MilleBinary.h"
#include "GblFitter.h"
#include "readfile3.C"
//#include "bin.C"
#include "QEvent2.h"
#include "QAnalysis.h"

using namespace qanalysisconst;

class QAnalysisN: public QAnalysis{
 public:
    static const int SCharge=1;
//    static const int MCharge=8;
    static const int MCharge=2;
    HistoMan hman1;
 public:
    char *runlogname;
    std::map<int, int > ladnev;
    std::map<int, int > ladnevx;
    gbl::MilleBinary* milleFile;
    TkTrackN *tkraw;
    TkTrackN *tkfinal;
    TRandom3 *rd;
    int ialign;
    unsigned int prrun;
    unsigned int btime;
    unsigned int etime;
    int  initalign;
    int  alignnev;
    double aligndt[2];
    double aligntime;
    long int sumev;
    long int bev;
    long int ccev;
    unsigned int stimer[2];
    map<unsigned int,string> splinelist; 
    map<unsigned int,pair<double, double> >timelist;//run,time[2]
    TString ofiles;
    vector<TString> aligngen;
    vector<TString> alignres;
    double cput[2][10];
 public:
    QAnalysisN(TChain *ch):QAnalysis(ch){Init();}
    ~QAnalysisN(){Clear();}
    int Init();
    int Clear();
    int PreProcess(); 
    int GenerateSmearMC(int seed,int isend=0);
    int InitAlignment(int isend=0);
    int InitAlignmentS(unsigned int utime);
    int DoAlignment();
    int BookHistos(const char *ofile);
    int SelectEvent(QEvent *ev0=0,int pos=0);
    int Save(const char *odir=0);
    int WriteRunList(const char *runlogdir=0);
};

//------
int QAnalysisN::Init(){
  runlogname=0;
  milleFile=0;
  tkfinal=tkraw=0;
  rd=0;
  initalign=0;
  ialign=prrun=ccev=sumev=bev=0;
  btime=etime=0;
  for(int j=0;j<2;j++){
    for(int i=0;i<10;i++)cput[j][i]=0;
  }
//-----
  bool isreal=GetQRunManager()->qisreal;
#if defined (USEDT1)
  aligndt[0]=aligndt[1]=5*60;//4min 
#else
  aligndt[0]=aligndt[1]=1.5*60;//2min
#endif
  aligntime=0;
  if(isreal)alignnev=2000;//Data: current events cut~3000/3=1000
  else {//MC
#if defined (USENEVG)
   alignnev=1000000;
#else
   alignnev=100000;
#endif
  }
//-----
  tkraw=new TkTrackN();
  tkraw->ConstructTracker();
  tkfinal=new TkTrackN();
  tkfinal->ConstructTracker();
  const char *alignfile[2]={0};
  alignfile[1]="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv3.res";//Data
  const char *afterfile=isreal?alignfile[1]:alignfile[0];//first alignment
  if(afterfile)cout<<"afterfile="<<afterfile<<endl;
  tkfinal->LoadAlignModulePar(afterfile);
//------
  return 0;
}

//------
int QAnalysisN::Clear(){
  if(milleFile)delete milleFile;
  if(tkfinal)delete tkfinal;
  if(tkraw)delete tkraw;
  if(rd)delete rd;
  return 0;
}

//------BookHisto
int QAnalysisN::BookHistos(const char *ofile){

  hman1.Clear();
  hman1.Enable();
//----
  ofiles=ofile;
  TString app("_");
  app+=abs(ialign);
  app+=".root";
  ofiles.ReplaceAll(".root",app);
  cout<<"ialign="<<ialign<<" ofile="<<ofile<<" ofilen="<<ofiles.Data()<<endl; 
//----
  hman1.Setname(ofiles.Data());


  bool isreal=GetQRunManager()->qisreal;
//---
  char histn[1000];
//--normal bin
  double hebin[1000];
  const int bin=getamsbin(hebin,1)-1;
  for(int ixy=0;ixy<2;ixy++){
    sprintf(histn,"chisoxy%d",ixy);
    hman1.Add(new TH1D(histn,histn,500,0,100));
    sprintf(histn,"chiso2xy%d",ixy);
    hman1.Add(new TH1D(histn,histn,500,0,100));
  }
  for(int iud=0;iud<2;iud++){
    sprintf(histn,"tofqud%d",iud);
    hman1.Add(new TH1D(histn,histn,100,0,5));
  }
  hman1.Add(new TH1D("tofbetah","tofbetah",200,-2,2));
  hman1.Add(new TH1D("tkinq","tkinq",100,0,5));
  hman1.Add(new TH1D("tkl1q","tkl1q",100,0,5));
  hman1.Add(new TH1D("ntrack","ntrack",10,0,10));
  hman1.Add(new TH1D("tkl9q","tkl9q",100,0,5)); 
  hman1.Add(new TH1D("cosrig","cosrig",bin,hebin));
  hman1.Add(new TH1D("accev","accev",500,0,70000));
  hman1.Add(new TH1D("adt",  "adt",  500,0,300));
  for(int iz=SCharge;iz<=MCharge;iz++){
    sprintf(histn,"selmom_z%d",iz);
    hman1.Add(new TH1D(histn,histn,bin,hebin));
    sprintf(histn,"selmom1_z%d",iz);
    hman1.Add(new TH1D(histn,histn,bin,hebin));
    for(int ib=0;ib<2;ib++){
      sprintf(histn,"seleffp_b%d",ib);
      if(iz==SCharge)hman1.Add(new TH1D(histn,histn,15,0,15));
    }
    for(int ixy=1;ixy<=2;ixy++){
      sprintf(histn,"ladnev_z%dxy%d",iz,ixy);
      hman1.Add(new TH1D(histn,histn,400,-200,200));
      sprintf(histn,"sennev_z%dxy%d",iz,ixy);
      hman1.Add(new TH2D(histn,histn,400,-200,200,16,0,16));
      int uxy=(ixy==1)?1:0; 
      if(iz==SCharge){
        sprintf(histn,"ladpos_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,400,-200,200,100,-35,35));
        else      hman1.Add(new TH2F(histn,histn,400,-200,200,20,-4,4));
        sprintf(histn,"laddir_xy%d",uxy);
        hman1.Add(new TH2D(histn,histn,400,-200,200,100,-1,1));
#ifdef USEFAST
#ifdef USEEXTALIGN 
        sprintf(histn,"senpos_xy%d",uxy);
        if(uxy==0)hman1.Add(new TH2F(histn,histn,8000,-4000,4000,20,-2.5,2.5));
        else      hman1.Add(new TH2F(histn,histn,8000,-4000,4000,20,-4,4));
        sprintf(histn,"sendir_xy%d",uxy);
        hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-0.5,0.5));
        sprintf(histn,"senres_xy%d",uxy);
        if(ialign==0)hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-400,400));
        else         hman1.Add(new TH2F(histn,histn,8000,-4000,4000,100,-100,100));
#endif
#endif
        sprintf(histn,"ladres_xy%d",uxy);
        if(ialign==0)hman1.Add(new TH2F(histn,histn,400,-200,200,100,-800,800));
        else         hman1.Add(new TH2F(histn,histn,400,-200,200,100,-100,100));
        if(!isreal){
          sprintf(histn,"ladres1_xy%d",uxy);
          if(ialign==0)hman1.Add(new TH2F(histn,histn,400,-200,200,100,-800,800));
          else         hman1.Add(new TH2F(histn,histn,400,-200,200,100,-100,100));
        }
      }
    }
    for(int ir=0;ir<4;ir++){
      sprintf(histn,"srigr%d_q%d",ir,iz);
      if(!isreal)hman1.Add(new TH2F(histn,histn,bin,hebin,1000,-25,25));
      sprintf(histn,"cosrigr%d",ir);
      if(iz==SCharge)hman1.Add(new TH1D(histn,histn,bin,hebin));
      for(int ixy=0;ixy<3;ixy++){
        sprintf(histn,"chisr%dz%dxy%d",ir,iz,ixy);
        hman1.Add(new TH1D(histn,histn,500,0,100));
      }
      for(int il=0;il<9;il++){
#ifdef USEEXTALIGN
        if(il!=0&&il!=8)continue;
#endif
        for(int ixy=0;ixy<2;ixy++){
          sprintf(histn,"res_r%dz%dl%dxy%d",ir,iz,il,ixy);
          if(ialign==0)hman1.Add(new TH1D(histn,histn,100,-1000,1000));
          else         hman1.Add(new TH1D(histn,histn,100,-10,10));
          if(!isreal){
            sprintf(histn,"resr_r%dz%dl%dxy%d",ir,iz,il,ixy); 
            if(ialign==0)hman1.Add(new TH2F(histn,histn,bin,hebin,100,-1000,1000));
            else         hman1.Add(new TH2F(histn,histn,bin,hebin,100,-10,10));
            sprintf(histn,"mis_r%dz%dl%dxy%d",ir,iz,il,ixy);
            if     (ialign==0)     hman1.Add(new TH1D(histn,histn,200,-2000,2000));
            else if(abs(ialign)<=2)hman1.Add(new TH1D(histn,histn,200,-100,100));
            else                   hman1.Add(new TH1D(histn,histn,200,-40,40));
            sprintf(histn,"mis1_r%dz%dl%dxy%d",ir,iz,il,ixy);
            if     (ialign==0)     hman1.Add(new TH1D(histn,histn,200,-2000,2000));
            else if(abs(ialign)<=2)hman1.Add(new TH1D(histn,histn,200,-100,100));
            else                   hman1.Add(new TH1D(histn,histn,200,-40,40));
            sprintf(histn,"misr_r%dz%dl%dxy%d",ir,iz,il,ixy);
            if     (ialign==0)     hman1.Add(new TH2F(histn,histn,bin,hebin,200,-2000,2000));
            else if(abs(ialign)<=2)hman1.Add(new TH2F(histn,histn,bin,hebin,200,-100,100));
            else                   hman1.Add(new TH2F(histn,histn,bin,hebin,200,-40,40));
            sprintf(histn,"mis1r_r%dz%dl%dxy%d",ir,iz,il,ixy);
            if     (ialign==0)     hman1.Add(new TH2F(histn,histn,bin,hebin,200,-2000,2000));
            else if(abs(ialign)<=2)hman1.Add(new TH2F(histn,histn,bin,hebin,200,-100,100));
            else                   hman1.Add(new TH2F(histn,histn,bin,hebin,200,-40,40));
          }
        }
      }
    }
  }
  return 0;
}


//-------------------Pre Process
int QAnalysisN::PreProcess(){
 
  if(GetQRunManager()->qisreal==0)return 0;//Must be Data
  timelist.clear();//only for data
  Long64_t numr=qch->GetEntries();
  int nstat=0;//error bit
  for(Long64_t ii=0;ii<numr;ii++){
    QEvent *ev=GetEvent(ii);
    Long64_t numt=qch->GetEntries();//tree entries
    if((numt!=numr)||(ii>=numt)){return -1;}//raw files entries changed
    if(ev==0)continue;
    unsigned int prun=ev->run;
    double ptime=double(ev->time[0])+double(ev->time[1])*1e-6;
    if(timelist.find(prun)==timelist.end()){
      timelist[prun].first=timelist[prun].second=ptime;
    }
    else {
     if(ptime<timelist[prun].first)timelist[prun].first=ptime;
     if(ptime>timelist[prun].second)timelist[prun].second=ptime;  
    }
  }
  return timelist.size()>=1?timelist.size():-2;
}


//-------------------Generate Smear
int QAnalysisN::GenerateSmearMC(int seed,int isend){//seed should according to run/event

  if(GetQRunManager()->qisreal)return -1;//Must MC
  if(!rd)rd=new TRandom3(seed);
  double prext[6]={1000e-4,1000e-4,1000e-4,0.7e-3, 0.7e-3, 0.7e-3};
  map<int, pair<TVector3,TVector3> > dp;
  for(int iext=0;iext<2;iext++){
    int il=(iext==0)?0:8;
    for(int ipar=0;ipar<6;ipar++){
      int label=il*10+(ipar+1);
      if(label>=10000000){label=label%10000000;label=-label;}//negative tkid
      int id=label/10;//detid
      if(dp.find(id)==dp.end())dp[id]=make_pair(TVector3(0.,0.,0.),TVector3(0.,0.,0.));
      double var=rd->Gaus(0,prext[ipar]);
      if(ipar>=0&&ipar<3){(dp[id].first)[ipar]=var;}
      else               {(dp[id].second)[ipar-3]=var;}
    }
  }
  int ia=1;
//-----
  TkAlignParDB &paligndb=tkraw->aligndb[ia];
  map<unsigned int, TkAlignParN> &palign=paligndb.aparns;
  if((palign.size()>=999)||(palign.size()>=1&&isend)){
    paligndb.CopyToTDVBlock();
    TString millres=ofiles;
    millres.ReplaceAll(".root","_t");
    millres+=((unsigned int *)(paligndb.TDVBlock))[0];
    millres+=".gen";
    paligndb.StreamAlignTDV(millres.Data());
    aligngen.push_back(millres); 
    palign.clear();
  }
  if(isend)return 10;
//-----
  TkAlignParN &ppar=palign[btime];
  ppar.Clear();
  ppar.apar=dp;
  ppar.atime[0]=btime;
  ppar.atime[1]=btime;
  return tkraw->UpdateAlignModule(btime,ia,1);//Adding smearing for MC
}


int QAnalysisN::InitAlignment(int isend){
  int ia=1;
  TkAlignParDB &paligndb=tkfinal->aligndb[ia];
  map<unsigned int, TkAlignParN> &palign=paligndb.aparns;
  if((palign.size()>=999)||(palign.size()>=1&&isend)){
    TString millres=ofiles;
    millres.ReplaceAll(".root","_t");
    millres+=((unsigned int *)(paligndb.TDVBlock))[0];
    millres+=".res";
    paligndb.StreamAlignTDV(millres.Data());
    alignres.push_back(millres);
    palign.clear();
  }
  if(isend)return 10;
  if(milleFile)return 0;
  if(abs(ialign)>=1&&(palign.size()==0)){
    TString millres=ofiles;
//----
    TString app("_");
    app+=abs(ialign);
    app+=".root";
    TString app1("_");
    app1+=abs(ialign)-1;
    app1+=".root";
    millres.ReplaceAll(app,app1);
//----
    millres.ReplaceAll(".root","_t");
    millres+=btime;
    millres+=".res";
    int force0=0;
    int wopt=(ialign<0)?3000:0;//ialign<0 add spline correction in writting
    int stat=paligndb.LoadAlignPar(millres.Data(),0,btime,force0,0,wopt);
    if(stat<0)return stat;//error flag
  }
//----
  int force=1,um=-1;
  if(ialign==0)um=10;//only load spline,no aparns
  else {  
    if(palign.find(btime)==palign.end()){
      cerr<<"Error TkTrackN::LoadAlignModule not find btime="<<btime<<endl;
      return -2;//set to error if time not exactly match
    }
  }
  int stat=tkfinal->UpdateAlignModule(btime,ia,force,um);
  if(stat<0)return stat;
//------
  TString milldata="dynmille.bin";
  milleFile=new gbl::MilleBinary(milldata.Data());
  return 1;
}

////-------------------InitAlignmentS(spline)
int QAnalysisN::InitAlignmentS(unsigned int utime){
  if(splinelist.size()==0){
#ifdef AFTERSPLINE2
    const char *alignfiles="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth_All/dyalignf_splinefit.list";  
#else
    const char *alignfiles="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth/dyalignf_splinefit.list";
#endif
    ifstream fbin(alignfiles);
    if(!fbin){cerr<<"Error QAnalysisN::InitAlignmentS: can not open "<<alignfiles<<endl; return -10;}
    unsigned int stime[2];
    string sfile;
    while(fbin.good()&&!fbin.eof()){
      fbin>>stime[0]>>stime[1]>>sfile;
      if(!fbin.good()){continue;}
      splinelist[stime[0]]=sfile;
    }
    fbin.close();
    cout<<"Load splinelist size="<<splinelist.size()<<endl;
    if(splinelist.size()==0){cerr<<"error splinelist size=0"; return -10;}
    stimer[0]=stimer[1]=0;
  }
  int ia=1;
  if((stimer[0]==0&&stimer[1]==0)||utime<stimer[0]||(stimer[1]!=0&&utime>=stimer[1])){
    map<unsigned int,string>::iterator it=splinelist.upper_bound(utime);
    stimer[1]=(it==splinelist.end())?0:it->first;
    if(it!=splinelist.begin())it--;
    stimer[0]=(it==splinelist.begin())?0:it->first;
//    cout<<"utime="<<utime<<" rt="<<stimer[0]<<","<<stimer[1]<<endl; 
    TkAlignParDB &paligndb=tkfinal->aligndb[ia];
//    int force0=0;
    int force0=(paligndb.aparns.size()>=1)?-1:0;//previous load still be kept by default
    int wopt=(ialign<0)?3000:0;//ialign<0 add spline correction in writting
    int stat=paligndb.LoadAlignPar((it->second).c_str(),0,utime,force0,0,wopt);
    if(stat<0)return stat;
  }
  int force=0,um=-1;
  if(ialign==0)um=10;//only load spline,no aparns
  return tkfinal->UpdateAlignModule(utime,ia,force,um);
}


//-------------------Do Alignment
int QAnalysisN::DoAlignment(){
  if(!milleFile)return 0;
  delete milleFile;
  milleFile=0;
  int ia=1;
  cout<<"process ialign="<<ialign<<" ccev="<<ccev<<" sumev="<<sumev<<" time="<<btime<<","<<etime<<" run="<<prrun<<endl;
  hman1.Fill("accev",ccev);
  hman1.Fill("adt",double(etime)-double(btime));
  TkAlignParDB &paligndb=tkfinal->aligndb[ia];
  bool nevcut=1;
#ifdef USEFINAL
  if(ccev<alignnev/3)nevcut=0;
#else
  if(ccev<alignnev/100)nevcut=0;
#endif
  if(!nevcut){//too few event failed, have to return
    cout<<"ialign="<<ialign<<" failed with too few events="<<ccev<<"/"<<alignnev<<" sumev="<<sumev<<" time="<<btime<<","<<etime<<" run="<<prrun<<endl;
    if(abs(ialign)>=1){
      map<unsigned int, TkAlignParN> &palign=paligndb.aparns;
      map<unsigned int, TkAlignParN>::iterator it=palign.find(btime);//find time in the list copy from earlier results
      if(it!=palign.end()){//failed but exist in current list
        cerr<<"warring failed ialign="<<ialign<<" would still copy earlier result<="<<(ialign-1)<<" sumev="<<sumev<<" btime="<<btime<<endl;
//        if(abs(ialign)==1)palign.erase(it);//we can also choose to erase it from current list
      }
    }
    return 0;
  }
  const char *comd=0;
#if defined (USET3CHISCUT)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt1.txt";
  else if(abs(ialign)==2)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt2.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt3.txt";
#elif defined (USET2CHISCUT)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt1.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrtt2.txt";
#elif defined (USET1CHISCUT)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrt.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrt1.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstrt2.txt";
#elif defined (USETCHISCUT)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr1.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr20.txt";
#elif defined (USET0CHISCUT)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr1.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr1.txt";
#else
#if defined (AFTERSPLINE) && defined (USEFIXPAR3)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr_fixpar3.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr1_fixpar3.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr2_fixpar3.txt";
#elif defined (AFTERSPLINE) && defined (USEFIXPAR2)
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr_fixpar2.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr1_fixpar2.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr2_fixpar2.txt";
#else 
  if     (ialign==0)     comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr.txt";
  else if(abs(ialign)==1)comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr1.txt";
  else                   comd="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/pede -i /afs/cern.ch/work/q/qyan/work/alignment/dynalignstr2.txt";
#endif
#endif
  char comda[2000];
  strcpy(comda,comd);
#ifdef USEFINAL
  if(ialign<0)strcat(comda," | (sed -u 5q;tail -n51)");
  else        strcat(comda," | (sed -u 2q;tail -n4)");
#endif
  int icom=-10,nd=5;
  for(int id=0;id<nd;id++){
    if(id<=1)icom=system(comda);
    else {
      cout<<"try pede="<<id<<" comd="<<comd<<endl;
      icom=system(comd);
    }
    if(icom==0)break;
    sleep(20);
  }
  cout<<"ialign="<<ialign<<" pede stat="<<icom<<endl;
  int force=(ialign==0)?-1:-2;
  int wopt=(ialign<0)?3000:0;//ialign<0 add spline correction in writting
  wopt+=300;//has error3
  int nm=paligndb.LoadAlignPar("millepede.res",0,btime,force,etime,wopt);//applying alignment
  if(nm>=0&&icom!=0){nm=-10;}
  if(nm<0){cerr<<"ialign="<<ialign<<" error pede "<<icom<<" nm="<<nm<<endl;}
//---------
  return nm;
}

//-------------------Select Event
int QAnalysisN::SelectEvent(QEvent *ev,int pos){

  clock_t tb,te,tb1,te1;
  if(ev->isreal){//Data
    if(btime==0||ev->run!=prrun||ev->time[0]>=aligntime+1e-6){
//-----
      tb=clock();
      int astat=DoAlignment();btime=ev->time[0];ccev=0;
      te=clock();
      cput[0][2]+=te-tb;
      if(astat<0)return -1000;//fatal error stop loop all events
//-----
      tb=te;
      initalign=InitAlignment();
      te=clock();
      cput[0][3]+=te-tb;
//-----
      if(btime==0||ev->run!=prrun){//new run: recalculate aligndt[1]
        double trun=timelist[ev->run].second-timelist[ev->run].first;
        int nt=int(trun/aligndt[0]+0.5);//time dt tunning: adjust to integral
        if(nt<=1)nt=1;
        aligndt[1]=trun/nt;
        if(aligndt[1]<1)aligndt[1]=1;//should>1s
        aligntime=timelist[ev->run].first;
      }
      while(aligntime<ev->time[0]+aligndt[1]/2.){aligntime+=aligndt[1];}//next time
    }
    etime=ev->time[0];
    prrun=ev->run;
    tb=clock();
#ifdef AFTERSPLINE
    if(initalign>=0)initalign=InitAlignmentS(ev->time[0]);
#endif
    te=clock();
    cput[0][4]+=te-tb;
  }
  else {//MC
    if(btime==0||sumev-bev>=alignnev){
//-----
      tb=clock();
      int astat=DoAlignment();btime++;bev=sumev;ccev=0;
      te=clock();
      cput[0][2]+=te-tb;
      if(astat<0)return -1000;//fatal error stop loop all events
//-----
      tb=te;
      initalign=InitAlignment();
#ifndef NOMCSMEAR
      GenerateSmearMC(ev->run);
#endif
      te=clock();
      cput[0][3]+=te-tb;
//-----
    }
    etime=btime; 
  }
  

//---Weight factor
  static char histn[1000];
  double ww=1;
  float mrig=ev->mmom/ev->mch;
  int   tk_zm=ev->mch;
  double mrigm=fabs(mrig);
  if(!ev->isreal){
#if defined (USEHIGHE)
    if(!((mrigm<100)||(mrigm>1000&&mrigm<1030)))return 0;
#elif defined (USELOWE)
    if(!((mrigm<10)||(mrigm>1000&&mrigm<1030)))return 0;
#else
    if(!((mrigm<30)||(mrigm>1000&&mrigm<1030)))return 0;
#endif
  }
//----
  if(ev->Select_Particle())hman1.Fill("tofbetah",ev->tof_betah);

//---Trigger
  sprintf(histn,"seleffp_b0");
  hman1.Fill(histn,0);
//  cout<<"run="<<ev->run<<" time="<<ev->time[0]<<endl;
//  cout<<"isreal="<<ev->isreal<<endl;
  if(!ev->isreal){//MC apply trigger selection
    if(!ev->Select_Trigger())return 1;
  }
  hman1.Fill(histn,1);

//---Particle(TOF+Tk) 
  if(!ev->Select_Particle())return 1;
  hman1.Fill(histn,2);

//---TOFBeta
  if(!ev->Select_TofBeta())return 1;
  hman1.Fill(histn,3);
  if(ev->tof_betah<0.4)return 1;
  hman1.Fill(histn,4);

//---InnerNHit
   int nplane=ev->GetTkInNPlane(1);
   if(ev->GetTkInNHit(1)<5||ev->GetTkInNHit(0)<3||nplane<3){return 1;}//>=3 Yplans && >=3 XHits &&>=5 Hits
//   if(ev->GetTkInNHit(1)<4||ev->GetTkInNHit(0)<3||ev->GetTkInNPlane(1)<3){return 1;}//>=3 Yplans && >=3 XHits &&>=5 Hits
//   if(ev->GetTkInNHit(1)<5||ev->GetTkInNHit(0)<2||ev->GetTkInNPlane(1)<4){return 1;}//4 plans  
   hman1.Fill(histn,5);

//----
   bool cutl1hzt=(ev->HasTkLHitXY(0)>=1);
   bool cutl9hzt=(ev->HasTkLHitXY(8)>=1);
   bool extcut=1;
#ifdef USEEXTALIGN
   if(!cutl1hzt&&!cutl9hzt){extcut=0;return 1;}
#endif
   hman1.Fill(histn,6);

//---InnerZ
   int  tk_z1=ev->GetTkInZ(2);
//--Normal Cut
   bool parqcut=(ev->Select_TkInQ(0,tk_z1)&&ev->Select_TofUQ(0,tk_z1)&&ev->Select_TofDQ(0,tk_z1));
   if(parqcut)hman1.Fill(histn,7);
   if(tk_z1<=2){//tight TOFQ cut
     parqcut=(parqcut&&ev->GetTOFUDQ(0)<tk_z1+1.&&ev->GetTOFUDQ(1)<tk_z1+1.);
   }
   if(parqcut)hman1.Fill(histn,8);
   bool partkcut=(ev->GetChis(1)<10000&&ev->GetChis(0)<10000);//~0.2cm
   if(parqcut&&partkcut)hman1.Fill(histn,9);
//-----
   cutl1hzt=(cutl1hzt&&ev->GetTkLQ(0,2)>tk_z1-0.4&&ev->GetTkLQ(0,2)<tk_z1+1.);
   cutl9hzt=(cutl9hzt&&ev->GetTkLQ(8,2)>tk_z1-0.4&&ev->GetTkLQ(8,2)<tk_z1+1.);
#ifdef USEEXTALIGN
   if(!cutl1hzt&&!cutl9hzt)extcut=0;
#endif
   if(parqcut&&partkcut&&extcut)hman1.Fill(histn,10);
//-----following is additional clean selection
   bool cutttof=1;
   if(tk_z1<=1)cutttof=(ev->GetTOFUDQ(1)<tk_z1+0.5);//apply only to proton
   if(parqcut&&partkcut&&extcut&&cutttof)hman1.Fill(histn,11);
//-----
   bool cuttktight=1;
   if(tk_z1<=1)cuttktight=(ev->ntrack==1);//apply only to proton
   if(parqcut&&partkcut&&extcut&&cutttof&&cuttktight)hman1.Fill(histn,12);
//----
   bool cut4plane=(nplane>=4);
   if(parqcut&&partkcut&&extcut&&cutttof&&cuttktight&&cut4plane)hman1.Fill(histn,13); 
//-------
   for(int ixy=0;ixy<2;ixy++){
     sprintf(histn,"chisoxy%d",ixy);
     hman1.Fill(histn,ev->GetChis(ixy));
   }
   for(int iud=0;iud<2;iud++){
     sprintf(histn,"tofqud%d",iud);
     hman1.Fill(histn,ev->GetTOFUDQ(iud));
   }
   hman1.Fill("tkinq",ev->GetTkInQ(2));
   if(ev->HasTkLHitXY(0)){
     hman1.Fill("tkl1q",ev->GetTkLQ(0,2));  
   }
   if(ev->HasTkLHitXY(8)){
     hman1.Fill("tkl9q",ev->GetTkLQ(8,2));
   }

///--Selection
   if(parqcut&&partkcut&&extcut&&(tk_z1>=1&&tk_z1<=2)){//only use proton and helium
//-------
     sumev++;
//---alignment must be initialed
     if(initalign<0)return 100;
//---
     int ib=0;
     if(!ev->isreal){
       bool issurvl8=(ev->mevmom1[13]!=-1000||ev->mevmom1[14]!=-1000);
       if(!issurvl8)ib=1;//background
#ifdef USESURVIVEL9
       bool issurvl9=(ev->mevmom1[18]!=-1000||ev->mevmom1[19]!=-1000);
       if(!issurvl9)return 100;
#endif
     }
#ifdef EXCLUDELOWE
     if(!ev->isreal&&mrigm<5)return 101;
#endif
#if defined (USECLEAN) || defined (USECLEAN2) || defined (USECLEAN3) || defined (USECLEANA)
     if(!cutttof)return 102;//low TOFQ<Z+0.5
#endif
#if defined (USECLEAN2) || defined (USECLEANA)
     if(!cuttktight)return 103;//1 track
#endif
#if defined (USECLEAN3) || defined (USECLEANA)
     if(!cut4plane)return 104;//>=4 planes
#endif
#if defined (USEHTOFBETA)
     if(fabs(ev->tof_betah)<0.8)return 105;//tof beta>0.8
#endif
//     cout<<"ev="<<ev->run<<","<<ev->event<<" tk_z1="<<tk_z1<<" beta="<<ev->tof_betah<<endl;
     map<int, AMSPlaneM> plrs;
     map<int, AMSPlaneM> plms;
     map<int, AMSPlaneM> plos;
     int inlast=-1;
     int nhiti[2]={0,0};
     int ladidl[9]={0};
     int senidl[9]={0};
     float chrg=tk_z1;
     double mass=TrFit::Mproton;
//-----
     tb=clock();
     for(int il=0;il<9;il++){
       int hashit=ev->HasTkLHitXY(il);
       if(hashit==0)continue;
//----
       if(il==0){if(!cutl1hzt)continue;}
       if(il==8){if(!cutl9hzt)continue;}
//----
       int tkid=ev->tk_lid[il];
       if(ladnev.find(tkid)==ladnev.end()){ladnev[tkid]=1;if(hashit==2)ladnevx[tkid]=1;}
       else {ladnev[tkid]++;if(hashit==2)ladnevx[tkid]++;}
       double chanx=ev->tk_lch[il][0];
       double chany=ev->tk_lch[il][1];
       if(tkid==0||chanx<0||chany<0)continue;
//----
/*       if(!ev->isreal){
         const int nladrej=6;
         int ladrej[]={-202,-302,-602,614,-702,714};
         for(int k=0;k<nladrej;k++){if(tkid==ladrej[k])return 7;}//reject these ladders as in TB
       }*/
       int tkid2=(tkid>0)?((tkid/100%100-1)*20+tkid%100):((tkid/100%100+1)*20+tkid%100);
       ladidl[il]=tkid2;
       TkLadderN *ladder=(TkLadderN *)tkfinal->modules_[tkid];
       AMSPlaneM p00=ladder->GetLocalCoo(chanx,chany);
       int senid2=(tkid2>=0)?tkid2*20+abs(p00.getid())%100:tkid2*20-abs(p00.getid())%100;
       senidl[il]=senid2;
       for(int ixy=1;ixy<=hashit;ixy++){
         sprintf(histn,"ladnev_z%dxy%d",tk_z1,ixy);
         hman1.Fill(histn,tkid2);
         int uxy=(ixy==1)?1:0;
       }
       for(int ixy=0;ixy<2;ixy++){
         sprintf(histn,"ladpos_xy%d",ixy);
         hman1.Fill(histn,tkid2,p00.getMGlobal()[ixy]);
         sprintf(histn,"laddir_xy%d",ixy);
         hman1.Fill(histn,tkid2,ev->tk_dir[il][ixy]/ev->tk_dir[il][2]);
#ifdef USEFAST
#ifdef USEEXTALIGN 
         sprintf(histn,"senpos_xy%d",ixy);
         hman1.Fill(histn,senid2,(ixy==0)?p00.getM().X():p00.getM().Y());
         sprintf(histn,"sendir_xy%d",ixy);
         hman1.Fill(histn,senid2,ev->tk_dir[il][ixy]/ev->tk_dir[il][2]);
#endif
#endif
       }
//------
       AMSPlaneM plmr=(ev->isreal)?tkfinal->GetGlobalCoo(tkid,chanx,chany,111):tkraw->GetGlobalCoo(tkid,chanx,chany,0);
       AMSPlaneM plrr=plmr;
       AMSPlaneM plor(TVector3(0.,0.,ev->tk_hitc[il][2]), TVector3(1,0,0), TVector3(0,1,0),il);
       plor.setM(ev->tk_hitc[il][0],ev->tk_hitc[il][1]);
//       cout<<"id="<<plmr.getid()<<endl;
       for(int ixy=1;ixy<=hashit;ixy++){
         sprintf(histn,"sennev_z%dxy%d",tk_z1,ixy);
         hman1.Fill(histn,tkid2,abs(plmr.getid())%100);
       }
       if(!ev->isreal){//MC adding smearing
         if(ev->mtrmom[il]==0)continue;
         AMSPoint p0(ev->mtrcoo1[il][0],ev->mtrcoo1[il][1],ev->mtrcoo1[il][2]);
         AMSDir pdir(ev->mtrdir1[il][0],ev->mtrdir1[il][1],ev->mtrdir1[il][2]);
         double prig=ev->mtrmom[il];//has to divided by Z
         TrProp prop=TrProp(p0,pdir,prig);
         double nrig;
//--without smearing
         double slengr=prop.Interpolate(plmr,nrig);
//--with the smearing
         AMSPlaneM plmm=tkraw->GetGlobalCoo(tkid,chanx,chany,111);
         double slengm=prop.Interpolate(plmm,nrig);
//------
         if(slengr<-999||slengm<-999)continue;
         TVector3 sloc=plmm.getPLocal()-plmr.getPLocal();
//         cout<<"il="<<il<<" prig="<<prig<<" mrig="<<mrig<<" smear="<<sloc[0]<<","<<sloc[1]<<","<<sloc[2]<<" sleng="<<slengr<<","<<slengm<<endl;
         plmr.moveM(sloc[0],sloc[1]);//adding smearing to the original (would have bias for the matrix in gcalib)
         plrr=plmr;
//-----Find new channel in the smeared geometry
         TkSensorN *sensor=(TkSensorN *)tkraw->modules_[plmr.getid()];
         double disxy[2];
         double senchanx,senchany;
         for(int ixy=0;ixy<2;ixy++){//try to find new matched channel
           disxy[ixy]=sensor->FindLocalChan((ixy==0)?senchanx:senchany,ixy,(ixy==0)?plmr.getM().X():plmr.getM().Y());//new chan
         }
         double dist=sqrt(disxy[0]*disxy[0]+disxy[1]*disxy[1]);
         double ladchanx=(dist==0)?sensor->GetInLadderChanX(senchanx):-2;
//-------current geometry measurment 
         if(ladchanx>-1){
           AMSPlaneM plma=tkfinal->GetGlobalCoo(tkid,ladchanx,senchany,111);
           plmr=plma;
           plrr=tkraw->GetGlobalCoo(tkid,ladchanx,senchany,111);
         }
         else {
           AMSPlaneM plma=tkfinal->GetGlobalCoo(tkid,chanx,chany,111);
           plmr.setO(plma.getO());
           plmr.setUV(plma.getU(),plma.getV());
           plrr.setO(plmm.getO());
           plrr.setUV(plmm.getU(),plmm.getV());
         }
//------           
       }
       double errx = 25.e-4;
       double erry = 13.e-4;
       if(chrg>=2){mass = TrFit::Mhelium/2*chrg; errx=25.e-4*0.65 ; erry = 6.5e-4;}
       if(il==0)  {errx = std::sqrt(errx*errx+9.0e-4*9.0e-4);  erry = std::sqrt(erry*erry+9.0e-4*9.0e-4);}
       if(il==9-1){errx = std::sqrt(errx*errx+11.5e-4*11.5e-4);erry = std::sqrt(erry*erry+11.5e-4*11.5e-4);}
       float ferx=(hashit>=2) ? 1 : 0;
       float fery=(hashit>=1) ? 1 : 0;
       AMSPoint ferr(ferx*errx,fery*erry,0);
       for(int ixy=0;ixy<2;ixy++){plmr.setMCov(ixy,ixy,ferr[ixy]*ferr[ixy]); plor.setMCov(ixy,ixy,ferr[ixy]*ferr[ixy]);}
//       cout<<"il"<<il<<" errx="<<ferr[0]<<" erry="<<ferr[1]<<endl;
//#if defined (USELAYERS)
       std::vector<std::pair<int,TMatrixD> > *malignder=plmr.getmalignder();
       malignder->erase(malignder->begin(),malignder->begin()+2);//erase frist two
#ifdef USEEXTALIGN
       if(il!=0&&il!=8)malignder->clear();
#endif
/*#elif defined (USESENSORS)
       std::vector<std::pair<int,TMatrixD> > *malignder=plmr.getmalignder();
       malignder->erase(malignder->begin()+1,malignder->begin()+3);//erase ladder+layer
#elif defined (USELADDERS)     
       std::vector<std::pair<int,TMatrixD> > *malignder=plmr.getmalignder();
       malignder->erase(malignder->begin());//erase sensor and layer
       malignder->erase(malignder->begin()+1);
#endif*/
       plms[il]=plmr;
       plrs[il]=plrr;
       plos[il]=plor;
       if(il>=1&&il<8){inlast=il;if(hashit>=2)nhiti[0]++;if(hashit>=1)nhiti[1]++;}
     }
     if(inlast>0){
       sprintf(histn,"selmom_z%d",tk_z1);
       hman1.Fill(histn,(ev->isreal)?ev->GetRigidity():ev->mtrmom[inlast]);//Data or MC
       sprintf(histn,"selmom1_z%d",tk_z1);
       hman1.Fill(histn,(ev->isreal)?ev->GetRigidity():mrig);//Data or MC
     }
     te=clock();
     cput[0][5]+=te-tb;
//-----
     if(nhiti[0]<3||nhiti[1]<5)return 5;//Hit cut
     tb=te;
     double rrgt0=0;
     double chis0[3]={0};
     int uir=3;//maxspan
     for(int ir=-1;ir<4;ir++){//different fitting(ir==3 max-span)
#ifdef USEEXTALIGN
       if(ir==uir){//require external layers in the alignment
         if(plms.find(0)==plms.end()&&plms.find(8)==plms.end())continue;
       }
#endif
       if(ialign>0){if(!(ir==-1||ir==0||ir==uir))continue;}//ialign==0||ialign<0 would fit all
#ifdef USEFAST
       if(ev->isreal){if(!(ir==-1||ir==uir))continue;}//data only accept (ir==-1||ir==uir)
#ifndef USERIGLIMIT
       if(ir==-1&abs(ialign)>=1){continue;}
#endif
#endif
#ifdef AFTERSPLINE
       if(ir==-1)continue;
#endif
       if(ir==1||ir==2){if(plms.find(0)==plms.end())continue;}//without L1
       if(ir==2){if(plms.find(8)==plms.end())continue;}//wihout L9
       TrFit ptrfit;
       TrFit otrfit;
       for(int il=0;il<9;il++){
         if(plms.find(il)==plms.end())continue;
         if(!(ir==1||ir==2||ir==3||(ir==-1&abs(ialign)>=1))){if(il==0)continue;}
         if(!(ir==2||ir==3||(ir==-1&&abs(ialign)>=1))){if(il==8)continue;}
         ptrfit.Add(plms[il]);
         otrfit.Add(plos[il]);
       }
       int method = TrFit::GBLFITTERAMS;
       int mscat=1;
       int eloss=0;
       double beta=0;
       double rrgt=0;
       if(ir==uir){
         mscat=2;
         rrgt=rrgt0;
#ifdef USERIGLIMIT
         if(abs(ialign)<=2){
           if     (rrgt>20)rrgt=20;
           else if(rrgt<-20)rrgt=-20;
         }
#endif
         if(!ev->isreal){
#if defined (USENORIGWEIGHT0)
           mscat=2;
           rrgt=mrig;//fix MS fitting
#elif defined (USENORIGWEIGHT)
           mscat=1;
           rrgt=mrig;//fix rigidity fitting
#endif
         }
         if(rrgt==0)mscat=1;//rigidity determined by itself
       }
//       rrgt=0;
       int uiter=0;//default
//       int uiter=-2;//force 2 iterations
#ifdef USEOTRFIT
       ptrfit=otrfit;
#endif
       tb1=clock();
       float fdone=ptrfit.DoFit(method,mscat,eloss,chrg,mass,beta,rrgt,uiter);
       te1=clock();
       cput[1][0]+=te1-tb1;
       bool done = (fdone>=0&&ptrfit.GetChisqX()>=0&&ptrfit.GetChisqY()>=0&&ptrfit.GetNdofX()>=1&&ptrfit.GetNdofY()>=1);//additional degree
       if(ir==-1){//inner or max-span rigidity
         if(!done)return 7;
         rrgt0=ptrfit.GetRigidity();
         hman1.Fill("cosrig",rrgt0);
         continue;
       }
       else if(ir==0){//inner rigidity
         if(!done)return 8;
#if defined (USEINNERCHISCUT)
         if(ptrfit.GetChisq()>10)return 8;
#elif defined (USEINNERCHISCUT1)
         if(ptrfit.GetChisqX()/ptrfit.GetNdofX()>10)return 8;
#endif
         chis0[0]=ptrfit.GetChisqX();
         chis0[1]=ptrfit.GetChisqY();
         chis0[2]=ptrfit.GetChisq();
       }
       else if(ir==uir&&abs(ialign)>=3){
         if(!done)continue;
#if defined (USEMAXSPCHISCUT)
         if(ptrfit.GetChisq()>10)continue;
#elif defined (USEMAXSPCHISCUT1)
         if(ptrfit.GetChisqX()/ptrfit.GetNdofX()>10)continue;
#endif
       }
       if(!done)continue;
//-----residual
       sprintf(histn,"cosrigr%d",ir);
       hman1.Fill(histn,ptrfit.GetRigidity()); 
       for(int ixy=0;ixy<2;ixy++){
         sprintf(histn,"chiso2xy%d",ixy);
         if(ir==uir)hman1.Fill(histn,ev->GetChis(ixy));
       }
       for(int ixy=0;ixy<3;ixy++){
         double pchis=0;
         if(ixy==0){
           if(ptrfit.GetNdofX()>0)pchis=ptrfit.GetChisqX()/ptrfit.GetNdofX();
         }
         else if(ixy==1){
           if(ptrfit.GetNdofY()>0)pchis=ptrfit.GetChisqY()/ptrfit.GetNdofY();
         }
         else  pchis=ptrfit.GetChisq();
         if(pchis>0)pchis=sqrt(pchis);
         sprintf(histn,"chisr%dz%dxy%d",ir,tk_z1,ixy);
         hman1.Fill(histn,pchis);
       }
       for(int il=0;il<9;il++){
#ifdef USEEXTALIGN
         if(il!=0&&il!=8)continue;
#endif
         int ihit=ptrfit.iHitJ(il+1);
         if(ihit<0)continue;
         TVector3 pld,res;
         if(!ev->isreal){
           AMSPlane plm,plr;
           if(plms.find(il)!=plms.end()&&plrs.find(il)!=plrs.end()){
             plm=plms[il];
             plr=plrs[il];
             ptrfit.InterpolateGbl(plm);
             ptrfit.InterpolateGbl(plr);
           }
           pld=plm.getPLocal()-plr.getPLocal();
           res=pld[0]*plr.getU()+pld[1]*plr.getV();
         }
//         cout<<"pld="<<pld[0]<<","<<pld[1]<<","<<pld[2]<<endl;
         for(int ixy=0;ixy<2;ixy++){
           double nrs=(ixy==0)?ptrfit.GetXr(ihit):ptrfit.GetYr(ihit);;
           if(nrs==0)continue;
           sprintf(histn,"res_r%dz%dl%dxy%d",ir,tk_z1,il,ixy);
           hman1.Fill(histn,nrs*1e4);
           if(!ev->isreal){
             sprintf(histn,"resr_r%dz%dl%dxy%d",ir,tk_z1,il,ixy);
             hman1.Fill(histn,mrig,nrs*1e4);
             sprintf(histn,"mis_r%dz%dl%dxy%d",ir,tk_z1,il,ixy);
             hman1.Fill(histn,pld[ixy]*1e4);
             sprintf(histn,"mis1_r%dz%dl%dxy%d",ir,tk_z1,il,ixy);
             hman1.Fill(histn,res[ixy]*1e4);
             sprintf(histn,"misr_r%dz%dl%dxy%d",ir,tk_z1,il,ixy);
             hman1.Fill(histn,mrig,pld[ixy]*1e4);
             sprintf(histn,"mis1r_r%dz%dl%dxy%d",ir,tk_z1,il,ixy);
             hman1.Fill(histn,mrig,res[ixy]*1e4);
           }
           if(ir!=uir)continue;
#ifdef USEFAST
#ifdef USEEXTALIGN 
           sprintf(histn,"senres_xy%d",ixy);
           hman1.Fill(histn,senidl[il],nrs*1e4);
#endif
#endif
           sprintf(histn,"ladres_xy%d",ixy);
           hman1.Fill(histn,ladidl[il],nrs*1e4);
           sprintf(histn,"ladres1_xy%d",ixy);
           if(!ev->isreal)hman1.Fill(histn,ladidl[il],pld[ixy]*1e4);
         }
       }
//------
       if(!ev->isreal){
         sprintf(histn,"srigr%d_q%d",ir,tk_z1);
         hman1.Fill(histn,mrig,1000.*(1./ptrfit.GetRigidity()-1./mrig));
       }
//----abc
#if defined (USEOTRFIT)
       continue;
#endif
       if(ir!=uir)continue;
#ifdef EXCLUDEHIGHE
       if(!ev->isreal&&mrigm>500)return 2;
#endif
//       cout<<"ir="<<ir<<" frig="<<ptrfit.GetRigidity()<<","<<mrig<<endl;
       if(!ptrfit.gblfitter)continue;
       genfit::GblFitter* gblfit=dynamic_cast<genfit::GblFitter*> (ptrfit.gblfitter);
       if(!gblfit)continue;
       if(!milleFile)continue;
       tb1=clock();
       gbl::GblTrajectory* traj=gblfit->getGblInfo();
//       unsigned int printlevel=1;
       unsigned int printlevel=0;
       traj->milleOut(*milleFile,printlevel); 
       for(int im=0;im<ptrfit.GetNhit();im++){
         AMSPlaneM *plm=ptrfit.GetHit(im);
         TVector3 dloc=plm->getDLocal();
         std::pair<std::vector<int>,TMatrixD> mat=plm->getResAlignDerGlobal(dloc); 
//----
         for(int ic=0;ic<mat.second.GetNrows();ic++){
           for(int ip=0;ip<mat.second.GetNcols();ip++){
//             cout<<"im="<<im<<" ic="<<ic<<" dlab="<<(mat.first)[ip]<<","<<(mat.second)(ic,ip)<<endl;
           }
         }
//------
       }
       ccev++;
       te1=clock();
       cput[1][1]+=te1-tb1;
//------
     }
     te=clock();
     cput[0][6]+=te-tb;
   }
     
  
  return 0;
}


int QAnalysisN::Save(const char *odir){
//----Save Alignment Par
  InitAlignment(1);
  if(qrunmanager->qisreal==0)GenerateSmearMC(1,1);//only for MC
  hman1.Save();
  hman1.Clear();
  if(odir){
    int cpstat=0;
    for(unsigned int i=0;i<alignres.size();i++){
      cpstat=cpofile(alignres[i].Data(),odir,0);
      if(cpstat!=0)return 3;//cp error
    }
    for(unsigned int i=0;i<aligngen.size();i++){
      cpstat=cpofile(aligngen[i].Data(),odir,0);
      if(cpstat!=0)return 3;//cp error
    }
    cpstat=cpofile(ofiles.Data(),odir,0);
    if(cpstat!=0)return 3;//cp error
  }
  return 0;
}


int QAnalysisN::WriteRunList(const char *runlogdir){

//---MC return
  if(qrunmanager->qisreal==0||!runlogdir)return -1;
  cout<<"runlogdir="<<runlogdir<<endl;
  WriteList(runlogdir,qrunmanager->qprocessfile);
  return (qrunmanager->qprocessfile).size();
//---
/*  vector<unsigned int> runlist=qrunmanager->qprocessrunlist;
  for(vector<unsigned int>::iterator iter=runlist.begin(); iter!=runlist.end(); iter++ ){
    unsigned int runid=(*iter);
    cout<<"prun="<<runid<<endl;
  }
  cout<<"runlogdir="<<runlogdir<<endl;
  qrunmanager->SetFDir(runlogdir,runlogname);
  vector<unsigned int> runlistn;//new processed runs
  qrunmanager->GetNewRunList(runlist,runlistn);
  qrunmanager->InsertRunList(runlistn);
  if(runlistn.size()!=runlist.size()){cout<<"runlist diff="<<runlist.size()<<","<<runlistn.size()<<endl;return -2;}
  return runlistn.size();*/
}


int Sum1_AMS_CINT(AMSChain *ch,TChain *ch1,const char *outfile,const char *outdir,const char *runlogdir,Long64_t num=-1,int ialign=0){

   QAnalysisN *analysis=new QAnalysisN(ch1);
   analysis->ialign=ialign;
//----Book Histo
   analysis->BookHistos(outfile);
//----Exclude Bad Run
/*#if defined (USEP4ISS)
   analysis->SelectGoodRun(1);
#elif defined (USEBEFORENEWEXT)
   analysis->SelectGoodRun(5);
#elif defined (USEAFTERNEWEXT)
   analysis->SelectGoodRun(50);
#elif defined (USESHZD)
   analysis->SelectGoodRun(90);
#else
   analysis->SelectGoodRun();
#endif*/
///---Process
   clock_t tb,te,tb1,te1;
//-----
   tb=clock();
   int pstat=analysis->PreProcess();
   te=clock();
   analysis->cput[0][0]+=te-tb;
   if(pstat<0){if(pstat==-2)pstat=2;return pstat;}//zero runs,nstat=2
//-----
   tb=te;
   int nstat=analysis->ProcessEvents(num);//nstat=1|=128: assign fatal error
   te=clock();
   analysis->cput[0][1]+=te-tb;
//-----
   tb=te;
   if(nstat==0){
     int astat=analysis->DoAlignment();
     if(astat<0)nstat=4;//nstat=4: assign fatal error
   }
   te=clock();
   analysis->cput[0][2]+=te-tb;
//----
   tb=te;
   if(nstat==0){
     nstat=analysis->Save(outdir);//nstat=3: assign fatal error
     if(nstat==0&&ialign<0)analysis->WriteRunList(runlogdir);//only save the last one
   }
   te=clock();
   analysis->cput[0][7]+=te-tb;
   if(nstat)cerr<<"Sum1_AMS_CINT::error nstat="<<nstat<<endl;
   cout<<"nladder="<<analysis->ladnev.size()<<" nladderx="<<analysis->ladnevx.size()<<" sumev="<<analysis->sumev<<endl;
//----
   cout<<"<<---------Begin Static CPU-------------->>"<<endl;
   const char *sname[8]={
      "PreProces","ProcessEv","DoAlignment", "IntiAlignment","IntiAlignmentS","CooCal/Hist","RigFit/Write","Save",
   };
   for(int icut=0;icut<8;icut++)cout<<sname[icut]<<"   "<<(double(analysis->cput[0][icut]/CLOCKS_PER_SEC))<<endl;
   cout<<"<<-----------------"<<endl;
   const char *sname1[2]={
     "RigFit","RigWrite",
   };
   for(int icut=0;icut<2;icut++)cout<<sname1[icut]<<"   "<<(double(analysis->cput[1][icut]/CLOCKS_PER_SEC))<<endl;
   cout<<"<<---------End Static CPU-------------->>"<<endl;
//----- 
   delete analysis;
   return nstat;//nstat should be 0
}


