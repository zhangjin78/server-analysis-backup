#include "TrChargeLossDBH.h"
#include "stdlib.h"
#include "TrChargeDBH.h"
#include "commonsi.h"

int TrChargeLossDBH::fUseVersion=12;
TrChargeLossDBH* TrChargeLossDBH::fHead=0;
Fit2DH* TrChargeLossDBH::kLoss[NCalib][2][NSQ+1][NK][NZQ];

bool TrChargeLossDBH::usedis=true;
bool TrChargeLossDBH::foldeta=true;

void TrChargeLossDBH::Init(){
   for(int iside=0;iside<2;iside++){
      for(int catg=0;catg<NK;catg++){
         for(int istrip=0;istrip<=NSQ;istrip++){
            for(int iz=0;iz<NZQ;iz++){
               for(int icalib=0;icalib<NCalib;icalib++){
                  kLoss[icalib][iside][istrip][catg][iz]=0;
               }
            }
         }
      }
   }
}
void TrChargeLossDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int catg=0;catg<NK;catg++){
         for(int istrip=0;istrip<=NSQ;istrip++){
            for(int iz=0;iz<NZQ;iz++){
               for(int icalib=0;icalib<NCalib;icalib++){
                  if(kLoss[icalib][iside][istrip][catg][iz]) {delete kLoss[icalib][iside][istrip][catg][iz]; kLoss[icalib][iside][istrip][catg][iz]=0;}
               }
            }
         }
      }
   }
   fHead=0;
}
bool TrChargeLossDBH::Ignorelist(int iside,int istrip,int Z){
   bool ignore=false;
   if(istrip<0) ignore=true;
   if(Z<=0) ignore=true;
   //ignore some charges due to contamination
   if((Z>8)&&(Z%2==1)) ignore=true;
   if(Z>28) ignore=true;
   //if(Z==4&&(!TrChargeDBH::IsMC)) ignore=true;
   //ignore some charges due to too low signal
   if(iside==0&&istrip==4&&Z<14) ignore=true;
   if(iside==0&&istrip==3&&Z<12) ignore=true;
   if(iside==0&&istrip==2&&Z<3)  ignore=true;
   if(iside==0&&istrip==1&&Z<3)  ignore=true;
   if(iside==1&&istrip==4&&Z<5)  ignore=true;
   if(iside==1&&istrip==3&&Z<4)  ignore=true;
   if(iside==1&&istrip==2&&Z<3)  ignore=true;
   if(iside==1&&istrip==1&&Z<2)  ignore=true;
   return ignore;
}

