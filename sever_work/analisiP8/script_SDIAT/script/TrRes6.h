//-----------------
//updated from: TrRes5.C
//updated date: 2020.08.09
//-----------------
//-----------------
//--Tracker resolution
#define BIASED

bool bFillNoWeight = false; //fill histogram without MC reweight or not //Only used in TrRes6.h, moved there

//#define RESIDUALEXAM //2020.08.10: need to update the codes //XXX
#ifdef RESIDUALEXAM
string sDir_re = "Tracker_Residual_Exam";
#endif //RESIDUALEXAM
//--unit transform factor
int icmTOum = 10000;

static bool bTrackTR[3];
static bool bInAllHit=false;
static double dRes=0.;

//--chis range
const int NChisBin = 5;
const double dChisBins[NChisBin] = {0., 10., 100., 1000., 10000.};

#ifdef BIASED
const int NBiasType=2;
#else
const int NBiasType=1;
#endif //BIASED

#ifdef AMSSOFT
bool BookHistoTrRes(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false) //2020.08.10
//bool BookHistoTrRes(HistoMan &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#else
bool BookHistoTrRes(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int QVer, bool dir=false) //2020.08.10
//bool BookHistoTrRes(TObjArray &hman1, bool bIsreal, TFile &outfile, const int QVer, bool dir=false)
#endif
{
	//--booking histogram
	string sDir=Form("Tracker_Resolution%d", QVer);
	if (dir) outfile.mkdir(sDir.c_str());
	
	//2020.08.10
	const int iBinRes=500;
	const int iNBinRes=2*iBinRes;
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		if (dir) outfile.mkdir(Form("%s/q%d", sDir.c_str(), *it));
		for (int ir=0; ir<3; ir++)
		{
			if (dir)
			{
				outfile.mkdir(Form("%s/q%d/%s", sDir.c_str(), *it, sSpan[ir].c_str()));
				outfile.mkdir(Form("%s/q%d/%s/Unbiased", sDir.c_str(), *it, sSpan[ir].c_str()));
				#ifdef BIASED
				outfile.mkdir(Form("%s/q%d/%s/Biased", sDir.c_str(), *it, sSpan[ir].c_str()));
				#endif //BIASED
				outfile.cd(Form("%s/q%d/%s", sDir.c_str(), *it, sSpan[ir].c_str()));
			}
			
			//--Tracker Residual sample
			histn = Form("rig_r%d_q%d_TR", ir, *it);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
			
			//--Tracker Residual
			for (int il=1; il<9-1; il++)
			{
				for (int ixy=0; ixy<2; ixy++)
				{
					if (dir) outfile.cd(Form("%s/q%d/%s/Unbiased", sDir.c_str(), *it, sSpan[ir].c_str()));
					
					histn = Form("TkResidualUB_r%d_q%d_l%d_xy%d", ir, *it, il+1, ixy);
					//hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNBinRes, -iBinRes, iBinRes));
					if (!ev.isreal && bFillNoWeight)
					{
						histn = Form("TkResidualUB_r%d_q%d_l%d_xy%d_nw", ir, *it, il+1, ixy);
						//hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNBinRes, -iBinRes, iBinRes));
					}
					
					#ifdef BIASED
					if (dir) outfile.cd(Form("%s/q%d/%s/Biased", sDir.c_str(), *it, sSpan[ir].c_str()));
					
					histn = Form("TkResidual_r%d_q%d_l%d_xy%d", ir, *it, il+1, ixy);
					//hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNBinRes, -iBinRes, iBinRes));
					if (!ev.isreal && bFillNoWeight)
					{
						histn = Form("TkResidual_r%d_q%d_l%d_xy%d_nw", ir, *it, il+1, ixy);
						//hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNBinRes, -iBinRes, iBinRes));
					}
					#endif //BIASED
				} //ixy
			} //il
		} //ir
	} //*it
	
	#ifdef RESIDUALEXAM
	if (dir) outfile.mkdir(sDir_re.c_str());
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		iNbin = getNbin(*it);
		pBins = getBins(*it);
		
		if (dir) outfile.mkdir(Form("%s/q%d", sDir_re.c_str(), *it));
		for (int ir=0; ir<3; ir++)
		{
			if (dir) outfile.mkdir(Form("%s/q%d/%s", sDir_re.c_str(), *it, sSpan[ir].c_str()));
			
			//--chis cut
			if (dir)
			{
				outfile.mkdir(Form("%s/q%d/%s/InnChisCut", sDir_re.c_str(), *it, sSpan[ir].c_str()));
				outfile.mkdir(Form("%s/q%d/%s/InnChisCut/Unbiased", sDir_re.c_str(), *it, sSpan[ir].c_str()));
				#ifdef BIASED
				outfile.mkdir(Form("%s/q%d/%s/InnChisCut/Biased", sDir_re.c_str(), *it, sSpan[ir].c_str()));
				#endif //BIASED
			}
			
			for (int iub=0; iub<NBiasType; iub++)
			{
				if (iub==0) histn="Unbiased";
				else if (iub==1) histn="Biased";
				if (dir) outfile.cd(Form("%s/q%d/%s/InnChisCut/%s", sDir_re.c_str(), *it, sSpan[ir].c_str(), histn.c_str()));
				
				for (int ichis=0; ichis<NChisBin+1; ichis++)
				{
					//--Tracker Residual sample
					histn = Form("rig_r%d_q%d_c%d_ub%d", ir, *it, ichis, iub);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
					//--Chis distribution
					histn = Form("InnChis_r%d_q%d_c%d_ub%d", ir, *it, ichis, iub);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 4001, -5., 20000.));
					
					//--Tracker Residual
					for (int il=1; il<9-1; il++)
					{
						for (int ixy=0; ixy<2; ixy++)
						{
							histn = Form("TkRes_r%d_q%d_l%d_xy%d_c%d_ub%d", ir, *it, il+1, ixy, ichis, iub);
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
						} //ixy
					} //il
				} //ichis
				
			} //iub
			
			//--fiducial volume
			if (dir)
			{
				outfile.mkdir(Form("%s/q%d/%s/FiduVol", sDir_re.c_str(), *it, sSpan[ir].c_str()));
				outfile.mkdir(Form("%s/q%d/%s/FiduVol/Unbiased", sDir_re.c_str(), *it, sSpan[ir].c_str()));
				#ifdef BIASED
				outfile.mkdir(Form("%s/q%d/%s/FiduVol/Biased", sDir_re.c_str(), *it, sSpan[ir].c_str()));
				#endif //BIASED
			}
			
			for (int iub=0; iub<NBiasType; iub++)
			{
				if (iub==0) histn="Unbiased";
				else if (iub==1) histn="Biased";
				if (dir) outfile.cd(Form("%s/q%d/%s/FiduVol/%s", sDir_re.c_str(), *it, sSpan[ir].c_str(), histn.c_str()));
				
				//--Tracker Residual sample
				histn = Form("rig_r%d_q%d_fv_ub%d", ir, *it, iub);
				hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
				//--Tracker Residual
				for (int il=1; il<9-1; il++)
				{
					for (int ixy=0; ixy<2; ixy++)
					{
						histn = Form("TkRes_r%d_q%d_l%d_xy%d_fv_ub%d", ir, *it, il+1, ixy, iub);
						hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
					} //ixy
				} //il
				
			} //iub
		} //ir
	} //*it
	#endif //RESIDUALEXAM
	
	return true;
} //BookHistoTrRes

