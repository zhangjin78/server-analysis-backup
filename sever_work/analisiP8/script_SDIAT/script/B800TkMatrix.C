///---Helium Resolution Matrix by Q.Yan 
//0: Double_t emgn1tgaus(Double_t *x,Double_t *par) : model
//1: GetEMGTkPDF(double rgen,double scale=1,int version=0,double shift=0,double copt=0,double cfrac=0) : He 1036-115MC resolution parameterization 
//2: DrawTkMatrixV(double rgen=150,double shift=0) : draw example 
#ifndef _TkMatrix_ 
#define _TkMatrix_
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include "TColor.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TLegend.h"
#include "TDatime.h"
#include "TProfile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TArrayD.h"
#include "TCanvas.h"
#include "QSplineFit.C"

//#define NTestMatrix

//--Gauassian resolution for exponital distribution
Double_t emgn(Double_t *x,Double_t *par){
  Double_t mean=(x[0]-par[2]);
  double sigvtau=par[1]/par[0];
  if(sigvtau>10){sigvtau=10;par[0]=par[1]/sigvtau;}//Tau=Sigma/10
  Double_t pdfv=(1./2.*sigvtau/par[1])*TMath::Exp(sigvtau*sigvtau/2.-mean*sigvtau/par[1])*TMath::Erfc(1./sqrt(2)*(sigvtau-mean/par[1])); 
  return par[3]*pdfv;
}

//--Gauassian resolution for exponital distribution
Double_t emgn1(Double_t *x,Double_t *par){
  Double_t mean=(x[0]-par[2]);//X[0]-Mean
  double sigvtau=par[1]/par[0];//Sigma/Tau
  double ww=1;
  double cuthv=10;
  if(sigvtau>cuthv){ww=cuthv/sigvtau;sigvtau=cuthv;};
  double emgvar=TMath::Erfc(1./sqrt(2)*(sigvtau-mean/par[1]));
  if(emgvar!=0){
     emgvar=ww*(1./2.*sigvtau/par[1])*TMath::Exp(sigvtau*sigvtau/2.-mean*sigvtau/par[1])*emgvar+(1-ww)*TMath::Gaus(x[0],par[2],par[1],1);
     emgvar=par[3]*emgvar;
  }
 return emgvar;
}

//--Gauassian resolution for exponital distribution3
Double_t emgn1tgaus(Double_t *x,Double_t *par){
  Double_t paremg[4];
  paremg[0]=par[2]*par[6]*par[7];//Tau //0:Norm 1:Mean 2:Sigma 3:Frac 4:Sigma2 5:Frac2 6:Sigma3 7:Tau
  paremg[1]=par[2]*par[6];//Sigma2
  paremg[2]=par[1];//Mean;
  paremg[3]=1;//Frac2
  return par[0]*(par[3]*TMath::Gaus(x[0],par[1],par[2],1)+(1-par[3])*par[5]*TMath::Gaus(x[0],par[1],par[2]*par[4],1)
         +(1-par[3])*(1-par[5])*emgn1(x,paremg));
}

///--1nd Gaus Mean/Sigma
Double_t tkmeanfun(Double_t *x,Double_t *par){
  double xv=log(x[0]);
  return par[0]/pow(xv-par[3],par[1])+par[2];
}

///Tracker resolution PDF 1/Rrec-1/Rgen[TV-1] for MC Generate Rigidity[rgen]
///He B1036-115MC+Point Tuning version=36
TF1 *GetEMGTkPDF(double rgen,double scale=1,int version=36,double shift=0,double copt=0,double cfrac=0){//Alignment Error +-1/26TV*1000
   
  double rgenr=rgen;
  if(rgenr<0.5)rgenr=0.5;
  if     (rgen<0.5){rgen=0.5;cout<<"PDF outof Limit"<<endl;}//Over Limit
  else if(rgen>4000)rgen=4000;
//---
  static TF1 *fundgv1036115He4NP=new TF1("emgn1tgausv1036115He4NP",emgn1tgaus,-10000,10000,8);//B1036115-He4 2015-10-31(id=36) Q.Yan Tuning
  TF1 *fundg=0;
  if     (version>=30&&int(version)%10==6)fundg=fundgv1036115He4NP;//B1036 2014-10-31(id=36)
  fundg->SetParNames("Scale","Mean","Sigma","Frac","Sigma2","Frac2","Sigma3","Tau");
  fundg->SetParameter(0,scale);//Scale
 
//---
  double fixpar[10]={0};
  double usr=rgen;

//---1nd Gaus Simga
  usr=rgen;
  if(usr>4000)usr=4000;
  if(version>=30&&int(version)%10==6){//B1036-115 id=36
    if     (usr<0.5)usr=0.5;
    else if(usr>5000)usr=5000;
    const int nnodes=8;
    double nodes[nnodes]={0.5,3.,10,30,100,300,1000,5000};
    double ynodes[nnodes]={1036.45,36.7692,9.41132,3.406,1.4046,0.607173,0.358206,0.302881};
    static QSplineFit *tkresspline=new QSplineFit("1ndGausSigmaS1036115He4NP",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::LogY),"b2e1");
    fixpar[2]=tkresspline->GetFun()->Eval(usr);;
  }
