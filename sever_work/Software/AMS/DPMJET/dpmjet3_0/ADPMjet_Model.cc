//*KEEP,ADPMjet_Model.
#include "ADPMjet_Model.h"
#include "ADPMcommon.h"
#include "G4DynamicParticle.hh"
#include "G4Evaporation.hh"
#include "G4FermiBreakUp.hh"
#include "G4PhotonEvaporation.hh"
#include "G4ExcitationHandler.hh"
#include "G4PreCompoundModel.hh"
#include "G4HadronicInteractionRegistry.hh"

//______________________________________________________________________________
ADPMjet_Model::ADPMjet_Model(int Ip, int Ipz, int It, int Itz, double Epn): G4HadronicInteraction("dpmjet3"),
      fNEvent(0),
      fIp(Ip),
      fIpz(Ipz),
      fIt(It),
      fItz(Itz),
      fEpn(Epn),
      fIdp(0),
      fDPMKEY(1),
      fEexc(1.)
{  
    G4cout<<"DPMJet3 Constructor"<<" "<<fIp<<" "<<fIpz<<" "<<fIt<<" "<<fItz<<" Epn="<<Epn<<G4endl;
    SetMinEnergy(1.0*GeV);
    SetMaxEnergy(1.e5*TeV);
    thePreComp = 0;
    if(((fDPMKEY/10)%10)==0)SetDefaultPreCompoundModel();
    theParticleTable = G4ParticleTable::GetParticleTable();
    theIonTable      = const_cast <G4IonTable *> (theParticleTable->GetIonTable());
}


//______________________________________________________________________________
void ADPMjet_Model::SetDefaultPreCompoundModel (int opt){

 if(thePreComp)delete thePreComp;
 if(opt==0){
   G4ExcitationHandler *anExcitationHandler = new G4ExcitationHandler;
   G4Evaporation * theEvaporation           = new G4Evaporation;
   G4FermiBreakUp * theFermiBreakUp         = new G4FermiBreakUp;
   G4PhotonEvaporation* thePhotonEvap       = new G4PhotonEvaporation;
   anExcitationHandler->SetEvaporation(theEvaporation);
   anExcitationHandler->SetFermiModel(theFermiBreakUp);
   anExcitationHandler->SetMaxAandZForFermiBreakUp(17, 9);
   anExcitationHandler->SetPhotonEvaporation(thePhotonEvap);
   thePreComp = new G4PreCompoundModel(anExcitationHandler);
 }
 else if(opt==1){
   G4HadronicInteraction* p =G4HadronicInteractionRegistry::Instance()->FindModel("PRECO");
   thePreComp = static_cast<G4PreCompoundModel*>(p);
   if(!thePreComp) { thePreComp = new G4PreCompoundModel; }
 }
 else {
   thePreComp = new G4PreCompoundModel;
 }
}


//______________________________________________________________________________
G4bool ADPMjet_Model::IsApplicable (const G4HadProjectile &theTrack, G4Nucleus &theTarget){

  // Get relevant information about the projectile and target (A, Z)
  const G4ParticleDefinition *definitionP = theTrack.GetDefinition();
  G4int AP   = definitionP->GetBaryonNumber();
  G4int ZP   = G4int(definitionP->GetPDGCharge()/eplus + 0.5);
  G4int AT   = theTarget.GetA_asInt();
  G4int ZT   = theTarget.GetZ_asInt();


  if (AP >= 1 && (ZP >= 1 || ZP<=-1) && AT >= 1 && ZT >=1) {
//  if (AP >= 2 && (ZP >= 1 || ZP<=-1) && AT >= 2 && ZT >=1) {//This has to be change
    return true;
  }
  else {
    return false;
  }

}


//______________________________________________________________________________
int ADPMjet_Model::GenerateEvent(int Ip, int Ipz, int It, int Itz, int Idp, double Elab){
   fNEvent++;
   DTEVNO.nevent=fNEvent;
   int kkmat=-1;
   if(Ip==0&&Ipz==0&&It==0&&Itz==0&&Idp==0){//use default
     Ip =fIp;
     Ipz=fIpz;
     It =fIt;
     Itz=fItz;
   }
   if(Elab==0)Elab=fEpn;
   int irej = 1;
   int evtcnt=0;
   while (irej) {
     Dt_Kkinc(Ip, Ipz, It, Itz, Idp, Elab, kkmat, irej);
     if(evtcnt++>=100){G4cout<<"ADPMjet_Model::Dt_Kkinc many failure"<<G4endl; break;}
   }
   return irej;
}


