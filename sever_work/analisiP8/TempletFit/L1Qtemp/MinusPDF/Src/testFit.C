#include "../../include/drawvariable.h"
#include "../../include/L1QTemplateFit_binning.h"
#include "../../include/RooFitHeader.h"

using namespace RooFit;

const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;

//pointer
TH1F *hshift = NULL;

// string histn;
string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/MinusPDF";

void testFit(const int run_charge=13, const int SelRig=20){
    if (SelRig<-1 || SelRig>iNBL1TF-1)
	{
		cout << "ERROR: SelRig=" << SelRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return;
	}

	//Fit Temlpate Layer 1or2
	const int iTempLayer = 1;
	const int rho = 2;

	if(iTempLayer != 1 && iTempLayer != 2) return;

    //--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1TF, dBL1TF);
	const double dLRig=hL1QFit.GetBinLowEdge(SelRig+1), dURig=hL1QFit.GetBinLowEdge(SelRig+2);

    //----input file
	//--- Template file
	string sRho = "";
	if(rho != 0) sRho += Form("_rho%dp0",rho);
    string ftmp = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/OutputFile/UBL1Q");
	histn = Form("%s/LQTemp_TransRoot_Clockwise%s/LQTemp_rig%d.root", ftmp.c_str(), sRho.c_str(),  SelRig);
	TFile fQTemp(Form("%s", histn.c_str()), "READ");

	//--L1 Charge Distribution
	string rootdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile";
	histn = Form("%s/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root", rootdir.c_str());
	TFile fL1Q(Form("%s", histn.c_str()), "READ");

    ///----- output file
	histn = "TestFit";
    string sDrawDir = Form("%s/Pic/%s/runQ%d", wdir.c_str(), histn.c_str(),run_charge);
    cout << "Create Pic dir " << system(Form("mkdir -pv %s", sDrawDir.c_str())) << endl;
	string sRootDir = Form("%s/Pic/%s/FitResult/L%dTemp", wdir.c_str(), histn.c_str(), iTempLayer);
	cout << "Create Root dir " << system(Form("mkdir -pv %s", sRootDir.c_str())) << endl;
	TFile *fOutPut = new TFile(Form("%s/TempFitShift_r1_L%d_runq%d_rig%d.root", sRootDir.c_str(), iTempLayer, run_charge, SelRig), "RECREATE");

    //--TTree
    histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", run_charge,iTempLayer, iTempLayer, run_charge, SelRig);
    if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_allrig", run_charge,iTempLayer,iTempLayer, run_charge);
    TTree *tLQTemp = static_cast<TTree*>(fL1Q.Get(histn.c_str()));

    //---Hist
    histn = Form("q%d_rig%d", run_charge, SelRig);
	TH1F *hL1QTemp = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	// TH1F *hL1QTemp = (TH1F*)fL1Q.Get(Form("Charge_Calibration1/q%d/L1Inner/L1QTemp/rig%d/l1/L1Q_r1_q%d_xy2_L1QTemp2.0sigma_sel1_rig%d_Tk2nd1", run_charge, SelRig, run_charge, SelRig));
	int NRebin = 5;
	double dEvL=0.5, dEvU=5*1e6;
	// double dDrawL = 0;
	// double dDrawU = 20;
	double dDrawL = run_charge - 4;
	double dDrawU = run_charge + 4;
	double dFitL = run_charge + 0.5;
	double dFitU = run_charge + 1;

	//--RooWorkspace
	int fit_ch = run_charge  + 1;
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	RooRealVar *rrvQXY = rwsLQTemp->var("UBL1QXY");
	RooDataHist *htempalte = NULL;
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, fit_ch, SelRig);
	RooKeysPdf *rkpLQ = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
	RooFitResult *rfrResult;
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
	RooRealVar *rrvshift;

	//----shift variables
	rrvshift = rwsLQTemp->var(Form("r1_q%d_xy2_shift_L%dQTemp", fit_ch, iTempLayer));

	//---Sotre Shift
	histn = "Shift";
	fOutPut->mkdir(histn.c_str());
	fOutPut->cd(histn.c_str());
	histn = Form("Shift_r1_q%d", fit_ch);
	hshift = new TH1F(histn.c_str(), histn.c_str(), iNBL1TF, dBL1TF);
	hshift->SetDirectory(gDirectory);

	double UBL1QXY = 0;
	double sumL1Q = 0;
	tLQTemp->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
	long int nentries = tLQTemp->GetEntries();
	for(int ie = 0; ie < nentries; ie++){
		tLQTemp->GetEntry(ie);
		hL1QTemp->Fill(UBL1QXY);
		sumL1Q += UBL1QXY;
	}
	hL1QTemp->Rebin(NRebin);
	double ymax = hL1QTemp->GetMaximum();
	dEvU = ymax*1.5; 
	htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hL1QTemp);

	// RooDataSet *dataset = new RooDataSet(Form("data_q%d", run_charge), Form("data_q%d", run_charge), RooArgSet(*rrvQXY),Import(*tLQTemp));

	histn = Form("nparticle");
	RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), 100, 0., 1e7);

	histn = Form("sumPDF");
	RooAddPdf *rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), RooArgList(*rkpLQ), RooArgList(*rrvn));

	rrvQXY->setRange("Fit", dFitL, dFitU);
	rrvQXY->setRange("draw", dDrawL, dDrawU);
	rfrResult = rapSumpdf->fitTo(*htempalte, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));
	// rfrResult = rapSumpdf->fitTo(*dataset, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));

	//----draw setting
	gStyle->SetOptStat("");
	gStyle->SetOptTitle(0);
	lTitle.SetTextAlign(21);
	lLegend.SetTextAlign(11);
	TH1::AddDirectory(kFALSE);
	int fillStyle=3001;

	cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);

	pPad1 = new TPad(histn.c_str(), histn.c_str(), .0, .3, 1., 1.);
	pPad1->cd();
	// gPad->SetLogy();
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

	pPad1->cd();
	rpQFrame = rrvQXY->frame(dDrawL, dDrawU);
	htempalte->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data
	// dataset->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data
	rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, fit_ch, SelRig);
	rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[2]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); 

	rpQFrame->GetXaxis()->SetTitleSize(0.05);
	rpQFrame->GetXaxis()->SetTitleOffset(0.8);
	rpQFrame->GetXaxis()->SetRangeUser(dDrawL,dDrawU);
	if(iTempLayer == 1) rpQFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
	else rpQFrame->GetXaxis()->SetTitle("Tracker L2 Charge");
	rpQFrame->GetXaxis()->CenterTitle();
	rpQFrame->GetYaxis()->SetTitle("Events"); //2020.10.26
	rpQFrame->GetYaxis()->CenterTitle();
	rpQFrame->GetYaxis()->SetTitleSize(0.05);
	rpQFrame->GetYaxis()->SetTitleOffset(0.8);
	rpQFrame->SetMinimum(dEvL);
	rpQFrame->SetMaximum(dEvU);
	// rpQFrame->addTH1(hPull,"P");
	rpQFrame->DrawClone();

	TLine lCuts_fit;
	lCuts_fit.SetLineWidth(3);
	lCuts_fit.SetLineStyle(2);
	lCuts_fit.SetLineColor(kRed);
	lCuts_fit.DrawLine(dFitL, dEvL, dFitL, dEvU);
	lCuts_fit.DrawLine(dFitU, dEvL, dFitU, dEvU);

	///Pull Hist
	RooHist *rhPull = rpQFrame->pullHist(nullptr, nullptr, false);

	lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{[Templat L%d]}", kBlack, iTempLayer));
	lLegend.DrawLatexNDC(.15, .80, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
	lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Fit range:[%.2f, %.2f]}", kBlack, dFitL, dFitU));
	lLegend.DrawLatexNDC(.15, .70, Form("#color[%d]{Fit Events:%.0f}", kRed, rrvn->getVal()));
	lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Shift:%.4f}", kRed, rrvshift->getVal()));

	//-- Sotre Information
	fOutPut->cd();
	histn = Form("Shift/Shift_r1_q%d", fit_ch);
	hshift = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
	hshift->SetBinContent(SelRig+1, rrvshift->getVal());
	hshift->SetBinError(SelRig+1, rrvshift->getError());
	hshift->SetEntries(1);
	histn = "Shift";
	fOutPut->cd(histn.c_str());
	hshift->Write();

	//----Draw Pull
	pPad2->cd();
	rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
	rpPullFrame->addPlotable(rhPull,"PZ");
	// rpPullFrame->addTH1(hPull,"P");
	rpPullFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
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

	cCanv->SaveAs(Form("%s/q%d_rig%d_L%dPDF%s.png", sDrawDir.c_str(), run_charge, SelRig, iTempLayer, sRho.c_str()));
	// cCanv->SaveAs(Form("%s/q%d_rig%d_L%dPDF_rho0p5.png", sDrawDir.c_str(), run_charge, SelRig, iTempLayer));

	fQTemp.Close();
	//--store output files
	fOutPut->Close();
}