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
 
Double_t tkqcorfun(Double_t *x,Double_t *par){  //Sigta
 double xv=log(x[0]);
 double eff;
 if(xv>par[4])eff=par[0];
 else        eff=par[0]+par[1]*(xv-par[4])+par[2]*pow(xv-par[4],2)+par[3]*pow(xv-par[4],3);
 return eff;
}


int mctkcor(float &l1q, float &l9q, float rigidity){//TkL1 L9 Charge

  Double_t rig=fabs(rigidity)>2?fabs(rigidity):2; 

//---L9 Charge Rigidity Correction
  Double_t l9paris[5]={2.04737,0.0322342,0.026253,0.00589927,3.58912};
  Double_t l9parmc[5]={2.14838,0.0444803,0.0674273,0.0240547,3.23947};
  double l9coris=tkqcorfun(&rig,l9paris);
  double l9cormc=tkqcorfun(&rig,l9parmc);  
  l9q=l9q/l9cormc*l9coris;

//---L1 Charge Rigidity Correction
  Double_t l1paris[5]={2.03645,-0.00985179,-0.013428,-0.0035763,3.11976};
  Double_t l1parmc[5]={2.21272,-0.0385303, -0.0199547,0.0031957,3.05412};
  double l1coris=tkqcorfun(&rig,l1paris);
  double l1cormc=tkqcorfun(&rig,l1parmc);
  l1q=l1q/l1cormc*l1coris;
  
//--
  return 0; 
}

///---ECAL fun
Double_t ecalcorfun(Double_t *x,Double_t *par){  //Sigta
 double xv=log(x[0]);
 double corv=par[0]*pow(xv,par[1])+par[2];
 if(xv<par[3]){
      corv= par[0]*pow(par[3],par[1])+par[2];
      corv=corv+par[4]*(xv-par[3])+par[5]*pow(xv-par[3],2)+par[6]*pow(xv-par[3],3);
  }
  return corv;
}

