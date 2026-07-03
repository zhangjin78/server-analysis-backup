#ifndef FLUXWEIGHTFF
#define FLUXWEIGHTFF
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
#include "TH2D.h"
#include <TPad.h>
#include "TFile.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TSpline.h"
#include "TLegend.h"
#include "QSplineFit.C"
#include "FluxTool.h"

///---Cosmic Fit Function
Double_t cosflux(Double_t *x,Double_t *par){ 

   double R=x[0];//Rigidity
   double Z=par[0];//Charge
   double A=par[1];//Mass
   double PHIV=par[2];//solar modulation energy GV
   double c=par[3];//nomalization
   double a=par[4];//Flux index -2.7
   double b=par[5];//Flux index correction if no 0
   double d=par[6];//Scale with R(should be fixed)
//---
   const double  PMass=0.938053;
//--
   double KE =sqrt(pow(Z/A*R,2)+PMass*PMass)-PMass;//Kinetic/nuclei
//---Solar Modulation Item
   double PHI=Z/A*PHIV;//phi0
   double SUNV=KE*(KE+2*PMass)/(KE+PHI)/(KE+PHI+2*PMass);
//---LIS Flux
   double NKE=KE+PHI;
   double PA=sqrt(NKE*(NKE+2*PMass));//p/a=R*Z/A New Rigidity
//---
   double fluxv=SUNV*c*pow(PA,a+b*log(PA))*pow(R,d);
//   double fluxv=SUNV*c*exp(a*log(PA)+b*pow(log(PA),2));
//   double fluxv=SUNV*(c*pow(PA,a)+b*pow(PA,e));
//---
   return fluxv;
}

