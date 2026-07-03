//-----------------
//updated from: RigEstCal2.C
//updated date: 2019.05.02
//Calibrate rigidity estimator, Cutoff and Ecal deposite energy, for Tracking Efficiency
//-----------------
//2019.05.03
//	1) update BookRigEst && StoreRigEst to book & store distribution selected by different charge calibration
//-----------------
#include "EstiBins.h"

#define REBIN2D

TH1F *h1RigEst=NULL;
TH2F *h2RigEst=NULL;
//bool BookRigEst(TFile &outfile, const int QVer, const bool dir=false)
bool BookRigEst(SelEvent &ev, TFile &outfile, const int QVer, const bool dir=false) //2023.03.10
{
	if (!ev.isreal)
	{
		cout << " !!! Only store Rigidity estimation for Data." << endl;
		return false;
	}
	//bool bSumw2=TH1::fgDefaultSumw2;
	bool bSumw2=TH1::GetDefaultSumw2(); //2020.01.21
	TH1::SetDefaultSumw2(kFALSE);
	
	string sDir = Form("Rigidity_Estimator_Calibration%d", QVer);
	
	int nbinx=0, nbiny1=0, nbiny2=0;
	double binxl=0., binxu=0., binyl1=0., binyu1=0., binyl2=0., binyu2=0.;
	if (dir)
	{
		outfile.mkdir(Form("%s", sDir.c_str()));
		outfile.cd(Form("%s", sDir.c_str()));
	}
	
	/*//use finner binning even-logly for rigidity as well //2022.06.10
	vector<double> vdELB;
	const int nEknRange=1;
	double dEknRange[nEknRange+1]={0.8, 100.};
	int nBinELBEkn[nEknRange]={100};
	
	vdELB.push_back(dEknRange[0]);
	iNbin=0;
	for (int iRange=0; iRange<nEknRange; iRange++)
	{
		iNbin+=nBinELBEkn[iRange];
		for (int ibin=1; ibin<nBinELBEkn[iRange]+1; ibin++)
		{
			double dBin=dEknRange[iRange]*pow(dEknRange[iRange+1]/dEknRange[iRange], (double)ibin/nBinELBEkn[iRange]);
			vdELB.push_back(dBin);
		} //ibin
	} //iRange
	pBins=&vdELB[0];
	cout << "Even log bin for rigidity estimator: " << iNbin<< " " << pBins<< " " << vdELB.size() << endl;*/
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		if (dir)
		{
			outfile.mkdir(Form("%s/q%d", sDir.c_str(), *it));
			outfile.cd(Form("%s/q%d", sDir.c_str(), *it));
		}
		
		//iNbin = getNbin(*it);
		//pBins = getBins(*it);
		
		for (int ir=1; ir<3; ir++)
		{
			for (int irig=0; irig<NRig; irig++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				string sRig=sRigName[irig];
				
				if (dir)
				{
					outfile.mkdir(Form("%s/q%d/%s%s", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str()));
					outfile.cd(Form("%s/q%d/%s%s", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str()));
				}
				
				for (int imr=0; imr<3; imr++)
				{
					if (imr==0)
					{
						nbinx=105, binxl=0., binxu=1.05;
						nbiny1=200, binyl1=0., binyu1=0.8;
						nbiny2=200, binyl2=0., binyu2=10.;
					}
					else if (imr==1)
					{
						nbinx=320, binxl=0., binxu=32.;
						nbiny1=150, binyl1=0., binyu1=1.5;
						nbiny2=200, binyl2=0., binyu2=6.;
					}
					else if (imr==2)
					{
						//nbinx=450, binxl=-50., binxu=850;
						nbinx=850, binxl=0., binxu=850;
						//nbiny1=240, binyl1=-2., binyu1=10.;
						nbiny1=200, binyl1=0., binyu1=10.;
						nbiny2=200, binyl2=0., binyu2=10.;
					}
					histn = Form("%s_r%d_q%d", sRigMethod[imr].c_str(), ir, *it); //estimation/rig vs estimation
					histn += sRig; //2023.03.10
					h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, nbiny1, binyl1, binyu1);
					
					histn = Form("%s_r%d_q%d_1", sRigMethod[imr].c_str(), ir, *it); //rig vs estimation
					histn += sRig; //2023.03.10
					//h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, iNbin, pBins);
					h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, 200, 0, 100.);
					
					histn = Form("%s_r%d_q%d_2", sRigMethod[imr].c_str(), ir, *it); //rig/estimation vs estimation
					histn += sRig; //2023.03.10
					h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, nbiny2, binyl2, binyu2);
					
					histn = Form("%s1d_r%d_q%d", sRigMethod[imr].c_str(), ir, *it);
					histn += sRig; //2023.03.10
					h1RigEst = new TH1F(histn.c_str(), histn.c_str(), nbinx*10, binxl, binxu);
					
					if (imr==2)
					{
						histn = Form("%s_r%d_q%d_nocut", sRigMethod[imr].c_str(), ir, *it); //estimation/rig vs estimation
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, nbiny1, binyl1, binyu1);
					
						histn = Form("%s_r%d_q%d_1_nocut", sRigMethod[imr].c_str(), ir, *it); //rig vs estimation
						histn += sRig; //2023.03.10
						//h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, iNbin, pBins);
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, 200, 0, 100);
					
						histn = Form("%s_r%d_q%d_2_nocut", sRigMethod[imr].c_str(), ir, *it); //rig/estimation vs estimation
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, nbiny2, binyl2, binyu2);
						
						histn = Form("%s1d_r%d_q%d_nocut", sRigMethod[imr].c_str(), ir, *it);
						histn += sRig; //2023.03.10
						h1RigEst = new TH1F(histn.c_str(), histn.c_str(), nbinx*10, binxl, binxu);
						
						histn = Form("BetavsEdep_r%d_q%d", ir, *it); //Beta vs Ecal_en/1000
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, 105, 0., 1.05);
				
						histn = Form("RigbetavsEdep_r%d_q%d", ir, *it); //rigidity-estimated_Beta vs Ecal_en/1000
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, 105, 0., 1.05);
						
						histn = Form("BetavsEdep_r%d_q%d_nocut", ir, *it); //Beta vs Ecal_en/1000
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, 105, 0., 1.05);
				
						histn = Form("RigbetavsEdep_r%d_q%d_nocut", ir, *it); //rigidity-estimated_Beta vs Ecal_en/1000
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), nbinx, binxl, binxu, 105, 0., 1.05);
					}
					
					#ifdef REBIN2D
					if (imr>0)
					{
						int iNbinEst=0;
						double *pBinsEst=NULL;
						if (imr==1) iNbinEst=NCutoffRebin, pBinsEst=CutoffRebins;
						else if (imr==2) iNbinEst=NEdepRebin, pBinsEst=EdepRebins;
						histn = Form("%s_r%d_q%d_rebin", sRigMethod[imr].c_str(), ir, *it); //estimation/rig vs estimation
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, nbiny1, binyl1, binyu1);
						
						histn = Form("%s_r%d_q%d_1_rebin", sRigMethod[imr].c_str(), ir, *it); //rig vs estimation
						histn += sRig; //2023.03.10
						//h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, iNbin, pBins);
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, 200, 0, 100);
					
						histn = Form("%s_r%d_q%d_2_rebin", sRigMethod[imr].c_str(), ir, *it); //rig/estimation vs estimation
						histn += sRig; //2023.03.10
						h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, nbiny2, binyl2, binyu2);
						
						if (imr==2)
						{
							histn = Form("%s_r%d_q%d_nocut_rebin", sRigMethod[imr].c_str(), ir, *it); //estimation/rig vs estimation
							histn += sRig; //2023.03.10
							h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, nbiny1, binyl1, binyu1);
					
							histn = Form("%s_r%d_q%d_1_nocut_rebin", sRigMethod[imr].c_str(), ir, *it); //rig vs estimation
							histn += sRig; //2023.03.10
							//h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, iNbin, pBins);
							h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, 200, 0, 100);
					
							histn = Form("%s_r%d_q%d_2_nocut_rebin", sRigMethod[imr].c_str(), ir, *it); //rig/estimation vs estimation
							histn += sRig; //2023.03.10
							h2RigEst = new TH2F(histn.c_str(), histn.c_str(), iNbinEst, pBinsEst, nbiny2, binyl2, binyu2);
						}
					}
					#endif //REBIN2D
				} //imr
			} //irig
		} //ir
	} //it
	
	TH1::SetDefaultSumw2(bSumw2);
	return true;
} //BookRigEst

