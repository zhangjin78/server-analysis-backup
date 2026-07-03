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

#define UBL1Q

int cKeysPdf5(const int run_charge=24, const int StoreRig=22, const int QVer=1)
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
	#ifdef UBL1Q
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/MC_CutHe_KeysPdf_Z12ToZ16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile/MC/amsd68nMCl1_B1130_KeysPDF_SURSEL_MC_Z12T16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/InerpolationRoot_KeysPdf_Z12ToZ16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/CutHe_KeysPdf_Z12ToZ16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/CutHe_amsd69nChenYa_B1130_KeysPDF_SURSEL_NewL1PDF_Z12ToZ16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/InerpolationRoot_KeysPdf_Z12ToZ16_CutHe.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile/NewL1PDF_AddSel1T4/amsd69nChenYa_B1130_KeysPDF_SURSEL_NewL1PDF_AddSel1T4_Z12T16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/TransRoot/OutRoot/Clockwise/InerpolationRoot_Z12ToZ16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/TransRoot/OutRoot/Anticlockwise/InerpolationRoot_Z12ToZ16.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/InputFile/SURSEL_asSurProbSe/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z3TZ18.root");
	// outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Plot/RMS/TransRoot/CutHe_KeysPdf_Z10ToZ16.root");
	outdir = Form("/afs/cern.ch/work/j/jzhang2/AMS/plot/SurvivalProb/RootFile/KeysPDF/UBL1Qabove3/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z24TZ28.root");
	#endif
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
	#ifdef UBL1Q
	// TFile fQTemplate(Form("%s/amsd69nChenYa_B1130_KeysPDF_SURSEL_asSurProbSe_Z10T18.root", outdir.c_str()), "READ");
	TFile fQTemplate(Form("%s", outdir.c_str()), "READ");
	#else
	// TFile fQTemplate(Form("%s/total_result_0311.root", outdir.c_str()), "READ");
	#endif

	//----Out Dir
	string outPic = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/Template";
	#ifdef UBL1Q
	outPic = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp/OutputFile/UBL1Q";
	#endif
	// string sOutDir = Form("%s/LQTemp", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_MC_CutHe_rho1p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_MC_rho1p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_rrvQ_rho1p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTempNoShift", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_TransRoot", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_CutHe", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_TransRootCutHe_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_CutHe_NoShift_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_CutHe_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_CutHe_NoShift_NewL1PDF_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_CutHe_NewL1PDF_rho2p0", outPic.c_str());
	// string sOutDir = Form("%d/LQTemp_rho0p5", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_AddSel1T4_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_AddSel1T4_NoShift_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_TransRoot_Clockwise_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_TransRoot_Anticlockwise_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_Z3TZ18_rho2p0", outPic.c_str());
	// string sOutDir = Form("%s/LQTemp_CutHe_Z10TZ16_rho2p0", outPic.c_str());
	string sOutDir = Form("%s/LQTemp_CutHe_Z24TZ28_rho2p0", outPic.c_str());
	
	int Charge=run_charge;
	int iNVar=(Charge<=10)?3:4;
	iNVar = 5; ////total iNvar pdfs
	// iNVar = 16; ////total iNvar pdfs
	// iNVar = 7; ////total iNvar pdfs
	
	//------RooFit
	RooPlot *rpQFrame;
	
	RooWorkspace rwsLQTemp(Form("LQ_template"));
	
	RooRealVar *rrvQ[3];
	// double dqL = 3; ///test rrcQ
	double dqL = 0;
	double dqU = 35;
	for (int ixy=XY0; ixy<nXY; ixy++)
	{
		if (ixy==0) histn = Form("L%dQX", 1);
		else if (ixy==1) histn = Form("L%dQY", 1);
		#ifdef UBL1Q
		else if (ixy==2) histn = Form("UBL%dQXY", 1);
		#else
		else if (ixy==2) histn = Form("L%dQXY", 1);
		#endif
		rrvQ[ixy] = new RooRealVar(histn.c_str(), histn.c_str(), dqL, dqU);
	} //ixy
	
	//loop for L1QTemp and L2QTemp
	for (int il=1; il<3; il++)
	// for (int il=2; il<3; il++) // only L2QTemp
	// for (int il=1; il<2; il++) // only L1QTemp
	{
		//------create directory for plots
		// cout << "creating outdir L" << il << "QTemp:" << system(Form("mkdir -pv %s/LQTemp/L%dQTemp/pic", outdir.c_str(), il)) << endl;
		outPic = Form("%s/L%dQTemp/pic", sOutDir.c_str(), il);
		cout << "creating outdir L" << il << "QTemp:" << system(Form("mkdir -pv %s", outPic.c_str())) << endl;
		
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
				#ifdef UBL1Q
				else if (ixy==2) histn2 = Form("UBL%dQXY", 1);
				#else
				else if (ixy==2) histn2 = Form("L%dQXY", 1);
				#endif
				
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
					// if (StoreRig==-1 && ich==run_charge) continue;
					if (ich<20 && ich%2==0 && StoreRig==-1) continue;
					
					histn = Form("KeysPdf_r%d_q%d_xy%d_rig%d", ir, ich, ixy, StoreRig);
					cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);
					
					gPad->SetLogy();
					gPad->SetGrid();
					rpQFrame = rrvQ[ixy]->frame();
		
					//--TTree
					histn = Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d", sDir.c_str(), ich, cSpan[ir], fSelSigma, il, il, ir, ich, iSelTemp, StoreRig);
					if (StoreRig==-1) histn = Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", sDir.c_str(), ich, cSpan[ir], fSelSigma, il, il, ir, ich, iSelTemp);
					#ifdef UBL1Q
					histn = Form("%s/q%d/%s/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d", sDir.c_str(), ich, cSpan[ir], il, il, ir, ich, iSelTemp, StoreRig);
					if (StoreRig==-1) histn = Form("%s/q%d/%s/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", sDir.c_str(), ich, cSpan[ir], il, il, ir, ich, iSelTemp);
					#endif
					// if (ich > 16 || ((ich%2)==1 && ich>11)) histn = Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", sDir.c_str(), ich, cSpan[ir], fSelSigma, il, il, ir, ich, iSelTemp);
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
					double rho = 2.;
					// if((ich % 2 == 0) && il == 1) rho = 0.5;
					// if(il == 2) rho = 2.;
					// if((ich > 18 || ich % 2 == 1 || iSelTemp < 3) && il == 2) rho = 2.0;
					// if((ich > 12 || ich % 2 == 1 || iSelTemp < 5) && il == 1) rho = 2.0;
					// if((ich % 2 == 1 || iSelTemp < 3) && il == 1) rho = 2.0;
					RooKeysPdf *rkpLQTemp = new RooKeysPdf(histn.c_str(), histn.c_str(), *rrvQWithShift[ixy][ich-Charge], *rrvQ[ixy], *rdsLQTemp, RooKeysPdf::NoMirror, rho);
					// RooKeysPdf *rkpLQTemp = new RooKeysPdf(histn.c_str(), histn.c_str(), *rrvQ[ixy], *rdsLQTemp, RooKeysPdf::NoMirror, rho);
					end = time(NULL);
					cout << "end: " << ctime(&end) << endl;
					histn = Form("r%d_q%d_xy%d_shift_L%dQTemp", ir, ich, ixy, il);
					// cout << "\tcharge:" << ich << "\tevaluate:" << rrvQWithShift[ixy][ich-Charge]->evaluate() << endl;
					// cout << "\tcharge:" << ich << "\texpression:" << rrvQWithShift[ixy][ich-Charge]->expression() << endl;
					// cout << "\tcharge:" << ich << "\tnParameters:" << rrvQWithShift[ixy][ich-Charge]->nParameters() << endl;
					// cout << "\tcharge:" << ich << "\tmean:" << rrvQShift[ixy][ich-Charge]->getVal() << endl;
					// cout << "\tcharge:" << ich << "\tMaxVal:" << rkpLQTemp->maxVal() << endl;
					
					rwsLQTemp.import(*rkpLQTemp);
					
					//--plot pdf
					rkpLQTemp->plotOn(rpQFrame, RooFit::DrawOption("L"));
					
					rpQFrame->GetXaxis()->SetRangeUser(ich-4,ich+4);
					rpQFrame->DrawClone();
					cCanv->Draw();
					cCanv->SaveAs(Form("%s/L%dQTemp_r%d_q%d_xy%d_rig%d.png", outPic.c_str(), il, ir, ich, ixy, StoreRig));
					delete cCanv;
				} //ixy
			} //ich
		} //ir
	} //il
	
	//----store RooWorkspace and root file
	histn = Form("%s/LQTemp_rig%d.root", sOutDir.c_str(), StoreRig);
	if (StoreRig==-1) histn = Form("%s/LQTemp_allrig.root", sOutDir.c_str());
	
	TFile fOutfile(histn.c_str(), "RECREATE");
	rwsLQTemp.Write();
	fOutfile.Close();
	
	//----
	end0 = time(NULL);
	cout << "Finish at " << ctime(&end0) << endl;
	cout << "total processing time:" << end0 - start0 << endl;
	
	return 0;
} //end of KeysPdf
