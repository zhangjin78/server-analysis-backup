//------------------
//update from: 		KeysPdfRead_batch4.C
//update date: 		2020.03.01
//------------------
//script to be able to use either L1Q or L2Q template using RooKeysPdf from root file, suitable to run in batch
//------------------
//------------------

//#define ATLAS
//#define BAOBAB
#define LXPLUS

#include "include/drawvariable.h"
#include "include/L1QTemplateFit_binning.h"
#include "include/RooFitHeader.h"

//#define SIMPLEPLOT
//#define NOPULL
//#define COMPAREPLOT

//#define L1QTEMPSMALLBIN
//#define L2QTEMPALL
#define FELQTEMP //2020.06.07
//#define SECTEMPFIT //2020.09.30
#ifdef L1QTEMPSMALLBIN
const int iNBL1Temp=L1QTF_NbinSmall;
double *dBL1Temp=L1QTF_BinsSmall;
#elif defined L2QTEMPALL
const int iNBL1Temp=L1QTF_NbinAllQ;
double *dBL1Temp=L1QTF_BinsAllQ;

//binning for template fit
/*const int iNBL1Fit=L1QTF_NbinAllQ;
double *dBL1Fit=L1QTF_BinsAllQ;*/
//2020.09.22
const int iNBL1Fit=L1QTF_NbinAllQFit;
double *dBL1Fit=L1QTF_BinsAllQFit;

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

//#define LINEARY

//#define ODDCHARGEALLRIG //2020.09.30

//2021.05.24
#define ISOTOPEBGSUB

//pointer
TH1F *hQDis = NULL, *hQDis1 = NULL, *hQDisDiff = NULL;
TH1D *hQDisD = NULL, *hQDisD1 = NULL;
TH2F *hQ2dDis=NULL, *hQ2dDis2=NULL;
TH2S *hQ2dDisS=NULL;

TH1F *hImp=NULL, *hIne=NULL;
//2019.12.09
TH1F *hTDiff=NULL;

#ifdef ATLAS
string sWDirOnline = "/atlas/users/chenya";
#elif defined BAOBAB
//string sWDirOnline = "/dpnc/beegfs/users/chenya/atlas_user";
string sWDirOnline = "/home/chenyao"; //2020.10.01
#elif defined LXPLUS
string sWDirOnline = "/afs/cern.ch/user/c/chenya"; //XXX
#endif

