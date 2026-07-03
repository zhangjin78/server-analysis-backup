string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS";
string rootdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile";
const int nRebin = 5;
const bool bCutHe  = true;

const int L1QTF_NbinAllQ2 = 23;
double L1QTF_BinsAllQ2[L1QTF_NbinAllQ2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,3000};
const int ncolor[3] = {kBlack, kBlue, kRed};
const int nfile = 10;
string infile[nfile];
TFile *inRoot[nfile];

void GassFitTH1(TH1* h1, const int run_charge, int SelRig, double par[2], double parerr[2]);
void TempDeltaQvsRig(const int run_charge = 14);
void TempDeltaQvsZ(const int SelRig);
void TempMeanvsRig(const int run_charge = 14);
void TempMeanvsZ(const int SelRig);

void plotDeltaQ(){
    infile[0] = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile/MC/amsd68nMCl1_B1130_KeysPDF_SURSEL_MC_Z12T16.root");
    infile[1] = Form("%s/TransRoot/MC_CutHe_KeysPdf_Z12ToZ16.root", wdir.c_str());
    infile[2] = Form("%s/TransRoot/InerpolationRoot_KeysPdf_Z12ToZ16_CutHe.root", wdir.c_str());

    for(int i = 0; i < nfile; i++) {
        if(infile[i].empty()) continue;
        inRoot[i] = new TFile(infile[i].c_str());
    }

    // for(int ich = 12; ich < 17; ich++) TempMeanvsRig(ich);
    // for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++) TempMeanvsZ(iselrig);

    for(int ich = 12; ich < 17; ich++) TempDeltaQvsRig(ich);
    // for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++) TempDeltaQvsZ(iselrig);
}

void GassFitTH1(TH1* h1, const int run_charge, int SelRig, double par[2], double parerr[2], int itri){
    //---Fit range
    int nbin = h1->GetNbinsX();
    double xmin = h1->GetBinLowEdge(1);
    double xmax = h1->GetBinLowEdge(nbin);
    xmin = run_charge - 1;
    xmax = run_charge + 1;

    TF1 *gf = new TF1("FitFunc", "gaus", xmin, xmax);
    h1->Fit("FitFunc", "r");
    par[0] = gf->GetParameter(1);
    parerr[0] = gf->GetParError(1);
    par[1] = gf->GetParameter(2);
    parerr[1] = gf->GetParError(2);

    string histn = Form("GausFit");
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),800,600);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.08);
    // c1->SetBottomMargin(0.13);
    c1->cd();
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.;
    double tFoffsety = 1.2;
    h1->GetXaxis()->SetTitle("Unbiased L1 Charge");
    h1->GetXaxis()->SetTitleSize(tFsize);
    h1->GetXaxis()->SetLabelSize(tFsize2);
    h1->GetXaxis()->SetTitleOffset(tFoffsetx);
    h1->GetXaxis()->CenterTitle(true);
    h1->GetYaxis()->SetTitle("Events");
    // h1->GetXaxis()->SetRangeUser(0, 20);
    h1->GetXaxis()->SetRangeUser(run_charge-3, run_charge+3);
    h1->GetYaxis()->SetTitleSize(tFsize);
    h1->GetYaxis()->SetLabelSize(tFsize2);
    h1->GetYaxis()->SetTitleOffset(tFoffsety);
    h1->GetYaxis()->CenterTitle(true);
    h1->SetMarkerStyle(20);
    h1->Draw("pez");

    gf->SetLineColor(kRed);
    gf->SetLineWidth(4);
    gf->Draw("same");

    string sPic = Form("%s/Pic/deltaQvsRigMC/runQ%d/GaussFit", wdir.c_str(), run_charge);
    if(bCutHe) sPic += "_CutHe";
    sPic = Form("%s/sam%d", sPic.c_str(), itri);
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/gausfit_q%d_rig%d.png", sPic.c_str(), run_charge, SelRig));
    delete c1;
}

