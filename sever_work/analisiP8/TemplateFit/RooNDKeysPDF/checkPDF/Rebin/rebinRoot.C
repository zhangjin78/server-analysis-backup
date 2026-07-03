string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/checkPDF/Rebin";
string rootdir = "/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/RootFile/KeysPDF/UBL1Qabove3";
string trandir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TemplateFit/RooNDKeysPDF/makePDF/InputFile/Migration/TransRoot";

const int L1QTF_NbinAllQ2 = 23;
double L1QTF_BinsAllQ2[L1QTF_NbinAllQ2+1] = {
        0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
        5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
        18.0,22.8,31.1,3000};

const int L1QTF_ZRebin = 8;
double L1QTF_BinsZRebin[L1QTF_NbinAllQ2+1] = {
        0.80,1.92,3.29,
        5.37,7.09,9.26,14.1,
        31.1,3000};
// const int L1QTF_ZRebin = 13;
// double L1QTF_BinsZRebin[L1QTF_ZRebin+1] = {
//             0.80,1.51,1.92,2.67,4.02,
//             5.37,6.47,7.76,9.26,12.0,
//             18.0,22.8,31.1,3000};

TFile *infile, *fOutfile;
int chargeZ_begin = 7;
int chargeZ_end = 7;
string histn;
const bool bShiftL2 = true;

void PlotChargeVar1(const int run_charge);
void MergeTree(string outdir);
void MergeTree2(string outdir); //Keep 23 bins in Rebin
void PlotChargeVar2(const int run_charge, string outdir);
void SaveHistRoot(const int iFile);

void rebinRoot(const int iFile = 0){
    string outdir = Form("%s/RebinRootFile/RRoot", wdir.c_str());
    system(Form("mkdir -pv %s", outdir.c_str()));

    if(iFile == 0){
        //For Z = 7 ~ 18
        chargeZ_begin = 7, chargeZ_end = 18;
        // histn = Form("%s/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z7TZ18.root", rootdir.c_str());
        // outdir = Form("%s/amsd69nChenYa_ReBin_Z7TZ18.root", outdir.c_str());
        histn = Form("%s/amsd69nChenYa_TransRoot_Z4TZ18_Z1516rZ14.root", trandir.c_str());
        outdir = Form("%s/amsd69nChenYa_TransRoot_Rebin_Z7TZ18_Z1516rZ14.root", outdir.c_str());
    }
    else if(iFile == 1){
        //For Z = 24 ~ 28
        chargeZ_begin = 24, chargeZ_end = 28;
        histn = Form("%s/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z24TZ28.root", rootdir.c_str());
        if(!bShiftL2) outdir = Form("%s/amsd69nChenYa_ReBin_Z24TZ28.root", outdir.c_str());
        else outdir = Form("%s/amsd69nChenYa_ReBin_Z24TZ28_ShiftL2.root", outdir.c_str());
    }
    else return;
    infile = new TFile(histn.c_str());

    ////--- 1. Poltting before Rebin
    // PlotChargeVar1(12);
    // for(int run_charge = chargeZ_begin; run_charge < chargeZ_end+1; run_charge++){
    //     PlotChargeVar1(run_charge);
    // }

    ////--- 2. Rebin root
    MergeTree2(outdir);

    ////--- 3. Poltting after Rebin
    // for(int run_charge = chargeZ_begin; run_charge < chargeZ_end+1; run_charge++){
    //     PlotChargeVar2(run_charge, outdir);
    // }


    // SaveHistRoot(iFile);
}

