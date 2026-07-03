string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp";
const int nRebin = 5;

const int L1QTF_NbinAllQ2 = 23;
double L1QTF_BinsAllQ2[L1QTF_NbinAllQ2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,3000};
const int ncolor[3] = {kBlack, kBlue, kRed};
const int nfile = 10;
string infile[nfile];
TFile *inRoot[nfile];

void L1TempEntries(const int run_charge = 14);

void plotEntries(){
    infile[0] = Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_TrigPeriod0.root", wdir.c_str());
    infile[1] = Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_TrigPeriod1.root", wdir.c_str());

    for(int i = 0; i < nfile; i++) {
        if(infile[i].empty()) continue;
        inRoot[i] = new TFile(infile[i].c_str());
    }

    for(int ich = 11; ich < 19; ich++) L1TempEntries(ich);
}

void L1TempEntries(const int run_charge = 14){
    TH1F *hbk = new TH1F("","", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    string histn;
    TGraph *gr[2];
    double maxen = 0;
    for(int itri = 0; itri < 2; itri++){
        gr[itri] = new TGraph();
        for(int iselrig = 0; iselrig < L1QTF_NbinAllQ2; iselrig++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L1Q_KeysPdf/L1Q_r1_q%d_sel1_rig%d", run_charge, run_charge, iselrig);
            TTree *L1QTem = static_cast<TTree*>(inRoot[itri]->Get(histn.c_str()));
            gr[itri]->SetPoint(iselrig, hbk->GetBinCenter(iselrig+1), L1QTem->GetEntries());
            if(maxen < L1QTem->GetEntries()) maxen = L1QTem->GetEntries();
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
    c1->SetLogy();
    ///// Font Size
    double tFsize = 0.04;
    double tFsize2 = 0.04;
    double tFoffsetx = 1.;
    double tFoffsety = 1.2;
    string htitle = Form("Data, L1Q=%d, L1 Template, New Selection", run_charge);
    hbk->SetTitle(htitle.c_str());
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(tFsize);
    hbk->GetXaxis()->SetLabelSize(tFsize2);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Events");
    hbk->GetXaxis()->SetRangeUser(0, 3000);
    hbk->GetYaxis()->SetRangeUser(0.1, maxen*10);
    hbk->GetYaxis()->SetTitleSize(tFsize);
    hbk->GetYaxis()->SetLabelSize(tFsize2);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->Draw();

    auto legend = new TLegend(0.45, 0.23, 0.82, 0.38);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.05);
    // legend->SetFillStyle(0);
    for(int itri = 0; itri < 2; itri++){
        gr[itri]->SetMarkerStyle(20);
        gr[itri]->SetMarkerColor(ncolor[itri]);
        gr[itri]->SetLineColor(ncolor[itri]);
        gr[itri]->Draw("pez same");
        legend->AddEntry(gr[itri], Form("TriggerPeriod:%d",itri), "p");
    }
    legend->Draw("");

    string sPic = Form("%s/Plot/Pic/runQ%d/L1Temp", wdir.c_str(), run_charge);
    system(Form("mkdir -pv %s", sPic.c_str()));
    c1->SaveAs(Form("%s/Events_runQ%d.png", sPic.c_str(), run_charge));
}