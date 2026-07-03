void DrawAMSGeom()
{
  //--- Definition of a simple geometry
  //   gSystem->Load("libGeom");
//   TGeoManager::Import("/afs/cern.ch/work/q/qyan/AMSVDEV/ams02tgeom.root");
//   TGeoManager::Import("/afs/cern.ch/work/q/qyan/AMSVDEV/ams02.root");
   TGeoManager::Import("/afs/cern.ch/exp/ams/Offline/AMSDataDir/v5.00/ams02tgeom.root");
   gGeoManager->DefaultColors();
   gGeoManager->GetVolume("AMSG_00001")->Draw();//Top
   new TBrowser;
}
