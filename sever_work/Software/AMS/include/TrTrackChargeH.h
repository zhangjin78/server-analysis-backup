#ifndef __TrTrackChargeH_H__
#define __TrTrackChargeH_H__

#include "TrChargeDBH.h"
#include "TObject.h"
#include "TrRecHitChargeH.h"

class TrTrackR;

/*!
A light weighted class for tracker track charge (no beta and rigidity correction)
*/

class TrTrackChargeLightH{
   public:
   enum LayerCombineOptions {
      // plain mean
      kPlainMean = 0x1,
      // truncated mean  
      kTruncMean = 0x2,
      // gaussianized mean
      kGaussMean = 0x4,
      // drop one hit to get the best rms
      kDropOne   = 0x8,
      // drop the highest and the lowest charge hit when they are 3 sigma away
      kDropFaraway1  = 0x10,
      // drop the highest or the lowest charge hit when they are 3 sigma away
      kDropFaraway2  = 0x20
   };

   /// the random floating number for MC charge smearing, the first index: 0=All Layers, 1=Inner, the second index: 0=x,1=y,2=x&y
   Float_t mcsmear[2][3][NMCQpars];

   /// the pointer to the TrTrackR
   TrTrackR* ptrk; //!
   /// the hit charge as a key with layer index
   map<int,TrRecHitChargeLightH> qhit;

   void Init();
   void Release();
   /// Default constructor
   TrTrackChargeLightH() { Init(); }
   /// Constructor with some parameters (Opt=-1 use the default option, Z0,IsCharge,beta,and fit_id are parameters used to calculate seed charge)
   TrTrackChargeLightH(TrTrackR* ptrk0,int Opt=-1,float Z0=-1,bool IsCharge=true,float beta=0,int fit_id=0);
   TrTrackChargeLightH(const TrTrackChargeLightH& orig) { copy(orig); }
   TrTrackChargeLightH& operator=(const TrTrackChargeLightH& that) { if (this!=&that) copy(that); return *this; }
   void copy(const TrTrackChargeLightH& orig);
   /// Default deconstructor
   virtual ~TrTrackChargeLightH() { Release(); }

   ///reassign the pointer to the TrTrackR/TrRecHitR/TrClusterR object
   void AssignPointer(TrTrackR* ptrk0);
   ///Set the inner track charge used for charge calibration (iside=0:x; 1:y; 2:x&y)
   void SetQtrk(float qxtrk, float qytrk, int iside=2);

   /// update the smear number
   void UpdateSmearing();
   /// get the mc charge smearing parameters from TrRecHitChargeLightH
   float GetMCSmearing(int JLayer,int iside,int icomp=0);

   /// remove unbiase bit for charge calculation, and remove smear bit for MC charge calculation
   int ConvertOpt(int Opt);

   /// Check wheather the parameters for charge calibration changed (Opt=-1 use the default option, Z0,IsCharge,beta,and fit_id are parameters used to calculate seed charge)
   int IsChanged(int Opt=-1,float Z0=-1,bool IsCharge=true,float beta=0,int fit_id=0,bool forceupdate=false);
   /// Calculate the charge for each layer (Opt=-1 use the default option, Z0,IsCharge,beta,and fit_id are parameters used to calculate seed charge)
   bool Calculate(int JLayer,int Opt=-1,float Z0=-1,bool IsCharge=true,float beta=0,int fit_id=0);
   /// Get the charge(no energy correction) of single layer (iside=0: X, iside=1: Y, iside=2: X&Y combined)
   float GetSqrtdEdX(int JLayer,int iside=2,int Opt=-1);
   /// Get the dEdX of single layer (iside=0: X, iside=1: Y, iside=2: X&Y combined)
   float GetdEdX(int JLayer,int iside=2,int Opt=-1);
   /// Get the deposited energy of single layer (iside=0: X, iside=1: Y, iside=2: X&Y combined)
   float GetEdep(int JLayer,int iside=2,int Opt=-1);
   /// Get the charge(energy corrected) of single layer or inner or all layers (Opt=-1 use the default option, Z0,IsCharge are parameters used to calculate seed charge)
   float GetQ0(float beta,int fit_id,int JLayer,int iside=2,int Opt=-1,float Z0=-1,bool IsCharge=true);
   /*!
   \param JLayer =
   \li 1~9 single layer charge
   \li >9  inner layers
   \li <1  all layers
   \param iside =
   \li 0 X side
   \li 1 Y side
   \li 2 X&Y combined
   !*/

