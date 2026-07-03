//  $Id: TofSimUtil.C,v 1.30 2023/11/28 23:08:41 qyan Exp $

//Author Qi Yan 2012/Feb/09 23:14
// ------------------------------------------------------------
//      History
//        Modified:  Adding diff surface  2012/05/17
//        Modified:  DB     update        2012/05/27
// -----------------------------------------------------------
#include "TofSimUtil.h"
#include "job.h"
#include "gmat.h"
#include "gvolume.h"
#include "amsgobj.h"
#include "G4ExtrudedSolid.hh"
#include "G4AssemblyVolume.hh"
#include "G4SDManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4MaterialTable.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4TwistedBox.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4UImanager.hh"
#include "G4UserLimits.hh"
#include "G4ReflectionFactory.hh"
#include "G4ReflectedSolid.hh"
#include "G4UnionSolid.hh"
#include "G4PhysicsTable.hh"
#include "geant4.h"
#include "trigger302.h"

//------
int TofSimUtil::Nbar[NLAY]={8,8,10,8};
G4double TofSimUtil::Overlapw=0.5*cm;
G4double TofSimUtil::Normw=12.*cm;
G4double TofSimUtil::Nthick=1.*cm;
G4double TofSimUtil::Zshift=1.55*cm;
G4double TofSimUtil::Zc[NLAY]={
        (66.62-0.145)*cm, (66.62-3.245)*cm,
        (-66.62+3.245)*cm,(-66.62+0.145)*cm
};
G4double TofSimUtil::Honshift[2][2]={//topxy +bowxy
  {0.1*cm,0.1*cm},
  {0.*cm, 0.*cm }
};

G4double TofSimUtil::HonshiftUL[2][2]={//topxy +bowxy
  {0.*cm, 0.*cm},
  {0.*cm, 0.*cm }
};


//--Honeycomb
G4double TofSimUtil::DHoneyH[3]={//make sure only cover Sci_l//no overlap with pmt
 125.*cm,116.*cm,10.*cm //small than 126.5x 117.2y
};
G4double TofSimUtil::PHoneyH[3];
G4double TofSimUtil::DHoneyL[3]={
 129.*cm,109.*cm,5.*cm //small than 130x  110y
};
G4double TofSimUtil::PHoneyL[3];
//--carbon
G4double TofSimUtil::DCarbonR=58.*cm;//117/2.
G4double TofSimUtil::DCarbonZ=0.34*cm;//center of two bar
G4double TofSimUtil::PCarbon[4][3];

//---New Add
//----HoneycombUL
G4double TofSimUtil::DHoneyUL[6][3]={//Manually shink 0.2cm to allow carbon fibre
 {125.*cm,116.*cm,4.95*cm}, //small than 126.5x 117.2y
 {129.*cm,109.*cm,4.7*cm},
 {125.*cm,116.*cm,0.05*cm},
 {116.*cm,116.*cm,0.05*cm},
 {129.*cm,109.*cm,0.05*cm},
 {129.*cm,109.*cm,0.05*cm}
};

G4double TofSimUtil::PHoneyUL[6][3]={// ZR=(76.62,66.62) (-71.62,-66.62)=>(76.62,66.82) (-71.62,66.82)
 {0.*cm, 0.*cm, 74.17*cm},
 {0.*cm, 0.*cm,-69.22*cm},
 {0.*cm, 0.*cm, 74.17*cm+4.95*cm/2.+0.025*cm},
 {0.*cm, 0.*cm, 74.17*cm-4.95*cm/2.-0.025*cm},
 {0.*cm, 0.*cm,-69.22*cm+4.7*cm/2.+0.025*cm},
 {0.*cm, 0.*cm,-69.22*cm-4.7*cm/2.-0.025*cm}
};
//----New2
G4double TofSimUtil::DHoneyUL2[9][3]={//Manually shink 0.2cm to allow carbon fibre
 {125.*cm,116.*cm,0.05*cm},//USkin1
 {125.*cm,116.*cm,4.90*cm},//UHoney1
 {125.*cm,116.*cm,0.05*cm},//USkin1+2
 {125.*cm,116.*cm,4.90*cm},//UHoney2
 {125.*cm,116.*cm,0.05*cm},//USkin2
//----
 {129.*cm,109.*cm,0.05*cm},//LSkin
 {129.*cm,109.*cm,4.70*cm},//LHoney
 {129.*cm,109.*cm,0.05*cm},//LSkin
 {129.*cm,109.*cm,0.05*cm},//LSkinExt
};

G4double TofSimUtil::PHoneyUL2[9][3]={// ZR=(76.695,66.745)  (-71.670,-66.820)
 {0.*cm, 0.*cm, 76.670*cm},//USkin1   Z=(76.695,76.645)
 {0.*cm, 0.*cm, 74.195*cm},//UHoney1  Z=(76.645,71.745)
 {0.*cm, 0.*cm, 71.720*cm},//USkin1+2 Z=(71.745,71.695)
 {0.*cm, 0.*cm, 69.245*cm},//UHoney2  Z=(71.695,66.795)
 {0.*cm, 0.*cm, 66.770*cm},//USkin2   Z=(66.790,66.745)
//----
 {0.*cm, 0.*cm,-66.845*cm},//LSkin    Z=(-66.820,-66.870)
 {0.*cm, 0.*cm,-69.220*cm},//LHoney   Z=(-66.870,-71.570)
 {0.*cm, 0.*cm,-71.595*cm},//LSkin    Z=(-71.570,-71.620)
 {0.*cm, 0.*cm,-71.645*cm},//LSkinExt Z=(-71.620,-71.670)
};


//----Carbon Fibre1
G4double TofSimUtil::DCarbonUL=0.12*cm;

G4double TofSimUtil::PCarbonUL[2][3]={// ZR=(66.71,66.59) (-66.71,-66.59)
 {0.*cm, 0.*cm, 66.65*cm},
 {0.*cm, 0.*cm,-66.65*cm}
};

//---SciH1 
// ZR=(66.475,63.925)

//---Carbon Fibre2+Foam
G4double TofSimUtil::DCarbon2UL=0.4*cm;//2Carbon M=0.55

G4double TofSimUtil::PCarbon2UL[2][3]={//X+Y+Z  ZR=+-(63.85,63.45)
 {0.*cm, 0.*cm,  63.65*cm},
 {0.*cm, 0.*cm, -63.65*cm}
};
//---New2
G4double TofSimUtil::DCarbon2UL2=0.18*cm;

G4double TofSimUtil::PCarbon2UL2[2][3]={//X+Y+Z  ZR=+-(63.74,63.56)
 {0.*cm, 0.*cm,  63.65*cm},
 {0.*cm, 0.*cm, -63.65*cm}
};

//--SciH2
// ZR=+-(63.375,60.825)

//---Carbon Fibre3
G4double TofSimUtil::PCarbon3UL[2][3]={//X+Y+Z ZR=+-(60.725,60.605) 
 {0.*cm, 0.*cm,  60.665*cm},
 {0.*cm, 0.*cm, -60.665*cm}
};
//----New2
G4double TofSimUtil::PCarbon3UL2[2][3]={//X+Y+Z ZR=+-(60.36,60.24)
 {0.*cm, 0.*cm,  60.3*cm},//Yes
 {0.*cm, 0.*cm, -60.3*cm}//Yes
};

//--Honeycomb2UL
G4double TofSimUtil::DHoney2UL=0.12*cm;

G4double TofSimUtil::PHoney2UL[2][3]={//X+Y+Z  ZR=(60.565,60.445)
 {0.*cm, 0.*cm,  60.505*cm},//No
 {0.*cm, 0.*cm, -60.505*cm}//No
};

//----EndAdd

//Sci+LG+PMT
G4double TofSimUtil::Sci_l[NLAY][NBAR]={
	{126.5*cm,130.5*cm,130.5*cm,130.5*cm,130.5*cm,130.5*cm,130.5*cm,126.5*cm,  0.,  0.},//my not accurate
	{117.2*cm,127.0*cm,132.2*cm,132.2*cm,132.2*cm,132.2*cm,127.0*cm,117.2*cm,  0.,  0.},
	{110.0*cm,117.2*cm,127.0*cm,132.2*cm,132.2*cm,132.2*cm,132.2*cm,127.0*cm,117.2*cm,110.0*cm},
	{130.0*cm,134.0*cm,134.0*cm,134.0*cm,134.0*cm,134.0*cm,134.0*cm,130.0*cm,  0.,  0.}
};
G4double TofSimUtil::Sci_w[NLAY][NBAR]={
	{22.5*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,22.5*cm,0.,   0.},
	{25.5*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,25.5*cm,0.,   0.},
	{18.5*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,18.5*cm},
	{26.*cm, 12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,12.*cm,26.*cm, 0.,   0.}
};
G4double TofSimUtil::Sci_lc[NLAY][NBAR]={
	{25.*cm, 0.,0.,0.,0.,0.,0.,25.*cm, 0, 0.},
	{24.3*cm,0.,0.,0.,0.,0.,0.,24.3*cm,0, 0.},
	{20.*cm, 0.,0.,0.,0.,0.,0.,0.,     0.,20.*cm},
	{26.9*cm,0.,0.,0.,0.,0.,0.,26.9*cm,0.,0.}

};
G4double TofSimUtil::Sci_wc[NLAY][NBAR]={
	{-5.*cm,  0.,0.,0.,0.,0.,0.,5.*cm,  0.,0.},
	{-13.5*cm,0.,0.,0.,0.,0.,0.,13.5*cm,0.,0.},//unrotation plane  1-xcut 8+x cut 
	{-6.5*cm, 0.,0.,0.,0.,0.,0.,0.,     0.,6.5*cm},
	{-8.5*cm, 0.,0.,0.,0.,0.,0.,8.5*cm, 0.,0.}
};

G4double TofSimUtil::Sci_t[NLAY][NBAR]={
	{1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,0.,0.},
	{1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,0.,0.},
	{1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm},
	{1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,1.*cm,0.,0.}
};
//----Lg
G4double TofSimUtil::Lg_l[NLAY][NBAR]={
	{2.*cm, 0.,    0.,   0.,   0.,   0.,    0.,    2.*cm, 0.,    0.},
	{10.*cm,5.*cm, 3.*cm,4.*cm,4.*cm,4.5*cm,5.5*cm,10.*cm,0.,    0.},
	{4.5*cm,10.*cm,5.*cm,3.*cm,4.*cm,4. *cm,4.5*cm,5.5*cm,10.*cm,4.5*cm},
	{2.*cm, 0.,    0.,   0.,   0.,   0.,    0.,    2.*cm, 0.,    0.}
};
G4double TofSimUtil::Lg_wc[NLAY][NBAR]={
	{0.49*cm,0.,     0.,     0.,     0.,     0.,     0.,     0.49*cm,0.,     0.},
	{0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.,     0.},
	{0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm,0.49*cm},
	{0.49*cm,0.,     0.,     0.,     0.,     0.,     0.,     0.49*cm,0.,     0.}
};

