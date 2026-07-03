//-----------------
//updated from: ChargeCalN8.C
//updated date: 2019.07.01
//-----------------
//Apply new method to gain charge cut:
//	- use InnerQ, L1Q and L9Q rough cut and also rms rough cut from all but charge distribution to get UTofQ and LTofQ sample 
//	  separately, require the Tof has Good path length (IsGoodPathL()) - GETTOFQ
//	- use TofQ charge cut from sample selected by GETTOFQ to get InnerQ sample (apply LTofQ cut for both L1Inner and Full Span).
//	  Also store the distribution for InnInnerQ - GETINNERQ
//	- Use L1Q rough, UTofQ, InnInnerQ and LTofQ cut to get L2Q sample, meanwhile apply UTofQ and InnerQ to get L1Q sample - GETL12Q
//All those steps are seperated
//-----------------
//2019.07.01
//	1) remove the use of Select_BZChargeRV2 & Select_BZChargeUV2 usage
//
//2019.07.30
//	1) update the GETROUGHALL so that L2QTEMP (itype==6) have same selection as GETL2Q (itype==4)
//
//2019.08.03
//	1) correctly apply the L1XY
//	2) for Full span, also apply the L9XY
//	3) for none reference charge (i.e. 8, 14 for example), only store the L1QTemp fit related distribution
//
//2019.08.04
//	1) for GetL1Q, exclude the L1 bad ladder to have better shape
//
//2019.08.08
//	1) for L1Q, store one good ladder (-811 for example) distribution in order to be compare with bad ladder
//	2) for GETL2Q, add also the L2XY & L2Q status
//
//2019.08.09
//	1) check the charge status pattern of bad ladder exclude, bad ladder and normal ladder of L1Q
//	2) set SumW2 for QvsRig_Cut histograms
//
//2019.08.11
//	1) for L2Q selection, apply TofQ cut on each Tof layers like the sample selected for InnerQ
//	2) check the X & Y side charge status for L1 bad ladder and compare with normal ladder & all but bad ladder excluded
//
//2019.08.14
//	1) store the distribution of L1 bad ladder with different gain-related Q status flagged (X side): bit 6, bit 7, bit 6 || bit 7, !(bit 6 && bit 7), also 
//	   check for normal ladder and all but exclude
//
//2019.08.22
//	1) use different bin for L1QTemplate fit - L1QTEMPSMALLBIN
//
//2019.09.03
//	1) also do store the L2QTEMP for FullSpan
//
//2019.09.04
//	1) store the InnerQ vs InnerQrms and InnInnerQ vs InnInneQrms
//	2) apply InnerQrms cut when cutting InnerQ
//	3) Apply on the InnInnerQrms for L2Q sample
//
//2019.09.11
//	1) check the template fit result on L1Q selected without L1Chisq<10 - L2QTMEPWOL1CHISQY
//
//2019.09.27
//	1) for L1QTemp, remove the UTofQ cut for L1 charge in order to fully consider the interaction between L1 -> L2 instead of just L1->UTof
//
//2019.10.02
//	1) for L1Q dis, store the one with UTofQ cut when L2QTEMP is defined
//
//2019.11.07
//	1) store the Charge template from L1Q by selecting downward good charge and selection on 2nd Track, for now simply use Select_Tk2nd, need to think about whether need to change later
//	2) change the type for L2QTEMP to 7 and set type for L1QTEMP as 6
//	3) for L1QTemp, store the L1Q distribution and the 2nd Q distribution before and after appling Select_Tk2nd
//	4) considering replace the histogram in each rigidity bin for LQTEMP into 2d histogram TODO
//
//2019.11.12
//	1) for L1Q QvsRig, store the L1QvsRig for X side and Y side, also store one normal ladder (-811 for example)
//	2) store the QvsRig_cut for L2QSam
//	3) store the L1 charge distribution after 2nd cut - CHECK2NDCUT
//
//2019.11.18
//	1) for another check for CHECK2NDCUT requiring only 1 track, i.e. ntrack==1
//	2) check the L1Q distribution for each ladder - L1QLADDERCHECK
//
//2019.11.25
//	1) update the cuts for 2nd to using Select_Tk2nd2() with different dRigCut
//	2) also store the L2Q distribution for each ladder - L2QLADDERCHECK
//
//2019.12.04
//	1) for the L1Q distribution to be fit by Template, apply exactly the same charge cut as for event count
//	2) for the L1Q distribution to be fit by Template, store 3 sets: without UTofQ cut, with event count UTofQ cut and with tight (+/-0.5) UTofQ cut
//
//2019.12.06
//	1) to reduce size, store only L1Inner and XY combine for L1QTEMP or L2QTEMP - REDUCESIZELQTEMP
//
//2019.12.10
//	1) add also the Ladder distribution for Tracker L9
//	2) store only L1Inner for L1 and L2 ladder check
//
//2020.01.21
//	1) store the L2QTemp for Q8-Q17, which will be used for L1->L2 survival probability background subtraction and others - L2QTEMPALL
//
//2020.02.29
//	1) For the L1Q distribution which going to be fitted by template, use exactly the same UTofQ and InnerQ cut for event count - L1QSAMTEMP
//
//2020.06.02
//	1) store also the charge distribition for Iron
//
//2020.06.05
//	1) add lower tofq cut for the charge template of heavy nuclei
//
//2020.06.06
//	1) set the same name for the tree variable of L1QTemp and L2QTemp
//
//2020.07.08
//	1) update the selection for charge template:
//		- removing bad tof path length
//		- store charge template for all rigidity, mainly for low abundance specise
//		- loose the LTofQ upper cut
//	2) store the L1Q distribution after applying Tk2nd cut
//
//2020.07.17
//	1) refine the charge selection for charge templates
//
//2020.09.21
//	1) stop storing the charge distribution for given rigidity bin and use the projection of 2D distribution instead. Put those distribution into flag QDISRIGBIN
//
//2020.11.21
//	1) for MC, store L1QTemp and L2QTemp, but not store the KeysPdf
//-----------------
//#include "L1QTemplateFit_binning.h"
#include "L1QTemplateFit_binning.h"

//--2D charge distribution
//#define QDIS2D
//#define QDIS2DSEL

#define GETROUGHALL //2019.04.07
//#define L1QSAMTEMP //2020.02.29

#define STOREALLQ //2023.02.07: store all distribution for all Q

//--charge sample
//#define GETTOFQ
//#define GETINNERQ
////#define INNERQEVENTSEL //2021.02.11
// #define GETL1Q
//#define GETL2Q
//#define GETL9Q
//#define GETQALL
//#define QVSRIG

//--LQ template
#define L1QTEMP //2019.11.07
#define L2QTEMP
// #define L2QTEMPALL //2020.01.21 //2023.02.21: use this binning, maybe rename the flag after
//#define FELQTEMP //2020.06.07
#if defined L1QTEMP || defined L2QTEMP
//#define GETL1Q
//2020.01.21
//#ifndef L2QTEMPALL
// #define GETL1Q //// 2023.02.28
//#endif //L2QTEMPALL
#define KEYSPDF
//2019.12.06
#define REDUCESIZELQTEMP
#define L1QSAMTEMP //2020.02.29
#define QDISRIGBIN //2020.09.21
//2023.05.08
#define QVSRIG
#endif //L1QTEMP || L2QTEMP

//2021.02.22
#ifdef GETL1Q
#define REDUCESIZELQTEMP
#define L1QSAMTEMP
#define ISOTOPEBGSUBQ //2022.06.06: use different flag to store always the unbiased l1q with bg subtraction
#endif //GETL1Q

int iOnlySel=1;


//#define L2QTMEPWOL1CHISQY //2019.09.11

//#define CHECK2NDCUT //2019.11.12

//#define L1QLADDERCHECK //2019.11.18
//#define L2QLADDERCHECK //2019.11.25
//#define L9QLADDERCHECK //2019.12.10

#ifdef KEYSPDF
TTree *tKeysPdf=NULL;
TTree *tTightL1Sam=NULL;
double L1QX=0., L1QY=0., L1QXY=0.; //2019.11.07
double L2QX=0., L2QY=0., L2QXY=0.;
#define SURSEL // 2023.04.16
#ifdef SURSEL
double UBL1QXY=0.;
double UBL1QXYSam = 0, L1QXYSam = 0;
#endif
#define NEWL1PDF //2024.06.16 teset
#define TRDPAR // 2025.04.11 
#ifdef TRDPAR
TTree *tTRDpara=NULL;
int TRD_nhitk = 0, TRD_onhitk = 0, TRD_amphitk = 0;
int TRD_ntrdcl =0;
#endif //TRDPAR
#endif //KEYSPDF

#define REDUCESIZE //2021.01.26: not used
//2019.12.06
#ifdef REDUCESIZELQTEMP
const int ixyLQTemp=2; //store start from 2, i.e. only XY combined
#else
const int ixyLQTemp=0; //store start from 0, i.e. X, Y and XY combined
#endif //REDUCESIZELQTEMP

#define INNINNERQ
#ifdef INNINNERQ
//#define TRUNQCOM
const int NQDis_store=4;
#else
const int NQDis_store=3;
#endif //INNINNERQ
//#define DIFFSIGMA

#define BADL1LAD
//#define BADL1LADCHECK //2019.08.14

//2019.05.12
//#define ECALMIPS
//2019.05.26
//#define ECALMIPS2

const int iQDL=0, iQDU=35;
const int i1DBin=100*(iQDU-iQDL);
//const int i1DBin=50*(iQDU-iQDL);
const int i2DBin=10*(iQDU-iQDL);

#ifdef DIFFSIGMA
//const int nSigma=4;
const int nSigma=3;
#else
const int nSigma=1;
#endif //DIFFSIGMA
const double dSigma[4] = {2, 1, 1.5, 2.5};
//const double dSigma[4] = {1, 1.5, 2, 2.5};

/*const int MType=7;
//const string sType[MType] = {"UTofQSam", "LTofQSam", "InnerQSam", "L1QSam", "L2QSam", "L9QSam", "LQExam"};
const string sType[MType] = {"UTofQSam", "LTofQSam", "InnerQSam", "L1QSam", "L2QSam", "L9QSam", "L2QTemp"};*/
//2019.11.07
const int MType=8;
const string sType[MType] = {"UTofQSam", "LTofQSam", "InnerQSam", "L1QSam", "L2QSam", "L9QSam", "L1QTemp", "L2QTemp"};

const int MSel=7;
//const int MSel=10;
//const double PI = 3.14159265357;
//const double PI = TMath::Pi(); //2019.03.20 //2021.09.09: defined in include/database.h

//#define L1QTEMPSMALLBIN //2019.08.22
#ifdef L1QTEMPSMALLBIN //2019.08.22
const int iNBL1TF=L1QTF_NbinSmall;
double *dBL1TF=L1QTF_BinsSmall;
#elif defined L2QTEMPALL//2020.01.21
//const int iNBL1TF=L1QTF_NbinAllQ;
//double *dBL1TF=L1QTF_BinsAllQ;
//2020.10.24
const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;
//2020.06.07
#elif defined FELQTEMP
/*const int iNBL1TF=L1QTF_NbinFe;
double *dBL1TF=L1QTF_BinsFe;*/
/*//2020.06.23
const int iNBL1TF=L1QTF_NbinFe2;
double *dBL1TF=L1QTF_BinsFe2;*/
//2020.07.11
const int iNBL1TF=L1QTF_NbinFe3;
double *dBL1TF=L1QTF_BinsFe3;
#else
//const int iNBL1TF=L1QTF_Nbin;
//double *dBL1TF=L1QTF_Bins;
//const int iNBL1TF=L1QTF_Nbin2; //2019.10.02
//double *dBL1TF=L1QTF_Bins2;
//--2023.02.07: now put the same as L2QTEMPALL, will update after
const int iNBL1TF=L1QTF_NbinAllQ2;
double *dBL1TF=L1QTF_BinsAllQ2;
#endif //L1QTEMPSMALLBIN

//--2023.04.22
const string sVarL1QSel[15] = {"ntrack", "nbetah", "ntrdtrack", "tof_hsumhu", "nqud", "ntrdseg", "tof_nclhl", "betah2q", "tofbq0", "tofbq1", "ntrdrawh", "trd_qk4", "trd_qk3", "ntrrawcl", "nrichh"};
const string sVarL1QSelFull[15] = {"no. TrTrack", "no. TOF Track", "no. TrdTrack", "Tof Used Hit", "no. U+L Tof Good Path Length", "no. Trd Segment", "no. UTOF fired", "betah2q", "tofbq0", "tofbq1", "no. TrdRawHit", "TRD Q - delta ray", "TRD Q - dE/dX", "no. TrRawCluster", "nRichHit"};
//nqud: no. good tof pathlength in upper and lower tof
//tofbq0
//tofbq1
//trd_qk4: TrdQ only delta ray
//trd_qk3: TrdQ only dE/dX

