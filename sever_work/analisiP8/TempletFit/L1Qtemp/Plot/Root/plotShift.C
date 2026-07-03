string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/Root";
const int nRebin = 5;

const int L1QTF_NbinAllQ2 = 23;
double L1QTF_BinsAllQ2[L1QTF_NbinAllQ2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,3000};
const int L1QTF_NbinSec2 = 36;
double L1QTF_BinsSec2[L1QTF_NbinSec2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,45.1,64.8,93.0,108.,125.,147.,175.,
211.,259.,330.,441.,660.,1200,3000
};

const int ncolor[3] = {kBlack, kBlue, kRed};
const int nfile = 20;
string infile[nfile];
TFile *inRoot[nfile];

void CompareShift(const int ch1 = 12, const int ch2 = 14);
void CompareShift_CutHe(const int ifile1 = 0, const int ifile2 = 6, const int run_charge = 14);
void L1TempShift(const int run_charge = 14);
void L1TempShift_CutHe(const int run_charge = 14);
void L1DistributionShift(const int run_charge = 14);
void L1DistributionShift_TransRootCutHe(const int run_charge = 14);

void plotShift(){
    infile[0] = Form("%s/TempShift/TempFitShift_r1_L1_Q10To18.root", wdir.c_str());
    infile[1] = Form("%s/TempShift/TempFitShift_r1_L2_Q10To18.root", wdir.c_str());
    infile[2] = Form("%s/QDisShift/QDisShift_r1_L1_Q12To16.root", wdir.c_str());
    infile[3] = Form("%s/QDisShift/QDisShift_r1_L2_Q12To16.root", wdir.c_str());
    infile[4] = Form("%s/QDisShift/TempFit/r1_xy2_NVar5_rebin5_q14_L1QTemp.root", wdir.c_str());
    infile[5] = Form("%s/QDisShift/TempFit/r1_xy2_NVar5_rebin5_q14_L2QTemp.root", wdir.c_str());
    infile[6] = Form("%s/CutHe/TempShift/CutHe_TempFitShift_L1_Q12T16.root", wdir.c_str());
    infile[7] = Form("%s/CutHe/TempShift/CutHe_TempFitShift_L2_Q12T16.root", wdir.c_str());
    infile[8] = Form("%s/FitMCpdf_CutHe/DataTempFit/DataTempFit_CutHe_L1_Q12T16.root", wdir.c_str());
    infile[9] = Form("%s/FitMCpdf_CutHe/DataTempFit/DataTempFit_CutHe_L2_Q12T16.root", wdir.c_str());
    infile[10] = Form("%s/FitMCpdf_CutHe/MCTempFit/MCTempFit_CutHe_L1_Q12T16.root", wdir.c_str());
    infile[11] = Form("%s/FitMCpdf_CutHe/MCTempFit/MCTempFit_CutHe_L2_Q12T16.root", wdir.c_str());
    infile[12] = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/OutputFile/CutHe/Interplote/FreeShift/r1_xy2_NVar5_rebin5_q14/CutHe_FreeShift_r1_xy2_NVar5_rebin5_q14_L1QTemp.root");

    for(int i = 0; i < nfile; i++) {
        if(infile[i].empty()) continue;
        inRoot[i] = new TFile(infile[i].c_str());
    }

    // for(int ich = 12; ich < 17; ich++) L1TempShift_CutHe(ich);
    // CompareShift_CutHe();

    // for(int ich = 12; ich < 17; ich++) L1TempShift(ich);
    // CompareShift();
    // for(int ich = 12; ich < 17; ich++) L1DistributionShift(ich);
    for(int ich = 12; ich < 17; ich++) L1DistributionShift_TransRootCutHe(ich);
}