//______________________________________________________________________________
int ADPMjet_Model::ImportParticles(int opt)
{

  if(opt){
    theParticleChange.Clear();
    theParticleChange.SetStatusChange(stopAndKill);
  }

//  all the particles
   G4int numpart = DTEVT1.nhkk;
    
//-----genrate particle
   for (G4int i=0; i < numpart; i++) {
      G4double px      = DTEVT1.phkk[i][0] * GeV;
      G4double py      = DTEVT1.phkk[i][1] * GeV;
      G4double pz      = DTEVT1.phkk[i][2] * GeV;
      G4double et      = DTEVT1.phkk[i][3] * GeV;
      G4double dpmMass = DTEVT1.phkk[i][4] * GeV;
      G4double excMass = (DTEVT2.phres[i][1]-DTEVT2.phres[i][0]) * GeV;
      G4int nucA       = DTEVT2.idres[i]; //A
      G4int nucZ       = DTEVT2.idxres[i];//Z
      if ((DTEVT1.isthkk[i] == 1|| DTEVT1.isthkk[i] == -1)&&opt){//no de-excitation required
         G4ParticleDefinition* theParticle = theParticleTable->FindParticle(DTEVT1.idhkk[i]);
         if(theParticle){
           G4double pmass=theParticle->GetPDGMass();//<1.MeV error
           if(dpmMass<pmass||fabs(dpmMass-pmass)<1.*MeV)et=std::sqrt(px*px + py*py + pz*pz + pmass*pmass);
           else {G4cout<<"DPMJET3 big diff Geant4-DPM Mass="<<(dpmMass/GeV)<<","<<(pmass/GeV)<<G4endl;}
           G4LorentzVector lv = G4LorentzVector(px,py,pz,et);
           G4DynamicParticle *theDynamicParticle = new G4DynamicParticle(theParticle,lv);
           theParticleChange.AddSecondary (theDynamicParticle);
         }
         else {G4cout<<"DPMJET3 pid="<<DTEVT1.idhkk[i]<<" not found"<<G4endl;}
      }
// Particle is a secondary nucleus. Determine the details of the nuclear
// fragment prior to de-excitation. (Note that the 1 eV in the total energy
// is a safety factor to avoid any possibility of negative rest mass energy.
// is a safety factor to avoid any possibility of negative rest mass energy.)
// Note also that we don't full trust the energy provided by the DPMJET,
// and there it's based on the Geant4-determined ion rest-mass.
      else if((DTEVT1.isthkk[i] == 1000)&&opt){//de-excitation //DTEVT1.idhkk[i]==80000
         G4double ionMass   = theIonTable->GetIonMass(nucZ,nucA);
         if(((fDPMKEY/100)%10)>0){//USE DPMJET total mass, ground mass DPMJET!=G4, binding energy could be biased
           if (dpmMass > ionMass) {ionMass = dpmMass;}
           else                   {ionMass = ionMass+1.0*eV;} 
         }
         else {//USE DPMJET binding energy 
//           G4cout<<"Z="<<nucZ<<" A="<<nucA<<" Mass="<<(dpmMass/GeV)<<","<<DTEVT2.phres[i][0]<<","<<DTEVT2.phres[i][1]<<","<<(ionMass/GeV)<<" eMass="<<(excMass/GeV)<<G4endl;
           if(excMass<1.0*eV){
             G4cout<<"ADPMjet_Model::excMassi error="<<excMass<<G4endl;
             excMass = 1.0*eV;
           }
           ionMass = ionMass+fEexc*excMass;//add DPMJET binding energy
         }
         G4double etf         = std::sqrt(px*px + py*py + pz*pz + ionMass*ionMass); //Total E
         G4LorentzVector lv   = G4LorentzVector(px,py,pz,etf);
         G4Fragment *fragment = new G4Fragment(nucA, nucZ, lv);
         if(thePreComp){//de-excitation first
            G4ReactionProductVector *products = 0;
//            G4cout<<"Z="<<nucZ<<" A="<<nucA<<","<<fragment->GetA()<<" P="<<(px/GeV)<<","<<(py/GeV)<<","<<(pz/GeV)<<" E="<<(et/GeV)<<","<<(etf/GeV)<<" Mass="<<(theIonTable->GetIonMass(nucZ,nucA)/GeV)<<","<<(dpmMass/GeV)<<" idhkk="<<DTEVT1.idhkk[i]<<G4endl;
            products = thePreComp->DeExcite(*fragment);
            G4ReactionProductVector::iterator iter;
            for (iter = products->begin(); iter != products->end(); ++iter){
               G4DynamicParticle *secondary = new G4DynamicParticle((*iter)->GetDefinition(),(*iter)->GetTotalEnergy(), (*iter)->GetMomentum());
               theParticleChange.AddSecondary (secondary);
               G4String particleName = (*iter)->GetDefinition()->GetParticleName();
               delete (*iter);
            }
            delete products; 
         }
         else {//without de-excitation
            G4ParticleDefinition *theParticleDefinition = theIonTable->GetIon(nucZ,nucA);
            G4DynamicParticle *theDynamicParticle =new G4DynamicParticle(theParticleDefinition,lv);
            theParticleChange.AddSecondary (theDynamicParticle);
         }
         delete fragment;
      }
   }

   if (verboseLevel >= 3) {
      G4double entot=0;
      G4double ptot[3]={0,0,0};
      G4double emax=0;
      G4int    imax=-1;
      G4int npartf=0;
      for(G4int i=0; i<numpart; i++){
        if(DTEVT1.isthkk[i]==1 || DTEVT1.isthkk[i]==-1 || DTEVT1.isthkk[i]==1000){
          npartf++;
          entot += DTEVT1.phkk[i][3]; // PHKK[i][3] <-> PHKK(4,i)
          for(G4int ipd=0;ipd<3;ipd++){ptot[ipd]+=DTEVT1.phkk[i][ipd];}
          if(verboseLevel >= 4){
             G4cout<<"i="<<i<<" DTEVT1="<<DTEVT1.isthkk[i]<<","<<DTEVT1.idhkk[i]<<" Energy="<<DTEVT1.phkk[i][3]<<" Pmom="<<DTEVT1.phkk[i][0]<<","<<DTEVT1.phkk[i][1]<<","<<DTEVT1.phkk[i][2]<< " Mass="<<DTEVT1.phkk[i][4]<<" AZ="<<DTEVT2.idres[i]<<","<<DTEVT2.idxres[i]<<G4endl;
          }
        }
        if(DTEVT1.phkk[i][3]>emax){emax=DTEVT1.phkk[i][3];imax=i;}
      }
      if(verboseLevel >= 4 && imax>=0)G4cout<<"imax="<<imax<<" DTEVT1M="<<DTEVT1.isthkk[imax]<<","<<DTEVT1.idhkk[imax]<<" Energy="<<DTEVT1.phkk[imax][3]<< " Mass="<<DTEVT1.phkk[imax][4]<<" AZ="<<DTEVT2.idres[imax]<<","<<DTEVT2.idxres[imax]<<G4endl;

     // Calculate and display the energy and momenta before and after the collision.
     // Everything is calculated for the lab frame
      G4double      TotalEPost = 0.0;
      G4ThreeVector TotalPPost;
      G4double charge     = 0.0;
      G4int    baryon     = 0;
      G4int    lepton     = 0;
      G4double MaxES      = 0;
      G4double MaxZS      = 0.;
      G4int    MaxAS      = 0;
      G4int nSecondaries  = theParticleChange.GetNumberOfSecondaries();
      for (G4int j=0; j<nSecondaries; j++) {
        G4DynamicParticle *theDParticle=theParticleChange.GetSecondary(j)->GetParticle();	
        G4double      ES = theDParticle->GetTotalEnergy();
        G4ThreeVector PS = theDParticle->GetMomentum();
        G4ParticleDefinition *theParticle = theDParticle->GetDefinition();
        G4double      ZS = theParticle->GetPDGCharge();
        G4int         AS = theParticle->GetBaryonNumber();
        G4int         LS = theParticle->GetLeptonNumber();
        TotalEPost += ES;
        TotalPPost += PS; 
        charge     += ZS;
        baryon     += AS;
        lepton     += LS;
        if(ES>MaxES){MaxZS=ZS; MaxAS=AS; MaxES=ES;}
      }
      G4cout <<"--------------------------------------------------------------------------------"<<G4endl;
      G4cout <<"Total energy after collision    = " <<TotalEPost/GeV<<" GeV" <<" Etot2="<<entot<<G4endl;
      G4cout <<"Total momentum after collision  = " <<TotalPPost/GeV<<" GeV/c" <<" Ptot2="<<ptot[0]<<","<<ptot[1]<<","<<ptot[2]<<G4endl;
      G4cout <<"Total nparticle after collision = " << nSecondaries<<" Npar2="<<npartf<<G4endl;
      G4cout <<"Total charge after collision    = " <<charge<<G4endl;
      G4cout <<"Total baryon number after collision  = "<<baryon<<G4endl;
      G4cout <<"Total lepton number after collision  = "<<lepton<<G4endl;
      if(imax>=0 && MaxES>0)G4cout <<"Largest energy fragment Z="<<MaxZS<<" Z2="<<GetFragmentZ(imax)<<" A="<<MaxAS<<" A2="<<GetFragmentA(imax)<<" E="<<(MaxES/GeV)<<" E2="<<GetEnergy(imax)<<G4endl;
      G4cout <<"--------------------------------------------------------------------------------" <<G4endl;
   }

   return numpart;
}



