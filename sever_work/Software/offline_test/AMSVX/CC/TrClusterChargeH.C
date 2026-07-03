#include "TrClusterChargeH.h"
#include "TrTrackChargeH.h"
#include "TrCluster.h"
#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#else
#include "event.h"
#include "extC.h"
#endif

bool TrClusterChargeLightH::CheckEta=false;
bool TrClusterChargeLightH::SmoothEta=true;
float TrClusterChargeLightH::fSmoothEta=0.1;
float TrClusterChargeLightH::fSmoothEtaUnb=1.0;
//UShort_t TrClusterChargeLightH::ClusterChargeOpt=(TrClusterChargeLightH::kStripWeight|(TrClusterChargeLightH::kOverflow|TrClusterChargeLightH::kAsymEta))|(TrClusterChargeLightH::kLinearCofG);
//int TrClusterChargeH::DefaultOpt=(TrClusterChargeLightH::kStripWeight|(TrClusterChargeLightH::kOverflow|TrClusterChargeLightH::kAsymEta))|(TrClusterChargeLightH::kLinearCofG);
void TrClusterChargeLightH::Init(){
   for(int ii=0;ii<2;ii++) mcsmear[ii]=0;
   cluster=0;
   Ztrk=0;
   ClusterChargeOpt=0;
   eta=0;
   for(int iside=0;iside<2;iside++) angle[iside]=0;
   seedaddress=(0+(20<<10));
   for(int istrip=0;istrip<=NStrip;istrip++) sigadc[istrip]=0;
   Zcomb=0;
   SqrtdEdX=0;
}
void TrClusterChargeLightH::Release(){
   for(int ii=0;ii<2;ii++) mcsmear[ii]=0;
   if(cluster) cluster=0;
   ClusterChargeOpt=0;
   eta=0;
   for(int iside=0;iside<2;iside++) angle[iside]=0;
   seedaddress=(0+(20<<10));
   for(int istrip=0;istrip<=NStrip;istrip++) sigadc[istrip]=0;
   Zcomb=0;
   SqrtdEdX=0;
}

void TrClusterChargeLightH::AssignPointer(TrClusterR* pcluster0){
   if(!pcluster0) return;
   cluster=pcluster0;
}

void TrClusterChargeLightH::UpdateSmearing(){
   if(!cluster) return;
   for(int ii=0;ii<2;ii++){
      if(mcsmear[ii]!=0) continue;
      double rnd[1];
      #ifdef __ROOTSHAREDLIBRARY__
      AMSEventR::GetRandArray(8993306+(cluster->GetLayerJ()*3+cluster->GetSide())+ii*33, (ii==0?2:1),  1, rnd);
      #else
      rnd[0]=rnormx();
      #endif
      mcsmear[ii]=(float)rnd[0];
   }
}

int TrClusterChargeLightH::ConvertOpt(int Opt,float dxdz,float dydz){
   Opt=TrTrackChargeH::ConvertChargeOpt(0,Opt);
   bool isunb=(!(dxdz<-1.99&&dydz<-1.99));
   if(isunb) Opt=(Opt|kUnbias);
   return (Opt&0xFFFFFDFF);
}