void L1TempShift(const int run_charge = 14){
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraphErrors *gr[2];
    TH1F *hShift[2];
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        int iFile = (itri == 0) ? 0 : 1; //TempShift
        // int iFile = (itri == 0) ? 2 : 3; //QDisShift
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            histn = Form("Shift/Shift_r1_q%d", run_charge);
            hShift[itri] = static_cast<TH1F*>(inRoot[iFile]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hShift[itri]->GetBinContent(iselrig+1));
            gr[itri]->SetPointError(iselrig, 0, hShift[itri]->GetBinError(iselrig+1));
        }
    }

    histn = Form("c1_q%d", run_charge);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.15);
    c1->SetBottomMargin(0.13);
    c1->SetLogx();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.3;
    double tFoffsety = 1.2;
    string htitle = Form("L1Q=%d, Shift between Template and PDF", run_charge);
    // string htitle = Form("L1Q=%d, Shift between Charge Distribution and PDF", run_charge);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Shift");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    hbk->GetYaxis()->SetRangeUser(-0.1, 0.1);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.45, 0.63, 0.82, 0.78);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        legend->AddEntry(gr[itri], Form("L%dPDF and L%dTemp",itri+1,itri+1), "p");
        // legend->AddEntry(gr[itri], Form("L%dPDF",itri+1,itri+1), "p");
    }
    legend->Draw("");

    string sPic = Form("%s/ShiftPic/TempShift", wdir.c_str());
    // string sPic = Form("%s/ShiftPic/QDisShift", wdir.c_str());
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/runQ%d.png", sPic.c_str(), run_charge));
}

void CompareShift(const int ch1 = 12, const int ch2 = 14){
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    TH1F *hbk2 = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraphErrors *gr[2];
    TGraphErrors *grDiff = new TGraphErrors();
    TH1F *hShift[2];
    double sumBin = 0;
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            int ich = (itri == 0)? ch1 : ch2;
            int ilFile = 2;
            histn = Form("Shift/Shift_r1_q%d", ich);
            hShift[itri] = static_cast<TH1F*>(inRoot[ilFile]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hShift[itri]->GetBinContent(iselrig+1));
            gr[itri]->SetPointError(iselrig, 0, hShift[itri]->GetBinError(iselrig+1));

            if(itri==0) sumBin += hShift[itri]->GetBinContent(iselrig+1);

            if(itri == 1){
                double d1 = hShift[0]->GetBinContent(iselrig+1);
                double d2 = hShift[1]->GetBinContent(iselrig+1);
                double e1 = hShift[0]->GetBinError(iselrig+1);
                double e2 = hShift[1]->GetBinError(iselrig+1);
                grDiff->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), d1-d2);
                grDiff->SetPointError(iselrig, 0, sqrt(e1*e1 + e2*e2));
            }
        }
    }

    cout << "\t the shift average:" << sumBin/L1QTF_NbinAllQ2 << endl;

    histn = Form("c1");
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    // c1->SetRightMargin(0.15);
    // c1->SetBottomMargin(0.13);
    c1->SetLogx();
    TPad *bot_pad = new TPad("bot_pad", "bot_pad",0.0,0.0,1.0,0.4);
    bot_pad->Draw();
    TPad *top_pad = new TPad("top_pad", "top_pad",0.0,0.4,1.0,1.0);
    top_pad->Draw();

    top_pad->cd();
    gStyle->SetErrorX(0);
    top_pad->SetGrid();
    top_pad->SetLogx();
    top_pad->SetLeftMargin(0.15);
    top_pad->SetBottomMargin(0);

    // string htitle = Form("Shift between Template and PDF for L1");
    string htitle = Form("Shift between Charge Distribution and PDF for L1");
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(0.06);
    hbk->GetXaxis()->SetLabelSize(0.06);
    hbk->GetXaxis()->SetTitleOffset(1.3);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Shift");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    hbk->GetYaxis()->SetRangeUser(-0.1, 0.1);
    hbk->GetYaxis()->SetTitleSize(0.06);
    hbk->GetYaxis()->SetLabelSize(0.06);
    hbk->GetYaxis()->SetTitleOffset(0.8);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.45, 0.63, 0.6, 0.78);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        int ich = (itri == 0)? ch1 : ch2;
        legend->AddEntry(gr[itri], Form("L1Q = %d",ich), "p");
    }
    legend->Draw("");

    bot_pad->cd();
    bot_pad->SetGrid();
    bot_pad->SetLogx();
    bot_pad->SetTopMargin(0);
    bot_pad->SetLeftMargin(0.15);
    bot_pad->SetBottomMargin(0.25);
    hbk2->SetTitle("");
    hbk2->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk2->GetXaxis()->SetMoreLogLabels(true);
    hbk2->GetXaxis()->SetTitleSize(0.08);
    hbk2->GetXaxis()->SetLabelSize(0.08);
    hbk2->GetXaxis()->SetTitleOffset(1.3);
    hbk2->GetXaxis()->CenterTitle(true);
    hbk2->GetXaxis()->SetRangeUser(2, 3000);
    hbk2->GetYaxis()->SetTitle(Form("Shift_{%d}-Shift_{%d}",ch1,ch2));
    hbk2->GetYaxis()->SetRangeUser(-0.05, 0.05);
    hbk2->GetYaxis()->SetTitleSize(0.08);
    hbk2->GetYaxis()->SetLabelSize(0.08);
    hbk2->GetYaxis()->SetTitleOffset(0.6);
    hbk2->GetYaxis()->CenterTitle(true);
    hbk2->Draw();

    grDiff->SetMarkerStyle(20);
    grDiff->SetMarkerColor(kRed);
    grDiff->SetLineColor(kRed);
    grDiff->Draw("same PEZ");

    // string sPic = Form("%s/ShiftPic/TempShift/Compare", wdir.c_str());
    string sPic = Form("%s/ShiftPic/QDisShift/Compare", wdir.c_str());
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/Shift_diff.png", sPic.c_str()));
}

