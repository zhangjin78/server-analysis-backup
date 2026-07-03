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

//define how many ixy stored
//const int XY0=0, nXY=3; //X, Y, XY
const int XY0=2, nXY=3; //only XY

//pointer
TH1F *hshift = NULL;
TH1F *hchisq = NULL;

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

void runplotKeysPdf(const int run_charge=14, const int SelRig=20)
{
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

	///----- output file
	string sDrawL1QTempFit = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L2Qtemp_L1QTempFit/result");
	TFile *fOutPut = new TFile(Form("%s/L1QTempFit_r1_q%d_xy2_rig%d.root", sDrawL1QTempFit.c_str(), run_charge, SelRig), "RECREATE");

	//--TTree
	string histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", run_charge, run_charge, SelRig);
	if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L1Q_KeysPdf/L1Q_r1_q%d_sel1_allrig", run_charge, run_charge);
	// string histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q%d_sel1_rig%d", run_charge, run_charge, SelRig);
	// if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q%d_sel1_allrig", run_charge, run_charge);
	TTree *tLQTemp = static_cast<TTree*>(fQTemplate.Get(histn.c_str()));

	//---Hist
	TH1F *hL2pdf = new TH1F(Form("q%d_rig%d", run_charge, SelRig), Form("q%d_rig%d", run_charge, SelRig), 3500, 0, 35);
	int NRebin = 2;
	double dEvL=0.5, dEvU=5*1e6;
	double dDrawL = run_charge - 4;
	double dDrawU = run_charge + 4;
	double dFitL = run_charge - 0.5;
	double dFitU = run_charge + 0.5;

	if(run_charge %2 == 1 && run_charge > 7) dEvU=2*1e3;
	if(run_charge == 9 || run_charge == 11 || run_charge > 12){
		if(run_charge % 2 == 1){
			dFitL = run_charge - 0.5;
			dFitU = run_charge + 0.4;
			NRebin = 10;
		}
		else{
			dFitL = run_charge - 0.7;
			dFitU = run_charge + 0.7;
			NRebin = 5;
		}
	}

	cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);

	TLine lCuts_fit;
	lCuts_fit.SetLineWidth(3);
	lCuts_fit.SetLineStyle(2);
	lCuts_fit.SetLineColor(kRed);

	hshift = new TH1F(Form("Shift_r1_q%d", run_charge), Form("Shift_r1_q%d", run_charge), iNBL1TF, dBL1TF);
	hshift->SetDirectory(gDirectory);

	hchisq = new TH1F(Form("Chisquare_r1_q%d", run_charge), Form("Chisquare_r1_q%d", run_charge), iNBL1TF, dBL1TF);
	hchisq->SetDirectory(gDirectory);

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
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	RooRealVar *rrvQXY = rwsLQTemp->var("L1QXY");
	RooDataHist *htempalte = NULL;
	histn = Form("L2Q_r1_q%d_xy2_sel1_rig%d_KeysPdf", run_charge, SelRig);
	RooKeysPdf *rkpLQ = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
	RooFitResult *rfrResult;
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
	RooRealVar *rrvshift;

	//----shift variables
	rrvshift = rwsLQTemp->var(Form("r1_q%d_xy2_shift_L2QTemp", run_charge));

	double L2QXY = 0;
	tLQTemp->SetBranchAddress("L1QXY", &L2QXY);
	long int nentries = tLQTemp->GetEntries();
	for(int ie = 0; ie < nentries; ie++){
		tLQTemp->GetEntry(ie);
		hL2pdf->Fill(L2QXY);
	}
	hL2pdf->Rebin(NRebin);
	htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hL2pdf);

	rrvQXY->setRange("Fit", dFitL, dFitU);
	rrvQXY->setRange("draw", dDrawL, dDrawU);
	rfrResult = rkpLQ->fitTo(*htempalte, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));
	// rfrResult = rkpLQ->fitTo(*htempalte, RooFit::Extended(1), RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));

	pPad1->cd();
	rpQFrame = rrvQXY->frame(dDrawL, dDrawU);
	htempalte->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data
	rkpLQ->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));

	double chis_fit = rpQFrame->chiSquare();

	RooHist *rhPull = rpQFrame->pullHist();

	double shift_v = rrvshift->getVal();
	double shift_e = rrvshift->getError();
	hshift = static_cast<TH1F*>(fOutPut->Get(Form("Shift_r1_q%d", run_charge)));
	hshift->SetBinContent(SelRig+1, shift_v);
	hshift->SetBinError(SelRig+1, shift_e);
	hshift->SetEntries(1);

	hchisq = static_cast<TH1F*>(fOutPut->Get(Form("Chisquare_r1_q%d", run_charge)));
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

	lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{Chisqure: %.2f}", kBlack, chis_fit));
	lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Rebin: %d}", kBlack, NRebin));
	lLegend.DrawLatexNDC(.65, .85, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
	lLegend.DrawLatexNDC(.65, .75, Form("#color[%d]{shift: %.2e #pm %.2e}", kBlack, rrvshift->getVal(), rrvshift->getError()));

	lCuts_fit.DrawLine(dFitL, dEvL, dFitL, dEvU);
	lCuts_fit.DrawLine(dFitU, dEvL, dFitU, dEvU);

	//----Draw Pull
	pPad2->cd();
	rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
	rpPullFrame->addPlotable(rhPull,"P");
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
	cCanv->SaveAs(Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L2Qtemp_L1QTempFit/L1QTemp/q%d_rig%d.png", run_charge, SelRig));

	// delete cCanv;

	//------
	//--close files
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
