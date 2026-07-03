#ifndef __TrChargeDBH_h__
#define __TrChargeDBH_h__

#include "TrChargeLossDBH.h"
#include "TrGainCHDBH.h"
#include "TrGainDBH.h"
#include "TrThetaDBH.h"
#include "TrTempDBH.h"

#include "TrBetaDBH.h"
#include "TrRigidityDBH.h"

#include "TrQMean.h"
#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef NZQ
#define NZQ 28
#endif
#ifndef NStrip
#define NStrip 5
#endif
#ifndef NSQ
#define NSQ 5
#endif
#ifndef NCalib
#define NCalib 2
#endif
#ifndef NMCQpars
#define NMCQpars 3
#endif

class TrChargeDBH : public TObject {
   private:

   static TrChargeDBH* fHead;
   
   public:
   static bool UseTrChargeH;
   static int IsMC;
   static int fUseVersion;
   static bool UseTotal;
   static bool Useqtuning;
   static bool UseMSplineH;

   enum BasicChargeOptions {
      kSmear     = 1,
      kUnbias    = 2
   };
   enum StripCombineOptions {
      kStripWeight    = 1<<2,
      kStripCovMatrix = 2<<2
   };
   enum XYCombineOptions {
      kXYSigma     = (1<<4),
      kXYWeight    = (2<<4),
      kXYCovMatrix = (4<<4)
   };
   enum LayerCombineOptions {
      kPlainMean = (1<<7),
      kTruncMean = (2<<7),
      kGaussMean = (4<<7),
      kDropOne   = (8<<7),
      kDropFaraway1  = (0x10<<7),
      kDropFaraway2  = (0x20<<7)
   };
   enum PatternOptions {
      kAll       = (1<<13),
      kInner     = (2<<13),
      kUpper     = (4<<13),
      kLower     = (8<<13)
   };
   enum EnergyCorrOptions {
      kBeta     = (1<<17),
      kRigidity = (2<<17)
   };

   static MSplineH* fGainTot[2];

   static TrAsymmDBH* fkAsymm;
   static TrChargeLossDBH* fkLoss;
   static TrGainCHDBH* fkGainCH;
   static TrGainDBH* fkGain;
   static TrThetaDBH* fkTheta;
   static TrTempDBH* fkTemp;

   static TrBetaDBH* fkBeta;
   static TrRigidityDBH* fkRigidity;

   static MSplineH* fPeakCorr[9][2];
   static MSplineH* fPeak[2][NSQ+1];
   static MSplineH* fSigm[2][NSQ+1];
   static MSplineH* fPeakXY[2];
   static MSplineH* fSigmXY[2];
   static MSplineH* fPeakAll;
   static MSplineH* fSigmAll;
   static MSplineH* fWeightLayer[NSQ+1][9][2];
   static MSplineH* fParsBestLayer[9];
   static MSplineH* fParsBestInner;
   static MSplineH* fParsBestFS;

   //parameters for unbiased charge
   static MSplineH* fPeakCorrUnb[9][2];
   static MSplineH* fPeakUnb[2][NSQ+1];
   static MSplineH* fSigmUnb[2][NSQ+1];
   static MSplineH* fPeakXYUnb[2];
   static MSplineH* fSigmXYUnb[2];
   static MSplineH* fPeakAllUnb;
   static MSplineH* fSigmAllUnb;
   static MSplineH* fWeightLayerUnb[NSQ+1][9][2];
   static MSplineH* fParsBestLayerUnb[9];
   static MSplineH* fParsBestInnerUnb;
   static MSplineH* fParsBestFSUnb;

