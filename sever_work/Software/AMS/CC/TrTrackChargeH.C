#include "TrTrackChargeH.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#else
#include "event.h"
#include "extC.h"
#endif

int TrTrackChargeH::DefaultOptISS=( (TrTrackChargeLightH::kDropFaraway2<<14) + (TrRecHitChargeLightH::kXYWeight<<11) + (TrClusterChargeLightH::kStripWeight|(TrClusterChargeLightH::kOverflow|TrClusterChargeLightH::kAsymEta)|TrClusterChargeLightH::kLinearCofG) );
int TrTrackChargeH::DefaultOptMC=( (TrTrackChargeLightH::kDropFaraway2<<14) + (TrRecHitChargeLightH::kXYWeight<<11) + (TrClusterChargeLightH::kStripWeight|(TrClusterChargeLightH::kSimAsym|TrClusterChargeLightH::kSimSignal)|TrClusterChargeLightH::kSimpCofG)|(TrClusterChargeLightH::kSmear) );
int TrTrackChargeH::NInnerQLoop=2;
void TrTrackChargeLightH::Init(){
   ptrk=0;
   for(int iside=0;iside<3;iside++){
      for(int ii=0;ii<2;ii++){
         mcsmear[0][iside][ii]=0;
         mcsmear[1][iside][ii]=0;
      }
   }
   qhit.clear();
}
void TrTrackChargeLightH::Release(){
   ptrk=0;
   for(int iside=0;iside<3;iside++){
      for(int ii=0;ii<2;ii++){
         mcsmear[0][iside][ii]=0;
         mcsmear[1][iside][ii]=0;
      }
   }
   qhit.clear();
}
void TrTrackChargeLightH::AssignPointer(TrTrackR* ptrk0){
   if(!ptrk0) return;
   ptrk=ptrk0;
   for(int ilay=0;ilay<9;ilay++){
      TrRecHitR* phit=ptrk->GetHitLJ(ilay+1);
      int nentry=qhit.count(ilay+1);
      bool exist=(nentry>=1);
      //map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      //bool exist=(it!=qhit.end());

      if((!phit)&&(!exist)) continue;
      else if((!phit)&&exist){
         qhit.erase(ilay+1);
         continue;
      }

      if(!exist){
         TrRecHitChargeLightH qhitbuff;
         qhitbuff.AssignPointer(phit);
         qhit.insert(make_pair(ilay+1,qhitbuff));
      }
      else{
         (qhit[ilay+1]).AssignPointer(phit);
      }
   }
}
void TrTrackChargeLightH::SetQtrk(float qxtrk, float qytrk, int iside){
   for(int ilay=0;ilay<9;ilay++){
      int nentry=qhit.count(ilay+1);
      bool exist=(nentry>=1);
      //map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      //bool exist=(it!=qhit.end());

      if(exist){
         (qhit[ilay+1]).SetQtrk(qxtrk,qytrk,iside);
      }
   }
}

void TrTrackChargeLightH::UpdateSmearing(){
   //smear for inner track charge only at the end, smear start from cluster charge
   //if(!ptrk) return;
   //for(int ik=0;ik<2;ik++){
   //   for(int iside=0;iside<3;iside++){
   //      for(int ii=0;ii<2;ii++){
   //         if(mcsmear[ik][iside][ii]!=0) continue;
   //         double rnd[1];
   //         #ifdef __ROOTSHAREDLIBRARY__
   //         AMSEventR::GetRandArray(8993306+(ik==0?(iside):(10*3+iside))+ii*33, ii==0?2:1,  1, rnd);
   //         #else
   //         rnd[0]=rnormx();
   //         #endif
   //         mcsmear[ik][iside][ii]=(float)rnd[0];
   //      }
   //   }
   //}
}
//GetMCSmearing do not need any more
float TrTrackChargeLightH::GetMCSmearing(int JLayer,int iside,int icomp){
   if(iside<0||iside>2) return 0;
   UpdateSmearing();
   if(JLayer<1) return mcsmear[0][iside][icomp];
   else if(JLayer>9) return mcsmear[1][iside][icomp];
   else{
      map<int,TrRecHitChargeLightH>::iterator it=qhit.find(JLayer);
      if(it==qhit.end()) return 0;
      else{
         if(iside==0||iside==1){
            map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(iside);
            if(it2==((it->second).qcluster).end()) return 0;
            else{
               (it2->second).UpdateSmearing();
               return (it2->second).mcsmear[icomp];
            }
         }
         else{
            (it->second).UpdateSmearing();
            return (it->second).mcsmear[icomp];
         }
      }
   }
}

int TrTrackChargeLightH::ConvertOpt(int Opt){
   Opt=TrTrackChargeH::ConvertChargeOpt(2,Opt);
   return (Opt&0xFFFFF9FF);
}

