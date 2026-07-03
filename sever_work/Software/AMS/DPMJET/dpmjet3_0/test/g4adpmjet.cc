//---standalone DPMJET test, created by Q.Yan qyan@cern.ch
#include "ADPMjet_Model.h"
#include "TLegend.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include <math.h>
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4NuclNuclDiffuseElastic.hh"
#include "G4SystemOfUnits.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4IonElasticPhysics.hh"
#include "G4HadronElasticProcess.hh"
#include "G4GGNuclNuclCrossSection.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4VModularPhysicsList.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4VUserDetectorConstruction.hh"
#include "ADPMcommon.h"
//-----
//#include "DetectorConstruction.hh"
//#include "PhysicsList.hh"
//#include "ActionInitialization.hh"
//#include "SteppingVerbose.hh"

int main(){

///--
  G4BosonConstructor pConstructorBS;
  pConstructorBS.ConstructParticle();

  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();
  
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();
  
  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();
  
  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();
  
  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();

//------
/*  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
 G4RunManager* runManager = new G4RunManager;
  DetectorConstruction* det = new DetectorConstruction;

  runManager->SetUserInitialization(det);
  PhysicsList* phys = new PhysicsList;
  runManager->SetUserInitialization(phys);

  runManager->SetUserInitialization(new ActionInitialization(det));*/
// 

 G4GenericIon::GenericIonDefinition() ;
 G4ParticleTable *ppart=G4ParticleTable::GetParticleTable();
 const G4IonTable *pIonT= ppart->GetIonTable();
 G4ProcessManager *gmanager=new G4ProcessManager(G4GenericIon::GenericIonDefinition());
 G4ParticleTable::GetParticleTable()->GetGenericIon()->SetProcessManager(gmanager);
//--
//---
// int up=0;
 int up=-1;
// int up=0;
 const int np=10;
 G4double p_pn[]={200,150,100,6.1,100,6.1,1000, 100,100,100};
// G4double p_pn[]={7,  7,  7,  7,  7,  7,  7,    7,  7,  7};
 G4int Zp[]     ={6,  80, 82 ,8,  8,  82, 82,   14, 2,  26};//C/Hg/Pb
 G4int Ap[]     ={12, 202,208,16, 16, 208,208,  28, 4,  56};
//------
 G4int Zt[]     ={6,  80, 82 ,6,  6,  82, 82,   6,  6,  6};
 G4int At[]     ={12, 202,208,12, 12, 208,208,  12, 12, 12};
//------
 char histn[100];
 const int nh=11;
 TH1D *h[nh][np];
 TH1D *hf[nh][np];
 int nbinz=83;
 double ranz[2]={2,85};
 int nbina=208;
 double rana[2]={2,210};
 for(int ip=0;ip<np;ip++){
   sprintf(histn,"fragzmap%d",ip);
   if(Zp[ip]<=2)h[0][ip]=new TH1D(histn,histn,3,1,4);
   else         h[0][ip]=new TH1D(histn,histn,nbinz,ranz[0],ranz[1]);
   sprintf(histn,"fragamap%d",ip);
   if(Zp[ip]<=2)h[1][ip]=new TH1D(histn,histn,7,1,8);
   else         h[1][ip]=new TH1D(histn,histn,nbina,rana[0],rana[1]);
   sprintf(histn,"fragzmap%dm",ip);
   if(Zp[ip]<=2)h[2][ip]=new TH1D(histn,histn,3,1,4);
   else         h[2][ip]=new TH1D(histn,histn,nbinz,ranz[0],ranz[1]);
   sprintf(histn,"fragnpar%d",ip);
   h[3][ip]=new TH1D(histn,histn,1000,0,1000);
   sprintf(histn,"fragen%d",ip);
   h[4][ip]=new TH1D(histn,histn,1000,p_pn[ip]*Ap[ip]*0.9,p_pn[ip]*Ap[ip]*1.1);
   sprintf(histn,"fragen%dv",ip);
   h[5][ip]=new TH1D(histn,histn, 1000,0.95,1.05);
   sprintf(histn,"frageva%dm",ip);
   h[6][ip]=new TH1D(histn,histn,1000,0,p_pn[ip]*1.2);
   sprintf(histn,"frageva%dvm",ip);
   h[7][ip]=new TH1D(histn,histn,1000,0,1.2);
   sprintf(histn,"fragamap%dm",ip);
   if(Zp[ip]<=2)h[8][ip]=new TH1D(histn,histn,7,1,8);
   else         h[8][ip]=new TH1D(histn,histn,nbina,rana[0],rana[1]);
   sprintf(histn,"fragzva%d",ip);
   h[9][ip]=new TH1D(histn,histn,1000,0,1.);
   sprintf(histn,"fragzva%dm",ip);
   h[10][ip]=new TH1D(histn,histn,1000,0,1.);
 }
//---------
  for(int ih=0;ih<nh;ih++){
   for(int ip=0;ip<np;ip++){
     TH1D *hn=h[ih][ip];
     sprintf(histn,"%sf",hn->GetName());
     TH1D *hnf=(TH1D *)hn->Clone(histn);
     hnf->SetTitle(histn);
     hf[ih][ip]=hnf;
   }
 }
 
 
//---------
 const int nh2=2;
 TH2D *h2[nh2][np];
 TH2D *h2f[nh2][np];
 for(int ip=0;ip<np;ip++){
   sprintf(histn,"fragzamap%d",ip);
   if(Zp[ip]<=2)h2[0][ip]=new TH2D(histn,histn,3,1,4,7,1,8);
   else         h2[0][ip]=new TH2D(histn,histn,nbinz,ranz[0],ranz[1],nbina,rana[0],rana[1]);
   sprintf(histn,"fragzamap%dm",ip);
   if(Zp[ip]<=2)h2[1][ip]=new TH2D(histn,histn,3,1,4,7,1,8);
   else         h2[1][ip]=new TH2D(histn,histn,nbinz,ranz[0],ranz[1],nbina,rana[0],rana[1]);
 }
//---------
 for(int ih=0;ih<nh2;ih++){
   for(int ip=0;ip<np;ip++){
     TH2D *hn=h2[ih][ip];
     sprintf(histn,"%sf",hn->GetName());
     TH2D *hnf=(TH2D *)hn->Clone(histn);
     hnf->SetTitle(histn);
     h2f[ih][ip]=hnf;
   }
 }
//----
// TH1D *h=new TH1();
  ADPMjet_Model *dpmjet=0;
  if(up>=0){
    dpmjet=new ADPMjet_Model(Ap[up],Zp[up],At[up],Zt[up],p_pn[up]);
  }
  else {
    dpmjet=new ADPMjet_Model(4,2,4,2,1e8);
  }
  dpmjet->GetParticleTable()->SetReadiness(1);
  dpmjet->GetParticleTable()->GetIonTable()->CreateAllIon();
//  dpmjet->GenerateAMSTable();
//  return 0;
// dpmjet->SetVerboseLevel(3);
 char *datacard="CC.inp";
// int dpmkey=2;//all calculation
 int dpmkey=1;//with table
// int dpmkey=101;
// int dpmkey=0;//with datacard
 dpmjet->SetADPMKEY(dpmkey);
// dpmjet->SetEexcFactor(0.5);
 dpmjet->Initialize(datacard);
//----
Long64_t nev=25000;
for(Long64_t iev=0;iev<nev;iev++){
 for(int ip=0;ip<np;ip++){
//   cout<<"Ev="<<iev<<endl;
   if(up>=0){if(ip!=up)continue;}//only accept up
//   if(ip==1||ip==2||ip==5||ip==6)continue;//light
   double Ip_pn=p_pn[ip];
   int    IAp =Ap[ip];
   int    IZp =Zp[ip];
   int    IAt =At[ip];
   int    IZt =Zt[ip];
   dpmjet->GenerateEvent(IAp,  IZp,  IAt, IZt, 0, Ip_pn);
   dpmjet->ImportParticles(1);
//-----
  int numpart = DTEVT1.nhkk;
  int numpartf=0;
  int imax=-1;
  double MnucEk=0;
  int    MnucA=0;
  int    MnucZ=0;
  for(int i=0; i<numpart; i++){
     if(DTEVT1.isthkk[i] ==1 || DTEVT1.isthkk[i] ==-1||DTEVT1.isthkk[i] == 1000){
        numpartf++;
     }
     if(DTEVT1.isthkk[i] == 1000){
        G4int    nucA = DTEVT2.idres[i]; //A
        G4int    nucZ = DTEVT2.idxres[i];//Z
        G4double nucE = DTEVT1.phkk[i][3];//E
        G4double Mass = DTEVT1.phkk[i][4];//Mass
        h[1][ip]->Fill(nucA+0.5); 
        h[0][ip]->Fill(nucZ+0.5);
        h2[0][ip]->Fill(nucZ+0.5,nucA+0.5); //Z VS A
        if(nucE-Mass>MnucEk){//Total Kinetic energy
          imax=i; MnucA=nucA; MnucZ=nucZ; MnucEk=nucE-Mass;
        }
     }
   }
   if (imax>=0){
     h[2][ip]->Fill(MnucZ+0.5);//Z
     h[8][ip]->Fill(MnucA+0.5);
     h[6][ip]->Fill(MnucEk/MnucA);//Kinetic energy/A
     h[7][ip]->Fill(MnucEk/MnucA/(Ip_pn-0.938));
     h2[1][ip]->Fill(MnucZ+0.5,MnucA+0.5); //Z VS A
   }
   else if(numpartf>0)h[2][ip]->Fill(1.5);
   h[3][ip]->Fill(numpartf);
   h[4][ip]->Fill(dpmjet->GetTotEnergy(0));
   h[5][ip]->Fill(dpmjet->GetTotEnergy(0)/(Ip_pn*IAp+IAt*0.938));
//----
   G4HadFinalState *pchange=dpmjet->GetParticleChange();
   int nSec=(pchange)?pchange->GetNumberOfSecondaries():0;
   if(nSec){
      G4double      TotalEPost = 0.0;
      G4ThreeVector TotalPPost;
      G4double MaxES      = 0;
      G4double MaxEkS     = 0;
      G4double MaxZS      = 0.;
      G4int    MaxAS      = 0;
      for (G4int j=0; j<nSec; j++) {
        G4DynamicParticle *theDParticle=pchange->GetSecondary(j)->GetParticle();
        G4double      ES = theDParticle->GetTotalEnergy()/GeV;
        G4double      EkS= theDParticle->GetKineticEnergy()/GeV;
        G4ThreeVector PS = theDParticle->GetMomentum()/GeV;
        G4ParticleDefinition *theParticle = theDParticle->GetDefinition();
        G4double      ZS = theParticle->GetPDGCharge();
        G4int         AS = theParticle->GetBaryonNumber();
        G4int         LS = theParticle->GetLeptonNumber();
        TotalEPost += ES;
        TotalPPost += PS;
        if(EkS>MaxEkS){MaxZS=ZS; MaxAS=AS; MaxES=ES; MaxEkS=EkS;}
        hf[1][ip]->Fill(AS+0.5);
        hf[0][ip]->Fill(ZS+0.5);
        h2f[0][ip]->Fill(ZS+0.5,AS+0.5); //Z VS A
        if(ZS>=3)hf[9][ip]->Fill(ZS/AS);
      }
      hf[2][ip]->Fill(MaxZS+0.5);//Z
      hf[8][ip]->Fill(MaxAS+0.5);
      hf[6][ip]->Fill(MaxEkS/MaxAS);//Kinetic energy/A
      hf[7][ip]->Fill(MaxEkS/MaxAS/(Ip_pn-0.938));
      h2f[1][ip]->Fill(MaxZS+0.5,MaxAS+0.5); //Z VS A
      hf[3][ip]->Fill(nSec);
      hf[4][ip]->Fill(TotalEPost);
      hf[5][ip]->Fill(TotalEPost/(Ip_pn*IAp+IAt*0.938));
      if(MaxZS>=3)hf[10][ip]->Fill(MaxZS/MaxAS);
   }
   dpmjet->DeleteAllSecondary();
//-----
  }
 }

 char filen[100];
 sprintf(filen,"dpmjettest_p%dk%d.root",up,dpmkey);
 TFile *fo=new TFile(filen,"RECREATE");
 fo->cd();
 for(int ih=0;ih<nh;ih++){
   for(int ip=0;ip<np;ip++){
     h[ih][ip]->Write();
     hf[ih][ip]->Write();
   }
 }
 for(int ih=0;ih<nh2;ih++){
   for(int ip=0;ip<np;ip++){
     h2[ih][ip]->Write();
     h2f[ih][ip]->Write();
   }
 }
 fo->Close();
 return 0; 
 
}
