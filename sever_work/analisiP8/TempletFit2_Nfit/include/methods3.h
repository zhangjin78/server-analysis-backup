#ifndef METHODS3_H
#define METHODS3_H
//----------
//updated from: methods2.h
//updated date: 2017.10.06
//difference: inserting string instead of c string (char)
//----------
//2017.10.06
//	1) use std::string as input instead of char*
//	2) remove BookCanv & BookCanvN
//
//2017.10.29
//	1) create a overload function for BookHistoN that use TClonesArray& instead of TClonesArray* as parameter
//
//2018.03.29
//	1) add new function to transform a delta 1/R vs R_M to delta R vs R_M using formula in work log - ResoCal
//	2) add new function, which sets the bin content for each bin of the TH1 as its bin width - DrawBinWidth
//
//2018.05.03
//	1) add new parameter, dir, in SlicesFit to control whether store the fit result in current directory
//
//2018.05.04
//	1) add new function, ErrorCalWeightMean, to calculate the error of weighted mean
//
//2018.06.07
//	1) add new function, GetEstimation, to calculate the estimation of certain distribution and associated RMS. opt=0: weighted mean, 
//	   opt=1: Most Probable Value (i.e. the low edge of bin with biggest bin content)
//
//2018.10.23
//	1) add new function, CalAbs, to calculate the correct abscissa for given rigidity bin assuming R^(-2.7)
//
//2019.01.22
//	1) add new function, FluxScale, to multiply the Flux by certain power of rigidity
//
//2019.02.19
//	1) comment out function LogY, which have the same name "LogY" for enum in splineFit.h and the function itself seems useless
//
//2019.08.30
//	1) add new function, SetCanvRatio, to set canvas and pads ploting 1) two Histograms or Graphs 2) ratio of two Histograms or Graph
//	2) add new function, PlotHistoRatio, to plot 1) two Histograms 2) ratio of two Histograms
//
//2019.11.01
//	1) add new function, CalGraphRatio, to calculate the ratio between two TGraph automatically match the points //XXX
//
//2020.01.28
//	1) add new function, IntSeparator, which return a string of integer with separator for every 3 digit
//
//2020.03.26
//	1) update the way to print event no. with digital separation to correctly account for the case that having 0 in between
//
//2020.04.04
//	1) add new function, CalPowError, for calculating the error after power law
//
//2020.12.23
//	1) add new function, CalGamma, to calculate the gamma factor from beta
//	2) add new function, CalMass, to get the mass with given charge, beta and Rig
//	3) add new function, BetaToRig, to get the Rigidity from charge, mass and Beta
//	4) add new function, RigToBeta, to get the Beta from charge, mass and Rigidity
//
//2021.01.12
//	1) add new funciton, GetISOMass, to get the isotope mass for given charge - //2021.04.01: move to method_simple
//----------

#include <vector>
#include <TClonesArray.h>
#define COMAPRERESULT

#include "bisection.h"

#include "parameter.h" //2020.12.24
#include "methods_simple.h" //2021.03.23

const float fX1=0.65, fY1=0.7, fX2=0.9, fY2=0.9;

//------draw difference
TH1F* DrawDiffA(TH1F *hDiff, TH1F *h1, TH1F *h2);
TH1* DrawDiffR(TH1 *hDiff, TH1 *h1, TH1 *h2, TH1 *h3, int opt=1);
//TH1F* DrawDiffR(TH1F *hDiff, TH1F *h1, TH1F *h2, TH1F *h3, int opt=1);

//------draw
void DrawPlotN1(TCanvas *c1, TH1F *EN, TH1F *ENw, TH1F *ENd);
void DrawPlotC(TCanvas *c2, TH1F *ENw, TH1F *ENc, TH1F *ENd, TH1F *ratew, TH1F *ratec, TH1F *rated, TString sName);
void DrawPlots(TCanvas *c1, TCanvas *c2, TH1F *EN, TH1F *ENw, TH1F *ENc, TH1F *ENd, TH1F *rate, TH1F *ratew, TH1F *ratec, TH1F *rated, TString sName);

//------rate calculation
//int RateCal(TH1F *rate, TH1F *EN, TH1F *ExpT);
void RateCal(TH1F *rate, TH1F *EN, TH1F *ExpT, bool BZbin=false);

//------booking histogram & Canvas
//int BookHisto(TClonesArray *ca_rig, TClonesArray *ca_rate, TClonesArray *ca_rigd, TClonesArray *ca_rated, string a, int SetDir=1, 
//int binopt=0, bool explicit_name=true);
void BookHisto(TClonesArray *ca_rig, TClonesArray *ca_rate, TClonesArray *ca_rigd, TClonesArray *ca_rated, TString a, int SetDir=1, 
int binopt=0, bool explicit_name=true);

int BookHistoN(TClonesArray *ca, string name, int SetDir=1, int dim=1, int nbinx=-1, vector<double> *binx=NULL, int nbiny=-1, vector<double> *biny=NULL, bool explicit_name=true); //use pointer as parameter
int BookHistoN(TClonesArray &ca, string name, int SetDir=1, int dim=1, int nbinx=-1, vector<double> *binx=NULL, int nbiny=-1, vector<double> *biny=NULL, bool explicit_name=true); //use address as parameter
int BookHistoN2(TClonesArray *ca, string name, int binopt=0, int SetDir=1, int dim=1, int nbinx=-1, vector<double> *binx=NULL, int nbiny=-1, vector<double> *biny=NULL, bool explicit_name=true);   //binopt=0: old C,O binning-nuclei_bins, =1: LiBeB binning-nuclei_bins2, =2: BZ binnig-nuclei_bins_BZ

