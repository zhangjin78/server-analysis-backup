//-----------------
//updated from: RigReso2.h
//updated date: 2023.02.04
//-----------------
//-----------------

#include "RigReso2.h" //mainly using the binnings

const string sRigDiffCut = "RigDiffCut_RigReso";

const int NRigDiffSigQ = 2;
const int iRigCutQ[2] = {2, 6};

//const int NAlgoCheck=4;
//const int AlgoCheck[4] = {1, 11, 6, 7}; //{choutko, choutko with MS, kalman, GBL}
//string sAlgo[4] = {"choutko", "choutkoMS", "kalman", "GBL"};
//const int NAlgoCheck=3;
//const int AlgoCheck[3] = {1, 6, 7}; //{choutko, kalman, GBL}
//string sAlgo[3] = {"choutko", "kalman", "GBL"};
//--using My dst, only choutko and GBL
const int NAlgoCheck=2;
const int AlgoCheck[2] = {1, 7}; //{choutko, GBL}
string sAlgo[2] = {"choutko", "GBL"};
//2022.11.17
const int NgrSig=2;
//const int NSigmaFit=2;
//const int NSigmaFit=1;
const int NSigmaFit=3; //2023.01.12: ifit=0: spline fit, =1: simple fit, =2: simple fit rescale to 100 GV

const int iSigmaFitF=0;
//const int iSigmaFitF=1;
//const int iSigmaFitF=2; //2023.02.27

const int NgrGet=2;

//--
//const string sVar0[9] = {"L1Chis", "ExtChis", "L1InnerChis", "InnerChis", "rRL1I_rRI", "L1ExtChis", "L1ScatAngle", "L1ScatAngleRatio", "CooResidual"};
const string sVar0[10] = {"L1Chis", "ExtChis", "L1InnerChis", "InnerChis", "rRL1I_rRI", "L1ExtChis", "L1ScatAngle", "L1ScatAngleRatio", "CooResidual", "CooResidualToL1ScatAngleRatio"};
const string sVarRDiffFS = "rRFS_rRI";
const string sVarFSChis = "FullSpanChis";
const string sVarL1L9Chis = "L1L9Chis"; //simliar to L1Chis, (FS chis + Inner chis)/2 (difference between chis and normalized by dof, which is 2)

//--
const string sXY[3] = {"X", "Y", "XY"};
//2022.11.03
//const int NRigBin=2;
//const double dGBLCheckRigL0[2] = {99., 295.};
//const double dGBLCheckRigU0[2] = {101., 305.};
//2022.11.22
//const int NRigBin=3;
//const double dGBLCheckRigL0[3] = {99., 49.5, 295.};
//const double dGBLCheckRigU0[3] = {101., 50.5, 305.};
//2023.03.16
const int NRigBin=5;
const double dGBLCheckRigL0[NRigBin] = { 99., 49.5, 295., 100., 800.};
const double dGBLCheckRigU0[NRigBin] = {101., 50.5, 305., 200., 1200};