G4double TofSimUtil::Lg_lc[NLAY][NBAR]={
	{2.*cm, 0.,    0.,    0.,    0.,   0.,     0.,     2.*cm,  0.,   0.},
	{5.*cm, 2.5*cm,1.5*cm,2.*cm, 2.*cm,2.25*cm,2.75*cm,5.*cm,  0.,   0.},
	{4.5*cm,5.*cm, 2.5*cm,1.5*cm,2.*cm,2.*cm,  2.25*cm,2.75*cm,5.*cm,4.5*cm},
	{2.*cm, 0.,    0.,    0.,    0.,   0.,     0.,     2.*cm,  0.,   0.}
};
/*
G4double TofSimUtil::Lg_lc[NLAY][NBAR]={
        {2.*cm, 0.,    0.,   0.,   0.,   0.,    0.,    2.*cm, 0.,    0.},
        {10.*cm,5.*cm, 3.*cm,4.*cm,4.*cm,4.5*cm,5.5*cm,10.*cm,0.,    0.},
        {4.5*cm,10.*cm,5.*cm,3.*cm,4.*cm,4. *cm,4.5*cm,5.5*cm,10.*cm,4.5*cm},
        {2.*cm, 0.,    0.,   0.,   0.,   0.,    0.,    2.*cm, 0.,    0.}
};
*/
//---bend need correction
/*G4double TofSimUtil::Lgb_a[NLAY][NBAR]={
        {19.9*deg,0.,      0.,      0. ,    0.,      0.,      0.,      19.9*deg,0.,      0.},
	{44.4*deg,44.4*deg,30.*deg, 0.,     44.4*deg,30.*deg, 44.4*deg,44.4*deg,0.,      0.},
	{44.4*deg,44.4*deg,44.4*deg,30.*deg,0.,      44.4*deg,30.*deg, 44.4*deg,44.4*deg,44.4*deg},
	{19.9*deg,0.,      0.,      0.,     0.,      0.,      0.,      19.9*deg,0.,      0.}
};*/
G4double TofSimUtil::Lgb_a[NLAY][NBAR]={// lay0 lay3 minus
        {-5.*deg,0.,      0.,      0. ,    0.,      0.,      0.,      -5.*deg,0.,      0.},//rotate down
        {5.*deg,5.*deg, 5.*deg,  0.,    5.*deg,  5.*deg,5.*deg,  5.*deg, 0.,      0.},//unrotate up
        {-5.*deg, -5.*deg,-5.*deg,-5.*deg,0.,      -5.*deg,-5.*deg,  -5.*deg, -5.*deg,-5.*deg},//unrotate
        {5.*deg,0.,      0.,      0.,     0.,      0.,      0.,      5.*deg,0.,      0.}
};
G4double TofSimUtil::Lgb_r[NLAY][NBAR]={
	{2.53*cm,0.,     0.,     0.,     0.,     0.,     0.,     2.53*cm,0.,     0.},
	{2.53*cm,2.53*cm,2.53*cm,0.,     2.53*cm,2.53*cm,2.53*cm,2.53*cm,0.,     0.},
	{2.53*cm,2.53*cm,2.53*cm,2.53*cm,0.,     2.53*cm,2.53*cm,2.53*cm,2.53*cm,2.53*cm},
	{2.53*cm,0.,     0.,     0.,     0.,     0.,     0.,     2.53*cm,0.,     0.}
};
//--twist part
int TofSimUtil::Npmt[NLAY][NBAR]={
	{3,2,2,2,2,2,2,3,0,0},
	{2,2,2,2,2,2,2,2,0,0},
	{2,2,2,2,2,2,2,2,2,2},
	{3,2,2,2,2,2,2,3,0,0}
};

G4double TofSimUtil::Lgt_l[NLAY][NBAR]={
        {0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,0.},
	{0.,    11.*cm,11.*cm,11.*cm,15.*cm,11.*cm,12.*cm,0. ,   0.,0.},//uncertain
	{12.*cm,0.,    11.*cm,11.*cm,11.*cm,15.*cm,11.*cm,12.*cm,0.,12.*cm},
	{0.,    0.,    0.,    0.,    0.,     0.,   0.,    0.,    0.,0.}
};
/*G4double TofSimUtil::Twista[NLAY][NBAR]={
        {0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,0.},
	{0.,     45.*deg,45.*deg,89.*deg,45.*deg,45.*deg,45.*deg,0.,     0.,0.},
	{45.*deg,0.,     45.*deg,45.*deg,89.*deg,45.*deg,45.*deg,45.*deg,0.,45.*deg},
	{0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,0.}
};*/
G4double TofSimUtil::Twista[NLAY][NBAR]={
        {0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,0.},
        {0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,0.},
        {0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,0.},
        {0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,     0.,0.},      
};
//---conial pmt
G4double TofSimUtil::Lgc_l[NLAY][NBAR]={
  {5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,0., 0.},
  {5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,0., 0.},
  {5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm},
  {5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,5.5*cm,0.,0.}
};
/*G4double TofSimUtil::Pm_s[NLAY][NBAR]={
  {3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,0.,0.},
  {3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,0.,0.},
  {3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm},
  {3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,3.*cm,0.,0.}
};
*/
G4double TofSimUtil::Pm_s[NLAY][NBAR]={
  {2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,0.,0.},
  {2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,0.,0.},
  {2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm},
  {2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,2.7*cm,0.,0.},
};

G4double        TofSimUtil::SCRIND=1.58;
G4double        TofSimUtil::LGRIND=1.49;
G4double        TofSimUtil::VARIND=1.;
G4double        TofSimUtil::LOBSIG[NLAY][NBAR];
///-----PMT eff
G4double        TofSimUtil::QEMAX=0.25;
G4double        TofSimUtil::SCEFFC[NLAY][NBAR];
G4double        TofSimUtil::PHEFFC[NLAY][NBAR][NSIDE][NPMTM];
G4PhysicsTable* TofSimUtil::TOFPM_Et=0;
//--Verbose
int             TofSimUtil::VerboseLevel=2;

TofSimUtil*      TofSimUtil::Head=0;

void TofSimUtil::CreateTofG4(){
  if(TofSimUtil::Head)delete TofSimUtil::Head;
  else {
     TofSimUtil::Head=new TofSimUtil();
   }

}

TofSimUtil::TofSimUtil(){
  init();
  DefineTOF_M();
  if(G4FFKEY.TFNewGeant4%10==1){//New Geant4 online simulation  mode
    DefineTOF_Mt();
    DefineTOF_Su();
    DefineTOF_PME();
    DefineTOF_Limit();
  }
}

TofSimUtil::~TofSimUtil(){
}

void TofSimUtil::init(){
//--read file  
   char fname[1000];
   const char *ldir="/afs/ams.cern.ch/user/qyan/Offline/AMSTOF/tofdata/";
   if(TFMCFFKEY.g4tfdir==1)strcpy(fname,ldir);
   else {
      strcpy(fname,AMSDATADIR.amsdatadir);
      strcat(fname,"tofdata/");
     }
   char tfg4db[100];
   int  tfg4dbv=(TFMCFFKEY.simfvern/100000)%10;
   sprintf(tfg4db,"TofGeant4_v%dpr.dat",tfg4dbv);
   strcat(fname,tfg4db);
//--
   
   cout<<"<------TOF Geant4 Simulation::init: Opening  file : "<<fname<<endl;
   ifstream vlfile(fname,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing TOF Geant4 Simulation-file !!: "<<fname<<endl;
   }

//---SC log Normal sigmal
   for(int ilay=0;ilay<NLAY;ilay++){
     for(int ibar=0;ibar<Nbar[ilay];ibar++){
       if(vlfile){
         vlfile>>LOBSIG[ilay][ibar];//deg
         LOBSIG[ilay][ibar]=LOBSIG[ilay][ibar]*3.1415926/180.;
       }
     }
   }

//---pn eff
  double pneff[NLAY][NBAR];
  double sideff[NLAY][NBAR][NSIDE];
  for(int ilay=0;ilay<NLAY;ilay++){
    for(int ibar=0;ibar<Nbar[ilay];ibar++){
        if(vlfile){
           vlfile>>pneff[ilay][ibar];
           if(pneff[ilay][ibar]<=1){sideff[ilay][ibar][0]=1.;sideff[ilay][ibar][1]=pneff[ilay][ibar];}
           else                    {sideff[ilay][ibar][0]=1./pneff[ilay][ibar];sideff[ilay][ibar][1]=1.;}
        }
      }
    }
//---bar eff
  for(int ilay=0;ilay<NLAY;ilay++){
    for(int ibar=0;ibar<Nbar[ilay];ibar++){
        if(vlfile){
           vlfile>>SCEFFC[ilay][ibar];
        }
      }
    }

//--Photon EFF Correction factor
   cout<<"Photon EFF Correction factor:"<<endl;
   for(int ilay=0;ilay<NLAY;ilay++){
      for(int is=0;is<NSIDE;is++){
        for(int ibar=0;ibar<Nbar[ilay];ibar++){
           for(int ipm=0;ipm<NPMTM;ipm++){
              PHEFFC[ilay][ibar][is][ipm]=sideff[ilay][ibar][is];
            }
         }
       }
    }
//--endflag
   int endflag;
   vlfile>> endflag;
   vlfile.close();
   if(endflag==12345){
     cout<<"<----TOF Geant4::file: "<<fname<<" successfully read !"<<endl;
   }
   else{
//--
     cerr<<"<----Error: read problems TOF Geant4-file!! "<<fname<<endl;
//Photon EFF Correction factor ReInit
     cout<<"ReInit Photon EFF Correction factor:"<<endl;
     for(int ilay=0;ilay<NLAY;ilay++){
       for(int is=0;is<NSIDE;is++){
         for(int ibar=0;ibar<NBAR;ibar++){
           for(int ipm=0;ipm<NPMTM;ipm++){
              PHEFFC[ilay][ibar][is][ipm]=1.;
//              cout<<PHEFFC[ilay][ibar][is][ipm]<<" ";
            }
         }
       }
     }
//---
   }
//---
   

}

void TofSimUtil::DefineTOF_M() {
  G4double a, z,density;

//*Elements
  H = new G4Element("H", "H", z=1., a=1.01*g/mole);
  C = new G4Element("C", "C", z=6., a=12.01*g/mole);
  N = new G4Element("N", "N", z=7., a=14.01*g/mole);
  O = new G4Element("O", "O", z=8., a=16.00*g/mole);
  Si= new G4Element("Si", "Si", z=14.,a=28.00*g/mole);
//Vacuum   
  TOFVa_M = new G4Material("TOFVa",z=1.,a=1.01*g/mole,
                        density=universe_mean_density,kStateGas,0.1*kelvin, 1.e-19*pascal);

//Aluminum
  TOFAl_M = new G4Material("TOFAl",z=13.,a=26.98*g/mole,density=2.70*g/cm3);
  TOFAl_M2 = new G4Material("TOFAl2",z=13.,a=26.98*g/mole,density=2.76*g/cm3);
  TOFAl_MU = new G4Material("TOFAlU",z=13.,a=26.98*g/mole,density=9.00*g/cm3);
  TOFAl_MM = new G4Material("TOFAlM",z=13.,a=26.98*g/mole,density=3.864*g/cm3);
  TOFAl_ML = new G4Material("TOFAlL",z=13.,a=26.98*g/mole,density=5.072*g/cm3);
  TOFAl_MExt = new G4Material("TOFAlExt",z=13.,a=26.98*g/mole,density=3.864*g/cm3);

//Aluminum Honey
  TOFAlHoney_M = new G4Material("TOFAl_Honey",z=13.,a=26.98*g/mole,density=0.085*g/cm3);  

  TOFAlHoneyUL_M[0] = new G4Material("TOFAl_HoneyUL0",z=13.,a=26.98*g/mole,density=0.0591735*g/cm3);//0.566+0.0139=0.5799
  TOFAlHoneyUL_M[1] = new G4Material("TOFAl_HoneyUL1",z=13.,a=26.98*g/mole,density=0.0906042*g/cm3);//0.421+0.0139=0.4349
  TOFAlHoneyUL_M2[0] = new G4Material("TOFAl_HoneyUL2",z=13.,a=26.98*g/mole,density=0.03210*g/cm3);//0.566+0.0139=0.5799
  TOFAlHoneyUL_M2[1] = new G4Material("TOFAl_HoneyUL3",z=13.,a=26.98*g/mole,density=0.03381*g/cm3);//0.421+0.0139=0.4349


  TOFAlHoney2UL_M =   new G4Material("TOFAl_Honey2UL",z=13.,a=26.98*g/mole,density=2.87583*g/cm3);//0.3312+0.0139=0.3451


//Carbon
  TOFCA_M = new G4Material("TOFCA",z=6., a=12.01*g/mole,density=2.265*g/cm3);

//Carbon Fibre
  TOFCAFibre_M = new G4Material("TOFCA_Fibre",z=6., a=12.01*g/mole,density=1.8*g/cm3);

  TOFCAFibreUL_M =  new G4Material("TOFC_FibreUL", z=6., a=12.01*g/mole,density=1.4*g/cm3);//1.4*0.12=0.168
  TOFCAFibre2UL_M = new G4Material("TOFC_Fibre2UL",z=6., a=12.01*g/mole,density=1.365*g/cm3);//0.168*2+1.5*0.28/2=0.546(0.756)
   

//Fe
  TOFFe_M = new G4Material("TOFFe",z=26, a=55.85*g/mole,density=7.87*g/cm3);

//Scintillator EJ-200 C10H11
   char scmatnam[1000];
   for(int ilay=0;ilay<NLAY;ilay++){
     for(int ibar=0;ibar<Nbar[ilay];ibar++){
       sprintf(scmatnam,"TOFSc_l%d_b%d",ilay,ibar);
       TOFSc_MA[ilay][ibar] = new G4Material(G4String(scmatnam), density= 1.032*g/cm3, 2);//-1.032
       TOFSc_MA[ilay][ibar]->AddElement(C, 10);
       TOFSc_MA[ilay][ibar]->AddElement(H, 11);
    }
 }

//LG PMMA C5H802
  TOFLg_M = new G4Material("TOFLg", density=1.16*g/cm3,3);//-1.16
  TOFLg_M->AddElement(C,5);
  TOFLg_M->AddElement(H,8);
  TOFLg_M->AddElement(O,2);

//PMT Si02
  TOFPM_M = new G4Material("TOFPM", density=2.64*g/cm3,2);
  TOFPM_M->AddElement(Si,1);
  TOFPM_M->AddElement(O,2); 
}


