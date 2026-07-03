string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/ShiftVar";
string indir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/OutputFile/CutHe/FreeShift";

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

const int nRebin = 5;
const int ncolor[3] = {kBlack, kBlue, kRed};
const int nfile = 20;
string infile[nfile];
TFile *inRoot[nfile];

void CompareShiftRig(const int run_charge = 14);
void CompareReShift(const int run_charge = 14);

void plotVar(){
    infile[0] = Form("%s/AllL1PDF/r1_xy2_NVar5_rebin1_q14/AllL1PDF_FreeShift_r1_xy2_NVar5_rebin1_q14_L1QTemp.root", indir.c_str());
    infile[1] = Form("%s/L1L2PDF/r1_xy2_NVar5_rebin1_q14/L1L2PDF_FreeShift_r1_xy2_NVar5_rebin1_q14_L1QTemp.root", indir.c_str());
    infile[2] = Form("%s/AllL1PDF_ReShift/r1_xy2_NVar5_rebin1_q14/AllL1PDF_ReShift_r1_xy2_NVar5_rebin1_q14_L1QTemp.root", indir.c_str());
    infile[3] = Form("%s/L1L2PDF_ReShift/r1_xy2_NVar5_rebin1_q14/L1L2PDF_ReShift_r1_xy2_NVar5_rebin1_q14_L1QTemp.root", indir.c_str());

    for(int i = 0; i < nfile; i++) {
        if(infile[i].empty()) continue;
        inRoot[i] = new TFile(infile[i].c_str());
    }

    // for(int ich = 12; ich < 17; ich++) CompareShiftRig(ich);
    for(int ich = 12; ich < 17; ich++) CompareReShift(ich);
}

void CompareShiftRig(const int run_charge = 14){

    const bool bReShift = true;

    const int iLayer = 1;
    TH1F *hbk = new TH1F("","", L1QTF_NbinSec2, L1QTF_BinsSec2);
    string histn;
    TGraphErrors *gr[2];
    TH1F *hShift[2];
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        int iFile = (itri == 0) ? 0 : 1;
        if(bReShift) iFile = (itri == 0) ? 2 : 3;
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
    legend->AddEntry(gr[0], Form("All L1 PDF"), "p");
    legend->AddEntry(gr[1], Form("L1L2 PDF"), "p");

    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
    }

    // if(run_charge % 2 == 0){
    //     double xmin = 2;
    //     double xmax = 100;
    //     if(run_charge == 12) xmax = 200;
    //     if(run_charge == 14) xmin = 15;
    //     TF1 *gf = new TF1("FitFunc", "[0]", xmin, xmax);
    //     gf->SetParameter(0, -0.01);
    //     gf->SetParLimits(0, -0.3, 0.3);
    //     gr[0]->Fit("FitFunc", "r");

    //     gf->SetLineWidth(3);
    //     gf->SetLineColor(kRed);
    //     gf->Draw("same");

    //     legend->AddEntry(gf, Form("Fit Shift: %.4f",gf->GetParameter(0)), "l");
    // }
    legend->Draw("");

    string sPic = Form("%s/Pic/Shift/L1DisFit_L%d", wdir.c_str(), iLayer);
    if(bReShift) sPic = Form("%s/Pic/Shift/L1DisFit_L%d_ReShift", wdir.c_str(), iLayer);
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/runQ%d.png", sPic.c_str(), run_charge));
}

void CompareReShift(const int run_charge = 14){
    
    const bool bL1L2 = true;

    const int iLayer = 1;
    TH1F *hbk = new TH1F("","", L1QTF_NbinSec2, L1QTF_BinsSec2);
    string histn;
    TGraphErrors *gr[2];
    TH1F *hShift[2];
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        int iFile = (itri == 0) ? 0 : 2;
        if(bL1L2) iFile = (itri == 0) ? 1 : 3;
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
    histn = "All L1 PDF";
    if(bL1L2) histn = "L1L2 PDF";
    legend->AddEntry(gr[0], Form("%s", histn.c_str()), "p");
    legend->AddEntry(gr[1], Form("%s, ReFit", histn.c_str()), "p");
    legend->Draw("");

    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
    }

    string sPic = Form("%s/Pic/Shift/CompareShift", wdir.c_str());
    if(bL1L2) sPic += "/L1L2PDF";
    else sPic += "/AllL1PDF";
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/runQ%d.png", sPic.c_str(), run_charge));
}