int TrTrackChargeLightH::IsChanged(int Opt,float Z0,bool IsCharge,float beta,int fit_id,bool forceupdate){
   int changed=0;
   if(!ptrk) return changed;
   double rigidity=(fit_id>=0&&ptrk)?ptrk->GetRigidity(fit_id):0;
   Opt=ConvertOpt(Opt); //remove kUnbias and kSmear bit for charge calculation

   int HitChargeOpt=TrTrackChargeH::ConvertChargeOpt(1,Opt);
   for(int ilay=0;ilay<9;ilay++){
      map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      if(it==qhit.end()) continue;
      double sqrtdedx0=Z0;
      if(IsCharge&&Z0>0){
         sqrtdedx0=TrChargeDBH::GetQ2SqrtdEdX(Z0,ilay+1,2,beta,rigidity,(TrChargeDBH::kBeta|TrChargeDBH::kRigidity));
      }
      if((it->second).IsChanged(HitChargeOpt,sqrtdedx0,forceupdate)!=0) changed=changed|(1<<ilay);
   }
   return changed;
}
bool TrTrackChargeLightH::Calculate(int JLayer,int Opt,float Z0,bool IsCharge,float beta,int fit_id){
   if(!ptrk) {Init(); return false;}
   //printf("TrTrackChargeLightH::Calculate fitid=%d\n",fit_id);
   double rigidity=(fit_id>=0&&ptrk)?ptrk->GetRigidity(fit_id):0;
   Opt=ConvertOpt(Opt); //remove kUnbias and kSmear bit for charge calculation

   int changed=0;
   //changed=IsChanged(Opt,Z0,IsCharge,beta,fit_id,false);
   int HitChargeOpt=TrTrackChargeH::ConvertChargeOpt(1,Opt);
   bool val_changed=false;
   for(int ilay=0;ilay<9;ilay++){
      if((JLayer>=1&&JLayer<=9)&&(JLayer!=ilay+1)) continue;
      map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      if(it==qhit.end()) continue;
      //if(changed&(1<<ilay)){
         double sqrtdedx0=Z0;
         if(IsCharge&&Z0>0){
            sqrtdedx0=TrChargeDBH::GetQ2SqrtdEdX(Z0,ilay+1,2,beta,rigidity,(TrChargeDBH::kBeta|TrChargeDBH::kRigidity));
         }
         bool changed_ilay=(it->second).Calculate(HitChargeOpt,sqrtdedx0);
         val_changed=val_changed||changed_ilay;
      //}
   }
   //printf("TrackCharge L%d finished. Z0=%.2f changed=%d\n",JLayer,Z0,changed);
   return val_changed;
}
TrTrackChargeLightH::TrTrackChargeLightH(TrTrackR* ptrk0,int Opt,float Z0,bool IsCharge,float beta,int fit_id){
   Init();
   if(!ptrk0) return;

   AssignPointer(ptrk0);
   Calculate(10,Opt,Z0,IsCharge,beta,fit_id);
   UpdateSmearing();
}
void TrTrackChargeLightH::copy(const TrTrackChargeLightH& orig){
   ptrk=orig.ptrk;
   for(int iside=0;iside<3;iside++){
      for(int ii=0;ii<2;ii++){
         mcsmear[0][iside][ii]=orig.mcsmear[0][iside][ii];
         mcsmear[1][iside][ii]=orig.mcsmear[1][iside][ii];
      }
   }
   qhit.insert((orig.qhit).begin(),(orig.qhit).end());
}