//______________________________________________________________________________
G4HadFinalState *ADPMjet_Model::ApplyYourself (const G4HadProjectile &theTrack, G4Nucleus &theTarget){


// The secondaries will be returned in G4HadFinalState &theParticleChange -
// initialise this.  The original track will always be discontinued and
// secondaries followed.
  theParticleChange.Clear();
  theParticleChange.SetStatusChange(stopAndKill);
   
  const G4ParticleDefinition *definitionP = theTrack.GetDefinition();
  G4int AP   = definitionP->GetBaryonNumber();
  G4int ZP   = G4int(definitionP->GetPDGCharge()/eplus+0.5);
  G4double M          = definitionP->GetPDGMass();
  G4ThreeVector pP    = theTrack.Get4Momentum().vect();
  G4double T          = theTrack.GetKineticEnergy()/G4double(AP);   // Units are MeV/nuc 
  G4double E          = theTrack.GetTotalEnergy()/G4double(AP);     // Units are MeV/nuc
  G4int AT         = theTarget.GetA_asInt();
  G4int ZT         = theTarget.GetZ_asInt();
  G4double mpnt  = theTarget.AtomicMass(AT, ZT);
  G4double TotalEPre  = theTrack.GetTotalEnergy() + mpnt;

 //-----
  int Ip  = AP;
  int Ipz = ZP;
  int It  = AT;
  int Itz = ZT;
  int Idp = 0;
  double Elab = E / GeV;//GeV/nuc

 // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
 // ***** WARNING *****
 // The following is a provisional "catch" for ions with A==Z.  The de-excitation
 // and precompound model can produce such nuclei, although they should decay
 // into free protons.  For the moment, DPMJET doesn't treat them ... i.e.
 // the FORTRAN code would crash.  Therefore, return such ions without 
 // nuclear interactions.
  if ((AP > 1 && AP == ZP)||GenerateEvent(Ip,Ipz,It,Itz,Idp,Elab)) {//Qi have to check on DPMJET3
    theParticleChange.SetStatusChange(isAlive);
    theParticleChange.SetEnergyChange(theTrack.GetKineticEnergy());
    theParticleChange.SetMomentumChange(theTrack.Get4Momentum().vect().unit());
    if (verboseLevel >= 2) {
      G4cout <<"PROJECTILE WITH AP = " <<AP <<"  ==  ZP = " <<ZP
             <<" REJECTED" <<G4endl;
      G4cout <<"########################################"
             <<"########################################"
             <<G4endl;
    }
    return &theParticleChange;
  }
//-----
   if (verboseLevel >= 3) {
     G4cout <<"--------------------------------------------------------------------------------"<<G4endl;
     G4cout <<"Total before collision ZP="<<ZP<<" AP="<<AP<<" ZT="<<ZT<<" AT="<<AT<<G4endl;
     G4cout <<"Total energy before collision   = " <<TotalEPre/GeV<<" GeV" <<G4endl;
     G4cout <<"Total momentum before collision = " <<pP/GeV<<" GeV/c" <<G4endl;
     G4cout <<"Total charge before collision   = " <<(ZP+ZT)*eplus<<G4endl;
     G4cout <<"Total baryon number before collision = "<<AP+AT<<G4endl;
     G4cout <<"Total lepton number before collision = 0"<<G4endl;
  }

//-----
  ImportParticles(1);
  return &theParticleChange;  
}