//2019.08.30
void SetCanvRatio(TCanvas *canv, TPad *p1, TPad *p2, const double dPadRatio=.3, const double dPadLeft=0., const double dPadRight=1.);
void PlotHistoRatio();


//------other function
void ErrorCal(TH1F *h, TH1F *hN, TH1F *hD, int opt=0, double scaling=100.); //opt=0: binominal error; 1: error propagation for TrigEff, hN=phys, hD=unphys
double ErrorCalWeightMean(TH1 &h, int opt=0); //h: the histogram to calculated weight mean error, opt=0: weight mean (leave the possiblity for further implementation
bool GetEstimation(TH1 *h, double &est, double &error, int opt=0, int fbin=1, int lbin=-1); //opt=0: weighted mean, opt=1: Most Probable Value

void LogPolN(std::string *fname, int n, int opt=0); //n: rank, opt=0:higher power in front; =1: lower power in front
//void LogY(std::string *fname);
double CalXlw(double xl, double xu, double a=2.7); //a: spectrum index
double CalPulls(double x, double ex, double x0, double ex0);
//int SlicesFit(TH2F* h2, TH1F* hMean, TH1F* hSigma, int opt=0, int nfit=1); //opt=0:fit iteratively, 1:double gaussian fit; nfit:no. of fits apply
//int SlicesFit(TH2F* h2, TObjArray &oaMean, TObjArray &oaSigma, int opt=0, int nfit=1); //opt=0:fit iteratively, 1:double gaussian fit; nfit:no. of fits apply
int SlicesFit(TH2F* h2, TObjArray &oaMean, TObjArray &oaSigma, int opt=0, int nfit=1, bool dir=false); //opt=0:fit iteratively, 1:double gaussian fit; nfit:no. of fits apply

//----------rigidity resolution calculation
TH1F* ResoCal(TH1F &hRR);
bool DrawBinWidth(TH1F &h);

////----------calculate correct abscissa for given rigidity bin //2021.03.23: move into methods_simple.h
//double CalAbs(const double lbin, const double ubin, const double index=-2.7);

//----------
void FluxScale(TGraphErrors &geFlux, const double index=-2.7);

//------------------------
//------draw difference
TH1F* DrawDiffA(TH1F *hDiff, TH1F *h1, TH1F *h2) {
	if (hDiff!=NULL) hDiff->Add(h1, h2, 1, -1); //hDiff not empty
	else { //hDiff empty, first clone it as h1
		hDiff = static_cast<TH1F*>(h1->Clone());
		hDiff->Add(h2, -1);
	}
	return hDiff;
}

TH1* DrawDiffR(TH1 *hDiff, TH1 *h1, TH1 *h2, TH1 *h3, int opt) { //opt=1: percentage scale
//TH1F* DrawDiffR(TH1F *hDiff, TH1F *h1, TH1F *h2, TH1F *h3, int opt) { //opt=1: percentage scale
	if (h2==NULL) { //only the ratio, no difference
		if (hDiff!=NULL) hDiff->Add(h1,1); //hDiff not empty
		else { //hDiff empty, first clone it as h1
			hDiff = static_cast<TH1F*>(h1->Clone());
		}
	}
	else {
		if (hDiff!=NULL) hDiff->Add(h1, h2, 1, -1); //hDiff not empty
		else { //hDiff empty, first clone it as h1
			hDiff = static_cast<TH1F*>(h1->Clone());
			hDiff->Add(h2, -1);
		}
	}
	hDiff->Divide(h3);
	if (opt==1) {
		hDiff->Scale(100);
		hDiff->GetYaxis()->SetTitle("difference/%");
		hDiff->GetYaxis()->CenterTitle();
	}
	for (int i=0; i<hDiff->GetNbinsX(); i++) hDiff->SetBinError(i+1,0.0001);
	return hDiff;
}

//------draw

void DrawPlotN1(TCanvas *c1, TH1F *EN, TH1F *ENw, TH1F *ENd) {
	TLegend *legend = NULL;

	//c1
	c1->cd(1)->SetLogx();
	gPad->SetLogy();
	gPad->SetGridy();
	
	EN->GetYaxis()->SetRangeUser(1, 1e6);
	EN->GetXaxis()->SetRangeUser(1.33, 6000);
	EN->GetXaxis()->SetTitle("Rigidity [GV]");
	EN->GetXaxis()->CenterTitle();
	EN->DrawCopy();
	
	legend = new TLegend(fX1-0.2, fY1, fX2, fY2);
	legend->AddEntry(EN, "N_{event count}^{N}");
	legend->SetMargin(0.5);
	legend->Draw();
	
	c1->cd(2)->SetLogx();
	gPad->SetLogy();
	gPad->SetGridy();
	
	ENw->SetLineColor(kBlack);
	ENw->GetYaxis()->SetRangeUser(1, 1e6);
	ENw->GetXaxis()->SetRangeUser(1.33, 6000);
	ENw->GetXaxis()->SetTitle("Rigidity [GV]");
	ENw->GetXaxis()->CenterTitle();
	ENw->DrawCopy();
	
	legend = new TLegend(fX1-0.2, fY1, fX2, fY2);
	legend->AddEntry(ENw, "N_{event count}^{N-1}");
	legend->SetMargin(0.5);
	legend->Draw();
	
	c1->cd(3)->SetLogx();
	gPad->SetGrid();
	DrawDiffR(ENd, ENw, EN, ENw);
	ENd->GetXaxis()->SetRangeUser(1.33, 6000);
	ENd->GetXaxis()->SetTitle("Rigidity [GV]");
	ENd->GetXaxis()->CenterTitle();
	ENd->DrawCopy();
	
}

