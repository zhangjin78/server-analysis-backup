// Custom AMS class to construct G4TessellatedSolid object based on the provided GDML model.
// The main use is to describe the L0 cupola's geometry, which has a complex shape and cannot
// be properly described via simple solids.
#include <ams_geometryconverter.h>
#include <memory>
#include <utility>
#ifdef __G4AMS__
#include <G4LogicalVolume.hh>
#include <G4GDMLParser.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>
#include <G4SystemOfUnits.hh>

#define VERBOSE false

ams_geometryconverter::ams_geometryconverter()
{
}


ams_geometryconverter::~ams_geometryconverter()
{
}


void ams_geometryconverter::PrintVolumeHierarchy(G4VPhysicalVolume* volume)
{
  G4LogicalVolume* logicalVolume = volume->GetLogicalVolume();
  if (!logicalVolume) {
    errorMessage_ = "ERROR: Logical volume not found for the physical volume " + volume->GetName();
    G4Exception("ams_geometryconverter::PrintVolumeHierarchy", "ams_geometryconverter001", FatalException, errorMessage_);
  }

  if (VERBOSE) {
    G4cout << "ams_geometryconverter:: Logical volume details:" << G4endl;
    G4cout << "ams_geometryconverter::  - Name: " << logicalVolume->GetName() << G4endl;
  }

  if (logicalVolume->GetMaterial()) {
    if (VERBOSE) G4cout << "ams_geometryconverter:: Material: " << logicalVolume->GetMaterial()->GetName() << G4endl;
  } else {
    errorMessage_ = "ERROR: Material not found for the logical volume " + logicalVolume->GetName();
    G4Exception("ams_geometryconverter::PrintVolumeHierarchy", "ams_geometryconverter002", FatalException, errorMessage_);
  }

  G4VSolid* solid = logicalVolume->GetSolid();
    
  if (!solid) {
    errorMessage_ = "ERROR: Solid not found for the logical volume " + logicalVolume->GetName();
    G4Exception("ams_geometryconverter::PrintVolumeHierarchy", "ams_geometryconverter003", FatalException, errorMessage_);
  }

  if (VERBOSE) G4cout << "ams_geometryconverter:: Solid type: " << solid->GetEntityType() << G4endl;

  int numDaughters = logicalVolume->GetNoDaughters();
  if (VERBOSE) G4cout << "ams_geometryconverter:: Number of daughters: " << numDaughters << G4endl;

  for (int i = 0; i < numDaughters; ++i) {
    G4VPhysicalVolume* daughterVolume = logicalVolume->GetDaughter(i);
    PrintVolumeHierarchy(daughterVolume);
  }
}


G4TessellatedSolid* ams_geometryconverter::createTessellatedSolid(const G4String &modelName)
{
  if (VERBOSE) G4cout << "ams_geometryconverter:: Starting construction of G4TessellatedSolid..." << G4endl;

  G4GDMLParser parser;

  parser.Read(modelName);

  if (VERBOSE) G4cout << "ams_geometryconverter:: Constructing world physical volume..." << G4endl;

  G4VPhysicalVolume* worldPhysVol_ = parser.GetWorldVolume();
  if (!worldPhysVol_) {
    errorMessage_ = "ERROR: World physical volume not found!";
    G4Exception("ams_geometryconverter::PrintVolumeHierarchy", "ams_geometryconverter004", FatalException, errorMessage_);
  }

  if (VERBOSE) {
    G4cout << "ams_geometryconverter:: World Physical Volume Details:" << G4endl;
    G4cout << "ams_geometryconverter::  - Name: " << worldPhysVol_->GetName() << G4endl;
    G4cout << "ams_geometryconverter::  - Copy number: " << worldPhysVol_->GetCopyNo() << G4endl;
    G4cout << "ams_geometryconverter::  - Number of daughters: " << worldPhysVol_->GetLogicalVolume()->GetNoDaughters() << G4endl;
  }

  PrintVolumeHierarchy(worldPhysVol_);
  
  G4LogicalVolume* cupolaLogVol = worldPhysVol_->GetLogicalVolume();

  G4VPhysicalVolume* cupolaPhysVol = cupolaLogVol->GetDaughter(0);
  
  G4LogicalVolume* detectorLogVol = cupolaPhysVol->GetLogicalVolume();
  
  G4VSolid* cupolaSolid = detectorLogVol->GetSolid();
  
  G4TessellatedSolid* createdSolid_ = dynamic_cast<G4TessellatedSolid*>(cupolaSolid);

  if (createdSolid_) {
    G4cout << "ams_geometryconverter:: G4TessellatedSolid object created." << G4endl;
    G4int numFacets = createdSolid_->GetNumberOfFacets();
    G4cout << "ams_geometryconverter:: Number of facets: " << numFacets << G4endl;
  } else {
    errorMessage_ = "ERROR: The solid is not tessellated!";
    G4Exception("ams_geometryconverter::PrintVolumeHierarchy", "ams_geometryconverter005", FatalException, errorMessage_);
  }

  return createdSolid_;
}
#endif