/*//--function to read sigma vs rig
//TF1 *f1RigDiffSigAll[NAlgoCheck][NgrSig][NSigmaFit][2] = {{{{NULL}}}}; //[algo][grSig][fit][Q, 0 for He and 1 for Carbon]
TF1 *f1RigDiffSigAll[NAlgoCheck][NgrSig][NSigmaFit][2]; //[algo][grSig][fit][Q, 0 for He and 1 for Carbon]

//const string sRigDiffSigFitVer = "GBLChisCheckScat8_ScatAngle2"; //should be replaced by ReadRigDiffSigmavsRig, now move to here //2023.05.11
bool ResetSigmavsRig()
{
	//-
	for (int ialgo=0; ialgo<NAlgoCheck; ialgo++)
	{
		for (int igrSig=0; igrSig<NgrSig; igrSig++)
		{
			for (int ifit=0; ifit<NSigmaFit; ifit++) //use simple fit with scale
			{
				for (int iq0=0; iq0<2; iq0++)
				{
					int iq=0;
					if (iq0==0) iq=2;
					else if (iq0==1) iq=6;
					
					//--
					if (f1RigDiffSigAll[ialgo][igrSig][ifit][iq0] != NULL) delete f1RigDiffSigAll[ialgo][igrSig][ifit][iq0];
					f1RigDiffSigAll[ialgo][igrSig][ifit][iq0] = NULL;
				} //iq0
			} //ifit
		} //igrSig
	} //algo
	return true;
}

bool ReadSigmavsRig(const int pass=8, const int ir=1, const string sSigmaFitVer = sRigDiffSigFitVer)
{
	//-
	ResetSigmavsRig();
	
	//--
	ifstream isFit;
	for (int ialgo=0; ialgo<NAlgoCheck; ialgo++)
	{
		for (int igrSig=0; igrSig<NgrSig; igrSig++)
		{
			for (int ifit=0; ifit<NSigmaFit; ifit++) //use simple fit with scale
			{
				if (ifit==0) continue;
				
				for (int iq0=0; iq0<2; iq0++)
				{
					int iq=0;
					if (iq0==0) iq=2;
					else if (iq0==1) iq=6;
					
					//--Read sigma vs rig
					histn = Form("%s/analisi/RigDiffSig/%s/rRL1I_rRI_Sigma_gr%d_r%d_q%d_cut1_Pass%d_%s", sWDirOnline.c_str(), sSigmaFitVer.c_str(), igrSig, ir, iq, pass, sAlgo[ialgo].c_str());
					if (ifit>=1) histn += "_SimpleFit";
					if (ifit>=2) histn += "_Scale";
					isFit.open(histn.c_str());
					if (!isFit.is_open()) continue;
					
					//--create sigma vs rig
					histn = Form("algo%d_r%d_q%d_grSig%d", ialgo, ir, iq, igrSig);
					if (ifit>=1) histn += "_SimpleFit";
					if (ifit>=2) histn += "_Scale";
					f1RigDiffSigAll[ialgo][igrSig][ifit][iq0] = new TF1(histn.c_str(), "sqrt([0]*[0]+[1]*[1]/x/x/x)", 0.1, 10000.);
					for (int ipar=0; ipar<2; ipar++)
					{
						double dPar=0;
						isFit >> dPar;
						f1RigDiffSigAll[ialgo][igrSig][ifit][iq0]->SetParameter(ipar, dPar);
					} //ipar
					isFit.close();
				} //iq0
			} //ifit
		} //igrSig
	} //algo
	return true;
}*/

//inclue Full Span //2023.01.11
/*SplineFit *sfRigDiffSig[NAlgoCheck][NgrSig][2];
TF1 *f1RigDiffSig[NAlgoCheck][NgrSig][NSigmaFit][2];

TF1 *f1FSRigDiffSig[NAlgoCheck][NgrSig][NSigmaFit];*/
//2023.05.11
SplineFit *sfRigDiffSig[NAlgoCheck][NgrSig][NRigDiffSigQ][2];
TF1 *f1RigDiffSig[NAlgoCheck][NgrSig][NSigmaFit][NRigDiffSigQ][2];

SplineFit *sfFSRigDiffSig[NAlgoCheck][NgrSig][NRigDiffSigQ];
TF1 *f1FSRigDiffSig[NAlgoCheck][NgrSig][NSigmaFit][NRigDiffSigQ];

//bool SetRigDiffSigmavsRig(TF1 *f1, const int ialgo, const int igrSig, const int iSigmaFit, const int ir, const int il1fs)
bool SetRigDiffSigmavsRig(TF1 *f1, const int ialgo, const int igrSig, const int iSigmaFit, const int ir, const int il1fs, const int iQ)
{
	int span=ir;
	if (il1fs==0 && ir>1)
	{
		//cout << "Use L1Inner for FS for rRL1-rRI" << endl;
		span=1;
	}
	if (il1fs==1 && ir<2)
	{
		cout << " *** SetRigDiffSigmavsRig: WARNING, no L1Inner geometry for rRFS-rRI, return NULL" << endl;
		return false;
	}

	//if (il1fs==0) f1RigDiffSig[ialgo][igrSig][iSigmaFit][span-1] = f1;
	//else if (il1fs==1) f1FSRigDiffSig[ialgo][igrSig][iSigmaFit] = f1;
	
	//2023.05.11
	int QIndex=-1;
	if (iQ==2) QIndex=0;
	else if (iQ==6) QIndex=1;
	if (QIndex==-1)
	{
		return false;
	}
	
	if (il1fs==0) f1RigDiffSig[ialgo][igrSig][iSigmaFit][QIndex][span-1] = f1;
	else if (il1fs==1) f1FSRigDiffSig[ialgo][igrSig][iSigmaFit][QIndex] = f1;
	return true;
}

