#ifndef __TrRecHitChargeH_H__
#define __TrRecHitChargeH_H__

#include "TrChargeDBH.h"
#include "TObject.h"
#include "TrClusterChargeH.h"
#include <map>

class TrRecHitR;

/*!
A light weighted class for tracker hit charge (no beta and rigidity correction)
*/

class TrRecHitChargeLightH{
   public:
   enum XYCombineOptions {
      kXYSigma  = 1,
      kXYWeight = 2,
      kXYCovMatrix =4
   };

   /// the random floating number for MC charge smearing
   Float_t mcsmear[NMCQpars];

   /// the pointer to the TrRecHitR
   TrRecHitR* phit; //!
   /// the XY Combination Option used
   UChar_t XYCombineOpt;
   //#pragma omp threadprivate (XYCombineOpt)
   /// the X&Y cluster charge as a key with side index (0:X,1:Y)
   map<int,TrClusterChargeLightH> qcluster;

   /// X&Y Combined Charge (without beta and rigidity correction)
   Float_t SqrtdEdX;

   void Init();
   void Release();
   /// Default constructor
   TrRecHitChargeLightH() { Init(); }
   /// Constructor with some parameters (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   TrRecHitChargeLightH(TrRecHitR* phit0,int Opt=-1,float Z0=-1,int imult=-2,float dxdz=-2,float dydz=-2);
   TrRecHitChargeLightH(const TrRecHitChargeLightH& orig) { copy(orig); }
   TrRecHitChargeLightH& operator=(const TrRecHitChargeLightH& that) { if (this!=&that) copy(that); return *this; }
   void copy(const TrRecHitChargeLightH& orig);
   /// Default deconstructor
   virtual ~TrRecHitChargeLightH() { Release(); }

   ///reassign the pointer to the TrRecHitR/TrClusterR object
   void AssignPointer(TrRecHitR* phit0);
   ///Set the inner track charge used for charge calibration (iside=0:x; 1:y; 2:x&y)
   void SetQtrk(float qxtrk,float qytrk,int iside=2);

   /// update the smear number
   void UpdateSmearing();
   /// get the mc smear parameter from the TrClusterChargeLightH, 0:x, 1:y, 2:x&y, icomp is the ith gaus component tunning
   float GetMCSmearing(int iside,int icomp=0);

   /// adding the unbias bit according to dxdz or dydz, and remove smear bit for MC charge calculation
   int ConvertOpt(int Opt,float dxdz,float dydz);

   /// Check wheather the parameters for charge calibration changed (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   int IsChanged(int Opt=-1,float Z0=-1,bool forceupdate=false,int imult=-2,float dxdz=-2,float dydz=-2);
   /// calculate the charge (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   bool Calculate(int Opt=-1,float Z0=-1,int imult=-2,float dxdz=-2,float dydz=-2);
   float GetCosTheta();
   /// get the charge (without beta and rigidity correction) (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetSqrtdEdX(int Opt=-1,float Z0=-1,int iside=2,int imult=-2,float dxdz=-2,float dydz=-2);
   /// get the deposited energy per pathlenth
   float GetdEdX(int Opt=-1,bool IsMIP=true,float Z0=-1,int iside=2,int imult=-2,float dxdz=-2,float dydz=-2);
   /// get the deposited energy (in MIP or KeV scale)
   float GetEdep(int Opt=-1,bool IsMIP=true,float Z0=-1,int iside=2,int imult=-2,float dxdz=-2,float dydz=-2);
   /// get the charge (with beta and rigidity correction) (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetQ0(float beta,float rigidity,int Opt=-1,float Z0=-1,int iside=2,int imult=-2,float dxdz=-2,float dydz=-2);

   //using TObject::Info;
   //ClassDef(TrRecHitChargeLightH,1);
};

/**
A class for tracker hit charge (with beta and rigidity correction)
*/

class TrRecHitChargeH: public TrRecHitChargeLightH {
   public:
   //static int DefaultOpt;
   //#pragma omp threadprivate (DefaultOpt)

   /// beta used for beta correction
   float beta;
   /// rigidity used for rigidity correction
   float rigidity;

   /// the charge of X and Y side with all correction
   float XYcharge[2];
   /// X&Y combined charge with all correction
   float charge;

   void Init();
   void Release();
   /// Default constructor
   TrRecHitChargeH() { Init(); }
   /// Constructor with some parameters (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   TrRecHitChargeH(TrRecHitR* phit0,float beta0,float rigidity0,int Opt=-1,float Z0=-1,int imult=-2,float dxdz=-2,float dydz=-2);
   TrRecHitChargeH(const TrRecHitChargeH& orig) { copy(orig); }
   TrRecHitChargeH& operator=(const TrRecHitChargeH& that) { if (this!=&that) copy(that); return *this; }
   void copy(const TrRecHitChargeH& orig);
   /// Default deconstructor
   virtual ~TrRecHitChargeH(){ Release(); }

   /// Check wheather the parameters for charge calibration changed (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   int IsChanged(float beta0,float rigidity0,int Opt=-1,float Z0=-1,bool forceupdate=false,int imult=-2,float dxdz=-2,float dydz=-2);
   /// calculate the charge (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   bool Calculate(float beta0,float rigidity0,int Opt=-1,float Z0=-1,int imult=-2,float dxdz=-2,float dydz=-2);
   /// get the charge (with beta and rigidity correction) (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetQ(float beta0,float rigidity0,int Opt=-1,float Z0=-1,int iside=2,int imult=-2,float dxdz=-2,float dydz=-2);

   //using TObject::Info;
   //ClassDef(TrRecHitChargeH,1);
};

#endif