void TempDeltaQvsRig(const int run_charge = 14){
    const int iTempLayer = 1;
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraphErrors *gr[2];
    int iFile = 1;

    double maxen = 0;
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        iFile = (itri == 0)? 1:2;
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", run_charge, run_charge, iselrig);
            TTree *L1QTem = static_cast<TTree*>(inRoot[iFile]->Get(histn.c_str()));

            histn = Form("hTemplate_q%d_rig%d_L%d_%d", run_charge, iselrig, iTempLayer, itri);
            TH1F *htemp = new TH1F(histn.c_str(), histn.c_str(), 3500,0,35);
            htemp->SetTitle(Form("%s,[%.2f, %.2f]GV", histn.c_str(), L1QTF_BinsAllQ2[iselrig],L1QTF_BinsAllQ2[iselrig+1]));
            htemp->Rebin(nRebin);
            long int nentries = L1QTem->GetEntries();
            double l1q;
            L1QTem->SetBranchAddress("UBL1QXY", &l1q);
            for(int i = 0; i < nentries; i++){
                L1QTem->GetEntry(i);
                // if(bCutHe && l1q < 3) continue;
                htemp->Fill(l1q);
            }
            double hrmse[2], hrms[2];
            // double hrms = htemp->GetRMS(); /// same as StdDev
            // double hrmse = htemp->GetRMSError();
            GassFitTH1(htemp, run_charge, iselrig, hrms, hrmse, itri);

            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hrms[1]);
            gr[itri]->SetPointError(iselrig, 0, hrmse[1]);
            if(maxen < hrms[1]) maxen = hrms[1];
        }
    }

    histn = Form("c1_q%d", run_charge);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),700,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.08);
    // c1->SetBottomMargin(0.13);
    c1->SetLogx();
    // c1->SetLogy();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.2;
    double tFoffsety = 1.2;
    string htitle = Form("L1Q=%d, L%d Template, GaussFit", run_charge, iTempLayer);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Sigma");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    hbk->GetYaxis()->SetRangeUser(0., 1.);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.55, 0.63, 0.75, 0.78);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    legend->AddEntry(gr[0], "MC","p");
    legend->AddEntry(gr[1], "Data,Template Migration","p");
    legend->Draw("");
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
    }

    string sPic = Form("%s/Pic/deltaQvsRigMC/runQ%d", wdir.c_str(), run_charge);
    system(Form("mkdir -pv %s", sPic.c_str()));
    string sCut = "";
    if(bCutHe) sCut += "_CutHe";
    c1->SaveAs(Form("%s/Sigma_runQ%d_L%dtemp%s.png", sPic.c_str(), run_charge, iTempLayer, sCut.c_str()));
}

