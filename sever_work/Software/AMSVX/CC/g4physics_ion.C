//  $Id: g4physics_ion.C,v 1.48 2019/04/29 10:44:26 choutko Exp $
// ------------------------------------------------------------
//      AMS GEANT4 new Ion Physics 
//      Ion Eleastic for high Energy->DPM-JET
// ------------------------------------------------------------
//      History
//        Created:       2012-Apr-09   Q.Yan
//        Modified:      2013-March-23 Q.Yan
//                                Fix Gap of Cross-Section
// -----------------------------------------------------------

#include "commons.h"
#include "g4physics.h"
#include "g4physics_ion.h"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"
#include "G4CrossSectionInelastic.hh"

#include "G4HadronInelasticProcess.hh"
#include "G4BinaryLightIonReaction.hh"
#include "G4BinaryLightIonReactionQ.hh"
#include "G4TripathiCrossSection.hh"
#include "G4TripathiLightCrossSection.hh"
#include "G4IonsShenCrossSection.hh"
#include "G4IonProtonCrossSection.hh"
#include "G4GeneralSpaceNNCrossSection.hh"
#include "G4EMDissociation.hh"
#include "G4FermiBreakUp.hh"
#include "G4Evaporation.hh"
#include "G4Version.hh"
#include "G4HadronicInteractionRegistry.hh"
#if G4VERSION_NUMBER  > 945 
#include "G4ComponentAntiNuclNuclearXS.hh"
#include "G4GGNuclNuclCrossSection.hh"
#include "G4GGNuclNuclCrossSectionQ.hh"
#include "G4INCLXXInterface.hh"
#include "G4INCLXXInterfaceQ.hh"
#include "G4FTFBuilder.hh"
#endif
#if G4VERSION_NUMBER  > 999
#include "G4AntiDeuteron.hh"
#include "G4AntiHe3.hh"
#include "G4AntiAlpha.hh"
#endif
#include "G4IonsHEAOCrossSection.hh"
#ifdef G4_USE_DPMJET
#include "G4DPMJET2_5Model.hh"
#include "G4DPMJET2_5Interface.hh"
#include "G4DPMJET2_5CrossSection.hh"
#endif
#ifdef G4_USE_DPMJET3
#include "ADPMjet_Model.h"
#endif

// -----------------------------------------------------------
IonDPMJETPhysics::IonDPMJETPhysics()
  : G4VHadronPhysics("ionInelasticDPMJET"),theIonBCl(0),theIonBC(0),theIonBC1(0),theEMD(0)
{
  fTripathi = fTripathiLight = fShen = fIonH = fEMD =0;
   theINCLXXl=0;
   theINCLXX=0;
   theFTF=0;
#ifdef G4_USE_DPMJET
   theDPM=0;
#endif
#ifdef G4_USE_DPMJET3
   theDPM3=0;
#endif
}

// -----------------------------------------------------------
IonDPMJETPhysics::~IonDPMJETPhysics()
{}

