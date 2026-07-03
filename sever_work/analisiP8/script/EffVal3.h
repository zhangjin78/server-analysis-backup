//-----------------
//updated from: EffVal2.C
//updated date: 2018.06.12
//-----------------
//2018.06.12
//	1) store the resolution for 3 kinds of estimation: new method using weighted mean, new method using MPV, old method
//	2) store ESTRIGCOM also for 3 kinds of estimation
//	3) store the estimated rigidity in 2D arrays, use flag DIFFVERCOM to decide whether store different version or not (in 
//	   default using new method with weighted mean 
//	4) do the comparison between different version for cutoff and Edep estimated rigidity
//
//2018.07.31
//	1) store also the resolution for new estimation: old method with weighted mean
//
//2018.08.05
//	1) store tracking efficiency using different estimation - TKEFF
//
//2018.08.19
//	1) for the L1Inner tracking eff, since we require tracker to pass all ECal only for ECal estimatior, the other two estimator 
//	   will have different geometer, so their eff will be different. Now also require the same for Beta & Cutoff
//-----------------
#define MYCAL
#ifdef MYCAL
//#include "B1120_Cal.h"
//#include "B1123_Cal.h"
#include "B1123_Calv2.h"
#include "B1123_Calv3.h"
#else
#include "B800MCCor.C"
#endif

#define SUCCESSIVE
//#define SUCCSAM

#define L1CHICUT

#define TKEFF

//#define RIGRESOLU
//#define ESTRIGCOM
#define DIFFVERCOM
#ifdef DIFFVERCOM
//const int nVersion=3;
const int nVersion=4;
#else
const int nVersion=1;
#endif //DIFFVERCOM
//------
//--EffVal
static int tkl1z=0;
static bool bFluxSam=false;

static bool bL1XYf=false, bL1Zf=false;
static bool bBZSelf=false;

static bool bL1Qf=false, bL9Qf=false, bExtQf[3]={false, false, false}, bTofQf[3]={false, false, false};

//static double dEstRig[3] = {0.};
static double dEstRigFlux[3][nVersion] = {{0.}};
static double dEstRigEff[3][nVersion] = {{0.}};
static double dEstFlux[3] = {0.};
static double dEstEff[3] = {0.};
static bool bEstCut[3] = {false, false, false};

static bool bTkEffSam=false;

