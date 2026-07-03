#include "../../include/drawvariable.h"
#include "../../include/L1QTemplateFit_binning.h"
#include "../../include/RooFitHeader.h"

using namespace RooFit;

const int iNBL1TF=L1QTF_NbinAllQ2; //23 bins
double *dBL1TF=L1QTF_BinsAllQ2;

//pointer
TH1F *hshift = NULL;

void diffSelFit(const int run_charge=14, const int SelRig=20, const int isel = 5){
    if (SelRig<-1 || SelRig>iNBL1TF-1)
	{
		cout << "ERROR: SelRig=" << SelRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return;
	}

	//Fit Temlpate Layer 1or2
	const int iTempLayer = 1;
	const int rho = 2;

	//-- RooKeysPdf or RooHistPdf
	const bool bKeysPdf = false;

    //--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1TF, dBL1TF);
	const double dLRig=hL1QFit.GetBinLowEdge(SelRig+1), dURig=hL1QFit.GetBinLowEdge(SelRig+2);

    //----input file
	//--- RooKeysPdf File
	string sRho = "";
	if(rho != 0) sRho += Form("_rho%dp0",rho);
    string ftmp = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp");
	TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp_CutHe_rho2p0/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");
	// TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp_CutHe_NoShift_rho2p0/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");
	// TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp%s/LQTemp_rig%d.root", ftmp.c_str(), sRho.c_str(), SelRig), "READ");
	// TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp_rho0p5/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");

	//--Charge Temp
	int ir = 1;
	TFile fTotalFile(Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/CutHe_KeysPdf_Z12ToZ16.root"), "READ");
	histn = Form("Charge_Calibration1/q%d/%s/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel1_rig%d", run_charge, cSpan[ir], iTempLayer, iTempLayer, ir, run_charge, SelRig);
	if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/%s/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel1_allrig", run_charge, cSpan[ir], iTempLayer, iTempLayer, ir, run_charge);
	cout << histn << endl;
	TTree *tLQTempHist = static_cast<TTree*>(fTotalFile.Get(histn.c_str()));
	double UBL1QXY_Hist;
	tLQTempHist->SetBranchAddress("UBL1QXY", &UBL1QXY_Hist);

	//-- Fit Sample
	// TFile fL1Q(Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root", ftmp.c_str()), "READ");
	TFile fTree(Form("/afs/cern.ch/work/j/jzhang2/AMS/result/analysis6/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_SaveTree_Fast4_Z12TZ14/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_SaveTree_Fast4_Z12TZ14.root"), "READ");//for Rig20
	// TFile fTree(Form("/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/L1QDistribution/root/ISS/rootTree/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_SaveTree_Fast3.root"), "READ");//for Rig20

    ///----- output file
    string sDrawDir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/Src/testPic");
    // cout << "Create Pic dir " << system(Form("mkdir -pv %s", sDrawDir.c_str())) << endl;

    //--TTree
    // string histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", run_charge,iTempLayer, iTempLayer, run_charge, SelRig);
    // if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_allrig", run_charge,iTempLayer,iTempLayer, run_charge);
    // TTree *tLQTemp = static_cast<TTree*>(fL1Q.Get(histn.c_str()));
	histn = Form("L1L2Sur4/q%d/L1Inner/TreeVar/Tree_r1_q%d", run_charge, run_charge);
    TTree *tLQTemp = static_cast<TTree*>(fTree.Get(histn.c_str()));

	int NRebin = 5;
	double dEvL=0.5, dEvU=5*1e6;
	double dDrawL = run_charge - 4;
	double dDrawU = run_charge + 4;
	// double dFitL = run_charge - 1.5;
	// double dFitU = run_charge + 1.5;
	double dFitL = run_charge - 0.45;
	double dFitU = run_charge + 0.45;
	double dAxisL = run_charge - 1.5;
	double dAxisU = run_charge + 1.5;

	//--RooWorkspace
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	RooRealVar *rrvQXY = rwsLQTemp->var("UBL1QXY");
	RooDataHist *htempalte = NULL;
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, run_charge, SelRig);
	RooKeysPdf *rkpLQ = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
	RooFitResult *rfrResult;
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
	RooRealVar *rrvshift;

	//--RooHistPdf
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_Hist", iTempLayer, run_charge, SelRig);
	TH1F *hL1QHist = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	hL1QHist->Rebin(NRebin);
	long int nevent = tLQTempHist->GetEntries();
	for(int ie = 0; ie < nevent; ie++){
		tLQTempHist->GetEntry(ie);
		hL1QHist->Fill(UBL1QXY_Hist);
	}
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_DataHist", iTempLayer, run_charge, SelRig);
	RooDataHist *dhHistPdf = new RooDataHist(histn.c_str(),histn.c_str(), *rrvQXY, hL1QHist);

    //---Hist
    histn = Form("q%d_rig%d", run_charge, SelRig);
	TH1F *hL1QTemp = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	hL1QTemp->Rebin(NRebin);

	//--- Charge Temp
    bool selFrag;
    bool selLTOFodd;
    bool selTOFS0;
    bool selInner;
    bool selUTOF;
	float tk_exqln[2][2][3];
	int tk_lid[9];
	float tk_qin[2][3];
	float tof_ql[4];
    tLQTemp->SetBranchAddress("selFrag", &selFrag);
    tLQTemp->SetBranchAddress("selLTOFodd", &selLTOFodd);
    tLQTemp->SetBranchAddress("selTOFS0", &selTOFS0);
    tLQTemp->SetBranchAddress("selUTOF", &selUTOF);
    tLQTemp->SetBranchAddress("selInner", &selInner);
    tLQTemp->SetBranchAddress("tk_exqln", &tk_exqln);
    tLQTemp->SetBranchAddress("tof_ql", &tof_ql);
    tLQTemp->SetBranchAddress("tk_qin", &tk_qin);
    tLQTemp->SetBranchAddress("tk_lid", &tk_lid); // new branch
	long int nentries = tLQTemp->GetEntries();
	for(int ie = 0; ie < nentries; ie++){
		tLQTemp->GetEntry(ie);
		bool bSel = true;
		if(tk_lid[0] == +811) continue;
		if(isel == 0) bSel = true;
		else if(isel == 1) bSel = selInner;
		else if(isel == 2) bSel = selInner && selLTOFodd;
		else if(isel == 3) bSel = selInner && selLTOFodd && selTOFS0;
		else if(isel == 4) bSel = selInner && selLTOFodd && selTOFS0 && selUTOF;
		else bSel = selFrag && selLTOFodd && selTOFS0 && selUTOF && selInner;
		// if(tk_exqln[0][0][2] < dFitL || tk_exqln[0][0][2] > dFitU) continue;

		// bool bChargeCut = false;
		// double dInnerQ = tk_qin[0][2];
		// double dUTofQ = (tof_ql[0]+tof_ql[1])/2.;
		// if(run_charge == 13){
		// 	bChargeCut = dInnerQ > 12.52 && dInnerQ < 13.48;
		// 	bChargeCut = bChargeCut && dUTofQ > 12.285 && dUTofQ < 14.5;
		// }
		// else if(run_charge == 14){
		// 	bChargeCut = dInnerQ > 13.5 && dInnerQ < 14.5;
		// 	bChargeCut = bChargeCut && dUTofQ > 13.2625 && dUTofQ < 15.5;
		// }
		// else if(run_charge == 15){
		// 	bChargeCut = dInnerQ > 14.5 && dInnerQ < 15.5;
		// 	bChargeCut = bChargeCut && dUTofQ > 14.24 && dUTofQ < 16.5;
		// }

		if(bSel) hL1QTemp->Fill(tk_exqln[0][0][2]);
	}

	// double UBL1QXY = 0;
	// tLQTemp->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
	// long int nentries = tLQTemp->GetEntries();
	// for(int ie = 0; ie < nentries; ie++){
	// 	tLQTemp->GetEntry(ie);
	// 	// if(UBL1QXY < dFitL || UBL1QXY > dFitU) continue;
	// 	hL1QTemp->Fill(UBL1QXY);
	// }

	
	htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hL1QTemp);

	double rNum = 0;
	double x12 = 0.5;
	if(run_charge % 2 == 1) x12 = 0.33;
	int xb1 = hL1QTemp->FindBin((run_charge)-x12);
	int xb2 = hL1QTemp->FindBin((run_charge)+x12);
	for(int ix = xb1; ix < xb2; ix++) rNum += hL1QTemp->GetBinContent(ix);

	// double ymax = hL1QTemp->GetMaximum();
	double ymax = hL1QTemp->GetBinContent(hL1QTemp->FindBin(run_charge));
	for(int ix = xb1; ix < xb2; ix++){
		if(ymax < hL1QTemp->GetBinContent(ix)) ymax = hL1QTemp->GetBinContent(ix);
	}
	dEvU = ymax*1.5;

	///---- for different charge
	double dIni = 0.8*rNum, dL = 0, dU = 1.2*rNum;
	if(run_charge == 12) dU = 1.1*rNum;
	if(run_charge == 14) dU = 1.4*rNum;
	if(run_charge == 16) dU = 1.1*rNum;
	histn = Form("nparticle");
	// RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), 100, 0., 1e7);
	RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), dIni, dL, dU);
	// RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), nentries);

	//----shift variables
	//--- RooKeysPdf Shift
	// rrvshift = rwsLQTemp->var(Form("r1_q%d_xy2_shift_L%dQTemp", run_charge, iTempLayer));
	// if(iTempLayer == 1){
	// 	rrvshift->setVal(0);
	// 	// rrvshift->setConstant(kTRUE);
	// }
	//-- RooHistPdf Shift
	histn = Form("r1_q%d_xy2_shift_L%dQTemp", run_charge, iTempLayer);
	rrvshift = new RooRealVar(histn.c_str(), histn.c_str(), 0.00, -0.3, 0.3);
	string histn1 = Form("r1_q%d_xy2_with_shift_L%dQTemp", run_charge, iTempLayer);
	string histn2 = "UBL1QXY";
	RooFormulaVar *rrvQWithShift = new RooFormulaVar(histn1.c_str(), histn1.c_str(), Form("%s-%s", histn2.c_str(), histn.c_str()), RooArgList(*rrvQXY, *rrvshift));

	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_HistPdf", iTempLayer, run_charge, SelRig);
	// RooHistPdf *rhpLQ = new RooHistPdf(histn.c_str(),histn.c_str(), *rrvQXY, *dhHistPdf);
	RooHistPdf *rhpLQ = new RooHistPdf(histn.c_str(),histn.c_str(), *rrvQWithShift, *rrvQXY, *dhHistPdf);

	histn = Form("sumPDF");
	RooAddPdf *rapSumpdf;
	if(bKeysPdf) rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), RooArgList(*rkpLQ), RooArgList(*rrvn));
	else rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), RooArgList(*rhpLQ), RooArgList(*rrvn));

	rrvQXY->setRange("Fit", dFitL, dFitU);
	rrvQXY->setRange("draw", dDrawL, dDrawU);
	rfrResult = rapSumpdf->fitTo(*htempalte, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(0));

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
	htempalte->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("PZ"));  ////plot data
	rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));
	if(bKeysPdf) histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, run_charge, SelRig);
	else histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_HistPdf", iTempLayer, run_charge, SelRig);
	rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[2]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); 

	rpQFrame->GetXaxis()->SetTitleSize(0.05);
	rpQFrame->GetXaxis()->SetTitleOffset(0.8);
	rpQFrame->GetXaxis()->SetRangeUser(dAxisL,dAxisU);
	rpQFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
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
	int nFitPara = 2; //shift + N
	double chis_fit = rpQFrame->chiSquare(nFitPara);
	// RooAbsReal *rhchis = rapSumpdf->createChi2(*htempalte);
	int binFitL = hL1QTemp->FindBin(dFitL);
	int binFitU = hL1QTemp->FindBin(dFitU-0.001);
	int ndf = binFitU - binFitL + 1 - nFitPara;
	// cout << "\ttotal Events: " << nentries << endl;

	// lLegend.DrawLatexNDC(.15, .90, Form("#color[%d]{[L1 Sample: Add Selection:%d]}", kBlack, isel));
	// lLegend.DrawLatexNDC(.15, .90, Form("#color[%d]{[Template L%d]}", kBlack, iTempLayer));
	lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
	lLegend.DrawLatexNDC(.15, .80, Form("#color[%d]{Fit Range:[%.2f, %.2f]}", kBlack, dFitL, dFitU));
	lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Fit Events: %.0f, N:%.2f}", kRed, rrvn->getVal(),rrvn->getVal()/rNum));
	lLegend.DrawLatexNDC(.15, .70, Form("#color[%d]{Shift: %.4f #pm %.4f}", kRed, rrvshift->getVal(), rrvshift->getError()));
	lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Chi2/ndf: %.0f/%d}", kBlack, chis_fit*ndf, ndf));

	//----Draw Pull
	pPad2->cd();
	rpPullFrame = rrvQXY->frame(dAxisL,dAxisU);
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
	rpPullFrame->GetYaxis()->SetRangeUser(-10,10);

	rpPullFrame->DrawClone();
		
	cCanv->cd();
	pPad1->DrawClone();
	pPad2->DrawClone();

	string sPdf = "KeysPDf";
	if(!bKeysPdf) sPdf = "HistPdf";
	histn = Form("%s/FreeShift/%s", sDrawDir.c_str(), sPdf.c_str());
	system(Form("mkdir -pv %s", histn.c_str()));
	// cCanv->SaveAs(Form("%s/q%d_rig%d_L%dPDF%s_BelowL1_%s.png", histn.c_str(), run_charge, SelRig, iTempLayer, sRho.c_str(), sPdf.c_str()));
	cCanv->SaveAs(Form("%s/q%d_rig%d_L%dPDF%s_Sel%d.png", histn.c_str(), run_charge, SelRig, iTempLayer, sRho.c_str(), isel));

	fQTemp.Close();

}