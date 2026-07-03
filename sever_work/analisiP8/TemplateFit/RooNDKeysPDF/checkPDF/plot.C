#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;
const int nColor[10] = {kBlue, kRed, kGreen+3, kBlack, kMagenta+1, 28, 7, kPink-9, kYellow+1, kSpring-8};

const string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF";
const string rootdir = Form("%s/OutputFile/RandomRoot", wdir.c_str());
const string rebindir = Form("%s/OutputFile/RebinRoot_TotalL1", wdir.c_str());
// const string rebindir = Form("%s/OutputFile/RebinRoot", wdir.c_str());
const string picdir = Form("%s/OutputFile/Pic", wdir.c_str());
const bool bRebin = true;

void CompareRhoSigma(const int run_charge = 12, const int ilayer = 1);

void plot(){
    // CompareRhoSigma(12, 1);
    for(int iq = 12; iq < 19; iq ++)
        for(int il = 1; il < 3; il++)
            CompareRhoSigma(iq, il);
    for(int iq = 24; iq < 29; iq ++)
        for(int il = 1; il < 3; il++)
            CompareRhoSigma(iq, il);
}

void CompareRhoSigma(const int run_charge = 12, const int ilayer = 1){
    if(ilayer != 1 && ilayer != 2) return;

    const int ngr = 6;
    TH1F *hChis[ngr];

    histn = Form("graph_%d_%d", run_charge, ilayer);
    TCanvas *c1 = new TCanvas(histn.c_str(),histn.c_str(),700,600);
    gStyle->SetOptStat(0);
    gPad->SetGridy();
    c1->SetLeftMargin(0.12);
    c1->SetRightMargin(0.05);
    c1->SetTopMargin(0.05);
    c1->SetBottomMargin(0.12);
    c1->SetLogx();

    ///// Font Size
    double tFsize = 0.03;
    double tFsize2 = 0.03;
    double tFoffsetx = 1.0;
    double tFoffsety = 1.4;

    TH1F *hbk = new TH1F("","", iNBL1TF, dBL1TF);
    // hbk->SetTitle("");
    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(0.04);
    hbk->GetXaxis()->SetLabelSize(tFsize);
    hbk->GetXaxis()->SetTitleOffset(tFoffsetx);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Chisquare/ndf");
    double ylow = 0., yup = 5.;
    hbk->GetYaxis()->SetRangeUser(ylow, yup);
    hbk->GetYaxis()->SetTitleSize(0.04);
    hbk->GetYaxis()->SetLabelSize(tFsize);
    hbk->GetYaxis()->SetTitleOffset(tFoffsety);
    hbk->GetYaxis()->CenterTitle(true);
    double dAxisL = 2., dAxisU = 500.;
    hbk->GetXaxis()->SetRangeUser(dAxisL, dAxisU);
    hbk->Draw("");
    double x1=0.54, y1=0.7, x2=0.94, y2=0.94;
    auto legend = new TLegend(x1, y1, x2, y2);
    legend->SetFillColor(0);
    legend->SetLineColor(0);
    legend->SetTextSize(0.04);
    double ymax = 0;
    for(int irho = 1; irho < 3; irho++){
        for(int isig = 1; isig < 4; isig++){
            string temp_dir = Form("%s/LQTemp_CutHe_Z7TZ18_NDKeys_NoShift_rho%dp0_sigma%dp0/runQ%d",
                                    rootdir.c_str(), irho, isig, run_charge);
            if(run_charge >= 24) temp_dir = Form("%s/LQTemp_CutHe_Z24TZ28_NDKeys_NoShift_rho%dp0_sigma%dp0/runQ%d",
                                    rebindir.c_str(), irho, isig, run_charge);
            string temp_root = Form("%s/NDKeys_NoShift_rho%dp0_sigma%dp0_runQ%d_L%dQTemp.root",
                                    temp_dir.c_str(), irho, isig, run_charge, ilayer);
            if(bRebin){
                temp_dir = Form("%s/LQTemp_Rebin_Z7TZ18_NDKeys_NoShift_rho%dp0_sigma%dp0/runQ%d",
                                    rebindir.c_str(), irho, isig, run_charge);
                if(run_charge >= 24) temp_dir = Form("%s/LQTemp_Rebin_Z24TZ28_NDKeys_NoShift_rho%dp0_sigma%dp0/runQ%d",
                                    rebindir.c_str(), irho, isig, run_charge);
                temp_root = Form("%s/NDKeys_Rebin_rho%dp0_sigma%dp0_runQ%d_L%dQTemp.root",
                                    temp_dir.c_str(), irho, isig, run_charge, ilayer);
            }
            TFile *infile = new TFile(temp_root.c_str());
            histn = Form("Chisquare/ChisquareNdf_q%d_L%d",run_charge, ilayer);
            int igr = (irho-1)*3+(isig-1);
            hChis[igr] = (TH1F*)infile->Get(histn.c_str());

            hChis[igr]->SetMarkerStyle(20);
            hChis[igr]->SetMarkerColor(nColor[igr]);
            hChis[igr]->SetLineColor(nColor[igr]);
            hChis[igr]->GetXaxis()->SetRangeUser(dAxisL, dAxisU);
            hChis[igr]->Draw("same pz");
            histn = Form("#color[%d]{Rho:%d.0,Sigma:%d.0}", nColor[igr], irho, isig);
            legend->AddEntry(hChis[igr], histn.c_str(), "p");

            double tmpmax = hChis[igr]->GetMaximum();
            if(ymax < tmpmax) ymax = tmpmax;
        }
    }
    hbk->GetYaxis()->SetRangeUser(ylow, ymax*1.5);
    legend->Draw("");
    histn = Form("%s/ComRhoSigma", picdir.c_str());
    if(bRebin) histn += "_Rebin_TotalL1";
    system(Form("mkdir -pv %s", histn.c_str()));
    c1->SaveAs(Form("%s/CheckTemp_runQ%d_Layer%d.png", histn.c_str(), run_charge, ilayer));
}