//TF1 *GetRigDiffSigmavsRig(const int ialgo, const int igrSig, const int iSigmaFit, const int ir, const int il1fs)
TF1 *GetRigDiffSigmavsRig(const int ialgo, const int igrSig, const int iSigmaFit, const int ir, const int il1fs, const int iQ)
{
	int span=ir;
	if (il1fs==0 && ir>1)
	{
		//cout << "Use L1Inner for FS for rRL1-rRI" << endl;
		span=1;
	}
	if (il1fs==1 && ir<2)
	{
		cout << " *** GetRigDiffSigmavsRig: WARNING, no L1Inner geometry for rRFS-rRI, return NULL" << endl;
		return NULL;
	}

	TF1 *f1 = NULL;
	//if (il1fs==0) f1 = f1RigDiffSig[ialgo][igrSig][iSigmaFit][span-1];
	//else if (il1fs==1) f1 = f1FSRigDiffSig[ialgo][igrSig][iSigmaFit];
	
	//2023.05.11
	int QIndex=-1;
	if (iQ==2) QIndex=0;
	else if (iQ==6) QIndex=1;
	if (QIndex==-1)
	{
		return NULL;
	}
	if (il1fs==0) f1 = f1RigDiffSig[ialgo][igrSig][iSigmaFit][QIndex][span-1];
	else if (il1fs==1) f1 = f1FSRigDiffSig[ialgo][igrSig][iSigmaFit][QIndex];
	
	return f1;
}

