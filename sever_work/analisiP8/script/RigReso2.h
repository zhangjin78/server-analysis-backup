//-----------------
//updated from: RigReso.C
//updated date: 2019.07.19
//-----------------
//2019.07.19
//	1) add new parameters to Book & Sel function to be able to do it for different charge version
//
//2020.12.26
//	1) store the Beta resolution as well, if the ISOTOPE flag is on
//	2) store the rigiditiy and beta resolution as function of E_k/n
//-----------------

#ifndef RIGRESO_H
#define RIGRESO_H

//2023.03.01: move to SelEvent14.h
/*#include "RigToEkn.h" //2020.12.26
//#include "LiBin.h" //2021.09.03
#include "ReadBetaCor.h" //2021.09.06*/
#include "SelEvent14.h"

int nRigResoBinNoNorm=20000;
double dRigResoRangeNoNorm[2]={-0.5,1.5};

/*//2019.05.15
int nRigResoBinNoNorm=15000;
double dRigResoRangeNoNorm[2]={-0.5,1.0};*/
//------store delta 1/R
//double dRigResoRange=1.0;
//double dRigResoRange=0.3;

//------store normalized delta 1/R, 2019.04.09
int nRigResoBin=5000;
double dRigResoRange=50;
double dNp0=1./4000.; //2019.04.17: updated scaling factor, which will now not crossing the distribution of sigma hence the normalized sigma will not have peak
//double dNp0=1./2000.;
double dNp1=1./10.;

//mass template //2021.09.03
//#define MASSKEYSPDF
#ifdef MASSKEYSPDF
TTree *tMassKeysPdf=NULL;
double dMASSKEY=0.;
#endif //MASSKEYSPDF

const double dBetaRange[3][2] = {{0.3, 0.9}, {0.7, 0.99}, {0.9, 0.999}};