void TofSimUtil::DefineTOF_Mt() {
//***Material properties tables Sci//  Energy(eV)=1240.8/Wavelength(nm)
  const G4int ENUMS = 22; //
  G4double PhEnergyS[ENUMS] = {
   2.386*eV,2.433*eV,2.482*eV,2.532*eV,2.585*eV,2.640*eV,2.697*eV,
   2.757*eV,2.788*eV,2.820*eV,2.852*eV,2.886*eV,2.920*eV,2.954*eV,
   3.026*eV,3.064*eV,3.102*eV,3.141*eV,3.182*eV,3.265*eV,3.354*eV,3.447*eV
  };
  G4double Sci_Fast[ENUMS] = {//from wavelenth to energy
   0.015, 0.03, 0.07, 0.13, 0.22, 0.37, 0.50,
   0.62,  0.70, 0.79, 0.91, 0.97, 1.00, 0.83,
   0.39,  0.26, 0.18, 0.13, 0.09, 0.06, 0.0265, 0.014,
  };//fast
  const G4int ENUMSI = 3;
  G4double PhEnergySI[ENUMSI]={
   2.386*eV,2.886*eV,3.447*eV,
  };
  G4double Sci_RIND[ENUMSI];
  for(G4int i=0;i<ENUMSI;i++)Sci_RIND[i]=SCRIND;
  
  G4double Sci_ABSL[ENUMSI]={
   400.*cm, 400.*cm, 400.*cm,
  };

   for(int ilay=0;ilay<NLAY;ilay++){
     for(int ibar=0;ibar<Nbar[ilay];ibar++){
        TOFSc_MtA[ilay][ibar] = new G4MaterialPropertiesTable();
        TOFSc_MtA[ilay][ibar]->AddProperty("FASTCOMPONENT", PhEnergyS, Sci_Fast, ENUMS);
        TOFSc_MtA[ilay][ibar]->AddProperty("SLOWCOMPONENT", PhEnergyS, Sci_Fast, ENUMS);// slow the same
        TOFSc_MtA[ilay][ibar]->AddProperty("RINDEX",        PhEnergySI, Sci_RIND, ENUMSI);
        TOFSc_MtA[ilay][ibar]->AddProperty("ABSLENGTH",     PhEnergySI, Sci_ABSL, ENUMSI);

//---photon yield
        G4double YIELD=SCEFFC[ilay][ibar]*QEMAX*10000./MeV;//quatum eff
        YIELD*=TFMCFFKEY.pheffref;//gap loss also come to front
        TOFSc_MtA[ilay][ibar]->AddConstProperty("SCINTILLATIONYIELD",YIELD);
        TOFSc_MtA[ilay][ibar]->AddConstProperty("RESOLUTIONSCALE", 1.0);
        TOFSc_MtA[ilay][ibar]->AddConstProperty("FASTTIMECONSTANT",2.1*ns);
        TOFSc_MtA[ilay][ibar]->AddConstProperty("SLOWTIMECONSTANT",2.1*ns);
        TOFSc_MtA[ilay][ibar]->AddConstProperty("FASTSCINTILLATIONRISETIME",0.9*ns);
        TOFSc_MtA[ilay][ibar]->AddConstProperty("SLOWSCINTILLATIONRISETIME",0.9*ns);
        TOFSc_MtA[ilay][ibar]->AddConstProperty("YIELDRATIO",0.0);
        TOFSc_MA[ilay][ibar]->SetMaterialPropertiesTable(TOFSc_MtA[ilay][ibar]);

//--Set Birks Constant for the scintillator de/dx/(1+a*de/dx)
        G4double BIRK=TFMCFFKEY.birk*mm/MeV;
        TOFSc_MA[ilay][ibar]->GetIonisation()->SetBirksConstant(BIRK);
     }
  }


//--Lg part
//  const G4int ENUML = 26;
  const G4int ENUML = 3;
  G4double PhEnergyL[ENUML] = {
  2.298*eV, 2.920*eV,3.649*eV
  };
 
  G4double Lg_RIND[ENUML];
  for(G4int i=0;i<ENUML;i++)Lg_RIND[i]=LGRIND;

  G4double Lg_ABSL[ENUML] = {
  300.*cm, 300.*cm, 300.*cm,
  };  

  TOFLg_Mt = new G4MaterialPropertiesTable();
  TOFLg_Mt->AddProperty("RINDEX",    PhEnergyL, Lg_RIND, ENUML);
  TOFLg_Mt->AddProperty("ABSLENGTH", PhEnergyL, Lg_ABSL, ENUML);
  TOFLg_M->SetMaterialPropertiesTable(TOFLg_Mt);
}


void TofSimUtil::DefineTOF_Su(){
//--dielectric absorption prob 
   const G4int ENUMSSA = 3;
   G4double PhEnergySu[ENUMSSA] = {
   2.12103*eV, 2.78831*eV, 3.59652*eV
   };
   
//--sci absorp+ref
  G4double DieAbsorptivity  [ENUMSSA];
  G4double Die_specularspike[ENUMSSA];
  G4double Die_specularlobe [ENUMSSA];
  G4double Die_backscatter  [ENUMSSA];
  for(G4int i=0;i<ENUMSSA;i++){
     DieAbsorptivity[i]  =TFMCFFKEY.absorp; 
     Die_specularlobe[i] =TFMCFFKEY.reflob;
     Die_backscatter[i]  =TFMCFFKEY.refbac;
     Die_specularspike[i]=1.-TFMCFFKEY.reflob-TFMCFFKEY.refbac-TFMCFFKEY.reflamber;
  }

  //---Al cover reflection 
  G4double Gap_RIND[ENUMSSA];
  for(G4int i=0;i<ENUMSSA;i++)Gap_RIND[i]=VARIND;
  
  G4double AlEfficiency[ENUMSSA] = {//detector effeciency
    0.,0.,0.,
  };
  const G4int ENUMSSA1 = 25; //
  G4double PhEnergySu1[ENUMSSA1] = {
  2.12103*eV,2.15791*eV,2.19611*eV,2.23568*eV,2.2767*eV, 2.31925*eV,2.36343*eV,
  2.40932*eV,2.45703*eV,2.50667*eV,2.55835*eV,2.61221*eV,2.66839*eV,2.72703*eV,
  2.78831*eV,2.85241*eV,2.91953*eV,2.98988*eV,3.0637*eV, 3.14127*eV,3.22286*eV,
  3.3088*eV, 3.39945*eV,3.49521*eV,3.59652*eV,
  };
 
  G4double AlReflection[ENUMSSA1] = {//reflection effciecy
   0.894,0.895,0.895,0.897,0.898,0.899,0.90,
   0.901,0.902,0.901,0.90, 0.899,0.898,0.897,
   0.896,0.894,0.892,0.889,0.886,0.883,0.88,
   0.875,0.873,0.87, 0.86
  };

  for(G4int i=0;i<ENUMSSA1;i++){
     AlReflection[i]    =AlReflection[i]*TFMCFFKEY.refskin;
  }

  G4MaterialPropertiesTable *AlPT = new G4MaterialPropertiesTable();//dielect+gap+mirr
  if(TFMCFFKEY.refmodel==1||TFMCFFKEY.refmodel>4){//absorp<0 mean normal polishedbackpainted
   AlPT->AddProperty("ABSORPTIVITY",         PhEnergySu, DieAbsorptivity,  ENUMSSA);//diel absorb
  }
  AlPT->AddProperty("SPECULARLOBECONSTANT", PhEnergySu, Die_specularlobe, ENUMSSA);//lobe face refl
  AlPT->AddProperty("SPECULARSPIKECONSTANT",PhEnergySu, Die_specularspike,ENUMSSA);//reflection
  AlPT->AddProperty("BACKSCATTERCONSTANT",  PhEnergySu, Die_backscatter,  ENUMSSA);//back reflection
  
  AlPT->AddProperty("RINDEX",      PhEnergySu, Gap_RIND,        ENUMSSA);//gap index
  AlPT->AddProperty("EFFICIENCY",  PhEnergySu, AlEfficiency,    ENUMSSA);//al det eff
  AlPT->AddProperty("REFLECTIVITY",PhEnergySu1,AlReflection,    ENUMSSA1);//al ref

//-----Surface build 
   char scsunam[1000];
   for(int ilay=0;ilay<NLAY;ilay++){
      for(int ibar=0;ibar<Nbar[ilay];ibar++){
          G4double lobsc=LOBSIG[ilay][ibar];
          sprintf(scsunam,"ScintAlSurface_l%d_b%d",ilay,ibar);
          if(TFMCFFKEY.refmodel==1||TFMCFFKEY.refmodel==2){
             TOFSC_SuA[ilay][ibar]=new G4OpticalSurface(G4String(scsunam),unified,polishedbackpainted,dielectric_dielectric,lobsc);
          }
          else if(TFMCFFKEY.refmodel==3){
             TOFSC_SuA[ilay][ibar]=new G4OpticalSurface(G4String(scsunam),unified,groundbackpainted,dielectric_dielectric,lobsc);
          }
          else if(TFMCFFKEY.refmodel==4){
             TOFSC_SuA[ilay][ibar]=new G4OpticalSurface(G4String(scsunam),unified,ground,dielectric_metal,lobsc);
          }
          else { //refmodel==5
             TOFSC_SuA[ilay][ibar]=new G4OpticalSurface(G4String(scsunam),unified,polishedbackpainted,dielectric_metal,lobsc);
          }
          //--Al polish factor
          if(TFMCFFKEY.refmodel==1||TFMCFFKEY.refmodel>4){
             G4double scskpolish=TFMCFFKEY.scskpol;
             TOFSC_SuA[ilay][ibar]->SetPolish(scskpolish);//Al polish factor
          }
          TOFSC_SuA[ilay][ibar]->SetMaterialPropertiesTable(AlPT);
      }
    }

//--PMT Detector Eff //PMT eff depend on angle and quatum
  const G4int PMENUMS = 3;
  G4double PhEnergyP[PMENUMS]={
   2.12103*eV,2.72703*eV,3.59652*eV,
  };
  G4double PM_EFF[PMENUMS]={
   1.,1.,1.,
  };
  G4double PM_Reflection[PMENUMS]={
   0.,0.,0.,
  };

  G4MaterialPropertiesTable *PMTPT = new G4MaterialPropertiesTable();
  PMTPT->AddProperty("REFLECTIVITY", PhEnergyP, PM_Reflection, PMENUMS);
  PMTPT->AddProperty("EFFICIENCY",PhEnergyP, PM_EFF,PMENUMS);
  TOFPM_Su=new G4OpticalSurface("PMTAbsorbSurface",glisur,polished,dielectric_metal);
  TOFPM_Su->SetMaterialPropertiesTable(PMTPT);
}