//--function to read and set the sigma vs rig function
void ReadRigDiffSigmavsRig(bool bVerbose=false)
{
	//const string sSigmaVer = "GBLChisCheckScat5_ScatAngle2";
	//const string sSigmaVer = "GBLChisCheckScat7_ScatAngle2"; //2022.12.19
	//const string sSigmaVer = "GBLChisCheckScat8_ScatAngle2"; //2023.01.12: for simple fit, use the one rescale to cross Rig=100 GV
	//const string sSigmaVer = "GBLChisCheckScat9_ScatAngle2"; //2023.02.27
	//const string sSigmaVer = "GBLChisCheckScat12_ScatAngle2"; //2023.05.11
	const string sSigmaVer = "GBLChisCheckScat14_ScatAngle2"; //2023.05.22
	
	//--spline fit
	for (int iq0=0; iq0<NRigDiffSigQ; iq0++) //2023.05.11
	{
		int iq = iRigCutQ[iq0];
		for (int ialgo=0; ialgo<NAlgoCheck; ialgo++)
		{
			for (int igrSig=0; igrSig<2; igrSig++)
			{
				//for (int ir=1; ir<3; ir++)
				for (int ir=1; ir<3; ir++)
				{
					for (int il1fs=0; il1fs<2; il1fs++)
					{
						if (il1fs==0 && ir!=1) continue;
						if (il1fs==1 && ir!=2) continue;
						
						//--
						string sVar = (il1fs==0)?"rRL1I_rRI":"rRFS_rRI";
						if (bVerbose) cout << " ****** ir=" << ir << " " << sVar << " ****** " << endl;
						int icut = (il1fs==0)?1:0;
						//string sParFile = Form("%s/analisi/RigDiffSig/%s/q%d/%s/%s_Sigma_gr%d_r%d_q%d_cut%d_Pass%s_%s", sWDirOnline.c_str(), sSigmaVer.c_str(), MCQ, sSpan[ir].c_str(), sVar.c_str(), igrSig, ir, MCQ, icut, ((dstver==68)?"8":"7"), sAlgo[ialgo].c_str());
						string sParFile = Form("%s/analisi/RigDiffSig/%s/q%d/%s/%s_Sigma_gr%d_r%d_q%d_cut%d_Pass%s_%s", sWDirOnline.c_str(), sSigmaVer.c_str(), iq, sSpan[ir].c_str(), sVar.c_str(), igrSig, ir, iq, icut, ((dstver==68)?"8":"7"), sAlgo[ialgo].c_str());
						if (bVerbose) cout << "Spline fit: " << sParFile << endl; //XXX
						//sfRigDiffSig[ialgo][igrSig][iq0][ir-1] = new SplineFit(sParFile.c_str());
						//f1RigDiffSig[ialgo][igrSig][0][iq0][ir-1] = sfRigDiffSig[ialgo][igrSig][iq0][ir-1]->f1SplineFit;
						//2023.05.12
						SplineFit *sfFit = new SplineFit(sParFile.c_str());
						if (il1fs==0) sfRigDiffSig[ialgo][igrSig][iq0][ir-1] = sfFit;
						else if (il1fs==1) sfFSRigDiffSig[ialgo][igrSig][iq0] = sfFit;
						SetRigDiffSigmavsRig(sfFit->f1SplineFit, ialgo, igrSig, 0, ir, il1fs, iq); //2023.05.12
						if (bVerbose) cout << GetRigDiffSigmavsRig(ialgo, igrSig, 0, ir, il1fs, iq)->Eval(100.) << endl; //XXX
						
						//--fit with simple function
						for (int ifit=1; ifit<3; ifit++) //ifit=1: no rescale, =2: rescale to 100 GV
						{
							ifstream isFit;
							string sFit="";
							if (ifit>=1) sFit = "_SimpleFit";
							if (ifit==2) sFit += "_Scale";
							histn = sParFile+sFit;
							if (bVerbose)
							{
								if (ifit==1) cout << "Simple Fit: " << histn << endl;
								else if (ifit==2) cout << "Simple Fit after rescale: " << histn << endl;
							}
							
							isFit.open(Form("%s.par", histn.c_str())); //2023.02.27
							//TF1 *f1SigmavsRig  = new TF1(Form("algo%d_r%d%s", ialgo, ir, sFit.c_str()), "sqrt([0]*[0]+[1]*[1]/x/x/x)", 0.1, 10000.);
							TF1 *f1SigmavsRig  = new TF1(Form("algo%d_r%d%s", ialgo, ir, sFit.c_str()), "sqrt([0]*[0]+[1]*[1]/pow(x, [2]))", 0.1, 10000.);
							for (int ipar=0; ipar<3; ipar++)
							{
								if (ipar<2)
								{
									double dPar=0;
									isFit >> dPar;
									f1SigmavsRig->SetParameter(ipar, dPar);
									if (bVerbose) cout << "ialgo=" << ialgo << " ipar=" << ipar << " " << f1SigmavsRig->GetParameter(ipar) << endl; //XXX
								}
								else
								{
									int iorder=0;
									isFit >> iorder;
									f1SigmavsRig->FixParameter(2, iorder);
									if (bVerbose) cout << "ialgo=" << ialgo << " order: " << f1SigmavsRig->GetParameter(ipar) << endl; //XXX
								}
							} //ipar
							isFit.close();
							if (bVerbose) cout << f1SigmavsRig->Eval(100.) << endl; //XXX
							//SetRigDiffSigmavsRig(f1SigmavsRig, ialgo, igrSig, ifit, ir, il1fs);
							SetRigDiffSigmavsRig(f1SigmavsRig, ialgo, igrSig, ifit, ir, il1fs, iq); //2023.05.11
							isFit.close();
						} //ifit
					} //il1fs
				} //ir
			} //igrSig
		} //ialgo
	} //iq0
}

