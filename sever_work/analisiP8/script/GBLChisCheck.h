//-----------------
//create date: 2023.02.27
//-----------------
//-----------------

#include "RigDiffCut.h"

#define GBLCHISCHECK_REWEIGHFLUXSHAPE

//--
//const int NVar=9;
const int NVar=5;
//const int NVar=4; //2023.02.27
const int iGBLVarL=0;
//const int iGBLVarL=4;

//--
#ifdef AMSSOFT
void BookHistoGBLChisCheck(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#else
void BookHistoGBLChisCheck(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int iQVer, bool dir=false)
#endif //AMSSOFT
{
	//--same as RigReso
	int iNEvenLogBin=100;
	double dEvenLogBins[iNEvenLogBin+1];
	iNbin = getNbin(ev.mch);
	pBins = getBins(ev.mch);
	dEvenLogBins[0]=pBins[0];
	dEvenLogBins[iNEvenLogBin]=pBins[iNbin];
	
	for (int ibin=1; ibin<iNEvenLogBin; ibin++)
	{
		dEvenLogBins[ibin]=dEvenLogBins[0]*pow(dEvenLogBins[iNEvenLogBin]/dEvenLogBins[0],(double)ibin/iNEvenLogBin);
	} //ibin
	
	//-
	//const int NSqrtChis = 110;
	//const double dSqrtChisL = -1, dSqrtChisU = 10;
	//const int NSqrtChis = 510; //10 bin for 1
	//const double dSqrtChisL = -1, dSqrtChisU = 50;
	const int NSqrtChis = 1505; //5 bin for 1
	const double dSqrtChisL = -1, dSqrtChisU = 300;
	//const int NSqrtChis = 520; //10 bin for 1
	//const double dSqrtChisL = -2, dSqrtChisU = 50;
	
	//const int NRigReso = 2000;
	//const double dRigResoL = -0.1, dRigResoU = 0.1;
	//const int NRigReso = 600;
	const int NRigReso = 120;
	const double dRigResoL = -0.03, dRigResoU = 0.03;
	//--
	//const int NRigDiff=2000;
	//const double dRigDiffL=-0.02, dRigDiffU=0.02;
	//2022.11.03
	//const int NRigDiff=3000;
	const int NRigDiff=600;
	const double dRigDiffL=-0.03, dRigDiffU=0.03;
	
	//2023.05.13
	//const int NBinRigDiff=4000;
	const int NBinRigDiff=400; //rebinned
	
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		#ifndef EFFSEC
		if (!ev.isreal && *it!=ev.mch) continue;
		#endif //EFFSEC
		
		for (int ir=iSpanL; ir<iSpanU; ir++)
		{
			for (int ialgo=0; ialgo<NAlgoCheck; ialgo++)
			{
				cout << "*it=" << *it << " ir=" << ir << " ialgo=" << ialgo << endl; //XXX
				outfile.mkdir(Form("GBL_Chis_Check%d/q%d/%s/Algo%d", iQVer, *it, sSpan[ir].c_str(), ialgo));
				outfile.cd(Form("GBL_Chis_Check%d/q%d/%s/Algo%d", iQVer, *it, sSpan[ir].c_str(), ialgo));
				
				string sRig="";
				string sEvCut="";
				/*//--drRig vs Gen rig
				histn = Form("drRigvsRigGen_r%d_q%d%s%s", ir, *it, sRig.c_str(), sEvCut.c_str());
				histn += "_"+sAlgo[ialgo];
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBinNoNorm, dRigResoRangeNoNorm[0], dRigResoRangeNoNorm[1]));*/
										
				//--store the Norm L1Inner ChisY distribution vs rigidity, to obtain the efficiency of L1InnerChisY cut, that should be very different for Pass7 and Pass8
				for (int igr=0; igr<2; igr++) //igr=0: Generated rig, =1: reconstructed rig
				{
					if (ev.isreal && igr==0) continue;
					string sVar;
					if (ir==1) sVar = "L1IChisY";
					else if (ir==2) sVar = "FSChisY";
					histn = Form("sqrt%svsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
					histn += "_"+sAlgo[ialgo];
					if (igr==0) hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, NSqrtChis, dSqrtChisL, dSqrtChisU));
					else hman1.Add(new TH2F(histn.c_str(), histn.c_str(), getNbin(*it), getBins(*it), NSqrtChis, dSqrtChisL, dSqrtChisU));
					GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
					GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("#sqrt{%s}", sVar.c_str()));
				} //igr
				
				//--store the no. events, use to normalize between different algorithm //2022.10.16
				//2022.10.31: should not apply L1ChisY as these are for Inner
				for (int igr=0; igr<2; igr++)
				{
					histn = Form("rig_gr%d_r%d_q%d", igr, ir, *it);
					histn += "_"+sAlgo[ialgo];
					if (igr==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins));
					else hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				} //igr
				
				//--
				for (int ivar=iGBLVarL; ivar<NVar; ivar++) //ivar=0: L1Chis (X, Y), =1: ExtChis (X, Y, XY), =2: L1Inner chis (X, Y), =3: Inner chis (X, Y), =4: 1/R_L1I-1/R_Inner, =5: L1ExtChis (X, Y, XY), =6: L1ScatAngle (X, Y), =7: L1ScatAngleRatio (X, Y), =8: CooResidual (X, Y)
				{
					if (ialgo==0 && ivar==1) continue;
					#ifndef READEXTCHIS
					if (ivar==1) continue;
					if (ivar>=5) continue;
					#endif //READEXTCHIS
					
					outfile.mkdir(Form("GBL_Chis_Check%d/q%d/%s/Algo%d/Var%d", iQVer, *it, sSpan[ir].c_str(), ialgo, ivar));
					outfile.cd(Form("GBL_Chis_Check%d/q%d/%s/Algo%d/Var%d", iQVer, *it, sSpan[ir].c_str(), ialgo, ivar));
					
					int iXYL=0, iXYU=2;
					if (ivar==1 || ivar==5) iXYU=3;
					if (ivar==4) iXYL=-1, iXYU=iXYL+1;
					for (int ixy=iXYL; ixy<iXYU; ixy++)
					{
						string sXYVar = (ixy>=0)?sXY[ixy]:"";
						
						int iInterL=0, iInterU=2;
						if (ialgo==0) iInterU=1;
						if (ivar<=4) iInterL=-1, iInterU=iInterL+1;
						for (int inter=iInterL; inter<iInterU; inter++)
						{
							string sInter = (inter>=0)?Form("_inter%d", inter):"";
							
							int iVCooL=0, iVCooU=5;
							if (ivar<=4) iVCooL=-1, iVCooU=iVCooL+1;
							for (int ivcoo=iVCooL; ivcoo<iVCooU; ivcoo++)
							{
								string sVCoo = (ivcoo>=0)?Form("_VCoo%d", ivcoo):"";
								
								int irigcalL=0, irigcalU=2;
								if (ivar<=4) irigcalL=-1, irigcalU=irigcalL+1;
								for (int irigcal=irigcalL; irigcal<irigcalU; irigcal++)
								{
									string sRigCal = (irigcal>=0)?Form("_RigCal%d", irigcal):"";
									string sVar = sVar0[ivar]+sXYVar+sInter+sVCoo+sRigCal;
									
									//for (int icut=0; icut<3; icut++) //icut=0: no L1ChisY cut, =1: sqrt(L1ChisY)<2, =2: sqrt(L1ChisY)<sqrt(10)
									//for (int icut=0; icut<2; icut++) //icut=0: no L1ChisY cut, =1: sqrt(L1ChisY)<sqrt(10)
									for (int icut=0; icut<3; icut++) //icut=0: no L1ChisY cut, =1: sqrt(L1ChisY)<sqrt(10) for L1Inenr and sqrt(L1L9ChisY)<10 for Full Span, =2: sqrt(L1ChisX)<sqrt(10) for L1Inner //2023.04.05
									{
										//--
										if (ev.isreal) continue;
										
										//--drRig vs sqrt(L1ChisY) in rigidity ~100 GV
										//for (int irigbin=0; irigbin<10; irigbin++)
										//for (int irigbin=0; irigbin<1; irigbin++)
										for (int irigbin=0; irigbin<NRigBin; irigbin++) //2022.11,03
										{
											//double dGBLCheckRigL = 95.+1.*irigbin;
											//double dGBLCheckRigU = 95.+1.*(irigbin+1);
											//double dGBLCheckRigL = 99.+2.*irigbin;
											//double dGBLCheckRigU = 99.+2.*(irigbin+1);
											//2022.11.03
											double dGBLCheckRigL = dGBLCheckRigL0[irigbin];
											double dGBLCheckRigU = dGBLCheckRigU0[irigbin];
											
											histn = Form("drRigvs%s_r%d_q%d%s%s", sVar.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
											histn += "_"+sAlgo[ialgo];
											histn += Form("_cut%d", icut);
											histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
											//if (sVar0[ivar]=="rRL1I_rRI") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigDiff, dRigDiffL, dRigDiffU, NRigReso, dRigResoL, dRigResoU));
											//else if (sVar0[ivar]=="L1ExtChis") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), 1010, -1, 100, NRigReso, dRigResoL, dRigResoU));
											//else if (sVar0[ivar]=="L1ScatAngle") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), 1000, -0.1, 0.1, NRigReso, dRigResoL, dRigResoU));
											//else if (sVar0[ivar]=="L1ScatAngleRatio") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), 1000, -50, 50, NRigReso, dRigResoL, dRigResoU));
											//else if (sVar0[ivar]=="CooResidual") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), 1000, -0.1, 0.1, NRigReso, dRigResoL, dRigResoU));
											//else if (sVar0[ivar]=="CooResidualToL1ScatAngleRatio") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), 1000, -0.1, 0.1, NRigReso, dRigResoL, dRigResoU));
											////all the other chis
											//else hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NSqrtChis, dSqrtChisL, dSqrtChisU, NRigReso, dRigResoL, dRigResoU));
											if (sVar0[ivar]=="rRL1I_rRI") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigDiff, dRigDiffL, dRigDiffU, NRigReso, dRigResoL, dRigResoU));
											else if (sVar0[ivar]=="L1ExtChis") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1010, -1, 100, NRigReso, dRigResoL, dRigResoU));
											else if (sVar0[ivar]=="L1ScatAngle") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, -0.1, 0.1, NRigReso, dRigResoL, dRigResoU));
											else if (sVar0[ivar]=="L1ScatAngleRatio") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, -50, 50, NRigReso, dRigResoL, dRigResoU));
											else if (sVar0[ivar]=="CooResidual") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, -0.1, 0.1, NRigReso, dRigResoL, dRigResoU));
											else if (sVar0[ivar]=="CooResidualToL1ScatAngleRatio") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), 1000, -0.1, 0.1, NRigReso, dRigResoL, dRigResoU));
											//all the other chis
											else hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NSqrtChis, dSqrtChisL, dSqrtChisU, NRigReso, dRigResoL, dRigResoU));
											if (sVar0[ivar]=="rRL1I_rRI" || ivar>5) GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s", sVar.c_str()));
											else GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("#sqrt{%s}", sVar.c_str()));
											GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/R_{Rec}-1/R_{Gen} [GV^{-1}]"));
											
											if (ivar==4)
											{
												//--store 1/R_FS - 1/R_Inn for FS //2022.11.16
												if (ir==2)
												{
													histn = Form("drRigvs%s_r%d_q%d%s%s", sVarRDiffFS.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
													histn += "_"+sAlgo[ialgo];
													histn += Form("_cut%d", icut);
													histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
													//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigDiff, dRigDiffL, dRigDiffU, NRigReso, dRigResoL, dRigResoU));
													hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigDiff, dRigDiffL, dRigDiffU, NRigReso, dRigResoL, dRigResoU));
													GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("%s", sVarRDiffFS.c_str()));
													GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/R_{Rec}-1/R_{Gen} [GV^{-1}]"));
												}
												
												//--store the normalized rig diff //2022.12.19
												//for (int igrSig=0; igrSig<2; igrSig++) //igrSig=0: parametrize as function of GenRig, =1: ... RecRig
												for (int igrSig=0; igrSig<1; igrSig++) //igrSig=0: parametrize as function of GenRig, =1: ... RecRig
												{
													//for (int igr=0; igr<2; igr++) //igr=0: use GenRig to get sigma, igr=1; use RecRig ...
													for (int igr=1; igr<2; igr++) //igr=0: use GenRig to get sigma, igr=1; use RecRig ...
													{
														for (int ifit=iSigmaFitF; ifit<NSigmaFit; ifit++) //ifit=0: spline fit, =1: simple fit
														{
															for (int iRigSigQ=0; iRigSigQ<2; iRigSigQ++)
															{
																histn = Form("Norm_grSig%d_gr%d_fit%d_drRigvs%s_r%d_q%d%s%s", igrSig, igr, ifit, sVar.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
																histn += "_"+sAlgo[ialgo];
																histn += Form("_cut%d", icut);
																histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
																histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
																hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NBinRigDiff, -20, 20, NRigReso, dRigResoL, dRigResoU));
																GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("(%s)/#sigma", sVar.c_str()));
																
																//--store normalized 1/R_FS - 1/R_Inn for FS //2023.02.27
																if (ir==2)
																{
																	histn = Form("Norm_grSig%d_gr%d_fit%d_drRigvs%s_r%d_q%d%s%s", igrSig, igr, ifit, sVarRDiffFS.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
																	histn += "_"+sAlgo[ialgo];
																	histn += Form("_cut%d", icut);
																	histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
																	histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
																	hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NBinRigDiff, -20, 20, NRigReso, dRigResoL, dRigResoU));
																	GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("(%s)/#sigma", sVarRDiffFS.c_str()));
																}
															} //iRigSigQ
														} //ifit
													} //igr
												} //igrSig
											}
											
											//--For full span, store more variables
											if (ir!=2) continue;
											
											//--store L1L9Chis //2023.02.27
											if (ivar==0)
											{
												string sVarL1L9 = sVarL1L9Chis+sXYVar+sInter+sVCoo+sRigCal;
												histn = Form("drRigvs%s_r%d_q%d%s%s", sVarL1L9.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
												//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NSqrtChis, dSqrtChisL, dSqrtChisU, NRigReso, dRigResoL, dRigResoU));
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NSqrtChis, dSqrtChisL, dSqrtChisU, NRigReso, dRigResoL, dRigResoU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("#sqrt{%s}", sVarL1L9.c_str()));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/R_{Rec}-1/R_{Gen} [GV^{-1}]"));
											}
											
											//--store FSChis //2022.11.17
											if (ivar==2)
											{
												string sVarFS = sVarFSChis+sXYVar+sInter+sVCoo+sRigCal;
												histn = Form("drRigvs%s_r%d_q%d%s%s", sVarFS.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
												//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NSqrtChis, dSqrtChisL, dSqrtChisU, NRigReso, dRigResoL, dRigResoU));
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NSqrtChis, dSqrtChisL, dSqrtChisU, NRigReso, dRigResoL, dRigResoU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("#sqrt{%s}", sVarFS.c_str()));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("1/R_{Rec}-1/R_{Gen} [GV^{-1}]"));
											}
										} //irigbin
										
										//--var vs Rig
										for (int igr=0; igr<2; igr++) //igr=0: Generated rig, =1: reconstructed rig
										{
											if (ev.isreal && igr==0) continue;
											const int NRigBin = (igr==0)?iNEvenLogBin:getNbin(*it);
											double *pRigBin = (igr==0)?dEvenLogBins:getBins(*it);
											histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
											histn += "_"+sAlgo[ialgo];
											histn += Form("_cut%d", icut);
											if (sVar0[ivar]=="rRL1I_rRI") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NRigDiff, dRigDiffL, dRigDiffU));
											else if (sVar0[ivar]=="L1ExtChis") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1010, -1, 100));
											else if (sVar0[ivar]=="L1ScatAngle") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.1, 0.1));
											else if (sVar0[ivar]=="L1ScatAngleRatio") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -50, 50));
											else if (sVar0[ivar]=="CooResidual") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.1, 0.1));
											else if (sVar0[ivar]=="CooResidualToL1ScatAngleRatio") hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.1, 0.1));
											//all the other chis
											else hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NSqrtChis, dSqrtChisL, dSqrtChisU));
											//if (sVar0[ivar]=="rRL1I_rRI") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NRigDiff, dRigDiffL, dRigDiffU));
											//else if (sVar0[ivar]=="L1ExtChis") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1010, -1, 100));
											//else if (sVar0[ivar]=="L1ScatAngle") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.1, 0.1));
											//else if (sVar0[ivar]=="L1ScatAngleRatio") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -50, 50));
											//else if (sVar0[ivar]=="CooResidual") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.1, 0.1));
											//else if (sVar0[ivar]=="CooResidualToL1ScatAngleRatio") hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.1, 0.1));
											////all the other chis
											//else hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NSqrtChis, dSqrtChisL, dSqrtChisU));
											GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
											if (sVar0[ivar]=="rRL1I_rRI" || ivar>5) GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s", sVar.c_str()));
											else GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("#sqrt{%s}", sVar.c_str()));
											
											//--Normalized 1/R_L1I-1/R_Inn
											//for (int irigdiff=-1; irigdiff<2; irigdiff++) //irigdiff=-1: (only for igr=0) using generated rig, =0: get sigma using L1Inner rig, =1: get sigma using Inner rig
											for (int irigdiff=1; irigdiff<2; irigdiff++) //irigdiff=-1: (only for igr=0) using generated rig, =0: get sigma using L1Inner rig, =1: get sigma using Inner rig
											{
												if (ivar!=4) continue;
												if (igr>0 && irigdiff==-1) continue;
												
												//for (int igrSig=0; igrSig<2; igrSig++) //2022.12.19
												for (int igrSig=0; igrSig<1; igrSig++) //2023.02.27
												{
													for (int ifit=iSigmaFitF; ifit<NSigmaFit; ifit++) //ifit=0: spline fit, =1: simple fit
													{
														for (int iRigSigQ=0; iRigSigQ<2; iRigSigQ++)
														{
															//histn = Form("Norm%s_RigDiff%dvsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn = Form("Norm%s_grSig%d_RigDiff%dvsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igrSig, irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn += "_"+sAlgo[ialgo];
															histn += Form("_cut%d", icut);
															if (ifit==1) histn += "_simpleFit";
															if (ifit==2) histn += "_scaled";
															histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
															//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 4000, -20, 20));
															hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NBinRigDiff, -20, 20));
															
															/*//--store the RigDiff difference using GenRig and RecRig //2023.01.17
															histn = Form("Norm%s_grSig%d_RigDiff%d_diffvsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igrSig, irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn += "_"+sAlgo[ialgo];
															histn += Form("_cut%d", icut);
															if (ifit==1) histn += "_simpleFit";
															if (ifit==2) histn += "_scaled";
															histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
															hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, 1000, -0.5, 0.5));*/
															
															//--normalized 1/R_FS-1/R_Inn //2023.02.27
															if (ir!=2) continue;
															histn = Form("Norm%s_RigDiff%dvsRig_gr%d_r%d_q%d%s%s", sVarRDiffFS.c_str(), irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn += "_"+sAlgo[ialgo];
															histn += Form("_cut%d", icut);
															if (ifit==1) histn += "_simpleFit";
															if (ifit==2) histn += "_scaled";
															histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
															hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NBinRigDiff, -20, 20));
														} //iRigSigQ
													} //ifit
												} //igrSig
											} //irigdiff
											
											//--for vs reconstructed rigidity, store the result with RecRig<0 //2023.01.11
											if (igr==1)
											{
												//--RigDiff vs fabs(RecRig)
												histn = Form("%svsAbsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NRigDiff, dRigDiffL, dRigDiffU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s", sVar.c_str()));
												
												//--RigDiff vs RecRig for RecRig<0
												histn = Form("%svsRig_lt0_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NRigDiff, dRigDiffL, dRigDiffU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s", sVar.c_str()));
											}
											
											//--Full span
											if (ir!=2) continue;
											
											//--store L1L9Chis //2023.02.27
											if (ivar==0)
											{
												string sVarL1L9= sVarL1L9Chis+sXYVar+sInter+sVCoo+sRigCal;
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarL1L9.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NSqrtChis, dSqrtChisL, dSqrtChisU));
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NSqrtChis, dSqrtChisL, dSqrtChisU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("#sqrt{%s}", sVarL1L9.c_str()));
											}
											
											//--store FSChis //2022.11.17
											if (ivar==2)
											{
												string sVarFS = sVarFSChis+sXYVar+sInter+sVCoo+sRigCal;
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarFS.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NSqrtChis, dSqrtChisL, dSqrtChisU));
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NSqrtChis, dSqrtChisL, dSqrtChisU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("#sqrt{%s}", sVarFS.c_str()));
											}
											
											//--store 1/R_FS - 1/R_Inn for Full Span as well
											if (ivar==4)
											{
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarRDiffFS.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												//hman1.Add(new TH2S(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NRigDiff, dRigDiffL, dRigDiffU));
												hman1.Add(new TH2F(histn.c_str(), histn.c_str(), NRigBin, pRigBin, NRigDiff, dRigDiffL, dRigDiffU));
												GetHist2d(hman1, histn.c_str())->GetXaxis()->SetTitle(Form("R_{%s} [GV]", (igr==0)?"Gen":"Rec"));
												GetHist2d(hman1, histn.c_str())->GetYaxis()->SetTitle(Form("%s", sVarRDiffFS.c_str()));
											}
										} //igr
									} //icut
								} //irigcal
							} //ivcoo
						} //inter
					} //ixy
				} //ivar
			} //ialgo
		} //ir
	} //it
} //BookHistoGBLChisCheck

