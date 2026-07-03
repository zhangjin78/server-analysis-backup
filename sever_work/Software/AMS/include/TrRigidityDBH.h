#ifndef __TrRigidityDBH_h__
#define __TrRigidityDBH_h__

#include "MSplineH.h"
#include "TrChargeLossDBH.h"

#ifndef NZQ
#define NZQ 28
#endif

class TrRigidityDBH : public TObject {
   private:

   static TrRigidityDBH* fHead;
   
   public:

   static MSplineH* kRigidity[NZQ][9][3];
   static MSplineH* kRigidity_SA[NZQ][9][3];

   static int fUseVersion;
   TrRigidityDBH() {Init();}
   ~TrRigidityDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadRigidity(int JLayer,int iside,char* filename,bool IsSA=false);
   static bool LoadRigidityFromDir(char *dirname);
   static TrRigidityDBH* GetHead();

   static MSplineH* GetRigidity(int JLayer,int iside,bool IsSA,int Z);
   static int FindRefLRZ(int &Zright,int Zinput,int JLayer,int iside,bool IsSA);
   static double GetValue(int JLayer,int iside,bool IsSA,int Z,double rigidity);
   static MSplineH* GetEDependence(double rigidity,int JLayer,int iside=2,bool IsSA=false);
   static TGraph* GetCorrectionGraph(double rigidity,int JLayer,int iside=2,bool IsSA=false);
   static MSplineH* GetCorrection(double rigidity,int JLayer,int iside=2,bool IsSA=false);
   static double GetRigidityCorrectedValue(double rigidity,int JLayer,int iside,double charge,bool IsSA=false,bool UseNewAlgo=false);

   using TObject::Info;
   ClassDef(TrRigidityDBH,1);
};

#endif
