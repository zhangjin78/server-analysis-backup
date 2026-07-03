#include "../include/drawvariable.h"
#include "../include/L1QTemplateFit_binning.h"
#include "../include/RooFitHeader.h"

using namespace RooFit;

const int iNBL1TF=L1QTF_NbinAllQ2; //23 bins
double *dBL1TF=L1QTF_BinsAllQ2;

//pointer
TH1F *hshift = NULL;

void diffSamFit(const int run_charge=12, const int SelRig=10){
    if (SelRig<-1 || SelRig>iNBL1TF-1)
	{
		cout << "ERROR: SelRig=" << SelRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return;
	}

	//Fit Temlpate Layer 1or2
	const int iTempLayer = 2;
	const int rho = 2;

    //--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1TF, dBL1TF);
	const double dLRig=hL1QFit.GetBinLowEdge(SelRig+1), dURig=hL1QFit.GetBinLowEdge(SelRig+2);

    //----input file
	string sRho = "";
	if(rho != 0) sRho += Form("_rho%dp0",rho);
    string ftmp = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp");
	// TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTempNoShift/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");
	TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp%s/LQTemp_rig%d.root", ftmp.c_str(), sRho.c_str(), SelRig), "READ");
	// TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp_rho0p5/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");
	TFile fL1Q(Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root", ftmp.c_str()), "READ");
	TFile fL1Dis(Form("/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/L1QDistribution/root/ISS/ForceNewTrig/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_CorrSel2.root"), "READ");
	TFile fTree(Form("/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/L1QDistribution/root/ISS/rootTree/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_SaveTree_Fast3.root"), "READ");//for Rig20

    ///----- output file
    string sDrawDir = Form("%s/Plot/Pic/FitShift/runQ%d/L%dTemp", ftmp.c_str(),run_charge,iTempLayer);
    // cout << "Create Pic dir " << system(Form("mkdir -pv %s", sDrawDir.c_str())) << endl;
	string sRootDir = Form("%s/Plot/Root/QDisShift/L%dTemp", ftmp.c_str(), iTempLayer);
	cout << "Create Pic dir " << system(Form("mkdir -pv %s", sRootDir.c_str())) << endl;
	TFile *fOutPut = new TFile(Form("%s/TempFitShift_r1_L%d_runq%d_rig%d.root", sRootDir.c_str(), iTempLayer, run_charge, SelRig), "RECREATE");

    //--TTree
    string histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", run_charge,iTempLayer, iTempLayer, run_charge, SelRig);
    if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_allrig", run_charge,iTempLayer,iTempLayer, run_charge);
    TTree *tLQTemp = static_cast<TTree*>(fL1Q.Get(histn.c_str()));
	// histn = Form("L1L2Sur4/q%d/L1Inner/TreeVar/Tree_r1_q%d", run_charge, run_charge);
    // TTree *tLQTemp = static_cast<TTree*>(fTree.Get(histn.c_str()));

	//--L1 Charge Distribution
	int ir = 1;
	histn = Form("L1L2Sur4/q14/%s/period1/tofqcut0/L1QvsRig_r%d_q14_L1L2Sur_noL1Qcut_period1_tofqcut0", 
									sSpan[ir].c_str(), ir);
	TH2F* hQ2dDis = static_cast<TH2F*>(fL1Dis.Get(histn.c_str()));
	histn = Form("L1Q_r%d_rig%d", ir, SelRig);
	TH1D* hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRig), hQ2dDis->GetXaxis()->FindBin(dURig-0.001), "e");

    //---Hist
    histn = Form("q%d_rig%d", run_charge, SelRig);
	TH1F *hL1QTempAll = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	int NRebin = 5;
	double dEvL=0.5, dEvU=5*1e6;
	double dDrawL = run_charge - 4;
	double dDrawU = run_charge + 4;
	double dFitL = run_charge - 0.4;
	double dFitU = run_charge + 0.4;
	double dAxisL = run_charge - 1.5;
	double dAxisU = run_charge + 1.5;

	//---Sotre Shift
	histn = "Shift";
	fOutPut->mkdir(histn.c_str());
	fOutPut->cd(histn.c_str());
	histn = Form("Shift_r1_q%d", run_charge);
	hshift = new TH1F(histn.c_str(), histn.c_str(), iNBL1TF, dBL1TF);
	hshift->SetDirectory(gDirectory);

	//--RooWorkspace
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	RooRealVar *rrvQXY = rwsLQTemp->var("UBL1QXY");
	RooDataHist *htempalte = NULL;
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, run_charge, SelRig);
	RooKeysPdf *rkpLQ = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
	RooFitResult *rfrResult;
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
	RooRealVar *rrvshift;
	
	hQDisD->Rebin(NRebin);;
	int bFitL = hQDisD->FindBin(dAxisL);
	int bFitU = hQDisD->FindBin(dAxisU);
	histn = Form("L1Q_rig%d_RangeFit", SelRig);
	// TH1F *hL1QTemp = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	// hL1QTemp->Rebin(NRebin);
	// TH1D *hL1QTemp= new TH1D(histn.c_str(), histn.c_str(), bFitU-bFitL+1, hQDisD->GetBinLowEdge(bFitL), hQDisD->GetBinLowEdge(bFitU));
	// for(int ib= bFitL; ib <= bFitU; ib++){
	// 	hL1QTemp->SetBinContent(ib-bFitL+1, hQDisD->GetBinContent(ib));
	// }

	//--Selection for ChargeTemp
	// const int isel = 5;
    // bool selFrag;
    // bool selLTOFodd;
    // bool selTOFS0;
    // bool selInner;
    // bool selUTOF;
	// float tk_exqln[2][2][3];
	// int tk_lid[9];
    // tLQTemp->SetBranchAddress("selFrag", &selFrag);
    // tLQTemp->SetBranchAddress("selLTOFodd", &selLTOFodd);
    // tLQTemp->SetBranchAddress("selTOFS0", &selTOFS0);
    // tLQTemp->SetBranchAddress("selUTOF", &selUTOF);
    // tLQTemp->SetBranchAddress("selInner", &selInner);
    // tLQTemp->SetBranchAddress("tk_exqln", &tk_exqln);
    // // tLQTemp->SetBranchAddress("tk_lid", &tk_lid); // new branch
	// long int nentries = tLQTemp->GetEntries();
	// for(int ie = 0; ie < nentries; ie++){
	// 	tLQTemp->GetEntry(ie);
	// 	bool bSel = true;
	// 	if(isel == 0) bSel = true;
	// 	else if(isel == 1) bSel = selFrag;
	// 	else if(isel == 2) bSel = selFrag && selLTOFodd;
	// 	else if(isel == 3) bSel = selFrag && selLTOFodd && selTOFS0;
	// 	else if(isel == 4) bSel = selFrag && selLTOFodd && selTOFS0 && selUTOF;
	// 	else bSel = selFrag && selLTOFodd && selTOFS0 && selUTOF && selInner;
	// 	if(tk_exqln[0][0][2] < dFitL || tk_exqln[0][0][2] > dFitU) continue;
	// 	if(bSel) hL1QTemp->Fill(tk_exqln[0][0][2]);
	// }

	// double UBL1QXY = 0;
	// tLQTemp->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
	// long int nentries = tLQTemp->GetEntries();
	// for(int ie = 0; ie < nentries; ie++){
	// 	tLQTemp->GetEntry(ie);
	// 	// if(UBL1QXY < dFitL || UBL1QXY > dFitU) continue;
	// 	hL1QTemp->Fill(UBL1QXY);
	// }

	// double ymax = hL1QTemp->GetMaximum();
	double ymax = hQDisD->GetBinContent(hQDisD->FindBin(run_charge));
	dEvU = ymax*1.5;
	htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hQDisD);

	double rNum = 0;
	double x12 = 0.5;
	if(run_charge % 2 == 1) x12 = 0.33;
	int xb1 = hQDisD->FindBin((run_charge)-x12);
	int xb2 = hQDisD->FindBin((run_charge)+x12);
	for(int ix = xb1; ix < xb2; ix++) rNum += hQDisD->GetBinContent(ix);

	///---- for different charge
	double dIni = 0.8*rNum, dL = 0, dU = 1.2*rNum;
	if(run_charge == 12) dU = 0.99*rNum;
	if(run_charge == 14) dU = 0.99*rNum;
	if(run_charge == 16) dU = 0.9*rNum;
	histn = Form("nparticle");
	// RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), 100, 0., 1e7);
	RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), dIni, dL, dU);
	// RooRealVar *rrvn = new RooRealVar(histn.c_str(), histn.c_str(), nentries);

	//----shift variables
	rrvshift = rwsLQTemp->var(Form("r1_q%d_xy2_shift_L%dQTemp", run_charge, iTempLayer));
	if(iTempLayer == 1){
		rrvshift->setVal(0);
		// rrvshift->setConstant(kTRUE);
	}

	histn = Form("sumPDF");
	RooAddPdf *rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), RooArgList(*rkpLQ), RooArgList(*rrvn));

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
	histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iTempLayer, run_charge, SelRig);
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
	int binFitL = hQDisD->FindBin(dFitL);
	int binFitU = hQDisD->FindBin(dFitU-0.001);
	int ndf = binFitU - binFitL + 1 - nFitPara;
	// cout << "\ttotal Events: " << nentries << endl;

	// lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{[L1 Sample: Add Selection:%d]}", kBlack, isel));
	lLegend.DrawLatexNDC(.15, .90, Form("#color[%d]{[Template L%d]}", kBlack, iTempLayer));
	lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
	lLegend.DrawLatexNDC(.15, .80, Form("#color[%d]{Fit Range:[%.2f, %.2f]}", kBlack, dFitL, dFitU));
	lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{Fit Events: %.0f, N:%.2f}", kRed, rrvn->getVal(),rrvn->getVal()/rNum));
	lLegend.DrawLatexNDC(.15, .70, Form("#color[%d]{Shift: %.4f #pm %.4f}", kRed, rrvshift->getVal(), rrvshift->getError()));
	lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Chi2/ndf: %.0f/%d}", kBlack, chis_fit*ndf, ndf));

	//-- Sotre Information
	fOutPut->cd();
	histn = Form("Shift/Shift_r1_q%d", run_charge);
	hshift = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
	hshift->SetBinContent(SelRig+1, rrvshift->getVal());
	hshift->SetBinError(SelRig+1, rrvshift->getError());
	hshift->SetEntries(1);
	histn = "Shift";
	fOutPut->cd(histn.c_str());
	hshift->Write();

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

	histn = Form("%s", sDrawDir.c_str());
	system(Form("mkdir -pv %s", histn.c_str()));
	cCanv->SaveAs(Form("%s/q%d_rig%d_L%dPDF%s.png", histn.c_str(), run_charge, SelRig, iTempLayer, sRho.c_str()));

	fQTemp.Close();

	//--store output files
	fOutPut->Close();

}