void PlotChargeVar1(const int run_charge){
    const int nRigs = L1QTF_NbinAllQ2;
    double *binsRig = L1QTF_BinsAllQ2;
    const int nRebin = 5;

    TH1F *hL1Qrig[nRigs][2];
    TH1F *hEvents[2], *hMean[2], *hSigma[2];
    for(int il = 1; il < 3; il++){
        histn = Form("Before_Evnents_runQ%d_L%d", run_charge, il);
        hEvents[il-1] = new TH1F(histn.c_str(), histn.c_str(), nRigs, binsRig);
        histn = Form("Before_Mean_runQ%d_L%d", run_charge, il);
        hMean[il-1] = new TH1F(histn.c_str(), histn.c_str(), nRigs, binsRig);
        histn = Form("Before_Sigma_runQ%d_L%d", run_charge, il);
        hSigma[il-1] = new TH1F(histn.c_str(), histn.c_str(), nRigs, binsRig);
    }

    for(int irig = 0; irig < nRigs; irig++){
        for(int il = 1; il < 3; il++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", run_charge, il, il, run_charge, irig);
            TTree *inTree = static_cast<TTree*>(infile->Get(histn.c_str()));

            histn = Form("L%dQ_r1_q%d_sel1_rig%d", il, run_charge, irig);
            hL1Qrig[irig][il-1] = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
            hL1Qrig[irig][il-1]->Rebin(nRebin);
            
            double UBL1QXY = 0;
            inTree->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
	        long int nentries = inTree->GetEntries();
            for(int ie = 0; ie < nentries; ie++){
                inTree->GetEntry(ie);
                hL1Qrig[irig][il-1]->Fill(UBL1QXY);
            }

            // string histn_F1 = "[0]*[1]*pow([3],[3])/TMath::Gamma([3])*exp([3]*([1]*([2]-x)-exp(([1]*([2]-x)))))";
            // TF1 *gf = new TF1("FitFunc", histn_F1.c_str(), run_charge-2, run_charge+2);
            // double xmax = hL1Qrig[irig][il-1]->GetBinCenter(hL1Qrig[irig][il-1]->GetMaximumBin());
            // double xmaxEv = hL1Qrig[irig][il-1]->GetMaximum();
            // cout << "HERE!!! " << xmax << "\t" << xmaxEv << endl;
            // gf->SetParameters(xmaxEv, 0.1, xmax, 20);

            // histn_F2 = "[0]*exp( [3]*([1]-x)/[2] - exp( ([1]-x)/[2] ) )";
            // TF1 *gf = new TF1("FitFunc", histn_F2.c_str(), run_charge-2, run_charge+2);
            // double xmax = hL1Qrig[irig][il-1]->GetBinCenter(hL1Qrig[irig][il-1]->GetMaximumBin());
            // cout << "HERE!!! " << xmax << endl;
            // gf->SetParameters(nentries, xmax, 0.2, 1);
            // gf->SetParLimits(1, xmax-0.3, xmax+0.3);

            TF1 *gf = new TF1("FitFunc", "gaus", run_charge-2, run_charge+2);
            gf->SetParameter(1, run_charge);

            hL1Qrig[irig][il-1]->Fit("FitFunc", "r");
            // ////--- Gaus
            // double fit_con = gf->GetParameter(0);
            // double fit_con_err = gf->GetParError(0);
            // double fit_mean = gf->GetParameter(1);
            // double fit_mean_err = gf->GetParError(1);
            // double fit_sigma = gf->GetParameter(2);
            // double fit_sigma_err = gf->GetParError(2);
            ////--- histn_F1
            // double fit_con = gf->GetParameter(0);
            // double fit_mean = gf->GetParameter(2);
            // double fit_sigma = gf->GetParameter(1);
            ////--- Hist TH1
            double fit_con = nentries;
            double fit_con_err = sqrt(nentries);
            double fit_mean = hL1Qrig[irig][il-1]->GetMean();
            double fit_mean_err = hL1Qrig[irig][il-1]->GetMeanError();
            double fit_sigma = hL1Qrig[irig][il-1]->GetRMS();
            double fit_sigma_err = hL1Qrig[irig][il-1]->GetRMSError();

            histn = Form("can_runQ%d_rig%d_L%d", run_charge, irig, il);
            TCanvas *cRig = new TCanvas(histn.c_str(),histn.c_str(),800,600);
            gStyle->SetOptStat(0);
            gStyle->SetErrorX(0);
            gStyle->SetOptFit(1);
            cRig->SetLeftMargin(0.15);
            if(il == 1) histn = "Unbiased L1 Charge";
            else histn = "Tracker L2 Charge";
            hL1Qrig[irig][il-1]->GetXaxis()->SetTitle(histn.c_str());
            hL1Qrig[irig][il-1]->GetYaxis()->SetTitle("Events");
            hL1Qrig[irig][il-1]->GetXaxis()->SetRangeUser(run_charge-1.5, run_charge+1.5);
            hL1Qrig[irig][il-1]->GetXaxis()->CenterTitle();
            hL1Qrig[irig][il-1]->GetYaxis()->CenterTitle();
            hL1Qrig[irig][il-1]->SetMarkerStyle(20);
            hL1Qrig[irig][il-1]->SetMarkerColor(kBlack);
            hL1Qrig[irig][il-1]->SetLineColor(kBlack);
            hL1Qrig[irig][il-1]->Draw("pe");
            gf->SetLineWidth(3);
            gf->SetLineColor(kRed);
            gf->Draw("same");

            histn = Form("%s/Pic/1_RigHist_Before/runQ%d/L%d", wdir.c_str(), run_charge, il);
            system(Form("mkdir -pv %s", histn.c_str()));
            histn = Form("%s/TempHist_runQ%d_Rig%d_L%d.png", histn.c_str(), run_charge, irig, il);
            cRig->SaveAs(histn.c_str());

            hEvents[il-1]->SetBinContent(irig+1, nentries);
            hMean[il-1]->SetBinContent(irig+1, fit_mean);
            hMean[il-1]->SetBinError(irig+1, fit_mean_err);
            hSigma[il-1]->SetBinContent(irig+1, fit_sigma);
            hSigma[il-1]->SetBinError(irig+1, fit_sigma_err);
        }
    }

    string sHist[3] = {"Events", "Mean", "Sigma"};
    for(int il = 1; il < 3; il++){
        for(int ih = 0; ih < 3; ih ++){
            TH1F *htmp;
            if(ih == 0) htmp = (TH1F *)hEvents[il-1]->Clone();
            else if(ih == 1) htmp = (TH1F *)hMean[il-1]->Clone();
            else if(ih == 2) htmp = (TH1F *)hSigma[il-1]->Clone();
            else break;

            histn = Form("%s_runQ%d_L%d", sHist[ih].c_str(), run_charge, il);
            TCanvas *cHist = new TCanvas(histn.c_str(),histn.c_str(),800,600);
            gStyle->SetOptStat(0);
            gStyle->SetErrorX(0);
            cHist->SetLogx();
            cHist->SetLeftMargin(0.15);
            htmp->GetXaxis()->SetTitle("Rigidity [GV]");
            htmp->GetYaxis()->SetTitle(sHist[ih].c_str());
            htmp->GetXaxis()->SetMoreLogLabels(true);
            htmp->GetXaxis()->SetRangeUser(1.8, 3000);
            htmp->GetXaxis()->CenterTitle();
            htmp->GetYaxis()->CenterTitle();
            htmp->SetMarkerStyle(20);
            htmp->SetMarkerColor(kBlack);
            htmp->SetLineColor(kBlack);
            htmp->Draw("pe");

            histn = Form("%s/Pic/2_ChargeShape_Before/runQ%d/L%d", wdir.c_str(), run_charge, il);
            system(Form("mkdir -pv %s", histn.c_str()));
            histn = Form("%s/%s_runQ%d_L%d.png", histn.c_str(), sHist[ih].c_str(), run_charge, il);
            cHist->SaveAs(histn.c_str());
        }
    }
}

