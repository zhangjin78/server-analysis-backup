#ifndef ISOTOPEEV_H
#define ISOTOPEEV_H
//-----------------
//create date: 2023.02.28
//-----------------
//booking histogram, doing selection and filling the histogram for All isotope Event count related
//-----------------
//-----------------

#include "SelEvent14.h"
#include "bins.h"
#include "ReadBetaCor.h"

//--
#ifdef AMSSOFT
bool BookHistoIsotopeEv(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#else
bool BookHistoIsotopeEv(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#endif //AMSSOFT
{
	//--
	string sDir=Form("Event_count%d", iQVer);
	
	//--
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		//only store for Li, Be, B and N
		if (*it>8 || *it<3 || *it==6) continue;
					
		for (int ir=iSpanL; ir<iSpanU; ir++)
		{
			//only store for Inner+Max L1Q to reduce storage and speed up
			if (ir!=0) break;
			
			//loop for choutko and kalman rigidity
			for (int irig=0; irig<2; irig++) //irig=0: choutco, irig=1: kalman
			{
				string sRig="";
				if (irig==1) sRig="_kalman";
				for (int ibeta=0; ibeta<3; ibeta++)
				{
					if (irig==0) outfile.mkdir(Form("Event_count%d/q%d/%s/Beta%d", iQVer, *it, sSpan[ir].c_str(), ibeta));
					outfile.cd(Form("Event_count%d/q%d/%s/Beta%d", iQVer, *it, sSpan[ir].c_str(), ibeta));
					
					//--distribution of rich q
					if (ibeta>0)
					{
						histn = Form("richq0vsMass_beta%d_r%d_q%d_rig%d", ibeta, ir, *it, irig);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 300, 0, 30, 3500, 0 ,35));
						GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
						GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle("rich_q[0]");
					}
					//apply additional cut
					for (int ievcut=0; ievcut<4; ievcut++) //0: extra cut, 1: ntrack=1, 2: normalized tof chis t&s<5, 3: tof chis t&c<5
					{
						if (ibeta!=0 && ievcut>1) break; //only apply tof chis cut (ievcut=2 or =3) when using tof beta
						
						//--rigidity event count
						histn = Form("rig_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
						
						//--beta event count
						histn = Form("Beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 750, 0, 1.5));
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
						
						//--event count in Generated Ekn, for isotope accpetance
						if (!ev.isreal && ir==0)
						{
							histn = Form("Ekn_beta%d_r%d_q%d_rig%d_evcut%d_GR%d_rew%d", ibeta, ir, *it, irig, ievcut, 0, 0);
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenEkn, pBinsGenEkn)); //Even log bin
						
							GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("Generated Ekn [GeV/c]"));
							GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
						
						}
						
						//--beta vs Rig
						//--with Beta Correction
						for (int iMassCor=0; iMassCor<2; iMassCor++)
						{
							#ifndef MASSKEYSPDF
							if (iMassCor>0) continue;
							#endif //MASSKEYSPDF
							if (ev.isreal && iMassCor>0) continue;
							string sMassCor=(iMassCor==0)?"":"_MassCor";
							if (iMassCor==1)
							{
								if (ibeta==0) continue; //not correct for Tof Beta for now
								else
								{
									if (ievcut!=0) continue;
								}
							}
							
							//--beta vs Rig
							histn = Form("Beta%dvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, *it, irig, ievcut, sMassCor.c_str());
							double dBetaLow=0.35, dBetaUp=1.15;
							int iNBeta=800;
							if (ibeta==1) dBetaLow=0.77, dBetaUp=1.02, iNBeta=500;
							else if (ibeta==2) dBetaLow=0.955, dBetaUp=1.01, iNBeta=1100;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNBeta, dBetaLow, dBetaUp));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							
							//--1/beta vs rig
							histn = Form("rBeta%dvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, *it, irig, ievcut, sMassCor.c_str());
							double dRBetaLow=0.9, dRBetaUp=1.5;
							int iNRBeta=600;
							if (ibeta>=1) dRBetaLow=0.99, dRBetaUp=1.05, iNRBeta=1200;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNRBeta, dRBetaLow, dRBetaUp));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							
							//--store the TOF beta using tof_betah, that is not well tuned for MC. only for MC and TOF
							if (!ev.isreal && ibeta==0)
							{
								//--beta vs Rig
								histn = Form("Beta%d_NotTunedvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, *it, irig, ievcut, sMassCor.c_str());
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNBeta, dBetaLow, dBetaUp));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
								
								//--1/beta vs rig
								histn = Form("rBeta%d_NotTunedvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, *it, irig, ievcut, sMassCor.c_str());
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNRBeta, dRBetaLow, dRBetaUp));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							}
						} //iMassCor
						
						//--beta diff between tof and rich
						if (ibeta>0)
						{
							for (int ievcut0=0; ievcut0<4; ievcut0++) //0: extra cut, 1: ntrack=1, 2: normalized tof chis t&s<5, 3: tof chis t&c<5
							{
								if (ievcut!=0) break;
								//beta diff
								histn = Form("BetaTof-BetaRich%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, *it, irig, ievcut0);
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 550, 0, 1.1, 500, -0.25, 0.25));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("#beta_{%s}", sBeta[ibeta].c_str()));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("#beta_{TOF} - #beta_{%s}", sBeta[ibeta].c_str()));
								
								//1/beta diff
								histn = Form("rBetaTof-rBetaRich%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, *it, irig, ievcut0);
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 550, 0, 1.1, 500, -0.25, 0.25));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("#beta_{%s}", sBeta[ibeta].c_str()));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/#beta_{TOF} - 1/#beta_{%s}", sBeta[ibeta].c_str()));
							} //ievcut0
						}
						
						//--mass distribution
						histn = Form("Mass%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 300, 0, 30));
						
						//--mass vs beta
						histn = Form("Mass%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 750, 0, 1.5, 300, 0, 30));
						GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
						GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
						
						//--1/mass distribution
						histn = Form("rMass%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 200, 0, 1));
						
						//--1/mass vs beta
						histn = Form("rMass%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 750, 0, 1.5, 200, 0, 1));
						GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
						GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/(%s Mass)", sBeta[ibeta].c_str()));
						
						//--mass vs rigidity
						histn = Form("Mass%dvsRig_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 300, 0, 30));
						GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
						GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
						
						//--1/mass vs rigidity
						histn = Form("rMass%dvsRig_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 200, 0, 1));
						GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
						GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/(%s Mass)", sBeta[ibeta].c_str()));
						
						//distribution of tof chis, only fortof
						for (int itofcstc=0; itofcstc<2; itofcstc++) //0: tof_chist, 1: tof_chisc
						{
							if (ibeta!=0) break;
							string stofcs=Form("tof_chis%s", ((itofcstc==0)?"t":"c"));
							
							for (int itofnorm=0; itofnorm<2; itofnorm++) //0: non-normlized, 1: normalized
							{
								string stofcsn=stofcs;
								if (itofnorm==1) stofcsn=stofcsn+"_n";
								
								//--tof chis distribution
								histn = Form("%s%d_r%d_q%d_rig%d_evcut%d", stofcsn.c_str(), ibeta, ir, *it, irig, ievcut);
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 200, 0, 200));
								
								//--tof chis vs beta
								histn = Form("%s%dvsBeta_r%d_q%d_rig%d_evcut%d", stofcsn.c_str(), ibeta, ir, *it, irig, ievcut);
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 750, 0, 1.5, 200, 0, 200));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(stofcsn.c_str());
								
								//--tof chis vs rig
								histn = Form("%s%dvsRig_r%d_q%d_rig%d_evcut%d", stofcsn.c_str(), ibeta, ir, *it, irig, ievcut);
								hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 200, 0, 200));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(stofcsn.c_str());
							} //itofnorm
						} //itofcstc
						
						if (ibeta==0) continue;
						if (ievcut>1) continue;
						
						//store the impact point Z coordinate, to figure out the center plane of rich //2022.05.30
						histn = Form("ImpactZ_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1000, -80, -70));
						
						//--rich efficiency //2022.05.28
						for (int igr=1; igr>=0; igr--)
						{
							if (ev.isreal && igr==0) continue;
							//--eff vs rigidity
							histn = Form("RichEff_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
							if (igr==0) histn += "_gen";
							hman1.Add(new TEfficiency(histn.c_str(), histn.c_str(), iNbin, pBins));
							//GetTEff(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
							//GetTEff(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("Rich Efficiency"));
							
							//store the rich efficiency selected using MC truth that particle suvival up to rich, for rich and LTof, iskip=17
							if (ev.isreal) continue;
							histn = Form("RichEff_beta%d_r%d_q%d_rig%d_evcut%d_MCTruth", ibeta, ir, *it, irig, ievcut);
							if (igr==0) histn += "_gen";
							hman1.Add(new TEfficiency(histn.c_str(), histn.c_str(), iNbin, pBins));
							//GetTEff(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
							//GetTEff(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("Rich Efficiency"));
						}
						
						//can't store as function of beta or Ekn, because we don't have measurement of beta before having rich, same as for TkEff
						//histn = Form("RichEff_Ekn_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
						//hman1.Add(new TEfficiency(histn.c_str(), histn.c_str(), iLiNBin, pLiBins));
						//GetTEff(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Ekn [GeV/n]", sBeta[ibeta].c_str()));
						//GetTEff(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("Rich Efficiency"));
						
						//store as function of generated rigidity
						if (!ev.isreal)
						{
							histn = Form("RichEff_GenRig_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
							hman1.Add(new TEfficiency(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//store the rich efficiency selected using MC truth that particle suvival up to rich, for rich and LTof, iskip=17
							histn = Form("RichEff_GenRig_beta%d_r%d_q%d_rig%d_evcut%d_MCTruth", ibeta, ir, *it, irig, ievcut);
							hman1.Add(new TEfficiency(histn.c_str(), histn.c_str(), iNbin, pBins));
						}
					} //ievcut
				} //ibeta
			} //irig
		} //ir
	} //*it
	
	return true;
} //BookHistoIsotopeEv

