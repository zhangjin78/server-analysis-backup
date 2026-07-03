#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

using namespace RooFit;

const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;

void KeysPdfRead_L1TempFitL2PDF(const int SelRig=20,
								const int run_charge=14,
								int QVer = 1,
								int NVar0 = 3){
    if (SelRig<-1 || SelRig>iNBL1TF-1)
	{
		cout << "ERROR: SelRig=" << SelRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return;
	}

	//Fit Temlpate Layer 1or2
	const int iTempLayer = 1;
	const int iPDFLayer = 2;
	const int rho = 2;

    //--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1TF, dBL1TF);
	const double dLRig=hL1QFit.GetBinLowEdge(SelRig+1), dURig=hL1QFit.GetBinLowEdge(SelRig+2);

    //----input file
	string sRho = "";
	if(rho != 0) sRho += Form("_rho%dp0",rho);
    string ftmp = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp");
	TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp%s/LQTemp_rig%d.root", ftmp.c_str(), sRho.c_str(), SelRig), "READ");
	// TFile fQTemp(Form("%s/OutputFile/UBL1Q/LQTemp_rho0p5/LQTemp_rig%d.root", ftmp.c_str(), SelRig), "READ");
	TFile fL1Q(Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root", ftmp.c_str()), "READ");

    ///----- output file
	string sOutDir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/result/L1TempFitL2PDF/runQ%d", run_charge);
    string sDrawDir = Form("%s/Pic/Rig%d", sOutDir.c_str(), SelRig);
    cout << "Create Pic dir " << system(Form("mkdir -pv %s", sDrawDir.c_str())) << endl;

	double dCutL = run_charge - 0.5;
	double dCutU = run_charge + 0.5;

	for(int ich = run_charge-2; ich <= run_charge+2; ich++){
		if(ich != 12) continue;
		//--TTree
		string histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", ich,iTempLayer, iTempLayer, ich, SelRig);
		if (SelRig==-1) histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_allrig", ich,iTempLayer,iTempLayer, ich);
		TTree *tLQTemp = static_cast<TTree*>(fL1Q.Get(histn.c_str()));

		//---Hist
		histn = Form("L1Q_total_q%d_rig%d", ich, SelRig);
		TH1F *hL1QTempAll = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
		int NRebin = 5;
		double dEvL=0.5, dEvU=5*1e6;
		double dFitL = ich - 1.5;
		double dFitU = ich + 1;
		double dDrawL = ich - 4;
		double dDrawU = ich + 4;
		double dAxisL = ich - 2;
		double dAxisU = ich + 2;

		int iNVar = 3;

		//--RooWorkspace
		RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
		RooRealVar *rrvQXY = rwsLQTemp->var("UBL1QXY");
		RooDataHist *htempalte = NULL;
		RooFitResult *rfrResult;
		RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
		RooKeysPdf *rkpLQ[iNVar];
		RooRealVar *rrvshift[iNVar];
		RooRealVar *rrvn[iNVar];
		RooArgList ralLQ, raln;

		int startCh = ich - iNVar/2;

		for(int ivar = 0; ivar < iNVar; ivar++){
			//----RooKeysPdf
			histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iPDFLayer, startCh+ivar, SelRig);
			rkpLQ[ivar] = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
			//----shift variables
			rrvshift[ivar] = rwsLQTemp->var(Form("r1_q%d_xy2_shift_L%dQTemp", startCh+ivar, iPDFLayer));
			//----Fit par
			histn = Form("nparticle_q%d", startCh+ivar);
			rrvn[ivar] = new RooRealVar(histn.c_str(), histn.c_str(), 100, 0., 1e7);
			//----Add sumPDF
			ralLQ.add(*rkpLQ[ivar]);
			raln.add(*rrvn[ivar]);
		}
		histn = Form("runQ%d_q%d_rig%d_KeysPdf_sum", run_charge, ich, SelRig);
		RooAddPdf *rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), ralLQ,raln);

		hL1QTempAll->Rebin(NRebin);
		int bFitL = hL1QTempAll->FindBin(dFitL);
		int bFitU = hL1QTempAll->FindBin(dFitU);
		histn = Form("L1Q_rig%d_RangeFit", SelRig);
		TH1D *hL1QTemp= new TH1D(histn.c_str(), histn.c_str(), bFitU-bFitL+1, hL1QTempAll->GetBinLowEdge(bFitL), hL1QTempAll->GetBinLowEdge(bFitU));
		for(int ib= bFitL; ib <= bFitU; ib++){
			hL1QTemp->SetBinContent(ib-bFitL+1, hL1QTempAll->GetBinContent(ib));
		}

		double UBL1QXY = 0;
		tLQTemp->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
		long int nentries = tLQTemp->GetEntries();
		for(int ie = 0; ie < nentries; ie++){
			tLQTemp->GetEntry(ie);
			// if(UBL1QXY < dFitL || UBL1QXY > dFitU) continue;
			hL1QTempAll->Fill(UBL1QXY);
		}
		// double ymax = hL1QTempAll->GetMaximum();
		double ymax = hL1QTempAll->GetBinContent(hL1QTempAll->FindBin(ich));
		dEvU = ymax*1.5;
		htempalte = new RooDataHist(Form("template_q%d", run_charge), Form("template_q%d", run_charge), *rrvQXY, hL1QTempAll);

		rrvQXY->setRange("Fit", dFitL, dFitU);
		rrvQXY->setRange("draw", dDrawL, dDrawU);
		rrvQXY->setRange("Integral", dCutL, dCutU);
		rfrResult = rapSumpdf->fitTo(*htempalte, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));

		//----draw setting
		gStyle->SetOptStat("");
		gStyle->SetOptTitle(0);
		lTitle.SetTextAlign(21);
		lLegend.SetTextAlign(11);
		TH1::AddDirectory(kFALSE);
		int fillStyle=3001;
		histn = Form("Canv_runQ%d_q%d_Rig%d", run_charge, ich, SelRig);
		cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);
		histn = Form("Pad1_runQ%d_q%d_Rig%d", run_charge, ich, SelRig);
		pPad1 = new TPad(histn.c_str(), histn.c_str(), .0, .3, 1., 1.);
		pPad1->cd();
		// gPad->SetLogy();
		gPad->SetGridy();
		gPad->SetTicks();
		gPad->SetTopMargin(0.05);
		gPad->SetBottomMargin(0);
		gPad->SetRightMargin(0.05);
		histn = Form("Pad2_runQ%d_q%d_Rig%d", run_charge, ich, SelRig);
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
		//----Components
		for (int ivar=0; ivar<iNVar; ivar++){
			if((startCh + ivar)%2 != 1) continue;
			histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iPDFLayer, startCh+ivar, SelRig);
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(2), RooFit::LineColor(iColor[ivar]), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
		}
		for (int ivar=0; ivar<iNVar; ivar++){
			if((startCh + ivar)%2 == 1) continue;
			histn = Form("L%dQ_r1_q%d_xy2_sel1_rig%d_KeysPdf", iPDFLayer, startCh+ivar, SelRig);
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(2), RooFit::LineColor(iColor[ivar]), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
		}
		htempalte->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data
		rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));

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
		TLine lCuts_cut;
		lCuts_cut.SetLineWidth(3);
		lCuts_cut.SetLineStyle(2);
		lCuts_cut.SetLineColor(kBlack);
		lCuts_cut.DrawLine(dCutL, dEvL, dCutL, dEvU);
		lCuts_cut.DrawLine(dCutU, dEvL, dCutU, dEvU);

		///Pull Hist
		// RooHist *rhPull = rpQFrame->pullHist(nullptr, nullptr, false);
		RooHist *rhPull = rpQFrame->pullHist();
		double chis_fit = rpQFrame->chiSquare();

		lLegend.DrawLatexNDC(.15, .85, Form("#color[%d]{[Templat L%d]}", kBlack, iTempLayer));
		lLegend.DrawLatexNDC(.15, .80, Form("#color[%d]{[PDF L%d]}", kBlack, iPDFLayer));
		lLegend.DrawLatexNDC(.15, .75, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
		lLegend.DrawLatexNDC(.15, .70, Form("#color[%d]{Fit range:[%.2f, %.2f]}", kBlack, dFitL, dFitU));
		lLegend.DrawLatexNDC(.15, .65, Form("#color[%d]{Chisquare:%.2f}", kBlack, chis_fit));
		for (int ivar=0; ivar<iNVar; ivar++){
			lLegend.DrawLatexNDC(.15, .60-.05*ivar, Form("#color[%d]{N_{%d}:%.0f}", iColor[ivar], startCh+ivar, rrvn[ivar]->getVal()));
			lLegend.DrawLatexNDC(.25, .60-.05*ivar, Form("#color[%d]{Shift:%.4f}", iColor[ivar], rrvshift[ivar]->getVal()));
		}

		//------calculate containmination based on fitting
		cout << "****** calculating containmination ******" << endl;
		RooAbsReal *rarIntAll = rapSumpdf->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral")); //set the pdf to normalized in [0, 35]
		RooAbsReal *rarInt[iNVar];
		double sumrrvn = 0;
		for (int ivar=0; ivar<iNVar; ivar++){
			rarInt[ivar] = rkpLQ[ivar]->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral"));
			sumrrvn += rrvn[ivar]->getVal();
		}
		lLegend.DrawLatexNDC(.70, .80, Form("#color[%d]{Cut range:[%.2f, %.2f]}", kBlack, dCutL, dCutU));
		for (int ivar=0; ivar<iNVar; ivar++){
			double dFrac = (rrvn[ivar]->getVal() * rarInt[ivar]->getVal()) / (sumrrvn * rarIntAll->getVal());
			lLegend.DrawLatexNDC(.70, .75-ivar*0.05, Form("#color[%d]{Q%d}: %.2f%%", iColor[ivar], startCh+ivar, dFrac*100.));
		}

		//----Draw Pull
		pPad2->cd();
		rpPullFrame = rrvQXY->frame(dDrawL,dDrawU);
		rpPullFrame->addPlotable(rhPull,"PZ");
		rpPullFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
		rpPullFrame->GetXaxis()->CenterTitle();
		rpPullFrame->GetXaxis()->SetTitleSize(0.1);
		rpPullFrame->GetXaxis()->SetTitleOffset(0.7);
		rpPullFrame->GetXaxis()->SetLabelSize(0.07);
		rpPullFrame->GetXaxis()->SetRangeUser(dAxisL,dAxisU);
		rpPullFrame->GetYaxis()->SetTitle("Pull");
		rpPullFrame->GetYaxis()->CenterTitle();
		rpPullFrame->GetYaxis()->SetTitleSize(0.1);
		rpPullFrame->GetYaxis()->SetTitleOffset(0.4);
		rpPullFrame->GetYaxis()->SetLabelSize(0.07);

		rpPullFrame->DrawClone();
			
		cCanv->cd();
		pPad1->DrawClone();
		pPad2->DrawClone();
		histn = Form("%s/runQ%d_q%d_rig%d_L%dTemp_L%dPDF.png", sDrawDir.c_str(), run_charge, ich, SelRig, iTempLayer, iPDFLayer);
		cCanv->SaveAs(histn.c_str());
	}

	fQTemp.Close();
}