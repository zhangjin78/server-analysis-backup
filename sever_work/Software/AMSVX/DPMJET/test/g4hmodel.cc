//---standalone G4HadronModel test, created by Q.Yan qyan@cern.ch
#include "G4PreCompoundModel.hh"
#include "G4FermiBreakUp.hh"
#include "G4HadronicInteractionRegistry.hh"
#include "G4BinaryLightIonReactionQ.hh"
#include "G4INCLXXInterfaceQ.hh"
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
 G4double p_pn[]={6,  6,  6,  6,  6,  6,  6,    6,  6,  6};
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
  G4ParticleTable::GetParticleTable()->SetReadiness(1);
  G4IonTable::GetIonTable()->CreateAllIon();
//------G4FermiBreakUp
  G4HadronicInteraction* p = G4HadronicInteractionRegistry::Instance()->FindModel("PRECO");
  G4PreCompoundModel* thePreComp = static_cast<G4PreCompoundModel*>(p);
  G4cout<<"thePreComp="<<thePreComp<<G4endl;
  if(!thePreComp){ thePreComp = new G4PreCompoundModel; }
  thePreComp->GetExcitationHandler()->SetFermiModel(new G4FermiBreakUp);
//------
//  G4INCLXXInterfaceQ *hmodel=new G4INCLXXInterfaceQ(thePreComp);
  G4BinaryLightIonReactionQ *hmodel=new G4BinaryLightIonReactionQ(thePreComp);
//  ADPMjet_Model *hmodel=new ADPMjet_Model();
//  hmodel->Initialize();
//  hmodel->SetEexcFactor(0.5);
//----
Long64_t nev=25000;
 for(int ip=0;ip<np;ip++){
   if(up>=0){if(ip!=up)continue;}//only accept up
//   if(ip==1||ip==2||ip==5||ip==6)continue;//light
   G4double Ip_pn=p_pn[ip]*GeV;
   int    IAp =Ap[ip];
   int    IZp =Zp[ip];
   int    IAt =At[ip];
   int    IZt =Zt[ip];
   G4ParticleDefinition* particle=G4IonTable::GetIonTable()->GetIon(IZp,IAp);
   G4ThreeVector lv(0,0,1);
   G4double Ekin=Ip_pn*IAp-particle->GetPDGMass();//Ekin=Etot-Mass
   G4DynamicParticle pParticle(particle,lv,Ekin);
//-----ready for simulation
   for(Long64_t iev=0;iev<nev;iev++){
      G4HadProjectile theTrack(pParticle);
      G4Nucleus pTarget(IAt,IZt);
      G4int    AP       = particle->GetBaryonNumber();
      G4int    ZP       = G4int(particle->GetPDGCharge()/eplus + 0.5);
      G4double EkP      = pParticle.GetKineticEnergy()/GeV;
      G4double EP       = pParticle.GetTotalEnergy()/GeV;
      G4int    AT       = pTarget.GetA_asInt();
      G4int    ZT       = pTarget.GetZ_asInt();
      G4double TotalEPost = 0.0;
      G4double MaxES      = 0;
      G4double MaxEkS     = 0;
      G4double MaxZS      = 0.;
      G4int    MaxAS      = 0;
      if(iev==0)G4cout<<"PartiInfo="<<AP<<","<<ZP<<","<<AT<<","<<ZT<<","<<EkP<<","<<EP<<G4endl;
      G4HadFinalState *pchange=hmodel->ApplyYourself(theTrack,pTarget);
      int nSec=(pchange)?pchange->GetNumberOfSecondaries():0;
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
        if(EkS>MaxEkS){MaxZS=ZS; MaxAS=AS; MaxES=ES; MaxEkS=EkS;}
        hf[1][ip]->Fill(AS+0.5);
        hf[0][ip]->Fill(ZS+0.5);
        h2f[0][ip]->Fill(ZS+0.5,AS+0.5); //Z VS A
        if(ZS>=3)hf[9][ip]->Fill(ZS/AS);
      }
      hf[2][ip]->Fill(MaxZS+0.5);//Z
      hf[8][ip]->Fill(MaxAS+0.5);
      hf[6][ip]->Fill(MaxEkS/MaxAS);//Kinetic energy/A
      hf[7][ip]->Fill(MaxEkS/MaxAS/(EkP/AP));
      h2f[1][ip]->Fill(MaxZS+0.5,MaxAS+0.5); //Z VS A
      hf[3][ip]->Fill(nSec);
      hf[4][ip]->Fill(TotalEPost);
      hf[5][ip]->Fill(TotalEPost/(EP+AT*0.938));
      if(MaxZS>=3)hf[10][ip]->Fill(MaxZS/MaxAS);
      hmodel->DeleteAllSecondary();
//-----
    }
 }

 char filen[100];
 sprintf(filen,"hmodeltest_p%d.root",up);
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
