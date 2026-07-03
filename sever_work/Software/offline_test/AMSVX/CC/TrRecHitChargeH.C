#include "TrRecHitChargeH.h"
#include "TrRecHit.h"
#include "TrTrackChargeH.h"
#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#else
#include "event.h"
#include "extC.h"
#endif

//UChar_t TrRecHitChargeLightH::XYCombineOpt=TrRecHitChargeLightH::kXYSigma;
//int TrRecHitChargeH::DefaultOpt=( ( ((TrClusterChargeLightH::kStripWeight|(TrClusterChargeLightH::kOverflow|TrClusterChargeLightH::kAsymEta))|(TrClusterChargeLightH::kLinearCofG) )<<3) +TrRecHitChargeLightH::kXYWeight);
void TrRecHitChargeLightH::Init(){
   phit=0;
   for(int ii=0;ii<2;ii++) mcsmear[ii]=0;
   XYCombineOpt=0;
   qcluster.clear();
   SqrtdEdX=0;
}
void TrRecHitChargeLightH::Release(){
   phit=0;
   for(int ii=0;ii<2;ii++) mcsmear[ii]=0;
   XYCombineOpt=0;
   qcluster.clear();
   SqrtdEdX=0;
}
void TrRecHitChargeLightH::AssignPointer(TrRecHitR* phit0){
   if(!phit0) return;
   phit=phit0;
   for(int iside=0;iside<2;iside++){
      TrClusterR* pcluster=(iside==0)?phit->GetXCluster():phit->GetYCluster();
      int nentry=qcluster.count(iside);
      bool exist=(nentry>=1);
      //map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
      //bool exist=(it!=qcluster.end());

      if((!pcluster)&&(!exist)) continue;
      else if((!pcluster)&&exist){
         qcluster.erase(iside);
         continue;
      }

      if(!exist){
         TrClusterChargeLightH qbuff;
         qbuff.AssignPointer(pcluster);
         qcluster.insert(pair<int,TrClusterChargeLightH>(iside,qbuff));
      }
      else{
         (qcluster[iside]).AssignPointer(pcluster);
      }
   }
}
void TrRecHitChargeLightH::SetQtrk(float qxtrk, float qytrk,int iside){
   for(int iside0=0;iside0<2;iside0++){
      int nentry=qcluster.count(iside0);
      bool exist=(nentry>=1);
      //map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside0);
      //bool exist=(it!=qcluster.end());

      if(exist){
         if(iside==2||(iside==iside0)) (qcluster[iside0]).SetQtrk(iside0==0?qxtrk:qytrk);
      }
   }
}

void TrRecHitChargeLightH::UpdateSmearing(){
   //smear hit charge only at the end, smear start from cluster charge
   //if(!phit) return;
   //for(int ii=0;ii<2;ii++){
   //   if(mcsmear[ii]!=0) continue;
   //   double rnd[1];
   //   #ifdef __ROOTSHAREDLIBRARY__
   //   AMSEventR::GetRandArray(8993306+(phit->GetLayerJ()*3+2)+ii*33, ii==0?2:1,  1, rnd);
   //   #else
   //   rnd[0]=rnormx();
   //   #endif
   //   mcsmear[ii]=(float)rnd[0];
   //}
}
float TrRecHitChargeLightH::GetMCSmearing(int iside,int icomp){
   if(iside<2&&iside>=0){
      map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
      if(it==qcluster.end()) return 0;
      else{
         (it->second).UpdateSmearing();
         return (it->second).mcsmear[icomp];
      }
   }
   else{
      UpdateSmearing();
      return mcsmear[icomp];
   }
}

int TrRecHitChargeLightH::ConvertOpt(int Opt,float dxdz,float dydz){
   Opt=TrTrackChargeH::ConvertChargeOpt(1,Opt);
   bool isunb=(!(dxdz<-1.99&&dydz<-1.99));
   if(isunb) Opt=Opt|TrClusterChargeLightH::kUnbias;
   return (Opt&0xFFFFFDFF);
}