void MergeTree(string outdir){
    fOutfile = new TFile(outdir.c_str(), "RECREATE");

    TH1F *hOrg = new TH1F("BeforeRebin", "", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    TH1F *hReb = new TH1F("AfterRebin", "", L1QTF_ZRebin, L1QTF_BinsZRebin);
    TTree *inTree = NULL, *outTree = NULL;
    double outL1Q, inL1Q;

    for(int ich = chargeZ_begin; ich < chargeZ_end+1; ich++){
        cout << "Charge:" <<ich << endl;
        for(int il = 1; il < 3; il ++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf", ich, il);
            fOutfile->mkdir(histn.c_str());
            cout << "\t" << histn << endl;

            for(int irig = 0; irig < L1QTF_ZRebin; irig++){
                histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf", ich, il);
                fOutfile->cd(histn.c_str());
                histn = Form("L%dQ_r1_q%d_sel1_rig%d", il, ich, irig);
                outTree = new TTree( histn.c_str(), histn.c_str());
                cout << "\t\t" << histn << endl;
                outTree->Branch("UBL1QXY", &outL1Q, "UBL1QXY/D");

                double rig_s1 = hReb->GetBinLowEdge(irig+1);
                double rig_s2 = hReb->GetBinLowEdge(irig+2);
                int rig_b1 = hOrg->FindBin(rig_s1) -1;
                int rig_b2 = hOrg->FindBin(rig_s2) -1;
                for(int irig2 = rig_b1; irig2 < rig_b2; irig2++){
                    histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", ich, il, il, ich, irig2);
                    inTree = static_cast<TTree*>(infile->Get(histn.c_str()));
                    cout << "\t\t The Merge Bin:" << irig2 << endl;
                    inTree->SetBranchAddress(Form("UBL1QXY"), &inL1Q);
                    long int nentries = inTree->GetEntries();
                    for(int ie = 0; ie < nentries; ie++){
                        inTree->GetEntry(ie);
                        outL1Q = inL1Q;
                        outTree->Fill();
                    }
                }
                outTree->Write();
            }
        }
    }
    fOutfile->Close();
}

void MergeTree2(string outdir){ /// Keep 23 bins in rebin
    fOutfile = new TFile(outdir.c_str(), "RECREATE");

    const double Shift_L1L2 = -0.24; // set the shift between L1 and L2 for Z = 25

    TH1F *hOrg = new TH1F("BeforeRebin", "", L1QTF_NbinAllQ2, L1QTF_BinsAllQ2);
    TH1F *hReb = new TH1F("AfterRebin", "", L1QTF_ZRebin, L1QTF_BinsZRebin);
    TTree *inTree = NULL, *outTree = NULL;
    double outL1Q, inL1Q;

    for(int ich = chargeZ_begin; ich < chargeZ_end+1; ich++){
        cout << "Charge:" <<ich << endl;
        for(int il = 1; il < 3; il ++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf", ich, il);
            fOutfile->mkdir(histn.c_str());
            // cout << "\t" << histn << endl;

            for(int irig = 0; irig < L1QTF_NbinAllQ2; irig++){
                histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf", ich, il);
                fOutfile->cd(histn.c_str());
                histn = Form("L%dQ_r1_q%d_sel1_rig%d", il, ich, irig);
                outTree = new TTree( histn.c_str(), histn.c_str());
                // cout << "\t\t" << histn << endl;
                outTree->Branch("UBL1QXY", &outL1Q, "UBL1QXY/D");

                int bin_Reb = hReb->FindBin(hOrg->GetBinLowEdge(irig+1));
                double rig_s1 = hReb->GetBinLowEdge(bin_Reb);
                double rig_s2 = hReb->GetBinLowEdge(bin_Reb+1);
                int rig_b1 = hOrg->FindBin(rig_s1) -1;
                int rig_b2 = hOrg->FindBin(rig_s2) -1;
                // if(ich < 11 || ich == 12 || ich == 14 || (ich == 26 && il == 1)){
                if(ich < 11 || ich == 12 || ich == 14 || (ich == 26 && il == 1) || ich == 15 || ich == 16){
                    rig_b1 = irig;
                    rig_b2 = irig+1;
                }
                else if((ich%2 == 1 && ich > 13) || ich == 18 || ich == 28){
                    rig_b1 = 0;
                    rig_b2 = 23;
                }
                for(int irig2 = rig_b1; irig2 < rig_b2; irig2++){
                    histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", ich, il, il, ich, irig2);
                    inTree = static_cast<TTree*>(infile->Get(histn.c_str()));
                    // cout << "\t\t The Merge Bin:" << irig2 << endl;
                    inTree->SetBranchAddress(Form("UBL1QXY"), &inL1Q);
                    long int nentries = inTree->GetEntries();
                    for(int ie = 0; ie < nentries; ie++){
                        // if(ich != 25) continue;
                        // if(il != 2) continue;
                        // if(ie > 10) continue;
                        // if(irig != 0) continue;
                        inTree->GetEntry(ie);
                        outL1Q = inL1Q;
                        if(ich == 25 && il == 2 && bShiftL2) outL1Q += Shift_L1L2;
                        outTree->Fill();
                    }
                }
                outTree->Write();
            }
        }
    }
    fOutfile->Close();
}

void PlotChargeVar2(const int run_charge, string outdir){
    cout << outdir << endl;
    TFile *inoutfile = new TFile(outdir.c_str());
    const int nRigs = L1QTF_NbinAllQ2;
    double *binsRig = L1QTF_BinsAllQ2;
    // const int nRigs = L1QTF_ZRebin;
    // double *binsRig = L1QTF_BinsZRebin;
    const int nRebin = 5;

    TH1F *hL1Qrig[nRigs][2];
    TH1F *hEvents[2], *hMean[2], *hSigma[2];
    TH1F *hEventsR[2], *hMeanR[2], *hSigmaR[2];
    for(int il = 1; il < 3; il++){
        histn = Form("After_Evnents_runQ%d_L%d", run_charge, il);
        hEvents[il-1] = new TH1F(histn.c_str(), histn.c_str(), nRigs, binsRig);
        histn = Form("After_Mean_runQ%d_L%d", run_charge, il);
        hMean[il-1] = new TH1F(histn.c_str(), histn.c_str(), nRigs, binsRig);
        histn = Form("After_Sigma_runQ%d_L%d", run_charge, il);
        hSigma[il-1] = new TH1F(histn.c_str(), histn.c_str(), nRigs, binsRig);

        histn = Form("After_Evnents_runQ%d_L%d_Rebin", run_charge, il);
        hEventsR[il-1] = new TH1F(histn.c_str(), histn.c_str(), L1QTF_ZRebin, L1QTF_BinsZRebin);
        histn = Form("After_Mean_runQ%d_L%d_Rebin", run_charge, il);
        hMeanR[il-1] = new TH1F(histn.c_str(), histn.c_str(), L1QTF_ZRebin, L1QTF_BinsZRebin);
        histn = Form("After_Sigma_runQ%d_L%d_Rebin", run_charge, il);
        hSigmaR[il-1] = new TH1F(histn.c_str(), histn.c_str(), L1QTF_ZRebin, L1QTF_BinsZRebin);
    }

    for(int irig = 0; irig < nRigs; irig++){
        for(int il = 1; il < 3; il++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", run_charge, il, il, run_charge, irig);
            TTree *inTree = static_cast<TTree*>(inoutfile->Get(histn.c_str()));

            histn = Form("L%dQ_r1_q%d_sel1_rig%d", il, run_charge, irig);
            hL1Qrig[irig][il-1] = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
            hL1Qrig[irig][il-1]->Rebin(nRebin);
            
            double UBL1QXY = 0;
            inTree->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
	        long int nentries = inTree->GetEntries();
            for(int ie = 0; ie < nentries; ie++){
                inTree->GetEntry(ie);
                hL1Qrig[irig][il-1]->Fill(UBL1QXY);
            }

            TF1 *gf = new TF1("FitFunc", "gaus", run_charge-2, run_charge+2);
            gf->SetParameter(1, run_charge);
            hL1Qrig[irig][il-1]->Fit("FitFunc", "r");
            // ////--- Gaus
            // double fit_con = gf->GetParameter(0);
            // double fit_con_err = gf->GetParError(0);
            // double fit_mean = gf->GetParameter(1);
            // double fit_mean_err = gf->GetParError(1);
            // double fit_sigma = gf->GetParameter(2);
            // double fit_sigma_err = gf->GetParError(2);
            ////--- Hist TH1
            double fit_con = nentries;
            double fit_con_err = sqrt(nentries);
            double fit_mean = hL1Qrig[irig][il-1]->GetMean();
            double fit_mean_err = hL1Qrig[irig][il-1]->GetMeanError();
            double fit_sigma = hL1Qrig[irig][il-1]->GetRMS();
            double fit_sigma_err = hL1Qrig[irig][il-1]->GetRMSError();

            histn = Form("can2_runQ%d_rig%d_L%d", run_charge, irig, il);
            TCanvas *cRig = new TCanvas(histn.c_str(),histn.c_str(),800,600);
            gStyle->SetOptStat(0);
            gStyle->SetErrorX(0);
            gStyle->SetOptFit(1);
            cRig->SetLeftMargin(0.15);
            if(il == 1) histn = "Unbiased L1 Charge";
            else histn = "Tracker L2 Charge";
            hL1Qrig[irig][il-1]->GetXaxis()->SetTitle(histn.c_str());
            hL1Qrig[irig][il-1]->GetYaxis()->SetTitle("Events");
            hL1Qrig[irig][il-1]->GetXaxis()->SetRangeUser(run_charge-1.5, run_charge+1.5);
            hL1Qrig[irig][il-1]->GetXaxis()->CenterTitle();
            hL1Qrig[irig][il-1]->GetYaxis()->CenterTitle();
            hL1Qrig[irig][il-1]->SetMarkerStyle(20);
            hL1Qrig[irig][il-1]->SetMarkerColor(kBlack);
            hL1Qrig[irig][il-1]->SetLineColor(kBlack);
            hL1Qrig[irig][il-1]->Draw("pe");
            gf->SetLineWidth(3);
            gf->SetLineColor(kRed);
            gf->Draw("same");

            histn = Form("%s/Pic/3_RigHist_Sfter/runQ%d/L%d", wdir.c_str(), run_charge, il);
            system(Form("mkdir -pv %s", histn.c_str()));
            histn = Form("%s/TempHist_runQ%d_Rig%d_L%d.png", histn.c_str(), run_charge, irig, il);
            cRig->SaveAs(histn.c_str());

            hEvents[il-1]->SetBinContent(irig+1, nentries);
            hMean[il-1]->SetBinContent(irig+1, fit_mean);
            hMean[il-1]->SetBinError(irig+1, fit_mean_err);
            hSigma[il-1]->SetBinContent(irig+1, fit_sigma);
            hSigma[il-1]->SetBinError(irig+1, fit_sigma_err);
        }
    }

    //----
    for(int arig = 1; arig <= L1QTF_ZRebin; arig++){
        
    }

    string sHist[3] = {"Events", "Mean", "Sigma"};
    for(int il = 1; il < 3; il++){
        for(int ih = 0; ih < 3; ih ++){
            TH1F *htmp;
            if(ih == 0) htmp = (TH1F *)hEvents[il-1]->Clone();
            else if(ih == 1) htmp = (TH1F *)hMean[il-1]->Clone();
            else if(ih == 2) htmp = (TH1F *)hSigma[il-1]->Clone();
            else break;

            histn = Form("%s2_runQ%d_L%d", sHist[ih].c_str(), run_charge, il);
            TCanvas *cHist = new TCanvas(histn.c_str(),histn.c_str(),800,600);
            gStyle->SetOptStat(0);
            gStyle->SetErrorX(0);
            cHist->SetLogx();
            cHist->SetLeftMargin(0.15);
            htmp->GetXaxis()->SetTitle("Rigidity [GV]");
            htmp->GetYaxis()->SetTitle(sHist[ih].c_str());
            htmp->GetXaxis()->SetMoreLogLabels(true);
            htmp->GetXaxis()->SetRangeUser(1.8, 3000);
            htmp->GetXaxis()->CenterTitle();
            htmp->GetYaxis()->CenterTitle();
            htmp->SetMarkerStyle(20);
            htmp->SetMarkerColor(kBlack);
            htmp->SetLineColor(kBlack);
            htmp->Draw("pe");

            histn = Form("%s/Pic/4_ChargeShape_After/runQ%d/L%d", wdir.c_str(), run_charge, il);
            system(Form("mkdir -pv %s", histn.c_str()));
            histn = Form("%s/%s_runQ%d_L%d.png", histn.c_str(), sHist[ih].c_str(), run_charge, il);
            cHist->SaveAs(histn.c_str());
        }
    }
}

void SaveHistRoot(const int iFile){
    int nRebin[3] = {2, 5, 10};
    string sRebin[2] = {"Before_EnergyRebin", "After_EnergyRebin"};
    TFile *inRoot[2];
    string outdir = Form("%s/RebinRootFile/RRoot", wdir.c_str());
    string indir;
    if(iFile == 0){
        //For Z = 7 ~ 18
        chargeZ_begin = 7, chargeZ_end = 18;
        indir = Form("%s/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z7TZ18.root", rootdir.c_str());
        outdir = Form("%s/amsd69nChenYa_ReBin_Z7TZ18.root", outdir.c_str());
    }
    else if(iFile == 1){
        //For Z = 24 ~ 28
        chargeZ_begin = 24, chargeZ_end = 28;
        indir = Form("%s/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z24TZ28.root", rootdir.c_str());
        outdir = Form("%s/amsd69nChenYa_ReBin_Z24TZ28.root", outdir.c_str());
    }
    else return;
    inRoot[0] = new TFile(indir.c_str());
    inRoot[1] = new TFile(outdir.c_str());

    system(Form("mkdir -pv %s/RebinRootFile/HistRoot", wdir.c_str()));
    TFile *outRoot = new TFile(Form("%s/RebinRootFile/HistRoot/ChargeTempHist%d.root", wdir.c_str(), iFile), "RECREATE");

    for(int ier = 0; ier < 2; ier ++)
    for(int ich = chargeZ_begin; ich < chargeZ_end+1; ich++)
    for(int ire = 0; ire < 3; ire++){
        for(int il = 1; il < 3; il++){
            histn = Form("ChargeTempSample/%s/q%d/L%dQ/Rebin%d", sRebin[ier].c_str(), ich, il, nRebin[ire]);
            outRoot->mkdir(histn.c_str());
            cout << "\t" << histn << endl;
            for(int irig = 0; irig < L1QTF_NbinAllQ2; irig++){
                histn = Form("L%dQ_q%d_%s_HistRebin%d_rig%d", il, ich, sRebin[ier].c_str(), ire, irig);
                TH1F *hL1Qrig = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
                hL1Qrig->Rebin(nRebin[ire]);

                histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", ich, il, il, ich, irig);
                TTree *inTree = static_cast<TTree*>(inRoot[ier]->Get(histn.c_str()));
                double UBL1QXY = 0;
                inTree->SetBranchAddress(Form("UBL1QXY"), &UBL1QXY);
                long int nentries = inTree->GetEntries();
                for(int ie = 0; ie < nentries; ie++){
                    inTree->GetEntry(ie);
                    hL1Qrig->Fill(UBL1QXY);
                }
                if(il == 1) histn = "Unbiased L1 Charge";
                else histn = "Tracker L2 Charge";
                hL1Qrig->GetXaxis()->SetTitle(histn.c_str());
                hL1Qrig->GetYaxis()->SetTitle("Events");
                // hL1Qrig->GetXaxis()->SetRangeUser(ich-1.5, ich+1.5);
                hL1Qrig->GetXaxis()->CenterTitle();
                hL1Qrig->GetYaxis()->CenterTitle();
                hL1Qrig->SetMarkerStyle(20);
                hL1Qrig->SetMarkerColor(kBlack);
                hL1Qrig->SetLineColor(kBlack);
                hL1Qrig->Draw("pe");
                hL1Qrig->Write();

                histn = Form("ChargeTempSample/%s/q%d/L%dQ/Rebin%d", sRebin[ier].c_str(), ich, il, nRebin[ire]);
                outRoot->cd(histn.c_str());
            }
        }
    }

    outRoot->Close();
}