void L1DistributionShift(const int run_charge = 14){
    const int iLayer = 1;
    TH1F *hbk = new TH1F("","", L1QTF_NbinSec2, L1QTF_BinsSec2);
    string histn;
    TGraphErrors *gr[2];
    TH1F *hShift[2];
    for(int itri = 0; itri < 1; itri++){
        gr[itri] = new TGraphErrors();
        int iFile = (iLayer == 1) ? 4 : 5;
        for(int iselrig = 0; iselrig < L1QTF_NbinSec2; iselrig++){
            histn = Form("Shift/Shift_r1_q%d", run_charge);
            hShift[itri] = static_cast<TH1F*>(inRoot[iFile]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hShift[itri]->GetBinContent(iselrig+1));
            gr[itri]->SetPointError(iselrig, 0, hShift[itri]->GetBinError(iselrig+1));
        }
    }

    histn = Form("c1_q%d_%d", run_charge, iLayer);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.15);
    c1->SetBottomMargin(0.13);
    c1->SetLogx();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.3;
    double tFoffsety = 1.2;
    // string htitle = Form("L1Q=%d, Shift between Template and PDF", run_charge);
    string htitle = Form("L1Q=%d, Shift between Charge Distribution and L%dPDF", run_charge, iLayer);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Shift");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    if(run_charge%2 == 1)  hbk->GetYaxis()->SetRangeUser(-0.2, 0.2);
    else hbk->GetYaxis()->SetRangeUser(-0.1, 0.1);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.45, 0.63, 0.82, 0.78);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 1; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        legend->AddEntry(gr[itri], Form("L%dPDF",iLayer), "p");
    }

    if(run_charge % 2 == 0){
        double xmin = 2;
        double xmax = 100;
        if(run_charge == 12) xmax = 40;
        if(run_charge == 14) xmin = 7;
        TF1 *gf = new TF1("FitFunc", "[0]", xmin, xmax);
        gf->SetParameter(0, -0.01);
        gf->SetParLimits(0, -0.3, 0.3);
        gr[0]->Fit("FitFunc", "r");

        gf->SetLineWidth(3);
        gf->SetLineColor(kRed);
        gf->Draw("same");

        legend->AddEntry(gf, Form("Fit Shift: %.4f",gf->GetParameter(0)), "l");
    }
    legend->Draw("");

    // string sPic = Form("%s/ShiftPic/TempShift", wdir.c_str());
    string sPic = Form("%s/ShiftPic/QDisShift/TempFit_L%d", wdir.c_str(), iLayer);
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/runQ%d.png", sPic.c_str(), run_charge));
}