   //MC charge tunning parameters
   static MSplineH* fMCNormLayer[NMCQpars][9][3];
   static MSplineH* fMCPeakLayer[NMCQpars][9][3];
   static MSplineH* fMCSigmLayer[NMCQpars][9][3];
   static MSplineH* fMCNormInner[NMCQpars][3];
   static MSplineH* fMCPeakInner[NMCQpars][3];
   static MSplineH* fMCSigmInner[NMCQpars][3];
   static MSplineH* fMCNormAll[NMCQpars][3];
   static MSplineH* fMCPeakAll[NMCQpars][3];
   static MSplineH* fMCSigmAll[NMCQpars][3];

   static MSplineH* fMCNormLayerUnb[NMCQpars][9][3];
   static MSplineH* fMCPeakLayerUnb[NMCQpars][9][3];
   static MSplineH* fMCSigmLayerUnb[NMCQpars][9][3];
   static MSplineH* fMCNormInnerUnb[NMCQpars][3];
   static MSplineH* fMCPeakInnerUnb[NMCQpars][3];
   static MSplineH* fMCSigmInnerUnb[NMCQpars][3];
   static MSplineH* fMCNormAllUnb[NMCQpars][3];
   static MSplineH* fMCPeakAllUnb[NMCQpars][3];
   static MSplineH* fMCSigmAllUnb[NMCQpars][3];

   static MSplineH* kMCConvLayer[9][3][NZQ];
   static MSplineH* kMCConvInner[3][NZQ];
   static MSplineH* kMCConvAll[3][NZQ];

   static MSplineH* kMCConvLayerUnb[9][3][NZQ];
   static MSplineH* kMCConvInnerUnb[3][NZQ];
   static MSplineH* kMCConvAllUnb[3][NZQ];

   //parameters for MC charge after tunning,used for XY combination
   static MSplineH* fTunedPeakXY[2];
   static MSplineH* fTunedSigmXY[2];
   static MSplineH* fTunedPeakAll;
   static MSplineH* fTunedSigmAll;
   static MSplineH* fTunedParsBestLayer[9];
   static MSplineH* fTunedParsBestInner;
   static MSplineH* fTunedParsBestFS;
   static MSplineH* fTunedPeakXYUnb[2];
   static MSplineH* fTunedSigmXYUnb[2];
   static MSplineH* fTunedPeakAllUnb;
   static MSplineH* fTunedSigmAllUnb;
   static MSplineH* fTunedParsBestLayerUnb[9];
   static MSplineH* fTunedParsBestInnerUnb;
   static MSplineH* fTunedParsBestFSUnb;

   static int run_current;
   static int time_current;
   #pragma omp threadprivate (run_current)
   #pragma omp threadprivate (time_current)

   static int xcatg;
   static int addr_kinit[2][NSQ+1];
   static double eta_kinit[2];
   static double theta_kinit[2];
   static double charge_kinit[2][NSQ+1];
   static double eta_kasymm[2][NSQ+1];
   static double theta_kasymm[2][NSQ+1][2];
   static double charge_kasymm[2][NSQ+1];

   #pragma omp threadprivate (xcatg)
   #pragma omp threadprivate (addr_kinit)
   #pragma omp threadprivate (eta_kinit)
   #pragma omp threadprivate (theta_kinit)
   #pragma omp threadprivate (charge_kinit)
   #pragma omp threadprivate (eta_kasymm)
   #pragma omp threadprivate (theta_kasymm)
   #pragma omp threadprivate (charge_kasymm)

   static double charge_kloss[2][NSQ+1][NCalib];
   static double charge_kgainch[2][NSQ+1][NCalib];
   static double charge_kgain[2][NSQ+1][NCalib];
   static double charge_ktheta[2][NSQ+1][NCalib];
   static double charge_ktemp[2][NSQ+1][NCalib];
   static double charge_kall[2][NSQ+1];
   static double charge_kxy[3];

   #pragma omp threadprivate (charge_kloss)
   #pragma omp threadprivate (charge_kgainch)
   #pragma omp threadprivate (charge_kgain)
   #pragma omp threadprivate (charge_ktheta)
   #pragma omp threadprivate (charge_ktemp)
   #pragma omp threadprivate (charge_kall)
   #pragma omp threadprivate (charge_kxy)

