#ifndef ACCEPTANCE_H
#define ACCEPTANCE_H
//-----------------
//create date: 2023.02.16
//-----------------
//booking histogram, doing selection and filling the histogram for All acceptance related
//-----------------
//2023.02.16
//	1) similar to TOI, store the Acceptance BELOW L1, i.e. skip>0 and <L2, BookHistoAccBelowL1 and SelAccBelowL1 TODO
//
//2023.02.19
//	1) update the TrMCCluster cut, to use mtrpri[il] instead, which is trmc->IsPrimary()?1:0
//
//2023.03.13
//	1) add new function, MCL1L2BranchingRatio, to store the MC fragmentation branching ratio to different charge and Mass
//	2) add parameter irig for Acceptance TODO
//-----------------

#include "SelEvent14.h"
#include "bins.h" //use EvenLogBin()

//extend the generated rigidity range for l1 focus to larger than 2000 GV
double dExtRig=0., dExtww0=1.;

//--Starting charge for Top Of Instrument
const int iTOILQ=3;

//#define TOIL1QDIS //store the L1Q distribution with different MC truth cut

#define REDUCESIZETOI //to speed up, do not store toiq to fur away from mch

//-- //2023.03.13
#ifdef AMSSOFT
bool BookHistoMCBranchingRatio(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoMCBranchingRatio(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{
	if (ev.isreal)
	{
		cout << "Only run BookHistoMCL1L2BranchingRatio for MC while ev.isreal=" << ev.isreal << endl;
		return false;
	}
	
	string sDirBR=Form("MCBranchingRatio%d", QVer);
	if (dir) outfile.mkdir(sDirBR.c_str());
	
	EvenLogBinEkn();
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		for (int ipart=0; ipart<1; ipart++) //ipart=0: L1->L2, ipart=1: L8->L9
		{
			if (dir)
			{
				outfile.mkdir(Form("%s/q%d/%s", sDirBR.c_str(), *it, ((ipart==0)?"L1L2":"L8L9")));
				outfile.cd(Form("%s/q%d/%s", sDirBR.c_str(), *it, ((ipart==0)?"L1L2":"L8L9")));
			}
			
			for (int ire=0; ire<2; ire++) //ire=0: rigidity, =1: Ekn
			{
				int NBinRE = (ire==0)?iNbinGenRig:iNbinGenEkn;
				double *pBinRE = (ire==0)?pBinsGenRig:pBinsGenEkn;
				string sEVar = (ire==0)?"Rig":"Ekn";
				string sEVarFull = (ire==0)?"Rigidity":"Ekn";
				string sEVarUnit = (ire==0)?"GV":"GeV/n";
				
				for (int imec=0; imec<2; imec++)
				{
					//--fragment into different charge
					histn = Form("FragQvs%s_q%d_part%d", sEVar.c_str(), *it, ipart);
					histn += Form("_%s", ((imec==0)?"MaxE":"MaxC"));
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NBinRE, pBinRE, 350, 0, 35));
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("Generated %s [%s]", sEVarFull.c_str(), sEVarUnit.c_str()));
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					
					//--mass distribution for each channel, store the fragment channel up to Z-10
					int iQFracL=*it-4;
					if (iQFracL<=0) iQFracL=1;
					for (int iq=iQFracL; iq<*it; iq++)
					{
						histn = Form("FragMvs%s_q%d_part%d_FracQ%d", sEVar.c_str(), *it, ipart, iq);
						histn += Form("_%s", ((imec==0)?"MaxE":"MaxC"));
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NBinRE, pBinRE, 300, 0, 30));
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("Generated %s [%s]", sEVarFull.c_str(), sEVarUnit.c_str()));
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					} //iq
				} //imec
			} //ire
		} //ipar
	} //it
	
	//--
	return true;
} //BookHistoMCBranchingRatio

//--
#ifdef AMSSOFT
bool BookHistoAcc(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoAcc(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{
	if (ev.isreal)
	{
		cout << "Only book Acceptance for MC while ev.isreal=" << ev.isreal << endl;
		return false;
	}
	
	EvenLogBinGeom(); //2023.04.04
	
	//--
	string sDirAcc=Form("Acceptance%d", QVer);
	if (dir) outfile.mkdir(sDirAcc.c_str());
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		if (dir) outfile.mkdir(Form("%s/q%d", sDirAcc.c_str(), *it));
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		for (int ir=0; ir<3; ir++)
		{
			for (int irig=0; irig<NRig; irig++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				string sRig=sRigName[irig]; //2023.03.10
					
				if (dir)
				{
					//outfile.mkdir(Form("%s/q%d/%s", sDirAcc.c_str(), *it, sSpan[ir].c_str()));
					//outfile.cd(Form("%s/q%d/%s", sDirAcc.c_str(), *it, sSpan[ir].c_str()));
					//2023.03.27
					histn = Form("%s/q%d/%s", sDirAcc.c_str(), *it, sSpan[ir].c_str());
					histn += sRig;
					outfile.mkdir(histn.c_str());
					outfile.cd(histn.c_str());
				}
		
				//----geometrical acceptance
				//geometrical acceptance for l1 & l19 separately
				if (irig==0)
				{
					histn = Form("rig_r%d_q%d_geom_rew0_MCV%d", ir, *it, MCVer);
					//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig)); //Even log bin
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
				}
				
				#ifdef ACCSYS
				//----break down of effective acceptance
				//loop using two sets of information to decide stay primary: 0: NSkip, 1: TrMCCluster
				//for (int iMCTruth=0; iMCTruth<2; iMCTruth++)
				for (int iMCTruth=0; iMCTruth<3; iMCTruth++)
				{
					//within fiducial volume, stay primary //should be same for all rig algorithm
					histn = Form("rig_r%d_q%d_geom&pri_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
					histn += sRig;
					//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					
					//within fiducial volume, stay primary up to certain layer //2020.03.18 //should be same for all rig algorithm
					for (int il=0; il<4; il++) //il=0: l1, il=1: l2, il=2: l8, il=3: l9
					{
						if (il==3 && ir==1) continue; //only store up to L9 for FullSpan
						int SurLayer=0;
						if (il==0) SurLayer=1;
						else if (il==1) SurLayer=2;
						else if (il==2) SurLayer=8;
						else if (il==3) SurLayer=9;
						
						histn = Form("rig_r%d_q%d_geom&pri_L%d_rew0_MCV%d_MCTruth%d", ir, *it, SurLayer, MCVer, iMCTruth);
						histn += sRig;
						//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					} //il
					
					//within fiducial volume, stay primary and pass event selection
					histn = Form("rig_r%d_q%d_geom&pri&sel_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
					histn += sRig;
					//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					//breakdown into each subdetector: Inner Tracker, external layer, TofQ cut //2019.11.06
					//0: Inner Tracker, including Good Particle, InTrk fiducial & TrTrack sel, InnerQ cut, TofQ lower limits;
					//1: L1
					//2: L9 (Full Span only)
					//3: Chisqure depends on geometry
					//4: UTofQ upper limit cut
					//5: Trigger
					for (int ieff=0; ieff<6; ieff++)
					{
						if (ir==1 && ieff==2) continue;
						histn = Form("rig_r%d_q%d_geom&pri&sel%d_rew0_MCV%d_MCTruth%d", ir, *it, ieff, MCVer, iMCTruth);
						histn += sRig;
						//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					} //ieff
				
					//within fiducial volume, pass event selection
					histn = Form("rig_r%d_q%d_geom&sel_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
					histn += sRig;
					//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
				
					//pass event selection, should be the same as rig_r%d_q%d_GR0_rew0
					histn = Form("rig_r%d_q%d_sel_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
					histn += sRig;
					//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
				
					//pass event selection, stay primary
					histn = Form("rig_r%d_q%d_sel&pri_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
					histn += sRig;
					//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
					GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
					GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
				
					//non-primary events after event selection
					//use TrMCCluster to check secondary info, now have only 2 type
					for (int itype=0; itype<2; itype++)
					{
						//------event distribution
						histn = Form("rig_r%d_q%d_sel&nopri%d_rew0_MCV%d_MCTruth%d", ir, *it, itype, MCVer, iMCTruth);
						histn += sRig;
						//hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRigGeom, pBinsGenRigGeom)); //Even log bin, up to 8 TV //2023.04.04
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
					
						//------charge distribution if non of the maximum momentum are not the same maximum charge has same charge
						if (itype>0)
						{
							histn = Form("ChargevsRig_r%d_q%d_sel&nopri%d_rew0_MCV%d_MCTruth%d", ir, *it, itype, MCVer, iMCTruth);
							histn += sRig;
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig, 35, 0.5, 35.5));
							GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Generated Rigidity [GV]");
							GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Secondary charge");
						}
					} //itype
				} //iMCTruth
				#endif //ACCSYS
				
				//----effective acceptance
				for (int igr=0; igr<2; igr++) //igr=0: generated rigidity, igr=1: reconstructed rigidity
				{
					for (int irew=0; irew<4; irew++) //=0: without reweight, =1: shape reweight, =2: only reweight to combin l1&l19, =3: l1&l19 combind + shape reweight
					{
						histn = Form("rig_r%d_q%d_GR%d_rew%d", ir, *it, igr, irew);
						histn += sRig;
						if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig)); //2Even log bin
						else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
						
						//reconstructed rigidity distribution with generated rigidity with [1,2] TV
						if (ir==1)
						{
							histn = Form("rig_r%d_q%d_GR%d_rew%d_Gen_1_2TV", ir, *it, igr, irew);
							histn += sRig;
							if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig)); //Even log bin
							else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							if (igr==0 && irew==0)
							{
								histn = Form("rig_r%d_q%d_GR%d_rew%d_Gen_1_2TV_old", ir, *it, igr, irew);
								histn += sRig;
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig)); //Even log bin
							}
						}
						
						//2021.10.22
						//#ifndef INNERUNBIAEDL1Q
						#ifdef INNERUNBIAEDL1Q
						if (ir==0)
						{
							histn = Form("rig_r%d_q%d_GR%d_rew%d_UnbiasedL1Q", ir, *it, igr, irew);
							histn += sRig;
							if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig)); //2Even log bin
							else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
							GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
						}
						#endif //INNERUNBIAEDL1Q
						
						#ifdef ACCCHECK
						if (ir!=1) continue;
						histn = Form("rig_r%d_q%d_GR%d_rew%d_acccheck", ir, *it, igr, irew);
						histn += sRig;
						if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
						else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
						GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
						
						//reconstructed rigidity distribution with generated rigidity with [1,2] TV
						if (ir==1)
						{
							histn = Form("rig_r%d_q%d_GR%d_rew%d_acccheck_Gen_1_2TV", ir, *it, igr, irew);
							histn += sRig;
							if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
							else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						}
						#endif //ACCCHECK
					} //irew
				} //igr
				
				//distribution of re-generated rigidity
				if (ir==1 && MCVer==1)
				{
					//------re-generated rigidity distribution
					histn = Form("rig_r1_q%d_reGenRig", *it);
					histn += sRig;
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 2000, 1500, 3500));
				}
			} //irig
		} //ir
	} //*it
	
	//----
	BookHistoMCBranchingRatio(hman1, ev, outfile, QVer, dir);
	
	return true;
} //BookHistoAcc

