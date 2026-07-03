#ifndef __TrGainDBH_h__
#define __TrGainDBH_h__

#include "MSplineH.h"

#ifndef NSQ
#define NSQ 5
#endif
#ifndef NCalib
#define NCalib 2
#endif

class TrGainDBH : public TObject {
   private:

   static TrGainDBH* fHead;
   
   public:

   static MSplineH* kGain[NCalib][2][NSQ+1][1920];

   static int fUseVersion;
   TrGainDBH() {Init();}
   ~TrGainDBH() {Release();}
   static void Init();
   static void Release();
   static bool ReadGain(int iside,int istrip,char* filename,int icalib=0);
   static bool LoadGainFromDir(char *dirname,int icalib=0);
   static TrGainDBH* GetHead();

   static MSplineH* GetCorrection(int iside,int istrip,int iva,int icalib=0);
   static int CountGain(int iside=-1,int istrip=-1,int icalib=0);
   static double GetGainCorrectedValue(int iside,int istrip,int vaindex,double charge,double Z0,int icalib=0);
   static double GetGainCorrectedValue(int iside,int istrip,int tkid,int iva,double charge,double Z0,int icalib=0);

   using TObject::Info;
   ClassDef(TrGainDBH,1);
};

#endif