int TrRecHitChargeLightH::IsChanged(int Opt,float Z0,bool forceupdate,int imult,float dxdz,float dydz){
   int changed=0;
   if(!phit) return changed;
   Opt=ConvertOpt(Opt,dxdz,dydz);
   int ClusterOpt=TrTrackChargeH::ConvertChargeOpt(0,Opt);
   bool isunb=(ClusterOpt&TrClusterChargeLightH::kUnbias);
   int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,false);

   int xmult=(imult>=0)?imult:phit->GetResolvedMultiplicity();

   //check the combination option
   if(XYCombineOpt!=XYCombineOpt0){
      changed=changed|(1<<0);
      if(forceupdate){
         XYCombineOpt=XYCombineOpt0;
      }
   }

   //check for each side
   for(int iside=0;iside<2;iside++){
      map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
      if(it==qcluster.end()) continue;
      int xychanged=(it->second).IsChanged(xmult,ClusterOpt,Z0,forceupdate,dxdz,dydz);
      changed=changed|(xychanged<<(iside*(7+NStrip)+1));
   }

   return changed;
}
bool TrRecHitChargeLightH::Calculate(int Opt,float Z0,int imult,float dxdz,float dydz){
   if(!phit) {Init(); return false;}
   bool debug=false;

   int xmult=(imult>=0)?imult:phit->GetResolvedMultiplicity();
   Opt=ConvertOpt(Opt,dxdz,dydz);
   int mask=0;
   for(int ibit=0;ibit<7+NStrip;ibit++) mask=mask|(1<<ibit);

   double SqrtdEdX0[3]={0,0,SqrtdEdX};
   for(int iside=0;iside<2;iside++){
      map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
      if(it==qcluster.end()) continue;
      SqrtdEdX0[iside]=(it->second).SqrtdEdX;
   }
   double q0_XY[3]={SqrtdEdX0[0],SqrtdEdX0[1],SqrtdEdX0[2]};
   double Zx=Z0;
   int nloop=2;
   double margin=0.005;
   for(int iloop=0;iloop<nloop;iloop++){
      int changed=0;
      //changed=IsChanged(Opt,Zx,false,xmult,dxdz,dydz);
      //if(changed==0) break;

      int JLayer=phit?phit->GetLayerJ():0;
      int ClusterOpt=TrTrackChargeH::ConvertChargeOpt(0,Opt);
      int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);
      for(int iside=0;iside<2;iside++){
         map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
         if(it==qcluster.end()) continue;
         if(debug) printf("L%d Loop%d is%d:\n",JLayer,iloop,iside);
         //if(!(changed&(mask<<(iside*(7+NStrip)+1)))){
         //   q0_XY[iside]=(it->second).SqrtdEdX;
         //   continue;
         //}
         bool ichange=(it->second).Calculate(xmult,ClusterOpt,Zx,dxdz,dydz);
         if(ichange) changed=(changed|(1<<iside));
         q0_XY[iside]=(it->second).SqrtdEdX;
      }
      int XYCombineOpt_hit=TrTrackChargeH::ConvertCombineOpt(1,Opt,false);
      if(XYCombineOpt_hit!=XYCombineOpt){
         XYCombineOpt=XYCombineOpt_hit;
         changed=(changed|(1<<2));
      }

      if(!changed) break;

      SqrtdEdX=TrChargeDBH::XYCombination(q0_XY,JLayer,Zx,XYCombineOpt0);
      q0_XY[2]=SqrtdEdX;
      double Zx_new=q0_XY[0]>0?q0_XY[0]:q0_XY[1];
      if(debug) printf("L%d Loop%d changed=%d mask=%d(%.2f,%.2f): Qx=%.2f Qy=%.2f Q=%.2f\n",JLayer,iloop,changed,mask,Z0,Zx,q0_XY[0],q0_XY[1],SqrtdEdX);

      if(Z0<=0&&Zx_new>0) Zx=Zx_new;
      if(Z0>0) break;
   }
   //printf("HitCharge L%d(tkid=%+4d) finished, Z0=%.2f qold={%.2f,%.2f,%.2f} qnew={%.2f,%.2f,%.2f}\n",phit->GetLayerJ(),phit->GetTkId(),Z0,SqrtdEdX0[0],SqrtdEdX0[1],SqrtdEdX0[2],q0_XY[0],q0_XY[1],q0_XY[2]);
   bool result=false;
   for(int iside=0;iside<3;iside++){
      if(fabs(SqrtdEdX0[iside]-q0_XY[iside])>margin) result=true;
   }
   return result;
}
float TrRecHitChargeLightH::GetCosTheta(){
   double angle[2]={0,0};
   map<int,TrClusterChargeLightH>::iterator it[2]={qcluster.find(0),qcluster.find(1)};
   for(int iside=0;iside<2;iside++){
      if(it[iside]==qcluster.end()){
         if(it[1-iside]!=qcluster.end()) angle[iside]=(it[1-iside]->second).angle[iside];
         continue;
      }
      angle[iside]=((it[iside])->second).angle[iside];
   }
   double costheta=1/sqrt(1+angle[0]*angle[0]+angle[1]*angle[1]);
   return costheta;
}

