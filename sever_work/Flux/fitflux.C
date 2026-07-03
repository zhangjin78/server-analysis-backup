#include "./include/splineFit3.h"

const string wdir = "";
const string fluxdir = "/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/FluxCalculation/Input/FluxFit/FluxModel";

const int nuclei_nbinBZ = 73;
double nuclei_binsBZ[nuclei_nbinBZ+1] = {0.8,1.00,1.16,1.33,1.51,1.71,1.92,2.15,2.40,2.67,	//0-9
						     2.97,3.29,3.64,4.02,4.43,4.88,5.37,5.90,6.47,7.09,	//10-19
						     7.76,8.48,9.26,10.1,11.0,12.0,13.0,14.1,15.3,16.6,	//20-29
						     18.0,19.5,21.1,22.8,24.7,26.7,28.8,31.1,33.5,36.1,	//30-39
						     38.9,41.9,45.1,48.5,52.2,56.1,60.3,64.8,69.7,74.9,	//40-49
						     80.5,86.5,93.0, 100.,108.,116.,125.,135.,147.,160.,	//50-59
						     175.,192.,211.,233.,259.,291.,330.,379.,441.,525.,	//60-69
						     660.,860.,1200.,3000.}; 	


void fitflux(){}

void SaveFitFunc(int run_charge = 14){
    TFile *file1 = new TFile("./root/silicon68_20230304P8_B1236602RAMCKY11COMBUNFOLDNNB_totalQYAN.root");

    TGraphErrors *grFlux = (TGraphErrors*)file1->Get("Z14fluxh_totalAgr");
    TH1F *hbk = new TH1F("", "", nuclei_nbinBZ, nuclei_binsBZ);

    ///Get point X
    //// No.0-2.03175 No.66-1897.37
    // for (int i = 0; i < grFlux->GetN(); i++){
    //     cout << "No." << i << "\t " << grFlux->GetPointX(i) << endl;
    // }
    TGraphErrors *geFluxLL = new TGraphErrors();
    string histn = Form("Flux_LogLog");
	geFluxLL->SetNameTitle(histn.c_str(), histn.c_str());
	for (int ip=0; ip<grFlux->GetN(); ip++)
	{
		double dX, dY;
		grFlux->GetPoint(ip,dX,dY);
		geFluxLL->SetPoint(ip,log(dX),log(dY));
	} //ip

    const int numNude = 9;
    const int FitOpt=SplineFit::LogX|SplineFit::LogY|SplineFit::ExtrapolateLB|SplineFit::ExtrapolateLE;
    double dRigL =  2.0;
    double dRigU =  1000;
    double na0 = grFlux->GetPointX(0);
    double na1 = grFlux->GetPointX(4);
    double na2 = grFlux->GetPointX(17);
    double na3 = grFlux->GetPointX(33);
    // cout << "\t" << na0 << "\t" << na1 << "\t" << na2 << "\t" << na3 << endl;

    //----setting nodes
    double nNode[numNude] = {1.7, 2.5, 5.0, 10., 23., 60., 167., 500., 25000.};
	vector<double> vdnode, vdynode;
	double node, ynode;
	for (int inode=0; inode<numNude; inode++)
	{
		//--x node
		node=nNode[inode];
		
		//--y node
		ynode=exp(geFluxLL->Eval(log(node)));
		if (ynode==0) ynode = vdynode.back();
		
		cout << inode << " " << node << " " << geFluxLL->Eval(log(node)) << " " << ynode << endl;
		vdnode.push_back(node);
		vdynode.push_back(ynode);
	} //inode

    SplineFit *sPFit = new SplineFit(numNude, "b2e2", FitOpt);
    sPFit->SetRange(0.1,100000);
    sPFit->SetGraph(grFlux);
    TF1 *tf1 = sPFit->BuildTF1(Form("TF1_Flux"));
    sPFit->SetXnode(&vdnode[0]);
    sPFit->SetXnodeLimits();
    sPFit->SetYnode(&vdynode[0]);
    sPFit->doFit(2.15);

    histn = "./fluxfit_amsd68n_q14_20230304";
    sPFit->StoreFun(histn.c_str());

    TCanvas *c1 = new TCanvas("c1","compare",700,600);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);
    gPad->SetGrid();
    c1->SetLeftMargin(0.15);
    // c1->SetRightMargin(0.15);
    c1->SetLogx();
    c1->SetLogy();
    // c1->SetFillColor(19);

    hbk->GetXaxis()->SetTitle("Rigidity [GV]");
    hbk->GetXaxis()->SetMoreLogLabels(true);
    hbk->GetXaxis()->SetTitleSize(0.04);
    hbk->GetXaxis()->SetLabelSize(0.03);
    hbk->GetXaxis()->SetTitleOffset(1.1);
    hbk->GetXaxis()->CenterTitle(true);
    hbk->GetYaxis()->SetTitle("Flux [m^{-2}sr^{-1}sec^{-1}GV^{-1}]");
    hbk->GetYaxis()->SetRangeUser(1e-7, 1);
    // hbk->GetYaxis()->SetRangeUser(0, 0.3);
    hbk->GetYaxis()->SetTitleSize(0.04);
    hbk->GetYaxis()->SetLabelSize(0.03);
    hbk->GetYaxis()->SetTitleOffset(1.2);
    hbk->GetYaxis()->CenterTitle(true);
    hbk->GetXaxis()->SetRangeUser(2, 2000);

    tf1->SetLineColor(kBlue);

    hbk->Draw();
    grFlux->Draw("same pez");
    tf1->Draw("same");
    
}