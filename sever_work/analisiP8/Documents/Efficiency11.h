//-----------------
//updated from: Efficiency10.C
//updated date: 2019.10.16
//-----------------
//2019.10.16
//	1) update the logic of EFFTIGHTSAM:
//		- define a array of tightenness of the sample, set no. samples we want to study by flag EFFTIGHTSAM, and then loop over them for selection
//		- put also the normal selection inside the loop with tightenness of sample = 1
//		- now the tightenness are set to be {1,0.75,0.6,0.5} and 4 samples in total
//		- store for L1BZ & L1PU Eff, TofQEff, TkEff and L9Eff
//	2) remove FillTrigEff
//	3) remove BetaCorL1Q distribution for Tracking Efficiency
//
//2019.10.17
//	1) remove L1BZCHECK
//	2) remove L1PUEFFTIGHTSAM
//	3) remove L1EFFWITHOUTTOFQ
//
//2019.10.25
//	1) EFFMCTRUTH: store the check on efficiency using MC truth.
//	   For L9Eff, store the L8->L9 survival probability by select a primary up to L8 events using MC truth and the L9 selection efficiency by select a primary up to L9 events using MC truth
//	2) in BookHistoL1Eff, use the loop to book histograms for BZ and PU
//	3) change the naming convention for L1Eff, TofQEff and L9Eff
//
//2019.10.28
//	1) For Efficiency using MC truth, also store efficiency requiring unfragmnented together with the charge cut since that might influence the charge resolution (need to think).
//	   Which is simply the Efficiency from the SamCleanness
//	2) for L9 Efficiency, also add the sample cleanness
//
//2019.11.01
//	1) for TofQ cut for Tracking Efficiency, use the symmetric cut with the lower limit from twiki
//
//2019.11.11
//	1) for L1Inner, only require the particle stay primary up to L7
//
//2020.02.06
//	1) update all BookHistoXXX to have SelEvent as parameter and for MC only book the MC charge
//
//2020.06.03
//	1) update the tracking efficiency for Iron:
//		- loose the Inner Tracker charge cut range for only the track reconstruction efficiency
//		- for successive eff, move the charge cut to the last, store the InnerQ vs rig before applying the InnerQ cut
//
//2020.06.04
//	1) store Inner Tracker charge efficiency using normal sample - BookHistoTkQEff & SelTkQEff
//
//2020.07.08
//	1) store UTofQ for L1Inner from sample selected by LTofQ
//	2) store L9Eff together with lTofQ cut
//	3) refine the Inner Tracker eff
//
//2020.07.16
//	1) store TkQEff without L1Q status for sample
//
//2020.07.31
//	1) for TkQEff, take out each sample charge selection and store the result
//
//2020.08.03
//	1) store the TkQEff same not passing the FS selection
//
//2020.09.09
//	1) store the TkEff sample successive - TKEFFSAMSUCC
//
//2020.10.25
//	1) use flag EFFSEC to control whether store the efficiency sample for nuclei different from the generated nuclei of the MC
//
//2020.11.05
//	1) for EFFSEC, store for tighten sample as well
//	2) for EFFSEC, store also those require to not fragment up to L2, which shows the fraction removing the part of below L1 BG. Probably not need TODO
//
//2021.01.17
//	1) for Trigger and TOFQ eff, store the sample requiring only ntrack==1 to select clean sample for secondary - TRIGTOFQONETRACKSAM
//
//2023.02.19
//	1) store MC truth sample for TofQ eff
//	2) store MC truth sample for TkQ eff
//
//2023.02.20
//	1) for TkEff, always store the one loose InnerQ cut; update the Successive cut order, and keep the InnerQ cut as the last cut
//
//2023.02.22
//	1) store the L1PUEff and L9Eff with tighten sample for primary as well even without the TKEFFNOQCUT flag, and deprecate TKEFFNOQCUT entirely
//
//2023.04.14
//	1) store the L1Eff and L9Eff with additional ChisY cut, accounting for the L1L9ChisY cut
//	2) store the efficiency of FSChis and L1L9ChisY over MC truth sample (survivled up L9 and have matched TrMCCluster in L1 and L9), try different requirement on L1L9 XY or not
//
//2023.06.29
//	1) when storing EffSec, store the 1) fragment before L1, 2) fragment below L1 and 3) stay primary for sample and selection
//
//2023.07.10
//	1) specific study for Inner Q eff:
//		- L1Inner in FS geom, use to compare with L1Inner to correct the difference of geometry
//		- L1Inner in FS geom + unbiased L9Q cut
//-----------------
#define MYCAL
#ifdef MYCAL
//#include "ReadRigCalib.h"
#include "ReadRigCalib2.h" //2023.03.26
#else
#include "B800MCCor.C"
#endif

// #define SUCCESSIVE

// #define CHARGEDIS

// #define SELCHECKL9 ////need to debug
// #define SELCHECK

//------L1 efficiency
#define L9EFFWITHOUTTOFQ

//#define UNBIASEDL1BADLAD

//----
//#define EFFTIGHTSAM
//2020.10.12: not store eff tighten sample if store for the nearby nuclei of secondary
//#ifndef TKEFFNOQCUT
#if not defined TKEFFNOQCUT || defined EFFSEC //2020.11.05
#define EFFTIGHTSAM
#endif //TKEFFNOQCUT

const double dTS[4]={1,0.75,0.6,0.5};
#ifdef EFFTIGHTSAM
const int iNSam=3;
#else
const int iNSam=0;
#endif //EFFTIGHTSAM

#define EFFMCTRUTH
/*//2020.11.04
#ifndef EFFSEC
#define EFFMCTRUTH
#endif //EFFSEC*/
//charge distribution for sample selected using MCTruth //2023.05.08
#define EFFMCTRUTHQDIS

//#define TKEFFSAMSUCC

//2020.11.14
//#define TKQEFFCHECK

//2021.01.17
#define TRIGTOFQONETRACKSAM
//------
/*//2020.01.26: defined in drawvariable.h
static const int Mimr=2; //max rigidity estimator used*/

//TkEff
//static bool bTRDTOFPar=false, bL1XY=false, bL1Z=false;
static bool bTRDTOFPar=false, bL1XY=false, bRefQ=false; //2023.02.20: rename bL1Z to more meaningful one
static bool bBZSel=false;
static bool bGeometry[3]={false, false, false};
static bool bL1Q=false, bL9Q=false, bExtQ[3]={false, false, false}, bTofQ[3]={false, false, false};
static bool bMethodCut[Mimr+1];
static bool bMethodCutEcalOld=false;
static bool bToFNHits[3]={false, false, false}, bToFChis=false;

//--L1Q
static bool l1Q=false, l1Qs=false;

//--L9Q
static bool l9xy=false, l9Q=false;

static double rigidity[Mimr+1] = {0.};


#ifdef SUCCESSIVE
static bool bTkSucc[nSuccTk];
#endif //SUCCESSIVE

//------Trigger efficiency
#ifdef AMSSOFT
bool BookHistoTrigEff(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false) //2020.02.06
//bool BookHistoTrigEff(HistoMan &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoTrigEff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false) //2020.02.06
//bool BookHistoTrigEff(TObjArray &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{
	#ifdef SELCHECK
	return true;
	#endif ///SELCHECK

	string sDir=Form("Trigger_Efficiency%d", QVer);
	//if (dir) outfile.mkdir(sDir.c_str());
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		cout << "*it=" << *it << endl; //XXX
		//if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		//2020.10.25
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		if (dir)
		{
			outfile.mkdir(Form("%s/q%d", sDir.c_str(), *it));
			outfile.cd(Form("%s/q%d", sDir.c_str(), *it));
		}
		for (int ir=0; ir<3; ir++)
		{
			for (int irig=0; irig<NRig; irig++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				
				string sRig=sRigName[irig];
				
				//--
				if (dir)
				{
					outfile.mkdir(Form("%s/q%d/%s%s", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str()));
					outfile.cd(Form("%s/q%d/%s%s", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str()));
				}
				
				//two trigger period together
				for (int ipatt=0; ipatt<3; ipatt++)
				{
					if (ipatt==0) histn1 = "trig";
					else if (ipatt==1) histn1 = "unbiased";
					else if (ipatt==2) histn1 = "unbiasedOld";
					
					//--event counts
					for (int igr=1; igr>=0; igr--)
					{
						if (igr==0 && ev.isreal) continue;
						
						//--
						histn = Form("rig_r%d_q%d_%s", ir, *it, histn1.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
						//2023.06.29
						#ifdef EFFSEC
						for (int ifrac=0; ifrac<4; ifrac++)
						{
							if (ev.isreal) break;
							
							histn = Form("rig_r%d_q%d_%s", ir, *it, histn1.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sEffSecMCTruth[ifrac];
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						} //ifrac
						#endif //EFFSEC
						
						//2021.01.17
						#ifdef TRIGTOFQONETRACKSAM
						histn = Form("rig_r%d_q%d_%s_ntrack1", ir, *it, histn1.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						#endif //TRIGTOFQONETRACKSAM
					} //igr
				} //ipatt
				
				//------DAQ efficiency
				for (int iDaqP=0; iDaqP<2; iDaqP++)
				{
					for (int igr=1; igr>=0; igr--)
					{
						if (igr==0 && ev.isreal) continue;
						
						histn = Form("daqlenvsRig_r%d_q%d_daqp%d", ir, *it, iDaqP);
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 40000));
						
						histn = Form("daqsdlenvsRig_r%d_q%d_daqp%d", ir, *it, iDaqP);
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 40000));
						
						histn = Form("daqsdlenuvsRig_r%d_q%d_daqp%d", ir, *it, iDaqP);
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 40000));
						
						histn = Form("daqjlensumvsRig_r%d_q%d_daqp%d", ir, *it, iDaqP);
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 40000));
						
						for (int iJinJ=1; iJinJ<3; iJinJ++)
						{
							histn = Form("daqlJinJ%dvsRig_r%d_q%d_daqp%d", iJinJ, ir, *it, iDaqP);
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 400, 0, 40000));
						} //iJinJ
					} //igr
					
					if (iDaqP!=1) continue;
					histn = Form("JinJ1vsJinJ2_r%d_q%d_daqp%d", ir, *it, iDaqP);
					histn += sRig;
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 400, 0, 40000, 400, 0, 40000));
				} //iDaqP
				
				//------store two trigger period separetely //190611
				for (int iperiod=0; iperiod<2; iperiod++)
				{
					if (dir)
					{
						outfile.mkdir(Form("%s/q%d/%s%s/period%d", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str(), iperiod));
						outfile.cd(Form("%s/q%d/%s%s/period%d", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str(), iperiod));
					}
					
					for (int ipatt=0; ipatt<3; ipatt++)
					{
						if (ipatt==0) histn1 = "trig";
						else if (ipatt==1) histn1 = "unbiased";
						else if (ipatt==2) histn1 = "unbiasedOld";
					
						for (int igr=1; igr>=0; igr--)
						{
							if (igr==0 && ev.isreal) continue;
							 
							//--event counts
							histn = Form("rig_r%d_q%d_p%d_%s", ir, *it, iperiod, histn1.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//2021.01.17
							#ifdef TRIGTOFQONETRACKSAM
							histn = Form("rig_r%d_q%d_p%d_%s_ntrack1", ir, *it, iperiod, histn1.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							#endif //TRIGTOFQONETRACKSAM
						} //igr
						
						//--acc counts
						histn = Form("Nacc_r%d_q%d_p%d_%s", ir, *it, iperiod, histn1.c_str());
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
						
						histn = Form("Nacc_r%d_q%d_p%d_%s_jmembpattb8", ir, *it, iperiod, histn1.c_str());
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
						
						histn = Form("Nacc_r%d_q%d_p%d_%s_jmembpattb9", ir, *it, iperiod, histn1.c_str());
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
						
						histn = Form("Nacc_r%d_q%d_p%d_%s_Physbpattb2", ir, *it, iperiod, histn1.c_str());
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
						
						histn = Form("Nacc_r%d_q%d_p%d_%s_Physbpattb2Only", ir, *it, iperiod, histn1.c_str());
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
						
						histn = Form("Nacc_r%d_q%d_p%d_%s_jmembpattb9&&notother", ir, *it, iperiod, histn1.c_str());
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
					} //ipatt
					
					//--store trig & unbiased events with specified NACC1
					for (int inacc=1; inacc<8+1; inacc++)
					{
						if (inacc!=5 && inacc!=8) continue;
						for (int ipatt=0; ipatt<3; ipatt++)
						{
							if (ipatt==0) histn1 = "trig";
							else if (ipatt==1) histn1 = "unbiased";
							else if (ipatt==2) histn1 = "unbiasedOld";
							
							for (int igr=1; igr>=0; igr--)
							{
								if (igr==0 && ev.isreal) continue;
								
								//--
								histn = Form("rig_r%d_q%d_p%d_nacc%d_%s", ir, *it, iperiod, inacc, histn1.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								//2021.01.17
								#ifdef TRIGTOFQONETRACKSAM
								histn = Form("rig_r%d_q%d_p%d_nacc%d_%s_ntrack1", ir, *it, iperiod, inacc, histn1.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								#endif //TRIGTOFQONETRACKSAM
							} //igr
							
							//--acc counts
							histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s", ir, *it, iperiod, inacc, histn1.c_str());
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
							
							histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_jmembpattb8", ir, *it, iperiod, inacc, histn1.c_str());
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
							
							histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_jmembpattb9", ir, *it, iperiod, inacc, histn1.c_str());
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
							
							histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_Physbpattb2", ir, *it, iperiod, inacc, histn1.c_str());
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
							
							histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_Physbpattb2Only", ir, *it, iperiod, inacc, histn1.c_str());
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
							
							histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_jmembpattb9&&notother", ir, *it, iperiod, inacc, histn1.c_str());
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9, -0.5, 8.5));
						}
					} //inacc
					
				} //iperiod
			} //irig
		} //ir
	} //it
	return true;
} //BookHistoTrigEff

