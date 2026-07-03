#ifndef _ADPMjet
#define _ADPMjet

//+SEQ,CopyRight,T=NOINCLUDE.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// 		  ADPMjet_Model (DPMJET3 Geant4 Interface)		//
//                                                                      //
// This class implements an interface to the DPMJET 3.0 event generator.//
// History
//    Created:       2018-Nov-01  Q.Yan qyan@cern.ch                    //
//////////////////////////////////////////////////////////////////////////

//#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleTable.hh"
#include "G4HadronicInteraction.hh"
#include "G4WilsonAblationModel.hh"
#include "G4VPreCompoundModel.hh"
#include "G4HadFinalState.hh"
#include "G4Track.hh"
#include "G4Nucleus.hh"
#include "G4Fragment.hh"
#include "G4HadProjectile.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include <sstream>


class ADPMjet_Model : public G4HadronicInteraction {

public:
   
   ADPMjet_Model(int Ip=208, int Ipz=82, int It=208, int Itz=82, double Epn=1.e8);
   virtual    ~ADPMjet_Model() {;}
   int         GetADPMKEY();
   int         SetADPMKEY(int usekey);
   virtual int GenerateEvent(int Ip=0, int Ipz=0, int It=0, int Itz=0, int Idp=0, double Elab=0);

   // Geant4 interface
   virtual int ImportParticles(int opt=0);
   void        DeleteAllSecondary();
   void SetDefaultPreCompoundModel (int opt=0);
   G4VPreCompoundModel* GetPreCompoundModel() const;
   void SetPreCompoundModel(G4VPreCompoundModel* value);
   void SetNoDeexcitation ();
   void SetDeexcitationMode(int opt=0);
   void SetEexcFactor(double useexc);
   G4ParticleTable *GetParticleTable() const;
   G4HadFinalState *GetParticleChange();

   G4bool IsApplicable (const G4HadProjectile &theTrack, G4Nucleus &theTarget);
   virtual G4HadFinalState *ApplyYourself(const G4HadProjectile &, G4Nucleus &);

   // Parameters for the generation:
   virtual void  SetNEvent(int iev)    {fNEvent = iev;}
   virtual int GetNEvent()             {return fNEvent;}
   
   virtual void  SetfIp(int Ip)        {fIp = Ip;}
   virtual int GetfIp() const	         {return fIp;}

   virtual void  SetfIpz(int Ipz)      {fIpz = Ipz;}
   virtual int GetfIpz() const         {return fIpz;}

   virtual void  SetfIt(int It)        {fIt = It;}
   virtual int GetfIt() const	         {return fIt;}

   virtual void  SetfItz(int Itz)      {fItz = Itz;}
   virtual int GetfItz() const         {return fItz;}

   virtual void  SetfEpn(double Epn)   {fEpn = Epn;}
   virtual double GetfEpn() const      {return fEpn;}

   virtual void  SetfIdp(int idp)      {fIdp = idp;}
   virtual int GetfIdp() const         {return fIdp;}

   
   // Access to DPMJET common blocks:
   virtual int    GetEvNum() const;	    	  
   virtual int    GetEntriesNum() const;	    	  
   virtual int    GetNumStablePc() const;	    	  
   virtual double GetTotEnergy(int opt=0) const;
   virtual int    GetStatusCode(int evnum) const; 
   virtual int    GetPDGCode(int evnum) const; 
   virtual double Getpx(int evnum) const;  
   virtual double Getpy(int evnum) const;  
   virtual double Getpz(int evnum) const;  
   virtual double GetEnergy(int evnum) const;  
   virtual double GetMass(int evnum) const;
   
   virtual int    GetFragmentA(int evnum) const;	
   virtual int    GetFragmentZ(int evnum) const;	
   
   virtual double GetXSFrac() const;
   virtual double GetBImpac() const;
   virtual double GetProjRadius() const;
   virtual double GetTargRadius() const;
   virtual int    GetProjWounded() const;
   virtual int    GetTargWounded() const;
   virtual int    GetProjParticipants() const;
   virtual int    GetTargParticipants() const;
   virtual int    GetProcessCode() const;

   // Phojet access (POEVT1)
   virtual int NHEP()                 const;
   virtual int ISTHEP(int i)          const;
   virtual int IDHEP(int i)           const;
   virtual double PHEP(int i, int j)  const;
   
   // Access to DPMJET routines:
   virtual int  GenerateAMSTable(char *dbdir=0);
   virtual int  GenerateCards(char *dcfile);
   virtual void Initialize(char *dcfile=0, char *dbdir=0);
   virtual void Dt_Setdpmdir(char *dbdir=0); 
   virtual void Dt_Seticasca(int icasca);
   virtual void Dt_Dtuini(int nevts, double epn, int npmass, int npchar, 
   			  int ntmass, int ntchar, int idp, int iemu);
	
   virtual void Dt_Kkinc(int npmass, int npchar, int ntmass, int ntchar, 
   			 int idp, double elab, int kkmat, int& irej);

   virtual void Pho_Phist(int imode, double weight);

   virtual void Dt_Dtuout();

   virtual double Dt_Rndm(int idummy);   
   virtual void Dt_Rndmst(int na1, int na2, int na3, int nb1);   
   virtual void Dt_Rndmin(int u, int c, int cd, int cm, int i, int j);   
   virtual void Dt_Rndmou(int u, int c, int cd, int cm, int i, int j);   


protected:
   G4VPreCompoundModel     *thePreComp;
   G4ParticleTable         *theParticleTable;
   G4IonTable              *theIonTable;
   
protected:

   int        fNEvent;  // Event number to be generated 
   int        fIp;	  // Projectile mass
   int        fIpz;	  // Projectile charge
   int        fIt;	  // Target mass
   int        fItz;	  // Target charge
   double     fEpn;	  // Beam energy
   int        fIdp;	  // Internal particle code
   int        fDPMKEY;
   double     fEexc;      //Ecited energy bias
   //
   
};


inline int ADPMjet_Model::GetADPMKEY(){
  return fDPMKEY;
}

inline int ADPMjet_Model::SetADPMKEY(int usekey){
  fDPMKEY=usekey;
  return fDPMKEY;
}

inline void ADPMjet_Model::SetEexcFactor(double useexc){
  fEexc=useexc;
}

inline void ADPMjet_Model::DeleteAllSecondary(){
  const G4int nSecondaries = theParticleChange.GetNumberOfSecondaries();
  for(G4int j=0; j<nSecondaries; ++j){
     delete theParticleChange.GetSecondary(j)->GetParticle();
  }
  theParticleChange.Clear();
}

inline G4VPreCompoundModel *ADPMjet_Model::GetPreCompoundModel () const
  {return thePreComp;}


inline void ADPMjet_Model::SetPreCompoundModel(G4VPreCompoundModel *aPreCompoundModel)
  {thePreComp = aPreCompoundModel;}


inline void ADPMjet_Model::SetNoDeexcitation(){
   if(thePreComp){
     delete thePreComp;
     thePreComp = 0;
   }
}

inline void ADPMjet_Model::SetDeexcitationMode(int opt){
  fDPMKEY=fDPMKEY-100*((fDPMKEY/100)%10);//clear
  if(opt>=2)fDPMKEY+=100; 
}

inline G4ParticleTable *ADPMjet_Model::GetParticleTable () const{
  return theParticleTable;
}

inline G4HadFinalState *ADPMjet_Model::GetParticleChange(){
  return &theParticleChange;
}

#endif
