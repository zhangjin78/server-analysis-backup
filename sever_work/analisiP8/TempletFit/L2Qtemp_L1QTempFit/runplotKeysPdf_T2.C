//------------------
//update from:		KeysPdf4.C
//update date: 		2019.11.07
//------------------
//script to build L1Q and L2Q template using RooKeysPdf
//------------------
//------------------

#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

#define SDIAT

using namespace RooFit;

//define how many ixy stored
//const int XY0=0, nXY=3; //X, Y, XY
const int XY0=2, nXY=3; //only XY

//pointer
TH1F *hshift = NULL;
TH1F *hchisq = NULL;
TTree *tchisq=NULL;

//2020.01.22
//#define L1QTEMPSMALLBIN
#define L2QTEMPALL
// #define FELQTEMP
#ifdef L1QTEMPSMALLBIN
const int iNBL1TF=L1QTF_NbinSmall;
double *dBL1TF=L1QTF_BinsSmall;
#elif defined L2QTEMPALL
const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;
#elif defined FELQTEMP
const int iNBL1TF=L1QTF_NbinFe3;
double *dBL1TF=L1QTF_BinsFe3;
#else

#endif //L1QTEMPSMALLBIN

void runplotKeysPdf_T2(const int run_charge=7, const int SelRig=6)
{
	if(run_charge % 2 == 0) return;  /// only for odd charge 
	if(run_charge > 16){
		cout << "only fit the charge < 17" << endl;
		return;
	}
	//------check Rigidity
	//store template for all rigidity
	if (SelRig<-1 || SelRig>iNBL1TF-1)
	{
		cout << "ERROR: SelRig=" << SelRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return -1;
	}
	//--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1TF, dBL1TF);
	const double dLRig=hL1QFit.GetBinLowEdge(SelRig+1), dURig=hL1QFit.GetBinLowEdge(SelRig+2);
	
	//----draw setting
	gStyle->SetOptStat("");
	gStyle->SetOptTitle(0);
	lTitle.SetTextAlign(21);
	lLegend.SetTextAlign(11);
	TH1::AddDirectory(kFALSE);

	//----input result
	string ftmp = Form("/afs/cern.ch/work/j/jzhang2/AMS/result/CHARGECAL_NEWTRQ");
	TFile fQTemplate(Form("%s/total_result.root", ftmp.c_str()), "READ");
	TFile fQTemp(Form("%s/LQTemp/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");

	string fTSH = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L2Qtemp_L1QTempFit/result";
	TFile fshift(Form("%s/L2TempFitL1QTemp_shift_T1.root", fTSH.c_str()), "READ");
	TH1F *hT1shift = (TH1F *)fshift.Get(Form("Shift/Shift_r1_q%d", run_charge+1));
	double evenQs = hT1shift->GetBinContent(SelRig+1);
	TFile fL1Q(Form("%s/total_result_L1QTemp.root", ftmp.c_str()), "READ");

	///----- output file
	string sDrawL1QTempFit = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L2Qtemp_L1QTempFit/result/T2");
	cout << "Create root dir " << system(Form("mkdir -pv %s", sDrawL1QTempFit.c_str())) << endl;
	TFile *fOutPut = new TFile(Form("%s/L1QTempFit_r1_q%d_xy2_rig%d.root", sDrawL1QTempFit.c_str(), run_charge, SelRig), "RECREATE");

	//--TTree
	string histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", run_charge, run_charge, SelRig);
	if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L1Q_KeysPdf/L1Q_r1_q%d_sel1_allrig", run_charge, run_charge);
	// string histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q%d_sel1_rig%d", run_charge, run_charge, SelRig);
	// if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q%d_sel1_allrig", run_charge, run_charge);
	TTree *tLQTemp = static_cast<TTree*>(fQTemplate.Get(histn.c_str()));

	//---Hist
	TH1F *hL2pdf = new TH1F(Form("q%d_rig%d", run_charge, SelRig), Form("q%d_rig%d", run_charge, SelRig), 3500, 0, 35);
	TH1F *hL1QTemp = (TH1F*)fL1Q.Get(Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L1QTemp/rig%d/l1/L1Q_r1_q%d_xy2_L1QTemp2.0sigma_sel1_rig%d_Tk2nd0", run_charge, SelRig, run_charge, SelRig));
	int NRebin = 2;
	double dEvL=0.5, dEvU=5*1e6;
	double dDrawL = run_charge - 4;
	double dDrawU = run_charge + 4;
	double dFitL = run_charge - 1.2;
	double dFitU = run_charge + 1.8;

	// if(run_charge %2 == 1 && run_charge > 7) dEvU=2*1e3;
	// if(run_charge %2 == 1 && run_charge > 9 && SelRig < 4) NRebin=10;
	// if(run_charge == 9 && SelRig < 2) NRebin=10;
	

	cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);

	TLine lCuts_fit;
	lCuts_fit.SetLineWidth(3);
	lCuts_fit.SetLineStyle(2);
	lCuts_fit.SetLineColor(kRed);

	histn = "Shift";
	fOutPut->mkdir(histn.c_str());
	fOutPut->cd(histn.c_str());
	histn = Form("Shift_r1_q%d", run_charge);
	hshift = new TH1F(histn.c_str(), histn.c_str(), iNBL1TF, dBL1TF);
	hshift->SetDirectory(gDirectory);

	histn = "Chisquare";
	fOutPut->mkdir(histn.c_str());
	fOutPut->cd(histn.c_str());
	histn = Form("Chisquare_r1_q%d", run_charge);
	hchisq = new TH1F(histn.c_str(), histn.c_str(), iNBL1TF, dBL1TF);
	hchisq->SetDirectory(gDirectory);

	histn = Form("Tree_q%d", run_charge);
	fOutPut->mkdir(histn.c_str());
	fOutPut->cd(histn.c_str());
	histn = Form("parametor_chisq_q%d", run_charge);
	tchisq = new TTree(histn.c_str(),histn.c_str());

	pPad1 = new TPad(histn.c_str(), histn.c_str(), .0, .3, 1., 1.);
	pPad1->cd();
	gPad->SetLogy();
	gPad->SetGridy();
	gPad->SetTicks();
	gPad->SetTopMargin(0.05);
	gPad->SetBottomMargin(0);
	gPad->SetRightMargin(0.05);

	pPad2 = new TPad(histn.c_str(), histn.c_str(), .0, .0, 1., .3);
	pPad2->cd();
	//gPad->SetGrid();
	gPad->SetGridy();
	gPad->SetTicks();
	gPad->SetTopMargin(0);
	gPad->SetBottomMargin(0.2);
	gPad->SetRightMargin(0.05);


	//--RooWorkspace
	const int iNVar = 3;
	int bcharge = run_charge -1 ;
	RooRealVar *rrvshift[iNVar];
	RooKeysPdf *rkpLQ[iNVar];
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	RooRealVar *rrvQXY = rwsLQTemp->var("L1QXY");
	RooDataHist *htempalte = NULL;
	RooFitResult *rfrResult;
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;

	for(int ivar = 0; ivar < iNVar; ivar++){
		histn = Form("L2Q_r1_q%d_xy2_sel1_rig%d_KeysPdf", bcharge+ivar, SelRig);
		rkpLQ[ivar] = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));

		//----shift variables
		rrvshift[ivar] = rwsLQTemp->var(Form("r1_q%d_xy2_shift_L2QTemp", bcharge+ivar));
		if(bcharge + ivar != run_charge){
			rrvshift[ivar]->setVal(evenQs);
			rrvshift[ivar]->setConstant(kTRUE);
			cout << "\tq_" << bcharge+ivar << "\tset_shift:" << rrvshift[ivar] << endl;
		}
	}
	
	double L2QXY = 0;
	tLQTemp->SetBranchAddress("L1QXY", &L2QXY);
	long int nentries = tLQTemp->GetEntries();
	for(int ie = 0; ie < nentries; ie++){
		tLQTemp->GetEntry(ie);
		hL2pdf->Fill(L2QXY);
	}
	hL2pdf->Rebin(NRebin);
	// double ymax = hL2pdf->GetMaximum();
	hL1QTemp->Rebin(NRebin);
	double ymax = hL1QTemp->GetMaximum();
	dEvU = ymax*1.5; 
	// htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hL2pdf);
	htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hL1QTemp);

	rrvQXY->setRange("Fit", dFitL, dFitU);
	rrvQXY->setRange("draw", dDrawL, dDrawU);

	////- sum pdf
	RooRealVar *rrvn[iNVar];
	double nEvents = hL1QTemp->Integral(1, hL1QTemp->GetNbinsX());
	for (int ivar=0; ivar<iNVar; ivar++)
	{
		histn = Form("nparticle%d", ivar);
		double dIni, dL, dU;
		dIni=100, dL=0., dU=nEvents*1.1;
		if((bcharge + ivar) == run_charge) dIni = 0.8*dU;
		rrvn[ivar] = new RooRealVar(histn.c_str(), histn.c_str(), dIni, dL, dU);
	} //ivar

	histn = Form("L2Q_r1_q%d_xy2_sel1_rig%d_KeysPdf_sum", run_charge, SelRig);
	RooAddPdf *rapSumpdf=NULL;
	RooArgList ralLQ, raln;
	for (int ivar=0; ivar<iNVar; ivar++)
	{
		ralLQ.add(*rkpLQ[ivar]);
		raln.add(*rrvn[ivar]);
	}
	
	rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), ralLQ,raln);

	rfrResult = rapSumpdf->fitTo(*htempalte, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(0));
	// rfrResult = rapSumpdf->fitTo(*htempalte, RooFit::Extended(1), RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));

	pPad1->cd();
	rpQFrame = rrvQXY->frame(dDrawL, dDrawU);
	htempalte->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data
	rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));
	
	double chis_fit = rpQFrame->chiSquare(nullptr, nullptr, 2); // Fit para 1
	RooAbsReal *rhchis = rapSumpdf->createChi2(*htempalte, RooFit::Range("Fit"));

	///Pull Hist
	RooHist *rhPull = rpQFrame->pullHist(nullptr, nullptr, false);
	int bins = hL1QTemp->FindBin(dFitU) - hL1QTemp->FindBin(dFitL) + 1;
	chis_fit = rhchis->getVal()/(bins-iNVar*2+1);

	tchisq->Branch("Chisquare", &chis_fit, "Chisquare/D");

	// //----components
	int fillStyle=3001;
	for (int ivar=0; ivar<iNVar; ivar++){
		if(bcharge + ivar != run_charge) continue;
		histn = Form("L2Q_r1_q%d_xy2_sel1_rig%d_KeysPdf", bcharge+ivar, SelRig);
		rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(2), RooFit::LineColor(iColor[ivar]), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
	}
	for (int ivar=0; ivar<iNVar; ivar++){
		if(bcharge + ivar == run_charge) continue;
		histn = Form("L2Q_r1_q%d_xy2_sel1_rig%d_KeysPdf", bcharge+ivar, SelRig);
		rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(2), RooFit::LineColor(iColor[ivar]), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
	}
	double sumfitEv = 0;
	for (int ivar=0; ivar<iNVar; ivar++){
		sumfitEv += rrvn[ivar]->getVal();
	}


	double shift_v = rrvshift[0+run_charge-bcharge]->getVal();
	double shift_e = rrvshift[0+run_charge-bcharge]->getError();
	histn = Form("Shift/Shift_r1_q%d", run_charge);
	hshift = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
	hshift->SetBinContent(SelRig+1, shift_v);
	hshift->SetBinError(SelRig+1, shift_e);
	hshift->SetEntries(1);

	histn = Form("Chisquare/Chisquare_r1_q%d", run_charge);
	hchisq = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
	hchisq->SetBinContent(SelRig+1, chis_fit);
	hchisq->SetEntries(1);
	
	rpQFrame->GetXaxis()->SetTitleSize(0.05);
	rpQFrame->GetXaxis()->SetTitleOffset(0.8);
	rpQFrame->GetXaxis()->SetRangeUser(dDrawL,dDrawU);
	rpQFrame->GetXaxis()->SetTitle("TRACKER L1 Charge");
	rpQFrame->GetXaxis()->CenterTitle();
	rpQFrame->GetYaxis()->SetTitle("Events"); //2020.10.26
	rpQFrame->GetYaxis()->CenterTitle();
	rpQFrame->GetYaxis()->SetTitleSize(0.05);
	rpQFrame->GetYaxis()->SetTitleOffset(0.8);
	rpQFrame->SetMinimum(dEvL);
	rpQFrame->SetMaximum(dEvU);
	rpQFrame->DrawClone();

	lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{Chisquare: %.2f}", kBlack, chis_fit));
	lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Rebin: %d}", kBlack, NRebin));
	// lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Total Events:%ld}", kBlack, nentries));
	lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Fit range:[%.2f, %.2f]}", kBlack, dFitL, dFitU));
	lLegend.DrawLatexNDC(.15, .55, Form("#color[%d]{Fit Range Events:%.0f}", kBlack, rpQFrame->getFitRangeNEvt(dFitL, dFitU)));
	lLegend.DrawLatexNDC(.15, .45, Form("#color[%d]{Fit Events:%.0f}", kRed, sumfitEv));
	lLegend.DrawLatexNDC(.65, .85, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
	lLegend.DrawLatexNDC(.65, .75, Form("#color[%d]{shift: %.2e #pm %.2e}", iColor[run_charge-bcharge], shift_v, shift_e));

	lCuts_fit.DrawLine(dFitL, dEvL, dFitL, dEvU);
	lCuts_fit.DrawLine(dFitU, dEvL, dFitU, dEvU);

	//----Draw Pull
	pPad2->cd();
	rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
	rpPullFrame->addPlotable(rhPull,"PZ");
	rpPullFrame->GetXaxis()->SetTitle("TRACKER L1 Charge");
	rpPullFrame->GetXaxis()->CenterTitle();
	rpPullFrame->GetXaxis()->SetTitleSize(0.1);
	rpPullFrame->GetXaxis()->SetTitleOffset(0.7);
	rpPullFrame->GetXaxis()->SetLabelSize(0.07);
	rpPullFrame->GetYaxis()->SetTitle("Pull");
	rpPullFrame->GetYaxis()->CenterTitle();
	rpPullFrame->GetYaxis()->SetTitleSize(0.1);
	rpPullFrame->GetYaxis()->SetTitleOffset(0.4);
	rpPullFrame->GetYaxis()->SetLabelSize(0.07);

	rpPullFrame->DrawClone();
		
	cCanv->cd();
	pPad1->DrawClone();
	pPad2->DrawClone();


	cCanv->Draw();
	string sPicdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L2Qtemp_L1QTempFit/L1QTemp/T2/pic";
	system(Form("mkdir -pv %s", sPicdir.c_str()));
	cCanv->SaveAs(Form("%s/q%d_rig%d.png", sPicdir.c_str(), run_charge, SelRig));
	sPicdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L2Qtemp_L1QTempFit/L1QTemp/T2/root";
	system(Form("mkdir -pv %s", sPicdir.c_str()));
	cCanv->SaveAs(Form("%s/q%d_rig%d.root", sPicdir.c_str(), run_charge, SelRig));

	// delete cCanv;

	//------
	//--close files
	tchisq->Fill();
	fQTemplate.Close();
	fQTemp.Close();

	//--store output files
	fOutPut->Write();
	fOutPut->Close();
} 

// void runplotKeysPdf(int icharge=16){
// 	for(int irig = 10; irig < 12; irig++){
// 		plotKeysPdf(icharge, irig);
// 	}
// }

// void runplotKeysPdf(int SelRig=16){
// 	for(int ich =6; ich < 18; ich++){
// 		plotKeysPdf(ich, SelRig);
// 	}
// }