//void StoreRigEst(TFile &outfile, SelEvent &ev, const int QVer, const int ich, const int ir, const double weight)
void StoreRigEst(TFile &outfile, SelEvent &ev, const int QVer, const int ich, const int ir, const double weight, const int irig) //2023.03.10
{
	//--
	if (!ev.isreal) return;
	
	//--
	string sRig=sRigName[irig]; //2023.03.10
	
	string sDir = Form("Rigidity_Estimator_Calibration%d", QVer);
	outfile.cd(Form("%s/q%d/%s%s", sDir.c_str(), *it, sSpan[ir].c_str(), sRig.c_str()));
	
	static float fBeta=0, fCutoff=0, fEcalen=0;
	static float fBetaR=0, fCutoffR=0, fEcalenR=0;
	
	//--
	double dRig = ev.GetRigidity(ir, irig);
	
	//Beta
	fBeta = ev.tof_betah;
	fBetaR = fBeta/dRig;
	histn = Form("%s_r%d_q%d", sRigMethod[0].c_str(), ir, ich); //beta/rig vs beta
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fBeta, fBetaR);
	
	histn = Form("%s_r%d_q%d_1", sRigMethod[0].c_str(), ir, ich); //rig vs beta
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fBeta, dRig);
	
	histn = Form("%s_r%d_q%d_2", sRigMethod[0].c_str(), ir, ich); //rig/beta vs beta
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fBeta, 1./fBetaR);
	
	histn = Form("%s1d_r%d_q%d", sRigMethod[0].c_str(), ir, ich);
	histn += sRig; //2023.03.10
	h1RigEst = static_cast<TH1F*>(gDirectory->Get(histn.c_str()));
	h1RigEst->Fill(fBeta);
	
	//Cutoff
	#ifdef AMSSOFT
	AMSSetupR::RTI a;
	if(AMSEventR::GetRTI(a,ev.time[0])==0) fCutoff = a.cfi[icffv][1];
	#else
	fCutoff = ev.mcutoffi[icffv][1];
	#endif //AMSSOFT
	fCutoffR = fCutoff/dRig;
	
	histn = Form("%s_r%d_q%d", sRigMethod[1].c_str(), ir, ich); //cutoff/rig vs cutoff
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fCutoff, fCutoffR);
	
	histn = Form("%s_r%d_q%d_1", sRigMethod[1].c_str(), ir, ich); //rig vs cutoff
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fCutoff, dRig);
	
	histn = Form("%s_r%d_q%d_2", sRigMethod[1].c_str(), ir, ich); //rig/cutoff vs cutoff
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fCutoff, 1./fCutoffR);
	
	histn = Form("%s1d_r%d_q%d", sRigMethod[1].c_str(), ir, ich);
	histn += sRig; //2023.03.10
	h1RigEst = static_cast<TH1F*>(gDirectory->Get(histn.c_str()));
	h1RigEst->Fill(fCutoff);
	
	#ifdef REBIN2D
	histn = Form("%s_r%d_q%d_rebin", sRigMethod[1].c_str(), ir, ich); //cutoff/rig vs cutoff
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fCutoff, fCutoffR);
	
	histn = Form("%s_r%d_q%d_1_rebin", sRigMethod[1].c_str(), ir, ich); //rig vs cutoff
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fCutoff, dRig);
	
	histn = Form("%s_r%d_q%d_2_rebin", sRigMethod[1].c_str(), ir, ich); //rig/cutoff vs cutoff
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fCutoff, 1./fCutoffR);
	#endif //REBIN2D
	
	//Ecal
	fEcalen = ev.ecal_en[2]/1000.;
	fEcalenR = fEcalen/dRig;
	
	histn = Form("BetavsEdep_r%d_q%d_nocut", ir, ich); //Beta vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, fBeta);
	
	fBeta = MPROTON*dMass[ich]/ich/dRig;
	fBeta = 1./sqrt(1 + pow(fBeta, 2));
	histn = Form("RigbetavsEdep_r%d_q%d_nocut", ir, ich); //rigidity-estimated_Beta vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, fBeta);
	if (ev.IsPassEcal())
	{
		histn = Form("%s_r%d_q%d", sRigMethod[2].c_str(), ir, ich); //(Ecal_en/1000)/rig vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, fEcalenR);
		
		histn = Form("%s_r%d_q%d_1", sRigMethod[2].c_str(), ir, ich); //rig vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, dRig);
		
		histn = Form("%s_r%d_q%d_2", sRigMethod[2].c_str(), ir, ich); //rig/(Ecal_en/1000) vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, 1./fEcalenR);
		
		#ifdef REBIN2D
		histn = Form("%s_r%d_q%d_rebin", sRigMethod[2].c_str(), ir, ich); //(Ecal_en/1000)/rig vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, fEcalenR);
		
		histn = Form("%s_r%d_q%d_1_rebin", sRigMethod[2].c_str(), ir, ich); //rig vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, dRig);
		
		histn = Form("%s_r%d_q%d_2_rebin", sRigMethod[2].c_str(), ir, ich); //rig/(Ecal_en/1000) vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, 1./fEcalenR);
		#endif //REBIN2D
		
		fBeta = ev.tof_betah;
		histn = Form("BetavsEdep_r%d_q%d", ir, ich); //Beta vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, fBeta);
		
		fBeta = MPROTON*dMass[ich]/ich/dRig;
		fBeta = 1./sqrt(1 + pow(fBeta, 2));
		histn = Form("RigbetavsEdep_r%d_q%d", ir, ich); //rigidity-estimated_Beta vs Ecal_en/1000
		histn += sRig; //2023.03.10
		h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
		h2RigEst->Fill(fEcalen, fBeta);
		
		histn = Form("%s1d_r%d_q%d", sRigMethod[2].c_str(), ir, ich);
		histn += sRig; //2023.03.10
		h1RigEst = static_cast<TH1F*>(gDirectory->Get(histn.c_str()));
		h1RigEst->Fill(fEcalen);
	}
	
	histn = Form("%s_r%d_q%d_nocut", sRigMethod[2].c_str(), ir, ich); //(Ecal_en/1000)/rig vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, fEcalenR);
	
	histn = Form("%s_r%d_q%d_1_nocut", sRigMethod[2].c_str(), ir, ich); //rig vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, dRig);
	
	histn = Form("%s_r%d_q%d_2_nocut", sRigMethod[2].c_str(), ir, ich); //rig/(Ecal_en/1000) vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, 1./fEcalenR);
	
	#ifdef REBIN2D
	histn = Form("%s_r%d_q%d_nocut_rebin", sRigMethod[2].c_str(), ir, ich); //(Ecal_en/1000)/rig vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, fEcalenR);
	
	histn = Form("%s_r%d_q%d_1_nocut_rebin", sRigMethod[2].c_str(), ir, ich); //rig vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, dRig);
	
	histn = Form("%s_r%d_q%d_2_nocut_rebin", sRigMethod[2].c_str(), ir, ich); //rig/(Ecal_en/1000) vs Ecal_en/1000
	histn += sRig; //2023.03.10
	h2RigEst = static_cast<TH2F*>(gDirectory->Get(histn.c_str()));
	h2RigEst->Fill(fEcalen, 1./fEcalenR);
	#endif //REBIN2D
	
	histn = Form("%s1d_r%d_q%d_nocut", sRigMethod[2].c_str(), ir, ich);
	histn += sRig; //2023.03.10
	h1RigEst = static_cast<TH1F*>(gDirectory->Get(histn.c_str()));
	h1RigEst->Fill(fEcalen);
	
} //StoreRigEst