///--ECAL correction
int mcecalcor(float &ecallq, float &ecalhq, int il, float rigidity,int mcz=2){

  Double_t rig=fabs(rigidity)>2?fabs(rigidity):2;
  if(rig>3000)rig=3000;  
  if(mcz==1){
    rig=fabs(rigidity)>1?fabs(rigidity):1;
    if(rig>2000)rig=2000;
  }
//---Helium
  Double_t ecalparmc[4][7]={//MC
    0.0479861, 0.0126610, -0.0378890, 2.2490407,  0.0001293, -0.0000977, -0.0016877,
    0.0225523, 0.0337158, -0.0126005, 2.1077173, -0.0007609, -0.0009312, -0.0024658,//ECAL Layer
    0.0042997, 0.0980365,  0.0055333, 2.1033041, -0.0006734, -0.0008072, -0.0022522,
    0.0276485, 0.0226715, -0.0179146, 2.1613681, -0.0004149, -0.0011459, -0.0024140,//ECAL Hit
  };

  Double_t ecalparis[4][7]={//ISS
    0.0035795, 0.2193598, 0.0057460, 2.1314707, -0.0008672, -0.0018276, -0.0022184,
    0.0044904, 0.1774051, 0.0047072, 2.3338160, -0.0003557, -0.0011491, -0.0014756,//ECAL Layer
    0.0033852, 0.1642835, 0.0057009, 2.3583422, -0.0005074, -0.0011155, -0.0013167,
    0.0042430, 0.1476668, 0.0047559, 2.5950428, -0.0002014, -0.0007800, -0.0008929,//ECAL Hit
  };
//--Proton  
  Double_t ecalparmcpr[4][7]={//MC
    0.0059173, 0.0753294,  0.0024726, 1.4908326, 0.0011266,  0.0009793, -0.0008088,
    0.0046032, 0.0659339,  0.0038048, 1.8275192, 0.0001725, -0.0011532, -0.0011761,//ECAL Layer
    0.0027833, 0.1240611,  0.0053738, 1.5385776, 0.0002157, -0.0008651, -0.0015466,
    0.0081520, 0.0339619,  0.0000180, 1.8371391, -0.0009639, -0.0027263, -0.0016935,//ECAL Hit
  };

  Double_t ecalparispr[4][7]={//ISS
    0.0056698, 0.0305338, 0.0027235, 1.9848928, -0.0004627, -0.0007033, -0.0007309,
    0.0044469, 0.0353072, 0.0039166, 1.9922967, -0.0005095, -0.0008335, -0.0008128,//ECAL Layer
    0.0007592, 0.3328560, 0.0072632, 2.0268567, -0.0004891, -0.0008704, -0.0007962,
    0.0079269, 0.0366281, 0.0001099, 1.9963815, -0.0005389, -0.0009532, -0.0008901,//ECAL Hit
  };
//--Carbon
  Double_t ecalparmccarbon[4][7]={//MC
/*    0.0121945,0.0340709,-0.0006724,3.3181087,0.0001283,-0.0013381,-0.0007535,
    0.0214790,0.0298771,-0.0100514,3.3743065,0.0001983,-0.0012401,-0.0006795,//ECAL Layer
    0.0103825,0.0454376,0.0002921,2.4849065,-0.0005793,-0.0017645,-0.0016642,
    0.0049463,0.0363247,0.0064336,3.6317355,0.0000518,-0.0010552,-0.0005126,//ECAL Hit*/
//---New
     0.0129756,0.0306634,-0.0017389,3.3469179,0.0001233,-0.0012567,-0.0006896,
     0.0327358,0.0140561,-0.0212122,3.6488948,0.0001282,-0.0012673,-0.0005697,//ECAL Layer
     0.0067624,0.0316620,0.0039864,3.1818788,0.0000698,-0.0009796,-0.0006742,
     0.0085294,0.0285890,0.0024304,3.5988475,0.0000703,-0.0010454,-0.0005158,//ECAL Hit   
  };
  Double_t ecalpariscarbon[4][7]={//ISS
/*    0.0019136,0.4269783,0.0069054,3.1594495,0.0003685,-0.0006875,-0.0004203,
    0.0065552,0.2260307,0.0015053,2.6419639,0.0006265,-0.0002590,-0.0005261,//ECAL Layer
    0.0094613,0.0738040,-0.0010287,2.5569284,0.0000758,-0.0003878,-0.0005480,
    0.0053113,0.1768355,0.0028654,2.8124160,0.0003251,-0.0004243,-0.0004823,//ECAL Hit*/
//---New
    0.0098685,0.1281274,-0.0013518,2.9552966,0.0004843,-0.0005854,-0.0004737,
    0.0083964,0.1873732,-0.0004390,2.6710280,0.0005822,-0.0003113,-0.0005233,//ECAL Layer
    0.0066681,0.0894437,0.0019896,2.4596994,0.0002029,-0.0001840,-0.0005518,
    0.0070507,0.1253749,0.0012514,2.8099254,0.0003279,-0.0003843,-0.0004710,//ECAL Hit  
  };
//--Add Nitrogen
   Double_t ecalparmcn14[4][7]={//MC
   0.0052868,0.0620451,0.0060594,3.5792322,0.0001031,-0.0010030,-0.0004802,
   0.0078527,0.0599314,0.0035742,3.8564051,0.0001306,-0.0009679,-0.0003947,//ECAL Layer
   0.0041681,0.0451582,0.0066618,3.4906205,0.0000570,-0.0007224,-0.0004153,
   0.0638600,0.0086318,-0.0534267,2.7730790,0.0017202,0.0016013,0.0000448,//ECAL Hit
  };
  Double_t ecalparisn14[4][7]={//ISS
   0.0018649,0.4586750,0.0065418,2.8982779,0.0004836,-0.0005456,-0.0004574,
   0.0020999,0.5059314,0.0058962,2.9849590,0.0007652,-0.0002240,-0.0003347,//ECAL Layer
   0.0039107,0.1075002,0.0046600,2.6873587,0.0002839,-0.0002874,-0.0004317,
   0.0101011,0.0913695,-0.0021210,2.0885495,0.0004727,0.0016118,0.0002170, //ECAL Hit
  };
//--Add Oxygen
  Double_t ecalparmcoxygen[4][7]={//MC
    0.0974206,0.0073841,-0.0865369,2.1778020,0.0016197,0.0033359,0.0009173,
    0.0138592,0.0288212,-0.0021380,4.2706568,0.0000964,-0.0006323,-0.0002066,//ECAL Layer
    0.0047010,0.0550126,0.0061987,4.0053008,0.0000699,-0.0003655,-0.0001582,
    0.0064332,0.0451105,0.0045934,4.4424161,0.0000699,-0.0003527,-0.0001136,//ECAL Hit
  };
  Double_t ecalparisoxygen[4][7]={//ISS
    0.0011764,0.5379166,0.0071479,2.8332090,0.0007235,-0.0001123,-0.0002619,
    0.0010932,0.6794235,0.0069187,3.3921935,0.0003662,-0.0005704,-0.0002811,//ECAL Layer
    0.0027088,0.1793857,0.0054304,1.8686126,0.0002907,0.0018557,0.0006692,
    0.0029421,0.2456303,0.0049037,2.9764441,0.0004073,-0.0002577,-0.0002893,//ECAL Hit
  };
//--
//--Li
  Double_t ecalparmcli[4][7]={//MC
    0.0050678,0.1033482,0.0060726,3.1253335,0.0001885,-0.0013347,-0.0010009,
    0.0131151,0.0427609,-0.0017866,3.3148203,0.0001781,-0.0016080,-0.0009763,//ECAL Layer
    0.0009466,0.3630223,0.0095447,2.6497549,0.0005582,-0.0001488,-0.0011318,
    0.0098682,0.0354235,0.0010303,3.3307863,0.0001069,-0.0015499,-0.0009281,//ECAL Hit
  };
  Double_t ecalparisli[4][7]={//ISS
   0.0048457,0.2792245,0.0040306,2.5200731,-0.0001856,-0.0012477,-0.0012543,
   0.0121218,0.1200776,-0.0032565,2.4295636,0.0001345,-0.0006640,-0.0011904,//ECAL Layer
   0.0002196,1.1494843,0.0090206,2.8778767,-0.0003453,-0.0012656,-0.0008886,
   0.0007159,0.7260869,0.0082870,2.4324861,-0.0001762,-0.0006393,-0.0011062,//ECAL Hit
  };
//---Be
  Double_t ecalparmcbe[4][7]={//MC
   0.0099138,0.0447797,0.0016369,3.0894024,0.0001511,-0.0013280,-0.0008001,
   0.0439297,0.0136732,-0.0324330,3.0684906,0.0001969,-0.0015292,-0.0009184,//ECAL Layer
   0.0205059,0.0193420,-0.0096637,2.9399391,0.0001351,-0.0010048,-0.0007816,
   0.0146091,0.0224148,-0.0034788,3.1506410,0.0001066,-0.0013005,-0.0007653,//ECAL Hit
  };
  Double_t ecalparisbe[4][7]={//ISS
   0.0089394,0.1218354,0.0004134,2.5836514,0.0010266,0.0003189,-0.0004373,
   0.0106023,0.1609808,-0.0022778,2.8119632,0.0002978,-0.0006403,-0.0006082,//ECAL Layer
   0.0042797,0.0727417,0.0052207,2.8124106,0.0002613,-0.0004569,-0.0005411,
   0.0137490,0.0533730,-0.0048397,3.1780955,0.0000484,-0.0007286,-0.0004559,//ECAL Hit
  };
 //--Neon
  Double_t ecalparmcneon[4][7]={//MC
    0.0214015,0.0587542,-0.0127901,3.8437382,0.0004929,-0.0005564,-0.0002404,
    0.0214015,0.0587542,-0.0127901,3.8437382,0.0004929,-0.0005564,-0.0002404,//ECALLayer
    0.0105372,0.0890449,-0.0028450,3.5920373,0.0009758,0.0002285,-0.0000148,
    0.0105372,0.0890449,-0.0028450,3.5920373,0.0009758,0.0002285,-0.0000148,//ECALHit
  };
  Double_t ecalparisneon[4][7]={//ISS
    0.0126878,0.1031811,-0.0060266,1.9579214,0.0002380,-0.0002122,-0.0003988,//ECALLayer
    0.0126878,0.1031811,-0.0060266,1.9579214,0.0002380,-0.0002122,-0.0003988,
    0.0007190,0.5398599,0.0065193,1.2945635,-0.0009865,0.0025602,0.0098092,//ECALHit
    0.0007190,0.5398599,0.0065193,1.2945635,-0.0009865,0.0025602,0.0098092,
  };
 //--Mg
  Double_t ecalparmcmg[4][7]={//MC
    0.0201237,0.0637159,-0.0113932,4.0619768,0.0003551,-0.0007097,-0.0002520,
    0.0201237,0.0637159,-0.0113932,4.0619768,0.0003551,-0.0007097,-0.0002520,//ECALLayer
    0.1200641,0.0176991,-0.1143773,1.8667570,0.0013734,0.0060782,0.0038244,
    0.1200641,0.0176991,-0.1143773,1.8667570,0.0013734,0.0060782,0.0038244,//ECALHIT
  };
  Double_t ecalparismg[4][7]={//ISS
    0.0062290,0.2734669,-0.0011648,1.1713093,-0.0031443,-0.0103396,-0.0130113,
    0.0062290,0.2734669,-0.0011648,1.1713093,-0.0031443,-0.0103396,-0.0130113,//ECALLayer
    0.0190668,0.0858725,-0.0141188,1.7577770,0.0006832,-0.0003959,-0.0004292,
    0.0190668,0.0858725,-0.0141188,1.7577770,0.0006832,-0.0003959,-0.0004292,//ECALHIT
  };
  //--Si
  Double_t ecalparmcsi[4][7]={//MC
    0.0161090,0.1045212,-0.0082196,3.5276479,0.0005428,-0.0007645,-0.0004191,
    0.0161090,0.1045212,-0.0082196,3.5276479,0.0005428,-0.0007645,-0.0004191,//ECALLayer
    0.0521485,0.0424184,-0.0466433,2.3164839,0.0006719,-0.0006534,-0.0013877,
    0.0521485,0.0424184,-0.0466433,2.3164839,0.0006719,-0.0006534,-0.0013877,//ECALHIT
  };
  Double_t ecalparissi[4][7]={//ISS
    0.0019132,0.4445065,0.0039777,2.6752719,0.0025618,0.0008634,0.0000857,
    0.0019132,0.4445065,0.0039777,2.6752719,0.0025618,0.0008634,0.0000857,//ECALLayer
    0.0512436,0.0467038,-0.0475780,1.4341011,0.0000938,-0.0074376,-0.0129073,
    0.0512436,0.0467038,-0.0475780,1.4341011,0.0000938,-0.0074376,-0.0129073,//ECALHIT
  };

  int uil=(il<2)?il:1;
//----MC
  double ecallqcormc=ecalcorfun(&rig,ecalparmc[uil]);//Layer Charge
  double ecalhqcormc=ecalcorfun(&rig,ecalparmc[2+uil]);//Hit Charge
//---ISS
  double ecallqcoris=ecalcorfun(&rig,ecalparis[uil]);//Layer Charge
  double ecalhqcoris=ecalcorfun(&rig,ecalparis[2+uil]);//Hit Charge
//---Proton
  if(mcz==1){
     ecallqcormc=ecalcorfun(&rig,ecalparmcpr[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcpr[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparispr[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparispr[2+uil]);//Hit Charge
  }
  else if(mcz==6||mcz==5){
     ecallqcormc=ecalcorfun(&rig,ecalparmccarbon[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmccarbon[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalpariscarbon[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalpariscarbon[2+uil]);//Hit Charge
  }
  else if(mcz==8){
     ecallqcormc=ecalcorfun(&rig,ecalparmcoxygen[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcoxygen[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparisoxygen[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparisoxygen[2+uil]);//Hit Charge
  }
  else if(mcz==7){
     ecallqcormc=ecalcorfun(&rig,ecalparmcn14[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcn14[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparisn14[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparisn14[2+uil]);//Hit Charge
  }
  else if(mcz==3){
     ecallqcormc=ecalcorfun(&rig,ecalparmcli[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcli[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparisli[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparisli[2+uil]);//Hit Charge
  }
  else if(mcz==4){
     ecallqcormc=ecalcorfun(&rig,ecalparmcbe[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcbe[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparisbe[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparisbe[2+uil]);//Hit Charge
  }
  else if(mcz==10){
     ecallqcormc=ecalcorfun(&rig,ecalparmcneon[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcneon[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparisneon[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparisneon[2+uil]);//Hit Charge
  }
  else if(mcz==12){
     ecallqcormc=ecalcorfun(&rig,ecalparmcmg[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcmg[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparismg[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparismg[2+uil]);//Hit Charge
  }
  else if(mcz==14){
     ecallqcormc=ecalcorfun(&rig,ecalparmcsi[uil]);//Layer Charge
     ecalhqcormc=ecalcorfun(&rig,ecalparmcsi[2+uil]);//Hit Charge
     ecallqcoris=ecalcorfun(&rig,ecalparissi[uil]);//Layer Charge
     ecalhqcoris=ecalcorfun(&rig,ecalparissi[2+uil]);//Hit Charge
  }
//--
  ecallq=ecallq/ecallqcormc*ecallqcoris; 
  ecalhq=ecalhq/ecalhqcormc*ecalhqcoris;
  return 0; 
}

///---Helium Shower Correction
Double_t showcorfun(Double_t *x,Double_t *par){  //Sigta
 double lx=log(x[0]);
 if(lx>par[4]){return par[0]+par[5]*pow(lx,par[6]);}
 else         {return par[0]+par[5]*pow(par[4],par[6])+par[1]*(lx-par[4])+par[2]*pow((lx-par[4]),2)+par[3]*pow((lx-par[4]),3);}
};

double  getmcshowedepc(double mcshowedep){
//  double ecalparmcis916[]={0.384512,0,0,0,1.37441,0.810884,-0.560207};//B916
  double ecalparmcis916[]={0.395857,0,0,0,1.4144,0.805306,-0.585225};//B916
  double ecalparmcis[]={0.798407,0,0,0,1.75985,0.990441,-2.5614};//B800
  double mcshowedepc=mcshowedep/showcorfun(&mcshowedep,ecalparmcis916);//Enhance
  return mcshowedepc;
};


///---Helium Cutoff Rigidity(cutoff->rigidity)
Double_t cutoffcorfun(Double_t *x,Double_t *par){  //Sigta
 double xv=log(x[0]);
 return par[0]+par[1]*xv+par[2]*pow(xv,2)+par[3]*pow(xv,3)+par[4]*pow(xv,4);
}

///---Helium/Proton Cutoff Correction
double getcutoffrig(double cutoff,int version=1,int z=2,bool isreal=0){
   double cutoffcv=cutoff;
   double cutoffrig=1;
   if(z>=26&&isreal){//Fe56 Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.220229,0.371893,0.727436,0.811273,0.823082,0.838543};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineFe56ISB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=26&&!isreal){//Fe56 1220MC
     if     (cutoffcv<2)   cutoffcv=2;
     else if(cutoffcv>2000.)cutoffcv=2000.;
     const int nnode=7;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32,2000};
     double ynode[nnode]={5.26684,2.5192,1.24378,1.08722,1.04269,1.02494,1.00581};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineFe56MCB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==13&&isreal){//Al27 Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.252904,0.476042,0.742216,0.823576,0.900275,0.85686};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineAl27ISB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==13&&!isreal){//Al27 1220MC
     if     (cutoffcv<1.7)  cutoffcv=1.7;
     else if(cutoffcv>2000.)cutoffcv=2000.;
     const int nnode=7;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32,2000};
     double ynode[nnode]={2.54164,1.54787,1.10505,1.04452,1.02094,1.01446,1.00246};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineAl27MCB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==11&&isreal){//Na23 Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.271709,0.486122,0.742116,0.84251,0.914826,0.899094};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineNa23ISB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==11&&!isreal){//Na23 1220MC
     if     (cutoffcv<1.7)  cutoffcv=1.7;
     else if(cutoffcv>2000.)cutoffcv=2000.;
     const int nnode=7;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32,2000};
     double ynode[nnode]={2.24998,1.44626,1.08889,1.03816,1.01872,1.01278,1.00418};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineNa23MCB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==9&&isreal){//F19 Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.265198,0.458572,0.729375,0.849287,0.922529,0.9211};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineF19ISB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==9&&!isreal){//F19 1220MC
     if     (cutoffcv<1.7)  cutoffcv=1.7;
     else if(cutoffcv>2000.)cutoffcv=2000.;
     const int nnode=7;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32,2000};
     double ynode[nnode]={2.03005,1.35632,1.07331,1.0323,1.0148,1.01159,1.00266};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineF19MCB1220N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(!isreal){//MC
     return cutoff;
   } 
   else if(z>=16&&version>=4){//S32 Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.240775,0.413912,0.726462,0.820522,0.897751,0.848536};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineS32B1213N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=14&&version>=4){//Si Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.229246,0.409687,0.721586,0.788406,0.85631,0.832297};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineSiB1213N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=12&&version>=4){//Mg Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.246673,0.453641,0.74023,0.798175,0.874521,0.859121};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineMgB1213N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   } 
   else if(z>=10&&version>=4){//Ne Cutoff 25degree(7years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.203171,0.377189,0.649834,0.808162,0.874033,0.941951};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineNeB1213N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=8&&version>=4){//O Cutoff 25degree(5years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.235766,0.449896,0.699443,0.75565,0.860043,0.86};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineOB1075N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==7&&version>=4){//N Cutoff 25degree(5years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.258097,0.490318,0.709826,0.810821,0.896449,1.07083};//cutoffi[0][1]->rigidity
     double ynode[nnode]={0.262061,0.488934,0.716439,0.817497,0.90387,0.902941};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineN14B1075N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=6&&version>=4){//C Cutoff 25degree(5years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.250399,0.477495,0.699412,0.764306,0.86101,0.994726};//cutoffi[0][1]->rigidity
     double ynode[nnode]={0.252539,0.481061,0.707472,0.772043,0.870552,0.862554};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineCB1075N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=5&&version>=4){//Boron Cutoff 25degree(5years ISS) 
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.269368,0.468079,0.712478,0.834422,0.924926,1.11459};
     double ynode[nnode]={0.270401,0.475897,0.718513,0.843667,0.928214,0.928558};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineBoron1075N",nnode,node,ynode,(QSplineFit::LogX2),"b1e2");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=4&&version>=4){//Beryllium Cutoff 25degree(5years ISS) 
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.272514,0.479601,0.689053,0.829663,0.913712,1.08793};
     double ynode[nnode]={0.271553,0.476539,0.700222,0.830457,0.931929,0.937471};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineBe1082N",nnode,node,ynode,(QSplineFit::LogX2),"b1e2");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=3&&version>=4){//Lithium Cutoff 25degree(5years ISS) 
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.265344,0.459081,0.683201,0.828572,0.91923,1.10529};
     double ynode[nnode]={0.256105,0.46664,0.689859,0.837793,0.927718,0.943196};
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineLithium1082N",nnode,node,ynode,(QSplineFit::LogX2),"b1e2");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==2&&version>=4){//Helium Cutoff 25degree B1081MC(5years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.289395,0.529818,0.691676,0.788728,0.859789,1.02414};//cutoffi[0][1]->rigidity
     double ynode[nnode]={0.293954,0.534909,0.699109,0.797146,0.867234,0.860136};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineHeB1081N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==1&&version>=4){//Proton Cutoff 25degree B1082MC(5years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
//     double ynode[nnode]={0.425694,0.63058,0.715138,0.818377,0.880383,1.07674};//cutoffi[0][1]->rigidity
     double ynode[nnode]={0.435626,0.637208,0.726542,0.824046,0.887606,0.907727};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplinePrB1082N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z>=5&&version>=2){//O|C|B Cutoff 25degree(4years ISS)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.248553,0.475845,0.690588,0.764934,0.860664,0.997498};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineCB1048N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==2&&version>=2){//Helium Cutoff 25degree B1036MC
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>32.)cutoffcv=32.;
     const int nnode=6;
     double node[nnode]={1, 2.0, 4.0, 8, 16, 32};
     double ynode[nnode]={0.280866,0.502808,0.683751,0.804904,0.869264,1.06991};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineHeB1036N",nnode,node,ynode,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if (z==6){//Cutoff 25 degree/Carbon(L1InnerL9)
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>50.)cutoffcv=50.;
     const int nnode=7;
     double node[nnode]={0.5, 1.0, 2.0, 5.0, 10, 20, 50};
     double ynode[nnode]={0.194609,0.228071,0.490048,0.744831,0.780514,0.915049,1.15384};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplineCarbon",nnode,node,ynode,(QSplineFit::LogX2),"b1e2");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else if(z==1){//Cutoff 25 degree/Proton
     if     (cutoffcv<1)  cutoffcv=1;
     else if(cutoffcv>50.)cutoffcv=50.;
     const int nnode=7;
     double node[nnode]={0.5, 1.0, 2.0, 5.0, 10, 20, 50};
     double ynode[nnode]={0.124183,0.366005,0.628356,0.751274,0.8375,0.966614,1.99391};//cutoffi[0][1]->rigidity
     static QSplineFit *rvcspline=new QSplineFit("RVCSplinePr",nnode,node,ynode,(QSplineFit::LogX2),"b1e2");
     cutoffrig=cutoff/(rvcspline->GetFun()->Eval(cutoffcv));
   }
   else {//Helium 45 degree
     if(version==1){
       if     (cutoffcv<1.4)cutoffcv=1.4;
       else if(cutoffcv>50.)cutoffcv=50.;
       double cutoffpar916[]={0.250083,0.264092,0.214294,-0.150608,0.0270317};
       cutoffrig=cutoff/cutoffcorfun(&cutoffcv,cutoffpar916);
    }
    else {
      if     (cutoffcv<1.5)cutoffcv=1.5;
      else if(cutoffcv>50.)cutoffcv=50.;
      double cutoffpar916[]={0.196348,0.500222,-0.0698814,-0.0244015,0.00848863};//cutoffi[3][1]->rigidity
      cutoffrig=cutoff/cutoffcorfun(&cutoffcv,cutoffpar916);
    }
  }
  return cutoffrig;
}

///---Helium Edep Rigidity(ecal-edep->rigidity)
Double_t edeprigcorfun(Double_t *x,Double_t *par){  //Sigta
 double xv=log(x[0]);
 return par[0]+par[1]*xv+par[2]*pow(xv,2)+par[3]*pow(xv,3)+par[4]*pow(xv,4);
}

///---Helium Edep Correction
double getedeprig(double showedep,int z=2,bool isreal=1,int vers=1){
   double showedepcv=showedep;
   double showedeprig=showedep;
   if(z>=26){//Fe56 B1220 Edep>3.5GeV recR>3.5GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>2000.)showedepcv=2000.;
     const int nnodemc=9;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodemc[nnodemc]={0.354945,0.84613,1.07248,3.62068,5.01287,7.11935,5.83893,6.79878,4.34038};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineFe561220MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=9;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodeis[nnodeis]={0.244698,0.55172,1.09447,3.15166,4.33845,4.25323,4.09802,5.38753,2.37969};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineFe561220IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=16&&vers>=4){//S32 B1215 Edep>3.5GeV recR>3.8GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>1200.)showedepcv=1200.;
     const int nnodemc=8;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodemc[nnodemc]={0.00372441,0.777757,1.14926,3.23598,3.73613,3.22361,4.04014,3.12333};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineS321215MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodeis[nnodeis]={0.457935,0.763891,1.18702,2.49834,2.35068,2.92429,3.49011,2.28204};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineS321215IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=14&&vers>=4){//Silicon B1213 Edep>3.5GeV recR>3.8GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>1200.)showedepcv=1200.;
     const int nnodemc=8;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodemc[nnodemc]={0.273836,0.78453,0.910784,2.87048,3.10075,2.99019,3.69897,2.69406};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineSi1213MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodeis[nnodeis]={0.487848,0.777961,1.22522,2.17859,1.97737,2.81507,2.98417,2.86865};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineSi1213IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
  else if(z>=13){//Al27 B1220 Edep>3.5GeV recR>3.6-3.8 GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>2000.)showedepcv=2000.;
     const int nnodemc=9;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodemc[nnodemc]={0.456997,0.800907,1.17124,2.99946,3.06001,2.9569,3.56875,3.39294,2.18536};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineAl271220MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=9;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodeis[nnodeis]={0.526259,0.814616,1.29576,2.05769,2.04243,2.82243,2.82127,3.19357,2.7868};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineAl271220IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=12&&vers>=4){//Mg B1215 Edep>3.5GeV recR>3.1GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>1200.)showedepcv=1200.;
     const int nnodemc=8;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodemc[nnodemc]={0.475343,0.734203,1.29648,2.67641,2.67077,2.76283,3.28069,2.2611};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineMg1215MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodeis[nnodeis]={0.551862,0.920283,1.49352,1.89379,1.89734,2.62394,2.61216,2.92354};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineMg1215IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=11){//Na23 B1220 Edep>3.5GeV recR>3.6-3.8 GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>2000.)showedepcv=2000.;
     const int nnodemc=9;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodemc[nnodemc]={0.567282,0.788748,1.37222,2.60775,2.59247,2.68739,3.12389,2.8727,1.87518};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineNa231220MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=9;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodeis[nnodeis]={0.625083,0.962668,1.65685,1.85432,1.94406,2.51948,2.64361,2.9568,1.8139};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineNa231220IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=10&&vers>=4){//Ne B1215 Edep>3.5GeV recR>2.5GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>1200.)showedepcv=1200.;
     const int nnodemc=8;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodemc[nnodemc]={0.515425,1.15773,1.62423,2.27288,2.26666,2.45938,2.83662,1.86027};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineNe1215MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120,1200};
     double ynodeis[nnodeis]={0.126283,0.581753,1.64254,1.63706,1.86255,2.27714,2.31068,2.0692};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineNe1215IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=9){//F19 B1220 Edep>3.5GeV recR>3.6-3.8 GV
     if     (showedepcv<3.5) showedepcv=3.5;
     else if(showedepcv>2000.)showedepcv=2000.;
     const int nnodemc=9;
     double nodemc[nnodemc]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodemc[nnodemc]={0.68788,1.17464,1.86082,2.20993,2.29015,2.3809,2.68127,2.33592,1.59085};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineF191220MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=9;
     double nodeis[nnodeis]={2., 3, 5.0, 15, 25, 50, 120, 300, 2000};
     double ynodeis[nnodeis]={0.70200,1.19876,1.76633,1.61922,1.88249,2.08462,2.47386,2.33592,1.59085};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineF191220IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z>=8&&vers>=4){//Oxygen B1075_201 Edep>2.5GeV recR>3.7GV
     if     (showedepcv<2.5) showedepcv=2.5;
     else if(showedepcv>800.)showedepcv=800.;
     const int nnodemc=7;
     double nodemc[nnodemc]={2.5, 5.0, 10, 30, 90, 270,800};
     double ynodemc[nnodemc]={0.818695,1.91126,2.16314,2.43818,2.47937,2.39002,1.39793};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineO1075MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=7;
     double nodeis[nnodeis]={2.5, 5.0, 10, 30, 90, 270,800};
      double ynodeis[nnodeis]={0.686582,1.68842,1.40549,1.85342,1.94666,1.87724,1.3806}; 
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineO1075IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==7&&vers>=4){//Nitrogen B1064 Edep>2.5GeV recR>5.6GV
     if     (showedepcv<2.5) showedepcv=2.5;
     else if(showedepcv>300.)showedepcv=300.;
     const int nnodemc=7;
     double nodemc[nnodemc]={2.5, 3.3, 5.0, 10, 30, 90, 300};
     double ynodemc[nnodemc]={0.597216,1.19861,1.61764,2.07707,2.24271,2.4095,2.35951};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineN141064MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=7;
     double nodeis[nnodeis]={2.5, 3.3, 5.0, 10, 30, 90, 300};
      double ynodeis[nnodeis]={0.548863,1.12018,1.37808,1.29361,1.72332,1.84099,1.68697};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineN141064IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==6&&vers>=4){//Carbon B1075_201 Edep>2.3GeV recR>3.9GV
     if     (showedepcv<2.3) showedepcv=2.3;
     else if(showedepcv>800.)showedepcv=800.;
     const int nnodemc=8;
     double nodemc[nnodemc]={2.3, 3.3, 5.0, 10, 30, 90, 270,800};
     double ynodemc[nnodemc]={0.667419,1.26603,1.41813,1.98884,1.90491,2.06571,1.64951,0.695338};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineC1075MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={2.3, 3.3, 5.0, 10, 30, 90, 270,800};
     double ynodeis[nnodeis]={0.587946,1.01869,1.00803,1.15498,1.47256,1.59853,1.34558,1.33631};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineC1075IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
  else if(z==5&&vers>=4){//Boron B1075_201 Edep>2.1GeV recR>3.7GV
     if     (showedepcv<2.1) showedepcv=2.1;
     else if(showedepcv>300.)showedepcv=300.;
     const int nnodemc=9;
     double nodemc[nnodemc]={2.1, 4.0, 5.0,7.0, 8.5, 10,  30, 90, 300};
     double ynodemc[nnodemc]={0.570792,1.12422,0.911004,2.10631,1.87413,1.74329,1.76147,1.86985,1.70885};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineB111075MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=9;
     double nodeis[nnodeis]={2.1, 4.0, 5.0,7.0, 8.5, 10,  30, 90, 300};
     double ynodeis[nnodeis]={0.657034,0.714452,0.865208,1.03028,0.992105,1.02828,1.34538,1.39472,1.31644};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineB111075IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
    else if(z==4&&vers>=4){//Beryllium B1082_201 Edep>2GeV recR>5GV
     if     (showedepcv<2.0) showedepcv=2.0;
     else if(showedepcv>300.)showedepcv=300.;
     const int nnodemc=9;
     double nodemc[nnodemc]={2.0, 4.0, 5.0, 6.0, 8.5, 10,  30, 90, 300};
     double ynodemc[nnodemc]={0.648919,1.20375,1.82424,1.43599,1.37149,1.42205,1.52174,1.60134,1.07302};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineBe71082201MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=9;
     double nodeis[nnodeis]={2.0, 4.0, 5.0, 6.0, 8.5, 10,  30, 90, 300};
     double ynodeis[nnodeis]={0.400246,0.72521,0.816398,0.774233,0.875211,0.909193,1.15651,1.09923,0.965349};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineBe71082201IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==3&&vers>=4){//Li B1082_201 Edep>2GeV recR>3.7GV
     if     (showedepcv<2.0) showedepcv=2.0;
     else if(showedepcv>300.)showedepcv=300.;
     const int nnodemc=7;
     double nodemc[nnodemc]={2.0, 3.0, 4.0, 10,  30, 90, 300};
     double ynodemc[nnodemc]={0.581026,1.12985,1.06348,1.00266,1.09275,1.17864,1.0664};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineL61082201MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=7;
     double nodeis[nnodeis]={2.0, 3.0, 4.0, 10,  30, 90, 300};
     double ynodeis[nnodeis]={0.544633,0.535835,0.568324,0.735879,0.902687,0.845503,0.801194};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineL61082201IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==2&&vers>=4){//He-1081(5yearsISS) Edep>1.2GeV recR>3.2GV
     if     (showedepcv<1.2) showedepcv=1.2;
     else if(isreal==1&&showedepcv>600.)showedepcv=600.;
     else if(isreal==0&&showedepcv>800.)showedepcv=800.;
/*     const int nnodemc=11;
     double nodemc[nnodemc]={1.2,1.7,2.2, 2.6, 4.0, 9.0, 20, 60, 100,  200, 600};
     double ynodemc[nnodemc]={0.533957,0.801567,0.716855,0.672132,0.705533,0.767084,0.804727,0.799133,0.698319,0.360233,0.304532};*/
     const int nnodemc=12;
     double nodemc[nnodemc]={1.2,1.7,2.2, 2.6, 4.0, 9.0, 20, 60, 100,  150, 300,800};
      double ynodemc[nnodemc]={0.540305,0.771347,0.663425,0.638575,0.685109,0.742031,0.776833,0.776306,0.670452,0.442826,0.28154,0.295591};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineHe1114MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=11;
     double nodeis[nnodeis]={1.2,1.7,2.2, 2.6, 4.0, 9.0, 20, 60, 100,  200, 600};
     double ynodeis[nnodeis]={0.371922,0.369609,0.382077,0.406626,0.457835,0.560292,0.622753,0.587626,0.571762,0.498667,0.440412};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineHe1081IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==1&&vers>=4){//Pr-1082(5yearsISS) Edep>1GeV recR>4GV
     if     (showedepcv<1) showedepcv=1;
     else if(showedepcv>400.)showedepcv=400.;
     const int nnodemc=11;
     double nodemc[nnodemc]={1, 2.0, 3.5, 6.0, 7.5, 9.0, 20, 60, 100, 200, 400};
     double ynodemc[nnodemc]={0.39256,0.450322,0.444342,0.424005,0.479131,0.472893,0.434433,0.379869,0.344332,0.309497,0.30241};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplinePr1082MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=11;
     double nodeis[nnodeis]={1, 2.0, 3.5, 6.0, 7.5, 9.0, 20, 60, 100, 200, 400};
     double ynodeis[nnodeis]={0.251044,0.309981,0.328849,0.363598,0.379791,0.386177,0.391303,0.359362,0.341314,0.318128,0.296629};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplinePr1082IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==8){//Oxygen B1036
     if     (showedepcv<3) showedepcv=3;
     else if(showedepcv>800.)showedepcv=800.;
     const int nnodemc=7;
     double nodemc[nnodemc]={2.5, 5.0, 10, 30, 90, 270,800};
     double ynodemc[nnodemc]={0.418841,1.66056,1.80646,2.01222,2.38368,2.25133,1.29075};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineOxygenMC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=7;
     double nodeis[nnodeis]={2.5, 5.0, 10, 30, 90, 270,800};
     double ynodeis[nnodeis]={0.688647,1.67126,1.39475,1.84848,1.93737,1.84406,1.31992};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineOxygenIS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;      
   }
   else if(z==5&&vers>=2){//Boron B1064
     if     (showedepcv<1.5) showedepcv=1.5;
     else if(showedepcv>500.)showedepcv=500.;
     const int nnodemc=7;
     double nodemc[nnodemc]={1.5,2.5, 5.0, 10, 30, 100, 500};
     double ynodemc[nnodemc]={0.205157,0.522007,0.727075,1.27328,1.33651,1.72292,0.912591};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineB111064MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=7;
     double nodeis[nnodeis]={1.5,2.5, 5.0, 10, 30, 100, 500};
     double ynodeis[nnodeis]={0.340753,0.594288,0.836936,1.06941,1.31891,1.44935,1.11659};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineB111064IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==6&&vers>=2){//Carbon B1048
     if     (showedepcv<2.5) showedepcv=2.5;
     else if(showedepcv>800.)showedepcv=800.;
     const int nnodemc=8;
     double nodemc[nnodemc]={2.3, 3.3, 5.0, 10, 30, 90, 270,800};
     double ynodemc[nnodemc]={0.300782,0.906925,0.949702,1.36134,1.61177,1.95681,1.5839,0.760253};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineC1048MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={2.3, 3.3, 5.0, 10, 30, 90, 270,800};
     double ynodeis[nnodeis]={0.591131,0.988732,0.99056,1.15144,1.47532,1.59928,1.34552,1.33585};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineC1048IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     showedeprig=showedep/corv;
   }
   else if(z==6){//Carbon B1005
     if     (showedepcv<1) showedepcv=1;
     else if(showedepcv>800.)showedepcv=800.;
//----
     const int nnodemc=8;
     double nodemc[nnodemc]={0.7, 2.0, 5.0, 10, 30, 90, 270,800};
     double ynodemc[nnodemc]={0.122059,0.258777,0.951862,1.22418,1.28826,1.19673,0.888815,0.735069};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineCarbonMC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=8;
     double nodeis[nnodeis]={0.7, 2.0, 5.0, 10, 30, 90, 270,800};
     double ynodeis[nnodeis]={-0.0501132,0.50825,0.978899,1.12005,1.49557,1.51449,1.43446,1.26946};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineCarbonIS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
//----
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     if(corv<0.2)corv=0.2;
//     showedeprig=showedep/(rvespline->GetFun()->Eval(showedepcv));
     showedeprig=showedep/corv;
   }
   else if(z==3){//Li B1005
     if     (showedepcv<1) showedepcv=1;
     else if(showedepcv>300.)showedepcv=300.;
//---
     const int nnodemc=7;
     double nodemc[nnodemc]={0.7, 2.0, 5.0, 15, 40, 90, 300};
     double ynodemc[nnodemc]={0.0516439,0.381472,0.678389,0.684791,0.743627,0.628494,0.486176};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineLiMC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2),"b1e1");
     const int nnodeis=7;
     double nodeis[nnodeis]={0.7, 2.0, 5.0, 15, 40, 90, 300};
     double ynodeis[nnodeis]={0.0755364,0.4042,0.608137,0.80136,0.902887,0.796169,0.732512};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineLiIS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2),"b1e1");
//---
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else         rvespline=rvesplinemc;
     double corv=rvespline->GetFun()->Eval(showedepcv);
     if(corv<0.2)corv=0.2;
//     showedeprig=showedep/(rvespline->GetFun()->Eval(showedepcv));
     showedeprig=showedep/corv;
   }
   else if(z==1){//Proton
     if     (showedepcv<1) showedepcv=1;
     else if(showedepcv>100.)showedepcv=100.;
     const int nnode=10;
     double node[nnode]={0.5, 1.0, 2.0, 5.0, 10, 20, 50, 100, 250, 800};
     double ynode[nnode]={0.172075,0.242087,0.312844,0.349503,0.390956,0.386582,0.377517,0.345696,0.33391,0.0260687};
     static QSplineFit *rvespline=new QSplineFit("RVESplinePr",nnode,node,ynode,(QSplineFit::LogX2),"b1e1");
     showedeprig=showedep/(rvespline->GetFun()->Eval(showedepcv));
   }
   else if(z==2&&vers>=2){//Helium B1036MC
     if     (showedepcv<1) showedepcv=1;
     else if(showedepcv>800.)showedepcv=800.;
     const int nnodemc=9;
     double nodemc[nnodemc]={1, 3.0, 9.0, 20, 60, 100,  200, 400, 800};
//     double ynodemc[nnodemc]={0.393263,0.486489,0.642468,0.746268,0.828791,0.671258,0.378413,0.395931,0.495247};
     double ynodemc[nnodemc]={0.397045,0.484199,0.64398,0.74222,0.829552,0.673613,0.380623,0.395175,0.496607};
     static QSplineFit *rvesplinemc=new QSplineFit("RVESplineHeB1036MC",nnodemc,nodemc,ynodemc,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
//     double ynodemcf[nnodemc]={0.390626,0.453872,0.565387,0.638004,0.594062,0.391585,0.212708,0.217813,0.217597};
     double ynodemcf[nnodemc]={0.389574,0.454363,0.565295,0.637758,0.593736,0.392171,0.212773,0.217905,0.217916};
     static QSplineFit *rvesplinemcf=new QSplineFit("RVESplineHeB1036MCFast",nnodemc,nodemc,ynodemcf,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");//Fast MC
     const int nnodeis=9;
     double nodeis[nnodeis]={1, 3.0, 9.0, 20, 60, 100,  200, 400, 800};
     double ynodeis[nnodeis]={0.338986,0.415348,0.560167,0.628606,0.595856,0.585343,0.51421,0.48622,0.434583};
     static QSplineFit *rvesplineis=new QSplineFit("RVESplineHeB1036IS",nnodeis,nodeis,ynodeis,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE),"b1e1");
     QSplineFit *rvespline=0;
     if(isreal==1)rvespline=rvesplineis;
     else {
        rvespline=rvesplinemc;
        if(vers>=3)rvespline=rvesplinemcf; //Fast MC
     }
     double corv=rvespline->GetFun()->Eval(showedepcv);
     if(corv<0.3)corv=0.3;
     showedeprig=showedep/corv;
   }
   else {//Helium
     if     (showedepcv<1.5) showedepcv=1.5;
     else if(showedepcv>450.)showedepcv=450.;
     double showedeppar916[]={0.336773,-0.0422859,0.136883,-0.0399685,0.00315131};//cutoffi[3][1]->rigidity
     showedeprig=showedep/edeprigcorfun(&showedepcv,showedeppar916);
  }
  return showedeprig;
}

///--1/R_{L1}-1/R_{Inner} energy dependence(L1Inner Rigidity) correction
double getdl1innervrcor(float dl1innervr,float rigidity,int z=14){ 
  if(z==10){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={2.69424,1.91957,1.13666,0.910863,0.892103,1.05103,1.5898,2.02935,2.05008};
     static QSplineFit *rmsspline=new QSplineFit("Z10dl1innervrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1innervr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==12){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={2.66125,1.89732,1.14607,0.929687,0.918068,1.10235,1.71131,2.27438,2.26337};
     static QSplineFit *rmsspline=new QSplineFit("Z12dl1innervrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1innervr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==26){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={2.06404,1.60063,0.983578,0.735164,0.678017,0.753708,1.11337,1.41052,1.56234};
     static QSplineFit *rmsspline=new QSplineFit("Z26dl1innervrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1innervr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else {
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={2.55813,1.85316,1.15732,0.959431,0.959122,1.17526,1.79777,2.26801,2.31676};
     static QSplineFit *rmsspline=new QSplineFit("Z14dl1innervrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1innervr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
}


///--1/R_{L1}-1/R_{L9} energy dependence(L1IL9 Rigidity) correction
double getdl1l9vrcor(float dl1l9vr,float rigidity,int z=6){

  if (z==2){
/*     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
//     double ynodes[nnodes]={3.78094,2.76085,1.92323,1.82114,1.97561,2.37654,2.42161,1.73121,1.60065};
     double ynodes[nnodes]={3.69482,2.60222,1.61716,1.41366,1.46623,1.72882,2.0061,1.58067,1.44006};//New resolution+New ww*/
     const int nnodes=12;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,80,120,160,350,700,1000};
     double ynodes[nnodes]={3.93253,2.77328,1.6924,1.42809,1.45948,1.7184,1.98834,1.93868,1.84522,1.48921,1.33726,1.32583};
     static QSplineFit *rmsspline=new QSplineFit("Z2dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==1){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={3.63864,3.02835,2.65222,2.74638,3.23631,3.70152,3.19757,2.49832,2.33355};
     static QSplineFit *rmsspline=new QSplineFit("Z1pdl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==26){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.12256,3.06229,1.79957,1.34602,1.28073,1.41863,1.67118,1.16751,0.973525};
     static QSplineFit *rmsspline=new QSplineFit("Z26dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==14){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.06068,3.09373,2.00026,1.7415,1.77936,2.09053,2.18693,1.72299,1.31479};
     static QSplineFit *rmsspline=new QSplineFit("Z14dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==12){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.29015,3.14899,1.98743,1.661,1.6792,1.9976,2.1764,1.67549,1.05516};
     static QSplineFit *rmsspline=new QSplineFit("Z12dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==10){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.54405,3.21503,1.93899,1.63662,1.62271,1.93777,2.04868,1.65418,1.00165};
     static QSplineFit *rmsspline=new QSplineFit("Z10dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
  }
  else if(z==8){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
//     double ynodes[nnodes]={4.48681,3.24638,2.22171,2.04237,2.20397,2.64671,2.73324,2.07498,1.88619};
     double ynodes[nnodes]={4.36608,3.08887,1.86052,1.54929,1.56641,1.81121,2.07245,1.56363,1.19682};//New resolution+New ww
     static QSplineFit *rmsspline=new QSplineFit("Z8dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
      return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
   else if(z==7){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.54499,3.17621,1.81607,1.43553,1.40454,1.59298,1.89564,1.4493,1.19548};//New resolution+New ww
     static QSplineFit *rmsspline=new QSplineFit("Z7Ndl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
      return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
   else if(z==6){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.15939,2.90304,1.64638,1.2764,1.25235,1.39056,1.6632,1.37806,1.01079};//New resolution+New ww
     static QSplineFit *rmsspline=new QSplineFit("Z6Ndl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
      return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
   else if(z==5){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
//     double ynodes[nnodes]={4.66193,3.39864,2.29952,2.1042,2.26165,2.6852,2.67351,1.76083,1.60768};
     double ynodes[nnodes]={4.47505,3.15239,1.7658,1.36109,1.33162,1.48704,1.76671,1.43647,1.02721};//New resolution+New ww
     static QSplineFit *rmsspline=new QSplineFit("Z5dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
  else if(z==4){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,200,500};
     double ynodes[nnodes]={4.33346,3.03151,1.76984,1.41104,1.39958,1.56716,1.92105,1.82492,1.39271};//New resolution+New ww
     static QSplineFit *rmsspline=new QSplineFit("Z4dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
   else if(z==3){
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,200,500};
//     double ynodes[nnodes]={4.77567,3.46033,2.27301,2.06205,2.21091,2.62657,2.72535,2.17512,1.81202};
     double ynodes[nnodes]={4.48267,3.11874,1.81632,1.47904,1.47927,1.71885,2.06968,1.97968,1.51863};//New resolution+New ww
     static QSplineFit *rmsspline=new QSplineFit("Z3dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     return  dl1l9vr/rmsspline->GetFun()->Eval(fabs(rigidity));
   }
   else {
     const int nnodes=9;
     double nodes[nnodes]={1.5,2.5,5,10,20,40,100,300,1000};
     double ynodes[nnodes]={4.32603,3.16601,2.18687,2.02614,2.19003,2.64441,2.72376,2.02467,1.66963};
     static QSplineFit *rmsspline=new QSplineFit("Z6dl1l9vrcorspline",nnodes,nodes,ynodes,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
     double cor2=1;
     if(z==1){//e+/e-
       const int nnodes2=7;
       double nodes2[nnodes2]={0.5, 2.5, 5, 10, 26, 77, 230};
       double ynodes2[nnodes2]={1.12752,0.849797,1.1117,1.23821,1.44226,1.32446,1.30879};
       static QSplineFit *rmsspline2=new QSplineFit("Z1dl1l9vrcorspline2",nnodes2,nodes2,ynodes2,(QSplineFit::LogX2|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
       cor2=rmsspline2->GetFun()->Eval(fabs(rigidity));
     }
     return  dl1l9vr/cor2/rmsspline->GetFun()->Eval(fabs(rigidity));
   } 
}

