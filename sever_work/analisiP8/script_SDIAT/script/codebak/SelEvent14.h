#ifndef SELEVENT14_H
#define SELEVENT14_H
//-----------------
//updated from: SelEvent13.h
//updated date: 2019.06.27
//-----------------
//dst version: amsd54n or larger
//-----------------
//2020.12.31
//	1) add new parameters, RIGALGO and RIGPGC, to decide the defalut rigidity algorithm and alignment
//
//2021.09.07
//	1) add new function, Select_BetaCutoff, to do beta cutoff for given mass assumption
//	
//2021.09.17
//	1) add the rich index 
//
//2021.09.29
//	1) add new function, CalCutoffpi, to calculate the IGRF cutoff given the position and direction of the reconstructed track
//	2) add new function, InitCutoffpi, to initialize the cutoffpi
//	3) add new function, GetAMSEventR, to read the corresponding AMSEventR for current event
//
//2021.10.26
//	1) update Select_ChargeV4 to always use old charge calibration for proton and Helium
//
//2021.12.21
//	1) add new function, Select_RichGeom, that only do the geometrical seletion for Rich
//
//2022.05.28
//	1) add new parameter bool bGood to Select_RichGeom, that ddecide whether include good rich and clean ring cut
//
//2022.05.30
//	1) add new function, Select_RichGeomUnbiased, that use the TrTrackR extrapolation to rich plane to decide whether within rich geometry
//
//2022.08.01
//	1) add new function, GetRichVar, to retrive rich variable in order
//	2) add new function, Select_Rich_N1, to apply N-1 rich cut
//
//2022.11.09
//	1) add new member function, AddBranch_OnlyDRRich, and corresponding members
//	2) add new member function, SelectRichDR, that do rich slection base on the different cluster
//	2) add new member function, GetBetaDR, that return corresponding beta with direct- and reflected-only
//
//2023.02.14
//	1) remove ChargeCutV3
//	2) rename Charge v1 as Charge_Q8
//	3) rename Charge cut V4 as Charge Cut
//
//2023.02.16
//	1) add new function, SelectEv(), that call the selection function and store the result into booleans
//
//2023.02.20
//	1) add new function, Select_InnerQRMS, for InnerQ rms cut
//
//2023.02.27
//	1) refine Select_Run and Select_RTI to use QRunManager and QRTIManager when USEQTOOL is on
//
//2023.03.01
//	1) add new function, Select_UnbiasedL1XY and Select_UnbiasedL1Q, for MaxL1Q+Inner geometry
//	2) add new member, dGeoSafety, the safety factor for geomagnetic cutoff
//
//2023.04.14
//	1) add new function, PrimaryUpToTrLayer, to check whether particle stay primary up to givern Tracker layer
//-----------------
#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TF1.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TColor.h>
#include <TString.h>
#include <TObject.h>
#include <TChain.h>

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <bitset> //2019.08.14

//non C++11 treatment for mc mass retrieve //2021.01.19
#include <sstream>

//2023.02.16: move these headfile from analysis6.C to here
#include "HistoMan.C"
#include "QSplineFit.C"
#include "file_io.h"
#include "drawvariable.h"
#include "splineFit3.h" //need to include drawvariable.h //2023.02.16

#ifndef PARA
#define PARA
#include "parameter.h"
#include "bins.h"
//#include "include/ChargeCut.h" //2023.02.14: removed
//#include "include/ChargeCutN2.h"
//#include "include/ChargeCutN3.h"
//2023.02.14: all charge cut range
#include "QSelRange.h"

//2021.09.07
#include "LiBin.h"

//2022.08.16
#include "RichVar.h"

//--
#include "RigToEkn.h" //2020.12.26
//#include "LiBin.h" //2021.09.03
#include "ReadBetaCor.h" //2021.09.06

//2023.03.01 //move from RigReso2.h
#ifdef GBATCH
//#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#include "richrecOff.h"
#include "TrRecon.h"
#include "TrExtAlignDB.h" //TrExtAlignDB::SmearExtAlign();//MC Smear Ext-Layer
#endif //GBATCH

//2023.02.25
#ifdef USEQTOOL
#include "QRunManager2.h"
#include "BadRun.h" //list of bad run for P8
#include "Qreadfile3.C" //functions to deal with runlist
#ifdef GBATCH
#ifdef USEQRTI
#include "QRTIManager.h"
#endif //USEQRTI
//#include "QAnalysis.h"
#endif //GBATCH
#endif //USEQTOOL

#endif //PARA

/*//2020.08.30
#define CORRIGINDST*/

//#define AFSRTI

//2021.10.14
#ifdef ATLAS
string sWDirOnline = "/atlas/users/chenya";
#elif defined BAOBAB
//string sWDirOnline = "/dpnc/beegfs/users/chenya/atlas_user";
string sWDirOnline = "/home/chenyao/"; //2020.10.08
string sAMSRawFileDir = "";
#elif defined LXPLUS
//string sWDirOnline = "/afs/cern.ch/user/c/chenya";
//2022.04.05: add new flag for chenyam
#ifdef MLXPLUS
string sWDirOnline = "/afs/cern.ch/user/c/chenyam";
string sAMSRawFileDir = "/afs/cern.ch/user/c/chenyam/runlist/ISS/B1130/amsroot_list_pass7";
#else
string sWDirOnline = "/afs/cern.ch/user/c/chenya";
//string sAMSRawFileDir = "/eos/ams/Data/AMS02/2018/ISS.B1130/pass7";
string sAMSRawFileDir = "/afs/cern.ch/user/c/chenya/runlist/ISS/B1130/amsroot_list_pass7";
#endif //MLXPLUS

#elif defined SDIAT
string sWDirOnline = "/home/ams/yachen";
//string sAMSRawFileDir = "/lustre/data/ams/Data/AMS02/2018/ISS.B1130/pass7_new";
string sAMSRawFileDir = "/home/ams/yachen/runlist/ISS/B1130/amsroot_list_pass7";
#endif

int ProdVer=-1;
//2019.12.11
int dstver=-1;
int MCQ=-1;
//2020.12.12
int MCMass=-1;
//2020.02.18
bool bECalMIPs=false;

//2019.04.01
const string sVarDis[6] = {"InnerTrk", "ExtTrk", "Tof", "UBExtTrk_TrkCor", "UBTof", "UBExtTrk_TofCor"};
TH1F *h1fVD;
TH2F *h2fVD;
TH2I *h2iVDvsRIG;

//--
// int RIGALGO=0;///choutko
int RIGALGO=1;//GBL
int RIGPGC=2;

//2022.10.01
const int iSpanVar[6] = {1, 2, 4, 3, 5, 6}; //Inner, L1+Inner, L1+Inner+L9 (Force Full Span), Inner+L9, InnerUp, InnerDown

//-- //2023.02.16
bool bSumw2;

//--
vector<int> viQSel;
vector<int>::iterator it;

//--
int iSpanL=1, iSpanU=3;

//--
int MCVer=0;

//--Fiducial volume cut //move into SelEvent14.h //2023.02.16
int iFVTight=2;

//--
//const int NRig=3;
const int NRig=2;

//--
bool bTrig=false, bRun=false, bRTI=false, bRTIdL1L9=false, bPart=false, bBeta=false, bChis=false, bTk2nd=false;
//bool bRig[3], bTrack[3], bTkgeom[3];
//bool bRigEff[3];
//2023.03.10
bool bRig[3][NRig], bTrack[3][NRig], bTkgeom[3];
bool bRigEff[3][NRig];
bool bCharge[3];

//--
double dGeoSafety=1.2; //move definition into SelEvent14.h //2023.02.16

//--
using namespace std;

class SelEvent
{
public:
	//enum QDisId{InnerQ, UTofQ, LTofQ, InnInnerQ, L1Q, L9Q}; //move into QSelRange.h //2023.02.14

//--
public:
	SelEvent();
	void SetBranch(TChain &ch);
	//------
	int AddBranch_Header(TChain &ch);
	int AddBranch_Trigger(TChain &ch);
	int AddBranch_RTI(TChain &ch);
	int AddBranch_Particle(TChain &ch);
	int AddBranch_Trk(TChain &ch);
	int AddBranch_Tof(TChain &ch);
	int AddBranch_Ecal(TChain &ch);
	int AddBranch_EcalMIPs(TChain &ch); //2020.02.18
	int AddBranch_Tk2nd(TChain &ch); //2019.11.07
	int AddBranch_Trd(TChain &ch);
	int AddBranch_L1L9Particle(TChain &ch);
	int AddBranch_MC(TChain &ch);
	int AddBranch_NewTrQ(TChain &ch);
	
	//--Unbiased external tracker hit check
	void AddBranch_UnbiasedHitCheck(TChain &ch);
	
	//--Rich variable
	void AddBranchRich(TChain &ch);
	
	//--external chis obtained by TrFit::GetExtLayerChi2
	int AddBranch_ExtChis(TChain &ch);
	
	//read only-direct and only-reflected hits //2022.11.09
	int AddBranch_OnlyDRRich(TChain &ch);
	
	//-----
	void StoreTree(TTree &otree);
	void SetPassSel(int ipos); //set the iPassSel for my tree, ipos=-1:initial iPassSel to 0
	void FillOutTree(TTree &otree);
	//--reading from pre-selected tree file
	void SetBranch_OutTree(TChain &ch);
	bool PassSelection(int ispan, int opt=0); //if pass selection with pattern ispan, opt=0: normal selection, =1: tracking efficiency sample(not ready yet)
	
	//------read corresponding AMS raw file
	#ifdef GBATCH
	AMSEventR* GetAMSEventR(const bool bForceReGet=false, const int iVerbose=0, const bool bFast=false, const string sAMSRawFileDir0=""); //2022.12.07
	#endif //GBATCH
	
	//------Header
	float GetThetamD() const {return thetam*180/3.141592653;}
	float GetPhimD() const {return phim*180/3.141592653;}
	//2021.04.18
	float GetThetasD() const {return thetas*180/3.141592653;}
	float GetPhisD() const {return phis*180/3.141592653;}
	//int TriggerPeriod() const {return (run<1456507386)?0:1;} //2020.03.11
	int TriggerPeriod() const; //2020.03.13: correctly account for MC
	//2020.06.02
	int RebuildTrig(const int nacc) const;
	
	//------Daq //2019.07.10
	unsigned int GetDaqJinJLength(int iJinJ);
	
	//------Geometry
	int Extrapolate(const float point[3], const float dir[3], float *result, const float z) const;
	
	//------RTI
	#ifdef AFSRTI
	void ProcessTime(unsigned int *ProcTime, TH1F *th, int deg=1, int opt=0); //opt=1: use RTIdL1L9 selection
	void MCutoffDis(unsigned int *ProcTime, TH1F *th, int deg=1, int opt=0); //opt=1: use RTIdL1L9 selection
	#endif //AFSRTI
	
	//------TrTrack
	int HasTkLHitXY(int ilay) const;// 2-XY 1-Y 0-No hits
	inline int GetTkInNHit(int ixy=1,int ispan=1) const;
	inline bool  IsPassTkL(int ilay) const;
	int GetTkNHitF(int ispan=1) const;
	//inline float GetChis(int ispan=1,int ixy=1,int ialgo=0,int ipgc=2) const;//ixy=0:Normal X, =1:Normal Y, =2:ChisQ(not the normal one)
	//inline float GetL1ChisY(int ialgo=0,int ipgc=2) const;
	//inline float GetL9ChisY(int ialgo=0,int ipgc=2) const;
	//2020.12.31
	inline float GetChis(const int ispan=1, const int ixy=1, const int ialgo=RIGALGO, const int ipgc=RIGPGC) const;//ixy=0:Normal X, =1:Normal Y, =2:ChisQ(not the normal one)
	inline float GetL1ChisY(const int ialgo=RIGALGO, const int ipgc=RIGPGC) const;
	inline float GetL9ChisY(const int ialgo=RIGALGO, const int ipgc=RIGPGC) const;
	//2022.10.01
	float GetExtChisDiffRig(const int iext, const int ixy, const int ialgo=RIGALGO, const int ipgc=RIGPGC) const;
	float GetL1L9Chis(const int ixy, const int ialgo=RIGALGO, const int ipgc=RIGPGC) const; //FS - Inner chis //2023.02.27
	float GetExtChisGBL(const int iext, const int ispan, const int ixy, const int ipgc=RIGPGC) const;
	//2022.10.08
	//float GetExtChis(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter) const;
	//float GetExtScatAngle(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter) const;
	//float GetExtScatAngleRatio(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter) const;
	//float GetExtHitRes(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter) const;
	//add parameter, ivcoo, to choose result using different VarCoo //2022.10.15
	//float GetExtChis(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0) const;
	//float GetExtScatAngle(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0) const;
	//float GetExtScatAngleRatio(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0) const;
	//float GetExtHitRes(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0) const;
	//add parameter, irigcal, to choose what pattern use to calculate the rigidity used for VarAlpha //2022.10.23
	float GetExtChis(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0, const int irigcal=0) const;
	float GetExtScatAngle(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0, const int irigcal=0) const;
	float GetExtScatAngleRatio(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0, const int irigcal=0) const;
	float GetExtHitRes(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo=0, const int irigcal=0) const;
	
	int GetTkInZ() const {return int(tk_q[1]+0.5);}
	//--old tracker charge
	float GetTkInQ() const;
	float GetTkInQrms() const {return tk_qrms[1];}
	float GetTkLQ(int ilay) const {return tk_ql[ilay];}
	float GetTkLQXY(int ilay, int ixy) const {return tk_ql2[ilay][ixy];}
	float GetTkLQu(int ilay) const {return ((tk_ql2[ilay][0]>0)?tk_ql2[ilay][0]:tk_ql2[ilay][1]);} //tracker layer Q used: XQ, if XQ=0 then YQ
	float GetTkMeanQSide(int ixy, int sopt=0, int lopt=254) const; //mean Q for certain layers (describe by lopt, in defalue takes 7 inner layer, with lopt 2 to 8 bits filped). sopt=0: mean, =1:truncated mean
	//float GetRigidity(int ispan=0,int ialgo=0,int ipgc=2) const;//ialgo=0 VC, ialgo=1 Alcaraz
	//float GetRigidity(const int ispan=0, const int ialgo=0, const int ipgc=2, const bool bRigCor=true) const;//ialgo=0 VC, ialgo=1 Alcaraz //2020.09.01
	//2020.12.31
	float GetRigidity(const int ispan=0, const int ialgo=RIGALGO, const int ipgc=RIGPGC, const bool bRigCor=true) const;//ialgo=0 VC, ialgo=1 Alcaraz, ialgo=2 Kalman
	bool PassTkL(int ilay, float point[3], float dir[3]) const; //for given start point and dir, calculate whether this track will pass track layer ilay (used for MC geomertical acceptance)
	float GetBinCutoff(int deg=1) const;
	bool FitUsedLayer(int ilay) const {return ((tk_hitc[ilay][0]!=0)||(tk_hitc[ilay][1]!=0)||(tk_hitc[ilay][2]!=0));}
	float CalTrunCharge(int ixy=2, const int opt=0xFC) const; //opt=0xFC=(b)11111100, InnInnerQ, opt=0xFE=(b)=11111110, InnerQ
	float CalTrunCharge2(const int ixy=2, const int ver=0, const int opt=0xFC) const; //opt=0xFC=(b)11111100, InnInnerQ, opt=0xFE=(b)=11111110, InnerQ
	float CalTrunChargeRMS(const int ixy=2, const int ver=0, const int opt=0xFC) const; //opt=0xFC=(b)11111100, InnInnerQ, opt=0xFE=(b)=11111110, InnerQ //2019.09.04
	
	//--Tracker Charge Getter functions, retrieving tracker related Q (normal build and unbiased), able to specify calibration version iVer%3=0: old charge, iVer%3=1: YJ's charge, iVer=2%3: HL's charge
	float GetTkInQNew(int ixy=2, int iVer=0) const;
	float GetTkInQrmsNew(int ixy=2, int iVer=0) const;
	float GetTkInInQNew(int ixy=2, int iVer=0) const;
	float GetTkInInQrmsNew(int ixy=2, int iVer=0) const;
	float GetTkLQNew(int ilay, int ixy, int iVer=0) const;
	float CalLQAsy(int ilay, int iVer=0) const; //2002.08.20
	
	float GetUBExtQTrkCor(int iexl, int ixy=2, int iVer=0) const;
	int GetUBExtQStatusTrkCor(int iexl, int iVer=0) const;
	bool IsUBXYExtQTrkCor(int iexl, int iVer=0) const; //2020.02.18
	float GetUBExtQBetaCor(int iexl, int ixy=2, int iVer=0) const;
	float GetUBExtQBetaMatch(int iexl, int iVer=0) const;
	
	//------Tof
	bool IsGoodTofLQStat(int ilay) const {return (tof_qs/int(pow(10.,3-ilay))%10==1);}
	bool IsGoodTofQUD(int iud) const {return (IsGoodTofLQStat(iud*2) && IsGoodTofLQStat(iud*2+1));}
	int GetToFQlN() const;
	int GetToFQlGoodN() const;
	float GetTOFUDQ(int iud=0, int igeom=2) const; //different tof layer charge for different span
	//float GetTOFUDQ(int iud=0) const; //2023.04.23 //keep the same manner, move to old definition
	float GetTofLQ(int ilay=0, int igeom=2) const; //different tof layer charge for different span
	//2021.02.09
	float GetTOFChis(const int itc=0, const int inorm=0) const; //retrieve the tof chis
	
	//------Ecal
	bool IsPassEcalBeta() const {return (fabs(ec_pos[0])<=ecalxy[0] && fabs(ec_pos[1])<=ecalxy[1]);} //check whehter the beta extrapolation is within the first layer of Ecal
	bool IsPassEcal(int opt=0, int ilay=8, int iecalayer=8) const; //check whehter the tracker track extrapolation from ilay is within the first to iecalayer superlayer of Ecal. opt=0: normal track, =1: unbiased beta track, =2: only require the beta extrapolation to first ecal layer within the ecal 
	bool IsPassEcalLayer(float pos[3], float dir[3], int iecalayer, bool bTight=true) const; //check for given position & direction whether it will pass certain Ecal layer
	bool IsPassEcal2(int opt=0, int ilay=8, int iecalayer=17, bool bTight=true) const; //check whehter the tracker track extrapolation from ilay is within the first to iecalayer of Ecal, use detail Ecal cells coordinates. opt=0: normal track, =1: unbiased beta track, =2: only require the beta extrapolation to first ecal layer within the ecal. bTight: whether exclude the 2 cells at edge
	//2019.05.28
	/*bool EcalMatchness(int opt, int ilay, int iecalayer) const; //opt=0: TrTrack extrapolation pass S1 cell; opt=1: TrTrack extrapolation pass CoG cell; opt=2: TrTrack extrapolation pass S1 & CoG cell
	bool EcalMatchness2(int opt, int is, int ilay, int iecalayer) const; //opt=0: TrTrack extrapolation pass S1 cell; opt=1: TrTrack extrapolation pass CoG cell; opt=2: TrTrack extrapolation pass S1 & CoG cell*/
	bool ECalMIPsSel(int iECalLay=0) const; //2020.02.18
	bool ECalMaxEdepSel(int iECalLay=0) const; //2020.02.19
	
	//------variables distribution //2019.04.01
	//----book
	int BookVarDisInnerTrk(const string sDir, const int QVer=-1);
	int BookVarDisExtTrk(const string sDir, const int QVer=-1);
	int BookVarDisTof(const string sDir); //2019.04.07
	//2019.09.01
	int BookVarDisInnerTrkRIG(const string sDir, const int Charge, const int QVer=-1); //need Charge to decide Rigidity binning
	int BookVarDisExtTrkRIG(const string sDir, const int Charge, const int QVer=-1);
	int BookVarDisTofRIG(const string sDir, const int Charge);
	//2019.04.02: unbiased variable, for efficiencies
	int BookVarDisUBExtTrkTrkCor(const string sDir); //2019.04.07
	int BookVarDisUBTof(const string sDir);
	int BookVarDisUBExtTrkTofCor(const string sDir);
	
	//----store
	int StoreVarDisInnerTrk(const string sDir, const double ww=1., const int QVer=-1); //2019.04.07
	int StoreVarDisExtTrk(const string sDir, const double ww=1., const int QVer=-1);
	int StoreVarDisTof(const string sDir, const double ww=1., const int QVer=-1);
	//2019.09.01
	int StoreVarDisInnerTrkRIG(const string sDir, const int Span, const double ww=1., const int QVer=-1); //need Span to decide the rigidity
	int StoreVarDisExtTrkRIG(const string sDir, const int Span, const double ww=1., const int QVer=-1);
	int StoreVarDisTofRIG(const string sDir, const int Span, const double ww=1., const int QVer=-1);
	//2019.04.02: unbiased variable, for efficiencies
	int StoreVarDisUBExtTrkTrkCor(const string sDir, const double ww=1., const int QVer=-1); //2019.04.07
	int StoreVarDisUBTof(const string sDir, const double ww=1., const int QVer=-1);
	int StoreVarDisUBExtTrkTofCor(const string sDir, const double ww=1., const int QVer=-1);
	
	//------Select
	//--Trigger
	bool Select_Trigger(int opt=0); //0:phys trig, 1:only unbiased trig, 2:unbiased trig (others might flagged)
	//2020.08.12
	bool Select_TriggerRB(const int opt=0, const int inacc=5); //do trigger selection based on rebuilt trigger pattern
	bool Select_TrigUnb(); //2019.06.11: unbiased trigger suggested by QY, which is the reverse of physical trigger
	bool Select_TrigUnb2(); //2019.06.13: unbiased trigger suggested by QY, correct version
	//--RTI
	//#ifdef AMSSOFT
	#ifdef AFSRTI
	bool Select_RTI(AMSSetupR::RTI &a);	//input RTI
	bool Select_RTI();
	bool Select_RTI_dst();
	bool Select_RTIdL1L9(unsigned int t);
	bool Select_RTIdL1L9();
	#else
	bool Select_RTI();
	//bool Select_RTIdL1L9() {return (isreal)?(rticdif[0][1] < 35 && rticdif[1][1] < 45):true;}
	bool Select_RTIdL1L9() {return (isreal && (amsdn%100)<68)?(rticdif[0][1] < 35 && rticdif[1][1] < 45):true;} //2023.05.05
	#endif //AFSRTI
	bool Select_Run(unsigned int r);
	bool Select_Run() {return (Select_Run(run));}
	bool Select_Particle();
	int  Select_Particle_succ(int mode, vector<bool> &vbSel); //mode=0: succ, =1: only 1, =2: N-1
	bool Select_Beta();
	bool Select_TofChis();
	bool Select_Tk2nd();
	bool Select_Tk2nd2(const double dRigCut=0.5); //2019.11.25
	bool Select_Tk2ndRecon(); //2020.03.05
	
	//--geomagnetic cutoff
	//bool Select_Rig(int opt=1, int deg=1, int igeom=2); //opt=0:old methond, =1:new
	//bool Select_Rig(const int opt=1, const int deg=1, const int igeom=2, const double dSafety=1.2); //opt=0:old methond, =1:new //2020.01.28
	//bool Select_Rig(const int opt=1, const int deg=1, const int igeom=2, const double dSafety=1.2, const int ialgo=0, const int ipgc=2); //opt=0:old methond, =1:new //2020.08.24
	//2020.12.31
	bool Select_Rig(const int opt=1, const int deg=1, const int igeom=2, const double dSafety=1.2, const int ialgo=RIGALGO, const int ipgc=RIGPGC); //opt=0:old methond, =1:new
	bool Select_RigN(double cutoffv,TH1 *hev,double margin=1.2, int igeom=2); //rig cutoff using QY's function, inserting TH1F and use TH1:FindBin the correspond bin
	//2021.09.07
	//bool Select_BetaCutoff(const int Z, const int A, const int ibeta, const int opt=1, const int deg=1, const double dSafety=1.2, const bool bVerbose=false); //opt=0:old methond, =1:new
	bool Select_BetaCutoff(const int Z, const int A, const int ibeta, const int opt=1, const int deg=1, const double dSafety=1.2, const bool bMaximumCutoff=true, const bool bVerbose=false); //opt=0:old methond, =1:new //2021.09.27
	//2021.09.29
	bool CalCutoffpi(const bool bVerbose=false);
	bool InitCutoffpi() {bCutoffpi=false; fCutoffpi=0.; return true;};
	
	//--Trd
	//int Select_Trd(const int opt=0, const int ispan=0) const; //opt=0: variable in dst, =1: new variable in Trd dst; ispan=0: maximum span, =1: Inner tracker track
	
	//--Track
	//bool Select_TrTrack(int igeom=2, int chisOpt=111); //igeom=1: l1inner, =2: full span; chisOpt=1: do inner chis cut, =10: L1 chis cut (GetL1ChisY() < 10), =100: do external chis cut (L1Inner of L1InnerL9);
	//bool Select_TrTrack(const int igeom=2, const int chisOpt=111, const int ialgo=0, const int ipgc=2) const; //igeom=1: l1inner, =2: full span; chisOpt=1: do inner chis cut, =10: L1 chis cut (GetL1ChisY() < 10), =100: do external chis cut (L1Inner of L1InnerL9); //2020.08.24
	//2020.12.31
	bool Select_TrTrack(const int igeom=2, const int chisOpt=111, const int ialgo=RIGALGO, const int ipgc=RIGPGC) const; //igeom=1: l1inner, =2: full span; chisOpt=1: do inner chis cut, =10: L1 chis cut (GetL1ChisY() < 10), =100: do external chis cut (L1Inner of L1InnerL9);
	int  Select_TrTrack_succ(int mode, vector<bool> &vbSel, int igeom=2, int chisOpt=111); //mode=0: succ, =1: only 1, =2: N-1
	