void TofSimUtil::DefineTOF_PME(){
  const G4int ENUM =23;
  G4double PhEnergy[ENUM] = {
   1.823*eV,1.909*eV,1.970*eV,2.060*eV,2.166*eV,2.204*eV,2.341*eV,2.436*eV,
   2.536*eV,2.672*eV,2.812*eV,3.095*eV,3.322*eV,3.582*eV,3.720*eV,3.877*eV,
   3.969*eV,4.107*eV,4.221*eV,4.306*eV,4.391*eV,4.591*eV,4.730*eV
  };
  G4double EnEFF[ENUM] = {//2.482-3.265
   0.0008,0.0046,0.0111,0.0271,0.0499,0.0627,0.1092,0.1514,
   0.1825,0.2035,0.2223,0.2396,0.2439,0.2313,0.2030,0.1723,
   0.1490,0.1059,0.0648,0.0457,0.0292,0.01,  0.0051
  };

  const G4int ANUM =19;
  G4double CosAn[ANUM] ={
   0,    0.077,0.157,0.248,0.332,0.416,0.496,0.570,
   0.641,0.704,0.763,0.818,0.865,0.906,0.940,0.964,
   0.985,0.996,1
  };
  G4double AnEFF[ANUM] ={
   0,    0.068,0.198,0.342,0.459,0.566,0.669,0.744,
   0.782,0.849,0.886,0.926,0.943,0.957,0.966,0.971,
   0.965,0.972,0.994
  };
  if (TOFPM_Et)return;
  TOFPM_Et= new G4PhysicsTable();
  G4PhysicsOrderedFreeVector* PMTEnVector=new G4PhysicsOrderedFreeVector();
  for (int i=0; i<ENUM; i++){
     PMTEnVector->InsertValues(PhEnergy[i], EnEFF[i]);
   }
   TOFPM_Et->insertAt(0,PMTEnVector);

  G4PhysicsOrderedFreeVector* PMTAnVector=new G4PhysicsOrderedFreeVector();
  for (int i=0; i<ANUM; i++){
     PMTAnVector->InsertValues(CosAn[i], AnEFF[i]);
   }
   TOFPM_Et->insertAt(1,PMTAnVector);
}

void TofSimUtil::DefineTOF_Limit(){
  G4double maxStep = 2.*m;//--step cut
  G4double maxTrackL=0.02*m;//Track Max length
  G4double maxTime=DBL_MAX;
  G4double minEkine=2.1*eV;     //    = 250.0*eV; // minimum kinetic energy required in volume
  TOF_Limit = new G4UserLimits(maxStep,
                                maxTrackL, //Track Max length
                                maxTime,// Track Max Time
                                minEkine);//Track min E

}