//====================== access to dpmjet subroutines =========================
int  ADPMjet_Model::GenerateAMSTable(char *dbdir){
   const int nelem=19;//only A is improtant, AMS detector material
   const int amselemz[nelem]={
    1, 2, 5,  6,  7,  8,  9,  11, 12, 13, 14, 26, 29, 51,  54,  60,  74,  79,  82, //P/B/C/N/O/F/Mg/Al/Si/Fe/Nd/Xe/Sb/W/Au/Pb
   };
   const int amselema[nelem]={
    1, 4, 11, 12, 14, 16, 19, 23, 24, 27, 28, 56, 64, 122, 131, 144, 184, 197, 208,//P/B/C/N/O/F/Mg/Al/Si/Fe/Nd/Xe/Sb/W/Au/Pb
   };
   DTCOMP.ncompo=nelem;
   for(int i=0;i<DTCOMP.ncompo;i++){
     DTCOMP.iemuch[i]=amselemz[i];
     DTCOMP.iemuma[i]=amselema[i]; 
   }
//----
   int Ncase=-100;
   double Epmax=3e8;//max energy up to 3*10^5TV
   int Apmax=100;
   int Zpmax=90;
   int Atmax=amselema[nelem-1];
   int Ztmax=amselemz[nelem-1];
   int Idp=0;
   int Iglau=0;
//  Set DPMJET database directory
   Dt_Setdpmdir(dbdir);
//  Call DPMJET initialisation0
   dt_init(Ncase,Epmax,Apmax,Zpmax,Atmax,Ztmax,Idp,Iglau);
   return 0;
}