TrRecHitChargeLightH::TrRecHitChargeLightH(TrRecHitR* phit0,int Opt,float Z0,int imult,float dxdz,float dydz){
   Init();
   if(!phit0) return;

   AssignPointer(phit0);
   Calculate(Opt,Z0,imult,dxdz,dydz);
   UpdateSmearing();
}
void TrRecHitChargeLightH::copy(const TrRecHitChargeLightH& orig){
   phit=orig.phit;
   for(int ii=0;ii<2;ii++) mcsmear[ii]=orig.mcsmear[ii];
   XYCombineOpt=orig.XYCombineOpt;
   qcluster.insert((orig.qcluster).begin(),(orig.qcluster).end());
   SqrtdEdX=orig.SqrtdEdX;
}

float TrRecHitChargeLightH::GetSqrtdEdX(int Opt,float Z0,int iside,int imult,float dxdz,float dydz){
   Calculate(Opt,Z0,imult,dxdz,dydz);
   bool dosmear=TrChargeDBH::IsMC&&(TrTrackChargeH::ConvertChargeOpt(1,Opt)&TrClusterChargeLightH::kSmear);
   if(dosmear) UpdateSmearing();
   bool isunb=(ConvertOpt(Opt,dxdz,dydz)&TrClusterChargeLightH::kUnbias);
   double q0_XY[2]={0,0};
   for(int iside0=0;iside0<2;iside0++){
      map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside0);
      if(it==qcluster.end()) continue;
      else{
         float SqrtdEdX0=(it->second).SqrtdEdX;
         if(dosmear){
            (it->second).UpdateSmearing();
            int Ztrue=-1;
            //if(dosmear&&TrChargeDBH::Useqtuning) Ztrue=(it->second).GetTrueZ(imult>=0?imult:(phit?phit->GetResolvedMultiplicity():0));
            q0_XY[iside0]=TrChargeDBH::GetMCSmearing(SqrtdEdX0,(it->second).mcsmear,phit?phit->GetLayerJ():0,iside,isunb,Ztrue);
         }
         else q0_XY[iside0]=SqrtdEdX0;
      }
   }
   if(iside==0||iside==1){
      return q0_XY[iside];
   }
   else{
      if(dosmear){
         int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);
         double result=TrChargeDBH::XYCombination(q0_XY,phit?phit->GetLayerJ():0,SqrtdEdX,XYCombineOpt0);
         return TrChargeDBH::GetMCSmearing(result,mcsmear,phit?phit->GetLayerJ():0,iside,isunb);
      }
      else return SqrtdEdX;
   }
}
float TrRecHitChargeLightH::GetdEdX(int Opt,bool IsMIP,float Z0,int iside,int imult,float dxdz,float dydz){
   double dedx0=pow(GetSqrtdEdX(Opt,Z0,iside,imult,dxdz,dydz),2);
   if(IsMIP) return dedx0;
   else      return dedx0*0.1;
}
float TrRecHitChargeLightH::GetEdep(int Opt,bool IsMIP,float Z0,int iside,int imult,float dxdz,float dydz){
   double dedx0=GetdEdX(Opt,IsMIP,Z0,iside,imult,dxdz,dydz);
   double costheta=GetCosTheta();
   return dedx0/costheta;
}
float TrRecHitChargeLightH::GetQ0(float beta,float rigidity,int Opt,float Z0,int iside,int imult,float dxdz,float dydz){
   double q0=GetSqrtdEdX(Opt,Z0,iside,imult,dxdz,dydz);
   int JLayer=phit?phit->GetLayerJ():0;
   return TrChargeDBH::GetHead()->GetQ(q0,JLayer,iside,beta,rigidity);
}