bool TrChargeLossDBH::LoadMapFromDir(char *dirname,int icalib){
   if(icalib<0||icalib>=NCalib){
      printf("kLoss Calibration Type Wrong(%d),Didn't Load Maps!\n",icalib);
      return false;
   }
   char dirname_copy[200]="";
   strcpy(dirname_copy,dirname);
   bool loaded=false;
   for(int iside=0;iside<2;iside++){
      for(int catg=0;catg<NK;catg++){
         for(int istrip=0;istrip<=NSQ;istrip++){
            int istrip0=( usedis||((!usedis)&&foldeta) )?istrip:(istrip-(NSQ-1)/2);
            for(int iz=0;iz<NZQ;iz++){
               char filename[200]="";
               if((iz+1<=26)&&Ignorelist(iside,istrip0,iz+1)) continue;
               int fitindex=3;
               if( ((!TrChargeDBH::IsMC)&&((TrChargeLossDBH::fUseVersion%10)==1)) && (istrip<NSQ&&icalib==0) ) fitindex=0;
               strcpy(filename,Form("%s/ChargeLossTable_%s%d_S%d_Z%02d_Algo%d.txt",dirname_copy,iside==0?"x":"y",catg,istrip0,iz+1,fitindex));
               bool loadmap=false;
               Fit2DH* map=new Fit2DH();
               bool success=map->ReadMap(filename);
               if(success){
                  bool wronglyloaded=false;
                  double etamin,etamax;
                  if(map->xfirst) {etamin=map->min_fir; etamax=map->max_fir;}
                  else            {etamin=map->min_sec; etamax=map->max_sec;}
                  bool folded=( usedis||((!usedis)&&foldeta) );
                  bool positive=IsPositive(istrip0,TrAsymmDBH::useright);
                  if( (!folded)&&(etamin>-0.25 || etamax<0.25) ) wronglyloaded=true;
                  if( (folded&&positive) && (etamin<-0.25) ) wronglyloaded=true;
                  if( (folded&&(!positive)) && (etamax>0.25) ) wronglyloaded=true;

                  if(wronglyloaded){
                     printf("Load Wrong Map: %s\n",filename);
                  }
                  else{
                     kLoss[icalib][iside][istrip][catg][iz]=map;
                     loadmap=true;
                     loaded=loaded||loadmap;
                  }
               }
               //printf("Read Map from %s %s\n",filename,loadmap?"successfully":"failed");
               if(!loadmap) {delete map;}
            }
         }
      }
   }
   //printf("TrChargeLossDBH:: Read Maps(kLoss%d) from %s %s\n",icalib,dirname_copy,loaded?"successfully":"failed");
   return loaded;
}
TrChargeLossDBH* TrChargeLossDBH::GetHead(){
   if(!fHead){
      fHead=new TrChargeLossDBH();
      bool loaded=true;
      //Loading all maps for all the kLoss correction: kLoss0,kLoss1,...
      char DatabaseDir1[200]="";
      char DatabaseDir2[200]="";
      strcpy(DatabaseDir1,Form("%sChargeCalibHDB/%sv%d/chargeloss1",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeLossDBH::fUseVersion/10):(TrChargeLossDBH::fUseVersion%10)));//the directory has been changed to v6.00
      strcpy(DatabaseDir2,Form("%sChargeCalibHDB/%sv%d/chargeloss2",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeLossDBH::fUseVersion/10):(TrChargeLossDBH::fUseVersion%10)));
      loaded=fHead->LoadMapFromDir(DatabaseDir1,0)&&loaded;
      loaded=fHead->LoadMapFromDir(DatabaseDir2,1)&&loaded;
      //fHead->LoadMapFromDir(DatabaseDir2,1);
      printf("TrChargeLossDBH:: Read ChargeLoss Maps from %s %s\n",DatabaseDir1,loaded?"successfully":"failed");
      if(!loaded){
         if(fHead) delete fHead;
         fHead = 0;
      }
      return fHead;
   }
   else return fHead;
}