int  ADPMjet_Model::GenerateCards(char *dcfile){

  FILE* out = fopen(dcfile,"w");
//  Projectile and Target definition 

    if (fIp == 1 && fIpz ==1) {
        fprintf(out, "PROJPAR                                                               PROTON\n");
    } else if (fIp == 1 && fIpz == -1) {
        fprintf(out, "PROJPAR                                                               APROTON\n");
    } else {
        fprintf(out, "PROJPAR   %10.1f%10.1f%10.1f%10.1f%10.1f%10.1f\n", (float) fIp, (float) fIpz,  0., 0., 0., 0.);
    }

    if (fIt == 1 && fItz ==1) {
        fprintf(out, "TARPAR                                                                PROTON\n");
    } else if (fIt == 1 && fItz == -1) {
        fprintf(out, "TARPAR                                                                APROTON\n");
    } else {
        fprintf(out, "TARPAR    %10.1f%10.1f%10.1f%10.1f%10.1f%10.1f\n", (float) fIt, (float) fItz,  0., 0., 0., 0.);
    }

// projectile particle energy/nucleon
     fprintf(out, "ENERGY      %10.1f%10.1f%10.1f%10.1f%10.1f%10.1f\n",fEpn, 0., 0., 0., 0., 0.);

//  START card
  fprintf(out, "START            1.0       0.0\n");
  fprintf(out, "STOP\n");
  fclose(out);
  return 0;
}