int TrClusterChargeLightH::IsChanged(int imult,int Opt,float Z0,bool forceupdate,float dxdz,float dydz){
   int changed=0;
   if(!cluster) return changed;
   Opt=ConvertOpt(Opt,dxdz,dydz);
   bool isunb=(Opt&kUnbias);
   bool debug=false;

   if(imult<0||(cluster->GetSide()!=0)) imult=0;

   int SigOpt=0;
   if(Opt&kOverflow) SigOpt=SigOpt|TrClusterR::kOverflow;
   if(Opt&kAsym)     SigOpt=SigOpt|TrClusterR::kAsym;
   if(Opt&kSimAsym)  SigOpt=SigOpt|TrClusterR::kSimAsym;
   if(Opt&kAsymEta)  SigOpt=SigOpt|TrClusterR::kAsymEta;
   if(Opt&kSimSignal)SigOpt=SigOpt|TrClusterR::kSimSignal;
   int SigOpt0=0;
   if(ClusterChargeOpt&kOverflow) SigOpt0=SigOpt0|TrClusterR::kOverflow;
   if(ClusterChargeOpt&kAsym)     SigOpt0=SigOpt0|TrClusterR::kAsym;
   if(ClusterChargeOpt&kSimAsym)  SigOpt0=SigOpt0|TrClusterR::kSimAsym;
   if(ClusterChargeOpt&kAsymEta)  SigOpt0=SigOpt0|TrClusterR::kAsymEta;
   if(ClusterChargeOpt&kSimSignal)SigOpt0=SigOpt0|TrClusterR::kSimSignal;
   bool SigOpt_changed=( (SigOpt0!=SigOpt) || (sigadc[NStrip]<=0) );
   
   int EtaOpt=0;
   if(Opt&kSimpCofG) EtaOpt=EtaOpt|kSimpCofG;
   if(Opt&kLinearCofG) EtaOpt=EtaOpt|kLinearCofG;
   int EtaOpt0=0;
   if(ClusterChargeOpt&kSimpCofG) EtaOpt0=EtaOpt0|kSimpCofG;
   if(ClusterChargeOpt&kLinearCofG) EtaOpt0=EtaOpt0|kLinearCofG;

   int UnbiasOpt=0;
   if(Opt&kUnbias) UnbiasOpt=UnbiasOpt|kUnbias;
   int UnbiasOpt0=0;
   if(ClusterChargeOpt&kUnbias) UnbiasOpt0=UnbiasOpt0|kUnbias;
   bool UnbiasOpt_changed=(UnbiasOpt0!=UnbiasOpt);

   bool EtaOpt_changed=(EtaOpt0!=EtaOpt)||((!TrChargeDBH::IsMC)&&(UnbiasOpt0!=UnbiasOpt));

   int CombineOpt0=TrTrackChargeH::ConvertCombineOpt(0,ClusterChargeOpt,false);
   int CombineOpt =TrTrackChargeH::ConvertCombineOpt(0,Opt,false);
   bool CombOpt_changed=(CombineOpt0!=CombineOpt);

   bool QOpt_changed=((SigOpt_changed||EtaOpt_changed)||CombOpt_changed);

   int imult0=((seedaddress&0x3C00)>>10);
   int seedchannel0=(seedaddress&0x3FF);
   bool xmult_changed=(cluster->GetSide()==0)&&(imult!=imult0);

   if(forceupdate){
      ClusterChargeOpt=((SigOpt|EtaOpt)|(CombineOpt|UnbiasOpt));
   }

   //check the charge used for strip combination and eta calculation (if Z0<0,use the one stored Zcomb)
   bool Zcomb_changed=false;
   double Zcomb0=(Z0>=0)?Z0:Zcomb;
   if(Z0>=0){
      double margin=0.01;
      if(fabs(Zcomb-Zcomb0)>margin|| ( (int(Zcomb+0.5)) != (int(Zcomb0+0.5)) ) ){
         changed=changed|(1<<(5+NStrip));
         Zcomb_changed=true;
         if(forceupdate) Zcomb=Zcomb0;
      }
   }

   //check angle
   float angle0[2];
   for(int iside=0;iside<2;iside++){
      if(iside==0){
         //if(!isunb) angle0[iside]=cluster->GetDxDz();
         //else       angle0[iside]=dxdz;
         if(dxdz<-1.99) angle0[iside]=cluster->GetDxDz();
         else           angle0[iside]=dxdz;
      }
      else{
         //if(!isunb) angle0[iside]=cluster->GetDyDz();
         //else       angle0[iside]=dydz;
         if(dydz<-1.99) angle0[iside]=cluster->GetDyDz();
         else           angle0[iside]=dydz;
      }
      double margin=0.001;
      if(fabs(angle0[iside]-angle[iside])>margin){
         changed=changed|(1<<(1+iside));
         if(forceupdate) angle[iside]=angle0[iside];
      }
   }

   bool Eta_changed=(EtaOpt|kLinearCofG); //check whether the condition for eta calculation changed(only for ISS data)
   if(Ztrk<=0){
      Eta_changed=Eta_changed&&(xmult_changed||Zcomb_changed);
   }
   else {if(Ztrk<1.5) Eta_changed=(Eta_changed&&xmult_changed);}
   Eta_changed=Eta_changed||EtaOpt_changed;

   int inputchanged=Eta_changed||( changed || (xmult_changed || QOpt_changed) ); //changed=angel && Z0

   //if the input parameters are the same, the charge are already calculated before, then everything should be the same, and return false
   if((!inputchanged)&&(SqrtdEdX>0)) return changed;

   //check seedaddress
   //if(SigOpt_changed||xmult_changed){
      int seedindex,secondindex;
      cluster->GetSeedSecondIndex(seedindex,secondindex,SigOpt);
      int seedaddress0=(cluster->GetAddress(seedindex))+(imult<<10);
      if(seedaddress0!=seedaddress){
         changed=changed|(1<<3);
         if(forceupdate) seedaddress=seedaddress0;
      }
   //}

   //check signal
   double totsignal=0;
   if(SigOpt_changed){
      for (int ii=0; ii<cluster->GetNelem(); ii++) {
         double signal = cluster->GetSignal(ii,SigOpt);
         if (signal>0){
            if(TrChargeDBH::IsMC) totsignal += signal;
            else{
               if(abs(ii-seedindex)<=((NStrip-1)/2)) totsignal += signal;
            }
         }
      }
      double totz2 = 0;
      if(SigOpt&TrClusterR::kSimSignal) totz2 = pow(TrSimSignalDB::GetHead()->GetValue(sqrt(totsignal),cluster->GetTkId(),(int((seedaddress0&0x3FF)/64)),true),2);
      float sigadc0[NStrip+1];
      for(int istrip=0;istrip<=NStrip;istrip++){
         if(istrip==NStrip) sigadc0[istrip]=totsignal;
         else{
            int seed0=(NStrip-1)/2;
            int iindex=seedindex+istrip-seed0;
            sigadc0[istrip]=(iindex>=0&&iindex<cluster->GetNelem())?cluster->GetSignal(iindex,SigOpt):0;
         }
         if(sigadc0[istrip]<0) sigadc0[istrip]=0;
         if(SigOpt&TrClusterR::kSimSignal){
            sigadc0[istrip]=totsignal>0?(totz2/totsignal*sigadc0[istrip]):0;
         }
         double margin=(SigOpt&TrClusterR::kSimSignal)?(totz2*0.001):1;
         if(debug) printf("TrClusterChargeLightH:: sig%d={%.2f,%.2f} margin=%.2f\n",istrip,sigadc[istrip],sigadc0[istrip],margin);
         if(fabs(sigadc0[istrip]-sigadc[istrip])>margin){
            changed=changed|(1<<(4+istrip));
            if(forceupdate) sigadc[istrip]=sigadc0[istrip];
         }
      }
   }

   //check the strip combination method
   if( CombOpt_changed ){
      changed=changed|(1<<(6+NStrip));
   }

   //check position (or eta)
   if( CheckEta||Eta_changed ){
      int xmult=(cluster->GetSide()==0)?imult:0;
      float eta0=0;
      if(EtaOpt&kLinearCofG){
         double Qref=Zcomb0;
         if(!isunb){ //not unbiased hit
            if(Ztrk>0) Qref=Ztrk;
            else if(cluster->GetQtrk()>0) {Qref=cluster->GetQtrk(); SetQtrk(Qref);}
            else{
               //use some rough charge estimator to estimate the initial charge;
               int nsol;
               double xsol[5];
               TrChargeDBH::GetHead()->fGainTot[cluster->GetSide()]->EvalInverseMultiple(totsignal,nsol,xsol);
               if(nsol>=1) Qref=sqrt(xsol[0]);
            }
         }
         else if(Qref<=0){  //unbiased hit, do not use GetQtrk, use the cluster charge itself or the giving charge.
            //use some rough charge estimator to estimate the initial charge;
            int nsol;
            double xsol[5];
            TrChargeDBH::GetHead()->fGainTot[cluster->GetSide()]->EvalInverseMultiple(totsignal,nsol,xsol);
            if(nsol>=1) Qref=sqrt(xsol[0]);
         }

         int Zref=(int)(Qref+0.5);
         if(Zref<1) Zref=1;
         if(!SmoothEta){
            eta0=cluster->GetLinearCofG(TrClusterR::DefaultUsedStrips,xmult,TrClusterR::DefaultBestResidualOpt,Zref,angle0[0],angle0[1]);
         }
         else{
            //int Zcofg[3];
            //double cofg3[3];
            //Zcofg[0]=Zref-1;
            //Zcofg[1]=Zref;
            //Zcofg[2]=Zref+1;
            //for(int ii=0;ii<3;ii++){
            //   if(Zcofg[ii]>0){
            //      cofg3[ii]=cluster->GetLinearCofG(TrClusterR::DefaultUsedStrips,xmult,TrClusterR::DefaultBestResidualOpt,Zcofg[ii],angle0[0],angle0[1]);
            //   }
            //}
            //if(Zcofg[1]<1) cofg3[1]=cofg3[2];
            //if(Zcofg[0]<1) cofg3[0]=cofg3[1];
            ////smooth the cofg with Z (Qref<Zcofg[0],cofg=cofg3[0]; Qref>Zcofg[2],cofg=cofg3[2])
            //if(Qref<=Zcofg[0]){
            //   eta0=cofg3[0];
            //}
            //else if(Qref>Zcofg[0]&&Qref<=Zcofg[1]){
            //   double xx=(Qref-Zcofg[0])/(Zcofg[1]-Zcofg[0]);
            //   double yy1 = 1./(exp(-(0-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
            //   double yy2 = 1./(exp(-(1-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
            //   double yy  = 1./(exp(-(xx-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
            //   double cof_frac=(yy-yy1)/(yy2-yy1);
            //   eta0=cof_frac*cofg3[1]+(1-cof_frac)*cofg3[0];
            //}
            //else if(Qref>Zcofg[1]&&Qref<=Zcofg[2]){
            //   double xx=(Qref-Zcofg[1])/(Zcofg[2]-Zcofg[1]);
            //   double yy1 = 1./(exp(-(0-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
            //   double yy2 = 1./(exp(-(1-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
            //   double yy  = 1./(exp(-(xx-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
            //   double cof_frac=(yy-yy1)/(yy2-yy1);
            //   eta0=cof_frac*cofg3[2]+(1-cof_frac)*cofg3[1];
            //}
            //else if(Qref>Zcofg[2]){
            //   eta0=cofg3[2];
            //}

            double cofg2[2]={0,0};
            int Zcofg[2]={(int)Qref,((int)Qref)+1};
            if(Zcofg[0]<1){
               eta0=cluster->GetLinearCofG(TrClusterR::DefaultUsedStrips,xmult,TrClusterR::DefaultBestResidualOpt,1,angle0[0],angle0[1]);
            }
            else{
               cofg2[0]=cluster->GetLinearCofG(TrClusterR::DefaultUsedStrips,xmult,TrClusterR::DefaultBestResidualOpt,Zcofg[0],angle0[0],angle0[1]);
               cofg2[1]=cluster->GetLinearCofG(TrClusterR::DefaultUsedStrips,xmult,TrClusterR::DefaultBestResidualOpt,Zcofg[1],angle0[0],angle0[1]);
               double xx=(Qref-Zcofg[0])/(Zcofg[1]-Zcofg[0]);
               double yy1 = 1./(exp(-(0-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
               double yy2 = 1./(exp(-(1-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
               double yy  = 1./(exp(-(xx-0.5)/(isunb?fSmoothEtaUnb:fSmoothEta))+1);
               double cof_frac=(yy-yy1)/(yy2-yy1);
               eta0=cof_frac*cofg2[1]+(1-cof_frac)*cofg2[0];
            }

            //if(debug) printf("CofG %.2f(%.2f): %.2f(%d) %.2f(%d) %.2f(%d)\n",eta0,Qref,cofg3[0],Zcofg[0],cofg3[1],Zcofg[1],cofg3[2],Zcofg[2]);
         }
      }
      else if(EtaOpt&kSimpCofG){
         eta0=cluster->GetCofG(2,TrClusterR::kSimAsym);
      }
      double margin=0.001;
      if(fabs(eta0-eta)>margin){
         changed=changed|(1<<0);
         if(forceupdate) eta=eta0;
      }
   }

   return changed;
}
bool TrClusterChargeLightH::Calculate(int imult,int Opt,float Z0,float dxdz,float dydz){
   if(!cluster) {Init(); return false;}
   Opt=ConvertOpt(Opt,dxdz,dydz);
   bool debug=false;

   double SqrtdEdX0=SqrtdEdX;
   double Zx=Z0;
   int nloop=2;
   for(int iloop=0;iloop<nloop;iloop++){
      int changed=IsChanged(imult,Opt,Zx,true,dxdz,dydz);
      if(changed==0) break;

      int CombineOpt=TrTrackChargeH::ConvertCombineOpt(0,ClusterChargeOpt,true);
      int imult0=((seedaddress&0x3C00)>>10);
      int seedchannel=(seedaddress&0x3FF);
      if( (cluster->GetSide()==0&&(seedchannel<640||seedchannel>1023)) || (cluster->GetSide()==1&&(seedchannel<0||seedchannel>639)) ){
         printf("SeedAddr Error: is%d seedaddr=%4d\n",cluster->GetSide(),seedchannel);
      }

      SqrtdEdX=TrChargeDBH::GetHead()->GetSqrtdEdX(cluster->GetTkId(),cluster->GetSide(),sigadc,eta,angle,seedchannel,imult0,Zcomb,NCalib,CombineOpt);
      if(Z0<=0&&SqrtdEdX>0) Zx=SqrtdEdX;
      if(Z0>0) break;
   }
   //printf("ClusterCharge L%d(tkid=%+4d) s%d finished, Z0=%.2f qold=%.2f qnew=%.2f\n",cluster->GetLayerJ(),cluster->GetTkId(),cluster->GetSide(),Z0,SqrtdEdX0,SqrtdEdX);

   if(debug){
      for(int is=0;is<NSQ;is++){
         int iside=cluster->GetSide();
         printf("TkId=%+4d is%d S%d sig=%6.1f eta=%.2f theta={%.2f,%.2f} addr={%d,%d} kloss0=%.2f kgainch0=%.2f kgain0=%.2f ktheta0=%.2f ktemp0=%.2f kloss1=%.2f kgainch1=%.2f kgain1=%.2f ktheta1=%.2f ktemp1=%.2f Zcomb=%.2f\n",cluster->GetTkId(),iside,is,TrChargeDBH::GetHead()->charge_kasymm[iside][is],TrChargeDBH::GetHead()->eta_kasymm[iside][is],TrChargeDBH::GetHead()->theta_kasymm[iside][is][0],TrChargeDBH::GetHead()->theta_kasymm[iside][is][1],imult,TrChargeDBH::GetHead()->addr_kinit[iside][is],TrChargeDBH::GetHead()->charge_kloss[iside][is][0],TrChargeDBH::GetHead()->charge_kgainch[iside][is][0],TrChargeDBH::GetHead()->charge_kgain[iside][is][0],TrChargeDBH::GetHead()->charge_ktheta[iside][is][0],TrChargeDBH::GetHead()->charge_ktemp[iside][is][0],TrChargeDBH::GetHead()->charge_kloss[iside][is][1],TrChargeDBH::GetHead()->charge_kgainch[iside][is][1],TrChargeDBH::GetHead()->charge_kgain[iside][is][1],TrChargeDBH::GetHead()->charge_ktheta[iside][is][1],TrChargeDBH::GetHead()->charge_ktemp[iside][is][1],Zcomb);
      }
   }

   double margin=0.005;
   if(fabs(SqrtdEdX0-SqrtdEdX)>margin) return true;
   else return false;
}

TrClusterChargeLightH::TrClusterChargeLightH(TrClusterR* cluster0,int imult,int Opt,float Z0,float dxdz,float dydz){
   Init();
   if(!cluster0) return;
   AssignPointer(cluster0);
   Calculate(imult,Opt,Z0,dxdz,dydz);
   UpdateSmearing();
}
void TrClusterChargeLightH::copy(const TrClusterChargeLightH& orig){
   cluster=orig.cluster;
   for(int ii=0;ii<2;ii++) mcsmear[ii]=orig.mcsmear[ii];
   ClusterChargeOpt=orig.ClusterChargeOpt;
   eta=orig.eta;
   for(int iside=0;iside<2;iside++) angle[iside]=orig.angle[iside];
   seedaddress=orig.seedaddress;
   for(int is=0;is<=NStrip;is++) sigadc[is]=orig.sigadc[is];
   Zcomb=orig.Zcomb;
   SqrtdEdX=orig.SqrtdEdX;
}

int TrClusterChargeLightH::GetTrueZ(int imult){
   int Z0=-1;
   double M0=0;
   if(!TrChargeDBH::IsMC) return Z0;
   if(!cluster) return Z0;
   #ifdef __ROOTSHAREDLIBRARY__
   if(!(AMSEventR::Head())) return Z0;
   if(cluster->GetSide()!=0) imult=0;
   double x0=cluster->GetGCoord(imult);
   double dmin = 1000;
   TrMCClusterR *mc = 0;
   for (unsigned int i = 0; i < AMSEventR::Head()->NTrMCCluster(); i++) {
      TrMCClusterR *m = AMSEventR::Head()->pTrMCCluster(i);
      if ( (!m) || (m->GetTkId() != cluster->GetTkId()) ) continue;
      double d = x0-(cluster->GetSide()==0?m->GetXgl().x():m->GetXgl().y());
      if (TMath::Abs(d) < TMath::Abs(dmin)) {
        mc   = m;
        dmin = d;
      }
   }
   if(!mc) return Z0;
   mc->GetChargeAndMass(Z0,M0);
   #endif
   return Z0;
}

float TrClusterChargeLightH::GetSqrtdEdX(int imult,int Opt,float Z0,float dxdz,float dydz){
   Calculate(imult,Opt,Z0,dxdz,dydz);
   bool dosmear=(TrTrackChargeH::ConvertChargeOpt(0,Opt)&TrClusterChargeLightH::kSmear)&&TrChargeDBH::IsMC;
   if(dosmear) UpdateSmearing();
   bool isunb=(ConvertOpt(Opt,dxdz,dydz)&TrClusterChargeLightH::kUnbias);
   int Ztrue=-1;
   //if(dosmear&&TrChargeDBH::Useqtuning) Ztrue=GetTrueZ(imult);
   if(dosmear) return TrChargeDBH::GetMCSmearing(SqrtdEdX,mcsmear,cluster?cluster->GetLayerJ():0,cluster?cluster->GetSide():0,isunb,Ztrue);
   else return SqrtdEdX;
}
float TrClusterChargeLightH::GetdEdX(int imult,int Opt,bool IsMIP,float Z0,float dxdz,float dydz){
   float SqrtdEdX0=GetSqrtdEdX(imult,Opt,Z0,dxdz,dydz);
   if(IsMIP) return pow(SqrtdEdX0,2);
   else      return pow(SqrtdEdX0,2)*0.1; //unit in MeV,100 keV per MIP
}
float TrClusterChargeLightH::GetEdep(int imult,int Opt,bool IsMIP,float Z0,float dxdz,float dydz){
   double dedx=GetdEdX(imult,Opt,Z0,dxdz,dydz);
   double costheta=1/sqrt(1+angle[0]*angle[0]+angle[1]*angle[1]);
   if(IsMIP) return dedx/costheta;
   else      return dedx/costheta*0.1;  //unit in MeV,100 keV per MIP
}

float TrClusterChargeLightH::GetQ0(int imult,float beta,float rigidity,int Opt,int EOpt,float Z0,float dxdz,float dydz){
   double q0=GetSqrtdEdX(imult,Opt,Z0,dxdz,dydz);
   int JLayer=cluster?cluster->GetLayerJ():0;
   return TrChargeDBH::GetHead()->GetQ(q0,JLayer,cluster?cluster->GetSide():2,beta,rigidity,EOpt);
}

/////////////////////////
//TrClusterCharge
////////////////////////

void TrClusterChargeH::Init(){
   TrClusterChargeLightH::Init();
   beta=0;
   rigidity=0;
   charge=0;
}
void TrClusterChargeH::Release(){
   beta=0;
   rigidity=0;
   charge=0;
}

TrClusterChargeH::TrClusterChargeH(TrClusterR* cluster0,int imult,float beta0,float rigidity0,int Opt,float Z0,float dxdz,float dydz) {
   if(!cluster0) return;
   AssignPointer(cluster0);
   TrClusterChargeLightH::Calculate(imult,Opt,Z0,dxdz,dydz);
   UpdateSmearing();

   int JLayer=cluster0?cluster0->GetLayerJ():0;
   if(beta0>-10) beta=beta0;
   if(rigidity0>-1e10) rigidity=rigidity0;
   int EOpt=0;
   if(beta0!=0) EOpt=EOpt|TrChargeDBH::kBeta;
   if(rigidity0!=0) EOpt=EOpt|TrChargeDBH::kRigidity;
   double q0=SqrtdEdX;
   charge=TrChargeDBH::GetHead()->GetQ(q0,JLayer,cluster0->GetSide(),beta,rigidity,EOpt);
}
void TrClusterChargeH::copy(const TrClusterChargeH& orig){
   TrClusterChargeLightH::copy(orig);
   beta=orig.beta;
   rigidity=orig.rigidity;
   charge=orig.charge;
}
int TrClusterChargeH::IsChanged(int imult,float beta0,float rigidity0,int Opt,float Z0,bool forceupdate,float dxdz,float dydz){
   int changed=TrClusterChargeLightH::IsChanged(imult,Opt,Z0,forceupdate,dxdz,dydz);
   double margin=0.001;
   if(fabs(beta-beta0)>margin&&(beta0>-10)){
      changed=changed|(1<<(7+NStrip));
      if(forceupdate) beta=beta0;
   }
   if(fabs(rigidity-rigidity0)>margin&&(rigidity0>-1e10)){
      changed=changed|(1<<(8+NStrip));
      if(forceupdate) rigidity=rigidity0;
   }
   return changed;
}
bool TrClusterChargeH::Calculate(int imult,float beta0,float rigidity0,int Opt,float Z0,float dxdz,float dydz){
   if(!cluster) {Init(); return false;}
   bool dedx_changed=TrClusterChargeLightH::Calculate(imult,Opt,Z0,dxdz,dydz);
   double margin=0.001;
   bool ecor_changed=( (fabs(beta-beta0)>margin&&(beta0>-10)) || (fabs(rigidity-rigidity0)>margin&&(rigidity0>-1e10)) );
   bool changed=dedx_changed||ecor_changed;
   if(beta0>-10) beta=beta0;
   if(rigidity0>-1e10) rigidity=rigidity0;

   if(!changed) return false;

   int JLayer=cluster?cluster->GetLayerJ():0;
   int EOpt=0;
   if(beta0!=0) EOpt=EOpt|TrChargeDBH::kBeta;
   if(rigidity0!=0) EOpt=EOpt|TrChargeDBH::kRigidity;
   double q0=SqrtdEdX;
   double charge0=TrChargeDBH::GetHead()->GetQ(q0,JLayer,cluster->GetSide(),beta,rigidity,EOpt);
   margin=0.001;
   if(fabs(charge-charge0)>margin){
      charge=charge0;
      return true;
   }
   else{
      charge=charge0;
      return false;
   }
}
float TrClusterChargeH::GetQ(int imult,float beta0,float rigidity0,int Opt,float Z0,float dxdz,float dydz){
   Calculate(imult,beta0,rigidity0,Opt,Z0,dxdz,dydz);
   bool dosmear=(TrTrackChargeH::ConvertChargeOpt(0,Opt)&TrClusterChargeLightH::kSmear)&&TrChargeDBH::IsMC;
   if(dosmear) UpdateSmearing();
   bool isunb=(ConvertOpt(Opt,dxdz,dydz)&TrClusterChargeLightH::kUnbias);
   int Ztrue=-1;
   if((dosmear&&TrChargeDBH::Useqtuning)&&((beta0!=0)||(rigidity0!=0))) Ztrue=GetTrueZ(imult);
   if(dosmear) return TrChargeDBH::GetMCSmearing(charge,mcsmear,cluster?cluster->GetLayerJ():0,cluster?cluster->GetSide():0,isunb,Ztrue);
   else return charge;
}