bool TofSimUtil::MakeTOFG4Volumes(AMSgvolume *mother){
   
   G4LogicalVolume   *mvol=mother->pg4l();
   G4VPhysicalVolume *mvop=mother->pg4v();
   G4cout<<"---------->>TOF Geant4 Geometry Init"<<G4endl;
   int nrot=5000;//temp
   char scname[100];
   char pmname[100];
   char lgcname[100];
   tofgvol=new TOFgvolume("TOFMAT","TOFSHAP",0,0,0);
   for(int ilay=0;ilay<NLAY;ilay++){
      for(int ibar=0;ibar<Nbar[ilay];ibar++){
         InitialBarPar(ilay,ibar);
         int sciid,pmtid; 
         sciid=100*(ilay+1)+ibar+1;
         //int kk=100*(ilay+1)+ibar+1;
         int kk=ilay*10+ibar+1;//different from id
         if(kk>=10)sprintf(scname,"TF%d",kk);
         else      sprintf(scname,"TF0%d",kk);
         TOFSc_M=TOFSc_MA[ilay][ibar];
         G4AssemblyVolume *AsBar=ConstructSubBar(G4String(scname));
//--bar pos
         G4RotationMatrix BarRot;
         BarRot.rotateZ(barrot[0]);
         BarRot.rotateY(barrot[1]);
         G4ThreeVector BarPos(barp[0],barp[1],barp[2]);
         G4Transform3D BarTr=G4Transform3D(BarRot,BarPos);
         AsBar->MakeImprint(mvol,BarTr);//already add to out mother volume 
//--set id
         pm_log1=G4ReflectionFactory::Instance()->GetReflectedLV(pm_log);
         lgc_log1=G4ReflectionFactory::Instance()->GetReflectedLV(lgc_log);
//--set id
	 G4LogicalVolume*  lgp_log1=G4ReflectionFactory::Instance()->GetReflectedLV(lgp_log);
	 //--set id
	 G4LogicalVolume*  lgt_log1=G4ReflectionFactory::Instance()->GetReflectedLV(lgt_log);
	 G4LogicalVolume*  lgb_log1=G4ReflectionFactory::Instance()->GetReflectedLV(lgb_log);

         int Barid=ilay*1000+ibar*100;//id lbsp//lay bar side pmt
         std::vector<G4VPhysicalVolume*>::iterator Barphv=AsBar->GetVolumesIterator();
         int ipm=0;
         for(unsigned int ivo=0;ivo<AsBar->TotalImprintedVolumes();ivo++){
//             G4cout<<"ivo="<<ivo<<G4endl;
            if((*(Barphv+ivo))->GetLogicalVolume()==sci_log){
               (*(Barphv+ivo))->SetName(G4String(scname));
               (*(Barphv+ivo))->SetCopyNo(G4int(sciid));//sci volume
             }
            else if(((*(Barphv+ivo))->GetLogicalVolume()==pm_log)||((*(Barphv+ivo))->GetLogicalVolume()==pm_log1)){
               if(ipm<npmt)pmtid=Barid+1*10+ipm;//pside //pmt
               else        pmtid=Barid+0*10+(ipm-npmt);//nside
               if(Lgb_a[ilay][ibar]<0)pmtid=(pmtid/10)*10+(npmt-1-pmtid%10);
               sprintf(pmname,"TOFPM%d",pmtid);
               (*(Barphv+ivo))->SetName(G4String(pmname));
               (*(Barphv+ivo))->SetCopyNo(G4int(pmtid));
               ipm++;
            }
           else if((*(Barphv+ivo))->GetLogicalVolume()==lgc_log||((*(Barphv+ivo))->GetLogicalVolume()==lgc_log1)){
              sprintf(lgcname,"TOFLG%d",sciid);
              G4String Lgname=lgcname;
              Lgname=Lgname+(*(Barphv+ivo))->GetName();
              (*(Barphv+ivo))->SetName(Lgname);
	      G4int nn= (*(Barphv+ivo))->GetCopyNo();
	      nn+=10000*(ilay+1)+1000*(ibar+1);
	      (*(Barphv+ivo))->SetCopyNo(nn);
            }
           else if((*(Barphv+ivo))->GetLogicalVolume()==lgp_log||((*(Barphv+ivo))->GetLogicalVolume()==lgp_log1)){
//               sprintf(lgcname,"TOFLG%d",sciid);
//               G4String Lgname=lgcname;
//               Lgname=Lgname+(*(Barphv+ivo))->GetName();
//               (*(Barphv+ivo))->SetName(Lgname);
	      G4int nn= (*(Barphv+ivo))->GetCopyNo();
	      nn+=10000*(ilay+1)+1000*(ibar+1);
	      (*(Barphv+ivo))->SetCopyNo(nn);
            }
           else if((*(Barphv+ivo))->GetLogicalVolume()==lgt_log||((*(Barphv+ivo))->GetLogicalVolume()==lgt_log1)){
//               sprintf(lgcname,"TOFLG%d",sciid);
//               G4String Lgname=lgcname;
//               Lgname=Lgname+(*(Barphv+ivo))->GetName();
//               (*(Barphv+ivo))->SetName(Lgname);
	      G4int nn= (*(Barphv+ivo))->GetCopyNo();
	      nn+=10000*(ilay+1)+1000*(ibar+1);
	      (*(Barphv+ivo))->SetCopyNo(nn);
            }
           else if((*(Barphv+ivo))->GetLogicalVolume()==lgb_log||((*(Barphv+ivo))->GetLogicalVolume()==lgb_log1)){
//               sprintf(lgcname,"TOFLG%d",sciid);
//               G4String Lgname=lgcname;
//               Lgname=Lgname+(*(Barphv+ivo))->GetName();
//               (*(Barphv+ivo))->SetName(Lgname);
	      G4int nn= (*(Barphv+ivo))->GetCopyNo();
	      nn+=10000*(ilay+1)+1000*(ibar+1);
	      (*(Barphv+ivo))->SetCopyNo(nn);
            }
	    //G4cout<<"Barid="<<Barid<<" ivo="<<ivo<<" name="<<(*(Barphv+ivo))->GetName()<<" copy="<<(*(Barphv+ivo))->GetCopyNo()<<G4endl;
           
//---convert to gvol
           if((*(Barphv+ivo))->GetLogicalVolume()==sci_log){
              AMSgvolume *gvol=GetAMSgvolume((*(Barphv+ivo)), nrot);
              mother->add(gvol);
            
            }
           //G4cout<<"add to mother"<<G4endl;
            
//--
        }
//--sensitive
       sci_log->SetSensitiveDetector(AMSG4DummySD::pSD());
#if G4VERSION_NUMBER  > 999
       AMSgvolume::SensMap.insert(make_pair(sci_log,AMSG4DummySD::pSD()));
#endif
       //lgc_log->SetSensitiveDetector(AMSG4DummySD::pSD());
       //lgc_log1->SetSensitiveDetector(AMSG4DummySD::pSD());
//--surface property
       if(G4FFKEY.TFNewGeant4%10==1){
         pm_log-> SetSensitiveDetector(AMSG4DummySD::pSD());
#if G4VERSION_NUMBER  > 999
         AMSgvolume::SensMap.insert(make_pair(pm_log,AMSG4DummySD::pSD()));
#endif
         pm_log1->SetSensitiveDetector(AMSG4DummySD::pSD());
#if G4VERSION_NUMBER  > 999
         AMSgvolume::SensMap.insert(make_pair(pm_log1,AMSG4DummySD::pSD()));
#endif
         TOFSC_Su=TOFSC_SuA[ilay][ibar];
         TOFLG_Su=TOFSC_SuA[ilay][ibar];
         ConstructBarSurface(AsBar,mvop);
       }
    }
   }

  if((G4FFKEY.TFNewGeant4/10)%10==0){
//---UP Honeycomb
     G4VSolid* HoneyH_sol=new G4Box("HoneyH_sol",DHoneyH[0]/2.,DHoneyH[1]/2.,DHoneyH[2]/2.); //Present
     G4LogicalVolume *HoneyH_log=new G4LogicalVolume(HoneyH_sol,TOFAlHoney_M,"HoneyH_log");
     PHoneyH[0]=0.;      PHoneyH[1]=0.;      PHoneyH[2]=DHoneyH[2]/2.+66.62*cm;
     new G4PVPlacement(0,G4ThreeVector(PHoneyH[0],PHoneyH[1],PHoneyH[2]),HoneyH_log,"HoneyCombH",mvol,false,0);
//--DOWN 
     G4VSolid* HoneyL_sol=new G4Box("HoneyL_sol",DHoneyL[0]/2.,DHoneyL[1]/2.,DHoneyL[2]/2.); //Present
     G4LogicalVolume* HoneyL_log=new G4LogicalVolume(HoneyL_sol,TOFAlHoney_M,"HoneyL_log");
     PHoneyL[0]=0.;     PHoneyL[1]=0.;      PHoneyL[2]=-1.*DHoneyL[2]/2.-66.62*cm;
     new G4PVPlacement(0,G4ThreeVector(PHoneyL[0],PHoneyL[1],PHoneyL[2]),HoneyL_log,"HoneyCombL",mvol,false,0); 

//CarbonUP   
     G4VSolid *Carbon_sol=new G4Tubs("Carbon_sol",0.,DCarbonR,DCarbonZ/2.,0.,2*M_PI*rad);
     G4LogicalVolume *Carbon_log=new G4LogicalVolume(Carbon_sol,TOFCAFibre_M,"Carbon_log"); 
     PCarbon[0][0]=0.;  PCarbon[0][1]=0.;   PCarbon[0][2]=(Zc[0]-Zshift-Nthick+Zc[1])/2.;
     cout<<" carbon up 1 cp2n="<<PCarbon[0][2]<<" "<<DCarbonZ<<endl; 
     new G4PVPlacement(0,G4ThreeVector(PCarbon[0][0],PCarbon[0][1],PCarbon[0][2]),Carbon_log,"CarbonP1",mvol,false,0);
     PCarbon[1][0]=0.;  PCarbon[1][1]=0.;   PCarbon[1][2]=Zc[1]-Zshift-Nthick-Nthick/3.;
     cout<<" carbon up 2 cp2n="<<PCarbon[1][2]<<" "<<DCarbonZ<<endl; 
     new G4PVPlacement(0,G4ThreeVector(PCarbon[1][0],PCarbon[1][1],PCarbon[1][2]),Carbon_log,"CarbonP2",mvol,false,1);
//--DOWN
     PCarbon[2][0]=0.;  PCarbon[2][1]=0.;   PCarbon[2][2]=Zc[2]+Zshift+Nthick+Nthick;
     cout<<" carbon down 1 cp2n="<<PCarbon[2][2]<<" "<<DCarbonZ<<endl; 
     new G4PVPlacement(0,G4ThreeVector(PCarbon[2][0],PCarbon[2][1],PCarbon[2][2]),Carbon_log,"CarbonP3",mvol,false,2);
     PCarbon[3][0]=0.;  PCarbon[3][1]=0.;   PCarbon[3][2]=(Zc[3]+Zshift+Nthick+Zc[2])/2.;
     cout<<" carbon down 2 cp2n="<<PCarbon[3][2]<<" "<<DCarbonZ<<endl; 
     new G4PVPlacement(0,G4ThreeVector(PCarbon[3][0],PCarbon[3][1],PCarbon[3][2]),Carbon_log,"CarbonP4",mvol,false,3);
 }

 else if((G4FFKEY.TFNewGeant4/10)%10==1){

   G4cout<<"---------->>TOF Geant4 New Support Structure Geometry Used"<<G4endl;

   for(int iul=0;iul<2;iul++){
//---Honeycomb+Al Wrap Skin
      G4VSolid* Honey_sol=new G4Box(Form("TFHoneyUL%d_sol",iul), DHoneyUL[iul][0]/2.,DHoneyUL[iul][1]/2.,DHoneyUL[iul][2]/2.); //Present
      G4VSolid* Honey_sol1=new G4Box(Form("TFHoneyULU%d_sol",iul), DHoneyUL[iul*2+2][0]/2.,DHoneyUL[iul*2+2][1]/2.,DHoneyUL[iul*2+2][2]/2.); //Present
      G4VSolid* Honey_sol2=new G4Box(Form("TFHoneyULD%d_sol",iul), DHoneyUL[iul*2+3][0]/2.,DHoneyUL[iul*2+3][1]/2.,DHoneyUL[iul*2+3][2]/2.); //Present
      G4LogicalVolume *Honey_log=new G4LogicalVolume(Honey_sol,TOFAlHoneyUL_M2[iul],Form("TFHoneyUL%d_log",iul));//U=0.5799g/cm^2 L=0.4349g/cm^2 
      G4LogicalVolume *Honey_log1=new G4LogicalVolume(Honey_sol1,TOFAl_M2,Form("TFHoneyULU%d_log",iul));//U=0.5799g/cm^2 L=0.4349g/cm^2 
      G4LogicalVolume *Honey_log2=new G4LogicalVolume(Honey_sol2,TOFAl_M2,Form("TFHoneyULD%d_log",iul));//U=0.5799g/cm^2 L=0.4349g/cm^2 
      new G4PVPlacement(0,G4ThreeVector(PHoneyUL[iul][0],PHoneyUL[iul][1],PHoneyUL[iul][2]),Honey_log,Form("TFHoneyUL%d",iul),mvol,false,0);
      new G4PVPlacement(0,G4ThreeVector(PHoneyUL[iul*2+2][0],PHoneyUL[iul*2+2][1],PHoneyUL[iul*2+2][2]),Honey_log1,Form("TFHoneyULU%d",iul),mvol,false,0);
      new G4PVPlacement(0,G4ThreeVector(PHoneyUL[iul*2+3][0],PHoneyUL[iul*2+3][1],PHoneyUL[iul*2+3][2]),Honey_log2,Form("TFHoneyULD%d",iul),mvol,false,0);
      if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFHoneyUL%d",iul)<<" Z="<<(PHoneyUL[iul][2]+DHoneyUL[iul][2]/2.)/cm<<" "<<(PHoneyUL[iul][2]-DHoneyUL[iul][2]/2.)/cm<<G4endl;

//---Carbon Fibre1
      G4VSolid* Carbon_sol=new G4Box(Form("TFCFibreUL%d_sol",iul),DHoneyUL[iul][0]/2.,DHoneyUL[iul][1]/2.,DCarbonUL/2.); //Present
      G4LogicalVolume *Carbon_log=new G4LogicalVolume(Carbon_sol,TOFCAFibreUL_M,Form("TFCFibreUL%d_log",iul));//0.168g/cm^2
      new G4PVPlacement(0,G4ThreeVector(PCarbonUL[iul][0],PCarbonUL[iul][1],PCarbonUL[iul][2]),Carbon_log,Form("TFCFibreUL%d",iul),mvol,false,0);
      if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFCFibreUL%d",iul)<<" Z="<<(PCarbonUL[iul][2]+DCarbonUL/2.)/cm<<" "<<(PCarbonUL[iul][2]-DCarbonUL/2.)/cm<<G4endl;
//---Scintillator1

//---Carbon Fibre2+Foam     
      G4VSolid* Carbon2_sol=new G4Box(Form("TFCFibre2UL%d_sol",iul),DHoneyUL[iul][0]/2.,DHoneyUL[iul][1]/2.,DCarbon2UL/2.); //Present
      G4LogicalVolume *Carbon2_log=new G4LogicalVolume(Carbon2_sol,TOFCAFibre2UL_M,Form("TFCFibre2UL%d_log",iul));//0.546/cm^2
      new G4PVPlacement(0,G4ThreeVector(PCarbon2UL[iul][0],PCarbon2UL[iul][1],PCarbon2UL[iul][2]),Carbon2_log,Form("TFCFibre2UL%d",iul),mvol,false,0);
      if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFCFibre2UL%d",iul)<<" Z="<<(PCarbon2UL[iul][2]+DCarbon2UL/2.)/cm<<" "<<(PCarbon2UL[iul][2]-DCarbon2UL/2.)/cm<<G4endl;
//---Scintillator2

//---Carbon Fibre3
      new G4PVPlacement(0,G4ThreeVector(PCarbon3UL[iul][0],PCarbon3UL[iul][1],PCarbon3UL[iul][2]),Carbon_log,Form("TFCFibre3UL%d",iul),mvol,false,1);//0.168g/cm^2
      if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFCFibre3UL%d",iul)<<" Z="<<(PCarbon3UL[iul][2]+DCarbonUL/2.)/cm<<" "<<(PCarbon3UL[iul][2]-DCarbonUL/2.)/cm<<G4endl;

//---Honeycomb2
      G4VSolid* Honey2_sol=new G4Box(Form("TFHoney2UL%d_sol",iul),DHoneyUL[iul][0]/2.,DHoneyUL[iul][1]/2.,DHoney2UL/2.); //Present
      G4LogicalVolume *Honey2_log=new G4LogicalVolume(Honey2_sol,TOFAlHoney2UL_M,Form("TFHoney2UL%d_log",iul));//0.3451g/cm^2
      new G4PVPlacement(0,G4ThreeVector(PHoney2UL[iul][0],PHoney2UL[iul][1],PHoney2UL[iul][2]),Honey2_log,Form("TFHoney2UL%d",iul),mvol,false,0);
      if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFHoney2UL%d",iul)<<" Z="<<(PHoney2UL[iul][2]+DHoney2UL/2.)/cm<<" "<<(PHoney2UL[iul][2]-DHoney2UL/2.)/cm<<G4endl;
     
//---Total:0.2066X0   Al: 1.705g/cm^2  C: 1.764g/cm^2  C8H10: 4.128g/cm^2
   }
  }

  else {

  G4cout<<"---------->>TOF Geant4 New Support Structure Geometry(2016-02-03) Used"<<G4endl;
  G4double tofspar[5],tofppar[3];
  geant tofgvolpar[5]={0};

  for(int iul=0;iul<2;iul++){

//---Honeycomb
   int np=(iul==0)?5:4;
   for(int ip=0;ip<np;ip++){
     G4Material   *TOFAl_MH=0;
     if(iul==0){//0.3036g/cm^2+0.15729g/cm^2+0.1932g/cm^2+0.15729g/cm^2+0.3036g/cm^2
       if     (ip==0)TOFAl_MH=TOFAl_MU;            //0.3036g/cm^2
       else if(ip==1)TOFAl_MH=TOFAlHoneyUL_M2[iul];//0.15729g/cm^2
       else if(ip==2)TOFAl_MH=TOFAl_MM;            //0.1932g/cm^2
       else if(ip==3)TOFAl_MH=TOFAlHoneyUL_M2[iul];//0.15729g/cm^2
       else if(ip==4)TOFAl_MH=TOFAl_ML;            //0.3036g/cm^2
     }
     else {//0.3036g/cm^2+0.15729g/cm^2+0.3036g/cm^2+0.3036g/cm^2
       if     (ip==0)TOFAl_MH=TOFAl_ML;            //0.3036g/cm^2
       else if(ip==1)TOFAl_MH=TOFAlHoneyUL_M2[iul];//0.15729g/cm^2
       else if(ip==2)TOFAl_MH=TOFAl_MU;            //0.3036g/cm^2
       else if(ip==3)TOFAl_MH=TOFAl_MExt;          //0.3036g/cm^2
     }
     for(int i=0;i<3;i++){tofspar[i]=DHoneyUL2[iul*5+ip][i]/2.; tofgvolpar[i]=tofspar[i]/cm; tofppar[i]=PHoneyUL2[iul*5+ip][i];}
     G4VSolid          *Honey_sol=new G4Box(Form("TFHoneyUL%dP%d_sol",iul,ip),tofspar[0],tofspar[1],tofspar[2]);
     G4LogicalVolume   *Honey_log=new G4LogicalVolume(Honey_sol,TOFAl_MH,Form("TFHoneyUL%dP%d_log",iul,ip));
     G4VPhysicalVolume *Honey_phy=new G4PVPlacement(0,G4ThreeVector(tofppar[0],tofppar[1],tofppar[2]),Honey_log,Form("TFHoneyUL%dP%d",iul,ip),mvol,false,0);
     tofgvol->add(new TOFgvolume(TOFAl_MH->GetName(),"BOX",3,tofgvolpar,Honey_log));
     mother->add(GetAMSgvolume(Honey_phy, nrot));
   }
   int iulh=iul*5+np-1;
   if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFHoneyUL%d",iul)<<" Z="<<(PHoneyUL2[iul*5][2]+DHoneyUL2[iul*5][2]/2.)/cm<<" "<<(PHoneyUL2[iulh][2]-DHoneyUL2[iulh][2]/2.)/cm<<G4endl;

//---Carbon Fibre1
   for(int i=0;i<3;i++){if(i<2)tofspar[i]=DHoneyUL2[iul*5][i]/2.;else tofspar[i]=DCarbonUL/2.; tofgvolpar[i]=tofspar[i]/cm; tofppar[i]=PCarbonUL[iul][i];}
   G4VSolid          *Carbon_sol=new G4Box(Form("TFCFibreUL%d_sol",iul),tofspar[0],tofspar[1],tofspar[2]); 
   G4LogicalVolume   *Carbon_log=new G4LogicalVolume(Carbon_sol,TOFCAFibreUL_M,Form("TFCFibreUL%d_log",iul));//0.168g/cm^2
   G4VPhysicalVolume *Carbon_phy=new G4PVPlacement(0,G4ThreeVector(tofppar[0],tofppar[1],tofppar[2]),Carbon_log,Form("TFCFibreUL%d",iul),mvol,false,0);
   tofgvol->add(new TOFgvolume(TOFCAFibreUL_M->GetName(),"BOX",3,tofgvolpar,Carbon_log));
   mother->add(GetAMSgvolume(Carbon_phy, nrot));
   if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFCFibreUL%d",iul)<<" Z="<<(PCarbonUL[iul][2]+DCarbonUL/2.)/cm<<" "<<(PCarbonUL[iul][2]-DCarbonUL/2.)/cm<<G4endl;
   
//---Scintillator1

//---Carbon Fibre2+Foam
   for(int i=0;i<3;i++){if(i<2)tofspar[i]=DHoneyUL2[iul*5][i]/2.;else tofspar[i]=DCarbon2UL2/2.; tofgvolpar[i]=tofspar[i]/cm; tofppar[i]=PCarbon2UL2[iul][i];} 
   G4VSolid          *Carbon2_sol=new G4Box(Form("TFCFibre2UL%d_sol",iul),tofspar[0],tofspar[1],tofspar[2]);
   G4LogicalVolume   *Carbon2_log=new G4LogicalVolume(Carbon2_sol,TOFCAFibre2UL_M,Form("TFCFibre2UL%d_log",iul));//0.3276/cm^2
   G4VPhysicalVolume *Carbon2_phy=new G4PVPlacement(0,G4ThreeVector(tofppar[0],tofppar[1],tofppar[2]),Carbon2_log,Form("TFCFibre2UL%d",iul),mvol,false,0);
   tofgvol->add(new TOFgvolume(TOFCAFibre2UL_M->GetName(),"BOX",3,tofgvolpar,Carbon2_log));
   mother->add(GetAMSgvolume(Carbon2_phy, nrot));
   if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFCFibre2UL%d",iul)<<" Z="<<(PCarbon2UL2[iul][2]+DCarbon2UL2/2.)/cm<<" "<<(PCarbon2UL2[iul][2]-DCarbon2UL2/2.)/cm<<G4endl;

//---Scintillator2

//---Carbon Fibre3
   for(int i=0;i<3;i++){tofppar[i]=PCarbon3UL2[iul][i];} 
   G4VPhysicalVolume *Carbon3_phy=new G4PVPlacement(0,G4ThreeVector(tofppar[0],tofppar[1],tofppar[2]),Carbon_log,Form("TFCFibre3UL%d",iul),mvol,false,1);//0.168g/cm^2
   mother->add(GetAMSgvolume(Carbon3_phy, nrot));
   if(VerboseLevel>1)G4cout<<"---------->>"<<Form("TFCFibre3UL%d",iul)<<" Z="<<(PCarbon3UL2[iul][2]+DCarbonUL/2.)/cm<<" "<<(PCarbon3UL2[iul][2]-DCarbonUL/2.)/cm<<G4endl;

  }
 } 

   AMSJob::map(1);
   G4cout<<"<<----------End of TOF Geant4 Geometry Init"<<G4endl;   

//Add TOF geometry to map LVL302 need TOF geometry information
   G4cout<<"------>>LVL302 reinit"<<G4endl;
   TriggerLVL302::init();
//--
   return true;
}


