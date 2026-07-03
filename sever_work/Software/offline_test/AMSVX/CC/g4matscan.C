#include "g4matscan.h"
#include "job.h"

#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include "G4MatScanMessenger.hh"
#include "G4RayShooter.hh"
#include "G4MSSteppingAction.hh"
#include "G4GeometryManager.hh"
#include "G4StateManager.hh"
#include "G4Event.hh"
#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4SDManager.hh"

#include "TString.h"
#include "TFile.h"
#include "TH3.h"

namespace AMSMscan {
  enum { NZ = 14, Ne = 9 };
                // 1  2  3  4  5  6  7  8  9 10 11 12 13 14
  G4int Zi[NZ] = { 0,-1,-1,-1,-1, 1, 2, 3, 4,-1, 5,-1, 6, 7 };
  G4int Ze[Ne] = { 1, 6, 7, 8, 9, 11, 13, 14, 82 };
};

AMSMaterialScanner::AMSMaterialScanner()
{
  theRayShooter = new G4RayShooter();
  theEventManager = G4EventManager::GetEventManager();

  theUserEventAction    = 0;
  theUserStackingAction = 0;
  theUserTrackingAction = 0;
  theUserSteppingAction = 0;
  theMSSteppingAction   = 0;

  xMin = yMin = zMin = xMax = yMax = zMax = dZ = dY = dZ = 0;
  nDiv = 0;
}

AMSMaterialScanner::~AMSMaterialScanner()
{
  delete theRayShooter;
  delete theMSSteppingAction;
}

void AMSMaterialScanner::Scan()
{
  G4StateManager     *theStateMan = G4StateManager::GetStateManager();
  G4ApplicationState currentState = theStateMan->GetCurrentState();
  if(currentState != G4State_Idle) {
    G4cerr << "Illegal application state - Scan() ignored." << G4endl;
    return;
  }
  if (dZ == 0) {
    G4cerr << "Call Set() before Scan()" << G4endl;
    return;
  }

  if (!theMSSteppingAction) theMSSteppingAction = new AMSMSSteppingAction;
  theMSSteppingAction->Initialize(zMin, zMax, dZ);

  StoreUserActions();
  DoScan();
  RestoreUserActions();
}

void AMSMaterialScanner::StoreUserActions()
{ 
  theUserEventAction    = theEventManager->GetUserEventAction();
  theUserStackingAction = theEventManager->GetUserStackingAction();
  theUserTrackingAction = theEventManager->GetUserTrackingAction();
  theUserSteppingAction = theEventManager->GetUserSteppingAction();

  theEventManager->SetUserAction((G4UserEventAction   *)0);
  theEventManager->SetUserAction((G4UserTrackingAction*)0);
  theEventManager->SetUserAction((G4UserStackingAction*)0);
  theEventManager->SetUserAction(theMSSteppingAction);

  G4SDManager *theSDMan = G4SDManager::GetSDMpointerIfExist();
  if (theSDMan) theSDMan->Activate("/", false);

  G4GeometryManager *theGeomMan = G4GeometryManager::GetInstance();
  theGeomMan->OpenGeometry();
  theGeomMan->CloseGeometry(true);
}

void AMSMaterialScanner::RestoreUserActions()
{
  theEventManager->SetUserAction(theUserEventAction);
  theEventManager->SetUserAction(theUserStackingAction);
  theEventManager->SetUserAction(theUserTrackingAction);
  theEventManager->SetUserAction(theUserSteppingAction);

  G4SDManager* theSDMan = G4SDManager::GetSDMpointerIfExist();
  if(theSDMan) theSDMan->Activate("/", true); 
}