#ifdef AMSSOFT
bool BookHistoTOI(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoTOI(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{
	if (ev.isreal)
	{
		cout << "Only book Acceptance for MC while ev.isreal=" << ev.isreal << endl;
		return false;
	}
	
	//--
	//string sDirAcc=Form("Acceptance%d", QVer);
	string sDirAcc=Form("TOICorrection%d", QVer); //2023.02.16
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		for (int ir=0; ir<3; ir++)
		{
			for (int irig=0; irig<NRig; irig++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				string sRig=sRigName[irig]; //2023.03.10
				
				//acceptance for lower charge for TOI correction
				for (int itoiq=iTOILQ; itoiq<=*it; itoiq++)
				{
					#ifdef REDUCESIZETOI //to speed up, do not store toiq to fur away from mch
					if (ev.mch>14 && itoiq<=8) continue;
					#endif //REDUCESIZETOI
					
					if (dir) 
					{
						//outfile.mkdir(Form("%s/q%d/%s/TOI_q%d", sDirAcc.c_str(), *it, sSpan[ir].c_str(), itoiq));
						//outfile.cd(Form("%s/q%d/%s/TOI_q%d", sDirAcc.c_str(), *it, sSpan[ir].c_str(), itoiq));
						histn = Form("%s/q%d/%s", sDirAcc.c_str(), *it, sSpan[ir].c_str());
						histn += sRig;
						histn += Form("/TOI_q%d", itoiq);
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					
					//----effective acceptance
					for (int igr=0; igr<2; igr++) //igr=0: generated rigidity, igr=1: reconstructed rigidity
					{
						for (int irew=0; irew<4; irew++) //=0: without reweight, =1: shape reweight, =2: only reweight to combin l1&l19, =3: l1&l19 combind + shape reweight
						{
							//store the charge distribution for both with or without requiring interaction before Tracker L1 using MC truth
							//this is to check the below L1 BG, for itoiq<mch, events that remain as mch in L1 should be below L1 BG, which should only presented in type1 but not type0 where the mc truth cut is applied
							#ifdef TOIL1QDIS
							for (int itype=0; itype<2; itype++) //itoiq=mch: only itype=0 (no MC truth cut), !=mch: itype=0: with MC truth cut, =1: without MC truth cut
							{
								if (*it-itoiq>3) continue; //to reduce size, store only few nearby channel //2023.02.16
								if (itoiq==*it && itype==1) continue;
								histn = Form("L1QvsRig_r%d_q%d_GR%d_rew%d_TOIQ%d_type%d", ir, *it, igr, irew, itoiq, itype);
								histn += sRig; //2023.03.27
								if (igr==0) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig, 350, 0, 35));
								else if (igr==1) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle("L1Q");
								
								histn = Form("unbiasedL1QvsRig_r%d_q%d_GR%d_rew%d_TOIQ%d_type%d", ir, *it, igr, irew, itoiq, itype);
								histn += sRig; //2023.03.27
								if (igr==0) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig, 350, 0, 35));
								else if (igr==1) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle("unbiased L1Q");
								
							} //itype
							#endif //TOIL1QDIS
							
							//store the result with additional cuts, for example ntrack==1
							//for (int iTrMC=-1; iTrMC<2; iTrMC++) //for itoiq!=mch, iTrMC=-1: iSkip<0 && no TrMCCluster cut, =0: iSkip<0 && TrMCCluster Q at L1 should be different to mch, =1: fulfill either 1) iSkip<0 or 2) TrMCCluster Q changed (should be the smae as -1, as all 2) should be included in 1), need to check)XXX
							//2023.02.19
							for (int iTrMC=-1; iTrMC<3; iTrMC++) //for itoiq!=mch, iTrMC=-1: iSkip<0, =0: iSkip<0 && TrMCCluster Q at L1 should be different to mch, =1: fulfill either 1) iSkip<0 or 2) TrMCCluster Q changed, =2: use mtrpri to check whether primary
							{
								if (itoiq==*it && iTrMC>=0) break;
								
								string sTrMC="";
								if (iTrMC>=0) sTrMC=Form("_TrMC%d", iTrMC);
								
								//for (int ievcut=0; ievcut<3; ievcut++) //ievcut=0: no additional cut, ievcut=1: ntrack==1, ievcut=2: nomalized tof_chist<5 && tof_chisc<5
								for (int ievcut=0; ievcut<4; ievcut++) //ievcut=0: no additional cut, =1: ntrack==1 || no 2nd check, =2: ntrack==1, =3: no 2nd track
								{
									if (ievcut>0 && iTrMC>1) break;
									
									string sEvCut="";
									if (ievcut>0) sEvCut=Form("_EvCut%d", ievcut);
									
									histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d%s%s", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
									histn += sRig; //2023.03.27
									if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
									else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
									GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
									GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
									
									//store generated rigidty also in flux bin
									if (igr==0)
									{
										histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d%s%s_fluxbin", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
										histn += sRig; //2023.03.27
										hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
										GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
										GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
									}
								} //ievcut
							} //iTrMC
							
							//L1Inner selection+Full Span geometry
							if (ir==1)
							{
								histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_FSGeom", ir, *it, igr, irew, itoiq);
								histn += sRig; //2023.03.27
								if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
								else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
								GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
								
								//store generated rigidty also in flux bin
								if (igr==0)
								{
									histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_FSGeom_fluxbin", ir, *it, igr, irew, itoiq);
									histn += sRig; //2023.03.27
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
									GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
									GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
								}
							}
							
							//store the event counts without requiring interaction before Tracker L1 using MC truth
							if (itoiq==*it) continue;
							histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_noMCTruthCut", ir, *it, igr, irew, itoiq);
							histn += sRig; //2023.03.27
							if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
							else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
							GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
							
							if (igr==0)
							{
								histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_fluxbin_noMCTruthCut", ir, *it, igr, irew, itoiq);
								histn += sRig; //2023.03.27
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
							}
							
						} //irew
					} //igr
				} //itoiq
			} //irig
		} //ir
	} //*it
	
	return true;
} //BookHistoTOI