void TempDeltaQvsZ(const int SelRig){
    const int iTempLayer = 1;
    const int sCharge = 12;
    const int eCharge = 16;
    int nch = eCharge - sCharge +1;
    TH1F *hbk = new TH1F("","", nch, sCharge-0.5, eCharge+0.5);
    string histn;
    TGraphErrors *gr[2];
    int iFile = 0;
    if(bCutHe) iFile = 1;

    double maxen = 0;
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        iFile = (itri == 0) ? 1:2;
        for(int ich = sCharge; ich <= eCharge; ich++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", ich, ich, SelRig);
            TTree *L1QTem = static_cast<TTree*>(inRoot[iFile]->Get(histn.c_str()));

            histn = Form("hTemplate_q%d_rig%d_L%d_%d", ich, SelRig, iTempLayer, itri);
            TH1F *htemp = new TH1F(histn.c_str(), histn.c_str(), 3500,0,35);
            htemp->SetTitle(Form("%s,[%.2f, %.2f]GV", histn.c_str(), L1QTF_BinsAllQ2[SelRig],L1QTF_BinsAllQ2[SelRig+1]));
            htemp->Rebin(nRebin);
            long int nentries = L1QTem->GetEntries();
            double l1q;
            L1QTem->SetBranchAddress("UBL1QXY", &l1q);
            for(int i = 0; i < nentries; i++){
                L1QTem->GetEntry(i);
                // if(bCutHe && l1q < 3) continue;
                htemp->Fill(l1q);
            }
            double hrmse[2], hrms[2];
            // double hrms = htemp->GetRMS(); /// same as StdDev
            // double hrmse = htemp->GetRMSError();
            GassFitTH1(htemp, ich, SelRig, hrms, hrmse, itri);

            int ip = ich - sCharge;
            gr[itri]->SetPoint(ip, ich, hrms[1]);
            gr[itri]->SetPointError(ip, 0, hrmse[1]);
            if(maxen < hrms[1]) maxen = hrms[1];
        }
    }

    histn = Form("c1_Rig%d", SelRig);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),700,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.08);
    // c1->SetBottomMargin(0.13);
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.2;
    double tFoffsety = 1.2;
    string htitle = Form("MC, L%d Template, GaussFit", iTempLayer);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Charge");
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Sigma");
    hbk->GetYaxis()->SetRangeUser(0., 1.);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.24, 0.7, 0.45, 0.88);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    legend->AddEntry(gr[0], "MC","p");
    legend->AddEntry(gr[1], "Data,Template Migration","p");
    legend->Draw("");
    for(int itri = 0; itri < 2; itri++){
        int ich = sCharge + itri;
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
    }

    string sPic = Form("%s/Pic/deltaQvsZMC/GaussFit/Sigma", wdir.c_str());
    if(bCutHe) sPic += "_CutHe";
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/L%dtemp_Rig%d.png", sPic.c_str(), iTempLayer, SelRig));
}

void TempMeanvsRig(const int run_charge = 14){
    const int iTempLayer = 1;
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraphErrors *gr[2];
    int iFile = 1;

    double maxen = 0;
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraphErrors();
        iFile = (itri == 0) ? 1:2;
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", run_charge, run_charge, iselrig);
            TTree *L1QTem = static_cast<TTree*>(inRoot[iFile]->Get(histn.c_str()));

            histn = Form("hTemplate_q%d_rig%d_L%d_%d", run_charge, iselrig, iTempLayer, itri);
            TH1F *htemp = new TH1F(histn.c_str(), histn.c_str(), 3500,0,35);
            htemp->SetTitle(Form("%s,[%.2f, %.2f]GV", histn.c_str(), L1QTF_BinsAllQ2[iselrig],L1QTF_BinsAllQ2[iselrig+1]));
            htemp->Rebin(nRebin);
            long int nentries = L1QTem->GetEntries();
            double l1q;
            L1QTem->SetBranchAddress("UBL1QXY", &l1q);
            for(int i = 0; i < nentries; i++){
                L1QTem->GetEntry(i);
                // if(bCutHe && l1q < 3) continue;
                htemp->Fill(l1q);
            }
            double hrmse[2], hrms[2];
            // double hrms = htemp->GetRMS(); /// same as StdDev
            // double hrmse = htemp->GetRMSError();
            GassFitTH1(htemp, run_charge, iselrig, hrms, hrmse, itri);

            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), hrms[0]);
            gr[itri]->SetPointError(iselrig, 0, hrmse[0]);
            if(maxen < hrms[0]) maxen = hrms[0];
        }
    }

    histn = Form("c1_q%d", run_charge);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),700,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.08);
    // c1->SetBottomMargin(0.13);
    c1->SetLogx();
    // c1->SetLogy();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.2;
    double tFoffsety = 1.2;
    string htitle = Form("MC, L1Q=%d, L%d Template, GaussFit", run_charge, iTempLayer);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Mean");
    hbk->GetXaxis()->SetRangeUser(2, 3000);
    hbk->GetYaxis()->SetRangeUser(run_charge-1, run_charge+1);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.55, 0.63, 0.75, 0.78);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    legend->AddEntry(gr[0], "MC","p");
    legend->AddEntry(gr[1], "Data,Template Migration","p");
    legend->Draw("");
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
    }

    string sPic = Form("%s/Pic/deltaQvsRigMC/runQ%d", wdir.c_str(), run_charge);
    system(Form("mkdir -pv %s", sPic.c_str()));
    string sCut = "";
    if(bCutHe) sCut += "_CutHe";
    c1->SaveAs(Form("%s/Mean_runQ%d_L%dtemp%s.png", sPic.c_str(), run_charge, iTempLayer, sCut.c_str()));
}

