// Custom AMS class to construct G4TessellatedSolid object based on the provided GDML model.
// The main use is to describe the L0 cupola's geometry, which has a complex shape and cannot
// be properly described via simple solids.
#ifndef __AMS_GEOMETRYCONVERTER__
#define __AMS_GEOMETRYCONVERTER__

#ifdef __G4AMS__
#include "G4String.hh"
#include "G4TessellatedSolid.hh"

class ams_geometryconverter
{
  public:

    ams_geometryconverter();
    ~ams_geometryconverter();

    G4TessellatedSolid* createTessellatedSolid(const G4String &modelName);

  private:
    G4String errorMessage_;

    void PrintVolumeHierarchy(G4VPhysicalVolume* volume);
};
#endif
#endif