#ifdef AMSSOFT
bool BookHistoBelowL1MC(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoBelowL1MC(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false)
#endif //AMSSOFT
{
	if (ev.isreal)
	{
		cout << "Only book Acceptance for MC while ev.isreal=" << ev.isreal << endl;
		return false;
	}
	
	//--
	string sDirAcc=Form("BelowL1MC%d", QVer);
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		for (int ir=0; ir<3; ir++)
		{
			for (int irig=0; irig<NRig; irig++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev.amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				string sRig=sRigName[irig]; //2023.03.10
				
				//acceptance for lower charge for TOI correction
				for (int itoiq=iTOILQ; itoiq<=*it; itoiq++)
				{
					#ifdef REDUCESIZETOI //to speed up, do not store toiq to fur away from mch
					if (ev.mch>14 && itoiq<=8) continue;
					#endif //REDUCESIZETOI
					
					if (dir) 
					{
						//outfile.mkdir(Form("%s/q%d/%s/BelowL1MC_q%d", sDirAcc.c_str(), *it, sSpan[ir].c_str(), itoiq));
						//outfile.cd(Form("%s/q%d/%s/BelowL1MC_q%d", sDirAcc.c_str(), *it, sSpan[ir].c_str(), itoiq));
						histn = Form("%s/q%d/%s", sDirAcc.c_str(), *it, sSpan[ir].c_str());
						histn += sRig;
						histn += Form("/BelowL1MC_q%d", itoiq);
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					
					//----effective acceptance
					for (int igr=0; igr<2; igr++) //igr=0: generated rigidity, igr=1: reconstructed rigidity
					{
						for (int irew=0; irew<4; irew++) //=0: without reweight, =1: shape reweight, =2: only reweight to combin l1&l19, =3: l1&l19 combind + shape reweight
						{
							//store the charge distribution for both with or without requiring interaction before Tracker L1 using MC truth
							#ifdef TOIL1QDIS
							for (int itype=0; itype<2; itype++) //itoiq=mch: only itype=0 (no MC truth cut), !=mch: itype=0: with MC truth cut, =1: without MC truth cut
							{
								if (*it-itoiq>3) continue; //to reduce size, store only few nearby channel //2023.02.16
								if (itoiq==*it && itype==1) continue;
								histn = Form("L1QvsRig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_type%d", ir, *it, igr, irew, itoiq, itype);
								histn += sRig; //2023.03.27
								if (igr==0) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig, 350, 0, 35));
								else if (igr==1) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle("L1Q");
								
								histn = Form("unbiasedL1QvsRig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_type%d", ir, *it, igr, irew, itoiq, itype);
								histn += sRig; //2023.03.27
								if (igr==0) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig, 350, 0, 35));
								else if (igr==1) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 350, 0, 35));
								GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle("unbiased L1Q");
								
							} //itype
							#endif //TOIL1QDIS
							
							//store the result with additional cuts, for example ntrack==1
							//for (int iTrMC=-1; iTrMC<2; iTrMC++) //for itoiq!=mch, iTrMC=-1: iSkip cut && no TrMCCluster cut, =0: iSkip cut && TrMCCluster Q cut, =1: fulfill either 1) iSkip cut or 2) TrMCCluster Q cut (should be the smae as -1, as all 2) should be included in 1), need to check)XXX
							//2023.02.19
							for (int iTrMC=-1; iTrMC<3; iTrMC++) //for itoiq!=mch, iTrMC=-1: iSkip<0, =0: iSkip<0 && TrMCCluster Q at L1 should be different to mch, =1: fulfill either 1) iSkip<0 or 2) TrMCCluster Q changed, =2: use mtrpri to check whether primary
							{
								if (itoiq==*it && iTrMC>=0) break;
								
								string sTrMC="";
								if (iTrMC>=0) sTrMC=Form("_TrMC%d", iTrMC);
								
								//for (int ievcut=0; ievcut<3; ievcut++) //ievcut=0: no additional cut, ievcut=1: ntrack==1, ievcut=2: nomalized tof_chist<5 && tof_chisc<5
								for (int ievcut=0; ievcut<4; ievcut++) //ievcut=0: no additional cut, =1: ntrack==1 || no 2nd check, =2: ntrack==1, =3: no 2nd track
								{
									if (ievcut>0 && iTrMC>1) break;
									
									string sEvCut="";
									if (ievcut>0) sEvCut=Form("_EvCut%d", ievcut);
									
									histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d%s%s", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
									histn += sRig; //2023.03.27
									if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
									else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
									GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
									GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
									
									//store generated rigidty also in flux bin
									if (igr==0)
									{
										histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d%s%s_fluxbin", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
										histn += sRig; //2023.03.27
										hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								
										GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
										GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
									}
								} //ievcut
							} //iTrMC
							
							//L1Inner selection+Full Span geometry
							if (ir==1)
							{
								histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_FSGeom", ir, *it, igr, irew, itoiq);
								histn += sRig; //2023.03.27
								if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
								else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						
								GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
								
								//store generated rigidty also in flux bin
								if (igr==0)
								{
									histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_FSGeom_fluxbin", ir, *it, igr, irew, itoiq);
									histn += sRig; //2023.03.27
									hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
									GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
									GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
								}
							}
							
							//store the event counts without requiring interaction below Tracker L1 using MC truth
							if (itoiq==*it) continue;
							histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_noMCTruthCut", ir, *it, igr, irew, itoiq);
							histn += sRig; //2023.03.27
							if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
							else if (igr==1) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
							GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
							if (igr==0)
							{
								histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_fluxbin_noMCTruthCut", ir, *it, igr, irew, itoiq);
								histn += sRig; //2023.03.27
								hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", (igr==0)?"Generated":"Reconstructed"));
								GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
							}
							
						} //irew
					} //igr
				} //itoiq
			} //irig
		} //ir
	} //*it
	
	return true;
} //BookHistoBelowL1MC

