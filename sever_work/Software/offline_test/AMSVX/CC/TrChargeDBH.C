#include "TrChargeDBH.h"
#include "TkDBc.h"
#include "TkCoo.h"
#include "stdlib.h"
#include "root.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "event.h"
#endif
#include "commonsi.h"

bool TrChargeDBH::UseTrChargeH=true;
int TrChargeDBH::IsMC=0;
int TrChargeDBH::fUseVersion=12;
bool TrChargeDBH::UseTotal=false;
bool TrChargeDBH::Useqtuning=false;
bool TrChargeDBH::UseMSplineH=false;

mean_t TrChargeDBH::GetMean(int type, const vector<double>& signal,int iside) {
  if      ( ( (type&kPlainMean))&&(!(type&kTruncMean))&&(!(type&kGaussMean)) ) return GetPlainMean(signal);
  else if ( (!(type&kPlainMean))&&( (type&kTruncMean))&&(!(type&kGaussMean)) ) return GetTruncMean(signal);
  else if ( (!(type&kPlainMean))&&(!(type&kTruncMean))&&( (type&kGaussMean)) ) return GetGaussMean(signal);
  else if ( (type&kDropOne) ) return GetDropOne(signal);
  else if ( (type&kDropFaraway1) ) return GetDropFaraway1(signal,type,iside);
  else if ( (type&kDropFaraway2) ) return GetDropFaraway2(signal,type,iside);
  else    printf("TrChargeDBH::GetMean-W not a valid mean type selected, returning empty mean_t, %2x.\n",type);
  return mean_t();
}

mean_t TrChargeDBH::GetPlainMean(const vector<double>& signal) {
  int   n    = 0;
  double mean = 0;
  double rms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2);
    n++;
  }
  mean /= n;
  rms  /= n;
  rms = sqrt(rms - mean*mean);
  if (n==0) return mean_t();
  return mean_t(kPlainMean,-1,-1,n,mean,rms);
}


mean_t TrChargeDBH::GetTruncMean(const vector<double>& signal) {
  int   n       = 0;
  double mean    = 0;
  double maxampl = 0;
  double rms     = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if (signal.at(ii)>maxampl) maxampl = signal.at(ii);
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2);
    n++;
  }
  if (n<=1) return mean_t();
  mean = (mean - maxampl)/(n-1);
  rms  = (rms - pow(maxampl,2))/(n-1);
  rms  = sqrt(fabs(rms - mean*mean)); // avoids problem in case of zeroes    
  return mean_t(kTruncMean,-1,-1,n-1,mean,rms);
}

mean_t TrChargeDBH::GetGaussMean(const vector<double>& signal) {
  // calculate mean and rms
  int   n    = 0;
  double mean = 0;
  double rms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms += pow(signal.at(ii),2);
    n++;
  }
  if (n==0) return mean_t();
  // iterate selecting clusters
  mean /= n;
  rms  /= n;
  rms = sqrt(rms - mean*mean);
  int   gaussn    = 0;
  double gaussmean = 0;
  double gaussrms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if ((signal[ii]-mean)>3*rms) continue; // 3 sigma
    gaussmean += signal.at(ii);
    gaussrms  += pow(signal.at(ii),2);
    gaussn++;
  }
  if (gaussn==0) return mean_t();
  gaussmean /= gaussn;
  gaussrms  /= gaussn;
  gaussrms = sqrt(gaussrms - gaussmean*gaussmean);
  return mean_t(kGaussMean,-1,-1,gaussn,gaussmean,gaussrms);
}

mean_t TrChargeDBH::GetDropOne(const vector<double>& signal) {
   int nn=0;
   double mean=0;
   double rms=0;
   for(int ii=0; ii<(int)signal.size(); ii++){
      if((int)signal.size()>2){
         int ni=0;
         double meani=0;
         double rmsi=0;
         for(int i2=0; i2<(int)signal.size(); i2++){
            if(ii==i2) continue;
            if(signal.at(i2)<0) continue;
            meani+=signal.at(i2);
            rmsi+=pow(signal.at(i2),2);
            ni++;
         }
         if(ni>0){
            meani/=ni;
            rmsi/=ni;
            rmsi=sqrt(rmsi-meani*meani);
         }
         if(rmsi>0){
            if((rms>0&&rms>rmsi)||(rms<=0)){
               nn=ni;
               mean=meani;
               rms=rmsi;
            }
         }
      }
      else{
         if(signal.at(ii)<0) continue;
         mean+=signal.at(ii);
         rms+=pow(signal.at(ii),2);
         nn++;
      }
   }
   if(nn==0) return mean_t();
   if((int)signal.size()<=2){
      mean/=nn;
      rms/=nn;
      rms=sqrt(rms-mean*mean);
   }
   return mean_t(kDropOne,-1,-1,nn,mean,rms);
}

mean_t TrChargeDBH::GetDropFaraway1(const vector<double>& signal,int Opt,int iside) {
  //find the lowest and highest one
  int indexfar[2]={-1,-1};
  double qmin=10000;
  double qmax=0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if(signal.at(ii)>qmax) { qmax=signal.at(ii); indexfar[1]=ii; }
    if(signal.at(ii)<qmin) { qmin=signal.at(ii); indexfar[0]=ii; }
  }
  if(indexfar[0]<0||indexfar[1]<0) return mean_t();

  //get some mean
  int nn=0;
  double mean=0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if(ii==indexfar[0]||ii==indexfar[1]) continue;
    mean+=signal.at(ii);
    nn++;
  }
  if (nn==0){
    int    n0    = 0;
    double mean0 = 0;
    double rms0  = 0;
    for (int ii=0; ii<(int)signal.size(); ii++) {
      if (signal.at(ii)<0) continue;
      mean0+=signal.at(ii);
      rms0+=pow(signal.at(ii),2);
      n0++;
    }
    mean0/=n0;
    rms0/=n0;
    rms0=sqrt(rms0-mean0*mean0);
    return mean_t(kDropFaraway1,-1,-1,n0,mean0,rms0);
  }
  mean /= nn;

  //remove lowest or highest when they are too far away
  int   n2    = 0;
  double mean2 = 0;
  double rms2  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if(ii==indexfar[0]||ii==indexfar[1]){
       double tmean=mean;
       double trms=(tmean*0.2);
       MSplineH* spl=0;
       if(iside<0||iside>1){
          spl=((Opt&kUnbias)&&(!TrChargeDBH::IsMC))?fSigmAllUnb:fSigmAll;
          if((Opt&kSmear)&&TrChargeDBH::IsMC){
             spl=(Opt&kUnbias)?fTunedSigmAllUnb:fTunedSigmAll;
          }
       }
       else{
          spl=((Opt&kUnbias)&&(!TrChargeDBH::IsMC))?fSigmXYUnb[iside]:fSigmXY[iside];
          if((Opt&kSmear)&&TrChargeDBH::IsMC){
             spl=(Opt&kUnbias)?fTunedSigmXYUnb[iside]:fTunedSigmXY[iside];
          }
       }
       if(spl) trms=spl->Eval(tmean);
       if ( (fabs(signal.at(ii)-tmean)>3*trms)||(tmean<4.7&&ii==indexfar[1]) ) continue; // 3 sigma
    }
    mean2 += signal.at(ii);
    rms2  += pow(signal.at(ii),2);
    n2++;
  }
  if (n2==0) return mean_t();
  mean2 /= n2;
  rms2  /= n2;
  rms2 = sqrt(rms2 - mean2*mean2);
  return mean_t(kDropFaraway1,-1,-1,n2,mean2,rms2);
}
mean_t TrChargeDBH::GetDropFaraway2(const vector<double>& signal,int Opt,int iside) {
  //find the lowest and highest one
  int indexfar[2]={-1,-1};
  double qmin=10000;
  double qmax=0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if(signal.at(ii)>qmax) { qmax=signal.at(ii); indexfar[1]=ii; }
    if(signal.at(ii)<qmin) { qmin=signal.at(ii); indexfar[0]=ii; }
  }
  if(indexfar[0]<0||indexfar[1]<0) return mean_t();

  //get some mean
  int nn=0;
  double mean=0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if(ii==indexfar[0]||ii==indexfar[1]) continue;
    mean+=signal.at(ii);
    nn++;
  }
  if (nn==0){
    int    n0    = 0;
    double mean0 = 0;
    double rms0  = 0;
    for (int ii=0; ii<(int)signal.size(); ii++) {
      if (signal.at(ii)<0) continue;
      mean0+=signal.at(ii);
      rms0+=pow(signal.at(ii),2);
      n0++;
    }
    mean0/=n0;
    rms0/=n0;
    rms0=sqrt(rms0-mean0*mean0);
    return mean_t(kDropFaraway2,-1,-1,n0,mean0,rms0);
  }
  mean /= nn;

  //remove one of lowest or highest when they are too far away
  int   n2    = 0;
  double mean2 = 0;
  double rms2  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if(ii==indexfar[0]||ii==indexfar[1]){
       double tmean=mean;
       double trms=(tmean*0.2);
       MSplineH* spl=0;
       if(iside<0||iside>1){
          spl=((Opt&kUnbias)&&(!TrChargeDBH::IsMC))?fSigmAllUnb:fSigmAll;
          if((Opt&kSmear)&&TrChargeDBH::IsMC){
             spl=(Opt&kUnbias)?fTunedSigmAllUnb:fTunedSigmAll;
          }
       }
       else{
          spl=((Opt&kUnbias)&&(!TrChargeDBH::IsMC))?fSigmXYUnb[iside]:fSigmXY[iside];
          if((Opt&kSmear)&&TrChargeDBH::IsMC){
             spl=(Opt&kUnbias)?fTunedSigmXYUnb[iside]:fTunedSigmXY[iside];
          }
       }
       if(spl) trms=spl->Eval(tmean);
       bool removelow=fabs(signal.at(indexfar[0])-tmean)>fabs(signal.at(indexfar[1])-tmean);
       if(tmean<4.5) removelow=false;
       if ( (fabs(signal.at(ii)-tmean)>3*trms)||(tmean<4.7) ){
          if(removelow&&ii==indexfar[0]) continue;
          if((!removelow)&&ii==indexfar[1]) continue;
       }
    }
    mean2 += signal.at(ii);
    rms2  += pow(signal.at(ii),2);
    n2++;
  }
  if (n2==0) return mean_t();
  mean2 /= n2;
  rms2  /= n2;
  rms2 = sqrt(rms2 - mean2*mean2);
  return mean_t(kDropFaraway2,-1,-1,n2,mean2,rms2);
}

TrChargeDBH* TrChargeDBH::fHead=0;
MSplineH* TrChargeDBH::fGainTot[2];
TrAsymmDBH* TrChargeDBH::fkAsymm=0;
TrChargeLossDBH* TrChargeDBH::fkLoss=0;
TrGainCHDBH* TrChargeDBH::fkGainCH=0;
TrGainDBH* TrChargeDBH::fkGain=0;
TrThetaDBH* TrChargeDBH::fkTheta=0;
TrTempDBH* TrChargeDBH::fkTemp=0;

TrBetaDBH* TrChargeDBH::fkBeta=0;
TrRigidityDBH* TrChargeDBH::fkRigidity=0;

MSplineH* TrChargeDBH::fPeakCorr[9][2];
MSplineH* TrChargeDBH::fPeak[2][NSQ+1];
MSplineH* TrChargeDBH::fSigm[2][NSQ+1];
MSplineH* TrChargeDBH::fPeakXY[2];
MSplineH* TrChargeDBH::fSigmXY[2];
MSplineH* TrChargeDBH::fPeakAll;
MSplineH* TrChargeDBH::fSigmAll;
MSplineH* TrChargeDBH::fWeightLayer[NSQ+1][9][2];
MSplineH* TrChargeDBH::fParsBestLayer[9];
MSplineH* TrChargeDBH::fParsBestInner;
MSplineH* TrChargeDBH::fParsBestFS;

