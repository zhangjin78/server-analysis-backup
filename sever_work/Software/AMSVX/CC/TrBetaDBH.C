#include "TrBetaDBH.h"
#include "stdlib.h"
#include "TrChargeDBH.h"
#include "commonsi.h"

int TrBetaDBH::fUseVersion=12;
TrBetaDBH* TrBetaDBH::fHead=0;
MSplineH* TrBetaDBH::kBeta[NZQ][9][3];

void TrBetaDBH::Init(){
   for(int ilay=0;ilay<9;ilay++){
      for(int iz=0;iz<NZQ;iz++){
         for(int iside=0;iside<3;iside++) kBeta[iz][ilay][iside]=0;
      }
   }
}
void TrBetaDBH::Release(){
   for(int ilay=0;ilay<9;ilay++){
      for(int iz=0;iz<NZQ;iz++){
         for(int iside=0;iside<3;iside++){
            if(kBeta[iz][ilay][iside]) {delete kBeta[iz][ilay][iside]; kBeta[iz][ilay][iside]=0;}
         }
      }
   }
   fHead=0;
}

bool TrBetaDBH::ReadBeta(int JLayer,int iside,char* filename){
   if(JLayer<1||JLayer>9) return false;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return false;

   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int nz=0;
   fscanf(fin,"%d",&nz);
   for(int ii=0;ii<nz;ii++){
      int ZZ;
      int nknot;
      int logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf",&ZZ,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      if(ZZ<1||ZZ>NZQ) return false;
      double xknot[100],yknot[100];
      for(int i2=0;i2<nknot;i2++) fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      if(kBeta[ZZ-1][JLayer-1][iside]) {delete kBeta[ZZ-1][JLayer-1][iside]; kBeta[ZZ-1][JLayer-1][iside]=0;}
      kBeta[ZZ-1][JLayer-1][iside]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
   }
   fclose(fin);
   return true;
}
bool TrBetaDBH::LoadBetaFromDir(char *dirname){
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool loaded=false;
   for(int ilay=0;ilay<9;ilay++){
      for(int iside=0;iside<3;iside++){
         char filename[200];
         strcpy(filename,Form("%s/BetaTable_L%d%s_Algo3.txt",dirname_copy,ilay+1,iside>1?"":(iside==0?"_x":"_y")));
         bool loadbeta=ReadBeta(ilay+1,iside,filename);
         //printf("Load BetaCorrection(kBeta) From %s(%d) %s\n",filename,ilay+1,loadbeta?"successfully":"failed");
         loaded=loaded||loadbeta;
      }
   }
   return loaded;
}
TrBetaDBH* TrBetaDBH::GetHead(){
   if(!fHead){
      fHead=new TrBetaDBH();
      //Loading all the kBeta correction.
      char DatabaseDir1[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/beta",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrBetaDBH::fUseVersion/10):(TrBetaDBH::fUseVersion%10)));
      bool loaded=fHead->LoadBetaFromDir(DatabaseDir1);
      printf("TrBetaDBH:: Load BetaCorrection(kBeta) From %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if(!loaded){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

MSplineH* TrBetaDBH::GetEDependence(double beta,int JLayer,int iside){
   if(JLayer<1||JLayer>9) return 0;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return 0;
   int np=0;
   double xx[NZQ+1],yy[NZQ+1];
   xx[np]=0; yy[np]=0; np++;
   double xpre=0,ypre=0;
   for(int iz=0;iz<NZQ;iz++){
      if((iz+1<=26)&&TrChargeLossDBH::Ignorelist(2,10,iz+1)) continue;
      if(!kBeta[iz][ilay][iside]) continue;

      double qbeta=kBeta[iz][ilay][iside]->Eval(beta);
      xx[np]=iz+1;
      yy[np]=qbeta;
      if( (np>0) && ((qbeta-ypre)*(iz+1-xpre)<=0) ) continue;
      xpre=iz+1;ypre=qbeta;

      np++;
   }
   if(np<2) return 0;
   MSplineH* spl=new MSplineH(np,xx,yy,false,false,MSplineH::kLinear,MSplineH::kLinear,0,0);
   return spl;
}
MSplineH* TrBetaDBH::GetBeta(int JLayer,int iside,int Z){
   if(JLayer<1||JLayer>9) return 0;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return 0;
   if(Z<1||Z>NZQ) return 0;
   return kBeta[Z-1][ilay][iside];
}
int TrBetaDBH::FindRefLRZ(int &Zright,int Zinput,int JLayer,int iside){
   int zleft=0;
   int zright=0;
   for(int Z=Zinput;Z>0;Z--){
      MSplineH* pmap=GetBeta(JLayer,iside,Z);
      if(pmap){zleft=Z; break;}
   }
   for(int Z=Zinput+1;Z<=NZQ;Z++){
      MSplineH* pmap=GetBeta(JLayer,iside,Z);
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
            MSplineH* pmap=GetBeta(JLayer,iside,Z);
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
double TrBetaDBH::GetValue(int JLayer,int iside,int Z,double beta){
   MSplineH* spl=GetBeta(JLayer,iside,Z);
   return spl?spl->Eval(beta):0;
}
TGraph* TrBetaDBH::GetCorrectionGraph(double beta,int JLayer,int iside){
   if(JLayer<1||JLayer>9) return 0;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return 0;

   int np=0;
   double xpre=0,ypre=0;
   double xx[NZQ],yy[NZQ];
   for(int iz=0;iz<NZQ;iz++){
      if((iz+1<=26)&&TrChargeLossDBH::Ignorelist(2,10,iz+1)) continue;
      if(!kBeta[iz][ilay][iside]) continue;

      double qbeta=kBeta[iz][ilay][iside]->Eval(beta);
      xx[np]=qbeta;
      yy[np]=qbeta/(iz+1);
      if(fabs(yy[np]-1)>2.5) continue;
      if( (np>0) && ((qbeta-ypre)*(iz+1-xpre)<=0) ) continue;
      xpre=iz+1;ypre=qbeta;

      np++;
   }
   if(np<1) return 0;
   TGraph* res=new TGraph(np,xx,yy);
   return res;
}
MSplineH* TrBetaDBH::GetCorrection(double beta,int JLayer,int iside){
   TGraph* gr=GetCorrectionGraph(beta,JLayer,iside);
   if(!gr) return 0;
   MSplineH* spl=new MSplineH(gr,false,false,MSplineH::kFlat,MSplineH::kFlat,0,0);
   delete gr;
   return spl;
}

double TrBetaDBH::GetBetaCorrectedValue(double beta,int JLayer,int iside,double charge,bool UseNewAlgo){
   if(charge<=0) return 0;
   if(JLayer<1||JLayer>9) return charge;
   if(iside<0||iside>2) return charge;
   double x0;
   if(!UseNewAlgo){
      MSplineH* spl=GetCorrection(fabs(beta),JLayer,iside);
      if(!spl) return charge;
      x0=spl->Eval(charge);
      delete spl;
      if(x0<=0) x0=1;
      return charge/x0;
   }
   else{
      //TGraph* spl=GetCorrectionGraph(fabs(beta),JLayer,iside);
      //if(!spl) return charge;
      //x0=spl->Eval(charge);
      //delete spl;

      double Qinput=charge;
      int Zinput=int(Qinput);
      int Zlref;
      int Zrref;
      Zlref=FindRefLRZ(Zrref,Zinput,JLayer,iside);
      if(Zrref<=0) return charge;
      double qlref=(Zlref<=0)?0:GetValue(JLayer,iside,Zlref,beta);
      double qrref=(Zrref<=0)?0:GetValue(JLayer,iside,Zrref,beta);
      double slope=(qrref-qlref)/(Zrref-Zlref);
      double logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
      double slplimit=0.7;
      double fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
      int clock=30;
      while( !( (charge>=qlref&&charge<=qrref) && fineslope ) ){
         //printf("TrBeta:Zlref={%d,%.2lf} Zrref={%d,%.2lf} slope={%.2lf,%.2lf} adc=%.2f qin=%.2lf\n",Zlref,qlref,Zrref,qrref,slope,logslope,charge,Qinput);
         if(!fineslope) break;
         if(charge<qlref){
            Zrref=Zlref; qrref=qlref;
            int Zbuff;
            Zlref=FindRefLRZ(Zbuff,Zrref-1,JLayer,iside);
            qlref=(Zlref<=0)?0:GetValue(JLayer,iside,Zlref,beta);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         if(charge>qrref){
            Zlref=Zrref; qlref=qrref;
            int Zbuff;
            Zrref=FindRefLRZ(Zbuff,Zlref,JLayer,iside);
            if(Zrref<=Zlref){  //already the last Z;
               return Zlref+(charge-qlref)/slope;
            }
            qrref=(Zrref<=0)?0:GetValue(JLayer,iside,Zrref,beta);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         clock--;
         if(clock<0) break;
      }
      //printf("TrBeta:Outloop: Zlref={%d,%.2lf} Zrref={%d,%.2lf} slope={%.2lf,%.2lf} adc=%.2f qin=%.2lf\n",Zlref,qlref,Zrref,qrref,slope,logslope,charge,Qinput);
      if(!fineslope) return charge;
      if(charge>=qlref&&charge<=qrref){
         return Zlref-(qlref-charge)/(qlref-qrref)*(Zlref-Zrref);
      }
      else return charge;
   }
}