//---Sigma=Sigma*(1+cfrac)
  if(copt==2)fixpar[2]=fixpar[2]*(1+cfrac);//==1 Change-MDR Sigma
#ifdef NTestMatrix
  cout<<"ipar=2"<<" xv="<<usr<<" fixpar="<<fixpar[2]<<endl;
#endif
  fundg->SetParameter(2,fixpar[2]);

//---Range Set
  fundg->SetRange(-30*fixpar[2],30*fixpar[2]);
  fundg->SetNpx(1000);

//---1nd Gaus Mean
   usr=rgen;
   if(usr>4000)usr=4000;
   if(version>=30&&int(version)%10==6){//B1036-115 id=36
      if     (usr<1.3)usr=1.3;
      else if(usr>4000)usr=4000;
      double fpar1[]={0.414112,1.75581,-0.0121386,-0.388694};
      fixpar[1]=tkmeanfun(&usr,fpar1);
      if(fixpar[1]>0.8)fixpar[1]=0.8;
   }
   fixpar[1]=fixpar[1]*fixpar[2];//Mean*Sigma
   fixpar[1]=fixpar[1]+shift;
   if(copt==1)fixpar[1]=fixpar[1]+1./rgenr*1000.*cfrac;//+-0.24%=0.0024
#ifdef NTestMatrix
   cout<<"ipar=1"<<" xv="<<usr<<" fixpar="<<fixpar[1]<<endl;
#endif   
   fundg->SetParameter(1,fixpar[1]); 

//---1nd Gaus Fraction
   usr=rgen;
   if(version>=30&&int(version)%10==6){ //B1036-115 id=36
      if     (usr<1.3)usr=1.3;
      else if(usr>4000)usr=4000;
      const int nnode=10;
      double node[nnode]={1,2,4,10,25,50,100,250,500,5000};
      double ynode[nnode]={0.642082,0.664896,0.570502,0.766422,0.790881,0.770507,0.775384,0.745987,0.744984,0.776709};
      static QSplineFit *gausfrac=new QSplineFit("1ndGausFracU1036115He4NP",nnode,node,ynode,(QSplineFit::LogX2),"b1e1");
      fixpar[3]=gausfrac->GetFun()->Eval(usr);
   }
  if(copt==3){
     fixpar[3]=fixpar[3]*(1+cfrac);//Frac 1nd-Gaus Frac
     if     (fixpar[3]<0)fixpar[3]=0;
     else if(fixpar[3]>1)fixpar[3]=1;
   }
#ifdef NTestMatrix
  cout<<"ipar=3"<<" xv="<<usr<<" fixpar="<<fixpar[3]<<endl;
#endif   
   fundg->SetParameter(3,fixpar[3]);     
 
//---2nd/1nd Gaus Sigma
   usr=rgen;
   if(version>=30&&int(version)%10==6){ //B1036 id=36
     if     (usr<1)usr=1;
     else if(usr>4000)usr=4000;
     const int nnodes2=10;
     double nodes2[nnodes2]={0.8,3,10,30,60,120,240,500,1000,4000};
     double ynodes2[nnodes2]={0.965797,1.29335,1.49105,1.74417,1.9525,2.50286,2.90489,2.72067,2.77307,2.35653};
     static QSplineFit *gaus2ndsigmau=new QSplineFit("Gaus2nd_SigmaU1036115He4TP",nnodes2,nodes2,ynodes2,(QSplineFit::LogX2),"b1e1");
     fixpar[4]=gaus2ndsigmau->GetFun()->Eval(usr);
   }
   if(copt==4)fixpar[4]=fixpar[4]*(1+cfrac);//Frac 2nd-Sigma/1nd-Sigma
