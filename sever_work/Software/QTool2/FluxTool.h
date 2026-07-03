#ifndef _fluxtool_
#define _fluxtool_
#include <signal.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include  "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include <TGraphErrors.h>
#include "TGraphAsymmErrors.h"
#include "TH2D.h"
#include <TPad.h>
#include "TFile.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TSpline.h"
#include "TLegend.h"

///--Scale by TGraphErrors PowerLaw
TGraphErrors* ScaleGraphFlux(TGraphErrors* gflux,double scaleindex,double normal=1.){

  TGraphErrors* gr1=(TGraphErrors* )gflux->Clone();
  for(int ip=0;ip<gr1->GetN();ip++){
     double xv=0,yv=0,ey=0;
     gr1->GetPoint(ip,xv,yv);
     ey=gr1->GetErrorY(ip);
     yv=yv*pow(xv,scaleindex)*normal;
     ey=ey*pow(xv,scaleindex)*normal;
     gr1->SetPoint(ip,xv,yv);
     gr1->SetPointError(ip,0,ey);
   }
   return gr1;
}

///--Scale by TGraphAsymmErrors PowerLaw
TGraphAsymmErrors* ScaleGraphFlux(TGraphAsymmErrors* gflux,double scaleindex,double normal=1.){

  TGraphAsymmErrors* gr1=(TGraphAsymmErrors* )gflux->Clone();
  for(int ip=0;ip<gr1->GetN();ip++){
     double xv=0,yv=0,eyl=0,eyh=0;
     gr1->GetPoint(ip,xv,yv);
     eyl=gr1->GetErrorYlow(ip);
     eyh=gr1->GetErrorYhigh(ip);
     yv=yv*pow(xv,scaleindex)*normal;
     eyl=eyl*pow(xv,scaleindex)*normal;
     eyh=eyh*pow(xv,scaleindex)*normal;
     gr1->SetPoint(ip,xv,yv);
     gr1->SetPointError(ip,0,0,eyl,eyh);
   }
   return gr1;
}


///--Divide by TGraphErrors
TGraphErrors* DivideFlux(TGraphErrors* gflux,TGraphErrors* gflux2,int eopt=1){

  TGraphErrors* grn=(TGraphErrors* )gflux->Clone();
  grn->Set(0);
  int ip0=0;
  TGraphErrors* gr1=gflux;
  TGraphErrors* gr2=gflux2;
  for(int ip=0;ip<gr1->GetN();ip++){
     double xv=0,yv=0,ey=0;
     gr1->GetPoint(ip,xv,yv);
     ey=gr1->GetErrorY(ip);
     double xv2=0,yv2=0,ey2=0;
     for(int ip2=0;ip2<gr2->GetN();ip2++){
         gr2->GetPoint(ip2,xv2,yv2);
         ey2=gr2->GetErrorY(ip2);
         if(eopt==2)ey2=0;
//         if(fabs(xv2-xv)<=fabs(xv)*0.01){
         if(fabs(xv2-xv)<=fabs(xv2+xv)*0.001){
            double yvn=yv/yv2;
            double eyn=yvn*sqrt(pow(ey/yv,2)+pow(ey2/yv2,2));
            grn->SetPoint(ip0,xv,yvn); 
            if(eopt==0)eyn=0;
            grn->SetPointError(ip0++,0,eyn);
         }
         else continue;
     }
   }
   return grn;
}

///--Divide by TGraphErrors
TGraphErrors* AverageFlux(TGraphErrors* gflux[],int ng=1,int eopt=1,int iref=0){

  TGraphErrors* gav=(TGraphErrors* )gflux[iref]->Clone();
//-----
  for(int ip=0;ip<gav->GetN();ip++){
     double xvn=0,yvn=0,eyn=0;
     gav->GetPoint(ip,xvn,yvn);
     eyn=gav->GetErrorY(ip);
     int iug=1;
     for(int ig=0;ig<ng;ig++){
       if(ig==iref)continue;
       TGraphErrors* gn2=gflux[ig];
       if(gn2==0)continue;
       double xv2=0,yv2=0,ey2=0;
//---Find Other
       for(int ip2=0;ip2<gn2->GetN();ip2++){
         gn2->GetPoint(ip2,xv2,yv2);
         ey2=gn2->GetErrorY(ip2);
         if(fabs(xv2-xvn)>fabs(xvn)*0.01)continue;
         yvn+=yv2;
         eyn+=ey2;
         iug++;//if found break
         break;
       }
//---
     }
     yvn=yvn/iug;
     eyn=eyn/iug;
     if(eopt==0)eyn=0;
     gav->SetPoint(ip,xvn,yvn);
     gav->SetPointError(ip,0,eyn);
  }
  return gav;
}

//----
TH1* AverageTH1(TH1* h[],int ng=1,int eopt=1){
   TH1 *hav=0;
   int iuh=0;
   for(int ih=0;ih<ng;ih++){
      TH1 *hn=h[ih];
      if(hn==0)continue;
      if(hav==0)hav=(TH1D *)hn->Clone();
      else hav->Add(hn);
      iuh++;
   }
   if(hav)hav->Scale(1./iuh);
   return hav;
}


