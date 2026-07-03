#ifndef AMSMaterialScanner_H
#define AMSMaterialScanner_H 1

// class description:
//
// AMSMaterialScanner
//

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Event;
class G4EventManager;
class G4UserEventAction;
class G4UserStackingAction;
class G4UserTrackingAction;
class G4UserSteppingAction;
class G4MatScanMessenger;
class G4RayShooter;
class G4Region;

class AMSMSSteppingAction;

class AMSMaterialScanner
{
  public:
    AMSMaterialScanner();
   ~AMSMaterialScanner();

  public:
    void Scan();
    // The main entry point which triggers ray tracing.
    // This method is available only if Geant4 is at Idle state.

  private:
    void DoScan();
    // Event loop
    void StoreUserActions();
    void RestoreUserActions();
    // Store and restore user action classes if defined

  private:
    G4RayShooter   *theRayShooter;
    G4EventManager *theEventManager;

    G4UserEventAction    *theUserEventAction;
    G4UserStackingAction *theUserStackingAction;
    G4UserTrackingAction *theUserTrackingAction;
    G4UserSteppingAction *theUserSteppingAction;
    AMSMSSteppingAction  *theMSSteppingAction;

    G4double xMin, yMin, zMin;
    G4double xMax, yMax, zMax;
    G4double dX, dY, dZ;
    G4int nDiv;

  public:
    inline void Set(G4double xmin, G4double ymin, G4double zmin,
		    G4double xmax, G4double ymax, G4double zmax,
		    G4double dx,   G4double dy,   G4double dz, G4int ndiv) {
      xMin=xmin; yMin=ymin; zMin=zmin;
      xMax=xmax; yMax=ymax; zMax=zmax;
        dX=dx;     dY=dy;     dZ=dz;  nDiv=ndiv;
    }
};

#endif


#ifndef AMSMSSteppingAction_h
#define AMSMSSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class AMSMSSteppingAction : public G4UserSteppingAction
{
  public:
    AMSMSSteppingAction();
    virtual ~AMSMSSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

    void Initialize(G4double zmin, G4double zmax, G4double dz);
    void Reset();

    G4double Get(G4int i, G4int j) const;

  private:
    G4int nZ;
    G4double dZ;
    G4double zMin;
    G4double zMax;
    G4double *eMap;
};

#endif