void L1TempShift_CutHe(const int run_charge = 14){
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraphErrors *gr[2];
    TH1F *hShift[2];
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        // int iFile = (itri == 0) ? 6 : 7; //TempShift
        // int iFile = (itri == 0) ? 8 : 9; //TempShift Data  MCPDF
        int iFile = (itri == 0) ? 10 : 11; //TempShift MC  MCPDF
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            histn = Form("Shift/Shift_r1_q%d", run_charge);
            hShift[itri] = static_cast<TH1F*>(inRoot[iFile]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hShift[itri]->GetBinContent(iselrig+1));
            gr[itri]->SetPointError(iselrig, 0, hShift[itri]->GetBinError(iselrig+1));
        }
    }

    histn = Form("c1_q%d", run_charge);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.06);
    // c1->SetBottomMargin(0.13);
    c1->SetLogx();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.3;
    double tFoffsety = 1.2;
    string htitle = Form("L1Q=%d, Shift between Template and PDF, CutHe", run_charge);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Shift");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    if(run_charge %2 == 0) hbk->GetYaxis()->SetRangeUser(-0.1, 0.1);
    else hbk->GetYaxis()->SetRangeUser(-0.2, 0.2);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    double x1 = 0.3, x2 = 0.82, y1 = 0.73, y2 = 0.88;
    if(run_charge%2 == 1 ) y1= 0.15, y2 = 0.3;
    auto legend = new TLegend(x1, y1, x2, y2);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        legend->AddEntry(gr[itri], Form("L%dPDF(MC) and L%dTemp(MC)",itri+1,itri+1), "p");
        // legend->AddEntry(gr[itri], Form("L%dPDF(MC) and L%dTemp(Data)",itri+1,itri+1), "p");
    }
    legend->Draw("");

    string sPic = Form("%s/ShiftPic/MCPDF/CutHe/MCTemp/Layer", wdir.c_str());
    // string sPic = Form("%s/ShiftPic/CutHe/TempShift/Layer", wdir.c_str());
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/runQ%d.png", sPic.c_str(), run_charge));
}

void CompareShift_CutHe(const int ifile1 = 0, const int ifile2 = 6, const int run_charge = 14){
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    TH1F *hbk2 = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraphErrors *gr[2];
    TGraphErrors *grDiff = new TGraphErrors();
    TH1F *hShift[2];
    double sumBin = 0;
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            int ich = run_charge;
            int ilFile = (itri == 0)? ifile1 : ifile2;
            histn = Form("Shift/Shift_r1_q%d", ich);
            hShift[itri] = static_cast<TH1F*>(inRoot[ilFile]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hShift[itri]->GetBinContent(iselrig+1));
            gr[itri]->SetPointError(iselrig, 0, hShift[itri]->GetBinError(iselrig+1));

            if(itri==0) sumBin += hShift[itri]->GetBinContent(iselrig+1);

            if(itri == 1){
                double d1 = hShift[0]->GetBinContent(iselrig+1);
                double d2 = hShift[1]->GetBinContent(iselrig+1);
                double e1 = hShift[0]->GetBinError(iselrig+1);
                double e2 = hShift[1]->GetBinError(iselrig+1);
                grDiff->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), d1-d2);
                grDiff->SetPointError(iselrig, 0, sqrt(e1*e1 + e2*e2));
            }
        }
    }

    cout << "\t the shift average:" << sumBin/L1QTF_NbinAllQ2 << endl;

    histn = Form("c1");
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    // c1->SetRightMargin(0.15);
    // c1->SetBottomMargin(0.13);
    c1->SetLogx();
    TPad *bot_pad = new TPad("bot_pad", "bot_pad",0.0,0.0,1.0,0.4);
    bot_pad->Draw();
    TPad *top_pad = new TPad("top_pad", "top_pad",0.0,0.4,1.0,1.0);
    top_pad->Draw();

    top_pad->cd();
    gStyle->SetErrorX(0);
    top_pad->SetGrid();
    top_pad->SetLogx();
    top_pad->SetLeftMargin(0.15);
    top_pad->SetBottomMargin(0);

    // string htitle = Form("Shift between Template and PDF for L1");
    string htitle = Form("Shift between Charge Distribution and PDF for L1");
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(0.06);
    hbk->GetXaxis()->SetLabelSize(0.06);
    hbk->GetXaxis()->SetTitleOffset(1.3);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Shift");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    hbk->GetYaxis()->SetRangeUser(-0.1, 0.1);
    hbk->GetYaxis()->SetTitleSize(0.06);
    hbk->GetYaxis()->SetLabelSize(0.06);
    hbk->GetYaxis()->SetTitleOffset(0.8);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.45, 0.63, 0.6, 0.78);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        int ich = run_charge;
        string sTemp = "";
        if(itri == 1 ) sTemp = "Cut He";
        legend->AddEntry(gr[itri], Form("L1Q = %d, %s",ich, sTemp.c_str()), "p");
    }
    legend->Draw("");

    bot_pad->cd();
    bot_pad->SetGrid();
    bot_pad->SetLogx();
    bot_pad->SetTopMargin(0);
    bot_pad->SetLeftMargin(0.15);
    bot_pad->SetBottomMargin(0.25);
    hbk2->SetTitle("");
    hbk2->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk2->GetXaxis()->SetMoreLogLabels(true);
    hbk2->GetXaxis()->SetTitleSize(0.08);
    hbk2->GetXaxis()->SetLabelSize(0.08);
    hbk2->GetXaxis()->SetTitleOffset(1.3);
    hbk2->GetXaxis()->CenterTitle(true);
    hbk2->GetXaxis()->SetRangeUser(2, 3000);
    hbk2->GetYaxis()->SetTitle(Form("Shift-Shift_{CutHe}"));
    hbk2->GetYaxis()->SetRangeUser(-0.05, 0.05);
    hbk2->GetYaxis()->SetTitleSize(0.08);
    hbk2->GetYaxis()->SetLabelSize(0.08);
    hbk2->GetYaxis()->SetTitleOffset(0.6);
    hbk2->GetYaxis()->CenterTitle(true);
    hbk2->Draw();

    grDiff->SetMarkerStyle(20);
    grDiff->SetMarkerColor(kRed);
    grDiff->SetLineColor(kRed);
    grDiff->Draw("same PEZ");

    string sPic = Form("%s/ShiftPic/CutHe/TempShift/Compare", wdir.c_str());
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/Shift_diffTemp_L1.png", sPic.c_str()));
}

