#include "TrTempDBH.h"
#include "root.h"
#include "stdlib.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "event.h"
#endif
#include "TrChargeDBH.h"
#include "commonsi.h"

AMSSetupR TrTempDBH::setup;
int TrTempDBH::run_current;
int TrTempDBH::time_current;
double TrTempDBH::temperature_current[9];
int TrTempDBH::fUseVersion=12;
TrTempDBH* TrTempDBH::fHead=0;
MSplineH* TrTempDBH::kTemp[NCalib][2][NSQ+1][2][9];

void TrTempDBH::Init(){
   run_current=1000000000;
   time_current=1000000000;
   for(int il=0;il<9;il++) temperature_current[il]=-100;
   setup.fHeader.Run=run_current;
   setup.fHeader.FEventTime=run_current-120;
   setup.fHeader.LEventTime=run_current+3600;
   if(run_current>=1305853512) setup.LoadExt();

   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int ilay=0;ilay<9;ilay++){
            for(int icalib=0;icalib<NCalib;icalib++){
               kTemp[icalib][iside][istrip][0][ilay]=0;
               kTemp[icalib][iside][istrip][1][ilay]=0;
            }
         }
      }
   }
}
void TrTempDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int ilay=0;ilay<9;ilay++){
            for(int icalib=0;icalib<NCalib;icalib++){
               if(kTemp[icalib][iside][istrip][0][ilay]) {delete kTemp[icalib][iside][istrip][0][ilay];kTemp[icalib][iside][istrip][0][ilay]=0;}
               if(kTemp[icalib][iside][istrip][1][ilay]) {delete kTemp[icalib][iside][istrip][1][ilay];kTemp[icalib][iside][istrip][1][ilay]=0;}
            }
         }
      }
   }
   fHead=0;
}