#ifdef AMSSOFT
void SelGBLChisCheck(HistoMan &hman1, SelEvent &ev, const int ir, const int iQVer)
#else
void SelGBLChisCheck(TObjArray &hman1, SelEvent &ev, const int ir, const int iQVer)
#endif //AMSSOFT
{
	//--
	double dGenRig=-1, dDeltaRRig=-10;
	if (!ev.isreal) dGenRig = ev.mmom/ev.mch;
	for (int ialgo=0; ialgo<NAlgoCheck; ialgo++)
	{
		string sRig="";
		string sEvCut="";
		
		double dRig = ev.GetRigidity(ir, ialgo);
		if (!ev.isreal) dDeltaRRig = 1./dRig - 1./dGenRig;
		
		//--preselection, fiducial volume and chis: Inner chis + L1I or FS chis, wo L1Chis
		//bool bTrackSel = bTkgeom[ir] && ev.Select_TrTrack(ir,101,ialgo);
		bool bTrackSel = bTkgeom[ir] && ev.Select_TrTrack(ir,1,ialgo); //only apply Inner chis, will store the Inner chis -> L1Inner or FS chis eff //2022.10.03
		if (!(bTrackSel)) continue;
		
		//--use new charge selection function
		for (it=viQSel.begin(); it!=viQSel.end(); it++)
		{
			#ifndef EFFSEC
			if (!ev.isreal && *it!=ev.mch) continue;
			#endif //EFFSEC
			
			if (ev.Select_Charge(*it, ir, iQVer))
			{
				//--store the Norm L1Inner ChisY distribution vs rigidity, to obtain the efficiency of L1InnerChisY cut, that should be very different for Pass7 and Pass8
				for (int igr=0; igr<2; igr++) //igr=0: Generated rig, =1: reconstructed rig
				{
					if (ev.isreal && igr==0) continue;
					
					string sVar;
					if (ir==1) sVar = "L1IChisY";
					else if (ir==2) sVar = "FSChisY";
					histn = Form("sqrt%svsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
					histn += "_"+sAlgo[ialgo];
					GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, sqrt(ev.GetChis(ir, 1, ialgo))); //noreweight
				} //igr
				////--drRig vs Gen rig
				//histn = Form("drRigvsRigGen_r%d_q%d%s%s", ir, *it, sRig.c_str(), sEvCut.c_str());
				//histn += "_"+sAlgo[ialgo];
				////GetHist2d(hman1, histn.c_str())->Fill(dGenRig, dDeltaRRig, ww[ir]);
				//GetHist2d(hman1, histn.c_str())->Fill(dGenRig, dDeltaRRig); //noreweight
				
				//--store the no. events, use to normalize between different algorithm //2022.10.16
				//2022.10.31: should not apply L1ChisY as these are for Inner
				for (int igr=0; igr<2; igr++)
				{
					if (!ev.Select_TrTrack(ir,100,ialgo)) break;
					histn = Form("rig_gr%d_r%d_q%d", igr, ir, *it);
					histn += "_"+sAlgo[ialgo];
					GetHist(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig);
				} //igr
				
				//--
				for (int ivar=iGBLVarL; ivar<NVar; ivar++) //ivar=0: L1Chis (X, Y), =1: ExtChis (X, Y, XY), =2: L1Inner chis (X, Y), =3: Inner chis (X, Y), =4: 1/R_L1I-1/R_Inner, =5: L1ExtChis (X, Y, XY), =6: L1ScatAngle (X, Y), =7: L1ScatAngleRatio (X, Y), =8: CooResidual (X, Y)
				//for (int ivar=0; ivar<9; ivar++) //ivar=0: L1Chis (X, Y), =1: ExtChis (X, Y, XY), =2: L1Inner chis (X, Y), =3: Inner chis (X, Y), =4: 1/R_L1I-1/R_Inner, =5: L1ExtChis (X, Y, XY), =6: L1ScatAngle (X, Y), =7: L1ScatAngleRatio (X, Y), =8: CooResidual (X, Y)
				//for (int ivar=0; ivar<10; ivar++) //ivar=0: L1Chis (X, Y), =1: ExtChis (X, Y, XY), =2: L1Inner chis (X, Y), =3: Inner chis (X, Y), =4: 1/R_L1I-1/R_Inner, =5: L1ExtChis (X, Y, XY), =6: L1ScatAngle (X, Y), =7: L1ScatAngleRatio (X, Y), =8: CooResidual (X, Y), =9: CooResidual/L1ScatAngleRatio (X, Y)
				{
					//--apply external chis cut //2022.10.03
					if (!ev.Select_TrTrack(ir,100,ialgo)) break;
					
					//2022.10.16
					if (ialgo==0 && ivar==1) continue;
					#ifndef READEXTCHIS
					if (ivar==1) continue;
					if (ivar>=5) continue;
					#endif //READEXTCHIS
					int iXYL=0, iXYU=2;
					if (ivar==1 || ivar==5) iXYU=3;
					if (ivar==4) iXYL=-1, iXYU=iXYL+1;
					for (int ixy=iXYL; ixy<iXYU; ixy++)
					{
						string sXYVar = (ixy>=0)?sXY[ixy]:"";
						
						int iInterL=0, iInterU=2;
						if (ialgo==0) iInterU=1;
						if (ivar<=4) iInterL=-1, iInterU=iInterL+1;
						for (int inter=iInterL; inter<iInterU; inter++)
						{
							string sInter = (inter>=0)?Form("_inter%d", inter):"";
							
							int iVCooL=0, iVCooU=5;
							if (ivar<=4) iVCooL=-1, iVCooU=iVCooL+1;
							for (int ivcoo=iVCooL; ivcoo<iVCooU; ivcoo++)
							{
								string sVCoo = (ivcoo>=0)?Form("_VCoo%d", ivcoo):"";
								int irigcalL=0, irigcalU=2;
								if (ivar<=4) irigcalL=-1, irigcalU=irigcalL+1;
								for (int irigcal=irigcalL; irigcal<irigcalU; irigcal++)
								{
									string sRigCal = (irigcal>=0)?Form("_RigCal%d", irigcal):"";
									string sVar = sVar0[ivar]+sXYVar+sInter+sVCoo+sRigCal;
									
									double dVar=0.;
									if (ivar==0) dVar = ev.GetExtChisDiffRig(0, ixy, ialgo);
									else if (ivar==1) dVar = ev.GetExtChisGBL(0, ir, ixy);
									else if (ivar==2) dVar = ev.GetChis(1, ixy, ialgo);
									else if (ivar==3) dVar = ev.GetChis(0, ixy, ialgo);
									else if (ivar==4) dVar = 1./ev.GetRigidity(1, ialgo) - 1./ev.GetRigidity(0, ialgo);
									else if (ivar==5) dVar = ev.GetExtChis(0, ialgo, RIGPGC, ixy, inter, ivcoo, irigcal);
									else if (ivar==6) dVar = ev.GetExtScatAngle(0, ialgo, RIGPGC, ixy, inter, ivcoo, irigcal);
									else if (ivar==7) dVar = ev.GetExtScatAngleRatio(0, ialgo, RIGPGC, ixy, inter, ivcoo, irigcal);
									else if (ivar==8) dVar = ev.GetExtHitRes(0, ialgo, RIGPGC, ixy, inter, ivcoo, irigcal);
									else if (ivar==9) dVar = ev.GetExtHitRes(0, ialgo, RIGPGC, ixy, inter, ivcoo, irigcal)/ev.GetExtScatAngleRatio(0, ialgo, RIGPGC, ixy, inter, ivcoo, irigcal);
									if (sVar0[ivar]!="rRL1I_rRI" && ivar<=5) dVar = sqrt(dVar);
									/*if (ivar==5 && ixy==2 && ialgo==1)
									{
										cout << "ievent=" << ievent << " ivar=" << ivar << " ialgo=" << ialgo << " ixy=" << ixy << " inter=" << inter << " ivcoo=" << ivcoo << " dVar=" << pow(dVar, 2) << " " << ev.tk_L1ExtChis[ialgo][RIGPGC][0][inter][ivcoo] << " " << ev.tk_L1ExtChis[ialgo][RIGPGC][1][inter][ivcoo] << endl; //XXX
									}*/
									
									//for (int icut=0; icut<3; icut++)
									//for (int icut=0; icut<2; icut++) //2022.10.08
									for (int icut=0; icut<3; icut++) //icut=0: no L1ChisY cut, =1: sqrt(L1ChisY)<sqrt(10) for L1Inenr and sqrt(L1L9ChisY)<10 for Full Span, =2: sqrt(L1ChisX)<sqrt(10) for L1Inner //2023.04.05
									{
										//if (icut==1 && sqrt(ev.GetL1ChisY(ialgo, 2))>2) continue;
										//else if (icut==2 && ev.GetL1ChisY(ialgo, 2)>10.) continue;
										//if (icut==1 && ev.GetL1ChisY(ialgo, 2)>10.) continue;
										bool bCut = true;
										if (icut>=1)
										{
											if (ir==1) bCut = bCut && ev.GetL1ChisY(ialgo, 2)<10.;			//L1ChisY for L1Inner
											else if (ir==2) bCut = bCut && ev.GetL1L9Chis(1, ialgo)<10.;	//L1L9ChisY for FullSpan
										}
										if (icut>=2) bCut = bCut && ev.GetExtChisDiffRig(0, 0, ialgo)<10.;	//L1ChisX
										
										if (!bCut) continue;
										
										//--drRig
										if (ev.isreal) continue;
										
										//--drRig vs sqrt(L1ChisY) in rigidity ~100 GV
										//for (int irigbin=0; irigbin<10; irigbin++)
										//for (int irigbin=0; irigbin<1; irigbin++)
										for (int irigbin=0; irigbin<NRigBin; irigbin++)
										{
											//double dGBLCheckRigL = 95.+1.*irigbin;
											//double dGBLCheckRigU = 95.+1.*(irigbin+1);
											//double dGBLCheckRigL = 99.+2.*irigbin;
											//double dGBLCheckRigU = 99.+2.*(irigbin+1);
											//2022.11.03
											double dGBLCheckRigL = dGBLCheckRigL0[irigbin];
											double dGBLCheckRigU = dGBLCheckRigU0[irigbin];
											//double rigidity = (igr==0)?dGenRig:dRig;
											double rigidity = dGenRig;
											//if (irigbin==0 && ivar==4 && ir==1 && igr==1 && (dGenRig>=dGBLCheckRigL && dGenRig<=dGBLCheckRigU)) cout << "ievent=" << ievent << " igr=" << igr << " ir=" << ir << " rigidity=" << rigidity << " genrig=" << dGenRig << endl; //XXX
											if (rigidity<dGBLCheckRigL || rigidity>dGBLCheckRigU) continue;
											histn = Form("drRigvs%s_r%d_q%d%s%s", sVar.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
											histn += "_"+sAlgo[ialgo];
											histn += Form("_cut%d", icut);
											histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
											#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
											GetHist2d(hman1, histn.c_str())->Fill(dVar, dDeltaRRig, ww0[ir]); //only flux shape
											#else
											GetHist2d(hman1, histn.c_str())->Fill(dVar, dDeltaRRig); //noreweight
											#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											
											if (ivar==4)
											{
												//--store 1/R_FS - 1/R_Inn for Full Span as well
												if (ir==2)
												{
													histn = Form("drRigvs%s_r%d_q%d%s%s", sVarRDiffFS.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
													histn += "_"+sAlgo[ialgo];
													histn += Form("_cut%d", icut);
													histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
													#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
													GetHist2d(hman1, histn.c_str())->Fill(1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo), dDeltaRRig, ww0[ir]); //only flux shape
													#else
													GetHist2d(hman1, histn.c_str())->Fill(1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo), dDeltaRRig); //noreweight
													#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
												}
													
												//--store the normalized rig diff //2022.12.19
												//for (int igrSig=0; igrSig<2; igrSig++) //igrSig=0: parametrize as function of GenRig, =1: ... RecRig
												for (int igrSig=0; igrSig<1; igrSig++) //igrSig=0: parametrize as function of GenRig, =1: ... RecRig
												{
													//for (int igrGet=0; igrGet<2; igrGet++) //igrGet=0: use GenRig to get sigma, igrGet=1; use RecRig ...
													for (int igrGet=1; igrGet<2; igrGet++) //igrGet=0: use GenRig to get sigma, igrGet=1; use RecRig ...
													{
														double dRigNorm = (igrGet==0)?dGenRig:ev.GetRigidity(ir, ialgo);
														//2023.01.11
														if (igrGet==1 && dRigNorm<0) dRigNorm = fabs(dRigNorm);
														/*if (dRigNorm<0) //XXX //2023.01.11
														{
															if (igrGet==1)
															{
																cout << "ievent=" << ievent << endl;
																cout << "GenRig=" << dGenRig << " RecRig=" << ev.GetRigidity(ir, ialgo) << " dNorm=" << dNorm << " drRig=" << dDeltaRRig << endl;
															}
														}*/
														if (dRigNorm<0) continue;
														
														for (int ifit=iSigmaFitF; ifit<NSigmaFit; ifit++) //ifit=0: spline fit, =1: simple fit, =2: simple fit with rescale
														{
															for (int iRigSigQ=0; iRigSigQ<2; iRigSigQ++)
															{
																//double dNorm =f1RigDiffSig[ialgo][igrSig][ifit]->Eval(dRigNorm); //2022.12.19
																//double dNorm = f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(dRigNorm); //2023.01.12
																//double dNorm = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0)->Eval(dRigNorm); //2023.02.27
																//double dNorm = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0, MCQ)->Eval(dRigNorm); //2023.05.11
																double dNorm = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0, iRigCutQ[iRigSigQ])->Eval(dRigNorm); //2023.05.12
																if (dNorm<=0) continue;
																
																histn = Form("Norm_grSig%d_gr%d_fit%d_drRigvs%s_r%d_q%d%s%s", igrSig, igrGet, ifit, sVar.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
																histn += "_"+sAlgo[ialgo];
																histn += Form("_cut%d", icut);
																histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
																histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
																#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
																GetHist2d(hman1, histn.c_str())->Fill(dVar/dNorm, dDeltaRRig, ww0[ir]); //only flux shape
																#else
																GetHist2d(hman1, histn.c_str())->Fill(dVar/dNorm, dDeltaRRig); //noreweight
																#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
																
																//--normalized 1/R_FS-1/R_Inn //2023.02.27
																if (ir!=2) continue;
																//double dNormFS = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 1)->Eval(dRigNorm);
																double dNormFS = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 1, MCQ)->Eval(dRigNorm); //2023.05.11
																if (dNormFS<=0) continue;
																double dVarFS = 1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo);
																histn = Form("Norm_grSig%d_gr%d_fit%d_drRigvs%s_r%d_q%d%s%s", igrSig, igrGet, ifit, sVarRDiffFS.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
																histn += "_"+sAlgo[ialgo];
																histn += Form("_cut%d", icut);
																histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
																histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
																#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
																GetHist2d(hman1, histn.c_str())->Fill(dVarFS/dNormFS, dDeltaRRig, ww0[ir]); //only flux shape
																#else
																GetHist2d(hman1, histn.c_str())->Fill(dVarFS/dNormFS, dDeltaRRig); //noreweight
																#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
															} //iRigSigQ
														} //ifit
													} //igrGet
												} //igrSig
											}
											
											//--Full Span //2022.11.17
											if (ir!=2) continue;
											
											//--store L1L9Chis //2023.02.27
											if (ivar==0)
											{
												string sVarL1L9= sVarL1L9Chis+sXYVar+sInter+sVCoo+sRigCal;
												double dVarL1L9 = sqrt(ev.GetL1L9Chis(ixy, ialgo));
												histn = Form("drRigvs%s_r%d_q%d%s%s", sVarL1L9.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill(dVarL1L9, dDeltaRRig, ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill(dVarL1L9, dDeltaRRig); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											}
											
											//--store FSChis //2022.11.17
											if (ivar==2)
											{
												string sVarFS = sVarFSChis+sXYVar+sInter+sVCoo+sRigCal;
												double dVarFS = sqrt(ev.GetChis(2, ixy, ialgo));
												histn = Form("drRigvs%s_r%d_q%d%s%s", sVarFS.c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												histn += Form("_%.1f_%.1f", dGBLCheckRigL, dGBLCheckRigU);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill(dVarFS, dDeltaRRig, ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill(dVarFS, dDeltaRRig); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											}
										} //irigbin
										
										//--variable vs Rig
										for (int igr=0; igr<2; igr++) //igr=0: Generated rig, =1: reconstructed rig
										{
											if (ev.isreal && igr==0) continue;
											histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
											histn += "_"+sAlgo[ialgo];
											histn += Form("_cut%d", icut);
											//GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVar); //noreweight
											GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVar, ww0[ir]); //only flux shape
											
											//--store 1/R_FS - 1/R_Inn for Full Span as well
											if (ivar==4 && ir==2)
											{
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarRDiffFS.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, 1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo), ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, 1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo)); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											}
											
											//--Normalized 1/R_L1I-1/R_Inn
											//for (int irigdiff=-1; irigdiff<2; irigdiff++) //irigdiff=-1: (only for igr=0) using generated rig, =0: get sigma using L1Inner rig, =1: get sigma using Inner rig
											for (int irigdiff=1; irigdiff<2; irigdiff++) //irigdiff=-1: (only for igr=0) using generated rig, =0: get sigma using L1Inner rig, =1: get sigma using Inner rig
											{
												if (ivar!=4) continue;
												if (igr>0 && irigdiff==-1) continue;
												double dRigNorm = 0.;
												if (irigdiff==-1) dRigNorm = dGenRig;
												else if (irigdiff>=0) dRigNorm = ev.GetRigidity((irigdiff==0)?0:ir, ialgo);
												if (dRigNorm<0) continue;
												//for (int igrSig=0; igrSig<2; igrSig++) //2022.12.19
												for (int igrSig=0; igrSig<1; igrSig++) //2023.02.27
												{
													for (int ifit=iSigmaFitF; ifit<NSigmaFit; ifit++) //ifit=0: spline fit, =1: simple fit
													{
														for (int iRigSigQ=0; iRigSigQ<2; iRigSigQ++)
														{
															//double dNorm =f1RigDiffSig[ialgo]->Eval(dRigNorm);
															//double dNorm =f1RigDiffSig[ialgo][ifit]->Eval(dRigNorm); //2022.11.17
															//double dNorm =f1RigDiffSig[ialgo][igrSig][ifit]->Eval(dRigNorm); //2022.12.19
															//double dNorm = f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(dRigNorm); //2023.01.12
															//double dNorm = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0)->Eval(dRigNorm); //2023.02.27
															//double dNorm = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0, MCQ)->Eval(dRigNorm); //2023.05.11
															double dNorm = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0, iRigCutQ[iRigSigQ])->Eval(dRigNorm); //2023.05.11
															if (dNorm<=0) continue;
															//cout << "igr=" << igr << " irigdiff=" << irigdiff << " " << dRigNorm << " " << dNorm << endl; //XXX
															
															histn = Form("Norm%s_grSig%d_RigDiff%dvsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igrSig, irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn += "_"+sAlgo[ialgo];
															histn += Form("_cut%d", icut);
															if (ifit==1) histn += "_simpleFit";
															if (ifit==2) histn += "_scaled";
															histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
															#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
															GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVar/dNorm, ww0[ir]); //only flux shape
															#else
															GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVar/dNorm); //noreweight
															#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
															
															/*//--store the RigDiff difference using GenRig and RecRig //2023.01.17
															histn = Form("Norm%s_grSig%d_RigDiff%d_diffvsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igrSig, irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn += "_"+sAlgo[ialgo];
															histn += Form("_cut%d", icut);
															if (ifit==1) histn += "_simpleFit";
															if (ifit==2) histn += "_scaled";
															histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
															//double dVarGenRig = dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(dGenRig);
															//double dVarRecRig = dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(ev.GetRigidity(ir, ialgo));
															//2023.02.27
															//double dVarGenRig = dVar/GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0)->Eval(dGenRig);
															//double dVarRecRig = dVar/GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0)->Eval(ev.GetRigidity(ir, ialgo));
															//2023.05.11
															double dVarGenRig = dVar/GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0, MCQ)->Eval(dGenRig);
															double dVarRecRig = dVar/GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 0, MCQ)->Eval(ev.GetRigidity(ir, ialgo));
															//GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, (dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(dGenRig) - dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(ev.GetRigidity(ir, ialgo)))/dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(ev.GetRigidity(ir, ialgo)), ww0[ir]); //only flux shape
															//GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, (dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(dGenRig) - dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval(ev.GetRigidity(ir, ialgo)))/(dVar/f1RigDiffSig[ialgo][igrSig][ifit][0]->Eval((igr==0)?dGenRig:ev.GetRigidity(ir, ialgo))), ww0[ir]); //only flux shape
															#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
															GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, (dVarGenRig-dVarRecRig)/((igr==0)?dVarGenRig:dVarRecRig), ww0[ir]); //only flux shape
															#else
															GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, (dVarGenRig-dVarRecRig)/((igr==0)?dVarGenRig:dVarRecRig)); //noreweight
															#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE*/
															
															//--normalized 1/R_FS-1/R_Inn //2023.02.27
															if (ir!=2) continue;
															//histn = Form("Norm%s_RigDiff%dvsRig_gr%d_r%d_q%d%s%s", sVarRDiffFS.c_str(), irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str());
															histn = Form("Norm%s_grSig%d_RigDiff%dvsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igrSig, irigdiff, igr, ir, *it, sRig.c_str(), sEvCut.c_str()); //2023.03.07: should use exactly the same naming
															histn += "_"+sAlgo[ialgo];
															histn += Form("_cut%d", icut);
															if (ifit==1) histn += "_simpleFit";
															if (ifit==2) histn += "_scaled";
															histn += Form("_SigQ%d", iRigCutQ[iRigSigQ]);
															//double dNormFS = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 1)->Eval(dRigNorm);
															//double dNormFS = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 1, MCQ)->Eval(dRigNorm); //2023.05.11
															double dNormFS = GetRigDiffSigmavsRig(ialgo, igrSig, ifit, ir, 1, iRigCutQ[iRigSigQ])->Eval(dRigNorm); //2023.05.12
															double dVarFS = 1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo);
															
															#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
															GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVarFS/dNormFS, ww0[ir]); //only flux shape
															#else
															GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVarFS/dNormFS); //noreweight
															#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
														} //iRigSigQ
													} //ifit
												} //igrSig
											} //irigdiff
											
											//--for vs reconstructed rigidity, store the result with RecRig<0 //2023.01.11
											if (igr==1)
											{
												//--RigDiff vs fabs(RecRig)
												histn = Form("%svsAbsRig_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill(fabs((igr==0)?dGenRig:dRig), dVar, ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill(fabs((igr==0)?dGenRig:dRig), dVar); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
												
												//--RigDiff vs RecRig for RecRig<0
												if (dRig<=0)
												{
													histn = Form("%svsRig_lt0_gr%d_r%d_q%d%s%s", sVar.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
													histn += "_"+sAlgo[ialgo];
													histn += Form("_cut%d", icut);
													#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
													GetHist2d(hman1, histn.c_str())->Fill(-1*((igr==0)?dGenRig:dRig), dVar, ww0[ir]); //only flux shape
													#else
													GetHist2d(hman1, histn.c_str())->Fill(-1*((igr==0)?dGenRig:dRig), dVar); //noreweight
													#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
												}
											}
											
											//--Full Span //2022.11.17
											if (ir!=2) continue;
											
											//--store L1L9Chis //2023.02.27
											if (ivar==0)
											{
												string sVarL1L9 = sVarL1L9Chis+sXYVar+sInter+sVCoo+sRigCal;
												double dVarL1L9 = sqrt(ev.GetL1L9Chis(ixy, ialgo));
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarL1L9.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVarL1L9, ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVarL1L9); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											}
											
											//--store FSChis //2022.11.17
											if (ivar==2)
											{
												string sVarFS = sVarFSChis+sXYVar+sInter+sVCoo+sRigCal;
												double dVarFS = sqrt(ev.GetChis(2, ixy, ialgo));
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarFS.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVarFS, ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, dVarFS); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											}
											
											//--store 1/R_FS - 1/R_Inn for Full Span as well
											if (ivar==4)
											{
												histn = Form("%svsRig_gr%d_r%d_q%d%s%s", sVarRDiffFS.c_str(), igr, ir, *it, sRig.c_str(), sEvCut.c_str());
												histn += "_"+sAlgo[ialgo];
												histn += Form("_cut%d", icut);
												#ifdef GBLCHISCHECK_REWEIGHFLUXSHAPE
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, 1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo), ww0[ir]); //only flux shape
												#else
												GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dGenRig:dRig, 1./ev.GetRigidity(2, ialgo) - 1./ev.GetRigidity(0, ialgo)); //noreweight
												#endif //GBLCHISCHECK_REWEIGHFLUXSHAPE
											}
										} //igr
									} //icut
								} //irigcal
								
							} //ivcoo
						} //inter
					} //ixy
				} //ivar
			}
		} //*it
	} //ialgo
} //SelGBLChisCheck
