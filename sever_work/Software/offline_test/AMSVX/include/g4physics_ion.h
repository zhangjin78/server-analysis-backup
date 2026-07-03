#ifndef G4Physics_Ion_h
#define G4Physics_Ion_h 1
// ------------------------------------------------------------
//      AMS GEANT4 new Ion Physics 
//      Ion Eleastic for high Energy->DPM-JET
// ------------------------------------------------------------
//      History
//        Created:       2012-Apr-09  Q.Yan
//        Modified:
// -----------------------------------------------------------
#include "G4Version.hh"
#include "G4VHadronPhysics.hh"
#include "globals.hh"
class G4StrangeletP;
class G4BinaryLightIonReaction;
class G4EMDissociation;
class G4VCrossSectionDataSet;
class G4IonsHEAOCrossSection;
#ifdef G4_USE_DPMJET
class G4DPMJET2_5Model;
class G4DPMJET2_5CrossSection;
#endif
#ifdef G4_USE_DPMJET3
class ADPMjet_Model;
#endif
class IonDPMJETPhysics : public G4VHadronPhysics
{
 public:
    IonDPMJETPhysics();
    virtual ~IonDPMJETPhysics();
    void ConstructProcess();
    int UseInclXX;   
    IonDPMJETPhysics(int useinclxx):G4VHadronPhysics(),UseInclXX(useinclxx){};
 private:

  void AddProcess(const G4String& name, G4ParticleDefinition* part,
                  bool isIon, bool isAnti=false);

  void AddProcessEMD(const G4String& name, G4ParticleDefinition* part);
  G4VCrossSectionDataSet* fTripathi;
  G4VCrossSectionDataSet* fTripathiLight;
  G4VCrossSectionDataSet* fShen;
  G4VCrossSectionDataSet* fIonH;
  G4VCrossSectionDataSet* fEMD;
  G4IonsHEAOCrossSection * HEAOXS;
#ifdef G4_USE_DPMJET
  G4DPMJET2_5CrossSection* dpmXS;
  G4DPMJET2_5Model*        theDPM;
#endif
#ifdef G4_USE_DPMJET3
  ADPMjet_Model*           theDPM3;
#endif
  G4HadronicInteraction*   theIonBCl;
  G4HadronicInteraction*   theIonBC;
  G4HadronicInteraction*   theINCLXXl;
  G4HadronicInteraction*   theINCLXX;
  G4HadronicInteraction*   theFTF;
  G4HadronicInteraction*   theIonBC1;
  G4EMDissociation*        theEMD;
};

#endif
