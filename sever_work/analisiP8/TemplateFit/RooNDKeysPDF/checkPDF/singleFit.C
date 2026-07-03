#include "./include/drawvariable.h"
#include "./include/L1QTemplateFit_binning.h"
#include "./include/RooFitHeader.h"

using namespace RooFit;

const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;

TH1F *hChis = NULL;

void singleFit( const int run_charge=14, 
				const int SelRig=20,
				const int iTempLayer = 1,
				int nd_rho = 1,
				int nd_sigma = 1){
    if (SelRig<-1 || SelRig>iNBL1TF-1)
	{
		cout << "ERROR: SelRig=" << SelRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return;
	}

    //--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1TF, dBL1TF);
	const double dLRig=hL1QFit.GetBinLowEdge(SelRig+1), dURig=hL1QFit.GetBinLowEdge(SelRig+2);

	//Fit Temlpate Layer 1or2
	// const int iTempLayer = 1;
	// int nd_rho = 2, nd_sigma = 1;

	if(iTempLayer != 1 && iTempLayer != 2) return;

    //----input file
	const string sTotalDir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/makePDF";
	string sTempName = Form("LQTemp_CutHe_Z7TZ18_NDKeys_NoShift_rho%dp0_sigma%dp0", nd_rho, nd_sigma);
	if(run_charge >= 24) sTempName = Form("LQTemp_CutHe_Z24TZ28_NDKeys_NoShift_rho%dp0_sigma%dp0", nd_rho, nd_sigma);

	sTempName = Form("LQTemp_Rebin_Z7TZ18_NDKeys_NoShift_rho%dp0_sigma%dp0", nd_rho, nd_sigma);
	if(run_charge >= 24) sTempName = Form("LQTemp_Rebin_Z24TZ28_NDKeys_NoShift_rho%dp0_sigma%dp0", nd_rho, nd_sigma);
	//--- Template file
	// histn = Form("%s/OutputFile/UBL1Q/%s/LQTemp_rig%d.root", sTotalDir.c_str(), sTempName.c_str(),  SelRig);
	histn = Form("%s/OutputFile/UBL1Q_Rebin/%s/LQTemp_rig%d.root", sTotalDir.c_str(), sTempName.c_str(),  SelRig);
	TFile fQTemp(histn.c_str(), "READ");

	//--L1 Charge Distribution
	// string rootdir = Form("%s/InputFile/RandomRoot/", sTotalDir.c_str());
	// if(run_charge >= 24 && run_charge <= 28) rootdir += "amsd69nChenYa_RandomRoot_Z24TZ28.root";
	// else rootdir += "amsd69nChenYa_RandomRoot_Z7TZ18.root";
	// string rootdir = Form("%s/InputFile/RebinRoot/", sTotalDir.c_str());
	// rootdir += "amsd69nChenYa_ReBinTrans_Z7TZ18.root";
	string rebindir_total = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/Rebin/RebinRootFile/RRoot/amsd69nChenYa_ReBin_Z7TZ18.root";
	if(run_charge >= 24) rebindir_total = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/Rebin/RebinRootFile/RRoot/amsd69nChenYa_ReBin_Z24TZ28.root";
	string rootdir = rebindir_total;
	TFile fL1Q(Form("%s", rootdir.c_str()), "READ");

    ///----- output file
    string sDrawDir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/OutputFile/RebinRoot_TotalL1/%s/runQ%d/L%d", sTempName.c_str(),run_charge,iTempLayer);
    // string sDrawDir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/OutputFile/RebinRoot/%s/runQ%d/L%d", sTempName.c_str(),run_charge,iTempLayer);
    cout << "Create Pic dir " << system(Form("mkdir -pv %s", sDrawDir.c_str())) << endl;
	string sRootDir = Form("%s/Root", sDrawDir.c_str());
	cout << "Create Pic dir " << system(Form("mkdir -pv %s", sRootDir.c_str())) << endl;
	TFile *fOutPut = new TFile(Form("%s/L%dTempCheck_q%d_rig%d.root", sRootDir.c_str(), iTempLayer, run_charge, SelRig), "RECREATE");

	//---- Sotre Information
	histn = "Chisquare";
	fOutPut->mkdir(histn.c_str());
	fOutPut->cd(histn.c_str());
	histn = Form("ChisquareNdf_q%d_L%d", run_charge, iTempLayer);
	hChis = new TH1F(histn.c_str(), histn.c_str(), iNBL1TF, dBL1TF);
	hChis->SetDirectory(gDirectory);

    //--TTree
    string histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", run_charge,iTempLayer, iTempLayer, run_charge, SelRig);
    if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_allrig", run_charge,iTempLayer,iTempLayer, run_charge);
    TTree *tLQTemp = static_cast<TTree*>(fL1Q.Get(histn.c_str()));

    //---Hist
    histn = Form("q%d_rig%d", run_charge, SelRig);
	TH1F *hL1QTemp = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	int NRebin = 20;
	double dEvL=0.5, dEvU=5*1e6;
	// double dDrawL = 0;
	// double dDrawU = 20;
	double dDrawL = run_charge - 4;
	double dDrawU = run_charge + 4;
	double dFitL = run_charge - 1;
	double dFitU = run_charge + 1;

	//--RooWorkspace
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	RooRealVar *rrvQXY = rwsLQTemp->var("UBL1QXY");
	RooDataHist *htempalte = NULL;
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, run_charge, SelRig);
	RooNDKeysPdf *rkpLQ = static_cast<RooNDKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
	RooFitResult *rfrResult;
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;

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
	RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), nentries);
	// RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), 100, 0., 1e7);

	histn = Form("sumPDF");
	RooAddPdf *rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), RooArgList(*rkpLQ), RooArgList(*rrvn));

	rrvQXY->setRange("Fit", dFitL, dFitU);
	rrvQXY->setRange("draw", dDrawL, dDrawU);
	// rfrResult = rapSumpdf->fitTo(*htempalte, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));
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
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, run_charge, SelRig);
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
	// lCuts_fit.DrawLine(dFitL, dEvL, dFitL, dEvU);
	// lCuts_fit.DrawLine(dFitU, dEvL, dFitU, dEvU);

	///Pull Hist
	RooHist *rhPull = rpQFrame->pullHist(nullptr, nullptr, false);

	////--- Calculate the chisq
	double chisq = 0;
	double chisL = dDrawL;
	double chisU = dDrawU;
	cout << "\t pull hit: " << rhPull->GetN() << endl;
	for(int i = 0; i < rhPull->GetN(); i++){
		if(rhPull->GetPointX(i) < chisL || rhPull->GetPointX(i) > chisU) continue;
		chisq += rhPull->GetPointY(i) * rhPull->GetPointY(i);
		// cout << "\t" << rhPull->GetPointY(i) << "\t total:" << chisq << endl;
	}
	int bchisL = hL1QTemp->FindBin(chisL);
	int bchisU = hL1QTemp->FindBin(chisU);
	int ndf = bchisU - bchisL +1;

	fOutPut->cd("Chisquare");
	histn = Form("Chisquare/ChisquareNdf_q%d_L%d", run_charge, iTempLayer);
	hChis = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
	hChis->SetBinContent(SelRig+1, chisq/ndf);
	// hChis->SetBinError(SelRig+1, 0);
	hChis->SetEntries(1);
	//--store output files
	hChis->Write();

	lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{[Templat L%d]}", kBlack, iTempLayer));
	// lLegend.DrawLatexNDC(.15, .80, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
	lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Events:%.0ld}", kRed, nentries));
	lLegend.DrawLatexNDC(.15, .70, Form("#color[%d]{Rho:%d.0, Sigma:%d.0}", kRed, nd_rho, nd_sigma));
	lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Chi2/ndf: %.0f/%d}", kBlue, chisq, ndf));
	// lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Fit range:[%.2f, %.2f]}", kBlack, dFitL, dFitU));
	// lLegend.DrawLatexNDC(.15, .70, Form("#color[%d]{Fit Events:%.0f}", kRed, rrvn->getVal()));

	//----Draw Pull
	pPad2->cd();
	rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
	rpPullFrame->addPlotable(rhPull,"PZ");
	// rpPullFrame->addTH1(hPull,"P");
	rpPullFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
	rpPullFrame->GetXaxis()->CenterTitle();
	rpPullFrame->GetXaxis()->SetTitleSize(0.1);
	rpPullFrame->GetXaxis()->SetTitleOffset(0.7);
	rpPullFrame->GetXaxis()->SetLabelSize(0.1);
	rpPullFrame->GetYaxis()->SetTitle("Pull");
	rpPullFrame->GetYaxis()->CenterTitle();
	rpPullFrame->GetYaxis()->SetTitleSize(0.1);
	rpPullFrame->GetYaxis()->SetTitleOffset(0.4);
	rpPullFrame->GetYaxis()->SetLabelSize(0.1);
	// rpPullFrame->GetYaxis()->SetRangeUser(-100, 100);

	rpPullFrame->DrawClone();
		
	cCanv->cd();
	pPad1->DrawClone();
	pPad2->DrawClone();

	cCanv->SaveAs(Form("%s/q%d_rig%d_L%dPDF.png", sDrawDir.c_str(), run_charge, SelRig, iTempLayer));

	fQTemp.Close();
	fOutPut->Close();
}