//--function to read cut
double dRigDiffCut[NAlgoCheck][NgrSig][NgrGet][NSigmaFit][NRigBin][2] = {{{{{{-1.}}}}}}; //[algo][grSig][fit][Q, 0 for He and 1 for Carbon]
const string sRigDiffCutVer = "GBLChisCheckScat9_ScatAngle2";
bool ReadRigDiffCut(const int pass=8, const int ir=1, const string sSigmaCutVer = sRigDiffCutVer)
{
	//--
	ifstream isCut;
	for (int ialgo=1; ialgo<2; ialgo++)
	{
		string sRig="_";
		sRig += sAlgo[ialgo];
		
		for (int iq0=0; iq0<2; iq0++)
		{
			int iq=-1;
			if (iq0==0) iq=2;
			else if (iq0==1) iq=6;
			
			for (int igrSig=1; igrSig<2; igrSig++) //now use sigma vs Rec Rig
			{
				for (int igrGet=1; igrGet<2; igrGet++) //use RecRig to get sigma
				{
					for (int ifit=2; ifit<3; ifit++) //use simple fit with scale
					{
						for (int irigbin=0; irigbin<3; irigbin++)
						{
							double dGBLCheckRigL = dGBLCheckRigL0[irigbin];
							double dGBLCheckRigU = dGBLCheckRigU0[irigbin];
							
							//--
							histn = Form("%s/analisi/RigDiffSig/%s/CutNSig_NormRigDiff_Pass%d_q%d_%s_%.1f_%.1f_Double_grSig%d_grGet%d_fit%d", sWDirOnline.c_str(), sSigmaCutVer.c_str(), pass, iq, sAlgo[ialgo].c_str(), dGBLCheckRigL, dGBLCheckRigU, igrSig, igrGet, ifit);
							isCut.open(histn.c_str());
							if (!isCut.is_open()) continue;
							
							//--
							double dCut=0;
							isCut >> dCut;
							dRigDiffCut[ialgo][igrSig][igrGet][ifit][irigbin][iq0] = fabs(dCut);
							cout << histn << " " << dRigDiffCut[ialgo][igrSig][igrGet][ifit][irigbin][iq0] << endl; //XXX
							
							isCut.close();
						} //irigbin
					} //ifit
				} //igrGet
			} //igrSig
		} //iq0
	} //ialgo
	return true;
}

