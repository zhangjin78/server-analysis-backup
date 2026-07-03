#include "TrGainCHDBH.h"
#include "TkDBc.h"
#include "stdlib.h"
#include "TrChargeDBH.h"
#include "commonsi.h"


int TrGainCHDBH::fUseVersion=12;
TrGainCHDBH* TrGainCHDBH::fHead=0;
MSplineH* TrGainCHDBH::kGainCH[NCalib][2][NSQ+1][2][1920];

void TrGainCHDBH::Init(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         int nva=(iside==0)?(192*6):(192*10);
         for(int iva=0;iva<1920;iva++){
            for(int icalib=0;icalib<NCalib;icalib++){
               kGainCH[icalib][iside][istrip][0][iva]=0;
               kGainCH[icalib][iside][istrip][1][iva]=0;
            }
         }
      }
   }
}
void TrGainCHDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int iva=0;iva<1920;iva++){
            for(int icalib=0;icalib<NCalib;icalib++){
               if(kGainCH[icalib][iside][istrip][0][iva]) {delete kGainCH[icalib][iside][istrip][0][iva];kGainCH[icalib][iside][istrip][0][iva]=0;}
               if(kGainCH[icalib][iside][istrip][1][iva]) {delete kGainCH[icalib][iside][istrip][1][iva];kGainCH[icalib][iside][istrip][1][iva]=0;}
            }
         }
      }
   }
   fHead=0;
}

