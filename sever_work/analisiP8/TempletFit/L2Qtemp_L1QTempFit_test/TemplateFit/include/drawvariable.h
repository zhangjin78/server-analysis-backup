#ifndef DRAWVARIABLE
#define DRAWVARIABLE
#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TObject.h>
#include <TF1.h>
#include <TH1.h>
#include <TH1I.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2S.h>
#include <TH2F.h>
#include <TH3S.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TColor.h>
#include <TGraph.h>
#include <TArrow.h>
#include <TString.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TRandom3.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TClonesArray.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
//#include <TProfile.h>
//#include <TFitResult.h>
//#include <TFitResultPtr.h>
//#include <Fit/FitResult.h>
#include <TChain.h>
#include <TEfficiency.h>
#include "TVirtualFitter.h" //2019.02.19: retrieve confidence interval from fit
#include "TGaxis.h"

//#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <fstream>
#include <iostream>
#include <iomanip> //setw(), etc
#include <algorithm>

/*//--RooFit
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"

#include "RooPlot.h"
#include "RooHist.h"

#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "RooKeysPdf.h"

#include "RooGaussian.h"
#include "RooPolynomial.h"

#include "RooTFnBinding.h" */

//------
/*#include "/home/tim/unige/AMS-02/scripts/include/bins.h"
#include "/home/tim/unige/AMS-02/scripts/include/parameter.h"
#include "/home/tim/unige/AMS-02/scripts/include/methods3.h"
#include "/home/tim/unige/AMS-02/scripts/include/drawline.h"
#include "/home/tim/unige/AMS-02/scripts/include/QSplineFit.C"*/
#include "bins.h"
#include "parameter.h"
#include "methods3.h"
#include "drawline.h"
#include "QSplineFit.C"

//using namespace RooFit;

int iNbin=0;
double *pBins=NULL;

//----histogram
string histn, histn1, histn2, histn3, histn4, histnC;
string histnQDis;
string sHDir;
int iSetDir=1, iNSetDir=3;
//gROOT->cd();

//const int SSpan = 0;
const int SSpan = 1;
const int MSpan = 2;
const int Mimr=2;
//------
//double dRigL=0.8, dRigU=6000.; //2019.03.05: UnfoldBin, not showing the extra [6000,8000] bin
double dRigL=0.8, dRigU=3300.; //2019.05.06: binning for Silicon using secondary bin
double dRigU1[2] = {3000., 6000.};

//----histogram pointer
TH1F *h1F=NULL;
TH1F *hRig=NULL, *hRig1=NULL, *hRig2=NULL;
TH1F *hDiff=NULL;
TH1D *hDRig=NULL, *hDRig1=NULL;
TH2F *hRig2D=NULL;

TGraph *gGraph=NULL;
TGraphErrors *gGrE=NULL, *gGrE1=NULL, *gGrE2=NULL;

//----plot
TCanvas *cCanv=NULL, *cCanv1=NULL, *cCanv2=NULL, *cCanvC=NULL;
TPad *pPad=NULL, *pPad1=NULL, *pPad2=NULL, *pPad3=NULL;
TPad *pPadA[2]={NULL};
TLegend *legend=NULL, *legend1=NULL;
float legend_x1=0.65, legend_y1=0.7, legend_x2=0.9, legend_y2=0.9;

TLatex lTitle, lLegend, lLegend1;
TLine lLine;
TArrow aArrow;
TBox bBox;
TPaveText *pLegend=NULL;

int markerstyle[2] = {4, 20};
//int iMarkerStyle[3] = {3, 4, 20};
int iMarkerStyle[3] = {23, 4, 20}; //2021.03.29
int iMarkerStyle2[9] = {23, 4, 20, 24, 10, 5, 11, 22, 21}; //2021.05.01
int iMarkerStyle3[5] = {23, 4, 20, 24, 10}; //2021.10.25
float ledge=0.95, uedge=1.05;

//2019.03.05
/*TH1F hAxis("axis", "axis", 6000, dRigL, dRigU); //2019.02.01: choose the correct rigidity range as publication
TH1F hAxisEff("axis_eff", "axis_eff", 1000, dRigL, dRigU);
//TH1F hAxisRatio("axis_ratio", "axis_ratio", 1000, dRigL, dRigU);
TH1F hAxisRatio("axis_ratio", "axis_ratio", 1000, dRigL, dRigU); //2019.08.18: plot the ISS/MC efficiency ratio up to 10TV*/ 
//2019.09.04: define hAxis, hAxisEff and hAxisRatio in new_drawAnalysis6 in order to set the binning according to charge
TH1F *hAxis, *hAxisEff, *hAxisRatio;
TH1F *hAxisPull; //2019.09.24
TH1F *hAxisEffCom, *hAxisEffComRatio; //2019.10.16
TH1F *hAxisRatioCom, *hAxisRatioComRatio; //2019.10.16
TH1F *hAxisDevConstant=NULL; //2020.04.28
TH1F *hAxisEffRatioAverage=NULL; //2020.10.13

