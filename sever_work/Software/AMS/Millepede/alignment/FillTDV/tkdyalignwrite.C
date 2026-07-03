//--author Q.Yan qyan@cern.ch fill data to database
#include <signal.h>
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
#include  "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include "root_RVSP.h"
#include "amschain.h"
#include "TkGeomN.h"
#include "timeid.h"
int main(int argc,char ** argv){//in_file ofile num

//--L1/L9 dynamic alignment based on ISS data
   int vers=1;
   const char *falign="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth/dyalignf_splinefit.list";
/*   int vers=2;
   const char *falign="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth_All/dyalignf_splinefit.list";*/
//---
   std::map<pair<unsigned int,unsigned int>,string> fitpar;
   ifstream ffit(falign);
   string s;
   while(std::getline(ffit,s)){
     istringstream iss(s);
     unsigned int ptime[2];
     string pname;
     if(!(iss>>ptime[0]>>ptime[1]>>pname))continue;
     pair<unsigned int,unsigned int> tn=make_pair(ptime[0],ptime[1]);
     fitpar[tn]=pname;
   }
   ffit.close();
   cout<<" size="<<fitpar.size()<<endl;


//---write TDV
  TkTrackN *tkgeom=TkTrackN::GetHead();
  tkgeom->UseAlignVersion(vers,10);
  TkAlignParDB &aligndb=(tkgeom->aligndb[1]);
  long int pretime=0;
  for(map<pair<unsigned int,unsigned int>,string>::iterator it=fitpar.begin();it!=fitpar.end();it++){
    long int btime=(it->first).first;
    long int etime=(it->first).second;
    cout<<"time="<<btime<<","<<etime<<" file="<<(it->second)<<endl;
//--Write TDV
    if(pretime!=0&&btime>pretime)cerr<<"missing alignment"<<pretime<<","<<btime<<endl;
    int stat=aligndb.LoadAlignPar((it->second).c_str());
    if(stat<0){cerr<<"err load="<<(it->second)<<endl;return -2;}
    aligndb.StreamAlignTDV();
    long int btimew=btime;
//-----manual add to the first,have to remove!!!
    if(it==fitpar.begin())btimew-=100;//the first
//-----
    aligndb.WriteAlignTDV(btimew,etime,vers);//write TDV
    if(it->first==fitpar.rbegin()->first){//write twice for the last element, extension to long-long
      aligndb.WriteAlignTDV(etime+1,1900000000-1,vers);
    }
    aligndb.aparss.clear();
    pretime=etime;
//----load from TDV to test
//    long int ptime=(btime+etime)/2;
/*    long int ptime=btime;
    cout<<"btime="<<btime<<" ptime="<<ptime<<endl;
    int force=0;
    aligndb.LoadAlignTDV(ptime,1,force);
    aligndb.StreamAlignTDV();*/
  }
  return 0;
}