#ifdef AMSSOFT
bool BookChargeCal(HistoMan &hman1, bool bIsreal, TFile &outfile, int QVer, bool dir=false, int mch=0)
#else
//bool BookChargeCal(TObjArray &hman1, bool bIsreal, TFile &outfile, int QVer, bool dir=false, int mch=0)
bool BookChargeCal(TObjArray &hman1, SelEvent &ev, TFile &outfile, int QVer, bool dir=false) //2023.04.05
#endif //AMSSOFT
{
	TH1F *hQDis = NULL;
	TH2F *hQ2dDis=NULL;
	TH2S *hQ2dDisS=NULL;
	TH2I *hQ2dDisI=NULL;
	string sDir = Form("Charge_Calibration%d", QVer);
	
	#ifdef PRESELECTED
	if (dir)
	{
		outfile.mkdir(Form("%s/PreSelected", sDir.c_str()));
	}
	#endif //PRESELECTED
	//for (int ir=0; ir<3; ir++) //non-charge selection
	//2019.12.06
	#ifdef REDUCESIZELQTEMP
	//for (int ir=1; ir<2; ir++)
	//2021.03.23
	int iLowSpan=0;
	#if not defined GETL1Q || defined L1QTEMP || defined L2QTEMP
	iLowSpan=1;
	#endif //!GETL1Q || L1QTEMP || L2QTEMP
	for (int ir=iLowSpan; ir<2; ir++)
	#else
	for (int ir=0; ir<3; ir++)
	#endif //REDUCESIZELQTEMP
	{
		for (int ialgo=0; ialgo<NRig; ialgo++)
		{
			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && ialgo==1) continue; //skip ialgo=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			string sRig=sRigName[ialgo];
			
			#ifdef PRESELECTED //2023.05.08
			if (dir)
			{
				//outfile.mkdir(Form("%s/PreSelected/%s", sDir.c_str(), cSpan[ir]));
				//outfile.cd(Form("%s/PreSelected/%s", sDir.c_str(), cSpan[ir]));
				histn = Form("%s/PreSelected/%s", sDir.c_str(), sSpan[ir].c_str());
				histn += sRig;
				outfile.mkdir(histn.c_str());
				outfile.cd(histn.c_str());
			}
			//sub-detector charge distribution
			for (int iqdis=0; iqdis<NQDis_store; iqdis++)
			{
				//all but charge
				histn = Form("%s_r%d", cQDis[iqdis], ir);
				histn += sRig; //2023.04.05
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
				hQDis = (TH1F*)GetHist(hman1, histn.c_str());
				hQDis->GetXaxis()->SetTitle(Form("%s", cQDis[iqdis]));
			} //iqdis
			
			#ifdef TRUNQCOM
			//--InnerQ vs InnInnerQ
			histn = Form("InnerQvsInnInnerQ_r%d", ir);
			histn += sRig; //2023.04.05
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle(Form("InnInnerQ"));
			hQ2dDis->GetYaxis()->SetTitle(Form("InnerQ"));
			
			//--InnerQ vs InnerQ from CalTrunCharge
			histn = Form("InnerQvsInnerQCal_r%d", ir);
			histn += sRig; //2023.04.05
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle(Form("InnerQCal"));
			hQ2dDis->GetYaxis()->SetTitle(Form("InnerQ"));
			#endif //TRUNQCOM
			
			//all tof layer 2019.04.27
			for (int il=0; il<4; il++)
			{
				histn = Form("TofL%dQ_r%d", il+1, ir);
				histn += sRig; //2023.04.05
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
				hQDis = (TH1F*)GetHist(hman1, histn.c_str());
				hQDis->GetXaxis()->SetTitle(Form("TofL%dQ", il+1));
			} //il
			
			//all layers distribution
			for (int il=0; il<9; il++)
			{
				if (il>1 && il<8) continue;
				histn = Form("L%dQ_r%d", il+1, ir);
				histn += sRig; //2023.04.05
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
				hQDis = (TH1F*)GetHist(hman1, histn.c_str());
				hQDis->GetXaxis()->SetTitle(Form("L%dQ", il+1));
				
				#ifdef BADL1LAD
				if (il!=0) continue;
				histn = Form("L%dQ_r%d_badl1lad", il+1, ir);
				histn += sRig; //2023.04.05
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
				hQDis = (TH1F*)GetHist(hman1, histn.c_str());
				hQDis->GetXaxis()->SetTitle(Form("L%dQ", il+1));
				#endif //BADL1LAD
			} //il
			
			//qrms
			histn = Form("InnerQrms_r%d", ir);
			histn += sRig; //2023.04.05
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 500, 0, 5));
			hQDis = (TH1F*)GetHist(hman1, histn.c_str());
			hQDis->GetXaxis()->SetTitle("InnerQrms");
			
			histn = Form("InnInnerQrms_r%d", ir);
			histn += sRig; //2023.04.05
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 500, 0, 5));
			hQDis = (TH1F*)GetHist(hman1, histn.c_str());
			hQDis->GetXaxis()->SetTitle("InnInnerQrms");
			
			#ifdef QVSRIG
			if (dir)
			{
				//outfile.mkdir(Form("%s/PreSelected/%s/QvsRig", sDir.c_str(), cSpan[ir]));
				//outfile.cd(Form("%s/PreSelected/%s/QvsRig", sDir.c_str(), cSpan[ir]));
				histn = Form("%s/PreSelected/%s", sDir.c_str(), sSpan[ir].c_str());
				histn += sRig;
				histn += "/QvsRig";
				outfile.mkdir(histn.c_str());
			}
			
			iNbin = getNbin(viQSel[0]);
			pBins = getBins(viQSel[0]);
			
			//sub-detector charge distribution
			for (int iqdis=0; iqdis<NQDis_store; iqdis++)
			{
				//all but charge
				histn = Form("%svsRig_r%d", cQDis[iqdis], ir);
				histn += sRig;
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
				hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
				hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
				hQ2dDis->GetYaxis()->SetTitle(Form("%s", cQDis[iqdis]));
				hQ2dDis->Sumw2();
			} //iqdis
			
			//all tof layer 2019.04.27
			for (int il=0; il<4; il++)
			{
				histn = Form("TofL%dQvsRig_r%d", il+1, ir);
				histn += sRig;
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
				hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
				hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
				hQ2dDis->GetXaxis()->SetTitle(Form("TofL%dQ", il+1));
				hQ2dDis->Sumw2();
			} //il
			
			//all layers distribution
			for (int il=0; il<9; il++)
			{
				if (il>1 && il<8) continue;
				histn = Form("L%dQvsRig_r%d", il+1, ir);
				histn += sRig;
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
				hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
				hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
				hQ2dDis->GetYaxis()->SetTitle(Form("L%dQ", il+1));
				hQ2dDis->Sumw2();
				
				#ifdef BADL1LAD
				if (il!=0) continue;
				histn = Form("L%dQvsRig_r%d_badl1lad", il+1, ir);
				histn += sRig;
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
				hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
				hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
				hQ2dDis->GetYaxis()->SetTitle(Form("L%dQ", il+1));
				hQ2dDis->Sumw2();
				#endif //BADL1LAD
			} //il
			
			//qrms
			histn = Form("InnerQrmsvsRig_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 500, 0, 5));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
			hQ2dDis->GetYaxis()->SetTitle("InnerQrms");
			hQ2dDis->Sumw2();
			#endif //QVSRIG
			#endif //PRESELECTED
			
			//if (ir==0) continue;
			//2021.03.23: store Inner pattern as well
			#if not defined GETL1Q || defined L1QTEMP || defined L2QTEMP
			if (ir<1) continue;
			#endif //!GETL1Q || L1QTEMP || L2QTEMP
			
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				if (!ev.isreal && *it!=ev.mch) continue;
				#if not defined L1QTEMP && not defined L2QTEMP && not defined GETINNERQ && not defined GETL1Q
				if (*it!=8 && *it!=10 && *it!=14 && *it!=26) continue;
				#endif //L1QTEMP || L2QTEMP
				for (int isigma=0; isigma<nSigma; isigma++)
				{
					if (dir)
					{
						//outfile.mkdir(Form("%s/q%d/%s/%.1fsigma", sDir.c_str(), *it, cSpan[ir], dSigma[isigma]));
						histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
						histn += sRig;
						histn += Form("/%.1fsigma", dSigma[isigma]);
						outfile.mkdir(histn.c_str());
					}
					#if defined GETTOFQ || defined GETINNERQ || defined GETL1Q || defined GETL2Q || defined GETL9Q || defined L1QTEMP || defined L2QTEMP
					for (int itype=0; itype<MType; itype++)
					{
						#ifndef GETTOFQ
						if (itype==0 || itype==1) continue;
						#endif //GETTOFQ
						#ifndef GETINNERQ
						if (itype==2) continue;
						#endif //GETINNERQ
						#ifndef GETL1Q
						if (itype==3) continue;
						#endif //GETL1Q
						#ifndef GETL2Q
						if (itype==4) continue;
						#endif //GETL2Q
						#ifndef GETL9Q
						if (itype==5) continue;
						#endif //GETL9Q
						#ifndef L1QTEMP
						if (itype==6) continue;
						#endif //L1QTEMP
						#ifndef L2QTEMP
						if (itype==7) continue;
						#endif //L2QTEMP
						if (ir<2 && (itype==1 || itype==5)) continue; //only do FS for LTofQSam and L9QSam
						
						#if defined L1QTEMP || defined L2QTEMP || defined GETINNERQ || defined GETL1Q
						//--force to store all distribution for all charge
						#ifndef STOREALLQ
						if (*it!=8 && *it!=10 && *it!=14 && *it!=26 && ((itype!=2 && itype!=3 && itype!=6 && itype!=7))) continue;
						#endif //STOREALLQ
						
						#else
						if (*it!=8 && *it!=10 && *it!=14 && *it!=26) continue;
						#endif //L1QTEMP || L2QTEMP
						
						if (dir)
						{
							//histn = Form("%s/q%d/%s/%.1fsigma/%s", sDir.c_str(), *it, cSpan[ir], dSigma[isigma], sType[itype].c_str());
							histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
							histn += sRig;
							histn += Form("/%.1fsigma/%s", dSigma[isigma], sType[itype].c_str());
							outfile.mkdir(histn.c_str());
							outfile.cd(histn.c_str());
						}
						
						//------InnerQ, InnInnerQ, TofQ
						for (int iqdis=0; iqdis<NQDis_store; iqdis++)
						{
							histn = Form("%s_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
							hQDis = (TH1F*)GetHist(hman1, histn.c_str());
							hQDis->GetXaxis()->SetTitle(Form("%s", cQDis[iqdis]));
							hQDis->GetYaxis()->SetTitle("Nevent");
							
							if (iqdis==0)
							{
								#ifdef TRUNQCOM
								//--InnerQ vs InnInnerQ
								histn = Form("InnerQvsInnInnerQ_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle(Form("InnInnerQ"));
								hQ2dDis->GetYaxis()->SetTitle(Form("InnerQ"));

								//--InnerQ vs InnerQ from CalTrunCharge
								histn = Form("InnerQvsInnerQCal_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle(Form("InnerQCal"));
								hQ2dDis->GetYaxis()->SetTitle(Form("InnerQ"));
								#endif //TRUNQCOM
								
								//--InnerQrms
								histn = Form("InnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 500, 0, 5));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle("InnerQrms");
								hQDis->GetYaxis()->SetTitle("Nevent");
								
								//2019.09.04: plot the InnerQ vs InnerQrms
								histn = Form("InnerQvsInnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2I(histn.c_str(), histn.c_str(), 500, 0, 5, 350, 0, 35));
								hQ2dDisI = (TH2I*)GetHist(hman1, histn.c_str());
								hQ2dDisI->GetXaxis()->SetTitle("InnerQrms");
								hQ2dDisI->GetYaxis()->SetTitle("InnerQ");
							}
							else if (iqdis==3)
							{
								//--InnerQrms
								histn = Form("InnInnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 500, 0, 5));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle("InnInnerQrms");
								hQDis->GetYaxis()->SetTitle("Nevent");
								
								//2019.09.04: plot the InnerQ vs InnerQrms
								histn = Form("InnInnerQvsInnInnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2I(histn.c_str(), histn.c_str(), 500, 0, 5, 350, 0, 35));
								hQ2dDisI = (TH2I*)GetHist(hman1, histn.c_str());
								hQ2dDisI->GetXaxis()->SetTitle("InnInnerQrms");
								hQ2dDisI->GetYaxis()->SetTitle("InnInnerQ");
							}
						} //iqdis
						
						//------all tof layer 2019.04.27
						for (int il=0; il<4; il++)
						{
							histn = Form("TofL%dQ_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
							hQDis = (TH1F*)GetHist(hman1, histn.c_str());
							hQDis->GetXaxis()->SetTitle(Form("TofL%dQ", il+1));
						} //il
						
						//------tracker layer charge
						for (int il=0; il<9; il++)
						{
							if ((itype!=2 && itype!=4) && (il>1 && il<8)) continue;
							histn = Form("L%dQ_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
							hQDis = (TH1F*)GetHist(hman1, histn.c_str());
							hQDis->GetXaxis()->SetTitle(Form("L%dQ", il+1));
							
							if (itype==3 && il==0)
							{
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
									hQDis = (TH1F*)GetHist(hman1, histn.c_str());
									hQDis->GetXaxis()->SetTitle(Form("L%dQ", il+1));
									hQDis->GetYaxis()->SetTitle("Nevent");
								} //ixy
							}
						
							#ifdef BADL1LAD
							if (il==0)
							{
								histn = Form("L%dQ_r%d_q%d_badl1lad_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle(Form("L%dQ", il+1));
								
								//#ifdef BADL1LADCHECK
								histn = Form("L%dQ_r%d_q%d_norml1lad_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle(Form("L%dQ", il+1));
								//#endif //BADL1LADCHECK
							}
							#endif //BADL1LAD
								
							//store unbiased L1Q as well
							if (itype==3 && il==0)
							{
								histn = Form("unbiasedL%dQ_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle(Form("unbiased L%dQ", il+1));
								
								#ifdef ISOTOPEBGSUBQ
								histn = Form("unbiasedL%dQ_r%d_q%d_%s%.1fsigma_BGSub", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle(Form("unbiased L%dQ", il+1));
								#endif //ISOTOPEBGSUB
							}
						} //il
						
						//#ifdef L2QTEMP
						#if defined L1QTEMP || defined L2QTEMP
						//L3-L8 Q distribution before and after cut on it, L2QTEMP (itype==7) only
						if (itype==7)
						{
							for (int ixy=ixyLQTemp; ixy<3; ixy++)
							{
								if (ixy==0) histn1="X";
								else if (ixy==1) histn1="Y";
								else if (ixy==2) histn1="";
								histn = Form("L3L8Q_r%d_q%d_xy%d_%s%.1fsigma0", ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle(Form("L3L8 %sQ", histn1.c_str()));
								hQDis->GetYaxis()->SetTitle("Nevent");

								histn = Form("L3L8Q_r%d_q%d_xy%d_%s%.1fsigma1", ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
								hQDis = (TH1F*)GetHist(hman1, histn.c_str());
								hQDis->GetXaxis()->SetTitle(Form("L3L8 %sQ", histn1.c_str()));
								hQDis->GetYaxis()->SetTitle("Nevent");
								
							} //ixy
							
							iNbin = getNbin(viQSel[0]);
							pBins = getBins(viQSel[0]);
							
							histn = Form("rig_r%d_q%d_%s%.1fsigma_sel%d", ir, *it, sType[itype].c_str(), dSigma[isigma], iOnlySel);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							hQDis = (TH1F*)GetHist(hman1, histn.c_str());
							hQDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
							
							hQDis->SetLineColor(kRed);
							hQDis->SetFillColor(kRed);
							hQDis->SetFillStyle(3002);
						}
						
						#ifdef QDISRIGBIN
						for (int irig=0; irig<iNBL1TF; irig++)
						{
							if (itype!=3 && itype!=6 && itype!=7) break; //store distributions in L1QSam, L1QTemp and L2QTemp
							if (dir)
							{
								//histn = Form("%s/q%d/%s/%.1fsigma/%s/rig%d", sDir.c_str(), *it, cSpan[ir], dSigma[isigma], sType[itype].c_str(), irig);
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								// histn += Form("/%.1fsigma/%s/rig%d", dSigma[isigma], sType[itype].c_str(), irig);
								histn += Form("/%s/rig%d", sType[itype].c_str(), irig);
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							for (int il=1-1; il<9-1; il++)
							{
								if (((itype==3 || itype==6)&& il!=0) || (itype==7 && il!=1)) continue; //only l1 for L1QSam (itype=3) & L1QTemp (itype=6) and l2 for L2QTemp (itype=7)
								if (dir)
								{
									//histn = Form("%s/q%d/%s/%.1fsigma/%s/rig%d/l%d", sDir.c_str(), *it, cSpan[ir], dSigma[isigma], sType[itype].c_str(), irig, il+1);
									histn = Form("%s/q%d/%s", sDir.c_str(), *it, sSpan[ir].c_str());
									histn += sRig;
									// histn += Form("/%.1fsigma/%s/rig%d/l%d", dSigma[isigma], sType[itype].c_str(), irig, il+1);
									histn += Form("/%s/rig%d/l%d", sType[itype].c_str(), irig, il+1);
									outfile.mkdir(histn.c_str());
									outfile.cd(histn.c_str());
								}
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									if (ixy==0) histn1="X";
									else if (ixy==1) histn1="Y";
									else if (ixy==2) histn1="";
									for (int isel=0; isel<MSel; isel++)
									{
										if (isel!=iOnlySel) continue;
										//--X, Y distribution
										histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
										histn += sRig;
										hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
										hQDis = (TH1F*)GetHist(hman1, histn.c_str());
										hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
										
										//store also the L1Q with uTOFQ cut
										if (il==0 && itype==3) //only for L1QSam
										{
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_uTOFQ", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
											hQDis = (TH1F*)GetHist(hman1, histn.c_str());
											hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											
											//store the L1Q with tight uTOFQ cut
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
											hQDis = (TH1F*)GetHist(hman1, histn.c_str());
											hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											
											if (ixy==2)
											{
												histn = Form("UTofQ_r%d_q%d_%s%.1fsigma_sel%d_rig%d", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, irig);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("UTofQ"));
												
												histn = Form("UTofQ_r%d_q%d_%s%.1fsigma_sel%d_rig%d_cutted", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, irig);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("UTofQ"));
												
												//store the uTOFQ after tight uTOFQ cut
												histn = Form("UTofQ_r%d_q%d_%s%.1fsigma_sel%d_rig%d_cutted_tight", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, irig);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("UTofQ"));
											}
										}
										
										#ifdef BADL1LADCHECK
										if (il==0)
										{
											//------L1Q all but bad ladder
											//charge stauts distribution
											if (ixy!=2)
											{
												histn = Form("L%dQstatus_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 13, -1.5, 11.5)); //-1: no bit flipped
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ status", il+1, histn1.c_str()));
											}
										
											//L1Q with different gain-related Q status
											for (int ipat=0; ipat<4; ipat++)
											{
												if (ixy==1) continue; //only check XQ & XYQ
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_pat%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig, ipat);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											} //ipat
											
											//------bad l1 ladder +811
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_badl1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
											hQDis = (TH1F*)GetHist(hman1, histn.c_str());
											hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											
											//charge stauts distribution
											if (ixy!=2)
											{
												histn = Form("L%dQstatus_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_badl1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 13, -1.5, 11.5)); //-1: no bit flipped
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ status", il+1, histn1.c_str()));
											}
											
											//L1Q with different gain-related Q status
											for (int ipat=0; ipat<4; ipat++)
											{
												if (ixy==1) continue; //only check XQ & XYQ
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_pat%d_badl1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig, ipat);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											} //ipat
											
											//------normal l1 ladder, -811
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_norml1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
											hQDis = (TH1F*)GetHist(hman1, histn.c_str());
											hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											
											//charge stauts distribution
											if (ixy!=2)
											{
												histn = Form("L%dQstatus_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_norml1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 13, -1.5, 11.5)); //-1: no bit flipped
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ status", il+1, histn1.c_str()));
											}
											
											//L1Q with different gain-related Q status
											for (int ipat=0; ipat<4; ipat++)
											{
												if (ixy==1) continue; //only check XQ & XYQ
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_pat%d_norml1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig, ipat);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
											} //ipat
										}
										#endif //BADL1LADCHECK
										
										//rigidity distribution
										if (itype==7 && il==1 && ixy==2)
										{
											histn = Form("rig_r%d_q%d_%s%.1fsigma_sel%d_rig%d", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, irig);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
											hQDis = (TH1F*)GetHist(hman1, histn.c_str());
											hQDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
											
											hQDis->SetLineColor(kBlue);
											hQDis->SetFillColor(kBlue);
											hQDis->SetFillStyle(3001);
										}
										
										//L1Q and 2nd Q before and after Select_Tk2nd for L1QTemp
										if (itype==6)
										{
											for (int itk2nd=0; itk2nd<2; itk2nd++)
											{
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_Tk2nd%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig, itk2nd);
												histn += sRig;
												hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
												hQDis = (TH1F*)GetHist(hman1, histn.c_str());
												hQDis->GetXaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
												
												if (ixy==2)
												{
													histn = Form("betah2q_r%d_q%d_%s%.1fsigma_rig%d_Tk2nd%d", ir, *it, sType[itype].c_str(), dSigma[isigma], irig, itk2nd);
													histn += sRig;
													hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
													hQDis = (TH1F*)GetHist(hman1, histn.c_str());
													hQDis->GetXaxis()->SetTitle(Form("betah2q"));

													#ifdef SURSEL
													if(itk2nd == 0 && il == 0){
														histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_Tk2nd%d_UnbiasedL1Q", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, irig, itk2nd);
														histn += sRig;
														hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
														hQDis = (TH1F*)GetHist(hman1, histn.c_str());
														hQDis->GetXaxis()->SetTitle(Form("UnbiasedL%d%sQ", il+1, histn1.c_str()));
													}
													#endif // SURSEL
												}
											} //itk2nd
										}
									} //isel
								} //ixy
							} //il
							
						} //irig
						#endif //QDISRIGBIN
						
						#endif //L1QTEMP || L2QTEMP
						
						//--Q vs Rig with tighter cut, only store for L2Q
						#ifdef QVSRIG
						if (itype==4 || itype==7)
						{
							if (dir)
							{
								//histn = Form("%s/q%d/%s/%.1fsigma/%s/QvsRig_Cut", sDir.c_str(), *it, cSpan[ir], dSigma[isigma], sType[itype].c_str());
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								histn += Form("/%.1fsigma/%s/QvsRig_Cut", dSigma[isigma], sType[itype].c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
						
							iNbin = getNbin(viQSel[0]);
							pBins = getBins(viQSel[0]);
						
							//------InnerQ, InnInnerQ, TofQ
							for (int iqdis=0; iqdis<NQDis_store; iqdis++)
							{
								histn = Form("%svsRig_Cut_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle(Form("%s", cQDis[iqdis]));
								hQ2dDis->Sumw2();
								
								if (iqdis==0)
								{
									//--InnerQrms
									histn = Form("InnerQrmsvsRig_Cut_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle("InnerQrms");
									hQ2dDis->Sumw2();
								}
							} //iqdis
						
							//------tracker layer charge
							for (int il=0; il<9; il++)
							{
								if (il>1 && il<8) continue;
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									histn = Form("L%dQvsRig_Cut_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("L%dQ", il+1));
									hQ2dDis->Sumw2();
								
									//--1d L2Q distirbuiton, used to compare the template
									histn = Form("L%dQ_Cut_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
									GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("L%dQ", il+1));
									
								} //ixy
							} //il
							
							#ifdef L2QLADDERCHECK
							if (itype==4 && ir==1)
							{
								if (dir)
								{
									histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
									histn += sRig;
									histn += Form("/%.1fsigma/%s/QvsRig_Cut/L2QLadderCheck", dSigma[isigma], sType[itype].c_str());
									outfile.mkdir(histn.c_str());
									outfile.cd(histn.c_str());
								}
								for (int iside=0; iside<2; iside++)
								{
									for (int ilad=1; ilad<16; ilad++)
									{
										if (dir)
										{
											histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
											histn += sRig;
											histn += Form("/%.1fsigma/%s/QvsRig_Cut/L2QLadderCheck/%s%d", dSigma[isigma], sType[itype].c_str(), ((iside==0)?"+":"-"), ilad+100);
											outfile.mkdir(histn.c_str());
											outfile.cd(histn.c_str());
										}
									
										for (int ixy=ixyLQTemp; ixy<3; ixy++)
										{
											histn = Form("L%dQvsRig_Cut_r%d_q%d_xy%d_lad%s%d_%s%.1fsigma", 2, ir, *it, ixy, ((iside==0)?"+":"-"), ilad+100, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
											hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
											hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
											hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", 1, histn1.c_str()));
											hQ2dDis->Sumw2();
										} //ixy
									} //ilad
								} //iside
							}
							#endif //L2QLADDERCHECK
						}
						#endif //QVSRIG
						
						#ifdef QVSRIG
						if (dir)
						{
							histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
							histn += sRig;
							histn += Form("/%.1fsigma/%s/QvsRig", dSigma[isigma], sType[itype].c_str());
							outfile.mkdir(histn.c_str());
							outfile.cd(histn.c_str());
						}
						
						iNbin = getNbin(viQSel[0]);
						pBins = getBins(viQSel[0]);
						
						//------InnerQ, InnInnerQ, TofQ
						for (int iqdis=0; iqdis<NQDis_store; iqdis++)
						{
							histn = Form("%svsRig_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
							hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
							hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
							hQ2dDis->GetYaxis()->SetTitle(Form("%s", cQDis[iqdis]));
							hQ2dDis->Sumw2();
							
							if (iqdis==0)
							{
								//--InnerQrms
								histn = Form("InnerQrmsvsRig_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle("InnerQrms");
								hQ2dDis->Sumw2();
							}
						} //iqdis
						
						//------all tof layer
						for (int il=0; il<4; il++)
						{
							histn = Form("TofL%dQvsRig_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
							hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
							hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
							hQDis->GetXaxis()->SetTitle(Form("TofL%dQ", il+1));
							hQ2dDis->Sumw2();
						} //il
						
						//------tracker layer charge
						for (int il=0; il<9; il++)
						{
							if (il>1 && il<8) continue;
							for (int ixy=ixyLQTemp; ixy<3; ixy++)
							{
								if (ixy==0) histn1="X";
								else if (ixy==1) histn1="Y";
								else if (ixy==2) histn1="";
								
								histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
								hQ2dDis->Sumw2();
								
								//store the distribution after additional event cut
								for (int ievcut=1; ievcut<3; ievcut++)
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_EvCut%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], ievcut);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
									hQ2dDis->Sumw2();
								} //ievcut
								
								#ifdef BADL1LAD
								if (il==0 && itype==3) //only store for L1QSam
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_badl1lad_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
									hQ2dDis->Sumw2();
								
									//normal l1 ladder
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_norml1lad_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
									hQ2dDis->Sumw2();
								}
								#endif //BADL1LAD
								
								//L1QvsRig after Tk2nd cut
								if (itype==6 && il==0 && ixy==2)
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_Tk2ndCut", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
								}
								
								//L1QVsRig with tight uTOFQ cut
								if (itype==3 && il==0 && ixy==2)
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", il+1, histn1.c_str()));
								}
								
								//store unbiased L1Q as well
								if (itype==3 && il==0)
								{
									histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("unbiasedL%d%sQ", il+1, histn1.c_str()));
									hQ2dDis->Sumw2();
									
									if (ixy==2)
									{
										histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
										hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
										hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
										hQ2dDis->GetYaxis()->SetTitle(Form("unbiasedL%d%sQ", il+1, histn1.c_str()));
									}
									
									#ifdef ISOTOPEBGSUBQ
									histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_BGSub", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("unbiasedL%d%sQ", il+1, histn1.c_str()));
									hQ2dDis->Sumw2();
									
									if (ixy==2)
									{
										histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_BGSub_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
										hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
										hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
										hQ2dDis->GetYaxis()->SetTitle(Form("unbiasedL%d%sQ", il+1, histn1.c_str()));
									}
									#endif //ISOTOPEBGSUB
								}
							} //ixy
						} //il
						#endif //QVSRIG
						
						#ifdef CHECK2NDCUT
						if (itype==3)
						{
							//------2nd variable as function of rigidity in different L1Q range
							if (dir)
							{
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								histn = Form("/%.1fsigma/%s/QvsRig/L1Q_2nd_Check2", dSigma[isigma], sType[itype].c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							
							//isam=0: no 2nd cut
							//isam=1: ntrack=1
							//isam=2: ntrack>1 (!isam1) and no well recon Tk2nd
							//isam=3: have well recon Tk2nd (!isam2) and betah2r/tof_betah<=0
							//isam=4: have well recon Tk2nd and betah2r/tof_betah>0 (!isam3)
							for (int isam=0; isam<5; isam++)
							{
								if (ir<1) break;
								
								if (dir)
								{
									histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
									histn += sRig;
									histn += Form("/%.1fsigma/%s/QvsRig/L1Q_2nd_Check2/sam%d", dSigma[isigma], sType[itype].c_str(), isam);
									outfile.mkdir(histn.c_str());
									outfile.cd(histn.c_str());
								}
								
								//------L1Q
								histn = Form("L1QvsRig_r%d_q%d_%s_sam%d", ir, *it, sType[itype].c_str(), isam);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
								hQ2dDis->GetYaxis()->SetTitle("L1Q");
								hQ2dDis->Sumw2();
								
								////iqrange=0: <Z-0.5, iqrange=1: Z+/-0.5, iqrange=2: >=Z+0.5, iqrange=3: >=Z+2+0.5
								//for (int iqrange=0; iqrange<4; iqrange++)
								//iqrange=0: <Z+0.5, iqrange=1: >=Z+0.5, iqrange=2: >=Z+2-0.5
								for (int iqrange=0; iqrange<3; iqrange++)
								{
									//------betah2r
									histn = Form("+logbetah2rvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 80, -4, 4));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
									hQ2dDis->GetYaxis()->SetTitle("log(betah2r) [GV]");
									hQ2dDis->Sumw2();
									
									histn = Form("-logbetah2rvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 80, -4, 4));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
									hQ2dDis->GetYaxis()->SetTitle("log(-betah2r) [GV]");
									hQ2dDis->Sumw2();
									
									//------betah2r/rigidity
									histn = Form("betah2r_over_RigvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 800, -4, 4));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
									hQ2dDis->GetYaxis()->SetTitle("betah2r/Rig");
									hQ2dDis->Sumw2();
									
									//------betah2q
									histn = Form("betah2qvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
									hQ2dDis->GetYaxis()->SetTitle("betah2q");
									hQ2dDis->Sumw2();
									
									//------tof_oq
									for (int itofl=0; itofl<2; itofl++)
									{
										for (int ioq=0; ioq<2; ioq++)
										{
											histn = Form("tof_oq_%d_%dvsRig_r%d_q%d_%s_sam%d_qrange%d", itofl, ioq, ir, *it, sType[itype].c_str(), isam, iqrange);
											histn += sRig;
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
											hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
											hQ2dDis->GetXaxis()->SetTitle(Form("Rigidity [GV]"));
											hQ2dDis->GetYaxis()->SetTitle(Form("tof_oq_%d_%d", itofl, ioq));
											hQ2dDis->Sumw2();
										} //ioq
									} //itofl
								} //iqrange
							} //isam
						}
						#endif //CHECK2NDCUT
						
						#ifdef L1QLADDERCHECK
						if (itype==3 && ir==1)
						{
							if (dir)
							{
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								histn += Form("/%.1fsigma/%s/QvsRig/L1QLadderCheck", dSigma[isigma], sType[itype].c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							for (int iside=0; iside<2; iside++)
							{
								for (int ilad=1; ilad<16; ilad++)
								{
									if (dir)
									{
										histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
										histn += sRig;
										histn += Form("/%.1fsigma/%s/QvsRig/L1QLadderCheck/%s%d", dSigma[isigma], sType[itype].c_str(), ((iside==0)?"+":"-"), ilad+800);
										outfile.mkdir(histn.c_str());
										outfile.cd(histn.c_str());
									}
									
									for (int ixy=ixyLQTemp; ixy<3; ixy++)
									{
										histn = Form("L%dQvsRig_r%d_q%d_xy%d_lad%s%d_%s%.1fsigma", 1, ir, *it, ixy, ((iside==0)?"+":"-"), ilad+800, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
										hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
										hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
										hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", 1, histn1.c_str()));
										hQ2dDis->Sumw2();
									} //ixy
								} //ilad
							} //iside
						}
						#endif //L1QLADDERCHECK
						
						//do also for L9Q the ladder distribution
						#ifdef L9QLADDERCHECK
						if (itype==5 && ir==2)
						{
							if (dir)
							{
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								histn += Form("/%.1fsigma/%s/QvsRig/L9QLadderCheck", dSigma[isigma], sType[itype].c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							for (int iside=0; iside<2; iside++)
							{
								for (int ilad=1; ilad<9; ilad++)
								{
									if (dir)
									{
										histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
										histn += sRig;
										histn += Form("/%.1fsigma/%s/QvsRig/L9QLadderCheck/%s%d", dSigma[isigma], sType[itype].c_str(), ((iside==0)?"+":"-"), ilad+900);
										outfile.mkdir(histn.c_str());
										outfile.cd(histn.c_str());
									}
									
									for (int ixy=ixyLQTemp; ixy<3; ixy++)
									{
										histn = Form("L%dQvsRig_r%d_q%d_xy%d_lad%s%d_%s%.1fsigma", 9, ir, *it, ixy, ((iside==0)?"+":"-"), ilad+900, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
										hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
										hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
										hQ2dDis->GetYaxis()->SetTitle(Form("L%d%sQ", 9, histn1.c_str()));
										hQ2dDis->Sumw2();
									} //ixy
								} //ilad
							} //iside
						}
						#endif //L9QLADDERCHECK
						
						if (itype==5 && *it==14)
						{
							#ifdef ECALMIPS
							if (dir)
							{
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								histn += Form("/%.1fsigma/%s/ECalMIPs", dSigma[isigma], sType[itype].c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							
							histn = Form("rig_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%s%.1fsigma_PassEcal", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("L9QvsRig_r%d_q%d_%s%.1fsigma_PassEcal", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
							
							histn = Form("rig_r%d_q%d_%s%.1fsigma_ishow", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("L9QvsRig_r%d_q%d_%s%.1fsigma_ishow", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
							
							//even log bin for eh & el
							double iEl=0.001, iEu=500.;
							//double iEl=0.1, iEu=500.;
							vector<double> vdEbin;
							for (int ibin=0; ibin<1000+1; ibin++)
							{
								vdEbin.push_back(iEl*exp(ibin/1000.*log(iEu/iEl)));
							} //ibin
							
							for (int iqr=0; iqr<3; iqr++) //iqr=0:<z-0.5, iqr=1:[z-0.5,z+0.5], iqr=2:>z+0.5
							{
								if (dir)
								{
									histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
									histn += sRig;
									histn += Form("/%.1fsigma/%s/ECalMIPs/QR%d", dSigma[isigma], sType[itype].c_str(), iqr);
									outfile.mkdir(histn.c_str());
									outfile.cd(histn.c_str());
								}
								
								//----distribution for each layer
								for (int ilay=0; ilay<18; ilay++)
								{
									//--nhit
									histn = Form("nhitvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 100, 0, 100));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("ecal_nhit l%d", ilay+1));
									
									//--energy each layer
									histn = Form("ecal_elvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, &vdEbin[0]));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("ecal_el l%d [GeV]", ilay+1));
									
									//--highest energy for one cell
									histn = Form("ecal_ehvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, &vdEbin[0]));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("ecal_eh l%d [GeV]", ilay+1));
									
									//--ecal_eh/ecal_el
									histn = Form("ehelvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 200, 0, 1));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
									hQ2dDis->GetYaxis()->SetTitle(Form("eh/el l%d", ilay+1));
								} //ilay
								
								//----rms
								//--nhit
								histn = Form("nhitRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 300, 0, 150));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle(Form("ecal_nhit RMS"));
								
								//--energy each layer
								histn = Form("ecal_elRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 200));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle(Form("ecal_el RMS"));
								
								//--highest energy for one cell
								histn = Form("ecal_ehRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 200));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle(Form("ecal_eh RMS"));
								
								//--ecal_eh/ecal_el
								histn = Form("ehelRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
								histn += sRig;
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 300, 0, 30));
								hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
								hQ2dDis->GetXaxis()->SetTitle("Rigidity [GV]");
								hQ2dDis->GetYaxis()->SetTitle(Form("eh/el RMS"));
								
								//2D plots among 4 variables
								if (dir)
								{
									histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
									histn += sRig;
									histn += Form("/%.1fsigma/%s/ECalMIPs/QR%d/2D", dSigma[isigma], sType[itype].c_str(), iqr);
									outfile.mkdir(histn.c_str());
									outfile.cd(histn.c_str());
								}
								
								//----distribution for each layer
								for (int ilay=0; ilay<18; ilay++)
								{
									//--nhit
									histn = Form("nhitvsecal_el_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 100, 0, 100, 1000, &vdEbin[0]));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("ecal_nhit l%d", ilay+1));
									hQ2dDis->GetYaxis()->SetTitle(Form("ecal_el l%d [GeV]", ilay+1));
									
									histn = Form("nhitvsecal_eh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 100, 0, 100, 1000, &vdEbin[0]));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("ecal_nhit l%d", ilay+1));
									hQ2dDis->GetYaxis()->SetTitle(Form("ecal_eh l%d [GeV]", ilay+1));
									
									histn = Form("nhitvseleh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 100, 0, 100, 200, 0, 1));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("ecal_nhit l%d", ilay+1));
									hQ2dDis->GetYaxis()->SetTitle(Form("eleh l%d", ilay+1));
									
									//--energy each layer, ecal_el
									histn = Form("ecal_elvsecal_eh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0], 1000, &vdEbin[0]));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("ecal_el l%d [Gev]", ilay+1));
									hQ2dDis->GetYaxis()->SetTitle(Form("ecal_eh l%d [Gev]", ilay+1));
									
									histn = Form("ecal_elvseleh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0], 200, 0, 1));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("ecal_el l%d [Gev]", ilay+1));
									hQ2dDis->GetYaxis()->SetTitle(Form("eleh l%d", ilay+1));
									
									//--highest energy for one cell
									histn = Form("ecal_ehvseleh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], iqr);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0], 200, 0, 1));
									hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
									hQ2dDis->GetXaxis()->SetTitle(Form("ecal_eh l%d [Gev]", ilay+1));
									hQ2dDis->GetYaxis()->SetTitle(Form("eleh l%d", ilay+1));
								} //ilay
							} //iqr
							#endif //ECALMIPS
							
							#ifdef ECALMIPS2
							if (dir)
							{
								histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
								histn += sRig;
								histn += Form("/%.1fsigma/%s/ECalMIPs2", dSigma[isigma], sType[itype].c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							
							histn = Form("rig_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//add Ecal Q vs L9Q & Rich Q vs L9Q
							histn = Form("EcalQvsL9Q_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							
							histn = Form("RichQvsL9Q_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							
							//even log bin for Edep
							double iEl=0.001, iEu=500.;
							vector<double> vdEbin;
							for (int ibin=0; ibin<1000+1; ibin++)
							{
								vdEbin.push_back(iEl*exp(ibin/1000.*log(iEu/iEl)));
							} //ibin
							
							for (int is=0; is<2; is++) //is=0: not requiring ishow; is=1: ishow>=0 for now only store the event count & L9Q without any MIPs selection
							//for (int is=0; is<1; is++) //for now only store for not requiring shower
							{
								if (dir)
								{
									histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
									histn += sRig;
									histn += Form("/%.1fsigma/%s/ECalMIPs2/is%d", dSigma[isigma], sType[itype].c_str(), is);
									outfile.mkdir(histn.c_str());
									outfile.cd(histn.c_str());
								}
								for (int isel=0; isel<8; isel++) //isel<4: pass MIPs selection for certain layers start from first; isel>=4: start from fourth layer
								//for (int isel=0; isel<2; isel++) //isel=0: pass S3/S5>0.98 for certain layers start from first; isel=1: start from fourth layer
								{
									if (dir)
									{
										histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
										histn += sRig;
										histn += Form("/%.1fsigma/%s/ECalMIPs2/is%d/Sel%d", dSigma[isigma], sType[itype].c_str(), is, isel);
										outfile.mkdir(histn.c_str());
										outfile.cd(histn.c_str());
									}
									for (int ilaysel=0; ilaysel<18+1; ilaysel++)
									{
										if (isel>=4 && ilaysel<4) continue;
										//if (isel==1 && ilaysel<4) continue;
										if (dir)
										{
											histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
											histn += sRig;
											histn += Form("/%.1fsigma/%s/ECalMIPs2/is%d/Sel%d/laySel%d", dSigma[isigma], sType[itype].c_str(), is, isel, ilaysel);
											outfile.mkdir(histn.c_str());
											outfile.cd(histn.c_str());
										}
										
										//Nev & Q
										histn = Form("rig_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
										histn += sRig;
										hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
										
										histn = Form("L9Q_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
										histn += sRig;
										hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35));
										
										//add Ecal Q vs L9Q & Rich Q vs L9Q
										histn = Form("EcalQvsL9Q_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
										histn += sRig;
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
										
										histn = Form("RichQvsL9Q_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
										histn += sRig;
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
										
										#ifdef ECALMIPS2LAYERDIS
										for (int ilay=0; ilay<18; ilay++)
										{
											if (dir)
											{
												histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
												histn += sRig;
												histn += Form("/%.1fsigma/%s/ECalMIPs2/is%d/Sel%d/laySel%d/l%d", dSigma[isigma], sType[itype].c_str(), is, isel, ilaysel, ilay);
												outfile.mkdir(histn.c_str());
												outfile.cd(histn.c_str());
											}
											//Ecal variable
											histn = Form("S1_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0]));
											
											histn = Form("S3_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0]));
											
											histn = Form("S5_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0]));
											
											histn = Form("all_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, &vdEbin[0]));
											
											histn = Form("S3S5_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1050, 0,1.05));
										
											histn = Form("S5all_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
											histn += sRig;
											hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1050, 0,1.05));
										} //ilay
										#endif //ECALMIPS2LAYERDIS
									} //ilaysel
								} //isel
							} //is
							#endif //ECALMIPS2
						}
					} //itype
					#endif //GETTOFQ ||  GETINNERQ ||  GETL1Q ||  GETL2Q || GETL9Q
					
					#ifdef GETQALL
					if (dir)
					{
						histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
						histn += sRig;
						histn += Form("/%.1fsigma/QAll", dSigma[isigma]);
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					for (int irig=0; irig<iNBL1TF; irig++)
					{
						histn = Form("UTofQ_r%d_q%d_all_%.1fsigma_rig%d", ir, *it, dSigma[isigma], irig);
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
						
						histn = Form("UTofQ_r%d_q%d_all_%.1fsigma_rig%d_PL", ir, *it, dSigma[isigma], irig);
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
						
						histn = Form("InnerQ_r%d_q%d_all_%.1fsigma_rig%d", ir, *it, dSigma[isigma], irig);
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
						
						histn = Form("InnerQ_r%d_q%d_all_%.1fsigma_rig%d_PL", ir, *it, dSigma[isigma], irig);
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
					} //irig
					#endif //GETQALL
				} //isigma
			} //*it
		} //ialgo
	} //ir
	
	#ifdef PRESELECTED //2023.05.08
	#ifdef QDIS2D
	for (int ir=1; ir<3; ir++)
	{
		if (dir)
		{
			outfile.mkdir(Form("%s/PreSelected/%s/2D", sDir.c_str(), cSpan[ir]));
			outfile.cd(Form("%s/PreSelected/%s/2D", sDir.c_str(), cSpan[ir]));
		}
		//L1Q vs lower detector
		histn = Form("L1Q_UTofQ_r%d", ir);
		histn += sRig;
		hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
		hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
		hQ2dDis->GetXaxis()->SetTitle("UTofQ");
		hQ2dDis->GetYaxis()->SetTitle("L1Q");
		
		histn = Form("L1Q_InnerQ_r%d", ir);
		histn += sRig;
		hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
		hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
		hQ2dDis->GetXaxis()->SetTitle("InnerQ");
		hQ2dDis->GetYaxis()->SetTitle("L1Q");
		
		if (ir==2)
		{
			histn = Form("L1Q_LTofQ_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("LTofQ");
			hQ2dDis->GetYaxis()->SetTitle("L1Q");
			
			histn = Form("L1Q_L9Q_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("L9Q");
			hQ2dDis->GetYaxis()->SetTitle("L1Q");
		} //FS
		
		//UTofQ vs lower detector
		histn = Form("UTofQ_InnerQ_r%d", ir);
		histn += sRig;
		hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU)); //use 0.1 binning for whole range
		hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
		hQ2dDis->GetXaxis()->SetTitle("InnerQ");
		hQ2dDis->GetYaxis()->SetTitle("UTofQ");
		
		if (ir==2)
		{
			histn = Form("UTofQ_LTofQ_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("LTofQ");
			hQ2dDis->GetYaxis()->SetTitle("UTofQ");
			
			histn = Form("UTofQ_L9Q_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("L9Q");
			hQ2dDis->GetYaxis()->SetTitle("UTofQ");
		} //FS
		
		//InnerQ vs lower detector
		if (ir==2)
		{
			histn = Form("InnerQ_LTofQ_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("LTofQ");
			hQ2dDis->GetYaxis()->SetTitle("InnerQ");
			
			histn = Form("InnerQ_L9Q_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("L9Q");
			hQ2dDis->GetYaxis()->SetTitle("InnerQ");
		} //FS
		
		//LTofQ vs lower detector
		if (ir==2)
		{
			histn = Form("LTofQ_L9Q_r%d", ir);
			histn += sRig;
			hman1.Add(new TH2F(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
			hQ2dDis = (TH2F*)GetHist(hman1, histn.c_str());
			hQ2dDis->GetXaxis()->SetTitle("L9Q");
			hQ2dDis->GetYaxis()->SetTitle("LTofQ");
		} //FS
	} //ir
	#endif //QDIS2D
	#endif //PRESELECTED //2023.05.08
	
	#if defined L1QTEMP || defined L2QTEMP
	#ifdef KEYSPDF
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		// if (!ev.isreal) break; //only store for ISS //test for MC 2024.06.04
		#ifdef REDUCESIZELQTEMP
		for (int ir=1; ir<2; ir++)
		#else
		for (int ir=1; ir<3; ir++)
		#endif //REDUCESIZELQTEMP
		{
			for (int ialgo=0; ialgo<NRig; ialgo++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev.amsdn%100)<68 && ialgo==1) continue; //skip ialgo=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				string sRig=sRigName[ialgo];
				
				for (int isigma=0; isigma<nSigma; isigma++)
				{
					//also set for L1QTEMP
					for (int il=0; il<2; il++)
					{
						#ifndef L1QTEMP
						if (il==0) continue;
						#endif //L1QTEMP
						#ifndef L2QTEMP
						if (il==1) continue;
						#endif //L2QTEMP
						if (dir)
						{
							//outfile.mkdir(Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf", sDir.c_str(), *it, cSpan[ir], dSigma[isigma], il+1));
							//outfile.cd(Form("%s/q%d/%s/%.1fsigma/L%dQ_KeysPdf", sDir.c_str(), *it, cSpan[ir], dSigma[isigma], il+1));
							histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
							histn += sRig;
							// histn += Form("/%.1fsigma/L%dQ_KeysPdf", dSigma[isigma], il+1);
							histn += Form("/L%dQ_KeysPdf", il+1);
							outfile.mkdir(histn.c_str());
							outfile.cd(histn.c_str());
						}
						for (int irig=-1; irig<iNBL1TF; irig++) //irig=-1: all rig
						{
							for (int isel=0; isel<MSel; isel++)
							{
								if (isel!=iOnlySel) continue;
								histn = Form("L%dQ_r%d_q%d_sel%d_rig%d", il+1, ir, *it, isel, irig);
								if (irig==-1) histn = Form("L%dQ_r%d_q%d_sel%d_allrig", il+1, ir, *it, isel);
								histn += sRig;
								
								tKeysPdf = new TTree(histn.c_str(), histn.c_str());

								histn = Form("L%dQ_r%d_q%d_sel%d_rig%d_TightL1Sam", il+1, ir, *it, isel, irig);
								if (irig==-1) histn = Form("L%dQ_r%d_q%d_sel%d_allrig_TightL1Sam", il+1, ir, *it, isel);
								histn += sRig;
								tTightL1Sam = new TTree(histn.c_str(), histn.c_str());

								//set the branch as double
								if (il==0)
								{
									#ifndef REDUCESIZELQTEMP
									tKeysPdf->Branch("L1QX", &L1QX, "L1QX/D");
									tKeysPdf->Branch("L1QY", &L1QY, "L1QY/D");
									#endif //REDUCESIZELQTEMP
									tKeysPdf->Branch("L1QXY", &L1QXY, "L1QXY/D");

									#ifdef SURSEL
									tKeysPdf->Branch("UBL1QXY", &UBL1QXY, "UBL1QXY/D");
									tTightL1Sam->Branch("L1QXY", &L1QXYSam, "UBL1QXY/D");
									tTightL1Sam->Branch("UBL1QXY", &UBL1QXYSam, "UBL1QXY/D");
									#endif
								}
								else if (il==1)
								{
									//set the name as L1Q as well in order to be able to mix charge template from L1Q and L2Q
									#ifndef REDUCESIZELQTEMP
									tKeysPdf->Branch("L1QX", &L2QX, "L1QX/D");
									tKeysPdf->Branch("L1QY", &L2QY, "L1QY/D");
									#endif //REDUCESIZELQTEMP
									tKeysPdf->Branch("L1QXY", &L2QXY, "L1QXY/D");

									#ifdef SURSEL
									tKeysPdf->Branch("UBL1QXY", &L2QXY, "UBL1QXY/D");
									#endif
								}
							} //isel
						} //irig
					} //il
					#ifdef TRDPAR
					if (dir){
						histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
						histn += sRig;
						histn += Form("/TRD_Information");
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					for (int irig=-1; irig<iNBL1TF; irig++) //irig=-1: all rig
					{
						for (int isel=0; isel<MSel; isel++)
						{
							if (isel!=iOnlySel) continue;
							histn = Form("TRD_r%d_q%d_sel%d_rig%d", ir, *it, isel, irig);
							if (irig==-1) histn = Form("TRD_r%d_q%d_sel%d_allrig", ir, *it, isel);
							histn += sRig;
							tTRDpara = new TTree(histn.c_str(), histn.c_str());
							tTRDpara->Branch("TRD_nhitk", &TRD_nhitk, "TRD_nhitk/I");
							tTRDpara->Branch("TRD_onhitk", &TRD_onhitk, "TRD_onhitk/I");
							tTRDpara->Branch("TRD_amphitk", &TRD_amphitk, "TRD_amphitk/I");
							tTRDpara->Branch("TRD_ntrdcl", &TRD_ntrdcl, "TRD_ntrdcl/I");
						}
					}
					#endif //TRDPAR
				} //isigma
			} //ialgo
		} //ir
	} //*it
	#endif //KEYSPDF
	#endif //L1QTEMP && L2QTEMP
	
	return true;
} //BookChargeCal

#ifdef AMSSOFT
bool SelChargeCal(HistoMan &hman1, SelEvent &ev, TFile &outfile, const bool bRig[3], const bool bTrack[3], const bool bTkgeom[3], const int QVer, const double ww[3])
#else
//bool SelChargeCal(TObjArray &hman1, SelEvent &ev, TFile &outfile, const bool bRig[3], const bool bTrack[3], const bool bTkgeom[3], const int QVer, const double ww[3])
bool SelChargeCal(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, const double ww[3])
#endif //AMSSOFT
{
	string sDir = Form("Charge_Calibration%d", QVer);
	
	//variables used only inside this file
	static double dCharge[NQDis_store] = {0.};
	static double dInInQ[3] = {0.}, dInQrms = 0., dInInQrms = 0.;
	static double dTkLQ[9] = {0.};
	static double dTkLQXY[9][3] = {{0.}};
	static int comXY=0;
	
	double dQdis[2][3]={{0.}};
	
	//use new Q getter function
	dCharge[0] = ev.GetTkInQNew(2, QVer);	//innerQ
	dInQrms = ev.GetTkInQrmsNew(2, QVer);
	dCharge[1] = ev.GetTOFUDQ(0);			//UTofQ
	dCharge[2] = ev.GetTOFUDQ(1);			//LTOfQ
	
	for (int ixy=0; ixy<3; ixy++) dInInQ[ixy] = ev.CalTrunCharge2(ixy, QVer);
	#ifdef INNINNERQ
	dCharge[3] = ev.CalTrunCharge2(2, QVer);
	dInInQrms = ev.CalTrunChargeRMS(2, QVer);
	#endif //INNINNERQ
	
	for (int il=0; il<9; il++)
	{
		dTkLQ[il] = ev.GetTkLQNew(il, 2, QVer);
		for (int ixy=0; ixy<3; ixy++)
		{
			dTkLQXY[il][ixy] = ev.GetTkLQNew(il, ixy, QVer);
		} //ixy
	} //ils
	
	for (int ixy=0; ixy<3; ixy++)
	{
		dQdis[0][ixy] = ev.GetTkInQNew(ixy, QVer);
		dQdis[1][ixy] = ev.GetTkInQrmsNew(ixy, QVer);
	} //ixy
	
	TH1F hL1QTF("hL1QTF", "hL1QTF", iNBL1TF, dBL1TF);
	
	//--fill histograms
	//non-pattern selections
	#ifdef REDUCESIZELQTEMP
	int iLowSpan=0;
	#if not defined GETL1Q || defined L1QTEMP || defined L2QTEMP
	iLowSpan=1;
	#endif //!GETL1Q || L1QTEMP || L2QTEMP
	for (int ir=iLowSpan; ir<2; ir++)
	#else
	for (int ir=0; ir<3; ir++)
	#endif //REDUCESIZELQTEMP
	{
		for (int ialgo=0; ialgo<NRig; ialgo++)
		{
			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && ialgo==1) continue; //skip ialgo=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			
			//--tracker + fiducial volume + L1Q status + L1XY
			#ifdef L2QTMEPWOL1CHISQY //TrTrack without L1ChisqY<10
			//if (!(ev.Select_TrTrack(ir, 101) && bTkgeom[ir] && ((ev.tk_qls[0] & 0x10013D)==0))) continue;
			if (!(ev.Select_TrTrack(ir, 101, ialgo) && bTkgeom[ir] && ((ev.tk_qls[0] & 0x10013D)==0))) continue; //2023.04.06
			#else
			//if (!(bTrack[ir] && bTkgeom[ir] && bRig[ir] && ((ev.tk_qls[0] & 0x10013D)==0))) continue;
			#ifndef SURSEL //2023.04.16
			// if (!(bTrack[ir][ialgo] && bTkgeom[ir] && bRig[ir][ialgo] && ((ev.tk_qls[0] & 0x10013D)==0))) continue; //2023.04.06
			if (!(bTrack[ir][ialgo] && bTkgeom[ir] && bRig[ir][ialgo] && ((ev.tk_qls[0] & 0x10013D)==0))) continue; //2023.04.06
			#else
			int iReferSpan=(ir==1)?0:3; //using Inner rigidity for L1Inner,InnerL9 fo FullSpan
			if (!(ev.Select_TrTrack(iReferSpan, 111, ialgo) && bTkgeom[ir] && ev.Select_Rig(1, icffv, iReferSpan, 1.2, ialgo) && ((ev.tk_qls[0] & 0x10013D)==0))) continue; //2023.04.06
			// if (!(bTrack[ir][ialgo] && bTkgeom[ir] && bRig[ir][ialgo] && ((ev.tk_qls[0] & 0x10013D)==0))) continue; //2023.04.06
			#endif
			#endif
			string sRig=sRigName[ialgo];
			
			#ifdef PRESELECTED //2023.05.08
			for (int iqdis=0; iqdis<NQDis_store; iqdis++)
			{
				//all but charge
				histn = Form("%s_r%d", cQDis[iqdis], ir);
				histn += sRig;
				GetHist(hman1, histn.c_str())->Fill(dCharge[iqdis], ww[ir]);
			} //iqdis
			
			#ifdef TRUNQCOM
			//--InnerQ vs InnInnerQ
			histn = Form("InnerQvsInnInnerQ_r%d", ir);
			histn += sRig;
			GetHist2d(hman1, histn.c_str())->Fill(dCharge[3], dCharge[0], ww[ir]);
			
			//--InnerQ vs InnerQ from CalTrunCharge
			histn = Form("InnerQvsInnerQCal_r%d", ir);
			histn += sRig;
			GetHist2d(hman1, histn.c_str())->Fill(ev.CalTrunCharge(0xFE), dCharge[0], ww[ir]);
			#endif //TRUNQCOM
			
			//------all tof layer
			for (int il=0; il<4; il++)
			{
				histn = Form("TofL%dQ_r%d", il+1, ir);
				histn += sRig;
				GetHist(hman1, histn.c_str())->Fill(ev.tof_ql[il], ww[ir]);
			} //il
			
			//all inner layers distribution
			for (int il=0; il<9; il++)
			{
				if (il>1 && il<8) continue;
				histn = Form("L%dQ_r%d", il+1, ir);
				histn += sRig;
				GetHist(hman1, histn.c_str())->Fill(dTkLQ[il], ww[ir]);
				
				#ifdef BADL1LAD
				if (il==0 && ev.tk_lid[0]==+811)
				{
					histn = Form("L%dQ_r%d_badl1lad", il+1, ir);
					histn += sRig;
					GetHist(hman1, histn.c_str())->Fill(dTkLQ[il], ww[ir]);
				
				}
				#endif //BADL1LAD
			} //il
			
			//qrms
			histn = Form("InnerQrms_r%d", ir);
			histn += sRig;
			GetHist(hman1, histn.c_str())->Fill(dInQrms, ww[ir]);
			
			histn = Form("InnInnerQrms_r%d", ir);
			histn += sRig;
			GetHist(hman1, histn.c_str())->Fill(dInInQrms, ww[ir]);
			
			#ifdef QVSRIG
			for (int iqdis=0; iqdis<NQDis_store; iqdis++)
			{
				//all but charge
				histn = Form("%svsRig_r%d", cQDis[iqdis], ir);
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dCharge[iqdis], ww[ir]);
			} //iqdis
			
			//------all tof layer
			for (int il=0; il<4; il++)
			{
				histn = Form("TofL%dQvsRig_r%d", il+1, ir);
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.tof_ql[il], ww[ir]);
			} //il
			
			//all inner layers distribution
			for (int il=0; il<9; il++)
			{
				if (il>1 && il<8) continue;
				histn = Form("L%dQvsRig_r%d", il+1, ir);
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQ[il], ww[ir]);
				
				#ifdef BADL1LAD
				if (il==0 && ev.tk_lid[0]==+811)
				{
					histn = Form("L%dQvsRig_r%d_badl1lad", il+1, ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQ[il], ww[ir]);
				}
				#endif //BADL1LAD
			} //il
			
			//qrms
			histn = Form("InnerQrmsvsRig_r%d", ir);
			histn += sRig;
			GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dInQrms, ww[ir]);
			#endif //QVSRIG
			
			//2d plots
			#ifdef QDIS2D
			if (ir>0)
			{
				//L1Q vs lower detector
				histn = Form("L1Q_UTofQ_r%d", ir);
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(dCharge[1], dTkLQ[0], ww[ir]);
			
				histn = Form("L1Q_InnerQ_r%d", ir);
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(dCharge[0], dTkLQ[0], ww[ir]);
			
				if (ir==2)
				{
					histn = Form("L1Q_LTofQ_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dCharge[2], dTkLQ[0], ww[ir]);
				
					histn = Form("L1Q_L9Q_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], dTkLQ[0], ww[ir]);
				} //FS
			
				//UTofQ vs lower detector
				histn = Form("UTofQ_InnerQ_r%d", ir);
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(dCharge[0], dCharge[1], ww[ir]);
	
				if (ir==2)
				{
					histn = Form("UTofQ_LTofQ_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dCharge[2], dCharge[1], ww[ir]);
				
					histn = Form("UTofQ_L9Q_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], dCharge[1], ww[ir]);
				} //FS
			
				//InnerQ vs lower detector
				if (ir==2)
				{
					histn = Form("InnerQ_LTofQ_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dCharge[2], dCharge[0], ww[ir]);
				
					histn = Form("InnerQ_L9Q_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], dCharge[0], ww[ir]);
				} //FS
			
				//LTofQ vs lower detector
				if (ir==2)
				{
					histn = Form("LTofQ_L9Q_r%d", ir);
					histn += sRig;
					GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], dCharge[2], ww[ir]);
				} //FS
			
			} //L1Inner and FS
			#endif //QDIS2D
			#endif //PRESELECTED
			
			//store Inner pattern as well
			#if not defined GETL1Q || defined L1QTEMP || defined L2QTEMP
			if (ir<1) continue;
			#endif //!GETL1Q || L1QTEMP || L2QTEMP
			
			bool bHZ=false;
			
			#ifndef SURSEL
			int iSelRig = hL1QTF.FindBin(ev.GetRigidity(ir))-1;
			#else
			int iSelRig = hL1QTF.FindBin(ev.GetRigidity(iReferSpan))-1;
			#endif
			if (iSelRig>=hL1QTF.GetNbinsX() || iSelRig<0) iSelRig=-1; //overflow or underflow
			
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				if (!ev.isreal && *it!=ev.mch) continue;
				for (int isigma=0; isigma<nSigma; isigma++)
				{
					#if defined GETTOFQ || defined GETINNERQ || defined GETL1Q || defined GETL2Q || defined GETL9Q || defined L1QTEMP || defined L2QTEMP
					for (int itype=0; itype<MType; itype++)
					{
						#ifndef GETTOFQ
						if (itype==0 || itype==1) continue;
						#endif //GETTOFQ
						#ifndef GETINNERQ
						if (itype==2) continue;
						#endif //GETINNERQ
						#ifndef GETL1Q
						if (itype==3) continue;
						#endif //GETL1Q
						#ifndef GETL2Q
						if (itype==4) continue;
						#endif //GETL2Q
						#ifndef GETL9Q
						if (itype==5) continue;
						#endif //GETL9Q
						#ifndef L1QTEMP
						if (itype==6) continue;
						#endif //L1QTEMP
						#ifndef L2QTEMP
						if (itype==7) continue;
						#endif //L2QTEMP
						
						if (ir<2 && (itype==1 || itype==5)) continue;
						#if defined L1QTEMP || defined L2QTEMP || defined GETINNERQ || defined GETL1Q
						//--force to store all distribution for all charge //2023.02.07
						#ifndef STOREALLQ
						if (*it!=8 && *it!=10 && *it!=14 && *it!=26 && ((itype!=2 && itype!=3 && itype!=6 && itype!=7))) continue;
						#endif //STOREALLQ
						#else
						if (*it!=8 && *it!=10 && *it!=14 && *it!=26) continue;
						#endif //L1QTEMP || L2QTEMP
						
						bool bTypeSel=false;
						
						#ifdef GETROUGHALL
						//for L2QTEMP (itype==7) used same selection as GETL2Q (itype==4)
						bTypeSel=true;
						
						//as discussed with QY, use +/-0.5 for all charge cut
						for (int iqdis=0; iqdis<6; iqdis++)
						{
							if (iqdis==3) continue;
							if (ir<2 && (iqdis==2 || iqdis==5)) continue;
							if (itype==0 && iqdis==1) continue; //UTofQ
							if (itype==1 && iqdis==2) continue; //LTofQ
							if (itype==2 && iqdis==0) continue; //InnerQ
							if (itype==3 && iqdis==4) continue; //L1Q
							if (itype==5 && iqdis==5) continue; //L9Q
							if (itype==6 && iqdis==4) continue; //L1QTemp
							// cout << "\tiType:" << itype << "\tiqdis:" << iqdis << endl;
							
							#if defined L1QTemp || defined L2QTEMP //do not apply Upper tof cut for L1 charge for L1Template fit
							if (itype==3 && iqdis==1) continue;
							#endif //L2QTEMP
							
							if ((itype==2 || itype==3 || itype==4 || itype==6 || itype==7) && (iqdis==1 || iqdis==2)) //two layers for each tof when selecting InnerQ, L1Q, L2Q, L1QTemp and L2QTemp
							{
								//--utofq cut
								if (iqdis==1) bTypeSel = bTypeSel && (fabs(ev.GetTOFUDQ(0)-*it)<0.5);
								// #ifdef NEWL1PDF
								// if (iqdis==1) bTypeSel = bTypeSel && (fabs(ev.GetTOFUDQ(0)-*it)<0.25);
								// #endif //NEWL1PDF
								// if (iqdis==1) cout << "\t Charge Seleciton for UTOF" << endl;
								//--ltofq cut
								//tighter on upper limit for secondary, as the heavier nearby charge has much larger abundance; looser on upper limit for Iron, as the heavier nearby charge is negligible
								if (iqdis==2)
								{
									if (*it%2==1) bTypeSel = bTypeSel && (fabs(ev.GetTOFUDQ(1)-*it)<0.5);
									else bTypeSel = bTypeSel && ev.GetTOFUDQ(1)>*it-0.5 && ev.GetTOFUDQ(1)<*it+1.5;
									// cout << "\t Charge Seleciton for LTOF" << endl;
								}
								
								//--compatiable charge measurement from each tof layers
								int layi=(iqdis==1)?0:2;
								bTypeSel = bTypeSel && (fabs(ev.GetTofLQ(layi)-ev.GetTofLQ(layi+1))<fabs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
								// cout << "\t Charge Seleciton for layer in TOF" << endl;
								
								//add lower tofq cut for InnerQ sample and for charge template
								if (itype==2 || ((itype==6 || itype==7) && (*it%2==1 || *it>20)))
								{
									//tighter on upper limit for secondary, as the heavier nearby charge has much larger abundance; looser on upper limite for Iron, as the heavier nearby charge is negligible
									if (*it%2==1) bTypeSel = bTypeSel && (fabs(ev.GetTOFUDQ(1)-*it)<0.5);
									else bTypeSel = bTypeSel && ev.GetTOFUDQ(1)>*it-0.5 && ev.GetTOFUDQ(1)<*it+1.5;
									bTypeSel = bTypeSel && (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(3))<fabs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
									
									bTypeSel = bTypeSel && ev.IsGoodTofQUD(0) && ev.IsGoodTofQUD(1);
									// cout << "\t Charge Seleciton for LTOF on odd charge" << endl;
								}
								
								//for InnerQSam force apply tight LTOFQ cut, no matter what geometry
								if (itype==2)
								{
									bTypeSel = bTypeSel && (fabs(ev.GetTOFUDQ(1)-*it)<0.5);
								}
							}
							else if ((itype==4 || itype==7)&& iqdis==0) //L3-L8 for GETL2Q & L2QTEMP
							{
								//bTypeSel = bTypeSel && (fabs(ev.CalTrunCharge2(2,QVer)-*it)<0.5);
								//2023.02.14: update to the InnerQ range, which is smaller than 0.5 for Q<14
								//bTypeSel = bTypeSel && (fabs(ev.CalTrunCharge2(2,QVer)-*it)<ev.GetQSelRange(0, *it, 0, QVer));
								bTypeSel = bTypeSel && (fabs(ev.CalTrunCharge2(2,QVer)-*it)<fabs(*it-ev.GetQSelRange(0, *it, 0, QVer)));
								
								//bTypeSel = bTypeSel && (ev.CalTrunChargeRMS(2,QVer)<1); //2020.09.28: take out the InnInnerQ rms cut
							}
							//apply same cuts as event count for Inner Tracker for L1Q which going to be fitted by template to extract background
							#if defined L1QTEMP || defined L2QTEMP
							else if (itype==0)
							{
								bTypeSel = bTypeSel && ev.Select_InnerQ(*it,QVer);
							}
							#endif
							//+/-0.5
							else 
							{
								bTypeSel = bTypeSel && (fabs(ev.GetSubDetQ(iqdis,QVer)-*it)<0.5);
								// #ifdef NEWL1PDF
								// if(iqdis == 0) bTypeSel = bTypeSel && (fabs(ev.GetSubDetQ(iqdis,QVer)-*it)<0.25);
								// #endif //NEWL1PDF
								//loose InnerQ rms cut
								//if (iqdis==0) bTypeSel = bTypeSel && (dInQrms<1);
								if (iqdis==0 && *it<9) bTypeSel = bTypeSel && (dInQrms<1); //not apply Qrms cut for Q>=9, same as event selection //2023.02.14
								// cout << "\t Charge Seleciton for Qdis: " << iqdis << endl;
								if(itype==6){
									if(*it<14 && *it%2==0) {
										bTypeSel = bTypeSel && (fabs(ev.GetSubDetQ(iqdis,QVer)-*it)<0.25); //2025.03.31 for less background
										if(*it<10){
											bTypeSel = bTypeSel && (fabs(ev.GetTOFUDQ(0)-*it)<0.25);
											if(*it<8){
												bTypeSel = bTypeSel && ((ev.GetSubDetQ(iqdis,QVer)-*it)<0.15);
											}
										}
									}
								}
							}
						} //iqdis
						#ifdef SURSEL /// keep the same selection with SurProb
						if(itype==6 || itype==7){
							const int iTrigPeriod=ev.TriggerPeriod();
							// cout << "\tTrigger Period: " << iTrigPeriod << endl;
							bTypeSel = bTypeSel && (iTrigPeriod == 1) && ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0  && ((ev.tk_exqls[0] & 0x10013D)==0);
							// bTypeSel = bTypeSel && (iTrigPeriod == 0) && ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0  && ((ev.tk_exqls[0] & 0x10013D)==0);
							// bTypeSel = bTypeSel && ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0  && ((ev.tk_exqls[0] & 0x10013D)==0);
							// bTypeSel = bTypeSel && (iTrigPeriod == 1);
							// cout << "\t Charge Seleciton for UBL1Hit" << endl;
							#ifdef NEWL1PDF // 2024.06.20
							if(*it > 3) bTypeSel = bTypeSel && ev.GetUBExtQTrkCor(0, 2, QVer)>3;
							#endif //NEWL1PDF
						}
						#endif //SURSEL
						#endif //GETROUGHALL
						
						#ifdef L1QSAMTEMP
						if (itype==3)
						{
							bTypeSel = ev.Select_TofQ(*it,ir,QVer) && ev.Select_InnerQ(*it,QVer);
							//apply unbiased L1XY for Inner+MaxL1Q
							#ifdef INNERUNBIAEDL1Q
							bool bUnbiasedL1XY=true;
							if (ir==0)
							{
								bUnbiasedL1XY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0; //unbiased L1XY
								//bUnbiasedL1Q=(ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && (ev.GetUBExtQTrkCor(0, 2, QVer)<ev.GetQSelRange(1, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
								bTypeSel = bTypeSel && bUnbiasedL1XY;
							}
							#endif //INNERUNBIAEDL1Q
						}
						
						#endif //L1QSAMTEMP
						
						#ifdef INNERQEVENTSEL
						if (itype==2)
						{
							bTypeSel = ev.Select_TofQ(*it,2,QVer) && ev.Select_ExtQ(*it,ir,QVer); //force to apply LTOFQ for L1Inner as well
							if (*it<9 && *it>2) bTypeSel = bTypeSel && (ev.GetTkInQrmsNew(2, QVer) < 0.55);
						}
						#endif //INNERQEVENTSEL
						
						if (bTypeSel)
						{
							for (int iqdis=0; iqdis<NQDis_store; iqdis++)
							{
								histn = Form("%s_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist(hman1, histn.c_str())->Fill(dCharge[iqdis], ww[ir]);
								if (iqdis==0)
								{
									#ifdef TRUNQCOM
									//--InnerQ vs InnInnerQ
									histn = Form("InnerQvsInnInnerQ_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(dCharge[3], dCharge[0], ww[ir]);

									//--InnerQ vs InnerQ from CalTrunCharge
									histn = Form("InnerQvsInnerQCal_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.CalTrunCharge(0xFE), dCharge[0], ww[ir]);
									#endif //TRUNQCOM
								}
							} //iqdis
							
							//------all tof layer
							for (int il=0; il<4; il++)
							{
								histn = Form("TofL%dQ_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist(hman1, histn.c_str())->Fill(ev.tof_ql[il], ww[ir]);
							} //il
							
							for (int il=0; il<9; il++)
							{
								if ((itype!=2 && itype!=4) && (il>1 && il<8)) continue;
								histn = Form("L%dQ_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								//GetHist(hman1, histn.c_str())->Fill(dTkLQ[il], ww[ir]);
								if (il!=0 || ev.tk_lid[0]!=+811) GetHist(hman1, histn.c_str())->Fill(dTkLQ[il], ww[ir]); //to have a better L1Q shape, exclude bad ladder
								
								if (itype==3 && il==0)
								{
									if ((fabs(ev.tof_ql[0]-*it)<0.5) && (fabs(ev.tof_ql[1]-*it)<0.5))
									{
										for (int ixy=ixyLQTemp; ixy<3; ixy++)
										{
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											if (ev.tk_lid[0]!=+811) GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
										} //ixy
									}
								}
								
								#ifdef BADL1LAD
								if (il==0)
								{
									if (ev.tk_lid[0]==+811)
									{
										histn = Form("L%dQ_r%d_q%d_badl1lad_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										GetHist(hman1, histn.c_str())->Fill(dTkLQ[il], ww[ir]);
									}
								
									//normal l1 ladder
									if (ev.tk_lid[0]==-811)
									{
										histn = Form("L%dQ_r%d_q%d_norml1lad_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										GetHist(hman1, histn.c_str())->Fill(dTkLQ[il], ww[ir]);
									}
								}
								#endif //BADL1LAD
								
								//store unbiased L1Q as well
								if (itype==3 && il==0)
								{
									histn = Form("unbiasedL%dQ_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									if (il!=0 || ev.tk_lid[0]!=+811) GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ww[ir]); //to have a better L1Q shape, exclude bad ladder
									#ifdef ISOTOPEBGSUBQ
									histn = Form("unbiasedL%dQ_r%d_q%d_%s%.1fsigma_BGSub", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									if (il!=0 || ev.tk_lid[0]!=+811)
									{
										if (ev.ntrack==1 || ev.Select_Tk2nd()) GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ww[ir]);
									}
									#endif //ISOTOPEBGSUB
								}
							} //il
							
							//qrms
							histn = Form("InnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dInQrms, ww[ir]);
							
							histn = Form("InnerQvsInnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dInQrms, dCharge[0], ww[ir]);
							
							histn = Form("InnInnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dInInQrms, ww[ir]);
							
							histn = Form("InnInnerQvsInnInnerQrms_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dInInQrms, dCharge[3], ww[ir]);
							
							#if defined L1QTEMP || defined L2QTEMP
							bool bL2QTempSel=false;
							#ifdef KEYSPDF
							L1QX=dTkLQXY[0][0];
							L1QY=dTkLQXY[0][1];
							L1QXY=dTkLQXY[0][2];
							
							L2QX=dTkLQXY[1][0];
							L2QY=dTkLQXY[1][1];
							L2QXY=dTkLQXY[1][2];
							#ifdef SURSEL
							UBL1QXY = ev.GetUBExtQTrkCor(0, 2, QVer);
							#endif //SURSEL
							#ifdef TRDPAR
							TRD_nhitk = ev.trd_nhitk;
							TRD_onhitk = ev.trd_onhitk;
							TRD_amphitk = ev.trd_amphitk;
							TRD_ntrdcl = ev.ntrdcl;
							#endif //TRDPAR
							#endif //KEYSPDF
							
							for (int ixy=ixyLQTemp; ixy<3; ixy++)
							{
								if (itype!=3 && itype!=6 && itype!=7) break; //store distributions in L1QSam, L1QTemp and L2QTemp
								if (ixy==0) histn1="X";
								else if (ixy==1) histn1="Y";
								else if (ixy==2) histn1="";
								
								//--all inner layer
								for (int il=1-1; il<9-1; il++)
								{
									if (((itype==3 || itype==6) && il!=0) || (itype==7 && il!=1)) continue; //only l1 for L1QSam (itype=3) and L1QTemp (itype==6), and l2 for L2QTemp (itype=7)
									
									//--extra selection for LQTemp
									//--L1QTemp: 2nd Track selection
									if (itype==6)
									{
										#ifdef QDISRIGBIN
										if (iSelRig!=-1)
										{
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_Tk2nd0", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], iOnlySel, iSelRig);
											histn += sRig;
											if (il!=0 || ev.tk_lid[0]!=+811) GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
										
											histn = Form("betah2q_r%d_q%d_%s%.1fsigma_rig%d_Tk2nd0", ir, *it, sType[itype].c_str(), dSigma[isigma], iSelRig);
											histn += sRig;
											if (ixy==2) GetHist(hman1, histn.c_str())->Fill(ev.betah2q, ww[ir]);

											#ifdef SURSEL
											if((il == 0) && ixy == 2){
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_Tk2nd0_UnbiasedL1Q", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], iOnlySel, iSelRig);
												histn += sRig;
												GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ww[ir]);

												L1QXYSam = dTkLQXY[il][ixy];
												UBL1QXYSam = ev.GetUBExtQTrkCor(0, 2, QVer);
												histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
												histn += sRig;
												histn += Form("/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d_TightL1Sam", il+1, il+1, ir, *it, iOnlySel, iSelRig);
												histn += sRig;
												tTightL1Sam = static_cast<TTree*>(outfile.Get(histn.c_str()));
												tTightL1Sam->Fill();
											}
											#endif // SURSEL
										}
										#ifdef SURSEL
										if((il == 0) && ixy == 2){
											L1QXYSam = dTkLQXY[il][ixy];
											UBL1QXYSam = ev.GetUBExtQTrkCor(0, 2, QVer);
											histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
											histn += sRig;
											histn += Form("/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig_TightL1Sam", il+1, il+1, ir, *it, iOnlySel);
											histn += sRig;
											tTightL1Sam = static_cast<TTree*>(outfile.Get(histn.c_str()));
											tTightL1Sam->Fill();
										}
										#endif // SURSEL
										#endif //QDISRIGBIN
										
										//----
										#ifndef NEWL1PDF
										//if (!ev.Select_Tk2nd()) continue;
										if (!( ev.ntrack==1 || !ev.Select_Tk2ndRecon() )) continue; //2020.04.17: change the non-fragment selection
										//if (!( ev.ntrack==1 || !ev.Select_Tk2nd() )) continue; //2023.02.14: should use the Select_Tk2nd instead
										// cout << "\t Charge Seleciton for Fragmentation" << endl;
										#endif //NEWL1PDF
										
										#ifdef QVSRIG
										histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_Tk2ndCut", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										if (ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]);
										#endif //QVSRIG
										
										//----
										#ifdef QDISRIGBIN
										if (iSelRig!=-1)
										{
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_Tk2nd1", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], iOnlySel, iSelRig);
											histn += sRig;
											if (il!=0 || ev.tk_lid[0]!=+811) GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
										
											histn = Form("betah2q_r%d_q%d_%s%.1fsigma_rig%d_Tk2nd1", ir, *it, sType[itype].c_str(), dSigma[isigma], iSelRig);
											histn += sRig;
											if (ixy==2) GetHist(hman1, histn.c_str())->Fill(ev.betah2q, ww[ir]);
										}
										#endif //QDISRIGBIN
									}
									//--L2QTemp: L2XY & L3-L8 Charge cut
									else if (itype==7)
									{
										//--L2XY
										if (!(ev.HasTkLHitXY(1)==2)) continue;
									
										//--L3-L8 Q cut
										histn = Form("L3L8Q_r%d_q%d_xy%d_%s%.1fsigma0", ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										GetHist(hman1, histn.c_str())->Fill(dInInQ[ixy], ww[ir]);
										//if (fabs(ev.CalTrunCharge2(2,QVer)-*it)>=0.5) continue; //2019.07.31: use the correct variable and the +/-0.5 range
										//if (fabs(ev.CalTrunCharge2(2,QVer)-*it)>=ev.GetQSelRange(0, *it, 0, QVer)) continue; //2023.02.14: use the range as event cut, tighter for Z<14
										if (fabs(ev.CalTrunCharge2(2,QVer)-*it)>=fabs(ev.GetQSelRange(0, *it, 0, QVer)-*it)) continue; //2023.04.15: correctly apply the cut
										histn = Form("L3L8Q_r%d_q%d_xy%d_%s%.1fsigma1", ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										GetHist(hman1, histn.c_str())->Fill(dInInQ[ixy], ww[ir]);
									
									}
									
									for (int isel=0; isel<MSel; isel++)
									{
										if (isel!=iOnlySel) continue;
										//only apply extra q_status on L2QTemp (itype==7)
										if (itype==7)
										{
											if (isel==0) bL2QTempSel=true;
											else if (isel==1) bL2QTempSel=ev.Select_TkLQStatus(il);
											else if (isel==2) bL2QTempSel=((ev.tk_qls[il]&0x13D100)==0);
											else if (isel==3) bL2QTempSel=ev.Select_TkLQStatus(il) && ((ev.tk_qls[il]&0x13D100)==0);
											else if (isel==4) bL2QTempSel=((ev.tk_qls[il]&0x1001FD)==0);
											else if (isel==5) bL2QTempSel=((ev.tk_qls[il]&0x1FD100)==0);
											else if (isel==6) bL2QTempSel=((ev.tk_qls[il]&0x1001FD)==0) && ((ev.tk_qls[il]&0x1FD100)==0);
											else if (isel==7) bL2QTempSel=((ev.tk_qls[il]&0x10013D)==0) && ((ev.tk_qls[il]&0x1001FD)!=0);
											else if (isel==8) bL2QTempSel=((ev.tk_qls[il]&0x13D100)==0) && ((ev.tk_qls[il]&0x1FD100)!=0);
											else if (isel==9) bL2QTempSel=((ev.tk_qls[il]&0x10013D)==0) && ((ev.tk_qls[il]&0x13D100)==0) && (((ev.tk_qls[il]&0x1001FD)!=0) || ((ev.tk_qls[il]&0x1FD100)!=0)); //XY old Q status and not X Q status new or not Y Q status new
							
											if (!bL2QTempSel) continue;
										}
										
										#ifdef QDISRIGBIN
										if (iSelRig!=-1)
										{
											//--X, Y distribution
											histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
											histn += sRig;
											if (il!=0 || ev.tk_lid[0]!=+811)
											{
												GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
												
												//store the L1Q with uTOFQ cut
												if (il==0 && itype==3) //only for L1QSam
												{
													histn = Form("UTofQ_r%d_q%d_%s%.1fsigma_sel%d_rig%d", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
													histn += sRig;
													if (ixy==2) GetHist(hman1, histn.c_str())->Fill(dCharge[1], ww[ir]);
													
													//L1Q with event count uTOFQ cut
													if (ev.Select_TofQ(*it,1,QVer))
													{
														histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_uTOFQ", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
														histn += sRig;
														GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
														
														histn = Form("UTofQ_r%d_q%d_%s%.1fsigma_sel%d_rig%d_cutted", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
														histn += sRig;
														if (ixy==2) GetHist(hman1, histn.c_str())->Fill(dCharge[1], ww[ir]);
													}
													
													//L1Q with tight uTOFQ cut
													if ((fabs(ev.tof_ql[0]-*it)<0.5) && (fabs(ev.tof_ql[1]-*it)<0.5))
													{
														histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
														histn += sRig;
														GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
														
														histn = Form("UTofQ_r%d_q%d_%s%.1fsigma_sel%d_rig%d_cutted_tight", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
														histn += sRig;
														if (ixy==2) GetHist(hman1, histn.c_str())->Fill(dCharge[1], ww[ir]);
													}
													
												}
												//charge stauts distribution
												#ifdef BADL1LADCHECK
												if (il==0)
												{
													if (ixy!=2)
													{
														histn = Form("L%dQstatus_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
														histn += sRig;
														int ibitshift=(ixy==0)?0:12; //x-side: 0-11, y-side: 12-23
														bool bBitFlip=false;
														for (int ibit=0; ibit<12; ibit++)
														{
															if ((ev.tk_qls[il]&(1<<(ibit+ibitshift)))!=0)
															{
																GetHist(hman1, histn.c_str())->Fill(ibit, ww[ir]);
																bBitFlip=true;
															}
														} //ibit
														if (!bBitFlip) GetHist(hman1, histn.c_str())->Fill(-1, ww[ir]);
													}
													
													//L1Q with different gain-related Q status
													for (int ipat=0; ipat<4; ipat++)
													{
														if (ixy==1) continue; //only check XQ & XYQ
														bool bBitPat=false;
														if (ipat==0) bBitPat=((ev.tk_qls[il]&(1<<(6)))!=0); //X side bit 6
														else if (ipat==1) bBitPat=((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 7
														else if (ipat==2) bBitPat=((ev.tk_qls[il]&(1<<(6)))!=0) || ((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 6 or 7
														else if (ipat==3) bBitPat=!((ev.tk_qls[il]&(1<<(6)))!=0) && !((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 6 and 7 not flagged
														
														if (!bBitPat) continue;
														histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_pat%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig, ipat);
														histn += sRig;
														GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
													} //ipat
												}
												#endif //BADL1LADCHECK
											}
											
											#ifdef BADL1LADCHECK
											if (il==0 && ev.tk_lid[0]==+811)
											{
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_badl1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
												histn += sRig;
												GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
												
												//charge stauts distribution
												if (ixy!=2)
												{
													histn = Form("L%dQstatus_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_badl1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
													histn += sRig;
													int ibitshift=(ixy==0)?0:12; //x-side: 0-11, y-side: 12-23
													bool bBitFlip=false;
													for (int ibit=0; ibit<12; ibit++)
													{
														if ((ev.tk_qls[il]&(1<<(ibit+ibitshift)))!=0)
														{
															GetHist(hman1, histn.c_str())->Fill(ibit, ww[ir]);
															bBitFlip=true;
														}
													} //ibit
													if (!bBitFlip) GetHist(hman1, histn.c_str())->Fill(-1, ww[ir]);
												}
												
												//L1Q with different gain-related Q status
												for (int ipat=0; ipat<4; ipat++)
												{
													if (ixy==1) continue; //only check XQ & XYQ
													bool bBitPat=false;
													if (ipat==0) bBitPat=((ev.tk_qls[il]&(1<<(6)))!=0); //X side bit 6
													else if (ipat==1) bBitPat=((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 7
													else if (ipat==2) bBitPat=((ev.tk_qls[il]&(1<<(6)))!=0) || ((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 6 or 7
													else if (ipat==3) bBitPat=!((ev.tk_qls[il]&(1<<(6)))!=0) && !((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 6 and 7 not flagged
													
													if (!bBitPat) continue;
													histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_pat%d_badl1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig, ipat);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
												} //ipat
											}
											
											//normal l1 ladder
											if (il==0 && ev.tk_lid[0]==-811)
											{
												histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_norml1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
												histn += sRig;
												GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
												
												//charge stauts distribution
												if (ixy!=2)
												{
													histn = Form("L%dQstatus_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_norml1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
													histn += sRig;
													int ibitshift=(ixy==0)?0:12; //x-side: 0-11, y-side: 12-23
													bool bBitFlip=false;
													for (int ibit=0; ibit<12; ibit++)
													{
														if ((ev.tk_qls[il]&(1<<(ibit+ibitshift)))!=0)
														{
															GetHist(hman1, histn.c_str())->Fill(ibit, ww[ir]);
															bBitFlip=true;
														}
													} //ibit
													if (!bBitFlip) GetHist(hman1, histn.c_str())->Fill(-1, ww[ir]);
												}
												
												//L1Q with different gain-related Q status
												for (int ipat=0; ipat<4; ipat++)
												{
													if (ixy==1) continue; //only check XQ & XYQ
													bool bBitPat=false;
													if (ipat==0) bBitPat=((ev.tk_qls[il]&(1<<(6)))!=0); //X side bit 6
													else if (ipat==1) bBitPat=((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 7
													else if (ipat==2) bBitPat=((ev.tk_qls[il]&(1<<(6)))!=0) || ((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 6 or 7
													else if (ipat==3) bBitPat=!((ev.tk_qls[il]&(1<<(6)))!=0) && !((ev.tk_qls[il]&(1<<(7)))!=0); //X side bit 6 and 7 not flagged
													
													if (!bBitPat) continue;
													histn = Form("L%dQ_r%d_q%d_xy%d_%s%.1fsigma_sel%d_rig%d_pat%d_norml1lad", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], isel, iSelRig, ipat);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il][ixy], ww[ir]);
												} //ipat
											}
											#endif //BADL1LADCHECK
										}
										#endif //QDISRIGBIN
										
										#ifdef KEYSPDF
										// if ((itype==6 || itype==7) && ixy==2 && ev.isreal)
										if ((itype==6 || itype==7) && ixy==2) //test fot MC 2024.06.04
										{
											if (iSelRig!=-1)
											{
												histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
												histn += sRig;
												// histn += Form("/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d", dSigma[isigma], il+1, il+1, ir, *it, isel, iSelRig);
												histn += Form("/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_rig%d", il+1, il+1, ir, *it, isel, iSelRig);
												histn += sRig;
												tKeysPdf = static_cast<TTree*>(outfile.Get(histn.c_str()));
												if (il!=0 || ev.tk_lid[0]!=+811) tKeysPdf->Fill(); //not storing L1 bad ladder
												// tKeysPdf->Fill(); //2024.05.07
											}
											
											//all rig
											histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
											histn += sRig;
											// histn += Form("/%.1fsigma/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", dSigma[isigma], il+1, il+1, ir, *it, isel);
											histn += Form("/L%dQ_KeysPdf/L%dQ_r%d_q%d_sel%d_allrig", il+1, il+1, ir, *it, isel);
											histn += sRig;
											tKeysPdf = static_cast<TTree*>(outfile.Get(histn.c_str()));
											if (il!=0 || ev.tk_lid[0]!=+811) tKeysPdf->Fill(); //not storing L1 bad ladder
											// tKeysPdf->Fill(); //2024.05.07
										}
										#ifdef TRDPAR
										// cout << "TRD number of Hits: " << TRD_nhitk << endl;
										if ((itype==6) && ixy==2 && il == 0){
											if (iSelRig!=-1)
											{
												histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
												histn += sRig;
												histn += Form("/TRD_Information/TRD_r%d_q%d_sel%d_rig%d", ir, *it, isel, iSelRig);
												histn += sRig;
												tTRDpara = static_cast<TTree*>(outfile.Get(histn.c_str()));
												tTRDpara->Fill();
											}
											
											//all rig
											histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
											histn += sRig;
											histn += Form("/TRD_Information/TRD_r%d_q%d_sel%d_allrig", ir, *it, isel);
											histn += sRig;
											tTRDpara = static_cast<TTree*>(outfile.Get(histn.c_str()));
											tTRDpara->Fill();
										}
										#endif //TRDPAR
										#endif //KEYSPDF
										
										
										//rigidity distribution
										if (itype==7 && il==1 && ixy==2)
										{
											histn = Form("rig_r%d_q%d_%s%.1fsigma_sel%d", ir, *it, sType[itype].c_str(), dSigma[isigma], isel);
											histn += sRig;
											GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
											
											#ifdef QDISRIGBIN
											if (iSelRig!=-1)
											{
												histn = Form("rig_r%d_q%d_%s%.1fsigma_sel%d_rig%d", ir, *it, sType[itype].c_str(), dSigma[isigma], isel, iSelRig);
												histn += sRig;
												GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
											}
											#endif //QDISRIGBIN
										}
										
										#ifdef QVSRIG //Q vs Rig after charge status cut on L2Q
										if (itype==3 || itype==6) continue;
										for (int iqdis=0; iqdis<NQDis_store; iqdis++)
										{
											if (ixy!=2) break;
											histn = Form("%svsRig_Cut_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dCharge[iqdis], ww[ir]);
										} //iqdis
					
										for (int il0=0; il0<9; il0++)
										{
											if (il0>1 && il0<8) continue;
											histn = Form("L%dQvsRig_Cut_r%d_q%d_xy%d_%s%.1fsigma", il0+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											if (il0!=0 || ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il0][ixy], ww[ir]); //to have a better L1Q shape, exclude bad ladder
											
											//--1d L2Q distirbuiton, used to compare the template
											histn = Form("L%dQ_Cut_r%d_q%d_xy%d_%s%.1fsigma", il0+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											if (il0!=0 || ev.tk_lid[0]!=+811) GetHist(hman1, histn.c_str())->Fill(dTkLQXY[il0][ixy], ww[ir]);
										} //il0
					
										//qrms
										histn = Form("InnerQrmsvsRig_Cut_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										if (ixy==2) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dInQrms, ww[ir]);
										#endif //QVSRIG
									} //isel
								} //il
								
							} //ixy
							#endif //L1QTEMP || L2QTEMP
							
							#ifdef QVSRIG
							for (int iqdis=0; iqdis<NQDis_store; iqdis++)
							{
								histn = Form("%svsRig_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dCharge[iqdis], ww[ir]);
							} //iqdis
							
							//------all tof layer
							for (int il=0; il<4; il++)
							{
								histn = Form("TofL%dQvsRig_r%d_q%d_%s%.1fsigma", il+1, ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.tof_ql[il], ww[ir]);
							} //il
							
							for (int il=0; il<9; il++)
							{
								if (il>1 && il<8) continue;
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									if (il!=0 || ev.tk_lid[0]!=+811)
									{
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]); //to have a better L1Q shape, exclude bad ladder
										
										//store the distribution after additional event cut
										for (int ievcut=1; ievcut<3; ievcut++)
										{
											bool bEvCut=true;
											if (ievcut==1) bEvCut=(ev.ntrack==1);
											else if (ievcut==2) bEvCut=(ev.tof_chist_n<5 && ev.tof_chisc_n<5);
											else bEvCut=true;
											
											if (!bEvCut) continue;
											histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_EvCut%d", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma], ievcut);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]); //to have a better L1Q shape, exclude bad ladder
										} //ievcut
									}
									
									#ifdef BADL1LAD
									if (il==0 && itype==3) //store only for L1QSam
									{
										if (ev.tk_lid[0]==+811)
										{
											histn = Form("L%dQvsRig_r%d_q%d_xy%d_badl1lad_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]);
				
										}
										//normal l1 ladder
										if (ev.tk_lid[0]==-811)
										{
											histn = Form("L%dQvsRig_r%d_q%d_xy%d_norml1lad_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]);
				
										}
									}
									#endif //BADL1LAD
									
									if (itype==3 && il==0 && ixy==2)
									{
										if ((fabs(ev.tof_ql[0]-*it)<0.5) && (fabs(ev.tof_ql[1]-*it)<0.5))
										{
											histn = Form("L%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_uTOFQ_tight", 1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											if (ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]);
										}
									}
								
									//store unbiased L1Q as well
									if (itype==3 && il==0)
									{
										histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										if (il!=0 || ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.GetUBExtQTrkCor(0, ixy, QVer), ww[ir]); //to have a better L1Q shape, exclude bad ladder
										
										if (ixy==2)
										{
											if ((fabs(ev.tof_ql[0]-*it)<0.5) && (fabs(ev.tof_ql[1]-*it)<0.5))
											{
												histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
												histn += sRig;
												if (ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.GetUBExtQTrkCor(0, ixy, QVer), ww[ir]);
											}
										}
										#ifdef ISOTOPEBGSUBQ
										if (ev.ntrack==1 || ev.Select_Tk2nd())
										{
											histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_BGSub", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
											histn += sRig;
											if (il!=0 || ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.GetUBExtQTrkCor(0, ixy, QVer), ww[ir]); //2019.08.04: to have a better L1Q shape, exclude bad ladder
											
											if (ixy==2)
											{
												if ((fabs(ev.tof_ql[0]-*it)<0.5) && (fabs(ev.tof_ql[1]-*it)<0.5))
												{
													histn = Form("unbiasedL%dQvsRig_r%d_q%d_xy%d_%s%.1fsigma_BGSub_uTOFQ_tight", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
													histn += sRig;
													if (ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.GetUBExtQTrkCor(0, ixy, QVer), ww[ir]);
												}
											}
										}
										#endif //ISOTOPEBGSUB
									}
								} //ixy
							} //il
							
							//qrms
							histn = Form("InnerQrmsvsRig_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dInQrms, ww[ir]);
							
							//store for GETL2Q (itype==4) the charge distribution with L2XY & L2Q status applied
							if (itype==4 && (ev.HasTkLHitXY(1)==2) && ev.Select_TkLQStatus(1))
							{
								for (int iqdis=0; iqdis<NQDis_store; iqdis++)
								{
									histn = Form("%svsRig_Cut_r%d_q%d_%s%.1fsigma", cQDis[iqdis], ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dCharge[iqdis], ww[ir]);
								} //iqdis
			
								for (int il=0; il<9; il++)
								{
									if (il>1 && il<8) continue;
									for (int ixy=ixyLQTemp; ixy<3; ixy++)
									{
										histn = Form("L%dQvsRig_Cut_r%d_q%d_xy%d_%s%.1fsigma", il+1, ir, *it, ixy, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										//if (il!=0 || ev.tk_lid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]);
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[il][ixy], ww[ir]);
									} //ixy
								} //il
			
								//qrms
								histn = Form("InnerQrmsvsRig_Cut_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dInQrms, ww[ir]);
								
								#ifdef L2QLADDERCHECK
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									if (ir!=1) break;
									histn = Form("L%dQvsRig_Cut_r%d_q%d_xy%d_lad%+d_%s%.1fsigma", 2, ir, *it, ixy, ev.tk_lid[1], sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[1][ixy], ww[ir]);
								} //ixy
								#endif //L2QLADDERCHECK
							}
							
							#ifdef CHECK2NDCUT
							if (itype==3)
							{
								//isam=0: no 2nd cut
								//isam=1: ntrack=1
								//isam=2: ntrack>1 (!isam1) and no well recon Tk2nd
								//isam=3: have well recon Tk2nd (!isam2) and betah2r/tof_betah<=0
								//isam=4: have well recon Tk2nd and betah2r/tof_betah>0 (!isam3)
								for (int isam0=0; isam0<2; isam0++)
								{
									if (ir<1) break;
									
									int isam=isam0;
									if (isam0>0)
									{
										if (ev.ntrack==1) isam=1;
										else if (!ev.Select_Tk2ndRecon()) isam=2;
										else if (ev.betah2r/ev.tof_betah<=0) isam=3;
										else isam=4;
									}
									
									histn = Form("L1QvsRig_r%d_q%d_%s_sam%d", ir, *it, sType[itype].c_str(), isam);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[0][2], ww[ir]);
									
									////iqrange=0: <Z-0.5, iqrange=1: Z+/-0.5, iqrange=2: >=Z+0.5, iqrange=3: >=Z+2+0.5
									//for (int iqrange=0; iqrange<4; iqrange++)
									//2020.03.05
									//iqrange=0: <Z+0.5, iqrange=1: >=Z+0.5, iqrange=2: >=Z+2-0.5
									for (int iqrange=0; iqrange<3; iqrange++)
									{
										/*if (iqrange==0) {if (dTkLQXY[0][2]>=*it-0.5) continue;}
										else if (iqrange==1) {if (dTkLQXY[0][2]<*it-0.5 || dTkLQXY[0][2]>=*it+0.5) continue;}
										else if (iqrange==2) {if (dTkLQXY[0][2]<*it+0.5) continue;}
										else if (iqrange==3) {if (dTkLQXY[0][2]<*it+2-0.5) continue;}*/
										
										//2020.03.05
										if (iqrange==0) {if (dTkLQXY[0][2]>=*it+0.5) continue;}
										else if (iqrange==1) {if (dTkLQXY[0][2]<*it+0.5) continue;}
										else if (iqrange==2) {if (dTkLQXY[0][2]<*it+2-0.5) continue;}
									
										//------betah2r
										if (ev.betah2r>0) histn = Form("+logbetah2rvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
										else histn = Form("-logbetah2rvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
										histn += sRig;
										if (ev.betah2r!=0) GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), log(fabs(ev.betah2r)), ww[ir]);
										
										//------betah2r/rig
										histn = Form("betah2r_over_RigvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.betah2r/ev.GetRigidity(ir), ww[ir]);
										
										//------betah2q
										histn = Form("betah2qvsRig_r%d_q%d_%s_sam%d_qrange%d", ir, *it, sType[itype].c_str(), isam, iqrange);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.betah2q, ww[ir]);
									
										//------tof_oq
										for (int itofl=0; itofl<2; itofl++)
										{
											for (int ioq=0; ioq<2; ioq++)
											{
												histn = Form("tof_oq_%d_%dvsRig_r%d_q%d_%s_sam%d_qrange%d", itofl, ioq, ir, *it, sType[itype].c_str(), isam, iqrange);
												histn += sRig;
												GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.tof_oq[itofl][ioq], ww[ir]);
											} //ioq
										} //itofl
									} //iqrange
								} //isam0
							}
							#endif //CHECK2NDCUT
							
							#ifdef L1QLADDERCHECK
							if (itype==3 && ir==1)
							{
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_lad%+d_%s%.1fsigma", 1, ir, *it, ixy, ev.tk_lid[0], sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[0][ixy], ww[ir]);
								} //ixy
							}
							#endif //L1QLADDERCHECK
							
							//store also for L9Q the ladder distribution
							#ifdef L9QLADDERCHECK
							if (itype==5 && ir==2)
							{
								for (int ixy=ixyLQTemp; ixy<3; ixy++)
								{
									histn = Form("L%dQvsRig_r%d_q%d_xy%d_lad%+d_%s%.1fsigma", 9, ir, *it, ixy, ev.tk_lid[8], sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQXY[8][ixy], ww[ir]);
								} //ixy
							}
							#endif //L9QLADDERCHECK
							#endif //QVSRIG
							
							if (itype==5 && *it==14)
							{
								#ifdef ECALMIPS
								histn = Form("rig_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
								
								if (ev.IsPassEcal())
								{
									histn = Form("rig_r%d_q%d_%s%.1fsigma_PassEcal", ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
									
									histn = Form("L9QvsRig_r%d_q%d_%s%.1fsigma_PassEcal", ir, *it, sType[itype].c_str(), dSigma[isigma]);
									histn += sRig;
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQ[8], ww[ir]);
									
									if (ev.ishow>=0)
									{
										histn = Form("rig_r%d_q%d_%s%.1fsigma_ishow", ir, *it, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
										histn = Form("L9QvsRig_r%d_q%d_%s%.1fsigma_ishow", ir, *it, sType[itype].c_str(), dSigma[isigma]);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), dTkLQ[8], ww[ir]);
										
										int QRange; //iqr=0:<z-0.5, iqr=1:[z-0.5,z+0.5], iqr=2:>z+0.5
										if (dTkLQ[8]<*it-0.5) QRange=0;
										else if (dTkLQ[8]>=*it-0.5 && dTkLQ[8]<*it+0.5) QRange=1;
										else QRange=2;
								
										double nhitRMS=0, elRMS=0, ehRMS=0, elehrRMS=0;
										double nhitMean=0, elMean=0, ehMean=0, elehrMean=0;
										for (int ilay=0; ilay<18; ilay++)
										{
											nhitMean+=ev.ecal_nhit[ilay];
											elMean+=ev.ecal_el[ilay]/1000.;
											ehMean+=ev.ecal_eh[ilay]/1000.;
											elehrMean+=ev.ecal_eh[ilay]/ev.ecal_el[ilay];
										} //ilay
								
										//----distribution for each layer
										for (int ilay=0; ilay<18; ilay++)
										{
											nhitRMS+=pow(ev.ecal_nhit[ilay]-nhitMean,2);
											elRMS+=pow(ev.ecal_el[ilay]/1000.-elMean,2);
											ehRMS+=pow(ev.ecal_eh[ilay]/1000.-ehMean,2);
											elehrRMS+=pow(ev.ecal_eh[ilay]/ev.ecal_el[ilay]-elehrMean,2);
									
											//--nhit
											histn = Form("nhitvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.ecal_nhit[ilay], ww[ir]);
								
											//--energy each layer
											histn = Form("ecal_elvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.ecal_el[ilay]/1000., ww[ir]);
								
											//--highest energy for one cell
											histn = Form("ecal_ehvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.ecal_eh[ilay]/1000., ww[ir]);
								
											//--ecal_eh/ecal_el
											histn = Form("ehelvsRig_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ev.ecal_eh[ilay]/ev.ecal_el[ilay], ww[ir]);
											//2019.05.14: 2D plots among 4 variables
											//--nhit
											histn = Form("nhitvsecal_el_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.ecal_nhit[ilay], ev.ecal_el[ilay]/1000., ww[ir]);
											histn = Form("nhitvsecal_eh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.ecal_nhit[ilay], ev.ecal_eh[ilay]/1000., ww[ir]);
											
											histn = Form("nhitvseleh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.ecal_nhit[ilay], ev.ecal_eh[ilay]/ev.ecal_el[ilay], ww[ir]);
											
											//--energy each layer, ecal_el
											histn = Form("ecal_elvsecal_eh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.ecal_el[ilay]/1000., ev.ecal_eh[ilay]/1000., ww[ir]);
											
											histn = Form("ecal_elvseleh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.ecal_el[ilay]/1000., ev.ecal_eh[ilay]/ev.ecal_el[ilay], ww[ir]);
											
											//--highest energy for one cell
											histn = Form("ecal_ehvseleh_l%d_r%d_q%d_%s%.1fsigma_qr%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
											histn += sRig;
											GetHist2d(hman1, histn.c_str())->Fill(ev.ecal_eh[ilay]/1000., ev.ecal_eh[ilay]/ev.ecal_el[ilay], ww[ir]);
										} //ilay
							
										//----rms
										nhitRMS=sqrt(nhitRMS/nhitMean);
										elRMS=sqrt(elRMS/elMean);
										ehRMS=sqrt(ehRMS/ehMean);
										elehrRMS=sqrt(elehrRMS/elehrMean);
								
										//--nhit
										histn = Form("nhitRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), nhitRMS, ww[ir]);
							
										//--energy each layer
										histn = Form("ecal_elRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), elRMS, ww[ir]);
							
										//--highest energy for one cell
										histn = Form("ecal_ehRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ehRMS, ww[ir]);
							
										//--ecal_eh/ecal_el
										histn = Form("ehelRMSvsRig_r%d_q%d_%s%.1fsigma_qr%d", ir, *it, sType[itype].c_str(), dSigma[isigma], QRange);
										histn += sRig;
										GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), elehrRMS, ww[ir]);
									} //ishow>=0
								} //IsPassEcal
								#endif //ECALMIPS
								
								#ifdef ECALMIPS2
								histn = Form("rig_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
								
								histn = Form("EcalQvsL9Q_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], ev.ecal_q, ww[ir]);
							
								histn = Form("RichQvsL9Q_r%d_q%d_%s%.1fsigma", ir, *it, sType[itype].c_str(), dSigma[isigma]);
								histn += sRig;
								bool bRich=ev.SelectRich();
								if (bRich) GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], ev.ecal_q, ww[ir]);
								
								if (ev.IsPassEcal2()) //2019.05.28: use detail Ecal coordinate
								//if (ev.IsPassEcal())
								{
									for (int is=0; is<2; is++) //2019.05.27: is=0: not requiring ishow; is=1: ishow>=0 for now only store the event count & L9Q without any MIPs selection
									//for (int is=0; is<1; is++) //2019.05.26: for now only store for not requiring shower
									{
										if (is==1 && ev.ishow<0) continue;
										for (int isel=0; isel<8; isel++) //isel<4: pass MIPs selection for certain layers start from first; isel>=4: start from fourth layer
										//for (int isel=0; isel<2; isel++) //isel=0: pass S3/S5>0.98 for certain layers start from first; isel=1: start from fourth layer
										{
											//if (is==1 && isel>0) break;
											for (int ilaysel=0; ilaysel<18+1; ilaysel++)
											{
												//if (is==1 && ilaysel>0) break;
												if (isel>=4 && ilaysel<4) continue;
												//if (isel==1 && ilaysel<4) continue;
												bool bPass=true;
												if (ilaysel>0)
												{
													bPass=ev.GetEcalS3(ilaysel-1,is)/ev.GetEcalS5(ilaysel-1,is)<0.98;
													if (isel%4>0) bPass = bPass && ev.EcalMatchness2(isel%4-1,is,8,ilaysel-1);
													//bPass=ev.ecal_S3[ilaysel-1]/ev.ecal_S5[ilaysel-1]<0.98;
													//if (isel%4>0) bPass = bPass && ev.EcalMatchness(isel%4-1,8,ilaysel-1);
													
													if (!bPass) break;
													//if (ev.ecal_S3[ilaysel-1]/ev.ecal_S5[ilaysel-1]<0.98) break;
												}
												//Nev & Q
												histn = Form("rig_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
												histn += sRig;
												GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww[ir]);
							
												histn = Form("L9Q_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
												histn += sRig;
												GetHist(hman1, histn.c_str())->Fill(dTkLQ[8], ww[ir]);
												
												histn = Form("EcalQvsL9Q_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
												histn += sRig;
												GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], ev.ecal_q, ww[ir]);
										
												histn = Form("RichQvsL9Q_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
												histn += sRig;
												if (bRich) GetHist2d(hman1, histn.c_str())->Fill(dTkLQ[8], ev.ecal_q, ww[ir]);
												
												#ifdef ECALMIPS2LAYERDIS
												for (int ilay=0; ilay<18; ilay++)
												{
													//Ecal variable
													histn = Form("S1_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(ev.GetEcalS1(ilay,is), ww[ir]);
													//GetHist(hman1, histn.c_str())->Fill(ev.ecal_S1[ilay], ww[ir]);
											
													histn = Form("S3_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(ev.GetEcalS3(ilay,is), ww[ir]);
													//GetHist(hman1, histn.c_str())->Fill(ev.ecal_S3[ilay], ww[ir]);
											
													histn = Form("S5_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(ev.GetEcalS5(ilay,is), ww[ir]);
													//GetHist(hman1, histn.c_str())->Fill(ev.ecal_S5[ilay], ww[ir]);
											
													histn = Form("all_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(ev.GetEcalAll(ilay,is), ww[ir]);
													//GetHist(hman1, histn.c_str())->Fill(ev.ecal_el2[ilay], ww[ir]);
											
													histn = Form("S3S5_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(ev.GetEcalS3(ilay,is)/ev.GetEcalS5(ilay,is), ww[ir]);
													//GetHist(hman1, histn.c_str())->Fill(ev.ecal_S3[ilay]/ev.ecal_S5[ilay], ww[ir]);
										
													histn = Form("S5all_l%d_r%d_q%d_%s%.1fsigma_is%d_Sel%d_Pass%d", ilay+1, ir, *it, sType[itype].c_str(), dSigma[isigma], is, isel, ilaysel);
													histn += sRig;
													GetHist(hman1, histn.c_str())->Fill(ev.GetEcalS5(ilay,is)/ev.GetEcalAll(ilay,is), ww[ir]);
													//GetHist(hman1, histn.c_str())->Fill(ev.ecal_S5[ilay]/ev.ecal_el2[ilay], ww[ir]);
												} //ilay
												#endif //ECALMIPS2LAYERDIS
											} //ilaysel
										} //isel
									} //is
								} //IsPassEcal
								#endif //ECALMIPS2
							}
						} //bTypeSel
					} //itype
					#endif //GETTOFQ || GETINNERQ || GETL1Q || GETL2Q
					
					#ifdef GETQALL //2019.07.01: need to update to new charge selection function
					/*//--UTofQ all, L1Q & InnerQ
					//int iSelRig=-1; //2019.03,20
					if (ev.Select_BZChargeU(*it, 4, 0, dSigma[isigma], 1) && ev.Select_BZChargeU(*it, 0, 0, dSigma[isigma], 1))
					{
						if (iSelRig!=-1)
						{
							histn = Form("UTofQ_r%d_q%d_all_%.1fsigma_rig%d", ir, *it, dSigma[isigma], iSelRig);
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dCharge[1], ww[ir]);
							
							histn = Form("UTofQ_r%d_q%d_all_%.1fsigma_rig%d_PL", ir, *it, dSigma[isigma], iSelRig);
							histn += sRig;
							if (ev.IsGoodTofQUD(0)) GetHist(hman1, histn.c_str())->Fill(dCharge[1], ww[ir]);
						}
					} //L1Q && InnerQ
					
					//--InnerQ all, L1Q, UTofQ & InnerQ
					//iSelRig=-1; //2019.03,20
					if (ev.Select_BZChargeU(*it, 4, 0, dSigma[isigma], 1) && ev.Select_BZChargeU(*it, 1, 0, dSigma[isigma], 1) && ev.Select_BZChargeU(*it, 2, 0, dSigma[isigma], 1))
					{
						if (iSelRig!=-1)
						{
							histn = Form("InnerQ_r%d_q%d_all_%.1fsigma_rig%d", ir, *it, dSigma[isigma], iSelRig);
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dCharge[0], ww[ir]);
							
							histn = Form("InnerQ_r%d_q%d_all_%.1fsigma_rig%d_PL", ir, *it, dSigma[isigma], iSelRig);
							histn += sRig;
							if (ev.IsGoodTofQUD(0) && ev.IsGoodTofQUD(1)) GetHist(hman1, histn.c_str())->Fill(dCharge[0], ww[ir]);
						}
					} //L1Q && UTofQ && LTofQ*/
					#endif //GETQALL
					
				} //isigma
				
				if (bHZ) break; //find for one BZ then break the loop on all BZSELECTION charges
			} //it
		} //ialgo
	} //ir
	
	return true;
} //SelChargeCal

bool BookL1QDisSel(TObjArray &hman1, SelEvent &ev, TFile &outfile, int QVer, bool dir=false)
{
	string sDir = Form("L1QDisSel%d", QVer);
	for (int ir=1; ir<3; ir++)
	{
		for (int ialgo=0; ialgo<NRig; ialgo++)
		{
			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && ialgo==1) continue; //skip ialgo=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			string sRig=sRigName[ialgo];
			
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				if (!ev.isreal && *it!=ev.mch) continue;
				
				//----define signal and background region according to the difference of InnerQ and L1Q
				//--store the distributon for a few different rigidity range, to see whether the variable distribution stay the same, to see whether the cut has rigidity dependence.
				//As for low rigidity, the BG is higher, stronger power is needed, while for high rig the background is smaller, worse power can be toleranted
				for (int itofqcut=0; itofqcut<2; itofqcut++)
				{
					if (dir)
					{
						histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
						histn += sRig;
						histn += Form("/tofqcut%d", itofqcut);
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					
					//--
					for (int iqdis=0; iqdis<6; iqdis++)
					{
						histn = Form("%s_r%d_q%d", cQDis[iqdis], ir, *it);
						histn += Form("_tofqcut%d", itofqcut);
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
						TH1F *hQDis = (TH1F*)GetHist(hman1, histn.c_str());
						hQDis->GetXaxis()->SetTitle(Form("%s", cQDis[iqdis]));
						hQDis->GetYaxis()->SetTitle("Nevent");
					} //iqdis
					
					//------all tof layer
					for (int itofl=0; itofl<4; itofl++)
					{
						histn = Form("TofL%dQ_r%d_q%d", itofl+1, ir, *it);
						histn += Form("_tofqcut%d", itofqcut);
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU));
						TH1F *hQDis = (TH1F*)GetHist(hman1, histn.c_str());
						hQDis->GetXaxis()->SetTitle(Form("TofL%dQ", itofl+1));
					} //il
					
					//--variable vs L1Q in different rigidity range
					for (int irig=-1; irig<3; irig++) //irig=-1: all, =0: <7 GV, =1: >20 GV, =2: >100 GV
					{
						if (dir)
						{
							histn = Form("%s/q%d/%s", sDir.c_str(), *it, cSpan[ir]);
							histn += sRig;
							histn += Form("/tofqcut%d", itofqcut);
							histn += Form("/RigRange%d", irig);
							outfile.mkdir(histn.c_str());
							outfile.cd(histn.c_str());
						}
						
						for (int ivar=0; ivar<15; ivar++) //now check the same variables as QY
						{
							histn = Form("L1Qvs%s_r%d_q%d", sVarL1QSel[ivar].c_str(), ir, *it);
							histn += Form("_tofqcut%d", itofqcut);
							histn += Form("_RigRange%d", irig);
							histn += sRig;
							
							int nb=10;
							float bl=0,bh=10;
							if(ivar>=6)nb=20;
							if(ivar==6)bh=20;
							if(ivar==5){nb=80;bh=160;}
							if(ivar==7||ivar==8||ivar==9){nb=50;bh=7;}
							if(ivar==10){nb=150;bh=1500;}
							if(ivar==11||ivar==12){nb=50;bl=-5;bh=5;}
							if(ivar==13){nb=150;bh=1500;}
							if(ivar==14){nb=150,bh=1500;}
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), nb, bl, bh, 350, 0, 35));
						} //ivar
					} //irig
				} //itofqcut
			} //it
		} //ialgo
	} //ir
	return true;
} //BookL1QDisSel

bool SelL1QDisSel(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, const double ww[3])
{
	string sDir = Form("L1QDisSel%d", QVer);
	
	static double dCharge[6] = {0.};
	dCharge[0] = ev.GetTkInQNew(2, QVer);		//innerQ
	dCharge[1] = ev.GetTOFUDQ(0);				//UTofQ
	dCharge[2] = ev.GetTOFUDQ(1);				//LTOfQ
	dCharge[3] = ev.CalTrunCharge2(2, QVer);	//InnInnerQ
	dCharge[4] = ev.GetTkLQNew(0, 2, QVer);		//L1Q
	dCharge[5] = ev.GetTkLQNew(8, 2, QVer);		//L9Q
	
	for (int ir=1; ir<3; ir++)
	{
		for (int ialgo=0; ialgo<NRig; ialgo++)
		{
			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && ialgo==1) continue; //skip ialgo=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			string sRig=sRigName[ialgo];
			
			if (!(bTrack[ir][ialgo] && bTkgeom[ir] && bRig[ir][ialgo] && ((ev.tk_qls[0] & 0x10013D)==0))) continue;
			
			double dL1Q = ev.GetTkLQNew(0, 2, QVer);
			
			//--
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				if (!ev.isreal && *it!=ev.mch) continue;
				
				//--apply event charge selection
				//if (!ev.Select_Charge(*it, ir, QVer)) continue; //should not appy L1Q cut
				bool bChargeCut = ev.Select_InnerQ(*it, QVer) && ev.Select_TofQ(*it, ir, QVer);
				if (ir==2) bChargeCut = bChargeCut && ev.Select_ExtQ(*it, 3, QVer);
				if (!bChargeCut) continue;
				
				//--
				for (int itofqcut=0; itofqcut<2; itofqcut++)
				{
					if (itofqcut==1)
					{
						int nqud[2]={0};
						for(int ilay=0;ilay<4;ilay++)
						{
						   if(ev.IsGoodTofLQStat(ilay)) {nqud[ilay/2]++;}
						}
						bool tofgpt=(nqud[0]+nqud[1]==4);
						bool cuttofqz2u=(ev.Select_TofQ(*it, 1)&&fabs(ev.GetTOFUDQ(0)-*it)<0.5);
						bool cuttofqz2d=(ev.Select_TofQ(*it, 3)&&fabs(ev.GetTOFUDQ(1)-*it)<0.5);
						bool cutttof2=(tofgpt&&cuttofqz2u&&cuttofqz2d);
						if (!cutttof2) continue;
					}
					
					//--Q distribution, to check the sample selection
					for (int iqdis=0; iqdis<6; iqdis++)
					{
						histn = Form("%s_r%d_q%d", cQDis[iqdis], ir, *it);
						histn += Form("_tofqcut%d", itofqcut);
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(dCharge[iqdis], ww[ir]);
					} //iqdis
					
					for (int itofl=0; itofl<4; itofl++)
					{
						histn = Form("TofL%dQ_r%d_q%d", itofl+1, ir, *it);
						histn += Form("_tofqcut%d", itofqcut);
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(ev.tof_ql[itofl], ww[ir]);
					} //itofl
					
					//--
					for (int ivar=0; ivar<15; ivar++) //now check the same variables as QY
					{
						double dVar = ev.GetL1QDisSelVar(ivar, *it);
						for (int irig=-1; irig<3; irig++) //irig=-1: all, =0: <7 GV, =1: >20 GV, =2: >100 GV
						{
							if (irig>=0)
							{
								double dRig = ev.GetRigidity(ir, ialgo);
								if (irig==0 && dRig>7) continue;
								else if (irig==1 && dRig<20) continue;
								else if (irig==2 && dRig<100) continue;
							}
							
							histn = Form("L1Qvs%s_r%d_q%d", sVarL1QSel[ivar].c_str(), ir, *it);
							histn += Form("_tofqcut%d", itofqcut);
							histn += Form("_RigRange%d", irig);
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dVar, dL1Q, ww[ir]);
						} //irig
					} //ivar
				} //itofqcut
			} //it
		} //ialgo
	} //ir
	return true;
} //SelL1QDisSel
