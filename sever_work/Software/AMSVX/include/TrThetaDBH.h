#ifndef __TrThetaDBH_h__
#define __TrThetaDBH_h__

#include "MSplineH.h"
#include "TrChargeLossDBH.h"

#ifndef NZQ
#define NZQ 28
#endif
#ifndef NSQ
#define NSQ 5
#endif
#ifndef NCalib
#define NCalib 2
#endif

class TrThetaDBH : public TObject {
   private:

   static TrThetaDBH* fHead;
   
   public:

   static MSplineH* kTheta[NCalib][2][NSQ+1][NZQ];

   static int fUseVersion;
   TrThetaDBH() {Init();}
   ~TrThetaDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadTheta(int iside,int istrip,char* filename,int icalib=0);
   static bool LoadThetaFromDir(char *dirname,int icalib=0);
   static TrThetaDBH* GetHead();

   static MSplineH* GetTheta(int iside,int istrip,int Z,int icalib);
   static int FindRefLRZ(int &Zright,int Zinput,int iside,int istrip,int icalib);
   static double GetValue(int iside,int istrip,int Z,int icalib,double theta);
   static TGraph* GetCorrectionGraph(int iside,int istrip,double theta,int icalib=0);
   static MSplineH* GetCorrection(int iside,int istrip,double theta,int icalib=0);
   static int CountTheta(int iside=-1,int istrip=-1,int icalib=0);
   static double GetThetaCorrectedValue(int iside,int istrip,double theta,double charge,double Z0,int icalib=0,bool UseNewAlgo=false);

   using TObject::Info;
   ClassDef(TrThetaDBH,1);
};

#endif