bool TrGainCHDBH::ReadGainCH(int iside,int istrip,char* filename,int icalib){
   if(icalib<0||icalib>=NCalib) return false;
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ngain=0;
   fscanf(fin,"%d",&ngain);
   for(int ii=0;ii<ngain;ii++){
      for(int ivar=0;ivar<2;ivar++){
         int vaindex;
         int nknot;
         int logx,logy,extralow,extrahig;
         double blow,bhig,chi0,chi1,chi2,chi3;
         fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf",&vaindex,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         int nva=(iside==0)?(192*6):(192*10);
         if(vaindex<0||vaindex>=nva) return false;
         double xknot[100],yknot[100];
         for(int i2=0;i2<nknot;i2++) fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         if(kGainCH[icalib][iside][istrip][ivar][vaindex]) {delete kGainCH[icalib][iside][istrip][ivar][vaindex]; kGainCH[icalib][iside][istrip][ivar][vaindex]=0;}
         kGainCH[icalib][iside][istrip][ivar][vaindex]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         kGainCH[icalib][iside][istrip][ivar][vaindex]->fitquality[0]=chi0;
         kGainCH[icalib][iside][istrip][ivar][vaindex]->fitquality[1]=chi1;
         kGainCH[icalib][iside][istrip][ivar][vaindex]->fitquality[2]=chi2;
         kGainCH[icalib][iside][istrip][ivar][vaindex]->fitquality[3]=chi3;
      }
   }
   fclose(fin);
   return ngain;
}
bool TrGainCHDBH::LoadGainCHFromDir(char *dirname,int icalib){
   if(icalib<0||icalib>=NCalib){
      printf("kGainCH Calibration Type Wrong(%d),Didn't Load GainCHLines!\n",icalib);
      return false;
   }
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool loaded=false;
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         char filename[200];
         strcpy(filename,Form("%s/GainCHTable_%s_S%d_Algo3.txt",dirname_copy,iside==0?"x":"y",istrip));
         bool loadgain=ReadGainCH(iside,istrip,filename,icalib);
         //printf("Load GainLines(kGainCH%d) From %s(%d,%d) %s\n",icalib,filename,iside,istrip,loadgain?"successfully":"failed");
         loaded=loaded||loadgain;
      }
   }
   return loaded;
}
TrGainCHDBH* TrGainCHDBH::GetHead(){
   if(!fHead){
      fHead=new TrGainCHDBH();
      bool loaded=true;
      //Loading all gainlines for all the kGainCH correction: kGainCH0,kGainCH1,...
      char DatabaseDir1[200]="";
      char DatabaseDir2[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/gain1",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrGainCHDBH::fUseVersion/10):(TrGainCHDBH::fUseVersion%10)));//the directory has been changed to v6.00
      strcpy(DatabaseDir2,Form("%sChargeCalibHDB/%sv%d/gain2",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrGainCHDBH::fUseVersion/10):(TrGainCHDBH::fUseVersion%10)));
      loaded=fHead->LoadGainCHFromDir(DatabaseDir1,0)&&loaded;
      loaded=fHead->LoadGainCHFromDir(DatabaseDir2,1)&&loaded;
      //fHead->LoadGainCHFromDir(DatabaseDir2,1);
      printf("TrGainDBH:: Read GainTable from %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if((!loaded)&&(!TrChargeDBH::IsMC)){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

MSplineH* TrGainCHDBH::GetSlopeAndAmp(int iside,int istrip,int iva,bool IsSlope,int icalib){
   if(icalib<0||icalib>=NCalib) return 0;
   if(iside<0||iside>=2) return 0;
   if(istrip<0||istrip>NSQ) return 0;
   int nva=(iside==0)?(192*6):(192*10);
   if(iva<0||iva>=nva) return 0;
   return kGainCH[icalib][iside][istrip][IsSlope][iva];
}

TGraph* TrGainCHDBH::GetCorrectionGraph(int iside,int istrip,int iva,int ich,int icalib){
   MSplineH* spl=GetSlopeAndAmp(iside,istrip,iva,false,icalib);
   MSplineH* spl2=GetSlopeAndAmp(iside,istrip,iva,true,icalib);
   if(!spl) return 0;
   if(!spl2) return 0;

   int np=0;
   double xx[NZQ];
   double yy[NZQ];
   for(int iz=0;iz<NZQ;iz++){
      double amp=spl->Eval(iz+1);
      double slope=spl2->Eval(iz+1);
      //if(slope>0.01) slope=0.01;
      //if(slope<-0.01) slope=-0.01;
      if(amp+slope*0<=0) continue;
      if(amp+slope*63<=0) continue;
      double corr=(amp+slope*ich)/(iz+1);
      if(fabs(corr-1)>0.4) continue;
      xx[np]=iz+1;
      yy[np]=corr;
      np++;
   }
   if(np<5) return 0;
   TGraph* res=new TGraph(np,xx,yy);
   return res;
}
MSplineH* TrGainCHDBH::GetCorrection(int iside,int istrip,int iva,int ich,int icalib){
   TGraph* gr=GetCorrectionGraph(iside,istrip,iva,ich,icalib);
   if(!gr) return 0;
   MSplineH* res=new MSplineH(gr,false,false,MSplineH::kFlat,MSplineH::kFlat,0.,0.);
   delete gr;
   return res;
}

int TrGainCHDBH::CountGainCH(int iside,int istrip,int icalib){
   int ngain=0;
   for(int iside0=0;iside0<2;iside0++){
      if(iside>=0&&iside!=iside0) continue;
      for(int istrip0=0;istrip0<=NSQ;istrip0++){
         if(istrip>=0&&istrip!=istrip0) continue;
         int nva=(iside==0)?(192*6):(192*10);
         for(int iva=0;iva<nva;iva++){
            for(int icalib0=0;icalib0<NCalib;icalib0++){
               if(icalib>=0&&icalib!=icalib0) continue;
               MSplineH* spl=GetCorrection(iside0,istrip0,iva,32,icalib0);
               if(spl) {ngain++; delete spl;}
            }
         }
      }
   }
   return ngain;
}

double TrGainCHDBH::GetGainCHCorrectedValue(int iside,int istrip,int vaindex,int ich,double charge,double Z0,int icalib,bool UseNewAlgo){
   if(icalib<0||icalib>=NCalib) return charge;
   if(charge<=0) return 0;
   double x0;
   if(!UseNewAlgo){
      MSplineH* spl=GetCorrection((int)iside,istrip,vaindex,ich,icalib);
      if(!spl) return charge;
      x0=spl->Eval(Z0>0?Z0:charge);
      delete spl;
      if(x0<=0) x0=1;
      return charge/x0;
   }
   else{
      //TGraph* spl=GetCorrectionGraph((int)iside,istrip,vaindex,ich,icalib);
      //if(!spl) return charge;
      //x0=spl->Eval(Z0>0?Z0:charge);
      //delete spl;

      double Qinput=Z0>0?Z0:charge;
      int Zinput=int(Qinput);
      MSplineH* amp=GetSlopeAndAmp(iside,istrip,vaindex,false,icalib);
      MSplineH* slp=GetSlopeAndAmp(iside,istrip,vaindex,true,icalib);
      if(!(amp&&slp)) return charge;
      int Zlref=Zinput;
      double qlref=(Zlref<=0)?0:(amp->Eval(Zlref)+slp->Eval(Zlref)*ich);
      int Zrref=Zinput+1;
      double qrref=(Zrref<=0)?0:(amp->Eval(Zrref)+slp->Eval(Zrref)*ich);
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
      bool fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
      int clock=30;
      while( !( (charge>=qlref&&charge<=qrref) && fineslope ) ){
         if(!fineslope) break;
         if(charge<qlref){
            Zrref=Zlref; qrref=qlref;
            Zlref--;
            qlref=(Zlref<=0)?0:(amp->Eval(Zlref)+slp->Eval(Zlref)*ich);
            slope=(qrref-qlref)/(Zrref-Zlref);
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(Zrref*1./Zlref));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         if(charge>qrref){
            if(Zrref>=28){
               return Zrref+(charge-qrref)/slope;
            }
            Zlref=Zrref; qlref=qrref;
            Zrref++;
            qrref=(Zrref<=0)?0:(amp->Eval(Zrref)+slp->Eval(Zrref)*ich);
            if(qrref<=0) return Zlref+(charge-qlref)/slope; //some strange situation
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
double TrGainCHDBH::GetGainCHCorrectedValue(int iside,int istrip,int tkid,int iva,int ich,double charge,double Z0,int icalib,bool UseNewAlgo){
   //printf("gainchbegin: %s S%d TkId=%+4d VA%d ch%d q0=%.2f Z0=%.2f calib=%d\n",iside==0?"x":"y",istrip,tkid,iva,ich,charge,Z0,icalib);
   if(icalib<0||icalib>=NCalib) return charge;
   if(iside==0&&(iva<10||iva>15)){
      printf("TrGainCHDBH::vaindex(%d) should be inside (10,15). reset it to 10.\n",iva);
      iva=10;
   }
   if(iside!=0&&(iva<0||iva>9)){
      printf("TrGainCHDBH::vaindex(%d) should be inside (0,9). reset it to 0.\n",iva);
      iva=0;
   }
   //if(!(TkDBc::Head)){
   //   TkDBc::CreateTkDBc();
   //   TkDBc::Head->init();
   //}
   int ilad=TkDBc::Head->TkId2Entry(tkid);
   int vaindex=(iside==0)?(ilad*6+iva-10):(ilad*10+iva);
   double res=GetGainCHCorrectedValue(iside,istrip,vaindex,ich,charge,Z0,icalib,UseNewAlgo);
   //printf("gainch end: res=%.2f\n",res);
   return res;
}
