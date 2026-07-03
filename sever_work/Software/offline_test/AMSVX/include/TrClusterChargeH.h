#ifndef __TrClusterChargeH_H__
#define __TrClusterChargeH_H__

#include "TrChargeDBH.h"
#include "TObject.h"

class TrClusterR;

/*!
A light weighted class for tracker cluster charge (no beta and rigidity correction)
*/

class TrClusterChargeLightH{
  private:
   /// the inner track charge or the charge for unbiased charge calculation
   Float_t Ztrk;
  public:
 
   //! Static member to force updating eta for charge calibration... 
   static bool CheckEta;
   static bool SmoothEta;
   static float fSmoothEta;
   static float fSmoothEtaUnb;

   enum ClusterStripCombineOptions {
      kStripWeight    = 1,
      kStripCovMatrix = 2
   };
   enum ClusterSigOptions {
      kOverflow = 1<<2,
      kAsym     = 2<<2,
      kSimAsym  = 4<<2,
      kAsymEta  = 8<<2,
      kSimSignal= 16<<2
   };
   enum ClusterEtaOptions {
      kSimpCofG   = 1<<7,
      kLinearCofG = 2<<7
   };
   enum ClusterBasicOptions {
      kSmear  = 1<<9,
      kUnbias = 2<<9
   };

   /// the random floating number for MC charge smearing
   Float_t mcsmear[2];

   /// Pointer to the cluster
   TrClusterR* cluster; //!

   /// Option for cluster charge calibration (combination of cluster signal option, position option and strip charge combination option)
   UShort_t ClusterChargeOpt;
   //#pragma omp threadprivate (ClusterChargeOpt)
   //The variables needed for charge calibration
   /// Position
   Float_t eta;
   /// angle in x and y side
   Float_t angle[2];
   /// Combination of multiplicity and seed address (xmult*addr)
   Short_t seedaddress;
   /// The signal amplitude of each strip (2 correspond to seed strip)
   Float_t sigadc[NStrip+1];
   /// the seed charge for strip charge combination
   Float_t Zcomb;

   /// the result: charge without energy correction
   Float_t SqrtdEdX;

   void Init();
   void Release();
   /// Default constructor
   TrClusterChargeLightH() { Init(); }
   /// Constructor with some parameters (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   TrClusterChargeLightH(TrClusterR* cluster0,int imult,int Opt=-1,float Z0=-1,float dxdz=-2,float dydz=-2);
   TrClusterChargeLightH(const TrClusterChargeLightH& orig) { copy(orig); }
   TrClusterChargeLightH& operator=(const TrClusterChargeLightH& that) { if (this!=&that) copy(that); return *this; }
   void copy(const TrClusterChargeLightH& orig);
   /// Default deconstructor
   virtual ~TrClusterChargeLightH() { Release(); }

   ///Reassign the pointer to the TrClusterR object
   void AssignPointer(TrClusterR* pcluster0);
   ///Set the inner track charge used for charge calibration
   void SetQtrk(float qtrk) { Ztrk=qtrk; }
   ///Get the inner track charge used for charge calibration
   float GetQtrk() { return Ztrk; }

   /// update the smear number
   void UpdateSmearing();

   /// adding the unbias bit according to dxdz or dydz
   int ConvertOpt(int Opt,float dxdz,float dydz);

   /// Check wheather the variables for charge calibration changed (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   int IsChanged(int imult,int Opt=-1,float Z0=-1,bool forceupdate=false,float dxdz=-2,float dydz=-2);
   /// ReCalculate the charge(no energy correction) (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   bool Calculate(int imult,int Opt=-1,float Z0=-1,float dxdz=-2,float dydz=-2);
   /// Get the true Z from closest MC cluster
   int GetTrueZ(int imult);
   /// Calculate the charge(no energy correction) by giving some options (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetSqrtdEdX(int imult,int Opt=-1,float Z0=-1,float dxdz=-2,float dydz=-2);
   float GetdEdX(int imult,int Opt=-1,bool IsMIP=true,float Z0=-1,float dxdz=-2,float dydz=-2);
   /// Calculate the total energy deposited by giving some options (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetEdep(int imult,int Opt=-1,bool IsMIP=true,float Z0=-1,float dxdz=-2,float dydz=-2);
   /// Calculate the charge(energy corrected) by giving some options (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetQ0(int imult,float beta,float rigidity,int Opt=-1,int EOpt=(TrChargeDBH::kBeta|TrChargeDBH::kRigidity),float Z0=-1,float dxdz=-2,float dydz=-2);

   //using TObject::Info;
   //ClassDef(TrClusterChargeLightH,1);
};

/**
A class for tracker cluster charge (with beta and rigidity correction)
*/

class TrClusterChargeH: public TrClusterChargeLightH {
   public:
   //static int DefaultOpt;
   //#pragma omp threadprivate (DefaultOpt)

   /// beta used for beta correction
   float beta;
   /// rigidity used for rigidity correction
   float rigidity;

   /// the result: charge with all correction
   float charge;

   void Init();
   void Release();
   /// Default constructor
   TrClusterChargeH() { Init(); }
   /// Constructor with some parameters (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   TrClusterChargeH(TrClusterR* cluster0,int imult,float beta0,float rigidity0,int Opt=-1,float Z0=-1,float dxdz=-2,float dydz=-2);
   TrClusterChargeH(const TrClusterChargeH& orig) { copy(orig); }
   TrClusterChargeH& operator=(const TrClusterChargeH& that) { if (this!=&that) copy(that); return *this; }
   void copy(const TrClusterChargeH& orig);
   /// Default deconstructor
   virtual ~TrClusterChargeH(){ Release(); }

   /// Check wheather the variables for charge calibration changed (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   int IsChanged(int imult,float beta0,float rigidity0,int Opt=-1,float Z0=-1,bool forceupdate=false,float dxdz=-2,float dydz=-2);
   /// ReCalculate the charge(with energy correction) (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   bool Calculate(int imult,float beta0,float rigidity0,int Opt=-1,float Z0=-1,float dxdz=-2,float dydz=-2);
   /// Calculate the charge(energy corrected) by giving some options (Opt=-1 use the default option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetQ(int imult,float beta0,float rigidity0,int Opt=-1,float Z0=-1,float dxdz=-2,float dydz=-2);

   //using TObject::Info;
   //ClassDef(TrClusterChargeH,1);
};

#endif