// -----------------------------------------------------------
void IonDPMJETPhysics::ConstructProcess(){


if(UseInclXX==0||UseInclXX==2||UseInclXX==3){//UseInclXX=0 Low-Energy BIC, UseInclXX=2|3 Low-Energy InclXX
  G4double dpmemin=5.*GeV; 
  G4double dpmemax = 1000*TeV;
  //--Low Energy(INCLXX)/third
#if G4VERSION_NUMBER  > 945 
  theINCLXXl = new G4INCLXXInterface();
  theINCLXXl->SetMinEnergy(0.0);
  theINCLXXl->SetMaxEnergy(dpmemin*1.02);
  theINCLXX = new G4INCLXXInterface();
  theINCLXX->SetMinEnergy(0.0);
  theINCLXX->SetMaxEnergy(3*58*GeV);
#endif
  //--Low Energy(BIC)/third
  theIonBCl = new G4BinaryLightIonReaction();
  theIonBCl->SetMinEnergy(0.0);
  theIonBCl->SetMaxEnergy(dpmemin*1.02);
  theIonBC = new G4BinaryLightIonReaction();
  theIonBC->SetMinEnergy(0.0);
  theIonBC->SetMaxEnergy(100*GeV);
  //--High Energy, CALORIMETER(FTF)/second
#if G4VERSION_NUMBER  > 945 
  G4HadronicInteraction* p = G4HadronicInteractionRegistry::Instance()->FindModel("PRECO");
  G4PreCompoundModel* thePreCompound = static_cast<G4PreCompoundModel*>(p);
  if(!thePreCompound) { thePreCompound = new G4PreCompoundModel; }
  G4FTFBuilder *b=new G4FTFBuilder("FTFP",thePreCompound);
  theFTF = b->GetModel();
  theFTF->SetMinEnergy(3*58*GeV-0.1);
  theFTF->SetMaxEnergy(dpmemax);
#endif
  //--High Energy, CALORIMETER(BIC)/second
  theIonBC1 = new G4BinaryLightIonReaction();
  theIonBC1->SetMinEnergy(dpmemin);
  theIonBC1->SetMaxEnergy(dpmemax);
  //--High Energy,Model(DPMJET25)/first
#ifdef G4_USE_DPMJET
  theDPM = new G4DPMJET2_5Model();//FIX ME what to do with projectles with a>58 ???
  theDPM->SetMinEnergy(dpmemin);
  theDPM->SetMaxEnergy(dpmemax);
  // theDPM->SetVerboseLevel(10);
  G4int dpmAmax=58;
  G4int dpmAmin=2;
  G4ElementTable::iterator iter;
  G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
  for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
    G4int AA  =(*iter)->GetN();
    if (AA>=dpmAmin && AA<=dpmAmax){
      theDPM->ActivateFor(*iter);
#if G4VERSION_NUMBER  > 945 
      theFTF->DeActivateFor(*iter);
#endif
      theIonBC1->DeActivateFor(*iter);
    }
    else {
#if G4VERSION_NUMBER  > 945 
      theFTF->ActivateFor(*iter);
#endif
      theIonBC1->ActivateFor(*iter); 
      theDPM->DeActivateFor(*iter);
    }
  }
#endif
}
else if(UseInclXX==4||UseInclXX==5){
  G4double dpm3emin=6.1*GeV;
  G4double dpm3emax = 1.e5*TeV;//1e8GeV
  //--Low Energy(INCLXX)/second
  G4HadronicInteraction* p = G4HadronicInteractionRegistry::Instance()->FindModel("PRECO");
  G4PreCompoundModel* thePreCompINC = static_cast<G4PreCompoundModel*>(p);
  if(!thePreCompINC){ thePreCompINC = new G4PreCompoundModel; }
  G4PreCompoundModel *thePreCompBIC = new G4PreCompoundModel;
  if((G4FFKEY.IonPhysicsModel/100000)%10>=4){//using G4FermiBreakUp istead of G4FermiBreakUpVI
    thePreCompINC->GetExcitationHandler()->SetFermiModel(new G4FermiBreakUp);
    thePreCompBIC->GetExcitationHandler()->SetFermiModel(new G4FermiBreakUp); 
  }
  if((G4FFKEY.IonPhysicsModel/100000)%10==5){//using GEMEvaporation
    G4Evaporation *theEvapINC=static_cast<G4Evaporation*>(thePreCompINC->GetExcitationHandler()->GetEvaporation());
    G4Evaporation *theEvapBIC=static_cast<G4Evaporation*>(thePreCompBIC->GetExcitationHandler()->GetEvaporation());
    if(theEvapINC)theEvapINC->SetGEMChannel();
    else          {G4cout<<"err:G4INC has no G4Evaporation"<<G4endl;} 
    if(theEvapBIC)theEvapBIC->SetGEMChannel();
    else          {G4cout<<"err:G4BIC has no G4Evaporation"<<G4endl;}
  }

#if G4VERSION_NUMBER  > 963
  if(G4FFKEY.HEexcBias[0]!=1.){
    theINCLXX = new G4INCLXXInterfaceQ(thePreCompINC);
    static_cast<G4INCLXXInterfaceQ *>(theINCLXX)->SetEexcFactor(G4FFKEY.HEexcBias[0]);
  }
  else  theINCLXX = new G4INCLXXInterface(thePreCompINC);
  theINCLXX->SetMinEnergy(0.0);
  theINCLXX->SetMaxEnergy(dpm3emin*1.1);//6.7GeV/n (minor-overlap)
  if     ((G4FFKEY.IonPhysicsModel/10000)%10==2)theINCLXX->SetMaxEnergy(dpm3emin*2.);//12.2GeV/n more-overlap
  else if((G4FFKEY.IonPhysicsModel/10000)%10==3)theINCLXX->SetMaxEnergy(dpm3emin*4.);//24.4GeV/n
#endif
  //--Low Energy(BIC)/second
  if(G4FFKEY.HEexcBias[0]!=1.){
    theIonBC = new G4BinaryLightIonReactionQ(thePreCompBIC);
    static_cast<G4BinaryLightIonReactionQ *>(theIonBC)->SetEexcFactor(G4FFKEY.HEexcBias[0]);
  }
  else theIonBC = new G4BinaryLightIonReaction(thePreCompBIC);
  theIonBC->SetMinEnergy(0.0);
  theIonBC->SetMaxEnergy(dpm3emin*1.1);//6.7GeV/n (minor-overlap)
  if     ((G4FFKEY.IonPhysicsModel/10000)%10==2)theIonBC->SetMaxEnergy(dpm3emin*2.);//12.2GeV/n more-overlap
  else if((G4FFKEY.IonPhysicsModel/10000)%10==3)theIonBC->SetMaxEnergy(dpm3emin*4.);//24.4GeV/n
  //--High Energy(DPMJET3)/first
#ifdef G4_USE_DPMJET3
  theDPM3=new ADPMjet_Model();
  theDPM3->Initialize();
  theDPM3->SetMinEnergy(dpm3emin);
  theDPM3->SetMaxEnergy(dpm3emax);
//  theDPM3->SetVerboseLevel(3);
  if(G4FFKEY.HEexcBias[1]!=1.)theDPM3->SetEexcFactor(G4FFKEY.HEexcBias[1]);
  if     ((G4FFKEY.IonPhysicsModel/100000)%10==1)theDPM3->SetNoDeexcitation();
  else if((G4FFKEY.IonPhysicsModel/100000)%10==2)theDPM3->SetDeexcitationMode(2);
  else if((G4FFKEY.IonPhysicsModel/100000)%10==3)theDPM3->SetDefaultPreCompoundModel(2);//using G4 default G4FermiBreakUpVI
//----
  if((G4FFKEY.IonPhysicsModel/100000)%10==5){//using GEMEvaporation
    G4Evaporation *theEvapDPM3=static_cast<G4Evaporation*>(theDPM3->GetPreCompoundModel()->GetExcitationHandler()->GetEvaporation());
    if(theEvapDPM3)theEvapDPM3->SetGEMChannel();
    else          {G4cout<<"err:DPM3 has no G4Evaporation"<<G4endl;}
  }
#endif
}
else {
  G4cout << "IonDPMJETPhysics::ConstructProces-Not exsit " << G4endl;  abort();
}

