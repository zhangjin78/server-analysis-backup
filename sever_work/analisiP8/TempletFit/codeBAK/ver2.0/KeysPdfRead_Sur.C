//------------------
//update from: 		KeysPdfRead_batch5.C
//update date: 		2023.11.27
//------------------
//
//------------------
//------------------

#define SDIAT

#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

//#define SIMPLEPLOT
// #define NOPULL
//#define COMPAREPLOT

//#define L1QTEMPSMALLBIN
#define L2QTEMPALL
// #define FELQTEMP //2020.06.07
//#define SECTEMPFIT //2020.09.30
#ifdef L1QTEMPSMALLBIN
const int iNBL1Temp=L1QTF_NbinSmall;
double *dBL1Temp=L1QTF_BinsSmall;
#elif defined L2QTEMPALL
// const int iNBL1Temp=L1QTF_NbinAllQ;
// double *dBL1Temp=L1QTF_BinsAllQ;

//binning for template fit
/*const int iNBL1Fit=L1QTF_NbinAllQ;
double *dBL1Fit=L1QTF_BinsAllQ;*/
//2020.09.22
// const int iNBL1Fit=L1QTF_NbinAllQFit;
// double *dBL1Fit=L1QTF_BinsAllQFit;
//2023.11.27
const int iNBL1Temp=L1QTF_NbinAllQ2;
double *dBL1Temp=L1QTF_BinsAllQ2;

const int iNBL1Fit=L1QTF_NbinSec2;
double *dBL1Fit=L1QTF_BinsSec2;

//2020.06.07
#elif defined FELQTEMP
/*const int iNBL1Temp=L1QTF_NbinFe;
double *dBL1Temp=L1QTF_BinsFe;*/
/*//2020.06.23
const int iNBL1Temp=L1QTF_NbinFe2;
double *dBL1Temp=L1QTF_BinsFe2;*/
//2020.07.12
const int iNBL1Temp=L1QTF_NbinFe3;
double *dBL1Temp=L1QTF_BinsFe3;

//binning for template fit, which is finner than the binning for template //2020.07.12
const int iNBL1Fit=L1QTF_NbinFeFit;
double *dBL1Fit=L1QTF_BinsFeFit;
//2020.09.30
#elif defined SECTEMPFIT
/*const int iNBL1Temp=L1QTF_NbinAllQ;
double *dBL1Temp=L1QTF_BinsAllQ;

//binning for template fit
const int iNBL1Fit=L1QTF_NbinSec;
double *dBL1Fit=L1QTF_BinsSec;*/

//2020.10.26
const int iNBL1Temp=L1QTF_NbinAllQ2;
double *dBL1Temp=L1QTF_BinsAllQ2;

//binning for template fit
const int iNBL1Fit=L1QTF_NbinSec2;
double *dBL1Fit=L1QTF_BinsSec2;

#else
//const int iNBL1Temp=L1QTF_Nbin;
//double *dBL1Temp=L1QTF_Bins;
const int iNBL1Temp=L1QTF_Nbin2;
double *dBL1Temp=L1QTF_Bins2;
#endif //L1QTEMPSMALLBIN

#define LINEARY

//#define ODDCHARGEALLRIG //2020.09.30

//2021.05.24
// #define ISOTOPEBGSUB

//pointer
TH1F *hQDis = NULL, *hQDis1 = NULL, *hQDisDiff = NULL;
TH1D *hQDisD = NULL, *hQDisD1 = NULL;
TH2F *hQ2dDis=NULL, *hQ2dDis2=NULL;
TH2S *hQ2dDisS=NULL;

TH1F *hImp=NULL, *hIne=NULL;
TH1F *hChis=NULL, *hErr=NULL;
TH1F *hShift=NULL;
//2019.12.09
TH1F *hTDiff=NULL;

TTree *tTempFit=NULL;

#ifdef SDIAT
string sWDirOnline = "/afs/cern.ch/work/j/jzhang2/AMS/result"; //XXX
#endif

#define UBL1Q