/*#ifdef AMSSOFT
bool SelTrRes(HistoMan &hman1, SelEvent &ev, const int s_charge, const int m_charge, bool bRig[3], bool bTrack[3], bool bTkgeom[3], bool bCharge[3])
#else
bool SelTrRes(TObjArray &hman1, SelEvent &ev, const int s_charge, const int m_charge, bool bRig[3], bool bTrack[3], bool bTkgeom[3], bool bCharge[3])
#endif
{
	int tkz = ev.GetTkInZn();
	
	for (int ir=0; ir<3; ir++)
	{
		bTrackTR[ir] = ev.Select_TrTrack(ir, 0);
	}
	
	//--tracker residual
	bInAllHit=true;
	for (int il=0+1; il<9-1; il++) if (!ev.HasTkLHitXY(il)>0) bInAllHit=false; //require hits on all inner layer
	for (int ir=0; ir<3; ir++)
	{
		if (!bInAllHit) break;
		//non-charge selection
		if (!(bRig[ir] && bTrackTR[ir] && bTkgeom[ir])) continue;
		
		//charge range
		#ifdef BZSELECTION
		if (std::find(viQSel.begin(), viQSel.end(), tkz) == viQSel.end() && (tkz<s_charge || tkz>m_charge)) break;
		#else
		if (tkz<s_charge || tkz>m_charge) break;
		#endif //BZSELECTION
		#ifdef REFERENCE //check only 2, 6, 8 within the s_charge and m_charge
		if (tkz>=s_charge && tkz<=m_charge) if (tkz!=2 && tkz!=6 && tkz!=8) break;
		#endif //REFERENCE
		if (!ev.isreal && tkz!=ev.mch) break;
		
		//charge selection
		if (!bCharge[ir]) continue;
		histn = Form("rig_r%d_q%d_TR", ir, tkz);
		GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww);
		
		for (int il=0+1; il<9-1; il++)
		{
			for (int ixy=0; ixy<2; ixy++)
			{
				histn = Form("TkResidualUB_r%d_q%d_l%d_xy%d", ir, tkz, il+1, ixy);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*ev.tk_res1[il][ixy]);
//				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*ev.tk_res1[il][ixy], ww);
				if (!ev.isreal && bFillNoWeight)
				{
					histn = Form("TkResidualUB_r%d_q%d_l%d_xy%d_nw", ir, tkz, il+1, ixy);
					GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*ev.tk_res1[il][ixy]);
				}
			}
			
			#ifdef BIASED
			if (!ev.FitUsedLayer(il)) continue;
			for (int ixy=0; ixy<2; ixy++)
			{
				histn = Form("TkResidual_r%d_q%d_l%d_xy%d", ir, tkz, il+1, ixy);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*ev.tk_res[ir][il][ixy]);
//				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*ev.tk_res[ir][il][ixy], ww);
				if (!ev.isreal && bFillNoWeight) {
					histn = Form("TkResidual_r%d_q%d_l%d_xy%d_nw", ir, tkz, il+1, ixy);
					GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*ev.tk_res[ir][il][ixy]);
				}
				
			} //ixy
			#endif //BIASED
		} //il
	}//ir
	
	#ifdef RESIDUALEXAM
	//--tracker residual exam
	for (int ir=0; ir<3; ir++)
	{
		//if (!bInAllHit) break;
		//non-charge & non-inner tracker reconstruction (fiducial volume, Nhits, hit pattern and chis cut)selection
		//if (!(bRig[ir] && bTrackTR[ir] && bTkgeom[ir])) continue;
		if (!bRig[ir]) continue;
		
		//charge range
		#ifdef BZSELECTION
		if (std::find(viQSel.begin(), viQSel.end(), tkz) == viQSel.end() && (tkz<s_charge || tkz>m_charge)) break;
		#else
		if (tkz<s_charge || tkz>m_charge) break;
		#endif //BZSELECTION
		#ifdef REFERENCE //check only 2, 6, 8 within the s_charge and m_charge
		if (tkz>=s_charge && tkz<=m_charge) if (tkz!=2 && tkz!=6 && tkz!=8) break;
		#endif //REFERENCE
		if (!ev.isreal && tkz!=ev.mch) break;
		
		//charge selection
		if (!bCharge[ir]) continue;
		
		for (int iub=0; iub<NBiasType; iub++)
		{
			//--chis exam
			if (bTkgeom[ir] && bTrackTR[ir] && bInAllHit)
			{
				double dInnChis = ev.GetChis(0, 1);
				int iChisBin=0;
				if (dInnChis<=dChisBins[1]) iChisBin=0;
				else if (dInnChis<=dChisBins[2]) iChisBin=1;
				else if (dInnChis<=dChisBins[3]) iChisBin=2;
				else if (dInnChis<=dChisBins[4]) iChisBin=3;
				else iChisBin=4;
				
				//--Tracker Residual sample
				histn = Form("rig_r%d_q%d_c%d_ub%d", ir, tkz, iChisBin, iub);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww);
				
				//--Chis distribution
				histn = Form("InnChis_r%d_q%d_c%d_ub%d", ir, tkz, iChisBin, iub);
				GetHist(hman1, histn.c_str())->Fill(dInnChis, ww);
			
				//--Tracker Residual
				for (int il=1; il<9-1; il++)
				{
					if (iub==1 && !ev.FitUsedLayer(il)) continue; //since already require all hits, so will also pass this selection
					for (int ixy=0; ixy<2; ixy++)
					{
						histn = Form("TkRes_r%d_q%d_l%d_xy%d_c%d_ub%d", ir, tkz, il+1, ixy, iChisBin, iub);
						double fiducial = (iub==0)?ev.tk_res1[il][ixy]:ev.tk_res[ir][il][ixy];
						GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*fiducial);
					} //ixy
				} //il
			}
			
			//--fiducial volume
			if (!bTkgeom[ir] && bTrackTR[ir] && bInAllHit)
			{
				//--Tracker Residual sample
				histn = Form("rig_r%d_q%d_fv_ub%d", ir, tkz, iub);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww);
			
				//--Tracker Residual
				for (int il=1; il<9-1; il++)
				{
					if (iub==1 && !ev.FitUsedLayer(il)) continue;
					for (int ixy=0; ixy<2; ixy++)
					{
						histn = Form("TkRes_r%d_q%d_l%d_xy%d_fv_ub%d", ir, tkz, il+1, ixy, iub);
						double fiducial = (iub==0)?ev.tk_res1[il][ixy]:ev.tk_res[ir][il][ixy];
						GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*fiducial);
					} //ixy
				} //il
			}
		} //iub
	} //ir
	#endif //RESIDUALEXAM
	return true;
} //SelTrRes*/

