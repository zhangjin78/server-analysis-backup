#include "TTree.h"
using namespace RooFit;

void fitL2(){
    TFile *file1 = new TFile("/home/ams/jzhang/AMS/Flux/Sulfur/plot/SurvivalProbability/root/ISS/P8_SurvivalProbability_test.root"); /// ISS
    TH2F* h1p = (TH2F*)file1->Get("L1L2Sur4/q9/L1Inner/period1/tofqcut0/L1QvsRig_r1_q9_L1L2Sur_noL1Qcut_period1_tofqcut0");

    TH1D *hdata = h1p->ProjectionY("hdata", 37, 37);  ///28.8 GV-31.1

    const int nelement = 8;
    const char element[nelement][1000] = {"O", "F", "Ne", "Na", "Mg", "Al", "Si", "N"};
    TTree *tKeysPdf[nelement];
    TH1F *hL2pdf[nelement];

    TFile *filetemp = new TFile("/home/ams/jzhang/AMS/Flux/Sulfur/testFlux/output/CHARGECAL_NEWTRQ_test/CHARGECAL_NEWTRQ_test.root"); /// ISS tempalte fit
    tKeysPdf[0] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q8/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q8_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[1] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q9/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q9_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[2] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q10/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q10_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[3] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q11/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q11_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[4] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q12/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q12_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[5] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q13/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q13_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[6] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q14/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q14_sel1_rig21")); ///28.8GV-31.1GV
    tKeysPdf[7] = static_cast<TTree*>(filetemp->Get("Charge_Calibration1/q7/L1Inner/2.0sigma/L2Q_KeysPdf/L2Q_r1_q7_sel1_rig21")); ///28.8GV-31.1GV

    RooRealVar x("x","L1_Charge", 7, 14);
    // RooRealVar x("x","L1_Charge", 8.5, 9.5);
    
    RooDataHist *htempalte[nelement];
    RooHistPdf *pdf[nelement];
    RooRealVar *fitpar[nelement-1];

    RooDataHist rooData("L1_ChargeDis", "L1_ChargeDis", x, hdata);
    RooArgList ralLQ, raln;

    for(int im = 0; im < nelement; im++){
        double L2QXY = 0;

        hL2pdf[im] = new TH1F(element[im], element[im], 3500, 0, 35);

        tKeysPdf[im]->SetBranchAddress("L1QXY", &L2QXY);

        long int nentries = tKeysPdf[im]->GetEntries();

        for(int ie = 0; ie < nentries; ie++){
            tKeysPdf[im]->GetEntry(ie);
            hL2pdf[im]->Fill(L2QXY);
        }

        htempalte[im] = new RooDataHist(Form("template%s", element[im]), Form("template%s", element[im]), x, hL2pdf[im]);
        pdf[im] = new RooHistPdf(Form("pdf%s", element[im]), Form("pdf%s", element[im]), x, *htempalte[im]);

        ralLQ.add(*pdf[im]);
        if(im == nelement-1) continue;
        fitpar[im] = new RooRealVar(Form("para%s", element[im]), Form("para%s", element[im]), 0.01, 0, 1);
        raln.add(*fitpar[im]);
    }

    // RooAddPdf model("model","model",RooArgList(*pdf[0],*pdf[1],*pdf[2],*pdf[3],*pdf[4],*pdf[5],*pdf[6]),RooArgList(*fitpar[0],*fitpar[1],*fitpar[2],*fitpar[3],*fitpar[4],*fitpar[5],*fitpar[6]));
    RooAddPdf model("model","model",ralLQ,raln);

    RooFitResult *fitResult = model.fitTo( rooData);

    TString varname = x.GetName();
    // TH1 *hdata2 = dynamic_cast<TH1*>(rooData.createHistogram(varname.Data(), 600));
    // TH1 *hmodle = dynamic_cast<TH1*>(model.createHistogram(varname.Data(), 600));
    // TH1 *hO = dynamic_cast<TH1*>(pdf[0]->createHistogram(varname.Data(), 600));
    // TH1 *hF = dynamic_cast<TH1*>(pdf[1]->createHistogram(varname.Data(), 600));
    // TH1 *hNe = dynamic_cast<TH1*>(pdf[2]->createHistogram(varname.Data(), 600));
    // TH1 *hNa = dynamic_cast<TH1*>(pdf[3]->createHistogram(varname.Data(), 600));
    // TH1 *hMg = dynamic_cast<TH1*>(pdf[4]->createHistogram(varname.Data(), 600));
    // TH1 *hAl = dynamic_cast<TH1*>(pdf[5]->createHistogram(varname.Data(), 600));
    // TH1 *hSi = dynamic_cast<TH1*>(pdf[6]->createHistogram(varname.Data(), 600));

    // double set_alpha = 0.35;

    // hmodle->SetLineColor(kRed);
    // hmodle->SetLineWidth(2);

    // hO->SetLineColor(7);
    // hO->SetFillColorAlpha(7, set_alpha);
    // hF->SetLineColor(4);
    // hF->SetFillColorAlpha(4, set_alpha);
    // hNe->SetLineColor(6);
    // hNe->SetFillColorAlpha(6, set_alpha);
    // hNa->SetLineColor(9);
    // hNa->SetFillColorAlpha(9, set_alpha);
    // hMg->SetLineColor(8);
    // hMg->SetFillColorAlpha(8, set_alpha);
    // hAl->SetLineColor(12);
    // hAl->SetFillColorAlpha(12, set_alpha);
    // hSi->SetLineColor(28);
    // hSi->SetFillColorAlpha(28, set_alpha);

    RooPlot* xframe = x.frame();
    rooData.plotOn(xframe);
    model.plotOn(xframe,RooFit::LineColor(kRed));
    model.plotOn(xframe,RooFit::Components("pdfO"),RooFit::LineColor(7));
    model.plotOn(xframe,RooFit::Components("pdfF"),RooFit::LineColor(4));
    model.plotOn(xframe,RooFit::Components("pdfNe"),RooFit::LineColor(6));
    model.plotOn(xframe,RooFit::Components("pdfNa"),RooFit::LineColor(9));
    model.plotOn(xframe,RooFit::Components("pdfMg"),RooFit::LineColor(8));
    model.plotOn(xframe,RooFit::Components("pdfAl"),RooFit::LineColor(12));
    model.plotOn(xframe,RooFit::Components("pdfSi"),RooFit::LineColor(28));
    model.plotOn(xframe,RooFit::Components("pdfN"),RooFit::LineColor(22));

    xframe->GetYaxis()->SetRangeUser(0.5, 100000);
    xframe->GetXaxis()->SetRangeUser(7, 14);
    xframe->GetXaxis()->SetTitle("L1 Charge");
    xframe->GetXaxis()->CenterTitle(true);
    xframe->GetYaxis()->SetTitle("Events");
    xframe->GetYaxis()->CenterTitle(true);

    TCanvas *c1 = new TCanvas("c1","compare",900,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    // gPad->SetGrid();
    c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.15);
    // c1->SetLogz();
    // c1->SetLogx();
    c1->SetLogy();
    // c1->SetFillColor(19);

    xframe->Draw();
    // hdata2->Draw();
    // hmodle->Draw("same");
    // hO->Draw("HIST same");

    double xlimit1 = 8.5;
    double xlimit2 = 9.5;
    TLine * line1 = new TLine(xlimit1, 0.5, xlimit1, 100000);
    line1->SetLineWidth(3);
    line1->SetLineColor(kBlack);
    line1->Draw();
    TLine * line2 = new TLine(xlimit2, 0.5, xlimit2, 100000);
    line2->SetLineWidth(3);
    line2->SetLineColor(kBlack);
    line2->Draw();


    ///

    double factor_O = fitpar[0]->getVal();
    double factor_F = fitpar[1]->getVal();
    double factor_Ne = fitpar[2]->getVal();
    double factor_Na = fitpar[3]->getVal();
    double factor_Mg = fitpar[4]->getVal();
    double factor_Al = fitpar[5]->getVal();
    double factor_Si = fitpar[6]->getVal();
    // double factor_N = fitpar[6]->getVal();
    double s1 = 0;
    double s2 = 0;
    double s3 = 0;
    double s4 = 0;
    for(int j = 1; j <= hL2pdf[0]->GetNbinsX(); j++){
        s1 += hL2pdf[0]->GetBinContent(j);
        s2 += hL2pdf[1]->GetBinContent(j);
        s3 += hL2pdf[2]->GetBinContent(j);
        s4 += hL2pdf[3]->GetBinContent(j);
    }

    int xbin1 = hdata->FindBin(8.5);
    int xbin2 = hdata->FindBin(9.5)-1;
    double sum1 = hdata->Integral(xbin1, xbin2);
    // double sum1 = factor_O+factor_F+factor_Ne+factor_Na;
    xbin1=hL2pdf[0]->FindBin(8.5);
    xbin2=hL2pdf[0]->FindBin(9.5)-1;
    cout << xbin1 << "\t" << xbin2 << endl;
    double sum2 = hL2pdf[0]->Integral(xbin1, xbin2) / s1;
    xbin1=hL2pdf[1]->FindBin(8.5);
    xbin2=hL2pdf[1]->FindBin(9.5)-1;
    cout << xbin1 << "\t" << xbin2 << endl;
    double sum3 = hL2pdf[1]->Integral(xbin1, xbin2) / s2;
    xbin1=hL2pdf[2]->FindBin(8.5);
    xbin2=hL2pdf[2]->FindBin(9.5)-1;
    cout << xbin1 << "\t" << xbin2 << endl;
    double sum4 = hL2pdf[2]->Integral(xbin1, xbin2) / s3;
    xbin1=hL2pdf[3]->FindBin(8.5);
    xbin2=hL2pdf[3]->FindBin(9.5)-1;
    cout << xbin1 << "\t" << xbin2 << endl;
    double sum5 = hL2pdf[3]->Integral(xbin1, xbin2) / s4;

    x.setRange("Integral", 8.5,9.5);

    RooAbsReal *a_0 = pdf[0]->createIntegral(x, NormSet(x), Range("Integral"));
    RooAbsReal *a_1 = pdf[1]->createIntegral(x, NormSet(x), Range("Integral"));
    RooAbsReal *a_2 = pdf[2]->createIntegral(x, NormSet(x), Range("Integral"));
    RooAbsReal *sumd = model.createIntegral(x, NormSet(x), Range("Integral"));

    double sumevent = sumd->getVal();
    double a0 = a_0->getVal();
    double a1 = a_1->getVal();
    double a2 = a_2->getVal();
    // cout << sum1 << endl;
    cout << sumevent << endl;
    cout << a0 << "\t" << a1 << "\t" << a2 << endl;
    cout << a0*factor_O/sumevent << "\t" << a1*factor_F/sumevent << "\t" << a2*factor_Ne/sumevent << endl;
}