#ifdef AMSSOFT
bool BookRigDiffCut(HistoMan &hman1, const bool bIsreal, TFile &outfile, int mccharge, const int QVer, const bool dir=false)
#else
bool BookRigDiffCut(TObjArray &hman1, const bool bIsreal, TFile &outfile, int mccharge, const int QVer, const bool dir=false)
#endif //AMSSOFT
{
	if (bIsreal)
	{
		//cout << "***Only do Rigidity Resolution for MC. Stop booking histogram***" << endl;
		return false;
	}
	
	const string sDir=Form("%s%d", sRigDiffCut.c_str(), QVer);
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
	//for (int ir=0; ir<4; ir++)
	//for (int ir=1; ir<2; ir++) //now do for L1Inner only
	for (int ir=1; ir<3; ir++)
	{
		if (dir)
		{
			outfile.mkdir(Form("%s/q%d/%s", sDir.c_str(), mccharge, sSpan[ir].c_str()));
			outfile.cd(Form("%s/q%d/%s", sDir.c_str(), mccharge, sSpan[ir].c_str()));
		}
		
		//for (int ialgo=0; ialgo<2; ialgo++)
		//for (int ialgo=1; ialgo<2; ialgo++)
		for (int ialgo=0; ialgo<NAlgoCheck; ialgo++) //2023.05.24
		{
			//string sRig="_";
			//sRig += sAlgo[ialgo];
			string sRig=""; //2023.05.24
			
			if (dir)
			{
				histn = Form("%s/q%d/%s/Algo%d", sDir.c_str(), mccharge, sSpan[ir].c_str(), ialgo);
				outfile.mkdir(histn.c_str());
			}
			
			//for (int ievcut=0; ievcut<2; ievcut++)
			for (int ievcut=0; ievcut<1; ievcut++)
			{
				string sEvCut="";
				if (ievcut>0) sEvCut = Form("_evcut%d", ievcut);
				
				//for (int ivar=-1; ivar<NVar; ivar++) //ivar=-1: not apply any cut //need to rearrange the include //XXX
				for (int ivar=-1; ivar<0; ivar++) //ivar=-1: not apply any cut //need to rearrange the include
				{
					if (dir)
					{
						histn = Form("%s/q%d/%s/Algo%d/Var%d", sDir.c_str(), mccharge, sSpan[ir].c_str(), ialgo, ivar);
						outfile.mkdir(histn.c_str());
						outfile.cd(histn.c_str());
					}
					//if (ivar>=0 && ivar<iGBLVarL) continue; //need to rearange the include //XXX
					
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
						
						//--
						string sVar = sVar0[ivar]+sXYVar;
						
						for (int inorm=0; inorm<(ivar==4)?2:1; inorm++)
						{
							for (int iRigDiffQ=0; iRigDiffQ<2; iRigDiffQ++) //iRigDiffQ=0: apply the RigDiff cut normalized using He sigma vs Rig, =1: ... using Carbon sigma vs Rig
							{
								//for (int igrSig=1; igrSig<2; igrSig++) //now use sigma vs Rec Rig
								for (int igrSig=0; igrSig<1; igrSig++) //now use sigma vs Gen Rig //2023.05.24
								{
									for (int igrGet=1; igrGet<2; igrGet++) //use RecRig to get sigma
									{
										for (int ifit=2; ifit<3; ifit++) //use simple fit with scale
										{
											for (int icutq=0; icutq<2; icutq++) //icutq=0: cut from He, =1: cut from C
											{
												if (iRigDiffQ==-1 && icutq>0) break;
												for (int irigbin=0; irigbin<3; irigbin++)
												{
													if (iRigDiffQ==-1 && irigbin>0) break;
													if (irigbin==1) continue; //not use the cut for ~50 GV
													
													double dGBLCheckRigL = dGBLCheckRigL0[irigbin];
													double dGBLCheckRigU = dGBLCheckRigU0[irigbin];
													
													string sSuffix = "";
													if (iRigDiffQ>=0)
													{
														sSuffix = Form("_RigDiffQ%d_grSig%d_grGet%d_fit%d_cutq%d_%.1f_%.1f", iRigCutQ[iRigDiffQ], igrSig, igrGet, ifit, iRigCutQ[icutq], dGBLCheckRigL, dGBLCheckRigU);
													}
													
													//--event count
													histn = Form("rig_r%d_q%d%s%s_rec", ir, mccharge, sRig.c_str(), sEvCut.c_str());
													histn += sSuffix;
													hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
													
													histn = Form("rig_r%d_q%d%s%s_gen", ir, mccharge, sRig.c_str(), sEvCut.c_str());
													histn += sSuffix;
													if (ialgo==0) hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
													
													//--drRig vs Rig
													for (int igr=0; igr<2; igr++)
													{
														histn = Form("drRigvsRig%s_r%d_q%d%s%s", ((igr==0)?"Gen":"Rec"), ir, mccharge, sRig.c_str(), sEvCut.c_str());
														histn += sSuffix;
														hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNEvenLogBin, dEvenLogBins, nRigResoBinNoNorm, dRigResoRangeNoNorm[0], dRigResoRangeNoNorm[1]));
														GetHist2d(hman1, histn.c_str())->Sumw2();
													} //igr
												} //irigbin
											} //icutq
										} //ifit
									} //igrGet
								} //igrSig
							} //iRigDiffQ
						} //inorm
					} //ixy
				} //ivar
			} //ievcut
		} //ialgo
	} //ir
	
	//--
	cout << " ****** BookRigDiffCut: read RigDiff sigma vs Rig ****** " << endl;
	//ReadSigmavsRig();
	ReadRigDiffSigmavsRig(); //2023.05.11
	cout << " ****** BookRigDiffCut: read RigDiff Cut ****** " << endl;
	ReadRigDiffCut();
	return true;
} //BookRigDiffCut

