#include "TrAsymmDBH.h"

TrAsymmDBH* TrAsymmDBH::fHead;
bool TrAsymmDBH::useright=true;
Fit2DH* TrAsymmDBH::kAsymm[2][NSQ+1][NZQ];

void TrAsymmDBH::Init(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int iz=0;iz<NZQ;iz++){
            kAsymm[iside][istrip][iz]=0;
         }
      }
   }
   fHead=0;
}
void TrAsymmDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int iz=0;iz<NZQ;iz++){
            if(kAsymm[iside][istrip][iz]) {delete kAsymm[iside][istrip][iz]; kAsymm[iside][istrip][iz]=0;}
         }
      }
   }
   fHead=0;
}

TrAsymmDBH* TrAsymmDBH::GetHead(){
   if(!fHead){
      fHead=new TrAsymmDBH();
      return fHead;
   }
   else return fHead;
}

bool TrAsymmDBH::DoAsymmetry(int seedindex,int stripindex,double ip){
   if(TrChargeLossDBH::usedis||((!TrChargeLossDBH::usedis)&&TrChargeLossDBH::foldeta)){
      if(stripindex>=NSQ) return !( (useright&&ip>=0)||((!useright)&&ip<=0) );
      return !( ((useright&&stripindex>seedindex)||((!useright)&&stripindex<seedindex)) || (stripindex==seedindex&&((useright&&ip>=0)||((!useright)&&ip<=0))) );
   }
   else return false;
}
double TrAsymmDBH::GetAsymmetryCorrection(int Z,int iside,int seedindex,int stripindex,double eta,double theta){
   if(Z<1||Z>NZQ) return 1;
   bool doasym=DoAsymmetry(seedindex,stripindex,eta);
   if(!doasym) return 1;
   int istrip0=(stripindex<NSQ)?abs(stripindex-seedindex):0;
   if(istrip0<0||istrip0>NSQ) return 1;
   if(!kAsymm[iside][istrip0][Z-1]) return 1;
   double efasym;
   return kAsymm[iside][istrip0][Z-1]->Eval(efasym,eta,theta);
}