Fit2DH* TrChargeLossDBH::GetMap(int iside,int catg,int istrip,int Z,int icalib){
   if(icalib<0||icalib>=NCalib) return 0;
   if(!usedis&&(!foldeta)) istrip+=(NSQ-1)/2;
   if(iside<0||iside>1) return 0;
   if(catg<0||catg>NK) return 0;
   if(istrip<0||istrip>NSQ) return 0;
   if(Z<1||Z>NZQ) return 0;
   return kLoss[icalib][iside][istrip][catg][Z-1];
}
int TrChargeLossDBH::FindRefLRZ(int &Zright,int Zinput,int iside,int catg,int istrip,int icalib){
   int zleft=0;
   int zright=0;
   for(int Z=Zinput;Z>0;Z--){
      Fit2DH* pmap=GetMap(iside,catg,istrip,Z,icalib);
      if(pmap){zleft=Z; break;}
   }
   for(int Z=Zinput+1;Z<=NZQ;Z++){
      Fit2DH* pmap=GetMap(iside,catg,istrip,Z,icalib);
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
            Fit2DH* pmap=GetMap(iside,catg,istrip,Z,icalib);
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
double TrChargeLossDBH::GetValue(int iside,int catg,int istrip,int Z,int icalib,double ip,double ia,bool UseNewAlgo){
   Fit2DH* pmap=GetMap(iside,catg,istrip,Z,icalib);
   double error;
   return pmap?pmap->Eval(error,ip,ia,UseNewAlgo):0;
}
TGraph* TrChargeLossDBH::GetCorrectionGraph(int iside,int catg,int istrip,double ip,double ia,int icalib,bool IsSquare,bool UseNewAlgo){
   TGraph* gr=new TGraph();
   int np=0;
   gr->SetPoint(np,0.,0.);
   np++;
   double limits=0.2;
   double xpre=0,ypre=0;
   for(int iz=0;iz<NZQ;iz++){
      if((iz+1<=26)&&TrChargeLossDBH::Ignorelist(iside,istrip,iz+1)) continue;
      Fit2DH* fit=GetMap(iside,catg,istrip,iz+1,icalib);
      if(!fit) continue;
      double error;
      double refadc=fit->Eval(error,ip,ia,UseNewAlgo);
      if(refadc<=0) continue;
      if(icalib>0){
         if(fabs(refadc/(iz+1)-1)>limits) continue;
         if(np<=0) {xpre=iz+1; ypre=refadc;}
         else{
            if((iz+1-xpre)*(refadc-ypre)<=0) continue;
            else {xpre=iz+1; ypre=refadc;}
         }
      }
      gr->SetPoint(np,IsSquare?pow((iz+1.),2):(iz+1),refadc);
      np++;
   }
   if(np>3) return gr;
   else{ delete gr; return 0;}
}
MSplineH* TrChargeLossDBH::GetCorrection(int iside,int catg,int istrip,double ip,double ia,int icalib,bool IsSquare,bool UseNewAlgo){
   TGraph* gr=GetCorrectionGraph(iside,catg,istrip,ip,ia,icalib,IsSquare,UseNewAlgo);
   if(!gr) return 0;
   MSplineH* spl=new MSplineH((TGraphErrors*)gr,false,false,MSplineH::kLinear,MSplineH::kLinear,0.,0.);
   delete gr;
   return spl;
}

int TrChargeLossDBH::GetCategory(int iside,bool IsK7, int sensoraddress,double ip){
   if(iside==1) return 0;
   if(!IsK7) return 0;
   else{
      int ik=-1;
      if(sensoraddress>64&&sensoraddress<=159) ik=1;
      else if((sensoraddress%2==0)&&ip>=0) ik=2;
      else if((sensoraddress%2==0)&&ip<0) ik=3;
      else if((sensoraddress%2!=0)&&ip<=0) ik=2;
      else if((sensoraddress%2!=0)&&ip>0) ik=3;
      return ik;
   }
}

int TrChargeLossDBH::GetStripIndex(int seedindex,int stripindex,double ip){
   int istrip0=-1;
   if(ip>=0.5) ip=0.4999;
   if(ip<=-0.5) ip=-0.4999;
   if(stripindex>=NSQ) return NSQ;
   if(!foldeta){
      int res=stripindex-seedindex+(NSQ-1)/2;
      return res;
   }
   if(!usedis){
      int res=abs(stripindex-seedindex);
      return res;
   }
   double DisToIp=fabs(stripindex-(seedindex+ip));
   for(int ii=0;ii<NSQ;ii++){
      if(DisToIp>=(ii/2.)&&DisToIp<(ii/2.)+0.5) {istrip0=ii; break;}
   }
   return istrip0;
}
bool TrChargeLossDBH::IsPositive(int istrip,bool useright){
   if(istrip>=NSQ) return useright;
   bool folded=usedis||((!usedis)&&foldeta);
   if(!folded) return false;
   if((!usedis)&&foldeta){
      if(istrip!=0) return false;
      else return useright;
   }
   bool positive;
   if(istrip==0) positive=useright;
   else{
      if(istrip%2==1) positive=useright;
      else positive=(!useright);
   }
   return positive;
}

double TrChargeLossDBH::GetChargeLossCorrectedValue(int iside,int catg,int istrip,double ip,double ia,double adc,double Z0,int icalib,bool UseNewAlgo){
   if(adc<=0) return 0.;

   double charge=0;

   bool IsSquare=(icalib==0);

   if(UseNewAlgo){
      //if(Z0>0&&IsSquare) Z0=Z0*Z0;
      //TGraph* spl=GetCorrectionGraph(iside,catg,istrip,ip,ia,icalib,IsSquare,UseNewAlgo);
      //charge=GetInvertedValue(spl,adc,Z0);
      //if(charge<0) charge=0;
      //if(IsSquare) charge=sqrt(charge);
      //return charge;

      if(icalib==0&&Z0<=0) return 0; //due to some strange behavier in y side, we need a reference charge for the first charge calibration
      double Qinput=Z0>0?Z0:adc;
      int Zinput=int(Qinput);
      int Zlref;
      int Zrref;
      Zlref=FindRefLRZ(Zrref,Zinput,iside,catg,istrip,icalib);
      if(Zrref<=0) return icalib==0?0:adc;
      double qlref=(Zlref<=0)?0:GetValue(iside,catg,istrip,Zlref,icalib,ip,ia,UseNewAlgo);
      double qrref=(Zrref<=0)?0:GetValue(iside,catg,istrip,Zrref,icalib,ip,ia,UseNewAlgo);
      double slope=(qrref-qlref)/(IsSquare?(Zrref*Zrref-Zlref*Zlref):(Zrref-Zlref));
      double logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(pow(Zrref*1./Zlref,IsSquare?2:1)));
      double slplimit=istrip==NSQ?0.95:0.7;
      double fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
      int clock=30;
      while( !( (adc>=qlref&&adc<=qrref) && fineslope ) ){
         //printf("Zlref={%d,%.2lf} Zrref={%d,%.2lf} slope={%.2lf,%.2lf} adc=%.2f qin=%.2lf\n",Zlref,qlref,Zrref,qrref,slope,logslope,adc,Qinput);
         if(!fineslope) break;
         if(adc<qlref){
            Zrref=Zlref; qrref=qlref;
            int Zbuff;
            Zlref=FindRefLRZ(Zbuff,Zrref-1,iside,catg,istrip,icalib);
            qlref=(Zlref<=0)?0:GetValue(iside,catg,istrip,Zlref,icalib,ip,ia,UseNewAlgo);
            slope=(qrref-qlref)/(IsSquare?(Zrref*Zrref-Zlref*Zlref):(Zrref-Zlref));
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(pow(Zrref*1./Zlref,IsSquare?2:1)));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         if(adc>qrref){
            Zlref=Zrref; qlref=qrref;
            int Zbuff;
            Zrref=FindRefLRZ(Zbuff,Zlref,iside,catg,istrip,icalib);
            if(Zrref<=Zlref){  //already the last Z;
               if(IsSquare){ charge=sqrt(Zlref*Zlref+(adc-qlref)/slope); return charge<100?charge:(icalib==0?0:adc);}
               else{ charge=Zlref+(adc-qlref)/slope; return charge<100?charge:(icalib==0?0:adc);}
            }
            qrref=(Zrref<=0)?0:GetValue(iside,catg,istrip,Zrref,icalib,ip,ia,UseNewAlgo);
            slope=(qrref-qlref)/(IsSquare?(Zrref*Zrref-Zlref*Zlref):(Zrref-Zlref));
            logslope=(Zlref<=0||qlref<=0)?1.0e20:(log10(qrref/qlref)/log10(pow(Zrref*1./Zlref,IsSquare?2:1)));
            fineslope=fabs(logslope-1)<slplimit||logslope>1.0e10;
         }
         clock--;
         if(clock<0) break;
      }
      //printf("Outloop:Zlref={%d,%.2lf} Zrref={%d,%.2lf} slope={%.2lf,%.2lf} adc=%.2f qin=%.2lf\n",Zlref,qlref,Zrref,qrref,slope,logslope,adc,Qinput);
      if(!fineslope) return (icalib==0?0:adc);
      if(adc>=qlref&&adc<=qrref){
         if(IsSquare) return sqrt(Zlref*Zlref-(qlref-adc)/(qlref-qrref)*(Zlref*Zlref-Zrref*Zrref));
         else return Zlref-(qlref-adc)/(qlref-qrref)*(Zlref-Zrref);
      }
      else return (icalib==0?0:adc);
   }

   if(Z0>0&&IsSquare) Z0=Z0*Z0;
   MSplineH* spl=GetCorrection(iside,catg,istrip,ip,ia,icalib,IsSquare,UseNewAlgo);
   if(!spl){
      if(icalib<=0) return 0;
      else return adc;
   }
   int nsol;
   double xsol[20];
   spl->EvalInverseMultiple(adc,nsol,xsol,Z0);
   if(Z0==0&&nsol>1){
      if(xsol[nsol-1]>spl->fX[spl->fN-1]) nsol--;
   }

   double xmin=0,xmax=0,ymin=0,ymax=0,xtmin=0,xtmax=0;
   bool IsMult=spl->MultipleLimit(ymin,ymax,xmin,xmax,xtmin,xtmax);
   bool IsAbNorm=false;
   if(!IsMult){
     if(nsol==1) charge=xsol[0];
     else IsAbNorm=true;
   }
   else{
      if(Z0>0){
         if(xtmin>spl->fX[spl->fN-1]) IsAbNorm=true;  //the gainline is always monotonic
         if(xtmax<spl->fX[0]) IsAbNorm=true;     //the gainline is always monotonic
         if(nsol==1) charge=xsol[0];
      }
      else{
         if(adc<ymin*0.96&&nsol==1) charge=xsol[0];
         if(adc>ymax*1.04&&nsol==1) charge=xsol[0];
      }
   }
   //if(IsAbNorm) printf("This shouldn't happen!\n");
   if(spl) {delete spl;}
   double res=charge>=0?(IsSquare?sqrt(charge):charge):0;
   //printf("adc=%.2f qloss=%.2f (eta=%.2f theta=%.2f,%s,S%d,K%d)\n",adc,res,ip,ia,iside==0?"x":"y",istrip,catg);
   return res;
}