MSplineH* TrChargeDBH::fPeakCorrUnb[9][2];
MSplineH* TrChargeDBH::fPeakUnb[2][NSQ+1];
MSplineH* TrChargeDBH::fSigmUnb[2][NSQ+1];
MSplineH* TrChargeDBH::fPeakXYUnb[2];
MSplineH* TrChargeDBH::fSigmXYUnb[2];
MSplineH* TrChargeDBH::fPeakAllUnb;
MSplineH* TrChargeDBH::fSigmAllUnb;
MSplineH* TrChargeDBH::fWeightLayerUnb[NSQ+1][9][2];
MSplineH* TrChargeDBH::fParsBestLayerUnb[9];
MSplineH* TrChargeDBH::fParsBestInnerUnb;
MSplineH* TrChargeDBH::fParsBestFSUnb;

MSplineH* TrChargeDBH::fMCNormLayer[NMCQpars][9][3];
MSplineH* TrChargeDBH::fMCPeakLayer[NMCQpars][9][3];
MSplineH* TrChargeDBH::fMCSigmLayer[NMCQpars][9][3];
MSplineH* TrChargeDBH::fMCNormInner[NMCQpars][3];
MSplineH* TrChargeDBH::fMCPeakInner[NMCQpars][3];
MSplineH* TrChargeDBH::fMCSigmInner[NMCQpars][3];
MSplineH* TrChargeDBH::fMCNormAll[NMCQpars][3];
MSplineH* TrChargeDBH::fMCPeakAll[NMCQpars][3];
MSplineH* TrChargeDBH::fMCSigmAll[NMCQpars][3];

MSplineH* TrChargeDBH::fMCNormLayerUnb[NMCQpars][9][3];
MSplineH* TrChargeDBH::fMCPeakLayerUnb[NMCQpars][9][3];
MSplineH* TrChargeDBH::fMCSigmLayerUnb[NMCQpars][9][3];
MSplineH* TrChargeDBH::fMCNormInnerUnb[NMCQpars][3];
MSplineH* TrChargeDBH::fMCPeakInnerUnb[NMCQpars][3];
MSplineH* TrChargeDBH::fMCSigmInnerUnb[NMCQpars][3];
MSplineH* TrChargeDBH::fMCNormAllUnb[NMCQpars][3];
MSplineH* TrChargeDBH::fMCPeakAllUnb[NMCQpars][3];
MSplineH* TrChargeDBH::fMCSigmAllUnb[NMCQpars][3];

MSplineH* TrChargeDBH::kMCConvLayer[9][3][NZQ];
MSplineH* TrChargeDBH::kMCConvInner[3][NZQ];
MSplineH* TrChargeDBH::kMCConvAll[3][NZQ];

MSplineH* TrChargeDBH::kMCConvLayerUnb[9][3][NZQ];
MSplineH* TrChargeDBH::kMCConvInnerUnb[3][NZQ];
MSplineH* TrChargeDBH::kMCConvAllUnb[3][NZQ];

MSplineH* TrChargeDBH::fTunedPeakXY[2];
MSplineH* TrChargeDBH::fTunedSigmXY[2];
MSplineH* TrChargeDBH::fTunedPeakAll;
MSplineH* TrChargeDBH::fTunedSigmAll;
MSplineH* TrChargeDBH::fTunedParsBestLayer[9];
MSplineH* TrChargeDBH::fTunedParsBestInner;
MSplineH* TrChargeDBH::fTunedParsBestFS;
MSplineH* TrChargeDBH::fTunedPeakXYUnb[2];
MSplineH* TrChargeDBH::fTunedSigmXYUnb[2];
MSplineH* TrChargeDBH::fTunedPeakAllUnb;
MSplineH* TrChargeDBH::fTunedSigmAllUnb;
MSplineH* TrChargeDBH::fTunedParsBestLayerUnb[9];
MSplineH* TrChargeDBH::fTunedParsBestInnerUnb;
MSplineH* TrChargeDBH::fTunedParsBestFSUnb;

int TrChargeDBH::run_current=0;
int TrChargeDBH::time_current=0;

int TrChargeDBH::xcatg;
int TrChargeDBH::addr_kinit[2][NSQ+1];
double TrChargeDBH::eta_kinit[2];
double TrChargeDBH::theta_kinit[2];
double TrChargeDBH::charge_kinit[2][NSQ+1];
double TrChargeDBH::eta_kasymm[2][NSQ+1];
double TrChargeDBH::theta_kasymm[2][NSQ+1][2];
double TrChargeDBH::charge_kasymm[2][NSQ+1];

double TrChargeDBH::charge_kloss[2][NSQ+1][NCalib];
double TrChargeDBH::charge_kgainch[2][NSQ+1][NCalib];
double TrChargeDBH::charge_kgain[2][NSQ+1][NCalib];
double TrChargeDBH::charge_ktheta[2][NSQ+1][NCalib];
double TrChargeDBH::charge_ktemp[2][NSQ+1][NCalib];
double TrChargeDBH::charge_kall[2][NSQ+1];
double TrChargeDBH::charge_kxy[3];

double TrChargeDBH::charge_kbeta[3];
double TrChargeDBH::charge_krigidity[3];