float TrTrackChargeLightH::GetSqrtdEdX(int JLayer,int iside,int Opt){
   if(JLayer<1||JLayer>9) return 0;
   map<int,TrRecHitChargeLightH>::iterator it=qhit.find(JLayer);
   if(it==qhit.end()) return 0;
   Calculate(JLayer,Opt,-1);
   bool dosmear=TrChargeDBH::IsMC&&(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kSmear);
   if(dosmear) UpdateSmearing();
   bool isunb=(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kUnbias);
   if(isunb){
      Opt=ConvertOpt(Opt)|(dosmear?TrClusterChargeLightH::kSmear:0);  //adding smear option if dosmear
      isunb=false;
   }
   double q0_XY[2]={0,0};
   for(int iside0=0;iside0<2;iside0++){
      map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(iside0);
      if(it2==((it->second).qcluster).end()) continue;
      else{
         int Ztrue=-1;
         //if(dosmear&&TrChargeDBH::Useqtuning) Ztrue=(it2->second).GetTrueZ(((it->second).phit)->GetResolvedMultiplicity());
         if(dosmear) q0_XY[iside0]=TrChargeDBH::GetMCSmearing((it2->second).SqrtdEdX,(it2->second).mcsmear,JLayer,iside0,isunb,Ztrue);
         else q0_XY[iside0]=(it2->second).SqrtdEdX;
      }
   }
   if(iside==0||iside==1){
      return q0_XY[iside];
   }
   else{
      if(dosmear){
         int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);
         double result=TrChargeDBH::XYCombination(q0_XY,JLayer,(it->second).SqrtdEdX,XYCombineOpt0);
         return TrChargeDBH::GetMCSmearing(result,(it->second).mcsmear,JLayer,2,isunb);
      }
      else return (it->second).SqrtdEdX;
   }
}
float TrTrackChargeLightH::GetdEdX(int JLayer,int iside,int Opt){
   return pow(GetSqrtdEdX(JLayer,iside,Opt),2);
}
float TrTrackChargeLightH::GetEdep(int JLayer,int iside,int Opt){
   if(JLayer<1||JLayer>9) return 0;
   map<int,TrRecHitChargeLightH>::iterator it=qhit.find(JLayer);
   if(it==qhit.end()) return 0;
   double dedx=GetdEdX(JLayer,iside,Opt);
   double costheta=(it->second).GetCosTheta();
   return dedx/costheta;
}
float TrTrackChargeLightH::GetQ0(float beta,int fit_id,int JLayer,int iside,int Opt,float Z0,bool IsCharge){
   double rigidity=(fit_id>=0&&ptrk)?ptrk->GetRigidity(fit_id):0;
   if(JLayer>=1&&JLayer<=9){
      double sqrtdedx=GetSqrtdEdX(JLayer,iside,Opt);
      float charge0=TrChargeDBH::GetQ(sqrtdedx,JLayer,iside,beta,rigidity);
      return charge0;
   }
   bool dosmear=TrChargeDBH::IsMC&&(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kSmear);
   if(dosmear) UpdateSmearing();
   bool isunb=(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kUnbias);
   if(isunb){
      Opt=ConvertOpt(Opt)|(dosmear?TrClusterChargeLightH::kSmear:0);  //adding smear option if dosmear
      isunb=false;
   }
   int LayerCombineOpt0=TrTrackChargeH::ConvertCombineOpt(2,Opt,true);
   int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);

   Calculate(JLayer,Opt,Z0,IsCharge,beta,fit_id);
   double q0[2][9]={{0}};
   for(int ilay=0;ilay<9;ilay++){
      for(int ii=0;ii<2;ii++) q0[ii][ilay]=0;
      map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      if(it==qhit.end()) continue;
      TrRecHitR* phit=(it->second).phit;
      if(!phit) continue;
      int qstatus=phit->GetQStatus();
      for(int ii=0;ii<2;ii++){
         if(iside>=0&&iside<2&&iside!=ii) continue;
         map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(ii);
         if(it2==((it->second).qcluster).end()) continue;
         if(ii==0&&((qstatus&0x1001FD)!=0)) continue;
         if(ii==1&&((qstatus&0x1FD100)!=0)) continue;
         q0[ii][ilay]=TrChargeDBH::GetQ((it2->second).SqrtdEdX,ilay+1,ii,beta,rigidity,TrChargeDBH::kBeta|TrChargeDBH::kRigidity);
         if(dosmear){
            int Ztrue=-1;
            if(TrChargeDBH::Useqtuning&&((beta!=0)||(rigidity!=0))) Ztrue=(it2->second).GetTrueZ(phit->GetResolvedMultiplicity());
            q0[ii][ilay]=TrChargeDBH::GetMCSmearing((it2->second).SqrtdEdX,(it2->second).mcsmear,ilay+1,ii,isunb,Ztrue);
            q0[ii][ilay]=TrChargeDBH::GetQ(q0[ii][ilay],ilay+1,ii,beta,rigidity,TrChargeDBH::kBeta|TrChargeDBH::kRigidity);
         }
      }
   }

   if(iside>=0&&iside<2){
      float charge0=TrChargeDBH::LayerCombination(q0[iside],LayerCombineOpt0|(JLayer>9?TrChargeDBH::kInner:TrChargeDBH::kAll)).Mean;
      if(dosmear) return TrChargeDBH::GetMCSmearing(charge0,mcsmear[JLayer>9][iside],JLayer,iside,isunb);
      else return charge0;
   }
   else{
      double charge_xy[2];
      int np_xy[2];
      for(int ii=0;ii<2;ii++){
         mean_t qlayercombined=TrChargeDBH::LayerCombination(q0[ii],LayerCombineOpt0|(JLayer>9?TrChargeDBH::kInner:TrChargeDBH::kAll));
         charge_xy[ii]=qlayercombined.Mean;
         np_xy[ii]=qlayercombined.NPoints;
      }
      float charge0=TrChargeDBH::XYCombination(charge_xy,JLayer>9?0:-1,Z0,XYCombineOpt0,np_xy);
      if(dosmear) return TrChargeDBH::GetMCSmearing(charge0,mcsmear[JLayer>9][2],JLayer,2,isunb);
      else return charge0;
   }
}

