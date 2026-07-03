#include "TrRigidityDBH.h"
#include "stdlib.h"
#include "TrChargeDBH.h"
#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#else
#include "event.h"
#endif
#include "commonsi.h"


int TrRigidityDBH::fUseVersion=12;
TrRigidityDBH* TrRigidityDBH::fHead=0;
MSplineH* TrRigidityDBH::kRigidity[NZQ][9][3];
MSplineH* TrRigidityDBH::kRigidity_SA[NZQ][9][3];

void TrRigidityDBH::Init(){
   for(int ilay=0;ilay<9;ilay++){
      for(int iz=0;iz<NZQ;iz++){
         for(int iside=0;iside<3;iside++){
            kRigidity[iz][ilay][iside]=0;
            kRigidity_SA[iz][ilay][iside]=0;
         }
      }
   }
}
void TrRigidityDBH::Release(){
   for(int ilay=0;ilay<9;ilay++){
      for(int iz=0;iz<NZQ;iz++){
         for(int iside=0;iside<3;iside++){
            if(kRigidity[iz][ilay][iside]) {delete kRigidity[iz][ilay][iside]; kRigidity[iz][ilay][iside]=0;}
            if(kRigidity_SA[iz][ilay][iside]) {delete kRigidity_SA[iz][ilay][iside]; kRigidity_SA[iz][ilay][iside]=0;}
         }
      }
   }
   fHead=0;
}