void DrawPlotC(TCanvas *c2, TH1F *ENw, TH1F *ENc, TH1F *ENd, TH1F *ratew, TH1F *ratec, TH1F *rated, TString sName) {
	
	TLegend *legend = NULL;
	
	c2->cd(1)->SetLogx();
	gPad->SetLogy();
	
	ratew->SetMarkerStyle(21);
	ratew->SetMarkerColor(kRed);
	ratew->GetXaxis()->SetRangeUser(1.33, 6000);
	ratew->GetYaxis()->SetRangeUser(1e-10, 1);
	ratew->GetXaxis()->SetTitle("Rigidity [GV]");
	ratew->GetXaxis()->CenterTitle();
	ratew->DrawCopy("P0");
	
	ratec->SetMarkerColor(kGreen);
	ratec->SetMarkerStyle(21);
	ratec->GetXaxis()->SetTitle("Rigidity [GV]");
	ratec->GetXaxis()->CenterTitle();
	ratec->DrawCopy("SAMEP0");
	
	legend = new TLegend(fX1-0.2, fY1, fX2, fY2);
	legend->AddEntry(ratew, "rate", "P");
	legend->AddEntry(ratec, sName, "P");
	legend->SetMargin(0.5);
	legend->Draw();
	
	c2->cd(2)->SetLogx();
	gPad->SetGrid();
//	DrawDiffR(rated, ratew, ratec, ratew);
	DrawDiffR(rated, ratew, ratec, ratec);
	rated->GetXaxis()->SetRangeUser(1.33, 6000);
	rated->GetXaxis()->SetTitle("Rigidity [GV]");
	rated->GetXaxis()->CenterTitle();
	rated->DrawCopy();
	
	c2->cd(3)->SetLogx();
	gPad->SetLogy();
	gPad->SetGridy();
	
	ENw->GetXaxis()->SetTitle("Rigidity [GV]");
	ENw->GetXaxis()->CenterTitle();
	ENw->DrawCopy();
	ENc->SetLineColor(kGreen);
	ENc->GetYaxis()->SetRangeUser(1, 1e6);
	ENc->GetXaxis()->SetRangeUser(1.33, 6000);
	ENc->GetXaxis()->SetTitle("Rigidity [GV]");
	ENc->GetXaxis()->CenterTitle();
	ENc->DrawCopy("SAME");
	
	legend = new TLegend(fX1-0.2, fY1, fX2, fY2);
	legend->AddEntry(ENw, "Event Count");
	legend->AddEntry(ENc, sName);
	legend->SetMargin(0.5);
	legend->Draw();
	
	c2->cd(4)->SetLogx();
	gPad->SetGrid();
	DrawDiffR(ENd, ENw, ENc, ENc);
//	DrawDiffR(ENd, ENw, ENc, ENw);
	ENd->GetXaxis()->SetRangeUser(1.33, 6000);
	ENd->GetXaxis()->SetTitle("Rigidity [GV]");
	ENd->GetXaxis()->CenterTitle();
	ENd->DrawCopy();
	
}

void DrawPlots(TCanvas *c1, TCanvas *c2, TH1F *EN, TH1F *ENw, TH1F *ENc, TH1F *ENd, TH1F *rate, TH1F *ratew, TH1F *ratec, TH1F *rated, TString sName) 
{
	//c1
	DrawPlotN1(c1, EN, ENw, ENd);
	
	#ifdef COMAPRERESULT
	//c2
	DrawPlotC(c2, ENw, ENc, ENd, ratew, ratec, rated, sName);
	#endif
}

//rate calculation
//int RateCal(TH1F *rate, TH1F *EN, TH1F *ExpT) {
//	rate->Divide(EN, ExpT);
//	for (int i=0; i<nuclei_nbin; i++) {
////		if(rate->GetXaxis()->GetBinCenterLog(i+1)<1.9) {
////			rate->SetBinContent(i+1, 0);
////			continue;
////		}
//		if (i<3) {
//			rate->SetBinContent(i+1, 0);
//			continue;
//		}
//		rate->SetBinContent(i+1, rate->GetBinContent(i+1)/rate->GetXaxis()->GetBinWidth(i+1));
//		
//	}
//}