AMSgvolume *TofSimUtil::GetAMSgvolume(G4VPhysicalVolume *phyv, int &nrot){
  
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  G4RotationMatrix *rotv=phyv->GetRotation ();
  if(rotv!=0){
    G4RotationMatrix rrotv=*rotv;
    rrotv.invert(); 
    for(int ii = 0; ii < 9; ii++)nrm[ii/3][ii%3]=rrotv (ii/3,ii%3);
  }

//--
  G4ThreeVector trans=phyv->GetTranslation ();
  geant coo[3];
  coo[0]=trans.x()/cm;coo[1]=trans.y()/cm;coo[2]=trans.z()/cm;

  G4LogicalVolume *logv=phyv->GetLogicalVolume();
  G4Material       *mat=logv->GetMaterial();
  TOFgvolume *tvol=tofgvol->findgvolume(logv);
//---just to keep the same define as in LTRANS Trap part//using for coo convert
  if(strcmp(tvol->getshape(),"TRDSPX")==0){
    geant dx=tvol->getpar()[3];
    for(int i=0;i<3;i++)coo[i]+=nrm[i][0]*dx;//local->global
  }
//----

  AMSgvolume *gvol=new  AMSgvolume(mat->GetName(),nrot++,phyv->GetName(),tvol->getshape(),tvol->getpar(),tvol->getnpar(),coo,nrm,"ONLY",0,phyv->GetCopyNo(),1);
//  AMSgvolume *gvol=new  AMSgvolume(mat->GetName(),nrot,phyv->GetName(),"TOFGEOM",par,npar,coo,nrm,"ONLY",0,nrot++,1);
  gvol->setpg4l(logv);
  gvol->setpg4v(phyv);  
  return gvol; 
}