   static double charge_kbeta[3];
   static double charge_krigidity[3];

   #pragma omp threadprivate (charge_kbeta)
   #pragma omp threadprivate (charge_krigidity)

   TrChargeDBH() {Init();}
   ~TrChargeDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadCharge(char* filename,bool isunb=false,bool istuned=false);
   static bool ReadXYWeight(char* filename,bool isunb=false,bool istuned=false);
   static bool ReadStripWeight(char* filename,bool isunb=false);
   static bool ReadPeakShift(char* filename,bool isunb=false);
   static bool ReadDifference(char* filename,int icomp=0,bool isunb=false);
   static bool ReadMultSmearing(char* filename,bool isunb=false);
   static bool ReadZQTuning(char* filename,int iside,int JLayer,bool isunb);
   static bool ReadQTuning(char* dirname);
   static TrChargeDBH* GetHead();
   static TrChargeDBH* GetHead(int run,int time,int ismc=-1);

   static float GetWeight(int iside,int istrip,int JLayer,float charge,bool isunb=false);
   static void InitCharge(int iside0=2);
   static double StripCombination(double StripCharge[NSQ+1],int iside,int JLayer,float Z0,int Opt=kStripWeight);
   static double XYCombination(double XYCharge[2],int JLayer,float Z0,int Opt=kXYWeight,int* npoints=0);
   static mean_t GetMean(int type, const vector<double>& signal,int iside=2);
   static mean_t GetPlainMean(const vector<double>& signal);
   static mean_t GetTruncMean(const vector<double>& signal);
   static mean_t GetGaussMean(const vector<double>& signal);
   static mean_t GetDropOne(const vector<double>& signal);
   static mean_t GetDropFaraway1(const vector<double>& signal,int Opt=0,int iside=2);
   static mean_t GetDropFaraway2(const vector<double>& signal,int Opt=0,int iside=2);
   static mean_t LayerCombination(double QLayers[9],int Opt=(kDropFaraway2|kAll),int iside=2);
   static float GetSqrtdEdX(int tkid,int iside,float *adc,float ip,float *ia,int seedaddress,int imult,float Z0=0,int ncalib=NCalib,int Opt=kStripWeight);
   static float GetSqrtdEdX(int tkid,float *xadc,float *yadc,float *ip,float *ia,int *seedaddress,int imult,float Z0=0,int ncalib=NCalib,int Opt=(kXYWeight|kStripWeight));
   static float GetQ2SqrtdEdX(float Z0,int JLayer,int iside,float beta,float rigidity,int Opt=(kBeta|kRigidity));
   static float GetQ(float q0,int JLayer,int iside,float beta,float rigidity,int Opt=(kBeta|kRigidity));
   static float GetEdep(int tkid,int iside,float *adc,float ip,float *ia,int seedaddress,int imult,float Z0=0,int ncalib=NCalib,int Opt=kStripWeight);
   static float GetEdep(int tkid,float *xadc,float *yadc,float *ip,float *ia,int *seedaddress,int imult,float Z0=0,int ncalib=NCalib,int Opt=(kXYWeight|kStripWeight));
   static float GetQ(int tkid,int iside,float *adc,float ip,float *ia,int seedaddress,int imult,float beta=1,float rigidity=0,float Z0=0,int ncalib=NCalib,int Opt=(kStripWeight|(kBeta|kRigidity)));
   static float GetQ(int tkid,float *xadc,float *yadc,float *ip,float *ia,int *seedaddress,int imult,float beta=1,float rigidity=0,float Z0=0,int ncalib=NCalib,int Opt=((kXYWeight|kStripWeight)|(kBeta|kRigidity)));

   static float GetMCSmearing(float q0,float nrand[2],int JLayer,int iside,bool isunb=false,int Z=-1);

   using TObject::Info;
   ClassDef(TrChargeDBH,1);
};

#endif
