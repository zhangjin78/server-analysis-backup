///--Author Qi Yan (qyan@cern.ch)
///--A simple example of using AMSsoft for AMS data analysis
#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#include "HistoMan.h"


class AMSanalyze{

 public:
  HistoMan hman1;

 public:
  AMSanalyze(){}
  ~AMSanalyze(){};
  int BookHistos(const char *ofile);
  int ProcessEvent(AMSEventR* ev);
  int Save(){hman1.Save();return 0;}
};


int AMSanalyze::BookHistos(const char *ofile){

  hman1.Clear();
  hman1.Enable();
  hman1.Setname(ofile);
//---
  hman1.Add( new TH1F("nTrTrack","Number of Tracks; tracks; events",10,0,10));
  hman1.Add( new TH1F("Rigidity","Standard Rigidity; GV; events",100,0,1000));
  hman1.Add( new TH1F("RigidityGBL","Rigidity GBL; GV; events",100,0,1000));
  return 0;
}


int AMSanalyze::ProcessEvent(AMSEventR* ev){

  int nn=ev->nTrTrack();
  hman1.Fill("nTrTrack",nn);
  if(nn>0){
    TrTrackR* tr=ev->pTrTrack(0);
    hman1.Fill("Rigidity",tr->GetRigidity());
    int fcode=tr->iTrTrackPar(7,0,30);//GBL, max-span, V6-alignment
    if(fcode>=0)
      hman1.Fill("RigidityGBL",tr->GetRigidity(fcode));
  }
  return 0;
}


int analyze_CINT(AMSChain *ch, const char *outfile, Long64_t num=-1){

//---Init
  AMSanalyze *analyze=new AMSanalyze();

//---Book Histo
  AMSEventR::fRunList.clear();//Clear AMSEventR:fRunList
  analyze->BookHistos(outfile);

//---Process Events
  Long64_t num2= (ch->GetEntries()<num||num<0)?ch->GetEntries():num;
  for(Long64_t ii=0;ii<num2;ii++){
    if(ii%10000==0) printf("Processed %7lld out of %7lld\n",ii,num2);
    AMSEventR* ev=ch->GetEvent(ii);
    ev->RecordRTIRun();//Adding run to AMSEventR:fRunList
    analyze->ProcessEvent(ev);
  }

//---Histo Save
  analyze->Save();

//---Print Report
  for(map<unsigned int,AMSSetupR::RunI>::iterator it=AMSEventR::fRunList.begin();it!=AMSEventR::fRunList.end();it++){//Get Run by Run
    AMSSetupR::RunI runi=it->second; //run information
    unsigned int run=runi.run;
    cout<<"run="<<run<<endl;
    for(unsigned int k=0;k<runi.fname.size();k++){
      string fname=runi.fname[k]; 
      cout<<"  rootfile="<<fname<<endl;
    }
  }
  cout<<"Total Processed nrun="<<AMSEventR::fRunList.size()<<endl;


  return 0;
}