	//--Fiducial volume
	bool Select_TkGeom(int igeom=2, int opt=2); //opt=0: only select the no., =1: only select pattern, opt>1: both
	int  Select_TkGeom_succ(int mode, vector<bool> &vbSel, int igeom=2, int optt=2);
	
	//--Charge //old method using range imbede, same a Twiki
	bool Select_ChargeQ8(int igeom=2, int opt=1, bool diffTof=true); //diffTof: using different ToF charge for different span
	bool Select_InnerQ_Q8();
	bool Select_TofQ_Q8(int igeom=2, bool diffTof=true); //igeom=0: true(no selection), 1: UTof, 2: U&LTof, 3: LTof, diffTof: using different ToF charge for different span
	bool Select_ExtQ_Q8(int igeom=2); //igeom=0: true(no selection), 1: L1, 2: L1&L9, 3: L9
	
	//------new charge selection logic
	double GetSubDetQ(const int iqdis, const int iVer=0, const int iUB=0); //iUB=0: normal Q, iUB=1: tracker corrected unbiased external charge, iUB=2: Trd-Build unbiased Beta & external Q corrected by this Beta //2020.01.24: consider add a enumerate for iqdis which will be more readable XXX
	
	/*//------charge cut in twiki, new selection logic //not used anymore
	bool SelectQ_Twiki(const int ich, const int iqdis, const int iVer, const bool bTofLayerCut=false);
	bool Select_InnerQTwiki(const int ich, const int iVer=0);
	bool Select_TofQTwiki(const int ich, int igeom=2, const int iVer=0); //igeom=0: true(no selection), 1: UTof, 2: U&LTof, 3: LTof
	bool Select_ExtQTwiki(const int ich, int igeom=2, const int iVer=0); //igeom=0: true(no selection), 1: L1, 2: L1&L9, 3: L9
	bool Select_ChargeTwiki(const int ich, int igeom=2, const int iVer=0);*/
	
	//------charge cut
	//return charge selection range using GetQSelRangeTwiki for BZ and call GetQSelRange for Q<=8
	////2023.02.14: merging all other GetQSelRange into one, keep updating this function after, keep the definition of the function in SelEvent, which simply calls GetQSelRange defined in QSelRange.h
	//now not depends on iVer any more
	//double GetQSelRange(const int ilu, const int ich, const int iqdis, const int iVer, const bool bL1QUCut=false) const {return QDis::GetQSelRange(ilu, ich, iqdis, (isreal || bL1QUCut));} //defined in QSelRange.h
	//include the tighten directly //2023.07.17
	double GetQSelRange(const int ilu, const int ich, const int iqdis, const int iVer, const bool bL1QUCut=false, const float fTight=1) const;
	
	//--
	bool Select_SubDetQ(const int ich, const int iqdis, const int iVer=0, const float fTight=1., const bool bL1QUCut=false);
	bool Select_InnerQRMS(const int ich, const int iVer=0); //selection for InnerQ rms //2023.02.20
	bool Select_InnerQ(const int ich, const int iVer=0, const float fTight=1.);
	bool Select_TofQ(const int ich, int igeom=2, const int iVer=0, const float fTight=1.); //igeom=0: true(no selection), 1: UTof, 2: U&LTof, 3: LTof
	bool Select_ExtQ(const int ich, int igeom=2, const int iVer=0, const float fTight=1., const bool bL1QUCut=false); //igeom=0: true(no selection), 1: L1, 2: L1&L9, 3: L9 //2019.12.31
	bool Select_Charge(const int ich, int igeom=2, const int iVer=0, const float fTight=1., const bool bL1QUCut=false);
	
	bool Select_QStatus(int igeom=2); //igeom=0: true(no selection), 1: 2 UTof layers & L1q_status, 2: 4 Tof layers & L1q_status
	bool Select_TkLQStatus(int ilay) const {return ((tk_qls[ilay] & 0x10013D)==0);}
	bool Select_WithinTrack(int igeom=2);
	
	//----functions to select unbiased L1 for MaxL1Q+Inner geometry //2023.03.01
	bool Select_UnbiasedL1XY(const int iVer=0) const;
	bool Select_UnbiasedL1Q(const int ich, const int iVer=0) const;
	
	//----
	bool Select_PreSel();
	bool Select_Pattern(int igeom=2, int rig_opt=1, int deg=1, int trackOpt=111, int geom_opt=2);
	
	//------Geometrical acceptance
	bool Select_GeomAccept(int igeom);
	
	//------primary information //2019.10.13
	int CheckPrimary();
	bool PrimaryUpToTrLayer(const int iLayerJ); //check whether particle stay primary up to givern Tracker layer //2023.04.14
	int GetTrMCLZ(const int il); //get TrMCCluster charge //2023.02.19
	
	//------Rich quality cut, 2019.06.02
	float get_betadiff() const {return (tof_betah-rich_beta[0])/rich_beta[0];}
	float get_betadiffB() const {return (tof_betah-rich_betap)/rich_betap;} //2022.02.22
	bool SelectRich() const;
	bool SelectRichQ(const int charge) const {return (sqrt(rich_q[0])>charge-1) && (sqrt(rich_q[0])<charge+1.5);}
	bool SelectRich(const int charge) const {return SelectRich() && SelectRichQ(charge);}
	//--selection of RichRingBR //2022.02.22
	bool SelectRichB();
	bool SelectRichQB(const int charge) {return (rich_qp>charge-1) && (rich_qp<charge+1.5);}
	bool SelectRichB(const int charge) {return SelectRichB() && SelectRichQB(charge);}
	//bool Select_RichGeom(); //2021.12.21
	bool Select_RichGeom(bool bGood=true); //2022.05.28
	bool Select_RichGeomUnbiased(const bool bNaF); //2022.05.30
	//2022.08.01
	float GetRichVar(const int ivar);
	bool Select_Rich_N1(const int icut, const int charge);
	//2022.11.09
	bool SelectRichDR(const int icluster, const int idr, const int charge) const;
	double GetBetaDR(const int icluster, const int idr, const int ibetaver) const;
	
	//------ //2023.04.23
	float GetL1QDisSelVar(const int ivar, const int charge);
	
	//------ //2023.02.16
	void SelectEv();
	
	//------tool function
	bool CheckFlag(int status, int bit);
	int ModeSelection(int mode, vector<bool> &vbSel);
	
	//------use QTool for run mangement, bad run and RTI selection
	//----list of bad run
	#ifdef USEQTOOL
	int SelectGoodRun(int opt=0);
	#endif //USEQTOOL
	
	//----------
	#ifdef GBATCH
	AMSChain amsch;
	static AMSEventR *amsev0;
	#endif //GBATCH
	
	//------Header
	bool isreal;
	unsigned int run;
	unsigned int event;
	int amsdn; //dst version //2023.02.25
//	int run;
//	int event;
	unsigned int time[2]; //0:unix time, 1:usec time
	float thetam;
	float phim;
	//2021.04.18
	float thetas;
	float phis;
	int iPassSel; //StoreTree
	//2019.06.20
	unsigned int daqlen; //DaqEvent::Length
	unsigned int daqjrmerr; //DaqEvent::JRoomError
	unsigned int daqsdlen; //DaqEvent::SDLength()
	unsigned int daqsdlenu; //DaqEvent::SDLengthU()
	//2019.07.10
	unsigned int daqjlen[24]; //DaqEvent::JLength[24]
	
	//------Trigger
	int physbpatt1;
	int jmembpatt1;
	int physbpatt2;
	int jmembpatt2;
	//2020.06.02
	int iTrigSetting=0; //=0: not forcing trigger setting (default), =1: force to have old trigger setting (nacc_thres=5), =2: force to have new trigger setting (nacc_thres=8)
	
	//------RTI
	int irti;
	float zenith;	//zenith<40
	bool issaa;
	float rtilf;	//rtilf>0.5
	float rtinev;
	float rtinerr;
	float rtintrig;
	float rtinpar;
	int rtigood;	//(rtigood&0x3F)==0
	float glat;
	float glong;
	float rticdif[2][3]; 
	
	float mcutoffi[4][2];
	float cutoffpi[2]; //2021.09.27
	//2021.09.29
	bool bCutoffpi;
	float fCutoffpi;
	//2023.02.25
	float rtinexl[2][2];
	
	//------Particle
	int ibetah;
	int itrtrack;
	int ntofclh;
	int nbetah;
	int ntrack;
	int ntrrawcl;
	/*int ntrcl;
	int ntrhit;
	int nparticle;
	int nbeta;
	int ncharge;*/
	int ntrdtrack;
	//int nshow;
	int necalhit;
	int ntrdseg;
	int ntrdrawh;
	int ntrdcl;
	int nrich;
	int nrichb;
	int nrichh;
	#ifdef MVARIABLE
	int ntofclh_d[2];
	int nbetah_d[2];
	#endif //MVARIABLE
	#ifdef TRSTAT
	int trstat;
	bool isPassSelectPart;
	#endif //TRSTAT
	int ishow;
	