void TrChargeDBH::Init(){
   xcatg=0;
   for(int iside=0;iside<2;iside++){
      eta_kinit[iside]=0;
      theta_kinit[iside]=0;
      for(int istrip=0;istrip<=NSQ;istrip++){
         fPeak[iside][istrip]=0;
         fSigm[iside][istrip]=0;
         fPeakUnb[iside][istrip]=0;
         fSigmUnb[iside][istrip]=0;
         for(int ilay=0;ilay<9;ilay++){
            fWeightLayer[istrip][ilay][iside]=0;
            fWeightLayerUnb[istrip][ilay][iside]=0;
         }

         addr_kinit[iside][istrip]=0;
         charge_kinit[iside][istrip]=0;
         eta_kasymm[iside][istrip]=0;
         theta_kasymm[iside][istrip][0]=0;
         theta_kasymm[iside][istrip][1]=0;
         charge_kasymm[iside][istrip]=0;

         for(int icalib=0;icalib<NCalib;icalib++){
             charge_kloss[iside][istrip][icalib]=0;
           charge_kgainch[iside][istrip][icalib]=0;
             charge_kgain[iside][istrip][icalib]=0;
            charge_ktheta[iside][istrip][icalib]=0;
            charge_ktemp[iside][istrip][icalib]=0;
         }
         charge_kall[iside][istrip]=0;
      }
      charge_kxy[iside]=0;
      charge_kbeta[iside]=0;
      charge_krigidity[iside]=0;

      fPeakXY[iside]=0;
      fSigmXY[iside]=0;
      fPeakXYUnb[iside]=0;
      fSigmXYUnb[iside]=0;
      fTunedPeakXY[iside]=0;
      fTunedSigmXY[iside]=0;
      fTunedPeakXYUnb[iside]=0;
      fTunedSigmXYUnb[iside]=0;

      for(int ilay=0;ilay<9;ilay++){
         fPeakCorr[ilay][iside]=0;
         fPeakCorrUnb[ilay][iside]=0;
      }
   }
   charge_kxy[2]=0;
   charge_kbeta[2]=0;
   charge_krigidity[2]=0;

   fPeakAll=0;
   fSigmAll=0;
   fPeakAllUnb=0;
   fSigmAllUnb=0;
   fTunedPeakAll=0;
   fTunedSigmAll=0;
   fTunedPeakAllUnb=0;
   fTunedSigmAllUnb=0;
   for(int ilay=0;ilay<9;ilay++){
      fParsBestLayer[ilay]=0;
      fParsBestLayerUnb[ilay]=0;
      fTunedParsBestLayer[ilay]=0;
      fTunedParsBestLayerUnb[ilay]=0;
   }
   fParsBestInner=0;
   fParsBestFS=0;
   fParsBestInnerUnb=0;
   fParsBestFSUnb=0;
   fTunedParsBestInner=0;
   fTunedParsBestFS=0;
   fTunedParsBestInnerUnb=0;
   fTunedParsBestFSUnb=0;

   for(int ii=0;ii<NMCQpars;ii++){
   for(int iside=0;iside<3;iside++){
      fMCNormInner[ii][iside]=0;
      fMCPeakInner[ii][iside]=0;
      fMCSigmInner[ii][iside]=0;
      fMCNormAll[ii][iside]=0;
      fMCPeakAll[ii][iside]=0;
      fMCSigmAll[ii][iside]=0;
      fMCNormInnerUnb[ii][iside]=0;
      fMCPeakInnerUnb[ii][iside]=0;
      fMCSigmInnerUnb[ii][iside]=0;
      fMCNormAllUnb[ii][iside]=0;
      fMCPeakAllUnb[ii][iside]=0;
      fMCSigmAllUnb[ii][iside]=0;
      for(int ilay=0;ilay<9;ilay++){
         fMCNormLayer[ii][ilay][iside]=0;
         fMCPeakLayer[ii][ilay][iside]=0;
         fMCSigmLayer[ii][ilay][iside]=0;
         fMCNormLayerUnb[ii][ilay][iside]=0;
         fMCPeakLayerUnb[ii][ilay][iside]=0;
         fMCSigmLayerUnb[ii][ilay][iside]=0;
      }
   }
   }

   for(int iz=0;iz<NZQ;iz++){
      for(int iside=0;iside<3;iside++){
         kMCConvInner[iside][iz]=0;
         kMCConvInnerUnb[iside][iz]=0;
         kMCConvAll[iside][iz]=0;
         kMCConvAllUnb[iside][iz]=0;
         for(int ilay=0;ilay<9;ilay++){
            kMCConvLayer[ilay][iside][iz]=0;
            kMCConvLayerUnb[ilay][iside][iz]=0;
         }
      }
   }

   fHead=0;
   fGainTot[0]=0;
   fGainTot[1]=0;
   fkAsymm=0;
   fkLoss=0;
   fkGainCH=0;
   fkGain=0;
   fkTheta=0;
   fkTemp=0;

   fkBeta=0;
   fkRigidity=0;
}
void TrChargeDBH::Release(){
   for(int iside=0;iside<2;iside++){
      for(int istrip=0;istrip<=NSQ;istrip++){
         if(fPeak[iside][istrip]) {delete fPeak[iside][istrip]; fPeak[iside][istrip]=0;}
         if(fSigm[iside][istrip]) {delete fSigm[iside][istrip]; fSigm[iside][istrip]=0;}
         if(fPeakUnb[iside][istrip]) {delete fPeakUnb[iside][istrip]; fPeakUnb[iside][istrip]=0;}
         if(fSigmUnb[iside][istrip]) {delete fSigmUnb[iside][istrip]; fSigmUnb[iside][istrip]=0;}
         for(int ilay=0;ilay<9;ilay++){
            if(fWeightLayer[istrip][ilay][iside]) { delete fWeightLayer[istrip][ilay][iside]; fWeightLayer[istrip][ilay][iside]=0;}
            if(fWeightLayerUnb[istrip][ilay][iside]) { delete fWeightLayerUnb[istrip][ilay][iside]; fWeightLayerUnb[istrip][ilay][iside]=0;}
         }
      }
      if(fPeakXY[iside]) {delete fPeakXY[iside]; fPeakXY[iside]=0;}
      if(fSigmXY[iside]) {delete fSigmXY[iside]; fSigmXY[iside]=0;}
      if(fPeakXYUnb[iside]) {delete fPeakXYUnb[iside]; fPeakXYUnb[iside]=0;}
      if(fSigmXYUnb[iside]) {delete fSigmXYUnb[iside]; fSigmXYUnb[iside]=0;}
      if(fTunedPeakXY[iside]) {delete fTunedPeakXY[iside]; fTunedPeakXY[iside]=0;}
      if(fTunedSigmXY[iside]) {delete fTunedSigmXY[iside]; fTunedSigmXY[iside]=0;}
      if(fTunedPeakXYUnb[iside]) {delete fTunedPeakXYUnb[iside]; fTunedPeakXYUnb[iside]=0;}
      if(fTunedSigmXYUnb[iside]) {delete fTunedSigmXYUnb[iside]; fTunedSigmXYUnb[iside]=0;}

      for(int ilay=0;ilay<9;ilay++){
         if(fPeakCorr[ilay][iside]) {delete fPeakCorr[ilay][iside]; fPeakCorr[ilay][iside]=0;}
         if(fPeakCorrUnb[ilay][iside]) {delete fPeakCorrUnb[ilay][iside]; fPeakCorrUnb[ilay][iside]=0;}
      }
   }
   if(fPeakAll) {delete fPeakAll; fPeakAll=0;}
   if(fSigmAll) {delete fSigmAll; fSigmAll=0;}
   if(fPeakAllUnb) {delete fPeakAllUnb; fPeakAllUnb=0;}
   if(fSigmAllUnb) {delete fSigmAllUnb; fSigmAllUnb=0;}
   if(fTunedPeakAll) {delete fTunedPeakAll; fTunedPeakAll=0;}
   if(fTunedSigmAll) {delete fTunedSigmAll; fTunedSigmAll=0;}
   if(fTunedPeakAllUnb) {delete fTunedPeakAllUnb; fTunedPeakAllUnb=0;}
   if(fTunedSigmAllUnb) {delete fTunedSigmAllUnb; fTunedSigmAllUnb=0;}

   for(int ilay=0;ilay<9;ilay++){
      if(fParsBestLayer[ilay]) {delete fParsBestLayer[ilay]; fParsBestLayer[ilay]=0;}
      if(fParsBestLayerUnb[ilay]) {delete fParsBestLayerUnb[ilay]; fParsBestLayerUnb[ilay]=0;}
      if(fTunedParsBestLayer[ilay]) {delete fTunedParsBestLayer[ilay]; fTunedParsBestLayer[ilay]=0;}
      if(fTunedParsBestLayerUnb[ilay]) {delete fTunedParsBestLayerUnb[ilay]; fTunedParsBestLayerUnb[ilay]=0;}
   }
   if(fParsBestInner) {delete fParsBestInner; fParsBestInner=0;}
   if(fParsBestFS) {delete fParsBestFS; fParsBestFS=0;}
   if(fParsBestInnerUnb) {delete fParsBestInnerUnb; fParsBestInnerUnb=0;}
   if(fParsBestFSUnb) {delete fParsBestFSUnb; fParsBestFSUnb=0;}
   if(fTunedParsBestInner) {delete fTunedParsBestInner; fTunedParsBestInner=0;}
   if(fTunedParsBestFS) {delete fTunedParsBestFS; fTunedParsBestFS=0;}
   if(fTunedParsBestInnerUnb) {delete fTunedParsBestInnerUnb; fTunedParsBestInnerUnb=0;}
   if(fTunedParsBestFSUnb) {delete fTunedParsBestFSUnb; fTunedParsBestFSUnb=0;}

   for(int ii=0;ii<NMCQpars;ii++){
   for(int iside=0;iside<3;iside++){
      if(fMCNormInner[ii][iside]) {delete fMCNormInner[ii][iside]; fMCNormInner[ii][iside]=0;}
      if(fMCPeakInner[ii][iside]) {delete fMCPeakInner[ii][iside]; fMCPeakInner[ii][iside]=0;}
      if(fMCSigmInner[ii][iside]) {delete fMCSigmInner[ii][iside]; fMCSigmInner[ii][iside]=0;}
      if(fMCNormAll[ii][iside])   {delete fMCNormAll[ii][iside];   fMCNormAll[ii][iside]=0;  }
      if(fMCPeakAll[ii][iside])   {delete fMCPeakAll[ii][iside];   fMCPeakAll[ii][iside]=0;  }
      if(fMCSigmAll[ii][iside])   {delete fMCSigmAll[ii][iside];   fMCSigmAll[ii][iside]=0;  }
      if(fMCNormInnerUnb[ii][iside]) {delete fMCNormInnerUnb[ii][iside]; fMCNormInnerUnb[ii][iside]=0;}
      if(fMCPeakInnerUnb[ii][iside]) {delete fMCPeakInnerUnb[ii][iside]; fMCPeakInnerUnb[ii][iside]=0;}
      if(fMCSigmInnerUnb[ii][iside]) {delete fMCSigmInnerUnb[ii][iside]; fMCSigmInnerUnb[ii][iside]=0;}
      if(fMCNormAllUnb[ii][iside])   {delete fMCNormAllUnb[ii][iside];   fMCNormAllUnb[ii][iside]=0;  }
      if(fMCPeakAllUnb[ii][iside])   {delete fMCPeakAllUnb[ii][iside];   fMCPeakAllUnb[ii][iside]=0;  }
      if(fMCSigmAllUnb[ii][iside])   {delete fMCSigmAllUnb[ii][iside];   fMCSigmAllUnb[ii][iside]=0;  }
      for(int ilay=0;ilay<9;ilay++){
         if(fMCNormLayer[ii][ilay][iside]) {delete fMCNormLayer[ii][ilay][iside]; fMCNormLayer[ii][ilay][iside]=0;}
         if(fMCPeakLayer[ii][ilay][iside]) {delete fMCPeakLayer[ii][ilay][iside]; fMCPeakLayer[ii][ilay][iside]=0;}
         if(fMCSigmLayer[ii][ilay][iside]) {delete fMCSigmLayer[ii][ilay][iside]; fMCSigmLayer[ii][ilay][iside]=0;}
         if(fMCNormLayerUnb[ii][ilay][iside]) {delete fMCNormLayerUnb[ii][ilay][iside]; fMCNormLayerUnb[ii][ilay][iside]=0;}
         if(fMCPeakLayerUnb[ii][ilay][iside]) {delete fMCPeakLayerUnb[ii][ilay][iside]; fMCPeakLayerUnb[ii][ilay][iside]=0;}
         if(fMCSigmLayerUnb[ii][ilay][iside]) {delete fMCSigmLayerUnb[ii][ilay][iside]; fMCSigmLayerUnb[ii][ilay][iside]=0;}
      }
   }
   }

   for(int iz=0;iz<NZQ;iz++){
      for(int iside=0;iside<3;iside++){
         if(kMCConvInner[iside][iz])    {delete kMCConvInner[iside][iz];    kMCConvInner[iside][iz]=0;}
         if(kMCConvInnerUnb[iside][iz]) {delete kMCConvInnerUnb[iside][iz]; kMCConvInnerUnb[iside][iz]=0;}
         if(kMCConvAll[iside][iz])    {delete kMCConvAll[iside][iz];    kMCConvAll[iside][iz]=0;}
         if(kMCConvAllUnb[iside][iz]) {delete kMCConvAllUnb[iside][iz]; kMCConvAllUnb[iside][iz]=0;}
         for(int ilay=0;ilay<9;ilay++){
            if(kMCConvLayer[ilay][iside][iz])    {delete kMCConvLayer[ilay][iside][iz];    kMCConvLayer[ilay][iside][iz]=0;}
            if(kMCConvLayerUnb[ilay][iside][iz]) {delete kMCConvLayerUnb[ilay][iside][iz]; kMCConvLayerUnb[ilay][iside][iz]=0;}
         }
      }
   }

   if(fGainTot[0]) {delete fGainTot[0]; fGainTot[0]=0;}
   if(fGainTot[1]) {delete fGainTot[1]; fGainTot[1]=0;}
   if(fkAsymm) {delete fkAsymm; fkAsymm=0;}
   if(fkLoss) {delete fkLoss; fkLoss=0;}
   if(fkGainCH) {delete fkGainCH; fkGainCH=0;}
   if(fkGain) {delete fkGain; fkGain=0;}
   if(fkTheta) {delete fkTheta; fkTheta=0;}
   if(fkTemp) {delete fkTemp; fkTemp=0;}

   if(fkBeta) {delete fkBeta; fkBeta=0;}
   if(fkRigidity) {delete fkRigidity; fkRigidity=0;}

   fHead=0;
}
bool TrChargeDBH::ReadCharge(char* filename,bool isunb,bool istuned){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ntot=0;
   int ncharge;
   //Read overall charge
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<2;ii++){
         fscanf(fin,"%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         if(istuned){
            //printf("TrChargeDBH::ReadCharge error! read fPeakAll for tunned parameters\n");
            //continue;
            MSplineH* ptar=(ii==0)?fTunedPeakAll:fTunedSigmAll;
            if(isunb) ptar=(ii==0)?fTunedPeakAllUnb:fTunedSigmAllUnb;
            if(ptar) delete ptar;
            if(ii==0){
               if(isunb) fTunedPeakAllUnb=spl;
               else fTunedPeakAll=spl;
            }
            else{
               if(isunb) fTunedSigmAllUnb=spl;
               else fTunedSigmAll=spl;
            }
         }
         else{
            MSplineH* ptar=(ii==0)?fPeakAll:fSigmAll;
            if(isunb) ptar=(ii==0)?fPeakAllUnb:fSigmAllUnb;
            if(ptar) delete ptar;
            if(ii==0){
               if(isunb) fPeakAllUnb=spl;
               else fPeakAll=spl;
            }
            else{
               if(isunb) fSigmAllUnb=spl;
               else fSigmAll=spl;
            }
         }
      }
   }

   //read X and Y charge
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int iside;
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<2;ii++){
         fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&iside,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         if(iside<0||iside>1) continue;
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         if(istuned){
            MSplineH* ptar=(ii==0)?fTunedPeakXY[iside]:fTunedSigmXY[iside];
            if(isunb) ptar=(ii==0)?fTunedPeakXYUnb[iside]:fTunedSigmXYUnb[iside];
            if(ptar) delete ptar;
            if(ii==0){
               if(isunb) fTunedPeakXYUnb[iside]=spl;
               else fTunedPeakXY[iside]=spl;
            }
            else{
               if(isunb) fTunedSigmXYUnb[iside]=spl;
               else fTunedSigmXY[iside]=spl;
            }
         }
         else{
            MSplineH* ptar=(ii==0)?fPeakXY[iside]:fSigmXY[iside];
            if(isunb) ptar=(ii==0)?fPeakXYUnb[iside]:fSigmXYUnb[iside];
            if(ptar) delete ptar;
            if(ii==0){
               if(isunb) fPeakXYUnb[iside]=spl;
               else fPeakXY[iside]=spl;
            }
            else{
               if(isunb) fSigmXYUnb[iside]=spl;
               else fSigmXY[iside]=spl;
            }
         }
      }
   }

   //read X and Y and all strips charge
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int iside,istrip;
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<2;ii++){
         fscanf(fin,"%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&iside,&istrip,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         if(iside<0||iside>1) continue;
         if(istrip<0||istrip>NSQ) continue;
         if(istuned){
            printf("TrChargeDBH::ReadCharge error! read fPeak for tunned parameters\n");
            continue;
         }
         else{
            MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
            spl->fitquality[0]=chi0;
            spl->fitquality[1]=chi1;
            spl->fitquality[2]=chi2;
            spl->fitquality[3]=chi3;
            MSplineH* ptar=(ii==0)?fPeak[iside][istrip]:fSigm[iside][istrip];
            if(isunb) ptar=(ii==0)?fPeakUnb[iside][istrip]:fSigmUnb[iside][istrip];
            if(ptar) delete ptar;
            if(ii==0){
               if(isunb) fPeakUnb[iside][istrip]=spl;
               else fPeak[iside][istrip]=spl;
            }
            else{
               if(isunb) fSigmUnb[iside][istrip]=spl;
               else fSigm[iside][istrip]=spl;
            }
         }
      }
   }
   printf("TrChargeDBH:: ReadWeight(%d,%d) from %s %s\n",isunb,istuned,filename,ntot>0?"successfully":"failed");

   fclose(fin);
   return ntot;
}