///--Scale by TH1 PowerLaw
TH1* ScaleTH1Flux(TH1* hflux,double scaleindex,double normal=1.){
 
  TH1* hn=(TH1 *)hflux->Clone();
  for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
     double xv=hn->GetXaxis()->GetBinCenterLog(ibx);
     double yv=hn->GetBinContent(ibx)*pow(xv,scaleindex)*normal;
     double ey=hn->GetBinError(ibx)*pow(xv,scaleindex)*normal;
     hn->SetBinContent(ibx,yv);
     hn->SetBinError(ibx,ey);
   }
   return hn;
}

///--
int ConvertEU(int Z,double &Rig,double &KE,int opt){//opt*10 isotropic
   double A;
   if     (Z==5){A=11;if(opt/10==1)A=10;}//B 11}
   else          A=Z*2;//C O or Other
   if(opt%10==0)KE =sqrt(pow(Z/A*Rig,2)+0.938053*0.938053)-0.938053;//rig->kinetic
   else         Rig=(A/Z)*sqrt(pow(KE+0.938053,2)-0.938053*0.938053);//kinetic->rig
   return 0;
}

///--
double ConvertER(double ZVA,double Energy,int opt=1){//opt=0 Rig->Ekin/n, opt=1 Ekin/n->Rig
   static double PMass=0.938053;
   double EnergyN=Energy;
   if(opt%10==0)EnergyN =sqrt(pow(ZVA*fabs(Energy),2)+PMass*PMass)-PMass;//Rig->Ekin/n
   else         EnergyN=(1./ZVA)*sqrt(pow(fabs(Energy)+PMass,2)-PMass*PMass);//Ekin/n->Rig
   if(Energy<0)EnergyN=-EnergyN;
   return EnergyN;
}

///---Event/ExpoTime
TH1* GetEvT(TH1 *hevf,TH1 *htime,int ivopt=0,int chz=0){
  TH1 *hevt=(TH1 *)hevf->Clone();
  hevt->Reset();
  for(int ib=1;ib<=hevt->GetNbinsX();ib++){
     double nev=hevf->GetBinContent(ib);
     double enev=hevf->GetBinError(ib);
     double rv=(ivopt==1)?1./hevf->GetBinCenter(ib):hevf->GetBinCenter(ib);
     int tb=htime->GetXaxis()->FindBin(fabs(rv));
     if(tb>=htime->GetNbinsX()||(chz>=1&&rv<0)||(chz<=-1&&rv>0))tb=htime->GetNbinsX();
     double dt=htime->GetBinContent(tb);
//     cout<<"rv="<<rv<<" dt="<<dt<<endl;
     if(dt==0)continue;
     hevt->SetBinContent(ib,nev/dt);
     hevt->SetBinError(ib,enev/dt);//Statistic-Error only
     double lr=hevt->GetXaxis()->GetBinLowEdge(ib);
     double hr=hevt->GetXaxis()->GetBinUpEdge(ib);
     if     (lr==0)lr=hr/2;
     else if(hr==0)hr=lr/2;
     double xv=1./sqrt(lr*hr);
#ifdef OUTPUT
     cout<<"rig="<<xv<<" nev="<<nev<<" enev="<<enev<<" enev2="<<sqrt(nev)<<" ratio="<<(enev/sqrt(nev))<<" dt="<<dt<<endl;
#endif
  }
  return hevt;
}

///---R-bin Unfold
TH1* GetUnfoldHisto(TH1 *hevt,TH1 *hunfoldw){
  TH1 *hflux=(TH1 *)hevt->Clone();
  for(int ib=1;ib<=hflux->GetNbinsX();ib++){
//--binX
     double xv=hflux->GetXaxis()->GetBinCenterLog(ib);
//--Events/T
     double nev=hflux->GetBinContent(ib);
     double enev=hflux->GetBinError(ib);
//--Unfold-Ratio
     int uwb=hunfoldw->GetXaxis()->FindBin(xv);
     if(uwb>=hunfoldw->GetNbinsX())uwb=hunfoldw->GetNbinsX();
     double uwv=hunfoldw->GetBinContent(uwb);
//--Flux
     hflux->SetBinContent(ib,nev/uwv);
     hflux->SetBinError(ib,enev/sqrt(uwv));
   }
   return hflux;
}

//---RBin flux->KinBin flux()
TH1* ConvertERFlux(TH1 *h,double ZVA,int opt=0){//opt=0 Rig->Ekin/n, opt=1 Ekin/n->Rig
  TH1 *h1=(TH1 *)h->Clone();
//---bin
  double xb[10000];
  int ub=0;
  for(int ib=1;ib<=h1->GetNbinsX()+1;ib++){
    double xl=h->GetXaxis()->GetBinLowEdge(ib);
    xb[ub]=ConvertER(ZVA,xl,opt);
//    cout<<"zva="<<ZVA<<" ub="<<ub<<" xl="<<xl<<" xb="<<xb[ub]<<endl;
    ub++;
  }
  h1->Reset();
  h1->SetBins(ub-1,xb);
//---content
  for(int ib=1;ib<=h1->GetNbinsX();ib++){
    double yv=h->GetBinContent(ib);
    double ey=h->GetBinError(ib);
    if(yv==0)continue;
    double dx=h->GetXaxis()->GetBinUpEdge(ib)-h->GetXaxis()->GetBinLowEdge(ib);
    double dx1=h1->GetXaxis()->GetBinUpEdge(ib)-h1->GetXaxis()->GetBinLowEdge(ib);
    double yv1=yv*dx/dx1;
    double ey1=ey*dx/dx1;
    h1->SetBinContent(ib,yv1);
    h1->SetBinError(ib,ey1);
  }
  return h1;
}

#endif