void L1DistributionShift_TransRootCutHe(const int run_charge = 14){
    const int iLayer = 1;
    TH1F *hbk = new TH1F("","", L1QTF_NbinSec2, L1QTF_BinsSec2);
    string histn;
    TGraphErrors *gr[2];
    TH1F *hShift[2];
    for(int itri = 0; itri < 1; itri++){
        gr[itri] = new TGraphErrors();
        int iFile = (iLayer == 1) ? 12 : 5;
        for(int iselrig = 0; iselrig < L1QTF_NbinSec2; iselrig++){
            histn = Form("Shift/Shift_r1_q%d", run_charge);
            hShift[itri] = static_cast<TH1F*>(inRoot[iFile]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hShift[itri]->GetBinContent(iselrig+1));
            gr[itri]->SetPointError(iselrig, 0, hShift[itri]->GetBinError(iselrig+1));
        }
    }

    histn = Form("c1_q%d_%d", run_charge, iLayer);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.15);
    c1->SetBottomMargin(0.13);
    c1->SetLogx();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.3;
    double tFoffsety = 1.2;
    // string htitle = Form("L1Q=%d, Shift between Template and PDF", run_charge);
    string htitle = Form("L1Q=%d, Shift between Charge Distribution and L%dPDF", run_charge, iLayer);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Shift");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    if(run_charge%2 == 1)  hbk->GetYaxis()->SetRangeUser(-0.3, 0.3);
    else hbk->GetYaxis()->SetRangeUser(-0.1, 0.1);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.15, 0.73, 0.62, 0.88);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 1; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        legend->AddEntry(gr[itri], Form("L%dPDF(CutHe,TemplateMigration)",iLayer), "p");
    }

    if(run_charge % 2 == 0){
        double xmin = 2;
        double xmax = 100;
        if(run_charge == 12) xmax = 200;
        if(run_charge == 14) xmin = 15;
        TF1 *gf = new TF1("FitFunc", "[0]", xmin, xmax);
        gf->SetParameter(0, -0.01);
        gf->SetParLimits(0, -0.3, 0.3);
        gr[0]->Fit("FitFunc", "r");

        gf->SetLineWidth(3);
        gf->SetLineColor(kRed);
        gf->Draw("same");

        legend->AddEntry(gf, Form("Fit Shift: %.4f",gf->GetParameter(0)), "l");
    }
    legend->Draw("");

    // string sPic = Form("%s/ShiftPic/TempShift", wdir.c_str());
    string sPic = Form("%s/ShiftPic/QDisShift_TransRootCutHe/TempFit_L%d", wdir.c_str(), iLayer);
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/runQ%d.png", sPic.c_str(), run_charge));
}