void AMSMaterialScanner::DoScan()
{
  G4int nX = (xMax-xMin)/dX;
  G4int nY = (yMax-yMin)/dY;
  G4int nZ = (zMax-zMin)/dZ;
  
  G4int ntot = nX*nY*nDiv*nDiv;
  G4cout << "n= " << nX << " "<< nY << " " << ntot << G4endl;

  TString sfn = AMSJob::gethead()->GetRootPath();
  sfn.ReplaceAll(" ", "");
  sfn += "/g4mscan.root";

  TFile of(sfn, "recreate");
  if (!of.IsOpen()) return;

  G4cout << "Open output file: " << sfn.Data() << G4endl;

  // Confirm material table is updated
  G4RunManagerKernel::GetRunManagerKernel()->UpdateRegion();

  // Close geometry and set the application state
  G4GeometryManager *geomManager = G4GeometryManager::GetInstance();
  geomManager->OpenGeometry();
  geomManager->CloseGeometry(1,0);

  G4ThreeVector center(0,0,0);
  G4Navigator *navigator = G4TransportationManager
    ::GetTransportationManager()->GetNavigatorForTracking();
  navigator->LocateGlobalPointAndSetup(center, 0, false);

  G4StateManager *theStateMan = G4StateManager::GetStateManager();
  theStateMan->SetNewState(G4State_GeomClosed); 
 
  for (G4int i = 0; i < AMSMscan::Ne; i++)
    new TH3F(Form("hist%d", i+1), Form("Z= %d", AMSMscan::Ze[i]),
	     nX, xMin, xMax, nY, yMin, yMax, nZ, zMin, zMax);

  G4int iEvent = 0;
  for (G4int ix = 0; ix < nX*nDiv; ix++)
    for (G4int iy = 0; iy < nY*nDiv; iy++) {
      G4double ex = xMin+(ix+0.5)*dX/nDiv;
      G4double ey = yMin+(iy+0.5)*dY/nDiv;
      G4double ez = zMax;

      G4ThreeVector pos(ex*cm, ey*cm, ez*cm);
      G4ThreeVector dir(0, 0, -1);

      G4Event *anEvent = new G4Event(iEvent);
      theMSSteppingAction->Reset();
      theRayShooter->Shoot(anEvent, pos, dir);
      theEventManager->ProcessOneEvent(anEvent);

      for (G4int i = 0; i < AMSMscan::Ne; i++) {
	TH3F *hist = (TH3F *)of.Get(Form("hist%d", i+1));
	for (G4int iz = 0; hist && iz < nZ; iz++) 
	  hist->Fill(ex, ey, zMin+(iz+0.5)*dZ,
		     theMSSteppingAction->Get(i, iz)/nDiv/nDiv);
      }

      if (iy == 0)
	G4cout << Form("G4SCAN %8d (%5.1f%%) %7.2f %7.2f",
		       iEvent, 100.*iEvent/ntot, ex, ey) << G4endl;
      iEvent++;
      delete anEvent;
    }

  of.Write();

  theStateMan->SetNewState(G4State_Idle); 
}

#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "G4Material.hh"

AMSMSSteppingAction::AMSMSSteppingAction()
{
  eMap = 0;
}

AMSMSSteppingAction::~AMSMSSteppingAction()
{
  delete [] eMap;
}
 
void AMSMSSteppingAction::Initialize(G4double zmin, G4double zmax, G4double dz)
{
  if (dz == 0) return;

  zMin = zmin*cm;
  zMax = zmax*cm;
  if (zMax < zMin) { G4double swp = zMax; zMax = zMin; zMin = swp; }

  dZ = fabs(dz)*cm;
  nZ = (zMax-zMin)/dZ;
  if (nZ <= 0) return;

  G4int Ne = AMSMscan::Ne;

  eMap = new G4double[nZ*Ne];
  Reset();
}

void AMSMSSteppingAction::Reset()
{
  for (G4int i = 0; i < nZ*AMSMscan::Ne; i++) eMap[i] = 0;
}

G4double AMSMSSteppingAction::Get(G4int i, G4int j) const
{
  return (eMap && 0 <= i && i < AMSMscan::Ne &&
	          0 <= j && j < nZ) ? eMap[i*nZ+j] : 0;
}

void AMSMSSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4StepPoint *PostPoint = step->GetPostStepPoint();
  G4StepPoint *PrePoint  = step->GetPreStepPoint();
  G4VPhysicalVolume *PostPV = PostPoint->GetPhysicalVolume();
  G4VPhysicalVolume *PrePV  = PrePoint ->GetPhysicalVolume();

  G4Material *material = PrePoint->GetMaterial();
  if (!material) return;

  G4double dens = material->GetDensity();

  G4int Zmax = AMSMscan::Ze[AMSMscan::Ne-1];

  G4double z1 = PrePoint ->GetPosition().z();
  G4double z2 = PostPoint->GetPosition().z();
  if (z2 < z1) { G4double swp = z1; z1 = z2; z2 = swp; }
  if (z1 < zMin) z1 = zMin;
  if (z2 > zMax) z2 = zMax;

  G4int i1 = (z1-zMin)/dZ; if (i1 <   0) i1 = 0;
  G4int i2 = (z2-zMin)/dZ; if (i2 >= nZ) i2 = nZ-1;

  G4double l1 = (z1-zMin)-i1*dZ;
  G4double l2 = (z2-zMin)-i2*dZ;

  for (G4int i = 0; i < material->GetNumberOfElements(); i++) {
    const G4Element *elm = material->GetElement(i);
    G4int Z = elm->GetZ();
    if (Z < 1 || Zmax < Z) continue;

    G4int ie = (Z <= AMSMscan::NZ) ? AMSMscan::Zi[Z-1]
            : ((Z == AMSMscan::Ze[AMSMscan::Ne-1]) ? AMSMscan::Ne-1 : -1);
    if (ie < 0 || AMSMscan::Ne <= ie) continue;

    G4double W = material->GetFractionVector()[i];
    G4double A = elm->GetA();
    G4double S = W*dens/A/dZ*cm3;

    eMap[ie*nZ+i1] -= S*l1; 
    eMap[ie*nZ+i2] += S*l2; 
    for (G4int j = i1; j < i2; j++) eMap[ie*nZ+j] += S*dZ;
  }
}