int KeysPdfRead_batch5(	const int iSelRig=0,
				const int run_charge=14, 
				const int QVer=1,
				const int NVar0=-1,
				const int ixy=2,
				const int il=2,
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
	TH1F hL1QTemp("hL1QTemp", "hL1QTemp", iNBL1Temp, dBL1Temp);
	const int iRigTemp = hL1QTemp.FindBin(hL1QFit.GetBinCenter(iSelRig+1))-1;
	const double dLRigTemp=hL1QTemp.GetBinLowEdge(iRigTemp+1), dURigTemp=hL1QTemp.GetBinLowEdge(iRigTemp+2);
	cout << "OK set temp bin" << endl; //XXX
	
	cout << "iSelRig=" << iSelRig << " dLRig=" << dLRig << " dURig=" << dURig << " iRigTemp=" << iRigTemp << endl;
	string sPull="";
	#ifdef NOPULL
	sPull="_nopull";
	#endif //NOPULL
	
	//----
	int Charge=run_charge;
	if (run_charge==26) Charge=25;
	//2020.09.30
	//if (run_charge%2==1) Charge=run_charge-1;
	if (run_charge>8 && run_charge%2==1) Charge=run_charge-1; //2021.01.29: only include lower charge for heavy secondary for now
	
	//update to be able to deal Silicon
	double dCutL=0, dCutU=0;
	double dDrawL=0, dDrawU=0; //draw range for template fit
	double dDrawBigL=0, dDrawBigU=0; //big draw range, used for LQ template
	double dEvL=0.5, dEvU=0;
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
		dCutL=run_charge-0.0585*pow(run_charge,1.15)-0.35, dCutU=run_charge+0.0334*pow(run_charge,1.15)+0.20;
		//dDrawL=12, dDrawU=17.5;
		//dDrawBigL=12, dDrawBigU=20.5;
		//2019.11.20: set the draw range according to run_charge
		dDrawL=run_charge-2, dDrawU=run_charge+3.5;
		//if (run_charge%2==1) dDrawU=run_charge+2.5; //2020.09.30: specify for Secondary
		//if (run_charge%2==1) dDrawL=run_charge-1.35, dDrawU=run_charge+2.35; //2020.10.26: same as the F paper
		//2020.11.15: same as the F paper
		if (run_charge==9) dDrawL=run_charge-1.35, dDrawU=run_charge+2.35;
		else if (run_charge==11) dDrawL=run_charge-1.4, dDrawU=run_charge+2.35;
		else if (run_charge==13) dDrawL=run_charge-1.7, dDrawU=run_charge+2.35;
		
		dDrawBigL=run_charge-2, dDrawBigU=run_charge+6.5;
		//if (run_charge==26) dDrawL=run_charge-4, dDrawU=run_charge+3.5; //2020.06.06
		if (run_charge==26) //2020.06.17
		{
			//dCutL = run_charge-2.83, run_charge+1.62;
			dCutL = run_charge-2.83, dCutU = run_charge+1.62; //2020.09.22
			dDrawL=20, dDrawU=30;
		}
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
	
	double dTempFitL=0, dTempFitU=35;
	
	int iNVar=(run_charge==8)?3:3;
	if (run_charge==26) iNVar=4;
	
	if (NVar0!=-1) iNVar=NVar0;
	
//	int NRebin=10;
	int NRebin=2;
	//if (run_charge>=10) NRebin=5;
	if (run_charge>=9) NRebin=5;
	
	TLine lCuts;
	lCuts.SetLineWidth(3);
	lCuts.SetLineStyle(2);
	
	RooPlot *rpQFrame=NULL, *rpPullFrame=NULL;
	RooDataHist *rdhLQ=NULL;
	
	//int fillStyle=3002;
	int fillStyle=3001;
	
	RooDataHist *rdhLQTemp=NULL, *rdhL1Q=NULL;
	RooFitResult *rfrResult;
	
	//----retrieve RooWorkspace
	/*//2020.09.01
	int iDstver=63;
	if (run_charge>20) iDstver=64;*/
	//2020.09.22
	int iDstver=64;
	
	string sTotalTemp;
	sTotalTemp = "LQTemp4";
	if (run_charge>20) sTotalTemp = "LQTempUHZ7_noRigCor";
	else if (run_charge<8) sTotalTemp = "LQTemp4_LiN";
	string sLQTemp = sTotalTemp;
	if (ispan==0) sTotalTemp = "L1QTempFit4";
	
	const string sTotalDir = Form("%s/result/analysis6/amsd%dn_B1130_%s/total", sWDirOnline.c_str(), iDstver, sTotalTemp.c_str());
	
	const string sTempDir=Form("%s/result/analysis6/amsd%dn_B1130_%s/total", sWDirOnline.c_str(), iDstver, sLQTemp.c_str());
	TFile fQTemp(Form("%s/LQTemp/LQTemp_rig%d.root", sTempDir.c_str(), iRigTemp), "READ");
	cout << " *** Fit ibin=" << iSelRig << " temp ibin=" << iRigTemp << " *** " << endl;
	
	TFile fTotalFile(Form("%s/total_result.root", sTempDir.c_str()), "READ");
	TFile fL1Q(Form("%s/total_result.root", sTotalDir.c_str()), "READ");
	
	//--
	RooWorkspace *rwsLQTemp = static_cast<RooWorkspace*>(fQTemp.Get(Form("LQ_template")));
	if (ixy==0) histn=Form("L%dQX", 1);
	else if (ixy==1) histn=Form("L%dQY", 1);
	else if (ixy==2) histn=Form("L%dQXY", 1);
	
	RooRealVar *rrvQXY = rwsLQTemp->var(histn.c_str());
	rrvQXY->setRange("Integral", dCutL, dCutU);
	
	//NOT USED
	TFile fQTempAR(Form("%s/LQTemp/LQTemp_allrig.root", sTempDir.c_str()), "READ");
	RooWorkspace *rwsLQTempAR = static_cast<RooWorkspace*>(fQTempAR.Get(Form("LQ_template")));
	for (int ir=1; ir<2; ir++)
	{
		for (int ich=Charge; ich<Charge+iNVar; ich++)
		{
			//if (ich==run_charge) continue;
			if (ich>20 && ich==run_charge) continue; //2020.09.30
			//2020.09.30
			if (ich<20 && ich%2==0) continue;
			//2021.01.29
			if (ich<8) break;
			
			//--
			int ilTemp=il;
			if (ich>14 && ich!=26) ilTemp=2;
			//2020.10.01: use L2Q template for secondary signal
			//if (ich==run_charge && ich%2==1) ilTemp=2;
			if (ich==run_charge && (ich%2==1 || ich==4)) ilTemp=2; //2021.01.30: include Be as well
			
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge+ivar==14)?2:ir;
			#endif //FSTEMPLATE
			
			//--
			RooKeysPdf *rkpLQTemp;
			histn1 = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, ich, ixy, iSelTemp, -1);
			rkpLQTemp = static_cast<RooKeysPdf*>(rwsLQTempAR->pdf(histn1.c_str()));
			cout << histn1 << " " << rkpLQTemp << endl; //XXX
			rwsLQTemp->import(*rkpLQTemp, RooFit::RecycleConflictNodes()); //RecycleConflictNodes: use the existing formula in the workspace when importing
		} //ich
	} //ir
	//NOT USED
	
	//---- 
	for (int ir=ispan; ir<ispan+1; ir++)
	{
		cout << "Span=" << ir << endl;
		
		//----output directory
		string sDraw = Form("%s/L1QTempFit/r%d_xy%d_NVar%d_rebin%d_q%d", sTotalDir.c_str(), ir, ixy, iNVar, NRebin, run_charge);
		if (sTotalTemp.compare(sLQTemp)!=0) sDraw = Form("%s/L1QTempFit_%s/r%d_xy%d_NVar%d_rebin%d_q%d", sTotalDir.c_str(), sLQTemp.c_str(), ir, ixy, iNVar, NRebin, run_charge);
		if (bBadLadder) sDraw = sDraw + "_wBadLadder";
		
		#ifdef ISOTOPEBGSUB
		sDraw = sDraw + "_BGSub";
		#endif //ISOTOPEBGSUB
		
		string command = "mkdir -pv "+sDraw;
		cout << "Create output directory: " << sDraw << " " << system(command.c_str()) << endl;
		
		//create separete directory for LQTemplate & LQ distribution comparison, LQTempalte & L1Q distribution comparison and L1QTemplate fit
		//--LQ vs LQTemp
		string sDrawLQTemp = Form("%s/L%dQTemp", sDraw.c_str(), il);
		cout << "Create pic directory for L" << il << "QTemp " << system(Form("mkdir -pv %s", sDrawLQTemp.c_str())) << endl;
		
		//--L1Q vs LQTemp
		string sDrawL1QLQTemp = Form("%s/L1QvsL%dQTemp", sDraw.c_str(), il);
		cout << "Create pic directory for L1QvsL" << il << "QTemp " << system(Form("mkdir -pv %s", sDrawL1QLQTemp.c_str())) << endl;
		
		//--L1QTempFit
		string sDrawL1QTempFit = Form("%s/L1QTempFit_L%dQTemp", sDraw.c_str(), il);
		#ifdef SIMPLEPLOT
		sDrawL1QTempFit = sDrawL1QTempFit + "_simpleplot";
		cout << sDrawL1QTempFit << endl;
		#endif //SIMPLEPLOT
		cout << "Create pic directory  for L1QTempFit " << system(Form("mkdir -pv %s", sDrawL1QTempFit.c_str())) << endl;
		
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
		
		fOutPut->ls();
		
		//----shift variables
		RooRealVar *rrvshift[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge+ivar==14)?2:ir;
			#endif //FSTEMPLATE
			//2021.03.26
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe //2020.06.17
			int ilTemp=il;
			if (Charge+ivar>14 && Charge+ivar!=26) ilTemp=2;
			//2020.10.01: use L2Q template for secondary signal
			if ((Charge+ivar==run_charge) && (Charge+ivar)%2==1) ilTemp=2;
			
			rrvshift[ivar] = rwsLQTemp->var(Form("r%d_q%d_xy%d_shift_L%dQTemp", iSelSpan, Charge+ivar, ixy, ilTemp));
			
			//fix the shift variable for L1Q template
			if (ilTemp==1)
			{
				rrvshift[ivar]->setVal(0);
				rrvshift[ivar]->setConstant(kTRUE);
			}
		} //ivar
		
		//------
		cout << "****** retrieving L" << il << "Q Template for each charge, compare the L1Q distribution vs L" << il << "Q Template ******" << endl;
		for (int ich=Charge; ich<Charge+iNVar; ich++)
		{
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(ich==14)?2:ir;
			#endif //FSTEMPLATE
			//2021.03.26
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe
			int ilTemp=il;
			if (ich>14 && ich!=26) ilTemp=2;
			//2020.10.01: use L2Q template for secondary signal
			//if (ich==run_charge && ich%2==1) ilTemp=2;
			if (ich==run_charge && (ich%2==1 || ich==4)) ilTemp=2; //2021.01.30: include Be as well
			
			cout << "****** ich=" << ich << " iSelSpan=" << iSelSpan << " ilTemp=" << ilTemp << " ******" << endl;
			histn = Form("L%dQKeysPdf_r%d_q%d_xy%d_rig%d", ilTemp, ir, ich, ixy, iSelRig);
			
			//--
			cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);
			//----charge distribution
			pPad1 = new TPad("", "", .0, .3, 1., 1.);
			pPad1->cd();
			gPad->SetLogy();
			gPad->SetGrid();
			gPad->SetBottomMargin(0);
			gPad->SetRightMargin(0.1);
			
			//----pull
			pPad2 = new TPad("", "", .0, .0, 1., .3);
			pPad2->cd();
			gPad->SetGrid();
			gPad->SetTopMargin(0);
			gPad->SetBottomMargin(0.2);
			gPad->SetRightMargin(0.1);
			
			//----
			cCanv->cd();
			pPad1->Draw();
			pPad2->Draw();
			
			//----
			pPad1->cd();
			
			rpQFrame = rrvQXY->frame(ich-3, ich+3);
			
			//--RooDataHist
			if (ilTemp==1) histn = Form("%s/q%d/%s/%.1fsigma/L%dQTemp/QvsRig/L%dQvsRig_r%d_q%d_xy%d_L%dQTemp%.1fsigma_Tk2ndCut", sDir.c_str(), ich, sSpan[iSelSpan].c_str(), fSelSigma, ilTemp, ilTemp, iSelSpan, ich, ixy, ilTemp, fSelSigma);
			else if (ilTemp==2) histn = Form("%s/q%d/%s/%.1fsigma/L%dQTemp/QvsRig_Cut/L%dQvsRig_Cut_r%d_q%d_xy%d_L%dQTemp%.1fsigma", sDir.c_str(), ich, sSpan[iSelSpan].c_str(), fSelSigma, ilTemp, ilTemp, iSelSpan, ich, ixy, ilTemp, fSelSigma);
			
			hQ2dDis = static_cast<TH2F*>(fTotalFile.Get(histn.c_str()));
			cout << histn << " " << hQ2dDis << " " << fTotalFile.GetPath() << endl; //XXX
			
			cout << iSelTemp << "  [" << dLRigTemp << "," << dURigTemp << "]" << endl;
			cout << hQ2dDis->GetXaxis()->FindBin(dLRigTemp) << " to " << hQ2dDis->GetXaxis()->FindBin(dURigTemp)-1 << endl;
			
			histn = Form("L%dQTemp_r%d_rig%d", ilTemp, iSelSpan, iSelRig);
			hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRigTemp), hQ2dDis->GetXaxis()->FindBin(dURigTemp)-1, "e");
			
			hQDisD->Rebin(NRebin);
			
			histn = Form("L%dQ_r%d_q%d_xy%d_LQSam%.1fsigma_rig%d", ilTemp, iSelSpan, ich, ixy, fSelSigma, iRigTemp);
			if (ich>14 && ich!=26) histn = Form("L%dQ_r%d_q%d_xy%d_LQSam%.1fsigma_allrig", ilTemp, iSelSpan, ich, ixy, fSelSigma);
			cout << histn << endl;
			rdhLQTemp = new RooDataHist(histn.c_str(), histn.c_str(), RooArgList(*rrvQXY), hQDisD);
			cout << histn << " ****** " << rdhLQTemp << endl;
			rdhLQTemp->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::Name(histn.c_str()), RooFit::DrawOption("P"));
			
			//--RooKeysPdf
			legend = new TLegend(.7, .5, .9, .7);
			
			RooKeysPdf *rkpLQTemp;
			
			//fix bandwidth
			histn1 = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, ich, ixy, iSelTemp, iRigTemp);
			if (ich>14 && ich!=26) histn1 = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig-1_KeysPdf", ilTemp, iSelSpan, ich, ixy, iSelTemp);
			rkpLQTemp = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn1.c_str()));
			cout << histn1 << " ****** " << rkpLQTemp << endl;
			
			//--plot pdf
			rkpLQTemp->plotOn(rpQFrame, RooFit::LineColor(iColor[1]), RooFit::Name(histn1.c_str()), RooFit::DrawOption("L"));
			RooHist *rhPull_fixbandwidth = rpQFrame->pullHist(histn.c_str(), histn1.c_str());
			rhPull_fixbandwidth->SetMarkerColor(iColor[1]);
			rhPull_fixbandwidth->SetLineColor(iColor[1]);
			legend->AddEntry(rhPull_fixbandwidth, "RooKeysPdf, fix bandwidth", "L");
			
			rpQFrame->SetMinimum(dEvL);
			rpQFrame->SetMaximum(dEvU);
			//rpQFrame->GetYaxis()->SetTitle("N. Events");
			rpQFrame->GetYaxis()->SetTitle("Events"); //2020.10.26
			
			rpQFrame->DrawClone();
			lLegend.DrawLatexNDC(.12, .85, Form("#color[%d]{Rig: [%.2f, %.2f] GV}", kBlack, dLRigTemp, dURigTemp));
			lLegend.DrawLatexNDC(.12, .80, Form("#color[%d]{L%dQ: %.4e}", kBlack, ilTemp, hQDisD->GetEntries()));
			
			lLegend.DrawLatexNDC(.70, .85, Form("%s", sElementFull[ich-1].c_str()));
			
			//--Draw Pull
			pPad2->cd();
			
			rpPullFrame = rrvQXY->frame();
			rpPullFrame = rrvQXY->frame(ich-3, ich+3);
			//rpPullFrame->addPlotable(rhPull,"P");
			//rpPullFrame->addPlotable(rhPull_adaptive,"P");
			rpPullFrame->addPlotable(rhPull_fixbandwidth,"P");
			rpPullFrame->GetXaxis()->SetTitle(Form("TRACKER L%d Charge", ilTemp));
			rpPullFrame->GetXaxis()->CenterTitle();
			rpPullFrame->GetXaxis()->SetTitleSize(0.1);
			rpPullFrame->GetXaxis()->SetTitleOffset(0.7);
			rpPullFrame->GetXaxis()->SetLabelSize(0.07);
			rpPullFrame->GetYaxis()->SetTitle("Pull");
			rpPullFrame->GetYaxis()->CenterTitle();
			rpPullFrame->GetYaxis()->SetTitleSize(0.1);
			rpPullFrame->GetYaxis()->SetTitleOffset(0.4);
			rpPullFrame->GetYaxis()->SetLabelSize(0.07);
			
			rpPullFrame->DrawClone();
			
			cCanv->Draw();
			histn = Form("%s/L%dQTemp_r%d_q%d_xy%d_rig%d", sDrawLQTemp.c_str(), ilTemp, ir, ich, ixy, iSelRig);
			
			#ifdef FSTEMPLATE
			histn = histn+"_FSTEMP";
			#endif //FSTEMPLATE
			cCanv->SaveAs(Form("%s.png", histn.c_str()));
			delete cCanv;
			
			//--L1Q & LQ compare
			histn = Form("L1Q&L%dQ_r%d_q%d_xy%d_rig%d", ilTemp, ir, ich, ixy, iSelRig);
			cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);
			
			//----L1Q and LQ
			pPad1 = new TPad("", "", .0, .3, 1., 1.);
			pPad1->cd();
			gPad->SetLogy();
			gPad->SetGrid();
			gPad->SetBottomMargin(0);
			gPad->SetRightMargin(0.1);
			
			//----pull
			pPad2 = new TPad("", "", .0, .0, 1., .3);
			pPad2->cd();
			gPad->SetGrid();
			gPad->SetTopMargin(0);
			gPad->SetBottomMargin(0.2);
			gPad->SetRightMargin(0.1);
			
			//----
			cCanv->cd();
			pPad1->Draw();
			pPad2->Draw();
			
			//----L1Q and LQ
			pPad1->cd();
			rpQFrame = rrvQXY->frame(ich-3, ich+3);
			
			//--L1Q RooDataHist
			//get the L1Q histogram from QvsRig distirbution
			histn = Form("%s/q%d/%s/%.1fsigma/L1QSam/QvsRig/L1QvsRig_r%d_q%d_xy%d_L1QSam%.1fsigma_uTOFQ_tight", sDir.c_str(), ich, sSpan[ir].c_str(), fSelSigma, ir, ich, ixy, fSelSigma);
			if (ir==0)
			{
				#ifdef ISOTOPEBGSUB
				histn = Form("%s/q%d/%s/%.1fsigma/L1QSam/QvsRig/unbiasedL1QvsRig_r%d_q%d_xy%d_L1QSam%.1fsigma_BGSub_uTOFQ_tight", sDir.c_str(), ich, sSpan[ir].c_str(), fSelSigma, ir, ich, ixy, fSelSigma);
				#else
				histn = Form("%s/q%d/%s/%.1fsigma/L1QSam/QvsRig/unbiasedL1QvsRig_r%d_q%d_xy%d_L1QSam%.1fsigma_uTOFQ_tight", sDir.c_str(), ich, sSpan[ir].c_str(), fSelSigma, ir, ich, ixy, fSelSigma);
				#endif //ISOTOPEBGSUB
			}
			hQ2dDis = static_cast<TH2F*>(fL1Q.Get(histn.c_str()));
			cout << histn << " " << hQ2dDis << endl; //XXX
			
			histn = Form("L1Q_r%d_rig%d_uTOFQ_tight", ir, iSelRig);
			hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRigTemp), hQ2dDis->GetXaxis()->FindBin(dURigTemp)-1, "e");
			
			hQDisD->Rebin(NRebin);
			
			histn = Form("L1Q_r%d_q%d_xy%d_L1QSam%.1fsigma_rig%d", ir, ich, ixy, fSelSigma, iRigTemp);
			if (ir==0) histn = Form("unbiasedL1Q_r%d_q%d_xy%d_L1QSam%.1fsigma_rig%d", ir, ich, ixy, fSelSigma, iRigTemp); //2021.03.27
			rdhL1Q = new RooDataHist(histn.c_str(), histn.c_str(), RooArgList(*rrvQXY), hQDisD);
			rdhL1Q->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));
			
			//--RooKeysPdf
			histn1 = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, ich, ixy, iSelTemp, iRigTemp);
			if (ich>14 && ich!=26) histn1 = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig-1_KeysPdf", ilTemp, iSelSpan, ich, ixy, iSelTemp);
			rkpLQTemp = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn1.c_str()));
			//--plot pdf
			cout << "Q" << ich << " Current shift: " << rrvshift[ich-Charge]->getVal() << endl;
			rrvQXY->setRange(Form("Fit_L1Q_q%d", ich), ich-1, ich+0.5);
			//--
			rkpLQTemp->fitTo(*rdhL1Q, RooFit::Save(), RooFit::Range(Form("Fit_L1Q_q%d", ich)), RooFit::PrintLevel(-1));
			//--
			rkpLQTemp->plotOn(rpQFrame, RooFit::Range(ich-3, ich+3), RooFit::LineColor(iColor[0]), RooFit::DrawOption("L"));
			
			cout << "****** Shift after fit: " << rrvshift[ich-Charge]->getVal() << " ******" << endl;
			if (ich!=run_charge) rrvshift[ich-Charge]->setConstant(kTRUE);
			
			rpQFrame->SetMinimum(dEvL);
			rpQFrame->SetMaximum(dEvU);
			
			//rpQFrame->GetYaxis()->SetTitle("N. Events");
			rpQFrame->GetYaxis()->SetTitle("Events"); //2020.10.26
			
			rpQFrame->DrawClone();
			lLegend.DrawLatexNDC(.12, .85, Form("#color[%d]{Rig: [%.2f, %.2f] GV}", kBlack, dLRigTemp, dURigTemp));
			lLegend.DrawLatexNDC(.12, .80, Form("#color[%d]{L1Q: %.4e}", kBlack, hQDisD->GetEntries())); //print the L1Q events
			lLegend.DrawLatexNDC(.12, .75, Form("#color[%d]{L%dQ KeysPdf, shift: %.2e}", iColor[0], ilTemp, rrvshift[ich-Charge]->getVal()));
			
			lLegend.DrawLatexNDC(.70, .85, Form("%s", sElementFull[ich-1].c_str()));
			
			//--Draw Pull
			pPad2->cd();
			RooHist *rhPull = rpQFrame->pullHist();
			rpPullFrame = rrvQXY->frame(ich-3, ich+3);
			rpPullFrame->addPlotable(rhPull,"P");
			rpPullFrame->GetXaxis()->SetTitle("TRACKER L1 Charge");
			rpPullFrame->GetXaxis()->CenterTitle();
			rpPullFrame->GetXaxis()->SetTitleSize(0.1);
			rpPullFrame->GetXaxis()->SetTitleOffset(0.7);
			rpPullFrame->GetXaxis()->SetLabelSize(0.07);
			rpPullFrame->GetYaxis()->SetTitle("Pull");
			rpPullFrame->GetYaxis()->CenterTitle();
			rpPullFrame->GetYaxis()->SetTitleSize(0.1);
			rpPullFrame->GetYaxis()->SetTitleOffset(0.4);
			rpPullFrame->GetYaxis()->SetLabelSize(0.07);
			rpPullFrame->DrawClone();
			cCanv->Draw();
			histn = Form("%s/L1Q&L%dQ_r%d_q%d_xy%d_rig%d", sDrawL1QLQTemp.c_str(), ilTemp, ir, ich, ixy, iSelRig);
			#ifdef FSTEMPLATE
			histn = histn+"_FSTEMP";
			#endif //FSTEMPLATE
			cCanv->SaveAs(Form("%s.png", histn.c_str()));
			delete cCanv;
			
			//plot the LQ template along with fill color
			histn = Form("LQTemp_r%d_q%d_xy%d_rig%d", ir, ich, ixy, iSelRig);
			cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 600);
			gPad->SetLogy();
			gPad->SetGrid();
			gPad->SetTopMargin(0.05);
			gPad->SetRightMargin(0.05);
			
			rpQFrame = rrvQXY->frame(dCutL, ich+5);
			rkpLQTemp->plotOn(rpQFrame, RooFit::Range(dCutL, dCutU), RooFit::LineColor(iColor[0]), RooFit::FillColor(iColor[0]), RooFit::FillStyle(fillStyle), RooFit::DrawOption("FL"),RooFit::VLines());
			rkpLQTemp->plotOn(rpQFrame, RooFit::Range(dCutU, 35.), RooFit::LineColor(iColor[0]), RooFit::FillColor(iColor[1]), RooFit::FillStyle(fillStyle), RooFit::DrawOption("FL"),RooFit::VLines());
			rpQFrame->SetMinimum(1e-5);
			rpQFrame->SetMaximum(1);
			rpQFrame->GetYaxis()->SetTitle("Charge Template");
			
			rpQFrame->GetXaxis()->SetTitle("TRACKER L1 Charge");
			rpQFrame->DrawClone();
			lLegend.DrawLatexNDC(.70, .85, Form("%s", sElementFull[ich-1].c_str()));
			lCuts.DrawLine(dCutU, 1e-5, dCutU, 1);
			cCanv->Draw();
			histn = Form("%s/L%dQTemp_r%d_q%d_xy%d_rig%d_along", sDrawLQTemp.c_str(), ilTemp, ir, ich, ixy, iSelRig);
			cCanv->SaveAs(Form("%s.png", histn.c_str()));
			delete cCanv;
		} //ich
		
		cout << "****** Performing L1Q template fit ******" << endl;
		//------L1Q template fit
		histn = Form("L1Q_r%d_q%d_xy%d_rig%d", ir, run_charge, ixy, iSelRig);
		#ifdef COMPAREPLOT
		cCanv = new TCanvas(histn.c_str(), histn.c_str(), 600, 800);
		#else
		cCanv = new TCanvas(histn.c_str(), histn.c_str(), 1200, 800);
		#endif //COMPAREPLT
		
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
		histn = Form("Charge_Calibration%d/q%d/%s/2.0sigma/L1QSam/QvsRig/L1QvsRig_r%d_q%d_xy2_L1QSam2.0sigma", QVer, run_charge, sSpan[ir].c_str(), ir, run_charge);
		if (ir==0)
		{
			#ifdef ISOTOPEBGSUB
			histn = Form("%s/q%d/%s/%.1fsigma/L1QSam/QvsRig/unbiasedL1QvsRig_r%d_q%d_xy%d_L1QSam%.1fsigma_BGSub", sDir.c_str(), run_charge, sSpan[ir].c_str(), fSelSigma, ir, run_charge, ixy, fSelSigma);
			#else
			histn = Form("%s/q%d/%s/%.1fsigma/L1QSam/QvsRig/unbiasedL1QvsRig_r%d_q%d_xy%d_L1QSam%.1fsigma", sDir.c_str(), run_charge, sSpan[ir].c_str(), fSelSigma, ir, run_charge, ixy, fSelSigma);
			#endif //ISOTOPEBGSUB
		}
		hQ2dDis = static_cast<TH2F*>(fL1Q.Get(histn.c_str()));
		cout << iSelRig << "  [" << dLRig << "," << dURig << "]" << endl;
		cout << hQ2dDis->GetXaxis()->FindBin(dLRig) << " to " << hQ2dDis->GetXaxis()->FindBin(dURig)-1 << endl;
		
		histn = Form("L1Q_r%d_rig%d", ir, iSelRig);
		hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRig), hQ2dDis->GetXaxis()->FindBin(dURig)-1, "e");
		
		if (hQDisD->GetNbinsX()==3500) hQDisD->Rebin(NRebin);
		
		histn = Form("L1Q_r%d_q%d_xy%d_L1QSam%.1fsigma_rig%d", ir, run_charge, ixy, fSelSigma, iSelRig);
		rdhL1Q = new RooDataHist(histn.c_str(), histn.c_str(), RooArgList(*rrvQXY), hQDisD);
		rdhL1Q->plotOn(rpQFrame, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1), RooFit::DrawOption("P"));
		cout << "******Finish plotting L1Q******" << endl;
		
		//--sum pdf
		//----build fraction variable
		RooRealVar *rrvn[iNVar-1];
		for (int ivar=0; ivar<iNVar-1; ivar++)
		{
			histn = Form("n%d", ivar);
			double dIni, dL, dU;
			if (Charge+ivar==run_charge) dIni=0.9700, dL=0.85, dU=1.0;
			else dIni=0.0050, dL=0.00, dU=0.15;
			//limits for secondaries
			if (run_charge%2==1)
			{
				if (Charge+ivar==run_charge) dIni=0.8500, dL=0.4, dU=1.0;
				else dIni=0.0050, dL=0.00, dU=0.50;
			}
			rrvn[ivar] = new RooRealVar(histn.c_str(), histn.c_str(), dIni, dL, dU);
		} //ivar
		
		//----retrieve RooKeysPdf for each charge
		RooKeysPdf *rkpLQ[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge+ivar==14)?2:ir;
			#endif //FSTEMPLATE
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe
			int ilTemp=il;
			if (Charge+ivar>14 && Charge+ivar!=26) ilTemp=2;
			//use L2Q template for secondary signal
			if ((Charge+ivar==run_charge) && (Charge+ivar)%2==1) ilTemp=2;
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, iRigTemp);
			#ifdef ODDCHARGEALLRIG
			if (((Charge+ivar)%2==1) || (Charge+ivar>14 && Charge+ivar!=26))
			#else
			if (Charge+ivar>14 && Charge+ivar!=26)
			#endif //ODDCHARGEALLRIG
			{
				histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, -1);
			}
			
			rkpLQ[ivar] = static_cast<RooKeysPdf*>(rwsLQTemp->pdf(histn.c_str()));
		} //ivar
		
		histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf_sum", il, ir, Charge, ixy, iSelTemp, iSelRig);
		RooAddPdf *rapSumpdf=NULL;
		RooArgList ralLQ, raln;
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			ralLQ.add(*rkpLQ[ivar]);
			if (ivar==iNVar-1) continue;
			raln.add(*rrvn[ivar]);
		}
		rapSumpdf = new RooAddPdf(histn.c_str(), histn.c_str(), ralLQ,raln);
		
		//------fit
		double dFitL=Charge-0.5, dFitU=Charge+(iNVar-1)+0.5;
		
		rrvQXY->setRange("Fit", dFitL, dFitU); //set the fit range as [run_charge-3, run_charge+(iNVar-1)+0.5]
		rrvQXY->setRange("Norm", 0., 35.);
		rrvQXY->setRange("draw", dDrawL, dDrawU);
		
		cout << "start performing fit, fit range: [" << dFitL << "," << dFitU << "]" << endl;
		rfrResult = rapSumpdf->fitTo(*rdhL1Q, RooFit::Save(), RooFit::Range("Fit"), RooFit::PrintLevel(1));
		cout << "Finish performing fit" << endl;
		
		//------plot the fit result
		//----sum pdf
		rapSumpdf->plotOn(rpQFrame, RooFit::LineColor(kRed), RooFit::Range("draw"), RooFit::DrawOption("L"));
		
		//----chisquare & pull
		RooHist *rhPull = rpQFrame->pullHist();
		
		//----components
		if (run_charge==26)
		{
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge==14)?2:ir;
			#endif //FSTEMPLATE
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", il, iSelSpan, Charge+1, ixy, iSelTemp, iRigTemp);
		
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[1]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); //without drawing the line
		}
		if (run_charge%2==1)
		{
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge==14)?2:ir;
			#endif //FSTEMPLATE
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			int ilTemp=il;
			if (run_charge>14 && run_charge!=26) ilTemp=2;
			//use L2Q template for secondary signal
			if (run_charge%2==1) ilTemp=2;
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, run_charge, ixy, iSelTemp, iRigTemp);
			#ifdef ODDCHARGEALLRIG
			if (run_charge%2==1 || (run_charge>14 && run_charge!=26))
			#else
			if (run_charge>14 && run_charge!=26)
			#endif //ODDCHARGEALLRIG
			{
				histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, run_charge, ixy, iSelTemp, -1);
			}
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[run_charge-Charge]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines());
		}
		
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			//if (run_charge==14 && ivar==1) continue; //plot q15 at last
			if ((run_charge==14 || run_charge==26) && ivar==1) continue; //plot q15 at last, plot q26 first
			//--for secondary, plot the signal template first //2020.09.30
			if (run_charge%2==1 && Charge+ivar==run_charge) continue;
			
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge+ivar==14)?2:ir;
			#endif //FSTEMPLATE
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			//--force to use L2QTemp for UHZ except Fe
			int ilTemp=il;
			if (Charge+ivar>14 && Charge+ivar!=26) ilTemp=2;
			//use L2Q template for secondary signal
			if ((Charge+ivar==run_charge) && (Charge+ivar)%2==1) ilTemp=2;
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, iRigTemp);
			#ifdef ODDCHARGEALLRIG
			if ((Charge+ivar)%2==1 || (Charge+ivar>14 && Charge+ivar!=26))
			#else
			if (Charge+ivar>14 && Charge+ivar!=26)
			#endif //ODDCHARGEALLRIG
			{
				histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", ilTemp, iSelSpan, Charge+ivar, ixy, iSelTemp, -1);
			}
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(0), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[ivar]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); //2020.09.30: without drawing the line
		} //ivar
		
		if (run_charge==14)
		{
			int iSelSpan=ir;
			#ifdef FSTEMPLATE
			iSelSpan=(run_charge==14)?2:ir;
			#endif //FSTEMPLATE
			if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
			
			histn = Form("L%dQ_r%d_q%d_xy%d_sel%d_rig%d_KeysPdf", il, iSelSpan, Charge+1, ixy, iSelTemp, iRigTemp);
		
			rapSumpdf->plotOn(rpQFrame, RooFit::Components(histn.c_str()), RooFit::LineWidth(1), RooFit::FillStyle(fillStyle), RooFit::FillColor(iColor[1]), RooFit::Range("draw"), RooFit::DrawOption("FL"), RooFit::VLines()); //without drawing the line
		}
		
		rpQFrame->GetXaxis()->SetTitleSize(0.05);
		rpQFrame->GetXaxis()->SetTitleOffset(0.8);
		#ifdef NOPULL
		rpQFrame->GetXaxis()->SetTitle("TRACKER L1 Charge");
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
		
		//plot the name of the elements
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			rrvQXY->setVal(Charge+ivar);
			cout << Charge+ivar << " " << hQDisD->GetEntries() << " " << rapSumpdf->getVal() << endl; //XXX
			if (run_charge!=14 || Charge+ivar!=15) lLegend.DrawLatex(Charge+ivar, pow(hQDisD->GetBinContent(hQDisD->FindBin(Charge+ivar)), (run_charge==11 && Charge+ivar==10)?0.3:0.65), Form("#color[%d]{%s}", kBlack, sElementShort[Charge+ivar-1].c_str()));
		} //ivar
		
		lCuts.DrawLine(dCutL, dEvL, dCutL, dEvU);
		lCuts.DrawLine(dCutU, dEvL, dCutU, dEvU);
		lLegend.DrawLatexNDC(.60, .90, Form("#color[%d]{[%.2f GV, %.2f GV]}", kBlack, dLRig, dURig));
		
