#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

const int iNBL1Temp=L1QTF_NbinAllQ2;
double *dBL1Temp=L1QTF_BinsAllQ2;

const int iNBL1Fit=L1QTF_NbinSec2;
double *dBL1Fit=L1QTF_BinsSec2;

const string wDir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/HistPDF";
TH2F *hQ2dDis=NULL;
TH1D *hQDisD = NULL;

int HistPdfRead_Sur(int iSelRig=20,
				const int run_charge=14, 
				const int QVer=1,
				const int NVar0=-1,
				const int ixy=2,
				const int il=1,
				const int ispan=1,
				const bool bBadLadder=false)
{
    //--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1Fit, dBL1Fit);
	const double dLRig=hL1QFit.GetBinLowEdge(iSelRig+1), dURig=hL1QFit.GetBinLowEdge(iSelRig+2);
	cout << "OK set fit bin" << endl; //XXX
	//--binning for template
	TH1F hL2QTemp("hL2QTemp", "hL2QTemp", iNBL1Temp, dBL1Temp);
	const int iRigTemp = hL2QTemp.FindBin(hL1QFit.GetBinCenter(iSelRig+1))-1;
	const double dLRigTemp=hL2QTemp.GetBinLowEdge(iRigTemp+1), dURigTemp=hL2QTemp.GetBinLowEdge(iRigTemp+2);
	
	cout << "OK set temp bin" << endl; //XXX
	cout << " *** Fit ibin=" << iSelRig << " temp ibin=" << iRigTemp << " *** " << endl;
	cout << "iSelRig=" << iSelRig << " dLRig=" << dLRig << " dURig=" << dURig << " iRigTemp=" << iRigTemp << endl;

    //----parameter
	string sDir=Form("/Charge_Calibration%d", QVer);
    int iSelTemp = 1;
    //----
	int iNVar = 5;
	if (NVar0!=-1) iNVar=NVar0;
    int NRebin = 5;
    //----
    int Charge = run_charge - iNVar/2;
    double dCutL=0, dCutU=0;
	double dDrawL=0, dDrawU=0; //draw range for template fit
	double dDrawBigL=0, dDrawBigU=0; //big draw range, used for LQ template
	double dEvL=0.5, dEvU=0;
    //----
    double dFitL=Charge - 0.2;
	double dFitU=(Charge+iNVar-1) + 0.6;
	double dDataL=Charge - 0.6;
	double dDataU=run_charge + 2.7;
    //---
	dCutL = run_charge - 0.5; dCutU = run_charge + 0.5;
	dDrawL = run_charge - 8; dDrawU = run_charge + 10.5;
    dDrawBigL=run_charge-2, dDrawBigU=run_charge+6.5;
    //---
    double dAxisL = dFitL - 0.5;
	double dAxisU = dFitU + 0.5;
    double dTempFitL=0, dTempFitU=35;

    //---
    //----draw setting
	gStyle->SetOptStat("");
	gStyle->SetOptTitle(0);
	lTitle.SetTextAlign(21);
	lLegend.SetTextAlign(11);
	TH1::AddDirectory(kFALSE);
    int fillStyle=3001;
    //---
    TLine lCuts;
	lCuts.SetLineWidth(3);
	lCuts.SetLineStyle(2);
	TLine lCutsfit;
	lCutsfit.SetLineWidth(3);
	lCutsfit.SetLineStyle(2);
	lCutsfit.SetLineColor(kRed);


    RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
    RooDataHist *rdhL1Q=NULL;
	RooFitResult *rfrResult;

	// TFile fL1Q(Form("%s/InputFile/amsd69nChenYa_B1130_SurvivalProbability_noStroeBadL1Ladder.root", sTotalDir.c_str()), "READ");
	TFile fL1Q(Form("/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/L1QDistribution/root/ISS/ForceNewTrig/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_CorrSel2.root"), "READ");

    TFile fTotalFile(Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root", wDir.c_str()), "READ");

    RooRealVar *rrvQXY = new RooRealVar("UBL1QXY","UBL1QXY", dTempFitL, dTempFitU);
    rrvQXY->setRange("Integral", dCutL, dCutU);
    rrvQXY->setRange("Fit", dFitL, dFitU);
    rrvQXY->setRange("Norm", dTempFitL, dTempFitU);
    rrvQXY->setRange("draw", dDrawL, dDrawU);

    int ir = ispan;
    string sDraw = Form("%s/OutputFile/r%d_xy%d_NVar%d_rebin%d_q%d", wDir.c_str(), ir, ixy, iNVar, NRebin, run_charge);
    string sDrawL1QTempFit = Form("%s/L1QTempFit_L%dQTemp", sDraw.c_str(), il);
    cout << "Create pic directory  for L1QTempFit " << system(Form("mkdir -pv %s", sDrawL1QTempFit.c_str())) << endl;
    string sDrawRoot = Form("%s/pic", sDrawL1QTempFit.c_str());
    cout << "Create draw root pic " << system(Form("mkdir -pv %s", sDrawRoot.c_str())) << endl;

    cout << "****** Performing L1Q template fit ******" << endl;
    //------L1Q template fit
    histn = Form("L1Q_r%d_q%d_xy%d_rig%d", ir, run_charge, ixy, iSelRig);
    cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);

    pPad1 = new TPad(histn.c_str(), histn.c_str(), .0, .3, 1., 1.);
    pPad1->cd();
    //gPad->SetGrid();
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

    histn = Form("L1L2Sur4/q%d/%s/period1/tofqcut0/L1QvsRig_r%d_q%d_L1L2Sur_noL1Qcut_period1_tofqcut0", 
										run_charge, sSpan[ir].c_str(), ir, run_charge);
    hQ2dDis = static_cast<TH2F*>(fL1Q.Get(histn.c_str()));
    cout << iSelRig << "  [" << dLRig << "," << dURig << "]" << endl;
    cout << hQ2dDis->GetXaxis()->FindBin(dLRig) << " to " << hQ2dDis->GetXaxis()->FindBin(dURig)-1 << endl;

    histn = Form("L1Q_r%d_rig%d", ir, iSelRig);
    hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRig), hQ2dDis->GetXaxis()->FindBin(dURig)-1, "e");
    if (hQDisD->GetNbinsX()==3500) hQDisD->Rebin(NRebin);

    histn = Form("L1Q_r%d_q%d_xy%d_L1QSam_rig%d", ir, run_charge, ixy, iSelRig);
    rdhL1Q = new RooDataHist(histn.c_str(), histn.c_str(), RooArgList(*rrvQXY), hQDisD);
    rdhL1Q->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data L1Q
    cout << "******Finish plotting L1Q******" << endl;

    double maxBin = 0;
    for (int ivar=0; ivar<iNVar; ivar++){
        double curBin = hQDisD->GetBinContent(hQDisD->FindBin(Charge+ivar));
        if(maxBin < curBin) maxBin = curBin;
    }
    dEvU = maxBin * 1.8;

    //--sum pdf
    //----build fraction variable
    double rNum[iNVar];
    for(int ivar=0; ivar<iNVar; ivar++){
        rNum[ivar] = 0;
        double x12 = 0.5;
        if((Charge+ivar)%2 == 1) x12 = 0.4;
        int xb1 = hQDisD->FindBin((Charge+ivar)-x12);
        int xb2 = hQDisD->FindBin((Charge+ivar)+x12);
        for(int ix = xb1; ix <= xb2; ix++) rNum[ivar] += hQDisD->GetBinContent(ix);
        cout << "\t" << ivar+Charge << "\t" << rNum[ivar] << endl;
    }
    RooRealVar *rrvn[iNVar];
    for (int ivar=0; ivar<iNVar; ivar++)
    {
        int  icharge = Charge + ivar;
        histn = Form("n%d", ivar);
        double dIni, dL, dU;
        dIni = 0.96*rNum[ivar], dL = 0.8 * rNum[ivar], dU = 1.2 * rNum[ivar];
        if(icharge % 2 == 1){
            dIni = 0.8*rNum[ivar], dL = 0.4 * rNum[ivar], dU = 0.99 * rNum[ivar];
        }
        if(icharge == 12) dIni = 1.01*rNum[ivar], dL = 0.9 * rNum[ivar], dU = 1.25 * rNum[ivar];
        if(icharge == 13) dIni = 0.95*rNum[ivar], dL = 0.5 * rNum[ivar], dU = 1.1 * rNum[ivar];
        if(icharge == 14) dIni = 0.95*rNum[ivar], dL = 0.9 * rNum[ivar], dU = 1.25 * rNum[ivar];
        if(icharge == 15) dIni = 0.45*rNum[ivar], dL = 0.2 * rNum[ivar], dU = 0.9 * rNum[ivar];
        if(icharge == 16) dIni = 0.9*rNum[ivar], dL = 0.8 * rNum[ivar], dU = 1.5 * rNum[ivar];
        rrvn[ivar] = new RooRealVar(histn.c_str(), histn.c_str(), dIni, dL, dU);
    } //ivar

    //----retrieve RooHistPdf for each charge
    RooDataHist *htempalte[iNVar];
    RooHistPdf *rkpLQ[iNVar];
    for (int ivar=0; ivar<iNVar; ivar++){
        int ich = Charge + ivar;
        histn = Form("%s/q%d/%s/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d", sDir.c_str(), ich, cSpan[ir], il, il, ir, ich, iSelTemp, iRigTemp);
        if (iRigTemp==-1) histn = Form("%s/q%d/%s/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", sDir.c_str(), ich, cSpan[ir], il, il, ir, ich, iRigTemp);
        cout << histn << endl;
        TTree *tLQTemp = static_cast<TTree*>(fTotalFile.Get(histn.c_str()));
        double UBL1QXY;
        tLQTemp->SetBranchAddress("UBL1QXY", &UBL1QXY);

        histn = Form("L%dQ_r%d_q%d_rig%d_hist", il,ir,ich,iRigTemp);
        TH1F *hLQTemp = new TH1F(histn.c_str(),histn.c_str(),hQDisD->GetNbinsX(), 0, 35);
        long int nentries = tLQTemp->GetEntries();
        for(int ie = 0; ie < nentries; ie++){
            tLQTemp->GetEntry(ie);
            hLQTemp->Fill(UBL1QXY);
        }
        histn = Form("L%dQ_r%d_q%d_rig%d_DataHist", il,ir,ich,iRigTemp);
        htempalte[ivar] = new RooDataHist(histn.c_str(),histn.c_str(), *rrvQXY, hLQTemp);
        histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_HistPdf", il, ir, Charge+ivar, ixy, iSelTemp, iRigTemp);
        rkpLQ[ivar] = new RooHistPdf(histn.c_str(),histn.c_str(), *rrvQXY, *htempalte[ivar]);
    }

    histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_HistPdf_sum", il, ir, run_charge, ixy, iSelTemp, iSelRig);
    RooAddPdf *rapSumpdf=NULL;
    RooArgList ralLQ, raln;
    for (int ivar=0; ivar<iNVar; ivar++)
    {
        ralLQ.add(*rkpLQ[ivar]);
        raln.add(*rrvn[ivar]);
    }
    rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), ralLQ,raln);

    cout << "start performing fit, fit range: [" << dFitL << "," << dFitU << "]" << endl;
    rfrResult = rapSumpdf->fitTo(*rdhL1Q, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(0));
    cout << "Finish performing fit" << endl;

    //------plot the fit result
    //----sum pdf
    rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));
    double sumrrvn = 0;
    for (int ivar=0; ivar<iNVar; ivar++){
        sumrrvn += rrvn[ivar]->getVal();
    }
    //----chisquare & pull
    RooHist *rhPull = rpQFrame->pullHist();
    double chis_fit = rpQFrame->chiSquare(nullptr, nullptr, 2);

    // //----components
    for (int ivar=0; ivar<iNVar; ivar++) ///even charge
    {
        if ((Charge+ivar)%2==1) continue;
        histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_HistPdf", il, ir, Charge+ivar, ixy, iSelTemp, iRigTemp);
        rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
    }
    for (int ivar=0; ivar<iNVar; ivar++) ///odd charge
    {
        if ((Charge+ivar)%2==0) continue;
        histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_HistPdf", il, ir, Charge+ivar, ixy, iSelTemp, iRigTemp);
        rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
    }

    rpQFrame->GetXaxis()->SetTitleSize(0.05);
    rpQFrame->GetXaxis()->SetTitleOffset(0.8);
    rpQFrame->GetXaxis()->SetRangeUser(dAxisL,dAxisU);
    rpQFrame->GetYaxis()->SetTitle("Events"); //2020.10.26
    rpQFrame->GetYaxis()->CenterTitle();
    rpQFrame->GetYaxis()->SetTitleSize(0.05);
    rpQFrame->GetYaxis()->SetTitleOffset(0.8);
    rpQFrame->SetMinimum(dEvL);
    rpQFrame->SetMaximum(dEvU);
    rpQFrame->DrawClone();

    lCuts.DrawLine(dCutL, dEvL, dCutL, dEvU);
    lCuts.DrawLine(dCutU, dEvL, dCutU, dEvU);
    lCutsfit.DrawLine(dFitL, dEvL, dFitL, dEvU);
    lCutsfit.DrawLine(dFitU, dEvL, dFitU, dEvU);
    lLegend.DrawLatexNDC(.20, .90, Form("#color[%d]{Cut Range[%.2f, %.2f]}", kBlack, dCutL, dCutU));
    lLegend.DrawLatexNDC(.20, .85, Form("#color[%d]{Fit Range[%.2f, %.2f]}", kRed, dFitL, dFitU));
    lLegend.DrawLatexNDC(.65, .90, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));

    //------calculate containmination based on fitting
    cout << "****** calculating containmination ******" << endl;
    RooAbsReal *rarIntAll = rapSumpdf->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral")); //set the pdf to normalized in [0, 35]
    RooAbsReal *rarInt[iNVar];
    for (int ivar=0; ivar<iNVar; ivar++)
    {
        rarInt[ivar] = rkpLQ[ivar]->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral"));
        
        double dFrac=1;
        double dNprt=rrvn[ivar]->getVal();
        dFrac = (dNprt * rarInt[ivar]->getVal()) / (sumrrvn * rarIntAll->getVal());
        
        if (Charge+ivar==run_charge) lLegend.DrawLatexNDC(.20, .80-ivar*0.05, Form("#color[%d]{Q%d}: %.2f%%", iColor[ivar], Charge+ivar, dFrac*100.));
        else lLegend.DrawLatexNDC(.20, .80-ivar*0.05, Form("#color[%d]{Q%d}: %.2f%%", iColor[ivar], Charge+ivar, dFrac*100.));
    } //ivar

    lLegend.DrawLatexNDC(.65, .85, Form("#color[%d]{Chisquare:%.2f}", kBlack, chis_fit));
    for (int ivar=0; ivar<iNVar; ivar++){
        lLegend.DrawLatexNDC(.65, .80-ivar*0.05, Form("#color[%d]{N_{%d}}: %.0f", iColor[ivar], Charge+ivar, rrvn[ivar]->getVal()));
    }
    for (int ivar=0; ivar<iNVar; ivar++)
    {
        cout << "\t\t q_" << Charge+ivar << " \t" << rrvn[ivar]->getVal() / rNum[ivar] << endl;
    } //ivar

    //----Draw Pull
    pPad2->cd();
    rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
    rpPullFrame->addPlotable(rhPull,"P");
    rpPullFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
    rpPullFrame->GetXaxis()->CenterTitle();
    rpPullFrame->GetXaxis()->SetRangeUser(dAxisL,dAxisU);
    rpPullFrame->GetYaxis()->SetRangeUser(-10,10);
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

    histn = Form("%s/pic/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig, iNVar);
    cCanv->SaveAs(Form("%s.png", histn.c_str()));

    fTotalFile.Close();

    return 0;
}