	//------Track
	int tk_hitb[2];		//0:X+Y, 1:Y
	float tk_pos[9][3];	//reconstructed tracker position
	float tk_dir[9][3];	//reconstructed tracker direction
	float tk_chis1[3][3][7][3];////Algo+PGCIEMAT+Span+XYXY
	float tk_q[2];		//1:inner, Z-0.45< q_inn < Z+0.45; 0: all
	float tk_qrms[2];		//1:inner rms, q_inn_rms<0.55; 0: all
	float tk_ql[9];		//selection on tk_ql[0] tk_ql[8]:L1 L9
	float tk_ql2[9][2];	//tracker XQ & YQ
	int tk_lid[9];		//tk ladder id
	int tk_cad[9];		//tk cad //1000000000*K7+100000000*Mult+10000*y(address)+x(address)
	int tk_qls[9];		//Q status for each layer
//The meaning of tk_rigidity1 see dst line 2167, and the definition of parameter patt of TrTrackR::iTrTrackPar()
#ifdef USENEWDST
	float tk_rigidity1[3][3][8];//Algo+PGCIEMAT+Span
#else
	float tk_rigidity1[3][3][7];//Algo+PGCIEMAT+Span
#endif
	//2022.10.01
	float tk_ExtChisGBL[2][3][7][2];//L1orL9+PGCIEMAT+Span+XY, only GBL
	//2022.10.08
	float tk_L1ScatAngle[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use L1Inner for L1
	float tk_L1ScatAngleRatio[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use L1Inner for L1
	float tk_L1ExtChis[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use L1Inner for L1
	float tk_L1ExtHitRes[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use L1Inner for L1
	float tk_L9ScatAngle[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use FS for L9
	float tk_L9ScatAngleRatio[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use FS for L9
	float tk_L9ExtChis[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use L1Inner for L1
	float tk_L9ExtHitRes[2][3][2][2][5][2]; //[Algo, =0: Choutko, =1: GBL][PGCIEMAT][XY][method of interpolate][diff value for VarCoo], use L1Inner for L1
	//end 2022.10.08
	//2022.11.09
	float rich_betaDR[2][3];//getbeta+default+refit
	float rich_pbDR[2];//Prob To Ring
	int rich_usedDR[2];
	int rich_usedmDR[2];
	float rich_qDR[2][2];//q2 from ring+q from ChargeR
	float rich_npeDR[2][3];//Collect and expect
	bool rich_goodDR[2];
	bool rich_cleanDR[2];
	int rich_pmtDR[2];
	float rich_PMTChargeConsistencyDR[2];
	float rich_BetaConsistencyDR[2];
	//--rich beta direct-only and reflected-only
	float        rich_betaDROnly[2][3];//[0: direct-only, 1: reflected-only][0: getBeta() (Npe-weighted beta*correction), 1: average beta, 2: Npe-weighted beta]
	//end 2022.11.09
	float tk_rig[3]; //StoreTree, store only 3 patterns rigidity
	float tk_res[4][9][2];//Residual Span+Layer+XY
	float tk_res1[9][2];//Residual Layer+XY
	float tk_hitc[9][3]; //Layer+XYZ
	//--new tracker charge
	float tk_qin[2][3];
	float tk_qrmn[2][3];
	float tk_qln[2][9][3]; //[version:0=YJ,1=HL][ilay][pattern:0=X,1=Y,2=X+Y]
	
	//2020.08.30
	float Rigidity[7]; //corrected rigidity by calling GetCorrectGeomRigidity
	
	int l1i;
	int l9i;
	
	int l1in;
	int l9in;
	float tk_l1qHL;
	float tk_l9qHL;
	
	//------ToF
	float tof_betah;		//checked
	float tof_betahmc;		//checked //2023.01.11
	int tof_btype;		//==1 or < 10 or are the same??? XXX
	float	tof_chist;		//tof chi square time
	float	tof_chisc;		//tof chi square coordinate (with inner Track)
	int tof_hsumh;		//tof cluster no.
	int tof_hsumhu;		//tof cluster no. (used to fit Beta)
	float tof_ql[4];		//tof layer charge, used to calculate utofQ and ltofQ //Z-0.6 < q_utof < Z+1.5	//different one compare with twiki
	int tof_qs;			//tof charge state, 1111:all layer are valid, 0011:layer 2,3 are valid //REMAIN: these are not binary
	float tof_oq[4][2];	//highest and 2nd highest non ibetah tof bar charge //2020.03.02
	int tof_ob[4][2]; //2023.04.23
	//2021.02.09: add normalized tof chis between tof and tracker matchness
	float tof_chist_n;	//normalized time chis
	float tof_chisc_n;	//normalized coordinate chis
	//2023.04.23: variable to clean up L1Q dis from QY
	int tof_nclhl[4];
	int tof_barid[4];
	float tof_pos[4][3]; // TOF pos

	
	//------Ecal
	float ecal_en[4];		//EnergyE EnergyA EnergyD EnergyC
	float ec_pos[3];
	float ecal_q;
	//2019.05.12
	int nshow;
	int ecal_nhit[18];	//Hit Number for each Layer
	float ecal_el[18];	//Energy for each Layer
	float ecal_eh[18];	//Max Cell Energy for each Layer
	//2020.02.18
	float ecal_cog[18];	//deposited energy weighted center of gravity for ECal layer: SUM(icell*edep)/ecal_el2 
	int ecal_icellmax[18];	//maximum deposited energy cell id
	float ecal_S3[18];	//deposited energy for maximum cell + 2 nearby cell (1 on left, 1 on right)
	float ecal_S5[18];	//deposited energy for maximum cell + 4 nearby cell (2 on left, 2 on right)
	
	//------TrdTrack
	int nitrdseg;
	float distrd[3];///0x ,1y, 2theta
	float trd_rq;
	int trd_rz;
	float trd_rpz;
	int trd_pass;
	int trd_statk;
	int trd_nhitk;
	float trd_lik[3];//e/p e/He p/He
	float trd_like[3];
	int trd_onhitk;
	float trd_oampk;
	float trd_qk[5];//A+U+D+Non-Dedx
	int trd_qnhk[3];
	float trd_ipch;
	float trd_qrmsk[3];
	float trd_amplk[20];//raw amps (ADC-chan)
	float trd_pathlk[20];
	float trd_ampsk;//tot.adc, path_length corrected
	int trd_amphitk;//good hits for above
	float trd_pathsk;//tot path_length
	
	//------Anti
	int anti_nhit;
	int antipatt;
	
	//------secondary tracks cuts //2019.11.07: adding back
	int betah2hb[2];
	float betah2q;
	float betah2r;
	
	//------L1L9Particle
	float tk_l1q;
	float tk_l9q;
	float tk_l1qxy[2];
	float tk_l9qxy[2];
	int tk_l1qs;
	int tk_l9qs;
	int ibetahs;
	int itrdtracks;
	int tof_nhits;
	int tof_hsumhus;
	float betahs;
	float tof_chiscs;
	float tof_chists;
	float tof_qls[4];
	float trd_rqs;
	float ecal_pos[3];
	float ecal_ens;
	float ecal_dis;
	float tk_l1mds;
	float tk_pos1s[9][3];
	float tk_pos2s[9][3];
	float tk_dir1s[3];
	float tk_l1qvs;
	float tk_l9qvs;
	float tk_l1qvr;
	float tk_exql[2][3];
	int tk_exqls[2];
	int tk_exlid[2]; //2019.08.17
	int tk_excad[2];
	//new unbiased Tracker Q in QY's dst, dst version > 54 //2019.04.30
	float tk_exqln[2][2][3];//YJ+LU, unbiased Tracker Q Correct by Inner Tracker Track
	float tk_exqvn[2][2];//YJ+HL, unbiased Tracker Q Correct by unbiased Beta track
	
	//------MC
	float mmom;
	float mch;
	int mpar;
	int mtof_pass;
	float mevcoo[3];
	float mevdir[3];
	float mevcoo1[21][3]; //2019.10.13
	float mevmom1[21]; //2021.09.06
	//MC truth of secondary, 0: maximum momentum secondary, 1: maximum charge secondary //2019.10.30
	float mpare[2];	//momentum
	int mparp[2];	//particle id
	float mparc[2];	//charge
	//TrMCCluster with maximum momentum for each tracker layer //2019.11.06
	float mtrmom[9];	//maximum momentum
	int mtrpar[9];	//particle id
	float mtrcoo[9][3];	//step start point
	float mtrcoo1[9][3];	//mid point 
	int mtrpri[9];	//is primary
	int mtrz[9];	//integral charge in binary, retrieve by mtrz[il]&0x3f, see TrMCCluster::getZ()
	
	//------rich, 2019.06.02
	int irich;
	bool rich_good;
	bool rich_clean;
	float rich_beta[3];//getbeta+default+refit
	float rich_betap;
	float rich_pb;//Prob To Ring
	int rich_used; //ring->getUsedHits(); //2022.09.21
	int rich_usedm; //ring->getReflectedHits(); //2022.09.21
	float rich_npe[3];//Collect and expect
	bool rich_NaF;
	int rich_pmt;
	float rich_PMTChargeConsistency;
	float rich_BetaConsistency;
	float rich_pos[3];
	float rich_theta;
	float rich_phi;
	int rich_tile;
	float rich_q[2];//q2 from ring+q from ChargeR
	float rich_qp;
	//2021.09.17
	float rich_n[2]; //refractive index without and with correction
	//RichRingBR //2022.02.22
	float rich_pbp;
	
	//------
	//2019.04.07
	TFile *fOutput;
	
	//------tool function
	vector<bool> vbAllSel;
	
	//------QRunManager //2023.02.25
	#ifdef USEQTOOL
	QRunManager *qrm;
	#endif //USEQTOOL
};

#ifdef GBATCH
AMSEventR *SelEvent::amsev0 = NULL;
#endif //GBATCH

//----
SelEvent::SelEvent()
{
	//2023.02.25
	#ifdef USEQTOOL
	qrm = new QRunManager();
	#endif //USEQTOOL
}

/*SelEvent::~SelEvent()
{
	//2023.02.25
	#ifdef USEQTOOL
	delete qrm;
	#endif //USEQTOOL
}*/

//------
int SelEvent::AddBranch_Header(TChain &ch)
{
	ch.SetBranchAddress("isreal", &isreal);
	ch.SetBranchAddress("run", &run);
	ch.SetBranchAddress("event", &event);
	ch.SetBranchAddress("amsdn", &amsdn); //2023.02.25
	ch.SetBranchAddress("time", time);
	// ch.SetBranchAddress("thetam", &thetam);
	// ch.SetBranchAddress("phim", &phim);
	//2021.04.18
	// ch.SetBranchAddress("thetas", &thetas);
	// ch.SetBranchAddress("phis", &phis);
	
	ch.SetBranchAddress("daqlen", &daqlen);
	ch.SetBranchAddress("daqjrmerr", &daqjrmerr);
	ch.SetBranchAddress("daqsdlen", &daqsdlen);
	ch.SetBranchAddress("daqsdlenu", &daqsdlenu);
	ch.SetBranchAddress("daqjlen", daqjlen);
	return 1;
}

int SelEvent::AddBranch_Trigger(TChain &ch)
{
	ch.SetBranchAddress("physbpatt1", &physbpatt1);
	ch.SetBranchAddress("jmembpatt1", &jmembpatt1);
	ch.SetBranchAddress("physbpatt2", &physbpatt2);
	ch.SetBranchAddress("jmembpatt2", &jmembpatt2);
	ch.SetBranchAddress("anti_nhit", &anti_nhit);
	ch.SetBranchAddress("antipatt", &antipatt);
	return 1;
}

int SelEvent::AddBranch_RTI(TChain &ch)
{
	ch.SetBranchAddress("irti", &irti);
	ch.SetBranchAddress("zenith", &zenith);
	ch.SetBranchAddress("issaa", &issaa);
	ch.SetBranchAddress("rtilf", &rtilf);
	ch.SetBranchAddress("rtinev", &rtinev);
	ch.SetBranchAddress("rtinerr", &rtinerr);
	ch.SetBranchAddress("rtintrig", &rtintrig);
	ch.SetBranchAddress("rtinpar", &rtinpar);
	ch.SetBranchAddress("rtigood", &rtigood);
	ch.SetBranchAddress("glat", &glat);
	ch.SetBranchAddress("glong", &glong);
	ch.SetBranchAddress("mcutoffi", mcutoffi);
	ch.SetBranchAddress("cutoffpi", cutoffpi); //2021.09.27
	ch.SetBranchAddress("rticdif", rticdif);
	return 1;
}

int SelEvent::AddBranch_Particle(TChain &ch)
{
	ch.SetBranchAddress("ibetah", &ibetah);
	ch.SetBranchAddress("itrtrack", &itrtrack);
	// ch.SetBranchAddress("ntofclh", &ntofclh);
	// ch.SetBranchAddress("nbetah", &nbetah);
	ch.SetBranchAddress("ntrack", &ntrack);
	// ch.SetBranchAddress("ntrrawcl", &ntrrawcl);
	/*ch.SetBranchAddress("ntrcl", &ntrcl);
	ch.SetBranchAddress("ntrhit", &ntrhit);
	
	ch.SetBranchAddress("nparticle", &nparticle);
	ch.SetBranchAddress("nbeta", &nbeta);
	ch.SetBranchAddress("ncharge", &ncharge);*/
	// ch.SetBranchAddress("ntrdtrack", &ntrdtrack);
	//ch.SetBranchAddress("nshow", &nshow);
	// ch.SetBranchAddress("necalhit", &necalhit);
	// ch.SetBranchAddress("ntrdseg", &ntrdseg);
	// ch.SetBranchAddress("ntrdrawh", &ntrdrawh);
	ch.SetBranchAddress("ntrdcl", &ntrdcl);
	ch.SetBranchAddress("nrich", &nrich);
	ch.SetBranchAddress("nrichb", &nrichb);
	ch.SetBranchAddress("nrichh", &nrichh);
	#ifdef MVARIABLE
	ch.SetBranchAddress("ntofclh_d", ntofclh_d);
	ch.SetBranchAddress("nbetah_d", nbetah_d);
	#endif //MVARIABLE
	#ifdef TRSTAT
	ch.SetBranchAddress("trstat", &trstat);
	ch.SetBranchAddress("isPassSelectPart", &isPassSelectPart);
	#endif //TRSTAT
	// ch.SetBranchAddress("ishow", &ishow);
	
	return 1;
}

int SelEvent::AddBranch_Trk(TChain &ch)
{
	ch.SetBranchAddress("tk_hitb", tk_hitb);
	ch.SetBranchAddress("tk_pos", tk_pos);
	ch.SetBranchAddress("tk_dir", tk_dir);
	ch.SetBranchAddress("tk_chis1", tk_chis1);
	ch.SetBranchAddress("tk_q", tk_q);
	ch.SetBranchAddress("tk_qrms", tk_qrms);
	ch.SetBranchAddress("tk_ql", tk_ql);
	ch.SetBranchAddress("tk_ql2", tk_ql2);
	//ch.SetBranchAddress("tk_l1qs", &tk_l1qs);
	ch.SetBranchAddress("tk_lid", tk_lid);
	// ch.SetBranchAddress("tk_cad", tk_cad);
	ch.SetBranchAddress("tk_qls", tk_qls);
	ch.SetBranchAddress("tk_rigidity1", tk_rigidity1);
	// ch.SetBranchAddress("tk_res", tk_res);
	// ch.SetBranchAddress("tk_res1", tk_res1);
	// ch.SetBranchAddress("tk_hitc", tk_hitc);
	//2020.08.30
	#ifdef READRIGINDST //2020.09.01
	ch.SetBranchAddress("Rigidity", Rigidity);
	#endif //READRIGINDST
	return 1;
}

int SelEvent::AddBranch_Tof(TChain &ch)
{
	ch.SetBranchAddress("tof_betah", &tof_betah);
	ch.SetBranchAddress("tof_betahmc", &tof_betahmc); //2023.01.11: beta from MC truth time measurement, better agreement to data
	ch.SetBranchAddress("tof_btype", &tof_btype);
	ch.SetBranchAddress("tof_chist", &tof_chist);		//time chis
	ch.SetBranchAddress("tof_chisc", &tof_chisc);		//coordinate chis
	ch.SetBranchAddress("tof_hsumh", &tof_hsumh);
	ch.SetBranchAddress("tof_hsumhu", &tof_hsumhu);
	ch.SetBranchAddress("tof_ql", tof_ql);
	ch.SetBranchAddress("tof_qs", &tof_qs);
	// ch.SetBranchAddress("tof_oq", tof_oq); //2020.03.02
	// ch.SetBranchAddress("tof_ob", tof_ob); //2023.04.23
	//2021.02.09
	ch.SetBranchAddress("tof_chist_n", &tof_chist_n);	//normalized time chis
	ch.SetBranchAddress("tof_chisc_n", &tof_chisc_n);	//normalized coordinate chis
	//2023.04.23: variable to clean up L1Q dis from QY
	// ch.SetBranchAddress("tof_nclhl", tof_nclhl);
	ch.SetBranchAddress("tof_barid", tof_barid);
	// ch.SetBranchAddress("tof_pos", tof_pos);
	return 1;
}

int SelEvent::AddBranch_Ecal(TChain &ch)
{
	ch.SetBranchAddress("ecal_en", ecal_en); //EnergyE EnergyA EnergyD EnergyC
	ch.SetBranchAddress("ec_pos", ec_pos);
	ch.SetBranchAddress("ecal_q", &ecal_q);
	
	ch.SetBranchAddress("nshow", &nshow);
	ch.SetBranchAddress("ecal_nhit", ecal_nhit); //Hit Number for each Layer
	ch.SetBranchAddress("ecal_el", ecal_el); //Energy for each Layer
	ch.SetBranchAddress("ecal_eh", ecal_eh); //Max Cell Energy for each Layer
	return 1;
}

//2020.02.18
int SelEvent::AddBranch_EcalMIPs(TChain &ch)
{
	ch.SetBranchAddress("ecal_cog", ecal_cog);
	ch.SetBranchAddress("ecal_icellmax", ecal_icellmax);
	ch.SetBranchAddress("ecal_S3", ecal_S3);
	ch.SetBranchAddress("ecal_S5", ecal_S5);
	return 1;
}

int SelEvent::AddBranch_Tk2nd(TChain &ch) //2019.11.07: adding back
{
	ch.SetBranchAddress("betah2hb", betah2hb);
	ch.SetBranchAddress("betah2q", &betah2q);
	ch.SetBranchAddress("betah2r", &betah2r);
	return 1;
}

int SelEvent::AddBranch_Trd(TChain &ch){
	ch.SetBranchAddress("nitrdseg", &nitrdseg);
	ch.SetBranchAddress("distrd", distrd);
	ch.SetBranchAddress("trd_rq", &trd_rq);
	ch.SetBranchAddress("trd_rz", &trd_rz);
	ch.SetBranchAddress("trd_rpz", &trd_rpz);
	ch.SetBranchAddress("trd_pass", &trd_pass);
	ch.SetBranchAddress("trd_statk", &trd_statk);
	ch.SetBranchAddress("trd_nhitk", &trd_nhitk);
	ch.SetBranchAddress("trd_lik", trd_lik);
	ch.SetBranchAddress("trd_like", trd_like);
	ch.SetBranchAddress("trd_onhitk", &trd_onhitk);
	ch.SetBranchAddress("trd_oampk", &trd_oampk);
	ch.SetBranchAddress("trd_qk", trd_qk);
	ch.SetBranchAddress("trd_qnhk", trd_qnhk);
	ch.SetBranchAddress("trd_ipch", &trd_ipch);
	ch.SetBranchAddress("trd_qrmsk", trd_qrmsk);
	ch.SetBranchAddress("trd_amplk", trd_amplk);
	ch.SetBranchAddress("trd_pathlk", trd_pathlk);
	ch.SetBranchAddress("trd_ampsk", &trd_ampsk);
	ch.SetBranchAddress("trd_amphitk", &trd_amphitk);
	ch.SetBranchAddress("trd_pathsk", &trd_pathsk);
	return 1;
}

int SelEvent::AddBranch_L1L9Particle(TChain &ch){
	ch.SetBranchAddress("tk_l1q", &tk_l1q);
	ch.SetBranchAddress("tk_l9q", &tk_l9q);
	ch.SetBranchAddress("tk_l1qxy", tk_l1qxy);
	ch.SetBranchAddress("tk_l9qxy", tk_l9qxy);
	ch.SetBranchAddress("tk_l1qs", &tk_l1qs);
	ch.SetBranchAddress("tk_l9qs", &tk_l9qs);
	ch.SetBranchAddress("ibetahs", &ibetahs);
	ch.SetBranchAddress("itrdtracks", &itrdtracks);
	ch.SetBranchAddress("tof_nhits", &tof_nhits);
	ch.SetBranchAddress("betahs", &betahs);
	ch.SetBranchAddress("tof_chiscs", &tof_chiscs);
	ch.SetBranchAddress("tof_chists", &tof_chists);
	ch.SetBranchAddress("tof_qls", tof_qls);
	ch.SetBranchAddress("trd_rqs", &trd_rqs);
	ch.SetBranchAddress("ecal_pos", ecal_pos);
	ch.SetBranchAddress("ecal_ens", &ecal_ens);
	ch.SetBranchAddress("ecal_dis", &ecal_dis);
	ch.SetBranchAddress("tk_l1mds", &tk_l1mds);
	ch.SetBranchAddress("tk_pos1s", tk_pos1s);
	// ch.SetBranchAddress("tk_pos2s", tk_pos2s);
	ch.SetBranchAddress("tk_dir1s", tk_dir1s);
	ch.SetBranchAddress("tk_l1qvs", &tk_l1qvs);
	ch.SetBranchAddress("tk_l9qvs", &tk_l9qvs);
	// ch.SetBranchAddress("tk_l1qvr", &tk_l1qvr);
	ch.SetBranchAddress("tk_exql", &tk_exql);
	ch.SetBranchAddress("tk_exqls", &tk_exqls);
	ch.SetBranchAddress("tk_exlid", &tk_exlid); //2019.08.17
	// ch.SetBranchAddress("tk_excad", &tk_excad);
	//2019.04.30
	ch.SetBranchAddress("tk_exqln", &tk_exqln); //unbiased Tracker Q Correct by Inner Tracker Track
	ch.SetBranchAddress("tk_exqvn", &tk_exqvn); //unbiased Tracker Q Correct by unbiased Beta track
	return 1;
}

int SelEvent::AddBranch_MC(TChain &ch){
	ch.SetBranchAddress("mmom", &mmom);
	ch.SetBranchAddress("mch", &mch);
	ch.SetBranchAddress("mpar", &mpar);
	ch.SetBranchAddress("mtof_pass", &mtof_pass);
	ch.SetBranchAddress("mevcoo", mevcoo);
	ch.SetBranchAddress("mevdir", mevdir);
	ch.SetBranchAddress("mevcoo1", mevcoo1); //2019.10.13
	ch.SetBranchAddress("mevmom1", mevmom1); //2021.09.06
	//2019.10.30
	ch.SetBranchAddress("mpare", mpare);
	ch.SetBranchAddress("mparp", mparp);
	ch.SetBranchAddress("mparc", mparc);
	//2019.11.06
	ch.SetBranchAddress("mtrmom", mtrmom);
	ch.SetBranchAddress("mtrcoo", mtrcoo);
	ch.SetBranchAddress("mtrcoo1", mtrcoo1);
	ch.SetBranchAddress("mtrpar", mtrpar);
	ch.SetBranchAddress("mtrpri", mtrpri);
	ch.SetBranchAddress("mtrz", mtrz);
	return 1;
}

int SelEvent::AddBranch_NewTrQ(TChain &ch){
	ch.SetBranchAddress("tk_qin", &tk_qin);
	ch.SetBranchAddress("tk_qrmn", &tk_qrmn);
	ch.SetBranchAddress("tk_qln", &tk_qln);
	return 1;
}

void SelEvent::AddBranch_UnbiasedHitCheck(TChain &ch)
{
	//--Unbiased tracker hit based on old charge
	ch.SetBranchAddress("l1i", &l1i);
	ch.SetBranchAddress("l9i", &l9i);
	
	//--Unbiased tracker hit based on new charge
	ch.SetBranchAddress("l1in", &l1in);
	ch.SetBranchAddress("l9in", &l9in);
	ch.SetBranchAddress("tk_l1qHL", &tk_l1qHL);
	ch.SetBranchAddress("tk_l9qHL", &tk_l9qHL);
}

void SelEvent::AddBranchRich(TChain &ch)
{
	ch.SetBranchAddress("irich", &irich);
	ch.SetBranchAddress("rich_good", &rich_good);
	ch.SetBranchAddress("rich_clean", &rich_clean);
	ch.SetBranchAddress("rich_beta", rich_beta);
	ch.SetBranchAddress("rich_betap", &rich_betap);
	ch.SetBranchAddress("rich_used", &rich_used); //2022.09.21
	ch.SetBranchAddress("rich_usedm", &rich_usedm); //2022.09.22
	ch.SetBranchAddress("rich_pb", &rich_pb);
	ch.SetBranchAddress("rich_npe", rich_npe);
	ch.SetBranchAddress("rich_NaF", &rich_NaF);
	ch.SetBranchAddress("rich_pmt", &rich_pmt);
	ch.SetBranchAddress("rich_PMTChargeConsistency", &rich_PMTChargeConsistency);
	ch.SetBranchAddress("rich_BetaConsistency", &rich_BetaConsistency);
	ch.SetBranchAddress("rich_pos", rich_pos);
	ch.SetBranchAddress("rich_theta", &rich_theta);
	ch.SetBranchAddress("rich_phi", &rich_phi);
	ch.SetBranchAddress("rich_tile", &rich_tile);
	ch.SetBranchAddress("rich_q", rich_q);
	ch.SetBranchAddress("rich_qp", &rich_qp);
	//2021.09.17
	ch.SetBranchAddress("rich_n", &rich_n);
	//RichRingBR //2022.02.22
	ch.SetBranchAddress("rich_pbp", &rich_pbp);
}

//2022.10.01
int SelEvent::AddBranch_ExtChis(TChain &ch)
{
	ch.SetBranchAddress("tk_ExtChisGBL", tk_ExtChisGBL);
	//2022.10.08
	ch.SetBranchAddress("tk_L1ScatAngle", tk_L1ScatAngle);
	ch.SetBranchAddress("tk_L1ScatAngleRatio", tk_L1ScatAngleRatio);
	ch.SetBranchAddress("tk_L1ExtChis", tk_L1ExtChis);
	ch.SetBranchAddress("tk_L1ExtHitRes", tk_L1ExtHitRes);
	ch.SetBranchAddress("tk_L9ScatAngle", tk_L9ScatAngle);
	ch.SetBranchAddress("tk_L9ScatAngleRatio", tk_L9ScatAngleRatio);
	ch.SetBranchAddress("tk_L9ExtChis", tk_L9ExtChis);
	ch.SetBranchAddress("tk_L9ExtHitRes", tk_L9ExtHitRes);
	return 1;
}

//2022.11.09
int SelEvent::AddBranch_OnlyDRRich(TChain &ch)
{
	ch.SetBranchAddress("rich_betaDR", rich_betaDR);
	ch.SetBranchAddress("rich_pbDR", rich_pbDR);
	ch.SetBranchAddress("rich_usedDR", rich_usedDR);
	ch.SetBranchAddress("rich_usedmDR", rich_usedmDR);
	ch.SetBranchAddress("rich_qDR", rich_qDR);
	ch.SetBranchAddress("rich_npeDR", rich_npeDR);
	ch.SetBranchAddress("rich_goodDR", rich_goodDR);
	ch.SetBranchAddress("rich_cleanDR", rich_cleanDR);
	ch.SetBranchAddress("rich_pmtDR", rich_pmtDR);
	ch.SetBranchAddress("rich_PMTChargeConsistencyDR", rich_PMTChargeConsistencyDR);
	ch.SetBranchAddress("rich_BetaConsistencyDR", rich_BetaConsistencyDR);
	//--rich beta direct-only and reflected-only
	ch.SetBranchAddress("rich_betaDROnly", rich_betaDROnly);
	return 1;
}

void SelEvent::SetBranch(TChain &ch){
	AddBranch_Header(ch);
	AddBranch_Trigger(ch);
	AddBranch_RTI(ch);
	AddBranch_Particle(ch);
	AddBranch_Trk(ch);
	AddBranch_Tof(ch);
	AddBranch_Tk2nd(ch); //2019.11.07
	AddBranch_Ecal(ch);
	AddBranch_L1L9Particle(ch);
	//AddBranchRich(ch); //2019.06.02 //2019.07.01: comment out
	
	//2023.02.25
	ch.GetEntry(0);
	
	//new rti variable for PASS8, only for data (the dst for MC created earlier, doesn't have this variable yet)
	ch.GetEntry(0);
	if ((amsdn%100)>=68 && isreal) ch.SetBranchAddress("rtinexl", rtinexl);
	
	#ifdef USEQTOOL
	qrm->qisreal=isreal?1:0;//ISS or MC 
	#endif //USEQTOOL
}

void SelEvent::StoreTree(TTree &otree)
{
	//header
	otree.Branch("iPassSel", &iPassSel, "iPassSel/I");
	otree.Branch("isreal", &isreal, "isreal/O");
	otree.Branch("run", &run, "run/U");
	otree.Branch("event", &event, "event/U");
	
	//particle
	otree.Branch("ibetah", &ibetah, "ibetah/I");
	otree.Branch("itrtrack", &itrtrack, "itrtrack/I");
	
	//tracker
	otree.Branch("tk_q", tk_q, "tk_q[2]/F");
	otree.Branch("tk_qrms", tk_qrms, "tk_qrms[2]/F");
	otree.Branch("tk_ql", tk_ql, "tk_ql[9]/F");
	otree.Branch("tk_qls", tk_qls, "tk_qls[9]/I");
	otree.Branch("tk_ql2", tk_ql2, "tk_ql2[9][2]/F");
	otree.Branch("tk_rig", tk_rig, "tk_rig[3]/F");
	
	//Tof
	otree.Branch("tof_ql", tof_ql, "tof_ql[4]/F");
	//Ecal
	otree.Branch("ecal_en", ecal_en, "ecal_en[4]/F");
	
//	//unbiased Tof
//	otree.Branch("tof_qls", tof_qls, "tof_qls[4]/F");
}

void SelEvent::SetPassSel(int ipos)
{
	if (ipos==-1) iPassSel=0;
	else if (ipos>=0) iPassSel = iPassSel|(1<<ipos);
//	if (ipos!=-1) {cout << "Passed. ipos=" << ipos << " ";
//	cout << "iPassSel=" << iPassSel << endl;}
}

void SelEvent::FillOutTree(TTree &otree)
{
	if (iPassSel>0) {
		for (int ir=0; ir<3; ir++) tk_rig[ir] = GetRigidity(ir);
		otree.Fill();
	}
}

//reading from pre-selected tree file
void SelEvent::SetBranch_OutTree(TChain &ch)
{
	//header
	ch.SetBranchAddress("iPassSel", &iPassSel);
	ch.SetBranchAddress("isreal", &isreal);
	ch.SetBranchAddress("run", &run);
	ch.SetBranchAddress("event", &event);
	
	//particle
	ch.SetBranchAddress("ibetah", &ibetah);
	ch.SetBranchAddress("itrtrack", &itrtrack);
	
	//tracker
	ch.SetBranchAddress("tk_q", tk_q);
	ch.SetBranchAddress("tk_qrms", tk_qrms);
	ch.SetBranchAddress("tk_ql", tk_ql);
	ch.SetBranchAddress("tk_ql2", tk_ql2);
	ch.SetBranchAddress("tk_qls", tk_qls);
	
	//Tof
	ch.SetBranchAddress("tof_ql", tof_ql);
	
//	//unbiased Tof
//	ch.SetBranchAddress("tof_qls", tof_qls);
}

bool SelEvent::PassSelection(int ispan, int iopt)
{
	bool bPass=false;
	if (iopt==0) bPass=((iPassSel&(1<<ispan))!=0);
	
	return bPass;
}

//------read corresponding AMS raw file
#ifdef GBATCH
//AMSEventR* SelEvent::GetAMSEventR(const bool bVerbose)
//AMSEventR* SelEvent::GetAMSEventR(const bool bForceReGet, const bool bVerbose)
//AMSEventR* SelEvent::GetAMSEventR(const bool bForceReGet, const bool bVerbose, const bool bFast)
//AMSEventR* SelEvent::GetAMSEventR(const bool bForceReGet, const bool bVerbose, const bool bFast, const string sAMSRawFileDir0) //2022.09.22
AMSEventR* SelEvent::GetAMSEventR(const bool bForceReGet, const int iVerbose, const bool bFast, const string sAMSRawFileDir0) //2022.12.07
{
	//AMSEventR *amsev=NULL;
	
	//------build the AMSChain
	//#ifdef SDIAT
	//string sFile = Form("%s/%d/%d*.root", sAMSRawFileDir.c_str(), run/100000, run);
	//#else
	//string sFile = Form("%s/%d*.root", sAMSRawFileDir.c_str(), run);
	//#endif
	//--enable to run MC //2022.09.22
	string sAMSRawDir = sAMSRawFileDir;
	if (sAMSRawFileDir0!="") sAMSRawDir = sAMSRawFileDir0;
	
	time_t now;
	bool bReset=false;
	if (amsch.GetEntries()<=0) bReset=true;
	//--check if already get the AMSEventR
	//if (amsev0!=NULL && run==amsev0->Run() && event==amsev0->Event()) amsev = amsev0;
	//if (amsev0!=NULL && run==amsev0->Run() && event==amsev0->Event()) return amsev0;
	if (!bForceReGet && amsev0!=NULL && run==amsev0->Run() && event==amsev0->Event()) return amsev0; //2022.05.24
	//if (iVerbose>0) {now = std::time(NULL); cout << " *** start getting event from ams root file: run=" << run << " event=" << event << " ***" << ctime(&now) << endl;}
	//else amsev0 = amsch.GetEvent(run, event);
	else //2022.08.31
	{
		if (bFast) amsev0 = amsch.GetEventFast(run, event);
		else amsev0 = amsch.GetEvent(run, event);
	}
	//if (iVerbose>0) {now = std::time(NULL); cout << " *** finish getting event from ams root file: run=" << run << " event=" << event << " ***" << ctime(&now) << endl;}
	//else amsev0 = amsch.GetEventFast(run, event); //XXX
	if (amsev0==NULL) bReset=true;
	
	if (bReset)
	{
		amsch.Reset();
		//amsch.Add(sFile.c_str());
		if (iVerbose==2) {now = std::time(NULL);cout << " *** start adding file to ams chain: ***" << ctime(&now) << endl;}
		//amsch.AddFromFile(sAMSRawFileDir.c_str(), 0, INT_MAX, -1, 0, Form("%d", run)); //should call AddFromFile give specific pattern
		amsch.AddFromFile(sAMSRawDir.c_str(), 0, INT_MAX, -1, 0, Form("%d", run)); //should call AddFromFile give specific pattern //2022.09.22
		//amsch.AddFromFile(sAMSRawDir.c_str(), 0, INT_MAX, -1, 1, Form("%d", run)); //now set a timeout //2022.12.07
		if (iVerbose==2) {now = std::time(NULL);cout << " *** finish adding file to ams chain: ***" << ctime(&now) << endl;}
		
		//if (iVerbose>0) {now = std::time(NULL);cout << " *** start getting event from ams root file: ***" << ctime(&now) << endl;}
		//amsev0 = amsch.GetEvent(run, event);
		//2022.08.31
		if (bFast) amsev0 = amsch.GetEventFast(run, event);
		else amsev0 = amsch.GetEvent(run, event);
		now = std::time(NULL);
		//if (iVerbose>0) {now = std::time(NULL);cout << " *** finish getting event from ams root file: ***" << ctime(&now) << endl;}
		
		if (iVerbose>0)
		{
			cout << " ****** SelEvent::GetAMSEventR: added ams root file: run=" << run << " event=" << event << " ****** " << endl;
			//cout << sAMSRawFileDir << endl;
			cout << sAMSRawDir << endl; //2022.09.22
			//cout << sFile << endl;
			cout << amsch.GetEntries() << endl;
			cout << "run=" << run << " event=" << event << endl;
		}
	}
	
	//if (amsev0==NULL) cout << "run=" << run << " event=" << event << " AMSRootfile=" << sFile << " : cannot found the event" << endl;
	if (amsev0==NULL) cout << "run=" << run << " event=" << event << " : cannot found the event" << endl;
	//------
	return amsev0;
}
#endif //GBATCH

//------header
int SelEvent::TriggerPeriod() const
{
	//int Period=(run<1456507386)?0:1;
	//int Period=(run<1456503197)?0:1; //2020.07.08
	//2020.12.12
	#ifdef TRIGGER_BR_PERIOD
	int Period=(run<=1457048996)?0:1;
	#else
	int Period=(run<1456503197)?0:1;
	#endif //TRIGGER_BR_PERIOD
	if (!isreal) Period=(time[0]<1454843847)?0:1;
	return Period;
}

//2020.06.02
int SelEvent::RebuildTrig(const int nacc_thres) const
{
	int physbpatt=(isreal)?physbpatt2:physbpatt1;
	int jmembpatt=(isreal)?jmembpatt2:jmembpatt1;
	int nacc=0;
	for (int ibit=0; ibit<8; ibit++) if ((antipatt&(1<<ibit))>0) nacc++;

	int PhysBPattRebuild=physbpatt;
	if ((jmembpatt&0x200L)!=0 && nacc<nacc_thres) PhysBPattRebuild=PhysBPattRebuild|(1<<2);
	else PhysBPattRebuild=PhysBPattRebuild&(~(1<<2));
	
	return PhysBPattRebuild;
}

//------Daq
unsigned int SelEvent::GetDaqJinJLength(int iJinJ)
{
	unsigned int jinjb[2]={0xff4d00,0xb2ff};//2-JINJ bit
	unsigned int daqjlens[3]={0,0,0};
	for(int ib=0;ib<24;ib++)
	{
		//if (run<1456503197)daqjlens[1]+=daqjlen[ib];
		if (run<1447346927)daqjlens[1]+=daqjlen[ib];
		else if(jinjb[0]&(1<<ib))daqjlens[1]+=daqjlen[ib];//1-JINJ
		else if(jinjb[1]&(1<<ib))daqjlens[2]+=daqjlen[ib];//2-JINJ
		daqjlens[0]+=daqjlen[ib];
	}
	return daqjlens[iJinJ];
}

//------Geometry
int SelEvent::Extrapolate(const float point[3], const float dir[3], float *result, const float z) const { //using parameter equations
	result[2] = z;
	float t = (z - point[2])/dir[2];
	for (int i=0; i<2; i++) {
		result[i] = point[i] + t*dir[i];
	}
	return 0;
}
//------RTI
#ifdef AFSRTI
void SelEvent::ProcessTime(unsigned int *ProcTime, TH1F *th, int deg, int opt){ //default: opt=0
	for (unsigned int t=ProcTime[0]; t<=ProcTime[1]; t++){
		AMSSetupR::RTI a;
//--RTI Selection
		if (AMSEventR::GetRTI(a,t)!=0) continue; //No time  information
//		if (!Select_RTI(a)) continue;
		if (! (Select_Run(a.run) && Select_RTI(a)) ) continue;
		if (opt==1) {
			if (!Select_RTIdL1L9(t)) continue;
		}
//--Time Cal
		double nt=a.lf*a.nev/(a.nev+a.nerr);
		double ucutr=1.2*a.cfi[deg][1];//IGRF Cutoff  Recommended
//--Above Cutoff LowBin>=Cutof T+ 
		for(int ibr=1;ibr<=th->GetNbinsX();ibr++){//Above CutOff Time++
			if(th->GetBinLowEdge(ibr)>=ucutr){
				th->AddBinContent(ibr,nt);
			}
		}
		th->SetEntries(th->GetEntries()+1);
	}
}

void SelEvent::MCutoffDis(unsigned int *ProcTime, TH1F *th, int deg, int opt){ //default: opt=0
	for(unsigned int t=ProcTime[0]; t<=ProcTime[1]; t++){
		AMSSetupR::RTI a;
//--RTI Selection
		if (AMSEventR::GetRTI(a,t)!=0) continue; //No time  information
		if (! (Select_Run(a.run) && Select_RTI(a)) ) continue;
		if (opt==1) {
			if (!Select_RTIdL1L9(t)) continue;
		}
		double ucutr=1.2*a.cfi[deg][1];//IGRF Cutoff  Recommended
		th->Fill(ucutr);
	}
}
/*#else
oid SelEvent::ProcessTime(unsigned int *ProcTime, TH1F *th, int deg, int opt){ //default: opt=0
	for(unsigned int t=ProcTime[0]; t<=ProcTime[1]; t++){
//--RTI Selection
		if(! (Select_Run() && Select_RTI()) ) continue;
		if (opt==1) {
			if (!Select_RTIdL1L9()) continue;
		}
//--Time Cal
		double nt=rtilf*rtinev/(rtinev+rtinerr);
		double ucutr=1.2*mcutoffi[deg][1];//IGRF Cutoff  Recommended
//--Above Cutoff LowBin>=Cutof T+ 
		for(int ibr=1;ibr<=th->GetNbinsX();ibr++){//Above CutOff Time++
			if(th->GetBinLowEdge(ibr)>=ucutr){
				th->AddBinContent(ibr,nt);
			}
		}
		th->SetEntries(th->GetEntries()+1);
	}
}

void SelEvent::MCutoffDis(unsigned int *ProcTime, TH1F *th, int deg, int opt){ //default: opt=0
	for(unsigned int t=ProcTime[0]; t<=ProcTime[1]; t++){
//--RTI Selection
		if(! (Select_Run() && Select_RTI()) ) continue;
		if (opt==1) {
			if (!Select_RTIdL1L9()) continue;
		}
		double ucutr=1.2*mcutoffi[deg][1];//IGRF Cutoff  Recommended
		th->Fill(ucutr);
	}
}*/
#endif //AFSRTI

//------
//------TrTrack
int SelEvent::HasTkLHitXY(int ilay) const {
	bool lhxy[2]={false};
	for(int ixy=0;ixy<2;ixy++){if((tk_hitb[ixy]&(1<<ilay))>0)lhxy[ixy]=true;}
	if     (lhxy[0]&&lhxy[1])return 2;//X+Y
	else if(lhxy[1])         return 1;//Y
	else                     return 0;//No
}

int SelEvent::GetTkInNHit(int ixy,int ispan) const { //ixy=0:X+Y, ixy=1:Y, ispan=1:Inner, ispan=6:InInner(L3-L8)
	int nhiti[2]={0};
	int bl=0+1;
	if(ispan==6)bl=0+2;//InInner(L3-L8)
	for(int ilay=bl;ilay<9-1;ilay++){
		if(HasTkLHitXY(ilay)==2)nhiti[0]++;//X+Y
		if(HasTkLHitXY(ilay)>=1)nhiti[1]++;//Y
	}
	return nhiti[ixy];
}

bool SelEvent::IsPassTkL(int ilay) const {
	bool mlh=0;
	float coox=tk_pos[ilay][0];
	float cooy=tk_pos[ilay][1];
	float disr=coox*coox+cooy*cooy;
	if(ilay==9-1)mlh=(fabs(coox)<cirr[ilay]&&fabs(cooy)<ciry[ilay]);
	else         mlh=(sqrt(disr)<cirr[ilay]&&fabs(cooy)<ciry[ilay]);
	return mlh;
}

int SelEvent::GetTkNHitF(int ispan) const { //ispan=1:Inner, ispan=2: FS, ispan=6:InInner(L3-L8)
	int mnhitf=0;
	int fl=0, ll=0;
	if (ispan == 1) {fl=0+1; ll=9-1;}
	if (ispan == 2) {fl=0; ll=9;}
	if (ispan == 6) {fl=0+2; ll=9-1;}
	
	for(int il=fl;il<ll;il++) {
		if(IsPassTkL(il)){mnhitf++;}
	}
	return mnhitf;
}

float SelEvent::GetChis(int ispan, int ixy, int ialgo, int ipgc) const {
	float chis[7]={0};
	//amsd54
	chis[0]=tk_chis1[ialgo][ipgc][1][ixy];//Inner
	chis[1]=tk_chis1[ialgo][ipgc][2][ixy];//L1+Inner
	chis[2]=tk_chis1[ialgo][ipgc][4][ixy];//L1+Inner+L9 //Force Full Span
	chis[3]=tk_chis1[ialgo][ipgc][3][ixy];//Inner+L9
	chis[4]=tk_chis1[ialgo][ipgc][5][ixy];//InnerUp
	chis[5]=tk_chis1[ialgo][ipgc][6][ixy];//InnerDown
	/*//old
	chis[0]=tk_chis1[ialgo][ipgc][1][ixy];//Inner
	chis[1]=tk_chis1[ialgo][ipgc][2][ixy];//L1+Inner
	chis[2]=tk_chis1[ialgo][ipgc][6][ixy];//L1+Inner+L9
	chis[3]=tk_chis1[ialgo][ipgc][3][ixy];//Inner+L9
	chis[4]=tk_chis1[ialgo][ipgc][4][ixy];//InnerUp
	chis[5]=tk_chis1[ialgo][ipgc][5][ixy];//InnerDown*/
#ifdef USENEWDST
	chis[6]=tk_chis1[ialgo][ipgc][7][ixy];//InInner(L3-L8)
#else
	chis[6]=chis[5];
#endif
	return chis[ispan];
}

float SelEvent::GetL1ChisY(int ialgo,int ipgc) const
{
	bool l1hy=(HasTkLHitXY(0)>=1);
	if(!l1hy)return 99999;
	float chisyl1=GetChis(1,1,ialgo,ipgc)*(GetTkInNHit(1)+1-3);
	float chisyin=GetChis(0,1,ialgo,ipgc)*(GetTkInNHit(1)-3);
	chisyl1=(chisyl1-chisyin);
	return chisyl1;
}

float SelEvent::GetL9ChisY(int ialgo,int ipgc) const
{
	bool l9hy=(HasTkLHitXY(8)>=1);
	if(!l9hy)return 99999;
	float chisyl9=GetChis(3,1,ialgo,ipgc)*(GetTkInNHit(1)+1-3);
	float chisyin=GetChis(0,1,ialgo,ipgc)*(GetTkInNHit(1)-3);
	chisyl9=(chisyl9-chisyin);
	return chisyl9;
}

float SelEvent::GetExtChisDiffRig(const int iext, const int ixy, const int ialgo, const int ipgc) const
{
	const int il = (iext==0)?0:8;
	bool exth=(HasTkLHitXY(il)>=1);
	//if(!exth)return 99999;
	if(!exth)
	{
		cout << " ****** not passing external hit cut, iext=" << iext << " ixy=" << ixy << " ialgo=" << ialgo << " ipgc=" << ipgc << endl; //XXX
		return -2; //2023.05.18
	}
	float chisExt=GetChis((iext==0)?1:3,ixy,ialgo,ipgc)*(GetTkInNHit(ixy)+1-3);
	float chisin=GetChis(0,ixy,ialgo,ipgc)*(GetTkInNHit(ixy)-3);
	chisExt=(chisExt-chisin);
	return chisExt;
}

float SelEvent::GetL1L9Chis(const int ixy, const int ialgo, const int ipgc) const
{
	bool exth=(HasTkLHitXY(0)>=1) && (HasTkLHitXY(8)>=1); //have both L1 and L9 hit
	if(!exth)return 99999;
	float chisL1L9=GetChis(2,ixy,ialgo,ipgc)*(GetTkInNHit(ixy)+2-3);
	float chisin=GetChis(0,ixy,ialgo,ipgc)*(GetTkInNHit(ixy)-3);
	chisL1L9=(chisL1L9-chisin)/2.;
	return chisL1L9;
}

//2022.10.01
float SelEvent::GetExtChisGBL(const int iext, const int ispan, const int ixy, const int ipgc) const
{
	float ExtChis = 0.;
	int isp=iSpanVar[ispan];
	for (int ixy0=0; ixy0<2; ixy0++)
	{
		if (ixy!=2 && ixy0!=ixy) continue;
		ExtChis += tk_ExtChisGBL[iext][ipgc][isp][ixy0];
	} //ixy0
	if (ixy==2) ExtChis = ExtChis/2.;
	return ExtChis;
}

//2022.10.08
float SelEvent::GetExtChis(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo, const int irigcal) const
{
	float fExtChis = 0.;
	for (int ixy0=0; ixy0<2; ixy0++)
	{
		if (ixy<2 && ixy0!=ixy) continue;
		fExtChis += (iext==0)?tk_L1ExtChis[ialgo][ipgc][ixy0][inter][ivcoo][irigcal]:tk_L9ExtChis[ialgo][ipgc][ixy0][inter][ivcoo][irigcal];
	}
	return fExtChis;
}

float SelEvent::GetExtScatAngle(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo, const int irigcal) const
{
	float fExtScatAngle = 0.;
	if (iext==0) fExtScatAngle=tk_L1ScatAngle[ialgo][ipgc][ixy][inter][ivcoo][irigcal];
	else if (iext==1) fExtScatAngle=tk_L9ScatAngle[ialgo][ipgc][ixy][inter][ivcoo][irigcal];
	return fExtScatAngle;
}

float SelEvent::GetExtScatAngleRatio(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo, const int irigcal) const
{
	float fExtScatAngleRatio = 0.;
	if (iext==0) fExtScatAngleRatio=tk_L1ScatAngleRatio[ialgo][ipgc][ixy][inter][ivcoo][irigcal];
	else if (iext==1) fExtScatAngleRatio=tk_L9ScatAngleRatio[ialgo][ipgc][ixy][inter][ivcoo][irigcal];
	return fExtScatAngleRatio;
}

float SelEvent::GetExtHitRes(const int iext, const int ialgo, const int ipgc, const int ixy, const int inter, const int ivcoo, const int irigcal) const
{
	float fExtHitRes = (iext==0)?tk_L1ExtHitRes[ialgo][ipgc][ixy][inter][ivcoo][irigcal]:tk_L9ExtHitRes[ialgo][ipgc][ixy][inter][ivcoo][irigcal];
	return fExtHitRes;
}

float SelEvent::GetTkInQ() const
{
//	return tk_q[1];
	return (tk_q[1]==-1)?0:tk_q[1];
}

float SelEvent::GetTkMeanQSide(int ixy, int sopt, int lopt) const
{
	int iMaxQLayer=-1, iNLayerU=0;
	float fMeanQ=0, fMaxQ=0;
	for (int ilay=0; ilay<9; ilay++) {
		if ((lopt&(1<<ilay))==0 || tk_ql2[ilay][ixy]<=0) continue;
		fMeanQ+=tk_ql2[ilay][ixy];
		iNLayerU++;
		if (fMaxQ<tk_ql2[ilay][ixy]) {iMaxQLayer=ilay; fMaxQ=tk_ql2[ilay][ixy];}
	} //ilay
	
	if (iNLayerU<=1) return fMeanQ;
	if (sopt==1) {fMeanQ-=fMaxQ; iNLayerU--;};
	fMeanQ = fMeanQ / iNLayerU;
	return fMeanQ;
}

/*float SelEvent::GetRigidity(int ispan,int ialgo,int ipgc) const
{
    float rigidity[7]={0};
    //amsd54
    rigidity[0]=tk_rigidity1[ialgo][ipgc][1];//Inner
    rigidity[1]=tk_rigidity1[ialgo][ipgc][2];//Inner+L1
    rigidity[2]=tk_rigidity1[ialgo][ipgc][4];//Inner+L1+L9 //Force Full Span
    rigidity[3]=tk_rigidity1[ialgo][ipgc][3];//Inner+L9 
    rigidity[4]=tk_rigidity1[ialgo][ipgc][5];//InnerUp
    rigidity[5]=tk_rigidity1[ialgo][ipgc][6];//InnerDown
    ////old
    //rigidity[0]=tk_rigidity1[ialgo][ipgc][1];//Inner
    //rigidity[1]=tk_rigidity1[ialgo][ipgc][2];//Inner+L1
    //rigidity[2]=tk_rigidity1[ialgo][ipgc][6];//Inner+L1+L9
    //rigidity[3]=tk_rigidity1[ialgo][ipgc][3];//Inner+L9 
    //rigidity[4]=tk_rigidity1[ialgo][ipgc][4];//InnerUp
    //rigidity[5]=tk_rigidity1[ialgo][ipgc][5];//InnerDown
#ifdef USENEWDST
    rigidity[6]=tk_rigidity1[ialgo][ipgc][7];//InInner(L3-L8)
#else
    rigidity[6]=rigidity[5];
#endif
    //return rigidity[ispan];
    
    //2020.08.30
    float dRigidity = rigidity[ispan];
    #ifdef CORRIGINDST
    dRigidity = Rigidity[ispan];
    #endif //CORRIGINDST
    //2020.09.01
    #if defined GBATCH && defined CORRIGGBATCH
    dRigidity = AMSEventR::GetCorrectedGeomRigidity(dRigidity,ispan,0);
    #endif //GBATCH && CORRIGGBATCH
    return dRigidity;
}*/
//2020.09.01
float SelEvent::GetRigidity(const int ispan, const int ialgo, const int ipgc, const bool bRigCor) const
{
	float rigidity[7]={0};
	//amsd54
	rigidity[0]=tk_rigidity1[ialgo][ipgc][1];//Inner
	rigidity[1]=tk_rigidity1[ialgo][ipgc][2];//Inner+L1
	rigidity[2]=tk_rigidity1[ialgo][ipgc][4];//Inner+L1+L9 //Force Full Span
	rigidity[3]=tk_rigidity1[ialgo][ipgc][3];//Inner+L9 
	rigidity[4]=tk_rigidity1[ialgo][ipgc][5];//InnerUp
	rigidity[5]=tk_rigidity1[ialgo][ipgc][6];//InnerDown
#ifdef USENEWDST
	rigidity[6]=tk_rigidity1[ialgo][ipgc][7];//InInner(L3-L8)
#else
	rigidity[6]=rigidity[5];
#endif
	
	float dRigidity = rigidity[ispan];
	//if (bRigCor)
	//2020.10.13: not use for MC
	//if (bRigCor && isreal)
	if (bRigCor && isreal && amsdn<68) //only do correction for PASS7 //2023.03.28
	{
		#if defined READRIGINDST && defined CORRIGINDST
		dRigidity = Rigidity[ispan];
		#endif //CORRIGINDST
		#if defined GBATCH && defined CORRIGGBATCH
		dRigidity = AMSEventR::GetCorrectedGeomRigidity(dRigidity,ispan,0);
		#endif //(READRIGINDST && CORRIGINDST) || (GBATCH && CORRIGGBATCH)
	}
	return dRigidity;
}

bool SelEvent::PassTkL(int ilay, float point[3], float dir[3]) const {
	float tkpos[3]={0};
	Extrapolate(point, dir, tkpos, tk_pz[ilay]);
	bool mlh=0;
	float coox=tkpos[0];
	float cooy=tkpos[1];
	float disr=coox*coox+cooy*cooy;
	if(ilay==9-1)mlh=(fabs(coox)<cirr[ilay]&&fabs(cooy)<ciry[ilay]);
	else         mlh=(sqrt(disr)<cirr[ilay]&&fabs(cooy)<ciry[ilay]);
	return mlh;
}


float SelEvent::GetBinCutoff(int deg) const {
	int bin=0;
	#ifdef AMSSOFT
	AMSSetupR::RTI a;
	if(AMSEventR::GetRTI(a,time[0])!=0) return 0;
	while ((nuclei_bins[bin] < 1.2*a.cfi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
	#else
	while ((nuclei_bins[bin] < 1.2*mcutoffi[deg][1]) && bin <= nuclei_nbin) bin++;
	#endif //AMSSOFT
	return nuclei_bins[bin];
}

float SelEvent::CalTrunCharge(const int ixy, const int opt) const
{
	double dTrunQ=0., dMaxQ=0.;
	int nUsed=0;
	double dLQ=0.;
	for (int ilay=0; ilay<9; ilay++) {
		if ((opt&(1<<ilay))==0) continue;
		if (ixy<2) dLQ = tk_ql2[ilay][ixy];
		else if (ixy==2) dLQ = tk_ql[ilay];
		if (dLQ>=dMaxQ) dMaxQ = dLQ;
		dTrunQ += dLQ;
		nUsed++;
	} //ilay
	return (dTrunQ-dMaxQ)/(nUsed-1);
} //CalTrunCharge

float SelEvent::CalTrunCharge2(const int ixy, const int ver, const int opt) const
{
	double dTrunQ=0., dMaxQ=0.;
	int nUsed=0;
	double dLQ=0.;
	for (int ilay=0; ilay<9; ilay++)
	{
		if ((opt&(1<<ilay))==0) continue;
		dLQ = GetTkLQNew(ilay, ixy, ver);
		if (dLQ>=dMaxQ) dMaxQ = dLQ;
		dTrunQ += dLQ;
		nUsed++;
	} //ilay
	return (dTrunQ-dMaxQ)/(nUsed-1);
} //CalTrunCharge2

//2019.09.04
float SelEvent::CalTrunChargeRMS(const int ixy, const int ver, const int opt) const
{
	double dRMS=0., dMaxQ=0.;
	int nUsed=0;
	double dLQ=0.;
	const double dTrunQ = CalTrunCharge2(ixy, ver, opt);
	for (int ilay=0; ilay<9; ilay++)
	{
		if ((opt&(1<<ilay))==0) continue;
		double dLQ = GetTkLQNew(ilay, ixy, ver);
		if (dLQ>=dMaxQ) dMaxQ = dLQ;
		dRMS += pow(dLQ - dTrunQ, 2);
		nUsed++;
	} //ilay
	dRMS = sqrt((dRMS-pow(dMaxQ-dTrunQ,2))/(nUsed-1));
	return dRMS;
} //CalTrunChargeRMS

//--new getter function
float SelEvent::GetTkInQNew(int ixy, int iVer) const
{
	int QVer=iVer%3;
	if (QVer==0) //old charge
	{
		if (ixy==2) return tk_q[1];
		else return -1;
	}
	else if (QVer<3) return tk_qin[QVer-1][ixy]; //new charge
	return 0; //2020.01.21
}

float SelEvent::GetTkInQrmsNew(int ixy, int iVer) const
{
	int QVer=iVer%3;
	if (QVer==0) //old charge
	{
		if (ixy==2) return tk_qrms[1];
		else return -1;
	}
	else if (QVer<3) return tk_qrmn[QVer-1][ixy]; //new charge
	return 0; //2020.01.21
}

float SelEvent::GetTkInInQNew(int ixy, int iVer) const
{
	int QVer=iVer%3;
	//if (QVer==0) return -1; //old charge, now didn't stored
	//else if (QVer<3) return tk_inT[QVer-1][1][2]; //new charge
	return -1;
}

float SelEvent::GetTkInInQrmsNew(int ixy, int iVer) const
{
	int QVer=iVer%3;
	//if (QVer==0) return -1; //old charge, now didn't stored
	//else if (QVer<3) return tk_inTrms[QVer-1][1][2]; //new charge
	return -1;
}

float SelEvent::GetTkLQNew(int ilay, int ixy, int iVer) const
{
	int QVer=iVer%3;
	if (QVer==0) //old charge
	{
		if (ixy==2) return tk_ql[ilay];
		else return tk_ql2[ilay][ixy];
	}
	else if (QVer<3) return tk_qln[QVer-1][ilay][ixy];//new charge
	return 0; //2020.01.21
}

//2002.08.20
float SelEvent::CalLQAsy(int ilay, int iVer) const
{
	double dLQX=GetTkLQNew(ilay,0,iVer), dLQY=GetTkLQNew(ilay,1,iVer);
	if ((dLQX+dLQY)==0) return 0;
	return fabs(dLQX-dLQY)/(dLQX+dLQY);
}

float SelEvent::GetUBExtQTrkCor(int iexl, int ixy, int iVer) const
{
	int QVer=iVer%3;
	if (QVer==0) return tk_exql[iexl][ixy]; //old charge
	else if (QVer<3) return tk_exqln[QVer-1][iexl][ixy]; //2019.05.23: starting from amsd60n, both XQ & YQ are stored too
	return 0; //2020.01.21
}

int SelEvent::GetUBExtQStatusTrkCor(int iexl, int iVer) const
{
	/*int QVer=iVer%3;
	if (QVer==0) return tk_exqls[iexl]; //old charge
	else if (QVer<3) return tk_ubqs[iexl];//new charge, now only HL's charge*/
	
	//2019.06.07:for all charge calibration return the same charge status since it's not depends on the charge calibration
	return tk_exqls[iexl];
}

//2020.02.18
bool SelEvent::IsUBXYExtQTrkCor(int iexl, int iVer) const
{
	bool UBXY = (GetUBExtQTrkCor(iexl,0,iVer)>0) && (GetUBExtQTrkCor(iexl,1,iVer)>0);
	return UBXY;
}

float SelEvent::GetUBExtQBetaCor(int iexl, int ixy, int iVer) const
{
	int QVer=iVer%3;
	if (QVer==0) //old charge
	{
		if (ixy==2) return (iexl==0)?tk_l1qvs:tk_l9qvs;
		else return (iexl==0)?tk_l1qxy[ixy]:tk_l9qxy[ixy];
	}
	else if (QVer<3) //new charge
	{
		//2019.04.30: use new tracker Q from QY's dst, for XQ&YQ, use old charge
		if (ixy==2) return tk_exqvn[QVer-1][iexl];
		else return (iexl==0)?tk_l1qxy[ixy]:tk_l9qxy[ixy];
		//if (ixy==2) return (iexl==0)?tk_l1qvsHL:tk_l9qvsHL;
		//else return (iexl==0)?tk_l1qxyHL[ixy]:tk_l9qxyHL[ixy];
	}
	return 0; //2020.01.21
}

float SelEvent::GetUBExtQBetaMatch(int iexl, int iVer) const
{
	int QVer=iVer%3;
	if (QVer==0) return (iexl==0)?tk_l1mds:-1; //old charge
	//else if (QVer<3) return (iexl==0)?tk_l1mdsHL:-1; //new charge
	else return -1; //new charge //2019.07.01
}

//------ToF
int SelEvent::GetToFQlN() const {
	int qln = 0;
	for (int il=0; il<4; il++) {
		if (tof_ql[il]>0.) qln++;
	}
	return qln;
}

int SelEvent::GetToFQlGoodN() const
{
	int qlng = 0;
	for (int il=0; il<4; il++)
	{
		if (IsGoodTofLQStat(il)) qlng++;
	}
	return qlng;
}

float SelEvent::GetTOFUDQ(int iud, int igeom) const
//float SelEvent::GetTOFUDQ(int iud) const //2023.04.23
{
	float tofqud[2] = {0};
	/*int span=0;
	if (igeom==0) span=1;		//Inner
	else if (igeom==1) span=2;	//Inner+L1
	else if (igeom==2) span=0;	//FS*/
	
	tofqud[0] = (tof_ql[0]+tof_ql[1])/2.;
	tofqud[1] = (tof_ql[2]+tof_ql[3])/2.;
	return tofqud[iud];   
}

float SelEvent::GetTofLQ(int ilay, int igeom) const
{
	int span=0;
	if (igeom==0) span=1;		//Inner
	else if (igeom==1) span=2;	//Inner+L1
	else if (igeom==2) span=0;	//FS
	
	return tof_ql[ilay];
}

float SelEvent::GetTOFChis(const int itc, const int inorm) const
{
	float tofchis=0.;
	if (itc==0) tofchis=(inorm==0)?tof_chist:tof_chist_n;
	else if (itc==1) tofchis=(inorm==0)?tof_chisc:tof_chisc_n;
	else
	{
		cout << " *** SelEvent::GetTOFChis: wrong itc=" << itc << ", which should be 0 (tof_chist) or 1 (tof_chisc) *** " << endl;
		tofchis=-1;
	}
	
	return tofchis;
}

//--
bool SelEvent::IsPassEcal(int opt, int ilay, int iecalayer) const
{
	if (ilay<0 || ilay>8) {
		cout << "Wrong tracker layer insert." << endl;
		return -1;
	}
	if (iecalayer<0 || iecalayer>8) {
		cout << "Wrong Ecal layer insert." << endl;
		return -2;
	}
	
	float tkecal[3]={0};
	bool bPassEcal=true;
	// first Ecal superlayer
	if (opt==2) {
		bPassEcal = (fabs(ecal_pos[0])<=ecalxy[0]) && (fabs(ecal_pos[1])<=ecalxy[1]);
	}
	else {
		if (opt==0) Extrapolate(tk_pos[ilay], tk_dir[ilay], tkecal, ecal_pz);
		else if (opt==1) Extrapolate(tk_pos1s[ilay], tk_dir1s, tkecal, ecal_pz);
		bPassEcal = (fabs(tkecal[0])<=ecalxy[0]) && (fabs(tkecal[1])<=ecalxy[1]);
		
		// iecalayer superlayer
		if (iecalayer>0) {
			//Extrapolate(tk_pos[ilay], tk_dir[ilay], tkecal, ecal_pz-iecalayer*ecal_thickness);
			if (opt==0) Extrapolate(tk_pos[ilay], tk_dir[ilay], tkecal, ecal_pz-iecalayer*ecal_thickness);
			else if (opt==1) Extrapolate(tk_pos1s[ilay], tk_dir1s, tkecal, ecal_pz-iecalayer*ecal_thickness);
			bPassEcal = bPassEcal && (fabs(tkecal[0])<=ecalxy[0]) && (fabs(tkecal[1])<=ecalxy[1]);
		}
	}
	return bPassEcal;
}

bool SelEvent::IsPassEcalLayer(float pos[3], float dir[3], int iecalayer, bool bTight) const
{
	float extra_pos[3]={0};
	Extrapolate(pos, dir,  extra_pos, ecal_plane_top_z[iecalayer]);
	int ifcell=0, ilcell=71;
	if (bTight) ifcell=0+2, ilcell=71-2;
	bool bWithinX = extra_pos[0]>=ecal_cell_x[ifcell]-ecal_cell_dim/2. && extra_pos[0]<=ecal_cell_x[ilcell]+ecal_cell_dim/2.;
	bool bWithinY = extra_pos[1]>=ecal_cell_y[ifcell]-ecal_cell_dim/2. && extra_pos[1]<=ecal_cell_y[ilcell]+ecal_cell_dim/2.;
	return bWithinX && bWithinY;
}

bool SelEvent::IsPassEcal2(int opt, int ilay, int iecalayer, bool bTight) const
{
	if (ilay<0 || ilay>8)
	{
		cout << "Wrong tracker layer insert. Only 0 - 8 are accepted" << endl;
		return -1;
	}
	if (iecalayer<0 || iecalayer>17)
	{
		cout << "Wrong Ecal layer insert. Only 0 - 17 are accepted" << endl;
		return -2;
	}
	
	float tkecal[3]={0};
	float pos[3]={0}, dir[3]={0};
	bool bPassEcal=true;
	// first Ecal superlayer
	if (opt==2) bPassEcal = (fabs(ecal_pos[0])<=ecalxy[0]) && (fabs(ecal_pos[1])<=ecalxy[1]); //2019.05.28: not updated yet
	else
	{
		for (int ic=0; ic<3; ic++) pos[ic]=(opt==0)?tk_pos[ilay][ic]:tk_pos1s[ilay][ic], dir[ic]=(opt==0)?tk_dir[ilay][ic]:tk_dir1s[ic];
		bPassEcal = IsPassEcalLayer(pos,dir,0,bTight);
		if (iecalayer>0) bPassEcal = bPassEcal && IsPassEcalLayer(pos,dir,iecalayer,bTight);
	}
	return bPassEcal;
}

/*bool SelEvent::EcalMatchness(int opt, int ilay, int iecalayer) const
{
	float tkecal[3]={0.};
	Extrapolate(tk_pos[ilay], tk_dir[ilay], tkecal, ecal_plane_top_z[iecalayer]);
	
	int side=((iecalayer/2)%2==1)?0:1;
	
	bool bMatch=true;
	for (int iopt=0; iopt<2; iopt++)
	{
		if (opt<2 && iopt!=opt) continue;
		int id = (iopt==0)?ecal_icellmax[iecalayer]:(int)(ecal_cog[iecalayer]+0.5);
		double dCoo=(side==0)?ecal_cell_x[id]:ecal_cell_y[id];
		double dLimits=(iopt==0)?ecal_cell_dim/2.:ecal_cell_dim/2.*3.;
		bMatch = bMatch && (fabs(tkecal[side]-dCoo)<=dLimits);
	} //iopt
	return bMatch;
}
bool SelEvent::EcalMatchness2(int opt, int is, int ilay, int iecalayer) const
{
	float tkecal[3]={0.};
	Extrapolate(tk_pos[ilay], tk_dir[ilay], tkecal, ecal_plane_top_z[iecalayer]);
	
	int side=((iecalayer/2)%2==1)?0:1;
	
	bool bMatch=true;
	for (int iopt=0; iopt<2; iopt++)
	{
		if (opt<2 && iopt!=opt) continue;
		int id=0;
		if (iopt==0) id = (is==0)?ecal_icellmax[iecalayer]:ishow_icellmax[iecalayer];
		else if (iopt==1) id = (is==0)?(int)(ecal_cog[iecalayer]+0.5):(int)(ishow_cog[iecalayer]+0.5);
		
		double dCoo=(side==0)?ecal_cell_x[id]:ecal_cell_y[id];
		double dLimits=(iopt==0)?ecal_cell_dim/2.:ecal_cell_dim/2.*3.;
		bMatch = bMatch && (fabs(tkecal[side]-dCoo)<=dLimits);
	} //iopt
	return bMatch;
}*/

//2020.02.18
bool SelEvent::ECalMIPsSel(int iECalLay) const
{
	if (iECalLay<0 || iECalLay>17)
	{
		cout << "Wrong Ecal layer insert: " << iECalLay << ". Only 0 - 17 are accepted" << endl;
		return -2;
	}
	
	bool bECalMIPsSel = true;
	for (int il=0; il<=iECalLay; il++)
	{
		//--cog with +/- 1 cell to the maximum cell
		bECalMIPsSel = bECalMIPsSel && fabs(ecal_cog[il]-ecal_icellmax[il])<2;
		
		//--S3/S5<0.98
		if (ecal_S5[il]<=0) bECalMIPsSel = bECalMIPsSel && false;
		//else bECalMIPsSel = bECalMIPsSel && (ecal_S5[il]/ecal_S5[il]>0.98);
		else bECalMIPsSel = bECalMIPsSel && (ecal_S3[il]/ecal_S5[il]>0.98); //2020.02.20
	} //il
	return bECalMIPsSel;
}

//2020.02.19
bool SelEvent::ECalMaxEdepSel(int iECalLay) const
{
	if (iECalLay<0 || iECalLay>17)
	{
		cout << "Wrong Ecal layer insert: " << iECalLay << ". Only 0 - 17 are accepted" << endl;
		return -2;
	}
	
	bool bECalMaxEdepSel = true;
	for (int il=0; il<=iECalLay; il++)
	{
		//--max edep cell not in the 2 edge
		bECalMaxEdepSel = bECalMaxEdepSel && ((ecal_icellmax[il]>=2) && (ecal_icellmax[il]<=69));
	} //il
	return bECalMaxEdepSel;
}

//------variables distribution
//book
int SelEvent::BookVarDisInnerTrk(const string sDir, const int QVer)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	string sStoreDir = sDir+"/"+sVarDis[0];
	
	//------Good Particle
	fOutput->mkdir(Form("%s/GoodParticle", sStoreDir.c_str()));
	fOutput->cd(Form("%s/GoodParticle", sStoreDir.c_str()));
	
	histn = "itrtrack";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 10, -1, 9);
	
	//------Fiducial Volume
	fOutput->mkdir(Form("%s/FiducialVolume", sStoreDir.c_str()));
	fOutput->cd(Form("%s/FiducialVolume", sStoreDir.c_str()));
	
	histn = "NoInnerPassed";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 10, 0, 10);
	
	histn = "InnerFVPattern";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 2, -0.5, 1.5);
	
	//------TrTrack
	fOutput->mkdir(Form("%s/TrTrack", sStoreDir.c_str()));
	fOutput->cd(Form("%s/TrTrack", sStoreDir.c_str()));
	
	histn = "NoInnerHits";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 10, 0, 10);
	
	histn = "InnerHitPattern";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 2, -0.5, 1.5);
	
	histn = "InnerNormChisqY";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000);
	
	//------Charge
	//2019.04.30: use opt QVer to control what charge calibration to store
	for (int ixy=0; ixy<3; ixy++)
	{
		fOutput->mkdir(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		fOutput->cd(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
		
			histn = Form("InnerQ%d_xy%d", iQVer, ixy);
			h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
		
			histn = Form("InnerQrms%d_xy%d", iQVer, ixy);
			h1fVD = new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000);
		
			for (int ilay=1; ilay<8; ilay++)
			{
				histn = Form("L%dQ%d_xy%d", ilay+1, iQVer, ixy);
				h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
			} //ilay
		} //iQVer
	} //ixy
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}

int SelEvent::BookVarDisExtTrk(const string sDir, const int QVer)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	string sStoreDir = sDir+"/"+sVarDis[1];
	
	//------Fiducial volume
	fOutput->mkdir(Form("%s/FiducialVolume", sStoreDir.c_str()));
	fOutput->cd(Form("%s/FiducialVolume", sStoreDir.c_str()));
	
	histn = "L1_FiducialVolume";
	h2fVD = new TH2F(histn.c_str(), histn.c_str(), 100, -100, 100, 100, -100, 100);
	h2fVD->SetDrawOption("COLZ");
	
	histn = "L9_FiducialVolume";
	h2fVD = new TH2F(histn.c_str(), histn.c_str(), 100, -100, 100, 100, -100, 100);
	h2fVD->SetDrawOption("COLZ");
	
	//------TrTrack
	fOutput->mkdir(Form("%s/TrTrack", sStoreDir.c_str()));
	fOutput->cd(Form("%s/TrTrack", sStoreDir.c_str()));
	
	histn = "L1XY";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3, -0.5, 2.5);
	