void RateCal(TH1F *rate, TH1F *EN, TH1F *ExpT, bool BZbin) {
	if (BZbin) rate->Divide(EN, (TH1F*)ExpT->Rebin(nuclei_nbin, "", nuclei_bins_BZ)); //using BZ binning
	else rate->Divide(EN, ExpT);
	for (int i=0; i<nuclei_nbin; i++) {
//		if(rate->GetXaxis()->GetBinCenterLog(i+1)<1.9) {
//			rate->SetBinContent(i+1, 0);
//			continue;
//		}
		if (i<3) {
			rate->SetBinContent(i+1, 0);
			continue;
		}
		rate->SetBinContent(i+1, rate->GetBinContent(i+1)/rate->GetXaxis()->GetBinWidth(i+1));
		
	}
}

//------booking histogram & Canvas
//int BookHisto(TClonesArray *ca_rig, TClonesArray *ca_rate, TClonesArray *ca_rigd, TClonesArray *ca_rated, string a, int SetDir, 
//              int binopt, bool explicit_name)
//{
//	TString tstring_a(a);
//	BookHisto(ca_rig, ca_rate, ca_rigd, ca_rated, tstring_a, SetDir, binopt, explicit_name);
//}

void BookHisto(TClonesArray *ca_rig, TClonesArray *ca_rate, TClonesArray *ca_rigd, TClonesArray *ca_rated, TString a, int SetDir, 
              int binopt, bool explicit_name)
{
	int iNbin = nuclei_nbin;
	double *pBins = nuclei_bins;
	if (binopt==1) {
		iNbin = nuclei_nbin2;
		pBins = nuclei_bins2;
	}
	else if (binopt==2) {
		pBins = nuclei_bins_BZ;
	}
	int pos = ca_rig->GetLast()+1;
	TString a_modified(a);
	TH1F *rigs 	= (TH1F*)ca_rig->ConstructedAt(pos);
	rigs->SetBins(iNbin, pBins);
	rigs->GetXaxis()->SetTitle("Rigidity [GV]");
	rigs->GetXaxis()->CenterTitle();
	rigs->GetXaxis()->SetTitleOffset(1.2);
	rigs->GetYaxis()->SetTitle("Number of events");
	rigs->GetYaxis()->CenterTitle();
	if(SetDir&(1<<0)) rigs->SetDirectory(gDirectory);
	
	TH1F *rates	= (TH1F*)ca_rate->ConstructedAt(pos);
	rates->SetBins(iNbin, pBins);
	rates->GetXaxis()->SetTitle("Rigidity [GV]");
	rates->GetXaxis()->CenterTitle();
	rates->GetXaxis()->SetTitleOffset(1.2);
	rates->GetYaxis()->SetTitle("rate [s^{-1} GV^{-1}]");
	rates->GetYaxis()->CenterTitle();
	if(SetDir&(1<<1)) rates->SetDirectory(gDirectory);
	
	TH1F *rigd = (TH1F*)ca_rigd->ConstructedAt(pos); //rig difference
	rigd->SetBins(iNbin, pBins); 
	rigd->GetXaxis()->SetTitle("Rigidity [GV]");
	rigd->GetXaxis()->CenterTitle();
	rigd->GetXaxis()->SetTitleOffset(1.2);
	rigd->GetYaxis()->SetTitle("difference %");
	rigd->GetYaxis()->CenterTitle();
	if(SetDir&(1<<2)) rigd->SetDirectory(gDirectory);
	
	TH1F *rated = (TH1F*)ca_rated->ConstructedAt(pos); //rate difference
	rated->SetBins(iNbin, pBins); 
	rated->GetXaxis()->SetTitle("Rigidity [GV]");
	rated->GetXaxis()->CenterTitle();
	rated->GetXaxis()->SetTitleOffset(1.2);
	rated->GetYaxis()->SetTitle("difference %");
	rated->GetYaxis()->CenterTitle();
	if(SetDir&(1<<3)) rated->SetDirectory(gDirectory);
	
	if (explicit_name) {
		rigs->SetName("rig_"+a);
		rates->SetName("rate_"+a);
		rigd->SetName("rigd_"+a);
		rated->SetName("rated_"+a);
		if (a.Contains("r1")) {
			a_modified.Replace(a_modified.Index("r1"), 2, Form("%s", cSpan[1]));
			rigs->SetTitle("rig_"+a_modified);
			rates->SetTitle("rate_"+a_modified);
			rigd->SetTitle("rigd_"+a_modified);
			rated->SetTitle("rated_"+a_modified);
		}
		else if (a.Contains("r2")) {
			a_modified.Replace(a_modified.Index("r2"), 2, Form("%s", cSpan[2]));
			rigs->SetTitle("rig_"+a_modified);
			rates->SetTitle("rate_"+a_modified);
			rigd->SetTitle("rigd_"+a_modified);
			rated->SetTitle("rated_"+a_modified);
		}
	}
	else {
		rigs->SetNameTitle("rig_"+a, "rig_"+a);
		rates->SetNameTitle("rate_"+a, "rate_"+a);
		rigd->SetNameTitle("rigd_"+a, "rigd_"+a);
		rated->SetNameTitle("rated_"+a, "rated_"+a);
	}
}