   //using TObject::Info;
   //ClassDef(TrTrackChargeLightH,1);
};

/*!
A class for tracker track charge (with beta and rigidity correction)
*/

class TrTrackChargeH: public TrTrackChargeLightH {
   public:

   static int DefaultOptISS;
   static int DefaultOptMC;
   #pragma omp threadprivate (DefaultOptISS)
   #pragma omp threadprivate (DefaultOptMC)
   static int NInnerQLoop;

   /// the beta used for beta correction
   float beta;
   /// the rigidity used for rigidity correction
   float rigidity;

   /// the charge of single layer with all correction (0:X, 1:Y, 2:X&Y combined)
   float Layercharge[3][9];
   /// the option for combining changes of single layer
   UChar_t LayerCombineOpt;
   //#pragma omp threadprivate (LayerCombineOpt)
   /// the charge object of inner layers (0:X, 1:Y, 2:X&Y combined)
   mean_t Innercharge[3];
   /// the charge object of all layers (0:X, 1:Y, 2:X&Y combined)
   mean_t Allcharge[3];

   /// Conversion of the charge option for 0:TrCluster,1:TrRecHit,2:TrTrack, Opt is the input Track Charge Option (Opt=-1 is the default option)
   static int ConvertChargeOpt(int WhichClass,int Opt=-1);
   /// Convert the combination option for 0:TrCluster Strip Combine Option,1:TrRecHit XY Combine Option,2:TrTrack Layer Combine Option
   static int ConvertCombineOpt(int WhichClass,int Opt=-1,bool IsTrChargeDBH=true);

   void Init();
   void Release();
   /// Default constructor
   TrTrackChargeH() { Init(); }
   /// Constructor with a track (Opt=-1 is the default charge option, Z0=-1 use the stored seed charge(which was derived by interation))
   TrTrackChargeH(TrTrackR* ptrk0,float beta0,int fit_id=0,int Opt=-1,float Z0=-1);
   TrTrackChargeH(const TrTrackChargeH& orig) { copy(orig); }
   TrTrackChargeH& operator=(const TrTrackChargeH& that) { if (this!=&that) copy(that); return *this; }
   void copy(const TrTrackChargeH& orig);
   /// Default deconstructor
   virtual ~TrTrackChargeH(){ Release(); }

   /// Check wheather the parameters for charge calibration changed (Opt=-1 is the default charge option, Z0=-1 use the stored seed charge(which was derived by interation))
   int IsChanged(float beta0,int fit_id=0,int Opt=-1,float Z0=-1,bool forceupdate=false);
   /// Calculate the charge of 9 single layers
   bool CalculateChargeLayer(int JLayer,float beta0,int fit_id=0);
   /// Calculate the inner layer and all layer combined charge
   bool CalculateChargeLayerCombined(int Opt=-1,float Z0=-1);
   /// Calculate both single layer charge and layer combined charge (Opt=-1 is the default charge option, Z0=-1 use the stored seed charge(which was derived by interation))
   bool Calculate(float beta0,int fit_id=0,int Opt=-1,float Z0=-1);
   /// Get the single layer charge or layer combined charge (Opt=-1 is the default charge option, Z0=-1 use the stored seed charge(which was derived by interation))
   float GetQ(int JLayer,int iside,float beta0,int fit_id=0,int Opt=-1,float Z0=-1);
   /// Get the layer combined charge (Opt=-1 is the default charge option, Z0=-1 use the stored seed charge(which was derived by interation))
   mean_t GetCombQ(int iside,float beta0,int fit_id=0,bool IsInner=true,int Opt=-1,float Z0=-1);
   /*!
   \param JLayer =
   \li 1~9 single layer charge
   \li >9  inner layers
   \li <1  all layers
   \param iside =
   \li 0 X side
   \li 1 Y side
   \li 2 X&Y combined
   !*/

   //using TObject::Info;
   //ClassDef(TrTrackChargeH,1);
};

#endif