#ifdef AMSSOFT
bool BookHistoEffVal(HistoMan &hman1, const bool bIsreal, const int s_charge, const int m_charge, TFile &outfile, const bool dir=false)
//bool BookHistoEffVal(HistoMan &hman1, bool bIsreal, const int s_charge, const int m_charge)
#else
bool BookHistoEffVal(TObjArray &hman1, const bool bIsreal, const int s_charge, const int m_charge, TFile &outfile, const bool dir=false)
#endif //AMSSOFT
{
	if (dir) outfile.mkdir("TkEffSam_Validate");
	
	for (int ich=1; ich<=30; ich++) {
		#ifdef BZSELECTION
		if (std::find(viQSel.begin(), viQSel.end(), ich) == viQSel.end() && (ich<s_charge || ich>m_charge)) continue; //continue if not in viQSel and outside select range
		#else
		if (ich<s_charge || ich>m_charge) continue;
		#endif //BZSELECTION
		#ifdef REFERENCE //check only 2, 6, 8 within 
		if (ich>=s_charge && ich<=m_charge) if (ich!=2 && ich!=6 && ich!=8) continue;
		#endif //REFERENCE
		
		iNbin = getNbin(ich);
		pBins = getBins(ich);
		int iNbin2 = getNbin2(ich);
		double *pBins2 = getBins2(ich);
		if (dir) {
			outfile.mkdir(Form("TkEffSam_Validate/q%d", ich));
			outfile.cd(Form("TkEffSam_Validate/q%d", ich));
		}
		
		for (int ir=1; ir<3; ir++) {
			if (dir) {
				outfile.mkdir(Form("TkEffSam_Validate/q%d/%s", ich, cSpan[ir]));
				outfile.cd(Form("TkEffSam_Validate/q%d/%s", ich, cSpan[ir]));
			}
			
			for (int icom=0; icom<3; icom++) { //icom=0: flux, =1: flux with estimator cut, =2: efficiency
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/com%d", ich, cSpan[ir], icom));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/com%d", ich, cSpan[ir], icom));
				}
				
				//------flux without Inner Tracker vs TkEffSam
				if (icom==0) {
					histn = Form("rig_r%d_q%d_c%d_xcutoff", ir, ich, icom);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
					histn = Form("rig_r%d_q%d_c%d", ir, ich, icom);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
					/*#ifdef SUCCSAM
					if (dir) {
						outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/com%d/succsam", ich, cSpan[ir], icom));
						outfile.cd(Form("TkEffSam_Validate/q%d/%s/com%d/succsam", ich, cSpan[ir], icom));
					}
					for (int is=0; is<nSuccTkSamVal; is++) {
						if (ir<2 && is>=nSuccTkSamValR1) break;
						if (is==nSuccTkSamValR1-2) continue;
						histn = Form("rig_r%d_q%d_c%d_s%d", ir, ich, icom, is);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					} //is
					#endif //SUCCSAM*/
				}
				else {
					for (int imr=0; imr<=Mimr; imr++) {
						histn = Form("rig_r%d_q%d_c%d_mr%d_xcutoff", ir, ich, icom, imr);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						histn = Form("rig_r%d_q%d_c%d_mr%d", ir, ich, icom, imr);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
						/*#ifdef SUCCSAM
						if (dir) {
							outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/com%d/succsam%d", ich, cSpan[ir], icom, imr));
							outfile.cd(Form("TkEffSam_Validate/q%d/%s/com%d/succsam%d", ich, cSpan[ir], icom, imr));
						}
						for (int is=0; is<nSuccTkSamVal; is++) {
							if (ir<2 && is>=nSuccTkSamValR1) break;
							histn = Form("rig_r%d_q%d_c%d_mr%d_s%d", ir, ich, icom, imr, is);
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
						} //is
						#endif //SUCCSAM*/
					} //imr
				}
			} //icom
			
			//------2D plots, full flux vs TkEffSam
			if (dir) {
				outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/2D", ich, cSpan[ir]));
				outfile.cd(Form("TkEffSam_Validate/q%d/%s/2D", ich, cSpan[ir]));
			}
			
			for (int imr=0; imr<Mimr+1; imr++) {
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/2D/%s", ich, cSpan[ir], cRigMethod[imr]));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/2D/%s", ich, cSpan[ir], cRigMethod[imr]));
				}
				
				histn = Form("rig2D_r%d_q%d_mr%d_sam_xcutoff", ir, ich, imr);
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin2, pBins2, iNbin2, pBins2));
				
				histn = Form("rig2D_r%d_q%d_mr%d_sam", ir, ich, imr);
				hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin2, pBins2, iNbin2, pBins2));
				
				#ifdef SUCCSAM
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/2D/%s/succsam", ich, cSpan[ir], cRigMethod[imr]));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/2D/%s/succsam", ich, cSpan[ir], cRigMethod[imr]));
				}
				for (int is=0; is<nSuccTkSamVal; is++) {
					if (ir<2 && is>=nSuccTkSamValR1) break;
					histn = Form("rig2D_r%d_q%d_mr%d_sam_s%d", ir, ich, imr, is);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin2, pBins2, iNbin2, pBins2));
					
				} //is
				#endif //SUCCSAM
				
				#ifdef SUCCESSIVE
				for (int is=0; is<nSuccTk; is++) {
					histn = Form("rig2D_r%d_q%d_mr%d_s%d_xcutoff", ir, ich, imr, is);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin2, pBins2, iNbin2, pBins2));
					
					histn = Form("rig2D_r%d_q%d_mr%d_s%d", ir, ich, imr, is);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin2, pBins2, iNbin2, pBins2));
				} //is
				#endif //SUCCESSIVE
			} //imr
			
			#ifdef RIGRESOLU
			for (int iver=0; iver<nVersion; iver++) {
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/RigResolu%d", ich, cSpan[ir], iver));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/RigResolu%d", ich, cSpan[ir], iver));
				}
			
				for (int imr=0; imr<Mimr+1; imr++) {
					if (iver>0 && imr==0) continue;
					if (dir) {
						outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/RigResolu%d/%s", ich, cSpan[ir], iver, cRigMethod[imr]));
						outfile.cd(Form("TkEffSam_Validate/q%d/%s/RigResolu%d/%s", ich, cSpan[ir], iver, cRigMethod[imr]));
					}
				
					//------delta 1/R vs R_m
					//----apply Flux sam + TkEff sam
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam_xcutoff", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam_xcutoff_nocut", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam_nocut", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					//----apply Flux + TkEff sam
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel_xcutoff", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel_xcutoff_nocut", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel_nocut", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					//----FluxSam & Flux, using normal track
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_fluxsam_xcutoff", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_fluxsam", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_flux_xcutoff", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_flux", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 4000, -2, 2));
				
					//------delta R vs R_m
					//----apply Flux sam + TkEff sam
					histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sam_xcutoff", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
				
					histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sam", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
				
					//----apply Flux + TkEff sam
					histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sel_xcutoff", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
				
					histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sel", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 600, -300, 300));
				
				} //imr
			} //iver
			#endif //RIGRESOLU
			
			#ifdef TKEFF
			for (int iver=0; iver<nVersion; iver++) {
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/TkEff%d", ich, cSpan[ir], iver));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/TkEff%d", ich, cSpan[ir], iver));
				}
				for (int imr=0; imr<=Mimr; imr++) {
					if (iver>0 && imr==0) continue;
					if (dir) {
						outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/TkEff%d/%s", ich, cSpan[ir], iver, cRigMethod[imr]));
						outfile.cd(Form("TkEffSam_Validate/q%d/%s/TkEff%d/%s", ich, cSpan[ir], iver, cRigMethod[imr]));
					}
					histn = Form("rig_r%d_q%d_TkEffSam_v%d_mr%d", ir, ich, iver, imr);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
					histn = Form("rig_r%d_q%d_TkEffSel_v%d_mr%d", ir, ich, iver, imr);
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					#ifdef SUCCESSIVE
					for (int is=0; is<nSuccTk; is++) {
						histn = Form("rig_r%d_q%d_TkEffSel_v%d_mr%d_s%d", ir, ich, iver, imr, is);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					}
					#endif //SUCCESSIVE-2
					
					//--requiring passing all ecal for Beta and Cutoff
					if (imr<2) {
						histn = Form("rig_r%d_q%d_TkEffSamPassEcal_v%d_mr%d", ir, ich, iver, imr);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						histn = Form("rig_r%d_q%d_TkEffSelPassEcal_v%d_mr%d", ir, ich, iver, imr);
						hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
						#ifdef SUCCESSIVE
						for (int is=0; is<nSuccTk; is++) {
							histn = Form("rig_r%d_q%d_TkEffSelPassEcal_v%d_mr%d_s%d", ir, ich, iver, imr, is);
							hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						}
						#endif //SUCCESSIVE-2
					} //imr<2
				} //imr
			} //iver
			#endif //TKEFF
			
			#ifdef ESTRIGCOM
			for (int iver=0; iver<nVersion; iver++) {
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/EstCom%d", ich, cSpan[ir], iver));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/EstCom%d", ich, cSpan[ir], iver));
				}
			
				/*for (int imr=1; imr<Mimr+1; imr++) {
					histn = Form("estcom_r%d_q%d_mr%d_v%d_sam", ir, ich, imr, iver);
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				} //imr*/
			
				for (int itype=0; itype<2; itype++) {
					if (itype==0) histn1="fluxsam";
					else if (itype==1) histn1="tkeffsam";
					//--Comparison between different estimator
					histn = Form("BetavsCutoff_r%d_q%d_v%d_%s", ir, ich, iver, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
					histn = Form("BetavsEcal_r%d_q%d_v%d_%s", ir, ich, iver, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
					histn = Form("CutoffvsEcal_r%d_q%d_v%d_%s", ir, ich, iver, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
					histn = Form("BetavsCutoff_r%d_q%d_v%d_%s_cut", ir, ich, iver, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
					histn = Form("BetavsEcal_r%d_q%d_v%d_%s_cut", ir, ich, iver, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
					histn = Form("CutoffvsEcal_r%d_q%d_v%d_%s_cut", ir, ich, iver, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
				
					//--estimator distribution
					if (iver>0) continue;
					histn = Form("%s_r%d_q%d_%s", cRigMethod[0], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1100, 0, 1.1));
				
					histn = Form("%s_r%d_q%d_%s", cRigMethod[1], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 3500, 0., 35.));
				
					histn = Form("%s_r%d_q%d_%s", cRigMethod[2], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9000, 0., 900.));
				
					histn = Form("%s_r%d_q%d_%s_Mev", cRigMethod[2], ir, ich, histn1.c_str()); //to check the negative region
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 5100, -10, 500));
				
					histn = Form("%s_r%d_q%d_%s_cut", cRigMethod[0], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 1100, 0, 1.1));
				
					histn = Form("%s_r%d_q%d_%s_cut", cRigMethod[1], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 3500, 0., 35.));
				
					histn = Form("%s_r%d_q%d_%s_cut", cRigMethod[2], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 9000, 0., 900.));
				
					histn = Form("%s_r%d_q%d_%s_Mev_cut", cRigMethod[2], ir, ich, histn1.c_str());
					hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 5100, -10, 500));
				} //itype
			} //iver
			
			#ifdef DIFFVERCOM
			if (dir) {
				outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/DiffVerCom", ich, cSpan[ir]));
				outfile.cd(Form("TkEffSam_Validate/q%d/%s/DiffVerCom", ich, cSpan[ir]));
			}
			
			for (int imr=1; imr<3; imr++) {
				if (dir) {
					outfile.mkdir(Form("TkEffSam_Validate/q%d/%s/DiffVerCom/%s", ich, cSpan[ir], cRigMethod[imr]));
					outfile.cd(Form("TkEffSam_Validate/q%d/%s/DiffVerCom/%s", ich, cSpan[ir], cRigMethod[imr]));
				}
				for (int itype=0; itype<2; itype++) {
					if (itype==0) histn1="fluxsam";
					else if (itype==1) histn1="tkeffsam";
					//--Comparison between different estimator
					/*histn = Form("v0v1_r%d_q%d_mr%d_%s", ir, ich, imr, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
			
					histn = Form("v0v2_r%d_q%d_mr%d_%s", ir, ich, imr, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
			
					histn = Form("v1v2_r%d_q%d_mr%d_%s", ir, ich, imr, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
			
					histn = Form("v0v1_r%d_q%d_mr%d_%s_cut", ir, ich, imr, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
			
					histn = Form("v0v2_r%d_q%d_mr%d_%s_cut", ir, ich, imr, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
			
					histn = Form("v1v2_r%d_q%d_mr%d_%s_cut", ir, ich, imr, histn1.c_str());
					hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));*/
					
					for (int iver1=0; iver1<nVersion; iver1++) {
						for (int iver2=iver1+1; iver2<nVersion; iver2++) {
							histn = Form("v%dv%d_r%d_q%d_mr%d_%s", iver1, iver2, ir, ich, imr, histn1.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
						
							histn = Form("v%dv%d_r%d_q%d_mr%d_%s_cut", iver1, iver2, ir, ich, imr, histn1.c_str());
							hman1.Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, iNbin, pBins));
						} //iver2
					} //iver1

				} //itype
			}
			#endif //DIFFVERCOM
			#endif //ESTRIGCOM
		} //ir
	} //ich
	return true;
} //BookHistoEffVal