//---
  fTripathi = new G4TripathiCrossSection();//< 1GeV  all A
  fTripathiLight = new G4TripathiLightCrossSection();//K/n <10GeV t d he3 he
  fIonH = new G4IonProtonCrossSection();//proton Target <20GeV (Inject A>4)
  fShen = new G4IonsShenCrossSection();
#ifdef G4_USE_DPMJET
  dpmXS = new G4DPMJET2_5CrossSection;//DPMJET Cross-section<1000TeV
#endif
  HEAOXS = new G4IonsHEAOCrossSection();//HEAO  Cross-section

//-----
  AddProcess("dInelastic",G4Deuteron::Deuteron(),false);
  AddProcess("tInelastic",G4Triton::Triton(),false);
  AddProcess("He3Inelastic",G4He3::He3(),true);
  AddProcess("alphaInelastic",G4Alpha::Alpha(),true);
  AddProcess("ionInelastic",G4GenericIon::GenericIon(),true);
#if G4VERSION_NUMBER  > 2999
  AddProcess("adInelastic",G4AntiDeuteron::AntiDeuteron(),false,true);
  AddProcess("aHe3Inelastic",G4AntiHe3::AntiHe3(),true,true);
  AddProcess("aalphaInelastic",G4AntiAlpha::AntiAlpha(),true,true);