	histn = "L1InnerNormChisqY";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000);
	
	histn = "L1ChisqY";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000);
	
	histn = "L1InnerL9NormChisqY";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000);
	
	histn = "L9XY";
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3, -0.5, 2.5);
	//------Charge
	//2019.04.30
	for (int ixy=0; ixy<3; ixy++)
	{
		fOutput->mkdir(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		fOutput->cd(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
		
			histn = Form("L1Q%d_xy%d", iQVer, ixy);
			h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
		
			histn = Form("L9Q%d_xy%d", iQVer, ixy);
			h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
		} //iQVer
	} //ixy
	return 1;
}

int SelEvent::BookVarDisTof(const string sDir)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	string sStoreDir = sDir+"/"+sVarDis[2];
	
	//------Good Particle
	fOutput->mkdir(Form("%s/GoodParticle", sStoreDir.c_str()));
	fOutput->cd(Form("%s/GoodParticle", sStoreDir.c_str()));
	
	histn = Form("ibetah");
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 10, -1, 9);
	
	histn = Form("tof_btype");
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 10, 0, 10);
	
	histn = Form("tof_betah");
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 105, 0, 1.05);
	
	//------Charge
	fOutput->mkdir(Form("%s/Charge", sStoreDir.c_str()));
	fOutput->cd(Form("%s/Charge", sStoreDir.c_str()));
	
	histn = Form("UTofQ");
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	
	histn = Form("LTofQ");
	h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	
	for (int ilay=0; ilay<4; ilay++)
	{
		histn = Form("TofQ_l%d", ilay+1);
		h1fVD = new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35);
	} //ilay
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}