#ifdef AMSSOFT
bool SelEffVal(HistoMan &hman1, SelEvent &ev, const bool bIsreal, const int s_charge, const int m_charge, const double weight, const bool bPart, const bool bBeta, const bool bTrack[3], const bool bTkgeom[3], const bool bCharge[3], const int PRODVER=0)
#else
bool SelEffVal(TObjArray &hman1, SelEvent &ev, const bool bIsreal, const int s_charge, const int m_charge, const double weight, const bool bPart, const bool bBeta, const bool bTrack[3], const bool bTkgeom[3], const bool bCharge[3], const int PRODVER=0)
#endif //AMSSOFT
{
	int tk_z = ev.GetTkInZ();
	#ifdef BZSELECTION
	for (it=viQSel.begin(); it!=viQSel.end(); it++) {
		if (ev.Select_BZCharge(*it, 0)) {	//-2/+2	InnerQ
			tk_z = *it;
			break; //find for one BZ then break the loop on all BZSELECTION charges
		} //UTofQ
	} //it
	#endif //BZSELECTION
	
	//------sel for Flux sample, compatiable with TkEff sample
	//--L1XY & L1Z
	bL1XYf = (ev.HasTkLHitXY(0)==2);
	
	tkl1z = int(ev.GetTkLQ(0)+0.5);
	bL1Zf = (tkl1z>=s_charge) && (tkl1z<=m_charge);
	#ifdef REFERENCE
	if (bL1Zf) bL1Zf = (tkl1z==2) || (tkl1z==6) || (tkl1z==8);
	#endif //REFERENCE
	
	bBZSelf=false;
	#ifdef BZSELECTION
	for (it=viQSel.begin(); it!=viQSel.end(); it++) {
		if (ev.Select_BZCharge(*it, 3, 0)) {	//-2/+2	L1Q
			tkl1z = *it;
			bL1Zf=true;
			bBZSelf=true;
			break; //find for one BZ then break the loop on all BZSELECTION charges
		}
	} //it
	#endif //BZSELECTION
	
	//--L1Q
	bL1Qf = ev.Select_ExtQn(1, 1);
	bL1Qf = bL1Qf && ((ev.tk_qls[0] & 0x10013D)==0);
	
	//--L9Q
	bL9Qf = ev.Select_ExtQn(3, 1);
	bL9Qf = bL9Qf && (ev.HasTkLHitXY(8)==2);
	
	bExtQf[0] = true;
	bExtQf[1] = bL1Qf;
	bExtQf[2] = bL1Qf && bL9Qf;
	
	//tof
	bTofQf[1] = ev.Select_TofQn(1, true, 1);
	bTofQf[2] = ev.Select_TofQn(2, true, 1);
	
	//--beta
	dEstFlux[0] = ev.tof_betah;
	//dEstRigFlux[0][0] = dEstRigFlux[0][1] = dEstRigFlux[0][2] = MPROTON*dMass[tkl1z]*dEstFlux[0] / sqrt(1 - pow(dEstFlux[0], 2)) / tkl1z;
	for (int iver=0; iver<nVersion; iver++) dEstRigFlux[0][iver] = MPROTON*dMass[tkl1z]*dEstFlux[0] / sqrt(1 - pow(dEstFlux[0], 2)) / tkl1z;
	bEstCut[0] = (dEstFlux[0]>0 && dEstFlux[0]<0.96);
	
	//--cutoff
	if (!ev.isreal) {for (int iver=0; iver<nVersion; iver++) dEstRigFlux[0][iver] = ev.mmom/ev.mch; dEstFlux[1] = 0;}
	//if (!ev.isreal) {dEstRigFlux[1][0] = dEstRigFlux[1][1] = dEstRigFlux[1][2] = ev.mmom/ev.mch; dEstFlux[1] = 0;}
	else {
		//get estimator
		#ifdef AMSSOFT
		AMSSetupR::RTI a;
		if(AMSEventR::GetRTI(a,ev.time[0])!=0) dEstFlux[1] = 0;
		else dEstFlux[1] = a.cfi[icffv][1];
		#else
		dEstFlux[1] = ev.mcutoffi[icffv][1];
		#endif //AMSSOFT
		
		//get estimated rigidity
		#ifdef MYCAL
		#ifdef ESTRIGV2
		dEstRigFlux[1][0] = getcutoffrig2(dEstFlux[1], tkl1z);
		#else
		dEstRigFlux[1][0] = getcutoffrig(dEstFlux[1], tkl1z, PRODVER);
		#endif //ESTRIGVE
		
		#ifdef DIFFVERCOM
		for (int iver=0; iver<nVersion; iver++) dEstRigFlux[1][iver] = getcutoffrig3(dEstFlux[1], tkl1z, iver);
		//dEstRigFlux[1][0] = getcutoffrig3(dEstFlux[1], tkl1z, 0);
		//dEstRigFlux[1][1] = getcutoffrig3(dEstFlux[1], tkl1z, 1);
		//dEstRigFlux[1][2] = getcutoffrig3(dEstFlux[1], tkl1z, 2);
		#endif //DIFFVERCOM
		
		#else
		dEstRigFlux[1][0] = getcutoffrig(dEstFlux[1], 2, tkl1z);
		#endif //MYCAL
	}
	//bEstCut[1] = dEstRigFlux[1][0]>0.8 && dEstRigFlux[1][0]<32;
	bEstCut[1] = dEstFlux[1]>0.8 && dEstFlux[1]<32;
	
	//--Edep
	dEstFlux[2] = ev.ecal_en[2]/1000;
	#ifdef MYCAL
	#ifdef ESTRIGV2
	dEstRigFlux[2][0]=getedeprig2(dEstFlux[2], tkl1z);
	#else
	dEstRigFlux[2][0]=getedeprig(dEstFlux[2], tkl1z, PRODVER);
	#endif //ESTRIGV2
	#ifdef DIFFVERCOM
	for (int iver=0; iver<nVersion; iver++) dEstRigFlux[2][iver] = getedeprig3(dEstFlux[2], tkl1z, iver);
	//dEstRigFlux[2][0] = getedeprig3(dEstFlux[2], tkl1z, 0);
	//dEstRigFlux[2][1] = getedeprig3(dEstFlux[2], tkl1z, 1);
	//dEstRigFlux[2][2] = getedeprig3(dEstFlux[2], tkl1z, 2);
	#endif //DIFFVERCOM
	#else
	dEstRigFlux[2][0]=getedeprig(dEstFlux[2], tkl1z, ev.isreal, 2);
	#endif //MYCAL
	bEstCut[2] = (ev.ecal_en[2]>0);
	//bEstCut[2] = (ev.ecal_en[2]>0) && (ev.ecal_dis<10);
	bool CutEcalCharge = ev.ecal_en[2]>1500;
	if (tkl1z>=6) CutEcalCharge = ev.ecal_en[2]>2500;
	if (tkl1z>=8) CutEcalCharge = ev.ecal_en[2]>3000;
	//bEstCut[2] = bEstCut[2] && CutEcalCharge;
	bEstCut[2] = bEstCut[2] && ev.IsPassEcal();
	
	//------sel for track eff sample
	//--TrdTofParticle
	bTRDTOFPar = !(ev.ibetahs<0||ev.itrdtracks<0||ev.betahs<0.4);
	
	//--L1XY & L1Z
	bL1XY = (ev.tk_l1qxy[0]>0&&ev.tk_l1qxy[1]>0);
	int tk_l1zu = int(ev.tk_l1qvs+0.5);
//	bL1Z = tk_l1zu>=s_charge;
	bL1Z = (tk_l1zu>=s_charge) && (tk_l1zu<=m_charge);
	#ifdef REFERENCE
	if (bL1Z) bL1Z = (tk_l1zu==2) || (tk_l1zu==6) || (tk_l1zu==8);
	#endif //REFERENCE
	
	bBZSel=false;
	#ifdef BZSELECTION
	for (it=viQSel.begin(); it!=viQSel.end(); it++) {
		if (ev.Select_BZCharge(*it, 3, 1)) {	//-2/+2	unbiased L1Q
			tk_l1zu = *it;
			bL1Z=true;
			bBZSel=true;
//			bTRDTOFPar = bTRDTOFPar && ((ev.trd_rqs==0) || (ev.trd_rqs>2)); //requir the trd track quality
//			bTRDTOFPar = bTRDTOFPar && (ev.trd_rqs>2); //requir the trd track quality
			break; //find for one BZ then break the loop on all BZSELECTION charges
		} //UTofQ
	} //it
	#endif //BZSELECTION
	
	//--fiducial volume cut
	bool mlh[9];
	int mnhitf=0;
	for(int il=0;il<9;il++){
		mlh[il]=false;
		float coox=ev.tk_pos1s[il][0];
		float cooy=ev.tk_pos1s[il][1];
		float disr=coox*coox+cooy*cooy;
		if (il==9-1) mlh[il]=(fabs(coox)<cirr[il]&&fabs(cooy)<ciry[il]);
		else mlh[il]=(sqrt(disr)<cirr[il]&&fabs(cooy)<ciry[il]);
	} //il
	
	for(int il=0+1;il<9-1;il++){
		if(mlh[il]){mnhitf++;}
	} //il
	bGeometry[0] = ( (mnhitf>=5) && (mlh[1]) && (mlh[2] || mlh[3]) && (mlh[4] || mlh[5]) && (mlh[6] || mlh[7]) );
	bGeometry[1] = bGeometry[0] && mlh[0];
	bGeometry[2] = bGeometry[1] && mlh[8];
	
	//--L1Q
	l1Q = ((ev.tk_l1qvs > tk_l1zu-0.2) && (ev.tk_l1qvs < tk_l1zu+0.27));
	#ifdef BZSELECTION
//	if (std::find(viQSel.begin(), viQSel.end(), tk_l1zu) != viQSel.end()) l1Q = true;
	if (bBZSel) l1Q = true;
	#endif //BZSELECTION
	
	l1Qs = ((ev.tk_l1qs & 0x10013D)==0);
	
	bL1Q = l1Q && l1Qs && (ev.tk_l1mds<5);
	
	//--L9Q
	l9xy = (ev.tk_l9qxy[0]>0 && ev.tk_l9qxy[1]>0);
	l9Q = (ev.tk_l9qvs > tk_l1zu-0.5) && (ev.tk_l9qvs < tk_l1zu+1);
	#ifdef BZSELECTION
	if (std::find(viQSel.begin(), viQSel.end(), tk_l1zu) != viQSel.end()) l9Q = ev.Select_BZCharge(tk_l1zu, 4, 1);
	#endif //BZSELECTION
	bL9Q = l9xy && l9Q;
	
	bExtQ[0] = true;
	bExtQ[1] = bL1Q;
	bExtQ[2] = bL1Q && bL9Q;
	
	//--TOF
	//ToFBeta
	bToFNHits[1] = bToFNHits[2] = (ev.tof_nhits==4);
	bToFChis = (ev.tof_chiscs<10 && ev.tof_chists<10);
//	bToFChis = true;
	
	//ToFQ
	int tofz_unbiased = int(ev.tof_qls[0]+0.5);
	bool cuttofch[2] = {true, true};
	for (int ilay=0;ilay<2;ilay++) if (fabs(ev.tof_qls[ilay]-tofz_unbiased)>0.4) cuttofch[0]=false;
	for (int ilay=2;ilay<4;ilay++) if (fabs(ev.tof_qls[ilay]-tofz_unbiased)>0.4) cuttofch[1]=false;
	
	#ifdef BZSELECTION
	if (bBZSel) {
		if (ev.Select_BZCharge(tk_l1zu, 1, 1)) { //-2/+2	unbiased TofQ
			tofz_unbiased = tk_l1zu;
			cuttofch[0] = (fabs(ev.tof_qls[0]-tofz_unbiased)<=ISSTofLQ[tk_l1zu-1]) && (fabs(ev.tof_qls[1]-tofz_unbiased)<=ISSTofLQ[tk_l1zu-1]);
			cuttofch[1] = (fabs(ev.tof_qls[2]-tofz_unbiased)<=ISSTofLQ[tk_l1zu-1]) && (fabs(ev.tof_qls[3]-tofz_unbiased)<=ISSTofLQ[tk_l1zu-1]);
//			cuttofch[0] = (fabs(ev.tof_qls[0]-tofz_unbiased)<=0.8) && (fabs(ev.tof_qls[1]-tofz_unbiased)<=0.8);
//			cuttofch[1] = (fabs(ev.tof_qls[2]-tofz_unbiased)<=0.8) && (fabs(ev.tof_qls[3]-tofz_unbiased)<=0.8);
		} //unbiased Tof selected
	} //BZ selected
	#endif //BZSELECTION
	
	bTofQ[1] = (tofz_unbiased == tk_l1zu) && cuttofch[0];
	bTofQ[2] = bTofQ[1] && cuttofch[1];
	
	//------calculate for each rigidity estimator
	//beta
	double beta = ev.betahs;
	dEstEff[0] = beta;
	for (int iver=0; iver<nVersion; iver++) dEstRigEff[0][iver] = MPROTON*dMass[tk_l1zu]*beta / sqrt(1 - pow(beta, 2)) / tk_l1zu;
	//dEstRigEff[0][0] = dEstRigEff[0][1] = dEstRigEff[0][2] = MPROTON*dMass[tk_l1zu]*beta / sqrt(1 - pow(beta, 2)) / tk_l1zu;
	bMethodCut[0] = (dEstEff[0]>0 && dEstEff[0]<0.96);
	
	//cutoff or generate rigidity
	if (!bIsreal) {for (int iver=0; iver<nVersion; iver++) dEstRigEff[0][iver] == ev.mmom/ev.mch; dEstEff[1] = 0;}
	//if (!bIsreal) {dEstRigEff[1][0] = dEstRigEff[1][1] = dEstRigEff[1][2]= ev.mmom/ev.mch; dEstEff[1] = 0;}
	else {
		//get estimation
		#ifdef AMSSOFT
		AMSSetupR::RTI a;
		if(AMSEventR::GetRTI(a,ev.time[0])!=0) dEstEff[1] = 0;
		else dEstEff[1] = a.cfi[icffv][1];
		#else
		dEstEff[1] = ev.mcutoffi[icffv][1];
		#endif //AMSSOFT
		
		//get estimated rigidity
		#ifdef MYCAL
		#ifdef ESTRIGV2
		dEstRigEff[1][0] = getcutoffrig2(dEstEff[1], tk_l1zu);
		#else
		dEstRigEff[1][0] = getcutoffrig(dEstEff[1], tk_l1zu, PRODVER);
		#endif //ESTRIGV2
		#ifdef DIFFVERCOM
		for (int iver=0; iver<nVersion; iver++) dEstRigEff[1][iver] = getcutoffrig3(dEstEff[1], tk_l1zu, iver);
		//dEstRigEff[1][0] = getcutoffrig3(dEstEff[1], tk_l1zu, 0);
		//dEstRigEff[1][1] = getcutoffrig3(dEstEff[1], tk_l1zu, 1);
		//dEstRigEff[1][2] = getcutoffrig3(dEstEff[1], tk_l1zu, 2);
		//dEstRigEff[1][3] = getcutoffrig3(dEstEff[1], tk_l1zu, 3);
		#endif //DIFFVERCOM
		#else
		dEstRigEff[1][0] = getcutoffrig(dEstEff[1], 2, tk_l1zu);
		#endif //MYCAL
	}
	//bMethodCut[1] = dEstRigEff[1][0]>0.8 && dEstRigEff[1][0]<32;
	//bMethodCut[1] = dEstRigEff[1][0]>3.9 && dEstRigEff[1][0]<25;
	bMethodCut[1] = dEstEff[1]>0.8 && dEstEff[1]<32;
	
	//Ecal
	dEstEff[2] = ev.ecal_ens/1000;
	#ifdef MYCAL
	#ifdef ESTRIGV2
	dEstRigEff[2][0]=getedeprig2(dEstEff[2], tk_l1zu);
	#else
	dEstRigEff[2][0]=getedeprig(dEstEff[2], tk_l1zu, PRODVER);
	#endif //ESTRIGV2
	#ifdef DIFFVERCOM
	for (int iver=0; iver<nVersion; iver++) dEstRigEff[2][iver] = getedeprig3(dEstEff[2], tk_l1zu, iver);
	//dEstRigEff[2][0] = getedeprig3(dEstEff[2], tk_l1zu, 0);
	//dEstRigEff[2][1] = getedeprig3(dEstEff[2], tk_l1zu, 1);
	//dEstRigEff[2][2] = getedeprig3(dEstEff[2], tk_l1zu, 2);
	//dEstRigEff[2][3] = getedeprig3(dEstEff[2], tk_l1zu, 3);
	#endif //DIFFVERCOM
	#else
	dEstRigEff[2][0]=getedeprig(dEstEff[2], tk_l1zu, ev.isreal, 2);
	#endif
	bMethodCut[2] = (ev.ecal_ens>0) && (ev.ecal_dis<10);
	CutEcalCharge = ev.ecal_ens>1500;
	if (tk_l1zu>=6) CutEcalCharge = ev.ecal_ens>2500;
	if (tk_l1zu>=8) CutEcalCharge = ev.ecal_ens>3000;
	//bMethodCut[2] = bMethodCut[2] && CutEcalCharge;
	bMethodCutEcalOld = bMethodCut[2] && CutEcalCharge;
	bMethodCut[2] = bMethodCut[2] && ev.IsPassEcal(1); //unbiased tracker
	//bMethodCut[2] = bMethodCut[2] && ev.IsPassEcal(2); //beta
	#ifdef SUCCESSIVE
	bTkSucc[0] = bPart;
	bTkSucc[1] = bTkSucc[0] && bBeta;
	//Track
	bTkSucc[2] = bTkSucc[1] && (ev.GetTkInNHit()>=5);
	bool l2H = (ev.HasTkLHitXY(1)>0);
	bool l34H = ((ev.HasTkLHitXY(2)>0)||(ev.HasTkLHitXY(3)>0));
	bool l56H = ((ev.HasTkLHitXY(4)>0)||(ev.HasTkLHitXY(5)>0));
	bool l78H = ((ev.HasTkLHitXY(6)>0)||(ev.HasTkLHitXY(7)>0));
	bool innerHitYP = l2H && l34H && l56H && l78H;
	bTkSucc[3] = bTkSucc[2] && innerHitYP;
	bTkSucc[4] = bTkSucc[3] && (ev.GetChis(0, 1) < 10);
	
	//fiducial volume
	int Ex_mnhitf=0;
	for(int il=0+1;il<9-1;il++) {
		if(ev.IsPassTkL(il)){Ex_mnhitf++;}
	}
	bTkSucc[5] = bTkSucc[4] && (Ex_mnhitf>=5);
	bTkSucc[6] = bTkSucc[5] && bTkgeom[0]; //bTkgeom[0] is actually including bTkSucc[5]
	
	//charge
	bTkSucc[7] = bTkSucc[6] && (tk_z-0.45 < ev.GetTkInQ()) && (ev.GetTkInQ() < tk_z+0.45);
	#ifdef BZSELECTION
	if (bBZSel) bTkSucc[7] = bTkSucc[6] && (tk_z==tk_l1zu);
	#endif //BZSELECTION
	bTkSucc[8] = bTkSucc[7] && (ev.GetTkInQrms() < ( (tk_z<3)?0.4:( (tk_z<9)?0.55:(0.57+0.1*(tk_z-9)) ) ) );
	bTkSucc[9] = bTkSucc[8] && (tk_z == tk_l1zu);
	#endif //SUCCESSIVE
	
	#ifdef SUCCSAM
	bool bSuccSamTk[nSuccTkSamVal];
	bSuccSamTk[0]=bL1Zf;
	bSuccSamTk[1]=bSuccSamTk[0] && bPart;
	bSuccSamTk[2]=bSuccSamTk[1] && bL1XYf;
	bSuccSamTk[3]=bSuccSamTk[2] && ((ev.tk_qls[0] & 0x10013D)==0);
	bSuccSamTk[4]=bSuccSamTk[3] && bTofQf[1];
	bSuccSamTk[5]=bSuccSamTk[4] && ev.IsPassTkL(0);
	bSuccSamTk[6]=bSuccSamTk[5] && true;
	/*bSuccSamTk[7]=bSuccSamTk[6] && ev.HasTkLHitXY(8)==2;
	bSuccSamTk[8]=bSuccSamTk[7] && ev.Select_ExtQn(3, 1);
	bSuccSamTk[9]=bSuccSamTk[8] && ev.IsPassTkL(8);
	bSuccSamTk[10]=bSuccSamTk[9] && bTofQf[2];*/
	
	bool bSuccSamEff[nSuccTkSamVal];
	bSuccSamEff[0]=bL1Z;
	bSuccSamEff[1]=bSuccSamEff[0] && bTRDTOFPar;
	bSuccSamEff[2]=bSuccSamEff[1] && bL1XY;
	bSuccSamEff[3]=bSuccSamEff[2] && l1Qs;
	bSuccSamEff[4]=bSuccSamEff[3] && bTofQ[1];
	bSuccSamEff[5]=bSuccSamEff[4] && mlh[0];
	bSuccSamEff[6]=bSuccSamEff[5] && bGeometry[0];
	/*bSuccSamEff[7]=bSuccSamEff[6] && l9xy;
	bSuccSamEff[8]=bSuccSamEff[7] && l9Q;
	bSuccSamEff[9]=bSuccSamEff[8] && mlh[8];
	bSuccSamEff[10]=bSuccSamEff[9] && bTofQ[2];*/
	#endif //SUCCSAM
	
	//----selecting Flux sample and TkEff sample
	for (int ir=1; ir<3; ir++) {
		//TkEff sample (icom=2)
		bTkEffSam = bTRDTOFPar && bL1XY && bL1Z && bGeometry[ir] && bExtQ[ir] && bTofQ[ir];
		if (bTkEffSam)
		//if (bTRDTOFPar && bL1XY && bL1Z && bGeometry[ir] && bExtQ[ir] && bTofQ[ir] && bToFChis)
		{
			for (int imr=0; imr<=Mimr; imr++) {
				//if (!bMethodCut[imr]) continue;
				
				histn = Form("rig_r%d_q%d_c%d_mr%d_xcutoff", ir, tk_l1zu, 2, imr);
				if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][0], weight);
				
				if (ev.isreal && imr != 1 && !(dEstRigEff[imr][0] > ev.GetBinCutoff(icffv))) continue; //gcutoff
				
				histn = Form("rig_r%d_q%d_c%d_mr%d", ir, tk_l1zu, 2, imr);
				if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][0], weight);
				
				#ifdef TKEFF
				if (!bMethodCut[imr]) continue;
				for (int iver=0; iver<nVersion; iver++) {
					if (iver>0 && imr==0) continue;
					histn = Form("rig_r%d_q%d_TkEffSam_v%d_mr%d", ir, tk_l1zu, iver, imr);
					GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver], weight);
					
					if (bTkSucc[9]) {
						histn = Form("rig_r%d_q%d_TkEffSel_v%d_mr%d", ir, tk_l1zu, iver, imr);
						GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver], weight);
					}
					#ifdef SUCCESSIVE
					for (int is=0; is<nSuccTk; is++) {
						if (bTkSucc[is]) {
							histn = Form("rig_r%d_q%d_TkEffSel_v%d_mr%d_s%d", ir, tk_l1zu, iver, imr, is);
							GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver], weight);
						}
						else break;
					}
					#endif //SUCCESSIVE-2
					
					if (imr<2 && ev.IsPassEcal(1)) {
						histn = Form("rig_r%d_q%d_TkEffSamPassEcal_v%d_mr%d", ir, tk_l1zu, iver, imr);
						GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver], weight);
					
						if (bTkSucc[9]) {
							histn = Form("rig_r%d_q%d_TkEffSelPassEcal_v%d_mr%d", ir, tk_l1zu, iver, imr);
							GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver], weight);
						}
						#ifdef SUCCESSIVE
						for (int is=0; is<nSuccTk; is++) {
							if (bTkSucc[is]) {
								histn = Form("rig_r%d_q%d_TkEffSelPassEcal_v%d_mr%d_s%d", ir, tk_l1zu, iver, imr, is);
								GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver], weight);
							}
							else break;
						}
						#endif //SUCCESSIVE-2
					} //imr<2 && ev.IsPassEcal(1)
				} //iver
				#endif //TKEFF
			} //imr
			
			#ifdef ESTRIGCOM
			histn1="tkeffsam";
			for (int iver=0; iver<nVersion; iver++) {
				//--without cut
				histn = Form("BetavsCutoff_r%d_q%d_v%d_%s", ir, tk_l1zu, iver, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigEff[1][iver], dEstRigEff[0][iver]);
			
				histn = Form("BetavsEcal_r%d_q%d_v%d_%s", ir, tk_l1zu, iver, histn1.c_str());
				if (ev.IsPassEcal(1)) GetHist(hman1, histn.c_str())->Fill(dEstRigEff[2][iver], dEstRigEff[0][iver]);
			
				histn = Form("CutoffvsEcal_r%d_q%d_v%d_%s", ir, tk_l1zu, iver, histn1.c_str());
				if (ev.IsPassEcal(1)) GetHist(hman1, histn.c_str())->Fill(dEstRigEff[2][iver], dEstRigEff[1][iver]);
			
				//--with cut
				if (bMethodCut[0] || bMethodCut[1]) {
					histn = Form("BetavsCutoff_r%d_q%d_v%d_%s_cut", ir, tk_l1zu, iver, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill((bMethodCut[1])?dEstRigEff[1][iver]:-1, (bMethodCut[0])?dEstRigEff[0][iver]:-1);
				}
			
				if (bMethodCut[0] || bMethodCut[2]) {
					histn = Form("BetavsEcal_r%d_q%d_v%d_%s_cut", ir, tk_l1zu, iver, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill((bMethodCut[2])?dEstRigEff[2][iver]:-1, (bMethodCut[0])?dEstRigEff[0][iver]:-1);
				}
			
				if (bMethodCut[1] || bMethodCut[2]) {
					histn = Form("CutoffvsEcal_r%d_q%d_v%d_%s_cut", ir, tk_l1zu, iver, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill((bMethodCut[2])?dEstRigEff[2][iver]:-1, (bMethodCut[1])?dEstRigEff[1][iver]:-1);
				}
			
			} //iver
			
			//--1d distribution
			for (int imr=0; imr<3; imr++) {
				//without cut
				histn = Form("%s_r%d_q%d_%s", cRigMethod[imr], ir, tk_l1zu, histn1.c_str());
				if (imr!=2 || ev.IsPassEcal(1))GetHist(hman1, histn.c_str())->Fill(dEstEff[imr]);
			
				//with cut
				histn = Form("%s_r%d_q%d_%s_cut", cRigMethod[imr], ir, tk_l1zu, histn1.c_str());
				if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(dEstEff[imr]);
			
				if (imr==2) {
					histn = Form("%s_r%d_q%d_%s_Mev", cRigMethod[imr], ir, tk_l1zu, histn1.c_str());
					if (imr!=2 || ev.IsPassEcal()) GetHist(hman1, histn.c_str())->Fill(ev.ecal_ens);
				
					histn = Form("%s_r%d_q%d_%s_Mev_cut", cRigMethod[imr], ir, tk_l1zu, histn1.c_str());
					if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(ev.ecal_ens);
				}
			} //imr
			
			#ifdef DIFFVERCOM
			//--Comparison between different estimator
			for (int imr=1; imr<3; imr++) {
				/*//--Comparison between different estimator
				histn = Form("v0v1_r%d_q%d_mr%d_%s", ir, tk_l1zu, imr, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][1], dEstRigEff[imr][0]);
		
				histn = Form("v0v2_r%d_q%d_mr%d_%s", ir, tk_l1zu, imr, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][2], dEstRigEff[imr][0]);
		
				histn = Form("v1v2_r%d_q%d_mr%d_%s", ir, tk_l1zu, imr, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][2], dEstRigEff[imr][1]);
				
				if (bMethodCut[imr]) {
					histn = Form("v0v1_r%d_q%d_mr%d_%s_cut", ir, tk_l1zu, imr, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][1], dEstRigEff[imr][0]);
				
					histn = Form("v0v2_r%d_q%d_mr%d_%s_cut", ir, tk_l1zu, imr, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][2], dEstRigEff[imr][0]);
				
					histn = Form("v1v2_r%d_q%d_mr%d_%s_cut", ir, tk_l1zu, imr, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][2], dEstRigEff[imr][1]);
				}*/
				for (int iver1=0; iver1<nVersion; iver1++) {
					for (int iver2=iver1+1; iver2<nVersion; iver2++) {
						//--Comparison between different estimator
						histn = Form("v%dv%d_r%d_q%d_mr%d_%s", iver1, iver2, ir, tk_l1zu, imr, histn1.c_str());
						GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver2], dEstRigEff[imr][iver1]);
						
						if (bMethodCut[imr]) {
							histn = Form("v%dv%d_r%d_q%d_mr%d_%s_cut", iver1, iver2, ir, tk_l1zu, imr, histn1.c_str());
							GetHist(hman1, histn.c_str())->Fill(dEstRigEff[imr][iver2], dEstRigEff[imr][iver1]);
						}
					} //iver2
				} //iver1
			} //imr
			#endif //DIFFVERCOM
			#endif //ESTRIGCOM
		}
		
		//Flux sample (icom=0,1)
		bFluxSam = bPart && bL1XYf && bL1Zf && bTkgeom[ir] && bExtQf[ir] && bTofQf[ir];
		if (bFluxSam) {
			//icom=0
			histn = Form("rig_r%d_q%d_c%d_xcutoff", ir, tkl1z, 0);
			GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), weight);
			
			if (!ev.isreal || ev.GetRigidity(ir)> ev.GetBinCutoff(icffv)) //gcutoff
			{
				histn = Form("rig_r%d_q%d_c%d", ir, tkl1z, 0);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), weight);
			}
			
			//icom=1
			for (int imr=0; imr<=Mimr; imr++) {
				if (!bEstCut[imr]) continue;
				
				histn = Form("rig_r%d_q%d_c%d_mr%d_xcutoff", ir, tkl1z, 1, imr);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), weight);
				
				if (ev.isreal && imr != 1 && !(ev.GetRigidity(ir) > ev.GetBinCutoff(icffv))) continue; //gcutoff
				
				histn = Form("rig_r%d_q%d_c%d_mr%d", ir, tkl1z, 1, imr);
				GetHist(hman1, histn.c_str())->Fill(ev.GetRigidity(ir), weight);
			} //imr
			
			#ifdef ESTRIGCOM
			histn1="fluxsam";
			for (int iver=0; iver<nVersion; iver++) {
				histn = Form("BetavsCutoff_r%d_q%d_v%d_%s", ir, tkl1z, iver, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[1][iver], dEstRigFlux[0][0]);
			
				histn = Form("BetavsEcal_r%d_q%d_v%d_%s", ir, tkl1z, iver, histn1.c_str());
				if (ev.IsPassEcal()) GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[2][iver], dEstRigFlux[0][0]);
			
				histn = Form("CutoffvsEcal_r%d_q%d_v%d_%s", ir, tkl1z, iver, histn1.c_str());
				if (ev.IsPassEcal()) GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[2][iver], dEstRigFlux[1][iver]);
			
				//--with cut
				if (bEstCut[0] || bEstCut[1]) {
					histn = Form("BetavsCutoff_r%d_q%d_v%d_%s_cut", ir, tkl1z, iver, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill((bEstCut[1])?dEstRigFlux[1][iver]:-1, (bEstCut[0])?dEstRigFlux[0][0]:-1);
				}
			
				if (bEstCut[0] || bEstCut[2]) {
					histn = Form("BetavsEcal_r%d_q%d_v%d_%s_cut", ir, tkl1z, iver, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill((bEstCut[2])?dEstRigFlux[2][iver]:-1, (bEstCut[0])?dEstRigFlux[0][0]:-1);
				}
			
				if (bEstCut[1] || bEstCut[2]) {
					histn = Form("CutoffvsEcal_r%d_q%d_v%d_%s_cut", ir, tkl1z, iver, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill((bEstCut[2])?dEstRigFlux[2][iver]:-1, (bEstCut[1])?dEstRigFlux[1][iver]:-1);
				}
			}
			//--1d distribution
			for (int imr=0; imr<3; imr++) {
				//without cut
				histn = Form("%s_r%d_q%d_%s", cRigMethod[imr], ir, tkl1z, histn1.c_str());
				if (imr!=2 || ev.IsPassEcal()) GetHist(hman1, histn.c_str())->Fill(dEstFlux[imr]);
				
				//with cut
				histn = Form("%s_r%d_q%d_%s_cut", cRigMethod[imr], ir, tkl1z, histn1.c_str());
				if (bEstCut[imr]) GetHist(hman1, histn.c_str())->Fill(dEstFlux[imr]);
				
				if (imr==2) {
					histn = Form("%s_r%d_q%d_%s_Mev", cRigMethod[imr], ir, tkl1z, histn1.c_str());
					if (imr!=2 || ev.IsPassEcal()) GetHist(hman1, histn.c_str())->Fill(ev.ecal_en[2]);
					
					histn = Form("%s_r%d_q%d_%s_Mev_cut", cRigMethod[imr], ir, tkl1z, histn1.c_str());
					if (bEstCut[imr]) GetHist(hman1, histn.c_str())->Fill(ev.ecal_en[2]);
				}
			} //imr
			
			#ifdef DIFFVERCOM
			//--Comparison between different estimator
			for (int imr=1; imr<3; imr++) {
				/*//--Comparison between different estimator
				histn = Form("v0v1_r%d_q%d_mr%d_%s", ir, tkl1z, imr, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][1], dEstRigFlux[imr][0]);
		
				histn = Form("v0v2_r%d_q%d_mr%d_%s", ir, tkl1z, imr, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][2], dEstRigFlux[imr][0]);
		
				histn = Form("v1v2_r%d_q%d_mr%d_%s", ir, tkl1z, imr, histn1.c_str());
				GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][2], dEstRigFlux[imr][1]);
				
				if (bEstCut[imr]) {
					histn = Form("v0v1_r%d_q%d_mr%d_%s_cut", ir, tkl1z, imr, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][1], dEstRigFlux[imr][0]);
				
					histn = Form("v0v2_r%d_q%d_mr%d_%s_cut", ir, tkl1z, imr, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][2], dEstRigFlux[imr][0]);
				
					histn = Form("v1v2_r%d_q%d_mr%d_%s_cut", ir, tkl1z, imr, histn1.c_str());
					GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][2], dEstRigFlux[imr][1]);
				}*/
				for (int iver1=0; iver1<nVersion; iver1++) {
					for (int iver2=iver1+1; iver2<nVersion; iver2++) {
						//--Comparison between different estimator
						histn = Form("v%dv%d_r%d_q%d_mr%d_%s", iver1, iver2, ir, tkl1z, imr, histn1.c_str());
						GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][iver2], dEstRigFlux[imr][iver1]);
						
						if (bMethodCut[imr]) {
							histn = Form("v%dv%d_r%d_q%d_mr%d_%s_cut", iver1, iver2, ir, tkl1z, imr, histn1.c_str());
							GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][iver2], dEstRigFlux[imr][iver1]);
						}
					} //iver2
				} //iver1
			} //imr
			#endif //DIFFVERCOM
			#endif //ESTRIGCOM
		}
		
		if (tk_l1zu != tkl1z) continue;
		#ifdef SUCCSAM
		for (int imr=0; imr<=Mimr; imr++) {
			bSuccSamTk[7]=bSuccSamTk[6] && bEstCut[imr];
			bSuccSamTk[8]=bSuccSamTk[7] && (ev.HasTkLHitXY(8)==2);
			bSuccSamTk[9]=bSuccSamTk[8] && ev.Select_ExtQn(3, 1);
			bSuccSamTk[10]=bSuccSamTk[9] && ev.IsPassTkL(8);
			bSuccSamTk[11]=bSuccSamTk[10] && bTofQf[2];
			
			bSuccSamEff[7]=bSuccSamEff[6] && bMethodCut[imr];
			bSuccSamEff[8]=bSuccSamEff[7] && l9xy;
			bSuccSamEff[9]=bSuccSamEff[8] && l9Q;
			bSuccSamEff[10]=bSuccSamEff[9] && mlh[8];
			bSuccSamEff[11]=bSuccSamEff[10] && bTofQ[2];
			
			double rigf=ev.GetRigidity(ir);
			double rigu=dEstRigEff[imr][0];
			for (int is=0; is<nSuccTkSamVal; is++) {
				if (ir<2 && is>=nSuccTkSamValR1) break;
				if (!bSuccSamTk[is]) rigf=0;
				if (!bSuccSamEff[is]) rigu=0;
				if (rigf==0 && rigu==0) break; //stop if both not pass the selection
				histn = Form("rig2D_r%d_q%d_mr%d_sam_s%d", ir, tkl1z, imr, is);
				GetHist(hman1, histn.c_str())->Fill(rigf, rigu);
			} //is
		} //imr
		#endif //SUCCSAM
		
		//--2D & rigidity resolution
		for (int imr=0; imr<=Mimr; imr++) {
			bool bCutoffFlux=(!ev.isreal || imr == 1 || ev.GetRigidity(ir)> ev.GetBinCutoff(icffv));
			bool bCutoffFluxEst=(!ev.isreal || imr == 1 || dEstRigFlux[imr][0]> ev.GetBinCutoff(icffv));
			bool bCutoffTkEff=(!ev.isreal || imr == 1 || (dEstRigEff[imr][0] > ev.GetBinCutoff(icffv)));
			
			#ifdef RIGRESOLU
			for (int iver=0; iver<nVersion; iver++) {
				if (iver>0 && imr==0) continue;
				bCutoffFluxEst=(!ev.isreal || imr == 1 || dEstRigFlux[imr][iver]> ev.GetBinCutoff(icffv));
				double dDeltarRFlux = 1./dEstRigFlux[imr][iver] - 1./ev.GetRigidity(ir);
				if (bFluxSam && (imr!=2 || ev.IsPassEcal())) {
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_fluxsam_xcutoff", ir, tkl1z, imr, iver);
					GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][iver], dDeltarRFlux);
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_fluxsam", ir, tkl1z, imr, iver);
					if (bCutoffFlux && bCutoffFluxEst) GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][iver], dDeltarRFlux);
				
					if (bPart && bBeta && bTrack[0] && bTkgeom[0] && (tk_z == tkl1z) && ((bBZSel)?ev.GetTkInQrms()<(0.57+0.1*(tk_z-9)):bCharge[0])) {
						histn = Form("rRvsR_r%d_q%d_mr%d_v%d_flux_xcutoff", ir, tkl1z, imr, iver);
						GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][iver], dDeltarRFlux);
				
						histn = Form("rRvsR_r%d_q%d_mr%d_v%d_flux", ir, tkl1z, imr, iver);
						if (bCutoffFlux && bCutoffFluxEst) GetHist(hman1, histn.c_str())->Fill(dEstRigFlux[imr][iver], dDeltarRFlux);
					}
				}
			} //iver
			#endif //RIGRESOLU
			
			if (!bTkEffSam && !bFluxSam) continue;
			//if (!(bTkEffSam && bMethodCut[imr]) && !bFluxSam) continue;
			