int BookHistoN(TClonesArray *ca, string name, int SetDir, int dim, int nbinx, vector<double> *binx, int nbiny, vector<double> *biny, bool explicit_name) {
	TString name_modified(name.c_str());
	int pos = ca->GetLast()+1;
	bool bFault=false;
	TH1 *histo = NULL;
	
	if (dim==1) histo	= (TH1F*)ca->ConstructedAt(pos);
	else if (dim==2) histo	= (TH2F*)ca->ConstructedAt(pos);
	
	histo->SetName(name.c_str());
	if (explicit_name) {
		if (name_modified.Contains("r1")) {
			name_modified.Replace(name_modified.Index("r1"), 2, Form("%s", cSpan[1]));
			histo->SetTitle(name_modified);
		}
		else if (name_modified.Contains("r2")) {
			name_modified.Replace(name_modified.Index("r2"), 2, Form("%s", cSpan[2]));
			histo->SetTitle(name_modified);
		}
	}
	else {
		histo->SetTitle(name.c_str());
	}
	if (dim==1) {
		if (nbinx==-1) histo->SetBins(nuclei_nbin, nuclei_bins);
		else if (binx->size()<2) bFault=true;
		else if (binx->size()==2) histo->SetBins(nbinx, binx->at(0), binx->at(1));
		else if (binx->size()>2) histo->SetBins(nbinx, binx->data());
	}
	else if (dim==2) {
		if (nbinx==-1 && nbiny==-1) histo->SetBins(nuclei_nbin, nuclei_bins, nuclei_nbin, nuclei_bins); //both use flux binning
		else if (nbinx==-1 && nbiny!=-1) { //y use flux binning
			if (biny->size()<2) bFault=true;
//			else if (biny->size()==2) histo->SetBins(nuclei_nbin, nuclei_bins, nbiny, biny->at(0), biny->at(1));
			else if (biny->size()>2) histo->SetBins(nuclei_nbin, nuclei_bins, nbiny, biny->data());
		}
		else if (nbinx!=-1 && nbiny==-1) { //x use flux binning
			if (binx->size()<2) bFault=true;
//			else if (binx->size()==2) histo->SetBins(nbinx, binx->at(0), binx->at(1), nuclei_nbin, nuclei_bins);
			else if (binx->size()>2) histo->SetBins(nbinx, binx->data(), nuclei_nbin, nuclei_bins);
		}
		else if (nbinx!=-1 && nbiny!=-1) { //both not using flux binning
			if (binx->size()<2 || biny->size()<2) bFault=true;
//			else if (binx->size()==2 && biny->size()==2) 
//				histo->SetBins(nbinx, binx->at(0), binx->at(1), nbiny, biny->at(0), biny->at(1));
//			else if (binx->size()>2 && biny->size()==2) 
//				histo->SetBins(nbinx, binx->data(), nbiny, biny->at(0), biny->at(1));
//			else if (binx->size()==2 && biny->size()>2) 
//				histo->SetBins(nbinx, binx->at(0), binx->at(1), nbiny, biny->data());
			else if (binx->size()>2 && biny->size()>2) 
				histo->SetBins(nbinx, binx->data(), nbiny, biny->data());
		}
	}
	
	if (bFault) {
		cout << "insert wrong binnings, end booking" << endl;
		return -1;
	}
	if(SetDir&(1<<0)) histo->SetDirectory(gDirectory);
	histo->GetXaxis()->SetTitleOffset(1.2);
	
	return 0;
}

int BookHistoN(TClonesArray &ca, string name, int SetDir, int dim, int nbinx, vector<double> *binx, int nbiny, vector<double> *biny, bool explicit_name) {
	TClonesArray *pca = &ca;
	return BookHistoN(pca, name, SetDir, dim, nbinx, binx, nbiny, biny, explicit_name);
}

int BookHistoN2(TClonesArray *ca, string name, int binopt, int SetDir, int dim, int nbinx, vector<double> *binx, int nbiny, vector<double> *biny, bool explicit_name) {
	TString name_modified(name.c_str());
	int pos = ca->GetLast()+1;
	bool bFault=false;
	TH1 *histo = NULL;
	
	int iNBin = 0;
	double *pBins = NULL;
	
	if (binopt==0) { //old C,O binning
		iNBin=nuclei_nbin;
		pBins=nuclei_bins;
	}
	else if (binopt==1) { //LiBeB binning
		iNBin=nuclei_nbin2;
		pBins=nuclei_bins2;
	}
	else if (binopt==2) { //BZ binning
		iNBin=nuclei_nbin;
		pBins=nuclei_bins_BZ;
	}
	
	if (dim==1) histo	= (TH1F*)ca->ConstructedAt(pos);
	else if (dim==2) histo	= (TH2F*)ca->ConstructedAt(pos);
	
	histo->SetName(name.c_str());
	if (explicit_name) {
		if (name_modified.Contains("r1")) {
			name_modified.Replace(name_modified.Index("r1"), 2, Form("%s", cSpan[1]));
			histo->SetTitle(name_modified);
		}
		else if (name_modified.Contains("r2")) {
			name_modified.Replace(name_modified.Index("r2"), 2, Form("%s", cSpan[2]));
			histo->SetTitle(name_modified);
		}
	}
	else {
		histo->SetTitle(name.c_str());
	}
	if (dim==1) {
		if (nbinx==-1) histo->SetBins(iNBin, pBins);
		else if (binx->size()<2) bFault=true;
		else if (binx->size()==2) histo->SetBins(nbinx, binx->at(0), binx->at(1));
		else if (binx->size()>2) histo->SetBins(nbinx, binx->data());
	}
	else if (dim==2) {
		if (nbinx==-1 && nbiny==-1) histo->SetBins(iNBin, pBins, iNBin, pBins); //both use flux binning
		else if (nbinx==-1 && nbiny!=-1) { //y use flux binning
			if (biny->size()<2) bFault=true;
			else if (biny->size()>2) histo->SetBins(iNBin, pBins, nbiny, biny->data());
		}
		else if (nbinx!=-1 && nbiny==-1) { //x use flux binning
			if (binx->size()<2) bFault=true;
			else if (binx->size()>2) histo->SetBins(nbinx, binx->data(), iNBin, pBins);
		}
		else if (nbinx!=-1 && nbiny!=-1) { //both not using flux binning
			if (binx->size()<2 || biny->size()<2) bFault=true;
			else if (binx->size()>2 && biny->size()>2) 
				histo->SetBins(nbinx, binx->data(), nbiny, biny->data());
		}
	}
	
	if (bFault) {
		cout << "insert wrong binnings, end booking" << endl;
		return -1;
	}
	if(SetDir&(1<<0)) histo->SetDirectory(gDirectory);
	histo->GetXaxis()->SetTitleOffset(1.2);
	
	return 0;
}