G4AssemblyVolume* TofSimUtil::ConstructSubBar(G4String sci_name){

 //--sumbar  
  G4AssemblyVolume *Abar=new G4AssemblyVolume();

//Sci--
 G4ThreeVector scipos(0.0,0.0,0.0);
 G4RotationMatrix* sciRot=new G4RotationMatrix();
 G4double scipar[5];
 geant tofgvolpar[5]={0};
 if(sci_wc != 0.){
   scipar[0]=sci_w/2.; scipar[1]=sci_l/2.; scipar[2]=sci_t/2.;
   scipar[3]=sci_wc/2.;scipar[4]=sci_lc;
   for(int i=0;i<5;i++)tofgvolpar[i]=scipar[i]/cm;
//   sci_log=tofgvol->GetSameG4Vol("TOFSC_M","TRDSPX",5,tofgvolpar);
//   if(!sci_log){
     sci_sol=SpeTrd(sci_name,scipar,sci_wc>0?1:-1);
     sci_log=new G4LogicalVolume(sci_sol,TOFSc_M,"tsc_log");
     tofgvol->add(new TOFgvolume("TOFSC_M","TRDSPX",5,tofgvolpar,sci_log));
//    }
    sci_wc=fabs(sci_wc);
 }
 else {
     scipar[0]=sci_w/2.;scipar[1]=sci_l/2.;scipar[2]=sci_t/2.;
     for(int i=0;i<3;i++)tofgvolpar[i]=scipar[i]/cm;
//     sci_log=tofgvol->GetSameG4Vol("TOFSC_M","BOX ",3,scipar);
//     if(!sci_log){
       sci_sol=new G4Box(sci_name,scipar[0],scipar[1],scipar[2]);
       sci_log=new G4LogicalVolume(sci_sol,TOFSc_M,"bsc_log");
       tofgvol->add(new TOFgvolume("TOFSC_M","BOX ",3,tofgvolpar,sci_log));
//     }
  }
 Abar->AddPlacedVolume(sci_log,scipos,sciRot);


//--LG part
  G4AssemblyVolume *Alg=new G4AssemblyVolume();
  sci_w=sci_w-sci_wc;
  G4double lg_t=sci_t;//thick
  G4double lg_w=sci_w-2*lg_wc;
  G4double lgpy;

  G4double lgppar[5];
 if(lg_l>0){
    if(lg_lc==0){//box case
     lgppar[0]=lg_t/2.;lgppar[1]=lg_w/2.;lgppar[2]=lg_l/2.;
     lgpy=lg_l/2.;
     for(int i=0;i<3;i++)tofgvolpar[i]=lgppar[i]/cm;
     lgp_log=tofgvol->GetSameG4Vol("TOFLG_M","BOX ",3,tofgvolpar);
     if(!lgp_log){
       lgp_sol= new G4Box("lgp_sol",lgppar[0],lgppar[1],lgppar[2]);
       lgp_log= new G4LogicalVolume(lgp_sol,TOFLg_M,"lgp_log");
       tofgvol->add(new TOFgvolume("TOFLG_M","BOX ",3,tofgvolpar,lgp_log));
       }
     }
   else if(lg_lc==lg_l){//trd case
     lgppar[0]=lg_t/2.;lgppar[1]=lg_t/2.;lgppar[2]=sci_w/2.;lgppar[3]=lg_w/2.;lgppar[4]=lg_l/2.;
     lgpy=lg_l/2.;
     for(int i=0;i<5;i++)tofgvolpar[i]=lgppar[i]/cm;
     lgp_log=tofgvol->GetSameG4Vol("TOFLG_M","TRD2",5,tofgvolpar);
     if(!lgp_log){
       lgp_sol=new G4Trd("lgp_sol",lgppar[0],lgppar[1],lgppar[2],lgppar[3],lgppar[4]);
       lgp_log= new G4LogicalVolume(lgp_sol,TOFLg_M,"lgp_log");
       tofgvol->add(new TOFgvolume("TOFLG_M","TRD2",5,tofgvolpar,lgp_log));
     } 
   }
  else { //box+trd case
     lgppar[0]=lg_t/2.;
     lgppar[1]=sci_w/2.; lgppar[2]=lg_l/2.;
     lgppar[3]=lg_wc;    lgppar[4]=lg_lc/2.;
     lgpy=(lg_l-lg_lc)/2.;
     for(int i=0;i<5;i++)tofgvolpar[i]=lgppar[i]/cm;
     lgp_log=tofgvol->GetSameG4Vol("TOFLG_M","TRDSP",5,tofgvolpar);
     if(!lgp_log){
      lgp_sol=SpeTrd("lgp_sol",lgppar);
      lgp_log= new G4LogicalVolume(lgp_sol,TOFLg_M,"lgp_log");
      tofgvol->add(new TOFgvolume("TOFLG_M","TRDSP",5,tofgvolpar,lgp_log));
     }
   }

  G4ThreeVector lgppos(0.0,lgpy,0.0);
  G4RotationMatrix* lgpRot=new G4RotationMatrix();
  lgpRot->rotateZ( M_PI/2.*rad);
  lgpRot->rotateX(-M_PI/2.*rad);
  Alg->AddPlacedVolume(lgp_log,lgppos,lgpRot);
 }
//---LG bend part  //rotate anti clock
 G4bool negbend=0;//if positive bend
 if(lgb_a<0){lgb_a=-lgb_a;negbend=1;}
 if(lgb_a!=0.&&lgb_r>0){
   G4double lgbpar[5];
   lgbpar[0]=lgb_r;lgbpar[1]=lgb_r+lg_t;lgbpar[2]=lg_w/2.;lgbpar[3]=0;lgbpar[4]=lgb_a;
   for(int i=0;i<5;i++){if(i<3)tofgvolpar[i]=lgbpar[i]/cm; else tofgvolpar[i]=lgbpar[i]/deg; }
   tofgvolpar[4]=tofgvolpar[3]+tofgvolpar[4];//gvolume defination
   lgb_log=tofgvol->GetSameG4Vol("TOFLG_M","TUBS",5,tofgvolpar);
   if(!lgb_log){
    lgb_sol=new G4Tubs("lgb_sol",lgbpar[0],lgbpar[1],lgbpar[2],lgbpar[3],lgbpar[4]);//angle
    lgb_log=new G4LogicalVolume(lgb_sol,TOFLg_M,"lgb_log");
    tofgvol->add(new TOFgvolume("TOFLG_M","TUBS",5,tofgvolpar,lgb_log));
   }
   G4RotationMatrix *lbRot=new G4RotationMatrix;
   lbRot->rotateY(M_PI/2.*rad);
   G4ThreeVector lbpos(0,lg_l,lgb_r+lg_t/2.);
   Alg->AddPlacedVolume(lgb_log, lbpos, lbRot);
  }

//--LG two or three diff part
   G4double lgt_w=lg_w/npmt;

//--pmt and LG conial part
   G4AssemblyVolume *apmlgc=new G4AssemblyVolume();
  
   G4double pmpar[3]; 
   pmpar[0]=pm_s/2.;pmpar[1]=pm_s/2.;pmpar[2]=pm_s/2.;
   for(int i=0;i<3;i++)tofgvolpar[i]=pmpar[i]/cm;
   pm_log=tofgvol->GetSameG4Vol("TOFPM_M","BOX ",3,tofgvolpar);
   if(!pm_log){
     pm_sol=new G4Box("pm_sol",pmpar[0],pmpar[1],pmpar[2]);
     pm_log=new G4LogicalVolume(pm_sol,TOFPM_M,"pmt_log");
     tofgvol->add(new TOFgvolume("TOFPM_M","BOX ",3,tofgvolpar,pm_log)); 
   }
   
   G4double lgcpar[5];
   lgcpar[0]=lgt_w/2.;lgcpar[1]=pm_s/2.;lgcpar[2]=lg_t/2.;lgcpar[3]=pm_s/2.;lgcpar[4]=lgc_l/2.;
   for(int i=0;i<5;i++)tofgvolpar[i]=lgcpar[i]/cm;
   lgc_log=tofgvol->GetSameG4Vol("TOFLG_M","TRD2",5,tofgvolpar);
   if(!lgc_log){
      lgc_sol=new G4Trd("lgc_sol",lgcpar[0],lgcpar[1],lgcpar[2],lgcpar[3],lgcpar[4]);
      lgc_log=new G4LogicalVolume(lgc_sol,TOFLg_M,"lgc_log");
      tofgvol->add(new TOFgvolume("TOFLG_M","TRD2",5,tofgvolpar,lgc_log));
    }
   G4ThreeVector lgcpos(0.,0.,0.);
   apmlgc->AddPlacedVolume(lgc_log,lgcpos, 0);
   G4ThreeVector pmpos(0.,0.,(pm_s+lgc_l)/2.);
   apmlgc->AddPlacedVolume(pm_log, pmpos, 0);

  //---pmt+LG conial+ LG twist or box extend
   G4AssemblyVolume *algt=new G4AssemblyVolume();
   if(lgt_l>0){
     G4double lgtpar[4];
     if(twista!=0){//twist case //can not 90degree
        lgtpar[0]=twista;lgtpar[1]=lgt_w/2.;lgtpar[2]=lg_t/2.;lgtpar[3]=lgt_l/2.;
        for(int i=0;i<4;i++){if(i==0)tofgvolpar[i]=lgtpar[i]/deg;else tofgvolpar[i]=lgtpar[i]/cm;}
        if(twista>0){
          lgt_log=tofgvol->GetSameG4Vol("TOFLG_M","TWISTSP1",4,tofgvolpar);
          if(!lgt_log){
             lgt_sol=new G4TwistedBox("lgt_sol",lgtpar[0],lgtpar[1],lgtpar[2],lgtpar[3]);//npmt
             lgt_log=new G4LogicalVolume(lgt_sol,TOFLg_M,"lgt_log");
             tofgvol->add(new TOFgvolume("TOFLG_M","TWISTSP1",4,tofgvolpar,lgt_log));
           }
         }
         else {//twist angle<0 using Reflection
           lgt_log=tofgvol->GetSameG4Vol("TOFLG_M","TWISTSP2",4,tofgvolpar);
           if(!lgt_log){
             G4Transform3D tr_lgtsol;
             G4ReflectY3D  re_lgtsol;
             tr_lgtsol=tr_lgtsol*re_lgtsol;
             lgt_sol=new G4TwistedBox("lgt_sol",-lgtpar[0],lgtpar[1],lgtpar[2],lgtpar[3]);
             G4VSolid *lgt_solr=new G4ReflectedSolid ("lgt_solr",lgt_sol,tr_lgtsol);
             lgt_sol=lgt_solr;
             lgt_log=new G4LogicalVolume(lgt_sol,TOFLg_M,"lgt_log");
             tofgvol->add(new TOFgvolume("TOFLG_M","TWISTSP2",4,tofgvolpar,lgt_log));
           }
        }
     }
     else { //plane case
        lgtpar[0]=lgt_w/2.;lgtpar[1]=lg_t/2.;lgtpar[2]=lgt_l/2.;
        for(int i=0;i<3;i++)tofgvolpar[i]=lgtpar[i]/cm;
        lgt_log=tofgvol->GetSameG4Vol("TOFLG_M","BOX ",3,tofgvolpar);
        if(!lgt_log){
          lgt_sol=new G4Box("lgt_sol",lgtpar[0],lgtpar[1],lgtpar[2]);
          lgt_log=new G4LogicalVolume(lgt_sol,TOFLg_M,"lgt_log");
          tofgvol->add(new TOFgvolume("TOFLG_M","BOX ",3,tofgvolpar,lgt_log));
        }
     } 
     G4RotationMatrix *Rotlgt=new G4RotationMatrix;
     Rotlgt->rotateZ(twista/2.);
     G4ThreeVector lgtpos(0.0,0.0,0.0);
     algt->AddPlacedVolume(lgt_log, lgtpos, Rotlgt);
    }
    G4RotationMatrix *Rotlgc=new G4RotationMatrix;
    if(lgt_l>0)Rotlgc->rotateZ(twista); 
    G4ThreeVector apmlgcpos(0.0,0.0,(lgt_l+lgc_l)/2.);  
    algt->AddPlacedAssembly(apmlgc, apmlgcpos, Rotlgc);
  //--assemble twist to Lg
  G4RotationMatrix ltRot;
  G4Transform3D lttr;
  if(lgb_a!=0&&lgb_r>0){//bend case
    ltRot.rotateX(-(90.0*deg-lgb_a));
    G4double lgtby=(lgb_r+lg_t/2.)*sin(lgb_a);
    G4double lgtbz=(lgb_r+lg_t/2.)*(1-cos(lgb_a));
    G4double lgtcy=lgt_l/2.*cos(lgb_a);
    G4double lgtcz=lgt_l/2.*sin(lgb_a);
    for(int ipm=0;ipm<npmt;ipm++){
      G4ThreeVector ltv(-lg_w/2.+lgt_w*(0.5+ipm), lg_l+lgtby+lgtcy, lgtbz+lgtcz);//dis=lg_l+bend+twist
      lttr=G4Transform3D(ltRot,ltv);
      Alg->AddPlacedAssembly(algt, lttr);
     }
  }
 else {//plane case
     ltRot.rotateX(-90.0*deg);
     for(int ipm=0;ipm<npmt;ipm++){
      G4ThreeVector ltv(-lg_w/2.+lgt_w*(0.5+ipm), lg_l+lgt_l/2., 0.);//dis=lg_l
      lttr=G4Transform3D(ltRot,ltv);
      Alg->AddPlacedAssembly(algt, lttr);
     }
  }

//---Set Log Vis  Attribute
    Attributes();

//--Lg finish fix to bar //2 end
//--pside LG
   G4ThreeVector algpos(0.0,sci_l/2.,0.0);
   G4RotationMatrix *lgRot=new G4RotationMatrix;
   if(negbend)lgRot->rotateY(M_PI*rad);
   Abar->AddPlacedAssembly(Alg,algpos,lgRot); //Alg
//--nside LG
   G4Translate3D algposr(0.0,-sci_l/2.,0.0);//Reflection
   G4Transform3D algRotr=G4Rotate3D(*lgRot);
   G4ReflectY3D  algref;
   G4Transform3D algtrr=algposr*algRotr*algref;
   Abar->AddPlacedAssembly(Alg,algtrr);//Alg 

   return Abar;
}


AMSgmat* TofSimUtil::GetAMSgmat(G4Material *mat){
  if(mat==0)cout<<"Exit Could not find  Material of TOF"<<endl;
  const int n=mat->GetElementVector()->size();
  geant a[n],z[n],w[n];
  for(int i=0;i!=n;i++){
    a[i]=mat->GetElementVector()->at(i)->GetA()/g*mole;
    z[i]=mat->GetElementVector()->at(i)->GetZ();
    w[i]=(n==1)?1:mat->GetAtomsVector()[i]; 
  }
  geant density=mat->GetDensity()/g*cm3;
  geant radl=   mat->GetRadlen()/cm;
  geant absl=   mat->GetNuclearInterLength()/cm;
  G4String name=mat->GetName();
 
//---
  AMSgmat* amsmat=new AMSgmat(name, a, z, w, n, density,radl,absl,273.,mat);
  return amsmat;  
}


bool TofSimUtil::AddTOFgmat(AMSgmat &gmat){
  // G4cout<<"gmat="<<G4endl;
   for(int ilay=0;ilay<NLAY;ilay++){
     for(int ibar=0;ibar<Nbar[ilay];ibar++){
        AMSgmat* scgmat=GetAMSgmat(TOFSc_MA[ilay][ibar]);
        gmat.add(scgmat);
      }
   }

   AMSgmat* lggmat=GetAMSgmat(TOFLg_M);
   AMSgmat* pmgmat=GetAMSgmat(TOFPM_M);
   gmat.add(lggmat);
   gmat.add(pmgmat);

   //----
   gmat.add(GetAMSgmat(TOFAl_MU));
   gmat.add(GetAMSgmat(TOFAl_MM));
   gmat.add(GetAMSgmat(TOFAl_ML));
   gmat.add(GetAMSgmat(TOFAl_MExt));
   gmat.add(GetAMSgmat(TOFAlHoneyUL_M2[0]));
   gmat.add(GetAMSgmat(TOFAlHoneyUL_M2[1]));
   gmat.add(GetAMSgmat(TOFCAFibreUL_M));
   gmat.add(GetAMSgmat(TOFCAFibre2UL_M));
//   G4cout<<"gmatend"<<G4endl;
   return true;
}