///---Cosmic Flux GetWeight Function
double GetISSFluxW(double MCGenRig,double MCindex=-1,double Rescale=1.,int version=1,int z=2,int mparid=0){
//---Ratio  
   if(z>100){
     double z1=z/100,z2=z%100;
     return GetISSFluxW(MCGenRig,MCindex,Rescale,version,z1,mparid)/GetISSFluxW(MCGenRig,MCindex,Rescale,version,z2,mparid);   
   }
//---if not exist use helium
//   if(z!=1&&z!=2&&z!=3&&z!=4&&z!=5&&z!=6&&z!=7&&z!=8&&z!=10&&z!=12&&z!=14&&z!=16&&z!=26){
   if(!((z>=1&&z<=14)||z==16||z==26)){
     if(z>8)z=8;
     else   z=2; 
   }
//--
   TF1 *cosfluxfunvn=0;
   if(z==6){
     if(version==3){
//---Based on 2016-10-17(Q.Yan NScale-Carbon 1075203MC Flux) 
       const int nnode=10;
       double node[nnode]={1.6,2.5,3.2,5,8,15,40,100,400,10000};
       double ynode[nnode]={1.32572,1.48236,1.1716,0.594254,0.230578,0.0525963,0.00418315,0.000348857,8.3062e-06,2.09934e-09};
       static QSplineFit *fluxspline=new QSplineFit("Carbon_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
     else if(version==2){
//---Based on 2016-05-20(Q.Yan B1064_201 Carbon Flux)
       const int nnode=8;
       double node[nnode]={1.63256,2.50325,3.26511,7.6186,27.2093,97.9534,435.348,25000};
       double ynode[nnode]={1.25261,1.45057,1.14796,0.260603,0.011676,0.000373725,6.54869e-06,1.34838e-10};
       static QSplineFit *fluxspline=new QSplineFit("Carbon_Weight_Flux_V2N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
     else {
//---Based on 2016-04-18(Q.Yan B1064 Carbon Flux)
       const int nnode=8;
       double node[nnode]={1.63256,2.50325,3.26511,7.6186,27.2093,97.9534,435.348,25000};
       double ynode[nnode]={1.21849,1.43498,1.13922,0.255558,0.0114359,0.000366017,6.28733e-06,1.12679e-10};
       static QSplineFit *fluxspline=new QSplineFit("Carbon_Weight_Flux_V1N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
   }
//---Based on 2020-04-28(Q.Yan NScale-Fe_1220401MC Flux)
   else if(z==26||z==28){
     const int nnode=7;
     double node[nnode]={1.9,4,8,21,70,250,10000};
     double ynode[nnode]={0.174715,0.0716041,0.020846,0.00256751,0.000139801,4.76576e-06,3.55317e-10};
     static QSplineFit *fluxspline=new QSplineFit("Fe56_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
//---Based on 2019-04-30(Q.Yan NScale-S32_1215401MC Flux)
   else if(z==16){
     const int nnode=8;
     double node[nnode]={1.9,3.5,4.5,8,21,70,250,10000};
//     double ynode[nnode]={0.0433681,0.0316844,0.021454,0.00724318,0.000727922,3.13505e-05,8.78888e-07,6.03463e-11};
     double ynode[nnode]={0.0422544,0.0314966,0.0213807,0.00722593,0.000730238,3.13598e-05,8.85657e-07,6.51369e-11};
     static QSplineFit *fluxspline=new QSplineFit("S32_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==14){
//---Based on 2019-03-31(Q.Yan NScale-Si_1215401MC Flux)
     const int nnode=8;
     double node[nnode]={1.9,3.5,4.5,8,21,70,250,10000};
     double ynode[nnode]={0.272682,0.141019,0.0977524,0.0334704,0.00351319,0.000160144,4.83227e-06,4.39757e-10};
     static QSplineFit *fluxspline=new QSplineFit("Si28_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==13){
//---Based on 2020-07-20(Q.Yan NScale-Al_1220402MC Flux)
     const int nnode=7;
     double node[nnode]={1.9,3.5,4.5,8,40,200,10000};
     double ynode[nnode]={0.0462687,0.040488,0.0268992,0.00781462,0.000129095,1.37899e-06,3.71625e-11};
     static QSplineFit *fluxspline=new QSplineFit("Aluminum_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==12){
//---Based on 2019-03-31(Q.Yan NScale-Mg_1215401MC Flux)
     const int nnode=8;
     double node[nnode]={1.9,3.5,4.5,8,21,70,250,10000};
     double ynode[nnode]={0.381324,0.177548,0.125729,0.0422082,0.00421066,0.000177783,5.21675e-06,4.45155e-10};
     static QSplineFit *fluxspline=new QSplineFit("Mg24_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==11){
//---Based on 2020-07-20(Q.Yan NScale-Na_1220402MC Flux)
     const int nnode=7;
     double node[nnode]={1.9,3.5,4.5,8,40,200,10000};
     double ynode[nnode]={0.0616907,0.0358898,0.0235392,0.00685556,9.68874e-05,9.06974e-07,5.87682e-12};
     static QSplineFit *fluxspline=new QSplineFit("Sodium_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==10){
//---Based on 2019-03-31(Q.Yan NScale-Neon_1213401MC Flux)
     const int nnode=8;
     double node[nnode]={1.9,3.5,4.5,8,21,70,250,10000};
     double ynode[nnode]={0.255068,0.141128,0.100652,0.0343909,0.0034131,0.000145596,4.24793e-06,4.05798e-10};
     static QSplineFit *fluxspline=new QSplineFit("Neon_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==9){
//---Based on 2020-07-20(Q.Yan NScale-F_1220402MC Flux)
     const int nnode=7;
     double node[nnode]={1.9,3.5,4.5,8,40,200,10000};
     double ynode[nnode]={0.0327181,0.020895,0.0140331,0.00414912,5.25968e-05,4.11461e-07,5.85398e-12};
     static QSplineFit *fluxspline=new QSplineFit("Fluorine_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
     cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==8){
     if(version==3){
//---Based on 2016-10-17(Q.Yan NScale-Oxygen_1075204MC Flux) 
       const int nnode=8;
       double node[nnode]={1.9,3,3.7,8,25,80,350,12500};
       double ynode[nnode]={1.58004,1.17232,0.915573,0.225991,0.0149017,0.000692305,1.27111e-05,1.34985e-09};
       static QSplineFit *fluxspline=new QSplineFit("Oxygen_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
     else if(version==2){
//---Based on 2016-05-20(Q.Yan B1064_202 Oxygen Flux) 
       const int nnode=8;
       double node[nnode]={1.9 ,3., 3.7,  8,  25.,  80. , 350, 25000};
       double ynode[nnode]={1.35458,1.19029,0.947692,0.231649,0.0154627,0.000736339,1.27988e-05,1.92306e-10};
       static QSplineFit *fluxspline=new QSplineFit("Oxygen_Weight_Flux_V2N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
     else { 
//---Based on 2016-04-18(Q.Yan Oxygen Flux)
       const int nnode=8;
       double node[nnode]={1.9 ,3., 3.7,  8,  25.,  80. , 350, 25000};
       double ynode[nnode]={1.33582,1.15938,0.915388,0.228408,0.0150704,0.000710665,1.23762e-05,1.787e-10};
       static QSplineFit *fluxspline=new QSplineFit("Oxygen_Weight_Flux_V1N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
   }
   else if(z==7){
//---Based on 2017-01-02(Q.Yan NScale-Nitrogen_1087201MC Flux)
       int nnode=8;
       double node[100]={1.5,2.3,3,7,21,70,250,10000};
       double ynode[100]={0.271208,0.409694,0.351506,0.084101,0.00554422,0.000182146,4.60543e-06,3.86109e-10};
       static QSplineFit *fluxspline=new QSplineFit("Nitrogen_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==5){
     if(version==3){
//---Based on 2016-10-17(Q.Yan NScale-Boron_1075204MC Flux) 
       int nnode=8;
       double node[100]={1.5,2.3,3,7,21,70,250,10000};
       double ynode[100]={0.343727,0.460611,0.397935,0.0919349,0.00521674,0.000139351,2.76397e-06,8.60983e-11}; 
       static QSplineFit *fluxspline=new QSplineFit("Boron_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
     else  {
//---Based on 2016-04-18(Q.Yan B1064 Boron Flux)
       int nnode=8;
       double node[100]={1.5,2.3, 3,  6,  15.,  45. , 200, 25000};
       double ynode[100]={0.295101,0.444212,0.401281,0.132292,0.0139459,0.000561586,5.67585e-06,1.48226e-12};
       static QSplineFit *fluxspline=new QSplineFit("Boron_Weight_Flux_V2",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
   }
   else if(z==4){
//---Based on 2016-11-29(Q.Yan NScale-Beryllium_1082201MC Flux) 
       int nnode=8;
       double node[100]={1.5,2.3,3,7,21,70,250,10000};
//       double ynode[100]={0.139046,0.146446,0.126585,0.0316913,0.00186194,5.21843e-05,1.02048e-06,3.51387e-11};
       double ynode[100]={0.131841,0.137721,0.119056,0.0298554,0.00176541,4.90342e-05,9.07094e-07,3.45262e-11};
       static QSplineFit *fluxspline=new QSplineFit("Beryllium_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
   }
   else if(z==3){
     if(version==3){
//---Based on 2016-11-03(Q.Yan B1081_201 Lithium Flux)
       const int nnode=8;
       double node[nnode]={1.5,2.3, 3,  7,  21.,  70. , 250, 10000};
       double ynode[nnode]={0.208777,0.273148,0.252814,0.0670551,0.00381259,9.89365e-05,1.90077e-06,9.79549e-11};
       static QSplineFit *fluxspline=new QSplineFit("Lithium_Weight_Flux_V3",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
     else {
//---Based on 2015-April-21(Q.Yan Lithium Flux)
       const int nnode=8;
       double node[nnode]={1.5,2.3, 3,  7,  21.,  70. , 250, 10000};
       double ynode[nnode]={-0.276705,0.290343,0.266302,0.072232,0.00416156,0.00010642,1.99899e-06,1.13991e-10};
       static QSplineFit *fluxspline=new QSplineFit("Lithium_Weight_Flux_V1",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
     }
   }
   else if(z==2){
//---Based on 2014-Oct-2th(Q.Yan Helium Flux)
    if(version==1){
      const int nnode=7;
      double node[nnode]={1.1,3,10,30,100,300,50000};//50TV Constant
      double ynode[nnode]={79.1341,43.487,4.27222,0.265602,0.00994155,0.000489502,-9.59339e-10};
      static QSplineFit *fluxspline=new QSplineFit("Helim_Weight_Flux_V1",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
      cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
    }
//---Based on 2015-01-04(Q.Yan Helium Flux) No Cross-section correction(B916MC)
    else if(version==2){
      const int nnode=10;
      double node[nnode]={1.1, 1.7, 2.5, 5.0, 10., 23., 60,  167,  500.,25000};
      double ynode[nnode]={43.7675,58.6475,50.9901,18.9301,4.2117,0.518046,0.0389627,0.00235207,0.00012179,-5.12884e-09};
      static QSplineFit *fluxspline=new QSplineFit("Helim_Weight_Flux_V2",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
      cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
    }
//---Based on 2016-10-17(Q.Yan NScale-Helium_1081201MC Flux)
    else if(version==3){
      const int nnode=11;
      double node[nnode]={1.4,2,2.5,3,5,10,23,60,167,500,25000};
      double ynode[nnode]={55.1865,58.5452,51.3088,43.366,19.4744,4.36272,0.534778,0.0408081,0.00250437,0.000131008,5.82101e-09};
      static QSplineFit *fluxspline=new QSplineFit("Helim_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
      cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
    }
//---Based on ICRC
    else {
      static TF1 *cosfluxfunv0=new TF1("Helim_Weight_Flux_V0",cosflux,0,10000,7);
      cosfluxfunv0->FixParameter(0,2);//Charge
      cosfluxfunv0->FixParameter(1,4);//Mass
      cosfluxfunv0->FixParameter(2,0.8618);//soloar modulation
      cosfluxfunv0->FixParameter(3,1063);
      cosfluxfunv0->FixParameter(4,-2.818);
      cosfluxfunv0->FixParameter(5,0.01473);
      cosfluxfunv0->FixParameter(6,0.);
      cosfluxfunvn=cosfluxfunv0;
    }
//--
  }
  else {
//---Based on 2016-11-17(Q.Yan NScale-Proton_1082201MC Flux)
   if(version==3){
      const int nnode=12;
      double node[nnode]={0.8,1.3, 1.7, 2.3, 3.0, 5.0, 10., 23., 60,  167,  500.,12000};
      double ynode[nnode]={543.423,648.131,563.191,423.813,296.893,117.911,23.9023,2.68681,0.185803,0.0105442,0.000504267,1.55897e-07};
      static QSplineFit *fluxspline=new QSplineFit("Pr_Weight_Flux_V3N",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
      cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
    }
//---Based on 2014-Dec-14th(Q.Yan Proton Flux)
    else {
      const int nnode=10;
      double node[nnode]={0.8, 1.3, 1.7, 2.5, 5.0, 10., 30., 100.,300, 50000};//50TV Constant
      double ynode[nnode]={507.912,651.983,569.717,380.721,118.935,24.028,1.29263,0.0433961,0.00205652,1.52422e-09};
      static QSplineFit *fluxspline=new QSplineFit("Pr_Weight_Flux_V1",nnode,node,ynode,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
      cosfluxfunvn=fluxspline->GetFun(0.1,100000);//0.1-100TV
    }
  }
  double weightv=cosfluxfunvn->Eval(MCGenRig)/pow(MCGenRig,MCindex);
  double scale=1./(cosfluxfunvn->Eval(10.)/pow(10,MCindex));//normalized to 10GV
  if(MCindex==0)scale=1;
//---2nd electron+positron
  double scale2=1;
  if(abs(z)==1&&abs(mparid)==3){//electron(rawcouts)
     if(version==3){
       const int nnode2=7;
       double node2[nnode2]={1, 2, 4, 8, 16, 32, 64};
//       double ynode2[nnode2]={3.11223,3.80656,7.83464,9.18827,9.27815,6.1048,4.31283};
       double ynode2[nnode2]={2.96918,3.58134,7.57939,9.05505,9.15877,6.14735,4.31749};
       static QSplineFit *fluxspline2=new QSplineFit("El_Weight_Flux_V3",nnode2,node2,ynode2,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       TF1 *cosfluxfunvn2=fluxspline2->GetFun(0.1,100000);//0.1-100TV
       scale2=cosfluxfunvn2->Eval(MCGenRig);
     }
     else {
       const int nnode2=6;
       double node2[nnode2]={1, 2, 4, 8, 16, 32};
       double ynode2[nnode2]={3.58909,4.42799,8.68119,9.95605,11.314,8.59341};
       static QSplineFit *fluxspline2=new QSplineFit("El_Weight_Flux_V1",nnode2,node2,ynode2,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       TF1 *cosfluxfunvn2=fluxspline2->GetFun(0.1,100000);//0.1-100TV
//     scale2=cosfluxfunvn2->Eval(MCGenRig)/cosfluxfunvn2->Eval(10.);
       scale2=cosfluxfunvn2->Eval(MCGenRig);
     }
     scale*=scale2;
  }
  else if(abs(z)==1&&abs(mparid)==2){//positron(rawcouts)
     if(version==3){
       const int nnode2=7;
       double node2[nnode2]={1, 2, 4, 8, 16, 32, 64};
//       double ynode2[nnode2]={4.38651,1.22118,0.691724,0.540337,0.580738,0.504114,0.476182};
       double ynode2[nnode2]={4.8278,1.26118,0.696469,0.533803,0.576928,0.507748,0.472652};
       static QSplineFit *fluxspline2=new QSplineFit("Pos_Weight_Flux_V3",nnode2,node2,ynode2,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       TF1 *cosfluxfunvn2=fluxspline2->GetFun(0.1,100000);//0.1-100TV
       scale2=cosfluxfunvn2->Eval(MCGenRig);
     }
     else { 
       const int nnode2=6;
       double node2[nnode2]={1, 2, 4, 8, 16, 32};
       double ynode2[nnode2]={4.73951,1.27291,0.649025,0.455644,0.563151,0.539809};
       static QSplineFit *fluxspline2=new QSplineFit("Pos_Weight_Flux_V1",nnode2,node2,ynode2,(QSplineFit::LogX|QSplineFit::LogY|QSplineFit::ExtrapolateFlux),"b2e2");
       TF1 *cosfluxfunvn2=fluxspline2->GetFun(0.1,100000);//0.1-100TV
//     scale2=cosfluxfunvn2->Eval(MCGenRig)/cosfluxfunvn2->Eval(10.);
       scale2=cosfluxfunvn2->Eval(MCGenRig);
     }
     scale*=scale2;
  }
//---
  return weightv*scale*Rescale;
}

///--Cosmic Exposure Time
double GetISSExpoW(double MCGenRig){
  
  double urig=MCGenRig;
  if     (urig<1) urig=1;
  else if(urig>40)urig=40;
  static TSpline3 *htexp3=0; 
  if(htexp3==0){
     TFile *f=TFile::Open("/afs/cern.ch/work/q/qyan/work/analysis/PreSelect/bacground2/amshe_B620V1_time.root");
     TH1D *htexp=(TH1D *)f->Get("ExpoTime");
     htexp3=new TSpline3(htexp); 
  }
  return htexp3->Eval(urig)/htexp3->Eval(40);
}

///--Cosmic Total(Flux*Expo) Weight Function(bin by bin NoExpotime)
double GetISSTotW(double MCGenRig,double MCindex=-1,double Rescale=1.,bool isbinbybin=0,int z=2,int version=1,int mparid=0){//Default is helium
  if(isbinbybin==1)return GetISSFluxW(MCGenRig,MCindex,Rescale,version,z,mparid);//Bin by Bin reweight No-ExpoTime
  return GetISSFluxW(MCGenRig,MCindex,Rescale,version,z,mparid)*GetISSExpoW(MCGenRig); 
}


///--Cosmic Flux Normalized to MC Events Number Function
TH1* NormalizedISSFlux(TH1 *h,double MCindex=-1,double Rescale=1.,int z=2,int version=1){
    TH1 *hn=(TH1 *)h->Clone();
    int ndim=h->GetDimension();//1d or 2d
    for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
     double xv=hn->GetXaxis()->GetBinCenterLog(ibx);
     if(ndim==1){
       double yv=hn->GetBinContent(ibx)/GetISSFluxW(xv,MCindex,1,version,z)*Rescale;
       hn->SetBinContent(ibx,yv);
     }
     else {
       for(int iby=0;iby<=hn->GetNbinsY()+1;iby++){
         double yv=hn->GetBinContent(ibx,iby)/GetISSFluxW(xv,MCindex,1,version,z)*Rescale;
         hn->SetBinContent(ibx,iby,yv);
       } 
     }
  }
  return hn;
}

///--Cosmic Total(Flux*Expo) Normalized to MC Events Number Function(binbybin-noexpotime)
TH1* NormalizedISSTot(TH1 *h,double MCindex=-1,double Rescale=1.,bool isbinbybin=0,int z=2,int version=1,int mparid=0){
  TH1 *hn=(TH1 *)h->Clone();
  int ndim=h->GetDimension(); 
  for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
     double xv=hn->GetXaxis()->GetBinCenterLog(ibx);
     if(ndim==1){
        double yv=hn->GetBinContent(ibx)/GetISSTotW(xv,MCindex,1,isbinbybin,z,version,mparid)*Rescale;
        hn->SetBinContent(ibx,yv);
     }
     else {
       for(int iby=0;iby<=hn->GetNbinsY()+1;iby++){
         double yv=hn->GetBinContent(ibx,iby)/GetISSTotW(xv,MCindex,1,isbinbybin,z,version,mparid)*Rescale;
         hn->SetBinContent(ibx,iby,yv);
       }
    }
  } 
  return hn;
}

///--Add Two Range Histogram
TH1 *MergeMCHistW(TH1 *h1,TH1 *h2,double scale21,double rancut,int overlap=1){//MCGenEvent n2/n1(perbin)
   TH1 *h12=(TH1 *)h2->Clone();
   int ndim=h2->GetDimension();
   int bincut=h2->GetXaxis()->FindBin(rancut);
   for(int ibx=1;ibx<=h2->GetNbinsX();ibx++){
//--Overlap
    if(overlap==1){//In case Overlap
     if(ibx>=bincut)continue;//Event=RecEvent
     else{
        if(ndim==1){//1d 
          double yv=h1->GetBinContent(ibx)*scale21;
          h12->SetBinContent(ibx,yv);
        }
        else {//2d
          for(int iby=0;iby<=h1->GetNbinsY()+1;iby++){//h1 value
            double yv=h1->GetBinContent(ibx,iby)*scale21;
            h12->SetBinContent(ibx,iby,yv);
          }
        }
      }
    }
//---
    else {//No Overlap
       if(ndim==1){
         double yv=h2->GetBinContent(ibx)+h1->GetBinContent(ibx)*scale21;
         h12->SetBinContent(ibx,yv);
       }
       else {
           for(int iby=0;iby<=h1->GetNbinsY()+1;iby++){//h1 value
            double yv=h2->GetBinContent(ibx,iby)+h1->GetBinContent(ibx,iby)*scale21;
            h12->SetBinContent(ibx,iby,yv);
          }
       }
    }
//---
  }
  return h12;
}

///--Normlized+Merge Tot
TH1 *MergeNormalizedISSTot(TH1 *h1,TH1 *h2,double scale21,double rancut,int overlap=1,int opt=11,bool isbinbybin=0,int z=2,int version=1,int mparid=0){//hn1-normalied,hn2-normalized
   TH1 *hn1=0,*hn2=0;
   if(opt/10%10!=0)hn1=NormalizedISSTot(h1,-1,1.,isbinbybin,z,version,mparid);//
   else            hn1=(TH1 *)h1->Clone();
   if(opt%10!=0)   hn2=NormalizedISSTot(h2,-1,1.,isbinbybin,z,version,mparid);
   else            hn2=(TH1 *)h2->Clone();
   TH1 *hn12=MergeMCHistW(hn1,hn2,scale21,rancut,overlap);
   if(hn1)delete hn1;
   if(hn2)delete hn2;
   return hn12;
}

double getbincutoff(double cutoffv,TH1 *hev,double margin=1.2){
  static double prcutoffv=0;
  static TH1 *prhev=0;
  static double prmargin=0;
  static double prbincutoff=0;
  if(cutoffv==prcutoffv&&hev==prhev&&prmargin==margin){
    return prbincutoff;
  }
  double cutoffvr=cutoffv*margin;
  int ibrc=hev->FindBin(cutoffvr);
  double bincutoff=hev->GetBinLowEdge(ibrc+1);//LowEdge;
  {
    prcutoffv=cutoffv;
    prhev=hev;
    prmargin=margin;
    prbincutoff=bincutoff;
//    cout<<"cutoffv="<<cutoffv<<" "<<bincutoff<<endl;
  }
  return bincutoff;
}

double GetISSZenithUDBeamW(float l1x,float l1y,float costheta,float phi,float beta=1,int utod=1,bool isreal=1){//utod=1 beta>0 normalized to beta<0
    double ww=1;
//--MC->ISS
   if(!isreal){
//--X
    const int nnodexmc=7;
    double nodexmc[nnodexmc]={-65,-40,-20,0,20,40,65};
    double ynodexmc[nnodexmc]={1.04913,1.00137,1.08008,1.08402,0.943963,0.891571,0.927053};
    static QSplineFit *splinexmc=new QSplineFit("HeMCISSBetaProb_L1XRatio",nnodexmc,nodexmc,ynodexmc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwxmc=splinexmc->GetFun()->Eval(l1x);
//--Tune2
    const int nnodexmc1=7;
    double nodexmc1[nnodexmc1]={-65,-40,-20,0,20,40,65};
    double ynodexmc1[nnodexmc1]={0.865355,1.02458,1.02075,0.982643,0.999186,0.999809,0.952723};
    static QSplineFit *splinexmc1=new QSplineFit("HeMCISSBetaProb_L1XRatio1",nnodexmc1,nodexmc1,ynodexmc1,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwxmc1=splinexmc1->GetFun()->Eval(l1x);
    wwxmc=wwxmc*wwxmc1;
//--Y
    const int nnodeymc=7;
    double nodeymc[nnodeymc]={-50,-32,-16,0,16,32,50};
    double ynodeymc[nnodeymc]={1.36136,1.29614,1.15253,1.03297,0.909887,0.776186,0.753269};
    static QSplineFit *splineymc=new QSplineFit("HeMCISSBetaProb_L1YRatio",nnodeymc,nodeymc,ynodeymc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwymc=splineymc->GetFun()->Eval(l1y);
//--Tune2
    const int nnodeymc1=7;
    double nodeymc1[nnodeymc1]={-50,-32,-16,0,16,32,50};
    double ynodeymc1[nnodeymc1]={0.898508,0.901258,0.906813,0.942524,1.01543,1.12088,1.17258};
    static QSplineFit *splineymc1=new QSplineFit("HeMCISSBetaProb_L1YRatio1",nnodeymc1,nodeymc1,ynodeymc1,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwymc1=splineymc1->GetFun()->Eval(l1y);
    wwymc=wwymc*wwymc1;
//--Theta
    const int nnodethmc=7;
    double nodethmc[nnodethmc]={0.85,0.9,0.95,0.98,0.99,0.995,1};
    double ynodethmc[nnodethmc]={1.13804,0.86719,0.991483,1.16678,1.09889,0.899347,0.668488};
    static QSplineFit *splinethmc=new QSplineFit("HeMCISSProb_CosThetaRatio",nnodethmc,nodethmc,ynodethmc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwthmc=splinethmc->GetFun()->Eval(costheta);
//---Phi
    const int nnodephmc=9;
    double nodephmc[nnodephmc]={-3.1415926,-3.15/4*3.,-3.15/4*2,-3.15/4, 0, 3.15/4, 3.15/4*2, 3.15/4*3.,3.1415926};
    double ynodephmc[nnodephmc]={1.2193,1.17526,0.967101,1.30032,0.897006,1.11407,0.704187,0.861382,1.2193};
    static QSplineFit *splinephmc=new QSplineFit("HeMCISSBetaProb_PhiRatio",nnodephmc,nodephmc,ynodephmc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwphmc=splinephmc->GetFun()->Eval(phi);
//---
    ww=1./wwxmc/wwymc/wwthmc/wwphmc;
//---Theta beta>0 to beta<0
    const int nnodethu=7;
    double nodethu[nnodethu]={0.85,0.9,0.95,0.98,0.99,0.995,1};
    double ynodethu[nnodethu]={0.648689,0.80859,0.846822,1.0434,1.25217,1.40094,1.5816};
    static QSplineFit *splinethu=new QSplineFit("HePNMCBetaProb_CosThetaRatio",nnodethu,nodethu,ynodethu,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwthu=splinethu->GetFun()->Eval(costheta);
//----X beta>0 to beta<0
    const int nnodexu=7;
    double nodexu[nnodexu]={-65,-40,-20,0,20,40,65};
    double ynodexu[nnodexu]={1.11574,1.02035,0.96594,0.934719,0.977725,1.03223,1.12945};
    static QSplineFit *splinexu=new QSplineFit("HePNMCBetaProb_L1XRatio",nnodexu,nodexu,ynodexu,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwxu=splinexu->GetFun()->Eval(l1x);
//---Y beta>0 to beta<0
    const int nnodeyu=7;
    double nodeyu[nnodeyu]={-50,-32,-16,0,16,32,50};
    double ynodeyu[nnodeyu]={1.13925,1.02419,0.98062,0.95307,0.979007,1.02255,1.06144};
    static QSplineFit *splineyu=new QSplineFit("HePNMCBetaProb_L1YRatio",nnodeyu,nodeyu,ynodeyu,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwyu=splineyu->GetFun()->Eval(l1y);
 
//------same beam
     if(beta<0)ww=ww;
     else      ww=ww/wwthu/wwxu/wwyu;
//    cout<<"wwxmc="<<wwxmc<<" wwymc="<<wwymc<<" wwthmc="<<wwthmc<<" wwphmc="<<wwphmc<<endl;
//    return ww;
   }
//--L1X
     const int nnodex=7;
     double nodex[nnodex]={-65,-40,-20,0,20,40,65};
     double ynodex[nnodex]={1.08674,0.987289,1.06559,1.04467,0.958889,0.909509,0.932388};//Pos/Neg
     static QSplineFit *splinex=new QSplineFit("HePNBetaProb_L1XRatio",nnodex,nodex,ynodex,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwx=splinex->GetFun()->Eval(l1x);
//---Tune2
     double ynodex1[nnodex]={0.904932,1.05001,0.991304,0.942085,0.96634,1.02731,1.02668};//Pos/Neg
     static QSplineFit *splinex1=new QSplineFit("HePNBetaProb_L1XRatio1",nnodex,nodex,ynodex1,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwx1=splinex1->GetFun()->Eval(l1x);
     wwx=wwx*wwx1;
//--L1Y
     const int nnodey=7;
     double nodey[nnodey]={-65,-40,-20,0,20,40,65};
     double ynodey[nnodey]={1.79401,1.51895,1.28525,1.03524,0.797853,0.620586,0.483588};//Pos/Neg
     static QSplineFit *spliney=new QSplineFit("HePNBetaProb_L1YRatio",nnodey,nodey,ynodey,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwy=spliney->GetFun()->Eval(l1y);
//---Tune2
     double ynodey1[nnodey]={0.990782,0.941673,0.855645,0.882306,1.01073,1.29767,1.47628};
     static QSplineFit *spliney1=new QSplineFit("HePNBetaProb_L1YRatio1",nnodey,nodey,ynodey1,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwy1=spliney1->GetFun()->Eval(l1y);
     wwy=wwy*wwy1;
//--Cos(Theta)
    const int nnodeth=6;
    double nodeth[nnodeth]={0.85,0.9,0.95,0.98,0.99,1};
    double ynodeth[nnodeth]={0.744543,0.626731,0.823356,1.26618,1.44657,1.18051};
    static QSplineFit *splineth=new QSplineFit("HePNBetaProb_CosThetaRatio",nnodeth,nodeth,ynodeth,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwth=splineth->GetFun()->Eval(costheta);
//--Phi
    const int nnodeph=9;
    double nodeph[nnodeph]={-3.1415926,-3.15/4*3.,-3.15/4*2,-3.15/4, 0, 3.15/4, 3.15/4*2, 3.15/4*3.,3.1415926};
    double ynodeph[nnodeph]={1.3525,1.20032,0.987084,1.33272,0.912756,1.03113,0.646514,0.840908,1.3525};
    static QSplineFit *splineph=new QSplineFit("HePNBetaProb_PhiRatio",nnodeph,nodeph,ynodeph,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
    double wwph=splineph->GetFun()->Eval(phi);
//---
    if(!isreal){//MC
      if(utod!=1){// utod=0 beta<0 normalized to beta>0
        ww=ww*wwx*wwy*wwth*wwph;
      }
    }
    else {//ISS
     if(utod==1){//utod=1 beta>0 normalized to beta<0
       if(beta<0)ww=ww;
       else      ww=ww/(wwx*wwy*wwth*wwph);
//       cout<<"utod"<<" beta="<<beta<<" x="<<l1x<<" wwx="<<wwx<<" y="<<l1y<<" wwy="<<wwy<<endl;;
//       cout<<" costheta="<<costheta<<" wwth="<<wwth<<" phi="<<phi<<" wwph="<<wwph<<endl;
     }
     else {//utod=0 beta<0 normalized to beta>0
       if(beta>0)ww=ww;
       else      ww=ww*wwx*wwy*wwth*wwph; 
//       cout<<"dtou"<<" beta="<<beta<<" x="<<l1x<<" wwx="<<wwx<<" y="<<l1y<<" wwy="<<wwy<<" costheta="<<costheta<<" phi="<<wwph<<endl;
     }
    }
    return ww;
}

//--
double GetISSZenithUDBeamWF(float l9x,float l9y,float costheta,float phi,float beta=1,int utod=1,bool isreal=1,float l1x=0,float l1y=0){
    double ww=1;
    if(!isreal){
//--L9XMC beta>0 MC->ISS
      const int nnodexmc=5;//Pos
      double nodexmc[nnodexmc]={-45,-20,0,20,45};
      double ynodexmc[nnodexmc]={0.994592,1.01998,1.01037,0.987359,0.959869};
      static QSplineFit *splinexmc=new QSplineFit("HeMCISSBetaProb_L9XRatioF",nnodexmc,nodexmc,ynodexmc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwxmc=splinexmc->GetFun()->Eval(l9x);
///------
      const int nnodexmc2=5;//Pos
      double nodexmc2[nnodexmc2]={-60,-30,0,30,60};
      double ynodexmc2[nnodexmc2]={0.950549,1.01387,0.971811,0.98263,1.01209};
      static QSplineFit *splinexmc2=new QSplineFit("HeMCISSBetaProb_L1XRatio2F",nnodexmc2,nodexmc2,ynodexmc2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwxmc2=splinexmc2->GetFun()->Eval(l1x);
      if(l1x!=0&&l1y!=0){
        wwxmc=wwxmc*wwxmc2;
      }
//--L9YMC
      const int nnodeymc=5;
      double nodeymc[nnodeymc]={-30,-15,0,15,30};
      double ynodeymc[nnodeymc]={1.05085,1.02368,1.00113,0.967653,0.967196};//MC/ISS
      static QSplineFit *splineymc=new QSplineFit("HeMCISSBetaProb_L9YRatioF",nnodeymc,nodeymc,ynodeymc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwymc=splineymc->GetFun()->Eval(l9y);
//---
      const int nnodeymc2=5;
      double nodeymc2[nnodeymc2]={-60,-25,0,25,60};
      double ynodeymc2[nnodeymc2]={0.912561,0.913183,1.01234,1.02168,1.09089};//MC/ISS
      static QSplineFit *splineymc2=new QSplineFit("HeMCISSBetaProb_L1YRatio2F",nnodeymc2,nodeymc2,ynodeymc2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwymc2=splineymc2->GetFun()->Eval(l1y);
      if(l1x!=0&&l1y!=0){
        wwymc=wwymc*wwymc2;
      }
//---Cos(Theta) MC
     const int nnodethmc=5;
     double nodethmc[nnodethmc]={0.93,0.98,0.99,0.995,1};
     double ynodethmc[nnodethmc]={1.04981,0.999843,1.00156,0.997714,0.9818};
     static QSplineFit *splinethmc=new QSplineFit("HeMCISSBetaProb_CosThetaRatioF",nnodethmc,nodethmc,ynodethmc,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwthmc=splinethmc->GetFun()->Eval(costheta);
//---
      double wwphmc=1;
//---
      ww=1./wwxmc/wwymc/wwthmc/wwphmc;
//---L9XMC beta<0 to beta>0
      const int nnodexu=11;//Pos
      double nodexu[nnodexu]={-45,-38,-33,-30,-20,0,20,30,33,38,45};
      double ynodexu[nnodexu]={0.376854,0.539903,0.704282,1.07838,1.39968,1.35133,1.37644,1.08249,0.736067,0.545738,0.390077};
      static QSplineFit *splinexu=new QSplineFit("HeMCNPBetaProb_L9XRatioF",nnodexu,nodexu,ynodexu,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwxu=splinexu->GetFun()->Eval(l9x);
//---L1IL9 New
     const int nnodexu2=7;
     double nodexu2[nnodexu2]={-65,-40,-20,0,20,40,65};
     double ynodexu2[nnodexu2]={1.076,1.01019,1.0067,0.955098,0.978141,0.999973,1.01992};
     static QSplineFit *splinexu2=new QSplineFit("HeMCPNBetaProb_L1XRatio2F",nnodexu2,nodexu2,ynodexu2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwxu2=splinexu2->GetFun()->Eval(l1x);
     if(l1x!=0&&l1y!=0){
       wwxu=wwxu*wwxu2;
     }
//---L9YMC beta<0 to beta>0
      const int nnodeyu=5;
      double nodeyu[nnodeyu]={-30,-15,0,15,30};
      double ynodeyu[nnodeyu]={1.01077,1.00547,1.00039,0.980533,1.00299};//Pos/Neg
      static QSplineFit *splineyu=new QSplineFit("HeMCNPBetaProb_L9YRatioF",nnodeyu,nodeyu,ynodeyu,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwyu=splineyu->GetFun()->Eval(l9y);
//---L1IL9 New
     const int nnodeyu2=7;
     double nodeyu2[nnodeyu2]={-50,-32,-16,0,16,32,50};
     double ynodeyu2[nnodeyu2]={1.19038,0.961947,0.996513,1.00504,0.98463,0.971969,1.00301};
     static QSplineFit *splineyu2=new QSplineFit("HeMCPNBetaProb_L1YRatio2F",nnodeyu2,nodeyu2,ynodeyu2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwyu2=splineyu2->GetFun()->Eval(l1y);
//---
     const int nnodeyu3=7;
     double nodeyu3[nnodeyu3]={-50,-32,-16,0,16,32,50};
     double ynodeyu3[nnodeyu3]={0.94666,0.952623,0.975773,0.998963,1.01129,1.02222,1.03204};
     static QSplineFit *splineyu3=new QSplineFit("HeMCPNBetaProb_L1YRatio3F",nnodeyu3,nodeyu3,ynodeyu3,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwyu3=splineyu3->GetFun()->Eval(l1y);
     if(l1x!=0&&l1y!=0){
       wwyu=wwyu*wwyu2*wwyu3;
     }
//---Cos(Theta) MC beta<0 to beta>0
      const int nnodethu=5;
      double nodethu[nnodethu]={0.93,0.98,0.99,0.995,1};
      double ynodethu[nnodethu]={0.884556,0.984217,1.01992,1.02615,1.02718};
      static QSplineFit *splinethu=new QSplineFit("HeMCNPBetaProb_CosThetaRatioF",nnodethu,nodethu,ynodethu,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
      double wwthu=splinethu->GetFun()->Eval(costheta);
//---Phi(MC) beta<0 to beta>0
     double wwphu=1;
     const int nnodephu2=5;
     double nodephu2[nnodephu2]={-3.1415926,-3.15/4*2, 0, 3.15/4*2,3.1415926};
     double ynodephu2[nnodephu2]={0.961358,1.08937,0.980127,0.97249,0.961358};
     static QSplineFit *splinephu2=new QSplineFit("HeMCNPBetaProb_PhiRatio2F",nnodephu2,nodephu2,ynodephu2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwphu2=splinephu2->GetFun()->Eval(phi);
     if(l1x!=0&&l1y!=0){
       wwphu=wwphu*wwphu2;
     }
//--
      if(beta>0)ww=ww;
      else      ww=ww*wwxu*wwyu*wwthu*wwphu;
    }
//--L9X
     const int nnodex=11;
     double nodex[nnodex]={-45,-38,-33,-30,-20,0,20,30,33,38,45};
     double ynodex[nnodex]={0.402093,0.738063,0.853,1.00484,1.1653,1.12651,1.12784,0.964243,0.889486,0.728395,0.529876};
     static QSplineFit *splinex=new QSplineFit("HePNBetaProb_L9XRatioF",nnodex,nodex,ynodex,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwx=splinex->GetFun()->Eval(l9x);
//---L1IL9 New
     const int nnodex2=7;
     double nodex2[nnodex2]={-65,-40,-20,0,20,40,65};
     double ynodex2[nnodex2]={1.01066,0.925545,1.04937,1.04885,1.05958,0.863295,0.874216};
     static QSplineFit *splinex2=new QSplineFit("HePNBetaProb_L1XRatio2F",nnodex2,nodex2,ynodex2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwx2=splinex2->GetFun()->Eval(l1x);
     if(l1x!=0&&l1y!=0){
       wwx=wwx*wwx2;
     }
//--L9Y
     const int nnodey=5;
     double nodey[nnodey]={-30,-15,0,15,30};
     double ynodey[nnodey]={0.790043,0.810049,1.13699,1.09757,1.19606};//Pos/Neg
     static QSplineFit *spliney=new QSplineFit("HePNBetaProb_L9YRatioF",nnodey,nodey,ynodey,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwy=spliney->GetFun()->Eval(l9y);
//--Tune2
     const int nnodey1=5;
     double nodey1[nnodey1]={-30,-15,0,15,30};
     double ynodey1[nnodey1]={1.05023,1.03252,0.9919,0.946046,0.906851};
     static QSplineFit *spliney1=new QSplineFit("HePNBetaProb_L9YRatio1F",nnodey1,nodey1,ynodey1,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwy1=spliney1->GetFun()->Eval(l9y);
     wwy=wwy*wwy1;
//---L1IL9 New
     const int nnodey2=7;
     double nodey2[nnodey2]={-50,-32,-16,0,16,32,50};
     double ynodey2[nnodey2]={1.28833,1.18634,1.05155,0.985501,0.946471,0.862791,0.747323};
     static QSplineFit *spliney2=new QSplineFit("HePNBetaProb_L1YRatio2F",nnodey2,nodey2,ynodey2,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwy2=spliney2->GetFun()->Eval(l1y);
     if(l1x!=0&&l1y!=0){
       wwy=wwy*wwy2;
     }
//--Cos(Theta)
     const int nnodeth=5;
     double nodeth[nnodeth]={0.93,0.98,0.99,0.995,1};
     double ynodeth[nnodeth]={0.911326,1.19348,1.10846,0.851061,0.765288};
     static QSplineFit *splineth=new QSplineFit("HePNBetaProb_CosThetaRatioF",nnodeth,nodeth,ynodeth,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwth=splineth->GetFun()->Eval(costheta);
//---Phi
     const int nnodeph=9;
     double nodeph[nnodeph]={-3.1415926,-3.15/4*3.,-3.15/4*2,-3.15/4, 0, 3.15/4, 3.15/4*2, 3.15/4*3.,3.1415926};
     double ynodeph[nnodeph]={1.2806,0.979419,0.897113,1.32148,1.01554,0.932929,0.708696,0.822542,1.2806};
     static QSplineFit *splineph=new QSplineFit("HePNBetaProb_PhiRatioF",nnodeph,nodeph,ynodeph,(QSplineFit::ExtrapolateLB|QSplineFit::ExtrapolateLE),"b1e1");
     double wwph=splineph->GetFun()->Eval(phi);
//---
    if(!isreal){//MC
      if(utod==1){// utod=1 beta>0 normalized to beta<0
        ww=ww/(wwx*wwy*wwth*wwph);
      }
    }
    else {
     if(utod==1){//utod=1 beta>0 normalized to beta<0
       if(beta<0)ww=ww;
       else      ww=ww/(wwx*wwy*wwth*wwph);
     }
     else {//utod=0 beta<0 normalized to beta>0
       if(beta>0)ww=ww;
       else      ww=ww*wwx*wwy*wwth*wwph;
     }
    }
    return ww;
}

double GetMCB1057WeightLog(float rgen,int ivers=0){
  static const int ne0=11;
  static double ue0[ne0][2]={{1,1.5},{1.5,2},{2,20},{20,30},{30,50},{50,75},{75,100},{100,160},{160,500},{500,1000},{1000,2000}};
  static double wue0[ne0]={0.0123079,0.00998799,0.00689497,0.0100331,0.0131322,0.0174762,0.0439956,0.0555391,0.103581,0.549175,1};
  static double wue1[ne0]={0.0116335,0.0095484,0.00658667,0.00959124,0.0123878,0.0161898,0.0368304,0.0446302,0.103564,0.549175,1};
  static const int ne2=13;
  static double ue2[ne2][2]={{1,1.5},{1.5,2},{2,15},{15,20},{20,30},{30,50},{50,75},{75,100},{100,150},{150,160},{160,500},{500,1000},{1000,2000}};
  static double wue2[ne2]={0.0116378,0.00957391,0.0066184,0.00501846,0.00656505,0.00777144,0.00911218,0.0132966,0.0142025,0.0427654,0.10318,0.549068,1};
  static const int ne3=14;
  static double ue3[ne3][2]={{1,1.5},{1.5,2},{2,10},{10,15},{15,20},{20,30},{30,50},{50,75},{75,100},{100,150},{150,160},{160,500},{500,1000},{1000,2000}};
  static double wue3[ne3]={0.0115735,0.00952143,0.00659893,0.00617221,0.0034801,0.00416327,0.00462933,0.0050744,0.00615194,0.00685039,0.0427868,0.103198,0.549119,1};
int ne=0;
  double (*ue)[2];
  double *wue=0;
  if     (ivers==0){wue=wue0;ne=ne0;ue=ue0;}
  else if(ivers==1){wue=wue1;ne=ne0;ue=ue0;}
  else if(ivers==2){wue=wue2;ne=ne2;ue=ue2;}
  else             {wue=wue3;ne=ne3;ue=ue3;}
  double ww=1;
  double scale=100; 
  for(int ie=0;ie<ne;ie++){
     if     (ie==0&&rgen<=ue[ie][0]){ww=wue[ie]*scale;break;}
     else if(ie==ne-1&&rgen>=ue[ie][1]){ww=wue[ie]*scale;break;}
     else if(rgen>=ue[ie][0]&&rgen<ue[ie][1]){ww=wue[ie]*scale;break;}
  }
  return ww;
}

double GetMCB1057RICHWeight(float rgen,int ivers=0){
//---
  static const int ne0=7;
  static double ue0[ne0][2]={{1,30},{30,50},{50,75},{75,100},{100,160},{160,500},{500,2000}};
  static double wue0[ne0]={1.08001,1.06204,1.04742,1.09588,1.0997,1.08826,1};
//---
  static const int ne2=10;
  static double ue2[ne2][2]={{1,15},{15,20},{20,30},{30,50},{50,75},{75,100},{100,150},{150,160},{160,500},{500,2000}};
  static double wue2[ne2]={1.09803,1.10696,1.10238,1.09375,1.08424,1.11239,1.11325,1.10714,1.09167,1};
//---
  static const int ne3=10;
  static double ue3[ne3][2]={{1,15},{15,20},{20,30},{30,50},{50,75},{75,100},{100,150},{150,160},{160,500},{500,2000}};
  static double wue3[ne3]={1.09977,1.11634,1.11615,1.11077,1.10537,1.11744,1.1161,1.10714,1.09167,1};
//---
  int ne=0;
  double (*ue)[2];
  double *wue=0;
  if      (ivers<=1){wue=wue0;ne=ne0;ue=ue0;}
  else  if(ivers<=2){wue=wue2;ne=ne2;ue=ue2;}
  else              {wue=wue3;ne=ne3;ue=ue3;}  
  double ww=1;
  double scale=1;
  for(int ie=0;ie<ne;ie++){
     if     (ie==0&&rgen<=ue[ie][0]){ww=wue[ie]*scale;break;}
     else if(ie==ne-1&&rgen>=ue[ie][1]){ww=wue[ie]*scale;break;}
     else if(rgen>=ue[ie][0]&&rgen<ue[ie][1]){ww=wue[ie]*scale;break;}
  }
  return ww;
}


double GetMCB1057FluxWeight2(float rgen,int ivers=0){
    double rig=rgen;
    if     (rig<1)rig=1;
    else if(rig>495)rig=495; 
    const int nnode=14;
    double node[nnode]={1.,1.2,1.5,2.3,4.0,7.0,10,15,20,30,50,90,200,495};
    double ww2=1;
    if(ivers/10==0){
      if(ivers==0){
        double ynode[nnode]={6.54149,6.39945,5.95301,4.19134,2.21422,1.43122,1.11321,0.815585,0.759602,0.789054,0.843274,0.98855,0.908145,0.77132};
        static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057Weight2_V0",nnode,node,ynode,(QSplineFit::LogX),"b1e1");
        ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
      }
      else {
        double ynode[nnode]={6.25547,6.14035,5.73002,4.02911,2.12852,1.37473,1.07115,0.782076,0.735347,0.750563,0.806601,0.858968,0.841606,0.761363};
        static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057Weight2_V1",nnode,node,ynode,(QSplineFit::LogX),"b1e1"); 
        ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
        if(ivers>=2){
          double ynode2[nnode]={0.924601,0.939283,0.955385,0.924985,0.882067,0.877054,0.878043,0.8635,0.872502,0.876013,0.872864,0.870178,0.864166,0.82635};
          static QSplineFit *fluxspline2=new QSplineFit("HeFluxB1057Weight2_V2",nnode,node,ynode2,(QSplineFit::LogX),"b1e1");
          ww2=ww2/fluxspline2->GetFun()->Eval(rig);//0.1-100TV
        }
        if(ivers>=3){
          double ynode3[nnode]={0.97827,0.983977,1.01097,1.00927,1.00598,1.00415,1.00558,1.00514,1.00526,0.999296,0.991303,0.972158,0.990161,0.983212};
          static QSplineFit *fluxspline3=new QSplineFit("HeFluxB1057Weight2_V3",nnode,node,ynode3,(QSplineFit::LogX),"b1e1");
          ww2=ww2/fluxspline3->GetFun()->Eval(rig);//0.1-100TV
        }
      }
    }
    else  {//ivers>=10
       double ynode[nnode]={5.46674,5.31367,5.00377,3.52233,1.88178,1.21291,0.94798,0.691751,0.648961,0.656583,0.694955,0.721911,0.719302,0.661179};
       static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057Weight2_V10",nnode,node,ynode,(QSplineFit::LogX),"b1e1");
       ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
       if(ivers%10>=1){
          double ynode1[nnode]={1.06842,1.07497,1.09491,1.06197,1.00394,0.997076,0.997331,0.98252,0.993159,0.999894,1.00401,1.00331,1.00295,0.962203};
          static QSplineFit *fluxspline1=new QSplineFit("HeFluxB1057Weight2_V11",nnode,node,ynode1,(QSplineFit::LogX),"b1e1");
          ww2=ww2/fluxspline1->GetFun()->Eval(rig);//0.1-100TV
       }
       if(ivers%10>=2){
          double ynode2[nnode]={0.977278,0.979689,0.999219,1.00395,0.996959,0.996986,1.00176,1.00705,1.00504,1.00467,1.00177,1.00088,1.0146,0.979149};
          static QSplineFit *fluxspline2=new QSplineFit("HeFluxB1057Weight2_V12",nnode,node,ynode2,(QSplineFit::LogX),"b1e1");
          ww2=ww2/fluxspline2->GetFun()->Eval(rig);//0.1-100TV
       }
    }
    return ww2;
}

double GetMCB1057FluxWeight2Sp(float rgen,int ivers=0){
    double rig=rgen;
    if     (rig<1)rig=1;
    else if(rig>495)rig=495;
    const int nnode=14;
    double node[nnode]={1.,1.2,1.5,2.3,4.0,7.0,10,15,20,30,50,90,200,495};
    double ww2=1;
    if(ivers==0){
       double ynode[nnode]={0.555524,0.543893,0.528926,0.382011,0.192901,0.115415,0.0872299,0.0621957,0.0577106,0.0583568,0.0628747,0.0732595,0.0808141,0.0630788};
       static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057Weight2Sp_V0",nnode,node,ynode,(QSplineFit::LogX),"b1e1");
       ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
//---2nd Correction
       double ynode2[nnode]={0.889253,0.871902,0.866773,0.83023,0.834937,0.891861,0.918517,0.922577,0.934342,0.93336,0.907052,0.809919,0.715679,0.613784};
       static QSplineFit *fluxspline2=new QSplineFit("HeFluxB1057Weight2Sp_V0C",nnode,node,ynode2,(QSplineFit::LogX),"b1e1");
       ww2=ww2/fluxspline2->GetFun()->Eval(rig);
    }
    return ww2;
}


double GetMCB1057RICHWeight2(float rgen,int ivers=0){
//----
  double rig=rgen;
  double ww2=1;
  if     (rig<10)rig=10;
  else if(rig>600)rig=600;
  const int nnode=8;
  double node[nnode]={10,14,20,30,60,120,256,600};
  if(ivers<=1){
    double ynode[nnode]={1.20238,1.18545,1.1967,1.19284,1.18339,1.19391,1.20305,1.18832};
    static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057RICHWeight2_V1",nnode,node,ynode,(QSplineFit::LogX),"b1e1");
    ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
  }
  else if(ivers<=2){
    double ynode[nnode]={1.14707,1.13835,1.14006,1.13917,1.13141,1.12481,1.12767,1.12699};
    static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057RICHWeight2_V2",nnode,node,ynode,(QSplineFit::LogX),"b1e1");
    ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
  }
  else {
    double ynode[nnode]={1.1469,1.13784,1.14082,1.13982,1.1323,1.12462,1.12781,1.12688};
    static QSplineFit *fluxspline=new QSplineFit("HeFluxB1057RICHWeight2_V3",nnode,node,ynode,(QSplineFit::LogX),"b1e1");
    ww2=1./fluxspline->GetFun()->Eval(rig);//0.1-100TV
  }
//-----
  return ww2;
}

///--Flux Fit with AMS ISS
int FluxFitISS(){

   gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);
   double scaleindex=2.7;

   float pmin=1.5,pmax=3000;
   const int bin=50;
   double mon[bin+1];
   float de=(log(pmax)-log(pmin))/bin;
   for(int ibin=0;ibin<=bin;ibin++){
      mon[ibin]=log(pmin)+de*ibin;
      mon[ibin]=exp(mon[ibin]);
      cout<<"mon="<<mon[ibin]<<endl;
   }

   float rmin=1e-6,rmax=1e3;
   double ran[bin+1];
   float dr=(log(rmax)-log(rmin))/bin;
   for(int ibin=0;ibin<=bin;ibin++){
      ran[ibin]=log(rmin)+dr*ibin;
      ran[ibin]=exp(ran[ibin]);
      cout<<"ran="<<ran[ibin]<<endl;
   }
  
///----
   TF1 *cosfluxfun=new TF1("cosfluxfun",cosflux,0,10000,7);
      
   const int nfl=1;
   const char *filen[nfl]={
     "/afs/cern.ch/work/q/qyan/work/analysis/PreSelect/bacground2/crflx.root",
   };

   const int nh=1;
   TGraphErrors *h[nh][nfl];
   for(int ifl=0;ifl<nfl;ifl++){
     TFile *f=new TFile(filen[ifl]);
     h[0][ifl]=(TGraphErrors *)f->Get("gr2");
   }

//---Helium Flux
   cosfluxfun->FixParameter(0,2);//Charge
   cosfluxfun->FixParameter(1,4);//Mass
   cosfluxfun->SetParameter(2,0.5);//soloar modulation
   cosfluxfun->SetParameter(4,-2.7);
   cosfluxfun->SetParameter(5,0);
//   cosfluxfun->FixParameter(5,0);
   cosfluxfun->SetParLimits(2,0,3); 
   cosfluxfun->FixParameter(6,0);
   TCanvas *c1=new TCanvas();
   gPad->SetLogx();
   gPad->SetLogy();
   TCanvas *c2=new TCanvas();
   gPad->SetLogx();
   gPad->SetLogy();
   TH2D *h2=new TH2D("Helium Flux","Helium Flux",bin,mon,bin,ran);

//---
   for(int ifl=0;ifl<nfl;ifl++){
     TGraphErrors* gr1=(TGraphErrors *)h[0][ifl]->Clone();
//---
     TGraphErrors* gr2=(TGraphErrors *)h[0][ifl]->Clone();
     double xv[1000],yv[1000],eyv[1000];
     int nx=gr1->GetN();
     for(int ip=0;ip<gr1->GetN();ip++){
         gr1->GetPoint(ip,xv[ip],yv[ip]);
         eyv[ip]=gr1->GetErrorY(ip);
         double rig;
         ConvertEU(2,rig,xv[ip],1);//Kinetic->Rigidity
         xv[ip]=rig;
//----gr1
         cout<<"cx="<<xv[ip]<<" yv="<<yv[ip]<<" eyv="<<eyv[ip]<<endl;
         gr1->SetPoint(ip,xv[ip],yv[ip]);
         gr1->SetPointError(ip,0,eyv[ip]);
//----gr2
         yv[ip]=yv[ip]*pow(xv[ip],scaleindex)/pow(xv[0],scaleindex);
         eyv[ip]=eyv[ip]*pow(xv[ip],scaleindex)/pow(xv[0],scaleindex);
         gr2->SetPoint(ip,xv[ip],yv[ip]);
         gr2->SetPointError(ip,0,eyv[ip]);
     }
     h[0][ifl]=gr1;
//---Draw
     c1->cd();
     gPad->SetLogx();
     gPad->SetLogy();
     TH2D *h1=new TH2D("Helium Flux","Helium Flux",bin,mon,bin,ran);
     h1->Draw();
     gr1->Draw("Psame");
     cosfluxfun->SetLineColor(gr1->GetLineColor());
     cosfluxfun->FixParameter(6,0);
     gr1->Fit(cosfluxfun,"","same",2,3000);
//---
     c2->cd();
     gPad->SetLogx();
     gPad->SetLogy();
     TH2D *h2=new TH2D(Form("Helium Flux*R^{%.2f} ICRC",scaleindex),Form("Helium Flux*R^{%.2f} ICRC",scaleindex),bin,mon,bin,ran);
     h2->GetXaxis()->SetTitle("Rigidity[GV]");
     h2->GetXaxis()->SetTitleColor(1);
     h2->GetYaxis()->SetTitle(Form("Flux*R^{%.2f} [m^2 sr sec GV]-1",scaleindex));
     h2->Draw();
     gr2->Draw("Psame");
     cosfluxfun->SetLineColor(gr2->GetLineColor());
     cosfluxfun->FixParameter(6,scaleindex); 
     gr2->Fit(cosfluxfun,"","same",2,3000); 
//---
   }
//--
   return 0;
}



///--Flux Fit with AMS ISS
int ISSTestW(){

   float pminc=0.2,pmaxc=10000;
   const int binc=20000;
   double monc[binc+1];
   double dec=(log(pmaxc)-log(pminc))/binc;
   for(int ibin=0;ibin<=binc;ibin++){
      monc[ibin]=log(pminc)+dec*ibin;
      monc[ibin]=exp(monc[ibin]);
      cout<<monc[ibin]<<" ";
   }

//---Flux
   TH1D *ht=new TH1D("WTime","WTime",binc,monc);
   TH1D *hfp=new TH1D("WProtonFlux","WProtonFlux",binc,monc);
   TH1D *hfhe=new TH1D("WHeFlux","WHeFlux",binc,monc);
   TH1D *hfli=new TH1D("WLiFlux","WLiFlux",binc,monc);
   TH1D *hfb=new TH1D("WBFlux","WBFlux",binc,monc);
   TH1D *hfc=new TH1D("WCFlux","WCFlux",binc,monc);
   TH1D *hfo=new TH1D("WOFlux","WOFlux",binc,monc);
   TH1D *hflic_r=new TH1D("WLiCFlux","WLiCFlux",binc,monc);
   TH1D *hfel=new TH1D("WElFlux","WElFlux",binc,monc);
   TH1D *hfpos=new TH1D("WPosFlux","WPosFlux",binc,monc);
   for(int ib=1;ib<=hfp->GetNbinsX();ib++){
      double xv=hfp->GetXaxis()->GetBinCenterLog(ib); 
      double tv=GetISSExpoW(xv); 
      ht->SetBinContent(ib,tv);
      double fv=GetISSFluxW(xv,0,1,1,1);//Proton
      hfp->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,2,2);//Helium
      hfhe->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,1,3);//Lithium
      hfli->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,1,5);//Boron
      hfb->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,2,6);//Carbon
      hfc->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,2,8);//Oxygen
      hfo->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,1,306);//Li/C
      hflic_r->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,1,1,3);//Electron
      hfel->SetBinContent(ib,fv);
      fv=GetISSFluxW(xv,0,1,1,1,2);//Positron
      hfpos->SetBinContent(ib,fv);
   }

//--Exposure Time
   TCanvas *canvas=new TCanvas();
   gPad->SetLogx();
   ht->Draw();
   
//--Flux
   canvas=new TCanvas();
   gPad->SetLogx();
   gPad->SetLogy();
   hfp->SetTitle("Cosmic Ray Flux");
   hfp->GetXaxis()->SetTitle("Rigidity[GV]");
   hfp->GetXaxis()->SetTitleColor(1);
   hfp->SetLineColor(1);//Proton
   hfp->Draw();
   hfhe->SetLineColor(2);//Helium New
   hfhe->Draw("same");
   hfli->SetLineColor(3);//Lithium New
   hfli->Draw("same");
   hfb->SetLineColor(8);//Boron New
   hfb->Draw("same");
   hfc->SetLineColor(6);//Carbon New
   hfc->Draw("same");
   hfo->SetLineColor(4);//Oxygen New
   hfo->Draw("same");
   hfel->SetLineColor(7);//Electron
   hfel->Draw("same");
   hfpos->SetLineColor(kOrange);//Positron
   hfpos->Draw("same");

   TLegend* legend = new TLegend(0.55,0.55,0.88,0.88,"","brNDC");
   legend->AddEntry(hfp,"Proton Flux","PL");
   legend->AddEntry(hfhe,"Helium Flux","PL");
   legend->AddEntry(hfli,"Lithium Flux","PL");
   legend->AddEntry(hfb,"Boron Flux","PL");
   legend->AddEntry(hfc,"Carbon Flux","PL");
   legend->AddEntry(hfo,"Oxygen Flux","PL");
   legend->AddEntry(hfel,"Electron Flux","PL");
   legend->AddEntry(hfpos,"Positron Flux","PL");
   legend->Draw("same");
    
//---Ratio
   canvas=new TCanvas();
   gPad->SetLogx();
   hflic_r->SetTitle("Cosmic Ray Flux Ratio");
   hflic_r->GetXaxis()->SetTitle("Rigidity[GV]");
   hflic_r->GetXaxis()->SetTitleColor(1);
   hflic_r->SetLineColor(1);//Proton
   hflic_r->Draw();
   
   legend = new TLegend(0.55,0.55,0.88,0.88,"","brNDC");
   legend->AddEntry(hfp,"Li/C","PL");
   legend->Draw("same");
   return 0;
}

///---Test
int ISSTestW2(){

   const int ne=2;
   double rigr[ne][2]={//lh
      0.5, 200.,
      10., 4000,
   };

   const int nfl=2;
   const char *filen[nfl]={
     "/afs/cern.ch/work/q/qyan/Data/amsd8he_heB800_1400_effpick2w/amsd8he_heB800_1400_effpick2w.root",
     "/afs/cern.ch/work/q/qyan/Data/amsd8he_heB800fast_208000_effpick2w/amsd8he_heB800fast_208000_effpick2w.root",
   };

   const int nh=2;
   const int nh2=1;
   TH1D *h[nh][nfl]={{0}};
   TH2D *h2[nh2][nfl]={{0}};
   for(int ifl=0;ifl<nfl;ifl++){
     TFile *f=new TFile(filen[ifl]);
     h[0][ifl]=(TH1D *)f->Get("mcev_rrawp");
     h[1][ifl]=(TH1D *)f->Get("all_nr2_mr2_q2");
     h2[0][ifl]=(TH2D *)f->Get("all_rr2_q2_2dq1");
   }

//---Get
    double ev[2];
    for(int ie=0;ie<2;ie++){
       ev[ie]=h[0][ie]->GetBinContent(1)/log(rigr[ie][1]/rigr[ie][0]);
    }
    double scale=ev[1]/ev[0]; 
///--Scaled
    TCanvas *c=new TCanvas();
    c->SetLogx();
    c->SetLogy();
    TH1D *hn0=(TH1D *)h[1][0]->Clone();
    hn0->Scale(scale); 
    hn0->Draw();
    h[1][1]->SetLineColor(2);
    h[1][1]->Draw("same");
//---
    c=new TCanvas();
    c->SetLogx();
    TH1D *hn=(TH1D *)h[1][1]->Clone();
    hn->Divide(hn0);
    hn->Draw();
    hn->GetXaxis()->SetRangeUser(rigr[1][0],rigr[0][1]);
//--
    c=new TCanvas();
    c->SetLogx();
    TH1D *hn1=(TH1D *)NormalizedISSTot(h[1][0]);
    TH1D *hn2=(TH1D *)NormalizedISSTot(h[1][1]);
    hn1->Draw();
    hn2->SetLineColor(2);
    hn2->Draw("same");
//    TH1D *hn12=(TH1D *)MergeMCHistW(hn1,hn2,scale,50);
    TH1D *hn12=(TH1D *)MergeNormalizedISSTot(h[1][0],h[1][1],scale,50);
    hn12->Draw("same");
//--
    c=new TCanvas();
    c->SetLogx();
    gPad->SetLogz();
    TH2D *hn2d=(TH2D *)MergeNormalizedISSTot(h2[0][0],h2[0][1],scale,50);
    hn2d->Draw("COLZ");
    TH1D *hn2dp=(TH1D *)hn2d->ProjectionX();
    c=new TCanvas();
    c->SetLogx();
    c->SetLogy();
    hn2dp->Draw(); 
   return 0;
}
#endif