#ifdef AMSSOFT
bool SelRigDiffCut(HistoMan &hman1, SelEvent &ev, TFile &outfile, const int span, const int QVer, const double weight, const int ialgo)
#else
bool SelRigDiffCut(TObjArray &hman1, SelEvent &ev, TFile &outfile, const int span, const int QVer, const double weight, const int ialgo)
#endif //AMSSOFT
{
	if (ev.isreal)
	{
		//cout << "***Only do Rigidity Resolution for MC. Stop booking histogram***" << endl;
		return false;
	}
	
	const string sDir=Form("%s%d", sRigDiffCut.c_str(), QVer);
	
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
		int iRigAlgo=ialgo;
		double dRigRec = ev.GetRigidity(span,iRigAlgo), dRigGen = ev.mmom/ev.mch;
		string sRig="_";
		sRig += sAlgo[ialgo];
		
		double dEknGen = RigToEkn(dRigGen, (int)ev.mch, MCMass);
		double dDeltaRRig = 1/dRigRec - 1/dRigGen;
		double dDeltaRRigNoNorm=dDeltaRRig; //resolution without normalization
		
		/*histn = Form("rig_r%d_q%d_gen", span, (int)ev.mch);
		TH1 *h1bin = GetHist(hman1, histn.c_str());
		
		double dRigNorm=h1bin->GetBinCenter(h1bin->FindBin(dRigGen));
		dDeltaRRig=1/sqrt(pow(dNp0,2)+pow(dNp1/dRigNorm,2))*dDeltaRRig;*/
		
		double dRigDiff = -9999.;
		if (span==1) dRigDiff = 1./ev.GetRigidity(1, iRigAlgo) - 1./ev.GetRigidity(0, iRigAlgo);
		
		//for (int ievcut=0; ievcut<2; ievcut++)
		for (int ievcut=0; ievcut<1; ievcut++)
		{
			bool bEvCut=true;
			if (ievcut==1) bEvCut = ev.ntrack==1 || ev.Select_Tk2nd();
			if (!bEvCut) continue;
			
			string sEvCut="";
			if (ievcut>0) sEvCut = Form("_evcut%d", ievcut);
			
			for (int iRigDiffQ=-1; iRigDiffQ<2; iRigDiffQ++) //iRigDiffQ=-1: do not apply RigDiff cut, =0: apply the RigDiff cut normalized using He sigma vs Rig, =1: ... using Carbon sigma vs Rig
			{
				for (int igrSig=1; igrSig<2; igrSig++) //now use sigma vs Rec Rig
				{
					for (int igrGet=1; igrGet<2; igrGet++) //use RecRig to get sigma
					{
						double dRigNorm = (igrGet==0)?dRigGen:dRigRec;
						for (int ifit=2; ifit<3; ifit++) //use simple fit with scale
						{
							for (int icutq=0; icutq<2; icutq++) //icutq=0: cut from He, =1: cut from C
							{
								if (iRigDiffQ==-1 && icutq>0) break;
								for (int irigbin=0; irigbin<3; irigbin++)
								{
									if (iRigDiffQ==-1 && irigbin>0) break;
									if (irigbin==1) continue; //not use the cut for ~50 GV
									
									//--apply cut on Rig Diff
									bool bRigDiffCut = true;
									//if (iRigDiffQ!=-1) bRigDiffCut = fabs(dRigDiff/f1RigDiffSigAll[ialgo][igrSig][ifit][iRigDiffQ]->Eval(dRigNorm)) <= dRigDiffCut[ialgo][igrSig][igrGet][ifit][irigbin][icutq];
									if (iRigDiffQ!=-1) bRigDiffCut = fabs(dRigDiff/GetRigDiffSigmavsRig(ialgo, igrSig, ifit, 1, 0, iRigCutQ[iRigDiffQ])->Eval(dRigNorm)) <= dRigDiffCut[ialgo][igrSig][igrGet][ifit][irigbin][icutq];
									if (!bRigDiffCut) continue;
									
									//--
									double dGBLCheckRigL = dGBLCheckRigL0[irigbin];
									double dGBLCheckRigU = dGBLCheckRigU0[irigbin];
									
									string sSuffix = "";
									if (iRigDiffQ>=0)
									{
										sSuffix = Form("_RigDiffQ%d_grSig%d_grGet%d_fit%d_cutq%d_%.1f_%.1f", iRigCutQ[iRigDiffQ], igrSig, igrGet, ifit, iRigCutQ[icutq], dGBLCheckRigL, dGBLCheckRigU);
									}
									
									//--event count
									histn = Form("rig_r%d_q%d%s%s_rec", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
									histn += sSuffix;
									GetHist(hman1, histn.c_str())->Fill(dRigRec);
									
									histn = Form("rig_r%d_q%d%s%s_gen", span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
									histn += sSuffix;
									if (ialgo==0) GetHist(hman1, histn.c_str())->Fill(dRigGen);
									
									//--drRig vs Rig
									for (int igr=0; igr<2; igr++)
									{
										histn = Form("drRigvsRig%s_r%d_q%d%s%s", ((igr==0)?"Gen":"Rec"), span, (int)ev.mch, sRig.c_str(), sEvCut.c_str());
										histn += sSuffix;
										GetHist2d(hman1, histn.c_str())->Fill((igr==0)?dRigGen:dRigRec, dDeltaRRigNoNorm); 
									} //igr
								} //irigbin
							} //icutq
						} //ifit
					} //igrGet
				} //igrSig
			} //iRigDiffQ
		} //ievcut
	} //charge selection
	return true;
} //SelRigReso