//2019.08.30
void SetCanvRatio(TCanvas *canv, TPad *p1, TPad *p2, const double dPadRatio, const double dPadLeft, const double dPadRight)
{
	if (canv==NULL) {cout << "canv not exist!!!! EXIT" << endl; return;}
	if (p1==NULL) {cout << "p1 not exist!!!! EXIT" << endl; return;}
	if (p2==NULL) {cout << "p2 not exist!!!! EXIT" << endl; return;}
	
	cout << "canv " << canv << endl; //XXX
	cout << "p1 " << p1 << endl; //XXX
	cout << "p1->SetPad(" << dPadLeft << ", " << dPadRatio << ", " << dPadRight << ", 1.)" << endl; //XXX
	p1->SetPad(dPadLeft, dPadRatio, dPadRight, 1.);
	p1->cd();
	gPad->SetLogx();
	gPad->SetGrid();
	gPad->SetBottomMargin(0);
	gPad->SetRightMargin(0.05);
	
	cout << "p2 " << p2 << endl; //XXX
	cout << "p2->SetPad(" << dPadLeft << ", .0, " << dPadRight << ", " << dPadRatio << ")" << endl; //XXX
	p2->SetPad(dPadLeft, .0, dPadRight, dPadRatio);
	cout << "OK" << endl; //XXX
	p2->cd();
	gPad->SetLogx();
	gPad->SetGrid();
	gPad->SetTopMargin(0);
	gPad->SetBottomMargin(0.2);
	gPad->SetRightMargin(0.05);
	
	canv->cd();
	p1->Draw();
	p2->Draw();
}

//------other function
void ErrorCal(TH1F *h, TH1F *hN, TH1F *hD, int opt, double scaling) {
	double dEff=0;
	double dNhN=0., dNhD=0.;
	double dEhN=0., dEhD=0.;
	
	if (opt==0) { //other efficiency -- binomial
		for (int ibin=1; ibin<h->GetNbinsX()+1; ibin++) {
			if (hD->GetBinContent(ibin)==0) continue;
			dEff = h->GetBinContent(ibin);
			dEff = sqrt( dEff*(1-dEff)/(hD->GetBinContent(ibin)) );
			h->SetBinError(ibin, dEff);
		}
	}
	else if (opt==1) { //Trigger efficiency -- error propagation 
		for (int ibin=1; ibin<h->GetNbinsX()+1; ibin++) {
			if (hD->GetBinContent(ibin)==0) {
				h->SetBinError(ibin, 0);
//				h->SetBinError(ibin, hN->GetBinError(ibin));
				continue;
			}
			dNhN = hN->GetBinContent(ibin);
			dNhD = hD->GetBinContent(ibin);
			dEff = h->GetBinContent(ibin);
			dEff = pow(scaling, 2)*pow(dEff, 4)/pow(dNhN, 2)*(pow(dNhD, 2)/dNhN + dNhD);
			dEff = sqrt(dEff);
			h->SetBinError(ibin, dEff);
		}
	}
	else if (opt==2) { //data-MC ratio -- error propagation
		for (int ibin=1; ibin<h->GetNbinsX()+1; ibin++) {
			if (hD->GetBinContent(ibin)==0) continue;
			dNhN = hN->GetBinContent(ibin);
			dEhN = hN->GetBinError(ibin);
			dNhD = hD->GetBinContent(ibin);
			dEhD = hD->GetBinError(ibin);
			
			dEff = h->GetBinContent(ibin);
			dEff = pow(dEff, 2)*(pow(dEhN, 2)/pow(dNhN, 2)+pow(dEhD, 2)/pow(dNhD, 2));
			dEff = sqrt(dEff);
			h->SetBinError(ibin, dEff);
		}
	}
}