//			//--without cutoff
			bool bSuccTk = bFluxSam;
			double rigu=(bTkEffSam)?dEstRigEff[imr][0]:0;
			//double rigu=(bTkEffSam && bMethodCut[imr])?dEstRigEff[imr][0]:0;
			double rigu_cutoff = (bCutoffTkEff)?rigu:0;
			double rigf=(bSuccTk)?ev.GetRigidity(ir):0;
			double rigf_cutoff=(bCutoffFlux)?rigf:0;
			
			if (rigf==0 && rigu==0) continue; //stop if both not pass the selection
			
			histn = Form("rig2D_r%d_q%d_mr%d_sam_xcutoff", ir, tkl1z, imr);
			if ((rigu!=0 && bMethodCut[imr]) || rigf!=0) GetHist(hman1, histn.c_str())->Fill(rigu, rigf);
			
			if (rigf_cutoff!=0 || rigu_cutoff!=0) {
				histn = Form("rig2D_r%d_q%d_mr%d_sam", ir, tkl1z, imr);
				if ((rigu_cutoff!=0 && bMethodCut[imr]) || rigf_cutoff!=0) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, rigf_cutoff);
			}
			
			#ifdef RIGRESOLU
			rigf=(bSuccTk)?ev.GetRigidity(ir):0;
			rigf_cutoff=(bCutoffFlux)?rigf:0;
			for (int iver=0; iver<nVersion; iver++) {
				if (iver>0 && imr==0) continue;
				bCutoffTkEff=(!ev.isreal || imr == 1 || (dEstRigEff[imr][iver] > ev.GetBinCutoff(icffv)));
				rigu=(bTkEffSam)?dEstRigEff[imr][iver]:0;
				//rigu=(bTkEffSam && bMethodCut[imr])?dEstRigEff[imr][iver]:0;
				rigu_cutoff = (bCutoffTkEff)?rigu:0;
				
				double dDeltarR = 1./dEstRigEff[imr][iver] - 1./ev.GetRigidity(ir);
				if (rigf!=0 && rigu!=0) {
					//flux sam + TkEff sam
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam_xcutoff", ir, tkl1z, imr, iver);
					if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu, dDeltarR);
				
					histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam_xcutoff_nocut", ir, tkl1z, imr, iver);
					if (imr!=2 || ev.IsPassEcal(1)) GetHist(hman1, histn.c_str())->Fill(rigu, dDeltarR);
				
					histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sam_xcutoff", ir, tkl1z, imr, iver);
					if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu, rigu-rigf);
				
					if (rigf_cutoff!=0 && rigu_cutoff!=0) {
						histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam", ir, tkl1z, imr, iver);
						if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, dDeltarR);
				
						histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sam_nocut", ir, tkl1z, imr, iver);
						if (imr!=2 || ev.IsPassEcal(1)) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, dDeltarR);
				
						histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sam", ir, tkl1z, imr, iver);
						if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, rigu_cutoff-rigf_cutoff);
					}
				
					//flux + TkEff sam
					if (bPart && bBeta && bTrack[0] && bTkgeom[0] && (tk_z == tkl1z) && ((bBZSel)?ev.GetTkInQrms()<(0.57+0.1*(tk_z-9)):bCharge[0])) {
						histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel_xcutoff", ir, tkl1z, imr, iver);
						if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu, dDeltarR);
					
						histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel_xcutoff_nocut", ir, tkl1z, imr, iver);
						if (imr!=2 || ev.IsPassEcal(1)) GetHist(hman1, histn.c_str())->Fill(rigu, dDeltarR);
					
						histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sel_xcutoff", ir, tkl1z, imr, iver);
						if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu, rigu-rigf);
					
						if (rigf_cutoff!=0 && rigu_cutoff!=0) {
							histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel", ir, tkl1z, imr, iver);
							if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, dDeltarR);
					
							histn = Form("rRvsR_r%d_q%d_mr%d_v%d_sel_nocut", ir, tkl1z, imr, iver);
							if (imr!=2 || ev.IsPassEcal(1)) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, dDeltarR);
					
							histn = Form("dRvsR_r%d_q%d_mr%d_v%d_sel", ir, tkl1z, imr, iver);
							if (bMethodCut[imr]) GetHist(hman1, histn.c_str())->Fill(rigu_cutoff, rigu_cutoff-rigf_cutoff);
						}
					}
				}
			}
			#endif //RIGRESOLU
			
			#ifdef SUCCESSIVE
			if (bMethodCut[imr]) {
				for (int is=0; is<nSuccTk; is++) {
					bSuccTk = bSuccTk && bTkSucc[is];
					rigf=(bSuccTk)?ev.GetRigidity(ir):0;
					if (rigf==0 && rigu==0) break; //do not continue if both not pass the selection
					histn = Form("rig2D_r%d_q%d_mr%d_s%d_xcutoff", ir, tkl1z, imr, is);
					GetHist(hman1, histn.c_str())->Fill(rigu, rigf);
					
					histn = Form("rig2D_r%d_q%d_mr%d_s%d", ir, tkl1z, imr, is);
					if (bCutoffFlux && bCutoffTkEff) GetHist(hman1, histn.c_str())->Fill(rigu, rigf);
				} //is
			}
			#endif //SUCCESSIVE
			
		} //imr
	} //ir
	return true;
} //SelEffVal