//______________________________________________________________________________
void ADPMjet_Model::Initialize(char *dcfile, char *dbdir)
{

    int nEvt=-1;
    if((fDPMKEY%10)==2){//no table (slow)
      nEvt=-2;
    }
    else if((fDPMKEY%10)==1){//with calculated table (fast)
      nEvt=-1;
    }
    else if(dcfile){//with datacard
      GenerateCards(dcfile); 
      G4int opened = 0;
      int namelen=strlen(dcfile);
      dpmjet_openinp(&namelen, &opened,dcfile);
      nEvt=1;
    }
//  Set DPMJET database directory
    Dt_Setdpmdir(dbdir);
//  Set cascade option
    Dt_Seticasca(2);
//  Call DPMJET initialisation
    int iemu = 0; // No emulsion (default)
    Dt_Dtuini(nEvt, fEpn, fIp, fIpz, fIt, fItz, fIdp, iemu);
    G4cout<<"ADPMjet_Model::end Init="<<" fIdp="<<fIdp<<G4endl;
}


//______________________________________________________________________________
void ADPMjet_Model::Dt_Setdpmdir(char *dbdir)
{
   G4String dpmdirname;
   if(dbdir)dpmdirname=G4String(dbdir)+"/ ";
   else     dpmdirname=G4String(getenv("G4DPMJET3_DATA"))+"/ ";
   dpmdirname.copy(DPMDIR.dpmdir,dpmdirname.length(),0);
   G4cout<<"dpm3dir="<<DPMDIR.dpmdir<<G4endl;
}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Dtuini(int nevts, double epn, int npmass, int npchar, 
   			int ntmass, int ntchar, int idp, int iemu)
{
  // Call dmpjet routine DT_DTUINI passing the parameters 
  // in a way accepted by Fortran routines				   
     
  if (verboseLevel >= 3){
     G4cout<<"\n-------------------------------------------\n";
     G4cout<<"\n		Dt_Dtuini called with:\n\n";
     G4cout<<" Projectile	-> A = "<<npmass<<" Z = "<<npchar<<" \n";
     G4cout<<" Target    	-> A = "<<ntmass<<" Z = "<<ntchar<<" \n";
     G4cout<<" Proj. LAB E	-> E = "<<epn    <<" GeV \n";
     G4cout<<" nevts = "<<nevts<<" idp = "<<idp<<" iemu = "<<iemu<<" \n";
     G4cout<<"\n-------------------------------------------"<<G4endl;
   }
   dt_dtuini(nevts, epn, npmass, npchar, ntmass, ntchar, idp, iemu);
    
}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Kkinc(int npmass, int npchar, int ntmass, int ntchar, 
   		       int idp, double elab, int kkmat, int& irej)
{
  // Call dmpjet routine DT_KKINC passing the parameters 
  // in a way accepted by Fortran routines				   
  if(idp<=0)idp=1;
  dt_kkinc(npmass, npchar, ntmass, ntchar, idp, elab, kkmat, irej);
}

//______________________________________________________________________________
void ADPMjet_Model::Pho_Phist(int imode, double weight)
{
  // Call dmpjet routine PHO_PHIST passing the parameters 
  // in a way accepted by Fortran routines
  
  pho_phist(imode,weight);				   

}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Dtuout()
{
  // Call dmpjet routine DT_DTUOT passing the parameters 
  // in a way accepted by Fortran routines				   
  
  dt_dtuout();

}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Seticasca(int icasca){
  DTEVNO.icasca=icasca;
}

//______________________________________________________________________________
int ADPMjet_Model::GetEvNum() const
{
	return DTEVT1.nevhkk;
}
//______________________________________________________________________________
int ADPMjet_Model::GetEntriesNum() const
{
	return DTEVT1.nhkk;
}
//______________________________________________________________________________
int ADPMjet_Model::GetNumStablePc() const
{
	int NumStablePc = 0;
	for(int i=0; i<DTEVT1.nhkk; i++){
	   if(DTEVT1.isthkk[i] == 1) NumStablePc++;
	}
	return NumStablePc;
}