//		lLegend.DrawLatexNDC(.70, .85, Form("n1=%.4f", n1.getVal()));
//		lLegend.DrawLatexNDC(.70, .80, Form("n2=%.4f", n2.getVal()));
		
		//lLegend.DrawLatexNDC(.12, .85, Form("#color[%d]{Chis/NDF=%d/%d}", kBlack, (int)rfrResult.minNLL(), dURig));
		//lLegend.DrawLatexNDC(.12, .85, Form("#color[%d]{Chis=%d}", kBlack, (int)rfrResult->minNll()));
		
		//------calculate containmination based on fitting
		cout << "****** calculating containmination ******" << endl;
		RooAbsReal *rarIntAll = rapSumpdf->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral")); //set the pdf to normalized in [0, 35]
		RooAbsReal *rarInt[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			rarInt[ivar] = rkpLQ[ivar]->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral"));
			
			double dFrac=1;
			if (ivar<iNVar-1) dFrac=rrvn[ivar]->getVal();
			else if (ivar==iNVar-1) for (int ivar0=0; ivar0<iNVar-1; ivar0++) dFrac-=rrvn[ivar0]->getVal();
			cout << ivar << " " << dFrac << endl;
			
			#ifndef SIMPLEPLOT
			if (Charge+ivar==run_charge) lLegend.DrawLatexNDC(.12, .85-ivar*0.05, Form("#color[%d]{Q%d}: %.3f%%, shift:%.2e", iColor[ivar], Charge+ivar, dFrac*rarInt[ivar]->getVal()/rarIntAll->getVal()*100., rrvshift[ivar]->getVal()));
			else lLegend.DrawLatexNDC(.12, .85-ivar*0.05, Form("#color[%d]{Q%d}: %.3f%%", iColor[ivar], Charge+ivar, dFrac*rarInt[ivar]->getVal()/rarIntAll->getVal()*100.));
			#endif //SIMPLEPLOT
		} //ivar
		
		//------store Impurity
		cout << "start storing impurity" << endl;
		double fImp[iNVar];
		double fImpError[iNVar];
		double dIntSumPdf=rarIntAll->getVal();
		double dIntSumPdfDiff[2]={0.};
		double dFrac[iNVar];
		double dFracE[iNVar];
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			dFrac[ivar]=1;
			if (ivar<iNVar-1) dFrac[ivar]=rrvn[ivar]->getVal();
			else if (ivar==iNVar-1) for (int ivar0=0; ivar0<iNVar-1; ivar0++) dFrac[ivar]-=rrvn[ivar0]->getVal();
			
			//2020.07.12
			if (ivar<iNVar-1) dFracE[ivar]=rrvn[ivar]->getError();
			else if (ivar==iNVar-1)
			{
				dFracE[ivar]=0;
				for (int ivar0=0; ivar0<iNVar-1; ivar0++) dFracE[ivar]+=pow(rrvn[ivar0]->getError(), 2);
				dFracE[ivar]=sqrt(dFracE[ivar]);
			}
			cout << ivar << " " << dFrac[ivar] << "+/-" << dFracE[ivar] << endl;
		}
		
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			fImp[ivar] = dFrac[ivar]*rarInt[ivar]->getVal()/dIntSumPdf;
			fImpError[ivar] = sqrt(fImp[ivar]*(1-fImp[ivar])/ (dIntSumPdf*hQDisD->Integral(1,hQDisD->GetNbinsX())) ); //2020.01.08: add binomial error of impurity, hQDisDTemp->Integral(1,hQDisD->GetNbinsX()) is the normalization
			fImpError[ivar]=sqrt(pow(fImpError[ivar],2)+pow(dFracE[ivar]*rarInt[ivar]->getVal()/dIntSumPdf,2));
			cout << " ****** " << endl; //XXX
			cout << ivar << " " << fImp[ivar] << " " << fImpError[ivar] << " " << dIntSumPdf << " " << hQDisD->Integral(1,hQDisD->GetNbinsX()) << endl; //XXX
			cout << " ****** " << endl; //XXX
		}
		
		for (int ivar=0; ivar<iNVar; ivar++)
		{
			histn = Form("Impurity/Impurity_r%d_q%d_xy%d", ir, Charge+ivar, ixy);
			hImp = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
			hImp->SetBinContent(iSelRig+1, fImp[ivar]);
			hImp->SetBinError(iSelRig+1, fImpError[ivar]);
			hImp->SetEntries(1);
		} //ich
		
		//------calculate Inefficiency
		cout << "****** calculating L1 charge Inefficiency ******" << endl;
		rrvQXY->setRange("Integral_Inefficiency", dCutU, 35);
		//update the way to select which template to use for L1Q cut efficiency calculation
		int iSelVar=run_charge-Charge;
		
		RooAbsReal *rarIntIne = rkpLQ[iSelVar]->createIntegral(*rrvQXY,RooFit::NormSet(*rrvQXY),RooFit::Range("Integral_Inefficiency"));
		int iSelSpan=ir;
		#ifdef FSTEMPLATE
		iSelSpan=(run_charge+ivar==14)?2:ir;
		#endif //FSTEMPLATE
		if (iSelSpan<1) iSelSpan=1; //using L1Inner template for Inner+MaxL1Q
		
		//--obtain the LQTemp histogram from projection of QvsRig
		int ilTemp=il;
		if (run_charge>14 && run_charge!=26) ilTemp=2;
		//use L2Q template for secondary signal
		if (run_charge%2==1) ilTemp=2;
		
		if (ilTemp==1) histn = Form("%s/q%d/%s/%.1fsigma/L%dQTemp/QvsRig/L%dQvsRig_r%d_q%d_xy%d_L%dQTemp%.1fsigma_Tk2ndCut", sDir.c_str(), run_charge, sSpan[iSelSpan].c_str(), fSelSigma, ilTemp, ilTemp, iSelSpan, run_charge, ixy, ilTemp, fSelSigma);
		else if (ilTemp==2) histn = Form("%s/q%d/%s/%.1fsigma/L%dQTemp/QvsRig_Cut/L%dQvsRig_Cut_r%d_q%d_xy%d_L%dQTemp%.1fsigma", sDir.c_str(), run_charge, sSpan[iSelSpan].c_str(), fSelSigma, ilTemp, ilTemp, iSelSpan, run_charge, ixy, ilTemp, fSelSigma);
		hQ2dDis = static_cast<TH2F*>(fTotalFile.Get(histn.c_str()));
		
		cout << iSelTemp << "  [" << dLRigTemp << "," << dURigTemp << "]" << endl;
		cout << hQ2dDis->GetXaxis()->FindBin(dLRigTemp) << " to " << hQ2dDis->GetXaxis()->FindBin(dURigTemp)-1 << endl;
		
		histn = Form("L%dQTemp_r%d_rig%d", ilTemp, iSelSpan, iSelRig);
		hQDisD = hQ2dDis->ProjectionY(histn.c_str(), hQ2dDis->GetXaxis()->FindBin(dLRigTemp), hQ2dDis->GetXaxis()->FindBin(dURigTemp)-1, "e");
		
		double dFracQ=1;
		if (iSelVar<iNVar-1) dFracQ=rrvn[iSelVar]->getVal();
		else for (int ivar0=0; ivar0<iNVar-1; ivar0++) dFracQ-=rrvn[ivar0]->getVal();
		
		cout << "********" << dFracQ*rarInt[iSelVar]->getVal()*hQDisD->Integral(1,hQDisD->GetNbinsX()) << " " << dFracQ*rarIntIne->getVal()*hQDisD->Integral(1,hQDisD->GetNbinsX()) << " " << rarIntIne->getVal()/rarInt[iSelVar]->getVal()*100. << "%" << endl;
		
		#ifndef SIMPLEPLOT
		//lLegend.DrawLatexNDC(.55, .90, Form("#color[%d]{Event within Q%d cut range: %.8f}", kBlack, run_charge, rrvn[0]->getVal()*rarInt[0]->getVal()));
		//lLegend.DrawLatexNDC(.55, .85, Form("#color[%d]{Event above Q%d cut range: %.8f}", kBlack, run_charge, rrvn[0]->getVal()*rarIntIne->getVal()));
		//lLegend.DrawLatexNDC(.55, .80, Form("#color[%d]{Q%d Inefficiency: %.8f%%}", kBlack, run_charge, rarIntIne->getVal()/rarInt[0]->getVal()*100.));
		#endif //SIMPLEPLOT
		
		//------store Inefficiency
		histn = Form("Inefficiency/Inefficiency_r%d_q%d_xy%d", ir, run_charge, ixy);
		hIne = static_cast<TH1F*>(fOutPut->Get(histn.c_str()));
		//calculate the error of inefficiency as inverse of the binomial erro
		double dEff=rarInt[iSelVar]->getVal()/(rarInt[iSelVar]->getVal()+rarIntIne->getVal());
		double dError=sqrt(dEff*(1-dEff)/ ((rarInt[iSelVar]->getVal()+rarIntIne->getVal())*hQDisD->Integral(1,hQDisD->GetNbinsX())) ); //hQDisD->Integral(1,hQDisD->GetNbinsX()) is the normalization of PDF
		dError=1./pow(dEff,2)*dError;
		hIne->SetBinContent(iSelRig+1, 1/dEff);
		hIne->SetBinError(iSelRig+1, dError);
		
		hIne->SetEntries(1);
		cout << "Ineff: " << 1./dEff << "+/-" << dError << endl;
		
		//#ifndef SIMPLEPLOT //keep pull for SIMPLEPLOT
		#ifndef NOPULL
		//----Draw Pull
		pPad2->cd();
		rpPullFrame = rrvQXY->frame(dDrawL, dDrawU);
		rpPullFrame->addPlotable(rhPull,"P");
		rpPullFrame->GetXaxis()->SetTitle("TRACKER L1 Charge");
		rpPullFrame->GetXaxis()->CenterTitle();
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
		//#endif //SIMPLEPLOT
		
		#ifdef SIMPLEPLOT
		histn = Form("%s/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d_simplePlot", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig, iNVar);
		#elif defined COMPAREPLOT
		histn = Form("%s/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d_compareplot%s", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig, iNVar, sPull.c_str());
		#else
		histn = Form("%s/L1QTempFit_r%d_q%d_xy%d_rig%d_NVar%d", sDrawL1QTempFit.c_str(), ir, run_charge, ixy, iSelRig, iNVar);
		#endif //SIMPLEPLOT
		
		#ifdef FSTEMPLATE
		histn = histn+"_FSTEMP";
		#endif //FSTEMPLATE
		cCanv->SaveAs(Form("%s.png", histn.c_str()));
		
		//delete cCanv;
		
		//------
		//--close files
		fQTemp.Close();
		fTotalFile.Close();
	
		//--store output files
		fOutPut->Write();
		fOutPut->Close();
	} //ir
	
	return 0;
} //end of KeysPdfRead