#ifdef AMSSOFT
bool SelBranchingRatio(HistoMan &hman1, SelEvent &ev, const int QVer)
#else
bool SelBranchingRatio(TObjArray &hman1, SelEvent &ev, const int QVer)
#endif //AMSSOFT
{
	if (ev.isreal) return false;
	
	//skip events with generated rigidity outside the range
	double mrig=ev.mmom/ev.mch;
	if ((MCVer==1 && (mrig<1. || mrig>2000.)) || (MCVer==2 && (mrig<2. || mrig>8000.))) return false;
	
	const int iSkip=ev.CheckPrimary();
	
	//--
	bool bSkip=true;
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		for (int ipart=0; ipart<1; ipart++) //ipart=0: L1->L2, ipart=1: L8->L9
		{
			if (ipart==0) bSkip = (iSkip>=0 && iSkip<7);
			else if (ipart==0) bSkip = (iSkip>=13 && iSkip<18);
			if (!bSkip) continue;
			
			for (int ire=0; ire<2; ire++) //ire=0: rigidity, =1: Ekn
			{
				string sEVar = (ire==0)?"Rig":"Ekn";
				double dEVar = (ire==0)?mrig:RigToEkn(mrig, ev.mch, MCMass);
				
				for (int imec=0; imec<2; imec++)
				{
					double dFracQ = ev.mparc[imec];
					
					//--fragment into different charge
					histn = Form("FragQvs%s_q%d_part%d", sEVar.c_str(), *it, ipart);
					histn += Form("_%s", ((imec==0)?"MaxE":"MaxC"));
					GetHist(hman1, histn.c_str())->Fill(dEVar, dFracQ);
					
					//--mass distribution for each channel, store the fragment channel up to Z-10
					double dQFracL=*it-4;
					if (dQFracL<=0) dQFracL=1;
					for (int iq=dQFracL; iq<*it; iq++)
					{
						if (fabs(dFracQ-iq)>0.5) continue;
						
						histn = Form("FragMvs%s_q%d_part%d_FracQ%d", sEVar.c_str(), *it, ipart, iq);
						histn += Form("_%s", ((imec==0)?"MaxE":"MaxC"));
						GetHist(hman1, histn.c_str())->Fill(dEVar, ev.mparp[imec]);
					} //iq
				} //imec
			} //ire
		} //ipart
	} //it
	
	//--
	return true;
} //SelMCBranchingRatio