//______________________________________________________________________________
double ADPMjet_Model::GetTotEnergy(int opt) const
{
	double TotEnergy = 0.;
	for(int i=0; i<DTEVT1.nhkk; i++){
          if(opt==0){
           if(DTEVT1.isthkk[i] == 1|| DTEVT1.isthkk[i] == -1|| DTEVT1.isthkk[i] == 1000)TotEnergy += DTEVT1.phkk[i][3]; // PHKK[i][3] <-> PHKK(4,i)
          }
          else if(opt==1){
            if(DTEVT1.isthkk[i] == 1)TotEnergy += DTEVT1.phkk[i][3];
  	  }
          else {TotEnergy += DTEVT1.phkk[i][3];}
	}
	return TotEnergy;
}

//______________________________________________________________________________
int ADPMjet_Model::GetStatusCode(int evnum) const 
{
	return DTEVT1.isthkk[evnum];	
}
//______________________________________________________________________________
int ADPMjet_Model::GetPDGCode(int evnum) const   
{
	return DTEVT1.idhkk[evnum];
}
//______________________________________________________________________________
double ADPMjet_Model::Getpx(int evnum) const       
{
	return DTEVT1.phkk[evnum][0];
}
//______________________________________________________________________________
double ADPMjet_Model::Getpy(int evnum) const      
{
	return DTEVT1.phkk[evnum][1];
}
//______________________________________________________________________________
double ADPMjet_Model::Getpz(int evnum) const       
{
	return DTEVT1.phkk[evnum][2];
}
//______________________________________________________________________________
double ADPMjet_Model::GetEnergy(int evnum) const	      
{
	return DTEVT1.phkk[evnum][3];
}
//______________________________________________________________________________
double ADPMjet_Model::GetMass(int evnum) const 	      
{
	return DTEVT1.phkk[evnum][4];
}
//______________________________________________________________________________
int    ADPMjet_Model::GetFragmentA(int evnum) const	
{
	return DTEVT2.idres[evnum];
}
//______________________________________________________________________________
int    ADPMjet_Model::GetFragmentZ(int evnum) const	
{
	return DTEVT2.idxres[evnum];
}
//______________________________________________________________________________
double ADPMjet_Model::GetXSFrac() const 	      
{
	return DTIMPA.xsfrac;
}
//______________________________________________________________________________
double ADPMjet_Model::GetBImpac() const 	      
{
	return DTGLCP.bimpac;
}
//______________________________________________________________________________
double ADPMjet_Model::GetProjRadius() const 	      
{
	return DTGLCP.rproj;
}
//______________________________________________________________________________
double ADPMjet_Model::GetTargRadius() const 	      
{
	return DTGLCP.rtarg;
}
//______________________________________________________________________________
int ADPMjet_Model::GetProjWounded() const
{
	return DTGLCP.nwasam;
}
//______________________________________________________________________________
int ADPMjet_Model::GetTargWounded() const
{
	return DTGLCP.nwbsam;
}

//______________________________________________________________________________
int ADPMjet_Model::GetProjParticipants() const
{
	return DTGLCP.nwtaac;
}
//______________________________________________________________________________
int ADPMjet_Model::GetTargParticipants() const
{
	return DTGLCP.nwtbac;
}
//______________________________________________________________________________
int ADPMjet_Model::GetProcessCode() const
{
	return POPRCS.iproce;
}
//______________________________________________________________________________
double ADPMjet_Model::Dt_Rndm(int idummy)
{
	return dt_rndm(idummy);
}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Rndmst(int na1, int na2, int na3, int nb1)
{
	dt_rndmst(na1, na2, na3, nb1);
}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Rndmin(int u, int c, int cd, int cm, int i, int j)
{
	dt_rndmin(u, c, cd, cm, i, j);
}

//______________________________________________________________________________
void ADPMjet_Model::Dt_Rndmou(int u, int c, int cd, int cm, int i, int j)
{
	dt_rndmou(u, c, cd, cm, i, j);
}


int ADPMjet_Model::NHEP()                    const {return POEVT1.nhep;}
int ADPMjet_Model::ISTHEP(int i)             const {return POEVT1.isthep[i];}
int ADPMjet_Model::IDHEP(int i)              const {return POEVT1.idhep[i];}
double ADPMjet_Model::PHEP(int i, int j)   const {return POEVT1.phep[i][j];}
   
