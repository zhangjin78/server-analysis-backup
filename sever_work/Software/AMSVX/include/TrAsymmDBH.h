#ifndef __TrAsymmDBH_h__
#define __TrAsymmDBH_h__

#include "Fit2DH.h"
#include "TrChargeLossDBH.h"

#ifndef NZQ
#define NZQ 28
#endif
#ifndef NSQ
#define NSQ 5
#endif

class TrAsymmDBH : public TObject {
  private:
   static TrAsymmDBH* fHead;

  public:
   static bool useright;
   static Fit2DH* kAsymm[2][NSQ+1][NZQ];

   TrAsymmDBH() {Init();}
   ~TrAsymmDBH() {Release();}
   static void Init();
   static void Release();
   static TrAsymmDBH* GetHead();

   static bool DoAsymmetry(int seedindex,int stripindex,double ip);
   static double GetAsymmetryCorrection(int Z,int iside,int seedindex,int stripindex,double eta,double theta);
   using TObject::Info;
   ClassDef(TrAsymmDBH,1);
};

#endif