bool TrTempDBH::ReadTemp(int iside,int istrip,char* filename,int icalib){
   if(icalib<0||icalib>=NCalib) return false;
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ntemp=0;
   fscanf(fin,"%d",&ntemp);
   for(int ii=0;ii<ntemp;ii++){
      for(int ivar=0;ivar<2;ivar++){
         int JLayer;
         int nknot;
         int logx,logy,extralow,extrahig;
         double blow,bhig,chi0,chi1,chi2,chi3;
         fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf",&JLayer,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(JLayer<1||JLayer>9) return false;
         double xknot[100],yknot[100];
         for(int i2=0;i2<nknot;i2++) fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         if(kTemp[icalib][iside][istrip][ivar][JLayer-1]) {delete kTemp[icalib][iside][istrip][ivar][JLayer-1]; kTemp[icalib][iside][istrip][ivar][JLayer-1]=0;}
         kTemp[icalib][iside][istrip][ivar][JLayer-1]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);;
         kTemp[icalib][iside][istrip][ivar][JLayer-1]->fitquality[0]=chi0;
         kTemp[icalib][iside][istrip][ivar][JLayer-1]->fitquality[1]=chi1;
         kTemp[icalib][iside][istrip][ivar][JLayer-1]->fitquality[2]=chi2;
         kTemp[icalib][iside][istrip][ivar][JLayer-1]->fitquality[3]=chi3;
      }
   }
   fclose(fin);
   return ntemp;
}
bool TrTempDBH::LoadTempFromDir(char *dirname,int icalib){
   if(icalib<0||icalib>=NCalib){
      printf("kTemp Calibration Type Wrong(%d),Didn't Load Temperature Correction!\n",icalib);
      return false;
   }
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool loaded=false;
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         char filename[200];
         strcpy(filename,Form("%s/TempTable_%s_S%d_Algo3.txt",dirname_copy,iside==0?"x":"y",istrip));
         bool loadtemp=ReadTemp(iside,istrip,filename,icalib);
         //printf("Load Temperature Correction(kTemp%d) From %s(%d,%d) %s\n",icalib,filename,iside,istrip,loadtemp?"successfully":"failed");
         loaded=loaded||loadtemp;
      }
   }
   return loaded;
}
TrTempDBH* TrTempDBH::GetHead(){
   if(!fHead){
      fHead=new TrTempDBH();
      bool loaded=true;
      //Loading for all the kTemp correction: kTemp0,kTemp1,...
      char DatabaseDir1[200]="";
      char DatabaseDir2[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/temperature1",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrTempDBH::fUseVersion/10):(TrTempDBH::fUseVersion%10)));//the directory has been changed to v6.00
      strcpy(DatabaseDir2,Form("%sChargeCalibHDB/%sv%d/temperature2",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrTempDBH::fUseVersion/10):(TrTempDBH::fUseVersion%10)));
      loaded=fHead->LoadTempFromDir(DatabaseDir1,0)&&loaded;
      loaded=fHead->LoadTempFromDir(DatabaseDir2,1)&&loaded;
      //fHead->LoadTempFromDir(DatabaseDir2,1);
      printf("TrTempDBH:: Read TemperatureTable from %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if((!loaded)&&(!TrChargeDBH::IsMC)){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

MSplineH* TrTempDBH::GetSlopeAndAmp(int iside,int istrip,int JLayer,bool IsSlope,int icalib){
   if(icalib<0||icalib>=NCalib) return 0;
   if(iside<0||iside>=2) return 0;
   if(istrip<0||istrip>NSQ) return 0;
   if(JLayer<1||JLayer>9) return 0;
   return kTemp[icalib][iside][istrip][IsSlope][JLayer-1];
}
TGraph* TrTempDBH::GetCorrectionGraph(int iside,int istrip,int JLayer,double temp,int icalib){
   MSplineH* spl=GetSlopeAndAmp(iside,istrip,JLayer,false,icalib);
   MSplineH* spl2=GetSlopeAndAmp(iside,istrip,JLayer,true,icalib);
   if(!spl) return 0;
   if(!spl2) return 0;

   int np=0;
   double xx[NZQ];
   double yy[NZQ];
   for(int iz=0;iz<NZQ;iz++){
      double amp=spl->Eval(iz+1);
      double slope=spl2->Eval(iz+1);
      if(amp+slope*0<=0) continue;
      if(amp+slope*25<=0) continue;
      double corr=(amp+slope*temp)/(iz+1);
      xx[np]=iz+1;
      yy[np]=corr;
      np++;
   }
   if(np<5) return 0;
   TGraph* res=new TGraph(np,xx,yy);
   return res;
}
MSplineH* TrTempDBH::GetCorrection(int iside,int istrip,int JLayer,double temp,int icalib){
   TGraph* gr=GetCorrectionGraph(iside,istrip,JLayer,temp,icalib);
   if(!gr) return 0;
   MSplineH* res=new MSplineH(gr,false,false,MSplineH::kFlat,MSplineH::kFlat,0.,0.);
   delete gr;
   return res;
}

int TrTempDBH::CountTemp(int iside,int istrip,int icalib){
   int np=0;
   for(int iside0=0;iside0<2;iside0++){
      if(iside>=0&&iside!=iside0) continue;
      for(int istrip0=0;istrip0<=NSQ;istrip0++){
         if(istrip>=0&&istrip!=istrip0) continue;
         for(int ilay=0;ilay<9;ilay++){
            for(int icalib0=0;icalib0<NCalib;icalib0++){
               if(icalib>=0&&icalib!=icalib0) continue;
               MSplineH* spl=GetCorrection(iside0,istrip0,ilay+1,10.,icalib0);
               if(spl) {np++; delete spl;}
            }
         }
      }
   }
   return np;
}

double TrTempDBH::GetTempCorrectedValueForce(int iside,int istrip,int JLayer,double temperature,double charge,double Z0,int icalib,bool UseNewAlgo){
   if(icalib<0||icalib>=NCalib) return charge;
   if(charge<=0) return 0;
   if(fabs(temperature)>99) return charge;
   double x0;
   //if(TrChargeDBH::UseMSplineH||TrChargeDBH::IsMC){
   if(TrChargeDBH::IsMC){
      MSplineH* spl=GetCorrection(iside,istrip,JLayer,temperature,icalib);
      if(!spl) return charge;
      x0=spl->Eval(Z0>0?Z0:charge);
      delete spl;
      if(x0<=0) x0=1;
      return charge/x0;
   }
   else{
      //TGraph* spl=GetCorrectionGraph(iside,istrip,JLayer,temperature,icalib);
      //if(!spl) return charge;
      //x0=spl->Eval(Z0>0?Z0:charge);
      //delete spl;

      double Qinput=Z0>0?Z0:charge;
      int Zinput=int(Qinput);
      MSplineH* amp=GetSlopeAndAmp(iside,istrip,JLayer,false,icalib);
      MSplineH* slp=GetSlopeAndAmp(iside,istrip,JLayer,true,icalib);
      if(!(amp&&slp)) return charge;
      int Zlref=Zinput;
      double qlref=(Zlref<=0)?0:(amp->Eval(Zlref)+slp->Eval(Zlref)*temperature);
      int Zrref=Zinput+1;
      double qrref=(Zrref<=0)?0:(amp->Eval(Zrref)+slp->Eval(Zrref)*temperature);
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
            Zlref--;
            qlref=(Zlref<=0)?0:(amp->Eval(Zlref)+slp->Eval(Zlref)*temperature);
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
            qrref=(Zrref<=0)?0:(amp->Eval(Zrref)+slp->Eval(Zrref)*temperature);
            if(qlref<=0) return Zlref+(charge-qlref)/slope;  //some strange situation
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
double TrTempDBH::GetTemperature(int JLayer,int run,int time){
   double temperature=-100;
   //return temperature;
   if(JLayer<1||JLayer>9) return temperature;
   if(TrChargeDBH::IsMC) return temperature;
   //choose the AMSSetupR from database(without run and time) or recreated one(with reasonable run and time)
   AMSSetupR* fsetup=0;
   if(run<1000000000){
      if(AMSSetupR::gethead()) fsetup=AMSSetupR::gethead();
   }
   else{
      fsetup=(&setup);
      if(run!=run_current){
         fsetup->fHeader.Run=run;
         fsetup->fHeader.FEventTime=run-120;
         fsetup->fHeader.LEventTime=run+3600;
         fsetup->LoadExt();
         run_current=run;
      }
   }
   if(!fsetup) return temperature;
   if(time<1000000000){
      if(AMSEventR::Head()) time=AMSEventR::Head()->UTime();
      #ifndef __ROOTSHAREDLIBRARY__
      if(AMSEvent::gethead()) time=AMSEvent::gethead()->gettime();
      #endif
   }
   if(time<1000000000) return temperature;

   //return the temperature stored if no change of time
   //if((time==time_current)&&(temperature_current[JLayer-1]>-100)) return temperature_current[JLayer-1];
   if(abs(time-time_current)<60) return temperature_current[JLayer-1];

   float avetemp[12];
   for(int ii=0;ii<12;ii++) avetemp[ii]=-100;
   const string sensor[12]={
      "Sensor A","Sensor B","Sensor C","Sensor D", // inner Tracker on readout (A and C on thermal bars)
      "Sensor K","Sensor M", // L9 on readout
      "M-7X:0:1N-1","M-7X:1:1N-2","M-7X:2:1N-3","M-7X:3:1N-4","M-7X:7:1NS-2","M-7X:10:1NS-5" // L1 on readout
   };
   for (int i=0; i<12; i++) {
     vector<float>value;
     int ret = fsetup->fSlowControl.GetData(sensor[i].c_str(),time,0.5,value);
     if (ret==0) {
       float tottemp=0;
       for (int j=0; j<value.size(); j++) {
          tottemp+=value.at(j);
       }
       if(value.size()>=1){
          avetemp[i]=tottemp/value.size();
       }
     }
   }
   for(int ilay=1;ilay<=9;ilay++){
      float tempfilled=0;
      int nfilled=0;
      for(int i=0;i<12;i++){
         if(ilay==1){
            if(i<6||i>11) continue;
         }
         else if(ilay==9){
            if(i<4||i>5) continue;
         }
         else{
             if(i!=0&&i!=2) continue;
         }
         if(avetemp[i]<-99) continue;
         tempfilled+=avetemp[i];
         nfilled++;
      }
      temperature_current[ilay-1]=(nfilled>0)?(tempfilled/nfilled):-100;
      //if(JLayer==ilay) temperature_current[ilay-1]=(nfilled>0)?(tempfilled/nfilled):-100;
   }
   time_current=time;
   temperature=temperature_current[JLayer-1];
   return temperature;
}
double TrTempDBH::GetTempCorrectedValue(int iside,int istrip,int JLayer,double charge,double Z0,int icalib,int run,int time,bool UseNewAlgo){
   if(charge<=0) return 0;
   if(icalib<0||icalib>=NCalib) return charge;
   double temperature=GetTemperature(JLayer,run,time);
   bool debug=false;
   if(debug) printf("L%d: temp=%.2f\n",JLayer,temperature);
   if(fabs(temperature)>99) return charge;

   double result=GetTempCorrectedValueForce(iside,istrip,JLayer,temperature,charge,Z0,icalib,UseNewAlgo);
   return result;
}