bool TofSimUtil::AddTOFgtmed(AMSgtmed &gtmat){
   AMSgtmed *scgtmed[NLAY][NBAR];
   for(int ilay=0;ilay<NLAY;ilay++){
     for(int ibar=0;ibar<Nbar[ilay];ibar++){
         scgtmed[ilay][ibar]=new AMSgtmed(TOFSc_MA[ilay][ibar]->GetName(),TOFSc_MA[ilay][ibar]->GetName(),1);//sensitive volume
         gtmat.add(scgtmed[ilay][ibar]);
     }
   }
  AMSgtmed *lggtmed=new AMSgtmed(TOFLg_M->GetName(),TOFLg_M->GetName(),0);
  AMSgtmed *pmgtmed=new AMSgtmed(TOFPM_M->GetName(),TOFPM_M->GetName(),1);  
  gtmat.add(lggtmed);
  gtmat.add(pmgtmed);
//----
  if(G4FFKEY.TFNewGeant4%10==1){
    for(int ilay=0;ilay<NLAY;ilay++){
       for(int ibar=0;ibar<Nbar[ilay];ibar++){
          scgtmed[ilay][ibar]->getpgmat()->getpamsg4m()->SetMaterialPropertiesTable(TOFSc_MtA[ilay][ibar]);//adding table
     }
   }
   lggtmed->getpgmat()->getpamsg4m()->SetMaterialPropertiesTable(TOFLg_Mt);
  }

//----
  gtmat.add(new AMSgtmed(TOFAl_MU->GetName(),TOFAl_MU->GetName(),0));
  gtmat.add(new AMSgtmed(TOFAl_MM->GetName(),TOFAl_MM->GetName(),0));
  gtmat.add(new AMSgtmed(TOFAl_ML->GetName(),TOFAl_ML->GetName(),0));
  gtmat.add(new AMSgtmed(TOFAl_MExt->GetName(),TOFAl_MExt->GetName(),0));
  gtmat.add(new AMSgtmed(TOFAlHoneyUL_M2[0]->GetName(),TOFAlHoneyUL_M2[0]->GetName(),0));
  gtmat.add(new AMSgtmed(TOFAlHoneyUL_M2[1]->GetName(),TOFAlHoneyUL_M2[1]->GetName(),0));
  gtmat.add(new AMSgtmed(TOFCAFibreUL_M->GetName(), TOFCAFibreUL_M->GetName(), 0));
  gtmat.add(new AMSgtmed(TOFCAFibre2UL_M->GetName(),TOFCAFibre2UL_M->GetName(),0));

  return true;
}

void TofSimUtil::Attributes(){
   /* G4VisAttributes* sci_va  = new G4VisAttributes(G4Colour(0.,0.,1.));//blue
    sci_log->SetVisAttributes(sci_va);

    G4VisAttributes* lg_va  = new G4VisAttributes(G4Colour(1.,0.,1.));//magenta
    if(lgp_log!=0)lgp_log->SetVisAttributes(lg_va);
    if(lgb_log!=0)lgb_log->SetVisAttributes(lg_va);
    if(lgt_log!=0)lgt_log->SetVisAttributes(lg_va);
    if(lgc_log!=0)lgc_log->SetVisAttributes(lg_va);

     G4VisAttributes* pm_va = new G4VisAttributes(G4Colour(0.,1.,1.));//green
//    pm_va->SetForceSolid(true);
    pm_log->SetVisAttributes(pm_va);
*/
//--limit
/*   if(sci_log!=0)sci_log->SetUserLimits(TOF_Limit);
   if(lgp_log!=0)lgp_log->SetUserLimits(TOF_Limit);
   if(lgb_log!=0)lgb_log->SetUserLimits(TOF_Limit);
   if(lgt_log!=0)lgt_log->SetUserLimits(TOF_Limit);
   if(lgc_log!=0)lgc_log->SetUserLimits(TOF_Limit);
*/
//--

}

void TofSimUtil::ConstructBarSurface(G4AssemblyVolume *AsBar,G4VPhysicalVolume* mvop){

//--Bar cover
  std::vector<G4VPhysicalVolume*>::iterator Barphv=AsBar->GetVolumesIterator();
  for(unsigned int ivo=0;ivo<AsBar->TotalImprintedVolumes();ivo++){
    G4String volsnam=(*(Barphv+ivo))->GetName();
    volsnam=volsnam+"Wrap";
//    G4cout<<volsnam<<G4endl;
    if((*(Barphv+ivo))->GetLogicalVolume()==sci_log)new G4LogicalBorderSurface(volsnam,*(Barphv+ivo),mvop,TOFSC_Su);
    else                                            new G4LogicalBorderSurface(volsnam,*(Barphv+ivo),mvop,TOFLG_Su);
   }

//--PMT Absorb
  G4String pmsnam=pm_log->GetName();
  pmsnam=pmsnam+"PMT_cath";
  new G4LogicalSkinSurface(pmsnam,pm_log,TOFPM_Su);
  new G4LogicalSkinSurface(pmsnam,pm_log1,TOFPM_Su);
//--

}



G4VSolid* TofSimUtil::SpeTrd(G4String trapname,G4double trdpa[5],G4int idir){//5 par
//par x y z xc yc
 if(idir==1||idir==-1){//xdir trap
    G4double boxc=fabs(trdpa[3]);
    G4double boxx=trdpa[0]-boxc;//+x cut
    G4Box* trap_box= new G4Box(trapname+"_box",boxx,trdpa[1],trdpa[2]);
    G4Trd* trap_trd= new G4Trd(trapname+"_trd",trdpa[2],trdpa[2],trdpa[1],trdpa[1]-trdpa[4],boxc);
    G4RotationMatrix* yrot=new G4RotationMatrix;
   if(idir==1){//+xdir
     yrot->rotateY(-M_PI/2.*rad);
     G4VSolid *sep_trd=new G4UnionSolid(trapname,trap_box,trap_trd,yrot,G4ThreeVector(boxx+boxc,0.,0.));
      return sep_trd;
     }
    else {//-xdir
     yrot->rotateY(M_PI/2.*rad);
     G4VSolid *sep_trd=new G4UnionSolid(trapname,trap_box,trap_trd,yrot,G4ThreeVector(-1.*(boxx+boxc),0.,0.));
     return sep_trd;
     }
   }
//-----
// /\ //par x y z yc zc
  else {//+zdir
    G4double boxz=trdpa[2]-trdpa[4];
    G4Box* trap_box= new G4Box(trapname+"_box",trdpa[0],trdpa[1],boxz);
    G4Trd* trap_trd= new G4Trd(trapname+"_trd",trdpa[0],trdpa[0],trdpa[1],trdpa[1]-trdpa[3],trdpa[4]);
    G4VSolid *sep_trd=new G4UnionSolid(trapname,trap_box,trap_trd,0,G4ThreeVector(0.,0,boxz+trdpa[4]));
    return sep_trd;
  }

}

void  TofSimUtil::InitialBarPar(int ilay,int ibar){
  sci_log=0;lgp_log=0;lgb_log=0;lgt_log=0;
  lgc_log=0;lgc_log1=0;pm_log=0;pm_log1=0;
  sci_l=Sci_l[ilay][ibar];
  sci_w=Sci_w[ilay][ibar];
 sci_lc=Sci_lc[ilay][ibar];
 sci_wc=Sci_wc[ilay][ibar];
  sci_t=Sci_t[ilay][ibar];
//---plan
  lg_l=Lg_l[ilay][ibar];
  lg_wc=Lg_wc[ilay][ibar];
  lg_lc=Lg_lc[ilay][ibar];
//---bend
  lgb_a=Lgb_a[ilay][ibar];
  lgb_r=Lgb_r[ilay][ibar];
//--twist
  npmt=Npmt[ilay][ibar];
  lgt_l=Lgt_l[ilay][ibar];
  twista=Twista[ilay][ibar];
//--conial
  lgc_l=Lgc_l[ilay][ibar];
  pm_s=Pm_s[ilay][ibar];

 //--pos local 
//--lay
     G4double sumw=Normw*(Nbar[ilay]-2)+(Sci_w[ilay][0]-fabs(Sci_wc[ilay][0]))-Overlapw*(Nbar[ilay]-1);
     G4double trpos=-sumw/2.;//trans
     G4double lopos=0.;
     G4double zpos;//z coo
     if(ilay==0||ilay==1)zpos=Zc[ilay]-Nthick/2.;
     else zpos=Zc[ilay]+Nthick/2.;
//--ibar
    if(ibar!=0){
      for(int bar=1;bar<=ibar;bar++){
         trpos=trpos+(Sci_w[ilay][bar-1]-fabs(Sci_wc[ilay][bar-1])+Sci_w[ilay][bar]-fabs(Sci_wc[ilay][bar]))/2.-Overlapw;
       }
    }
    if((ilay==0||ilay==1)&&(ibar%2==0))zpos=zpos-Zshift;
    else if    ((ilay==2)&&(ibar%2==1))zpos=zpos+Zshift;
    else if    ((ilay==3)&&(ibar%2==0))zpos=zpos+Zshift;

//--pos grobel and rotation
   barp[2]= zpos;
   if(ilay==0||ilay==3){
     barrot[0]=M_PI/2.*rad;//Z
     barrot[1]=M_PI*rad;//Y reflection should be better because z is different
     barp[1]=trpos; barp[0]=lopos;
   }
   else  {
    barrot[0]=0.;//rotateZ
    barrot[1]=0.;//rotateX
    barp[0]=trpos; barp[1]=lopos;
   }
//---
   if((G4FFKEY.TFNewGeant4/10)%10==0){
      barp[0]+=Honshift[ilay/2][0];
      barp[1]+=Honshift[ilay/2][1];
   }
   else {
      barp[0]+=HonshiftUL[ilay/2][0];
      barp[1]+=HonshiftUL[ilay/2][1];
   }
//  if(ibar<Nbar[ilay]/2)barrot=barrot+M_PI*rad;//-x -y axis 

}

TOFgvolume::TOFgvolume(const char matter[],const char shape[],int npar, geant par[],G4LogicalVolume *pg4l):AMSNode(0),_npar(npar){
    _shape =new char[strlen(shape)>3?strlen(shape)+1:5];
    _matter=new char[strlen(matter)>3?strlen(matter)+1:5];
    strcpy(_shape,shape);
    strcpy(_matter,matter);
    if(_npar){
       _par=new geant[_npar];
       for(int i=0;i<_npar;i++)*(_par+i)=par[i];  
    }
   _pg4l=pg4l; 
}

G4LogicalVolume *TOFgvolume::GetSameG4Vol(const char matter[],const char shape[],int npar, geant par[]){

//  return 0; 
  TOFgvolume *cur = this;
  while(1){//go to head 1d
     if(cur->up()==0)break;
     cur=cur->up();
  }

  //G4cout<<"mother="<<cur->getmatter()<<G4endl; 
  bool samevol=1;
  while (cur){
    samevol=1;
    if(npar!=cur->getnpar()){samevol=0;}
    else {
      for(int i=0;i<npar;i++){
        if(par[i]!=cur->getpar()[i]){samevol=0;break;}
       }
    }
    if(samevol==0||strcmp(matter,cur->getmatter())||strcmp(shape, cur->getshape())){
      samevol=0;
      //G4cout<<"samevol="<<cur->getmatter()<<" shap="<<cur->getshape()<<G4endl;
      cur=cur->next()?cur->next():cur->down();
      continue;
    }
    if(samevol){
       //G4cout<<"samevol="<<cur->getmatter()<<" shap="<<cur->getshape()<<G4endl;
       return cur->getvol(); 
     }
  }
  return 0;
} 


TOFgvolume *TOFgvolume::findgvolume(G4LogicalVolume *g4l){

  TOFgvolume *cur = this;
  while(1){//go to head 1d
     if(cur->up()==0)break;
     cur=cur->up();
  }
  
  while (cur){
    if(cur->getvol()==g4l)return cur;
    cur=cur->next()?cur->next():cur->down();
  }
  return 0;
}