bool TrChargeDBH::ReadXYWeight(char* filename,bool isunb,bool istuned){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int nlayer;

   //Read Full Span XY weight
   fscanf(fin,"%d",&nlayer);
   for(int ilay=0;ilay<nlayer;ilay++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      fscanf(fin,"%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      for(int i2=0;i2<nknot;i2++){
         fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      }
      MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
      if(istuned){
         if(isunb){
            if(fTunedParsBestFSUnb) delete fTunedParsBestFSUnb;
            fTunedParsBestFSUnb=spl;
         }
         else{
            if(fTunedParsBestFS) delete fTunedParsBestFS;
            fTunedParsBestFS=spl;
         }
      }
      else{
         if(isunb){
            if(fParsBestFSUnb) delete fParsBestFSUnb;
            fParsBestFSUnb=spl;
         }
         else{
            if(fParsBestFS) delete fParsBestFS;
            fParsBestFS=spl;
         }
      }
   }

   //Read Inner XY weight
   fscanf(fin,"%d",&nlayer);
   for(int ilay=0;ilay<nlayer;ilay++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      fscanf(fin,"%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      for(int i2=0;i2<nknot;i2++){
         fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      }
      MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
      if(istuned){
         if(isunb){
            if(fTunedParsBestInnerUnb) delete fTunedParsBestInnerUnb;
            fTunedParsBestInnerUnb=spl;
         }
         else{
            if(fTunedParsBestInner) delete fTunedParsBestInner;
            fTunedParsBestInner=spl;
         }
      }
      else{
         if(isunb){
            if(fParsBestInnerUnb) delete fParsBestInnerUnb;
            fParsBestInnerUnb=spl;
         }
         else{
            if(fParsBestInner) delete fParsBestInner;
            fParsBestInner=spl;
         }
      }
   }

   //read Layer based XY weight
   fscanf(fin,"%d",&nlayer);
   for(int ilay=0;ilay<nlayer;ilay++){
      int JLayer;
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&JLayer,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      for(int i2=0;i2<nknot;i2++){
         fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      }
      if(JLayer<1||JLayer>9) continue;
      MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
      if(istuned){
         if(isunb){
            if(fTunedParsBestLayerUnb[JLayer-1]) delete fTunedParsBestLayerUnb[JLayer-1];
            fTunedParsBestLayerUnb[JLayer-1]=spl;
         }
         else{
            if(fTunedParsBestLayer[JLayer-1]) delete fTunedParsBestLayer[JLayer-1];
            fTunedParsBestLayer[JLayer-1]=spl;
         }
      }
      else{
         if(isunb){
            if(fParsBestLayerUnb[JLayer-1]) delete fParsBestLayerUnb[JLayer-1];
            fParsBestLayerUnb[JLayer-1]=spl;
         }
         else{
            if(fParsBestLayer[JLayer-1]) delete fParsBestLayer[JLayer-1];
            fParsBestLayer[JLayer-1]=spl;
         }
      }
   }

   printf("TrChargeDBH:: ReadXYWeight(%d,%d) from %s %s\n",isunb,istuned,filename,nlayer>0?"successfully":"failed");

   fclose(fin);
   return nlayer;
}

bool TrChargeDBH::ReadStripWeight(char* filename,bool isunb){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int nweight;
   fscanf(fin,"%d",&nweight);
   for(int ii=0;ii<nweight;ii++){
      int JLayer,iside,istrip;
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      fscanf(fin,"%d%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&JLayer,&iside,&istrip,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      for(int i2=0;i2<nknot;i2++){
         fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      }
      if(JLayer<1||JLayer>9) continue;
      if(iside<0||iside>1) continue;
      if(istrip<0||istrip>NSQ) continue;
      MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
      if(isunb){
         if(fWeightLayerUnb[istrip][JLayer-1][iside]) delete fWeightLayerUnb[istrip][JLayer-1][iside];
         fWeightLayerUnb[istrip][JLayer-1][iside]=spl;
      }
      else{
         if(fWeightLayer[istrip][JLayer-1][iside]) delete fWeightLayer[istrip][JLayer-1][iside];
         fWeightLayer[istrip][JLayer-1][iside]=spl;
      }
   }

   printf("TrChargeDBH:: ReadStripWeight(%d) from %s %s\n",isunb,filename,nweight>0?"successfully":"failed");

   fclose(fin);
   return nweight;
}

bool TrChargeDBH::ReadPeakShift(char* filename,bool isunb){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ntot=0;
   int ncharge;
   //read the peak shift of each single layer
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   bool isoverall=ncharge<5;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      int layer0,side0;
      if(isoverall) fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&side0,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      else fscanf(fin,"%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&layer0,&side0,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      if(side0<0||side0>2) return false;
      if((!isoverall)&&(layer0<1||layer0>9)) return false;
      for(int i2=0;i2<nknot;i2++){
         fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      }
      MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
      if(isoverall){
         for(int ilay=0;ilay<9;ilay++){
            if(isunb){
               if(fPeakCorrUnb[ilay][side0]&&ilay==0) delete fPeakCorrUnb[ilay][side0];
               fPeakCorrUnb[ilay][side0]=spl;
            }
            else{
               if(fPeakCorr[ilay][side0]&&ilay==0) delete fPeakCorr[ilay][side0];
               fPeakCorr[ilay][side0]=spl;
            }
         }
      }
      else{
         if(isunb){
            if(fPeakCorrUnb[layer0-1][side0]) delete fPeakCorrUnb[layer0-1][side0];
            fPeakCorrUnb[layer0-1][side0]=spl;
         }
         else{
            if(fPeakCorr[layer0-1][side0]) delete fPeakCorr[layer0-1][side0];
            fPeakCorr[layer0-1][side0]=spl;
         }
      }
   }
   fclose(fin);

   printf("TrChargeDBH::ReadPeakShift(%d) from %s %s\n",isunb,filename,ntot>0?"successfully":"failed");
   return ntot>0;
}

bool TrChargeDBH::ReadDifference(char* filename,int icomp,bool isunb){
   if(icomp<0||icomp>=NMCQpars) return false;
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ntot=0;
   int layer0;
   int side0;
   int ncharge;

   //read the all layer combined charge difference
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<2;ii++){
         fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&side0,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(side0<0||side0>2) return false;
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         //continue;  //right now tunning only happens at cluster level
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         MSplineH* ptar=(ii==0)?fMCPeakAll[icomp][side0]:fMCSigmAll[icomp][side0];
         if(isunb) ptar=(ii==0)?fMCPeakAllUnb[icomp][side0]:fMCSigmAllUnb[icomp][side0];
         if(ptar) delete ptar;
         if(ii==0){
            if(isunb) fMCPeakAllUnb[icomp][side0]=spl;
            else fMCPeakAll[icomp][side0]=spl;
         }
         else{
            if(isunb) fMCSigmAllUnb[icomp][side0]=spl;
            else fMCSigmAll[icomp][side0]=spl;
         }
      }
   }

   //read the inner layer combined charge difference
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<2;ii++){
         fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&side0,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(side0<0||side0>2) return false;
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         //continue;  //right now tunning only happens at cluster level
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         MSplineH* ptar=(ii==0)?fMCPeakInner[icomp][side0]:fMCSigmInner[icomp][side0];
         if(isunb) ptar=(ii==0)?fMCPeakInnerUnb[icomp][side0]:fMCSigmInnerUnb[icomp][side0];
         if(ptar) delete ptar;
         if(ii==0){
            if(isunb) fMCPeakInnerUnb[icomp][side0]=spl;
            else fMCPeakInner[icomp][side0]=spl;
         }
         else{
            if(isunb) fMCSigmInnerUnb[icomp][side0]=spl;
            else fMCSigmInner[icomp][side0]=spl;
         }
      }
   }

   //read single layer charge difference
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<2;ii++){
         fscanf(fin,"%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&layer0,&side0,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(side0<0||side0>2) return false;
         if(layer0<1||layer0>9) return false;
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         MSplineH* ptar=(ii==0)?fMCPeakLayer[icomp][layer0-1][side0]:fMCSigmLayer[icomp][layer0-1][side0];
         if(isunb) ptar=(ii==0)?fMCPeakLayerUnb[icomp][layer0-1][side0]:fMCSigmLayerUnb[icomp][layer0-1][side0];
         if(ptar) delete ptar;
         if(ii==0){
            if(isunb) fMCPeakLayerUnb[icomp][layer0-1][side0]=spl;
            else fMCPeakLayer[icomp][layer0-1][side0]=spl;
         }
         else{
            if(isunb) fMCSigmLayerUnb[icomp][layer0-1][side0]=spl;
            else fMCSigmLayer[icomp][layer0-1][side0]=spl;
         }
      }
   }
   fclose(fin);

   printf("TrChargeDBH:: ReadDifference(%d) from %s %s\n",isunb,filename,ntot>0?"successfully":"failed");
   return ntot>0;
}
bool TrChargeDBH::ReadMultSmearing(char* filename,bool isunb){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ntot=0;
   int layer0;
   int side0;
   int icomp;
   int ncharge;

   //read the all layer combined charge difference
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<3;ii++){
         fscanf(fin,"%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&side0,&icomp,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(side0<0||side0>2) return false;
         if(icomp<0||icomp>=NMCQpars) return false;
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         //continue;  //right now tunning only happens at cluster level
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         MSplineH* ptar=(ii==0)?fMCNormAll[icomp][side0]:(ii==1?fMCPeakAll[icomp][side0]:fMCSigmAll[icomp][side0]);
         if(isunb) ptar=(ii==0)?fMCNormAllUnb[icomp][side0]:(ii==1?fMCPeakAllUnb[icomp][side0]:fMCSigmAllUnb[icomp][side0]);
         if(ptar) delete ptar;
         if(ii==0){
            if(isunb) fMCNormAllUnb[icomp][side0]=spl;
            else fMCNormAll[icomp][side0]=spl;
         }
         else if(ii==1){
            if(isunb) fMCPeakAllUnb[icomp][side0]=spl;
            else fMCPeakAll[icomp][side0]=spl;
         }
         else{
            if(isunb) fMCSigmAllUnb[icomp][side0]=spl;
            else fMCSigmAll[icomp][side0]=spl;
         }
      }
   }

   //read the inner layer combined charge difference
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<3;ii++){
         fscanf(fin,"%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&side0,&icomp,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(side0<0||side0>2) return false;
         if(icomp<0||icomp>=NMCQpars) return false;
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         //continue;  //right now tunning only happens at cluster level
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         MSplineH* ptar=(ii==0)?fMCNormInner[icomp][side0]:(ii==1?fMCPeakInner[icomp][side0]:fMCSigmInner[icomp][side0]);
         if(isunb) ptar=(ii==0)?fMCNormInnerUnb[icomp][side0]:(ii==1?fMCPeakInnerUnb[icomp][side0]:fMCSigmInnerUnb[icomp][side0]);
         if(ptar) delete ptar;
         if(ii==0){
            if(isunb) fMCNormInnerUnb[icomp][side0]=spl;
            else fMCNormInner[icomp][side0]=spl;
         }
         else if(ii==1){
            if(isunb) fMCPeakInnerUnb[icomp][side0]=spl;
            else fMCPeakInner[icomp][side0]=spl;
         }
         else{
            if(isunb) fMCSigmInnerUnb[icomp][side0]=spl;
            else fMCSigmInner[icomp][side0]=spl;
         }
      }
   }

   //read single layer charge difference
   fscanf(fin,"%d",&ncharge);
   ntot+=ncharge;
   for(int iq=0;iq<ncharge;iq++){
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[100],yknot[100];
      for(int ii=0;ii<3;ii++){
         fscanf(fin,"%d%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&layer0,&side0,&icomp,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
         if(side0<0||side0>2) return false;
         if(layer0<1||layer0>9) return false;
         if(icomp<0||icomp>=NMCQpars) return false;
         for(int i2=0;i2<nknot;i2++){
            fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
         }
         MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
         spl->fitquality[0]=chi0;
         spl->fitquality[1]=chi1;
         spl->fitquality[2]=chi2;
         spl->fitquality[3]=chi3;
         MSplineH* ptar=(ii==0)?fMCNormLayer[icomp][layer0-1][side0]:(ii==1?fMCPeakLayer[icomp][layer0-1][side0]:fMCSigmLayer[icomp][layer0-1][side0]);
         if(isunb) ptar=(ii==0)?fMCNormLayerUnb[icomp][layer0-1][side0]:(ii==1?fMCPeakLayerUnb[icomp][layer0-1][side0]:fMCSigmLayerUnb[icomp][layer0-1][side0]);
         if(ptar) delete ptar;
         if(ii==0){
            if(isunb) fMCNormLayerUnb[icomp][layer0-1][side0]=spl;
            else fMCNormLayer[icomp][layer0-1][side0]=spl;
         }
         else if(ii==1){
            if(isunb) fMCPeakLayerUnb[icomp][layer0-1][side0]=spl;
            else fMCPeakLayer[icomp][layer0-1][side0]=spl;
         }
         else{
            if(isunb) fMCSigmLayerUnb[icomp][layer0-1][side0]=spl;
            else fMCSigmLayer[icomp][layer0-1][side0]=spl;
         }
      }
   }
   fclose(fin);

   printf("TrChargeDBH:: ReadMultSmearing(%d) from %s %s\n",isunb,filename,ntot>0?"successfully":"failed");
   return ntot>0;
}

