#include "TrSimUtil.h"


G4Region*         TrSimUtil::g4region_trk = 0;
G4ProductionCuts* TrSimUtil::g4cuts_trk = 0;


double TrSimUtil::GetElectronRangeInSilicon(double T) {
  double x = log10(T); // log10(MeV)
  double par[3] = {3.33216,1.24818,-0.176975};
  return pow(10.,par[0] + par[1]*x + par[2]*pow(x,2.0)); // micron 
}


double TrSimUtil::GetElectronEnergyThresholdFromRangeInSilicon(double range) {
  double x = log10(range); // log10(micron)
  double par[6] = {-2.21229,1.09095,-0.629586,0.304429,-0.0626908,0.00494443};
  return pow(10.,par[0] + par[1]*x + par[2]*pow(x,2.0) + par[3]*pow(x,3.0) + par[4]*pow(x,4.0) + par[5]*pow(x,5.0)); // MeV
}


void TrSimUtil::AddRootLogicalVolume(G4LogicalVolume* logic_volume) {
  // check
  if (!logic_volume) {
    printf("TrSimUtil::AddRootLogicalVolume-E no valid logic volume passed. Skip.\n");
    return;
  }
  // check material (?)
  // create region if not existing
  if (!g4region_trk) {
     printf("TrSimUtil::AddRootLogicalVolume-V creating G4Region ActiveSiliconRegion.\n");
     g4region_trk = new G4Region("ActiveSiliconRegion");
  }
  // add volume to region
  g4region_trk->AddRootLogicalVolume(logic_volume);
  // printf("TrSimUtil::AddRootLogicalVolume-V volume %s, and its daughters, added to ActiveSiliconRegion.\n",logic_volume->GetName().data()); // debug
}


void TrSimUtil::SetCuts(double default_cut) {
  // check
  if (!g4region_trk) {
    printf("TrSimUtil::SetCuts-E no Tracker G4Region defined. TrSimUtil::AddRootLogicalVolume should be called before this method.\n");
    return;
  }
  // create cuts
  if (!g4cuts_trk) g4cuts_trk = new G4ProductionCuts;
  // default range = minimum between silicon vertical width and default cut
  double silicon_vertical_width = 0.3; // mm
  default_cut = (default_cut<silicon_vertical_width) ? default_cut : silicon_vertical_width;  
  g4cuts_trk->SetProductionCut(default_cut*mm); 
  printf("TrSimUtil::SetCuts-V creating all particles production cut (cut = %7.3f mm).\n",default_cut);
  // delta-rays production cut
  double min_range = 1e-3*TRMCFFKEY.MinDeltaRayRange; // mm
  g4cuts_trk->SetProductionCut(min_range*mm,"gamma");
  g4cuts_trk->SetProductionCut(min_range*mm,"e-");
  g4cuts_trk->SetProductionCut(min_range*mm,"e+");
  printf("TrSimUtil::SetCuts-V creating e+, e- (delta-rays) and gamma range production cut (range = %7.6f mm).\n",min_range);
  // set cuts
  g4region_trk->SetProductionCuts(g4cuts_trk);
}


void TrSimUtil::EndOfRun() {
  if (g4region_trk) (G4RegionStore::GetInstance())->DeRegister(g4region_trk);
}