double ErrorCalWeightMean(TH1 &h, int opt)
{
	double err=0., erri=0., eNi=0., eRi=0.;
	double Ni=0, Nj=0., Ri=0, Rj=0;
	const double total = h.Integral(1, h.GetNbinsX());
	if (total==0) {
		cout << "empty histogram, err=0" << endl;
		return err;
	}
	for (int ibini=1; ibini<h.GetNbinsX(); ibini++) {
		Ni = h.GetBinContent(ibini);
		if (Ni==0) continue;
		eNi = h.GetBinError(ibini);
		Ri = h.GetBinCenter(ibini);
		eRi = h.GetBinWidth(ibini)/2.;
		erri=0;
		for (int ibinj=1; ibinj<h.GetNbinsX(); ibinj++) {
			if (ibini==ibinj) continue;
			Nj = h.GetBinContent(ibinj);
			Rj = h.GetBinCenter(ibinj);
			erri += Nj*(Ri-Rj);
		} //ibinj
		erri = pow(erri*eNi/total, 2)+pow(Ni*eRi, 2);
		err += erri;
	} //ibini
	err = sqrt(err)/total;
	return err;
}

bool GetEstimation(TH1 *h, double &est, double &error, int opt, int fbin, int lbin)
{
	double nom=0., denom=0.;
	int Nbin=(lbin==-1)?h->GetNbinsX():lbin;
	if (opt==0)
	{
		nom=0., denom=0.;
		for (int ibin=fbin; ibin<Nbin; ibin++)
		{
			double bincon = h->GetBinContent(ibin);
			nom += bincon*h->GetBinCenter(ibin);
			denom += bincon;
		} //ibin
		if (denom==0) return false;
		est = nom/denom;
		error=0;
		for (int ibin=fbin; ibin<Nbin; ibin++)
		{
			error += h->GetBinContent(ibin) * pow(est - h->GetBinCenter(ibin), 2);
		} //ibin
		error = sqrt(error/denom);
	}
	else if (opt==1)
	{
		h->GetXaxis()->SetRange(fbin, Nbin);
		est = h->GetBinLowEdge(h->GetMaximumBin());
		error=0, denom=0.;
		for (int ibin=fbin; ibin<Nbin; ibin++)
		{
			error += h->GetBinContent(ibin) * pow(est - h->GetBinCenter(ibin), 2);
			denom += h->GetBinContent(ibin);
		} //ibin
		if (denom==0) return false;
		error = sqrt(error/denom);
	}
	return true;
}

void LogPolN(std::string *fname, int n, int opt)
{
	if(!fname->empty()) fname->clear();
	for (int i=0; i<=n; i++) {
		if (opt==0) fname->append(Form("[%d]*(log(x))^%d", i, n-i));
		else if (opt==1) fname->append(Form("[%d]*(log(x))^%d", i, i));
		if (i!=n) fname->append("+");
	}
}

//void LogY(std::string *fname) {
//	fname->insert(0, "exp(");
//	fname->append(")");
//}

double CalXlw(double xl, double xu, double a) {
	double xlw = (xu-xl)*(-a+1) / (pow(xu, -a+1) - pow(xl, -a+1));
	xlw = pow(xlw, 1/a);
	return xlw;
}

double CalPulls(double x, double ex, double x0, double ex0) {
//	return ( (x-x0)/sqrt( abs( pow(ex0, 2) - pow(ex, 2) ) ) );
//	return ( (x-x0)/sqrt( abs( pow(ex0, 2) + pow(ex, 2) ) ) );
	return ( (x-x0)/ex0 );
}

//int SlicesFit(TH2F* h2, TH1F* hMean, TH1F* hSigma, int opt, int nfit) {
//	TObjArray oaFitResult;
//	//first fit, using FitSlicesY
//	h2->FitSlicesY(0, 0, -1, 0, "QNR", &oaFitResult);
//	hMean = (TH1F*)oaFitResult.FindObject(Form("%s_1", h2->GetName()));
//	hMean = (TH1F*)hMean->Clone();
//	
//	hSigma = (TH1F*)oaFitResult.FindObject(Form("%s_2", h2->GetName()));
//	hSigma = (TH1F*)hSigma->Clone();
//	
//	nfit--;
//	
//	//do the iterative fit
//	for (int ifit=nfit; ifit>0; ifit--) {
//		
//	}
//	
//	return 1;
//}