#ifdef AMSSOFT
bool BookHistoMassDis(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#else
bool BookHistoMassDis(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#endif //AMSSOFT
{
	//--
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		//only store for Li, Be, B and N
		if (*it>8 || *it<3 || *it==6) continue;
					
		//const int NISO=(*it==4)?3:2;
		const int NISO=(*it==4 || *it==6 || *it==8)?3:2;
		const int NCutoff=NISO+(NISO-1); //N sets above cutoff for each isotope, N-1 sets sets between each isotope range
		
		for (int ir=iSpanL; ir<iSpanU; ir++)
		{
			//only store for Inner+Max L1Q to reduce storage and speed up
			if (ir!=0) break;
			
			//loop for choutko and kalman rigidity
			for (int irig=0; irig<2; irig++) //irig=0: choutco, irig=1: kalman
			{
				string sRig="";
				if (irig==1) sRig="_kalman";
				
				for (int ibeta=0; ibeta<3; ibeta++)
				{
					//only store for Inner+Max L1Q to reduce storage and speed up
					if (ir!=0) break;
					
					outfile.mkdir(Form("Event_count%d/q%d/%s/MassDis%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta));
					outfile.cd(Form("Event_count%d/q%d/%s/MassDis%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta));
						
					for (int iCutoff=0; iCutoff<NCutoff; iCutoff++)
					{
						string sCutoff;
						if (iCutoff<NISO) sCutoff = Form("Cutoff%s", GetISOName(*it, iCutoff).c_str());
						else sCutoff = Form("Cutoff%s_%s", GetISOName(*it, iCutoff-NISO).c_str(), GetISOName(*it, iCutoff-NISO+1).c_str());
						
						outfile.mkdir(Form("Event_count%d/q%d/%s/MassDis%s_Beta%d/%s", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta, sCutoff.c_str()));
						outfile.cd(Form("Event_count%d/q%d/%s/MassDis%s_Beta%d/%s", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta, sCutoff.c_str()));
						//apply additional cut
						for (int ievcut=0; ievcut<2; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd track
						{
							if (ibeta>0 && ievcut>0) break; //only apply background subtraction cut for Tof-Beta
							
							//------mass distribution
							//--
							histn = Form("Mass%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 300, 0, 30));
							
							//--mass vs beta
							histn = Form("Mass%dvsBeta%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 750, 0, 1.5, 300, 0, 30));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
							
							//--mass vs Ekn
							histn = Form("Mass%dvsEkn%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iLiNBin, pLiBins, 300, 0, 30));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Ekn [GeV/n]", sBeta[ibeta].c_str()));
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
							
							//--mass vs rigidity
							histn = Form("Mass%dvsRig_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 300, 0, 30));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s Mass", sBeta[ibeta].c_str()));
							
							//------1/mass distribution
							//--
							histn = Form("rMass%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 200, 0, 1));
							
							//--1/mass vs beta
							histn = Form("rMass%dvsBeta%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 750, 0, 1.5, 200, 0, 1));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/(%s Mass)", sBeta[ibeta].c_str()));
							
							//--1/mass vs Ekn
							histn = Form("rMass%dvsEkn%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iLiNBin, pLiBins, 200, 0, 1));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Ekn [GeV/n]", sBeta[ibeta].c_str()));
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/(%s Mass)", sBeta[ibeta].c_str()));
							
							//--1/mass vs rigidity
							histn = Form("rMass%dvsRig_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, *it, irig, ievcut, sCutoff.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 200, 0, 1));
							GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
							GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/(%s Mass)", sBeta[ibeta].c_str()));
						} //ievcut
					} //iCutoff
				} //ibeta
			} //irig
		} //ir
	} //*it
	return true;
} //BookHistoMassDis

//--put all functions in this one
#ifdef AMSSOFT
bool BookHistoIsotope(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#else
bool BookHistoIsotope(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#endif //AMSSOFT
{
						//check rich refractive index //2021.09.18
						#ifdef CHECKRICHREFRACINDEX
						#endif //CHECKRICHREFRACINDEX
						
						#ifdef RICHVARCHECK
						#endif //RICHVARCHECK
						
						#ifdef RICHVARCHECKRESO
						#endif //RICHVARCHECKRESO
						
						//--check the rich tile position using no. event vs hit pos //2022.09.17
						#ifdef CHECKRICHGEO
						#endif //CHECKRICHGEO
	return true;
}

#ifdef AMSSOFT
bool SelIsotopeEv(HistoMan &hman1, SelEvent &ev, const int ir, const int charge, const int iQVer, const int irig)
#else
bool SelIsotopeEv(TObjArray &hman1, SelEvent &ev, const int ir, const int charge, const int iQVer, const int irig)
#endif //AMSSOFT
{
	int iRigAlgo=2*irig;
	double dRig=ev.GetRigidity(ir,iRigAlgo);
	string sRig="";
	if (irig==1) sRig="_kalman";
	
	//--pattern cut
	if (!(ev.Select_Rig(1, icffv, ir, dGeoSafety, iRigAlgo) && ev.Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir])) return false;
	
	//--charge cut
	if (!ev.Select_Charge(charge, ir, iQVer)) return false;
	
	//--unbiased L1Q cut
	#ifdef INNERUNBIAEDL1Q
	bool bUnbiasedL1XY=true;
	bool bUnbiasedL1Q=true;
	if (ir==0)
	{
		bUnbiasedL1XY = ev.Select_UnbiasedL1XY(iQVer);
		bUnbiasedL1Q = ev.Select_UnbiasedL1Q(*it, iQVer);
		
		if (!(bUnbiasedL1XY && bUnbiasedL1Q)) return false;
	}
	#endif //INNERUNBIAEDL1Q
	
	for (int ibeta=0; ibeta<3; ibeta++)
	{
		//only store for Li, Be, B and N
		if (charge>8 || charge<3 || charge==6) continue;
		
		//only store for Inner+Max L1Q to reduce storage and speed up
		if (ir!=0) break;
		
		if (ibeta>0)
		{
			if ((ibeta==1 && !ev.rich_NaF) || (ibeta==2 && ev.rich_NaF)) continue;
			
			//--distribution of rich q vs Mass
			if (ibeta>0 && ev.SelectRich())
			{
				double dBeta;
				if (ibeta==0) dBeta = ev.tof_betahmc;
				else dBeta = ev.rich_beta[0];
				double dMass = CalMass(charge, dBeta, dRig);
				histn = Form("richq0vsMass_beta%d_r%d_q%d_rig%d", ibeta, ir, charge, irig);
				GetHist2d(hman1, histn.c_str())->Fill(dMass, sqrt(ev.rich_q[0]), ww[ir]);
			}
			//if (!ev.SelectRich()) continue;
			
			//--rich efficiency
			if (ev.Select_RichGeomUnbiased(ibeta==1)) //should use the tracker track extrpolation to select the geometery and not use the rich ring at all, otherwise this is biased
			{
				for (int ievcut=0; ievcut<2; ievcut++)
				{
					if (ievcut==1) if (!(ev.ntrack==1 || ev.Select_Tk2nd())) continue;
					//eff vs rigidity
					for (int igr=1; igr>=0; igr--)
					{
						if (ev.isreal && igr==0) continue;
						histn = Form("RichEff_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
						if (igr==0) histn += "_gen";
						GetTEff(hman1, histn.c_str())->Fill(ev.SelectRich(charge), (igr==0)?ev.mmom/ev.mch:dRig, ww[ir]);
					} //igr
				} //ievcut
			}
			
			//store the rich efficiency selected using MC truth that particle suvival up to rich, for rich and LTof, iskip=17
			if (!ev.isreal && ev.CheckPrimary()>=17)
			{
				for (int ievcut=0; ievcut<2; ievcut++)
				{
					if (ievcut==1) if (!(ev.ntrack==1 || ev.Select_Tk2nd())) continue;
					for (int igr=1; igr>=0; igr--)
					{
						if (ev.isreal) continue;
						histn = Form("RichEff_beta%d_r%d_q%d_rig%d_evcut%d_MCTruth", ibeta, ir, charge, irig, ievcut);
						if (igr==0) histn += "_gen";
						GetTEff(hman1, histn.c_str())->Fill(ev.SelectRich(charge), (igr==0)?ev.mmom/ev.mch:dRig, ww[ir]);
					} //igr
				} //ievcut
			}
			
			if (!ev.SelectRich(charge)) continue; //add Rich q cut
		}
		
		double dBeta;
		if (ibeta==0) dBeta = ev.tof_betahmc;
		else dBeta = ev.rich_beta[0];
		double dMass = CalMass(charge, dBeta, dRig);
		
		for (int ievcut=0; ievcut<4; ievcut++)
		{
			if (ibeta!=0 && ievcut>1) break; //only apply tof chis cut (ievcut=2 or =3) when using tof beta
			if (ievcut==1)
			{
				//if (ev.ntrack!=1 && ev.Select_Tk2ndRecon()) continue; //2021.04.18
				if (!(ev.ntrack==1 || ev.Select_Tk2nd())) continue; //2021.05.24
			}
			else if (ievcut==2)
			{
				if (ev.tof_chist_n>=5 || ev.tof_chisc_n>=5) continue;
			}
			else if (ievcut==3)
			{
				if (ev.tof_chist>=5 || ev.tof_chisc>=5) continue;
			}
			
			//--rigidity event count
			histn = Form("rig_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist(hman1, histn.c_str())->Fill(dRig, ww[ir]);
			
			//--beta event count
			histn = Form("Beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist(hman1, histn.c_str())->Fill(dBeta, ww[ir]);
			
			//store the impact point Z coordinate, to figure out the center plane of rich //2022.05.30
			histn = Form("ImpactZ_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			if (ibeta>0 && ievcut<=1) GetHist(hman1, histn.c_str())->Fill(ev.rich_pos[2], ww[ir]);
			
			
			//--event count in Generated Ekn, for isotope accpetance //2021.09.14
			if (!ev.isreal && ir==0)
			{
				histn = Form("Ekn_beta%d_r%d_q%d_rig%d_evcut%d_GR%d_rew%d", ibeta, ir, charge, irig, ievcut, 0, 0);
				double mrig=ev.mmom/ev.mch;
				double mEkn = RigToEkn(mrig, ev.mch, MCMass);
				GetHist(hman1, histn.c_str())->Fill(mEkn, 1);
			}
			
			//--with Beta Correction
			for (int iMassCor=0; iMassCor<2; iMassCor++)
			{
				#ifndef MASSKEYSPDF
				if (iMassCor>0) continue;
				#endif //MASSKEYSPDF
				if (ev.isreal && iMassCor>0) continue;
				string sMassCor=(iMassCor==0)?"":"_MassCor";
				
				double dBetaCor = dBeta;
				double dBetaCorNotTuned = (ibeta==0)?ev.tof_betah:0;
				if (iMassCor==1)
				{
					if (ibeta==0) continue; //not correct for Tof Beta for now
					else
					{
						if (ievcut!=0) continue;
						double dBetaRich = RigToBeta((int)ev.mch, MCMass, ev.mevmom1[17]/ev.mch);
						if (iMassCor==1)
						{
							dBetaCor = GetCorBeta(dBetaRich, dBetaCor, (int)ev.mch, ievcut, ibeta); //correct with respect to the beta in RICH
							if (dBetaCor<0) continue;
						}
					}
				}
				//if (dBetaCor>=1.) continue;
				
				//--beta vs Rig
				histn = Form("Beta%dvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, charge, irig, ievcut, sMassCor.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dRig, dBetaCor, ww[ir]);
				
				//--1/beta vs rig
				histn = Form("rBeta%dvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, charge, irig, ievcut, sMassCor.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dRig, 1./dBetaCor, ww[ir]);
				
				//--store the TOF beta using tof_betah, that is not well tuned for MC. only for MC and TOF
				if (!ev.isreal && ibeta==0)
				{
					//--beta vs Rig
					histn = Form("Beta%d_NotTunedvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, charge, irig, ievcut, sMassCor.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(dRig, dBetaCorNotTuned, ww[ir]);
					
					//--1/beta vs rig
					histn = Form("rBeta%d_NotTunedvsRig_r%d_q%d_rig%d_evcut%d%s", ibeta, ir, charge, irig, ievcut, sMassCor.c_str());
					GetHist2d(hman1, histn.c_str())->Fill(dRig, 1./dBetaCorNotTuned, ww[ir]);
				}
			} //iMassCor
			
			//--beta diff between tof and rich
			if (ibeta>0)
			{
				for (int ievcut0=0; ievcut0<4; ievcut0++)
				{
					if (ievcut!=0) break;
					
					if (ievcut0==1)
					{
						//if (ev.ntrack!=1 && ev.Select_Tk2ndRecon()) continue; //2021.04.18
						if (!(ev.ntrack==1 || ev.Select_Tk2nd())) continue; //2021.05.24
					}
					else if (ievcut0==2)
					{
						if (ev.tof_chist_n>=5 || ev.tof_chisc_n>=5) continue;
					}
					else if (ievcut0==3)
					{
						if (ev.tof_chist>=5 || ev.tof_chisc>=5) continue;
					}
					
					//beta diff
					histn = Form("BetaTof-BetaRich%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, charge, irig, ievcut0);
					GetHist2d(hman1, histn.c_str())->Fill(dBeta, ev.tof_betahmc-dBeta, ww[ir]);
					
					//1/beta diff
					histn = Form("rBetaTof-rBetaRich%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, charge, irig, ievcut0);
					GetHist2d(hman1, histn.c_str())->Fill(dBeta, 1./ev.tof_betahmc-1./dBeta, ww[ir]);
				} //ievcut0
			}
			
			//--mass distribution
			histn = Form("Mass%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist(hman1, histn.c_str())->Fill(dMass, ww[ir]);
	
			//--mass vs beta
			histn = Form("Mass%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, charge, irig, ievcut);
			GetHist2d(hman1, histn.c_str())->Fill(dBeta, dMass, ww[ir]);
			
			//--1/mass distribution
			histn = Form("rMass%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist(hman1, histn.c_str())->Fill(1./dMass, ww[ir]);
			
			//--1/mass vs beta
			histn = Form("rMass%dvsBeta%d_r%d_q%d_rig%d_evcut%d", ibeta, ibeta, ir, charge, irig, ievcut);
			GetHist2d(hman1, histn.c_str())->Fill(dBeta, 1./dMass, ww[ir]);
			
			//--mass vs rigidity
			histn = Form("Mass%dvsRig_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist2d(hman1, histn.c_str())->Fill(dRig, dMass, ww[ir]);
			
			//--1/mass vs rigidity
			histn = Form("rMass%dvsRig_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist2d(hman1, histn.c_str())->Fill(dRig, 1./dMass, ww[ir]);
			
			//--mass vs Ekn
			histn = Form("Mass%dvsRig_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist2d(hman1, histn.c_str())->Fill(dRig, dMass, ww[ir]);
			
			//--1/mass vs Ekn
			histn = Form("rMass%dvsRig_r%d_q%d_rig%d_evcut%d", ibeta, ir, charge, irig, ievcut);
			GetHist2d(hman1, histn.c_str())->Fill(dRig, 1./dMass, ww[ir]);
			
			//distribution of tof chis
			for (int itofcstc=0; itofcstc<2; itofcstc++) //0: tof_chist, 1: tof_chisc
			{
				if (ibeta!=0) break;
				string stofcs=Form("tof_chis%s", ((itofcstc==0)?"t":"c"));
				
				for (int itofnorm=0; itofnorm<2; itofnorm++) //0: non-normlized, 1: normalized
				{
					string stofcsn=stofcs;
					if (itofnorm==1) stofcsn=stofcsn+"_n";
					
					float tofchis=ev.GetTOFChis(itofcstc, itofnorm);
					
					//--tof chis distribution
					histn = Form("%s%d_r%d_q%d_rig%d_evcut%d", stofcsn.c_str(), ibeta, ir, charge, irig, ievcut);
					GetHist(hman1, histn.c_str())->Fill(tofchis, ww[ir]);
					
					//--tof chis vs beta
					histn = Form("%s%dvsBeta_r%d_q%d_rig%d_evcut%d", stofcsn.c_str(), ibeta, ir, charge, irig, ievcut);
					GetHist2d(hman1, histn.c_str())->Fill(dBeta, tofchis, ww[ir]);
					
					//--tof chis vs rig
					histn = Form("%s%dvsRig_r%d_q%d_rig%d_evcut%d", stofcsn.c_str(), ibeta, ir, charge, irig, ievcut);
					GetHist2d(hman1, histn.c_str())->Fill(dRig, tofchis, ww[ir]);
				} //itofnorm
			} //itofcstc
		} //ievcut
	} //ibeta
	return true;
} //SelIsotopeEv

//isotope mass distribution with different beta cutoff (no rigidity cutoff)
#ifdef AMSSOFT
bool SelMassDis(HistoMan &hman1, SelEvent &ev, const int ir, const int charge, const int iQVer, const int irig)
#else
bool SelMassDis(TObjArray &hman1, SelEvent &ev, const int ir, const int charge, const int iQVer, const int irig)
#endif //AMSSOFT
{
	int iRigAlgo=2*irig;
	double dRig=ev.GetRigidity(ir,iRigAlgo);
	string sRig="";
	if (irig==1) sRig="_kalman";
	
	//only store for Inner+Max L1Q to reduce storage and speed up
	if (!(ev.Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir] && ir==0 && ev.isreal)) return false;
	
	//charge cut
	if (!ev.Select_Charge(charge, ir, iQVer)) return false;
	
	//unbiased L1Q
	#ifdef INNERUNBIAEDL1Q
	bool bUnbiasedL1XY=true;
	bool bUnbiasedL1Q=true;
	if (ir==0)
	{
		bUnbiasedL1XY = ev.Select_UnbiasedL1XY(iQVer);
		bUnbiasedL1Q = ev.Select_UnbiasedL1Q(*it, iQVer);
		
		if (!(bUnbiasedL1XY && bUnbiasedL1Q)) return false;
	}
	#endif //INNERUNBIAEDL1Q
	//--
	const int NISO=(charge==4 || charge==6 || charge==8)?3:2;
	const int NCutoff=NISO+(NISO-1); //N sets above cutoff for each isotope, N-1 sets sets between each isotope range
	
	for (int ibeta=0; ibeta<3; ibeta++)
	{
		if (ibeta>0)
		{
			if ((ibeta==1 && !ev.rich_NaF) || (ibeta==2 && ev.rich_NaF)) continue;
			if (!ev.SelectRich(charge)) continue; //add Rich q cut
		}
		
		double dBeta;
		if (ibeta==0) dBeta = ev.tof_betahmc;
		else dBeta = ev.rich_beta[0];
		double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
		double dMass = CalMass(charge, dBeta, dRig);
		
		for (int iCutoff=0; iCutoff<NCutoff; iCutoff++)
		{
			string sCutoff;
			bool bBetaCutoff=false;
			if (iCutoff<NISO)
			{
				sCutoff = Form("Cutoff%d%s", GetISOMass(charge, iCutoff), sElementShort[charge-1].c_str());
				bBetaCutoff = ev.Select_BetaCutoff(charge, GetISOMass(charge, iCutoff), ibeta, 1, 1, 1+dBetaCutoffSF[ibeta]);
			}
			else
			{
				sCutoff = Form("Cutoff%d%s_%d%s", GetISOMass(charge, iCutoff-NISO), sElementShort[charge-1].c_str(), GetISOMass(charge, iCutoff-NISO+1), sElementShort[charge-1].c_str());
				//use the cutoff calculated using the reconstructed direction
				#ifdef CALCUTOFFPI
				ev.CalCutoffpi();
				if (ev.bCutoffpi) cout << "calculated CutoffPi, " << ev.fCutoffpi << endl; //XXX
				#endif //CALCUTOFFPI
				bBetaCutoff = ev.Select_BetaCutoff(charge, GetISOMass(charge, iCutoff-NISO+1), ibeta, 1, 1, 1+dBetaCutoffSF[ibeta], false); //above heavier cutoff
				bBetaCutoff = bBetaCutoff && !ev.Select_BetaCutoff(charge, GetISOMass(charge, iCutoff-NISO), ibeta, 1, 1, 1-dBetaCutoffSF[ibeta], false); //below lighter cutoff
			}
			if (!bBetaCutoff) continue;
			
			//apply additional cut
			for (int ievcut=0; ievcut<2; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd track
			{
				if (ibeta>0 && ievcut>0) break; //only apply background subtraction cut for Tof-Beta
				if (ievcut==1)
				{
					//if (ev.ntrack!=1 && ev.Select_Tk2ndRecon()) continue;
					if (!(ev.ntrack==1 || ev.Select_Tk2nd())) continue;
				}
				
				//------mass distribution
				//--
				histn = Form("Mass%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist(hman1, histn.c_str())->Fill(dMass, ww[ir]);
				
				//--mass vs beta
				histn = Form("Mass%dvsBeta%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dBeta, dMass, ww[ir]);
				
				//--mass vs Ekn
				histn = Form("Mass%dvsEkn%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dEknRec, dMass, ww[ir]);
				
				//--mass vs rigidity
				histn = Form("Mass%dvsRig_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dRig, dMass, ww[ir]);
				
				//------1/mass distribution
				//--
				histn = Form("rMass%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist(hman1, histn.c_str())->Fill(1./dMass, ww[ir]);
				
				//--1/mass vs beta
				histn = Form("rMass%dvsBeta%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dBeta, 1./dMass, ww[ir]);
				
				//--1/mass vs Ekn
				histn = Form("rMass%dvsEkn%d_r%d_q%d_rig%d_evcut%d_%s", ibeta, ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dEknRec, 1./dMass, ww[ir]);
				
				//--1/mass vs rigidity
				histn = Form("rMass%dvsRig_r%d_q%d_rig%d_evcut%d_%s", ibeta, ir, charge, irig, ievcut, sCutoff.c_str());
				GetHist2d(hman1, histn.c_str())->Fill(dRig, 1./dMass, ww[ir]);
			} //ievcut
		} //iCutoff
	} //ibeta
	return true;
} //SelMassDis

//--call all isotope related selection
#ifdef AMSSOFT
bool SelIsotope(HistoMan &hman1, SelEvent &ev, const int ir, const int iQVer, const int irig)
#else
bool SelIsotope(TObjArray &hman1, SelEvent &ev, const int ir, const int iQVer, const int irig)
#endif //AMSSOFT
{
	//--use new charge selection function
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		#ifdef ISOTOPEEV
		SelIsotopeEv(hman1, ev, ir, *it, iQVer, irig);
		#endif //ISOTOPEEV
		
		//isotope mass distribution with different beta cutoff to be fitted by mass template (no rigidity cutoff)
		#ifdef MASSDIS
		SelMassDis(hman1, ev, ir, *it, iQVer, irig);
		#endif //MASSDIS
		
		//check rich refractive index //2021.09.18
		#ifdef CHECKRICHREFRACINDEX
		#endif //CHECKRICHREFRACINDEX
	} //it
	return true;
}
#endif //ISOTOPEEV_H