bool TrChargeDBH::ReadZQTuning(char* filename,int iside,int JLayer,bool isunb){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   int ncharge;
   int ilay=JLayer-1;
   if(iside<0||iside>2) iside=2;
   if(!((ilay>=0&&ilay<9) && (iside>=0&&iside<2))) return false;  //right now tunning only happens at cluster level
   //Read overall number of charge
   fscanf(fin,"%d",&ncharge);
   for(int iq=0;iq<ncharge;iq++){
      int Z0,nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi0,chi1,chi2,chi3;
      double xknot[120],yknot[120];
      fscanf(fin,"%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&Z0,&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi0,&chi1,&chi2,&chi3);
      for(int i2=0;i2<nknot;i2++){
         fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      }
      if(Z0<1||Z0>NZQ) return false;
      MSplineH* spl=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      spl->fitquality[0]=chi0;
      spl->fitquality[1]=chi1;
      spl->fitquality[2]=chi2;
      spl->fitquality[3]=chi3;
      if(isunb){
         if(JLayer>=1&&JLayer<=9){
            if(kMCConvLayerUnb[ilay][iside][Z0-1]) delete kMCConvLayerUnb[ilay][iside][Z0-1];
            kMCConvLayerUnb[ilay][iside][Z0-1]=spl;
         }
         else if(JLayer<1){
            if(kMCConvAllUnb[iside][Z0-1]) delete kMCConvAllUnb[iside][Z0-1];
            kMCConvAllUnb[iside][Z0-1]=spl;
         }
         else{
            if(kMCConvInnerUnb[iside][Z0-1]) delete kMCConvInnerUnb[iside][Z0-1];
            kMCConvInnerUnb[iside][Z0-1]=spl;
         }
      }
      else{
         if(JLayer>=1&&JLayer<=9){
            if(kMCConvLayer[ilay][iside][Z0-1]) delete kMCConvLayer[ilay][iside][Z0-1];
            kMCConvLayer[ilay][iside][Z0-1]=spl;
         }
         else if(JLayer<1){
            if(kMCConvAll[iside][Z0-1]) delete kMCConvAll[iside][Z0-1];
            kMCConvAll[iside][Z0-1]=spl;
         }
         else{
            if(kMCConvInner[iside][Z0-1]) delete kMCConvInner[iside][Z0-1];
            kMCConvInner[iside][Z0-1]=spl;
         }
      }
   }
   fclose(fin);
   return ncharge>0;
}
bool TrChargeDBH::ReadQTuning(char* dirname){
   int ntot=0;
   for(int iside0=0;iside0<3;iside0++){
      for(int ilay0=-1;ilay0<10;ilay0++){
         for(int IsUnb=0;IsUnb<2;IsUnb++){
            char filename[200]="";
            if(ilay0>=0&&ilay0<9) strcpy(filename,Form("%s/MCTunning_B1200_pars1_L%d%s%s.txt",dirname,ilay0+1,iside0<2?(iside0==0?"_x":"_y"):"",IsUnb?"_unb":""));
            else if(ilay0<0) strcpy(filename,Form("%s/MCTunning_B1200_pars1_All%s%s.txt",dirname,iside0<2?(iside0==0?"_x":"_y"):"",IsUnb?"_unb":""));
            else strcpy(filename,Form("%s/MCTunning_B1200_pars1_Inner%s%s.txt",dirname,iside0<2?(iside0==0?"_x":"_y"):"",IsUnb?"_unb":""));
            ntot+=ReadZQTuning(filename,iside0,ilay0+1,IsUnb);
         }
      }
   }
   printf("TrChargeDBH:: ReadQTuning(unbiased and others) from %s %s\n",dirname,ntot>0?"successfully":"failed");
   return ntot>0;
}

TrChargeDBH* TrChargeDBH::GetHead(){
   if(!UseTrChargeH) return 0;
   if(!fHead){
      bool debug=false;
      fHead=new TrChargeDBH();

      //Load the initial charge estimator
      if(TrChargeDBH::IsMC){
         const int np=27;
         double xx[np]={0,1,4,9,16,25,36,49,64,81,100,121,144,169,196,225,256,289,324,361,400,441,484,529,576,625,676};
         double yy[2][np]={{0.,31.28,135.68,336.41,648.82,1051.96,1523.03,2039.97,2576.68,3092.56,3608.04,4158.96,4727.90,5299.27,5855.16,6372.35,6842.98,7272.76,7669.76,8043.91,8406.72,8771.02,9150.84,9533.05,9901.88,10269.07,10647.35},{0.,34.45,147.26,319.81,528.60,733.92,903.60,1040.22,1153.69,1251.95,1350.64,1480.97,1635.57,1805.89,1981.32,2159.37,2345.73,2540.47,2743.68,2955.44,3175.84,3404.94,3642.85,3889.94,4146.23,4411.15,4684.03}};
         fGainTot[0]=new MSplineH(np,xx,yy[0],false,false,MSplineH::kLinear,MSplineH::kLinear,0,0);
         fGainTot[1]=new MSplineH(np,xx,yy[1],false,false,MSplineH::kLinear,MSplineH::kLinear,0,0);
      }
      else{
         const int np=17;
         double xx[np]={0,1,4,9,25,36,49,64,100,144,196,256,324,400,484,576,676};
         double yy[2][np]={{0.00,33.80,150.39,380.68,1117.04,1601.58,2162.03,2741.67,3863.66,4951.12,6204.04,7388.26,8174.63,8903.07,9712.12,10575.51,11261.88},{0.00,33.56,142.23,312.91,646.14,766.16,861.20,934.32,1120.09,1357.98,1621.49,1924.53,2267.99,2621.90,3026.49,3459.72,3868.45}};
         fGainTot[0]=new MSplineH(np,xx,yy[0],false,false,MSplineH::kLinear,MSplineH::kLinear,0,0);
         fGainTot[1]=new MSplineH(np,xx,yy[1],false,false,MSplineH::kLinear,MSplineH::kLinear,0,0);
      }

      //Loading all the correction database and weights
      char DatabaseDir1[2][200]={"",""};
      char DatabaseDir2[2][200]={"",""};
      char DatabaseDir3[2][200]={"",""};
      bool loaded=true;
      strcpy(DatabaseDir1[0],Form("%sChargeCalibHDB/%sv%d/weight_sigma%s.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));//the directory has been changed to v6.00
      strcpy(DatabaseDir1[1],Form("%sChargeCalibHDB/%sv%d/weight_sigma%s_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
      strcpy(DatabaseDir2[0],Form("%sChargeCalibHDB/%sv%d/weight_angle%s.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
      strcpy(DatabaseDir2[1],Form("%sChargeCalibHDB/%sv%d/weight_angle%s_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
      strcpy(DatabaseDir3[0],Form("%sChargeCalibHDB/%sv%d/weight_strip%s.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
      strcpy(DatabaseDir3[1],Form("%sChargeCalibHDB/%sv%d/weight_strip%s_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
      loaded=fHead->ReadCharge(DatabaseDir1[0],false)&&loaded;
      loaded=fHead->ReadXYWeight(DatabaseDir2[0],false)&&loaded;
      fHead->ReadStripWeight(DatabaseDir3[0],false);
      //load parameters for unbiased charge
      fHead->ReadCharge(DatabaseDir1[1],true);
      fHead->ReadXYWeight(DatabaseDir2[1],true);
      fHead->ReadStripWeight(DatabaseDir3[1],true);
      if(TrChargeDBH::IsMC){
         bool loadmcq=false;
         //load the MC charge resolution tuning
         //for(int ii=0;ii<NMCQpars;ii++){
         //   char DatabaseDir4[2][200]={"",""};
         //   strcpy(DatabaseDir4[0],Form("%sChargeCalibHDB/%sv%d/MCTunning_B1200_pars%d%s.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),ii,TrChargeDBH::UseTotal?"_tot":""));
         //   strcpy(DatabaseDir4[1],Form("%sChargeCalibHDB/%sv%d/MCTunning_B1200_pars%d%s_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),ii,TrChargeDBH::UseTotal?"_tot":""));
         //   loadmcq=fHead->ReadDifference(DatabaseDir4[0],ii,false)||loadmcq;
         //   fHead->ReadDifference(DatabaseDir4[1],ii,true);
         //}
         char DatabaseDir4[2][200]={"",""};
         strcpy(DatabaseDir4[0],Form("%sChargeCalibHDB/%sv%d/MCTunning_B1200_pars.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10)));
         strcpy(DatabaseDir4[1],Form("%sChargeCalibHDB/%sv%d/MCTunning_B1200_pars_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10)));
         loadmcq=fHead->ReadMultSmearing(DatabaseDir4[0],false)||loadmcq;
         fHead->ReadMultSmearing(DatabaseDir4[1],true);
         //load the MC charge shape tuning
         if(true){
            char DatabaseDir42[200]="";
            strcpy(DatabaseDir42,Form("%sChargeCalibHDB/%sv%d/qtuning",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10)));
            loadmcq=fHead->ReadQTuning(DatabaseDir42)&&loadmcq;
         }
         loaded=loadmcq&&loaded;
         bool loadtunedq=true;
         if(true){
            char DatabaseDir5[2][200]={"",""};
            char DatabaseDir6[2][200]={"",""};
            strcpy(DatabaseDir5[0],Form("%sChargeCalibHDB/%sv%d/weight_sigma_tuned%s.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
            strcpy(DatabaseDir5[1],Form("%sChargeCalibHDB/%sv%d/weight_sigma_tuned%s_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
            strcpy(DatabaseDir6[0],Form("%sChargeCalibHDB/%sv%d/weight_angle_tuned%s.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
            strcpy(DatabaseDir6[1],Form("%sChargeCalibHDB/%sv%d/weight_angle_tuned%s_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10),TrChargeDBH::UseTotal?"_tot":""));
            loadtunedq=fHead->ReadCharge(DatabaseDir5[0],false,true)&&loadtunedq;
            loadtunedq=fHead->ReadXYWeight(DatabaseDir6[0],false,true)&&loadtunedq;
            fHead->ReadCharge(DatabaseDir5[1],true,true);
            fHead->ReadXYWeight(DatabaseDir6[1],true,true);
         }
         loaded=loadtunedq&&loaded;
      }
      else{
         char DatabaseDir4[2][200]={"",""};
         strcpy(DatabaseDir4[0],Form("%sChargeCalibHDB/%sv%d/peakshift.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10)));
         strcpy(DatabaseDir4[1],Form("%sChargeCalibHDB/%sv%d/peakshift_unb.txt",AMSDATADIR.amsdatadir,TrChargeDBH::IsMC?"MC/":"",TrChargeDBH::IsMC?(TrChargeDBH::fUseVersion/10):(TrChargeDBH::fUseVersion%10)));
         bool loadshift=fHead->ReadPeakShift(DatabaseDir4[0],false);
         fHead->ReadPeakShift(DatabaseDir4[1],true);
      }
      if(debug) printf("load new TrChargeDBH %s...\n",loaded?"successfully":"failed");
      if(!loaded){
         if(fHead) delete fHead;
         fHead = 0;
      }

      fkAsymm=TrAsymmDBH::GetHead();
      fkLoss=TrChargeLossDBH::GetHead();
      fkGainCH=TrGainCHDBH::GetHead();
      fkGain=TrGainDBH::GetHead();
      fkTheta=TrThetaDBH::GetHead();
      fkTemp=TrTempDBH::GetHead();

      fkBeta=TrBetaDBH::GetHead();
      fkRigidity=TrRigidityDBH::GetHead();

      //if(!(TkDBc::Head)){
      //TkDBc::CreateTkDBc();
      //TkDBc::Head->init();
      //}
      return fHead;
   }
   else return fHead;
}
TrChargeDBH* TrChargeDBH::GetHead(int run,int time,int ismc){
   if(ismc>=0) IsMC=ismc;
   TrChargeDBH::run_current=run;
   TrChargeDBH::time_current=time;
   TrChargeDBH* phead=GetHead();
   return phead;
}

float TrChargeDBH::GetWeight(int iside,int istrip,int JLayer,float Z0,bool isunb){
   int Z=(int)(Z0+0.5);
   if(Z<1) Z=1;

   if(false){
      if(JLayer<1||JLayer>9) JLayer=5;
      if((iside>=0&&iside<=1)&&(istrip>=0&&istrip<=NSQ)){
         if(isunb){
            if(fWeightLayerUnb[istrip][JLayer-1][iside]) return fWeightLayerUnb[istrip][JLayer-1][iside]->Eval(Z);
         }
         else{
            if(fWeightLayer[istrip][JLayer-1][iside]) return fWeightLayer[istrip][JLayer-1][iside]->Eval(Z);
         }
      }
   }

   //ignore low signal strips
   if(iside==0&&istrip==4) return 0;
   if(iside==0&&istrip==3&&Z<21) return 0;
   if(iside==0&&istrip==2&&Z<12)  return 0;
   if(iside==0&&istrip==1&&Z<3)  return 0;
   if(iside==1&&istrip==4) return 0;
   if(iside==1&&istrip==3&&Z<8)  return 0;
   if(iside==1&&istrip==2)  return 0;
   if(iside==1&&istrip==1&&Z<2)  return 0;

   //ignore saturated strips
   if(iside==0&&istrip==0&&Z>14) return 0;
   if(iside==0&&istrip==1&&Z>18) return 0;
   if(iside==0&&istrip==2&&Z>26) return 0;
   if(iside==1&&istrip==0&&((Z>3&&Z<9)||(Z>17))) return 0;
   if(iside==1&&istrip==1&&(Z>8&&Z<14)) return 0;
   if(iside==1&&istrip==2) return 0;
   if(iside==1&&istrip==3&&(Z>9)) return 0;
   if(iside==1&&istrip==4) return 0;

   if(iside<0||iside>1) return 0;
   if(istrip<0||istrip>NSQ){
      if(true){
         if(istrip<0) return 0;
         if(isunb){
            if( (!fPeakXYUnb[iside])||(!fSigmXYUnb[iside]) ) return 0;
            //double peak=fPeakXYUnb[iside]->Eval(Z);
            double sigm=fSigmXYUnb[iside]->Eval(Z);
            if(sigm<=0) return 0;
            return 1/sigm/sigm;
         }
         else{
            if( (!fPeakXY[iside])||(!fSigmXY[iside]) ) return 0;
            //double peak=fPeakXY[iside]->Eval(Z);
            double sigm=fSigmXY[iside]->Eval(Z);
            if(sigm<=0) return 0;
            return 1/sigm/sigm;
         }
      }
      if(false){
         if(istrip<0) return 0;
         if(JLayer<0){
            if(!fParsBestFS) return 0;
            double angle=fParsBestFS->Eval(Z);
            return iside==0?TMath::Cos(angle):TMath::Sin(angle);
         }
         else if(JLayer==0){
            if(!fParsBestInner) return 0;
            double angle=fParsBestInner->Eval(Z);
            return iside==0?TMath::Cos(angle):TMath::Sin(angle);
         }
         else if(JLayer<=9){
            if(!fParsBestLayer[JLayer-1]) return 0;
            double angle=fParsBestLayer[JLayer-1]->Eval(Z);
            return iside==0?TMath::Cos(angle):TMath::Sin(angle);
         }
      }
      return 0;
   }
   if( (!fPeak[iside][istrip])||(!fSigm[iside][istrip]) ) return 0;
   //double peak=fPeak[iside][istrip]->Eval(Z);
   double sigm=fSigm[iside][istrip]->Eval(Z);
   if(sigm<=0) return 0;

   return 1/sigm/sigm;
}
void TrChargeDBH::InitCharge(int iside0){
   for(int iside=0;iside<2;iside++){
      if((iside0>=0&&iside0<2)&&iside!=iside0) continue;
      if(iside==0) xcatg=0;
      eta_kinit[iside]=0;
      theta_kinit[iside]=0;
      for(int istrip=0;istrip<=NSQ;istrip++){
         addr_kinit[iside][istrip]=0;
         charge_kinit[iside][istrip]=0;
         eta_kasymm[iside][istrip]=0;
         theta_kasymm[iside][istrip][0]=0;
         theta_kasymm[iside][istrip][1]=0;
         charge_kasymm[iside][istrip]=0;
      }
      for(int istrip=0;istrip<=NSQ;istrip++){
         charge_kall[iside][istrip]=0;
         for(int icalib=0;icalib<NCalib;icalib++){
             charge_kloss[iside][istrip][icalib]=0;
           charge_kgainch[iside][istrip][icalib]=0;
             charge_kgain[iside][istrip][icalib]=0;
            charge_ktheta[iside][istrip][icalib]=0;
             charge_ktemp[iside][istrip][icalib]=0;
         }
      }
      charge_kxy[iside]=0;
      charge_kbeta[iside]=0;
      charge_krigidity[iside]=0;
   }
   if(iside0>=2){
      charge_kxy[2]=0;
      charge_kbeta[2]=0;
      charge_krigidity[2]=0;
   }
}
double TrChargeDBH::StripCombination(double StripCharge[NSQ+1],int iside,int JLayer,float Z0,int Opt){
   if(iside<0||iside>1) return 0;
   bool debug=false;
   bool iszero=true;
   for(int istrip=0;istrip<=NSQ;istrip++) if(StripCharge[istrip]>0) iszero=false;
   if(iszero) return 0;
   double result=0;
   double Zx=Z0;
   if(Zx<=0){
      if(StripCharge[NSQ]>0) Zx=StripCharge[NSQ];
      else{
         int nstrip=0;
         for(int istrip=0;istrip<=NSQ;istrip++){
            if(StripCharge[istrip]>0){
               Zx+=StripCharge[istrip];
               nstrip++;
            }
         }
         if(nstrip>0) Zx/=nstrip;
      }
   }
   for(int iloop=0;iloop<2;iloop++){
      if(Opt&kStripWeight){
         double sumcharge=0;
         double sumweight=0;
         for(int istrip=0;istrip<=NSQ;istrip++){
            //float weight=1/pow(fSigm[iside][istrip]->Eval(Zx),2);
            //if(true){
            //   //ignore low signal strips and high saturated strips
            //   if(iside==0){
            //      if((istrip==0)&&(Zx>14.5)) continue;
            //      if((istrip==1)&&(Zx>18.5||Zx<8.5)) continue;
            //      if((istrip==2)&&(Zx<11.5||Zx>26.5)) continue;
            //      if((istrip==3)&&(Zx<20.5)) continue;
            //      if((istrip==4)) continue;
            //   }
            //   if(iside==1){
            //      if((istrip==0)&&((Zx>3.5&&Zx<8.5) ||Zx>17.5)) continue;
            //      if((istrip==1)&&((Zx>8.5&&Zx<13.5)||Zx<1.5)) continue;
            //      if((istrip==2)) continue;
            //      if((istrip==3)&&(Zx<7.5||Zx>9.5)) continue;
            //      if((istrip==4)) continue;
            //   }
            //}

            double stripcomb_xx[28];
            double stripcomb_yy[28];
            for(int iz=0;iz<28;iz++){
               stripcomb_xx[iz]=iz+1;
               stripcomb_yy[iz]=0;
               MSplineH* spl=((Opt&kUnbias)&&(!TrChargeDBH::IsMC))?fSigmUnb[iside][istrip]:fSigm[iside][istrip];
               if(spl) stripcomb_yy[iz]=1/pow(spl->Eval(stripcomb_xx[iz]),2);
               if(TrChargeDBH::IsMC){
                  if(iside==0){
                     if(UseTotal){
                        if(istrip<NSQ) stripcomb_yy[iz]=0;
                        else stripcomb_yy[iz]=1;
                     }
                     else{
                        if( istrip>=2 ) stripcomb_yy[iz]=0;
                     }
                  }
                  if(iside==1){
                     if(UseTotal){
                        if(istrip<NSQ) stripcomb_yy[iz]=0;
                        else stripcomb_yy[iz]=1;
                     }
                     else{
                        if( istrip>=2 ) stripcomb_yy[iz]=0;
                     }
                  }
               }
               else{
                  double qinf=100.;
                  double qsiglimit[2][6][2]={{{0,0},{0,3.5},{0,13.5},{0,20.5},{0,28.5},{0,0}
                                             },
                                             {{0,0},{0,1.5},{0,4.5},{0,7.5},{0,4.5},{0,0}
                                             }
                                            };
                  double qsatlimit[2][6][2]={{{10.5,qinf},{14.5,qinf},{28.5,qinf},{qinf,qinf},{qinf,qinf},{qinf,qinf}
                                             },
                                             {{3.5,9.5},{8.5,13.5},{3.5,qinf},{8.5,qinf},{3.5,qinf},{3.5,qinf}
                                             }
                                            };
                  if(stripcomb_xx[iz]<=qsiglimit[iside][istrip][1]&&stripcomb_xx[iz]>=qsiglimit[iside][istrip][0]) stripcomb_yy[iz]=0;
                  if(stripcomb_xx[iz]<=qsatlimit[iside][istrip][1]&&stripcomb_xx[iz]>=qsatlimit[iside][istrip][0]) stripcomb_yy[iz]=0;
                  if(Opt&kUnbias) ;//{if(iside==1&&istrip==1&&(stripcomb_xx[iz]>=17.5)) stripcomb_yy[iz]=0;}
                  else {if(iside==1&&istrip==0&&(stripcomb_xx[iz]>=17.5)) stripcomb_yy[iz]=0;}
               }
            }
            float weight;
            if(UseMSplineH){
               MSplineH* spl=new MSplineH(28,stripcomb_xx,stripcomb_yy,false,false,MSplineH::kFlat,MSplineH::kFlat,0,0);
               weight=spl->Eval(Zx);
               delete spl; spl=0;
            }     
            else{    
               if(Zx<=stripcomb_xx[0]) weight=stripcomb_yy[0];
               else if(Zx>=stripcomb_xx[27]) weight=stripcomb_yy[27];
               else{ 
                  int left=0;int right=27;
                  while(abs(right-left)>1){
                     int middle=(int)((left+right)/2.);
                     if(Zx<=stripcomb_xx[middle]){right=middle;}
                     else{left=middle;}
                  }  
                  double frac=(Zx-stripcomb_xx[left])/(stripcomb_xx[right]-stripcomb_xx[left]);
                  weight=stripcomb_yy[right]*frac+(1-frac)*stripcomb_yy[left];
               }
            }

            if(StripCharge[istrip]>0){
               sumweight+=weight;
               sumcharge+=StripCharge[istrip]*weight;
            }
            if(debug) printf("L%d %s loop%d S%d: qstrip=%.3f(%d) weight=%.3f(%.2f) sum={%.3f,%.3f}\n",JLayer,iside==0?"x":"y",iloop,istrip,StripCharge[istrip],std::isfinite(StripCharge[istrip]),weight,Zx,sumweight,sumcharge);
         }
         if(sumweight>0) result=sumcharge/sumweight;
         else{
            if(StripCharge[NSQ]>0) result=StripCharge[NSQ];
            else{
               int nstrip=0;
               for(int istrip=0;istrip<=NSQ;istrip++){
                  if(StripCharge[istrip]>0){
                     result+=StripCharge[istrip];
                     nstrip++;
                  }
               }
               result/=nstrip;
            }
         }
      }
      if(debug) printf("L%d %s loop%d after strip combination: Q=%.2f(Zx=%.2f) {%.2f,%.2f,%.2f,%.2f,%.2f,%.2f}\n",JLayer,iside==0?"x":"y",iloop,result,Zx,StripCharge[0],StripCharge[1],StripCharge[2],StripCharge[3],StripCharge[4],StripCharge[5]);

      if(Z0<=0&&result>0) Zx=result;
      if(Z0>0) break;
   }
   return result;
}
double TrChargeDBH::XYCombination(double XYCharge[2],int JLayer,float Z0,int Opt,int* npoints){
   if(XYCharge[0]<0) XYCharge[0]=0;
   if(XYCharge[1]<0) XYCharge[1]=0;
   if(XYCharge[0]<=0||XYCharge[1]<=0) return (XYCharge[0]<=0)?XYCharge[1]:XYCharge[0];
   //some specific region
   if(JLayer<1||JLayer>9){
      if(npoints[1]<3&&npoints[0]>=3) return XYCharge[0];
      if(npoints[0]<3&&npoints[1]>=3) return XYCharge[1];
   }
   //in some specific region, only use X side
   if(XYCharge[0]>=7.5&&XYCharge[0]<=9.5) return XYCharge[0];
   double result=0;
   double Zx=Z0;
   //if(Zx<=0) Zx=(XYCharge[0]<10.5)?XYCharge[0]:XYCharge[1];
   if(Zx<=0) Zx=(XYCharge[0]>0)?XYCharge[0]:XYCharge[1];
   for(int iloop=0;iloop<2;iloop++){
      if(Opt&kXYSigma){
         double sumcharge=0;
         double sumweight=0;
         for(int iside=0;iside<2;iside++){
            double weight=0;
            MSplineH* spl=((Opt&kUnbias)&&(!TrChargeDBH::IsMC))?fSigmXYUnb[iside]:fSigmXY[iside];
            if((Opt&kSmear)&&TrChargeDBH::IsMC){
               spl=(Opt&kUnbias)?fTunedSigmXYUnb[iside]:fTunedSigmXY[iside];
            }
            if(spl) weight=1/pow(spl->Eval(Zx),2);
            if(JLayer<1||JLayer>9){
               weight*=npoints[iside];
            }
            //if(Zx>4.5&&Zx<11.5&&(iside==1)) weight*=0.001;
            if(XYCharge[iside]>4.5&&XYCharge[iside]<11.5&&(iside==1&&(!TrChargeDBH::IsMC))) weight*=0.001;
            if(XYCharge[iside]>0){
               sumweight+=weight;
               sumcharge+=XYCharge[iside]*weight;
            }
         }
         if(sumweight>0) result=sumcharge/sumweight;
         else{
            result=(XYCharge[0]+XYCharge[1])/2;
         }
      }
      if(Opt&kXYWeight){
         if(JLayer>9) return 0;
         MSplineH* spl=(JLayer>=1)?fParsBestLayer[JLayer-1]:(JLayer==0?fParsBestInner:(fParsBestFS?fParsBestFS:fParsBestInner));
         if((Opt&kUnbias)&&(!TrChargeDBH::IsMC)){
            spl=(JLayer>=1)?fParsBestLayerUnb[JLayer-1]:(JLayer==0?fParsBestInnerUnb:(fParsBestFSUnb?fParsBestFSUnb:fParsBestInnerUnb));
         }
         if((Opt&kSmear)&&TrChargeDBH::IsMC){
            spl=(JLayer>=1)?fTunedParsBestLayer[JLayer-1]:(JLayer==0?fTunedParsBestInner:(fTunedParsBestFS?fTunedParsBestFS:fTunedParsBestInner));
            if(Opt&kUnbias) spl=(JLayer>=1)?fTunedParsBestLayerUnb[JLayer-1]:(JLayer==0?fTunedParsBestInnerUnb:(fTunedParsBestFSUnb?fTunedParsBestFSUnb:fTunedParsBestInnerUnb));
         }
         if(!spl) return 0;
         double angle=spl->Eval(Zx)/180.*3.1415926;
         double sumcharge=0;
         double sumweight=0;
         for(int iside=0;iside<2;iside++){
            double weight=(iside==0)?TMath::Cos(angle):TMath::Sin(angle);
            if(JLayer<1||JLayer>9){
               weight*=npoints[iside];
            }
            if(XYCharge[iside]>0){
               sumweight+=weight;
               sumcharge+=XYCharge[iside]*weight;
            }
         }
         if(sumweight>0) result=sumcharge/sumweight;
         else{
            result=(XYCharge[0]+XYCharge[1])/2;
         }
      }

      if(Z0<=0&&result>0) Zx=result;
      if(Z0>0) break;
   }
   return result;
}


mean_t TrChargeDBH::LayerCombination(double QLayers[9],int Opt,int iside){
   vector<double> qvector;
   for(int ilay=0;ilay<9;ilay++){
      bool fill=false;
      if(Opt&kAll) fill=true;
      if((Opt&kInner)&& (ilay>0&&ilay<8) ) fill=true;
      if((Opt&kUpper)&& (ilay==0) ) fill=true;
      if((Opt&kLower)&& (ilay==8) ) fill=true;
      if(fill&&QLayers[ilay]>0){
         qvector.push_back(QLayers[ilay]);
      }
   }

   return GetMean(Opt,qvector,iside);
}
float TrChargeDBH::GetSqrtdEdX(int tkid,int iside,float *adc,float ip,float *ia,int seedaddress,int imult,float Z0,int ncalib,int Opt){
   if(iside<0||iside>1) return 0;
   //init the variables
   InitCharge(iside);

   int OLayer=(int)(abs(tkid)/100);
   int JLayer=OLayer+1;
   if(OLayer==8) JLayer=1;
   if(OLayer==9) JLayer=9;
   if(JLayer<1||JLayer>9) return 0;
   if(iside==0&&(seedaddress<640||seedaddress>1023)) return 0;
   if(iside==1&&(seedaddress<0||seedaddress>639)) return 0;

   double sigtot=0;
   for(int ii=0;ii<NSQ;ii++){
      if(adc[ii]>0) sigtot+=adc[ii];
   }
   if(sigtot<=0) return 0;

   double sig[NSQ+1];
   for(int ii=0;ii<=NSQ;ii++){
      sig[ii]=adc[ii];
   }

   int seedindex=(NSQ-1)/2;
   int address[NSQ+1];
   for(int ii=0;ii<=NSQ;ii++){
      if(ii<NSQ) address[ii]=ii-seedindex+seedaddress;
      else address[ii]=seedaddress;
      if (iside == 1) {
        if (address[ii] >= 0) {
          if (address[ii] >= 640) address[ii] -= 640; // address >= 640
        }
        else                      address[ii] += 640;          // address < 0
      }
      else {
        if (address[ii] >= 640) {
          if (address[ii] >= 1024) address[ii] -= 384;    // address >= 1024
        }
        else                       address[ii] += 384;    // address < 640
      }
   }

   if(iside==0){
      xcatg=0;
      bool IsK7=TkDBc::Head->FindTkId(tkid)->IsK7();
      int sens;
      int sensoraddress=TkCoo::GetSensorAddress(tkid,seedaddress,imult,sens);
      xcatg=fkLoss->GetCategory(iside,IsK7,sensoraddress,ip);
   }
   eta_kinit[iside]=ip;
   theta_kinit[iside]=ia[iside];
   for(int istrip=0;istrip<=NSQ;istrip++){
      int istrip0=fkLoss->GetStripIndex(seedindex,istrip,ip);
      addr_kinit[iside][istrip0]=address[istrip];
      charge_kinit[iside][istrip0]=sig[istrip];
   }

   double Zx0=fGainTot[iside]->EvalInverseSimple(sigtot);
   //if(Zx0<=0){ //a rough estimation of the seed charge if needed
   //   int nsol;
   //   double xsol[5];
   //   fGainTot[iside]->EvalInverseMultiple(sigtot,nsol,xsol);
   //   Zx0=sqrt(xsol[0]);
   //}
   if(Zx0<=0) return 0;
   else Zx0=sqrt(Zx0);
   bool UseNewAlgo=(!IsMC)&&((Zx0<14.5)&&(!UseMSplineH));
   double Zx=Z0;
   for(int iloop=0;iloop<2;iloop++){
      //if(JLayer==2) printf("%sStripLoop%d: Z0=%.2f\n",iside==0?"x":"y",iloop,Zx);
      //Asymmetry
      for(int istrip=0;istrip<=NSQ;istrip++){
         int istrip0=fkLoss->GetStripIndex((NSQ-1)/2,istrip,ip);
         bool doasym=fkAsymm->DoAsymmetry((NSQ-1)/2,istrip,ip);
         if(charge_kinit[iside][istrip0]<=0){
            charge_kasymm[iside][istrip0]=0;
            continue;
         }
         if(doasym){
            double asymcor=fkAsymm->GetAsymmetryCorrection(0,iside,seedindex,istrip,ip,ia[iside]);
            charge_kasymm[iside][istrip0]=charge_kinit[iside][istrip0]*asymcor;
            eta_kasymm[iside][istrip0]=-ip;
            theta_kasymm[iside][istrip0][iside]=-ia[iside];
            theta_kasymm[iside][istrip0][1-iside]=ia[1-iside];
         }
         else{
            charge_kasymm[iside][istrip0]=charge_kinit[iside][istrip0];
            eta_kasymm[iside][istrip0]=ip;
            theta_kasymm[iside][istrip0][iside]=ia[iside];
            theta_kasymm[iside][istrip0][1-iside]=ia[1-iside];
         }
      }

      //Detector Effect Corrections
      for(int istrip=NSQ;istrip>=0;istrip--){
         double charge_init=charge_kasymm[iside][istrip];
         if(charge_init<=0){
            charge_kall[iside][istrip]=0;
            for(int icalib=0;icalib<ncalib;icalib++){
               charge_kloss[iside][istrip][icalib]=0;
               charge_kgainch[iside][istrip][icalib]=0;
               charge_kgain[iside][istrip][icalib]=0;
               charge_ktheta[iside][istrip][icalib]=0;
               charge_ktemp[iside][istrip][icalib]=0;
            }
            continue;
         }
         for(int icalib=0;icalib<ncalib;icalib++){
            //printf("bef: L%d(tkid=%+4d) %s s%d icalib=%d Z0=%.2f %.2f: %.2f %.2f\n",JLayer,tkid,iside==0?"x":"y",istrip,icalib,Zx,Zx0,charge_kasymm[iside][istrip],charge_init);
            charge_kloss[iside][istrip][icalib]=fkLoss->GetChargeLossCorrectedValue(iside,(iside==0)?xcatg:0,istrip,eta_kasymm[iside][istrip],theta_kasymm[iside][istrip][iside],charge_init,(icalib==0&&Zx<=0)?Zx0:Zx,icalib,UseNewAlgo); //special treatmeant of the seed charge for kLoss correction
            charge_init=charge_kloss[iside][istrip][icalib];
            if(!(std::isfinite(charge_init))) charge_init=charge_kloss[iside][istrip][icalib]=0;
            //printf("aft: L%d(tkid=%+4d) %s s%d icalib=%d Z0=%.2f %.2f: %.2f %.2f\n",JLayer,tkid,iside==0?"x":"y",istrip,icalib,Zx,Zx0,charge_kasymm[iside][istrip],charge_init);

            charge_kgainch[iside][istrip][icalib]=fkGainCH->GetGainCHCorrectedValue(iside,istrip,tkid,(addr_kinit[iside][istrip]/64),(addr_kinit[iside][istrip]%64),charge_init,Zx,icalib,UseNewAlgo);
            charge_init=charge_kgainch[iside][istrip][icalib];
            if(!(std::isfinite(charge_init))) charge_init=charge_kgainch[iside][istrip][icalib]=0;

            charge_kgain[iside][istrip][icalib]=fkGain->GetGainCorrectedValue(iside,istrip,tkid,(addr_kinit[iside][istrip]/64),charge_init,Zx,icalib);
            charge_init=charge_kgain[iside][istrip][icalib];
            if(!(std::isfinite(charge_init))) charge_init=charge_kgain[iside][istrip][icalib]=0;

            charge_ktheta[iside][istrip][icalib]=fkTheta->GetThetaCorrectedValue(iside,istrip,theta_kasymm[iside][istrip][1-iside],charge_init,Zx,icalib,UseNewAlgo);
            charge_init=charge_ktheta[iside][istrip][icalib];
            if(!(std::isfinite(charge_init))) charge_init=charge_ktheta[iside][istrip][icalib]=0;

            bool IsEvent=false;
            if(AMSEventR::Head()) IsEvent=true;
            #ifndef __ROOTSHAREDLIBRARY__
            if(AMSEvent::gethead()) IsEvent=true;
            #endif
            if(!IsEvent) charge_ktemp[iside][istrip][icalib]=fkTemp->GetTempCorrectedValue(iside,istrip,JLayer,charge_init,Zx,icalib,run_current,time_current,UseNewAlgo);
            else charge_ktemp[iside][istrip][icalib]=fkTemp->GetTempCorrectedValue(iside,istrip,JLayer,charge_init,Zx,icalib,UseNewAlgo);
            charge_init=charge_ktemp[iside][istrip][icalib];
            if(!(std::isfinite(charge_init))) charge_init=charge_ktemp[iside][istrip][icalib]=0;

            charge_kall[iside][istrip]=charge_init;
            //if(JLayer==3&&iside==0) printf("Z0=%.2f Zx=%.2f iloop%d icalib%d S%d q=%.3f\n",Z0,Zx,iloop,icalib,istrip,charge_kall[iside][istrip]);
         }
         //get the seed charge from total signal measurement if seed charge is 0, which will be used for single strip charge measurement
         if(Zx<=0&&istrip==NSQ){
            if(charge_kall[iside][istrip]>0) Zx=charge_kall[iside][istrip];
         }
      }

      //combinning all strip charge measurments
      charge_kxy[iside]=StripCombination(charge_kall[iside],iside,JLayer,Zx,Opt);
      //modify it due to some shift
      MSplineH* spl=((Opt&kUnbias)&&(!IsMC))?fPeakCorrUnb[JLayer-1][iside]:fPeakCorr[JLayer-1][iside];
      if(spl&&charge_kxy[iside]>0){
         int nsol;
         double xsol[10];
         spl->EvalInverseMultiple(charge_kxy[iside],nsol,xsol);
         if(nsol==1) charge_kxy[iside]=xsol[0];
      }
      //printf("iloop%d L%d s%d: Z0=%.2f Zx=%.2f Zx0=%.2f qcomb=%.2f{%.2f,%.2f,%.2f,%.2f,%.2f,%.2f}\n",iloop,JLayer,iside,Z0,Zx,Zx0,charge_kxy[iside],charge_kall[iside][0],charge_kall[iside][1],charge_kall[iside][2],charge_kall[iside][3],charge_kall[iside][4],charge_kall[iside][5]);

      if(Z0<=0&&charge_kxy[iside]>0) Zx=charge_kxy[iside];
      if(Z0>0) break;
   }

   return charge_kxy[iside];
}

float TrChargeDBH::GetSqrtdEdX(int tkid,float *xadc,float *yadc,float *ip,float *ia,int *seedaddress,int imult,float Z0,int ncalib,int Opt){
   InitCharge(2);
   int OLayer=(int)(abs(tkid)/100);
   int JLayer=OLayer+1;
   if(OLayer==8) JLayer=1;
   if(OLayer==9) JLayer=9;
   if(JLayer<1||JLayer>9){
      return 0;
   }

   double Zx=Z0;
   for(int iloop=0;iloop<2;iloop++){
      //if(JLayer==2) printf("XYLoop%d: Z0=%.2f\n",iloop,Zx);
      for(int iside=0;iside<2;iside++){
         charge_kxy[iside]=GetSqrtdEdX(tkid,iside,iside==0?xadc:yadc,ip[iside],ia,seedaddress[iside],imult,Zx,ncalib,Opt);
         //if(JLayer==2) printf("Q%s=%.2f\n",iside==0?"x":"y",charge_kxy[iside]);
      }
      charge_kxy[2]=XYCombination(charge_kxy,JLayer,Zx,Opt);

      if(Z0<=0&&charge_kxy[2]>0) Zx=charge_kxy[2];
      //if(Z0<=0&&charge_kxy[0]>0) Zx=charge_kxy[0];
      if(Z0>0) break;
   }

   return charge_kxy[2];
}

float TrChargeDBH::GetQ2SqrtdEdX(float Z0,int JLayer,int iside,float beta,float rigidity,int Opt){
   if(Z0<=0) return Z0;
   if(iside<0||iside>2) return Z0;

   double sqrtdedx0=Z0;
   if((Opt&kRigidity)&&rigidity!=0){
      if((Opt&kBeta)&&beta!=0){
         MSplineH* spl_rig=fkRigidity->GetEDependence(fabs(rigidity),JLayer,iside,false);
         if(spl_rig){
            sqrtdedx0=spl_rig->Eval(sqrtdedx0);
            delete spl_rig;
         }
         MSplineH* spl_beta=fkBeta->GetEDependence(fabs(beta),JLayer,iside);
         if(spl_beta){
            sqrtdedx0=spl_beta->Eval(sqrtdedx0);
            delete spl_beta;
         }
      }
      else{
         MSplineH* spl_rig=fkRigidity->GetEDependence(fabs(rigidity),JLayer,iside,true);
         if(spl_rig){
            sqrtdedx0=spl_rig->Eval(sqrtdedx0);
            delete spl_rig;
         }
      }
   }
   else{
      if((Opt&kBeta)&&beta!=0){
         MSplineH* spl_beta=fkBeta->GetEDependence(fabs(beta),JLayer,iside);
         if(spl_beta){
            sqrtdedx0=spl_beta->Eval(sqrtdedx0);
            delete spl_beta;
         }
      }
   }

   return sqrtdedx0;
}

float TrChargeDBH::GetQ(float q0,int JLayer,int iside,float beta,float rigidity,int Opt){
   if(q0<=0) return 0;
   if(iside<0||iside>2) return q0;

   double charge0=q0;
   charge_kbeta[iside]=charge0;
   charge_krigidity[iside]=charge0;
   if((Opt&kBeta)&&beta!=0){
      charge0=fkBeta->GetBetaCorrectedValue(fabs(beta),JLayer,iside,charge0);
      charge_kbeta[iside]=charge0;
      if((Opt&kRigidity)&&rigidity!=0){
         charge0=fkRigidity->GetRigidityCorrectedValue(fabs(rigidity),JLayer,iside,charge0,false);
      }
   }
   else if((Opt&kRigidity)&&rigidity!=0){
      charge0=fkRigidity->GetRigidityCorrectedValue(fabs(rigidity),JLayer,iside,charge0,true);
   }
   charge_krigidity[iside]=charge0;

   return charge0;
}

float TrChargeDBH::GetEdep(int tkid,int iside,float *adc,float ip,float *ia,int seedaddress,int imult,float Z0,int ncalib,int Opt){
   double dedx=pow(GetSqrtdEdX(tkid,iside,adc,ip,ia,seedaddress,imult,Z0,ncalib,Opt),2);
   double costheta=1/sqrt(1+ia[0]*ia[0]+ia[1]*ia[1]);
   return dedx/costheta;
}
float TrChargeDBH::GetEdep(int tkid,float *xadc,float *yadc,float *ip,float *ia,int *seedaddress,int imult,float Z0,int ncalib,int Opt){
   double dedx=pow(GetSqrtdEdX(tkid,xadc,yadc,ip,ia,seedaddress,imult,Z0,ncalib,Opt),2);
   double costheta=1/sqrt(1+ia[0]*ia[0]+ia[1]*ia[1]);
   return dedx/costheta;
}

float TrChargeDBH::GetQ(int tkid,int iside,float *adc,float ip,float *ia,int seedaddress,int imult,float beta,float rigidity,float Z0,int ncalib,int Opt){
   double q0=GetSqrtdEdX(tkid,iside,adc,ip,ia,seedaddress,imult,Z0,ncalib,Opt);
   int OLayer=(int)(abs(tkid)/100);
   if(OLayer<1||OLayer>9){
      return 0;
   }
   int JLayer=OLayer+1;
   if(OLayer==8) JLayer=1;
   if(OLayer==9) JLayer=9;

   double result=GetQ(q0,JLayer,iside,beta,rigidity,Opt);
   return result;
}
float TrChargeDBH::GetQ(int tkid,float *xadc,float *yadc,float *ip,float *ia,int *seedaddress,int imult,float beta,float rigidity,float Z0,int ncalib,int Opt){
   double q0=GetSqrtdEdX(tkid,xadc,yadc,ip,ia,seedaddress,imult,Z0,ncalib,Opt);
   int OLayer=(int)(abs(tkid)/100);
   if(OLayer<1||OLayer>9){
      return 0;
   }
   int JLayer=OLayer+1;
   if(OLayer==8) JLayer=1;
   if(OLayer==9) JLayer=9;

   for(int iside=0;iside<2;iside++) GetQ(charge_kxy[iside],JLayer,iside,beta,rigidity,Opt);

   double result=GetQ(q0,JLayer,2,beta,rigidity,Opt);
   return result;
}

float TrChargeDBH::GetMCSmearing(float q0,float nrand[2],int JLayer,int iside,bool isunb,int Z){
   if(!TrChargeDBH::IsMC) return q0;
   if(nrand[0]==0) return q0;
   if(q0<=0) return 0;
   if(iside<0||iside>2) return 0;
   double result=0;
   //the charge resolution tuning
   double normarr[NMCQpars];
   double peakarr[NMCQpars];
   double sigmarr[NMCQpars];
   for(int ii=0;ii<NMCQpars;ii++){
      normarr[ii]=0;
      peakarr[ii]=0;
      sigmarr[ii]=0;
      MSplineH* spl0=0;
      MSplineH* spl1=0;
      MSplineH* spl2=0;
      if(JLayer>=1&&JLayer<=9){
         spl0=isunb?fMCNormLayerUnb[ii][JLayer-1][iside]:fMCNormLayer[ii][JLayer-1][iside];
         spl1=isunb?fMCPeakLayerUnb[ii][JLayer-1][iside]:fMCPeakLayer[ii][JLayer-1][iside];
         spl2=isunb?fMCSigmLayerUnb[ii][JLayer-1][iside]:fMCSigmLayer[ii][JLayer-1][iside];
      }
      else if(JLayer<1){
         spl0=fMCNormAll[ii][iside]?fMCNormAll[ii][iside]:fMCNormInner[ii][iside];
         spl1=fMCPeakAll[ii][iside]?fMCPeakAll[ii][iside]:fMCPeakInner[ii][iside];
         spl2=fMCSigmAll[ii][iside]?fMCSigmAll[ii][iside]:fMCSigmInner[ii][iside];
         if(isunb){
         spl0=fMCNormAllUnb[ii][iside]?fMCNormAllUnb[ii][iside]:fMCNormInnerUnb[ii][iside];
         spl1=fMCPeakAllUnb[ii][iside]?fMCPeakAllUnb[ii][iside]:fMCPeakInnerUnb[ii][iside];
         spl2=fMCSigmAllUnb[ii][iside]?fMCSigmAllUnb[ii][iside]:fMCSigmInnerUnb[ii][iside];
         }
      }
      else{
         spl0=isunb?fMCNormInnerUnb[ii][iside]:fMCNormInner[ii][iside];
         spl1=isunb?fMCPeakInnerUnb[ii][iside]:fMCPeakInner[ii][iside];
         spl2=isunb?fMCSigmInnerUnb[ii][iside]:fMCSigmInner[ii][iside];
      }
      if(!(spl0&&spl1&&spl2)) continue;
      normarr[ii]=spl0->Eval(q0);
      peakarr[ii]=spl1->Eval(q0);
      sigmarr[ii]=spl2->Eval(q0);
   }
   double sum=0;
   for(int ii=0;ii<NMCQpars;ii++) sum+=normarr[ii];
   if(sum<=0) return q0;
   double sumarr[NMCQpars];
   for(int ii=0;ii<NMCQpars;ii++){
      sumarr[ii]=0;
      for(int i2=0;i2<=ii;i2++) sumarr[ii]+=normarr[i2]/sum;
   }
   for(int ii=0;ii<NMCQpars;ii++){
      if(!(nrand[1]>(ii-1>=0?sumarr[ii-1]:0)&&nrand[1]<=sumarr[ii])) continue;
      result+=(peakarr[ii]+nrand[0]*sigmarr[ii]);
   }
   result+=q0;
   //the charge shape tuning
   if(Z>0&&Z<NZQ){
      MSplineH* spl=0;
      if(JLayer>=1&&JLayer<=9){
         spl=isunb?kMCConvLayerUnb[JLayer-1][iside][Z-1]:kMCConvLayer[JLayer-1][iside][Z-1];
      }
      else if(JLayer<1){
         spl=isunb?kMCConvAllUnb[iside][Z-1]:kMCConvAll[iside][Z-1];
      }
      else{
         spl=isunb?kMCConvInnerUnb[iside][Z-1]:kMCConvInner[iside][Z-1];
      }
      if(spl){
         result=spl->Eval(result);
      }
   }

   return result;
}
