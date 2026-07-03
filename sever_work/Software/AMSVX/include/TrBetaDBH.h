#ifndef __TrBetaDBH_h__
#define __TrBetaDBH_h__

#include "MSplineH.h"
#include "TrChargeLossDBH.h"

#ifndef NZQ
#define NZQ 28
#endif

class TrBetaDBH : public TObject {
   private:

   static TrBetaDBH* fHead;
   
   public:

   static MSplineH* kBeta[NZQ][9][3];

   static int fUseVersion;
   TrBetaDBH() {Init();}
   ~TrBetaDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadBeta(int JLayer,int iside,char* filename);
   static bool LoadBetaFromDir(char *dirname);
   static TrBetaDBH* GetHead();

   static MSplineH* GetBeta(int JLayer,int iside,int Z);
   static int FindRefLRZ(int &Zright,int Zinput,int JLayer,int iside);
   static double GetValue(int JLayer,int iside,int Z,double beta);
   static MSplineH* GetEDependence(double beta,int JLayer,int iside=2);
   static TGraph* GetCorrectionGraph(double beta,int JLayer,int iside=2);
   static MSplineH* GetCorrection(double beta,int JLayer,int iside=2);
   static double GetBetaCorrectedValue(double beta,int JLayer,int iside,double charge,bool UseNewAlgo=false);

   using TObject::Info;
   ClassDef(TrBetaDBH,1);
};

#endif