///////////////////
//TrRecHitChargeH
///////////////////
void TrRecHitChargeH::Init(){
   TrRecHitChargeLightH::Init();
   beta=0;
   rigidity=0;
   XYcharge[0]=0;
   XYcharge[1]=0;
   charge=0;
}
void TrRecHitChargeH::Release(){
   beta=0;
   rigidity=0;
   XYcharge[0]=0;
   XYcharge[1]=0;
   charge=0;
}
TrRecHitChargeH::TrRecHitChargeH(TrRecHitR* phit0,float beta0,float rigidity0,int Opt,float Z0,int imult,float dxdz,float dydz) {
   if(!phit0) return;
   AssignPointer(phit0);
   TrRecHitChargeLightH::Calculate(Opt,Z0,imult,dxdz,dydz);
   UpdateSmearing();

   int JLayer=phit0?phit0->GetLayerJ():0;
   if(beta0>-10) beta=beta0;
   if(rigidity0>-1e10) rigidity=rigidity0;
   for(int iside=0;iside<2;iside++){
      XYcharge[iside]=0;
      map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
      if(it==qcluster.end()) continue;
      XYcharge[iside]=TrChargeDBH::GetHead()->GetQ((it->second).SqrtdEdX,JLayer,iside,beta,rigidity);
   }
   charge=TrChargeDBH::GetHead()->GetQ(SqrtdEdX,JLayer,2,beta,rigidity);
   //printf("TrRecHitChargeH: L%d beta=%.2f rig=%.2f dEdX={%.2f,%.2f,%.2f} Q={%.2f,%.2f,%.2f}\n",JLayer,beta0,rigidity0,XYdEdX[0],XYdEdX[1],dEdX,XYcharge[0],XYcharge[1],charge);
}
void TrRecHitChargeH::copy(const TrRecHitChargeH& orig){
   TrRecHitChargeLightH::copy(orig);

   beta=orig.beta;
   rigidity=orig.rigidity;
   for(int iside=0;iside<2;iside++) XYcharge[iside]=orig.XYcharge[iside];
   charge=orig.charge;
}
int TrRecHitChargeH::IsChanged(float beta0,float rigidity0,int Opt,float Z0,bool forceupdate,int imult,float dxdz,float dydz){
   int changed=TrRecHitChargeLightH::IsChanged(Opt,Z0,forceupdate,imult,dxdz,dydz);
   double margin=0.001;
   if(fabs(beta-beta0)>margin&&(beta0>-10)){
      changed=changed|(1<<(2+2*(7+NStrip)));
      if(forceupdate) beta=beta0;
   }
   if(fabs(rigidity-rigidity0)>margin&&(rigidity0>-1e10)){
      changed=changed|(1<<(3+2*(7+NStrip)));
      if(forceupdate) rigidity=rigidity0;
   }
   return changed;
}
bool TrRecHitChargeH::Calculate(float beta0,float rigidity0,int Opt,float Z0,int imult,float dxdz,float dydz){
   if(!phit) {Init(); return false;}

   bool dedx_changed=TrRecHitChargeLightH::Calculate(Opt,Z0,imult,dxdz,dydz);
   double margin=0.001;
   bool ecor_changed=( (fabs(beta-beta0)>margin&&(beta0>-10)) || (fabs(rigidity-rigidity0)>margin&&(rigidity0>-1e10)) );
   bool changed=dedx_changed||ecor_changed;
   if(beta0>-10) beta=beta0;
   if(rigidity0>-1e10) rigidity=rigidity0;

   if(!changed) return false;

   int JLayer=phit->GetLayerJ();
   int EOpt=0;
   if(beta0!=0) EOpt=EOpt|TrChargeDBH::kBeta;
   if(rigidity0!=0) EOpt=EOpt|TrChargeDBH::kRigidity;
   bool val_changed=false;
   for(int iside=0;iside<3;iside++){
      double q0=0;
      if(iside<2){
         map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside);
         if(it==qcluster.end()) q0=0;
         else q0=(it->second).SqrtdEdX;
      }
      else{
         q0=SqrtdEdX;
      }
      double charge0=TrChargeDBH::GetHead()->GetQ(q0,JLayer,iside,beta,rigidity,EOpt);
      margin=0.005;
      if(iside<2){
         if(fabs(charge0-XYcharge[iside])>margin){
            XYcharge[iside]=charge0;
            val_changed=true;
         }
         else{
            XYcharge[iside]=charge0;
         }
      }
      else{
         if(fabs(charge0-charge)>margin){
            charge=charge0;
            val_changed=true;
         }
         else{
            charge=charge0;
         }
      }
   }
   return val_changed;
}
float TrRecHitChargeH::GetQ(float beta0,float rigidity0,int Opt,float Z0,int iside,int imult,float dxdz,float dydz){
   if(!phit) return 0;
   Calculate(beta0,rigidity0,Opt,Z0,imult,dxdz,dydz);
   bool dosmear=TrChargeDBH::IsMC&&(TrTrackChargeH::ConvertChargeOpt(1,Opt)&TrClusterChargeLightH::kSmear);
   if(dosmear) UpdateSmearing();
   bool isunb=(ConvertOpt(Opt,dxdz,dydz)&TrClusterChargeLightH::kUnbias);
   double q0_XY[2]={0,0};
   for(int iside0=0;iside0<2;iside0++){
      if(dosmear){
         float mcsmear0[2];
         for(int ii=0;ii<2;ii++) mcsmear0[ii]=GetMCSmearing(iside0,ii);
         int Ztrue=-1;
         map<int,TrClusterChargeLightH>::iterator it=qcluster.find(iside0);
         if(!(it==qcluster.end())) if(TrChargeDBH::Useqtuning&&((beta0!=0)||(rigidity0!=0))) Ztrue=(it->second).GetTrueZ(imult>=0?imult:(phit?phit->GetResolvedMultiplicity():0));
         q0_XY[iside0]=TrChargeDBH::GetMCSmearing(XYcharge[iside0],mcsmear0,phit?phit->GetLayerJ():0,iside0,isunb,Ztrue);
      }
      else q0_XY[iside0]=XYcharge[iside0];
   }
   if(iside==0||iside==1){
      return q0_XY[iside];
   }
   else{
      if(dosmear){
         int XYCombineOpt0=TrTrackChargeH::ConvertCombineOpt(1,Opt,true);
         double result=TrChargeDBH::XYCombination(q0_XY,phit?phit->GetLayerJ():0,charge,XYCombineOpt0);
         result=TrChargeDBH::GetMCSmearing(result,mcsmear,phit?phit->GetLayerJ():0,iside,isunb);
         return result;
      }
      else return charge;
   }
}
