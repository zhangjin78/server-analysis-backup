#ifndef HISTOMAN_C
#define HISTOMAN_C
//-----------------
//updated from: ChargeCal2_noAmsSoft.C
//updated date: 2018.01.26
//-----------------
//not using AMS Software
//-----------------
//2018.01.26
//	1) keep the definition of GetHist
//
//2018.12.04
//	1) add new function, GetTEff, to retrieve TEfficiency from TObjArray
//-----------------

//#define AMSSOFT

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TColor.h>
#include <TGraph.h>
#include <TString.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TRandom3.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TProfile.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <Fit/FitResult.h>
#include <TChain.h>
#include <TEfficiency.h>

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef AMSSOFT
#include "root_RVSP.h"
#include "HistoMan.h"
#endif

using std::string;

#ifdef AMSSOFT
TH1* GetHist(HistoMan &hman1, string name) {return static_cast<TH1*>(hman1.Get(name.c_str()));}
#else
TH1* GetHist(TObjArray &hman1, string name) {return static_cast<TH1*>(hman1.FindObject(name.c_str()));}
TH2* GetHist2d(TObjArray &hman1, string name) {return static_cast<TH2*>(hman1.FindObject(name.c_str()));} //2019.05.12: return 2d plots
TEfficiency* GetTEff(TObjArray &hman1, string name) {return static_cast<TEfficiency*>(hman1.FindObject(name.c_str()));}
#endif //AMSSOFT
#endif //HISTOMAN_C
