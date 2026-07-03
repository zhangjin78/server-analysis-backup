#ifndef __TrGainCHDBH_h__
#define __TrGainCHDBH_h__

#include "MSplineH.h"

#ifndef NZQ
#define NZQ 28
#endif
#ifndef NSQ
#define NSQ 5
#endif
#ifndef NCalib
#define NCalib 2
#endif

class TrGainCHDBH : public TObject {
   private:

   static TrGainCHDBH* fHead;
   
   public:

   static MSplineH* kGainCH[NCalib][2][NSQ+1][2][1920];

   static int fUseVersion;
   TrGainCHDBH() {Init();}
   ~TrGainCHDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadGainCH(int iside,int istrip,char* filename,int icalib=0);
   static bool LoadGainCHFromDir(char *dirname,int icalib=0);
   static TrGainCHDBH* GetHead();

   static MSplineH* GetSlopeAndAmp(int iside,int istrip,int iva,bool IsSlope,int icalib=0);
   static TGraph* GetCorrectionGraph(int iside,int istrip,int iva,int ich,int icalib=0);
   static MSplineH* GetCorrection(int iside,int istrip,int iva,int ich,int icalib=0);
   static int CountGainCH(int iside=-1,int istrip=-1,int icalib=0);
   static double GetGainCHCorrectedValue(int iside,int istrip,int vaindex,int ich,double charge,double Z0,int icalib=0,bool UseNewAlgo=false);
   static double GetGainCHCorrectedValue(int iside,int istrip,int tkid,int iva,int ich,double charge,double Z0,int icalib=0,bool UseNewAlgo=false);

   using TObject::Info;
   ClassDef(TrGainCHDBH,1);
};

#endif