#ifdef AMSSOFT
bool SelTrigEff(HistoMan &hman1, SelEvent &ev, const int ir, const int ich, const int QVer, const double weight)
#else
bool SelTrigEff(TObjArray &hman1, SelEvent &ev, const int ir, const int irig, const int ich, const int QVer, const double weight)
//bool SelTrigEff(TObjArray &hman1, SelEvent &ev, const int ir, const int ich, const int QVer, const double weight)
#endif //AMSSOFT
{
	#ifdef SELCHECK
	return true;
	#endif ///SELCHECK

	string sRig=sRigName[irig]; //2023.03.22
	
	int nacc=0;
	for (int ibit=0; ibit<8; ibit++) if ((ev.antipatt&(1<<ibit))>0) nacc++;
	int physbpatt=(ev.isreal)?ev.physbpatt2:ev.physbpatt1;
	int jmembpatt=(ev.isreal)?ev.jmembpatt2:ev.jmembpatt1;
	
	/*int Period=(ev.run<1456507386)?0:1;
	if (!ev.isreal) Period=(ev.time[0]<1454843847)?0:1;*/
	//2020.08.30
	int Period=ev.TriggerPeriod();
	
	for (int ipatt=0; ipatt<3; ipatt++)
	{
		bool bTrigPatt=true;
		if (ipatt==0) {histn1 = "trig"; bTrigPatt=ev.Select_Trigger();}
		else if (ipatt==1) {histn1 = "unbiased"; bTrigPatt=ev.Select_TrigUnb2();}
		else if (ipatt==2) {histn1 = "unbiasedOld"; bTrigPatt=ev.Select_Trigger(1);}
		
		if (!bTrigPatt) continue;
		for (int igr=1; igr>=0; igr--)
		{
			if (igr==0 && ev.isreal) continue;
			double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(ir, irig);
			for (int ip0=0; ip0<2; ip0++) //ip0=1: different time period
			{
				histn = Form("rig_r%d_q%d", ir, ich);
				if (ip0==1) histn += Form("_p%d", Period);
				histn += Form("_%s", histn1.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig;
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
				//2021.01.17
				#ifdef TRIGTOFQONETRACKSAM
				if (ev.ntrack==1)
				{
					histn = Form("rig_r%d_q%d", ir, ich);
					if (ip0==1) histn += Form("_p%d", Period);
					histn += Form("_%s_ntrack1", histn1.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig;
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				#endif //TRIGTOFQONETRACKSAM
				
				//2023.06.29
				#ifdef EFFSEC
				if (!ev.isreal && ip0==1)
				{
					int ifrac = -1;
					if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
					else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
					else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
					else ifrac=3;
					
					histn = Form("rig_r%d_q%d_%s", ir, *it, histn1.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig;
					histn += sEffSecMCTruth[ifrac];
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				#endif //EFFSEC
			} //ip0
		} //igr
		
		//--acc counts
		histn = Form("Nacc_r%d_q%d_p%d_%s", ir, ich, Period, histn1.c_str());
		histn += sRig;
		GetHist(hman1, histn.c_str())->Fill(nacc, weight);
		
		histn = Form("Nacc_r%d_q%d_p%d_%s_jmembpattb8", ir, ich, Period, histn1.c_str());
		histn += sRig;
		if ((jmembpatt&(1<<8))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
		
		histn = Form("Nacc_r%d_q%d_p%d_%s_jmembpattb9", ir, ich, Period, histn1.c_str());
		histn += sRig;
		if ((jmembpatt&(1<<9))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
		
		histn = Form("Nacc_r%d_q%d_p%d_%s_Physbpattb2", ir, ich, Period, histn1.c_str());
		histn += sRig;
		if ((physbpatt&(1<<2))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
		
		histn = Form("Nacc_r%d_q%d_p%d_%s_Physbpattb2Only", ir, ich, Period, histn1.c_str());
		histn += sRig;
		if ((physbpatt&(1<<2))==(1<<2)) GetHist(hman1, histn.c_str())->Fill(nacc, weight); //store the NACC distribution with only physbpatt flag
		
		histn = Form("Nacc_r%d_q%d_p%d_%s_jmembpattb9&&notother", ir, ich, Period, histn1.c_str());
		histn += sRig;
		if ((physbpatt&0x3AL)==0 && (jmembpatt&(1<<9))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
	} //ipatt

	//--different trigger setting for different time period
	bool bTrigPattSel[3];
	int PhysBPattRebuild=0;
	for (int inacc=1; inacc<8+1; inacc++)
	{
		if (inacc!=5 && inacc!=8) continue;
		
		PhysBPattRebuild=physbpatt;
		if ((jmembpatt&0x200L)!=0 && nacc<inacc) PhysBPattRebuild=PhysBPattRebuild|(1<<2);
		else PhysBPattRebuild=PhysBPattRebuild&(~(1<<2));
		bTrigPattSel[0] = ((PhysBPattRebuild&0x3EL)!=0);
		/*bTrigPattSel[1] = (PhysBPattRebuild==1);
		bTrigPattSel[2] = ((PhysBPattRebuild&~0x3AL)!=0);*/
		bTrigPattSel[1] = !bTrigPattSel[0] && ((PhysBPattRebuild&1)==1);
		bTrigPattSel[2] = (PhysBPattRebuild==1);
		
		for (int ipatt=0; ipatt<3; ipatt++)
		{
			if (ipatt==0) histn1 = "trig";
			else if (ipatt==1) histn1 = "unbiased";
			else if (ipatt==2) histn1 = "unbiasedOld";
			
			if (!bTrigPattSel[ipatt]) continue;
			for (int igr=1; igr>=0; igr--)
			{
				if (igr==0 && ev.isreal) continue;
				double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(ir, irig);
				
				histn = Form("rig_r%d_q%d_p%d_nacc%d_%s", ir, ich, Period, inacc, histn1.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig;
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
			
				//2021.01.17
				#ifdef TRIGTOFQONETRACKSAM
				if (ev.ntrack==1)
				{
					histn = Form("rig_r%d_q%d_p%d_nacc%d_%s_ntrack1", ir, ich, Period, inacc, histn1.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig;
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				#endif //TRIGTOFQONETRACKSAM
			} //igr
			
			//--acc counts
			histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s", ir, ich, Period, inacc, histn1.c_str());
			histn += sRig;
			GetHist(hman1, histn.c_str())->Fill(nacc, weight);
			
			histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_jmembpattb8", ir, ich, Period, inacc, histn1.c_str());
			histn += sRig;
			if ((jmembpatt&(1<<8))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
			
			histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_jmembpattb9", ir, ich, Period, inacc, histn1.c_str());
			histn += sRig;
			if ((jmembpatt&(1<<9))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
			
			histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_Physbpattb2", ir, ich, Period, inacc, histn1.c_str());
			histn += sRig;
			if ((physbpatt&(1<<2))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
			
			histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_Physbpattb2Only", ir, ich, Period, inacc, histn1.c_str());
			histn += sRig;
			if ((physbpatt&(1<<2))==(1<<2)) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
			
			histn = Form("Nacc_r%d_q%d_p%d_nacc%d_%s_jmembpattb9&&notother", ir, ich, Period, inacc, histn1.c_str());
			histn += sRig;
			if ((physbpatt&0x3AL)==0 && (jmembpatt&(1<<9))>0) GetHist(hman1, histn.c_str())->Fill(nacc, weight);
		}
	} //inacc
	
	//------DAQ efficiency
	if (ev.Select_Trigger())
	{
		int DaqPeriod=(ev.run<1447346927)?0:1;
		
		for (int igr=1; igr>=0; igr--)
		{
			if (igr==0 && ev.isreal) continue;
			double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(ir, irig);
			
			histn = Form("daqlenvsRig_r%d_q%d_daqp%d", ir, ich, DaqPeriod);
			GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.daqlen, weight);
			
			histn = Form("daqsdlenvsRig_r%d_q%d_daqp%d", ir, ich, DaqPeriod);
			GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.daqsdlen, weight);
			
			histn = Form("daqsdlenuvsRig_r%d_q%d_daqp%d", ir, ich, DaqPeriod);
			GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.daqsdlenu, weight);
			
			histn = Form("daqjlensumvsRig_r%d_q%d_daqp%d", ir, ich, DaqPeriod);
			GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetDaqJinJLength(0), weight);
			
			for (int iJinJ=1; iJinJ<3; iJinJ++)
			{
				histn = Form("daqlJinJ%dvsRig_r%d_q%d_daqp%d", iJinJ, ir, ich, DaqPeriod);
				GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetDaqJinJLength(iJinJ), weight);
			} //iJinJ
		} //igr
		
		if (DaqPeriod==1)
		{
			histn = Form("JinJ1vsJinJ2_r%d_q%d_daqp%d", ir, ich, DaqPeriod);
			GetHist2d(hman1, histn.c_str())->Fill(ev.GetDaqJinJLength(2), ev.GetDaqJinJLength(1), weight);
		}
	}
	return true;
} //SelTrigEff

//------Tof charge efficiency
#ifdef AMSSOFT
bool BookHistoTofQEff(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false) //2020.02.06
//bool BookHistoTofQEff(HistoMan &hman1, const bool bIsreal, TFile &outfile, const int QVer, const bool dir=false)
#else
//bool BookHistoTofQEff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int irig, const int QVer, const bool dir=false) //2023.02.19
bool BookHistoTofQEff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false) //2020.02.06
//bool BookHistoTofQEff(TObjArray &hman1, const bool bIsreal, TFile &outfile, const int QVer, const bool dir=false)
#endif //AMSSOFT
{
	string sDir=Form("TofQ_Efficiency%d", QVer);
	//if (dir) outfile.mkdir(sDir.c_str());
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		//if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		//2020.10.25
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		for (int isamt=-1; isamt<iNSam; isamt++) //2019.10.16
		{
			#ifdef SELCHECK
			if (isamt!=-1) continue;
			#endif ///SELCHECK

			string sDirQ;
			if (isamt==-1) sDirQ=Form("%s/q%d", sDir.c_str(), *it);
			else  sDirQ=Form("%s/q%d_samt%d", sDir.c_str(), *it, isamt);
			/*string sEff="TofQ";
			if (isamt!=-1) sEff=Form("TofQ%d", isamt);*/
			string sEff="TofQEff";
			if (isamt!=-1) sEff=Form("TofQEff%d", isamt);
			
			if (dir)
			{
				outfile.mkdir(Form("%s", sDirQ.c_str()));
				outfile.cd(Form("%s", sDirQ.c_str()));
			}
			//for (int ir=1; ir<3; ir++)
			for (int ir=0; ir<3; ir++)
			{
				for (int irig=0; irig<NRig; irig++) //2023.03.11
				{
					#ifdef SELCHECK
					if(ir == 0 || irig != 0) continue;
					#endif ///SELCHECK

					#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
					if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
					#endif //PASS7GBL
					
					string sRig=sRigName[irig];
					
					//--
					if (dir)
					{
						outfile.mkdir(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
						outfile.mkdir(Form("%s/%s%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), 0));
						outfile.mkdir(Form("%s/%s%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), 1));
						outfile.cd(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
					}
			
					for (int itof=0; itof<3; itof++)
					{
						//if (ir==1 && itof>0) continue;
						if (ir<=1 && itof>0) continue; //2021.03.22
						string sEffTof = cTofEff[itof]+sEff;
						
						for (int igr=1; igr>=0; igr--)
						{
							if (igr==0 && ev.isreal) continue;
							histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//2023.06.29
							#ifdef EFFSEC
							for (int ifrac=0; ifrac<4; ifrac++)
							{
								if (ev.isreal) break;
								
								for (int isamsel=0; isamsel<2; isamsel++)
								{
									histn = Form("rig_r%d_q%d_%s", ir, *it, sEffTof.c_str());
									histn += (isamsel==0)?"Sam":"Sel";
									if (igr==0) histn += "_gen";
									histn += sRig;
									histn += sEffSecMCTruth[ifrac];
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								} //isamsel
							} //ifrac
							#endif //EFFSEC
							
							//2021.01.17
							#ifdef TRIGTOFQONETRACKSAM
							histn = Form("rig_r%d_q%d_%sSam_ntrack1", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sSel_ntrack1", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							#endif //TRIGTOFQONETRACKSAM
							
							#ifdef SUCCESSIVE
							for (int is=0; is<nSuccTofQ[itof]; is++)
							{
								histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEffTof.c_str(), is);
								if (igr==0) histn += "_gen";
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //is
							#endif //SUCCESSIVE

							#ifdef SELCHECK //// 2023.09.19
							if(igr == 1){
								for(int iqd = 0; iqd < 2; iqd ++){
									if(dir){
										outfile.cd(Form("%s/%s%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), iqd));
									}

									histn = Form("UTofQvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffTof.c_str(), iqd);
									histn += sRig;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 3500, 0, 35));
								}
								if(dir){
									outfile.cd(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
								}
							}
							#endif //SELCHECK
						} //igr
						
						//2023.02.19
						#ifdef EFFMCTRUTH
						for (int igr=1; igr>=0; igr--)
						{
							if (!(!ev.isreal && *it==(int)ev.mch)) break;
							
							if (dir) outfile.cd(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
							
							//------cleaness of the selected sample
							histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
							//------sample seletec by MC truth
							if (isamt!=-1) continue; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
							for (int isample=0; isample<2; isample++) //isample=0: only with MC truth cut, =1: normal sample cut+MC truth cut
							{
								string sMCTruthSam = (isample==0)?"":"_SamCut";
								histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, *it, sEffTof.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sMCTruthSam; //2023.05.08
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								#ifdef EFFMCTRUTHQDIS //2023.05.08
								for (int iud=0; iud<2; iud++)
								{
									histn = Form("%sTOFQvsrig_r%d_q%d_%sMCTruthSam", (iud==0)?"U":"L", ir, *it, sEffTof.c_str());
									if (igr==0) histn += "_gen";
									histn += sRig;
									histn += sMCTruthSam;
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
								} //iud
								#endif //EFFMCTRUTHQDIS
								
								histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, *it, sEffTof.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sMCTruthSam; //2023.05.08
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								#ifdef SUCCESSIVE
								for (int is=0; is<nSuccTofQ[itof]; is++)
								{
									histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", ir, *it, sEffTof.c_str(), is);
									if (igr==0) histn += "_gen";
									histn += sRig;
									histn += sMCTruthSam; //2023.05.08
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								} //is
								#endif //SUCCESSIVE
							} //isample
						} //igr
						#endif //EFFMCTRUTH
						
						//UTOFQ eff with LTofQ cut for L1Inner only //2020.07.09
						//if (ir!=1) continue;
						if (ir>1) continue; //2021.03.22
						for (int igr=1; igr>=0; igr--)
						{
							if (igr==0 && ev.isreal) continue;
							
							histn = Form("rig_r%d_q%d_%sSam_LTOFQCut", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sSel_LTOFQCut", ir, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							#ifdef SUCCESSIVE
							for (int is=0; is<nSuccTofQ[itof]; is++)
							{
								histn = Form("rig_r%d_q%d_%sSel_LTOFQCut_s%d", ir, *it, sEffTof.c_str(), is);
								if (igr==0) histn += "_gen";
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //is
							#endif //SUCCESSIVE
						}
					} //itof
				} //irig
			} //ir
		} //isamt
	} //it
	return true;
} //BookHistoTofQEff

#ifdef AMSSOFT
bool SelTofQEff(HistoMan &hman1, SelEvent &ev, const int span, const double weight=1., const int QVer=0)
//bool SelTofQEff(HistoMan &hman1, SelEvent &ev, bool bIsreal, const int span, const double weight=1., const int QVer=0)
#else
bool SelTofQEff(TObjArray &hman1, SelEvent &ev, const int span, const int irig, const double weight=1., const int QVer=0) //2023.02.19
//bool SelTofQEff(TObjArray &hman1, SelEvent &ev, const int span, const double weight=1., const int QVer=0)
//bool SelTofQEff(TObjArray &hman1, SelEvent &ev, bool bIsreal, const int span, const double weight=1., const int QVer=0)
#endif //AMSOFT
{
	#ifdef SELCHECK
	if(span == 0 || irig != 0) return true;
	#endif ///SELCHECK
	string sRig=sRigName[irig]; //2023.03.11
	
	bool bTofQEffSam=false;
	bool bTofQEffSel=false;
	bool bTofQEffSelSucc[2];
	
	bool bHZ=false;
	//if (span<1) return false;
	if (span<0) return false; //2021.03.22
	
	//2023.02.19
	int iSkip=ev.CheckPrimary();
	bool bSkip[3];
	bool bTruthFD[3]; //fiducial volume cut from generation plane
	//2023.03.05: use tighter requirement, up to L2 or L9
	//UTOF
	//bSkip[0]=iSkip>=6;
	//bSkip[0]=iSkip>=7;
	//bSkip[0]=ev.PrimaryUpToTrLayer(7) || ev.PrimaryUpToTrLayer(8); //stay up to whole Inner Tracker //2023.05.09
	bSkip[0]=ev.PrimaryUpToTrLayer(2); //stay up to L2, looser than used to selectd sample //2023.05.09
	bTruthFD[0]=ev.Select_GeomAccept(1); //2023.05.09
	//LTOF
	//bSkip[1]=iSkip>=17;
	//bSkip[1]=iSkip>=18;
	bSkip[1]=ev.PrimaryUpToTrLayer(9);
	bTruthFD[1]=ev.Select_GeomAccept(2); //2023.05.09
	//TTOF, i.e. same as LTOF
	//bSkip[2]=iSkip>=17;
	//bSkip[2]=iSkip>=18;
	bSkip[2]=ev.PrimaryUpToTrLayer(9);
	bTruthFD[2]=ev.Select_GeomAccept(2); //2023.05.09
	
	//--
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		//if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		//2020.10.25
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		//Inner Tracker Q as reference charge, first check whether the reference charge of each event is inside viQSel or not
		bHZ = (*it==int(ev.GetTkInQNew(2,QVer)+0.5));
		if (!bHZ) continue;
		
		for (int isamt=-1; isamt<iNSam; isamt++)
		{
			#ifdef SELCHECK
			if (isamt!=-1) continue;
			#endif ///SELCHECK

			double dTight=dTS[isamt+1];
			/*string sEff="TofQ";
			if (isamt!=-1) sEff=Form("TofQ%d", isamt);*/
			string sEff="TofQEff";
			if (isamt!=-1) sEff=Form("TofQEff%d", isamt);
			
			bTofQEffSam = ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, span, QVer, dTight); //different tightness
			#ifdef INNERUNBIAEDL1Q
			bool bUnbiasedL1XY=true;
			bool bUnbiasedL1Q=true;
			if (span==0)
			{
				bUnbiasedL1XY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
				bUnbiasedL1Q=(ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && (ev.GetUBExtQTrkCor(0, 2, QVer)<ev.GetQSelRange(1, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
				
				bTofQEffSam = bTofQEffSam && bUnbiasedL1XY && bUnbiasedL1Q;
			}
			#endif //INNERUNBIAEDL1Q
			
			if (bTofQEffSam)
			{
				for (int itof=0; itof<3; itof++)
				{
					//if (span==1 && itof>0) break;
					if (span<=1 && itof>0) break; //2021.03.22
					
					//--sample
					if (span==2)
					{
						/*if (itof==0) {if (!ev.Select_TofQ(*it, 3, QVer)) continue;}
						else if (itof==1) {if (!ev.Select_TofQ(*it, 1, QVer)) continue;}*/
						//2002.07.09
						if (itof==0) {if (!ev.Select_TofQ(*it, 3, QVer, dTight)) continue;}
						else if (itof==1) {if (!ev.Select_TofQ(*it, 1, QVer, dTight)) continue;}
					}
					
					string sEffTof = cTofEff[itof]+sEff;
					
					for (int igr=0; igr<2; igr++)
					{
						if (igr==0 && ev.isreal) continue;
						
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
						
						histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen"; //2023.03.11
						histn += sRig; //2023.03.11
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);

						#ifdef SELCHECK
						if(igr == 1){
							histn = Form("UTofQvsRigHZBin_r%d_q%d_%s%d", span, *it, sEffTof.c_str(), 0);
							histn += sRig; //2023.03.11
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(0), weight);
						}
						#endif ///SELCHECK
						
						//2021.01.17
						#ifdef TRIGTOFQONETRACKSAM
						histn = Form("rig_r%d_q%d_%sSam_ntrack1", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen"; //2023.03.11
						histn += sRig; //2023.03.11
						if (ev.ntrack==1) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						#endif //TRIGTOFQONETRACKSAM
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						}
						#endif //EFFSEC
					} //igr
					
					//--selection
					if (itof==0)
					{
						bTofQEffSel = ev.Select_TofQ(*it, 1, QVer);
						bTofQEffSelSucc[0]=ev.Select_TofQ(*it, 1, QVer);
					}
					else if (itof==1)
					{
						bTofQEffSel = ev.Select_TofQ(*it, 3, QVer);
						bTofQEffSelSucc[0]=ev.Select_TofQ(*it, 3, QVer);
					}
					else if (itof==2)
					{
						bTofQEffSel = ev.Select_TofQ(*it, 1, QVer) && ev.Select_TofQ(*it, 3, QVer);
						bTofQEffSelSucc[0]=ev.Select_TofQ(*it, 1, QVer);
						bTofQEffSelSucc[1]=ev.Select_TofQ(*it, 3, QVer);
					}
					
					for (int igr=0; igr<2; igr++)
					{
						if (igr==0 && ev.isreal) continue;
						
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
						
						histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen"; //2023.03.11
						histn += sRig; //2023.03.11
						if (bTofQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);

						#ifdef SELCHECK
						if(igr == 1 && bTofQEffSel){
							histn = Form("UTofQvsRigHZBin_r%d_q%d_%s%d", span, *it, sEffTof.c_str(), 1);
							histn += sRig; //2023.03.11
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(0), weight);
						}
						#endif ///SELCHECK
						
						//2021.01.17
						#ifdef TRIGTOFQONETRACKSAM
						histn = Form("rig_r%d_q%d_%sSel_ntrack1", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen"; //2023.03.11
						histn += sRig; //2023.03.11
						if (ev.ntrack==1 && bTofQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						#endif //TRIGTOFQONETRACKSAM
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						}
						#endif //EFFSEC
						
						#ifdef SUCCESSIVE
						for (int is=0; is<nSuccTofQ[itof]; is++)
						{
							if (!bTofQEffSelSucc[is]) break;
							histn = Form("rig_r%d_q%d_%sSel_s%d", span, *it, sEffTof.c_str(), is);
							if (igr==0) histn += "_gen"; //2023.03.11
							histn += sRig; //2023.03.11
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //is
						#endif //SUCCESSIVE
					} //igr
					
					//2023.02.19
					#ifdef EFFMCTRUTH
					//------cleaness of the selected sample
					for (int igr=0; igr<2; igr++)
					{
						if (!(!ev.isreal && *it==(int)ev.mch && bSkip[itof])) break;
						
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
						
						histn = Form("rig_r%d_q%d_%sSamCleanness", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.11
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //igr
					
					//------sample seletec by MC truth & normal sample cut, isample=1
					for (int igr=0; igr<2; igr++)
					{
						if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
						if (span<=1 && itof>0) break;
						
						//if (!bSkip[itof]) continue;
						//2023.05.09
						if (itof==0) {if (!(ev.PrimaryUpToTrLayer(7) || ev.PrimaryUpToTrLayer(8))) continue;}
						else if (itof>0) {if (!(ev.PrimaryUpToTrLayer(9))) continue;}
						
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
						string sMCTruthSam = "_SamCut";
						
						histn = Form("rig_r%d_q%d_%sMCTruthSam", span, *it, sEffTof.c_str());
						if (igr==1) histn += "_gen";
						histn += sRig; //2023.03.11
						histn += sMCTruthSam; //2023.05.08
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						#ifdef EFFMCTRUTHQDIS //2023.05.08
						for (int iud=0; iud<2; iud++)
						{
							histn = Form("%sTOFQvsrig_r%d_q%d_%sMCTruthSam", (iud==0)?"U":"L", span, *it, sEffTof.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sMCTruthSam; //2023.05.08
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(iud), weight);
						} //iud
						#endif //EFFMCTRUTHQDIS
						
						histn = Form("rig_r%d_q%d_%sMCTruthSel", span, *it, sEffTof.c_str());
						if (igr==1) histn += "_gen";
						histn += sRig; //2023.03.11
						histn += sMCTruthSam; //2023.05.08
						if (bTofQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						#ifdef SUCCESSIVE
						for (int is=0; is<nSuccTofQ[itof]; is++)
						{
							if (!bTofQEffSelSucc[is]) break;
							
							histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", span, *it, sEffTof.c_str(), is);
							if (igr==1) histn += "_gen";
							histn += sRig; //2023.03.11
							histn += sMCTruthSam; //2023.05.08
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //is
						#endif //SUCCESSIVE
					} //igr
					#endif //EFFMCTRUTH
					
					//------UTOFQ eff with LTofQ cut for L1Inner only //2020.07.09
					//if (span!=1) continue;
					if (span>1) continue;
					
					//ltofq cut
					if (!ev.Select_TofQ(*it, 3, QVer)) continue;
					
					for (int igr=0; igr<2; igr++)
					{
						if (igr==0 && ev.isreal) continue;
						
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
						
						histn = Form("rig_r%d_q%d_%sSam_LTOFQCut", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen"; //2023.03.11
						histn += sRig; //2023.03.11
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						histn = Form("rig_r%d_q%d_%sSel_LTOFQCut", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen"; //2023.03.11
						histn += sRig; //2023.03.11
						if (bTofQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						#ifdef SUCCESSIVE
						for (int is=0; is<nSuccTofQ[itof]; is++)
						{
							histn = Form("rig_r%d_q%d_%sSel_LTOFQCut_s%d", span, *it, sEffTof.c_str(), is);
							if (igr==0) histn += "_gen"; //2023.03.11
							histn += sRig; //2023.03.11
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //is
						#endif //SUCCESSIVE
					} //igr
				} //itof
			}
			
			//2023.02.19
			#ifdef EFFMCTRUTH
			//------sample seletec by MC truth only, isample=0
			for (int itof=0; itof<3; itof++)
			{
				if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
				if (span<=1 && itof>0) break;
				
				//if (!bSkip[itof]) continue;
				//if (!(bSkip[itof] && bTruthFD[itof])) continue; //apply fiducial volume cut as well //2023.05.09
				//2023.05.09
				if (itof==0) {if (!(ev.PrimaryUpToTrLayer(7) || ev.PrimaryUpToTrLayer(8))) continue;}
				else if (itof>0) {if (!(ev.PrimaryUpToTrLayer(9))) continue;}
				if (!bTruthFD[itof]) continue; //apply fiducial volume cut as well
				
				string sEffTof = cTofEff[itof]+sEff;
				for (int igr=0; igr<2; igr++)
				{
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
					
					histn = Form("rig_r%d_q%d_%sMCTruthSam", span, *it, sEffTof.c_str());
					if (igr==1) histn += "_gen";
					histn += sRig; //2023.03.11
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					#ifdef EFFMCTRUTHQDIS //2023.05.08
					for (int iud=0; iud<2; iud++)
					{
						histn = Form("%sTOFQvsrig_r%d_q%d_%sMCTruthSam", (iud==0)?"U":"L", span, *it, sEffTof.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(iud), weight);
					} //iud
					#endif //EFFMCTRUTHQDIS
					
					histn = Form("rig_r%d_q%d_%sMCTruthSel", span, *it, sEffTof.c_str());
					if (igr==1) histn += "_gen";
					histn += sRig; //2023.03.11
					if (bTofQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					#ifdef SUCCESSIVE
					for (int is=0; is<nSuccTofQ[itof]; is++)
					{
						if (!bTofQEffSelSucc[is]) break;
						
						histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", span, *it, sEffTof.c_str(), is);
						if (igr==1) histn += "_gen";
						histn += sRig; //2023.03.11
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //is
					#endif //SUCCESSIVE
				} //igr
			} //itof
			#endif //EFFMCTRUTH
		} //isamt
	} //it
	return bHZ;
} //SelTofQEff

//------L1 Efficiency (BZ & pick up)
#ifdef AMSSOFT
bool BookHistoL1Eff(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false) //2020.02.06
//bool BookHistoL1Eff(HistoMan &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoL1Eff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false, const int ichiscut=-1, const string sName0="") //use the const NRig, no need additional parameter //2023.03.11
//bool BookHistoL1Eff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false, const int NRig=1, const int ichiscut=-1, const string sName0="") //add new parameter to have suffix//2022.10.16
//bool BookHistoL1Eff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false, const int NRig=1) //decide how many rigidity to be stored //2022.10.03
//bool BookHistoL1Eff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false) //2020.02.06
//bool BookHistoL1Eff(TObjArray &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{
	string sDir = Form("L1_Efficiency%d", QVer);
	//2022.10.16
	string sName = sName0;
	if (ichiscut>=0) sName += Form("_icut%d", ichiscut);
	sDir += sName;
	
	for (int ir=1; ir<3; ir++)
	{
		for (it=viQSel.begin(); it!=viQSel.end(); it++)
		{
			//if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
			//2020.10.25
			#ifndef EFFSEC
			if (!ev.isreal && *it!=ev.mch) continue;
			#endif //EFFSEC
			
			iNbin = getNbin(*it);
			pBins = getBins(*it);
			
			for (int isamt=-1; isamt<iNSam; isamt++) //2019.10.17
			{
				#ifdef SELCHECK
				if (isamt!=-1) continue;
				#endif ///SELCHECK

				string sDirQ;

				if (isamt==-1) sDirQ=Form("%s/q%d", sDir.c_str(), *it);
				else  sDirQ=Form("%s/q%d_samt%d", sDir.c_str(), *it, isamt);
				//string sEff="L1";
				//if (isamt!=-1) sEff=Form("L1%d", isamt);
				
				//loop for both BZ and PU efficiency, change the naming convention //2019.10.25
				//for (int ieff=0; ieff<2; ieff++)
				for (int ieff=0; ieff<3; ieff++) //2023.02.22: ieff=2 for tighten L1PUEff sample for primary
				{
					if (ieff==2 && !(*it%2==0 && *it!=4)) continue; //only store two samples for primary
					
					for (int irig=0; irig<NRig; irig++) //2022.10.03
					{
						#ifdef SELCHECK
						if (irig!=0) continue;
						#endif ///SELCHECK

						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						string sEff=Form("L1%sEff", (ieff==0)?"BZ":"PU");
						if (isamt!=-1) sEff=Form("%s%d", sEff.c_str(), isamt);
						if (ieff==2) sEff += "TightSam"; //only change the name for Primary tighten sample //2023.02.22
						
						string sEffDir = sEff;
						//if (irig>0) sEffDir+=Form("_Rig%d", irig);
						
						string sRig=sRigName[irig];
						if (dir)
						{
							//outfile.mkdir(Form("%s/%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sEffDir.c_str()));
							//outfile.cd(Form("%s/%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sEffDir.c_str()));
							//2023.03.20
							outfile.mkdir(Form("%s/%s%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str()));
							outfile.cd(Form("%s/%s%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str()));
						}
						for (int igr=1; igr>=0; igr--)
						{
							if (igr==0 && ev.isreal) continue;
							histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//store the tighten Low Q Cut L1Eff, to judge tighten L1Q Cut for less BG //2023.07.17
							for (int itightLQCut=0; itightLQCut<3; itightLQCut++)
							{
								double dTightLQCut = 1.-(itightLQCut+1)*0.1;
								histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
								histn += Form("%.1fTightLQ", dTightLQCut);
								histn += "Sel";
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sName;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //itightLQCut
								
							//2023.06.29
							#ifdef EFFSEC
							for (int ifrac=0; ifrac<4; ifrac++)
							{
								if (ev.isreal) break;
								
								for (int isamsel=0; isamsel<2; isamsel++)
								{
									histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
									histn += (isamsel==0)?"Sam":"Sel";
									if (igr==0) histn += "_gen";
									histn += sRig;
									histn += sName;
									histn += sEffSecMCTruth[ifrac];
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								} //isamsel
							} //ifrac
							#endif //EFFSEC
							
							#ifdef SUCCESSIVE
							for (int is=0; is<((ieff==0)?nSuccL1BZ:nSuccL1PU[ir-1]); is++)
							{
								histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
								if (igr==0) histn += "_gen";
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //is
							#endif //SUCCESSIVE
						} //igr
						
						#ifdef CHARGEDIS
						int iNQsam=(ieff==0)?2+3:2;
						for (int isam=0; isam<iNQsam; isam++)
						{
							if (dir)
							{
								//outfile.mkdir(Form("%s/%s/%s/QD%d", sDirQ.c_str(), sSpan[ir].c_str(), sEffDir.c_str(), isam));
								//outfile.cd(Form("%s/%s/%s/QD%d", sDirQ.c_str(), sSpan[ir].c_str(), sEffDir.c_str(), isam));
								//2023.03.20
								outfile.mkdir(Form("%s/%s%s/%s/QD%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str(), isam));
								outfile.cd(Form("%s/%s%s/%s/QD%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str(), isam));
							}
							histn = Form("InnerQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
				
							histn = Form("UTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
				
							histn = Form("LTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
				
							histn = Form("L1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
				
							for (int ixy=0; ixy<3; ixy++)
							{
								histn = Form("unbiasedL1Q_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
						
								#ifdef UNBIASEDL1BADLAD
								//if (isam>1) continue;
								if (ieff>0 || isam>1) continue;
								histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_noBadLad", ir, *it, ixy, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
								histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_BadLad", ir, *it, ixy, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
								histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_normalLad", ir, *it, ixy, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
								#endif //UNBIASEDL1BADLAD
							} //ixy
							
							if (ieff==0) continue;
							histn = Form("L1InnerNormChisq_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 100));
				
							histn = Form("L1ChisY_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 100));
			
							histn = Form("L1InnerL9NormChisq_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 100));
						} //isam
						#endif //CHARGEDIS
						
						//2019.10.25
						#ifdef EFFMCTRUTH
						if (dir) outfile.cd(Form("%s/%s%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str()));
						
						for (int igr=1; igr>=0; igr--)
						{
							if (!(!ev.isreal && *it==(int)ev.mch)) break;
							
							//------cleaness of the selected sample
							histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
							//------sample seletec by MC truth
							//if (isamt==-1) //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
							if (!(isamt==-1 && ieff<2)) continue; //no need to store for tighten L1PUEff sample
							
							histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
			
							#ifdef SUCCESSIVE
							for (int is=0; is<((ieff==0)?nSuccL1BZ:nSuccL1PU[ir-1]); is++)
							{
								histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", ir, *it, sEff.c_str(), is);
								if (igr==0) histn += "_gen";
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							}
							#endif //SUCCESSIVE
						} //igr
						#endif //EFFMCTRUTH

						////2023.09.21
						#ifdef SELCHECK
						for (int isam=0; isam<3; isam++){
							if(dir){
								outfile.mkdir(Form("%s/%s%s/%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str(), isam));
								outfile.cd(Form("%s/%s%s/%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str(), isam));
							}
							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

							histn = Form("unbiasedL1Q_BetaCorvsTrkCor_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

							histn = Form("TKposxVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 500, -100, 100));

							histn = Form("TKposyVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 500, -100, 100));

							histn = Form("TKpos1sxVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 500, -100, 100));

							histn = Form("TKpos1syVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 500, -100, 100));

							histn = Form("TKpos2sxVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 500, -100, 100));

							histn = Form("TKpos2syVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 500, -100, 100));

							/// Rig_Bin
							histn = Form("unbiasedL1QvsL1Q_Rig2p67T3p64_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							histn = Form("unbiasedL1QvsL1Q_Rig14p1T15p3_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							histn = Form("unbiasedL1QvsL1Q_Rig48p5T56p1_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							histn = Form("unbiasedL1QvsL1Q_Rig175T211_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

							histn = Form("L1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

							////2023.10.07
							histn = Form("MCL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							////2023.10.07
							histn = Form("MCL1QvsunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							////2023.10.07
							histn = Form("InnerQvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							histn = Form("InnerQvsunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							
							//---L1Q limit above it+2
							histn = Form("L1QLimit_cooXvscooY_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 2000, -100, 100, 2000, -100, 100));

							histn = Form("L1QLimit_cooZ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 2000, -100, 100));
							histn = Form("L1QLimit_mom_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 1000));
							histn = Form("L1QLimit_Rig_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14)));
							histn = Form("L1QLimit_Rig_r%d_q%d_%s%dpri", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14)));

							for(int alay = 1; alay < 8; alay++){
								////2023.10.07
								histn = Form("L%dQvsunbiasedL1Q_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
								////2023.10.07
								histn = Form("L%dQvsL1Q_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							}
							for(int alay = 0; alay < 8; alay++){
								histn = Form("unbiasedL1QvsL1Q_L%dpri_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL2Q_L%dpri_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							}

							for(int ixy=0; ixy<3; ixy++){
								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("L1Q_oldQvsNewQ_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
							}

							if((isam == 1) && (ieff == 0)){		
								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_wQstatus", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woQstatus", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_wXYHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woXYHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woXHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woYHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_wL1q", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woL1q", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

								///********
								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_wQstatus", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woQstatus", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_wXYHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woXYHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woXHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woYHit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_wL1q", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woL1q", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));

								///********
							}

							if(ieff == 0) continue;
							
							histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

							histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

							histn = Form("L1InnerL9NormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

							if(isam == 1){
								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1Inner", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1InnerXYhit", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1InnerXYhitL1Charge", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_woChisqL1Inner", ir, *it, sEff.c_str(), isam);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

								if(ir == 1){
									histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1", ir, *it, sEff.c_str(), isam);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

									histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d_woChisqL1", ir, *it, sEff.c_str(), isam);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

									histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1InnerXYhitL1ChargeChisqL1", ir, *it, sEff.c_str(), isam);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));
								}
							}
						}
						#endif //SELCHECK
						
						//L1BZEff with different DAQ length //2020.08.05
						if (ieff!=0) continue;
						
						if (dir)
						{
							outfile.mkdir(Form("%s/%s%s/%s/daq", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str()));
							outfile.cd(Form("%s/%s%s/%s/daq", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), sEffDir.c_str()));
						}
						for (int igr=1; igr>=0; igr--)
						{
							if (igr==0 && ev.isreal) continue;
							
							for (int idaq=0; idaq<2; idaq++)
							{
								histn = Form("rig_r%d_q%d_daq%d_%sSam", ir, *it, idaq, sEff.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
		
								histn = Form("rig_r%d_q%d_daq%d_%sSel", ir, *it, idaq, sEff.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //idaq
							//2020.08.06
							for (int iNACC=0; iNACC<2; iNACC++)
							{
								histn = Form("rig_r%d_q%d_NACC%d_%sSam", ir, *it, iNACC, sEff.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								histn = Form("rig_r%d_q%d_NACC%d_%sSel", ir, *it, iNACC, sEff.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //iNACC
						} //igr
					} //irig
				} //ieff
			} //isamt
		} //it
	} //ir
	return true;
} //BookHistoL1Eff

#ifdef AMSSOFT
bool SelL1Eff(HistoMan &hman1, SelEvent &ev, const int ir, const double weight=1., const int QVer=0)
//bool SelL1Eff(HistoMan &hman1, SelEvent &ev, const int ir, bool bIsreal, const double weight=1., const int QVer=0)
#else
bool SelL1Eff(TObjArray &hman1, SelEvent &ev, const int ir, const int irig, const double weight=1., const int QVer=0, const int ichiscut=-1, const string sName0="") //2022.10.16
//bool SelL1Eff(TObjArray &hman1, SelEvent &ev, const int ir, const double weight=1., const int QVer=0, const int irig=0, const int ichiscut=-1, const string sName0="") //2022.10.16
//bool SelL1Eff(TObjArray &hman1, SelEvent &ev, const int ir, const double weight=1., const int QVer=0, const int irig=0) //2022.10.03
//bool SelL1Eff(TObjArray &hman1, SelEvent &ev, const int ir, const double weight=1., const int QVer=0)
//bool SelL1Eff(TObjArray &hman1, SelEvent &ev, const int ir, bool bIsreal, const double weight=1., const int QVer=0)
#endif //AMSOFT
{
	#ifdef SELCHECK
	if (irig!=0) return true;
	#endif ///SELCHECK

	//-- //2022.10.16
	string sName = sName0;
	if (ichiscut>=0) sName += Form("_icut%d", ichiscut);
	string sRig=sRigName[irig]; //2023.03.21
	
	//--
	int iRigSpan=(ir==1)?0:3;
	//--
	bool bL1Sam=false;
	bool bL1BZSel=false;
	bool bL1BZSelSucc[nSuccL1BZ];
	
	bool bL1PUSel=false;
	//bool bL1PUSelSucc[nSuccL1PU];
	bool bL1PUSelSucc[nSuccL1PU[0]];//2019.09.08
	
	bool bHZ=false;
	
	//2019.10.25
	int iSkip=ev.CheckPrimary();
	bool bSkip=true;
	//if (ir>0) bSkip=iSkip>=((ir==1)?13:18);
	if (ir>0) bSkip=iSkip>=((ir==1)?12:18); //2019.11.11
	else bSkip=true;
	
	//------
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		//if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		//2020.10.25
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		//Inner Tracker Q as reference charge, first check whether the reference charge of each event is inside viQSel or not
		bHZ = (*it==int(ev.GetTkInQNew(2,QVer)+0.5));
		if (!bHZ) continue;
		
		//--
		//2019.10.25: calculate the selection for numerator outside isamt loop which will be the same for all isamt
		//----BZ numerator
		bL1BZSel=ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0 && (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
		bL1BZSelSucc[0]=ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
		bL1BZSelSucc[1]=ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer);
		bL1BZSelSucc[2]=((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
	
		//----PU numerator
		//bL1PUSel=ev.GetChis(1, 1) < 10 && (ev.HasTkLHitXY(0)==2) && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
		//if (ir==1) bL1PUSel = bL1PUSel && ev.GetL1ChisY()<10;
		//2022.10.03: enable to decide what fitting algorithm used
		bL1PUSel=ev.GetChis(1, 1, irig) < 10 && (ev.HasTkLHitXY(0)==2) && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
		//if (ir==1) bL1PUSel = bL1PUSel && ev.GetL1ChisY(irig)<10;
		//2022.10.16
		if (ir==1)
		{
			if (ichiscut==-1) bL1PUSel = bL1PUSel && ev.GetL1ChisY(irig)<10;
			else if (ichiscut==1) bL1PUSel = bL1PUSel && ev.GetL1ChisY(irig)<10;
			else if (ichiscut==2) bL1PUSel = bL1PUSel && ev.GetL1ChisY(irig)<4;
		}
		
		bL1PUSelSucc[0]=ev.HasTkLHitXY(0)==2;
		//bL1PUSelSucc[1]=ev.GetChis(1, 1) < 10;
		bL1PUSelSucc[1]=ev.GetChis(1, 1, irig) < 10; //2023.03.27
		bL1PUSelSucc[2]=ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer);
		bL1PUSelSucc[3]=(ev.tk_qls[0]&0x10013D)==0;
		//if (ir==1) bL1PUSelSucc[4]=ev.GetL1ChisY()<10;
		//2022.10.16
		if (ir==1)
		{
			if (ichiscut==-1) bL1PUSelSucc[4] = ev.GetL1ChisY(irig)<10;
			else if (ichiscut==1) bL1PUSelSucc[4] = ev.GetL1ChisY(irig)<10;
			else if (ichiscut==2) bL1PUSelSucc[4] = ev.GetL1ChisY(irig)<4;
		}
		
		for (int isamt=-1; isamt<iNSam; isamt++)
		{
			#ifdef SELCHECK
			if (isamt!=-1) continue;
			#endif ///SELCHECK

			double dTight=dTS[isamt+1];
			//string sEff="L1";
			//if (isamt!=-1) sEff=Form("L1%d", isamt);
			//new naming convention //2019.10.25
			string sEff="L1BZEff";
			if (isamt!=-1) sEff=Form("%s%d", sEff.c_str(), isamt);
			//2022.10.04
			string sEffDir=sEff;
			if (irig>0) sEffDir+=Form("_Rig%d", irig);
			
			//charge selection for sample
			//bL1Sam = ev.Select_TofQ(*it, ir, QVer) && ev.Select_InnerQ(*it, QVer) && ev.Select_ExtQ(*it, iRigSpan, QVer);
			bL1Sam = ev.Select_TofQ(*it, ir, QVer, dTight) && ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, iRigSpan, QVer, dTight); //2019.10.17
			
			if (bL1Sam)
			{
				//--L1 BZ efficiency
				for (int igr=1; igr>=0; igr--)
				{
					if (igr==0 && ev.isreal) continue;
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
					
					//sample
					histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					//2023.06.29
					#ifdef EFFSEC
					if (!ev.isreal)
					{
						int ifrac = -1;
						if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
						else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
						else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
						else ifrac=3;
						
						histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						histn += sName;
						histn += sEffSecMCTruth[ifrac];
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //ifrac
					#endif //EFFSEC
					
					if (igr==1)
					{
						#ifdef CHARGEDIS
						histn = Form("InnerQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTkInQNew(2, QVer));
			
						histn = Form("UTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(0));
			
						histn = Form("LTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
			
						histn = Form("L1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer));
			
						for (int ixy=0; ixy<3; ixy++)
						{
							histn = Form("unbiasedL1Q_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, ixy, QVer));
						
							#ifdef UNBIASEDL1BADLAD
							histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_noBadLad", ir, *it, ixy, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							if (ev.tk_exlid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer));
						
							histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_BadLad", ir, *it, ixy, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							if (ev.tk_exlid[0]==+811) GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer));
						
							histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_normalLad", ir, *it, ixy, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							if (ev.tk_exlid[0]==-811) GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer));
							#endif //UNBIASEDL1BADLAD
						}
						#endif //CHARGEDIS

						////2023.09.21
						#ifdef SELCHECK
						histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

						histn = Form("unbiasedL1Q_BetaCorvsTrkCor_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetUBExtQBetaCor(0, 2, QVer), weight);

						histn = Form("TKposxVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.tk_pos[0][0], weight);

						histn = Form("TKposyVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.tk_pos[0][1], weight);

						histn = Form("TKpos1sxVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.tk_pos1s[0][0], weight);

						histn = Form("TKpos1syVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.tk_pos1s[0][1], weight);

						histn = Form("TKpos2sxVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.tk_pos2s[0][0], weight);

						histn = Form("TKpos2syVSunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.tk_pos2s[0][1], weight);

						histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

						histn = Form("L1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0, 2, QVer), weight);

						///Rig_Bin
						histn = Form("unbiasedL1QvsL1Q_Rig2p67T3p64_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						if(dRigStore > 2.67 && dRigStore < 3.64) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						histn = Form("unbiasedL1QvsL1Q_Rig14p1T15p3_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						if(dRigStore > 14.1 && dRigStore < 15.3) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						histn = Form("unbiasedL1QvsL1Q_Rig48p5T56p1_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						if(dRigStore > 48.5 && dRigStore < 56.1) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						histn = Form("unbiasedL1QvsL1Q_Rig175T211_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						if(dRigStore > 175 && dRigStore < 211) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

						for(int ixy=0; ixy<3; ixy++){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer), weight);
						}

						if(!ev.isreal){
							histn = Form("MCL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetTrMCLZ(1), weight);
							
							histn = Form("MCL1QvsunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTrMCLZ(1), weight);
							
							histn = Form("InnerQvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetTkInQNew(2, QVer), weight);
							
							histn = Form("InnerQvsunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTkInQNew(2, QVer), weight);

							for(int ixy=0; ixy<3; ixy++){
								histn = Form("L1Q_oldQvsNewQ_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, ixy, 4), ev.GetTkLQNew(0, ixy, 3), weight);
							}

							for(int alay = 1; alay < 8; alay++){
								histn = Form("L%dQvsL1Q_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetTkLQNew(alay, 2, QVer), weight);

								// if(abs(ev.GetTkLQNew(0, 2, QVer) - *it) > 1.2) continue;
								histn = Form("L%dQvsunbiasedL1Q_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTkLQNew(alay, 2, QVer), weight);
							}
							for(int alay = 0; alay < 8; alay++){
								histn = Form("unbiasedL1QvsL1Q_L%dpri_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if(ev.PrimaryUpToTrLayer(alay+1)) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

								histn = Form("unbiasedL1QvsL2Q_L%dpri_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTkLQNew(1, 2, QVer), weight);
							}

							if(ev.GetTkLQNew(0, 2, QVer) > *it + 2){
								histn = Form("L1QLimit_cooXvscooY_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.mtrcoo[0][0], ev.mtrcoo[0][1], weight);

								histn = Form("L1QLimit_cooZ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.mtrcoo[0][2], weight);
								histn = Form("L1QLimit_mom_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.mtrmom[0], weight);
								histn = Form("L1QLimit_Rig_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
								histn = Form("L1QLimit_Rig_r%d_q%d_%s%dpri", ir, *it, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if(ev.mtrpri[0]) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							}

							////2023.10.08
							// if(ev.GetTrMCLZ(8) == *it){
							if(ev.PrimaryUpToTrLayer(8)){
								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

								histn = Form("unbiasedL1Q_BetaCorvsTrkCor_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetUBExtQBetaCor(0, 2, QVer), weight);

								///Rig_Bin
								histn = Form("unbiasedL1QvsL1Q_Rig2p67T3p64_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if(dRigStore > 2.67 && dRigStore < 3.64) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
								histn = Form("unbiasedL1QvsL1Q_Rig14p1T15p3_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if(dRigStore > 14.1 && dRigStore < 15.3) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
								histn = Form("unbiasedL1QvsL1Q_Rig48p5T56p1_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if(dRigStore > 48.5 && dRigStore < 56.1) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
								histn = Form("unbiasedL1QvsL1Q_Rig175T211_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if(dRigStore > 175 && dRigStore < 211) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

								histn = Form("MCL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetTrMCLZ(1), weight);
								
								histn = Form("MCL1QvsunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTrMCLZ(1), weight);
								
								histn = Form("InnerQvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetTkInQNew(2, QVer), weight);
								
								histn = Form("InnerQvsunbiasedL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTkInQNew(2, QVer), weight);

								for(int ixy=0; ixy<3; ixy++){
									histn = Form("L1Q_oldQvsNewQ_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, ixy, 4), ev.GetTkLQNew(0, ixy, 3), weight);
								}

								for(int alay = 1; alay < 8; alay++){
									histn = Form("L%dQvsL1Q_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetTkLQNew(alay, 2, QVer), weight);

									// if(abs(ev.GetTkLQNew(0, 2, QVer) - *it) > 1.2) continue;
									histn = Form("L%dQvsunbiasedL1Q_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTkLQNew(alay, 2, QVer), weight);
								}
								if(ev.GetTkLQNew(0, 2, QVer) > *it + 2){
									histn = Form("L1QLimit_cooXvscooY_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist2d(hman1, histn.c_str())->Fill(ev.mtrcoo[0][0], ev.mtrcoo[0][1], weight);

									histn = Form("L1QLimit_cooZ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist(hman1, histn.c_str())->Fill(ev.mtrcoo[0][2], weight);

									histn = Form("L1QLimit_mom_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist(hman1, histn.c_str())->Fill(ev.mtrmom[0], weight);

									histn = Form("L1QLimit_Rig_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
									
									histn = Form("L1QLimit_Rig_r%d_q%d_%s%dpri", ir, *it, sEff.c_str(), 2);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									if(ev.mtrpri[0]) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
								}
							}
						}
						#endif //SELCHECK
					}
					
					//--numerator //2019.10.25: move outside isamt loop
					/*bL1BZSel=ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0 && (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
					bL1BZSelSucc[0]=ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
					bL1BZSelSucc[1]=ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer);
					bL1BZSelSucc[2]=((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);*/

					#ifdef SELCHECK
					bool bL1BZSel_wQs = ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
					bool bL1BZSel_woQs = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0 && (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer));
					bool bL1BZSel_wXY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
					bool bL1BZSel_woXY = (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
					bool bL1BZSel_woX = ev.GetUBExtQTrkCor(0, 1, QVer)>0 && (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
					bool bL1BZSel_woY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
					bool bL1BZSel_wL1q = (ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer));
					bool bL1BZSel_woL1q = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0 && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
					if(bL1BZSel_wQs){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_wQstatus", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_wQstatus", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_woQs){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woQstatus", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woQstatus", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_wXY){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_wXYHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_wXYHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_woXY){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woXYHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woXYHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_woX){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woXHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woXHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_woY){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woYHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woYHit", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_wL1q){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_wL1q", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_wL1q", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					if(bL1BZSel_woL1q){
						if(igr==1){
							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d_woL1q", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d_woL1q", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);
						}
					}
					#endif //SELCHECK
			
					if (bL1BZSel)
					{
						histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						//store the tighten Low Q Cut L1Eff, to judge tighten L1Q Cut for less BG //2023.07.17
						for (int itightLQCut=0; itightLQCut<3; itightLQCut++)
						{
							double dTightLQCut = 1.-(itightLQCut+1)*0.1;
							bool bTightLQCut = ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer, false, dTightLQCut);
							if (!bTightLQCut) continue;
							
							histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
							histn += Form("%.1fTightLQ", dTightLQCut);
							histn += "Sel";
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sName;
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //itightLQCut
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sName;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //ifrac
						#endif //EFFSEC
					
						if (igr==1)
						{
							#ifdef CHARGEDIS
							histn = Form("InnerQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkInQNew(2, QVer));
			
							histn = Form("UTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(0));
			
							histn = Form("LTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
			
							histn = Form("L1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer));
					
							for (int ixy=0; ixy<3; ixy++)
							{
								histn = Form("unbiasedL1Q_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, ixy, QVer));
							
								#ifdef UNBIASEDL1BADLAD
								histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_noBadLad", ir, *it, ixy, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if (ev.tk_exlid[0]!=+811) GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer));
					
								histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_BadLad", ir, *it, ixy, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if (ev.tk_exlid[0]==+811) GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer));
					
								histn = Form("unbiasedL1QvsRig_r%d_q%d_xy%d_%s%d_normalLad", ir, *it, ixy, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								if (ev.tk_exlid[0]==-811) GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer));
								#endif //UNBIASEDL1BADLAD
							} //ixy
							#endif //CHARGEDIS

							////2023.09.21
							#ifdef SELCHECK
							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("L1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0, 2, QVer), weight);

							for(int ixy=0; ixy<3; ixy++){
								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer), weight);
							}

							if(!ev.isreal){
								for(int alay = 0; alay < 8; alay++){
									histn = Form("unbiasedL1QvsL1Q_L%dpri_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 1);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									if(ev.mtrpri[alay]) GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

									histn = Form("unbiasedL1QvsL2Q_L%dpri_r%d_q%d_%s%d", alay+1, ir, *it, sEff.c_str(), 1);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist2d(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, 2, QVer), ev.GetTkLQNew(1, 2, QVer), weight);
								}
							}
							#endif //SELCHECK
						}
					}
					
					//L1BZEff with different DAQ length //2020.08.05
					int idaq=(ev.daqsdlenu<=24500)?0:1;
					
					histn = Form("rig_r%d_q%d_daq%d_%sSam", ir, *it, idaq, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					histn = Form("rig_r%d_q%d_daq%d_%sSel", ir, *it, idaq, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					if (bL1BZSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					//L1BZEff with different NACC //2020.08.06
					int nacc=0;
					for (int ibit=0; ibit<8; ibit++) if ((ev.antipatt&(1<<ibit))>0) nacc++;
					
					int iNACC=(nacc<5)?0:1;
					histn = Form("rig_r%d_q%d_NACC%d_%sSam", ir, *it, iNACC, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);

					histn = Form("rig_r%d_q%d_NACC%d_%sSel", ir, *it, iNACC, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					if (bL1BZSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					#ifdef SUCCESSIVE
					//--L1 BZ Efficiency
					for (int is=0; is<nSuccL1BZ; is++)
					{
						if (!bL1BZSelSucc[is]) break;
						histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
						if (igr==0) histn += "_gen";
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						//one more intermediate charge distribution: passed unbiased L1XY
						if (igr==0) continue;
						#ifdef CHARGEDIS
						histn = Form("InnerQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), is+2);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTkInQNew(2, QVer));
						
						histn = Form("UTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), is+2);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(0));
						
						histn = Form("LTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), is+2);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
						
						histn = Form("L1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), is+2);
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer));
						
						for (int ixy=0; ixy<3; ixy++)
						{
							histn = Form("unbiasedL1Q_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), is+2);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, ixy, QVer));
						} //ixy
						#endif //CHARGEDIS
					} //is
					#endif //SUCCESSIVE
				} //igr
					
				//2019.10.25
				#ifdef EFFMCTRUTH
				for (int igr=1; igr>=0; igr--)
				{
					if (!(!ev.isreal && *it==(int)ev.mch && bSkip)) break;
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
					
					//------cleaness of the selected sample
					histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				} //igr
				#endif //EFFMCTRUTH
				
				//--L1 pick up efficiency
				/*//2020.11.20: tighten selection for secondary, tighten upper tof ranges, require tof layer matchness and good tof path length
				bool bL1PUSam=true;
				#ifndef TKEFFNOQCUT //do the same for nearby charge if the flag is on
				if (*it%2==1)
				#endif //TKEFFNOQCUT
				{
					//tof charge
					bL1PUSam = bL1PUSam && ev.GetTOFUDQ(0) > (*it-dTight*0.5) && ev.GetTOFUDQ(0) < (*it+dTight*0.5); //upper tof q +/-0.5
					//tof layer Q to be compatiable for upper and lower tof
					bL1PUSam = bL1PUSam && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
					if (ir==2) bL1PUSam = bL1PUSam && (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(3))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
					//tof path length
					bL1PUSam = bL1PUSam && ev.IsGoodTofQUD(0);
					if (ir==2) bL1PUSam = bL1PUSam && ev.IsGoodTofQUD(1);
					
					//--LTofQ lower limit cut
					////tighten the lower tof Q for secondaries
					//if (*it%2==1) bL1PUSam = bL1PUSam && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*0.5);
					//else bL1PUSam = bL1PUSam && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*1.5);
				}*/
				
				//always store the tighten sample for Primary //2023.02.22
				bool bL1PUSam=true;
				//tof charge
				bL1PUSam = bL1PUSam && ev.GetTOFUDQ(0) > (*it-dTight*0.5) && ev.GetTOFUDQ(0) < (*it+dTight*0.5); //upper tof q +/-0.5
				//tof layer Q to be compatiable for upper and lower tof
				bL1PUSam = bL1PUSam && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
				if (ir==2) bL1PUSam = bL1PUSam && (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(3))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
				//tof path length
				bL1PUSam = bL1PUSam && ev.IsGoodTofQUD(0);
				if (ir==2) bL1PUSam = bL1PUSam && ev.IsGoodTofQUD(1);
				
				//--LTofQ lower limit cut
				////tighten the lower tof Q for secondaries
				//if (*it%2==1) bL1PUSam = bL1PUSam && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*0.5);
				//else bL1PUSam = bL1PUSam && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*1.5);
				
				for (int itightsam=0; itightsam<2; itightsam++)
				{
					if (itightsam==0 && !(*it%2==0 && *it!=4)) continue; //only store two samples for primary
					//if (itightsam==1 && !bL1PUSam) continue; //apply tighter sample cut
					if ((itightsam==1 || (*it%2==1 || *it==4)) && !bL1PUSam) continue; //apply tighter sample cut, also for secondary //2023.07.02
					
					//new naming convention //2019.10.25
					sEff="L1PUEff";
					if (isamt!=-1) sEff=Form("%s%d", sEff.c_str(), isamt);
					if (*it%2==0 && *it!=4 && itightsam==1) sEff += "TightSam"; //only change the name for Primary tighten sample //2023.02.22
					
					//--
					//if (bL1BZSel && ev.GetUBExtQTrkCor(0, 2, QVer)<*it+0.5) //with unbiased L1Q upper limit
					//if (bL1BZSel && bL1PUSam && ev.GetUBExtQTrkCor(0, 2, QVer)<*it+0.5) //with unbiased L1Q upper limit //2020.11.20: add few more additioal cuts for L1PU sample
					if (!(bL1BZSel && ev.GetUBExtQTrkCor(0, 2, QVer)<*it+0.5)) continue; //with unbiased L1Q upper limit
					for (int igr=1; igr>=0; igr--)
					{
						if (igr==0 && ev.isreal) continue;
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
						
						//sample
						histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sName;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //ifrac
						#endif //EFFSEC
						
						if (igr==1)
						{
							#ifdef CHARGEDIS
							histn = Form("InnerQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkInQNew(2, QVer));
			
							histn = Form("UTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(0));
			
							histn = Form("LTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
			
							histn = Form("L1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer));
							
							for (int ixy=0; ixy<3; ixy++)
							{
								histn = Form("unbiasedL1Q_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, ixy, QVer));
							} //ixy
							
							
							histn = Form("L1InnerNormChisq_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							//GetHist(hman1, histn.c_str())->Fill(ev.GetChis(1,1));
							GetHist(hman1, histn.c_str())->Fill(ev.GetChis(1,1,irig)); //2023.03.27
							
							histn = Form("L1ChisY_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(ev.GetL1ChisY());
							
							histn = Form("L1InnerL9NormChisq_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							//GetHist(hman1, histn.c_str())->Fill(ev.GetChis(2,1));
							GetHist(hman1, histn.c_str())->Fill(ev.GetChis(2,1,irig)); //2023.03.27
							#endif //CHARGEDIS

							////2023.09.21
							#ifdef SELCHECK
							histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

							histn = Form("L1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0, 2, QVer), weight);

							for(int ixy=0; ixy<3; ixy++){
								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 0);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer), weight);
							}

							histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);

							histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetL1ChisY(), weight);

							histn = Form("L1InnerL9NormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 0);
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(2,1,irig), weight);
							#endif //SELCHECK
						}
						
						//--new charge //2019.10.25: move outside isamt loop
						/*//L1Chisq<10 as last successive for L1Inner
						bL1PUSel=ev.GetChis(1, 1) < 10 && (ev.HasTkLHitXY(0)==2) && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
						if (ir==1) bL1PUSel = bL1PUSel && ev.GetL1ChisY()<10;
						
						bL1PUSelSucc[0]=ev.HasTkLHitXY(0)==2;
						bL1PUSelSucc[1]=ev.GetChis(1, 1) < 10;
						bL1PUSelSucc[2]=ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer);
						bL1PUSelSucc[3]=(ev.tk_qls[0]&0x10013D)==0;
						if (ir==1) bL1PUSelSucc[4]=ev.GetL1ChisY()<10;*/
						
						if (bL1PUSel)
						{
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							
							//store the tighten Low Q Cut L1Eff, to judge tighten L1Q Cut for less BG //2023.07.17
							for (int itightLQCut=0; itightLQCut<3; itightLQCut++)
							{
								double dTightLQCut = 1.-(itightLQCut+1)*0.1;
								bool bTightLQCut = ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer, false, dTightLQCut);
								if (!bTightLQCut) continue;
								
								histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
								histn += Form("%.1fTightLQ", dTightLQCut);
								histn += "Sel";
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sName;
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							} //itightLQCut
							
							//2023.06.29
							#ifdef EFFSEC
							if (!ev.isreal)
							{
								int ifrac = -1;
								if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
								else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
								else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
								else ifrac=3;
								
								histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sName;
								histn += sEffSecMCTruth[ifrac];
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							} //ifrac
							#endif //EFFSEC
							
							if (igr==1)
							{
								#ifdef CHARGEDIS
								histn = Form("InnerQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetTkInQNew(2, QVer));
			
								histn = Form("UTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(0));
			
								histn = Form("LTofQ_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
			
								histn = Form("L1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer));
						
								for (int ixy=0; ixy<3; ixy++)
								{
									histn = Form("unbiasedL1Q_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 1);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(0, ixy, QVer));
								} //ixy
						
								histn = Form("L1InnerNormChisq_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								//GetHist(hman1, histn.c_str())->Fill(ev.GetChis(1,1));
								GetHist(hman1, histn.c_str())->Fill(ev.GetChis(1,1,irig)); //2023.03.27
						
								histn = Form("L1ChisY_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist(hman1, histn.c_str())->Fill(ev.GetL1ChisY());
						
								histn = Form("L1InnerL9NormChisq_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								//GetHist(hman1, histn.c_str())->Fill(ev.GetChis(2,1));
								GetHist(hman1, histn.c_str())->Fill(ev.GetChis(2,1,irig)); //2023.03.27
								#endif //CHARGEDIS

								////2023.09.21
								#ifdef SELCHECK
								histn = Form("unbiasedL1QvsL1Q_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(ev.GetTkLQNew(0, 2, QVer), ev.GetUBExtQTrkCor(0, 2, QVer), weight);

								histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), weight);

								histn = Form("L1QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0, 2, QVer), weight);

								for(int ixy=0; ixy<3; ixy++){
									histn = Form("unbiasedL1QvsRigHZBin_r%d_q%d_xy%d_%s%d", ir, *it, ixy, sEff.c_str(), 1);
									histn += sRig; //2022.10.03
									histn += sName; //2022.10.16
									GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, ixy, QVer), weight);
								}

								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);

								histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetL1ChisY(), weight);

								histn = Form("L1InnerL9NormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(2,1,irig), weight);
								#endif //SELCHECK
							}
						}
						#ifdef SELCHECK
						bool bL1PUSel_wChisqL1Inner = ev.GetChis(1, 1, irig) < 10;
						bool bL1PUSel_woChisqL1Inner = (ev.HasTkLHitXY(0)==2) && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
						bool bL1PUSel_wChisqL1Inner_2 = bL1PUSel_wChisqL1Inner && (ev.HasTkLHitXY(0)==2);
						bool bL1PUSel_wChisqL1Inner_3 = bL1PUSel_wChisqL1Inner_2 && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
						bool bL1PUSel_wChisqL1Inner_4 = bL1PUSel_wChisqL1Inner_3 && ev.GetL1ChisY(irig)<10;
						// bool bL1PUSel_wXYHit = (ev.HasTkLHitXY(0)==2);
						// bool bL1PUSel_woXYHit =ev.GetChis(1, 1, irig) < 10 && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
						bool bL1PUSel_wChisqL1 = ev.GetL1ChisY(irig)<10;
						bool bL1PUSel_woChisqL1 = ev.GetChis(1, 1, irig) < 10 && (ev.HasTkLHitXY(0)==2) && (ev.GetTkLQNew(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && ((ev.tk_qls[0]&0x10013D)==0);
						if(igr == 1){
							if(bL1PUSel_wChisqL1Inner){
								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1Inner", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);
							}
							if(bL1PUSel_wChisqL1Inner_2){
								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1InnerXYhit", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);
							}
							if(bL1PUSel_wChisqL1Inner_3){
								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1InnerXYhitL1Charge", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);
							}
							if(ir == 1 && bL1PUSel_wChisqL1Inner_4){
								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1InnerXYhitL1ChargeChisqL1", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);
							}
							if(bL1PUSel_woChisqL1Inner){
								histn = Form("L1InnerNormChisqvsRigHZBin_r%d_q%d_%s%d_woChisqL1Inner", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(1,1,irig), weight);
							}
							if(ir == 1 && bL1PUSel_wChisqL1){
								histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d_wChisqL1", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetL1ChisY(), weight);
							}
							if(ir == 1 && bL1PUSel_woChisqL1){
								histn = Form("L1ChisqvsRigHZBin_r%d_q%d_%s%d_woChisqL1", ir, *it, sEff.c_str(), 1);
								histn += sRig; //2022.10.03
								histn += sName; //2022.10.16
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetL1ChisY(), weight);
							}
						}
						#endif //SELCHECK
						
						#ifdef SUCCESSIVE
						//--L1 pick up Efficiency
						for (int is=0; is<nSuccL1PU[ir-1]; is++)
						{
							if (!bL1PUSelSucc[is]) break;
							histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
							if (igr==0) histn += "_gen";
							histn += sRig; //2022.10.03
							histn += sName; //2022.10.16
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //is
						#endif //SUCCESSIVE
					} //igr
					
					//2019.10.25
					#ifdef EFFMCTRUTH
					for (int igr=1; igr>=0; igr--)
					{
						//if (!ev.isreal && bSkip)
						//if (!ev.isreal && *it==(int)ev.mch && bSkip) //2020.11.09
						if (!(!ev.isreal && *it==(int)ev.mch && bSkip)) break;
						
						if (igr==0 && ev.isreal) continue;
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
							
						//------cleaness of the selected sample
						histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2022.10.03
						histn += sName; //2022.10.16
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //igr
					#endif //EFFMCTRUTH
				} //itightsam
			} //bL1Sam
			
			//------sample seletec by MC truth
			#ifdef EFFMCTRUTH
			for (int igr=1; igr>=0; igr--)
			{
				//if (!ev.isreal && isamt==-1)
				//if (!ev.isreal && *it==(int)ev.mch && isamt==-1) //2020.11.09
				if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break;
				if (!bSkip) break;
				
				if (igr==0 && ev.isreal) continue;
				double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
					
				//----L1 BZ efficiency
				sEff="L1BZEff";
				if (isamt!=-1) sEff=Form("%s%d", sEff.c_str(), isamt);
				
				histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, *it, sEff.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig; //2022.10.03
				histn += sName; //2022.10.16
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
				if (bL1BZSel)
				{
					histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, *it, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				
				#ifdef SUCCESSIVE
				for (int is=0; is<nSuccL1BZ; is++)
				{
					if (!bL1BZSelSucc[is]) break;
					histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", ir, *it, sEff.c_str(), is);
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				#endif //SUCCESSIVE
				
				//----L1 PU Efficiency
				sEff="L1PUEff";
				if (isamt!=-1) sEff=Form("%s%d", sEff.c_str(), isamt);
				if (!bL1BZSel) continue;
				histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, *it, sEff.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig; //2022.10.03
				histn += sName; //2022.10.16
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
				if (bL1PUSel)
				{
					histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, *it, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				
				#ifdef SUCCESSIVE
				for (int is=0; is<nSuccL1PU[ir-1]; is++)
				{
					if (!bL1PUSelSucc[is]) break;
					histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", ir, *it, sEff.c_str(), is);
					if (igr==0) histn += "_gen";
					histn += sRig; //2022.10.03
					histn += sName; //2022.10.16
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				}
				#endif //SUCCESSIVE
			} //igr
			#endif //EFFMCTRUTH
		} //isamt
	} //it
	return bHZ;
} //SelL1Eff

//------L9 Efficiency (BZ & pick up)
#ifdef AMSSOFT
bool BookHistoL9Eff(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false)
//bool BookHistoL9Eff(HistoMan &hman1, const bool bIsreal, TFile &outfile, const int QVer, const bool dir=false)
#else
bool BookHistoL9Eff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false)
//bool BookHistoL9Eff(TObjArray &hman1, const bool bIsreal, TFile &outfile, const int QVer, const bool dir=false)
#endif //AMSSOFT
{
	string sDir = Form("L9_Efficiency%d", QVer);
	
	for (int ir=2; ir<3; ir++)
	{
		for (int irig=0; irig<NRig; irig++)
		{
			#ifdef SELCHECK
			if (irig!=0) continue;
			#endif ///SELCHECK

			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			string sRig=sRigName[irig];
			
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				#ifndef EFFSEC
				if (!ev.isreal && *it!=ev.mch) continue;
				#endif //EFFSEC
				
				iNbin = getNbin(*it);
				pBins = getBins(*it);
				
				for (int isamt=-1; isamt<iNSam; isamt++)
				{
					#ifdef SELCHECK
					if (isamt!=-1) continue;
					#endif ///SELCHECK
					
					string sDirQ;
					if (isamt==-1) sDirQ=Form("%s/q%d", sDir.c_str(), *it);
					else sDirQ=Form("%s/q%d_samt%d", sDir.c_str(), *it, isamt);
					
					for (int itightsam=0; itightsam<2; itightsam++) //2023.02.22
					{
						if (itightsam==1 && !(*it%2==0 && *it!=4)) continue; //only store two samples for primary
						
						int iL9EffL=-1;
						int iL9EffVer=0;
						#ifdef L9EFFWITHOUTTOFQ
						iL9EffVer=2;
						#endif //L9EFFWITHOUTTOFQ
						
						for (int il9=iL9EffL; il9<iL9EffVer; il9++) //2023.06.29
						{
							string sEff="L9Eff";
							if (isamt!=-1) sEff+=Form("%d", isamt);
							if (itightsam==1) sEff += "TightSam"; //only change the name for Primary tighten sample //2023.02.22
							
							//2023.06.29
							string sL9EffVer0=(il9>=0)?"noLTofQ":"";
							string sL9EffCut = "";
							if (il9>0) sL9EffCut = "_wLTofQ";
							string sL9EffVer = sL9EffVer0+sL9EffCut;
							
							//--L9 Efficiency
							if (dir)
							{
								histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
								histn += sRig;
								if (itightsam==1) histn += "TightSam"; //only change the name for Primary tighten sample
								if (il9>=0) histn += "/noLTofQ"; //2023.06.29
								if (il9<=0) outfile.mkdir(Form("%s", histn.c_str()));
								outfile.cd(Form("%s", histn.c_str()));
							}
							
							histn = Form("LTofQvsRig_r%d_q%d_%s%sSam", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
							histn += sRig; //2023.03.22
							if (il9<=0) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							histn = Form("LTofQvsRig_r%d_q%d_%s%sSel", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
							histn += sL9EffCut;
							histn += sRig; //2023.03.22
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							for (int igr=1; igr>=0; igr--)
							{
								if (ev.isreal && igr==0) continue;
								histn = Form("rig_r%d_q%d_%s%sSam", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								if (il9<=0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins)); //only need sample for with LTofQ (il19=-1) and without LTofQ (il19=0 & 1)
								
								histn = Form("rig_r%d_q%d_%s%sSel", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
								histn += sL9EffCut;
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								//2023.06.29
								#ifdef EFFSEC
								for (int ifrac=0; ifrac<4; ifrac++)
								{
									if (ev.isreal) break;
									
									for (int isamsel=0; isamsel<2; isamsel++)
									{
										if (il9>0 && isamsel==0) continue;
										histn = Form("rig_r%d_q%d_%s%s", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
										histn += (isamsel==0)?"Sam":"Sel";
										histn += sL9EffCut;
										if (igr==0) histn += "_gen";
										histn += sRig;
										histn += sEffSecMCTruth[ifrac];
										hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
									} //isamsel
								} //ifrac
								#endif //EFFSEC
								
								#ifdef SUCCESSIVE
								//--L9 Efficiency
								for (int is=0; is<nSuccL9; is++)
								{
									if (il9>0) break;
									histn = Form("rig_r%d_q%d_%s%sSel%s_s%d", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), sL9EffCut.c_str(), is);
									if (igr==0) histn += "_gen";
									histn += sRig; //2023.03.22
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								} //is
								#endif //SUCCESSIVE
							} //igr
							
							#ifdef EFFMCTRUTH
							for (int igr=1; igr>=0; igr--)
							{
								if (!(!ev.isreal && *it==(int)ev.mch)) break;
								
								//------cleanness of the selected sample
								histn = Form("rig_r%d_q%d_%s%sSamCleanness", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								if (il9<=0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								//------Clean events which also passed numerator cut
								histn = Form("rig_r%d_q%d_%s%sSamCleanness&Sel", ir, *it, sEff.c_str(), sL9EffVer0.c_str());
								histn += sL9EffCut;
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //igr
							#endif //EFFMCTRUTH

							#ifdef SELCHECKL9
							string scDir;
							if(il9 == 1){
								if (dir)
								{
									histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
									histn += sRig;
									if (itightsam==1) histn += "TightSam"; //only change the name for Primary tighten sample
									if (il9>=0) histn += "/noLTofQ"; //2023.06.29
									scDir = histn;
									outfile.mkdir(Form("%s/SC0", scDir.c_str()));
									outfile.mkdir(Form("%s/SC1", scDir.c_str()));
								}
								for(int isam = 0; isam < 2; isam ++){
									if(dir) outfile.cd(Form("%s/SC%d", scDir.c_str(), isam));
									for (int igr=1; igr>=0; igr--){
										if (ev.isreal && igr==0) continue;

										histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
										histn += sL9EffCut;
										if (igr==0) histn += "_gen";
										histn += sRig; //2023.03.22
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 3500, 0, 35));

										histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%s%d", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
										histn += sL9EffCut;
										if (igr==0) histn += "_gen";
										histn += sRig; //2023.03.22
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 3500, 0, 35));

										histn = Form("L1InnerL9NormChisqvsRigHZBin_r%d_q%d_%s%s%d", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
										histn += sL9EffCut;
										if (igr==0) histn += "_gen";
										histn += sRig; //2023.03.22
										hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 100));

										if(isam  == 1){
											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wXYHit", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wXYHitL1InnerL9Chisq", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wXYHitL1InnerL9ChisqLTOFCharge", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wXYHitL1InnerL9ChisqLTOFChargeL9Charge", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));
											
											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_woXYHit", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));
											
											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_woXHit", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_woYHit", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wChisq", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_woChisq", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wLtofQ", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_woLtofQ", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%s%d_wLtofQ", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%s%d_woLtofQ", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_wL9Q", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

											histn = Form("L9QvsRigHZBin_r%d_q%d_%s%s%d_woL9Q", ir, *it, sEff.c_str(), sL9EffVer0.c_str(), isam);
											histn += sL9EffCut;
											if (igr==0) histn += "_gen";
											histn += sRig; //2023.03.22
											hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));
										}
									}
								}
							}
							#endif ///SELCHECK
							
							/*//--L9Q
							if (igr==1)
							{
								histn = Form("L9Q_r%d_q%d_%sSam", ir, *it, sEff.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
					
								histn = Form("ubL9Q_r%d_q%d_%sSam", ir, *it, sEff.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
					
								histn = Form("EcalQ_r%d_q%d_%sSam", ir, *it, sEff.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
	
								histn = Form("L9Q_r%d_q%d_%sSel", ir, *it, sEff.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
					
								histn = Form("ubL9Q_r%d_q%d_%sSel", ir, *it, sEff.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
					
								histn = Form("EcalQ_r%d_q%d_%sSel", ir, *it, sEff.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
					
								#ifdef SUCCESSIVE
								//--L9 Efficiency
								for (int is=0; is<nSuccL9; is++)
								{
									histn = Form("L9Q_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
									histn += sRig; //2023.03.22
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
								
									histn = Form("ubL9Q_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
									histn += sRig; //2023.03.22
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
								
									histn = Form("EcalQ_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
									histn += sRig; //2023.03.22
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
								
									histn = Form("L9QvsEdep_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
									histn += sRig; //2023.03.22
									hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 850, 0., 850., 350, 0, 35));
								} //is
								#endif //SUCCESSIVE
							}*/
							
							#ifdef EFFMCTRUTH //move inside il9 loop to have sEff definition
							if (il9>=0) continue;
							if (dir)
							{
								histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
								histn += sRig;
								if (itightsam==1) histn += "TightSam"; //only change the name for Primary tighten sample
								if (il9>=0) histn += "/noLTofQ"; //2023.06.29
								histn += "/MCTruth";
								if (il9<=0) outfile.mkdir(Form("%s", histn.c_str()));
								outfile.cd(Form("%s", histn.c_str()));
							}
							
							for (int igr=0; igr<2; igr++)
							{
								if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break;
								
								string sGR=(igr==0)?"_gen":"";
								
								//------true L8->L9 survival probability
								histn = Form("rig_r%d_q%d_%s%sL8Sur%s", ir, *it, sEff.c_str(), sL9EffVer.c_str(), sGR.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								histn = Form("rig_r%d_q%d_%s%sL9Sur%s", ir, *it, sEff.c_str(), sL9EffVer.c_str(), sGR.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								//------true L9 selection efficiency
								histn = Form("rig_r%d_q%d_%s%sL9Sur&Sel%s", ir, *it, sEff.c_str(), sL9EffVer.c_str(), sGR.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								//------true L9 selection + LTofQ efficiency
								histn = Form("rig_r%d_q%d_%s%sL9Sur&Sel_wLTofQ%s", ir, *it, sEff.c_str(), sL9EffVer.c_str(), sGR.c_str());
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							}
							#endif //EFFMCTRUTH
						} //il9
						
						/*#ifdef L9EFFWITHOUTTOFQ
						if (dir)
						{
							//outfile.mkdir(Form("%s/%s/noLTofQ", sDirQ.c_str(), sSpan[ir].c_str()));
							//outfile.cd(Form("%s/%s/noLTofQ", sDirQ.c_str(), sSpan[ir].c_str()));
							//2023.02.22: to separate, add suffix in Span as well
							histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
							histn += sRig; //2023.03.22
							if (itightsam==1) histn += "TightSam"; //only change the name for Primary tighten sample //2023.02.22
							histn += "/noLTofQ";
							outfile.mkdir(Form("%s", histn.c_str()));
							outfile.cd(Form("%s", histn.c_str()));
						}
						histn = Form("LTofQ_r%d_q%d_%snoLTofQSam", ir, *it, sEff.c_str());
						histn += sRig; //2023.03.22
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 350, 0, 35));
						
						histn = Form("LTofQvsRig_r%d_q%d_%snoLTofQSam", ir, *it, sEff.c_str());
						histn += sRig; //2023.03.22
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
						histn = Form("LTofQvsRig_r%d_q%d_%snoLTofQSel", ir, *it, sEff.c_str());
						histn += sRig; //2023.03.22
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
						for (int igr=1; igr>=0; igr--)
						{
							if (igr==0 && ev.isreal) continue;
							
							histn = Form("rig_r%d_q%d_%snoLTofQSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%snoLTofQSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							#ifdef SUCCESSIVE
							for (int is=0; is<nSuccL9; is++)
							{
								histn = Form("rig_r%d_q%d_%snoLTofQSel_s%d", ir, *it, sEff.c_str(), is);
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //is
							#endif //SUCCESSIVE
						} //igr
						
						#ifdef EFFMCTRUTH
						for (int igr=1; igr>=0; igr--)
						{
							if (!(!ev.isreal && *it==(int)ev.mch)) break;
							
							//------cleanness of the selected sample
							histn = Form("rig_r%d_q%d_%snoLTofQSamCleanness", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//------Clean events which also passed numerator cut
							histn = Form("rig_r%d_q%d_%snoLTofQSamCleanness&Sel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						} //igr
						#endif //EFFMCTRUTH
						
						//L9Eff + LTofQ Eff
						for (int igr=1; igr>=0; igr--)
						{
							histn = Form("rig_r%d_q%d_%snoLTofQSel_wLTofQ", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("LTofQvsRig_r%d_q%d_%snoLTofQSel_wLTofQ", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						} //igr
						#endif //L9EFFWITHOUTTOFQ*/
						
						/*#ifdef EFFMCTRUTH
						if (dir)
						{
							//outfile.mkdir(Form("%s/%s/MCTruth", sDirQ.c_str(), sSpan[ir].c_str()));
							//outfile.cd(Form("%s/%s/MCTruth", sDirQ.c_str(), sSpan[ir].c_str()));
							//2023.02.22: to separate, add suffix in Span as well
							histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
							histn += sRig; //2023.03.22
							if (itightsam==1) histn += "TightSam"; //only change the name for Primary tighten sample //2023.02.22
							histn += "/MCTruth";
							outfile.mkdir(Form("%s", histn.c_str()));
							outfile.cd(Form("%s", histn.c_str()));
						}
						
						for (int igr=0; igr<2; igr++)
						{
							if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break;
							
							string sGR=(igr==0)?"_gen":"";
							
							//------true L8->L9 survival probability
							histn = Form("rig_r%d_q%d_%sL8Sur%s", ir, *it, sEff.c_str(), sGR.c_str());
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sL9Sur%s", ir, *it, sEff.c_str(), sGR.c_str());
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//------true L9 selection efficiency
							histn = Form("rig_r%d_q%d_%sL9Sur&Sel%s", ir, *it, sEff.c_str(), sGR.c_str());
							histn += sRig; //2023.03.22
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						}
						#endif //EFFMCTRUTH*/
					} //itightsam
				} //isamt
			} //it
		} //irig
	} //ir
	return true;
} //BookHistoL9Eff

#ifdef AMSSOFT
bool SelL9Eff(HistoMan &hman1, SelEvent &ev, const int ir, const double weight=1., const int QVer=0) //2020.02.06
//bool SelL9Eff(HistoMan &hman1, SelEvent &ev, const int ir, bool bIsreal, const double weight=1., const int QVer=0)
#else
bool SelL9Eff(TObjArray &hman1, SelEvent &ev, const int ir, const int irig, const double weight=1., const int QVer=0) //2023.03.21
//bool SelL9Eff(TObjArray &hman1, SelEvent &ev, const int ir, const double weight=1., const int QVer=0) //2020.02.06
//bool SelL9Eff(TObjArray &hman1, SelEvent &ev, const int ir, bool bIsreal, const double weight=1., const int QVer=0)
#endif //AMSOFT
{
	#ifdef SELCHECK
	if (irig!=0) return true;
	#endif ///SELCHECK

	if (ir!=2) return false;
	int iRigSpan=(ir==2)?1:0;
	string sRig=sRigName[irig]; //2023.03.21
	
	//--
	bool bL9Sam=false;
	bool bL9Sel=false;
	bool bL9SelSucc[nSuccL9];
	
	bool bSurSel=false;
	
	bool bHZ=false;
	
	//2019.10.25
	int iSkip=ev.CheckPrimary();
	//bool bL8Sur=(iSkip>=13);
	bool bL8Sur=(iSkip>=12); //2019.11.11
	bool bL9Sur=(iSkip>=18);
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		//if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		//2020.10.25
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		//Inner Tracker Q as reference charge, first check whether the reference charge of each event is inside viQSel or not
		bHZ = (*it==int(ev.GetTkInQNew(2,QVer)+0.5));
		if (!bHZ) continue;
		
		//numerator //2019.10.25
		//bL9Sel=ev.HasTkLHitXY(8)==2 && ev.GetChis(2, 1)<10 && ev.Select_ExtQ(*it, 3, QVer);
		bL9Sel=ev.HasTkLHitXY(8)==2 && ev.GetChis(2, 1, irig)<10 && ev.Select_ExtQ(*it, 3, QVer); //2023.03.27
		bL9SelSucc[0]=ev.HasTkLHitXY(8)==2;
		//bL9SelSucc[1]=ev.GetChis(2, 1)<10;
		bL9SelSucc[1]=ev.GetChis(2, 1, irig)<10; //2023.03.27
		bL9SelSucc[2]=ev.Select_ExtQ(*it, 3, QVer);
		bL9SelSucc[3]=ev.Select_TofQ(*it, 3, QVer); //2023.07.01
		
		for (int isamt=-1; isamt<iNSam; isamt++)
		{
			#ifdef SELCHECK
			if (isamt!=-1) continue;
			#endif ///SELCHECK

			double dTight=dTS[isamt+1];
			
			//----L9 tight sample selection, independent from il9 //2023.07.01
			bool bL9SamTight = true;
			//--tighten Upper tof q cut + good tof path length
			bL9SamTight = bL9SamTight && ev.GetTOFUDQ(0) > (*it-dTight*0.5) && ev.GetTOFUDQ(0) < (*it+dTight*0.5); //upper tof q +/-0.5
			//tof layer Q to be compatiable for upper and lower tof
			bL9SamTight = bL9SamTight && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
			//tof path length
			bL9SamTight = bL9SamTight && ev.IsGoodTofQUD(0);
			
			for (int il9=-1; il9<2; il9++)
			{
				/*string sEff="L9";
				if (isamt!=-1) sEff=Form("L9%d", isamt);*/
				string sEff="L9Eff";
				if (isamt!=-1) sEff=Form("L9Eff%d", isamt);
				
				//----charge selection, L1Q, UTofQ, InnerQ, LTofQ
				//bL9Sam = ev.Select_TofQ(*it, ir, QVer) && ev.Select_InnerQ(*it, QVer) && ev.Select_ExtQ(*it, 1, QVer);
				bL9Sam = ev.Select_TofQ(*it, ir, QVer, dTight) && ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, 1, QVer, dTight); //2019.10.17
				if (il9>=0) bL9Sam = ev.Select_TofQ(*it,  1, QVer, dTight) && ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, 1, QVer, dTight);
				bool bTOFQ = (il9==1)?ev.Select_TofQ(*it, 3, QVer):true;
				
				for (int itightsam=0; itightsam<2; itightsam++)
				{
					if (itightsam==0 && !(*it%2==0 && *it!=4)) continue; //only store two samples for primary
					//if (itightsam==1 && !bL9SamTight) continue; //apply tighter sample cut
					if ((itightsam==1 || (*it%2==1 || *it==4)) && !bL9SamTight) continue; //apply tighter sample cut, also for secondary //2023.07.02
					
					string sEffSuffix=sEff;
					if (*it%2==0 && *it!=4 && itightsam==1) sEffSuffix += "TightSam"; //only change the name for Primary tighten sample
					if (il9>=0) sEffSuffix += "noLTofQ"; //2023.07.01
					
					//--L9 efficiency
					if (!bL9Sam) continue;
					
					for (int igr=1; igr>=0; igr--)
					{
						if (igr==0 && ev.isreal) continue;
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
						
						//--denominator
						if (il9<=0)
						{
							if (igr==1)
							{
								//sample
								histn = Form("LTofQvsRig_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
								histn += sRig; //2023.03.21
								GetHist(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1));
							}
							
							histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							
							//2023.06.29
							#ifdef EFFSEC
							if (!ev.isreal)
							{
								int ifrac = -1;
								if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
								//if (ev.PrimaryUpToTrLayer(9)) ifrac=0; //for L9Eff, should survivl up to L9 //for sample, up to Inner is enough, same as others
								else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
								else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
								else ifrac=3;
								
								histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sEffSecMCTruth[ifrac];
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							} //ifrac
							#endif //EFFSEC
						}
						#ifdef SELCHECKL9
						if (il9 == 1){

							histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffSuffix.c_str(), 0);
							if (il9==1) histn += "_wLTofQ";
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							// printf("\t L9EFF--- %d -- %s\n",__LINE__, histn.c_str());

							histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffSuffix.c_str(), 0);
							if (il9==1) histn += "_wLTofQ";
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);

							histn = Form("L1InnerL9NormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffSuffix.c_str(), 0);
							if (il9==1) histn += "_wLTofQ";
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.22
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(2,1,irig), weight);

							bool bL9Sel_wXY = ev.HasTkLHitXY(8)==2; ////only XY Hit
							bool bL9Sel_wXY_2 = bL9Sel_wXY && ev.GetChis(2, 1, irig)<10;
							bool bL9Sel_wXY_3 = bL9Sel_wXY_2 && ev.Select_TofQ(*it, 3, QVer);
							bool bL9Sel_wXY_4 = bL9Sel_wXY_3 && ev.Select_ExtQ(*it, 3, QVer);
							bool bL9Sel_woXY = ev.GetChis(2, 1, irig)<10 && ev.Select_ExtQ(*it, 3, QVer) && ev.Select_TofQ(*it, 3, QVer); /// without XYHit
							bool bL9Sel_woX = ev.GetUBExtQTrkCor(0, 1, QVer)>0 && bL9Sel_woXY;
							bool bL9Sel_woY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && bL9Sel_woXY;
							bool bL9Sel_wchisq = ev.GetChis(2, 1, irig)<10;
							bool bL9Sel_wochisq = ev.HasTkLHitXY(8)==2 && ev.Select_ExtQ(*it, 3, QVer) && ev.Select_TofQ(*it, 3, QVer);
							bool bL9Sel_wltofq = ev.Select_TofQ(*it, 3, QVer);
							bool bL9Sel_woltofq = bL9Sel;
							bool bL9Sel_wl9q = ev.Select_ExtQ(*it, 3, QVer);
							bool bL9Sel_wol9q = ev.HasTkLHitXY(8)==2 && ev.GetChis(2, 1, irig)<10 && ev.Select_TofQ(*it, 3, QVer);

							if(bL9Sel_wXY){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wXYHit", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wXY_2){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wXYHitL1InnerL9Chisq", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wXY_3){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wXYHitL1InnerL9ChisqLTOFCharge", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wXY_4){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wXYHitL1InnerL9ChisqLTOFChargeL9Charge", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_woXY){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_woXYHit", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_woX){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_woXHit", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_woY){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_woYHit", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wchisq){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wChisq", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wochisq){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_woChisq", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wltofq){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wLtofQ", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);

								histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%d_wLtofQ", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
							}
							if(bL9Sel_woltofq){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_woLtofQ", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);

								histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%d_woLtofQ", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
							}
							if(bL9Sel_wl9q){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_wL9Q", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}
							if(bL9Sel_wol9q){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d_woL9Q", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);
							}

							if (bL9Sel && bTOFQ){
								histn = Form("L9QvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ";
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8, 2, QVer), weight);

								histn = Form("LTofQvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ";
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);

								histn = Form("L1InnerL9NormChisqvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEffSuffix.c_str(), 1);
								if (il9==1) histn += "_wLTofQ";
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.22
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetChis(2,1,irig), weight);
							}
						}
						#endif ///SELCHECK
						
						//--numerator
						//if (bL9Sel)
						//2023.07.01
						if (bL9Sel && bTOFQ)
						{
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
							if (il9==1) histn += "_wLTofQ"; //2023.07.01
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							
							if (igr==1)
							{
								//sample
								histn = Form("LTofQvsRig_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								histn += sRig; //2023.03.21
								GetHist(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1));
							}
							
							//2023.06.29
							#ifdef EFFSEC
							if (!ev.isreal)
							{
								int ifrac = -1;
								if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
								//if (ev.PrimaryUpToTrLayer(9)) ifrac=0; //for L9Eff, should survivl up to L9 //for sample, up to Inner is enough, same as others
								else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
								else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
								else ifrac=3;
								
								histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
								if (il9==1) histn += "_wLTofQ"; //2023.07.01
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sEffSecMCTruth[ifrac];
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							} //ifrac
							#endif //EFFSEC
						}
						
						#ifdef SUCCESSIVE
						//--L9 Efficiency
						for (int is=0; is<nSuccL9; is++)
						{
							if (il9>0) break;
							if (!bL9SelSucc[is]) break;
							histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							if (igr==0) histn += "_gen";
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
							/*if (igr==0) continue;
							histn = Form("L9Q_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(8, 2, QVer));
							histn = Form("ubL9Q_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(1, 2, QVer));
							histn = Form("EcalQ_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.ecal_q);
						
							histn = Form("L9QvsEdep_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.ecal_en[2]/1000., ev.GetTkLQ(8));*/
						} //is
						#endif //SUCCESSIVE
					} //igr
					
					/*//--L9Q
					if (igr==1)
					{
						histn = Form("L9Q_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
						GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(8, 2, QVer));
						histn = Form("ubL9Q_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
						GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(1, 2, QVer));
						histn = Form("EcalQ_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
						GetHist(hman1, histn.c_str())->Fill(ev.ecal_q);
						
						if (bL9Sel)
						{
							histn = Form("L9Q_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(8, 2, QVer));
							histn = Form("ubL9Q_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
							GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(1, 2, QVer));
							histn = Form("EcalQ_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
							GetHist(hman1, histn.c_str())->Fill(ev.ecal_q);
						}
						
						#ifdef SUCCESSIVE
						//--L9 Efficiency
						for (int is=0; is<nSuccL9; is++)
						{
							if (!bL9SelSucc[is]) break;
							
							if (igr==0) continue;
							histn = Form("L9Q_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.GetTkLQNew(8, 2, QVer));
							histn = Form("ubL9Q_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.GetUBExtQTrkCor(1, 2, QVer));
							histn = Form("EcalQ_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.ecal_q);
						
							histn = Form("L9QvsEdep_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							histn += sRig; //2023.03.22
							GetHist(hman1, histn.c_str())->Fill(ev.ecal_en[2]/1000., ev.GetTkLQ(8));
						} //is
					}
					#endif //SUCCESSIVE*/
					
					//--sample cleanness
					#ifdef EFFMCTRUTH
					for (int igr=1; igr>=0; igr--)
					{
						if (!(!ev.isreal && *it==(int)ev.mch && bL8Sur)) break;
						
						if (igr==0 && ev.isreal) continue;
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
						
						//------cleanness of the selected sample
						histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEffSuffix.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						if (il9<=0) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						//------Clean events which also passed numerator cut
						histn = Form("rig_r%d_q%d_%sSamCleanness&Sel", ir, *it, sEffSuffix.c_str());
						if (il9==1) histn += "_wLTofQ";
						if (igr==0) histn += "_gen";
						histn += sRig;
						if (bL9Sel && bTOFQ) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //igr
					#endif //EFFMCTRUTH
					
					//--survival probability
					#ifdef EFFMCTRUTH
					for (int igr=1; igr>=0; igr--)
					{
						if (il9>=0) break;
						if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break;
						
						if (igr==0 && ev.isreal) continue;
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
								
						if (!bL8Sur) continue;
						histn = Form("rig_r%d_q%d_%sL8Sur", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						if (!bL9Sur) continue;
						histn = Form("rig_r%d_q%d_%sL9Sur", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						if (!bL9Sel) continue;
						histn = Form("rig_r%d_q%d_%sL9Sur&Sel", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						if (!bTOFQ) continue;
						histn = Form("rig_r%d_q%d_%sL9Sur&Sel", ir, *it, sEff.c_str());
						histn += "_wLTofQ";
						if (igr==0) histn += "_gen";
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //igr
					#endif //EFFMCTRUTH
				} //itightsam
			} //il9
			
			/*#ifdef L9EFFWITHOUTTOFQ
			bL9Sam = ev.Select_TofQ(*it,  1, QVer, dTight) && ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, 1, QVer, dTight);
			
			//--tighten Upper tof q cut + good tof path length
			bL9SamTight = bL9SamTight && ev.GetTOFUDQ(0) > (*it-dTight*0.5) && ev.GetTOFUDQ(0) < (*it+dTight*0.5); //upper tof q +/-0.5
			//tof layer Q to be compatiable for upper and lower tof
			bL9SamTight = bL9SamTight && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
			//tof path length
			bL9SamTight = bL9SamTight && ev.IsGoodTofQUD(0);
			
			for (int itightsam=0; itightsam<2; itightsam++)
			{
				if (itightsam==0 && !(*it%2==0 && *it!=4)) continue; //only store two samples for primary
				if (itightsam==1 && !bL9SamTight) continue; //apply tighter sample cut
				
				string sEffSuffix=sEff;
				if (*it%2==0 && *it!=4 && itightsam==1) sEffSuffix += "TightSam"; //only change the name for Primary tighten sample
				
				//--
				if (!bL9Sam) continue;
				bHZ=true;
				
				//--L9 efficiency
				for (int igr=1; igr>=0; igr--)
				{
					if (igr==0 && ev.isreal) continue;
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
					
					//sample
					histn = Form("rig_r%d_q%d_%snoLTofQSam", ir, *it, sEffSuffix.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2023.03.21
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					//2023.06.29
					#ifdef EFFSEC
					if (!ev.isreal)
					{
						int ifrac = -1;
						if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
						else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
						else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
						else ifrac=3;
						
						histn = Form("rig_r%d_q%d_%snoLTofQSam", ir, *it, sEffSuffix.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						histn += sEffSecMCTruth[ifrac];
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //ifrac
					#endif //EFFSEC
					
					if (igr==1)
					{
						//histn = Form("LTofQ_r%d_q%d_%snoLTofQSam", ir, *it, sEffSuffix.c_str());
						//histn += sRig; //2023.03.21
						//GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
						
						histn = Form("LTofQvsRig_r%d_q%d_%snoLTofQSam", ir, *it, sEffSuffix.c_str());
						histn += sRig; //2023.03.21
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
					}
					
					//--numerator
					if (bL9Sel)
					{
						histn = Form("rig_r%d_q%d_%snoLTofQSel", ir, *it, sEffSuffix.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%snoLTofQSel", ir, *it, sEffSuffix.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //ifrac
						#endif //EFFSEC
						
						if (igr==1)
						{
							histn = Form("LTofQvsRig_r%d_q%d_%snoLTofQSel", ir, *it, sEffSuffix.c_str());
							histn += sRig; //2023.03.21
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
						}
						
						//L9Eff + LTofQ Eff
						if (ev.Select_TofQ(*it,  3, QVer))
						{
							histn = Form("rig_r%d_q%d_%snoLTofQSel_wLTofQ", ir, *it, sEffSuffix.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							
							//2023.06.29
							#ifdef EFFSEC
							if (!ev.isreal)
							{
								int ifrac = -1;
								if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
								else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
								else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
								else ifrac=3;
								
								histn = Form("rig_r%d_q%d_%snoLTofQSel_wLTofQ", ir, *it, sEffSuffix.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sEffSecMCTruth[ifrac];
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							} //ifrac
							#endif //EFFSEC
							
							if (igr==1)
							{
								histn = Form("LTofQvsRig_r%d_q%d_%snoLTofQSel_wLTofQ", ir, *it, sEffSuffix.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
							}
						}
					}
				
					#ifdef SUCCESSIVE
					//--L9 Efficiency
					for (int is=0; is<nSuccL9; is++)
					{
						if (!bL9SelSucc[is]) break;
						histn = Form("rig_r%d_q%d_%snoLTofQSel_s%d", ir, *it, sEffSuffix.c_str(), is);
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					} //is
					#endif //SUCCESSIVE
				} //igr
				
				//--sample cleanness
				#ifdef EFFMCTRUTH
				for (int igr=1; igr>=0; igr--)
				{
					if (!(!ev.isreal && *it==(int)ev.mch && bL8Sur)) break;
					
					if (igr==0 && ev.isreal) continue;
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
					
					//------cleanness of the selected sample
					histn = Form("rig_r%d_q%d_%snoLTofQSamCleanness", ir, *it, sEffSuffix.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2023.03.21
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
					//------Clean events which also passed numerator cut
					histn = Form("rig_r%d_q%d_%snoLTofQSamCleanness&Sel", ir, *it, sEffSuffix.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2023.03.21
					if (bL9Sel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				} //igr
				#endif //EFFMCTRUTH
			} //itightsam
			#endif //L9EFFWITHOUTTOFQ*/
			
			/*//--survival probability
			#ifdef EFFMCTRUTH
			for (int igr=1; igr>=0; igr--)
			{
				if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1)) break;
				
				if (igr==0 && ev.isreal) continue;
				double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
						
				if (!bL8Sur) continue;
				histn = Form("rig_r%d_q%d_%sL8Sur", ir, *it, sEff.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig; //2023.03.21
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
				if (!bL9Sur) continue;
				histn = Form("rig_r%d_q%d_%sL9Sur", ir, *it, sEff.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig; //2023.03.21
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
				if (!bL9Sel) continue;
				histn = Form("rig_r%d_q%d_%sL9Sur&Sel", ir, *it, sEff.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig; //2023.03.21
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
			} //igr
			#endif //EFFMCTRUTH*/
		} //isamt
	} //it
	return bHZ;
} //SelL9Eff

//------checking the FSChis and L1L9Chis over MC truth sample //2023.04.14
#ifdef AMSSOFT
bool BookFSChisEff(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false)
#else
bool BookFSChisEff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false)
#endif //AMSSOFT
{

	#ifdef SELCHECK
	return true;
	#endif ///SELCHECK

	if (ev.isreal)
	{
		cout << "Only run BookFSChisEff for MC while ev.isreal=" << ev.isreal << endl;
		return false;
	}
	
	string sDir = Form("FSChis_Efficiency%d", QVer);
	
	for (int ir=2; ir<3; ir++)
	{
		for (int irig=0; irig<NRig; irig++)
		{
			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			string sRig=sRigName[irig];
			
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				#ifndef EFFSEC
				if (!ev.isreal && *it!=ev.mch) continue;
				#endif //EFFSEC
				
				iNbin = getNbin(*it);
				pBins = getBins(*it);
				
				for (int isamt=-1; isamt<iNSam; isamt++)
				{
					string sDirQ;
					if (isamt==-1) sDirQ=Form("%s/q%d", sDir.c_str(), *it);
					else sDirQ=Form("%s/q%d_samt%d", sDir.c_str(), *it, isamt);
					
					for (int iextcut=0; iextcut<2; iextcut++)
					{
						string sEff="FSChisEff";
						if (iextcut==1) sEff += "ExtCut";
						if (isamt!=-1) sEff+=Form("%d", isamt);
						
						//--FSChis Efficiency
						if (dir)
						{
							histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
							histn += sRig;
							outfile.mkdir(Form("%s", histn.c_str()));
							outfile.cd(Form("%s", histn.c_str()));
						}
						
						for (int igr=1; igr>=0; igr--)
						{
							if (ev.isreal && igr==0) continue;
							histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							#ifdef SUCCESSIVE
							for (int is=0; is<3; is++)
							{
								histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
								if (igr==0) histn += "_gen";
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //is
							#endif //SUCCESSIVE
						} //igr
					} //iextcut
				} //isamt
			} //it
		} //irig
	} //ir
	return true;
} //BookFSChisEff

#ifdef AMSSOFT
#else
bool SelFSChisEff(TObjArray &hman1, SelEvent &ev, const int ir, const int irig, const double weight=1., const int QVer=0)
#endif //AMSOFT
{

	#ifdef SELCHECK
	return true;
	#endif ///SELCHECK

	if (ir!=2) return false;
	if (ev.isreal) return false;
	
	int iRigSpan=(ir==2)?1:0;
	string sRig=sRigName[irig];
	
	//--
	bool bFSChisSam=false;
	bool bFSChisSel=false;
	bool bFSChisSelSucc[nSuccL9];
	
	bool bSurSel=false;
	
	bool bHZ=false;
	
	//int iSkip=ev.CheckPrimary();
	////bool bL8Sur=(iSkip>=13);
	//bool bL8Sur=(iSkip>=12);
	//bool bL9Sur=(iSkip>=18);
	//2023.04.14
	bool bL8Sur=ev.PrimaryUpToTrLayer(8);
	bool bL9Sur=ev.PrimaryUpToTrLayer(9);
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		//Inner Tracker Q as reference charge, first check whether the reference charge of each event is inside viQSel or not
		bHZ = (*it==int(ev.GetTkInQNew(2,QVer)+0.5));
		if (!bHZ) continue;
		
		for (int iMCTruth=0; iMCTruth<2; iMCTruth++)
		{
			//--sample
			bFSChisSam = bRig[0][irig] && bTrack[0][irig] ; //cutoff && Inner Tracker && FS Geom
			bFSChisSam = bFSChisSam && ev.Select_InnerQ(*it, QVer) && ev.Select_TofQ(*it, 2, QVer); //Inner Q + Upper and Lower TOFQ cut
			if (iMCTruth==0) bFSChisSam = bFSChisSam && ev.PrimaryUpToTrLayer(9);//survivaled up to L9, using Skip
			else if (iMCTruth==1) bFSChisSam = bFSChisSam && ev.GetTrMCLZ(1)==*it && ev.GetTrMCLZ(9)==*it; //have matched TrMCCluster in L1 and L9
			
			//numerator
			bFSChisSel=ev.HasTkLHitXY(8)==2 && ev.GetChis(2, 1, irig)<10 && ev.Select_ExtQ(*it, 3, QVer);
			bFSChisSelSucc[0]=ev.HasTkLHitXY(8)==2;
			//bFSChisSelSucc[1]=ev.GetChis(2, 1)<10;
			bFSChisSelSucc[1]=ev.GetChis(2, 1, irig)<10; //2023.03.27
			bFSChisSelSucc[2]=ev.Select_ExtQ(*it, 3, QVer);
			
			for (int isamt=-1; isamt<iNSam; isamt++)
			{
				double dTight=dTS[isamt+1];
				/*string sEff="L9";
				if (isamt!=-1) sEff=Form("L9%d", isamt);*/
				string sEff="L9Eff";
				if (isamt!=-1) sEff=Form("L9Eff%d", isamt);
				
				//----charge selection, L1Q, UTofQ, InnerQ, LTofQ
				bFSChisSam = ev.Select_TofQ(*it, ir, QVer, dTight) && ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, 1, QVer, dTight); //2019.10.17
				//2023.02.22
				bool bFSChisSamTight = true;
				//--tighten Upper tof q cut + good tof path length
				bFSChisSamTight = bFSChisSamTight && ev.GetTOFUDQ(0) > (*it-dTight*0.5) && ev.GetTOFUDQ(0) < (*it+dTight*0.5); //upper tof q +/-0.5
				//tof layer Q to be compatiable for upper and lower tof
				bFSChisSamTight = bFSChisSamTight && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
				//tof path length
				bFSChisSamTight = bFSChisSamTight && ev.IsGoodTofQUD(0);
				
				for (int itightsam=0; itightsam<2; itightsam++)
				{
					if (itightsam==0 && !(*it%2==0 && *it!=4)) continue; //only store two samples for primary
					if (itightsam==1 && !bFSChisSamTight) continue; //apply tighter sample cut
					string sEffSuffix=sEff;
					if (*it%2==0 && *it!=4 && itightsam==1) sEffSuffix += "TightSam"; //only change the name for Primary tighten sample
					
					//--L9 efficiency
					if (!bFSChisSam) continue;
					
					//sample
					histn = Form("LTofQ_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
					histn += sRig; //2023.03.21
					GetHist(hman1, histn.c_str())->Fill(ev.GetTOFUDQ(1));
					
					for (int igr=1; igr>=0; igr--)
					{
						if (igr==0 && ev.isreal) continue;
						double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(iRigSpan, irig);
						
						//--denominator
						histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffSuffix.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
						//--numerator
						if (bFSChisSel)
						{
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffSuffix.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						}
						
						#ifdef SUCCESSIVE
						//--L9 Efficiency
						for (int is=0; is<nSuccL9; is++)
						{
							if (!bFSChisSelSucc[is]) break;
							histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEffSuffix.c_str(), is);
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //is
						#endif //SUCCESSIVE
					} //igr
					
				} //itightsam
				
				#ifdef L9EFFWITHOUTTOFQ
				bFSChisSam = ev.Select_TofQ(*it,  1, QVer, dTight) && ev.Select_InnerQ(*it, QVer, dTight) && ev.Select_ExtQ(*it, 1, QVer, dTight);
				//--tighten Upper tof q cut + good tof path length
				bFSChisSamTight = bFSChisSamTight && ev.GetTOFUDQ(0) > (*it-dTight*0.5) && ev.GetTOFUDQ(0) < (*it+dTight*0.5); //upper tof q +/-0.5
				//tof layer Q to be compatiable for upper and lower tof
				bFSChisSamTight = bFSChisSamTight && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
				//tof path length
				bFSChisSamTight = bFSChisSamTight && ev.IsGoodTofQUD(0);
				#endif //L9EFFWITHOUTTOFQ
			} //isamt
		} //iMCTruth
	} //it
	return bHZ;
} //SelFSChisEff

//------Tracking Efficiency
#ifdef AMSSOFT
bool BookHistoTkEffN(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
//bool BookHistoTkEffN(HistoMan &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#else
//bool BookHistoTkEffN(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int irig, const int QVer, bool dir=false) //2023.02.20
bool BookHistoTkEffN(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
//bool BookHistoTkEffN(TObjArray &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{

	#ifdef SELCHECK
	return true;
	#endif ///SELCHECK

	string sDir=Form("Tracking_Efficiency%d", QVer);
	//if (dir) outfile.mkdir(sDir.c_str());
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		for (int isamt=-1; isamt<iNSam; isamt++)
		{
			string sDirQ;
			if (isamt==-1) sDirQ=Form("%s/q%d", sDir.c_str(), *it);
			else  sDirQ=Form("%s/q%d_samt%d", sDir.c_str(), *it, isamt);
			string sEff="TkEff";
			if (isamt!=-1) sEff=Form("TkEff%d", isamt);
			
			if (dir)
			{
				outfile.mkdir(Form("%s", sDirQ.c_str()));
				outfile.cd(Form("%s", sDirQ.c_str()));
			}
			for (int ir=1; ir<3; ir++)
			{
				for (int irig=0; irig<NRig; irig++) //for TkEff, only difference between rig algorithm is the numerator //the estimator is different for different algorithm, so still need this for all
				{
					#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
					if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
					#endif //PASS7GBL
					
					string sRig=sRigName[irig];
					
					if (dir)
					{
						histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
						histn += sRig;
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					
					//for (int imr=0; imr<=Mimr; imr++)
					for (int imr=-1; imr<=Mimr; imr++) //imr=-1: generated rigidity //2023.03.26
					{
						if (imr==-1 && ev.isreal) continue;
						
						string sSuffix = "";
						if (imr>=0) sSuffix = Form("_mr%d", imr);
						else sSuffix = "_gen";
						
						if (dir)
						{
							histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
							histn += sRig;
							if (imr>=0)
							{
								histn += Form("/%s", sRigMethod[imr].c_str());
								outfile.mkdir(histn.c_str());
							}
							outfile.cd(histn.c_str());
						}
						histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
						histn += sSuffix;
						histn += sRig; //2023.03.26
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
						//--
						histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
						histn += sSuffix;
						histn += sRig; //2023.03.26
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
						//2023.06.29
						#ifdef EFFSEC
						for (int ifrac=0; ifrac<4; ifrac++)
						{
							if (ev.isreal) break;
							if (imr!=-1) break;
							
							for (int isamsel=0; isamsel<2; isamsel++)
							{
								histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
								histn += (isamsel==0)?"Sam":"Sel";
								histn += sSuffix;
								histn += sRig;
								histn += sEffSecMCTruth[ifrac];
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //isamsel
						} //ifrac
						#endif //EFFSEC
						
						for (int itofchisnorm=0; itofchisnorm<2; itofchisnorm++)
						{
							histn = Form("rig_r%d_q%d_%sSel_tofchis%d", ir, *it, sEff.c_str(), itofchisnorm);
							histn += sSuffix;
							histn += sRig; //2023.03.26
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						} //itofchisnorm
					
						#ifdef SUCCESSIVE
						if (dir)
						{
							//outfile.mkdir(("%s/%s/%s/succ", sDirQ.c_str(), sSpan[ir].c_str(), sRigMethod[imr].c_str()));
							histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
							histn += sRig;
							if (imr>=0)
							{
								histn += Form("/%s/succ", sRigMethod[imr].c_str());
								outfile.mkdir(histn.c_str());
							}
							outfile.cd(histn.c_str());
						}
						for (int is=0; is<nSuccTk; is++)
						{
							histn = Form("rig_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
							histn += sSuffix;
							histn += sRig; //2023.03.26
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//--store the InnerQ vs rig before and after applying InnerQ cut
							//if (is!=6 && is!=7) continue;
							//now the order is change and new InnerQ cut added, store the InnerQ after 1) rms cut, 2) >Z-2.5 and 3) event selection //2023.02.20
							if (is<7) continue;
							histn = Form("InnerQvsrig_r%d_q%d_%sSel_s%d", ir, *it, sEff.c_str(), is);
							histn += sSuffix;
							histn += sRig; //2023.03.26
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						}
						#endif //SUCCESSIVE
					} //imr
					
					//--
					#ifdef EFFMCTRUTH
					//if (!ev.isreal && *it==(int)ev.mch)
					if (!(!ev.isreal && *it==(int)ev.mch)) continue;
					
					//------cleaness of the selected sample
					//for (int imr=0; imr<3; imr++)
					for (int imr=-1; imr<3; imr++) //imr=-1: generated rigidity //2023.03.26
					{
						if (imr==-1 && ev.isreal) continue;
						
						string sSuffix = "";
						if (imr>=0) sSuffix = Form("_mr%d", imr);
						else sSuffix = "_gen";
						
						//if (dir) outfile.cd(Form("%s/%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sRigMethod[imr].c_str()));
						if (dir)
						{
							histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
							histn += sRig;
							if (imr>=0) histn += Form("/%s", sRigMethod[imr].c_str());
							outfile.cd(histn.c_str());
						}
						
						histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEff.c_str());
						histn += sSuffix;
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
						//--
						histn = Form("rig_r%d_q%d_%sSamCleanness&Sel", ir, *it, sEff.c_str());
						histn += sSuffix;
						histn += sRig; //2023.03.26
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					} //imr
					
					//------sample seleted by MC truth
					if (isamt!=-1) continue; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
					//2023.02.20
					//no need to require L1XY and L9XY, but this will ensure the exact active region as event selection as there might be some dead region in L1 and L9
					for (int iextcut=0; iextcut<2; iextcut++) //iextcut=0: no L1XY (and L9XY for FS) cut, =1: add external XY cut
					{
						//for (int imr=0; imr<3; imr++)
						for (int imr=-1; imr<3; imr++) //imr=-1: generated rigidity
						{
							//if (dir) outfile.cd(Form("%s/%s/%s", sDirQ.c_str(), sSpan[ir].c_str(), sRigMethod[imr].c_str()));
							if (dir)
							{
								histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
								histn += sRig;
								if (imr>=0) histn += Form("/%s", sRigMethod[imr].c_str());
								outfile.cd(histn.c_str());
							}
							
							string sSuffix = "";
							if (imr>=0) sSuffix = Form("_mr%d", imr);
							else sSuffix = "_gen";
						
							histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, *it, sEff.c_str());
							histn += sSuffix;
							histn += Form("_extcut%d", iextcut); //2023.02.20
							histn += sRig; //2023.03.26
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, *it, sEff.c_str());
							histn += sSuffix;
							histn += Form("_extcut%d", iextcut); //2023.02.20
							histn += sRig; //2023.03.26
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							#ifdef SUCCESSIVE
							for (int is=0; is<nSuccTk; is++)
							{
								histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", ir, *it, sEff.c_str(), is);
								histn += sSuffix;
								histn += Form("_extcut%d", iextcut); //2023.02.20
								histn += sRig; //2023.03.26
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							}
							#endif //SUCCESSIVE
						} //imr
					} //iextcut
					#endif //EFFMCTRUTH
				} //irig
					
				#ifdef TKEFFSAMSUCC
				histn = Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str());
				outfile.cd(histn.c_str());
				
				const int iTkEffSamCut=11;
				for (int is=0; is<iTkEffSamCut; is++)
				{
					histn = Form("rig_r%d_q%d_%sSam_s%d_gen", ir, *it, sEff.c_str(), is);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				} //is
				
				for (int icut=0; icut<iTkEffSamCut; icut++)
				{
					histn = Form("rig_r%d_q%d_%sSam_N1%d_gen", ir, *it, sEff.c_str(), icut);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				} //icut
				#endif //TKEFFSAMSUCC
			} //ir
		} //isamt
		//#endif //EFFTIGHTSAM
	} //it
	
	return true;
} //BookHistoTkEffN

#ifdef AMSSOFT
bool SelTkEffN(HistoMan &hman1, SelEvent &ev, double weight[3], bool bPart, bool bBeta, bool bTrack[3], bool bTkgeom[3], int QVer=0)
//bool SelTkEffN(HistoMan &hman1, SelEvent &ev, bool bIsreal, double weight[3], bool bPart, bool bBeta, bool bTrack[3], bool bTkgeom[3], int QVer=0)
#else
bool SelTkEffN(TObjArray &hman1, SelEvent &ev, double weight[3], const int irig, const int QVer=0) //2023.02.20
//bool SelTkEffN(TObjArray &hman1, SelEvent &ev, double weight[3], bool bPart, bool bBeta, bool bTrack[3], bool bTkgeom[3], int QVer=0)
//bool SelTkEffN(TObjArray &hman1, SelEvent &ev, bool bIsreal, double weight[3], bool bPart, bool bBeta, bool bTrack[3], bool bTkgeom[3], int QVer=0)
#endif //AMSSOFT
{
	#ifdef SELCHECK
	return true;
	#endif ///SELCHECK

	string sRig=sRigName[irig]; //2023.03.21
	
	//------sel for track eff sample
	//--TrdTofParticle
	bTRDTOFPar = (ev.ibetahs>=0&&ev.itrdtracks>=0&&ev.betahs>0.4);
	//--L1XY & L1Z
	bL1XY = (ev.GetUBExtQBetaCor(0,0,QVer)>0&&ev.GetUBExtQBetaCor(0,1,QVer)>0);
	
	//--fiducial volume cut
	bool mlh[9];
	int mnhitf=0;
	for(int il=0;il<9;il++)
	{
		mlh[il]=false;
		float coox=ev.tk_pos1s[il][0];
		float cooy=ev.tk_pos1s[il][1];
		float disr=coox*coox+cooy*cooy;
		if (il==9-1) mlh[il]=(fabs(coox)<cirr[il]&&fabs(cooy)<ciry[il]);
		else mlh[il]=(sqrt(disr)<cirr[il]&&fabs(cooy)<ciry[il]);
	} //il
	
	for(int il=0+1;il<9-1;il++) if(mlh[il]){mnhitf++;}
	bGeometry[0] = ( (mnhitf>=5) && (mlh[1]) && (mlh[2] || mlh[3]) && (mlh[4] || mlh[5]) && (mlh[6] || mlh[7]) );
	bGeometry[1] = bGeometry[0] && mlh[0];
	bGeometry[2] = bGeometry[1] && mlh[8];
	
	//--L1Q status
	l1Qs = ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0); //tk_exqls & tk_l1qs should be the same
	//--l9
	l9xy = (ev.GetUBExtQBetaCor(1,0,QVer)>0 && ev.GetUBExtQBetaCor(1,1,QVer)>0);
	
	//--TOF
	//ToFBeta
	bToFNHits[1] = bToFNHits[2] = true;
	bToFChis = (ev.tof_chiscs<20 && ev.tof_chists<20); //2020.11.09: add it back, as it actually clean up quite a bit for the secondary
	//bToFChis = true; //2020.11.04: remove
	//bToFNHits[1] = bToFNHits[2] = (ev.tof_nhits==4);
	//bToFChis = (ev.tof_chiscs<10 && ev.tof_chists<10);
	
	//--use unbiased L1Q as reference charge, i.e. Z = int(unbiased L1Q+0.5)
	//int tk_l1zu = int(ev.GetUBExtQBetaCor(0,2,QVer)+0.5);
	//use unbiased UTOFQ as reference Q
	int tk_utofq = int((ev.tof_qls[0]+ev.tof_qls[1])/2.+0.5); //2020.09.28: is it OK to use UTOFQ as reference Z also for light nuclei such as Oxygen? //2023.02.20: might need to apply tighter UPTOFQ lower cut TODO
	double dRefQ = (ev.tof_qls[0]+ev.tof_qls[1])/2.; //reference float charge as well
	
	//------ reference charge
	bRefQ=false;
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		if (*it==tk_utofq) //check whether tk_utofq is wanted charge i.e. whether it's in viQSel
		{
			bRefQ=true;
			break;
		}
	} //it
	//2020.07.11: stop running if not a intersting sample
	//if (!bRefQ) return false;
	//2020.09.10: for TkEff sample check, keep events not passing bRefQ
	#ifdef TKEFFSAMSUCC
	if (!bRefQ) tk_utofq=ev.mch;
	#else
	//if (!bRefQ) return false;
	//2020.11.09
	//if (!bRefQ && std::find(viQSel.begin(), viQSel.end(), (int)ev.mch) == viQSel.end()) return false;
	//2020/11/12
	if (!bRefQ)
	{
		if (std::find(viQSel.begin(), viQSel.end(), (int)ev.mch) == viQSel.end()) return false;
		else tk_utofq=(int)ev.mch;
	}
	#endif //TKEFFSAMSUCC
	
	//ToFQ //calculate the charge cut for sample inside isamt loop
	bool cuttofch[2];
	cuttofch[0]=true;
	cuttofch[1]=true;
	
	//------calculate for each rigidity estimator
	//beta
	double beta = ev.betahs;
	rigidity[0] = MPROTON*dMass[tk_utofq]*beta / sqrt(1 - pow(beta, 2)) / tk_utofq;
	if (!ev.isreal && dMass[tk_utofq]!=MCMass) rigidity[0] = MPROTON*MCMass*beta / sqrt(1 - pow(beta, 2)) / tk_utofq;
	bMethodCut[0] = (beta>0 && beta<0.96); //need to exam TODO
	
	//cutoff or generate rigidity
	if (!ev.isreal) rigidity[1] = ev.mmom/ev.mch;
	else
	{
		#ifdef AMSSOFT
		AMSSetupR::RTI a;
		if(AMSEventR::GetRTI(a,ev.time[0])!=0) rigidity[1] = 0;
		else rigidity[1] = a.cfi[icffv][1];
		#else
		rigidity[1] = ev.mcutoffi[icffv][1];
		#endif //AMSSOFT
		//rigidity[1] = getcutoffrig5(rigidity[1], tk_utofq, QVer, 1);
		rigidity[1] = getcutoffrig(rigidity[1], tk_utofq, QVer, irig, 1); //2023.03.26
	}
	bMethodCut[1] = rigidity[1]>0.8;
//	bMethodCut[1] = rigidity[1]>0.8 && rigidity[1]<32;
//	bMethodCut[1] = rigidity[1]>3.9 && rigidity[1]<25;
	
	//Ecal
	rigidity[2] = ev.ecal_ens/1000;
	//rigidity[2] = getedeprig5(rigidity[2], tk_utofq, QVer, 2);
	rigidity[2] = getedeprig(rigidity[2], tk_utofq, QVer, irig, 2); //2023.03.26
	
	/*bMethodCut[2] = (ev.ecal_ens>0) && (ev.ecal_dis<10);
	bool CutEcalCharge = ev.ecal_ens>1500;
	if (tk_utofq>=6) CutEcalCharge = ev.ecal_ens>2500;
	if (tk_utofq>=8) CutEcalCharge = ev.ecal_ens>3000;
	bMethodCut[2] = bMethodCut[2] && CutEcalCharge;*/
	bMethodCut[2] = true;
	
	#ifdef SUCCESSIVE
	//--Particle
	bTkSucc[0] = bPart;			//bPart
	bTkSucc[1] = bTkSucc[0] && bBeta;	//bBeta
	
	//--Track: no. hit, hit pattern and Inner chis
	bTkSucc[2] = bTkSucc[1] && (ev.GetTkInNHit()>=5);
	bool l2H = (ev.HasTkLHitXY(1)>0);
	bool l34H = ((ev.HasTkLHitXY(2)>0)||(ev.HasTkLHitXY(3)>0));
	bool l56H = ((ev.HasTkLHitXY(4)>0)||(ev.HasTkLHitXY(5)>0));
	bool l78H = ((ev.HasTkLHitXY(6)>0)||(ev.HasTkLHitXY(7)>0));
	bool innerHitYP = l2H && l34H && l56H && l78H;
	bTkSucc[3] = bTkSucc[2] && innerHitYP;
	//bTkSucc[4] = bTkSucc[3] && (ev.GetChis(0, 1) < 10); //bTrack[0]
	bTkSucc[4] = bTkSucc[3] && (ev.GetChis(0, 1, irig) < 10); //bTrack[0] //2023.03.27
	
	//--fiducial volume
	int Ex_mnhitf=0;
	for(int il=0+1;il<9-1;il++) if(ev.IsPassTkL(il)){Ex_mnhitf++;}
	
	bTkSucc[5] = bTkSucc[4] && (Ex_mnhitf>=5);
	bTkSucc[6] = bTkSucc[5] && bTkgeom[0]; //bTkgeom[0]
	
	//--new Q
	/*bTkSucc[7] = bTkSucc[6] && ev.Select_SubDetQ(tk_utofq, 0, QVer);
	if (tk_utofq>20 || tk_utofq%2==1 || (tk_utofq>=3 && tk_utofq<=5)) bTkSucc[7] = bTkSucc[6] && (ev.GetSubDetQ(0, QVer)>tk_utofq-2.5); //also separate for secondary
	#ifdef TKEFFNOQCUT
	bTkSucc[7] = bTkSucc[6] && (ev.GetSubDetQ(0, QVer)>tk_utofq-2.5);
	#endif //TKEFFNOQCUT
	bTkSucc[8] = bTkSucc[7] && ev.GetTkInQrmsNew(2, QVer) < ((tk_l1zu<9)?0.55:(0.57+0.1*(tk_l1zu-9))); //for now no need for ich<3*/
	//2023.02.20: apply Z>Q-2.5 for all nuclei and reverse the order with rms cut
	bTkSucc[7] = bTkSucc[6] && ev.Select_InnerQRMS(tk_utofq, QVer);
	bTkSucc[8] = bTkSucc[7] && (ev.GetSubDetQ(0, QVer)>tk_utofq-2.5);
	
	//2023.02.20: add event count InnerQ cut as additional successive cut, which is not in the bTkSel
	bTkSucc[9] = bTkSucc[8] && ev.Select_SubDetQ(tk_utofq, 0, QVer);
	#endif //SUCCESSIVE
	
	int iSkip=ev.CheckPrimary();
	bool bSkip=true;
	
	bool bRefQ0 = bRefQ;
	string sEff0="TkEff";
	
	//apply >Z-2.5 for all nuclei and add QRMS cut//2023.02.20
	bool bTKEffSel=false;
	bTKEffSel = bPart && bBeta; //particle
	//bTKEffSel = bTKEffSel && bTrack[0] && bTkgeom[0]; //fiducial volume and TrTrack selection
	bTKEffSel = bTKEffSel && bTrack[0][irig] && bTkgeom[0]; //fiducial volume and TrTrack selection //2023.03.27
	bTKEffSel = bTKEffSel && (ev.GetSubDetQ(0, QVer)>tk_utofq-2.5) && ev.Select_InnerQRMS(tk_utofq, QVer); //InnerQ and Qrms
	
	for (int isamt=-1; isamt<iNSam; isamt++)
	{
		if (!bRefQ0) break;
		
		double dTight=dTS[isamt+1];
		string sEff=sEff0;
		if (isamt!=-1) sEff=Form("%s%d", sEff0.c_str(), isamt);
		
		//------recalculate charge cut
		//--L1Q: +/- Z-Upper limit
		//l1Q = (fabs(ev.GetUBExtQBetaCor(0,2,QVer) -tk_utofq) < dTight*0.5);
		//2020.07.09: looser unbiased L1Q cut
		//l1Q = (fabs(ev.GetUBExtQBetaCor(0,2,QVer) -tk_utofq) < dTight*abs(tk_utofq-ev.GetQSelRange(1, tk_utofq, 4, QVer)));
		//2020.07.30: force L1Q upper cut
		l1Q = (fabs(ev.GetUBExtQBetaCor(0,2,QVer) -tk_utofq) < dTight*abs(tk_utofq-ev.GetQSelRange(1, tk_utofq, 4, QVer, true)));
		
		bL1Q = l1Q && l1Qs;
		
		//--L9Q, same as Event selection
		l9Q = (ev.GetUBExtQBetaCor(1,2,QVer) > tk_utofq-dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 5, QVer))) && (ev.GetUBExtQBetaCor(1,2,QVer) < tk_utofq + dTight*abs(tk_utofq-ev.GetQSelRange(1, tk_utofq, 5, QVer)));
		
		bL9Q = l9xy && l9Q;
		
		bExtQ[1] = bL1Q;
		bExtQ[2] = bL1Q && bL9Q;
		
		//--TOFQ
		//using UTOFQ as reference Q, already applied +/- 0.5 cut
		cuttofch[0]=true;
		cuttofch[1]=true;
		/*//event selection for UTOFQ applied for both unbiased Upper and Lower TOFQ
		for (int ilay=0;ilay<2;ilay++)
		{
			cuttofch[0] = cuttofch[0] && ((ev.tof_qls[ilay]>tk_utofq-dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 1, QVer))) && (ev.tof_qls[ilay]<tk_utofq + dTight*abs(tk_utofq-ev.GetQSelRange(1, tk_utofq, 1, QVer))));
			cuttofch[1] = cuttofch[1] && ((ev.tof_qls[ilay+2]>tk_utofq-dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 1, QVer))) && (ev.tof_qls[ilay+2]<tk_utofq + dTight*abs(tk_utofq-ev.GetQSelRange(1, tk_utofq, 1, QVer))));
		}*/
		//2019.11.01: symmetric TofQ cut, +/- lower UTOFQ cut
		/*for (int ilay=0;ilay<2;ilay++)
		{
			cuttofch[0] = cuttofch[0] && (fabs(ev.tof_qls[ilay]-tk_utofq)<dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 1, QVer)));
			cuttofch[1] = cuttofch[1] && (fabs(ev.tof_qls[ilay+2]-tk_utofq)<dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 1, QVer)));
		}*/
		//--compatiable measurement from two tof layers, difference between two tof layers should be smaller than lower UTOFQ cut (representing the width of Charge)
		cuttofch[0] = (fabs(ev.tof_qls[0]-ev.tof_qls[1])<dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 1, QVer)));
		cuttofch[1] = (fabs(ev.tof_qls[2]-ev.tof_qls[3])<dTight*abs(tk_utofq-ev.GetQSelRange(0, tk_utofq, 1, QVer)));
		
		//--LTofQ lower limit cut + loose upper limits
		double dubLTofQ=(ev.tof_qls[2]+ev.tof_qls[3])/2.;
		//cuttofch[1] = cuttofch[1] && dubLTofQ>(tk_utofq-dTight*0.5) && dubLTofQ<(tk_utofq+dTight*1.5); //[Z-0.5, Z+1.5]
		//2020.10.08: tighten the upper tofQ cut for secondary, where the below L1Q BG is non-negligible //2023.02.20: why this can remove below L1 BG? that fragments goes into same TOF counter and gives larger Q?
		if (tk_utofq%2==1 || (tk_utofq>=3 && tk_utofq<=5)) cuttofch[1] = cuttofch[1] && dubLTofQ>(tk_utofq-dTight*0.5) && dubLTofQ<(tk_utofq+dTight*0.5);
		else cuttofch[1] = cuttofch[1] && dubLTofQ>(tk_utofq-dTight*0.5) && dubLTofQ<(tk_utofq+dTight*1.5);
		//tighten the bRefQ selection as well, which is actually requiring abs(referenceQ-*it)<0.5, which is the same for isamt=-1
		bRefQ = bRefQ0 && abs(dRefQ-tk_utofq)<dTight*0.5;
		
		bTofQ[1] = cuttofch[0];
		bTofQ[2] = bTofQ[1] && cuttofch[1];
		
		if (bTRDTOFPar && bL1XY && bRefQ)
		{
			for (int ir=1; ir<3; ir++)
			{
				//if (ir>0) bSkip=iSkip>=((ir==1)?13:18); //up to L8 for L1Inner
				if (ir>0) bSkip=iSkip>=((ir==1)?12:18); //up to L7 for L1Inner
				else bSkip=true;
				
				if (weight[ir]==0) continue; //for l19, the weight for Inner & L1Inner pattern will be set to zero, hence will not fill those pattern
				if (bGeometry[ir] && bExtQ[ir] && bTofQ[2] && bToFChis)
				{
					//--new Q //define bTKEffSel before //2023.02.20
					/*bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && ev.Select_InnerQ(tk_utofq, QVer);
					if (tk_utofq>20 || tk_utofq%2==1 || (tk_utofq>=3 && tk_utofq<=5)) bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && (ev.GetSubDetQ(0, QVer)>tk_utofq-2.5);
					
					#ifdef TKEFFNOQCUT
					bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && (ev.GetSubDetQ(0, QVer)>tk_utofq-2.5);
					#endif //TKEFFNOQCUT*/
					
					//for (int imr=0; imr<=Mimr; imr++) //imr=0: beta, imr=1: cutoff, imr=2: Ecal
					for (int imr=-1; imr<=Mimr; imr++) //imr=-1: generated rigidity (only MC), imr=0: beta, imr=1: cutoff, imr=2: Ecal //2023.03.27
					{
						if (imr==-1 && ev.isreal) continue;
						else if (!bMethodCut[imr]) continue;
						
						string sSuffix = "";
						if (imr>=0) sSuffix = Form("_mr%d", imr);
						else sSuffix = "_gen";
						double dRigStore = (imr==-1)?ev.mmom/ev.mch:rigidity[imr];
						
						//sample
						histn = Form("rig_r%d_q%d_%sSam", ir, tk_utofq, sEff.c_str());
						histn += sSuffix;
						histn += sRig; //2023.03.26
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal && imr==-1)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%sSam", ir, tk_utofq, sEff.c_str());
							histn += sSuffix;
							histn += sRig;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
						} //ifrac
						#endif //EFFSEC
						
						#ifdef EFFMCTRUTH
						//------cleaness of the selected sample
						if (!ev.isreal && bSkip && find(viQSel.begin(), viQSel.end(), (int)ev.mch)!=viQSel.end())
						{
							histn = Form("rig_r%d_q%d_%sSamCleanness", ir, (int)ev.mch, sEff.c_str());
							histn += sSuffix;
							histn += sRig; //2023.03.26
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
							
							//Clean events which also passed numerator cut
							//selection
							if (bTKEffSel)
							{
								histn = Form("rig_r%d_q%d_%sSamCleanness&Sel", ir, tk_utofq, sEff.c_str());
								histn += sSuffix;
								histn += sRig; //2023.03.26
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
							}
						}
						#endif //EFFMCTRUTH
						
						//selection
						if (bTKEffSel)
						{
							histn = Form("rig_r%d_q%d_%sSel", ir, tk_utofq, sEff.c_str());
							histn += sSuffix;
							histn += sRig; //2023.03.26
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
							
							//2023.06.29
							#ifdef EFFSEC
							if (!ev.isreal && imr==-1)
							{
								int ifrac = -1;
								if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
								else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
								else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
								else ifrac=3;
								
								histn = Form("rig_r%d_q%d_%sSel", ir, tk_utofq, sEff.c_str());
								histn += sSuffix;
								histn += sRig;
								histn += sEffSecMCTruth[ifrac];
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
							} //ifrac
							#endif //EFFSEC
							
							//--efficiency with normalized tof chis cut
							for (int itofchisnorm=0; itofchisnorm<2; itofchisnorm++)
							{
								if (itofchisnorm==0)
								{
									if (ev.tof_chist>=5 || ev.tof_chisc>=5) continue;
								}
								else if (itofchisnorm==1)
								{
									if (ev.tof_chist_n>=5 || ev.tof_chisc_n>=5) continue;
								}
								histn = Form("rig_r%d_q%d_%sSel_tofchis%d", ir, tk_utofq, sEff.c_str(), itofchisnorm);
								histn += sSuffix;
								histn += sRig; //2023.03.26
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
							} //itofchisnorm
						}
					
						#ifdef SUCCESSIVE
						for (int is=0; is<nSuccTk; is++)
						{
							if (!bTkSucc[is]) break;
							
							histn = Form("rig_r%d_q%d_%sSel_s%d", ir, tk_utofq, sEff.c_str(), is);
							histn += sSuffix;
							histn += sRig; //2023.03.26
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
							
							//--store the InnerQ vs rig before and after applying InnerQ cut
							//if (is!=6 && is!=7) continue;
							//now the order is change and new InnerQ cut added, store the InnerQ after 1) rms cut, 2) >Z-2.5 and 3) event selection //2023.02.20
							if (is<7) continue;
							histn = Form("InnerQvsrig_r%d_q%d_%sSel_s%d", ir, tk_utofq, sEff.c_str(), is);
							histn += sSuffix;
							histn += sRig; //2023.03.26
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight[ir]);
						}
						#endif //SUCCESSIVE
					} //imr
				} //non-charge cuts
			} //ir
		} //non-pattern cuts
		
		//------successive sample
		#ifdef TKEFFSAMSUCC
		for (int ir=1; ir<3; ir++)
		{
			if (irig!=0) break;
			
			const int iTkEffSamCut=11;
			bool bTkEFFSamSucc[iTkEffSamCut];
			bTkEFFSamSucc[0] = bRefQ0;
			bTkEFFSamSucc[1] = ev.ibetahs>=0;
			bTkEFFSamSucc[2] = ev.itrdtracks>=0;
			bTkEFFSamSucc[3] = ev.betahs>0.4;
			bTkEFFSamSucc[4] = bL1XY;
			bTkEFFSamSucc[5] = bGeometry[ir];
			bTkEFFSamSucc[6] = bExtQ[ir];
			bTkEFFSamSucc[7] = bTofQ[1];
			bTkEFFSamSucc[8] = bTofQ[2];
			bTkEFFSamSucc[9] = ev.tof_chiscs<20;
			bTkEFFSamSucc[10] = ev.tof_chists<20;
			
			//--N-1 cut
			bool bTkEFFSamN1[iTkEffSamCut];
			for (int iCut=0; iCut<iTkEffSamCut; iCut++)
			{
				bTkEFFSamN1[iCut]=true;
				for (int iCut0=0; iCut0<iTkEffSamCut; iCut0++)
				{
					if (iCut0==iCut) continue;
					bTkEFFSamN1[iCut] = bTkEFFSamN1[iCut] && bTkEFFSamSucc[iCut0];
				} //iCut0
			} //iCut
			
			for (int iCut=0; iCut<iTkEffSamCut; iCut++)
			{
				if (!bTkEFFSamN1[iCut]) continue;
				histn = Form("rig_r%d_q%d_%sSam_N1%d_gen", ir, tk_utofq, sEff.c_str(), iCut);
				GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
			} //iCut
			
			//--successive
			for (int is=0; is<iTkEffSamCut; is++)
			{
				if (!bTkEFFSamSucc[is]) break;
				histn = Form("rig_r%d_q%d_%sSam_s%d_gen", ir, tk_utofq, sEff.c_str(), is);
				GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
			}
		} //ir
		#endif //TKEFFSAMSUCC
	} //isamt
	
	//move the sample only using non-fragment before charge determination
	#ifdef EFFMCTRUTH
	if (ev.isreal) return true;
	
	//2023.02.20
	//no need to require L1XY and L9XY, but this will ensure the exact active region as event selection as there might be some dead region in L1 and L9
	//--
	int iMCZ = (int)ev.mch;
	//apply >Z-2.5 for all nuclei and add QRMS cut//2023.02.20
	bTKEffSel = bPart && bBeta; //particle
	//bTKEffSel = bTKEffSel && bTrack[0] && bTkgeom[0]; //fiducial volume and TrTrack selection
	bTKEffSel = bTKEffSel && bTrack[0][irig] && bTkgeom[0]; //fiducial volume and TrTrack selection //2023.03.27
	bTKEffSel = bTKEffSel && (ev.GetSubDetQ(0, QVer)>iMCZ-2.5) && ev.Select_InnerQRMS(iMCZ, QVer); //InnerQ and Qrms
	
	for (int iextcut=0; iextcut<2; iextcut++) //iextcut=0: no L1XY (and L9XY for FS) cut, =1: add external XY cut
	{
		if (!ev.isreal && *it!=ev.mch) break; //2023.06.18
		bool bPresel = bTRDTOFPar && !ev.isreal;
		if (!bPresel) continue;
		
		for (int ir=1; ir<3; ir++)
		{
			if (ir>0) bSkip=iSkip>=((ir==1)?12:18);
			else bSkip=true;
			
			if (weight[ir]==0) continue; //for l19, the weight for Inner & L1Inner pattern will be set to zero, hence will not fill those pattern
			
			//if (ir==2 && !l9xy) continue; //not necessary, as MC Truth is already enforced and doesn't matter whether L9 has detection or not, this also increases the sample size as L9 reconstruction eff is not included. However, might create a small difference as this will ensure that particle passing through active region of L9 hence has exactly the same geometry as real event seleciton, maybe is good to keep it. store the efficiency with and ewithout external cut
			//2023.02.20
			bool bExtCut = true;
			if (iextcut==1)
			{
				bExtCut = bL1XY;
				if (ir==2) bExtCut = bExtCut && l9xy;
			}
			
			//--
			if (!(bGeometry[ir] && bToFChis && bSkip && bExtCut)) continue; //non-charge cuts
			
			//--new Q
			/*bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && ev.Select_InnerQ(iMCZ, QVer);
			//separate the InnerQ efficiency fo Iron and secondary
			//if (iMCZ>20 || iMCZ%2==1) bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && (ev.GetSubDetQ(0, QVer)>iMCZ-2.5);
			if (iMCZ>20 || iMCZ%2==1 || (iMCZ>=3 && iMCZ<=5)) bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && (ev.GetSubDetQ(0, QVer)>iMCZ-2.5); //2021.01.17
			#ifdef TKEFFNOQCUT
			bTKEffSel=bPart && bBeta && bTrack[0] && bTkgeom[0] && (ev.GetSubDetQ(0, QVer)>iMCZ-2.5);
			#endif //TKEFFNOQCUT*/
			
			//for (int imr=0; imr<=Mimr; imr++) //imr=0: beta, imr=1: cutoff, imr=2: Ecal
			for (int imr=-1; imr<=Mimr; imr++) //imr=-1: generated rigidity, imr=0: beta, imr=1: cutoff, imr=2: Ecal
			{
				//if (!bMethodCut[imr]) continue;
				//2023.03.27
				if (imr==-1 && ev.isreal) continue;
				else if (!bMethodCut[imr]) continue;
			
				string sSuffix = "";
				if (imr>=0) sSuffix = Form("_mr%d", imr);
				else sSuffix = "_gen";
				double dRigStore = (imr==-1)?ev.mmom/ev.mch:rigidity[imr];
				
				//sample
				histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, iMCZ, sEff0.c_str());
				histn += sSuffix;
				histn += Form("_extcut%d", iextcut); //2023.02.20
				histn += sRig; //2023.03.26
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
				
				//selection
				if (bTKEffSel)
				{
					histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, iMCZ, sEff0.c_str());
					histn += sSuffix;
					histn += Form("_extcut%d", iextcut); //2023.02.20
					histn += sRig; //2023.03.26
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
				}
			
				#ifdef SUCCESSIVE
				for (int is=0; is<nSuccTk; is++)
				{
					if (!bTkSucc[is]) break;
					histn = Form("rig_r%d_q%d_%sMCTruthSel_s%d", ir, iMCZ, sEff0.c_str(), is);
					histn += sSuffix;
					histn += Form("_extcut%d", iextcut); //2023.02.20
					histn += sRig; //2023.03.26
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight[ir]);
				}
				#endif //SUCCESSIVE
			} //imr
		} //ir
	} //iextcut
	#endif //EFFMCTRUTH
	
	return true;
} //SelTkEffN

//------Inner Tracker Charge efficiency
#ifdef AMSSOFT
bool BookHistoTkQEff(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false)
#else
//bool BookHistoTkQEff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int irig, const int QVer, const bool dir=false) //2023.02.19
bool BookHistoTkQEff(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false)
#endif //AMSSOFT
{
	string sDir=Form("TkQ_Efficiency%d", QVer);
	//if (dir) outfile.mkdir(sDir.c_str());
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		for (int isamt=-1; isamt<iNSam; isamt++)
		{
			#ifdef SELCHECK
			if (isamt!=-1) continue;
			#endif ///SELCHECK

			string sDirQ;
			if (isamt==-1) sDirQ=Form("%s/q%d", sDir.c_str(), *it);
			else  sDirQ=Form("%s/q%d_samt%d", sDir.c_str(), *it, isamt);
			string sEff="TkQEff";
			if (isamt!=-1) sEff=Form("TkQEff%d", isamt);
			
			if (dir)
			{
				outfile.mkdir(Form("%s", sDirQ.c_str()));
				outfile.cd(Form("%s", sDirQ.c_str()));
			}
			for (int ir=1; ir<3; ir++)
			{
				for (int irig=0; irig<NRig; irig++)
				{
					#ifdef SELCHECK
					if (irig!=0) continue;
					#endif ///SELCHECK
					
					#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
					if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
					#endif //PASS7GBL
					
					string sRig=sRigName[irig];
					if (dir)
					{
						//outfile.mkdir(Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str()));
						//outfile.cd(Form("%s/%s", sDirQ.c_str(), sSpan[ir].c_str()));
						//2023.03.21
						outfile.mkdir(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
						outfile.cd(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
					}
					
					//--charge distribution for sample
					histn = Form("InnerQvsrig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
					histn += sRig; //2023.03.21
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					for (int ilay=0; ilay<4; ilay++)
					{
						histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, ir, *it, sEff.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					} //ilay
					histn = Form("UTOFQvsrig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
					histn += sRig; //2023.03.21
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					histn = Form("LTOFQvsrig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
					histn += sRig; //2023.03.21
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					histn = Form("L1Qvsrig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
					histn += sRig; //2023.03.21
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					histn = Form("L9Qvsrig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
					histn += sRig; //2023.03.21
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					
					//--InnerQ after selection
					histn = Form("InnerQvsrig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
					histn += sRig; //2023.03.21
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					
					for (int igr=1; igr>=0; igr--)
					{
						if (igr==0 && ev.isreal) continue;
						
						/*histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
						histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));*/
						
						//--study the angle dependence of InnerQEff by comparing L1Inner in FS geom and etc. //2023.07.12
						for (int ifsgeom=-1; ifsgeom<2; ifsgeom++)
						{
							if (ir>1 && ifsgeom!=-1) break;
							
							string sEffSuffix = "";
							if (ifsgeom>=0) sEffSuffix+="_FSGeom";
							if (ifsgeom>=1) sEffSuffix+="_UnbiasedL9QCut";
							
							for (int idn=0; idn<2; idn++)
							{
								histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
								histn += sEffSuffix;
								histn += (idn==0)?"Sam":"Sel";
								if (igr==0) histn += "_gen";
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //idn
							
							//2023.06.29
							#ifdef EFFSEC
							for (int ifrac=0; ifrac<4; ifrac++)
							{
								if (ev.isreal) break;
								
								for (int isamsel=0; isamsel<2; isamsel++)
								{
									histn = Form("rig_r%d_q%d_%s", ir, *it, sEff.c_str());
									histn += sEffSuffix;
									histn += (isamsel==0)?"Sam":"Sel";
									if (igr==0) histn += "_gen";
									histn += sRig;
									histn += sEffSecMCTruth[ifrac];
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								} //isamsel
							} //ifrac
							#endif //EFFSEC
						} //ifsgeom
					}
					
					//2023.02.19
					#ifdef EFFMCTRUTH
					if (dir) outfile.cd(Form("%s/%s%s", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
					
					for (int igr=0; igr<2; igr++)
					{
						//if (ev.isreal || *it!=(int)ev.mch) break;
						if (!(!ev.isreal && *it==(int)ev.mch)) break;
						
						//------cleaness of the selected sample
						histn = Form("rig_r%d_q%d_%sSamCleanness", ir, *it, sEff.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig;
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						//------sample seletec by MC truth
						if (isamt!=-1) continue; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
						for (int isample=0; isample<2; isample++) //isample=0: only with MC truth cut, =1: normal sample cut+MC truth cut
						{
							string sMCTruthSam = (isample==0)?"":"_SamCut";
							histn = Form("rig_r%d_q%d_%sMCTruthSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sMCTruthSam; //2023.05.08
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							#ifdef EFFMCTRUTHQDIS //2023.05.08
							histn = Form("InnerQvsrig_r%d_q%d_%sMCTruthSam", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sMCTruthSam;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							#endif //EFFMCTRUTHQDIS
							
							histn = Form("rig_r%d_q%d_%sMCTruthSel", ir, *it, sEff.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sMCTruthSam; //2023.05.08
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						} //isample
					} //igr
					#endif //EFFMCTRUTH

					////2023.09.21
					#ifdef SELCHECK
					for (int isam=0; isam<2; isam++){
						if(dir){
							outfile.mkdir(Form("%s/%s%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), isam));
							outfile.cd(Form("%s/%s%s/SC%d", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str(), isam));
						}

						histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 3500, 0, 35));

						histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d", ir, *it, sEff.c_str(), isam);
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 10));

						if(isam == 0) continue;

						histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d_wrms", ir, *it, sEff.c_str(), isam);
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

						histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d_wInnerq", ir, *it, sEff.c_str(), isam);
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 350, 0, 35));

						histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d_wrms", ir, *it, sEff.c_str(), isam);
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 10));

						histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d_wInnerq", ir, *it, sEff.c_str(), isam);
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14), 1000, 0, 10));
					}
					#endif //SELCHECK
					
					/*//--TkQEff withou L1QStatus
					string sEffwoL1QS = sEff+"woL1QS";
					histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffwoL1QS.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					histn = Form("InnerQvsrig_r%d_q%d_%sSam", ir, *it, sEffwoL1QS.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					for (int ilay=0; ilay<4; ilay++)
					{
						histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, ir, *it, sEffwoL1QS.c_str());
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					} //ilay
					
					histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffwoL1QS.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					histn = Form("InnerQvsrig_r%d_q%d_%sSel", ir, *it, sEffwoL1QS.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
				
					if (!ev.isreal)
					{
						histn = Form("rig_r%d_q%d_%sSam_gen", ir, *it, sEffwoL1QS.c_str());
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
						histn = Form("rig_r%d_q%d_%sSel_gen", ir, *it, sEffwoL1QS.c_str());
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					}
					
					//--TkQEff without L9Q //2020.07.20
					if (ir<2) continue;
					string sEffwoL9Q = sEff+"woL9Q";
					histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffwoL9Q.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					histn = Form("InnerQvsrig_r%d_q%d_%sSam", ir, *it, sEffwoL9Q.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					for (int ilay=0; ilay<4; ilay++)
					{
						histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, ir, *it, sEffwoL9Q.c_str());
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					} //ilay
					
					histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffwoL9Q.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					histn = Form("InnerQvsrig_r%d_q%d_%sSel", ir, *it, sEffwoL9Q.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
				
					if (!ev.isreal)
					{
						histn = Form("rig_r%d_q%d_%sSam_gen", ir, *it, sEffwoL9Q.c_str());
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
						histn = Form("rig_r%d_q%d_%sSel_gen", ir, *it, sEffwoL9Q.c_str());
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					}*/
					
					//----TkQEff check, for the difference between L1Inner and FS geometry //2020.07.31
					#ifdef TKQEFFCHECK //2020.11.14
					if (isamt!=-1) continue; //2020.08.03
					const string sTkQEffCheck[] = {"woL1QS", "woL1Q", "woUTOFQ", "woLTOFQ", "woInnerQ", "woL9Q"};
					if (dir)
					{
						outfile.mkdir(Form("%s/%s%s/TkQEffCheck", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
						outfile.cd(Form("%s/%s%s/TkQEffCheck", sDirQ.c_str(), sSpan[ir].c_str(), sRig.c_str()));
					}
					
					for (int iCheck=0; iCheck<6; iCheck++)
					{
						if (iCheck==5 && ir<2) continue;
						string sEffCheck = sEff+sTkQEffCheck[iCheck];
						
						//--charge distribution
						//--denominator
						histn = Form("InnerQvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						for (int ilay=0; ilay<4; ilay++)
						{
							histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						} //ilay
						histn = Form("UTOFQvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
						histn = Form("LTOFQvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
						histn = Form("L1Qvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
						histn = Form("L9Qvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
						
						//--numerator
						histn = Form("InnerQvsrig_r%d_q%d_%sSel", ir, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
					
						for (int igr=1; igr>=0; igr--)
						{
							histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffCheck.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						} //igr
					} //iCheck
					
					//--TkQEff check, L1Inner + FS cuts //2020.07.31
					//const string sTkQEffCheck2[] = {"FSTrack", "FSTkGeom"};
					const string sTkQEffCheck2[] = {"FSL9XY", "FSL9Chis", "FSTkGeom"}; //separate it further
					for (int iCheck=0; iCheck<3; iCheck++)
					{
						
						if (ir==2) continue;
						
						for (int iCheck0=0; iCheck0<2; iCheck0++)
						{
							string sEffCheck = sEff+((iCheck0==0)?"":"not")+sTkQEffCheck2[iCheck];
							
							//--charge distribution
							//--denominator
							histn = Form("InnerQvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							for (int ilay=0; ilay<4; ilay++)
							{
								histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, ir, *it, sEffCheck.c_str());
								histn += sRig; //2023.03.21
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							} //ilay
							
							histn = Form("UTOFQvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							histn = Form("LTOFQvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							histn = Form("L1Qvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							histn = Form("L9Qvsrig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							//--numerator
							histn = Form("InnerQvsrig_r%d_q%d_%sSel", ir, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
							
							//--
							for (int igr=1; igr>=0; igr--)
							{
								histn = Form("rig_r%d_q%d_%sSam", ir, *it, sEffCheck.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
								histn = Form("rig_r%d_q%d_%sSel", ir, *it, sEffCheck.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							} //igr
						} //iCheck0
					} //iCheck
					#endif //TKQEFFCHECK //2020.11.14
				} //irig
			} //ir
		} //isamt
	} //it
	return true;
} //BookHistoTkQEff

#ifdef AMSSOFT
bool SelTkQEff(HistoMan &hman1, SelEvent &ev, const int span, const double weight=1., const int QVer=0)
#else
bool SelTkQEff(TObjArray &hman1, SelEvent &ev, const int span, const int irig=0, const double weight=1., const int QVer=0) //2023.02.19
//bool SelTkQEff(TObjArray &hman1, SelEvent &ev, const int span, const double weight=1., const int QVer=0)
#endif //AMSOFT
{
	#ifdef SELCHECK
	if (irig!=0) return true;
	#endif ///SELCHECK

	string sRig = sRigName[irig];
	bool bTkQEffSam=false;
	bool bTkQEffSel=false;
	
	bool bHZ=false;
	if (span<1) return false;
	
	//2023.02.19
	int iSkip=ev.CheckPrimary();
	//bool bSkip=iSkip>=13; //L8
	bool bSkip=iSkip>=12; //L7
	
	//--
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		//first layer L1 Q as reference charge, first check whether the reference charge of each event is inside viQSel or not
		//bHZ = (*it==int(ev.GetTkLQNew(0, 2, QVer)+0.5));
		//use utofq as reference Q //2020.07.09
		bHZ = (*it==int(ev.GetTOFUDQ(0)+0.5));
		
		if (!bHZ) continue;
		
		//--same selection for all samples //2023.02.19
		bTkQEffSel = ev.Select_InnerQ(*it, QVer);
		
		for (int isamt=-1; isamt<iNSam; isamt++)
		{
			#ifdef SELCHECK
			if (isamt!=-1) continue;
			#endif ///SELCHECK

			double dTight=dTS[isamt+1];
			string sEff="TkQEff";
			if (isamt!=-1) sEff=Form("TkQEff%d", isamt);
			
			//--looser sample charge selection
			//--L1Q: [Z-0.5, Z+tighter than event selection], lower limit is important, as the charge is mainly selected by TOFs, while TOFQ has long tail and lower charge might pass by the cuts even with both Upper and Lower TOF
			//put a tighter L1Q upper limit
			bTkQEffSam = ev.GetTkLQNew(0,2,QVer) < *it + 0.65*dTight*abs(*it-ev.GetQSelRange(1, *it, 4, QVer, true)) && ev.GetTkLQNew(0,2,QVer) > *it - dTight*0.5;
			
			//--tofQ
			//1. already requiring UTOFQ within +/- 0.5 when using UTOFQ as reference Q
			//2. 2 tof layer Q to be compatiable for upper and lower tof
			bTkQEffSam = bTkQEffSam && (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
			bTkQEffSam = bTkQEffSam && (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(3))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer)));
			//3. tof path length
			for (int iud=0; iud<2; iud++) bTkQEffSam = bTkQEffSam && ev.IsGoodTofQUD(iud);
			//4. LTofQ cut: [Z-0.5, Z+1.5] for primary, and +/-0.5 for secondary to remove higher Z
			if (*it%2==1 || (*it>=3 && *it<=5)) bTkQEffSam = bTkQEffSam && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*0.5);
			else bTkQEffSam = bTkQEffSam && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*1.5);
			//5. tighten the lower limits to further clean up the lower charge, i.e. >Z-0.3
			if (*it%2==1 || (*it>=3 && *it<=5)) bTkQEffSam = bTkQEffSam && ev.GetTOFUDQ(0) > (*it-dTight*0.3) && ev.GetTOFUDQ(1) > (*it-dTight*0.3);
			
			//--sample selection without L9Q //2020.07.20
			bool bTkQEffSamwoL9Q = bTkQEffSam && ev.Select_TkLQStatus(0);
			
			//--L9Q (FS), same as event selection
			if (span>=2) bTkQEffSam = bTkQEffSam && (ev.GetTkLQNew(8,2,QVer) > *it-dTight*abs(*it-ev.GetQSelRange(0, *it, 5, QVer))) && (ev.GetTkLQNew(8,2,QVer) < *it + dTight*abs(*it-ev.GetQSelRange(1, *it, 5, QVer)));
			
			//--loose inner q cut, same as in the numerator for Inner Tracking Eff
			bTkQEffSam = bTkQEffSam && ev.GetTkInQNew(2,QVer)>*it-2.5;
			
			if (bTkQEffSam && ev.Select_TkLQStatus(0))
			{
				for (int igr=1; igr>=0; igr--)
				{
					if (igr==0 && ev.isreal) continue;
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
					
					//--
					if (igr==1)
					{
						histn = Form("InnerQvsrig_r%d_q%d_%sSam", span, *it, sEff.c_str());
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);
						
						for (int ilay=0; ilay<4; ilay++)
						{
							histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, span, *it, sEff.c_str());
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTofLQ(ilay), weight);
						} //ilay
						
						histn = Form("UTOFQvsrig_r%d_q%d_%sSam", span, *it, sEff.c_str());
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(0), weight);
						
						histn = Form("LTOFQvsrig_r%d_q%d_%sSam", span, *it, sEff.c_str());
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
						
						histn = Form("L1Qvsrig_r%d_q%d_%sSam", span, *it, sEff.c_str());
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0,2,QVer), weight);
						
						histn = Form("L9Qvsrig_r%d_q%d_%sSam", span, *it, sEff.c_str());
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8,2,QVer), weight);

						////2023.09.21
						#ifdef SELCHECK
						histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d", span, *it, sEff.c_str(), 0);
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);

						histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d", span, *it, sEff.c_str(), 0);
						histn += sRig;
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkInQrmsNew(2, QVer), weight);
						#endif //SELCHECK
					}
					
					//--study the angle dependence of InnerQEff by comparing L1Inner in FS geom and etc. //2023.07.12
					for (int ifsgeom=-1; ifsgeom<2; ifsgeom++)
					{
						if (span>1 && ifsgeom!=-1) break;
						//FSGeom cut
						if (ifsgeom>=0 && !bTkgeom[2]) break;
						//unbiased L9XY + unbiased L9Q cut
						if (ifsgeom>=1)
						{
							if (!(ev.IsUBXYExtQTrkCor(1, QVer) && (ev.GetUBExtQTrkCor(1,2,QVer) > *it-dTight*abs(*it-ev.GetQSelRange(0, *it, 5, QVer))) && (ev.GetUBExtQTrkCor(1,2,QVer) < *it + dTight*abs(*it-ev.GetQSelRange(1, *it, 5, QVer))))) break;
						}
						
						string sEffSuffix = "";
						if (ifsgeom>=0) sEffSuffix+="_FSGeom";
						if (ifsgeom>=1) sEffSuffix+="_UnbiasedL9QCut";
						
						histn = Form("rig_r%d_q%d_%s", span, *it, sEff.c_str());
						histn += sEffSuffix;
						histn += "Sam";
						if (igr==0) histn += "_gen";
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						//2023.06.29
						#ifdef EFFSEC
						if (!ev.isreal)
						{
							int ifrac = -1;
							if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
							else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
							else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
							else ifrac=3;
							
							histn = Form("rig_r%d_q%d_%s", span, *it, sEff.c_str());
							histn += sEffSuffix;
							histn += "Sam";
							if (igr==0) histn += "_gen";
							histn += sRig;
							histn += sEffSecMCTruth[ifrac];
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						} //ifrac
						#endif //EFFSEC
						
						if (bTkQEffSel)
						{
							histn = Form("rig_r%d_q%d_%s", span, *it, sEff.c_str());
							histn += sEffSuffix;
							histn += "Sel";
							if (igr==0) histn += "_gen";
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							
							//2023.06.29
							#ifdef EFFSEC
							if (!ev.isreal)
							{
								int ifrac = -1;
								if (ev.PrimaryUpToTrLayer(8)) ifrac=0;
								else if (ev.PrimaryUpToTrLayer(2)) ifrac=1;
								else if (ev.PrimaryUpToTrLayer(1)) ifrac=2;
								else ifrac=3;
								
								histn = Form("rig_r%d_q%d_%s", span, *it, sEff.c_str());
								histn += sEffSuffix;
								histn += "Sel";
								if (igr==0) histn += "_gen";
								histn += sRig;
								histn += sEffSecMCTruth[ifrac];
								GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							} //ifrac
							#endif //EFFSEC
							
							if (igr==1 && ifsgeom==-1)
							{
								histn = Form("InnerQvsrig_r%d_q%d_%sSel", span, *it, sEff.c_str());
								histn += sRig;
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);

								#ifdef SELCHECK
								histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d", span, *it, sEff.c_str(), 1);
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);

								histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d", span, *it, sEff.c_str(), 1);
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkInQrmsNew(2, QVer), weight);
								#endif ///SELCHECK
							}
						}
						#ifdef SELCHECK
						bool bTkQEffSel_q = ev.Select_SubDetQ(*it, 0, QVer);
						bool bTkQEffSel_rms = ev. Select_InnerQRMS(*it, QVer);
						if(bTkQEffSel_q){
							if(igr==1 && ifsgeom==-1){

								histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d_wInnerq", span, *it, sEff.c_str(), 1);
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);

								histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d_wInnerq", span, *it, sEff.c_str(), 1);
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkInQrmsNew(2, QVer), weight);
							}
						}
						if(bTkQEffSel_rms){
							if(igr==1 && ifsgeom==-1){

								histn = Form("RMSvsRigHZBin_r%d_q%d_%s%d_wrms", span, *it, sEff.c_str(), 1);
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkInQrmsNew(2, QVer), weight);

								histn = Form("InnerQvsRigHZBin_r%d_q%d_%s%d_wrms", span, *it, sEff.c_str(), 1);
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);
							}
						}
						#endif ///SELCHECK
					} //ifsgeom
				} //igr
				
				//2023.02.19
				#ifdef EFFMCTRUTH
				//------cleaness of the selected sample
				for (int igr=0; igr<2; igr++)
				{
					if (!(!ev.isreal && *it==(int)ev.mch && bSkip)) break;
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
					
					histn = Form("rig_r%d_q%d_%sSamCleanness", span, *it, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig;
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				} //igr
				
				//------sample seletec by MC truth & normal sample cut, isample=1
				for (int igr=0; igr<2; igr++)
				{
					string sMCTruthSam = "_SamCut";
					if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1 && bSkip)) break; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
					
					histn = Form("rig_r%d_q%d_%sMCTruthSam", span, *it, sEff.c_str());
					if (igr==1) histn += "_gen";
					histn += sRig;
					histn += sMCTruthSam;
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					#ifdef EFFMCTRUTHQDIS //2023.05.08
					histn = Form("InnerQvsrig_r%d_q%d_%sMCTruthSam", span, *it, sEff.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig;
					histn += sMCTruthSam;
					GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkInQNew(2,QVer), weight);
					#endif //EFFMCTRUTHQDIS
					
					histn = Form("rig_r%d_q%d_%sMCTruthSel", span, *it, sEff.c_str());
					if (igr==1) histn += "_gen";
					histn += sRig;
					histn += sMCTruthSam;
					if (bTkQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				} //igr
				#endif //EFFMCTRUTH
					
				#ifdef TKQEFFCHECK
				const string sTkQEffCheck2[] = {"FSL9XY", "FSL9Chis", "FSTkGeom"}; //separate it further
				for (int igr=1; igr>=0; igr--)
				{
					if (igr==0 && ev.isreal) continue;
					if (isamt!=-1) break; 
					if (span==2) break;
						
					double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
					
					for (int iCheck=0; iCheck<3; iCheck++)
					{
						bool bCheck=true;
						if (iCheck==0) bCheck = (ev.HasTkLHitXY(8)==2);
						//else if (iCheck==1) bCheck = ev.GetChis(2, 1)<10;
						else if (iCheck==1) bCheck = ev.GetChis(2, 1, irig)<10; //2023.03.27
						else if (iCheck==2) bCheck = ev.IsPassTkL(8);
						
						string sEffCheck = sEff+(bCheck?"":"not")+sTkQEffCheck2[iCheck];

						if (igr==1)
						{
							histn = Form("InnerQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);
							
							for (int ilay=0; ilay<4; ilay++)
							{
								histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, span, *it, sEffCheck.c_str());
								histn += sRig;
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTofLQ(ilay), weight);
							} //ilay
							
							histn = Form("UTOFQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(0), weight);
							
							histn = Form("LTOFQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
							
							histn = Form("L1Qvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0,2,QVer), weight);
							 
							histn = Form("L9Qvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
							histn += sRig;
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8,2,QVer), weight);
						}
						
						//--
						histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						if (bTkQEffSel)
						{
							histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffCheck.c_str());
							if (igr==0) histn += "_gen";
							histn += sRig; //2023.03.21
							GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
							
							if (igr==1)
							{
								histn = Form("InnerQvsrig_r%d_q%d_%sSel", span, *it, sEffCheck.c_str());
								if (igr==0) histn += "_gen";
								histn += sRig; //2023.03.21
								GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);
							}
						}
					} //iCheck
				} //igr
				#endif //TKQEFFCHECK
			}
			
			/*//----TkQEff withou L1Q status
			if (bTkQEffSam)
			{
				string sEffwoL1QS = sEff+"woL1QS";
				
				histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffwoL1QS.c_str());
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
				histn = Form("InnerQvsrig_r%d_q%d_%sSam", span, *it, sEffwoL1QS.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetSubDetQ(0, QVer), weight);
				for (int ilay=0; ilay<4; ilay++)
				{
					histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, span, *it, sEffwoL1QS.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTofLQ(ilay), weight);
				} //ilay
				
				histn = Form("rig_r%d_q%d_%sSam_gen", span, *it, sEffwoL1QS.c_str());
				if (!ev.isreal) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, weight);
				
				if (bTkQEffSel)
				{
					histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffwoL1QS.c_str());
					GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
					histn = Form("InnerQvsrig_r%d_q%d_%sSel", span, *it, sEffwoL1QS.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetSubDetQ(0, QVer), weight);
				
					histn = Form("rig_r%d_q%d_%sSel_gen", span, *it, sEffwoL1QS.c_str());
					if (!ev.isreal) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, weight);
				}
			}
			
			//----TkQEff without L9Q cut for FullSpan //2020.07.20
			if (bTkQEffSamwoL9Q && span==2)
			{
				string sEffwoL9Q = sEff+"woL9Q";
				
				histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffwoL9Q.c_str());
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
				histn = Form("InnerQvsrig_r%d_q%d_%sSam", span, *it, sEffwoL9Q.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetSubDetQ(0, QVer), weight);
				for (int ilay=0; ilay<4; ilay++)
				{
					histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, span, *it, sEffwoL9Q.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTofLQ(ilay), weight);
				} //ilay
				
				histn = Form("rig_r%d_q%d_%sSam_gen", span, *it, sEffwoL9Q.c_str());
				if (!ev.isreal) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, weight);
				
				if (bTkQEffSel)
				{
					histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffwoL9Q.c_str());
					GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
					histn = Form("InnerQvsrig_r%d_q%d_%sSel", span, *it, sEffwoL9Q.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetSubDetQ(0, QVer), weight);
				
					histn = Form("rig_r%d_q%d_%sSel_gen", span, *it, sEffwoL9Q.c_str());
					if (!ev.isreal) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, weight);
				}
			}*/
			
			//------sample selected by MC truth
			#ifdef EFFMCTRUTH
			for (int igr=1; igr>=0; igr--)
			{
				if (!(!ev.isreal && *it==(int)ev.mch && isamt==-1 && bSkip)) break; //only store once since will not selected by charge cut so the tightness of the sample charge cut have no influence
				double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(span, irig);
				
				histn = Form("rig_r%d_q%d_%sMCTruthSam", span, *it, sEff.c_str());
				if (igr==1) histn += "_gen";
				histn += sRig;
				GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
				
				#ifdef EFFMCTRUTHQDIS //2023.05.08
				histn = Form("InnerQvsrig_r%d_q%d_%sMCTruthSam", span, *it, sEff.c_str());
				if (igr==0) histn += "_gen";
				histn += sRig;
				GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkInQNew(2,QVer), weight);
				#endif //EFFMCTRUTHQDIS
				
				histn = Form("rig_r%d_q%d_%sMCTruthSel", span, *it, sEff.c_str());
				if (igr==1) histn += "_gen";
				histn += sRig;
				if (bTkQEffSel) GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
			} //igr
			#endif //EFFMCTRUTH
			
			//----TkQEff Check
			#ifdef TKQEFFCHECK
			if (isamt!=-1) continue;
			const string sTkQEffCheck[] = {"woL1QS", "woL1Q", "woUTOFQ", "woLTOFQ", "woInnerQ", "woL9Q"};
			
			bool bTkQEffCheck[6];
			//--L1QS
			bTkQEffCheck[0] = ev.Select_TkLQStatus(0);
			//--L1Q
			bTkQEffCheck[1] = ev.GetTkLQNew(0,2,QVer) < *it + dTight*abs(*it-ev.GetQSelRange(1, *it, 4, QVer, true)) && ev.GetTkLQNew(0,2,QVer) > *it - dTight*0.5;
			//--UTOFQ
			bTkQEffCheck[2] = (fabs(ev.GetTofLQ(0)-ev.GetTofLQ(1))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer))) && ev.IsGoodTofQUD(0);
			//--LTOFQ
			bTkQEffCheck[3] = (fabs(ev.GetTofLQ(2)-ev.GetTofLQ(3))<dTight*abs(*it-ev.GetQSelRange(0, *it, 1, QVer))) && ev.IsGoodTofQUD(1) && ev.GetTOFUDQ(1) > (*it-dTight*0.5) && ev.GetTOFUDQ(1) < (*it+dTight*1.5);
			//--InnerQ
			bTkQEffCheck[4] = ev.GetTkInQNew(2,QVer)>*it-2.5;
			//--L9Q
			bTkQEffCheck[5] = bTkQEffSam = bTkQEffSam && (ev.GetTkLQNew(8,2,QVer) > *it-dTight*abs(*it-ev.GetQSelRange(0, *it, 5, QVer))) && (ev.GetTkLQNew(8,2,QVer) < *it + dTight*abs(*it-ev.GetQSelRange(1, *it, 5, QVer)));
			
			for (int igr=1; igr>=0; igr--)
			{
				for (int iCheck=0; iCheck<6; iCheck++)
				{
					if (iCheck==5 && span<2) continue;
					
					bool bCheck=true;
					for (int iCheck0=0; iCheck0<6; iCheck0++)
					{
						if (iCheck0==5 && span<2) continue;
						if (iCheck==iCheck0) continue;
						bCheck = bCheck && bTkQEffCheck[iCheck0];
					}
					
					if (!bCheck) continue;
					
					string sEffCheck = sEff+sTkQEffCheck[iCheck];
					
					if (igr==1)
					{
						histn = Form("InnerQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);
						
						for (int ilay=0; ilay<4; ilay++)
						{
							histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, span, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTofLQ(ilay), weight);
						} //ilay
						
						histn = Form("UTOFQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(0), weight);
						
						histn = Form("LTOFQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTOFUDQ(1), weight);
						
						histn = Form("L1Qvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(0,2,QVer), weight);
						
						histn = Form("L9Qvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
						histn += sRig; //2023.03.21
						GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetTkLQNew(8,2,QVer), weight);
					}
					
					histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
					if (igr==0) histn += "_gen";
					histn += sRig; //2023.03.21
					GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
					
					if (bTkQEffSel)
					{
						histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffCheck.c_str());
						if (igr==0) histn += "_gen";
						histn += sRig; //2023.03.21
						GetHist(hman1, histn.c_str())->Fill(dRigStore, weight);
						
						if (igr==1)
						{
							histn = Form("InnerQvsrig_r%d_q%d_%sSel", span, *it, sEffCheck.c_str());
							histn += sRig; //2023.03.21
							GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(0, QVer), weight);
						}
					}
				} //iCheck
			} //igr
			
			//2020.07.31
			/*//const string sTkQEffCheck2[] = {"FSTrack", "FSTkGeom"};
			const string sTkQEffCheck2[] = {"FSL9XY", "FSL9Chis", "FSTkGeom"}; //separate it further //2020.08.03
			for (int iCheck=0; iCheck<3; iCheck++)
			{
				if (span==2) break;
				//bool bCheck=bTkQEffSam && ev.Select_TkLQStatus(0);
				//if (iCheck==0) bCheck = bCheck && ev.Select_TrTrack(2);
				//else bCheck = bCheck && ev.Select_TkGeom(2, 2);
				//2020.08.03
				if (!(bTkQEffSam && ev.Select_TkLQStatus(0))) break;
				
				bool bCheck=true;
				if (iCheck==0) bCheck = (ev.HasTkLHitXY(8)==2);
				else if (iCheck==1) bCheck = ev.GetChis(2, 1)<10;
				else if (iCheck==2) bCheck = ev.IsPassTkL(8);
				
				string sEffCheck = sEff+(bCheck?"":"not")+sTkQEffCheck2[iCheck];
				histn = Form("rig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
				histn = Form("InnerQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetSubDetQ(0, QVer), weight);
				for (int ilay=0; ilay<4; ilay++)
				{
					histn = Form("TofL%dQvsrig_r%d_q%d_%sSam", ilay+1, span, *it, sEffCheck.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTofLQ(ilay), weight);
				} //ilay
				histn = Form("UTOFQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTOFUDQ(0), weight);
				histn = Form("LTOFQvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTOFUDQ(1), weight);
				histn = Form("L1Qvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTkLQNew(0,2,QVer), weight);
				histn = Form("L9Qvsrig_r%d_q%d_%sSam", span, *it, sEffCheck.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetTkLQNew(8,2,QVer), weight);
				
				histn = Form("rig_r%d_q%d_%sSam_gen", span, *it, sEffCheck.c_str());
				if (!ev.isreal) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, weight);
				
				if (bTkQEffSel)
				{
					histn = Form("rig_r%d_q%d_%sSel", span, *it, sEffCheck.c_str());
					GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
					histn = Form("InnerQvsrig_r%d_q%d_%sSel", span, *it, sEffCheck.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), ev.GetSubDetQ(0, QVer), weight);
				
					histn = Form("rig_r%d_q%d_%sSel_gen", span, *it, sEffCheck.c_str());
					if (!ev.isreal) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, weight);
				}
			} //iCheck*/
			#endif //TKQEFFCHECK
		} //isamt
	} //it
	return bHZ;
} //SelTkQEff