void TempMeanvsZ(const int SelRig){
    const int iTempLayer = 1;
    const int sCharge = 12;
    const int eCharge = 16;
    int nch = eCharge - sCharge +1;
    TH1F *hbk = new TH1F("","", nch, sCharge-0.5, eCharge+0.5);
    string histn;
    TGraphErrors *gr[2];
    int iFile = 0;
    if(bCutHe) iFile = 1;

    double maxen = 0;
    for(int itri = 0; itri < 2; itri++){
        iFile = (itri == 0) ? 1:2;
        gr[itri] = new TGraphErrors();
        for(int ich = sCharge; ich <= eCharge; ich++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", ich, ich, SelRig);
            TTree *L1QTem = static_cast<TTree*>(inRoot[iFile]->Get(histn.c_str()));

            histn = Form("hTemplate_q%d_rig%d_L%d_%d", ich, SelRig, iTempLayer, itri);
            TH1F *htemp = new TH1F(histn.c_str(), histn.c_str(), 3500,0,35);
            htemp->SetTitle(Form("%s,[%.2f, %.2f]GV", histn.c_str(), L1QTF_BinsAllQ2[SelRig],L1QTF_BinsAllQ2[SelRig+1]));
            htemp->Rebin(nRebin);
            long int nentries = L1QTem->GetEntries();
            double l1q;
            L1QTem->SetBranchAddress("UBL1QXY", &l1q);
            for(int i = 0; i < nentries; i++){
                L1QTem->GetEntry(i);
                // if(bCutHe && l1q < 3) continue;
                htemp->Fill(l1q);
            }
            double hrmse[2], hrms[2];
            // double hrms = htemp->GetRMS(); /// same as StdDev
            // double hrmse = htemp->GetRMSError();
            GassFitTH1(htemp, ich, SelRig, hrms, hrmse, itri);

            int ip = ich - sCharge;
            gr[itri]->SetPoint(ip, ich, hrms[0]);
            gr[itri]->SetPointError(ip, 0, hrmse[0]);
            if(maxen < hrms[0]) maxen = hrms[0];
        }
    }

    histn = Form("c1_Rig%d", SelRig);
    TCanvas *c1 = new TCanvas(histn.c_str(), histn.c_str(),700,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    // c1->SetLeftMargin(0.15);
    c1->SetRightMargin(0.08);
    // c1->SetBottomMargin(0.13);
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.2;
    double tFoffsety = 1.2;
    string htitle = Form("MC, L%d Template, GaussFit", iTempLayer);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Charge");
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Mean");
    hbk->GetYaxis()->SetRangeUser(sCharge-1, eCharge+1);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.24, 0.7, 0.45, 0.88);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    legend->AddEntry(gr[0], "MC","p");
    legend->AddEntry(gr[1], "Data,Template Migration","p");
    legend->Draw("");
    for(int itri = 0; itri < 2; itri++){
        int ich = sCharge + itri;
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
    }

    string sPic = Form("%s/Pic/deltaQvsZMC/GaussFit/Mean", wdir.c_str());
    if(bCutHe) sPic += "_CutHe";
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/L%dtemp_Rig%d.png", sPic.c_str(), iTempLayer, SelRig));
}