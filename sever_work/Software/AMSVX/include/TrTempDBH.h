#ifndef __TrTempDBH_h__
#define __TrTempDBH_h__

#include "MSplineH.h"
#include "root_setup.h"

#ifndef NZQ
#define NZQ 28
#endif
#ifndef NSQ
#define NSQ 5
#endif
#ifndef NCalib
#define NCalib 2
#endif

class TrTempDBH : public TObject {
   private:

   static TrTempDBH* fHead;

   //for tracker temperature (from SlowControlDB)
   static AMSSetupR setup;
   static int run_current;
   static int time_current;
   static double temperature_current[9];

   #pragma omp threadprivate (setup)
   #pragma omp threadprivate (run_current)
   #pragma omp threadprivate (time_current)
   #pragma omp threadprivate (temperature_current)
   
   public:

   static MSplineH* kTemp[NCalib][2][NSQ+1][2][9];

   static int fUseVersion;
   TrTempDBH() {Init();}
   ~TrTempDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadTemp(int iside,int istrip,char* filename,int icalib=0);
   static bool LoadTempFromDir(char *dirname,int icalib=0);
   static TrTempDBH* GetHead();

   static MSplineH* GetSlopeAndAmp(int iside,int istrip,int JLayer,bool IsSlope,int icalib=0);
   static TGraph* GetCorrectionGraph(int iside,int istrip,int JLayer,double temp,int icalib=0);
   static MSplineH* GetCorrection(int iside,int istrip,int JLayer,double temp,int icalib=0);
   static int CountTemp(int iside=-1,int istrip=-1,int icalib=0);
   static double GetTempCorrectedValueForce(int iside,int istrip,int JLayer,double temperature,double charge,double Z0,int icalib=0,bool UseNewAlgo=false);
   static double GetTemperature(int JLayer,int run=0,int time=0);
   static double GetTempCorrectedValue(int iside,int istrip,int JLayer,double charge,double Z0,int icalib=0,int run=0,int time=0,bool UseNewAlgo=false);

   using TObject::Info;
   ClassDef(TrTempDBH,1);
};

#endif