#endif
//-----
  if(G4FFKEY.UseEMDModel==1){
    cout<<"Also Use EMD Model"<<endl;
    theEMD = new G4EMDissociation();
    theEMD->SetMinEnergy(1.0*MeV);
    theEMD->SetMaxEnergy(100*TeV);
    fEMD = new G4EMDissociationCrossSection();
    AddProcessEMD("He3EMD",G4He3::He3());
    AddProcessEMD("AlphaEMD",G4Alpha::Alpha());
    AddProcessEMD("IonEMD",G4GenericIon::GenericIon());
  }
//-----
  G4cout << "IonDPMJETPhysics::ConstructProcess done! " << G4endl;
}

// -----------------------------------------------------------
void IonDPMJETPhysics::AddProcess(const G4String& name,
                                  G4ParticleDefinition* part,
                                  bool isIon, bool isAnti)
{
  if(isAnti){
   string pn=part->GetParticleName();
   pn+="Inelastic";
   if(AMSG4Physics::RemoveProcess(part,pn))cout<<"IonDPMJETPhysics::AddProcess-I-RemovedProcess "<<pn<<endl;
  }
  G4HadronInelasticProcess* hadi = new G4HadronInelasticProcess(name, part);
  G4ProcessManager* pManager = part->GetProcessManager();
  pManager->AddDiscreteProcess(hadi);
  hadi->AddDataSet(fShen); 
  if ((G4FFKEY.IonPhysicsModel/10)%10==4) {
#ifdef G4_USE_DPMJET
     hadi->AddDataSet(dpmXS);
#else
     G4cout << "IonDPMJETPhysics::ConstructProces-F-DPMJET2.5XS notsupported " << G4endl;  abort();
#endif
     if(isIon && (G4FFKEY.IonPhysicsModel/100)%10==1) { hadi->AddDataSet(fIonH); }
  }
  if ((G4FFKEY.IonPhysicsModel/10)%10==3) hadi->AddDataSet(HEAOXS);
  if ((G4FFKEY.IonPhysicsModel/10)%10==2) {
    G4GeneralSpaceNNCrossSection* generalCrossSection = new G4GeneralSpaceNNCrossSection;
    hadi->AddDataSet(generalCrossSection);
  }
#if G4VERSION_NUMBER  > 945 
  if ((G4FFKEY.IonPhysicsModel/10)%10==1||(G4FFKEY.IonPhysicsModel/10)%10==5) {
if(!isAnti){
   if ((G4FFKEY.IonPhysicsModel/10)%10==1){
     G4GGNuclNuclCrossSection* fGG = new G4GGNuclNuclCrossSection();
     hadi->AddDataSet(fGG);
   }
   else if((G4FFKEY.IonPhysicsModel/10)%10==5){
     G4GGNuclNuclCrossSectionQ* fGG = new G4GGNuclNuclCrossSectionQ();
     hadi->AddDataSet(fGG);
   }
}
else{
  G4ComponentAntiNuclNuclearXS*apgg=  new G4ComponentAntiNuclNuclearXS();
  G4CrossSectionInelastic* anucxs = 
    new G4CrossSectionInelastic(apgg);
    hadi->AddDataSet(anucxs);
    cout<<"IonDPMJETPhysics::AddProcess-I-AntiNuclearCrossSectionSet for "<<part->GetParticleName()<<endl;

}
  }
#endif
// Should be not be used with GG
if(UseInclXX==0||((UseInclXX==2||UseInclXX==3)&&isAnti)){//Low Energy(BIC)+DPMJET2.5 Y=3|5A|6A
  if((G4FFKEY.IonPhysicsModel/10000)%10==1)hadi->RegisterMe(theIonBCl);//no  overlap
  else                                     hadi->RegisterMe(theIonBC); //has overlap
  hadi->RegisterMe(theIonBC1);
#ifdef G4_USE_DPMJET
  hadi->RegisterMe(theDPM);
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-DPMJET2.5 notsupported " << G4endl;  abort(); 
#endif
}
else if(UseInclXX==2&&isIon){//Low Energy(INCLXX)+FTF+DPMJET2.5 Y=5I
#if G4VERSION_NUMBER  > 945
  hadi->RegisterMe(theINCLXX);//no  overlap
  hadi->RegisterMe(theFTF);
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-Inclxx notsupported " << G4endl;  abort();
#endif
#ifdef G4_USE_DPMJET
  hadi->RegisterMe(theDPM);
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-DPMJET2.5 notsupported " << G4endl;  abort();
#endif
}
else if((UseInclXX==2&&!isIon)||UseInclXX==3){//Low Energy(INCLXX)+BIC+DPMJET2.5 5L|6
#if G4VERSION_NUMBER  > 945
  if(!isIon||((G4FFKEY.IonPhysicsModel/10000)%10==1))hadi->RegisterMe(theINCLXXl);//no  overlap
  else                                               hadi->RegisterMe(theINCLXX); //has overlap
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-Inclxx notsupported " << G4endl;  abort();
#endif
  hadi->RegisterMe(theIonBC1);
#ifdef G4_USE_DPMJET
  hadi->RegisterMe(theDPM);
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-DPMJET2.5 notsupported " << G4endl;  abort();
#endif
}
else if(UseInclXX==4||(UseInclXX==5&&isAnti)){//Low Energy(BIC)+DPMJET3 Y=7|8A
  hadi->RegisterMe(theIonBC);//no  overlap
#ifdef G4_USE_DPMJET3
  hadi->RegisterMe(theDPM3);
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-DPMJET3 notsupported " << G4endl;  abort();
#endif
}
else if(UseInclXX==5){//Low Energy(INCLXX)+DPMJET3 Y=8
#if G4VERSION_NUMBER  > 945
  hadi->RegisterMe(theINCLXX);//no  overlap
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-Inclxx notsupported " << G4endl;  abort();
#endif
#ifdef G4_USE_DPMJET3
  hadi->RegisterMe(theDPM3);
#else
  G4cout << "IonDPMJETPhysics::ConstructProces-F-DPMJET3 notsupported " << G4endl;  abort();
#endif
}
else {
  G4cout << "IonDPMJETPhysics::ConstructProces-Not exsit " << G4endl;  abort();
}

  if (G4FFKEY.HCrossSectionBias[0]!=1) hadi->BiasCrossSectionByFactor2(G4FFKEY.HCrossSectionBias[0]);
}


// -----------------------------------------------------------
void IonDPMJETPhysics::AddProcessEMD(const G4String& name,
                                  G4ParticleDefinition* part){

    G4HadronInelasticProcess* hadi_EMD = new G4HadronInelasticProcess(name,part);
    G4ProcessManager* pManager = part->GetProcessManager();
    pManager->AddDiscreteProcess(hadi_EMD);
    hadi_EMD->AddDataSet(fEMD);
    hadi_EMD->RegisterMe(theEMD);
}