#ifdef NTestMatrix
   cout<<"ipar=4"<<" xv="<<usr<<" fixpar="<<fixpar[4]<<endl;
#endif
   fundg->SetParameter(4,fixpar[4]);

//---Par5 2nd Gaus Fraction
   usr=rgen;
   if(version>=30&&int(version)%10==6){ //B1036-115 id=36
       if     (usr<1.3)usr=1.3;
       else if(usr>4000)usr=4000;
       const int nnode2g=10;
       double node2g[nnode2g]={0.8,2,3,10,30,100,200,400,1000,6000};
       double ynode2g[nnode2g]={-1.69374,0.515147,0.813829,0.834806,0.913099,0.970863,0.92339,0.747098,0.811942,0.836167};
       static QSplineFit *gaus2ndfracu=new QSplineFit("Gaus2nd_Frac2U1036115He4NP",nnode2g,node2g,ynode2g,(QSplineFit::LogX2),"b2e1");
       fixpar[5]=gaus2ndfracu->GetFun()->Eval(usr);
       if(fixpar[5]<1e-4)fixpar[5]=1e-4;
   }
   if(copt==5)fixpar[5]=fixpar[5]*(1+cfrac);//Frac 2nd Frac
   if     (fixpar[5]<0)fixpar[5]=0;
   else if(fixpar[5]>1)fixpar[5]=1;
#ifdef NTestMatrix
   cout<<"ipar=5"<<" xv="<<usr<<" fixpar="<<fixpar[5]<<endl;
#endif
   fundg->SetParameter(5,fixpar[5]);

//--Par6 3nd Gaus(EMG) Sigma
   usr=rgen;
   if(version>=30&&int(version)%10==6){ //B1036 id=36
      if     (usr>4000)usr=4000;
      else if(usr<1.3)usr=1.3;
      const int nnode2=8;
      double node2[nnode2]={0.8,5,20,27,150,300,800,4000};
      double ynode2[nnode2]={0.878203,1.10309,1.32596,1.82971,7.15706,7.07895,8.81648,14.6988};
      static QSplineFit *emggaussigma=new QSplineFit("emggaussigmaB1036115He4NP",nnode2,node2,ynode2,(QSplineFit::LogX2),"b1e1");
      fixpar[6]=emggaussigma->GetFun()->Eval(usr);
   }
   if(copt==6)fixpar[6]=fixpar[6]*(1+cfrac);//Frac 2nd Frac
#ifdef NTestMatrix
   cout<<"ipar=6"<<" xv="<<usr<<" fixpar="<<fixpar[6]<<endl;
#endif
   fundg->SetParameter(6,fixpar[6]);

//--3nd EMG Tau
   usr=rgen;
   if(version>=30&&int(version)%10==6){ //B1036-115 id=36
      if     (usr<1.3)usr=1.3;
      else if(usr>4000)usr=4000;
      const int nnodet=8;
      double nodet[nnodet]={0.8,4,12,33,100,300,1000,5000};
      double ynodet[nnodet]={0.858305,1.54279,2.12978,1.13214,0.479974,0.113533,0.019989,0.0179627};
      static QSplineFit *tktau2=new QSplineFit("TauParU1036115He4NP",nnodet,nodet,ynodet,(QSplineFit::LogX2),"b1e1");
      fixpar[7]=tktau2->GetFun()->Eval(usr);
      if(fixpar[7]<1e-6)fixpar[7]=1e-6;
   }
   if(copt==7)fixpar[7]=fixpar[7]*(1+cfrac);//3nd EMG Tau
#ifdef NTestMatrix
   cout<<"ipar=7"<<" xv="<<usr<<" fixpar="<<fixpar[7]<<endl;
#endif
   fundg->SetParameter(7,fixpar[7]);

//---
  return fundg;
}

///--Draw
int DrawTkMatrixV(double rgen=150,double shift=0){

  TCanvas *c=new TCanvas();
  gPad->SetLogy();
  TF1 *femgv1036115He4NP=GetEMGTkPDF(rgen,1,36,shift);
  femgv1036115He4NP->GetXaxis()->SetTitle("1/R_{Rec}-1/R_{Gen}[TV-1]");
  femgv1036115He4NP->GetXaxis()->SetTitleColor(1);
  femgv1036115He4NP->SetTitle(Form("Helium MC d(1/R) Rig%.1fGV L1InnerL9",rgen));
  femgv1036115He4NP->SetLineColor(2);
  femgv1036115He4NP->Draw();
  return 0;
}

#endif