///////////////////
//TrTrackChargeH
///////////////////
int TrTrackChargeH::ConvertChargeOpt(int WhichClass,int Opt){
   if(Opt<0) Opt=TrChargeDBH::IsMC?TrTrackChargeH::DefaultOptMC:TrTrackChargeH::DefaultOptISS;
   if(WhichClass<=0){  //option for TrCluster Charge, the first 11 bits
      return Opt&0x7FF;
   }
   else if(WhichClass==1){  //option for TrRecHit Charge, the first 14 bits
      return Opt&0x3FFF;
   }
   else{  //option for TrTrack Charge, the first 24 bits
      return Opt&0xFFFFFF;
   }
}
int TrTrackChargeH::ConvertCombineOpt(int WhichClass,int Opt,bool IsTrChargeDBH){
   if(Opt<0) Opt=TrChargeDBH::IsMC?TrTrackChargeH::DefaultOptMC:TrTrackChargeH::DefaultOptISS;
   if(WhichClass<=0){  //option for TrCluster Strip Combination
      if(!IsTrChargeDBH) return (Opt&0x3);
      int result=0;
      if(Opt&TrClusterChargeLightH::kUnbias) result=result|TrChargeDBH::kUnbias;
      if(TrChargeDBH::IsMC&&(Opt&TrClusterChargeLightH::kSmear)) result=result|TrChargeDBH::kSmear;
      Opt=(Opt&0x3);
      if(Opt&TrClusterChargeLightH::kStripWeight) result=result|TrChargeDBH::kStripWeight;
      if(Opt&TrClusterChargeLightH::kStripCovMatrix) result=result|TrChargeDBH::kStripCovMatrix;
      return result;
   }
   else if(WhichClass==1){  //option for TrRecHit XY Combination
      if(!IsTrChargeDBH) return (Opt>>11)&0x7;
      int result=0;
      if(Opt&TrClusterChargeLightH::kUnbias) result=result|TrChargeDBH::kUnbias;
      if(TrChargeDBH::IsMC&&(Opt&TrClusterChargeLightH::kSmear)) result=result|TrChargeDBH::kSmear;
      Opt=(Opt>>11);
      if(Opt&TrRecHitChargeLightH::kXYSigma) result=result|TrChargeDBH::kXYSigma;
      if(Opt&TrRecHitChargeLightH::kXYWeight) result=result|TrChargeDBH::kXYWeight;
      if(Opt&TrRecHitChargeLightH::kXYCovMatrix) result=result|TrChargeDBH::kXYCovMatrix;
      return result;
   }
   else{  //option for TrTrack Layer Combination
      if(!IsTrChargeDBH) return (Opt>>14)&0x3F;
      int result=0;
      if(Opt&TrClusterChargeLightH::kUnbias) result=result|TrChargeDBH::kUnbias;
      if(TrChargeDBH::IsMC&&(Opt&TrClusterChargeLightH::kSmear)) result=result|TrChargeDBH::kSmear;
      Opt=(Opt>>14);
      if(Opt&TrTrackChargeLightH::kPlainMean) result=result|TrChargeDBH::kPlainMean;
      if(Opt&TrTrackChargeLightH::kTruncMean) result=result|TrChargeDBH::kTruncMean;
      if(Opt&TrTrackChargeLightH::kGaussMean) result=result|TrChargeDBH::kGaussMean;
      if(Opt&TrTrackChargeLightH::kDropOne) result=result|TrChargeDBH::kDropOne;
      if(Opt&TrTrackChargeLightH::kDropFaraway1) result=result|TrChargeDBH::kDropFaraway1;
      if(Opt&TrTrackChargeLightH::kDropFaraway2) result=result|TrChargeDBH::kDropFaraway2;
      return result;
   }
}