//TH1F *hAxis = new TH1F("axis", "axis", 74, 2., 6000.);
//TH1F hAxis("axis", "axis", 74, 0.8, 6000.);
//TH1F hAxis("axis", "axis", 1000, 0.8, 3000.); //2019.02.01: choose the correct rigidity range as publication
//TH1F hAxisEff("axis_eff", "axis_eff", 1000, 0.8, 3000.);
//TH1F hAxisRatio("axis_ratio", "axis_ratio", 1000, 0.8, 3000.);

//TH1F hAxis("axis", "axis", 74, 1.92, 6000.);
TH1F hAxisQ("axisQ", "axis charge", 35, 0, 35);

//drawing parameter
double dMargin = 0.5;
//const int iColor[10] = {kBlue, kRed, kGreen+1, kBlack, kMagenta, kYellow+1, kCyan-7, kBlue+1, kRed+1, kGreen+2};
//const int iColor[10] = {kBlue, kRed, kCyan+1, kBlack, kMagenta, kYellow+1, kCyan-7, kBlue+1, kRed+1, kGreen+2};
//const int iColor[10] = {kBlue, kRed, kGreen+3, kBlack, kMagenta, kYellow+1, kCyan-7, kBlue+1, kRed+1, kGreen+2};
const int iColor[10] = {kBlue, kRed, kGreen+3, kBlack, kViolet-2, kYellow+1, kCyan-7, kBlue+1, kRed+1, kGreen+2};
const int iColorLad[13] = {kBlue, kRed, kGreen+3, kBlack, kViolet-2, kYellow+1, kCyan-7, kBlue+1, kRed+1, kGreen+2, kViolet, kCyan, kBlue-3}; //2019.12.13
const int iFillStyle[7] = {3004, 3005, 3006, 3007, 3002};
const int iColorDM[2] = {kBlack, kRed}; //2019.09.08
//const int iColorFlux[3] = {kBlack, kRed, kMagenta}; //2019.09.24
//const int iColorFlux[3] = {kRed, kBlue, kRed}; //2020.01.07
const int iColorFlux[3] = {kGreen+2, kBlue, kRed}; //2021.03.29
const int iColorFlux2[4] = {kRed, kGreen+2, kBlue, kRed}; //2021.03.31
const int iColorFlux3[4] = {kRed, kGreen+2, kBlue, kBlack}; //2021.10.25
//const int iColorCor[8] = {kOrange, kBlue, kMagenta, kRed, kGreen+1, kCyan+1, kBlue+3, kSpring-7}; //2019.10.15
const int iColorCor[8] = {kRed, kBlue, kMagenta, kGreen+1, kOrange, kCyan+1, kBlue+3, kSpring-7}; //2019.11.04
const int iColorSamVar[4] = {kBlack, kRed, kBlue, kGreen+1}; //2019.10.21
//2020.11.10
const int iColorTOI[9] = {kBlue, kRed, kGreen+3, kViolet-2, kYellow+1, kCyan-7, kBlue+1, kRed+1, kGreen+2};

//------Production version
const string sProdVer[6] = {"ADCTHR100", "ADCTHR1000", "ZSEED", "MADCLAD", "PASS6", "ADCTHR1000P7"};
vector<string> vsProdVer;
vector<string>::iterator itVs;

Option_t *DrawOp = "PE1";
Option_t *DrawOpS = "PE1SAME";

Option_t *DrawOp1 = "P";
Option_t *DrawOp1S = "P SAME";

const double dLedgeX = 2.;
const double dUedgeX = 6000.;

//2019.05.21
const double dxp=0.9, dzp=0.8;
const double dy=dxp*dzp/(dxp*dzp+dxp+dzp), dz=dzp/(dxp*dzp+dxp+dzp);

//fit function
TF1 *f1MCunphyFit = NULL;
TF1 *f1FitRe = NULL;