#ifdef AMSSOFT
bool BookRigReso(HistoMan &hman1, const bool bIsreal, TFile &outfile, int mccharge, const int QVer, const bool dir=false) //2019.07.19
//bool BookRigReso(HistoMan &hman1, const bool bIsreal, TFile &outfile, int mccharge, const bool dir=false)
#else
bool BookRigReso(TObjArray &hman1, SelEvent &ev, TFile &outfile, int mccharge, const int QVer, const bool dir=false) //2019.07.19
//bool BookRigReso(TObjArray &hman1, const bool bIsreal, TFile &outfile, int mccharge, const int QVer, const bool dir=false) //2019.07.19
//bool BookRigReso(TObjArray &hman1, const bool bIsreal, TFile &outfile, int mccharge, const bool dir=false)
#endif //AMSSOFT
{
	if (ev.isreal)
	{
		//cout << "***Only do Rigidity Resolution for MC. Stop booking histogram***" << endl;
		return false;
	}
	//const string sDir="Rigidity_Resolution";
	const string sDir=Form("Rigidity_Resolution%d", QVer); //2019.07.19
	if (dir) outfile.mkdir(sDir.c_str());
	
	iNbin = getNbin(mccharge);
	pBins = getBins(mccharge);
	int iNbin2 = getNbin2(mccharge);
	double *pBins2 = getBins2(mccharge);
	//------use even log bin for R_true
	int iNEvenLogBin=100;
	double dEvenLogBins[iNEvenLogBin+1];
	dEvenLogBins[0]=pBins[0];
	dEvenLogBins[iNEvenLogBin]=pBins[iNbin];
	
	for (int ibin=1; ibin<iNEvenLogBin; ibin++)
	{
		dEvenLogBins[ibin]=dEvenLogBins[0]*pow(dEvenLogBins[iNEvenLogBin]/dEvenLogBins[0],(double)ibin/iNEvenLogBin);
	} //ibin
	
	//------inverse bin of even log bin
	int iNEvenLogBinInv=iNEvenLogBin;
	double dEvenLogBinsInv[iNEvenLogBinInv+1];
	for (int ibin=0; ibin<iNEvenLogBinInv+1; ibin++)
	{
		dEvenLogBinsInv[ibin]=1./dEvenLogBins[iNEvenLogBinInv-ibin];
	} //ibin
	
	//------use even log bin for Ekn_true
	double dEknEvenLogBins[iNEvenLogBin+1];
	dEknEvenLogBins[0]=0.1;
	dEknEvenLogBins[iNEvenLogBin]=200;
	
	for (int ibin=1; ibin<iNEvenLogBin; ibin++)
	{
		dEknEvenLogBins[ibin]=dEknEvenLogBins[0]*pow(dEknEvenLogBins[iNEvenLogBin]/dEknEvenLogBins[0],(double)ibin/iNEvenLogBin);
	} //ibin
	
	//------
	if (dir)
	{
		outfile.mkdir(Form("%s/q%d", sDir.c_str(), mccharge));
		outfile.cd(Form("%s/q%d", sDir.c_str(), mccharge));
	}
	
	//for (int ir=0; ir<3; ir++)
	for (int ir=0; ir<4; ir++)
	{
		//for (int irig=0; irig<2; irig++)
		for (int irig=0; irig<NRig; irig++) //2023.03.27
		{
			#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
			if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
			#endif //PASS7GBL
			//string sRig="";
			//if (irig==1) sRig="_kalman";
			string sRig = sRigName[irig]; //2023.03.27
				
			string sDirStore = Form("%s/q%d/%s", sDir.c_str(), mccharge, sSpan[ir].c_str());
			sDirStore += sRig;
			if (dir)
			{
				//outfile.mkdir(Form("%s/q%d/%s", sDir.c_str(), mccharge, sSpan[ir].c_str()));
				//outfile.cd(Form("%s/q%d/%s", sDir.c_str(), mccharge, sSpan[ir].c_str()));
				outfile.mkdir(sDirStore.c_str());
				outfile.cd(sDirStore.c_str());
			}
			
			for (int ievcut=0; ievcut<2; ievcut++)
			{
				//if (dir) outfile.cd(Form("%s/q%d/%s", sDir.c_str(), mccharge, sSpan[ir].c_str()));
				if (dir) outfile.cd(sDirStore.c_str());
				
				string sEvCut="";
				if (ievcut>0) sEvCut = Form("_evcut%d", ievcut);
				
				histn = Form("rig_r%d_q%d%s%s_rec", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
				histn = Form("rig_r%d_q%d%s%s_gen", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				if (irig==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
				histn = Form("drRigvsRigGen_r%d_q%d%s%s", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBin, -dRigResoRange, dRigResoRange));
				
				histn = Form("drRigvsRigGen_r%d_q%d%s%s_noNorm", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBinNoNorm, dRigResoRangeNoNorm[0], dRigResoRangeNoNorm[1]));
				GetHist2d(hman1, histn.c_str())->Sumw2(); //2019.08.22
				
				histn = Form("drRigvsRigRec_r%d_q%d%s%s", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBin, -dRigResoRange, dRigResoRange));
				
				//resolution matrix
				histn = Form("MigMatrix_r%d_q%d%s%s", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
				histn = Form("MigMatrixInv_r%d_q%d%s%s", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBinInv, dEvenLogBinsInv, iNEvenLogBinInv, dEvenLogBinsInv));
				
				//------no reweight
				histn = Form("rig_r%d_q%d%s%s_rec_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
				histn = Form("rig_r%d_q%d%s%s_gen_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				if (irig==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
				histn = Form("drRigvsRigGen_r%d_q%d%s%s_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBin, -dRigResoRange, dRigResoRange));
				
				histn = Form("drRigvsRigGen_r%d_q%d%s%s_noreweight_noNorm", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBinNoNorm, dRigResoRangeNoNorm[0], dRigResoRangeNoNorm[1]));
				
				//2020.12.26
				histn = Form("drRigvsEknGen_r%d_q%d%s%s_noreweight_noNorm", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEknEvenLogBins, nRigResoBinNoNorm, dRigResoRangeNoNorm[0], dRigResoRangeNoNorm[1]));
				
				histn = Form("drRigvsRigRec_r%d_q%d%s%s_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBin, -dRigResoRange, dRigResoRange));
				
				//resolution matrix
				histn = Form("MigMatrix_r%d_q%d%s%s_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, iNEvenLogBin, dEvenLogBins));
				
				histn = Form("MigMatrixInv_r%d_q%d%s%s_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str());
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBinInv, dEvenLogBinsInv, iNEvenLogBinInv, dEvenLogBinsInv));
				
				//2020.12.26: beta resolution
				#ifdef ISOTOPE
				if (dir)
				{
					//outfile.mkdir(Form("%s/q%d/%s/BetaRes%s%s", sDir.c_str(), mccharge, sSpan[ir].c_str(), sRig.c_str(), sEvCut.c_str()));
					//outfile.cd(Form("%s/q%d/%s/BetaRes%s%s", sDir.c_str(), mccharge, sSpan[ir].c_str(), sRig.c_str(), sEvCut.c_str()));
					histn = sDirStore;
					histn += Form("/BetaRes%s%s", sRig.c_str(), sEvCut.c_str());
					outfile.mkdir(histn.c_str());
					outfile.cd(histn.c_str());
				}
				
				for (int ibeta=0; ibeta<3; ibeta++)
				{
					if (ibeta>0 && ievcut>0) continue;
					for (int iMassCor=0; iMassCor<2; iMassCor++)
					{
						//Beta Correction
						if (ibeta==0) //not correct for Tof Beta for now //2021.09.06
						{
							if (iMassCor==1) continue;
						}
						string sMassCor=(iMassCor==0)?"":"_MCor";
						
						double dBetaGL=0.3, dBetaGU=1.;
						if (ibeta==1) dBetaGL=0.7;
						else if (ibeta==2) dBetaGL=0.9;
						//int iNBeta=100*(dBetaGU-dBetaGL);
						int iNBeta=100;
						
						double ddBeta=1.5;
						if (ibeta==1) ddBeta=0.05;
						else if (ibeta==2) ddBeta=0.03;
						//int iNdBeta = ddBeta*2*500;
						int iNdBeta = 1000;
						
						histn = Form("beta_r%d_q%d%s%s%s_rec_beta%d_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 120, 0.3, 1.5));
						
						histn = Form("beta_r%d_q%d%s%s%s_gen_beta%d_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
						if (irig==0 && iMassCor==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 120, 0.3, 1.5));
						
						histn = Form("dBetavsBetaGen_r%d_q%d%s%s%s_beta%d_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaGL, dBetaGU, iNdBeta, -ddBeta, ddBeta));
						
						histn = Form("dBetavsEknGen_r%d_q%d%s%s%s_beta%d_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEknEvenLogBins, iNdBeta, -ddBeta, ddBeta));
						
						//histn = Form("dBetavsBetaRec_r%d_q%d_beta%d_noreweight", ir, mccharge, ibeta);
						//hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 120, 0.3, 1.5, 1500, -1.5, 1.5));
						
						histn = Form("drBetavsBetaGen_r%d_q%d%s%s%s_beta%d_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaGL, dBetaGU, iNdBeta, -ddBeta, ddBeta));
						
						histn = Form("drBetavsEknGen_r%d_q%d%s%s%s_beta%d_noreweight", ir, mccharge, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEknEvenLogBins, iNdBeta, -ddBeta, ddBeta));
						
						//histn = Form("drBetavsBetaRec_r%d_q%d_beta%d_noreweight", ir, mccharge, ibeta);
						//hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 120, 0.3, 1.5, 1500, -1.5, 1.5));
					} //iMassCor
				} //ibeta
				
				#ifdef MASSKEYSPDF
				for (it=viQSel.begin(); it!=viQSel.end(); it++)
				{
					if (ir!=0) continue;
					for (int ibeta=0; ibeta<3; ibeta++)
					{
						if (ibeta>0 && ievcut>0) continue;
						for (int iMassCor=0; iMassCor<2; iMassCor++)
						{
							if (ibeta==0) //not correct for Tof Beta for now //2021.09.06
							{
								if (iMassCor==1) continue;
							}
							
							string sMassCor=(iMassCor==0)?"":"_MCor";
							
							if (dir)
							{
								//outfile.mkdir(Form("%s/q%d/%s/BetaRes%s%s/MassKeysPdf_Beta%d%s", sDir.c_str(), mccharge, sSpan[ir].c_str(), sRig.c_str(), sEvCut.c_str(), ibeta, sMassCor.c_str()));
								//outfile.cd(Form("%s/q%d/%s/BetaRes%s%s/MassKeysPdf_Beta%d%s", sDir.c_str(), mccharge, sSpan[ir].c_str(), sRig.c_str(), sEvCut.c_str(), ibeta, sMassCor.c_str()));
								histn = sDirStore;
								histn += Form("/BetaRes%s%s/MassKeysPdf_Beta%d%s", sRig.c_str(), sEvCut.c_str(), ibeta, sMassCor.c_str());
								outfile.mkdir(histn.c_str());
								outfile.cd(histn.c_str());
							}
							
							//Mass vs EknRec
							histn = Form("Mass%dvsEkn%d_r%d_q%d_rig%d_evcut%d%s", ibeta, ibeta, ir, *it, irig, ievcut, sMassCor.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iLiNBin, pLiBins, 300, 0, 30));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Ekn [GeV/n]", sBeta[ibeta].c_str()));
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
							
							//Mass vs EknGen
							histn = Form("Mass%dvsEknGen_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, *it, irig, ievcut, sMassCor.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iLiNBin, pLiBins, 300, 0, 30));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("Ekn Gen [GeV/n]"));
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
							
							//Mass Template
							for (int iEkn=0; iEkn<iLiNBin; iEkn++) //use different binning for different beta
							{
								//if (ibeta==0 && iEkn>10) continue;
								//if (ibeta==1 && (iEkn<2 || iEkn>20)) continue;
								//if (ibeta==2 && iEkn<12) continue;
								histn = Form("Mass_beta%d_r%d_q%d_Ekn%d%s%s%s", ibeta, ir, *it, iEkn, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str());
								tMassKeysPdf = new TTree(histn.c_str(), histn.c_str());
								//set the branch as double
								tMassKeysPdf->Branch("Mass", &dMASSKEY, "Mass/D");
							} //iEkn
						} //iMassCor
					} //il
				} //*it
				#endif //MASSKEYSPDF
				#endif //ISOTOPE
			} //ievcut
		} //irig
	} //ir
	return true;
} //BookRigReso

#ifdef AMSSOFT
bool SelRigReso(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int span, const int QVer, const double weight, const int irig) //2021.09.04
//bool SelRigReso(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int span, const int QVer, const double weight) //2021.09.03
//bool SelRigReso(HistoMan &hman1, SelEvent &ev, const int span, const int QVer, const double weight) //2019.07.19
//bool SelRigReso(HistoMan &hman1, SelEvent &ev, const int span, const double weight)
#else
bool SelRigReso(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int span, const int irig, const int QVer, const double weight) //2021.09.04
//bool SelRigReso(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int span, const int QVer, const double weight, const int irig) //2021.09.04
//bool SelRigReso(TObjArray &hman1, SelEvent &ev, const int span, const int QVer, const double weight) //2019.07.19
//bool SelRigReso(TObjArray &hman1, SelEvent &ev, const int span, const double weight)
#endif //AMSSOFT
{
	if (weight<=0) return true; //2023.04.01
	
	if (ev.isreal)
	{
		//cout << "***Only do Rigidity Resolution for MC. Stop booking histogram***" << endl;
		return false;
	}
	
	const string sDir=Form("Rigidity_Resolution%d", QVer); //2019.07.19
	
	//if (ev.mch!=ev.GetTkInZ()) return false; //2019.07.19: remove this requirement since and do the charge indentify purely in charge selection
	
	//if (ev.Select_Charge(ev.mch, span, 0))
	//if (ev.Select_Charge(ev.mch, span, QVer)) //2019.07.19
	//2021.03.13
	bool bUnbiasedL1XY=true;
	bool bUnbiasedL1Q=true;
	#ifdef INNERUNBIAEDL1Q
	if (span==0)
	{
		bUnbiasedL1XY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
		bUnbiasedL1Q=(ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, ev.mch, 4, QVer)) && (ev.GetUBExtQTrkCor(0, 2, QVer)<ev.GetQSelRange(1, ev.mch, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
	}
	#endif //INNERUNBIAEDL1Q
	if (ev.Select_Charge(ev.mch, span, QVer) && bUnbiasedL1XY && bUnbiasedL1Q)
	{
		//double dRigRec = ev.GetRigidity(span), dRigGen = ev.mmom/ev.mch;
		//2021.09.04
		//int iRigAlgo=2*irig;
		int iRigAlgo=irig; //2023.03.27
		double dRigRec = ev.GetRigidity(span,iRigAlgo), dRigGen = ev.mmom/ev.mch;
		//string sRig="";
		//if (irig==1) sRig="_kalman";
		string sRig = sRigName[irig]; //2023.03.27
		
		double dEknGen = RigToEkn(dRigGen, (int)ev.mch, MCMass); //2020.12.26
		double dDeltaRRig = 1/dRigRec - 1/dRigGen;
		double dDeltaRRigNoNorm=dDeltaRRig; //resolution without normalization
		
		histn = Form("rig_r%d_q%d_gen", span, (int)ev.mch);
		TH1 *h1bin = GetHist(hman1, histn.c_str());
		
		double dRigNorm=h1bin->GetBinCenter(h1bin->FindBin(dRigGen));
		dDeltaRRig=1/sqrt(pow(dNp0,2)+pow(dNp1/dRigNorm,2))*dDeltaRRig;
		
		//2021.09.04
		for (int ievcut=0; ievcut<2; ievcut++)
		{
			bool bEvCut=true;
			if (ievcut==1) bEvCut = ev.ntrack==1 || ev.Select_Tk2nd();
			if (!bEvCut) continue;
			
			string sEvCut="";
			if (ievcut>0) sEvCut = Form("_evcut%d", ievcut);
			
			histn = Form("rig_r%d_q%d%s%s_rec", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist(hman1, histn.c_str())->Fill(dRigRec, weight);
			
			histn = Form("rig_r%d_q%d%s%s_gen", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			if (irig==0) GetHist(hman1, histn.c_str())->Fill(dRigGen, weight);
			
			histn = Form("drRigvsRigGen_r%d_q%d%s%s", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigGen, dDeltaRRig, weight);
			
			histn = Form("drRigvsRigGen_r%d_q%d%s%s_noNorm", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigGen, dDeltaRRigNoNorm, weight); 
			
			histn = Form("drRigvsRigRec_r%d_q%d%s%s", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigRec, dDeltaRRig, weight);
			
			histn = Form("MigMatrix_r%d_q%d%s%s", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigGen, dRigRec, weight);
			
			histn = Form("MigMatrixInv_r%d_q%d%s%s", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(1./dRigGen, 1./dRigRec, weight);
			
			//--no reweight
			histn = Form("rig_r%d_q%d%s%s_rec_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist(hman1, histn.c_str())->Fill(dRigRec);
			
			histn = Form("rig_r%d_q%d%s%s_gen_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			if (irig==0) GetHist(hman1, histn.c_str())->Fill(dRigGen);
			
			histn = Form("drRigvsRigGen_r%d_q%d%s%s_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigGen, dDeltaRRig);
			
			histn = Form("drRigvsRigGen_r%d_q%d%s%s_noreweight_noNorm", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigGen, dDeltaRRigNoNorm); 
			
			//2020.12.26
			histn = Form("drRigvsEknGen_r%d_q%d%s%s_noreweight_noNorm", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dEknGen, dDeltaRRigNoNorm); 
			
			histn = Form("drRigvsRigRec_r%d_q%d%s%s_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigRec, dDeltaRRig);
			
			histn = Form("MigMatrix_r%d_q%d%s%s_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(dRigGen, dRigRec);
			
			histn = Form("MigMatrixInv_r%d_q%d%s%s_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
			GetHist2d(hman1, histn.c_str())->Fill(1./dRigGen, 1./dRigRec);
			
			//2020.12.26
			#ifdef ISOTOPE
			for (int ibeta=0; ibeta<3; ibeta++)
			{
				if (ibeta>0 && ievcut>0) continue;
				if (ibeta>0)
				{
					if (!ev.SelectRich()) continue;
					if ((ibeta==1 && !ev.rich_NaF) || (ibeta==2 && ev.rich_NaF)) continue;
				}
				
				double dBetaGen = RigToBeta((int)ev.mch, MCMass, dRigGen);
				for (int iMassCor=0; iMassCor<2; iMassCor++)
				{
					
					double dBeta;
					if (ibeta==0) dBeta = ev.tof_betah;
					else dBeta = ev.rich_beta[0];
					//Beta Correction
					if (ibeta==0) //not correct for Tof Beta for now //2021.09.06
					{
						if (iMassCor==1) continue;
					}
					else
					{
						//if (iMassCor==1) dBeta = GetCorBeta(dBetaGen, dBeta, (int)ev.mch, ievcut, ibeta);
						//2021.10.26
						if (iMassCor==1)
						{
							dBeta = GetCorBeta(dBetaGen, dBeta, (int)ev.mch, ievcut, ibeta);
							if (dBeta<0) continue;
						}
					}
					
					string sMassCor=(iMassCor==0)?"":"_MCor";
					
					histn = Form("beta_r%d_q%d%s%s%s_rec_beta%d_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
					GetHist(hman1, histn.c_str())->Fill(dBeta);
					
					histn = Form("beta_r%d_q%d%s%s%s_gen_beta%d_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
					if (irig==0 && iMassCor==0) GetHist(hman1, histn.c_str())->Fill(dBetaGen);
					
					histn = Form("dBetavsBetaGen_r%d_q%d%s%s%s_beta%d_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
					GetHist(hman1, histn.c_str())->Fill(dBetaGen, dBeta-dBetaGen);
					
					histn = Form("dBetavsEknGen_r%d_q%d%s%s%s_beta%d_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
					GetHist(hman1, histn.c_str())->Fill(dEknGen, dBeta-dBetaGen);
					
					//histn = Form("dBetavsBetaRec_r%d_q%d_beta%d_noreweight", span, (int)ev.mch, ibeta);
					//GetHist(hman1, histn.c_str())->Fill(dBeta, dBeta-dBetaGen);
					
					histn = Form("drBetavsBetaGen_r%d_q%d%s%s%s_beta%d_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
					GetHist(hman1, histn.c_str())->Fill(dBetaGen, 1./dBeta-1./dBetaGen);
					
					histn = Form("drBetavsEknGen_r%d_q%d%s%s%s_beta%d_noreweight", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str(), ibeta);
					GetHist(hman1, histn.c_str())->Fill(dEknGen, 1./dBeta-1./dBetaGen);
					
					//histn = Form("drBetavsBetaRec_r%d_q%d_beta%d_noreweight", span, (int)ev.mch, ibeta);
					//GetHist(hman1, histn.c_str())->Fill(dBeta, 1./dBeta-1./dBetaGen);
				} //iMassCor
			} //ibeta
			
			//Mass KeysPdf template
			#ifdef MASSKEYSPDF
			TH1F hLiBin("hLiBin", "hLiBin", iLiNBin, pLiBins);
			
			for (int ibeta=0; ibeta<3; ibeta++)
			{
				if (span!=0) continue;
				if (ibeta>0 && ievcut>0) continue;
				
				if (ibeta>0)
				{
					if (!ev.SelectRich()) continue;
					if ((ibeta==1 && !ev.rich_NaF) || (ibeta==2 && ev.rich_NaF)) continue;
				}
				
				double dBetaGen = RigToBeta((int)ev.mch, MCMass, dRigGen);
				
				for (int iMassCor=0; iMassCor<2; iMassCor++)
				{
					double dBeta;
					if (ibeta==0) dBeta = ev.tof_betah;
					else dBeta = ev.rich_beta[0];
					//Beta Correction
					if (ibeta==0) //not correct for Tof Beta for now //2021.09.06
					{
						if (iMassCor==1) continue;
					}
					else
					{
						//if (iMassCor==1) dBeta = GetCorBeta(dBetaGen, dBeta, (int)ev.mch, ievcut, ibeta);
						double dBetaRich = RigToBeta((int)ev.mch, MCMass, ev.mevmom1[17]/ev.mch);
						//if (iMassCor==1) dBeta = GetCorBeta(dBetaRich, dBeta, (int)ev.mch, ievcut, ibeta); //correct with respect to the beta in RICH
						//2021.10.26
						if (iMassCor==1)
						{
							dBeta = GetCorBeta(dBetaRich, dBeta, (int)ev.mch, ievcut, ibeta); //correct with respect to the beta in RICH
							if (dBeta<0) continue;
						}
					}
					if (dBeta>=1.) continue;
					
					string sMassCor=(iMassCor==0)?"":"_MCor";
					
					double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
					dMASSKEY = CalMass((int)ev.mch, dBeta, dRigRec);
					
					int iEknBin = hLiBin.FindBin(dEknRec)-1;
					//cout << ibeta << " " << dBeta << " " << dEknGen << " " << (1./sqrt(1-pow(dBetaGen,2))-1)*MPROTON << " " << dEknRec << " " << iEknBin << endl;
					if (iEknBin<0 || iEknBin>iLiNBin-1) continue;
					//if (ibeta==0 && iEknBin>10) continue;
					//if (ibeif (ibeta==1 && (iEknBin<2 || iEknBin>20)) continue;
					//if (ibeif (ibeta==2 && iEknBin<12) continue;
					
					//Mass vs EknRec
					histn = Form("Mass%dvsEkn%d_r%d_q%d_rig%d_evcut%d%s", ibeta, ibeta, span, (int)ev.mch, irig, ievcut, sMassCor.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEknRec, dMASSKEY);
					
					//Mass vs EknGen
					histn = Form("Mass%dvsEknGen_r%d_q%d_rig%d_evcut%d%s", ibeta, span, (int)ev.mch, irig, ievcut, sMassCor.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEknGen, dMASSKEY);
					
					//Mass Template
					histn = Form("%s/q%d/%s/BetaRes%s%s/MassKeysPdf_Beta%d%s/Mass_beta%d_r%d_q%d_Ekn%d%s%s%s", sDir.c_str(), (int)ev.mch, sSpan[span].c_str(), sRig.c_str(), sEvCut.c_str(), ibeta, sMassCor.c_str(), ibeta, span, (int)ev.mch, iEknBin, sRig.c_str(), sEvCut.c_str(), sMassCor.c_str());
					tMassKeysPdf = static_cast<TTree*>(outfile.Get(histn.c_str()));
					tMassKeysPdf->Fill(); 
				} //iMassCor
			} //ibeta
			
			#endif //MASSKEYSPDF
			#endif //ISOTOPE
		} //ievcut
	} //charge selection
	return true;
} //SelRigReso

#endif //RIGRESO_H