//2019.09.01
int SelEvent::BookVarDisInnerTrkRIG(const string sDir, const int Charge, const int QVer)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	iNbin = getNbin(Charge);
	pBins = getBins(Charge);
	
	string sStoreDir = sDir+"/"+sVarDis[0];
	
	//------Good Particle
	fOutput->mkdir(Form("%s/GoodParticle", sStoreDir.c_str()));
	fOutput->cd(Form("%s/GoodParticle", sStoreDir.c_str()));
	
	histn = "itrtrackvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 10, -1, 9);
	
	//------Fiducial Volume
	fOutput->mkdir(Form("%s/FiducialVolume", sStoreDir.c_str()));
	fOutput->cd(Form("%s/FiducialVolume", sStoreDir.c_str()));
	
	histn = "NoInnerPassedvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 10, 0, 10);
	
	histn = "InnerFVPatternvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 2, -0.5, 1.5);
	
	//------TrTrack
	fOutput->mkdir(Form("%s/TrTrack", sStoreDir.c_str()));
	fOutput->cd(Form("%s/TrTrack", sStoreDir.c_str()));
	
	histn = "NoInnerHitsvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 10, 0, 10);
	
	histn = "InnerHitPatternvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 2, -0.5, 1.5);
	
	histn = "InnerNormChisqYvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, 0, 1000);
	
	//------Charge
	for (int ixy=0; ixy<3; ixy++)
	{
		fOutput->mkdir(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		fOutput->cd(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
		
			histn = Form("InnerQ%dvsRig_xy%d", iQVer, ixy);
			h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
		
			histn = Form("InnerQrms%dvsRig_xy%d", iQVer, ixy);
			h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, 0, 1000);
		
			for (int ilay=1; ilay<8; ilay++)
			{
				histn = Form("L%dQ%dvsRig_xy%d", ilay+1, iQVer, ixy);
				h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
			} //ilay
		} //iQVer
	} //ixy
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}

int SelEvent::BookVarDisExtTrkRIG(const string sDir, const int Charge, const int QVer)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	iNbin = getNbin(Charge);
	pBins = getBins(Charge);
	
	string sStoreDir = sDir+"/"+sVarDis[1];
	
	/*//------Fiducial volume
	fOutput->mkdir(Form("%s/FiducialVolume", sStoreDir.c_str()));
	fOutput->cd(Form("%s/FiducialVolume", sStoreDir.c_str()));
	
	histn = "L1_FiducialVolume";
	h2fVD = new TH2F(histn.c_str(), histn.c_str(), 100, -100, 100, 100, -100, 100);
	h2fVD->SetDrawOption("COLZ");
	
	histn = "L9_FiducialVolume";
	h2fVD = new TH2F(histn.c_str(), histn.c_str(), 100, -100, 100, 100, -100, 100);
	h2fVD->SetDrawOption("COLZ");*/
	
	//------TrTrack
	fOutput->mkdir(Form("%s/TrTrack", sStoreDir.c_str()));
	fOutput->cd(Form("%s/TrTrack", sStoreDir.c_str()));
	
	histn = "L1XYvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3, -0.5, 2.5);
	
	histn = "L1InnerNormChisqYvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, 0, 1000);
	
	histn = "L1ChisqYvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, 0, 1000);
	
	histn = "L1InnerL9NormChisqYvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 1000, 0, 1000);
	
	histn = "L9XYvsRig";
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3, -0.5, 2.5);
	
	//------Charge
	for (int ixy=0; ixy<3; ixy++)
	{
		fOutput->mkdir(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		fOutput->cd(Form("%s/Charge/xy%d", sStoreDir.c_str(), ixy));
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
		
			histn = Form("L1Q%dvsRig_xy%d", iQVer, ixy);
			h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
		
			histn = Form("L9Q%dvsRig_xy%d", iQVer, ixy);
			h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
		} //iQVer
	} //ixy
	return 1;
}

int SelEvent::BookVarDisTofRIG(const string sDir, const int Charge)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	iNbin = getNbin(Charge);
	pBins = getBins(Charge);
	
	string sStoreDir = sDir+"/"+sVarDis[2];
	
	//------Good Particle
	fOutput->mkdir(Form("%s/GoodParticle", sStoreDir.c_str()));
	fOutput->cd(Form("%s/GoodParticle", sStoreDir.c_str()));
	
	histn = Form("ibetahvsRig");
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 10, -1, 9);
	
	histn = Form("tof_btypevsRig");
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 10, 0, 10);
	
	histn = Form("tof_betahvsRig");
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 105, 0, 1.05);
	
	//------Charge
	fOutput->mkdir(Form("%s/Charge", sStoreDir.c_str()));
	fOutput->cd(Form("%s/Charge", sStoreDir.c_str()));
	
	histn = Form("UTofQvsRig");
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
	
	histn = Form("LTofQvsRig");
	h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
	
	for (int ilay=0; ilay<4; ilay++)
	{
		histn = Form("TofQvsRig_l%d", ilay+1);
		h2iVDvsRIG = new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35);
	} //ilay
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}

