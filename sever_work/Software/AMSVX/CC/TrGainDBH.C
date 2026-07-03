#include "TrGainDBH.h"
#include "TkDBc.h"
#include "stdlib.h"
#include "TrChargeDBH.h"
#include "commonsi.h"

int TrGainDBH::fUseVersion=12;
TrGainDBH* TrGainDBH::fHead=0;
MSplineH* TrGainDBH::kGain[NCalib][2][NSQ+1][1920];

void TrGainDBH::Init(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         int nva=(iside==0)?(192*6):(192*10);
         for(int iva=0;iva<1920;iva++){
            for(int icalib=0;icalib<NCalib;icalib++){
               kGain[icalib][iside][istrip][iva]=0;
            }
         }
      }
   }
}
void TrGainDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         for(int iva=0;iva<1920;iva++){
            for(int icalib=0;icalib<NCalib;icalib++){
               if(kGain[icalib][iside][istrip][iva]) {delete kGain[icalib][iside][istrip][iva];kGain[icalib][iside][istrip][iva]=0;}
            }
         }
      }
   }
   fHead=0;
}

bool TrGainDBH::ReadGain(int iside,int istrip,char* filename,int icalib){
   if(icalib<0||icalib>=NCalib) return false;
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ngain=0;
   fscanf(fin,"%d",&ngain);
   for(int ii=0;ii<ngain;ii++){
      int vaindex;
      int nknot;
      int logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf",&vaindex,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      int nva=(iside==0)?(192*6):(192*10);
      if(vaindex<0||vaindex>=nva) return false;
      double xknot[100],yknot[100];
      for(int i2=0;i2<nknot;i2++) fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      if(kGain[icalib][iside][istrip][vaindex]){ delete kGain[icalib][iside][istrip][vaindex]; kGain[icalib][iside][istrip][vaindex]=0; }
      kGain[icalib][iside][istrip][vaindex]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      MSplineH* spl=kGain[icalib][iside][istrip][vaindex];
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
   }
   fclose(fin);
   return ngain;
}
bool TrGainDBH::LoadGainFromDir(char *dirname,int icalib){
   if(icalib<0||icalib>=NCalib){
      printf("kGain Calibration Type Wrong(%d),Didn't Load GainLines!\n",icalib);
      return false;
   }
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool loaded=false;
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         char filename[200];
         strcpy(filename,Form("%s/GainTable_%s_S%d_Algo3.txt",dirname_copy,iside==0?"x":"y",istrip));
         bool loadgain=ReadGain(iside,istrip,filename,icalib);
         //printf("Load GainLines(kGain%d) From %s(%d,%d) %s\n",icalib,filename,iside,istrip,loadgain?"successfully":"failed");
         loaded=loaded||loadgain;
      }
   }
   return loaded;
}
TrGainDBH* TrGainDBH::GetHead(){
   if(!fHead){
      fHead=new TrGainDBH();
      bool loaded=true;
      //Loading all gainlines for all the kGain correction: kGain0,kGain1,...
      char DatabaseDir1[200]="";
      char DatabaseDir2[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/gain1",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrGainDBH::fUseVersion/10):(TrGainDBH::fUseVersion%10)));//the directory has been changed to v6.00
      strcpy(DatabaseDir2,Form("%sChargeCalibHDB/%sv%d/gain2",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrGainDBH::fUseVersion/10):(TrGainDBH::fUseVersion%10)));
      loaded=fHead->LoadGainFromDir(DatabaseDir1,0)&&loaded;
      loaded=fHead->LoadGainFromDir(DatabaseDir2,1)&&loaded;
      //printf("TrGainDBH:: Read GainTable from %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if((!loaded)&&(!TrChargeDBH::IsMC)&&(TrGainDBH::fUseVersion%10)<3){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

MSplineH* TrGainDBH::GetCorrection(int iside,int istrip,int iva,int icalib){
   if(icalib<0||icalib>=NCalib) return 0;
   if(iside<0||iside>=2) return 0;
   if(istrip<0||istrip>NSQ) return 0;
   int nva=(iside==0)?(192*6):(192*10);
   if(iva<0||iva>=nva) return 0;
   return kGain[icalib][iside][istrip][iva];
}

int TrGainDBH::CountGain(int iside,int istrip,int icalib){
   int ngain=0;
   for(int iside0=0;iside0<2;iside0++){
      if(iside>=0&&iside!=iside0) continue;
      for(int istrip0=0;istrip0<=NSQ;istrip0++){
         if(istrip>=0&&istrip!=istrip0) continue;
         int nva=(iside==0)?(192*6):(192*10);
         for(int iva=0;iva<nva;iva++){
            for(int icalib0=0;icalib0<NCalib;icalib0++){
               if(icalib>=0&&icalib!=icalib0) continue;
               if(GetCorrection(iside0,istrip0,iva,icalib0)) ngain++;
            }
         }
      }
   }
   return ngain;
}

double TrGainDBH::GetGainCorrectedValue(int iside,int istrip,int vaindex,double charge,double Z0,int icalib){
   if(icalib<0||icalib>=NCalib) return charge;
   if(charge<=0) return 0;
   MSplineH* spl=GetCorrection((int)iside,istrip,vaindex,icalib);
   if(!spl) return charge;
   double x0=spl->Eval(Z0>0?Z0:charge);
   if(x0>1.4) x0=1.4;
   if(x0<0.6) x0=0.6;
   return charge/x0;
}
double TrGainDBH::GetGainCorrectedValue(int iside,int istrip,int tkid,int iva,double charge,double Z0,int icalib){
   //printf("gain begin: %s S%d TkId=%+4d VA%d q0=%.2f Z0=%.2f calib=%d\n",iside==0?"x":"y",istrip,tkid,iva,charge,Z0,icalib);
   if(icalib<0||icalib>=NCalib) return charge;
   if(iside==0&&(iva<10||iva>15)){
      printf("TrGainDBH::vaindex(%d) should be inside (10,15). reset it to 10.\n",iva);
      iva=10;
   }
   if(iside!=0&&(iva<0||iva>9)){
      printf("TrGainDBH::vaindex(%d) should be inside (0,9). reset it to 0.\n",iva);
      iva=0;
   }
   //if(!(TkDBc::Head)){
   //   TkDBc::CreateTkDBc();
   //   TkDBc::Head->init();
   //}
   int ilad=TkDBc::Head->TkId2Entry(tkid);
   int vaindex=(iside==0)?(ilad*6+iva-10):(ilad*10+iva);
   double res=GetGainCorrectedValue(iside,istrip,vaindex,charge,Z0,icalib);
   //printf("gain end: res=%.2f\n",res);
   return res;
}