bool TrRigidityDBH::ReadRigidity(int JLayer,int iside,char* filename,bool IsSA){
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
      if(IsSA){
         if(kRigidity_SA[ZZ-1][JLayer-1][iside]) {delete kRigidity_SA[ZZ-1][JLayer-1][iside]; kRigidity_SA[ZZ-1][JLayer-1][iside]=0;}
         kRigidity_SA[ZZ-1][JLayer-1][iside]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      }
      else{
         if(kRigidity[ZZ-1][JLayer-1][iside]) {delete kRigidity[ZZ-1][JLayer-1][iside]; kRigidity[ZZ-1][JLayer-1][iside]=0;}
         kRigidity[ZZ-1][JLayer-1][iside]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      }
   }
   fclose(fin);
   return true;
}
bool TrRigidityDBH::LoadRigidityFromDir(char *dirname){
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool ispass7=true;
   #ifdef __ROOTSHAREDLIBRARY__
   if(AMSEventR::Head()){
      if(AMSEventR::Head()->Version()>=1130) ispass7=true;
      else ispass7=false;
   }
   #else
   if(AMSEvent::gethead()){
      if(AMSCommonsI::getbuildno()>=1130) ispass7=true;
      else ispass7=false;
   }
   #endif
   if(TrChargeDBH::IsMC) ispass7=false;
   bool loaded=false;
   for(int ilay=0;ilay<9;ilay++){
      for(int iside=0;iside<3;iside++){
         char filename[200];
         char filename_SA[200];
         strcpy(filename,Form("%s/RigidityTable_L%d%s_Algo3%s.txt",dirname_copy,ilay+1,iside>1?"":(iside==0?"_x":"_y"),ispass7?"_pass7":""));
         strcpy(filename_SA,Form("%s/RigidityTable_SA_L%d%s_Algo3%s.txt",dirname_copy,ilay+1,iside>1?"":(iside==0?"_x":"_y"),ispass7?"_pass7":""));
         bool loadrig=ReadRigidity(ilay+1,iside,filename,false);
         //printf("Load RigidityCorrection(kRigidity) From %s(%d,s%d) %s\n",filename,ilay+1,iside,loadrig?"successfully":"failed");
         bool loadrig_SA=ReadRigidity(ilay+1,iside,filename_SA,true);
         //printf("Load RigiditySACorrection(kRigidity_SA) From %s(%d,s%d) %s\n",filename_SA,ilay+1,iside,loadrig_SA?"successfully":"failed");
         loaded=loaded||(loadrig||loadrig_SA);
      }
   }
   return loaded;
}
TrRigidityDBH* TrRigidityDBH::GetHead(){
   if(!fHead){
      fHead=new TrRigidityDBH();
      //Loading all the kRigidity correction.
      char DatabaseDir1[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/rigidity",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrRigidityDBH::fUseVersion/10):(TrRigidityDBH::fUseVersion%10)));//the directory has been changed to v6.00
      bool loaded=fHead->LoadRigidityFromDir(DatabaseDir1);
      printf("TrRigidityDBH:: Load RigidityCorrection(kRigidity) From %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if(!loaded){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

MSplineH* TrRigidityDBH::GetEDependence(double rigidity,int JLayer,int iside,bool IsSA){
   if(JLayer<1||JLayer>9) return 0;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return 0;
   int np=0;
   double xx[NZQ+1],yy[NZQ+1];
   xx[np]=0; yy[np]=0; np++;
   double xpre=0,ypre=0;
   for(int iz=0;iz<NZQ;iz++){
      if((iz+1<=26)&&TrChargeLossDBH::Ignorelist(2,10,iz+1)) continue;
      MSplineH* spl0=(IsSA)?(kRigidity_SA[iz][ilay][iside]):(kRigidity[iz][ilay][iside]);
      if(!spl0) continue;

      double qrig=spl0->Eval(rigidity);
      xx[np]=iz+1;
      yy[np]=qrig;
      if( (np>0) && ((qrig-ypre)*(iz+1-xpre)<=0) ) continue;
      xpre=iz+1;ypre=qrig;

      np++;
   }
   if(np<2) return 0;
   MSplineH* spl=new MSplineH(np,xx,yy,false,false,MSplineH::kLinear,MSplineH::kLinear,0,0);
   return spl;
}
MSplineH* TrRigidityDBH::GetRigidity(int JLayer,int iside,bool IsSA,int Z){
   if(JLayer<1||JLayer>9) return 0;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return 0;
   if(Z<1||Z>NZQ) return 0;
   return (IsSA)?(kRigidity_SA[Z-1][ilay][iside]):(kRigidity[Z-1][ilay][iside]);
}
int TrRigidityDBH::FindRefLRZ(int &Zright,int Zinput,int JLayer,int iside,bool IsSA){
   int zleft=0;
   int zright=0;
   for(int Z=Zinput;Z>0;Z--){
      MSplineH* pmap=GetRigidity(JLayer,iside,IsSA,Z);
      if(pmap){zleft=Z; break;}
   }
   for(int Z=Zinput+1;Z<=NZQ;Z++){
      MSplineH* pmap=GetRigidity(JLayer,iside,IsSA,Z);
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
            MSplineH* pmap=GetRigidity(JLayer,iside,IsSA,Z);
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
double TrRigidityDBH::GetValue(int JLayer,int iside,bool IsSA,int Z,double rigidity){
   MSplineH* spl=GetRigidity(JLayer,iside,IsSA,Z);
   return spl?spl->Eval(rigidity):0;
}
TGraph* TrRigidityDBH::GetCorrectionGraph(double rigidity,int JLayer,int iside,bool IsSA){
   if(JLayer<1||JLayer>9) return 0;
   int ilay=JLayer-1;
   if(iside<0||iside>2) return 0;

   int np=0;
   double xpre=0,ypre=0;
   double xx[NZQ],yy[NZQ];
   for(int iz=0;iz<NZQ;iz++){
      if((iz+1<=26)&&TrChargeLossDBH::Ignorelist(2,10,iz+1)) continue;
      MSplineH* spl0=(IsSA)?(kRigidity_SA[iz][ilay][iside]):(kRigidity[iz][ilay][iside]);
      if(!spl0) continue;

      double qrig=spl0->Eval(rigidity);
      xx[np]=qrig;
      yy[np]=qrig/(iz+1);
      if(fabs(yy[np]-1)>0.5) continue;
      if(np<=0) {xpre=iz+1;ypre=qrig;}
      else{
         if((qrig-ypre)*(iz+1-xpre)<=0) continue;
      }

      np++;
   }
   if(np<1) return 0;
   TGraph* res=new TGraph(np,xx,yy);
   return res;
}
MSplineH* TrRigidityDBH::GetCorrection(double rigidity,int JLayer,int iside,bool IsSA){
   TGraph* gr=GetCorrectionGraph(rigidity,JLayer,iside,IsSA);
   if(!gr) return 0;
   MSplineH* spl=new MSplineH(gr,false,false,MSplineH::kFlat,MSplineH::kFlat,0,0);
   delete gr;
   return spl;
}

double TrRigidityDBH::GetRigidityCorrectedValue(double rigidity,int JLayer,int iside,double charge,bool IsSA,bool UseNewAlgo){
   if(charge<=0) return 0;
   if(JLayer<1||JLayer>9) return charge;
   if(iside<0||iside>2) return charge;
   double x0;
   if(!UseNewAlgo){
      MSplineH* spl=GetCorrection(fabs(rigidity),JLayer,iside,IsSA);
      if(!spl) return charge;
      x0=spl->Eval(charge);
      delete spl;
      if(x0<=0) x0=1;
      return charge/x0;
   }
   else{
      //TGraph* spl=GetCorrectionGraph(fabs(rigidity),JLayer,iside,IsSA);
      //if(!spl) return charge;
      //x0=spl->Eval(charge);
      //delete spl;

      double Qinput=charge;
      int Zinput=int(Qinput);
      int Zlref;
      int Zrref;
      Zlref=FindRefLRZ(Zrref,Zinput,JLayer,iside,IsSA);
      if(Zrref<=0) return charge;
      double qlref=(Zlref<=0)?0:GetValue(JLayer,iside,IsSA,Zlref,rigidity);
      double qrref=(Zrref<=0)?0:GetValue(JLayer,iside,IsSA,Zrref,rigidity);
      double slope=(qrref-qlref)/(Zrref-Zlref);
      double logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
      double slplimit=0.7;
      double fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
      int clock=30;
      while( !( (charge>=qlref&&charge<=qrref) && fineslope ) ){
         //printf("TrRig:Zlref={%d,%.2lf} Zrref={%d,%.2lf} slope={%.2lf,%.2lf} adc=%.2f qin=%.2lf\n",Zlref,qlref,Zrref,qrref,slope,logslope,charge,Qinput);
         if(!fineslope) break;
         if(charge<qlref){
            Zrref=Zlref; qrref=qlref;
            int Zbuff;
            Zlref=FindRefLRZ(Zbuff,Zrref-1,JLayer,iside,IsSA);
            qlref=(Zlref<=0)?0:GetValue(JLayer,iside,IsSA,Zlref,rigidity);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         if(charge>qrref){
            Zlref=Zrref; qlref=qrref;
            int Zbuff;
            Zrref=FindRefLRZ(Zbuff,Zlref,JLayer,iside,IsSA);
            if(Zrref<=Zlref){  //already the last Z;
               return Zlref+(charge-qlref)/slope;
            }
            qrref=(Zrref<=0)?0:GetValue(JLayer,iside,IsSA,Zrref,rigidity);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         clock--;
         if(clock<0) break;
      }
      //printf("TrRig:Outloop:Zlref={%d,%.2lf} Zrref={%d,%.2lf} slope={%.2lf,%.2lf} adc=%.2f qin=%.2lf\n",Zlref,qlref,Zrref,qrref,slope,logslope,charge,Qinput);
      if(!fineslope) return charge;
      if(charge>=qlref&&charge<=qrref){
         return Zlref-(qlref-charge)/(qlref-qrref)*(Zlref-Zrref);
      }
      else return charge;
   }
}
