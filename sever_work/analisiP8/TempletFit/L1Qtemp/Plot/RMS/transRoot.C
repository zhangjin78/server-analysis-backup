string wdir = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS";

const int L1QTF_NbinAllQ2 = 23;
double L1QTF_BinsAllQ2[L1QTF_NbinAllQ2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,3000};

void transRoot(){
    const int sCharge = 12;
    const int eCharge = 16;
    int nch = eCharge - sCharge +1;
    string histn;
    const bool bCutHe = true;

    string inRoot = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root";
    TFile *infile = new TFile(inRoot.c_str());

    string outRoot = Form("%s/TransRoot", wdir.c_str());
    system(Form("mkdir -pv %s", outRoot.c_str()));
    histn = Form("%s/InerpolationRoot_KeysPdf_Z%dToZ%d.root", outRoot.c_str(), sCharge, eCharge);
    if(bCutHe) histn = Form("%s/InerpolationRoot_KeysPdf_Z%dToZ%d_CutHe.root", outRoot.c_str(), sCharge, eCharge);
    TFile *outfile = new TFile(Form("%s", histn.c_str()), "RECREATE");

    TTree *inTree = NULL, *outTree = NULL;
    
    double outL1Q;

    for(int ivar = 0; ivar < nch; ivar++){
        int ich = sCharge + ivar;
        for(int il = 1; il < 3; il ++){
            histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf", ich, il);
            outfile->mkdir(histn.c_str());
            for(int irig = 0; irig < L1QTF_NbinAllQ2; irig++){
                histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf", ich, il);
                outfile->cd(histn.c_str());
                histn = Form("L%dQ_r1_q%d_sel1_rig%d", il, ich, irig);
                outTree = new TTree( histn.c_str(), histn.c_str());
                outTree->Branch("UBL1QXY", &outL1Q, "UBL1QXY/D");

                int inCh = ich;
                if(ich < 12) continue; //for Z >= 13
                if(ich%2==1) inCh = ich-1;
                histn = Form("Charge_Calibration1/q%d/L1Inner/L%dQ_KeysPdf/L%dQ_r1_q%d_sel1_rig%d", inCh, il, il, inCh, irig);
                inTree = static_cast<TTree*>(infile->Get(histn.c_str()));
                double inL1Q;
                inTree->SetBranchAddress(Form("UBL1QXY"), &inL1Q);
                long int nentries = inTree->GetEntries();
                double sumL1Q = 0; int nen1 = 0;
                for(int ie = 0; ie < nentries; ie++){
                    inTree->GetEntry(ie);
                    if(bCutHe && (inL1Q < 3.)) continue;
                    sumL1Q += inL1Q;
                    nen1 ++;
                }
                double meanL1Q = sumL1Q / nen1;
                cout << "Charge " << ich << "\tRig" << irig << "\tmean: " << meanL1Q << endl;
                for(int ie = 0; ie < nentries; ie++){
                    inTree->GetEntry(ie);
                    if(bCutHe && (inL1Q < 3.)) continue;
                    if(ich % 2 == 0){
                        outL1Q = inL1Q;
                    }
                    else{
                        // outL1Q = inL1Q * ((double)ich / (double)(inCh)); /// old version
                        double ratio_ich = (double)ich / (double)(inCh);
                        outL1Q = (inL1Q - meanL1Q) * ratio_ich + (meanL1Q + 1); /// old version
                    }
                    outTree->Fill();
                }

                outTree->Write();
            } //irig
        } //il
    } //ivar
}