#include "TrThetaDBH.h"
#include "stdlib.h"
#include "TrChargeDBH.h"
#include "commonsi.h"

int TrThetaDBH::fUseVersion=12;
TrThetaDBH* TrThetaDBH::fHead=0;
MSplineH* TrThetaDBH::kTheta[NCalib][2][NSQ+1][NZQ];

void TrThetaDBH::Init(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int iz=0;iz<NZQ;iz++){
            for(int icalib=0;icalib<NCalib;icalib++){
               kTheta[icalib][iside][istrip][iz]=0;
            }
         }
      }
   }
}
void TrThetaDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int iz=0;iz<NZQ;iz++){
            for(int icalib=0;icalib<NCalib;icalib++){
               if(kTheta[icalib][iside][istrip][iz]) {delete kTheta[icalib][iside][istrip][iz];kTheta[icalib][iside][istrip][iz]=0;}
            }
         }
      }
   }
   fHead=0;
}

bool TrThetaDBH::ReadTheta(int iside,int istrip,char* filename,int icalib){
   if(icalib<0||icalib>=NCalib) return false;
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ntheta=0;
   fscanf(fin,"%d",&ntheta);
   for(int ii=0;ii<ntheta;ii++){
      int ZZ;
      int nknot;
      int logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf",&ZZ,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      if(ZZ<1||ZZ>NZQ) return false;
      double xknot[100],yknot[100];
      for(int i2=0;i2<nknot;i2++) fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      if((ZZ<=26)&&TrChargeLossDBH::Ignorelist(iside,istrip,ZZ)) continue;
      if(kTheta[icalib][iside][istrip][ZZ-1]) delete kTheta[icalib][iside][istrip][ZZ-1];
      kTheta[icalib][iside][istrip][ZZ-1]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      MSplineH* spl=kTheta[icalib][iside][istrip][ZZ-1];
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
   }
   fclose(fin);
   return ntheta;
}
bool TrThetaDBH::LoadThetaFromDir(char *dirname,int icalib){
   if(icalib<0||icalib>=NCalib){
      printf("kTheta Calibration Type Wrong(%d),Didn't Load ThetaCorrection!\n",icalib);
      return false;
   }
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool loaded=false;
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         char filename[200];
         strcpy(filename,Form("%s/ThetaTable_%s_S%d_Algo3.txt",dirname_copy,iside==0?"x":"y",istrip));
         bool loadtheta=ReadTheta(iside,istrip,filename,icalib);
         //printf("Load ThetaCorrection(kTheta%d) From %s(%d,%d) %s\n",icalib,filename,iside,istrip,loadtheta?"successfully":"failed");
         loaded=loaded||loadtheta;
      }
   }
   return loaded;
}
TrThetaDBH* TrThetaDBH::GetHead(){
   if(!fHead){
      fHead=new TrThetaDBH();
      bool loaded=true;
      //Loading all gainlines for all the kTheta correction: kTheta0,kTheta1,...
      char DatabaseDir1[200]="";
      char DatabaseDir2[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/angle1",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrThetaDBH::fUseVersion/10):(TrThetaDBH::fUseVersion%10)));//the directory has been changed to v6.00
      strcpy(DatabaseDir2,Form("%sChargeCalibHDB/%sv%d/angle2",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrThetaDBH::fUseVersion/10):(TrThetaDBH::fUseVersion%10)));
      loaded=fHead->LoadThetaFromDir(DatabaseDir1,0)&&loaded;
      loaded=fHead->LoadThetaFromDir(DatabaseDir2,1)&&loaded;
      //fHead->LoadThetaFromDir(DatabaseDir2,1);
      printf("TrThetaDBH:: Read AngleTable from %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if(!loaded){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

MSplineH* TrThetaDBH::GetTheta(int iside,int istrip,int Z,int icalib){
   if(icalib<0||icalib>=NCalib) return 0;
   if(iside<0||iside>=2) return 0;
   if(istrip<0||istrip>NSQ) return 0;
   if(Z<1||Z>NZQ) return 0;
   return kTheta[icalib][iside][istrip][Z-1];
}
int TrThetaDBH::FindRefLRZ(int &Zright,int Zinput,int iside,int istrip,int icalib){
   int zleft=0;
   int zright=0;
   for(int Z=Zinput;Z>0;Z--){
      MSplineH* pmap=GetTheta(iside,istrip,Z,icalib);
      if(pmap){zleft=Z; break;}
   }
   for(int Z=Zinput+1;Z<=NZQ;Z++){
      MSplineH* pmap=GetTheta(iside,istrip,Z,icalib);
      if(pmap){zright=Z; break;}
   }
   if(zright>0){
      Zright=zright;
      return zleft;
   }
   else{
      if(zleft<=0){
         Zright=zright;
         return zleft;
      }
      else{
         zright=zleft;
         zleft=0;
         for(int Z=zright-1;Z>0;Z--){
            MSplineH* pmap=GetTheta(iside,istrip,Z,icalib);
            if(pmap){zleft=Z; break;}
         }
         if(zleft<=0){
            Zright=0;
            return zleft;
         }
         else{
            Zright=zright;
            return zleft;
         }
      }
   }
}
double TrThetaDBH::GetValue(int iside,int istrip,int Z,int icalib,double theta){
   MSplineH* spl=GetTheta(iside,istrip,Z,icalib);
   return spl?spl->Eval(theta):0;
}
TGraph* TrThetaDBH::GetCorrectionGraph(int iside,int istrip,double theta,int icalib){
   if(icalib<0||icalib>=NCalib) return 0;
   if(iside<0||iside>=2) return 0;
   if(istrip<0||istrip>NSQ) return 0;

   int np=0;
   double xx[NZQ],yy[NZQ];
   for(int iz=0;iz<NZQ;iz++){
      if((iz+1<=26)&&TrChargeLossDBH::Ignorelist(iside,istrip,iz+1)) continue;
      if(!kTheta[icalib][iside][istrip][iz]) continue;

      double qtheta0=kTheta[icalib][iside][istrip][iz]->Eval(0);
      double qtheta1=kTheta[icalib][iside][istrip][iz]->Eval(theta);
      if(qtheta0<=0||qtheta1<=0) continue;
      xx[np]=iz+1;
      yy[np]=qtheta1/(iz+1);
      //yy[np]=qtheta1/qtheta0;

      np++;
   }
   if(np<2) return 0;
   TGraph* res=new TGraph(np,xx,yy);
   return res;
}
MSplineH* TrThetaDBH::GetCorrection(int iside,int istrip,double theta,int icalib){
   TGraph* gr=GetCorrectionGraph(iside,istrip,theta,icalib);
   if(!gr) return 0;
   MSplineH* spl=new MSplineH(gr,false,false,MSplineH::kFlat,MSplineH::kFlat,0,0);
   delete gr;
   return spl;
}

int TrThetaDBH::CountTheta(int iside,int istrip,int icalib){
   int ntheta=0;
   for(int iside0=0;iside0<2;iside0++){
      if(iside>=0&&iside!=iside0) continue;
      for(int istrip0=0;istrip0<=NSQ;istrip0++){
         if(istrip>=0&&istrip!=istrip0) continue;
         for(int iz=0;iz<NZQ;iz++){
            for(int icalib0=0;icalib0<NCalib;icalib0++){
               if(icalib>=0&&icalib!=icalib0) continue;
               if(kTheta[icalib][iside][istrip][iz]) ntheta++;
            }
         }
      }
   }
   return ntheta;
}

double TrThetaDBH::GetThetaCorrectedValue(int iside,int istrip,double theta,double charge,double Z0,int icalib,bool UseNewAlgo){
   if(icalib<0||icalib>=NCalib) return charge;
   if(charge<=0) return 0;
   if(iside<0||iside>=2) return charge;
   if(istrip<0||istrip>NSQ) return charge;
   if(fabs(theta)>=2) return charge;
   double x0;
   if(!UseNewAlgo){
      MSplineH* spl=GetCorrection(iside,istrip,theta,icalib);
      if(!spl) return charge;
      x0=spl->Eval(Z0>0?Z0:charge);
      delete spl;
      if(x0<=0) x0=1;
      return charge/x0;
   }
   else{
      //TGraph* spl=GetCorrectionGraph(iside,istrip,theta,icalib);
      //if(!spl) return charge;
      //x0=spl->Eval(Z0>0?Z0:charge);
      //delete spl;

      double Qinput=Z0>0?Z0:charge;
      int Zinput=int(Qinput);
      int Zlref;
      int Zrref;
      Zlref=FindRefLRZ(Zrref,Zinput,iside,istrip,icalib);
      if(Zrref<=0) return charge;
      double qlref=(Zlref<=0)?0:GetValue(iside,istrip,Zlref,icalib,theta);
      double qrref=(Zrref<=0)?0:GetValue(iside,istrip,Zrref,icalib,theta);
      //test the new approach
      if(true){
         double ratio1=(qlref<=0||Zlref<=0)?(qrref/Zrref):(qlref/Zlref);
         double ratio2=qrref/Zrref;
         if(ratio2<=0) return charge;
         if(Qinput<Zlref) return charge/ratio1;
         else if(Qinput>Zrref) return charge/ratio2;
         else return charge/((Qinput-Zlref)/(Zrref-Zlref)*ratio2+(Zrref-Qinput)/(Zrref-Zlref)*ratio1);
      }
      double slope=(qrref-qlref)/(Zrref-Zlref);
      double logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
      double slplimit=0.7;
      double fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
      int clock=30;
      while( !( (charge>=qlref&&charge<=qrref) && fineslope ) ){
         if(!fineslope) break;
         if(charge<qlref){
            Zrref=Zlref; qrref=qlref;
            int Zbuff;
            Zlref=FindRefLRZ(Zbuff,Zrref-1,iside,istrip,icalib);
            qlref=(Zlref<=0)?0:GetValue(iside,istrip,Zlref,icalib,theta);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         if(charge>qrref){
            Zlref=Zrref; qlref=qrref;
            int Zbuff;
            Zrref=FindRefLRZ(Zbuff,Zlref,iside,istrip,icalib);
            if(Zrref<=Zlref){  //already the last Z;
               return Zlref+(charge-qlref)/slope;
            }
            qrref=(Zrref<=0)?0:GetValue(iside,istrip,Zrref,icalib,theta);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         clock--;
         if(clock<0) break;
      }
      if(!fineslope) return charge;
      if(charge>=qlref&&charge<=qrref){
         return Zlref-(qlref-charge)/(qlref-qrref)*(Zlref-Zrref);
      }
      else return charge;
   }
}
