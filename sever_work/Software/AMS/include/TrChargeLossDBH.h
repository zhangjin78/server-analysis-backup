#ifndef __TrChargeLossDBH_h__
#define __TrChargeLossDBH_h__

#include "Fit2DH.h"
#include "TrAsymmDBH.h"

#ifndef NZQ
#define NZQ 28
#endif
#ifndef NStrip
#define NStrip 5
#endif
#ifndef NSQ
#define NSQ 5
#endif
#ifndef NK
#define NK 4
#endif
#ifndef NCalib
#define NCalib 2
#endif

class TrChargeLossDBH : public TObject {
   private:

   static TrChargeLossDBH* fHead;
   
   public:

   static Fit2DH* kLoss[NCalib][2][NSQ+1][NK][NZQ];

   static int fUseVersion;
   static bool usedis;
   static bool foldeta;

   TrChargeLossDBH() {Init();}
   ~TrChargeLossDBH() {Release();}
   static void Init();
   static void Release();
   static bool Ignorelist(int iside,int istrip,int Z);
   static bool LoadMapFromDir(char *dirname,int icalib=0);
   static TrChargeLossDBH* GetHead();

   static Fit2DH* GetMap(int iside,int catg,int istrip,int Z,int icalib=0);
   static int FindRefLRZ(int &Zright,int Zinput,int iside,int catg,int istrip,int icalib);
   static double GetValue(int iside,int catg,int istrip,int Z,int icalib,double ip,double ia,bool UseNewAlgo=false);
   static TGraph* GetCorrectionGraph(int iside,int catg,int istrip,double ip,double ia,int icalib=0,bool IsSquare=true,bool UseNewAlgo=false);
   static MSplineH* GetCorrection(int iside,int catg,int istrip,double ip,double ia,int icalib=0,bool IsSquare=true,bool UseNewAlgo=false);
   ///return wheather reverse the eta, and the correct ip and ia to do the kLoss correction
   static int GetCategory(int iside,bool IsK7,int sensoraddress,double ip);
   static int GetStripIndex(int seedindex,int stripindex,double ip);
   static bool IsPositive(int istrip,bool useright);
   static double GetChargeLossCorrectedValue(int iside,int catg,int istrip,double ip,double ia,double adc,double Z0=0,int icalib=0,bool UseNewAlgo=false);

   using TObject::Info;
   ClassDef(TrChargeLossDBH,1);
};

#endif