//2020.08.09
#ifdef AMSSOFT
bool SelTrRes(HistoMan &hman1, SelEvent &ev, const int span, const double weight=1., const int QVer=0)
#else
bool SelTrRes(TObjArray &hman1, SelEvent &ev, const int span, const double weight=1., const int QVer=0)
#endif
{
	
	bool bHZ=false;
	if (span<1) return false;
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		if (!ev.isreal && *it!=ev.mch) continue; //2020.02.06
		//Inner Tracker Q as reference charge, first check whether the reference charge of each event is inside viQSel or not
		bHZ = (*it==int(ev.GetTkInQNew(2,QVer)+0.5));
		if (!bHZ) continue;
		
		//charge selection
		if (!ev.Select_Charge(*it, span, QVer)) continue;
		histn = Form("rig_r%d_q%d_TR", span, *it);
		GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(span), weight);
		
		for (int il=0+1; il<9-1; il++)
		{
			for (int ixy=0; ixy<2; ixy++)
			{
				histn = Form("TkResidualUB_r%d_q%d_l%d_xy%d", span, *it, il+1, ixy);
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), icmTOum*ev.tk_res1[il][ixy], weight);
				if (!ev.isreal && bFillNoWeight)
				{
					histn = Form("TkResidualUB_r%d_q%d_l%d_xy%d_nw", span, *it, il+1, ixy);
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), icmTOum*ev.tk_res1[il][ixy]);
				}
			}
			
			#ifdef BIASED
			if (!ev.FitUsedLayer(il)) continue;
			for (int ixy=0; ixy<2; ixy++)
			{
				histn = Form("TkResidual_r%d_q%d_l%d_xy%d", span, *it, il+1, ixy);
				GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), icmTOum*ev.tk_res[span][il][ixy], weight);
				if (!ev.isreal && bFillNoWeight)
				{
					histn = Form("TkResidual_r%d_q%d_l%d_xy%d_nw", span, *it, il+1, ixy);
					GetHist2d(hman1, histn.c_str())->Fill(ev.GetRigidity(span), icmTOum*ev.tk_res[span][il][ixy]);
				}
				
			} //ixy
			#endif //BIASED
		} //il
	} //*it
	
	#ifdef RESIDUALEXAM
	//--tracker residual exam
	for (int ir=0; ir<3; ir++)
	{
		//if (!bInAllHit) break;
		//non-charge & non-inner tracker reconstruction (fiducial volume, Nhits, hit pattern and chis cut)selection
		//if (!(bRig[ir] && bTrackTR[ir] && bTkgeom[ir])) continue;
		if (!bRig[ir]) continue;
		
		//charge range
		#ifdef BZSELECTION
		if (std::find(viQSel.begin(), viQSel.end(), tkz) == viQSel.end() && (tkz<s_charge || tkz>m_charge)) break;
		#else
		if (tkz<s_charge || tkz>m_charge) break;
		#endif //BZSELECTION
		#ifdef REFERENCE //check only 2, 6, 8 within the s_charge and m_charge
		if (tkz>=s_charge && tkz<=m_charge) if (tkz!=2 && tkz!=6 && tkz!=8) break;
		#endif //REFERENCE
		if (!ev.isreal && tkz!=ev.mch) break;
		
		//charge selection
		if (!bCharge[ir]) continue;
		
		for (int iub=0; iub<NBiasType; iub++)
		{
			//--chis exam
			if (bTkgeom[ir] && bTrackTR[ir] && bInAllHit)
			{
				double dInnChis = ev.GetChis(0, 1);
				int iChisBin=0;
				if (dInnChis<=dChisBins[1]) iChisBin=0;
				else if (dInnChis<=dChisBins[2]) iChisBin=1;
				else if (dInnChis<=dChisBins[3]) iChisBin=2;
				else if (dInnChis<=dChisBins[4]) iChisBin=3;
				else iChisBin=4;
				
				//--Tracker Residual sample
				histn = Form("rig_r%d_q%d_c%d_ub%d", ir, tkz, iChisBin, iub);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww);
				
				//--Chis distribution
				histn = Form("InnChis_r%d_q%d_c%d_ub%d", ir, tkz, iChisBin, iub);
				GetHist(hman1, histn.c_str())->Fill(dInnChis, ww);
			
				//--Tracker Residual
				for (int il=1; il<9-1; il++)
				{
					if (iub==1 && !ev.FitUsedLayer(il)) continue; //since already require all hits, so will also pass this selection
					for (int ixy=0; ixy<2; ixy++)
					{
						histn = Form("TkRes_r%d_q%d_l%d_xy%d_c%d_ub%d", ir, tkz, il+1, ixy, iChisBin, iub);
						double fiducial = (iub==0)?ev.tk_res1[il][ixy]:ev.tk_res[ir][il][ixy];
						GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*fiducial);
					} //ixy
				} //il
			}
			
			//--fiducial volume
			if (!bTkgeom[ir] && bTrackTR[ir] && bInAllHit)
			{
				//--Tracker Residual sample
				histn = Form("rig_r%d_q%d_fv_ub%d", ir, tkz, iub);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), ww);
			
				//--Tracker Residual
				for (int il=1; il<9-1; il++)
				{
					if (iub==1 && !ev.FitUsedLayer(il)) continue;
					for (int ixy=0; ixy<2; ixy++)
					{
						histn = Form("TkRes_r%d_q%d_l%d_xy%d_fv_ub%d", ir, tkz, il+1, ixy, iub);
						double fiducial = (iub==0)?ev.tk_res1[il][ixy]:ev.tk_res[ir][il][ixy];
						GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), icmTOum*fiducial);
					} //ixy
				} //il
			}
		} //iub
	} //ir
	#endif //RESIDUALEXAM
	return true;
} //SelTrRes