//unbiased variable, for efficiencies
int BookVarDisUBExtTrkTrkCor(const string sDir)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	string sStoreDir = sDir+"/"+sVarDis[3];
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}
int BookVarDisUBTof(const string sDir)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	string sStoreDir = sDir+"/"+sVarDis[4];
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}
int BookVarDisUBExtTrkTofCor(const string sDir)
{
	//bool bSumw2=TH1::fgDefaultSumw2; //2019.09.01
	TH1::SetDefaultSumw2(kFALSE);
	
	string sStoreDir = sDir+"/"+sVarDis[5];
	
	TH1::SetDefaultSumw2(bSumw2);
	return 1;
}
//store
int SelEvent::StoreVarDisInnerTrk(const string sDir, const double ww, const int QVer)
{
	string sStoreDir = sDir+"/"+sVarDis[0];
	
	//------Good Particle
	histn = Form("%s/GoodParticle/itrtrack", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(itrtrack, ww);
	
	//------Fiducial Volume
	//----
	int mnhitf=0;
	for(int il=0+1;il<9-1;il++) {
		if(IsPassTkL(il)){mnhitf++;}
	}
	histn = Form("%s/FiducialVolume/NoInnerPassed", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(mnhitf, ww);
	
	//----
	bool L2=(IsPassTkL(1));
	bool L34=(IsPassTkL(2)||IsPassTkL(3));
	bool L56=(IsPassTkL(4)||IsPassTkL(5));
	bool L78=(IsPassTkL(6)||IsPassTkL(7));
	
	histn = Form("%s/FiducialVolume/InnerFVPattern", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill((L2&&L34&&L56&&L78)?1:0, ww);
	
	//------TrTrack
	//----
	histn = Form("%s/TrTrack/NoInnerHits", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetTkInNHit(), ww);
	
	//----
	bool l2H = (HasTkLHitXY(1)>0);
	bool l34H = ((HasTkLHitXY(2)>0)||(HasTkLHitXY(3)>0));
	bool l56H = ((HasTkLHitXY(4)>0)||(HasTkLHitXY(5)>0));
	bool l78H = ((HasTkLHitXY(6)>0)||(HasTkLHitXY(7)>0));
	histn = Form("%s/TrTrack/InnerHitPattern", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill((l2H && l34H && l56H && l78H)?1:0, ww);
	
	//----
	histn = Form("%s/TrTrack/InnerNormChisqY", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetChis(0, 1), ww);
	
	//------Charge
	//2019.04.30
	for (int ixy=0; ixy<3; ixy++)
	{
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
			histn = Form("%s/Charge/xy%d/InnerQ%d_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
			h1fVD->Fill(GetTkInQNew(ixy, iQVer), ww);
		
			histn = Form("%s/Charge/xy%d/InnerQrms%d_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
			h1fVD->Fill(GetTkInQrmsNew(ixy, iQVer), ww);
		
			for (int ilay=1; ilay<8; ilay++)
			{
				histn = Form("%s/Charge/xy%d/L%dQ%d_xy%d", sStoreDir.c_str(), ixy, ilay+1, iQVer, ixy);
				h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
				h1fVD->Fill(GetTkLQNew(ilay,ixy,iQVer), ww);
			} //ilay
		} //iQVer
	} //ixy
	
	return 1;
}
int SelEvent::StoreVarDisExtTrk(const string sDir, const double ww, const int QVer)
{
	string sStoreDir = sDir+"/"+sVarDis[1];
	
	//------Fiducial volume
	histn = Form("%s/FiducialVolume/L1_FiducialVolume", sStoreDir.c_str());
	h2fVD = static_cast<TH2F*>(fOutput->Get(histn.c_str()));
	h2fVD->Fill(tk_pos[0][0], tk_pos[0][1], ww);
	
	histn = Form("%s/FiducialVolume/L9_FiducialVolume", sStoreDir.c_str());
	h2fVD = static_cast<TH2F*>(fOutput->Get(histn.c_str()));
	h2fVD->Fill(tk_pos[8][0], tk_pos[8][1], ww);
	
	//------TrTrack
	histn = Form("%s/TrTrack/L1XY", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(HasTkLHitXY(0), ww);
	
	histn = Form("%s/TrTrack/L1InnerNormChisqY", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetChis(1, 1), ww);
	
	histn = Form("%s/TrTrack/L1ChisqY", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetL1ChisY(), ww);
	
	histn = Form("%s/TrTrack/L1InnerL9NormChisqY", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetChis(2, 1), ww);
	
	histn = Form("%s/TrTrack/L9XY", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(HasTkLHitXY(8), ww);
	
	//------Charge
	//2019.04.30
	for (int ixy=0; ixy<3; ixy++)
	{
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
			histn = Form("%s/Charge/xy%d/L1Q%d_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
			h1fVD->Fill(GetTkLQNew(0, ixy, iQVer), ww);
			
			histn = Form("%s/Charge/xy%d/L9Q%d_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
			h1fVD->Fill(GetTkLQNew(8, ixy, iQVer), ww);
		} //iQVer
	} //ixy
	
	return 1;
}

int SelEvent::StoreVarDisTof(const string sDir, const double ww, const int QVer)
{
	string sStoreDir = sDir+"/"+sVarDis[2];
	
	//------Good Particle
	histn = Form("%s/GoodParticle/ibetah", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(ibetah,ww);
	
	histn = Form("%s/GoodParticle/tof_btype", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(tof_btype,ww);
	
	histn = Form("%s/GoodParticle/tof_betah", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(tof_betah,ww);
	
	//------Charge
	histn = Form("%s/Charge/UTofQ", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetTOFUDQ(0),ww);
	
	histn = Form("%s/Charge/LTofQ", sStoreDir.c_str());
	h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
	h1fVD->Fill(GetTOFUDQ(1),ww);
	
	for (int ilay=0; ilay<4; ilay++)
	{
		histn = Form("%s/Charge/TofQ_l%d", sStoreDir.c_str(), ilay+1);
		h1fVD = static_cast<TH1F*>(fOutput->Get(histn.c_str()));
		h1fVD->Fill(tof_ql[ilay],ww);
	} //ilay
	
	return 1;
}

//2019.09.01: Distribution vs Rig
int SelEvent::StoreVarDisInnerTrkRIG(const string sDir, const int Span, const double ww, const int QVer)
{
	string sStoreDir = sDir+"/"+sVarDis[0];
	const double dRig = GetRigidity(Span);
	
	//------Good Particle
	histn = Form("%s/GoodParticle/itrtrackvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, itrtrack, ww);
	
	//------Fiducial Volume
	//----
	int mnhitf=0;
	for(int il=0+1;il<9-1;il++) {
		if(IsPassTkL(il)){mnhitf++;}
	}
	histn = Form("%s/FiducialVolume/NoInnerPassedvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, mnhitf, ww);
	
	//----
	bool L2=(IsPassTkL(1));
	bool L34=(IsPassTkL(2)||IsPassTkL(3));
	bool L56=(IsPassTkL(4)||IsPassTkL(5));
	bool L78=(IsPassTkL(6)||IsPassTkL(7));
	
	histn = Form("%s/FiducialVolume/InnerFVPatternvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, (L2&&L34&&L56&&L78)?1:0, ww);
	
	//------TrTrack
	//----
	histn = Form("%s/TrTrack/NoInnerHitsvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetTkInNHit(), ww);
	
	//----
	bool l2H = (HasTkLHitXY(1)>0);
	bool l34H = ((HasTkLHitXY(2)>0)||(HasTkLHitXY(3)>0));
	bool l56H = ((HasTkLHitXY(4)>0)||(HasTkLHitXY(5)>0));
	bool l78H = ((HasTkLHitXY(6)>0)||(HasTkLHitXY(7)>0));
	histn = Form("%s/TrTrack/InnerHitPatternvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, (l2H && l34H && l56H && l78H)?1:0, ww);
	
	//----
	histn = Form("%s/TrTrack/InnerNormChisqYvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetChis(0, 1), ww);
	
	//------Charge
	for (int ixy=0; ixy<3; ixy++)
	{
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
			histn = Form("%s/Charge/xy%d/InnerQ%dvsRig_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
			h2iVDvsRIG->Fill(dRig, GetTkInQNew(ixy, iQVer), ww);
		
			histn = Form("%s/Charge/xy%d/InnerQrms%dvsRig_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
			h2iVDvsRIG->Fill(dRig, GetTkInQrmsNew(ixy, iQVer), ww);
		
			for (int ilay=1; ilay<8; ilay++)
			{
				histn = Form("%s/Charge/xy%d/L%dQ%dvsRig_xy%d", sStoreDir.c_str(), ixy, ilay+1, iQVer, ixy);
				h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
				h2iVDvsRIG->Fill(dRig, GetTkLQNew(ilay,ixy,iQVer), ww);
			} //ilay
		} //iQVer
	} //ixy
	
	return 1;
}
int SelEvent::StoreVarDisExtTrkRIG(const string sDir, const int Span, const double ww, const int QVer)
{
	string sStoreDir = sDir+"/"+sVarDis[1];
	const double dRig = GetRigidity(Span);
	
	/*//------Fiducial volume
	histn = Form("%s/FiducialVolume/L1_FiducialVolume", sStoreDir.c_str());
	h2fVD = static_cast<TH2F*>(fOutput->Get(histn.c_str()));
	h2fVD->Fill(tk_pos[0][0], tk_pos[0][1], ww);
	
	histn = Form("%s/FiducialVolume/L9_FiducialVolume", sStoreDir.c_str());
	h2fVD = static_cast<TH2F*>(fOutput->Get(histn.c_str()));
	h2fVD->Fill(tk_pos[8][0], tk_pos[8][1], ww);*/
	
	//------TrTrack
	histn = Form("%s/TrTrack/L1XYvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, HasTkLHitXY(0), ww);
	
	histn = Form("%s/TrTrack/L1InnerNormChisqYvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetChis(1, 1), ww);
	
	histn = Form("%s/TrTrack/L1ChisqYvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetL1ChisY(), ww);
	
	histn = Form("%s/TrTrack/L1InnerL9NormChisqYvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetChis(2, 1), ww);
	
	histn = Form("%s/TrTrack/L9XYvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, HasTkLHitXY(8), ww);
	
	//------Charge
	//2019.04.30
	for (int ixy=0; ixy<3; ixy++)
	{
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			if (QVer!=-1 && iQVer!=QVer) continue;
			histn = Form("%s/Charge/xy%d/L1Q%dvsRig_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
			h2iVDvsRIG->Fill(dRig, GetTkLQNew(0, ixy, iQVer), ww);
			
			histn = Form("%s/Charge/xy%d/L9Q%dvsRig_xy%d", sStoreDir.c_str(), ixy, iQVer, ixy);
			h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
			h2iVDvsRIG->Fill(dRig, GetTkLQNew(8, ixy, iQVer), ww);
		} //iQVer
	} //ixy
	
	return 1;
}

int SelEvent::StoreVarDisTofRIG(const string sDir, const int Span, const double ww, const int QVer)
{
	string sStoreDir = sDir+"/"+sVarDis[2];
	const double dRig = GetRigidity(Span);
	//------Good Particle
	histn = Form("%s/GoodParticle/ibetahvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, ibetah,ww);
	
	histn = Form("%s/GoodParticle/tof_btypevsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, tof_btype,ww);
	
	histn = Form("%s/GoodParticle/tof_betahvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, tof_betah,ww);
	
	//------Charge
	histn = Form("%s/Charge/UTofQvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetTOFUDQ(0),ww);
	
	histn = Form("%s/Charge/LTofQvsRig", sStoreDir.c_str());
	h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
	h2iVDvsRIG->Fill(dRig, GetTOFUDQ(1),ww);
	
	for (int ilay=0; ilay<4; ilay++)
	{
		histn = Form("%s/Charge/TofQvsRig_l%d", sStoreDir.c_str(), ilay+1);
		h2iVDvsRIG = static_cast<TH2I*>(fOutput->Get(histn.c_str()));
		h2iVDvsRIG->Fill(dRig, tof_ql[ilay],ww);
	} //ilay
	
	return 1;
}

//2019.04.02: unbiased variable, for efficiencies
int SelEvent::StoreVarDisUBExtTrkTrkCor(const string sDir, const double ww, const int QVer) {return 1;}
int SelEvent::StoreVarDisUBTof(const string sDir, const double ww, const int QVer) {return 1;}
int SelEvent::StoreVarDisUBExtTrkTofCor(const string sDir, const double ww, const int QVer) {return 1;}

//------
/*bool SelEvent::Select_Trigger(int opt)
{
	bool trigcut=((physbpatt2&0x3EL)!=0);//Trigger ISS //'L' in 0x3EL stands for long
	if (!isreal) trigcut = ((physbpatt1&0x3EL)!=0);//Trigger MC
	if (opt==1) {
		trigcut = (physbpatt2==1);
		if (!isreal) trigcut = (physbpatt1==1);
	}
	else if (opt==2) {
		trigcut = (physbpatt2&1);
		if (!isreal) trigcut = (physbpatt1&1);
	}
	return trigcut;   
}*/

//2020.06.02
bool SelEvent::Select_Trigger(int opt)
{
	int PhysTrigBPatt=(isreal)?physbpatt2:physbpatt1;
	//check whether force trigger setting for MC
	if (!isreal && iTrigSetting>0) PhysTrigBPatt=RebuildTrig((iTrigSetting==1)?5:8);
	
	bool trigcut=0;
	if (opt==0) trigcut=((PhysTrigBPatt&0x3EL)!=0);//Trigger //'L' in 0x3EL stands for long
	else if (opt==1) trigcut = (PhysTrigBPatt==1);
	else if (opt==2) trigcut = (PhysTrigBPatt&1);
	
	return trigcut;   
}

//2020.08.12
bool SelEvent::Select_TriggerRB(const int opt, const int inacc)
{
	int PhysTrigBPatt=RebuildTrig(inacc);
	
	bool trigcut=0;
	if (opt==0) trigcut=((PhysTrigBPatt&0x3EL)!=0);//Trigger //'L' in 0x3EL stands for long
	else if (opt==1) trigcut = (PhysTrigBPatt==1);
	else if (opt==2) trigcut = (PhysTrigBPatt&1);
	
	return trigcut;   
}

bool SelEvent::Select_TrigUnb()
{
	bool trigcut=((physbpatt2&~0x3EL)!=0);//Trigger ISS
	if (!isreal) trigcut = ((physbpatt1&~0x3EL)!=0);//Trigger MC
	return trigcut;   
}

bool SelEvent::Select_TrigUnb2()
{
	bool trigcut = (physbpatt2&1);
	if (!isreal) trigcut = (physbpatt1&1);
	trigcut = trigcut && !Select_Trigger(0);
	return trigcut;   
}

#ifdef AFSRTI
bool SelEvent::Select_RTI(AMSSetupR::RTI &a)
{
	bool cut[10]={0};
	cut[0]=(a.ntrig/a.nev>0.98);
	cut[1]=(a.npart/a.ntrig>0.07/1600*a.ntrig&&a.npart/a.ntrig<0.25);
	cut[2]=(a.lf>0.5);
	cut[3]=(a.zenith<40);
	cut[4]=(a.nerr>=0&&a.nerr/a.nev<0.1);
	cut[5]=(a.npart>0&&a.nev<1800);
	cut[6]=((a.good&0x3F)==0);
	cut[7]=(a.IsInSAA()==0);
	bool rticut = (cut[0] && cut[1] && cut[2] && cut[3] && cut[4] && cut[5] && cut[6] && cut[7]);
	return rticut;
}

bool SelEvent::Select_RTI()
{
	bool rticut=true;
	if (isreal) {
		AMSSetupR::RTI a;
//		if(AMSEventR::GetRTI(a,time[0])!=0) return -1;
		
		cout << "start to  get RTI: " << endl;
		int rtiget=AMSEventR::GetRTI(a,time[0]);
		cout << "finish get RTI: " << rtiget << endl;
		if(rtiget!=0) return false;
		
		rticut=Select_RTI(a);
	}
	return rticut;
}

bool SelEvent::Select_RTI_dst()
{
	bool rticut=true;
	bool cut[10]={0};
	if (isreal) {
		cut[0]=(rtintrig/rtinev>0.98);
		cut[1]=(rtinpar/rtintrig>0.07/1600*rtintrig&&rtinpar/rtintrig<0.25);
		cut[2]=(rtilf>0.5);
		cut[3]=(rtinerr>=0&&rtinerr/rtinev<0.1);
		cut[4]=(rtinpar>0&&rtinev<1800);
		cut[5]=(zenith<40);
		cut[6]=((rtigood&0x3F)==0);
		cut[7]=(issaa==0);
		cut[8]=(irti>=0); //if it's MC or (AMSEventR::GetRTI(a,time[0])!=0), then irti=-1
		rticut = (cut[0] && cut[1] && cut[2] && cut[3] && cut[4] && cut[5] && cut[6] && cut[7] && cut[8]);
	}
	
	return rticut;
}

bool SelEvent::Select_RTIdL1L9(unsigned int t)
{
	if((amsdn%100)>=68) return true; //2023.05.05
	AMSPoint pn1, pn9, pd1, pd9;
	AMSEventR::GetRTIdL1L9(0, pn1, pd1, t, 60);
	AMSEventR::GetRTIdL1L9(1, pn9, pd9, t, 60);
	bool aligncut = (pd1.y() < 35 && pd9.y() < 45);
	return aligncut;
}

bool SelEvent::Select_RTIdL1L9()
{
	return (isreal)?(Select_RTIdL1L9(time[0])):true;
	//return (Select_RTIdL1L9(time[0]));
}
#else
//#if defined USEQTOOL && defined GBATCH
#if defined USEQRTI && defined GBATCH
bool SelEvent::Select_RTI()
{
	AMSSetupR::RTI a;
	if(AMSEventR::GetRTI(a,time[0])!=0) return false; //Time no information
	return QRTIManager::Select_RTI(a);
}
#else
//--use dst-stored RTI
bool SelEvent::Select_RTI()
{
	bool rticut=true;
	bool isphorun=(run>=1620025528&&run<1635856717); //2023.02.25
	bool cut[10]={0};
	if (isreal)
	{
		cut[0]=(rtintrig/rtinev>0.98);
		//cut[1]=(rtinpar/rtintrig>0.07/1600*rtintrig&&rtinpar/rtintrig<0.25);
		cut[1]=(rtinpar/rtintrig>(isphorun?0.02:0.07/1600*rtintrig)&&rtinpar/rtintrig<0.25); //2023.02.25: optimized for photon trigger period
		//cut[2]=(rtilf>0.5);
		cut[2]=(rtilf>(isphorun?0.35:0.5)); //2023.02.25: optimized for photon trigger period
		cut[3]=(rtinerr>=0&&rtinerr/rtinev<0.1);
		cut[4]=(rtinpar>0&&rtinev<1800);
		cut[5]=(zenith<40);
		cut[6]=((rtigood&0x3F)==0);
		cut[7]=(issaa==0);
		cut[8]=(irti>=0); //if it's MC or (AMSEventR::GetRTI(a,time[0])!=0), then irti=-1
		//rticut = (cut[0] && cut[1] && cut[2] && cut[3] && cut[4] && cut[5] && cut[6] && cut[7] && cut[8]);
		//additional RTI cut for PASS8
		cut[9]=1;
		if((amsdn%100)>=68) cut[9]=(rtinexl[0][0]>700&&rtinexl[1][0]>500);
		//Test for run 2025.08.21
		cut[9]=(rtinexl[0][0]>700&&rtinexl[1][0]>500);
		rticut = (cut[0] && cut[1] && cut[2] && cut[3] && cut[4] && cut[5] && cut[6] && cut[7] && cut[8] && cut[9]);
	}
	
	return rticut;
}
#endif //USEQRTI && GBATCH
#endif //AFSRTI

#ifdef USEQTOOL
bool SelEvent::Select_Run(unsigned int r)
{
	if (qrm->qbadrunlist.size()==0) SelectGoodRun(); //add bad run list if empty
	return qrm->IsSelRun(r) && !qrm->IsBadRun(run,0,1);
}
#else
bool SelEvent::Select_Run(unsigned int r)
{
	bool bad1 = (r != 1306219312);
	bool bad2 = (r != 1306219522);
	bool bad3 = (r != 1306233745);
	bool bad4 = (r < 1307125541) || (r > 1307218054);
	bool bad5 = (r != 1321198167);
	bool bad6 = (r < 1411991495) || (r > 1417187199); //ttcs-off period
	bool bad7 = (r < 1454830398) || (r > 1454932730); //trigger setting test period
	//return (bad1 && bad2 && bad3 && bad4 && bad5 && bad6);
	return (bad1 && bad2 && bad3 && bad4 && bad5 && bad6 && bad7);
}
#endif //USEQTOOL

bool SelEvent::Select_Particle()
{
//	bool parcut=(itrtrack>=0&&ibetah>=0&&tof_btype<10);
//	bool parcut=(itrtrack>=0&&ibetah>=0&&tof_btype==1);
	bool parcut=(itrtrack>=0&&ibetah>=0&&((GetTkInZ()<=2)?(tof_btype<5):(tof_btype==1)));
	return parcut;
}

int SelEvent::Select_Particle_succ(int mode, vector<bool> &vbSel)
{
	vbSel.clear();
	static bool bTrtrack, bBetah, bTofBtype;
	bTrtrack = (itrtrack>=0);
	bBetah = (ibetah>=0);
	bTofBtype = ((GetTkInZ()<=2)?(tof_btype<5):(tof_btype==1));
	
	if (mode>2 || mode<0) {cout << "Please choose the correct mode for Select_Particle." << endl; return false;}
	else if (mode==0) { //successive
		vbSel.push_back(bTrtrack);
		vbSel.push_back(bTrtrack && bBetah);
		vbSel.push_back(bTrtrack && bBetah && bTofBtype);
	}
	else if (mode==1) { //only 1
		vbSel.push_back(bTrtrack);
		vbSel.push_back(bBetah);
		vbSel.push_back(bTofBtype);
	}
	else if (mode==2) { //N-1
		vbSel.push_back(bBetah && bTofBtype);
		vbSel.push_back(bTrtrack && bTofBtype);
		vbSel.push_back(bTrtrack && bBetah);
	}
	return vbSel.size();
}


bool SelEvent::Select_Beta() {
	//2021.09.26: now will do selection of He as well, do the decision depending on old charge, as for heavy nuclei the old charge still cannot be smaller than 2
	return tof_betah>((GetTkInZ()<=2)?0.3:0.4);
	
	//2020.08.29: do not judge depends on GetTkInZ(), which is old charge, might be small number for heavy nuclei
	return tof_betah>0.4;
	
	//return tof_betah>((GetTkInZ()<=2)?0.3:0.4); //2019.06.01: logic operation have high priority than condition operator
	//return (tof_betah>(GetTkInZ()<=2)?0.3:0.4);
//	return (tof_betah>0.4);
}

bool SelEvent::Select_TofChis(){
	bool ToFChisT = (tof_chist<20);
	bool ToFChisC = (tof_chisc<20);
	return (ToFChisT && ToFChisC);
}

bool SelEvent::Select_Tk2nd()
{
	//return true;
	//2019.11.07: enable the Select of 2nd track to be used to clean the selected L1 charge template
	int nhit2i[2]={0};
	for(int ilay=0+1; ilay<9-1; ilay++) //L2 to L8 (inner Tr)
	{
		for(int ixy=0; ixy<2; ixy++)
		{
			if((betah2hb[ixy]&(1<<ilay))>0) nhit2i[ixy]++;
		}
	}
	bool tkcut2=(nhit2i[0]>=3&&nhit2i[1]>=5);//secondary Tk
	bool cuttk2nd=!(tkcut2&&betah2r/tof_betah>0&&fabs(betah2r)>0.5); //2019.11.07: might think of changing the selection
	return cuttk2nd;
}

//2019.11.25
bool SelEvent::Select_Tk2nd2(const double dRigCut)
{
	int nhit2i[2]={0};
	for(int ilay=0+1; ilay<9-1; ilay++) //L2 to L8 (inner Tr)
	{
		for(int ixy=0; ixy<2; ixy++)
		{
			if((betah2hb[ixy]&(1<<ilay))>0) nhit2i[ixy]++;
		}
	}
	bool tkcut2=(nhit2i[0]>=3&&nhit2i[1]>=5);//secondary Tk
	bool bRCut=fabs(betah2r)>dRigCut;
	bool cuttk2nd=!(tkcut2&&betah2r/tof_betah>0&&bRCut);
	if (dRigCut<0) cuttk2nd=!(tkcut2&&betah2r/tof_betah>0);
	return cuttk2nd;
}

//2020.03.05
bool SelEvent::Select_Tk2ndRecon()
{
	int nhit2i[2]={0};
	for(int ilay=0+1; ilay<9-1; ilay++) //L2 to L8 (inner Tr)
	{
		for(int ixy=0; ixy<2; ixy++)
		{
			if((betah2hb[ixy]&(1<<ilay))>0) nhit2i[ixy]++;
		}
	}
	bool tkcut2=(nhit2i[0]>=3&&nhit2i[1]>=5);//secondary Tk
	return tkcut2;
}

/*#ifdef AMSSOFT
bool SelEvent::Select_Rig(int opt, int deg, int igeom) {
	AMSSetupR::RTI a;
	if(AMSEventR::GetRTI(a,time[0])!=0) return 0;
	bool rigcut = false;	//rigidity cut (IGRF) - change to 30deg
	if (opt == 0) //old method
		rigcut = (GetRigidity(igeom)>1.2*a.cfi[deg][1]);
	else if (opt == 1) { //new method - bin cutoff
		int bin=0;
		while ((nuclei_bins[bin] < 1.2*a.cfi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	else if (opt == 2) { //new method - bin cutoff, but using rti in dst
		int bin=0;
		while ((nuclei_bins[bin] < 1.2*mcutoffi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	return rigcut;
}
#else
bool SelEvent::Select_Rig(int opt, int deg, int igeom) {
	bool rigcut = false;	//rigidity cut (IGRF) - change to 30deg
	if (opt >= 0) { //new method - bin cutoff, but using rti in dst
		int bin=0;
		while ((nuclei_bins[bin] < 1.2*mcutoffi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	return rigcut;
}
#endif //AMSSOFT*/
//2020.01.28
#ifdef AMSSOFT
bool SelEvent::Select_Rig(const int opt, const int deg, const int igeom, const double dSafety)
{
	//2020.02.14 ... 2024.02.18 for MC //
	if (!isreal) return true;
	
	AMSSetupR::RTI a;
	if(AMSEventR::GetRTI(a,time[0])!=0) return 0;
	bool rigcut = false;	//rigidity cut (IGRF) - change to 30deg
	if (opt == 0) //old method
		rigcut = (GetRigidity(igeom)>dSafety*a.cfi[deg][1]);
	else if (opt == 1) //new method - bin cutoff
	{
		int bin=0;
		while ((nuclei_bins[bin] < dSafety*a.cfi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	else if (opt == 2) //new method - bin cutoff, but using rti in dst
	{
		int bin=0;
		while ((nuclei_bins[bin] < dSafety*mcutoffi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	return rigcut;
}
#else
/*bool SelEvent::Select_Rig(const int opt, const int deg, const int igeom, const double dSafety)
{
	//2020.02.14
	if (!isreal) return true;
	
	bool rigcut = false;	//rigidity cut (IGRF) - change to 30deg
	if (opt >= 0) //new method - bin cutoff, but using rti in dst
	{
		int bin=0;
		while ((nuclei_bins[bin] < dSafety*mcutoffi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	return rigcut;
}*/

//2020.08.24
bool SelEvent::Select_Rig(const int opt, const int deg, const int igeom, const double dSafety, const int ialgo, const int ipgc)
{
	if (!isreal) return true;
	
	bool rigcut = false;	//rigidity cut (IGRF) - change to 30deg
	if (opt >= 0) //new method - bin cutoff, but using rti in dst
	{
		int bin=0;
		while ((nuclei_bins[bin] < dSafety*mcutoffi[deg][1]) && bin <= nuclei_nbin) bin++; //find out the lower edge of the bin 1.2*R_c falls in
		rigcut = (GetRigidity(igeom, ialgo, ipgc)>nuclei_bins[bin]);	//since do bin++ after judgement, it's the upper edge
	}
	return rigcut;
}
#endif //AMSSOFT

bool SelEvent::Select_RigN(double cutoffv,TH1 *hev,double margin, int igeom) {
	bool rigcut=false;
	static double prcutoffv=0;
	static TH1 *prhev=0;
	static double prmargin=0;
	static double prbincutoff=0;
	if(cutoffv==prcutoffv&&hev==prhev&&prmargin==margin){
		rigcut = (GetRigidity(igeom)>=prbincutoff);
		return rigcut;
	}
	double cutoffvr=cutoffv*margin;
	int ibrc=hev->FindBin(cutoffvr);
	double bincutoff=hev->GetBinLowEdge(ibrc+1);//LowEdge;
	{
		prcutoffv=cutoffv;
		prhev=hev;
		prmargin=margin;
		prbincutoff=bincutoff;
	}
	rigcut = (GetRigidity(igeom)>=bincutoff);
	return rigcut;
}

//bool SelEvent::Select_BetaCutoff(const int Z, const int A, const int ibeta, const int opt, const int deg, const double dSafety, const bool bVerbose)
bool SelEvent::Select_BetaCutoff(const int Z, const int A, const int ibeta, const int opt, const int deg, const double dSafety, const bool bMaximumCutoff, const bool bVerbose) //2021.09.27
{
	if (!isreal) return true;
	
	bool betacut = false; //beta cutoff, converted from rigidity cutoff (IGRF) - change to 30deg
	//get the beta and Ekn
	double dBeta;
	if (ibeta==0) dBeta = tof_betah;
	else dBeta = rich_beta[0];
	double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
	
	//calculate the corresponding beta and Ekn cutoff
	//double dBetaCutoff = RigToBeta(Z, A, mcutoffi[deg][1]);
	//2021.09.27: decide whether using maximum cutoff or the cutoff calculated using the reconstructed direction
	double dRigCutoff = (bMaximumCutoff)?mcutoffi[deg][1]:cutoffpi[1];
	//if (!bMaximumCutoff) cout << "Not using Maximum cutoff, dRigCutoff=" << dRigCutoff << " " << cutoffpi[1] << endl; //XXX
	double dBetaCutoff = RigToBeta(Z, A, dRigCutoff);
	if (dBetaCutoff*dSafety>=1.) return false;
	double dEknCutoff = (1./sqrt(1-pow(dSafety*dBetaCutoff,2))-1)*MPROTON;
	if (bVerbose) cout << Z << " " << A << " " << dSafety << " " << dBeta << " " << dEknRec << " " << dBetaCutoff << " " << dEknCutoff << endl; //XXX
	if (opt >= 0) //new method - bin cutoff, now using the Li Ekn Bin from Lauren //2021.09.07
	{
		int bin=0;
		while ((pLiBins[bin] < dEknCutoff) && bin <= iLiNBin) bin++; //find out the lower edge of the bin f*Beta_c falls in
		if (bin<=iLiNBin) betacut = (dEknRec>pLiBins[bin]); //since do bin++ after judgement, it's the upper edge
		else return false;
		if (bVerbose) cout << bin << " " << pLiBins[bin] << endl; //XXX
	}
	return betacut;
}

//2021.09.29
bool SelEvent::CalCutoffpi(const bool bVerbose)
{
	if (bCutoffpi)
	{
		cout << "Cutoffpi already calcualted, fCutoffpi=" << fCutoffpi << endl;
		return false;
	}
	
	//------return generated rigidity for MC
	if (!isreal)
	{
		bCutoffpi=true;
		fCutoffpi=mmom/mch;
		return true;
	}
	
	//------stop if no TrTrack
	if (itrtrack<0)
	{
		bCutoffpi=true;
		fCutoffpi=0.;
		return false;
	}
	
	int cfstat=-1;
	
	#ifdef GBATCH
	#ifdef CALCUTOFFPI
	//------
	//AMSEventR *pev=GetAMSEventR(bVerbose);
	AMSEventR *pev=GetAMSEventR(false, bVerbose); //2022.06.15
	if (pev==NULL)
	{
		if (bVerbose) cout << "SelEvent::GetCutoffpi: empty AMSEventR, run=" << run << " event=" << event << " itrtrack=" << itrtrack << endl;
		return false;
	}
	TrTrackR *trk=pev->pTrTrack(itrtrack);
	if (trk==NULL)
	{
		if (bVerbose) cout << "SelEvent::GetCutoffpi: empty trk, run=" << run << " event=" << event << " itrtrack=" << itrtrack << endl;
		return false;
	}
	
	int mfit=trk->iTrTrackPar(1,3,20);
	double obeta = tof_betah;
	if (ibetah>=0 && mfit>=0)
	{
		double cfp=0;
		AMSPoint postr;AMSDir dirtr;
		trk->InterpolateLayerJ(obeta>0?1:9,postr,dirtr,mfit);//Non-Hit
		if(dirtr[2]>0)dirtr=dirtr*(-1);
		if(obeta<0)dirtr=dirtr*(-1);
		int sign=1;
		cfstat=pev->GetIGRFCutoff(cfp,sign,dirtr);
		if (cfstat==0) fCutoffpi=cfp;
		if (cfstat==0 && bVerbose) cout << " ****** SelEvent::CalCutoffpi: successfully calculate the cutoff ****** " << run << " " << event << " fCutoffpi=" << fCutoffpi<< endl;
	}
	#endif //CALCUTOFFPI
	#endif //GBATCH
	
	bCutoffpi=true;
	
	return (cfstat==0)?true:false;
}

/*int SelEvent::Select_Trd(const int opt, const int ispan) const
{
	if (opt==0) //trd variables in MIT dst
	{
		if (trd_statk>-1000){
			if (trd_nhitk>=15 && trd_qk[0]) {
				return trd_statk;
			}
			else return -2;
		}
		else return -1;
	}
	else if (opt==1) //new variables in Trd dst
	{
		if (trd_statkN[0]>-1000){
			if (trd_nhitkN[0]>=15 && trd_qkN[0][0][0]) {
				return trd_statkN[0];
			}
			else return -2; 
		}
		else return -1;
	}
	else {
		cout << "please use correct opt: 0 using MIT dst variable; 1 using new Trd varible" << endl;
		return -3;
	}
	return -4;
}*/

/*bool SelEvent::Select_TrTrack(int igeom, int chisOpt) {
	bool innerNHitY = (GetTkInNHit()>=5);
	
	bool l2H = (HasTkLHitXY(1)>0);
	bool l34H = ((HasTkLHitXY(2)>0)||(HasTkLHitXY(3)>0));
	bool l56H = ((HasTkLHitXY(4)>0)||(HasTkLHitXY(5)>0));
	bool l78H = ((HasTkLHitXY(6)>0)||(HasTkLHitXY(7)>0));
	bool innerHitYP = l2H && l34H && l56H && l78H;
	
	bool l1H = (HasTkLHitXY(0)==2);
	bool l9H = (HasTkLHitXY(8)==2);
	
	bool innerHitChiY = (chisOpt%10>=1)?(GetChis(0, 1)<10):true;									//Inner
	bool l1innerHitChiY = (chisOpt%1000>=100)?((GetChis(1, 1)<10)&&((chisOpt%100>=10)?(GetL1ChisY()<10):true)):true;	//l1Inner
	bool l1innerl9HitChiY = (chisOpt%1000>=100)?(GetChis(2, 1)<10):true;								//l1Innerl9
	bool innerl9HitChiY = (chisOpt%1000>=100)?(GetChis(3, 1)<10):true;								//Innerl9
	
	bool S_TrTrack = innerNHitY && innerHitYP && innerHitChiY;
	
	if (igeom == 1) S_TrTrack = S_TrTrack && l1H && l1innerHitChiY;			//L1inner
	else if (igeom == 2)S_TrTrack = S_TrTrack && l1H && l9H && l1innerl9HitChiY;	//FS
	else if (igeom == 3) S_TrTrack = S_TrTrack && l9H && innerl9HitChiY;		//innerL9 (L9 Eff)
	//else if (igeom == 3) S_TrTrack = S_TrTrack && l9H;					//innerL9 (L9 Eff)
	//else if (igeom == 4) S_TrTrack = S_TrTrack && l9H && innerl9HitChiY;
	return S_TrTrack;
}*/

//2020.08.24
bool SelEvent::Select_TrTrack(const int igeom, const int chisOpt, const int ialgo, const int ipgc) const
{
	bool innerNHitY = (GetTkInNHit()>=5);
	
	bool l2H = (HasTkLHitXY(1)>0);
	bool l34H = ((HasTkLHitXY(2)>0)||(HasTkLHitXY(3)>0));
	bool l56H = ((HasTkLHitXY(4)>0)||(HasTkLHitXY(5)>0));
	bool l78H = ((HasTkLHitXY(6)>0)||(HasTkLHitXY(7)>0));
	bool innerHitYP = l2H && l34H && l56H && l78H;
	
	bool l1H = (HasTkLHitXY(0)==2);
	bool l9H = (HasTkLHitXY(8)==2);
	
	bool innerHitChiY = (chisOpt%10>=1)?(GetChis(0, 1, ialgo, ipgc)<10):true;											//Inner
	bool l1innerHitChiY = (chisOpt%1000>=100)?((GetChis(1, 1, ialgo, ipgc)<10)&&((chisOpt%100>=10)?(GetL1ChisY(ialgo, ipgc)<10):true)):true;	//l1Inner
	bool l1innerl9HitChiY = (chisOpt%1000>=100)?(GetChis(2, 1, ialgo, ipgc)<10):true;										//l1Innerl9
	bool innerl9HitChiY = (chisOpt%1000>=100)?(GetChis(3, 1, ialgo, ipgc)<10):true;										//Innerl9
	
	bool S_TrTrack = innerNHitY && innerHitYP && innerHitChiY;
	
	if (igeom == 1) S_TrTrack = S_TrTrack && l1H && l1innerHitChiY;			//L1inner
	else if (igeom == 2)S_TrTrack = S_TrTrack && l1H && l9H && l1innerl9HitChiY;	//FS
	else if (igeom == 3) S_TrTrack = S_TrTrack && l9H && innerl9HitChiY;		//innerL9 (L9 Eff)
	//else if (igeom == 3) S_TrTrack = S_TrTrack && l9H;					//innerL9 (L9 Eff)
	//else if (igeom == 4) S_TrTrack = S_TrTrack && l9H && innerl9HitChiY;
	return S_TrTrack;
}

int SelEvent::Select_TrTrack_succ(int mode, vector<bool> &vbSel, int igeom, int chisOpt)
{
	bool innerNHitY = (GetTkInNHit()>=5);
	
	bool l2H = (HasTkLHitXY(1)>0);
	bool l34H = ((HasTkLHitXY(2)>0)||(HasTkLHitXY(3)>0));
	bool l56H = ((HasTkLHitXY(4)>0)||(HasTkLHitXY(5)>0));
	bool l78H = ((HasTkLHitXY(6)>0)||(HasTkLHitXY(7)>0));
	bool innerHitYP = l2H && l34H && l56H && l78H;
	
	bool l1H = (HasTkLHitXY(0)==2);
	bool l9H = (HasTkLHitXY(8)==2);
	
	bool innerHitChiY = (chisOpt%10>=1)?(GetChis(0, 1)<10):true;									//Inner
	bool l1innerHitChiY = (chisOpt%1000>=100)?((GetChis(1, 1)<10)&&((chisOpt%100>=10)?(GetL1ChisY()<10):true)):true;	//l1Inner
	bool l1innerl9HitChiY = (chisOpt%1000>=100)?(GetChis(2, 1)<10):true;								//l1Innerl9
	
	vbAllSel.clear();
	vbAllSel.push_back(innerNHitY);
	vbAllSel.push_back(innerHitYP);
	vbAllSel.push_back(innerHitChiY);
	if (igeom==1) { //L1inner
		vbAllSel.push_back(l1H);
		vbAllSel.push_back(l1innerHitChiY);
	}
	else if (igeom==2) { //FS
		vbAllSel.push_back(l1H);
		vbAllSel.push_back(l9H);
		vbAllSel.push_back(l1innerl9HitChiY);
	}
	else if (igeom==3) { //InnerL9 (L9 Eff)
		vbAllSel.push_back(l9H);
	}
	
	return ModeSelection(mode, vbSel);
//------selection for each pattern
//	S_TrTrack = innerNHitY && innerHitYP && innerHitChiY; 						//Inner, common for all
//	S_TrTrack = innerNHitY && innerHitYP && innerHitChiY && l1H && l1innerHitChiY;		//L1Inner
//	S_TrTrack = innerNHitY && innerHitYP && innerHitChiY && l1H && l9H && l1innerl9HitChiY;	//FS
//	S_TrTrack = innerNHitY && innerHitYP && innerHitChiY && l9H;					//InnerL9
}

bool SelEvent::Select_TkGeom(int igeom, int opt)
{
	int mnhitf=0;
	for(int il=0+1;il<9-1;il++) {
		if(IsPassTkL(il)){mnhitf++;}
	}
	bool geomcut=(mnhitf>=5);
	if(opt>=1){
		bool L2=(IsPassTkL(1));
		bool L34=(IsPassTkL(2)||IsPassTkL(3));
		bool L56=(IsPassTkL(4)||IsPassTkL(5));
		bool L78=(IsPassTkL(6)||IsPassTkL(7));
		if(opt==1)geomcut=(L2&&L34&&L56&&L78);
		else      geomcut=(L2&&L34&&L56&&L78&&geomcut);
	}
	if(igeom>=1)geomcut=(geomcut&&IsPassTkL(0)); //l1inner
	if(igeom>=2)geomcut=(geomcut&&IsPassTkL(8));	//FS
	return geomcut;
}

int SelEvent::Select_TkGeom_succ(int mode, vector<bool> &vbSel, int igeom, int opt)
{
	vbAllSel.clear();
	
	int mnhitf=0;
	for(int il=0+1;il<9-1;il++) {
		if(IsPassTkL(il)){mnhitf++;}
	}
	if (opt!=1) vbAllSel.push_back(mnhitf>=5);
	
	if(opt>=1){
		bool L2=(IsPassTkL(1));
		bool L34=(IsPassTkL(2)||IsPassTkL(3));
		bool L56=(IsPassTkL(4)||IsPassTkL(5));
		bool L78=(IsPassTkL(6)||IsPassTkL(7));
		vbAllSel.push_back(L2&&L34&&L56&&L78);
	}
	if(igeom>=1) vbAllSel.push_back(IsPassTkL(0));	//l1inner
	if(igeom>=2) vbAllSel.push_back(IsPassTkL(8));	//FS
	
	return ModeSelection(mode, vbSel);
}

bool SelEvent::Select_ChargeQ8(int igeom, int opt, bool diffTof)
{
	int tkz = GetTkInZ();
	bool InnerQ = (tkz-0.45 < tk_q[1]) && (tk_q[1] < tkz+0.45);									//inner Q
	bool InnerQrms = (tk_qrms[1]<0.55);													//inner Qrms
	bool l1Q = (tkz-0.46-(tkz-3)*0.16 < tk_ql[0]) && (tk_ql[0] < ((tkz<=5)?tkz+0.65:tkz+0.65+(tkz-5)*0.03));	//l1 Q
	bool l1Qs = ((tk_qls[0] & 0x10013D)==0);												//l1 Q stauts
	bool l9Q = (tkz-0.5 < tk_ql[8]) && (tk_ql[8] < tkz+1.5+(tkz-3)*0.06);							//l9 Q   (FS)
	//bool utofQ = (tkz-0.6 < GetuToFQ()) && (GetuToFQ() < tkz+1.5);								//utof Q
	//bool ltofQ = (tkz-0.6 < GetlToFQ());												//ltof Q (FS)
	bool utofQ = (tkz-0.6 < GetTOFUDQ(0)) && (GetTOFUDQ(0) < tkz+1.5);							//utof Q
	bool ltofQ = (tkz-0.6 < GetTOFUDQ(1));												//ltof Q (FS)
	if (opt==1) {
		if (diffTof) {
			utofQ = (tkz-0.6 < GetTOFUDQ(0, igeom)) && (GetTOFUDQ(0, igeom) < tkz+1.5);
			ltofQ = (tkz-0.6 < GetTOFUDQ(1, igeom));
		}
		else {
			utofQ = (tkz-0.6 < GetTOFUDQ(0)) && (GetTOFUDQ(0) < tkz+1.5);
			ltofQ = (tkz-0.6 < GetTOFUDQ(1));
		}
	}
	bool S_Charge = (InnerQ && InnerQrms);
	if (igeom >= 1) S_Charge = S_Charge && l1Q && l1Qs && utofQ;
	if (igeom >= 2) S_Charge = S_Charge && l9Q && ltofQ;
	return S_Charge;
}

bool SelEvent::Select_InnerQ_Q8()
{
	int tkz = GetTkInZ();
	bool bInnerQ = (tkz-0.45 < tk_q[1]) && (tk_q[1] < tkz+0.45);								//inner Q
	bool bInnerQrms = (tk_qrms[1]<0.55);												//inner Qrms
	if (tkz<=2) {
		bInnerQ = (tkz-0.3 < tk_q[1]) && (tk_q[1] < tkz+0.4);
		bInnerQrms = (tk_qrms[1]<0.4);
	}
	else if (tkz>=9) {
		//innerQ & Qrms
		double sigma = 0.15+0.028*(tkz-9);
		double zran = 0.025+2.5*sigma;
		if (zran>0.5) zran=0.5;
		bInnerQ = (tkz-zran < tk_q[1]) && (tk_q[1] < tkz+zran);
		bInnerQrms = (tk_qrms[1]<0.57+0.1*(tkz-9));
		
	}
	return (bInnerQ && bInnerQrms);
}

bool SelEvent::Select_TofQ_Q8(int igeom, bool diffTof)
{
	if (igeom==0) return true;
	
	int tkz = GetTkInZ();
	
	int isp = (diffTof)?igeom:2;
	if (isp==3) isp=2; //if only take LTof, take the FS TofQ
	double utofQ = GetTOFUDQ(0, isp), ltofQ = GetTOFUDQ(1, isp);
	
	bool butofQ = (tkz-0.6 < utofQ) && (utofQ < tkz+1.5);										//utof Q
	bool bltofQ = (tkz-0.6 < ltofQ);													//ltof Q
	if (tkz<=2) {
		//butofQ = (tkz-(tkz==2)?0.75:0.5 < utofQ) && (utofQ < tkz+1.5);
		butofQ = (tkz-((tkz==2)?0.75:0.5) < utofQ) && (utofQ < tkz+1.5); //2020.01.21
		bltofQ = (tkz-0.5 < ltofQ) && ((tkz==1)?(ltofQ < tkz+3.0):true); //set upper limit only for Proton, to avoid selection
//		butofQ = (tkz-0.5 < utofQ) && (utofQ < tkz+1.5); 
//		bltofQ = (tkz-0.5 < ltofQ) && (ltofQ < tkz+3.0);
	}
	else if (tkz>=9) {
		//uTofQ
		double sigma = 0.25+0.009*(tkz-9);
		butofQ = (tkz-2*sigma < utofQ) && (utofQ < tkz+2*sigma); //2sigma
		double rsigma = 0.41+0.013*(tkz-9);
		butofQ = butofQ && (fabs(tof_ql[0]-tof_ql[1])<2*rsigma); //2sigma
//		butofQ = butofQ && (fabs(GetTofLQ(0, isp)-GetTofLQ(1, isp))<2*rsigma); //2sigma
		
		//lTofQ
		bltofQ = (tkz-2*sigma < ltofQ) && (ltofQ < tkz+2*sigma); //2sigma
		bltofQ = bltofQ && (fabs(tof_ql[2]-tof_ql[3])<2*rsigma); //2sigma
//		bltofQ = bltofQ && (fabs(GetTofLQ(2, isp)-GetTofLQ(3, isp))<2*rsigma); //2sigma
	}
	bool bTofQ[4];
	bTofQ[0] = true;
	bTofQ[1] = butofQ;
	bTofQ[2] = butofQ && bltofQ;
	bTofQ[3] = bltofQ;
	return bTofQ[igeom];
}

bool SelEvent::Select_ExtQ_Q8(int igeom)
{
	if (igeom==0) return true;
	int tkz = GetTkInZ();
	bool bl1Q = (tkz-0.46-(tkz-3)*0.16 < tk_ql[0]) && (tk_ql[0] < ((tkz<=5)?tkz+0.65:tkz+0.65+(tkz-5)*0.03));	//l1 Q
	bool bl1Qs = ((tk_qls[0] & 0x10013D)==0);												//l1 Q stauts
	bool bl9Q = (tkz-0.5 < tk_ql[8]) && (tk_ql[8] < tkz+1.5+(tkz-3)*0.06);							//l9 Q   (FS)
	if (tkz<=2) {
		bl1Q = (tkz-0.4 < tk_ql[0]) && (tk_ql[0] < tkz+0.9);
		bl9Q = (tkz-0.4 < tk_ql[8]) && (tk_ql[8] < tkz+0.9);
	}
	else if (tkz>=9) {
		//l1Q
		double sigma = 0.28+0.08*(tkz-9);
		bl1Q = (tkz-3.5*sigma < tk_ql[0]) && (tk_ql[0] < tkz+0.05+2*sigma); //>3.5 Sigma && < 2 Sigma
		
		//l9Q
		bl9Q = (tkz-0.05-1.7*sigma < tk_ql[8]) && (tk_ql[8] < tkz+3.5*sigma); //1.5 Sigma
	}
	bool bExtQ[4];
	bExtQ[0] = true;
	bExtQ[1] = bl1Q && bl1Qs;
	bExtQ[2] = bl1Q && bl1Qs && bl9Q;
	bExtQ[3] = bl9Q;
	return bExtQ[igeom];
}

//------
double SelEvent::GetSubDetQ(const int iqdis, const int iVer, const int iUB)
{
	double fCharge=0.;
	//--get charge and charge cut
	if (iqdis==0) fCharge = GetTkInQNew(2,iVer); //InnerQ
	else if (iqdis==1) fCharge = GetTOFUDQ(0); //UTofQ
	else if (iqdis==2) fCharge = GetTOFUDQ(1); //LTofQ
	else if (iqdis==3) fCharge=0.; //InnInnerQ
	else if (iqdis>=4 && iqdis<=5) //L1Q & L9Q
	{
		int ilay=(iqdis==4)?0:8;
		if (iUB==0) fCharge = GetTkLQNew(ilay, 2, iVer); //L1Q
		else if (iUB==1) fCharge = GetUBExtQTrkCor((ilay==0)?0:1, 2, iVer); //unbiased L1Q corrected by traker track
		else if (iUB==2) fCharge = GetUBExtQBetaCor((ilay==0)?0:1, 2, iVer); //unbiased L1Q corrected by unbiased Trd-Build beta
	}
	return fCharge;
} //GetSubDetQ

/*//------charge cut in twiki, including charge selection for both Q<=8 & Q>8, using GetQSelRange
bool SelEvent::SelectQ_Twiki(const int ich, const int iqdis, const int iVer, const bool bTofLayerCut)
{
	float fCharge=GetSubDetQ(iqdis, iVer);
	double dQSelL=GetQSelRangeTwiki(0, ich, iqdis), dQSelU=GetQSelRangeTwiki(1, ich, iqdis);
	bool bCharge=true;
	if (bTofLayerCut)
	{
		if (iqdis==1 || iqdis==2)
		{
			for (int ilay=2*(iqdis-1); ilay<2*iqdis; ilay++) bCharge = bCharge && (dQSelL< tof_ql[ilay]) && ((dQSelU>9000)?true:(tof_ql[ilay] < dQSelU));
		}
	}
	else bCharge =  (dQSelL< fCharge) && ((dQSelU>9000)?true:(fCharge < dQSelU));
	return bCharge;
}

bool SelEvent::Select_InnerQTwiki(const int ich, const int iVer)
{
	bool bQrms = GetTkInQrmsNew(2, iVer) < ((ich<9)?0.55:(0.57+0.1*(ich-9)));
	return SelectQ_Twiki(ich, 0, iVer) && bQrms;
}

bool SelEvent::Select_TofQTwiki(const int ich, int igeom, const int iVer)
{
	if (igeom==0) return true;
	bool butofQ = SelectQ_Twiki(ich, 1, iVer); //utof Q
	bool bltofQ = SelectQ_Twiki(ich, 2, iVer); //ltof Q
	bool bTofQ[4];
	bTofQ[0] = true;
	bTofQ[1] = butofQ;
	bTofQ[2] = butofQ && bltofQ;
	bTofQ[3] = bltofQ;
	return bTofQ[igeom];
}

bool SelEvent::Select_ExtQTwiki(const int ich, int igeom, const int iVer)
{
	if (igeom==0) return true;
	bool bl1Q = SelectQ_Twiki(ich, 4, iVer);	//l1 Q
	bool bl1Qs = ((tk_qls[0] & 0x10013D)==0);			//l1 Q stauts
	bool bl9Q = SelectQ_Twiki(ich, 5, iVer);	//l9 Q   (FS)
	bool bExtQ[4];
	bExtQ[0] = true;
	bExtQ[1] = bl1Q && bl1Qs;
	bExtQ[2] = bl1Q && bl1Qs && bl9Q;
	bExtQ[3] = bl9Q;
	return bExtQ[igeom];
}

bool SelEvent::Select_ChargeTwiki(const int ich, int igeom, const int iVer)
{
	return Select_InnerQTwiki(ich, iVer) && Select_TofQTwiki(ich, igeom, iVer) && Select_ExtQTwiki(ich, igeom, iVer);
}*/

//------charge cut
double SelEvent::GetQSelRange(const int ilu, const int ich, const int iqdis, const int iVer, const bool bL1QUCut, const float fTight) const
{
	double dQCut = QDis::GetQSelRange(ilu, ich, iqdis, (isreal || bL1QUCut));
	//tighten the cut accordingly
	dQCut = fTight*fabs(ich-dQCut);
	dQCut = ich+((ilu==0)?(-1):1)*dQCut;
	return dQCut;
}

bool SelEvent::Select_SubDetQ(const int ich, const int iqdis, const int iVer, const float fTight, const bool bL1QUCut)
{
	bool bCharge=false;
	
	int QVer=iVer%3;
	float fCharge=0.;
	
	//--get charge and charge cut
	fCharge=GetSubDetQ(iqdis, iVer);
	//double dQSelL=GetQSelRange(0, ich, iqdis, iVer, bL1QUCut), dQSelU=GetQSelRange(1, ich, iqdis, iVer, bL1QUCut);
	//if (fTight!=1) dQSelL=ich-fTight*fabs(ich-dQSelL), dQSelU=ich+fTight*fabs(ich-dQSelU);
	//2023.07.17
	double dQSelL=GetQSelRange(0, ich, iqdis, iVer, bL1QUCut, fTight), dQSelU=GetQSelRange(1, ich, iqdis, iVer, bL1QUCut, fTight);
	
	bCharge =  (dQSelL< fCharge) && ((dQSelU>9000)?true:(fCharge < dQSelU));
	
	return (bCharge);
}

//selection for InnerQ rms //2023.02.20
bool SelEvent::Select_InnerQRMS(const int ich, const int iVer)
{
	bool bQrms = GetTkInQrmsNew(2, iVer) < ((ich<9)?0.55:(0.57+0.1*(ich-9)));
	//2020.06.02
	if (ich>20) bQrms=true;
	//2021.09.26
	if (ich<=2) bQrms=true;
	return bQrms;
}

bool SelEvent::Select_InnerQ(const int ich, const int iVer, const float fTight)
{
	/*bool bQrms = GetTkInQrmsNew(2, iVer) < ((ich<9)?0.55:(0.57+0.1*(ich-9)));
	//2020.06.02
	if (ich>20) bQrms=true;
	//2021.09.26
	if (ich<=2) bQrms=true;
	return Select_SubDetQ(ich, 0, iVer, fTight) && bQrms;*/ //define as Select_InnerQRMS //2023.02.20
	//2023.02.20
	return Select_SubDetQ(ich, 0, iVer, fTight) && Select_InnerQRMS(ich, iVer);
}

bool SelEvent::Select_TofQ(const int ich, int igeom, const int iVer, const float fTight)
{
	//if (igeom==0) return true; //2021.03.22: apply upper tofq for Inner as well
	bool butofQ = Select_SubDetQ(ich, 1, iVer, fTight); //utof Q
	bool bltofQ = Select_SubDetQ(ich, 2, iVer, fTight); //ltof Q
	bool bTofQ[4];
	//bTofQ[0] = true;
	bTofQ[0] = butofQ; //2021.03.22: apply upper tofq for Inner as well
	bTofQ[1] = butofQ;
	bTofQ[2] = butofQ && bltofQ;
	bTofQ[3] = bltofQ;
	return bTofQ[igeom];
}

bool SelEvent::Select_ExtQ(const int ich, int igeom, const int iVer, const float fTight, const bool bL1QUCut)
{
	if (igeom==0) return true;
	bool bl1Q = Select_SubDetQ(ich, 4, iVer, fTight, bL1QUCut);	//l1 Q
	bool bl1Qs = ((tk_qls[0] & 0x10013D)==0);			//l1 Q stauts
	bool bl9Q = Select_SubDetQ(ich, 5, iVer, fTight);	//l9 Q   (FS)
	bool bExtQ[4];
	bExtQ[0] = true;
	bExtQ[1] = bl1Q && bl1Qs;
	bExtQ[2] = bl1Q && bl1Qs && bl9Q;
	bExtQ[3] = bl9Q;
	return bExtQ[igeom];
}

bool SelEvent::Select_Charge(const int ich, int igeom, const int iVer, const float fTight, const bool bL1QUCut)
{
	int QVer=iVer;
	if (ich<=2) QVer=0;
	return Select_InnerQ(ich, QVer, fTight) && Select_TofQ(ich, igeom, QVer, fTight) && Select_ExtQ(ich, igeom, QVer, fTight, bL1QUCut);
}

bool SelEvent::Select_QStatus(int igeom)
{
	bool bQStatus=true;
	bool bUTofNQ=(tof_ql[0]>0) && (tof_ql[1]>0); //two UTof has good charge
	bool bLTofNQ=(tof_ql[2]>0) && (tof_ql[3]>0); //two LTof has good charge
	bool bL1QStatus=((tk_qls[0] & 0x10013D)==0); //L1Q status
	if (igeom==0) bQStatus=true;
	else if (igeom==1) bQStatus=bL1QStatus && bUTofNQ;
	else if (igeom==2) bQStatus=bL1QStatus && bUTofNQ && bLTofNQ;
	return bQStatus;
}

bool SelEvent::Select_WithinTrack(int igeom)
{
	bool bWithinTr = true;
	
	int fl=0, ll=0;
	if (igeom==0) {fl=0+1; ll=9-1;}
	else if (igeom==1) {fl=0; ll=9-1;}
	else if (igeom==2) {fl=0; ll=9;}
	
	for (int il=fl; il<ll; il++){
		bWithinTr = (bWithinTr && PassTkL(il, mevcoo, mevdir));
	}
	return bWithinTr;
	
}

//----functions to select unbiased L1 for MaxL1Q+Inner geometry
bool SelEvent::Select_UnbiasedL1XY(const int iVer) const
{
	return GetUBExtQTrkCor(0, 0, iVer)>0 && GetUBExtQTrkCor(0, 1, iVer)>0;
}

bool SelEvent::Select_UnbiasedL1Q(const int ich, const int iVer) const
{
	return (GetUBExtQTrkCor(0, 2, iVer)>GetQSelRange(0, ich, 4, iVer)) && (GetUBExtQTrkCor(0, 2, iVer)<GetQSelRange(1, ich, 4, iVer)) && ((GetUBExtQStatusTrkCor(0,iVer) & 0x10013D)==0);
}

//----
bool SelEvent::Select_PreSel()
{
	return (Select_Trigger() && Select_Run() && Select_RTI() && Select_Particle() && Select_Beta());
}

bool SelEvent::Select_Pattern(int igeom, int rig_opt, int deg, int trackOpt, int geom_opt)
{
	static bool bPattern = false;
	bPattern = (isreal)?Select_Rig(rig_opt, deg, igeom):true;	//geomagnetic cutoff
	bPattern = bPattern && Select_TrTrack(igeom, trackOpt);	//track cut
	bPattern = bPattern && Select_TkGeom(igeom, geom_opt);	//fiducial volumn
	return bPattern;
}

//------Geometrical acceptance
bool SelEvent::Select_GeomAccept(int igeom)
{
	if (isreal) return false;
	
	//for geometrical acceptance, only require it to pass tracker fiducial volume selection, not all tracker nor passing tof
	bool bTrackerFidu = true;
	bool mlh[9];
	int mnhitf=0;
	for(int il=0;il<9;il++){
		mlh[il]=PassTkL(il, mevcoo, mevdir);
	} //il
	
	for(int il=0+1;il<9-1;il++){
		if(mlh[il]){mnhitf++;}
	} //il
	bTrackerFidu = ( (mnhitf>=5) && (mlh[1]) && (mlh[2] || mlh[3]) && (mlh[4] || mlh[5]) && (mlh[6] || mlh[7]) );
	if (igeom>=1) bTrackerFidu = bTrackerFidu && mlh[0];
	if (igeom>=2) bTrackerFidu = bTrackerFidu && mlh[8];
	return bTrackerFidu;
}

//------primary information //2019.10.13
int SelEvent::CheckPrimary()
{
	if (isreal)
	{
		//cout << "for ISS return -2" << endl;
		return -2;
	}
	
	int nskip=-1;
	for (int iskip=0; iskip<=20; iskip++)
	{
		/*if (mevcoo1[iskip][2]==-1000) continue;
		else
		{
			nskip=iskip;
			break;
		}*/
		//if (mevcoo1[iskip][2]!=-1000) nskip=iskip;
		if (mevmom1[iskip]!=-1000) nskip=iskip; //use mevmom1 to check
		//else break; //should not break, as it might happen (by some reasone) that il=-1000 while il+1 is not. Now will find the largest layer where mevmom1!=-1000 //2023.04.14
		//if (iskip==14) cout << iskip << " " << mevcoo1[iskip][2] << endl;
	} //iskip
	//if (nskip==14 || nskip==15) cout << nskip << " " << mevcoo1[nskip][2] << endl;
	return nskip;
}

//------check whether particle stay primary up to givern Tracker layer //2023.04.14
bool SelEvent::PrimaryUpToTrLayer(const int iLayerJ)
{
	int ilAcc=-1; //the corresponding MC Acceptance plane for given Tracker Layer //should make it a function and put into database //2023.04.14
	if (iLayerJ==1) ilAcc = 0;
	else if (iLayerJ>=2 && iLayerJ<=8) ilAcc=5+iLayerJ;
	else if (iLayerJ==9) ilAcc = 18;
	if (ilAcc<0)
	{
		cout << "SelEvent::PrimaryUpToTrLayer: wrong layer given, ilAcc=" << ilAcc << " iLayerJ=" << iLayerJ << endl; //XXX
		return false;
	}
	return (mevmom1[ilAcc]!=-1000 || mevmom1[ilAcc+1]!=-1000);
}

//------get TrMCCluster Z from mtrz
int SelEvent::GetTrMCLZ(const int il)
{
	if (isreal) return -9999;
	return mtrz[il-1]&0x3f; //see TrMCClusterR::Status() and TrMCCluster::getZ()
}

//------rich quality cut
bool SelEvent::SelectRich() const
{
	if (irich < 0) return false;
	float x = rich_pos[0];
	float y = rich_pos[1];
	unsigned int mask_rich = 0;
	
	if(!rich_good || !rich_clean ) mask_rich = mask_rich | (1<<0);
	if(rich_pmt < kCutRichPmt) mask_rich = mask_rich | (1<<1);
	if(rich_pb < kCutRichProb) mask_rich = mask_rich | (1<<2);          
	if(rich_npe[0]/rich_npe[2] < kCutRichCollOverTotal) mask_rich = mask_rich | (1<<3);
	if(rich_PMTChargeConsistency > kCutRichChargeConsistency) mask_rich = mask_rich | (1<<4);
	//if ( abs(get_betadiff()) > 0.06 ) mask_rich = mask_rich | (1<<5);
	if ( fabs(get_betadiff()) > 0.06 ) mask_rich = mask_rich | (1<<5); //2022.08.01
	
	if(rich_NaF)
	{
		if(rich_BetaConsistency > kCutRichBetaConsistency[0]) mask_rich = mask_rich | (1<<6);
		if(rich_npe[1] < kCutRichExpPhe[0]) mask_rich = mask_rich | (1<<7);
		
		//if(std::max(abs(x),abs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8);
		if(std::max(fabs(x),fabs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8); //2022.08.01
	}
	else
	{
		if(rich_BetaConsistency > kCutRichBetaConsistency[1]) mask_rich = mask_rich | (1<<6);
		if(rich_npe[1] < kCutRichExpPhe[1]) mask_rich = mask_rich | (1<<7);
		
		//if(std::max(abs(x),abs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8);
		if(std::max(fabs(x),fabs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8); //2022.08.01
		if(x*x+y*y > kCutRichAerogelExternalBorder) mask_rich = mask_rich | (1<<8);
		
		for(int ibad = 0; ibad < kNBadTiles ; ibad++)
		{
			if(rich_tile == kBadTile[ibad]) mask_rich = mask_rich | (1<<9);
		}
	}
	return (mask_rich == 0);
}

//--Rich geometrical cut //2021.12.21
//bool SelEvent::Select_RichGeom()
bool SelEvent::Select_RichGeom(bool bGood)
{
	if (irich < 0) return false;
	float x = rich_pos[0];
	float y = rich_pos[1];
	unsigned int mask_rich = 0;
	
	if (bGood) {if(!rich_good || !rich_clean ) mask_rich = mask_rich | (1<<0);}
	//if(rich_pmt < kCutRichPmt) mask_rich = mask_rich | (1<<1);
	//if(rich_pb < kCutRichProb) mask_rich = mask_rich | (1<<2);          
	//if(rich_npe[0]/rich_npe[2] < kCutRichCollOverTotal) mask_rich = mask_rich | (1<<3);
	//if(rich_PMTChargeConsistency > kCutRichChargeConsistency) mask_rich = mask_rich | (1<<4);
	//if ( abs(get_betadiff()) > 0.06 ) mask_rich = mask_rich | (1<<5);
	
	if(rich_NaF)
	{
		//if(rich_BetaConsistency > kCutRichBetaConsistency[0]) mask_rich = mask_rich | (1<<6);
		//if(rich_npe[1] < kCutRichExpPhe[0]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8);
	}
	else
	{
		//if(rich_BetaConsistency > kCutRichBetaConsistency[1]) mask_rich = mask_rich | (1<<6);
		//if(rich_npe[1] < kCutRichExpPhe[1]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8);
		if(x*x+y*y > kCutRichAerogelExternalBorder) mask_rich = mask_rich | (1<<8);
		
		for(int ibad = 0; ibad < kNBadTiles ; ibad++)
		{
			if(rich_tile == kBadTile[ibad]) mask_rich = mask_rich | (1<<9);
		}
	}
	return (mask_rich == 0);
}

bool SelEvent::Select_RichGeomUnbiased(const bool bNaF) //2022.05.30
{
	double dRichPlaneZ = (bNaF)?-75.5:-74.6; //result from Jiahui, need to check myself and consider the difference between Rich and AMS coordinate TODO
	float point[3];
	//extrapolate from tracker L8
	Extrapolate(tk_pos[8-1], tk_dir[8-1], point, dRichPlaneZ);
	float x = point[0];
	float y = point[1];
	unsigned int mask_rich = 0;
	
	//if(rich_pmt < kCutRichPmt) mask_rich = mask_rich | (1<<1);
	//if(rich_pb < kCutRichProb) mask_rich = mask_rich | (1<<2);          
	//if(rich_npe[0]/rich_npe[2] < kCutRichCollOverTotal) mask_rich = mask_rich | (1<<3);
	//if(rich_PMTChargeConsistency > kCutRichChargeConsistency) mask_rich = mask_rich | (1<<4);
	//if ( abs(get_betadiff()) > 0.06 ) mask_rich = mask_rich | (1<<5);
	
	if(rich_NaF)
	{
		//if(rich_BetaConsistency > kCutRichBetaConsistency[0]) mask_rich = mask_rich | (1<<6);
		//if(rich_npe[1] < kCutRichExpPhe[0]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8);
	}
	else
	{
		//if(rich_BetaConsistency > kCutRichBetaConsistency[1]) mask_rich = mask_rich | (1<<6);
		//if(rich_npe[1] < kCutRichExpPhe[1]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8);
		if(x*x+y*y > kCutRichAerogelExternalBorder) mask_rich = mask_rich | (1<<8);
		
		for(int ibad = 0; ibad < kNBadTiles ; ibad++)
		{
			if(rich_tile == kBadTile[ibad]) mask_rich = mask_rich | (1<<9);
		}
	}
	return (mask_rich == 0);
}

//2022.08.01
float SelEvent::GetRichVar(const int ivar)
{
	float fRichVar=0;
	if (ivar==0) fRichVar = rich_pmt;
	else if (ivar==1) fRichVar = rich_pb;
	else if (ivar==2) fRichVar = rich_npe[0]/rich_npe[2];
	else if (ivar==3) fRichVar = rich_PMTChargeConsistency;
	else if (ivar==4) fRichVar = fabs(get_betadiff());
	else if (ivar==5) fRichVar = sqrt(rich_q[0]);
	else if (ivar==6) fRichVar = rich_BetaConsistency;
	else if (ivar==7) fRichVar = rich_npe[1];
	else if (ivar==8) fRichVar = (rich_NaF)?-1:sqrt(rich_pos[0]*rich_pos[0]+rich_pos[1]*rich_pos[1]);
	return fRichVar;
}

bool SelEvent::Select_Rich_N1(const int icut, const int charge)
{
	if (icut<0)
	{
		cout << "SelEvent::Select_Rich_N1: icut should be >0 while icut=" << icut << endl;
		return false;
	}
	
	const int NCut[2] = {8, 10};
	if (irich < 0) return false;
	float x = rich_pos[0];
	float y = rich_pos[1];
	unsigned int mask_rich = 0;
	
	if(rich_pmt < kCutRichPmt) mask_rich = mask_rich | (1<<0);
	if(rich_pb < kCutRichProb) mask_rich = mask_rich | (1<<1);          
	if(rich_npe[0]/rich_npe[2] < kCutRichCollOverTotal) mask_rich = mask_rich | (1<<2);
	if(rich_PMTChargeConsistency > kCutRichChargeConsistency) mask_rich = mask_rich | (1<<3);
	if ( fabs(get_betadiff()) > 0.06 ) mask_rich = mask_rich | (1<<4);
	if ((sqrt(rich_q[0])<charge-1) || (sqrt(rich_q[0])>charge+1.5)) mask_rich = mask_rich | (1<<5);
	
	if(rich_NaF)
	{
		if(rich_BetaConsistency > kCutRichBetaConsistency[0]) mask_rich = mask_rich | (1<<6);
		if(rich_npe[1] < kCutRichExpPhe[0]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8);
	}
	else
	{
		if(rich_BetaConsistency > kCutRichBetaConsistency[1]) mask_rich = mask_rich | (1<<6);
		if(rich_npe[1] < kCutRichExpPhe[1]) mask_rich = mask_rich | (1<<7);
		
		if(x*x+y*y > kCutRichAerogelExternalBorder) mask_rich = mask_rich | (1<<8);
		if(std::max(fabs(x),fabs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<9);
		
		for(int ibad = 0; ibad < kNBadTiles ; ibad++)
		{
			if(rich_tile == kBadTile[ibad]) mask_rich = mask_rich | (1<<10);
		}
	}
	if(!rich_good || !rich_clean ) mask_rich = mask_rich | (1<<11);
	
	//--remove the icut-th selection
	if ((rich_NaF && icut>=NCut[0]) || (!rich_NaF && icut>=NCut[1]))
	{
		cout << "SelEvent::Select_Rich_N1: Exceed maximum icut for " << ((rich_NaF)?"NaF":"AgL") << ", icutM=" << NCut[(rich_NaF)?0:1] << " while icut=" << icut << endl;
		return false;
	}
	mask_rich = mask_rich & (~(1<<icut));
	return (mask_rich == 0);
}

//2022.11.09
bool SelEvent::SelectRichDR(const int icluster, const int idr, const int charge) const
{
	//if (icluster==1) return SelectRich(charge);
	//should require direct or reflected hits > 0 respectively
	if (icluster==1)
	{
		int nhit = (idr==0)?(rich_used-rich_usedm):rich_usedm;
		return SelectRich(charge) && (nhit>0);
	}
	else
	{
		if (irich < 0) return false;
		float x = rich_pos[0];
		float y = rich_pos[1];
		unsigned int mask_rich = 0;
		
		if(!rich_goodDR[idr] || !rich_cleanDR[idr] ) mask_rich = mask_rich | (1<<0);
		if(rich_pmtDR[idr] < kCutRichPmt) mask_rich = mask_rich | (1<<1);
		if(rich_pbDR[idr] < kCutRichProb) mask_rich = mask_rich | (1<<2);          
		if(rich_npeDR[idr][0]/rich_npeDR[idr][2] < kCutRichCollOverTotal) mask_rich = mask_rich | (1<<3);
		if(rich_PMTChargeConsistencyDR[idr] > kCutRichChargeConsistency) mask_rich = mask_rich | (1<<4);
		//if ( abs(get_betadiff()) > 0.06 ) mask_rich = mask_rich | (1<<5);
		if ( fabs((tof_betah-rich_betaDR[idr][0])/rich_betaDR[idr][0]) > 0.06 ) mask_rich = mask_rich | (1<<5); //2022.08.01
		
		if(rich_NaF)
		{
			if(rich_BetaConsistencyDR[idr] > kCutRichBetaConsistency[0]) mask_rich = mask_rich | (1<<6);
			if(rich_npeDR[idr][1] < kCutRichExpPhe[0]) mask_rich = mask_rich | (1<<7);
			
			//if(std::max(abs(x),abs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8);
			if(std::max(fabs(x),fabs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8); //2022.08.01
		}
		else
		{
			if(rich_BetaConsistencyDR[idr] > kCutRichBetaConsistency[1]) mask_rich = mask_rich | (1<<6);
			if(rich_npeDR[idr][1] < kCutRichExpPhe[1]) mask_rich = mask_rich | (1<<7);
			
			//if(std::max(abs(x),abs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8);
			if(std::max(fabs(x),fabs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8); //2022.08.01
			if(x*x+y*y > kCutRichAerogelExternalBorder) mask_rich = mask_rich | (1<<8);
			
			for(int ibad = 0; ibad < kNBadTiles ; ibad++)
			{
				if(rich_tile == kBadTile[ibad]) mask_rich = mask_rich | (1<<9);
			}
		}
		return (mask_rich == 0);
	}
}

double SelEvent::GetBetaDR(const int icluster, const int idr, const int ibetaver) const
{
	double dBeta = 0;
	if (icluster==0) dBeta = rich_betaDR[idr][ibetaver];
	else if (icluster==1) dBeta = rich_betaDROnly[idr][ibetaver];
	return dBeta;
}

//------ //2023.04.23
float SelEvent::GetL1QDisSelVar(const int ivar, const int charge)
{
	//--
	int nqud[2]={0};
	for(int ilay=0;ilay<4;ilay++)
	{
	   if(IsGoodTofLQStat(ilay)) {nqud[ilay/2]++;}
	}
	bool tofgpt=(nqud[0]+nqud[1]==4);
	bool cuttofqz2u=(Select_TofQ(charge, 1)&&fabs(GetTOFUDQ(0)-charge)<0.5);
	bool cuttofqz2d=(Select_TofQ(charge, 3)&&fabs(GetTOFUDQ(1)-charge)<0.5);
	bool cutttof2=(tofgpt&&cuttofqz2u&&cuttofqz2d);
	
	//--
	float tofbq[2]={0};
	for(int ilay=0;ilay<4;ilay++)
	{
		if(tof_oq[ilay][0]>tofbq[1]){tofbq[1]=tof_oq[ilay][0];} //max Q in all 4 layer
		if(tof_oq[ilay][1]>tofbq[0]){tofbq[0]=tof_oq[ilay][1];} //2nd max Q in all 4 layer or max Q that's more than 1 bar away from main track
		if(tof_ob[ilay][0]==-1||tof_barid[ilay]==-1)continue;
		if(abs(tof_barid[ilay]-tof_ob[ilay][0])>1)
		{
			if(tof_oq[ilay][0]>tofbq[0]){tofbq[0]=tof_oq[ilay][0];}
		}
	}
	//--
	float fL1QDisSelVar=-100;
	if (ivar==0) fL1QDisSelVar=ntrack;
	else if(ivar==1)fL1QDisSelVar=nbetah;
	else if(ivar==2)fL1QDisSelVar=ntrdtrack;
	else if(ivar==3)fL1QDisSelVar=tof_hsumhu;
	else if(ivar==4)fL1QDisSelVar=nqud[0]+nqud[1];
	else if(ivar==5){fL1QDisSelVar=ntrdseg;}
	//else if(ivar==6){if(!cutttof2)continue;fL1QDisSelVar=tof_nclhl[0]+tof_nclhl[1]-2;}
	else if(ivar==6){if(cutttof2) fL1QDisSelVar=tof_nclhl[0]+tof_nclhl[1]-2;}
	else if(ivar==7)fL1QDisSelVar=betah2q;
	//else if(ivar==8){if(!cutttof2)continue;fL1QDisSelVar=tofbq[0];}
	else if(ivar==8){if(cutttof2) fL1QDisSelVar=tofbq[0];}
	//else if(ivar==9){if(!cutttof2)continue;fL1QDisSelVar=tofbq[1];}
	else if(ivar==9){if(cutttof2) fL1QDisSelVar=tofbq[1];}
	else if(ivar==10){fL1QDisSelVar=ntrdrawh;}
	else if(ivar==11){fL1QDisSelVar=(trd_qk[0]!=0)?trd_qk[4]-charge:-4;}
	else if(ivar==12){fL1QDisSelVar=(trd_qk[0]!=0)?trd_qk[3]-charge:-4;}
	else if(ivar==13){fL1QDisSelVar=ntrrawcl;}
	else if(ivar==14){fL1QDisSelVar=nrichh;}
	return fL1QDisSelVar;
}

//------ //2023.02.16
void SelEvent::SelectEv()
{
	bTrig = Select_Trigger();
	#ifdef FORCEOLDTRIGSETTING
	bTrig = Select_TriggerRB(0,5);
	#endif //FORCEOLDTRIGSETTING
	
	bRun = Select_Run(); //2023.02.25: to be updated to QRunManager //2023.02.28: done, with USEQTOOL flag
	bRTI = Select_RTI(); //2023.02.25: to be updated to QRTIManager //2023.02.28: done, with USEQRTI flag, the selection using dst-stored one is also updated
	bRTIdL1L9 = Select_RTIdL1L9();
	bPart = Select_Particle();
	bBeta = Select_Beta();
	bTk2nd = Select_Tk2nd();

	for (int ir=0; ir<3; ir++)
	{
		/*//--
		bRig[ir] = Select_Rig(1, icffv, ir, dGeoSafety);
		
		#ifdef NOCUTOFFEFF
		bRigEff[ir]=true;
		#else
		if (!isreal) bRigEff[ir]=true;
		else bRigEff[ir] = Select_Rig(1, icffv, ir, dGeoSafety);
		#endif //NOCUTOFFEFF
		
		//--
		bTrack[ir] = Select_TrTrack(ir);	//have L1Chis cut
		//bTrack[ir] = Select_TrTrack(ir, 101);	//no L1Chis cut*/
		
		//store different algorithm as well //2023.03.10
		for (int irig=0; irig<NRig; irig++)
		{
			//--
			bRig[ir][irig] = Select_Rig(1, icffv, ir, dGeoSafety, irig);
			
			#ifdef NOCUTOFFEFF
			bRigEff[ir][irig]=true;
			#else
			if (!isreal) bRigEff[ir][irig]=true;
			//else bRigEff[ir][irig] = Select_Rig(1, icffv, ir, dGeoSafety, irig);
			else bRigEff[ir][irig] = bRig[ir][irig];
			#endif //NOCUTOFFEFF
			
			//--
			bTrack[ir][irig] = Select_TrTrack(ir, 111, irig);	//have L1Chis cut
			//bTrack[ir][irig] = Select_TrTrack(ir, 101);	//no L1Chis cut
		} //irig
		
		//-- //different geom for different fitting algorithm?
		bTkgeom[ir] = Select_TkGeom(ir, iFVTight);
		//for ir=0, require the Tracker L1 geometry as well if using unbiased L1Q cut
		#ifdef INNERUNBIAEDL1Q
		if (ir==0) bTkgeom[ir] = Select_TkGeom(1, iFVTight);
		#endif //INNERUNBIAEDL1Q
	}
}

//------rich quality cut
bool SelEvent::SelectRichB()
{
	if (irich < 0) return false;
	float x = rich_pos[0];
	float y = rich_pos[1];
	unsigned int mask_rich = 0;
	
	//if(!rich_good || !rich_clean ) mask_rich = mask_rich | (1<<0);
	//if(rich_pmt < kCutRichPmt) mask_rich = mask_rich | (1<<1);
	if(rich_pbp < kCutRichProb) mask_rich = mask_rich | (1<<2);          
	//if(rich_npe[0]/rich_npe[2] < kCutRichCollOverTotal) mask_rich = mask_rich | (1<<3);
	//if(rich_PMTChargeConsistency > kCutRichChargeConsistency) mask_rich = mask_rich | (1<<4);
	if ( fabs(get_betadiffB()) > 0.06 ) mask_rich = mask_rich | (1<<5);
	
	if(rich_NaF)
	{
		if(rich_BetaConsistency > kCutRichBetaConsistency[0]) mask_rich = mask_rich | (1<<6);
		//if(rich_npe[1] < kCutRichExpPhe[0]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) > kCutRichAerogelNafBorder[0]) mask_rich = mask_rich | (1<<8);
	}
	else
	{
		if(rich_BetaConsistency > kCutRichBetaConsistency[1]) mask_rich = mask_rich | (1<<6);
		//if(rich_npe[1] < kCutRichExpPhe[1]) mask_rich = mask_rich | (1<<7);
		
		if(std::max(fabs(x),fabs(y)) < kCutRichAerogelNafBorder[1]) mask_rich = mask_rich | (1<<8);
		if(x*x+y*y > kCutRichAerogelExternalBorder) mask_rich = mask_rich | (1<<8);
		
		for(int ibad = 0; ibad < kNBadTiles ; ibad++)
		{
			if(rich_tile == kBadTile[ibad]) mask_rich = mask_rich | (1<<9);
		}
	}
	
	return (mask_rich == 0);
}

//------tool function
bool SelEvent::CheckFlag(int status, int bit)
{
	bool flaged = false;
	if (status&(1<<bit)) flaged = true;
	return flaged;
}

int SelEvent::ModeSelection(int mode, vector<bool> &vbSel)
{
	vbSel.clear();
	for (unsigned int isel=0; isel<vbAllSel.size(); isel++) {
		bool bSel=true;
		//successive
		if (mode==0) bSel = (isel==0)?vbAllSel.at(isel):vbAllSel.at(isel)&&vbSel.at(isel-1);
		//only 1
		else if (mode==1) bSel = vbAllSel.at(isel);
		//N-1
		else if (mode==2)
			for (unsigned int isel1=0; isel1<vbAllSel.size(); isel1++) if (isel1!=isel) bSel = bSel && vbAllSel.at(isel1);
		vbSel.push_back(bSel);
	} //it
	return vbSel.size();
}

#ifdef USEQTOOL
int SelEvent::SelectGoodRun(int opt)
{
	if(isreal) //ISS BadRun
	{
		qrm->AddBadRun(1306219312);
		qrm->AddBadRun(1306219522);
		qrm->AddBadRun(1306233745);
		qrm->AddBadRun(1307125541,1307218054);//BADRUN
		qrm->AddBadRun(1321198167);
		//qrm->AddBadRun(1411995797,1417184590);//TTCS Off
		qrm->AddBadRun(1434801178,1434841341);//Bad TOF-SDR
		
		//------
		if ((amsdn%100)>=68) qrm->AddBadRun(badrun,nbadrun);//only applied for pass8 data //badrun and nbadrun defined in include/BadRun.h
		
		//-------
		int opt1=(opt%10);//<
		int opt2=((opt/10)%10);//>
		int opt3=((opt/100)%10);//opt3: =0 remove photon trigger, =1 keep photon trigger, =2 only photon trigger
		                    //2013-11-26(2.5years),2014-09-29(TTCSon),2016-05-26(5years),2017-05-12(6years),2018-05-28(7years),8.5 years
		unsigned int rune[6]={1385483969,          1411991495,        1464298202,        1494599304,        1527490046,     1572344387};
		if(opt1>=1&&opt1<=6){qrm->AddBadRun(rune[opt1-1]+1,2000000000);}//(1,rune[opt1-1]], Run<=rune[opt1-1]
		if(opt2>=1&&opt2<=6){qrm->AddBadRun(1,rune[opt2-1]);}//  (rune[opt2-1],2000000000), Run>rune[opt2-1]
		if(opt2==9) //after new DAQ
		{
		   qrm->AddBadRun(1,1456503197-1);//[1456503197,2000000000), Run>=1456503197
		}
		if     (opt3==0)qrm->AddBadRun(1620025528,1635856717-1);//remove photon trigger period
		else if(opt3==2){qrm->AddBadRun(1,1620025528-1); qrm->AddBadRun(1635856717,2000000000);}//only keep photon trigger period
	}
	return 0;
}
#endif //USEQTOOL
#endif //SELEVENT14_H