int SlicesFit(TH2F* h2, TObjArray &oaMean, TObjArray &oaSigma, int opt, int nfit, bool dir)
{
	TObjArray oaFitResult;
	TH1F *hMean=NULL, *hSigma=NULL;
	TH1F *hProjY=NULL;
	double dMean=0., dSigma=0.;
	TF1 *f1FitRe=NULL;
	
	//first fit, using FitSlicesY
	h2->FitSlicesY(0, 0, -1, 0, "QNR", &oaFitResult);
	hMean = (TH1F*)oaFitResult.FindObject(Form("%s_1", h2->GetName()));
	hMean = (TH1F*)hMean->Clone();
	if (!dir) hMean->SetDirectory(0); //in order to avoid same name
	oaMean.AddLast(hMean);
	
	hSigma = (TH1F*)oaFitResult.FindObject(Form("%s_2", h2->GetName()));
	hSigma = (TH1F*)hSigma->Clone();
	if (!dir) hSigma->SetDirectory(0); //in order to avoid same name
	oaSigma.AddLast(hSigma);
	
	nfit--;
	
	//do the fit iteratively
	for (int ifit=nfit; ifit>0; ifit--) {
		for (int ibin=1; ibin<h2->GetNbinsX()+1; ibin++) {
			hProjY = (TH1F*)h2->ProjectionY(Form("%d_%d", nfit-ifit+1, ibin), ibin, ibin);
			if (hProjY->GetEntries()==0 && hProjY->GetSumOfWeights()==0) continue;
			dMean = hMean->GetBinContent(ibin);
			dSigma = hSigma->GetBinContent(ibin);
			if (dMean==0 && dSigma==0) continue;
			if (dMean-2*dSigma<-300. || dMean+2*dSigma>300) continue;
			if (opt==0) { //simply fit iteratively
				hProjY->Fit("gaus", "Q0", "", dMean-2*dSigma, dMean+2*dSigma);
				
				f1FitRe = (TF1*)hProjY->GetFunction("gaus");
			}
			else if (opt==1) { //fit double gaussian
				
			}
			
			if (f1FitRe!=0) {
				dMean = f1FitRe->GetParameter(1);
				hMean->SetBinContent(ibin, dMean);
				dSigma = f1FitRe->GetParameter(2);
				hSigma->SetBinContent(ibin, dSigma);
			}
			else cout << "Refit fail. dMean=" << dMean << " dSigma=" << dSigma << endl;
			f1FitRe=0;
		}
	}
	return 1;
}

//----------rigidity resolution calculation
TH1F* ResoCal(TH1F &hRR)
{
	TH1F *hRe = static_cast<TH1F*>(hRR.Clone());
	for (int ibin=1; ibin<hRe->GetNbinsX()+1; ibin++) {
		double a = hRe->GetBinContent(ibin);
		if (a==0) continue;
		double Rm = hRe->GetBinCenter(ibin);
		a = (a*Rm)/(a-1./Rm);
		hRe->SetBinContent(ibin, a);
	} //ibin
	return hRe;
}

bool DrawBinWidth(TH1F &h)
{
	for (int ibin=1; ibin<h.GetNbinsX()+1; ibin++) {
		h.SetBinContent(ibin, h.GetBinWidth(ibin));
	} //ibin
	h.SetFillColor(kBlue);
	h.SetFillStyle(3002);
	return true;
}

/*//----------calculate correct abscissa for given rigidity bin //2021.03.23: move into methods_simple.h
double CalAbs(const double lbin, const double ubin, const double index)
{
	double Abs=0.;
	if (index==-1) {
		Abs=(ubin-lbin)/log(ubin/lbin); //x^-1 need to treat differently
	}
	else {
		Abs=(pow(ubin, index+1)-pow(lbin, index+1))/(index+1.)/(ubin-lbin);
		Abs=pow(Abs, 1/index);
	}
	return Abs;
}

//double CalAbs(const double lbin, const double ubin, const double index)
//{
//	double Abs=0.;
//	Abs=(pow(ubin, index+1)-pow(lbin, index+1))/(index+1.)/(ubin-lbin);
//	Abs=pow(Abs, 1/index);
//	return Abs;
//}*/

void FluxScale(TGraphErrors &geFlux, const double index)
{
	double dRig, dFlux;
	double dEFlux;
	for (int ip=0; ip<geFlux.GetN(); ip++) {
		geFlux.GetPoint(ip, dRig, dFlux);
		dEFlux = geFlux.GetErrorY(ip);
		geFlux.SetPoint(ip, dRig, dFlux*pow(dRig,index));
		geFlux.SetPointError(ip, 0, dEFlux*pow(dRig,index));
	} //ip
}

////2020.12.23 //2021.11.15: move to methods_simple.h
//double CalGamma(const double dBeta)
//{
//	return sqrt(1./(1.-pow(dBeta,2)));
//}
//
//double CalMass(const int iZ, const double dBeta, const double dRig)
//{
//	//return iZ*dRig/(dBeta*CalGamma(dBeta));
//	return iZ*dRig/(dBeta*CalGamma(dBeta))/MPROTON;
//}
//
//double BetaToRig(const int iZ, const int iMass, const double dBeta)
//{
//	//return (iMass*dBeta*CalGamma(dBeta))/iZ;
//	return (iMass*MPROTON*dBeta*CalGamma(dBeta))/iZ;
//}
//
//double RigToBeta(const int iZ, const int iMass, const double dRig)
//{
//	//double dZtoM = (double)iZ/(double)iMass;
//	double dZtoM = (double)iZ/(iMass*MPROTON);
//	return dZtoM*sqrt(1./(pow(1./dRig,2)+pow(dZtoM,2)));
//}
//
////2021.02.13: Ekn to Beta
//double EknToBeta(const double dEkn)
//{
//	return sqrt(pow(dEkn,2)+2*dEkn*MPROTON)/(dEkn+MPROTON);
//}

////2021.04.05 //2021.11.15: move to methods_simple.h
//string GetISOName(const int charge, const int iISO)
//{
//	string sISOName="";
//	
//	int iMass=GetISOMass(charge, iISO);
//	if (iMass<=0)
//	{
//		cout << " ****** wrong iISO=" << iISO << ", should be >=0 ****** " << endl;
//		return sISOName;
//	}
//	
//	sISOName = Form("%d%s", iMass, sElementShort[charge-1].c_str());
//	return sISOName;
//}
#endif //METHODS3_H