#ifdef AMSSOFT
bool SelAcc(HistoMan &hman1, SelEvent &ev, const int QVer)
#else
//bool SelAcc(TObjArray &hman1, SelEvent &ev, const int QVer)
bool SelAcc(TObjArray &hman1, SelEvent &ev, const int QVer, const int irig) //2023.03.27
#endif //AMSSOFT
{
	//skip events with generated rigidity outside the range
	double mrig=ev.mmom/ev.mch;
	if ((MCVer==1 && (mrig<1. || mrig>2000.)) || (MCVer==2 && (mrig<2. || mrig>8000.))) return false;
	
	string sRig=sRigName[irig]; //2023.03.10
	
	const int iSkip=ev.CheckPrimary();
	
	//----Geometrical acceptance
	for (int ir=0; ir<3; ir++)
	{
		bool bSkip=false;
		//if (ir>0) bSkip=iSkip>=((ir==1)?13:18); //L8 for L1Inner/Inner and L9 for FS
		//if (ir>0) bSkip=iSkip>=((ir==1)?12:18); //L7 for L1Inner/Inner and L9 for FS
		if (ir>0) bSkip=ev.PrimaryUpToTrLayer((ir==1)?8:9); //now use saver method to require the skip information //2023.04.15
		//--primary using TrMCCluster info
		bool bMCCPri=false;
		//if (ir==1) bMCCPri=(ev.mtrpri[7-1]==1) || (ev.mtrpri[8-1]==1); //L7 || L8
		if (ir==1) bMCCPri=(ev.mtrpri[8-1]==1); //L8
		else if (ir==2) bMCCPri=(ev.mtrpri[9-1]==1);
		//--require the charge of TrMCCluster to be matched //2023.04.15
		bool bMCCQ=false;
		if (ir==1) bMCCQ=(ev.GetTrMCLZ(7)==ev.mch) || (ev.GetTrMCLZ(8)==ev.mch); //L7 || L8
		else if (ir==2) bMCCQ=(ev.GetTrMCLZ(9)==ev.mch);
		
		//------geometrical acceptance
		if (ev.Select_GeomAccept(ir))
		{
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				#ifndef EFFSEC
				if (!ev.isreal && *it!=ev.mch) continue;
				#endif //EFFSEC
				
				//----break down the event selection into subsets
				bool bEvSel[6];
				//0: Inner Tracker+UTOFQ lower(+LTOFQ lower for FS)
				//bEvSel[0] = bPart && bBeta && bTkgeom[0] && bTrack[0] && ev.Select_InnerQ(*it, QVer) && ev.GetSubDetQ(1, QVer)>ev.GetQSelRange(0, *it, 1, QVer);
				bEvSel[0] = bPart && bBeta && bTkgeom[0] && bTrack[0][irig] && ev.Select_InnerQ(*it, QVer) && ev.GetSubDetQ(1, QVer)>ev.GetQSelRange(0, *it, 1, QVer); //2023.03.27
				if (ir==2) bEvSel[0] = bEvSel[0] && ev.GetSubDetQ(2, QVer)>ev.GetQSelRange(0, *it, 2, QVer);
				//1: L1: fiducial volume, L1XY, L1Q & Q status
				bEvSel[1] = ev.IsPassTkL(0) && (ev.HasTkLHitXY(0)==2) && ev.Select_ExtQ(*it, 1, QVer);
				//2: L9: fiducial volume, L9XY, L9Q
				bEvSel[2] = true;
				if (ir==2) bEvSel[2] = ev.IsPassTkL(8) && (ev.HasTkLHitXY(8)==2) && ev.Select_ExtQ(*it, 3, QVer);
				//3: chisquare
				//if (ir==1) bEvSel[3] = ev.GetChis(1, 1) < 10  && ev.GetL1ChisY()<10;
				//else bEvSel[3] = ev.GetChis(2, 1) < 10;
				//2023.03.27
				if (ir==1) bEvSel[3] = ev.GetChis(1, 1, irig) < 10  && ev.GetL1ChisY(irig)<10;
				else bEvSel[3] = ev.GetChis(2, 1, irig) < 10;
				//4: UTofQ upper limit
				bEvSel[4] = ev.GetSubDetQ(1, QVer)<ev.GetQSelRange(1, *it, 1, QVer);
				//5: Trigger
				bEvSel[5] = bTrig;
				
				if (irig==0)
				{
					histn = Form("rig_r%d_q%d_geom_rew0_MCV%d", ir, *it, MCVer);
					GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
				}
				
				#ifdef ACCSYS
				if (ir==0) continue;
				
				//----break down of effective acceptance
				//use two kinds of information to check whether stay primary
				//for (int iMCTruth=0; iMCTruth<2; iMCTruth++)
				for (int iMCTruth=0; iMCTruth<3; iMCTruth++)
				{
					//bool bPri=(iMCTruth==0)?bSkip:bMCCPri;
					bool bPri=false;
					if (iMCTruth==0) bPri = bSkip;
					else if (iMCTruth==1) bPri = bMCCPri;
					else if (iMCTruth==2) bPri = bMCCQ;
					if (bPri)
					{
						//within fiducial volume, stay primary
						histn = Form("rig_r%d_q%d_geom&pri_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
						histn += sRig; //2023.03.27
						GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
				
						//within fiducial volume, stay primary and pass event selection
						//if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && bRig[ir] && bTrack[ir] && bTkgeom[ir] && ev.Select_Charge(*it, ir, QVer))
						if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && bRig[ir][irig] && bTrack[ir][irig] && bTkgeom[ir] && ev.Select_Charge(*it, ir, QVer)) //2023.03.27
						{
							histn = Form("rig_r%d_q%d_geom&pri&sel_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
							histn += sRig; //2023.03.27
							GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
						}
						//breakdown into each subdetector: Inner Tracker, external layer, TofQ cut
						//0: Inner Tracker, including Good Particle, InTrk fiducial & TrTrack sel, InnerQ cut, TofQ lower limits;
						//1: L1
						//2: L9 (Full Span only)
						//3: Chisqure depends on geometry
						//4: UTofQ upper limit cut
						//5: Trigger
						for (int ieff=0; ieff<6; ieff++)
						{
							if (ir==1 && ieff==2) continue;
							if (!bEvSel[ieff]) break;
							histn = Form("rig_r%d_q%d_geom&pri&sel%d_rew0_MCV%d_MCTruth%d", ir, *it, ieff, MCVer, iMCTruth);
							histn += sRig; //2023.03.27
							GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
						} //ieff
					}
					
					//within fiducial volume, stay primary up to certain layer
					for (int il=0; il<4; il++) //il=0: l1, il=1: l2, il=2: l8, il=3: l9
					{
						if (il==3 && ir==1) continue;
						int SurLayer=0;
						bool bSurvive=true;
						if (il==0)
						{
							SurLayer=1;
							bSurvive=(iSkip>=0);
						}
						else if (il==1)
						{
							SurLayer=2;
							bSurvive=(iSkip>=7);
						}
						else if (il==2)
						{
							SurLayer=8;
							bSurvive=(iSkip>=13);
						}
						else if (il==3)
						{
							SurLayer=9;
							bSurvive=(iSkip>=18);
						}
						
						histn = Form("rig_r%d_q%d_geom&pri_L%d_rew0_MCV%d_MCTruth%d", ir, *it, SurLayer, MCVer, iMCTruth);
						histn += sRig; //2023.03.27
						if (bSurvive) GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
					} //il
					
					//within fiducial volume, pass event selection //2019.10.15
					//if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && bRig[ir] && bTrack[ir] && bTkgeom[ir] && ev.Select_Charge(*it, ir, QVer))
					if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && bRig[ir][irig] && bTrack[ir][irig] && bTkgeom[ir] && ev.Select_Charge(*it, ir, QVer)) //2023.03.27
					{
						histn = Form("rig_r%d_q%d_geom&sel_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
						histn += sRig;
						GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
					}
				} //iMCTruth
				#endif //ACCSYS
			} //it
		}
		
		#ifdef ACCSYS
		//if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && bRig[ir] && bTrack[ir] && bTkgeom[ir])
		if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && bRig[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.27
		{
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				#ifndef EFFSEC
				if (!ev.isreal && *it!=ev.mch) continue;
				#endif //EFFSEC
				
				if (ev.Select_Charge(*it, ir, QVer))
				{
					//use two kinds of information to check whether stay primary
					//--secondary information using TrMCCluster
					bool bSecSameQ=false;
					//if (ir==1) bSecSameQ = (ev.mtrz[7-1]==*it) || (ev.mtrz[8-1]==*it);
					//else if (ir==2) bSecSameQ = (ev.mtrz[9-1]==*it);
					//2023.02.19
					if (ir==1) bSecSameQ = (ev.GetTrMCLZ(7)==*it) || (ev.GetTrMCLZ(8)==*it);
					else if (ir==2) bSecSameQ = (ev.GetTrMCLZ(9)==*it);
					
					//for (int iMCTruth=0; iMCTruth<2; iMCTruth++)
					for (int iMCTruth=0; iMCTruth<3; iMCTruth++)
					{
						//pass event selection, should be the same as rig_r%d_q%d_GR0_rew0
						histn = Form("rig_r%d_q%d_sel_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
						histn += sRig; //2023.03.27
						GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
						
						//bool bPri=(iMCTruth==0)?bSkip:bMCCPri;
						bool bPri=false;
						if (iMCTruth==0) bPri = bSkip;
						else if (iMCTruth==1) bPri = bMCCPri;
						else if (iMCTruth==2) bPri = bMCCQ;
						if (bPri)
						{
							histn = Form("rig_r%d_q%d_sel&pri_rew0_MCV%d_MCTruth%d", ir, *it, MCVer, iMCTruth);
							histn += sRig; //2023.03.27
							GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
						}
						//non-primary events after event selection
						else
						{
							//use TrMCCluster to check the secondary information, now have only two type: stay charge 14 or not
							int itype=(bSecSameQ)?1:0;
							histn = Form("rig_r%d_q%d_sel&nopri%d_rew0_MCV%d_MCTruth%d", ir, *it, itype, MCVer, iMCTruth);
							histn += sRig; //2023.03.27
							GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch);
							
							if (itype>0)
							{
								histn = Form("ChargevsRig_r%d_q%d_sel&nopri%d_rew0_MCV%d_MCTruth%d", ir, *it, itype, MCVer, iMCTruth);
								histn += sRig; //2023.03.27
								int iSecQ=0;
								//if (ir==1) iSecQ=max(ev.mtrz[7-1],ev.mtrz[8-1]);
								//else if (ir==2) iSecQ=ev.mtrz[9-1];
								//2023.02.19
								if (ir==1) iSecQ=max(ev.GetTrMCLZ(7),ev.GetTrMCLZ(8));
								else if (ir==2) iSecQ=ev.GetTrMCLZ(9);
								GetHist2d(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, iSecQ);
							}
							
						}
					} //iMCTruth
				}
			} //*it
		}
		#endif //ACCSYS
	} //ir
	
	//----Effective acceptance
	if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta)
	{
		for (int ir=0; ir<3; ir++)
		{
			if (ir<2 && MCVer==2) continue;
			//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
			if (bRig[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.27
			{
				for (it=viQSel.begin(); it!=viQSel.end(); it++)
				{
					#ifndef EFFSEC
					if (!ev.isreal && *it!=ev.mch) continue;
					#endif //EFFSEC
					
					if (ev.Select_Charge(*it, ir, QVer))
					{
						#ifdef INNERGEOM
						bool bUnbiasedL1XY=true;
						bool bUnbiasedL1Q=true;
						if (ir==0)
						{
							bUnbiasedL1XY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
							bUnbiasedL1Q=(ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, *it, 4, QVer)) && (ev.GetUBExtQTrkCor(0, 2, QVer)<ev.GetQSelRange(1, *it, 4, QVer)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
							
							#ifdef INNERUNBIAEDL1Q
							if (!bUnbiasedL1XY || !bUnbiasedL1Q) continue;
							#endif //INNERUNBIAEDL1Q
						}
						#endif //INNERGEOM
						
						for (int irew=0; irew<4; irew++)
						{
							double dWW=1;
							if (irew==1) dWW=ww0[ir];
							else if (irew==2) dWW=ww2[ir];
							else if (irew==3) dWW=ww[ir];
							
							for (int igr=0; igr<2; igr++)
							{
								double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(ir, irig);
								histn = Form("rig_r%d_q%d_GR%d_rew%d", ir, *it, igr, irew);
								histn += sRig; //2023.03.27
								GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
							} //igr
							
							#ifdef INNERUNBIAEDL1Q
							if (ir==0 && bUnbiasedL1XY && bUnbiasedL1Q && ev.Select_TkGeom(1, iFVTight))
							{
								for (int igr=0; igr<2; igr++)
								{
									double dRigStore = (igr==0)?ev.mmom/ev.mch:ev.GetRigidity(ir, irig);
									histn = Form("rig_r%d_q%d_GR%d_rew%d_UnbiasedL1Q", ir, *it, igr, irew);
									histn += sRig; //2023.03.27
									GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
								} //igr
							}
							#endif //INNERUNBIAEDL1Q
							
							if (ir==1 && ev.mmom/ev.mch>=1000. && ev.mmom/ev.mch<=2000.)
							{
								//new weight according to new Gen-Rig
								dWW=1;
								if (irew==1) dWW=dExtww0;
								else if (irew==2) dWW=ww2[ir];
								else if (irew==3) dWW=ww2[ir]*dExtww0;
							
								for (int igr=0; igr<2; igr++)
								{
									double dRigStore = (igr==0)?dExtRig:ev.GetRigidity(ir, irig);
									histn = Form("rig_r%d_q%d_GR%d_rew%d_Gen_1_2TV", ir, *it, igr, irew);
									histn += sRig; //2023.03.27
									GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
									
									if (irew==0 && igr==0)
									{
										histn = Form("rig_r%d_q%d_GR%d_rew%d_Gen_1_2TV_old", ir, *it, igr, irew);
										histn += sRig; //2023.03.27
										GetHist(hman1, histn.c_str())->Fill(ev.mmom/ev.mch, dWW);
									}
								} //igr
							}
							
							#ifdef ACCCHECK
							if (ir!=1) continue;
							if (!bTkgeom[2]) continue; //require Full Span geometry
							for (int igr=0; igr<2; igr++)
							{
								double dRigStore = (igr==0)?dExtRig:ev.GetRigidity(ir, irig);
								histn = Form("rig_r%d_q%d_GR%d_rew%d_acccheck", ir, *it, igr, irew);
								histn += sRig; //2023.03.27
								GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
							} //irig
				
							//reconstructed rigidity distribution with generated rigidity with [1,2] TV
							if (ir==1 && ev.mmom/ev.mch>=1000. && ev.mmom/ev.mch<=2000.)
							{
								//new weight according to new Gen-Rig
								dWW=1;
								if (irew==1) dWW=dExtww0;
								else if (irew==2) dWW=ww2[ir];
								else if (irew==3) dWW=ww2[ir]*dExtww0;
							
								for (int igr=0; igr<2; igr++)
								{
									double dRigStore = (igr==0)?dExtRig:ev.GetRigidity(ir, irig);
									histn = Form("rig_r%d_q%d_GR%d_rew%d_acccheck_Gen_1_2TV", ir, *it, 0, irew);
									histn += sRig; //2023.03.27
									GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
								}
							}
							#endif //ACCCHECK
						}
						//distribution of re-generated rigidity
						if (ir==1 && MCVer==1 && ev.mmom/ev.mch>=1000. && ev.mmom/ev.mch<=2000.)
						{
							//------re-generated rigidity distribution
							histn = Form("rig_r1_q%d_reGenRig", *it);
							histn += sRig;
							GetHist(hman1, histn.c_str())->Fill(dExtRig);
						}
					}
				} //it
			}
		} //ir
	}
	
	//--
	//SelBranchingRatio(hman1, ev, QVer);
	if (irig==0) SelBranchingRatio(hman1, ev, QVer);
	
	//--
	return true;
} //SelAcc

#ifdef AMSSOFT
bool SelTOI(HistoMan &hman1, SelEvent &ev, const int QVer)
#else
//bool SelTOI(TObjArray &hman1, SelEvent &ev, const int QVer)
bool SelTOI(TObjArray &hman1, SelEvent &ev, const int QVer, const int irig) //2023.03.27
#endif //AMSSOFT
{
	//skip events with generated rigidity outside the range
	double mrig=ev.mmom/ev.mch;
	if ((MCVer==1 && (mrig<1. || mrig>2000.)) || (MCVer==2 && (mrig<2. || mrig>8000.))) return false;
	
	string sRig=sRigName[irig]; //2023.03.10
	
	const int iSkip=ev.CheckPrimary();
	
	//----Effective acceptance
	if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta)
	{
		for (int ir=0; ir<3; ir++)
		{
			if (ir<2 && MCVer==2) continue;
			//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
			if (bRig[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.27
			{
				for (it=viQSel.begin(); it!=viQSel.end(); it++)
				{
					#ifndef EFFSEC
					if (!ev.isreal && *it!=ev.mch) continue;
					#endif //EFFSEC
					
					//------acceptance for lower charge for TOI correction, i.e. A_X->Z
					for (int itoiq=iTOILQ; itoiq<=*it; itoiq++)
					{
						#ifdef REDUCESIZETOI //to speed up, do not store toiq to fur away from mch
						if (ev.mch>14 && itoiq<=8) continue;
						#endif //REDUCESIZETOI
						
						#ifdef INNERUNBIAEDL1Q
						bool bUnbiasedL1XY=true;
						bool bUnbiasedL1Q=true;
						if (ir==0)
						{
							bUnbiasedL1XY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
							bUnbiasedL1Q=(ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, itoiq, 4, QVer, true)) && (ev.GetUBExtQTrkCor(0, 2, QVer)<ev.GetQSelRange(1, itoiq, 4, QVer, true)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
							
						}
						#endif //INNERUNBIAEDL1Q
						
						for (int irew=0; irew<4; irew++)
						{
							double dWW=1;
							if (irew==1) dWW=ww0[ir];
							else if (irew==2) dWW=ww2[ir];
							else if (irew==3) dWW=ww[ir];
							
							bool bChargeCut=ev.Select_Charge(itoiq, ir, QVer,1.,true);
							#ifdef INNERUNBIAEDL1Q
							if (ir==0) bChargeCut = bChargeCut && (bUnbiasedL1XY && bUnbiasedL1Q);
							#endif //INNERUNBIAEDL1Q
							
							if (bChargeCut)
							{
								//reuqire the interaction before Tracker L1
								//for (int iTrMC=-1; iTrMC<2; iTrMC++)
								//2023.02.19
								for (int iTrMC=-1; iTrMC<3; iTrMC++)
								{
									if (itoiq==*it && iTrMC>=0) break;
									
									string sTrMC="";
									if (iTrMC>=0) sTrMC=Form("_TrMC%d", iTrMC);
									
									bool bTrMC=true;
									if (iTrMC<=0)
									{
										bTrMC = (iSkip<0 || itoiq==*it);
										//if (iTrMC==0) bTrMC = bTrMC && ev.mtrz[1-1]!=ev.mch;
										//2023.02.19
										if (iTrMC==0) bTrMC = bTrMC && ev.GetTrMCLZ(1)!=ev.mch;
									}
									//else if (iTrMC==1) bTrMC = (iSkip<0 || ev.mtrz[1-1]!=ev.mch); //iSkip<0 || mtrz0!=mch
									//2023.02.19
									else if (iTrMC==1) bTrMC = (iSkip<0 || ev.GetTrMCLZ(1)!=ev.mch); //iSkip<0 || mtrz0!=mch
									else if (iTrMC==2) bTrMC = (iSkip<0 || ev.mtrpri[1-1]==0); //iSkip<0 || mtrpri==0
									
									if (!bTrMC) continue;
									//if (iTrMC==2 && itoiq==ev.mch-1 && ir==1)
									//{
									//	cout << "itoiq=" << itoiq << " iTrMC=" << iTrMC << " ev.mch=" << ev.mch << " mtrz l1:" << ev.mtrz[1-1] << " " << ev.GetTrMCLZ(1) << ", l2: " << ev.mtrz[2-1] << " " << ev.GetTrMCLZ(2) << endl; //XXX
									//	cout << "mtrpri l1: " << ev.mtrpri[1-1] << " l2: " << ev.mtrpri[2-1] << endl;
									//}
									
									//----effective acceptance
									//for (int ievcut=0; ievcut<3; ievcut++) //ievcut=0: no additional cut, ievcut=1: ntrack==1, ievcut=2: normalized tof_chist<5 && tof_chisc<5
									for (int ievcut=0; ievcut<4; ievcut++) //ievcut=0: no additional cut, =1: ntrack==1 || no 2nd check, =2: ntrack==1, =3: no 2nd track
									{
										if (ievcut>0 && iTrMC>1) break;
										
										string sEvCut="";
										if (ievcut>0) sEvCut=Form("_EvCut%d", ievcut);
										
										//--additional event count selection
										bool bEvCut=true;
										////if (ievcut==1) bEvCut=(ev.ntrack==1);
										//if (ievcut==1) bEvCut=(ev.ntrack==1) || !ev.Select_Tk2ndRecon();
										//else if (ievcut==2) bEvCut=(ev.tof_chist_n<5 && ev.tof_chisc_n<5);
										//else bEvCut=true;
										if (ievcut==1) bEvCut=(ev.ntrack==1) || ev.Select_Tk2nd();
										else if (ievcut==2) bEvCut=(ev.ntrack==1);
										else if (ievcut==3) bEvCut=ev.Select_Tk2nd();
										else bEvCut=true;
										
										if (!bEvCut) continue;
										
										for (int igr=0; igr<2; igr++)
										{
											double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
											
											//--
											histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d%s%s", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
											
											if (igr>0) continue;
											histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d%s%s_fluxbin", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
										}
									} //ievcut
									
									//L1Inner selection+Full Span geometery
									if (iTrMC>-1) continue;
									if (ir==1 && bTkgeom[2])
									{
										for (int igr=0; igr<2; igr++)
										{
											double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
											
											//--
											histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_FSGeom", ir, *it, igr, irew, itoiq);
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
											
											if (igr>0) continue;
											histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_FSGeom_fluxbin", ir, *it, igr, irew, itoiq);
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
										} //igr
									}
								} //iTrMC
								
								//without requring the interaction before Tracker L1, i.e. no skip and TrMCCluster cut
								if (itoiq!=*it)
								{
									//----effective acceptance
									for (int igr=0; igr<2; igr++)
									{
										double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
										
										//--
										histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_noMCTruthCut", ir, *it, igr, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
										
										histn = Form("rig_r%d_q%d_GR%d_rew%d_TOIQ%d_fluxbin_noMCTruthCut", ir, *it, igr, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
									} //igr
								}
							}
							
							//checking the L1Q distribution before appling L1Q cut
							#ifdef TOIL1QDIS
							//for full span applied L9Q cut, otherwise not applied any extq cut (for L1Inner and Inner)
							//if (ev.Select_TofQ(*it, ir, QVer) && ev.Select_InnerQ(*it, QVer) && ev.Select_ExtQ(*it, (ir==2)?3:0, QVer)) 
							if (ev.Select_TofQ(itoiq, ir, QVer) && ev.Select_InnerQ(itoiq, QVer) && ev.Select_ExtQ(itoiq, (ir==2)?3:0, QVer)) //2023.02.16
							{
								for (int igr=0; igr<2; igr++)
								{
									if (*it-itoiq>3) continue; //to reduce size, store only few nearby channel //2023.02.16
									#ifdef INNERUNBIAEDL1Q
									if ((ir==0)  && !(bUnbiasedL1XY && bUnbiasedL1Q)) break;
									#endif //INNERUNBIAEDL1Q
									
									double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
									if (iSkip<0 || itoiq==*it)
									{
										histn = Form("L1QvsRig_r%d_q%d_GR%d_rew%d_TOIQ%d_type0", ir, *it, igr, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(4,QVer), dWW);
										
										histn = Form("unbiasedL1QvsRig_r%d_q%d_GR%d_rew%d_TOIQ%d_type0", ir, *it, igr, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), dWW);
									}
									
									if (itoiq==*it) continue;
									histn = Form("L1QvsRig_r%d_q%d_GR%d_rew%d_TOIQ%d_type1", ir, *it, igr, irew, itoiq);
									histn += sRig; //2023.03.27
									GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetSubDetQ(4,QVer), dWW);
									
									histn = Form("unbiasedL1QvsRig_r%d_q%d_GR%d_rew%d_TOIQ%d_type1", ir, *it, igr, irew, itoiq);
									histn += sRig; //2023.03.27
									GetHist2d(hman1, histn.c_str())->Fill(dRigStore, ev.GetUBExtQTrkCor(0, 2, QVer), dWW);
								} //igr
							}
							#endif //TOIL1QDIS
						} //irew
					} //itoiq
				} //it
			}
		} //ir
	}
	return true;
} //SelTOI

#ifdef AMSSOFT
bool SelBelowL1MC(HistoMan &hman1, SelEvent &ev, const int QVer)
#else
//bool SelBelowL1MC(TObjArray &hman1, SelEvent &ev, const int QVer)
bool SelBelowL1MC(TObjArray &hman1, SelEvent &ev, const int QVer, const int irig) //2023.03.27
#endif //AMSSOFT
{
	//skip events with generated rigidity outside the range
	double mrig=ev.mmom/ev.mch;
	if ((MCVer==1 && (mrig<1. || mrig>2000.)) || (MCVer==2 && (mrig<2. || mrig>8000.))) return false;
	
	string sRig=sRigName[irig]; //2023.03.10
	
	const int iSkip=ev.CheckPrimary();
	
	//----Effective acceptance
	if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta)
	{
		for (int ir=0; ir<3; ir++)
		{
			if (ir<2 && MCVer==2) continue;
			//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
			if (bRig[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.27
			{
				for (it=viQSel.begin(); it!=viQSel.end(); it++)
				{
					#ifndef EFFSEC
					if (!ev.isreal && *it!=ev.mch) continue;
					#endif //EFFSEC
					
					//------acceptance for lower charge for TOI correction, i.e. A_X->Z
					for (int itoiq=iTOILQ; itoiq<=*it; itoiq++)
					{
						#ifdef REDUCESIZETOI //to speed up, do not store toiq to fur away from mch
						if (ev.mch>14 && itoiq<=8) continue;
						#endif //REDUCESIZETOI
						
						#ifdef INNERUNBIAEDL1Q
						bool bUnbiasedL1XY=true;
						bool bUnbiasedL1Q=true;
						if (ir==0)
						{
							bUnbiasedL1XY = ev.GetUBExtQTrkCor(0, 0, QVer)>0 && ev.GetUBExtQTrkCor(0, 1, QVer)>0;
							bUnbiasedL1Q=(ev.GetUBExtQTrkCor(0, 2, QVer)>ev.GetQSelRange(0, itoiq, 4, QVer, true)) && (ev.GetUBExtQTrkCor(0, 2, QVer)<ev.GetQSelRange(1, itoiq, 4, QVer, true)) && ((ev.GetUBExtQStatusTrkCor(0,QVer) & 0x10013D)==0);
							
						}
						#endif //INNERUNBIAEDL1Q
						
						bool bChargeCut=ev.Select_Charge(itoiq, ir, QVer,1.,true);
						#ifdef INNERUNBIAEDL1Q
						if (ir==0) bChargeCut = bChargeCut && (bUnbiasedL1XY && bUnbiasedL1Q);
						#endif //INNERUNBIAEDL1Q
						
						if (bChargeCut)
						{
							//reuqire the interaction below Tracker L1, i.e. stay primary at L1 and interatec before L2
							//TODO check whether we need the requirement on L2. Maybe it's OK, unlike TOI, we need to separate Above L1 from below L1, here everything aside from Above L1 should all be below L1. Check the result withand without requirement on L2
							//one reason to not apply L2 cut is to store itoiq>mch to include charge confusion XXX
							//--new selection logic for TrMC
							//store the result with additional TrMCCluster requirement
							for (int iTrMC=-1; iTrMC<3; iTrMC++)
							{
								if (itoiq==*it && iTrMC>=0) break;
								
								string sTrMC="";
								if (iTrMC>=0) sTrMC=Form("_TrMC%d", iTrMC);
								
								bool bTrMC=true;
								if (iTrMC<=0)
								{
									bTrMC = ((iSkip>=0 && iSkip<7)|| itoiq==*it);
									//if (iTrMC==0) bTrMC = bTrMC && (ev.mtrz[1-1]==ev.mch && ev.mtrz[2-1]!=ev.mch); //stay mch at L1 but changed in L2
									//2023.02.19
									if (iTrMC==0) bTrMC = bTrMC && (ev.GetTrMCLZ(1)==ev.mch && ev.GetTrMCLZ(2)!=ev.mch); //stay mch at L1 but changed in L2
								}
								//else if (iTrMC==1) bTrMC = ((iSkip>=0 && iSkip<7) || (ev.mtrz[1-1]==ev.mch && ev.mtrz[2-1]!=ev.mch)); //iSkip cut || mtrz cut
								//2023.02.19
								else if (iTrMC==1) bTrMC = ((iSkip>=0 && iSkip<7) || (ev.GetTrMCLZ(1)==ev.mch && ev.GetTrMCLZ(2)!=ev.mch)); //iSkip cut || mtrz cut
								else if (iTrMC==2) bTrMC = (ev.mtrpri[1-1]==1 && ev.mtrpri[2-1]==0); //mtrpri cut
								
								if (!bTrMC) continue;
								
								//----effective acceptance
								//for (int ievcut=0; ievcut<3; ievcut++) //ievcut=0: no additional cut, ievcut=1: ntrack==1, ievcut=2: normalized tof_chist<5 && tof_chisc<5
								for (int ievcut=0; ievcut<4; ievcut++) //ievcut=0: no additional cut, =1: ntrack==1 || no 2nd check, =2: ntrack==1, =3: no 2nd track
								{
									if (ievcut>0 && iTrMC>1) break;
									
									string sEvCut="";
									if (ievcut>0) sEvCut=Form("_EvCut%d", ievcut);
									
									//--additional event count selection
									bool bEvCut=true;
									/*//if (ievcut==1) bEvCut=(ev.ntrack==1);
									if (ievcut==1) bEvCut=(ev.ntrack==1) || !ev.Select_Tk2ndRecon();
									else if (ievcut==2) bEvCut=(ev.tof_chist_n<5 && ev.tof_chisc_n<5);
									else bEvCut=true;*/
									if (ievcut==1) bEvCut=(ev.ntrack==1) || ev.Select_Tk2nd();
									else if (ievcut==2) bEvCut=(ev.ntrack==1);
									else if (ievcut==3) bEvCut=ev.Select_Tk2nd();
									else bEvCut=true;
									
									if (!bEvCut) continue;
									
									for (int irew=0; irew<4; irew++)
									{
										double dWW=1;
										if (irew==1) dWW=ww0[ir];
										else if (irew==2) dWW=ww2[ir];
										else if (irew==3) dWW=ww[ir];
										
										for (int igr=0; igr<2; igr++)
										{
											double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
											
											//--
											histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d%s%s", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
											
											histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d%s%s_fluxbin", ir, *it, igr, irew, itoiq, sTrMC.c_str(), sEvCut.c_str());
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
										} //igr
									} //irew
								} //ievcut
								
								//L1Inner selection+Full Span geometery
								if (iTrMC>-1) continue;
								if (ir==1 && bTkgeom[2])
								{
									for (int irew=0; irew<4; irew++)
									{
										double dWW=1;
										if (irew==1) dWW=ww0[ir];
										else if (irew==2) dWW=ww2[ir];
										else if (irew==3) dWW=ww[ir];
										
										for (int igr=0; igr<2; igr++)
										{
											double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
											
											//--
											histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_FSGeom", ir, *it, igr, irew, itoiq);
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
											
											histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_FSGeom_fluxbin", ir, *it, igr, irew, itoiq);
											histn += sRig; //2023.03.27
											GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
										} //igr
									} //irew
								}
							} //iTrMC
							
							//without requring the interaction before Tracker L1, i.e. no skip and TrMCCluster cut
							if (itoiq!=*it)
							{
								//----effective acceptance
								for (int irew=0; irew<4; irew++)
								{
									double dWW=1;
									if (irew==1) dWW=ww0[ir];
									else if (irew==2) dWW=ww2[ir];
									else if (irew==3) dWW=ww[ir];
									
									for (int igr=0; igr<2; igr++)
									{
										double dRigStore=(igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
										
										//--
										histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_noMCTruthCut", ir, *it, irig, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
										
										histn = Form("rig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_fluxbin_noMCTruthCut", ir, *it, irig, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist(hman1, histn.c_str())->Fill(dRigStore, dWW);
									} //igr
								} //irew
							}
						}
						
						//checking the L1Q distribution before appling L1Q cut
						#ifdef TOIL1QDIS
						//for full span applied L9Q cut, otherwise not applied any extq cut (for L1Inner and Inner)
						//if (ev.Select_TofQ(*it, ir, QVer) && ev.Select_InnerQ(*it, QVer) && ev.Select_ExtQ(*it, (ir==2)?3:0, QVer)) 
						if (ev.Select_TofQ(itoiq, ir, QVer) && ev.Select_InnerQ(itoiq, QVer) && ev.Select_ExtQ(itoiq, (ir==2)?3:0, QVer)) //2023.02.16
						{
							for (int igr=0; igr<2; igr++)
							{
								if (*it-itoiq>3) continue; //to reduce size, store only few nearby channel //2023.02.16
								#ifdef INNERUNBIAEDL1Q
								if ((ir==0)  && !(bUnbiasedL1XY && bUnbiasedL1Q)) break;
								#endif //INNERUNBIAEDL1Q
								
								double dRigStore = (igr==0)?(ev.mmom/ev.mch):ev.GetRigidity(ir, irig);
								if ((iSkip>=0 && iSkip<7)|| itoiq==*it)
								{
									for (int irew=0; irew<4; irew++)
									{
										double dWW=1;
										if (irew==1) dWW=ww0[ir];
										else if (irew==2) dWW=ww2[ir];
										else if (irew==3) dWW=ww[ir];
										
										histn = Form("L1QvsRig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_type0", ir, *it, igr, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist2d(hman1, histn.c_str())->Fill(dRigStore , ev.GetSubDetQ(4,QVer), dWW);
										
										histn = Form("unbiasedL1QvsRig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_type0", ir, *it, igr, irew, itoiq);
										histn += sRig; //2023.03.27
										GetHist2d(hman1, histn.c_str())->Fill(dRigStore , ev.GetUBExtQTrkCor(0, 2, QVer), dWW);
									} //irew
								}
								
								if (itoiq==*it) continue;
								for (int irew=0; irew<4; irew++)
								{
									double dWW=1;
									if (irew==1) dWW=ww0[ir];
									else if (irew==2) dWW=ww2[ir];
									else if (irew==3) dWW=ww[ir];
									
									histn = Form("L1QvsRig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_type1", ir, *it, igr, irew, itoiq);
									histn += sRig; //2023.03.27
									GetHist2d(hman1, histn.c_str())->Fill(dRigStore , ev.GetSubDetQ(4,QVer), dWW);
									
									histn = Form("unbiasedL1QvsRig_r%d_q%d_GR%d_rew%d_BelowL1MCQ%d_type1", ir, *it, igr, irew, itoiq);
									histn += sRig; //2023.03.27
									GetHist2d(hman1, histn.c_str())->Fill(dRigStore , ev.GetUBExtQTrkCor(0, 2, QVer), dWW);
								} //irew
							} //igr
						}
						#endif //TOIL1QDIS
					} //itoiq
				} //it
			}
		} //ir
	}
	return true;
} //SelBelowL1MC
#endif //ACCEPTANCE_H