int KeysPdfRead_Sur( int iSelRig=20,
				const int run_charge=14, 
				const int QVer=1,
				const int NVar0=-1,
				const int L1QSel=-1,
				const int ixy=2,
				const int il=1,
				const int ispan=1,
				const bool bBadLadder=false)
{
	//----draw setting
	gStyle->SetOptStat("");
	gStyle->SetOptTitle(0);
	lTitle.SetTextAlign(21);
	lLegend.SetTextAlign(11);
	TH1::AddDirectory(kFALSE);
	
	//----parameter
	string sDir=Form("Charge_Calibration%d", QVer);
	
	int iSelTemp=1;
	float fSelSigma=2.0;
	
	//--binning for fit
	TH1F hL1QFit("hL1QFit", "hL1QFit", iNBL1Fit, dBL1Fit);
	const double dLRig=hL1QFit.GetBinLowEdge(iSelRig+1), dURig=hL1QFit.GetBinLowEdge(iSelRig+2);
	cout << "OK set fit bin" << endl; //XXX
	//--binning for template
	TH1F hL2QTemp("hL2QTemp", "hL2QTemp", iNBL1Temp, dBL1Temp);
	const int iRigTemp = hL2QTemp.FindBin(hL1QFit.GetBinCenter(iSelRig+1))-1;
	const double dLRigTemp=hL2QTemp.GetBinLowEdge(iRigTemp+1), dURigTemp=hL2QTemp.GetBinLowEdge(iRigTemp+2);
	
	cout << "OK set temp bin" << endl; //XXX
	cout << " *** Fit ibin=" << iSelRig << " temp ibin=" << iRigTemp << " *** " << endl;
	cout << "iSelRig=" << iSelRig << " dLRig=" << dLRig << " dURig=" << dURig << " iRigTemp=" << iRigTemp << endl;
	string sPull="";
	#ifdef NOPULL
	sPull="_nopull";
	#endif //NOPULL

	//----
	int iNVar = 5;
	if (NVar0!=-1) iNVar=NVar0;

	int Charge = run_charge - iNVar/2;
	// iNVar = 10;
	// int Charge = 7;
	// if (run_charge==26) Charge=25;
	// //2020.09.30
	// if (run_charge>8 && run_charge%2==1) Charge=run_charge-1; //2021.01.29: only include lower charge for heavy secondary for now

	//update to be able to deal Silicon
	double dCutL=0, dCutU=0;
	double dDrawL=0, dDrawU=0; //draw range for template fit
	double dDrawBigL=0, dDrawBigU=0; //big draw range, used for LQ template
	double dEvL=0.5, dEvU=0;

	//------fit//for Navr 5
	// double dFitL=Charge+1 - 1.0;
	double dFitL=Charge - 0.5;
	double dFitU=(Charge+iNVar-1) + 0.6;
	double dDataL=Charge - 0.6;
	double dDataU=run_charge + 2.7;
	// double dFitL=run_charge - 0.5;
	// double dFitU=run_charge + 0.5;

	dDrawBigL=run_charge-2, dDrawBigU=run_charge+6.5;

	//2021.01.29: include element for Q<8 as well
	if (run_charge<=8 && run_charge>2)
	{
		dCutL=run_charge-0.46 - (run_charge-3)*0.16, dCutU=(run_charge<=5)?run_charge+0.65:run_charge+0.65+(run_charge-5)*0.03;
		//dDrawL=run_charge-0.8, dDrawU=run_charge+3.5;
		dDrawL=run_charge-0.8, dDrawU=run_charge+2.5;
		if (dDrawL>dCutL) dDrawL=dCutL-0.5;
		dEvU=2*1e6;
	}
	else if (run_charge>=9)
	{
		dCutL = run_charge - 0.5; dCutU = run_charge + 0.5;
		dDrawL = run_charge - 8; dDrawU = run_charge + 10.5;
	
		#ifdef L1QTEMPSMALLBIN
		dEvU=5*1e7;
		#else
		//dEvU=5*1e4;
		dEvU=5*1e7; //2019.10.30
		#endif //L1QTEMPSMALLBIN
		if (run_charge==26) dEvU=5*1e4; //2020.06.17
		if (run_charge%2==1) dEvU=5*1e4; //2020.09.30
		//2020.10.26
		#ifdef SIMPLEPLOT
		if (run_charge%2==1) dEvL=2.5, dEvU=5*1e3;
		#endif //SIMPLEPLOT
	}

	double dAxisL = dFitL - 0.5;
	double dAxisU = dFitU + 0.5;

	if(iNVar == 3){
		dFitL = dCutL, dFitU = dCutU;
	}
	
	if(L1QSel == 0){
		dCutL = run_charge - 0.25, dCutU = run_charge + 0.5;
	}
	else if(L1QSel == 1){
		dCutL = run_charge - 0.5, dCutU = run_charge + 0.25;
	}
	else if(L1QSel == 2){
		dCutL = run_charge - 0.25, dCutU = run_charge + 0.25;
	}
	else{ // -1
		dCutL = run_charge - 0.5, dCutU = run_charge + 0.5;
	}

	double dTempFitL=0, dTempFitU=35;
	
	
	// int NRebin=10;
	int NRebin=5;
	//if (run_charge>=10) NRebin=5;
	// if (run_charge>=9) NRebin=5;
	
	TLine lCuts;
	lCuts.SetLineWidth(3);
	lCuts.SetLineStyle(2);
	TLine lCutsfit;
	lCutsfit.SetLineWidth(3);
	lCutsfit.SetLineStyle(2);
	lCutsfit.SetLineColor(kRed);
	
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
	RooDataHist *rdhLQ=NULL;
	
	//int fillStyle=3002;
	int fillStyle=3001;
	
	RooDataHist *rdhLQTemp=NULL, *rdhL1Q=NULL;
	RooFitResult *rfrResult;

	int iDstver=68;

	const string sTotalDir = Form("%s/ISS", sWDirOnline.c_str());
	#ifdef UBL1Q
	string fileNewSel = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/InputFile";
	TFile fL1Q(Form("%s/amsd69nChenYa_B1130_SurvivalProbability_L1Qdistribution_DiffSel.root", fileNewSel.c_str()), "READ");
	#else
	TFile fL1Q(Form("%s/P8_SurvivalProbability_test2.root", sTotalDir.c_str()), "READ"); ///q9,10,11,12,13,14
	// TFile fL1Q(Form("%s/P8_SurvivalProbability_Sulfur.root", sTotalDir.c_str()), "READ"); ///q16,18,20
	#endif

	const string sTempDir=Form("/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/L1Qtemp");
	TFile fTotalFile(Form("%s/InputFile/amsd69nChenYa_B1130_KeysPDF_SURSEL_TrigPeriod1.root", sTempDir.c_str()), "READ");
	TFile fQTemp(Form("%s/OutputFile/UBL1Q/TrigerPeriod1_LQTemp/LQTemp_rig%d.root", sTempDir.c_str(), iRigTemp), "READ");

	////---- input shift root
	TFile fShift(Form("%s/Shift/L2TempFitL1QTemp_shiftT3T4.root", fileNewSel.c_str()), "READ");
	bool bshift = true;

	//--
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	
	if (ixy==0) histn=Form("L%dQX", 1);
	else if (ixy==1) histn=Form("L%dQY", 1);
	else if (ixy==2) histn=Form("L%dQXY", 1);
	// RooRealVar *rrvQXY = rwsLQTemp->var(histn.c_str());
	RooRealVar *rrvQXY = rwsLQTemp->var("UBL1QXY");

	rrvQXY->setRange("Integral", dCutL, dCutU);

	for (int ir=ispan; ir<ispan+1; ir++){
		cout << "Span=" << ir << endl;

		//----output directory
		string sTighten=(L1QSel==-1)?"":Form("_tighten%d", L1QSel);
		string sOutput = "/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/TempletFit/OutputFile";
		string sDraw = Form("%s/r%d_xy%d_NVar%d_rebin%d_q%d%s", sOutput.c_str(), ir, ixy, iNVar, NRebin, run_charge, sTighten.c_str());
		if (bBadLadder) sDraw = sDraw + "_wBadLadder";
		//--L1QTempFit
		string sDrawL1QTempFit = Form("%s/L1QTempFit_L%dQTemp", sDraw.c_str(), il);
		cout << "Create pic directory  for L1QTempFit " << system(Form("mkdir -pv %s", sDrawL1QTempFit.c_str())) << endl;
		////pic root file
		string sDrawPic = Form("%s/pic", sDrawL1QTempFit.c_str());
		cout << "Create draw pic " << system(Form("mkdir -pv %s", sDrawPic.c_str())) << endl;
		string sDrawRoot = Form("%s/picroot", sDrawL1QTempFit.c_str());
		cout << "Create draw root pic " << system(Form("mkdir -pv %s", sDrawRoot.c_str())) << endl;
		//store the L1Q impurity and inefficiency vs rigidty into root files
		TFile *fOutPut = new TFile(Form("%s/L1QTempFit_r%d_q%d_xy%d_rig%d.root", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig), "RECREATE");

		//store the results fixed by event count
		for (int ifit=0; ifit<2; ifit++)
		{
			//impurity
			histn = "Impurity";
			if (ifit==1) histn = histn+"_FixRelativeAbundance";
			
			fOutPut->mkdir(histn.c_str());
			fOutPut->cd(histn.c_str());
			
			for (int ivar=0; ivar<iNVar; ivar++)
			{
				histn = Form("Impurity_r%d_q%d_xy%d", ir, Charge+ivar, ixy);
				hImp = new TH1F(histn.c_str(), histn.c_str(), iNBL1Fit, dBL1Fit);
				hImp->SetDirectory(gDirectory);
			} //ivar
		
			//inefficiency
			histn = "Inefficiency";
			if (ifit==1) histn = histn+"_FixRelativeAbundance";
			
			fOutPut->mkdir(histn.c_str());
			fOutPut->cd(histn.c_str());
			
			for (int ich=run_charge; ich<run_charge+1; ich++)
			{
				histn = Form("Inefficiency_r%d_q%d_xy%d", ir, ich, ixy);
				hIne = new TH1F(histn.c_str(), histn.c_str(), iNBL1Fit, dBL1Fit);
				hIne->SetDirectory(gDirectory);
			} //ich
		} //ifit

		//// sotre the fit parametor
		histn = "Chisquare";
		fOutPut->mkdir(histn.c_str());
		fOutPut->cd(histn.c_str());
		histn = Form("Chisquare_r%d_xy%d_sum", ir, ixy);
		hChis = new TH1F(histn.c_str(), histn.c_str(), iNBL1Fit, dBL1Fit);
		hChis->SetDirectory(gDirectory);
		
		histn = "FitParaError";
		fOutPut->mkdir(histn.c_str());
		fOutPut->cd(histn.c_str());
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			histn = Form("FitParaError_r%d_q%d_xy%d", ir, Charge+ivar, ixy);
			hErr = new TH1F(histn.c_str(), histn.c_str(), iNBL1Fit, dBL1Fit);
			hErr->SetDirectory(gDirectory);
		} //ivar
		
		histn = "FitShift";
		fOutPut->mkdir(histn.c_str());
		fOutPut->cd(histn.c_str());
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			histn = Form("FitShift_r%d_q%d_xy%d", ir, Charge+ivar, ixy);
			hShift = new TH1F(histn.c_str(), histn.c_str(), iNBL1Fit, dBL1Fit);
			hShift->SetDirectory(gDirectory);
		} //ivar

		histn = Form("Tree_q%d", run_charge);
		fOutPut->mkdir(histn.c_str());
		fOutPut->cd(histn.c_str());
		histn = Form("TemplateFit_q%d_Nvar%d_L%dtemp", run_charge, iNVar, il);
		tTempFit = new TTree(histn.c_str(),histn.c_str());
		
		// fOutPut->ls();

		// //----shift variables
		RooRealVar *rrvshift[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			int iSelSpan=ir;
			
			//--force to use L2QTemp for UHZ except Fe //2020.06.17
			int ilTemp=il;
			// if (Charge+ivar>14 && Charge+ivar!=26) ilTemp=2;
			//2020.10.01: use L2Q template for secondary signal
			// if ((Charge+ivar)>14 && (Charge+ivar)%2==1) ilTemp=2;
			// if ((Charge+ivar)%2==1) ilTemp=2;
			
			rrvshift[ivar] = rwsLQTemp->var(Form("r%d_q%d_xy%d_shift_L%dQTemp", iSelSpan, Charge+ivar, ixy, ilTemp));
			
			//fix the shift variable for L1Q template
			if (ilTemp==1)
			{
				rrvshift[ivar]->setVal(0);
				rrvshift[ivar]->setConstant(kTRUE);
			}
			else{
				TH1F *hShiftFit = (TH1F*)fShift.Get(Form("Shift/Shift_r1_q%d", Charge+ivar));
				rrvshift[ivar]->setVal(hShiftFit->GetBinContent(iRigTemp+1));
				rrvshift[ivar]->setConstant(kTRUE);
			}
		} //ivar

		cout << "****** Performing L1Q template fit ******" << endl;
		//------L1Q template fit
		histn = Form("L1Q_r%d_q%d_xy%d_rig%d", ir, run_charge, ixy, iSelRig);

		cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);


		#ifdef NOPULL
		cCanv->cd();
		//gPad->SetGrid();
		gPad->SetTicks();
		#ifndef LINEARY
		gPad->SetLogy();
		#endif //LINEARY
		gPad->SetTopMargin(0.05);
		gPad->SetRightMargin(0.05);
		#else
		pPad1 = new TPad(histn.c_str(), histn.c_str(), .0, .3, 1., 1.);
		pPad1->cd();
		#ifndef LINEARY
		gPad->SetLogy();
		#endif //LINEARY
		//gPad->SetGrid();
		gPad->SetGridy();
		gPad->SetTicks();
		gPad->SetTopMargin(0.05);
		gPad->SetBottomMargin(0);
		gPad->SetRightMargin(0.05);

		pPad2 = new TPad(histn.c_str(), histn.c_str(), .0, .0, 1., .3);
		pPad2->cd();
		//gPad->SetGrid();
		gPad->SetGridy();
		gPad->SetTicks();
		gPad->SetTopMargin(0);
		gPad->SetBottomMargin(0.2);
		gPad->SetRightMargin(0.05);
		#endif //NOPULL
		
		//----template fit
		#ifdef NOPULL
		cCanv->cd();
		#else
		pPad1->cd(); //keep the pull for SIMPLEPLOT
		#endif //NOPULL

		rpQFrame = rrvQXY->frame(dDrawL, dDrawU);

		//--L1Q
		histn = Form("L1L2Sur4/q%d/%s/period1/tofqcut0/L1QvsRig_r%d_q%d_L1L2Sur_noL1Qcut_period1_tofqcut0", 
										run_charge, sSpan[ir].c_str(), ir, run_charge);
		hQ2dDis = static_cast<TH2F*>(fL1Q.Get(histn.c_str()));
		cout << iSelRig << "  [" << dLRig << "," << dURig << "]" << endl;
		cout << hQ2dDis->GetXaxis()->FindBin(dLRig) << " to " << hQ2dDis->GetXaxis()->FindBin(dURig)-1 << endl;

		histn = Form("L1Q_r%d_rig%d", ir, iSelRig);
		hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRig), hQ2dDis->GetXaxis()->FindBin(dURig)-1, "e");

		//// sum Entries in hL1Q Fit Range
		int xBinL = hQDisD->FindBin(dFitL);
		int xBinU = hQDisD->FindBin(dFitU);
		double events_hL1Q = 0;
		for(int ix = xBinL; ix < xBinU; ix++){
			events_hL1Q += hQDisD->GetBinContent(ix);
		}
		
		//--sum pdf
		//----build fraction variable
		double rNum[iNVar];
		for(int ivar=0; ivar<iNVar; ivar++){
			rNum[ivar] = 0;
			double x12 = 0.5;
			if((Charge+ivar)%2 == 1) x12 = 0.4;
			int xb1 = hQDisD->FindBin((Charge+ivar)-x12);
			int xb2 = hQDisD->FindBin((Charge+ivar)+x12);
			for(int ix = xb1; ix <= xb2; ix++) rNum[ivar] += hQDisD->GetBinContent(ix);
		}
		RooRealVar *rrvn[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			int icharge = Charge + ivar;
			histn = Form("n%d_q%d", ivar, icharge);
			double dIni, dL, dU;
			// dL=0, dU=0.6*events_hL1Q; dIni = 0.4*dU;  /// for 1st pdf
			dIni = 0.96*rNum[ivar], dL = 0.8 * rNum[ivar], dU = 1.2 * rNum[ivar];
			if(icharge % 2 == 1){
				dIni = 0.8*rNum[ivar], dL = 0.4 * rNum[ivar], dU = 0.99 * rNum[ivar];
			}
			// if(icharge == 12) dIni = 0.96*rNum[ivar], dL = 0.9 * rNum[ivar], dU = 1.0 * rNum[ivar];
			// if(icharge == 13) dIni = 0.55*rNum[ivar], dL = 0.5 * rNum[ivar], dU = 0.9 * rNum[ivar];
			// if(icharge == 14) dIni = 0.95*rNum[ivar], dL = 0.9 * rNum[ivar], dU = 1.0 * rNum[ivar];
			// if(icharge == 15) dIni = 0.5*rNum[ivar], dL = 0.5 * rNum[ivar], dU = 0.9 * rNum[ivar];
			// if(icharge == 16) dIni = 0.9*rNum[ivar], dL = 0.8 * rNum[ivar], dU = 1.083 * rNum[ivar];
			// if(icharge == 17) dIni = 0.75*rNum[ivar], dL = 0.3 * rNum[ivar], dU = 0.81 * rNum[ivar];
			// if(icharge == 18) dIni = 1.4*rNum[ivar], dL = 1.35 * rNum[ivar], dU = 1.55 * rNum[ivar];
			// if(icharge == 20) dIni = 1.15*rNum[ivar], dL = 0.9 * rNum[ivar], dU = 1.25 * rNum[ivar];

			// if(icharge == 14) dIni=0.6*events_hL1Q, dL = 0.2*events_hL1Q, dU=0.8*events_hL1Q;
			// else if(icharge == 15) dIni=0.01*events_hL1Q, dL = 0.005*events_hL1Q, dU=0.035*events_hL1Q;
			// else if(icharge == 16) dIni=0.13*events_hL1Q, dL = 0.05*events_hL1Q, dU=0.14*events_hL1Q;
			// else if(icharge == 17) dIni=0.01*events_hL1Q, dL = 0.005*events_hL1Q, dU=0.033*events_hL1Q;
			// else if(icharge == 18) dIni=0.02*events_hL1Q, dL = 0.01*events_hL1Q, dU=0.08*events_hL1Q;
			// if(icharge == 13) dIni=0.02*events_hL1Q, dL = 0.01*events_hL1Q, dU=0.08*events_hL1Q;
			rrvn[ivar] = new RooRealVar(histn.c_str(), histn.c_str(), dIni, dL, dU);
		} //ivar

		//----retrieve RooKeysPdf for each charge
		RooKeysPdf *rkpLQ[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			int iSelSpan=ir;
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe
			int ilTemp=il;
			// if ((Charge+ivar)>14 && Charge+ivar!=26) ilTemp=2;
			//use L2Q template for secondary signal
			// if ((Charge+ivar)>14 && (Charge+ivar)%2==1) ilTemp=2;
			// if ((Charge+ivar)%2==1) ilTemp=2;
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, iRigTemp);
			cout << "PDF setting :\t" << histn << endl;
			
			rkpLQ[ivar] = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
		} //ivar

		histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf_sum", il, ir, Charge, ixy, iSelTemp, iSelRig);
		RooAddPdf *rapSumpdf=NULL;
		RooArgList ralLQ, raln;
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			ralLQ.add(*rkpLQ[ivar]);
			raln.add(*rrvn[ivar]);
		}
		rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), ralLQ,raln);

		rrvQXY->setRange("Fit", dFitL, dFitU); //set the fit range as [run_charge-3, run_charge+(iNVar-1)+0.5]
		rrvQXY->setRange("Norm", 0., 35.);
		rrvQXY->setRange("draw", dDrawL, dDrawU);

		if (hQDisD->GetNbinsX()==3500) hQDisD->Rebin(NRebin);
		histn = Form("L1Q_r%d_q%d_xy%d_L1QSam_rig%d", ir, run_charge, ixy, iSelRig);
		rdhL1Q = new RooDataHist(histn.c_str(), histn.c_str(), RooArgList(*rrvQXY), hQDisD);
		rdhL1Q->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));  ////plot data L1Q
		cout << "******Finish plotting L1Q******" << endl;

		#ifdef LINEARY
		// dEvU = hQDisD->GetBinContent(hQDisD->FindBin(14)) * 1.8;
		double maxBin = 0;
		for (int ivar=0; ivar<iNVar; ivar++){
			double curBin = hQDisD->GetBinContent(hQDisD->FindBin(Charge+ivar));
			if(maxBin < curBin) maxBin = curBin;
		}
		dEvU = maxBin * 1.8;
		#endif

		cout << "start performing fit, fit range: [" << dFitL << "," << dFitU << "]" << endl;
		rfrResult = rapSumpdf->fitTo(*rdhL1Q, RooFit::Save(0), RooFit::SumCoefRange("Fit"), RooFit::Range("Fit"), RooFit::PrintLevel(0));
		cout << "Finish performing fit" << endl;

		//------plot the fit result
		//----sum pdf
		RooAbsReal *rarIntAll = rapSumpdf->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral")); //set the pdf to normalized in [0, 35]
		RooAbsReal *rarIntAll_fit = rapSumpdf->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Fit")); //set the pdf to normalized in [0, 35]
		double events_cut = rpQFrame->getFitRangeNEvt(dCutL, dCutU);
		double events_fit = rpQFrame->getFitRangeNEvt(dFitL, dFitU);
		double sumrrvn = 0;
		for (int ivar=0; ivar<iNVar; ivar++){
			sumrrvn += rrvn[ivar]->getVal();
		}
		rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));
		
		//----chisquare & pull
		RooHist *rhPull = rpQFrame->pullHist(nullptr, nullptr, false);
		double chis_fit = rpQFrame->chiSquare();
		RooAbsReal *rhchis = rapSumpdf->createChi2(*rdhL1Q, RooFit::Range("Fit"));
		int bins = hQDisD->FindBin(dFitU) - hQDisD->FindBin(dFitL) + 1;
		chis_fit = rhchis->getVal()/(bins-iNVar);

		// //----components
		for (int ivar=0; ivar<iNVar; ivar++) ///even charge
		{
			if ((Charge+ivar)%2==1) continue;
			
			int iSelSpan=ir;
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe
			int ilTemp = il;
			// if ((Charge+ivar)>14 && (Charge+ivar)%2==1) ilTemp=2;
			// if ((Charge+ivar)%2==1) ilTemp=2;
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, iRigTemp);
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); //2020.09.30: without drawing the line
			// rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"));
		} //ivar
		for (int ivar=0; ivar<iNVar; ivar++) ///odd charge
		{
			if ((Charge+ivar)%2==0) continue;
			
			int iSelSpan=ir;
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe
			int ilTemp = il;
			// if ((Charge+ivar)>14 && (Charge+ivar)%2==1) ilTemp=2;
			// if ((Charge+ivar)%2==1) ilTemp=2;
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, iRigTemp);
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); //2020.09.30: without drawing the line
			// rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"));
		} //ivar


		rpQFrame->GetXaxis()->SetTitleSize(0.05);
		rpQFrame->GetXaxis()->SetTitleOffset(0.8);
		rpQFrame->GetXaxis()->SetRangeUser(dAxisL,dAxisU);
		#ifdef NOPULL
		rpQFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
		rpQFrame->GetXaxis()->CenterTitle();
		#endif //NOPULL
		//rpQFrame->GetYaxis()->SetTitle("N. Events");
		rpQFrame->GetYaxis()->SetTitle("Events"); //2020.10.26
		rpQFrame->GetYaxis()->CenterTitle();
		rpQFrame->GetYaxis()->SetTitleSize(0.05);
		rpQFrame->GetYaxis()->SetTitleOffset(0.8);
		rpQFrame->SetMinimum(dEvL);
		rpQFrame->SetMaximum(dEvU);
		rpQFrame->DrawClone();

		////plot the name of the elements
		// for (int ivar=0; ivar<iNVar; ivar++)
		// {
		// 	rrvQXY->setVal(Charge+ivar);
		// 	cout << Charge+ivar << " " << hQDisD->GetEntries() << " " << rapSumpdf->getVal() << endl; //XXX
		// 	if (run_charge!=14 || Charge+ivar!=15) lLegend.DrawLatex(Charge+ivar, pow(hQDisD->GetBinContent(hQDisD->FindBin(Charge+ivar)), (run_charge==11 && Charge+ivar==10)?0.3:0.65), Form("#color[%d]{%s}", kBlack, sElementShort[Charge+ivar-1].c_str()));
		// } //ivar
		
		lCuts.DrawLine(dCutL, dEvL, dCutL, dEvU);
		lCuts.DrawLine(dCutU, dEvL, dCutU, dEvU);
		lCutsfit.DrawLine(dFitL, dEvL, dFitL, dEvU);
		lCutsfit.DrawLine(dFitU, dEvL, dFitU, dEvU);
		lLegend.DrawLatexNDC(.64, .85, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
		lLegend.DrawLatexNDC(.64, .80, Form("#color[%d]{Cut Range[%.2f, %.2f]}", kBlack, dCutL, dCutU));
		lLegend.DrawLatexNDC(.64, .75, Form("#color[%d]{Fit Range[%.2f, %.2f]}", kRed, dFitL, dFitU));
		lLegend.DrawLatexNDC(.64, .70, Form("#color[%d]{Cut Range Events:%.0f}", kBlack, rpQFrame->getFitRangeNEvt(dCutL, dCutU)));
		lLegend.DrawLatexNDC(.64, .65, Form("#color[%d]{Chisquare:%.2f}", kBlack, chis_fit));
		for (int ivar=0; ivar<iNVar; ivar++){
			lLegend.DrawLatexNDC(.7, .60-ivar*0.05, Form("#color[%d]{N_{%d}}: %.0f", iColor[ivar], Charge+ivar, rrvn[ivar]->getVal()));
		}

		//------calculate containmination based on fitting
		cout << "****** calculating containmination ******" << endl;
		// RooAbsReal *rarIntAll = rapSumpdf->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral")); //set the pdf to normalized in [0, 35]
		RooAbsReal *rarInt[iNVar];
		// double events_fit = rpQFrame->getFitRangeNEvt(dCutL, dCutU);
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			rarInt[ivar] = rkpLQ[ivar]->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral"));
			
			double dNprt=1;
			dNprt=rrvn[ivar]->getVal();
			// cout << ivar << " " << dNprt << " " << rarInt[ivar]->getVal() << endl;
			// double dfrac = dNprt * rarInt[ivar]->getVal() / events_cut;
			double dfrac = (dNprt * rarInt[ivar]->getVal()) / (sumrrvn * rarIntAll->getVal());
			
			#ifndef SIMPLEPLOT
			// if (Charge+ivar==run_charge) lLegend.DrawLatexNDC(.12, .85-ivar*0.05, Form("#color[%d]{Q%d}: %.3f%%, shift:%.2e", iColor[ivar], Charge+ivar, dFrac*rarInt[ivar]->getVal()/rarIntAll->getVal()*100., rrvshift[ivar]->getVal()));
			if (Charge+ivar==run_charge) lLegend.DrawLatexNDC(.2, .85-ivar*0.05, Form("#color[%d]{Q%d}: %.2f%%", iColor[ivar], Charge+ivar, dfrac*100.));
			else lLegend.DrawLatexNDC(.2, .85-ivar*0.05, Form("#color[%d]{Q%d}: %.2f%%", iColor[ivar], Charge+ivar, dfrac*100.));
			#endif //SIMPLEPLOT
		} //ivar

		///------store Impurity
		cout << "start storing impurity" << endl;
		double fImp[iNVar];
		double fImpError[iNVar];
		double dIntSumPdf=rarIntAll->getVal();
		double dIntSumPdfDiff[2]={0.};
		double dFrac[iNVar];
		double dFracE[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			dFrac[ivar]= rrvn[ivar]->getVal()*rarInt[ivar]->getVal()/events_cut;
			
			// double sigma_n = rrvn[ivar]->getError();
			double nimp = rrvn[ivar]->getVal()*rarInt[ivar]->getVal();
			double sigma_n = sqrt(nimp);
			double sigma_N = sqrt(events_cut);
			dFracE[ivar] = sqrt( (pow(sigma_n/nimp,2) + pow(sigma_N/events_cut,2))*(pow(dFrac[ivar],2)) );
			// else if (ivar==iNVar-1)
			// {
			// 	dFracE[ivar]=0;
			// 	for (int ivar0=0; ivar0<iNVar-1; ivar0++) dFracE[ivar]+=pow(rrvn[ivar0]->getError(), 2);
			// 	dFracE[ivar]=sqrt(dFracE[ivar]);
			// }
			cout << ivar << " " << dFrac[ivar]*100 << "+/-" << dFracE[ivar]*100 << endl;
		}
		string imerr = "Impurity";
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			histn = Form("%s/%s_r%d_q%d_xy%d", imerr.c_str(), imerr.c_str(), ir, Charge+ivar, ixy);
				cout << histn << endl;
				hImp = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
			hImp->SetBinContent(iSelRig+1, dFrac[ivar]);
			hImp->SetBinError(iSelRig+1, dFracE[ivar]);
			hImp->SetEntries(1);
		} //ivar

		////sotre the chis & error
		histn = Form("Chisquare/Chisquare_r%d_xy%d_sum", ir, ixy);
		hChis = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
		hChis->SetBinContent(iSelRig+1, chis_fit);
		hChis->SetEntries(1);

		for (int ivar=0; ivar<iNVar; ivar++)
		{
			histn = Form("FitParaError/FitParaError_r%d_q%d_xy%d", ir, Charge+ivar, ixy);
			hErr = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
			hErr->SetBinContent(iSelRig+1, dFracE[ivar]);
			hErr->SetEntries(1);
		} //ivar

		for (int ivar=0; ivar<iNVar; ivar++)
		{
			histn = Form("FitShift/FitShift_r%d_q%d_xy%d", ir, Charge+ivar, ixy);
			hShift = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
			hShift->SetBinContent(iSelRig+1, rrvshift[ivar]->getVal());
			hShift->SetEntries(1);
			cout << "\t q_" << Charge+ivar << " \t" << rrvshift[ivar]->getVal() << " \t" << rrvn[ivar]->getVal() / rNum[ivar] << endl;
		} //ivar

		////store the root tree
		double shiftq[iNVar], paraFitq[iNVar], paraerrFitq[iNVar], dIntValPdf[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++){
			shiftq[ivar] = rrvshift[ivar]->getVal();
			paraFitq[ivar] = rrvn[ivar]->getVal();
			paraerrFitq[ivar] = rrvn[ivar]->getError();
			dIntValPdf[ivar] = rarInt[ivar]->getVal();
		}
		tTempFit->Branch("RigidityBin", &iSelRig, "RigidityBin/I");
		tTempFit->Branch("Chisquare", &chis_fit, "Chisquare/D");
		tTempFit->Branch("CutRangeNEvents", &events_cut, "CutRangeNEvents/D");
		tTempFit->Branch("Integral_SumPdf", &dIntSumPdf, "Integral_SumPdf/D");
		for(int ivar = 0; ivar < iNVar; ivar ++){
			int icharge = Charge + ivar;
			tTempFit->Branch(Form("Shift_q%d", icharge), &shiftq[ivar], Form("Shift_q%d/D", icharge));
			tTempFit->Branch(Form("Portion_statistics_q%d", icharge), &dFrac[ivar], Form("Portion_statistics_q%d/D", icharge));
			tTempFit->Branch(Form("Portion_statisticsError_q%d", icharge), &dFracE[ivar], Form("Portion_statisticsError_q%d/D", icharge));
			tTempFit->Branch(Form("Fitting_Parameter_q%d", icharge), &paraFitq[ivar], Form("Fitting_Parameter_q%d/D", icharge));
			tTempFit->Branch(Form("Fitting_ParameterError_q%d", icharge), &paraerrFitq[ivar], Form("Fitting_ParameterError_q%d/D", icharge));
			tTempFit->Branch(Form("Integral_Pdf_q%d", icharge), &dIntValPdf[ivar], Form("Integral_Pdf_q%d/D", icharge));
		}

		#ifndef NOPULL
		//----Draw Pull
		pPad2->cd();
		rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
		rpPullFrame->addPlotable(rhPull,"P");
		rpPullFrame->GetXaxis()->SetTitle("Unbiased L1 Charge");
		rpPullFrame->GetXaxis()->CenterTitle();
		rpPullFrame->GetXaxis()->SetRangeUser(dAxisL,dAxisU);
		rpPullFrame->GetYaxis()->SetRangeUser(-10,10);
		rpPullFrame->GetXaxis()->SetTitleSize(0.1);
		rpPullFrame->GetXaxis()->SetTitleOffset(0.7);
		rpPullFrame->GetXaxis()->SetLabelSize(0.07);
		rpPullFrame->GetYaxis()->SetTitle("Pull");
		rpPullFrame->GetYaxis()->CenterTitle();
		rpPullFrame->GetYaxis()->SetTitleSize(0.1);
		rpPullFrame->GetYaxis()->SetTitleOffset(0.4);
		rpPullFrame->GetYaxis()->SetLabelSize(0.07);
		
		rpPullFrame->DrawClone();
		
		cCanv->cd();
		pPad1->DrawClone();
		pPad2->DrawClone();
		#endif //NOPULL
		
		cCanv->Draw();
		
		#ifdef SIMPLEPLOT
		histn = Form("%s/pic/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d_simplePlot", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig, iNVar);
		#else
		histn = Form("%s/pic/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig, iNVar);
		#endif //SIMPLEPLOT

		cCanv->SaveAs(Form("%s.png", histn.c_str()));
		string histnroot = Form("%s/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d", sDrawRoot.c_str(), ir, run_charge, ixy, iSelRig, iNVar);
		cCanv->SaveAs(Form("%s.root", histnroot.c_str()));
		
		//delete cCanv;
		
		tTempFit->Fill();
		//------
		//--close files
		fQTemp.Close();
		fTotalFile.Close();
	
		//--store output files
		fOutPut->Write();
		fOutPut->Close();
	} //ir


	return 0;
}//end of KeysPdfRead
