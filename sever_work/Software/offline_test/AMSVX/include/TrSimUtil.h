#ifndef __TrSimUtil__
#define __TrSimUtil__


#include "commons.h"
#include "gmat.h"
#include "globals.hh"


#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"
#include "G4UserLimits.hh"


//! Geant4 Utilities for Tracker (prodution and tracking thresholds)
class TrSimUtil {

 public:

  //! Tracker region needed for special tracking cuts  
  static G4Region*         g4region_trk;
  //! Production thresholds (delta-rays)
  static G4ProductionCuts* g4cuts_trk;

 public:

  //! Interface to the G4Region volume creation 
  static void AddRootLogicalVolume(G4LogicalVolume* logic_volume); 
  //! Get the electron range in Silicon (um) for a given kinetic energy (MeV)
  /* Private fit of ESTAR tables */   
  static double GetElectronRangeInSilicon(double T /*MeV*/);
  //! Get the electron kinetic energy (MeV) for a given range in Silicon (micron)
  static double GetElectronEnergyThresholdFromRangeInSilicon(double range /*um*/);
  //! Set cuts for the Tracker Silicon region
  /* Must be called afted G4Region definition and SetCutsWithDefaults() */
  static void SetCuts(double default_cut);
  //! Deregister region
  static void EndOfRun();

};

#endif