//------name for successive efficiencies
//const string sSuccTk[nSuccTk] = {"#splitline{itrtrack>=0, ibetah>=0, }{tof_btype}", 
//					   "#beta>0.4", 
//					   "inner no. Y hits>=5", 
//					   "#splitline{every inner plane has }{at least one hit}", 
//					   "#chi^{2}_{Y, Inner}<10", 
//					   "Inner Q cut", 
//					   "Inner Qrms cut",
//					   "#splitline{within at least 5 layer's }{fiducial volume}", 
//					   "#splitline{#splitline{within at least 1 layer's }{fiducial volume }}{for each inner plane}"}; 
//2023.02.20: add new successive cut and change the order
const string sSuccTk[nSuccTk] = {	"#splitline{itrtrack>=0, ibetah>=0, }{tof_btype}", 
									"#beta>0.4", 
									"inner no. Y hits>=5", 
									"#splitline{every inner plane has }{at least one hit}", 
									"#chi^{2}_{Y, Inner}<10", 
									"#splitline{within at least 5 layer's }{fiducial volume}", 
									"#splitline{#splitline{within at least 1 layer's }{fiducial volume }}{for each inner plane}"
									"Inner Qrms cut",
									"Inner Q > Z-2.5", //loose cut
									"Inner Q cut for EvCount"};

const string sSuccTkSam[nSuccTkSam] = {	"#splitline{has Trd-matched }{unbiased Beta}", 
							"#splitline{has Beta-matched }{unbiased Trd}", 
							"#splitline{trd Q>2 }{or trd Q not calculated}", 
							"Unbiased #beta>=0.4", 
							"X&Y hits in L1", 
							"L1 Z selection", 
							"fiducial volumn (L1)", 
							"fiducial volumn (Inner)", 
							"L1Q", 
							"L1Q status",
							"tk_l1mds<5",
							"tofZ=l1Z",
							"uTofq consistance",
							"lTofq consistance",
							"tof t&c chis cut",
							"Rigidity estimator cut",
							"#splitline{geomagnetic cutoff}{(except for cutoff esti)}",
							"fiducial volumn (L9)",
							"X&Y hits in L9",
							"L9 Z selection"};
							
const string sL1Eff[2] = {"BZ", "PU"};
const string sL1EffD[2] = {"BZ", "pick up"};
const string sSuccL1BZ[nSuccL1BZ] = {	"Unbiased L1XY", 
							"Unbiased L1 Q", 
							"Unbiased L1 Q status"};
/*const string sSuccL1PU[nSuccL1PU] = {	"L1XY",  
							"L1InnerChisY<10",
							"L1 Q",
							"L1 Q status"};*/
const string sSuccL1PU[nSuccL1PUT] = {	"L1XY",  
							"L1InnerNormChisY<10",
							"L1 Q",
							"L1 Q status",
							"L1ChisY<10"}; //2019.09.09
const string sSuccL9[nSuccL9] = {	"L9XY", 
						"L1InnerL9NormChisY<10",
						"L9Q"};

const string sUnix[7] = {	"1305853512-1337389512",
					"1337389512-1368925512",
					"1368925512-1400461512",
					"1400461512-1431997512",
					"1431997512-1463533512",
					"1463533512-1495069512",
					"1495069512-1526605512"};

const string sDate[7] = {	"05.20.2011 - 05.19.2012",
					"05.19.2012 - 05.19.2013",
					"05.19.2013 - 05.19.2014",
					"05.19.2014 - 05.19.2015",
					"05.19.2015 - 05.18.2016",
					"05.18.2016 - 05.18.2017",
					"05.18.2017 - 05.18.2018"};
const string sDate2[8] = {	"05.20.2011 - 05.19.2012",
					"05.19.2012 - 05.19.2013",
					"05.19.2013 - 05.19.2014",
					"05.19.2014 - 05.19.2015",
					"05.19.2015 - 02.25.2016(old trig)",
					"02.27.2016 - 05.18.2016(new trig)",
					"05.18.2016 - 05.18.2017",
					"05.18.2017 - 05.18.2018"};

//2019.03.26
const string sQVer[3] = {"Old Q", "YJ's Q", "HL's Q"};

//2019.04.26
const int iMarkerStyleQ[2][2] = {{20,4},{21,25}};

//2020.04.03
const string sWdir = "/mnt/d/Ubuntu/unige/AMS-02";
const string sResultDir = sWdir+"/online_result/MyProduction/";

//------confidence level of efficiency ratio fit //2021.09.24
const double dCI=0.68;
//const double dCI=0.95;
#endif //DRAWVARIABLE