void TrTrackChargeH::Init(){
   TrTrackChargeLightH::Init();
   beta=0;
   rigidity=0;
   for(int iside=0;iside<3;iside++){
      for(int ilay=0;ilay<9;ilay++) Layercharge[iside][ilay]=0;
   }
   LayerCombineOpt=0;
   for(int iside=0;iside<3;iside++){
      Innercharge[iside].clear();
      Allcharge[iside].clear();
   }
}
void TrTrackChargeH::Release(){
   beta=0;
   rigidity=0;
   for(int iside=0;iside<3;iside++){
      for(int ilay=0;ilay<9;ilay++) Layercharge[iside][ilay]=0;
   }
   LayerCombineOpt=0;
   for(int iside=0;iside<3;iside++){
      Innercharge[iside].clear();
      Allcharge[iside].clear();
   }
}
TrTrackChargeH::TrTrackChargeH(TrTrackR* ptrk0,float beta0,int fit_id,int Opt,float Z0) {
   //printf("TrTrackChargeH::TrTrackChargeH0 ptrk=%p opt=%d beta=%.2f fitid=%d Z0=%.2f\n",ptrk0,Opt,beta0,fit_id,Z0);
   if(!ptrk0) return;
   AssignPointer(ptrk0);
   //printf("TrTrackChargeH::TrTrackChargeH ptrk=%p opt=%d beta=%.2f fitid=%d Z0=%.2f\n",ptrk0,Opt,beta0,fit_id,Z0);
   Calculate(beta0,fit_id,Opt,Z0);
   UpdateSmearing();
}
void TrTrackChargeH::copy(const TrTrackChargeH& orig){
   TrTrackChargeLightH::copy(orig);

   beta=orig.beta;
   rigidity=orig.rigidity;
   LayerCombineOpt=orig.LayerCombineOpt;
   for(int iside=0;iside<3;iside++){
      for(int ilay=0;ilay<9;ilay++){
         Layercharge[iside][ilay]=orig.Layercharge[iside][ilay];
      }
      Innercharge[iside]=orig.Innercharge[iside];
      Allcharge[iside]=orig.Allcharge[iside];
   }
}
int TrTrackChargeH::IsChanged(float beta0,int fit_id,int Opt,float Z0,bool forceupdate){
   Opt=TrTrackChargeH::ConvertChargeOpt(2,Opt);
   int LayerCombineOpt0=TrTrackChargeH::ConvertCombineOpt(2,Opt,false);
   double rigidity0=(fit_id>=0&&ptrk)?ptrk->GetRigidity(fit_id):0;
   if(fit_id<=-10) rigidity0=rigidity;
   int changed=TrTrackChargeLightH::IsChanged(beta0>-10?beta0:beta,fit_id>-10?fit_id:0,Opt,Z0,true,forceupdate);

   double margin=0.001;
   if(fabs(beta-beta0)>margin&&(beta0>-10)){
      changed=changed|(1<<9);
      if(forceupdate){
         beta=beta0;
      }
   }
   if(fabs(rigidity-rigidity0)>margin&&(fit_id>-10)){
      changed=changed|(1<<10);
      if(forceupdate){
         rigidity=rigidity0;
      }
   }
   if(LayerCombineOpt!=LayerCombineOpt0){
      changed=changed|(1<<11);
      if(forceupdate){
         LayerCombineOpt=LayerCombineOpt0;
      }
   }
   return changed;
}
bool TrTrackChargeH::CalculateChargeLayer(int JLayer,float beta0,int fit_id){
   double rigidity0=(fit_id>=0&&ptrk)?ptrk->GetRigidity(fit_id):0;
   if(fit_id<=-10) rigidity0=rigidity;
   bool changed=false;
   for(int ilay=0;ilay<9;ilay++){
      if((JLayer>=1&&JLayer<=9)&&(JLayer!=ilay+1)) continue;
      for(int iside=0;iside<3;iside++) Layercharge[iside][ilay]=0;
      map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      if(it==qhit.end()) continue;
      for(int iside=0;iside<3;iside++){
         double q0=0;
         if(iside>=0&&iside<=1){
            map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(iside);
            if(it2!=((it->second).qcluster).end()) q0=(it2->second).SqrtdEdX;
         }
         else{
            q0=(it->second).SqrtdEdX;
         }
         //printf("EnCorrection: L%d s%d qin=%.2f beta=%.3f rig=%.2e\n",ilay+1,iside,q0,beta,rigidity0);
         double qlayer0=TrChargeDBH::GetHead()->GetQ(q0,ilay+1,iside,beta0>-10?beta0:beta,rigidity0,TrChargeDBH::kBeta|TrChargeDBH::kRigidity);
         //printf("EnCorrection: L%d s%d qot=%.2f beta=%.3f rig=%.2e\n",ilay+1,iside,qlayer0,beta,rigidity0);
         double margin=0.005;
         if(fabs(qlayer0-Layercharge[iside][ilay])>margin) changed=true;
         Layercharge[iside][ilay]=qlayer0;
         if(beta0>-10) beta=beta0;
         if(fit_id>-10) rigidity=rigidity0;
      }
   }
   return changed;
}
bool TrTrackChargeH::CalculateChargeLayerCombined(int Opt,float Z0){
   Opt=TrTrackChargeLightH::ConvertOpt(Opt); //remove kUnbias and kSmear bit for charge calculation
   //LayerCombineOpt=TrTrackChargeH::ConvertCombineOpt(2,Opt,false);
   int LayerCombineOpt0=TrTrackChargeH::ConvertCombineOpt(2,Opt,true);
   int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);

   bool changed=false;
   double qarray[2][9];
   for(int ilay=0;ilay<9;ilay++){
      for(int iside=0;iside<2;iside++) qarray[iside][ilay]=0;
      map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
      if(it==qhit.end()) continue;
      TrRecHitR* phit=(it->second).phit;
      if(!phit) continue;
      int qstatus=phit->GetQStatus();
      for(int iside=0;iside<2;iside++){
         if(iside==0&&( (qstatus&0x1001FD)==0 )) qarray[iside][ilay]=Layercharge[iside][ilay];
         if(iside==1&&( (qstatus&0x1FD100)==0 )) qarray[iside][ilay]=Layercharge[iside][ilay];
      }
   }

   //charge of inner layers
   double qinn_old[3]={Innercharge[0].Mean,Innercharge[1].Mean,Innercharge[2].Mean};
   int qinn_np[2];
   double qinn_mean[2];
   double qinn_rms[2];
   for(int iside=0;iside<2;iside++){
      Innercharge[iside]=TrChargeDBH::LayerCombination(qarray[iside],LayerCombineOpt0|TrChargeDBH::kInner);
      Innercharge[iside].Side=iside;
      qinn_mean[iside]=Innercharge[iside].Mean;
      qinn_rms[iside]=Innercharge[iside].RMS;
      qinn_np[iside]=Innercharge[iside].NPoints;
   }
   double qinn_comb=TrChargeDBH::XYCombination(qinn_mean,0,Z0,XYCombineOpt0,qinn_np);
   double qinnrms_comb=TrChargeDBH::XYCombination(qinn_rms,0,Z0,XYCombineOpt0,qinn_np);
   Innercharge[2]=mean_t(LayerCombineOpt0,-1,2,qinn_np[0]+qinn_np[1],qinn_comb,qinnrms_comb);
   double margin=0.005;
   for(int iside=0;iside<3;iside++){
      if(fabs(Innercharge[iside].Mean-qinn_old[iside])>margin) changed=true;
   }
   //charge of all layers
   double qall_old[3]={Allcharge[0].Mean,Allcharge[1].Mean,Allcharge[2].Mean};
   int qall_np[2];
   double qall_mean[2];
   double qall_rms[2];
   for(int iside=0;iside<2;iside++){
      Allcharge[iside]=TrChargeDBH::LayerCombination(qarray[iside],LayerCombineOpt0|TrChargeDBH::kAll);
      Allcharge[iside].Side=iside;
      qall_mean[iside]=Allcharge[iside].Mean;
      qall_rms[iside] =Allcharge[iside].RMS;
      qall_np[iside]  =Allcharge[iside].NPoints;
   }
   double qall_comb=TrChargeDBH::XYCombination(qall_mean,-1,Z0,XYCombineOpt0,qall_np);
   double qallrms_comb=TrChargeDBH::XYCombination(qall_rms,-1,Z0,XYCombineOpt0,qall_np);
   Allcharge[2]=mean_t(LayerCombineOpt0,-1,2,qall_np[0]+qall_np[1],qall_comb,qallrms_comb);
   for(int iside=0;iside<3;iside++){
      if(fabs(Allcharge[iside].Mean-qall_old[iside])>margin) changed=true;
   }
   //printf("LayerCombineOpt=%d qinnx=%.2f qallx=%.2f\n",LayerCombineOpt0,Innercharge[0].Mean,Allcharge[0].Mean);
   //for(int ilay=0;ilay<9;ilay++) printf("%.2f%s",qarray[0][ilay],ilay==8?"\n":",");
   //printf("LayerCombineOpt=%d qinny=%.2f qally=%.2f\n",LayerCombineOpt0,Innercharge[1].Mean,Allcharge[1].Mean);
   //for(int ilay=0;ilay<9;ilay++) printf("%.2f%s",qarray[1][ilay],ilay==8?"\n":",");
   //printf("qinn=%.2f qall=%.2f\n",Innercharge[2].Mean,Allcharge[2].Mean);

   return changed;
}
bool TrTrackChargeH::Calculate(float beta0,int fit_id,int Opt,float Z0){
   if(!ptrk) {Init(); return false;}
   //printf("TrTrackChargeH::Calculate fitid=%d\n",fit_id);
   double rigidity0=(fit_id>=0&&ptrk)?ptrk->GetRigidity(fit_id):0;
   if(fit_id<=-10) rigidity0=rigidity;

   Opt=TrTrackChargeLightH::ConvertOpt(Opt); //remove kUnbias and kSmear bit for charge calculation
   int LayerCombineOpt0=TrTrackChargeH::ConvertCombineOpt(2,Opt,false);

   double Qall0[3]={Allcharge[0].Mean,Allcharge[1].Mean,Allcharge[2].Mean};
   double Qall1[3]={Allcharge[0].Mean,Allcharge[1].Mean,Allcharge[2].Mean};
   double Zx=Z0;
   for(int iloop=0;iloop<NInnerQLoop;iloop++){  //looping several times to calculate inner charge
      //printf("InnerTrackCharge iloop%d begined\n",iloop);
      bool dedx_changed=TrTrackChargeLightH::Calculate(10,Opt,Zx,true,beta0,fit_id);
      bool opt_changed=(LayerCombineOpt0!=LayerCombineOpt);
      double margin=0.001;
      bool ecor_changed=( (fabs(beta-beta0)>margin&&(beta0>-10)) || (fabs(rigidity-rigidity0)>margin&&(fit_id>-10)) );
      bool recal=(dedx_changed||ecor_changed)||opt_changed;
      if(beta0>-10) beta=beta0;
      if(fit_id>-10) rigidity=rigidity0;
      LayerCombineOpt=LayerCombineOpt0;

      if(!recal) break;

      //recalculate the charge of single layer
      if(dedx_changed||ecor_changed){
         CalculateChargeLayer(10,beta0,fit_id);
      }
      //printf("TrTrackChargeH:: dedx_changed=%d ecor_changed=%d %.2f\n",dedx_changed,ecor_changed,Layercharge[2][2]);
      //printf("InnerTrackCharge LayerCharge finished\n");

      //recalculate the charge of combining several layers
      //printf("TrTrackChargeH:: layer combination\n");
      CalculateChargeLayerCombined(Opt,Zx);
      for(int iside=0;iside<3;iside++) Qall1[iside]=Allcharge[iside].Mean;
      //printf("InnerTrackCharge LayerCombCharge finished\n");

      //reset the inner tracker charge used for all the eta calculation
      //if(Innercharge[0].Mean>0||Innercharge[1].Mean>0) SetQtrk(Innercharge[1].Mean,Innercharge[0].Mean,2);
      double laydedx[3][9]={{0}};
      for(int ilay=0;ilay<9;ilay++){
         for(int iside=0;iside<3;iside++) laydedx[iside][ilay]=0;
         if(ilay==0||ilay==8) continue;
         map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
         if(it==qhit.end()) continue;
         int qstatus=((it->second).phit)?((it->second).phit)->GetQStatus():0;
         for(int iside=0;iside<3;iside++){
            if(iside==0&&((qstatus&0x1001FD)!=0)) continue;
            if(iside==1&&((qstatus&0x1FD100)!=0)) continue;
            if(iside==2&&((qstatus&0x1FD1FD)!=0)) continue;
            if(iside==2) {laydedx[iside][ilay]=(it->second).SqrtdEdX; continue;}
            map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(iside);
            if(it2==((it->second).qcluster).end()) continue;
            else{
               laydedx[iside][ilay]=(it2->second).SqrtdEdX;
            }
         }
      }
      double inndedx[3]={0,0,0};
      int LayerCombineOpt1=TrTrackChargeH::ConvertCombineOpt(2,Opt,true);
      for(int iside=0;iside<3;iside++) inndedx[iside]=TrChargeDBH::LayerCombination(laydedx[iside],LayerCombineOpt1|TrChargeDBH::kInner,iside).Mean;
      //printf("SetQtrk: beta=%.3f SqrtdEdX={%.3f,%.3f,%.3f} Q={%.3f,%.3f,%.3f}\n",beta0,inndedx[0],inndedx[1],inndedx[2],Innercharge[0].Mean,Innercharge[1].Mean,Innercharge[2].Mean);
      if(inndedx[0]>0||inndedx[1]>0) SetQtrk(inndedx[1],inndedx[0],2);
      //printf("InnerTrackCharge iloop%d finished\n",iloop);

      if(Z0<=0&&Innercharge[2].Mean>0) Zx=Innercharge[2].Mean;
      if(Z0>0) break;
   }
   bool result=false;
   double margin=0.005;
   for(int iside=0;iside<3;iside++){
      if(fabs(Qall0[iside]-Qall1[iside])>margin) result=true;
   }
   return result;
}
float TrTrackChargeH::GetQ(int JLayer,int iside,float beta0,int fit_id,int Opt,float Z0){
   if(!ptrk) return 0;
   if(iside<0||iside>2) iside=2;
   Calculate(beta0,fit_id,Opt,Z0);
   bool dosmear=TrChargeDBH::IsMC&&(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kSmear);
   if(dosmear) UpdateSmearing();
   else{
      if(JLayer>=1&&JLayer<=9) return Layercharge[iside][JLayer-1];
      else if(JLayer<1) return Allcharge[iside].Mean;
      else return Innercharge[iside].Mean;
   }
   bool isunb=(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kUnbias);
   if(isunb){
      Opt=ConvertOpt(Opt)|(dosmear?TrClusterChargeLightH::kSmear:0);  //adding smear option if dosmear
      isunb=false;
   }
   int LayerCombineOpt0=TrTrackChargeH::ConvertCombineOpt(2,Opt,true);
   int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);
   double q0[3][9]={{0}};
   for(int ilay=0;ilay<9;ilay++){
      for(int iside0=0;iside0<2;iside0++){
         float mcsmear0[2];
         for(int ii=0;ii<2;ii++) mcsmear0[ii]=GetMCSmearing(ilay,iside0,ii);
         int Ztrue=-1;
         map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
         if(!(it==qhit.end())){
            map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(iside0);
            if(!(it2==((it->second).qcluster).end())) if((dosmear&&TrChargeDBH::Useqtuning)&&((beta0!=0)||(fit_id>=0))) Ztrue=(it2->second).GetTrueZ(((it->second).phit)->GetResolvedMultiplicity());
         }
         q0[iside0][ilay]=TrChargeDBH::GetMCSmearing(Layercharge[iside0][ilay],mcsmear0,ilay+1,iside0,isunb,Ztrue);
      }
      float mcsmear0[2];
      for(int ii=0;ii<2;ii++) mcsmear0[ii]=GetMCSmearing(ilay,2,ii);
      double q0_XY[2]={q0[0][ilay],q0[1][ilay]};
      q0[2][ilay]=TrChargeDBH::XYCombination(q0_XY,ilay+1,Layercharge[2][ilay],XYCombineOpt0);
      q0[2][ilay]=TrChargeDBH::GetMCSmearing(q0[2][ilay],mcsmear0,ilay+1,2,isunb);
   }

   if(JLayer>=1&&JLayer<=9){
      return q0[iside][JLayer-1];
   }
   else{
      for(int ilay=0;ilay<9;ilay++){
         TrRecHitR* phit=ptrk->GetHitLJ(ilay+1);
         if(!phit) continue;
         int qstatus=phit->GetQStatus();
         for(int iside0=0;iside0<2;iside0++){
            if(iside0==0&&((qstatus&0x1001FD)!=0)) q0[iside0][ilay]=0;
            if(iside0==1&&((qstatus&0x1FD100)!=0)) q0[iside0][ilay]=0;
         }
      }
      float mcsmear0[2];
      for(int ii=0;ii<2;ii++) mcsmear0[ii]=GetMCSmearing(JLayer,iside,ii);
      mean_t qcomb[2];
      for(int iside0=0;iside0<2;iside0++){
         qcomb[iside0]=TrChargeDBH::LayerCombination(q0[iside0],LayerCombineOpt0|(JLayer<1?TrChargeDBH::kAll:TrChargeDBH::kInner),iside0);
      }
      if(iside>=0&&iside<2){
         float charge0=qcomb[iside].Mean;
         return TrChargeDBH::GetMCSmearing(charge0,mcsmear0,JLayer,iside,isunb);
      }
      else{
         double q0_XY[2]={qcomb[0].Mean,qcomb[1].Mean};
         int nXY[2]={qcomb[0].NPoints,qcomb[1].NPoints};
         float charge0=TrChargeDBH::XYCombination(q0_XY,JLayer>9?0:-1,JLayer>9?Innercharge[iside].Mean:Allcharge[iside].Mean,XYCombineOpt0,nXY);
         return TrChargeDBH::GetMCSmearing(charge0,mcsmear0,JLayer,2,isunb);
      }
   }
}
mean_t TrTrackChargeH::GetCombQ(int iside,float beta0,int fit_id,bool IsInner,int Opt,float Z0){
   if(!ptrk) return 0;
   if(iside<0||iside>2) iside=2;
   Calculate(beta0,fit_id,Opt,Z0);
   bool dosmear=TrChargeDBH::IsMC&&(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kSmear);
   if(dosmear) UpdateSmearing();
   else{
      if(!IsInner) return Allcharge[iside];
      else return Innercharge[iside];
   }
   bool isunb=(TrTrackChargeH::ConvertChargeOpt(2,Opt)&TrClusterChargeLightH::kUnbias);
   if(isunb){
      Opt=ConvertOpt(Opt)|(dosmear?TrClusterChargeLightH::kSmear:0);  //adding smear option if dosmear
      isunb=false;
   }
   int LayerCombineOpt0=TrTrackChargeH::ConvertCombineOpt(2,Opt,true);
   int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);
   double q0[2][9]={{0}};
   for(int ilay=0;ilay<9;ilay++){
      TrRecHitR* phit=ptrk->GetHitLJ(ilay+1);
      if(!phit) continue;
      int qstatus=phit->GetQStatus();
      for(int iside0=0;iside0<2;iside0++){
         float mcsmear0[2];
         for(int ii=0;ii<2;ii++) mcsmear0[ii]=GetMCSmearing(ilay,iside0,ii);
         int Ztrue=-1;
         map<int,TrRecHitChargeLightH>::iterator it=qhit.find(ilay+1);
         if(!(it==qhit.end())){
            map<int,TrClusterChargeLightH>::iterator it2=((it->second).qcluster).find(iside0);
            if(!(it2==((it->second).qcluster).end())) if((dosmear&&TrChargeDBH::Useqtuning)&&((beta0!=0)||(fit_id>=0))) Ztrue=(it2->second).GetTrueZ(((it->second).phit)->GetResolvedMultiplicity());
         }
         q0[iside0][ilay]=TrChargeDBH::GetMCSmearing(Layercharge[iside0][ilay],mcsmear0,ilay+1,iside0,isunb,Ztrue);
         if(iside0==0&&((qstatus&0x1001FD)!=0)) q0[iside0][ilay]=0;
         if(iside0==1&&((qstatus&0x1FD100)!=0)) q0[iside0][ilay]=0;
      }
   }
   mean_t qcomb[2];
   for(int iside0=0;iside0<2;iside0++){
      qcomb[iside0]=TrChargeDBH::LayerCombination(q0[iside0],LayerCombineOpt0|((!IsInner)?TrChargeDBH::kAll:TrChargeDBH::kInner),iside0);
   }
   if(iside>=0&&iside<2){
      qcomb[iside].Mean=TrChargeDBH::GetMCSmearing(qcomb[iside].Mean,mcsmear[IsInner][iside],IsInner?10:0,iside,isunb);
      return qcomb[iside];
   }
   else{
      double q0_XY[2]={qcomb[0].Mean,qcomb[1].Mean};
      double qrms_XY[2]={qcomb[0].RMS,qcomb[1].RMS};
      int nXY[2]={qcomb[0].NPoints,qcomb[1].NPoints};
      float charge0=TrChargeDBH::XYCombination(q0_XY,IsInner?0:-1,IsInner?Innercharge[iside].Mean:Allcharge[iside].Mean,XYCombineOpt0,nXY);
      charge0=TrChargeDBH::GetMCSmearing(charge0,mcsmear[IsInner][iside],IsInner?10:0,2,isunb);
      double qrms0=TrChargeDBH::XYCombination(qrms_XY,IsInner?0:-1,IsInner?Innercharge[2].Mean:Allcharge[2].Mean,XYCombineOpt0,nXY);
      return mean_t(LayerCombineOpt0,IsInner?0:-1,iside,nXY[0]+nXY[1],charge0,qrms0);
   }
}
