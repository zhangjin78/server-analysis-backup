#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

#define SDIAT

const int XY0=2, nXY=3; //only XY

//2020.01.22
//#define L1QTEMPSMALLBIN
#define L2QTEMPALL
// #define FELQTEMP
#ifdef L1QTEMPSMALLBIN
const int iNBL1TF=L1QTF_NbinSmall;
double *dBL1TF=L1QTF_BinsSmall;
#elif defined L2QTEMPALL
const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;
#elif defined FELQTEMP
const int iNBL1TF=L1QTF_NbinFe3;
double *dBL1TF=L1QTF_BinsFe3;
#else
//const int iNBL1TF=L1QTF_Nbin;
//double *dBL1TF=L1QTF_Bins;
//const int iNBL1TF=L1QTF_Nbin2;
//double *dBL1TF=L1QTF_Bins2;
#endif //L1QTEMPSMALLBIN

int cKeysPdf5(const int run_charge=6, const int StoreRig=20, const int QVer=1)
{
	time_t start0 = 0, end0 = 0;
	time_t start = 0, end = 0;
	start0 = time(NULL);
	cout << "Start at " << ctime(&start0) << endl;
	
	//------check Rigidity
	//store template for all rigidity
	if (StoreRig<-1 || StoreRig>iNBL1TF-1)
	{
		cout << "ERROR: StoreRig=" << StoreRig << " out of range." << endl;
		cout << "Accpetable range: [-1, " << iNBL1TF-1 << "] (-1: all rigidity range)" << endl;
		return -1;
	}
	
	//------set input directory
	string outdir=Form("/afs/cern.ch/work/j/jzhang2/AMS/result/CHARGECAL_NEWTRQ");
	cout << outdir << endl;
	
	//------draw setting
	gStyle->SetOptStat("");
	gStyle->SetOptTitle(0);
	lTitle.SetTextAlign(21);
	lLegend.SetTextAlign(11);
	
	//------parameter
	string sDir=Form("Charge_Calibration%d", QVer);
	int iSelTemp=1;
	float fSelSigma=2.0;
	
	//----input result
	TFile fQTemplate(Form("%s/total_result.root", outdir.c_str()), "READ");
	
	int Charge=run_charge;
	int iNVar=(Charge<=10)?3:4;
	if (run_charge==-1)
	{
		//2021.01.26
		//Charge=4;
		Charge=3; //2021.01.28
		iNVar=10-Charge+1;
	}
	else if (run_charge==26)
	{
		Charge=25;
		iNVar=4;
	}
	iNVar = 12;
	
	//------RooFit
	RooPlot *rpQFrame;
	
	RooWorkspace rwsLQTemp(Form("LQ_template"));
	
	RooRealVar *rrvQ[3];
	double dqL = 0;
	double dqU = 35;
	for (int ixy=XY0; ixy<nXY; ixy++)
	{
		if (ixy==0) histn = Form("L%dQX", 1);
		else if (ixy==1) histn = Form("L%dQY", 1);
		else if (ixy==2) histn = Form("L%dQXY", 1);
		rrvQ[ixy] = new RooRealVar(histn.c_str(), histn.c_str(), dqL, dqU);
	} //ixy
	
	//loop for L1QTemp and L2QTemp
	// for (int il=1; il<3; il++)
	for (int il=2; il<3; il++) // only L2QTemp
	{
		//------create directory for plots
		cout << "creating outdir L" << il << "QTemp:" << system(Form("mkdir -pv %s/LQTemp/L%dQTemp", outdir.c_str(), il)) << endl;
		
		//----
		for (int ir=1; ir<2; ir++)
		//for (int ir=1; ir<3; ir++)
		{
			RooRealVar *rrvQShift[3][iNVar];
			RooFormulaVar *rrvQWithShift[3][iNVar];
			
			for (int ixy=XY0; ixy<nXY; ixy++)
			{
				//--in order to mix the usage of L1Q and L2Q Template, always use the name L1Q
				if (ixy==0) histn2 = Form("L%dQX", 1);
				else if (ixy==1) histn2 = Form("L%dQY", 1);
				else if (ixy==2) histn2 = Form("L%dQXY", 1);
				
				for (int ivar=0; ivar<iNVar; ivar++)
				{
					//----fix bandwidth
					//mean shift
					histn = Form("r%d_q%d_xy%d_shift_L%dQTemp", ir, Charge+ivar, ixy, il);
					rrvQShift[ixy][ivar] = new RooRealVar(histn.c_str(), histn.c_str(), 0.00, -0.3, 0.3);
		
					//charge with shift
					histn1 = Form("r%d_q%d_xy%d_with_shift_L%dQTemp", ir, Charge+ivar, ixy, il);
					rrvQWithShift[ixy][ivar] = new RooFormulaVar(histn1.c_str(), histn1.c_str(), Form("%s-%s", histn2.c_str(), histn.c_str()), RooArgList(*rrvQ[ixy], *rrvQShift[ixy][ivar]));
					cout << histn2 << "-" << histn << endl;
				} //ivar
			} //ixy
		
			for (int ich=Charge; ich<=35; ich++)
			{
				for (int ixy=XY0; ixy<nXY; ixy++)
				{
					if (ich<Charge || ich>Charge+iNVar-1) continue;
					if (StoreRig==-1 && ich==run_charge) continue;
					if (ich<20 && ich%2==0 && StoreRig==-1) continue;
					
					histn = Form("KeysPdf_r%d_q%d_xy%d_rig%d", ir, ich, ixy, StoreRig);
					cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);
					
					// gPad->SetLogy();
					gPad->SetGrid();
					rpQFrame = rrvQ[ixy]->frame();
		
					//--TTree
					histn = Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d", sDir.c_str(), ich, cSpan[ir], fSelSigma, il, il, ir, ich, iSelTemp, StoreRig);
					if (StoreRig==-1) histn = Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", sDir.c_str(), ich, cSpan[ir], fSelSigma, il, il, ir, ich, iSelTemp);
					TTree *tLQTemp = static_cast<TTree*>(fQTemplate.Get(histn.c_str()));
					cout << histn << endl;
					cout << tLQTemp << endl;
					
					//--RooDataSet
					histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_DataSet", il, ir, ich, ixy, iSelTemp, StoreRig);
					RooDataSet *rdsLQTemp=NULL;
					rdsLQTemp = new RooDataSet(histn.c_str(), histn.c_str(), tLQTemp, RooArgSet(*rrvQ[ixy]));
					
					//--RooKeysPdf
					start = time(NULL);
					cout << "Q" << ich << " ixy=" << ixy << " StoreRig=" << StoreRig << " N.event=" << tLQTemp->GetEntries() << endl;
					//fix bandwidth
					cout << "Fix bandwidth" << endl;
					cout << "start: " << ctime(&start) << endl;
					histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", il, ir, ich, ixy, iSelTemp, StoreRig);
					// RooKeysPdf *rkpLQTemp = new RooKeysPdf(histn.c_str(), histn.c_str(), *rrvQWithShift[ixy][ich-Charge], *rrvQ[ixy], *rdsLQTemp);
					double rho = 1.;
					if(ich > 12 || ich % 2 == 1 || iSelTemp < 3) rho = 2.0;
					RooKeysPdf *rkpLQTemp = new RooKeysPdf(histn.c_str(), histn.c_str(), *rrvQWithShift[ixy][ich-Charge], *rrvQ[ixy], *rdsLQTemp, RooKeysPdf::NoMirror, rho);
					end = time(NULL);
					cout << "end: " << ctime(&end) << endl;
					
					rwsLQTemp.import(*rkpLQTemp);
					
					//--plot pdf
					rkpLQTemp->plotOn(rpQFrame, RooFit::DrawOption("L"));
					
					rpQFrame->DrawClone();
					cCanv->Draw();
					cCanv->SaveAs(Form("%s/LQTemp/L%dQTemp/L%dQTemp_r%d_q%d_xy%d_rig%d_test.png", outdir.c_str(), il, il, ir, ich, ixy, StoreRig));
					delete cCanv;
				} //ixy
			} //ich
		} //ir
	} //il
	
	//----store RooWorkspace and root file
	histn = Form("%s/LQTemp/LQTemp_rig%d.root", outdir.c_str(), StoreRig);
	if (StoreRig==-1) histn = Form("%s/LQTemp/LQTemp_allrig.root", outdir.c_str());
	
	TFile fOutfile(histn.c_str(), "RECREATE");
	rwsLQTemp.Write();
	fOutfile.Close();
	
	//----
	end0 = time(NULL);
	cout << "Finish at " << ctime(&end0) << endl;
	cout << "total processing time:" << end0 - start0 << endl;
	
	return 0;
} //end of KeysPdf
