//------------------
//updated from: analysis5.C
//updated date: 2019.06.27
//------------------
//------------------
#define FORCEHZBIN

//--
#define USEQTOOL //2023.02.27

//--
#define UNFOLDING
#define UNFOLDINGCHECK //this will turn on Sumw2=true, reweight according to flux shape
//#define NOISOTOPEMIX //not mix different isotope MC

//--MCreweight
#define MCREWEIGHT
/*#ifdef UNFOLDING
#define MCREWEIGHT
#endif //UNFOLDING*/
//#define NOREWEIGHT
//#define NOMCCOMBINE

//--flag to store NGev
//#define TOTALGENEV
#ifdef TOTALGENEV
#undef UNFOLDING
#undef MCREWEIGHT
#endif //TOTALGENEV

//--charge distribution
// #define CHARGECAL_NEWTRQ
#ifdef CHARGECAL_NEWTRQ
//#define RIG_NEWTRQ
#undef UNFOLDING
#endif //CHARGECAL_NEWTRQ

//*******Exposure Time
// #define EXPOTIME  //// 2023.09.14
#ifdef EXPOTIME
#undef CHARGECAL_NEWTRQ  /// don't know reason
#define UNFOLDING
#endif //EXPOTIME

//--L1Q distribution clean up //2023.04.23
//#define L1QDISSEL

//--Tracker resolution
//#define TRRES
#ifdef TRRES
#define MCFLUXREWEIGHT
#endif //TRRES

//--Efficiecny
// #define ESTCAL //Rigidity calibration
//2021.01.17
#ifdef ESTCAL
#undef UNFOLDING
#endif //ESTCAL
#ifdef UNFOLDING
#define EFFICIENCY
#endif //UNFOLDING

//put reweight the MC using flux result
/*#ifndef EFFICIENCY
#define TOICORRECTION
#define CHARGETOI //2020.10.05
#endif //EFFICIENCY*/
//#define TOICORRECTION //2021.04.01
//#define BELOWL1MC //2023.02.17
//--
#if defined TOICORRECTION || defined BELOWL1MC
#undef EFFICIENCY
#define CHARGETOI
#endif //TOICORRECTION || BELOWL1MC

#ifdef EFFICIENCY
#define TRIGEFF
#define TOFQEFF
#define L1EFF
#define L9EFF
#define TRACKEFFN
//--TkEff only contain reconstruction but without innerQ, i.e. the nearby eff //2020.10.12
//this also includes tighten sample selection for secondary nuclei, so as the nearby primary efficiency used as average, better change name, and store the always store TkEff wo Q TODO //2022.06.05
//#define TKEFFNOQCUT
//--check for efficiency sample contamination
//#define EFFSEC
#endif //EFFICIENCY

//--Efficiency validation
//#define EFFVAL

//--Rigidity resolution
//#define RIGRESO
#ifdef RIGRESO
#undef UNFOLDINGCHECK
#endif //RIGRESO

//--Acceptance, only for MC events
#ifdef UNFOLDING
#define ACCEPTANCE
#ifdef EFFSEC
#undef ACCEPTANCE
#endif //EFFSEC
#define ACCSYS

//use flux reweight also for efficiency calculation
#define MCFLUXREWEIGHT
#endif //UNFOLDING

//--isotopes
//#define ISOTOPE
//apply unbiased L1Q cut for inner pattern
#ifdef ISOTOPE
#define INNERGEOM //store Inner geometery
#define INNERUNBIAEDL1Q
#define ISOTOPEBGSUB //selection on no 2nd track to supress background
#ifndef TOICORRECTION
#define ISOTOPEEV
#define MASSDIS //mass distribution with different cutoff to be fitted by mass template

//#define CHECKRICHINDEXPOS
//#define CHECKRICHREFRACINDEX //check rich refractive index
#ifdef CHECKRICHREFRACINDEX 
#undef INNERUNBIAEDL1Q
#undef ISOTOPEEV
//#define DIRECTIONDEP
//#define LIPRICHBETA
//const int NLTCor=2;
const int NLTCor=1;

//#define NPEWEIGHTED
#ifdef NPEWEIGHTED
const int nWS=3;
#define GETEVENTFAST
#define UNIFORMITY
#define WEIGHTEDNPE

#else
const int nWS=1;
#endif //NPEWEIGHTED
#endif //CHECKRICHREFRACINDEX 

//#define RICHVARCHECK
//#define RICHVARCHECK2D

//--check the rich variable distribution, vs beta resolution for beta~1
//#define RICHVARCHECKRESO
#ifdef RICHVARCHECKRESO
#define RICHVARCHECKRESO2D
#define NOISOTOPEMIX //not mix isotope
#undef ISOTOPEEV
#undef ACCEPTANCE
#define FULLUNIFORMITYCORCHECK //check the beta with fullUniformityCorrection
#endif //RICHVARCHECKRESO
#endif //TOICORRECTION

//--check the rich tile position using no. event vs hit pos
//#define CHECKRICHGEO
#ifdef CHECKRICHGEO
#define NOISOTOPEMIX //not mix isotope
#undef ISOTOPEEV
//#undef ACCEPTANCE
#define ACCEPTANCE
#undef MCREWEIGHT
//--checking rich using RichMCCluster
//#define CHECKMCCLUSTER
//--add branches for only direct and reflected hits, for 1) use only direct and reflected in hit clustering, i.e. UseDirect and UseReflected, and 2) keep the same hit clustering (both direct and reflected) but only average for direct and reflected hits
//#define ONLYDRHITBETA
const int NDRRatioRegion=5;
const double dDRRatioStep=1./NDRRatioRegion;
const double dRigCutOnlyDR[3] = {100. ,150., 200.};
//#define APPLYCOR
#ifdef APPLYCOR
#include "richcor/ModelManager.h"
#endif //APPLYCOR

#endif //CHECKRICHGEO

const double dRigCut[3] = {50., 100. ,150.};
#endif //ISOTOPE

//--
//#define KALMANRIG
//#define PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10

//--flux
#if defined TOFQEFF || defined L1EFF || defined L9EFF || defined EFFVAL || defined RIGRESO || defined ACCEPTANCE || defined ESTCAL || defined ISOTOPE//for efficiency, not store event count for Traking & Trigger since there will either use different prescaled trees or not full time period
#define RIG_NEWTRQ
//#define RIGESTI
#define BADL1LADRIG
#define CUTOFFSAFETYVAR
// #define EVCOUNTINORBIT //2023.08.01 orbit
#endif //RIG_NEWTRQ

//#define RIG
#ifdef RIG
#ifdef AFS
#define EXPOTCAL
#endif //AFS
//#define EXPOTCAL
#endif //RIG

//--cutoff distribution, store only when running EFFVAL
#ifdef EFFVAL
#define CUTOFFDIS
#endif //EFFVAL

//--L1->L2 survival probabilty
#define L1L2SUR
#ifdef L1L2SUR
#define L1L2SURNEW
#endif //L1L2SUR

//--L8->L9 survival probability
#define L8L9SUR
#ifdef L8L9SUR
#define L8L9SURECALMIP
#endif //L8L9SUR

//--under cutoff study
//#define UNDERCUTOFF

//--rig vs estimated rig
//#define RIGESTCHECK

//--Trd interaction control //2021.09.28
//#define TRDINTERCON

//--chis check for GBL //2022.09.22
//#define GBLCHISCHECK
#ifdef GBLCHISCHECK
#define RIG_NEWTRQ
//#undef MCREWEIGHT

//#define READEXTCHIS //2022.10.01: read the stored L1 Chis with TrFit::GetExtChis
//#define GBLCHISCHECKL1Eff

#endif //GBLCHISCHECK

//--apply RigDiffCut and check the change of rigidity resolution //2023.02.04
//#define RIGDIFFCUT
#ifdef RIGDIFFCUT
#define RIG_NEWTRQ
#endif //RIGDIFFCUT

//2023.03.17
#if defined GBLCHISCHECK || defined RIGDIFFCUT
#define HELIUM	//2
#define CARBON	//6
#endif //GBLCHISCHECK || RIGDIFFCUT

//--TRD hit adc distribution study //2023.01.13
//#define TRDADC
#ifdef TRDADC
//const int NTRDADC=4097;
const int NTRDADC=6000;
//#define TRDADCOLD //old way to check the TRD adc by selecting different Q
#define TRDADCNEW //check the TRD adc without selection Q //2023.01.16

#define MAXADCDIS //the adc distribution of the 1st to 10th hits, from maximum to minimum //2023.01.26
const int TRDADCThrs[8] = {25, 155, 400, 700, 1100, 1500, 1800, 2000};

#define RIG_NEWTRQ
const int NTRDADCSum=4;
#endif //TRDADC

//------
//#define REFERENCE
#define BZSELECTION
#ifdef REFERENCE
#define BZSELECTION
#endif

#ifdef BZSELECTION
//--
#ifdef UNFOLDING
//#define PROTON
// #define LIGHTSEC
// #define LIGHTPRI  ///8
#define MIDPRI  ///14
#define MIDSEC   ///9
// #define P_CL_K //2023.02.16
// #define S_AR_CA //2023.03.03
//--
//#define HEAVYPRI
//#define NICKEL
#endif //UNFOLDING

#if defined HEAVYPRI && defined CHARGECAL_NEWTRQ
#define IRONGROUP //Iron nearby, 25-28
#endif //HEAVYPRI && CHARGECAL_NEWTRQ
//--
//#define PHOSPHORUS
#ifdef P_CL_K //2023.02.16
//#define SILICON	//14 //now keep the flag as a check for the new charge cut, will remove later
#define PHOSPHORUS	//15
#define CHLORINE	//17
#define POTASSIUM	//19
#endif //P_CL_K
#if defined PHOSPHORUS && defined CHARGECAL_NEWTRQ
//#define PHOSPHORUSGROUP //P nearby, 14-20
#define //P nearby, 14-20
#endif //PHOSPHORUS && CHARGECAL_NEWTRQ
//--
#ifdef S_AR_CA //2023.03.03
//#define SILICON		//14 //now keep the flag as a check for the new charge cut, will remove later
#define SULFUR 		//16
#define ARGON		//18
#define CALCIUM		//20
#endif //S_AR_CA
//--
#if defined NICKEL && defined CHARGECAL_NEWTRQ
#define NICKELGROUP //Ni nearby, 26-30
#endif //NICKEL && CHARGECAL_NEWTRQ

//----
//--Li, Be, B, N
#ifdef LIGHTSEC
#define LITHIUM		//3
#define BERYLLIUM	//4
#define BORON		//5
#define NITROGEN	//7
#endif //LIGHTSEC
//--Oxygen
#ifdef LIGHTPRI
//#define HELIUM	//2
// #define CARBON	//6
#define OXYGEN	//8
#endif //LIGHTPRI
//--Ne, Mg, Si, S
#ifdef MIDPRI
// #define NEON	//10
//#define MAGNESIUM	//12
#define SILICON	//14
//#define SULFUR
#endif //MIDPRI
//--F, Na, Al
#ifdef MIDSEC
#define FLUORINE	//9
// #define SODIUM	//11
// #define ALUMINIUM //13
#endif //MIDSEC
//--Iron
#ifdef HEAVYPRI
#define IRON
#endif //HEAVYPRI
//--P-group, 14-20
#ifdef PHOSPHORUSGROUP
#define SILICON		//14
#define PHOSPHORUS	//15
#define SULFUR		//16
#define CHLORINE	//17
#define ARGON		//18
#define POTASSIUM	//19
#define CALCIUM		//20
#endif //PHOSPHORUSGROUP
//--Fe-group, 25-28
#ifdef IRONGROUP
#define MANGANESE
#define COBALT
#define NICKEL
#endif //IRONGROUP
//--Ni-group, 26-30
#ifdef NICKELGROUP
#define IRON	//26
#define COBALT	//27
#define NICKEL	//28
#define COOPER	//29
#define ZINC	//30
#endif //NICKELGROUP

//--nuclei for TOI study
#ifdef CHARGETOI
//2021.02.01: include lower Q as well
#define LITHIUM	//3
#define BERYLLIUM	//4
#define BORON	//5
#define CARBON	//6
#define NITROGEN	//7
//--
#define OXYGEN	//8
#define FLUORINE	//9
#define NEON	//10
#define SODIUM 	//11
#define MAGNESIUM	//12
#define ALUMINIUM //13
#define SILICON	//14
#define SULFUR	//16
#endif //CHARGETOI

//--

//------
//#define FIVEYEARS //PASS7COEFF
//#define SEVENYEARSADD
//#define TENYEARSEXT //2022.05.19: extension to 10 years
#ifndef IRON
//#define SEVENYEARFLUX //2020.09.11: taking out to extend to 8.5 years for ONeSi as well
#endif

//limit to 10 years result //2023.03.09
// #define UPTOTENYEAR
#ifdef L1QDISSEL //2023.04.26
#undef UPTOTENYEAR
#endif //L1QDISSEL

//extension to 11.5 year //2023.06.18
// #define EXTELEVENYEAR
#ifdef EXTELEVENYEAR
#undef UPTOTENYEAR
#endif //EXTELEVENYEAR
//#define VARDIS

//2020.01.10: always use TWIKI (also for Oxygen), i.e. iQVer=4, unless selection for CHARGECAL_NEWTRQ
#ifndef CHARGECAL_NEWTRQ
#define QSELTWIKI
#endif //CHARGECAL_NEWTRQ

//#define ACCCHECK

//#define EXTASYCHECK //2019.09.02

//#define RIGRESOWOL1CHISQY //2019.09.09

#if defined IRON && not defined CHARGECAL_NEWTRQ && not defined FIVEYEARS
//#define FORCENEWTRIG //comment out for charge template
#endif //CHARGECAL_NEWTRQ
//#define FORCEOLDTRIGSETTING //2020.08.12
//2020.12.09
//#define FORCEOLDTRIG
// #define FORCENEWTRIG

//#define LOOSEFV //2020.08.19
// #define QYEVCOUNT //2020.08.20

//#define IRONFORCEOXYGENREWEIGH //2020.09.09 //2020.09.16: comment out

//2020.12.12: to match with bartel rotation period and to do the (F/Si)/(B/O) ratio, set a different value of period separation
//#define FORCEOLDTRIGBR
//#define FORCENEWTRIGBR
#if defined FORCEOLDTRIGBR || defined FORCENEWTRIGBR
#define TRIGGER_BR_PERIOD
#endif //FORCEOLDTRIGBR || FORCENEWTRIGBR

//--
#define EVENTCOUNTING
int iNEV[3][4]={{0}}; //[span][stage]
#endif //BZSELECTION

string sDirVD;

//MC reweight
double ww[3];
double ww0[3];
double ww2[3];
//double ww=1.;
double dNORM=1.;

//--
#include "SelEvent14.h"

//--Event Count, including Isotopes
#include "EvCount.h"

//--Acceptance & TOI
#include "Acceptance.h"

//--ChargeCalibration
#include "ChargeCalN9.C" //charge distribution

//--TrRes
#include "TrRes6.h"

//--Efficiency
#include "RigEstiCal3.h"
#include "Efficiency11.h"

//--Efficiency sample validation
//#include "EffVal3.h" //-- 20190111: have problem: Error: Illegal array dimension (Ignore subsequent errors) EffVal3.h:59:

//--Rigidity Resolution
#include "RigReso2.h"

//--GBL chis check //2023.02.27
#include "GBLChisCheck.h"

//2023.05.01
//#ifdef ESTCAL
#if defined ESTCAL || defined RIGRESO //2023.05.03
#define HELIUM			//2
#define LITHIUM			//3
#define BERYLLIUM		//4
#define BORON			//5
#define CARBON			//6
#define NITROGEN		//7
#define OXYGEN			//8
#define FLUORINE		//9
#define NEON			//10
#define SODIUM			//11
#define MAGNESIUM		//12
#define ALUMINIUM		//13
#define SILICON			//14
#define PHOSPHORUS		//15
#define SULFUR			//16
#define CHLORINE		//17
#define ARGON			//18
#define POTASSIUM		//19
#define CALCIUM			//20

//25-30
//#define MANGANESE	//25
#define IRON		//26
//#define COBALT		//27
#define NICKEL		//28
//#define COOPER		//29
//#define ZINC		//30
#endif //ESTCAL

#ifdef CHARGECAL_NEWTRQ
/*//#ifdef L2QTEMPALL
#if defined L2QTEMPALL || defined GETINNERQ
#define LITHIUM		//3
#define BERYLLIUM		//4
#define BORON		//5
#define CARBON		//6
#define NITROGEN		//7
#define OXYGEN		//8
#define FLUORINE		//9
#define NEON		//10
#define SODIUM		//11
#define MAGNESIUM		//12
#define ALUMINIUM		//13
#define SILICON		//14
#define PHOSPHORUS	//15
#define SULFUR		//16
#define CHLORINE		//17
#endif //L2QTEMPALL*/

//2023.04.06
//3-7
//#define LITHIUM	//3
//#define BERYLLIUM	//4
//#define BORON		//5
//#define CARBON		//6
//#define NITROGEN	//7

//8-20
#define OXYGEN		//8
#define FLUORINE	//9
#define NEON		//10
// #define SODIUM		//11
// #define MAGNESIUM	//12
// #define ALUMINIUM	//13
#define SILICON		//14
// #define PHOSPHORUS	//15
// #define SULFUR		//16
// #define CHLORINE	//17
//#define ARGON		//18
// #define POTASSIUM	//19
// #define CALCIUM		//20

// //21-24
// #define SCANDIUM
// #define TITANIUM
// #define VANADIUM
// #define CHROMIUM

/*//25-30
#define MANGANESE	//25
#define IRON		//26
#define COBALT		//27
#define NICKEL		//28
#define COOPER		//29
#define ZINC		//30*/
#endif //CHARGECAL_NEWTRQ

int analysis(TChain &ch, string odir, string ofile, long int nrun=-1, const int iGeoSafety=1)
{
	time_t start = 0, end = 0;
	time_t check = 0, check1=0;
	start = time(NULL);
	cout << "Start at " << ctime(&start) << endl;
	
	#ifdef BZSELECTION
	#ifdef PROTON
	viQSel.push_back(1);
	#endif //PROTON
	#ifdef HELIUM
	viQSel.push_back(2);
	#endif //HELIUM
	#ifdef LITHIUM
	viQSel.push_back(3);
	#endif //LITHIUM
	#ifdef BERYLLIUM
	viQSel.push_back(4);
	#endif //BERYLLIUM
	#ifdef BORON
	viQSel.push_back(5);
	#endif //BORON
	#ifdef CARBON
	viQSel.push_back(6);
	#endif //CARBON
	#ifdef NITROGEN
	viQSel.push_back(7);
	#endif //NITROGEN
	#ifdef OXYGEN
	viQSel.push_back(8);
	#endif //OXYGEN
	#ifdef FLUORINE
	viQSel.push_back(9);
	#endif //FLUORINE
	#ifdef NEON
	viQSel.push_back(10);
	#endif //NEON
	
	#ifdef SODIUM
	viQSel.push_back(11);
	#endif //SODIUM
	#ifdef MAGNESIUM
	viQSel.push_back(12);
	#endif //MAGNESIUM
	#ifdef ALUMINIUM
	viQSel.push_back(13);
	#endif //ALUMINIUM
	#ifdef SILICON
	viQSel.push_back(14);
	#endif //SILICON
	#ifdef PHOSPHORUS
	viQSel.push_back(15);
	#endif //PHOSPHORUS
	#ifdef SULFUR
	viQSel.push_back(16);
	#endif //SULFUR
	#ifdef CHLORINE
	viQSel.push_back(17);
	#endif //CHLORINE
	#ifdef ARGON
	viQSel.push_back(18);
	#endif //ARGON
	#ifdef POTASSIUM
	viQSel.push_back(19);
	#endif //POTASSIUM
	#ifdef CALCIUM
	viQSel.push_back(20);
	#endif //CALCIUM
	#ifdef SCANDIUM
	viQSel.push_back(21);
	#endif //SCANDIUM
	#ifdef TITANIUM
	viQSel.push_back(22);
	#endif //TITANIUM
	#ifdef VANADIUM
	viQSel.push_back(23);
	#endif //VANADIUM
	#ifdef CHROMIUM
	viQSel.push_back(24);
	#endif //CHROMIUM
	#ifdef  MANGANESE
	viQSel.push_back(25);
	#endif //MANGANESE
	#ifdef IRON
	viQSel.push_back(26);
	#endif //IRON
	#ifdef COBALT
	viQSel.push_back(27);
	#endif //COBALT
	#ifdef NICKEL
	viQSel.push_back(28);
	#endif //NICKEL
	#ifdef COOPER
	viQSel.push_back(29);
	#endif //COOPER
	#ifdef ZINC
	viQSel.push_back(30);
	#endif //ZINC
	
	#endif //BZSELECTION
	
	//--AMS RTI
	#ifdef AMSSOFT
	AMSSetupR::RTI::UseLatest(7); 
	#endif //AMSSOFT
	
	#if defined EXPOTCAL && defined AFSRTI
	AMSSetupR::RTI::UseLatest(7);
	#endif //EXPOTCAL && AFSRTI
	
	/*#ifdef UNFOLDING
	#ifdef FORCEHZBIN
	iBinVersion=2;
	#endif //FORCEHZBIN
	#endif //UNFOLDING*/
	//for now always force to use HZ bin
	#ifdef FORCEHZBIN
	iBinVersion=2;
	#endif //FORCEHZBIN
	
	//--selection charge
	int SCharge=8;
	int MCharge=8;
	
	bool bNewTrQ=false;
	int iSelSigma=1; //select charge with 1.5 sigma
	
	//2021.02.07
	iSpanL=1, iSpanU=3;
	//#ifdef INNERUNBIAEDL1Q
	//iSpanL=0;
	//#endif //INNERUNBIAEDL1Q
	//2021.10.22
	#ifdef INNERGEOM
	iSpanL=0;
	#endif //INNERGEOM
	
	//---output
	string commond = Form("mkdir -pv %s", odir.c_str());
	cout << "creating outdir:" << system(commond.c_str()) << endl;
	
	string sRunlogDir = odir+"/runlog";
	cout << "creating runlog dir: " << sRunlogDir << " " << system(Form("mkdir -pv %s", sRunlogDir.c_str())) << endl;
	
	string outf_name = Form("%s/%s.root", odir.c_str(), ofile.c_str());
	cout << outf_name.c_str() << endl;
	
	TFile outfile(outf_name.c_str(), "RECREATE");
	
	TObjArray hman1;
	
	//#ifdef UNFOLDING
	#ifdef UNFOLDINGCHECK //2019.11.11
	//bSumw2 = true;
	//TH1::SetDefaultSumw2(bSumw2);
	//cout << "****** Set Default Sumw2 ******" << endl;
	#endif //UNFOLDING
	
	string sFileDir = ch.GetFile()->GetPath();
	//2020.12.22: read the dst from runlist
	/*if (sFileDir.find("amsd64n")!=string::npos || sFileDir.find("190710_62n/iss_2")!=string::npos)
	{
		dstver=64;
	}
	else if (sFileDir.find("amsd63n")!=string::npos || sFileDir.find("191030_63n")!=string::npos)
	{
		dstver=63;
	}
	else if (sFileDir.find("amsd62n")!=string::npos)
	{
		dstver=62;
	}
	else if (sFileDir.find("amsd60n")!=string::npos)
	{
		dstver=60;
	}*/
	
	//2020.12.14
	#ifdef L8L9SURECALMIP
	bECalMIPs=true;
	#endif //L8L9SURECALMIP
	
	cout << "dst version: " << dstver;
	if (bECalMIPs) cout << ", ecal MIPs sample";
	cout << endl;
	
	//--input
	SelEvent *ev = new SelEvent();
	ev->fOutput = &outfile;
	
	ev->SelEvent::SetBranch(ch);
	if (bECalMIPs) ev->SelEvent::AddBranch_EcalMIPs(ch);
	//2020.12.23
	#ifdef ISOTOPE
	ev->AddBranchRich(ch);
	#endif //ISOTOPE
	
	//2021.09.28
	#ifdef TRDINTERCON
	ev->AddBranch_Trd(ch);
	#endif //TRDINTERCON
	
	//2022.10.01: read the stored L1 Chis with TrFit::GetExtChis
	#ifdef READEXTCHIS
	ev->AddBranch_ExtChis(ch);
	#endif //READEXTCHIS
	
	//2022.11.09: read only-direct and only-reflected hits
	#ifdef ONLYDRHITBETA
	ev->AddBranch_OnlyDRRich(ch);
	int NRichCor=1;
	//2022.11.22
	#ifdef APPLYCOR
	NRichCor=2;
	string sModelDir = sWDirOnline+"/analisi/richcor";
	//--load class GAMModel
	if (!TClass::GetDict("GAMModel")) gROOT->ProcessLine(Form(".L %s/GAMModel.C++", sModelDir.c_str()));
	
    ModelManager::init(Form("%s/model_data.root", sModelDir.c_str()), Form("%s/model_mc.root", sModelDir.c_str()));
    cout<<ModelManager::model[0][0].index_correction.GetEntries()<<endl;
	#endif //APPLYCOR
	#endif //ONLYDRHITBETA
	
	//2023.01.13
	#ifdef TRDADC
	ev->AddBranch_Trd(ch);
	#endif //TRDADC
	
	ch.GetEntry(0);
	MCVer=0; //2023.02.16
	string sRawDir = "";
	if (!ev->isreal)
	{
		//--//2022.10.16
		//#ifndef GBLCHISCHECK
		bSumw2 = true;
		TH1::SetDefaultSumw2(bSumw2);
		cout << "****** Set Default Sumw2 ******" << endl;
		//#endif //GBLCHISCHECK
		
		/*//-- //2022.08.17
		#ifndef RICHVARCHECKRESO2D
		if (!ev->isreal)
		{
			bSumw2 = true;
			TH1::SetDefaultSumw2(bSumw2);
			cout << "****** Set Default Sumw2 ******" << endl;
		}
		#else
		bSumw2 = true;
		TH1::SetDefaultSumw2(bSumw2);
		cout << "****** Set Default Sumw2 ******" << endl;
		#endif //RICHVARCHECKRESO2D*/
		
		//--
		ev->SelEvent::AddBranch_MC(ch);
		ch.GetEntry(0);
		MCQ=SCharge=MCharge=ev->mch;
		//cout << "MC charge: " << ev->mch << endl;
		//2020.12.22
		histn = histn.substr(sElementShort[MCQ-1].size(), histn.size()-sElementShort[MCQ-1].size());
		//MCMass = stoi(histn);
		//2021.01.19: change to non C++11 function
		std::istringstream  is(histn);
		is >> MCMass;
		
		cout << "MC charge=" << ev->mch << ", mass=" << MCMass << endl;
		
		//--Find out MC version, MCVer=1 for l1 & MCVer=2 for l19
		cout << "sFileDir=" << sFileDir << endl; //XXX
		//if (sFileDir.find("l1_")!=string::npos) MCVer=1;
		//else if (sFileDir.find("l19_")!=string::npos) MCVer=2;
		//2020.11.09
		if (sFileDir.find("l1_")!=string::npos || sFileDir.find("l1/")!=string::npos) MCVer=1;
		else if (sFileDir.find("l19_")!=string::npos || sFileDir.find("l19/")!=string::npos) MCVer=2;
		
		/*//for amsd68n N, the naming for l19 is wrong to be l1 //2023.05.03 //2023.05.13: corrected
		if (ev->mch==7 && dstver==68)
		{
			cout << " *** Specific treatment for naming of Nitrogen MC PASS8" << endl;
			if (sFileDir.find("l1_14")!=string::npos) MCVer=2;
			else if (sFileDir.find("l1_7")!=string::npos) MCVer=1;
		}*/
		
		#ifdef LXPLUS //2020.06.02: specific name for Iron l1
		//else if (MCQ==26) MCVer=1;
		else if (MCQ==26 && ((ev->amsdn%100)==64)) MCVer=1; //only for PASS7 amsd64n
		#endif //LXPLUS
		//2020.02.18: for now the MC for ECal MIP is l19 foucs
		//if (bECalMIPs) MCVer=2; //2020.12.14: no need to specify now
		
		cout << Form("MC version: %d (%s)", MCVer, (MCVer==1)?"l1":"l19") << endl;
		
		//--MC total events
		histn = Form("mcev_NGen%d", MCVer);
		hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 4, 0, 4));
		
		//--MC minimum event number in each run
		histn = Form("mcev_MinimumEvent%d", MCVer);
		hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 201, -0.5, 200.5));
		
		//--MC events outside the production range //2023.05.10
		int iRange[2], iStoreRange[2][2];
		int NBin[2];
		if (MCVer==1)
		{
			iRange[0] = 1, iRange[1] = 2000;
			iStoreRange[0][0] = 0, iStoreRange[0][1] = iRange[0];
			iStoreRange[1][0] = iRange[1], iStoreRange[1][1] = 3000;
			NBin[0] = NBin[1] = 10;
		}
		else if (MCVer==2)
		{
			iRange[0] = 2, iRange[1] = 8000;
			iStoreRange[0][0] = 0, iStoreRange[0][1] = iRange[0];
			iStoreRange[1][0] = iRange[1], iStoreRange[1][1] = 10000;
			NBin[0] = NBin[1] = 20;
		}
		for (int ilu=0; ilu<2; ilu++)
		{
			histn = (ilu==0)?"Under":"Over";
			histn = Form("mcev_%sflowEvents_MCVer%d", histn.c_str(), MCVer);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), NBin[ilu], iStoreRange[ilu][0], iStoreRange[ilu][1]));
		} //ilu
		
		//--raw file list for MC
		sRawDir = sWDirOnline+"/runlist/MC/";
		if (MCVer==1)
		{
			if (MCQ==2) sRawDir += "he4.pl1.l1.24000.4_00";
			else if (MCQ==3) //2022.12.28
			{
				if (MCMass==6) sRawDir += "li6.pl1.l1.36000.4_02";
				else if (MCMass==7) sRawDir += "li7.pl1.l1.36000.4_02";
			}
			else if (MCQ==4)
			{
				if (MCMass==7) sRawDir += "be7.pl1.l1.48000.4_02";
			}
			else if (MCQ==6) sRawDir += "c12.pl1.l1.612000.4_01";
			else if (MCQ==8) sRawDir += "o16.pl1.l1.816000.4_02";
		}
		else if (MCVer==2)
		{
			if (MCQ==6) sRawDir += "c12.pl1.l19.1248000.4_01";
			else if (MCQ==8) sRawDir += "o16.pl1.l19.1664000.4_02";
		}
		
	}
	cout << "sRawDir=" << sRawDir << endl;
	
	//-- //2023.02.27
	if ((ev->amsdn%100)>=68) cout << " ****** Using PASS8 reconstruction ****** " << endl;
	
	//2022.11.03
	#ifdef GBLCHISCHECK
	/*//const string sSigmaVer = "GBLChisCheckScat5_ScatAngle2";
	//const string sSigmaVer = "GBLChisCheckScat7_ScatAngle2"; //2022.12.19
	const string sSigmaVer = "GBLChisCheckScat8_ScatAngle2"; //2023.01.12: for simple fit, use the one rescale to cross Rig=100 GV
	//SplineFit *sfRigDiffSig[NAlgoCheck];
	////TF1 *f1RigDiffSig[NAlgoCheck];
	//TF1 *f1RigDiffSig[NAlgoCheck][NSigmaFit]; //2022.11.17
	//2022.12.19
	//SplineFit *sfRigDiffSig[NAlgoCheck][2];
	//TF1 *f1RigDiffSig[NAlgoCheck][2][NSigmaFit];
	//inclue Full Span //2023.01.11
	SplineFit *sfRigDiffSig[NAlgoCheck][NgrSig][2];
	TF1 *f1RigDiffSig[NAlgoCheck][NgrSig][NSigmaFit][2];
	//--spline fit
	for (int ialgo=0; ialgo<NAlgoCheck; ialgo++)
	{
		//histn = Form("%s/analisi/RigDiffSig/rRL1I_rRI_Sigma_gr0_r1_q2_cut1_Pass%s_%s", sWDirOnline.c_str(), ((dstver==68)?"8":"7"), sAlgo[ialgo].c_str());
		//histn = Form("%s/analisi/RigDiffSig/%s/rRL1I_rRI_Sigma_gr0_r1_q%d_cut1_Pass%s_%s", sWDirOnline.c_str(), sSigmaVer.c_str(), MCQ, ((dstver==68)?"8":"7"), sAlgo[ialgo].c_str());
		for (int igrSig=0; igrSig<2; igrSig++)
		{
			//for (int ir=1; ir<3; ir++)
			for (int ir=1; ir<2; ir++)
			{
				//--
				histn = Form("%s/analisi/RigDiffSig/%s/rRL1I_rRI_Sigma_gr%d_r%d_q%d_cut1_Pass%s_%s", sWDirOnline.c_str(), sSigmaVer.c_str(), igrSig, ir, MCQ, ((dstver==68)?"8":"7"), sAlgo[ialgo].c_str());
				cout << "Spline fit: " << histn << endl; //XXX
				sfRigDiffSig[ialgo][igrSig][ir-1] = new SplineFit(histn.c_str());
				f1RigDiffSig[ialgo][igrSig][0][ir-1] = sfRigDiffSig[ialgo][igrSig][ir-1]->f1SplineFit;
				cout << f1RigDiffSig[ialgo][igrSig][0][ir-1]->Eval(100.) << endl; //XXX
				
				//--fit with simple function
				//--no rescale
				ifstream isFit;
				histn += "_SimpleFit";
				cout << "Simple Fit: " << histn << endl;
				isFit.open(histn.c_str());
				f1RigDiffSig[ialgo][igrSig][1][ir-1] = new TF1(Form("algo%d_r%d_SimpleFit", ialgo, ir), "sqrt([0]*[0]+[1]*[1]/x/x/x)", 0.1, 10000.);
				for (int ipar=0; ipar<2; ipar++)
				{
					double dPar=0;
					isFit >> dPar;
					f1RigDiffSig[ialgo][igrSig][1][ir-1]->SetParameter(ipar, dPar);
					cout << "ialgo=" << ialgo << " ipar=" << ipar << " " << f1RigDiffSig[ialgo][igrSig][1][ir-1]->GetParameter(ipar) << endl; //XXX
				} //ipar
				isFit.close();
				cout << f1RigDiffSig[ialgo][igrSig][1][ir-1]->Eval(100.) << endl; //XXX
				
				//--rescale to 100 GV
				histn += "_Scale";
				cout << "Simple Fit after rescale: " << histn << endl;
				isFit.open(histn.c_str());
				f1RigDiffSig[ialgo][igrSig][2][ir-1] = new TF1(Form("algo%d_r%d_SimpleFit_rescale", ialgo, ir), "sqrt([0]*[0]+[1]*[1]/x/x/x)", 0.1, 10000.);
				for (int ipar=0; ipar<2; ipar++)
				{
					double dPar=0;
					isFit >> dPar;
					f1RigDiffSig[ialgo][igrSig][2][ir-1]->SetParameter(ipar, dPar);
					cout << "ialgo=" << ialgo << " ipar=" << ipar << " " << f1RigDiffSig[ialgo][igrSig][2][ir-1]->GetParameter(ipar) << endl; //XXX
				} //ipar
				cout << f1RigDiffSig[ialgo][igrSig][2][ir-1]->Eval(100.) << endl; //XXX
				isFit.close();
			} //ir
		} //igrSig
	} //ialgo*/
	ReadRigDiffSigmavsRig();
	#endif //GBLCHISCHECK
	
	if (dstver>=60)
	{
		#ifndef ALWAYSOLD
		bNewTrQ=true;
		ev->SelEvent::AddBranch_NewTrQ(ch);
		#endif //ALWAYSOLD
	}
	
	cout << "bNewTrQ=" << Form("%s", (bNewTrQ)?"true":"false") << endl;
	
	//2020.10.25: remove all the nuclei different from the generated charge of MC
	#ifdef BZSELECTION
	#ifdef EFFSEC //only store the influence to odd charge, or for Si store for heavier charge
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		if (ev->isreal) continue;
		bool bSkip = (*it%2==0 && *it<14);
		//if (*it==ev->mch) bSkip = true; //need to store mch, as for EffSec used event rate to reweigh
		if (bSkip)
		{
			viQSel.erase(it);
			it--;
		}
	} //it
	#else
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		if (!ev->isreal && *it!=ev->mch)
		{
			viQSel.erase(it);
			it--;
		}
	} //it
	#endif //EFFSEC
	/*//2020.11.20: if defined EFFSEC and the mc charge is in viQSel, then also only store mc charge //need to reset many times for differet charge, not efficiency
	#ifdef EFFSEC
	if (std::find(viQSel.begin(), viQSel.end(), ev->mch) != viQSel.end())
	#endif //EFFSEC
	{
		for (it=viQSel.begin(); it!=viQSel.end(); it++)
		{
			if (!ev->isreal && *it!=ev->mch)
			{
				viQSel.erase(it);
				it--;
			}
		} //it
	}*/
	#endif //BZSELECTION
	
	//--
	if (iGeoSafety!=1) dGeoSafety=1.0+0.2*iGeoSafety;
	cout << "Safety factor of Geomagnetic cutoff: " << dGeoSafety << endl;
	
	//--
	int nQVer = 3; //always be 3, will decide what to store using nQVerStore inside the loop
	int nQVerStore = (bNewTrQ)?1:1; //for now only use YJ's Q, i.e. only store 1 sets of charge calibration
	int QVeri=(bNewTrQ)?1:0;
	#ifdef QSELTWIKI
	nQVer = 2*nQVer;
	if (bNewTrQ) QVeri=4;
	#endif //QSELTWIKI
	int ChargeVersion=QVeri%3;
	int QCutVer=QVeri;
	
	cout << "nQVer=" << nQVer << " nQVerStore=" << nQVerStore << " QVeri=" << QVeri << endl;
	cout << "ChargeVersion=" << ChargeVersion << " QCutVer=" << QCutVer << endl;
	
	//TObjArray hmanQ[nQVer];
	TObjArray *hmanQ = new TObjArray[nQVer]; //2023.02.08
	
	//--read reweight file
	TF1 *f1Fit = NULL;
	double dwn[3] = {0};
	
	TF1 *f1Flux[3]={NULL,NULL,NULL};
	double dNorm[3]={1.,1.,1.};
	
	//--
	#ifdef MCREWEIGHT
	double totGen[2]={0.}; //total generated events
	
	double l1l=1., l1u=2000.;
	double l19l=2., l19u=8000.;
	
	TF1 *f1GenR=NULL;
	
	//total generated events from full data sets
	int iNGenBin=2; //max events
	int iFAiter=0;
	if (!ev->isreal)
	{
		//2019.12.28: now use dstversion and charge to decide the correspond directory
		//string sVersion = Form("amsd%dnMC%s", dstver, sElement[MCQ-1].c_str());
		string sVersion = Form("amsd%dnMC%s%d", dstver, sElementShort[MCQ-1].c_str(), MCMass);
		
		histn = Form("%s/analisi/FluxFit/%s/total_raw_EvenLogBin.root", sWDirOnline.c_str(), sVersion.c_str());
		
		TFile *fNGen = new TFile(histn.c_str(), "READ");
		cout << histn << " " << fNGen << endl; //XXX
		if (!fNGen->IsOpen())
		{
			cout << " ****** Warning: total generated events not found, stop running the script ****** " << endl;
			return -1;
		}
		
		for (int iVer=0; iVer<2; iVer++)
		{
			//only have l1 focus for He //2022.09.14
			//if (ev->mch==2 && iVer>0) continue; //now will check the exsitence of the histogram after //2023.02.08
			histn = Form("mcev_NGen%d", iVer+1);
			TH1F *hNGen = static_cast<TH1F*>(fNGen->Get(histn.c_str()));
			
			if (hNGen==NULL)
			{
				cout << " ****** not found total generated events for " << ((iVer==0)?"l1":"l19") << " focus ****** " << endl;
				continue;
			}
			totGen[iVer] = hNGen->GetBinContent(iNGenBin); //intend to break if no total generated event file ready //now check explicitly whether the files is opened before //2023.02.08
			//if (hNGen!=NULL) totGen[iVer] = hNGen->GetBinContent(iNGenBin);
		} //iVer
		fNGen->Close();
		cout << "total generated events: " << totGen[0] << " " << totGen[1] << endl;
		
		//--weights
		/*dwn[0] = 1.;
		dwn[1] = totGen[0]/(totGen[0]+totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		dwn[2] = totGen[0]/(totGen[1]*log(l1u/l1l)/log(l19u/l19l));*/
		//2021.02.01: remove the dependence on total no. events completely TODO 
		//dwn[0] = 1e10/totGen[0];
		//dwn[1] = 1e10/(totGen[0]+totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		//dwn[2] = 1e10/(totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		////2022.09.14: for He only have l1 focus
		//if (ev->mch==2) dwn[0] = dwn[1] = dwn[2] = 1e10/totGen[0];
		//else
		//{
		//	dwn[0] = 1e10/totGen[0];
		//	dwn[1] = 1e10/(totGen[0]+totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		//	dwn[2] = 1e10/(totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		//}
		//--if only l1 focus, set dwn[2] to 0 //2023.02.08
		dwn[0] = 1e10/totGen[0];
		dwn[1] = 1e10/(totGen[0]+totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		dwn[2] = (totGen[1]<=0)?0:1e10/(totGen[1]*log(l1u/l1l)/log(l19u/l19l));
		/*//2021.02.01: apply isotope mixture at this level
		//Li:	50%Li6+50%Li7
		//Be:	50%Be7+50%Be9
		//B:	30%B10+70%B11
		//N:	50%N14+50%N15
		double dIsotopeMixture=1.;
		if (MCQ==3)
		{
			if (MCMass==6 || MCMass==7) dIsotopeMixture=0.5;
		}
		else if (MCQ==4)
		{
			if (MCMass==7 || MCMass==9) dIsotopeMixture=0.5;
		}
		else if (MCQ==5)
		{
			if (MCMass==10) dIsotopeMixture=0.3;
			else if (MCMass==11) dIsotopeMixture=0.7;
			else dIsotopeMixture=1.0;
		}
		else if (MCQ==7)
		{
			if (MCMass==14 || MCMass==15) dIsotopeMixture=0.5;
		}
		
		for (int ir0; i<3; i++) dwn[i]=dwn[i]*dIsotopeMixture;*/
		
		//checking MC efficiency for Si with YJ's & HL's charge
		string fversion="";
		iFAiter=0;
		
		//string sYear="8hyr";
		string sYear="10yr";
		//2022.07.08
		//if (ev->mch==4) sYear="8hyr";
		//if (ev->mch==4 || ev->mch==6) sYear="8hyr"; //2022.08.17
		if (ev->mch==2 || ev->mch==4 || ev->mch==6) sYear="8hyr"; //2022.11.17
		
		cout << " ****** Reweigh the MC generation spectrum ****** " << endl;
		//for (int ir=1; ir<3; ir++)
		//for (int ir=iSpanL; ir<iSpanU; ir++) //2021.02.07
		for (int ir=0; ir<iSpanU; ir++) //2022.06.26
		{
			cout << "ir=" << ir << endl;
			dNorm[ir] = 1e10/totGen[0]; //2020.11.04
			
			#ifndef UNFOLDINGCHECK
			if (iFAiter==0) continue;
			#endif //UNFOLDINGCHECK
			if (iFAiter>0)
			{
				#ifdef UNFOLDINGCHECK
				if (ProdVer==21 || ProdVer==22) histn = Form("%s/analisi/FluxFit/PASS7MCO16v2/%s/FluxFit_r%d_q8_PASS7MCO16v2_%s_%d", sWDirOnline.c_str(), fversion.c_str(), ir, fversion.c_str(), iFAiter-1);
				else if (ProdVer==24 || ProdVer==25) histn = Form("%s/analisi/FluxFit/PASS7amsd57nMCO16/%s/FluxFit_r%d_q8_PASS7amsd57nMCO16_%s_%d", sWDirOnline.c_str(), fversion.c_str(), ir, fversion.c_str(), iFAiter-1);
				else if (ProdVer==26 || ProdVer==27) histn = Form("%s/analisi/FluxFit/PASS7amsd57nMCC12/%s/FluxFit_r%d_q6_PASS7amsd57nMCC12_%s_%d", sWDirOnline.c_str(), fversion.c_str(), ir, fversion.c_str(), iFAiter-1);
				#else
				if (ProdVer==21 || ProdVer==22) histn = Form("%s/analisi/FluxFit/PASS7COEFF/%s/FluxFit_r%d_q8_PASS7COEFF_%s_%d", sWDirOnline.c_str(), fversion.c_str(), ir, fversion.c_str(), iFAiter-1);
				else if (ProdVer==24 || ProdVer==25) histn = Form("%s/analisi/FluxFit/PASS7amsd57n/%s/FluxFit_r%d_q8_PASS7amsd57n_%s_%d", sWDirOnline.c_str(), fversion.c_str(), ir, fversion.c_str(), iFAiter-1);
				#endif //UNFOLDINGCHECK
				cout << histn << endl;
				SplineFit *spFit = new SplineFit(histn.c_str(), 1);
				f1Flux[ir] = spFit->f1SplineFit;
			}
			else
			{
				//#if defined TOICORRECTION || defined MCFLUXREWEIGHT
				#if defined TOICORRECTION || defined BELOWL1MC || defined MCFLUXREWEIGHT //2023.02.17
				cout << " ****** Reweigh to Flux shape ****** " << endl;
				//histn = "20200124"; //2020.01.26: Ne: My, Mg: QY, Si: My, S: QY
				//histn = "20200207"; //2020.02.07: Ne: My, Mg: QY, Si: My, S: QY
				//string sDate = "20200724"; //2020.07.24: O: My, Ne: My, Mg: QY, Si: My, S: QY
				//string sDate = "20201018"; //2020.10.18: O: My, F: My, Ne: My, Na:My, Mg: Zhen, Al: Zhen, Si: My, S: QY
				//string sDate = "20201128"; //2020.11.28: O: My, F: My, Ne: My, Na:My, Mg: Zhen, Al: Zhen, Si: My, S: QY
				string sDate = "20220626"; //2022.06.26: O: My, F: My, Ne: My, Na:My, Mg: Zhen, Al: Zhen, Si: My, S: QY
				//2022.07.08
				//if (ev->mch==4) sDate = "20201128";
				if (ev->mch==4 || ev->mch==6) sDate = "20201128";
				//histn = Form("%s/analisi/FluxFit/amsd%dn/%s/fluxfit_amsd%dn_q%d_%s", sWDirOnline.c_str(), dstver, sDate.c_str(), dstver, MCQ, sDate.c_str());
				
				////2020.10.05
				//if (MCQ%2==1) histn = Form("%s/analisi/FluxFit/amsd%dn/%s/fluxfit_amsd%dn_q%d_8hyr_%s", sWDirOnline.c_str(), dstver, sDate.c_str(), dstver, MCQ, sDate.c_str());
				//2020.10.25
				//histn = Form("%s/analisi/FluxFit/amsd%dn/%s/fluxfit_amsd%dn_q%d_8hyr_%s", sWDirOnline.c_str(), dstver, sDate.c_str(), dstver, MCQ, sDate.c_str());
				histn = Form("%s/analisi/FluxFit/FluxShape/%s/fluxfit_amsd%dn_q%d_%s_%s", sWDirOnline.c_str(), sDate.c_str(), dstver, MCQ, sYear.c_str(), sDate.c_str()); //2021.02.01: new directory
				
				if (MCQ==26)
				{
					#ifdef FORCENEWTRIG
					histn = Form("%s/analisi/FluxFit/amsd%dn/%s/fluxfit_amsd%dn_q%d_3hyr_%s", sWDirOnline.c_str(), dstver, sDate.c_str(), dstver, MCQ, sDate.c_str());
					#else
					histn = Form("%s/analisi/FluxFit/amsd%dn/%s/fluxfit_amsd%dn_q%d_8hyr_%s", sWDirOnline.c_str(), dstver, sDate.c_str(), dstver, MCQ, sDate.c_str());
					#endif //FORCENEWTRIG
					//2020.09.09
					#ifdef IRONFORCEOXYGENREWEIGH
					cout << "use Oxygen to reweigh Iron flux (mainly extend to lower rigidity)" << endl;
					histn = Form("%s/analisi/FluxFit/amsd%dn/%s/fluxfit_amsd%dn_q%d_%s", sWDirOnline.c_str(), 63, sDate.c_str(), 63, 8, sDate.c_str());
					#endif //IRONFORCEOXYGENREWEIGH
				}
				
				//use the event rate to reweigh for EffSec //2023.06.28
				#ifdef EFFSEC
				cout << "use fit to Event Rate for EffSec" << endl;
				histn = Form("%s/analisi/FluxFit/FluxShape/%s/Rate/fluxfit_amsd%dn_q%d_%s_%s", sWDirOnline.c_str(), sDate.c_str(), dstver, MCQ, sYear.c_str(), sDate.c_str());
				#endif //EFFSEC
				
				cout << histn << endl;
				
				//SplineFit *spFit = new SplineFit(histn.c_str());
				//cout << histn << " " << spFit << endl;
				//spFit->SetRange(0.1,100000);
				//f1Flux[ir] = spFit->f1SplineFit;
				//--check the accessiblity of the flux file, otherwise use R^-2.7
				if (ifstream(histn).good())
				{
					SplineFit *spFit = new SplineFit(histn.c_str());
					cout << histn << " " << spFit << endl;
					spFit->SetRange(0.1,100000);
					f1Flux[ir] = spFit->f1SplineFit;
				}
				else
				{
					cout << "Cannot access to the flux file, use R^(-2.7)" << endl;
					f1Flux[ir] = new TF1(Form("trueFlux_r%d", ir), "pow(x,-2.7)", 0.1, 10000);
				}
				#else
				cout << " ****** Reweigh to R^(-2.7) ****** " << endl;
				f1Flux[ir] = new TF1(Form("trueFlux_r%d", ir), "pow(x,-2.7)", 0.1, 10000);
				#endif //TOICORRECTION || MCFLUXREWEIGHT
			}
			//use common normalization for r1 & r2 since we will now combine them
			//dNorm[ir] = log(2000)/f1Flux[ir]->Integral(1, 2000);
			//dNorm[ir] = log(2000)/f1Flux[ir]->Integral(1, 2000)*1e10/totGen[0]; //2020.10.25
			/*dNorm[ir] = 1e10/totGen[0]; //2020.11.04*/
		}
		
		//new Gen-Rig for extending l1
		f1GenR = new TF1("1/R", "1/x", 2000, 10000); //set larger range in order to cover different binning
		
		//----plots about MC reweight
		outfile.mkdir("MCReweight");
		outfile.cd("MCReweight");
		
		iNbin = getNbin(ev->mch);
		pBins = getBins(ev->mch);
		histn = Form("rig_generated");
		hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
		
		histn = Form("rig/rR_generated");
		hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
		
		for (int ir=0; ir<3; ir++)
		{
			histn = Form("rig_generated_reweighted_r%d", ir);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
		} //ir
	}
	
	//2021.01.17: mixture for isotope
	//Li:	50%Li6+50%Li7
	////Be:	50%Be7+50%Be9
	//Be:	70%Be7+20%Be9+10%Be10 //2022.07.08: change to 70%Be7+20%Be9+10%Be10, to be the same as Jiahui
	//B:	30%B10+70%B11
	//N:	50%N14+50%N15
	double dIsotopeMixture=1.;
	//2022.08.02
	#ifndef NOISOTOPEMIX
	if (MCQ==3)
	{
		if (MCMass==6 || MCMass==7) dIsotopeMixture=0.5;
	}
	else if (MCQ==4)
	{
		//if (MCMass==7 || MCMass==9) dIsotopeMixture=0.5;
		//2022.07.08
		if (MCMass==7) dIsotopeMixture=0.7;
		else if (MCMass==9) dIsotopeMixture=0.2;
		else if (MCMass==10) dIsotopeMixture=0.1;
	}
	else if (MCQ==5)
	{
		if (MCMass==10) dIsotopeMixture=0.3;
		else if (MCMass==11) dIsotopeMixture=0.7;
		else dIsotopeMixture=1.0;
	}
	else if (MCQ==7)
	{
		if (MCMass==14 || MCMass==15) dIsotopeMixture=0.5;
	}
	#endif //NOISOTOPEMIX
	
	cout << "run_charge=" << MCQ << " mass=" << MCMass << " isotope mixture=" << dIsotopeMixture << endl;
	
	#endif //MCREWEIGHT
	
	//--booking histogram
	outfile.cd();
	
	#ifdef CUTOFFDIS
	histn = "thetam_vs_phim";
	hman1.Add(new TProfile2D(histn.c_str(), histn.c_str(), 370, -185, 185, 140, -70, 70));
	
	histn = "Cutoff_total";
	hman1.Add(new TH1F(histn.c_str(), histn.c_str(), 35000, 0., 35.));
	#endif //CUTOFFDIS
	
	#ifdef RIG_NEWTRQ
	#ifdef ISOTOPE //2021.09.14
	EvenLogBinEkn();
	#endif //ISOTOPE

	#if defined EXPOTIME
	outfile.mkdir("ExposureTime");
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		
	    iNbin = getNbin(*it);
	    pBins = getBins(*it);

	    outfile.mkdir(Form("ExposureTime/q%d", *it));
	    outfile.cd(Form("ExposureTime/q%d", *it));
	    for (int id=0; id<4; id++)
	    {
			//ExpoTime			
			histn = Form("expotime_q%d_d%d", *it, id);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
			
			//--BeBin
			histn = Form("expotime_q%d_d%d_BeBin", *it, id);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(3), getBinsPub(3)));
			
			//--HeBin
			histn = Form("expotime_q%d_d%d_HeBin", *it, id);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(8), getBinsPub(8)));
			
			//--HZBin
			histn = Form("expotime_q%d_d%d_HZBin", *it, id);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14)));
			
			//--FNaAl published binning
			histn = Form("expotime_q%d_d%d_FBin", *it, id);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(9), getBinsPub(9)));
			
			//--Fe published binning
			histn = Form("expotime_q%d_d%d_FeBin", *it, id);
			hman1.Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(26), getBinsPub(26)));
	    }
	}
	#endif //EXPOTIME
	
	//--read the Rich Cal long term time variation correction //2021.12.06
	#ifdef CHECKRICHREFRACINDEX
	int iRCLTCorDstVer = 64;
	string sRCLTCorVer = "RichIndex7";
	string sRCLTCor = Form("%s/analisi/calib/amsd%dn/RichCal_LongTermVar/%s", sWDirOnline.c_str(), iRCLTCorDstVer, sRCLTCorVer.c_str());
	cout << " ****** Reading Rich calibration long Term correction from " << sRCLTCor << " ****** " << endl;
	
	//--read the correction from He, now use the He correction for all nuclei
	int iRCLTCorSpan=0;
	int iRCLTCorQ=2;
	int iRCLTCorRig=1;
	int iRCLTCorEvcut=0;
	int iRCLTCorRigCut[3]={100, 100, 150};
	int iRCLTCorDayGroup[3]={1, 120, 60};
	
	//--
	TF1 *f1RCLT[3] = {NULL, NULL, NULL};
	for (int ibeta=1; ibeta<3; ibeta++)
	{
		//--read the parameter
		string sDayGroup = (iRCLTCorDayGroup[ibeta]==1)?"":Form("_%dDay", iRCLTCorDayGroup[ibeta]);
		string sRCLTCorBeta = Form("%s/Beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d%s", sRCLTCor.c_str(), ibeta, iRCLTCorSpan, iRCLTCorQ, iRCLTCorRig, iRCLTCorEvcut, iRCLTCorRigCut[ibeta], sDayGroup.c_str());
		cout << ibeta << " " << sRCLTCorBeta << endl;
		
		ifstream infile;
		infile.open(sRCLTCorBeta.c_str());
		string sPar;
		getline(infile, sPar, '\n');
		if (sPar=="") continue;
		istringstream issStream(sPar);
		double dOffset, dSlope;
		issStream >> dOffset >> dSlope;
		cout << ibeta << " " << dOffset << " " << dSlope << endl;
		
		//--set the correction function
		f1RCLT[ibeta] = new TF1(Form("MeanLongTerm_beta%d", ibeta), Form("[0]+[1]*x"));
		//f1RCLT[ibeta]->SetParameter(0, dOffset-1);
		f1RCLT[ibeta]->SetParameter(0, dOffset);
		f1RCLT[ibeta]->SetParameter(1, dSlope);
		cout << ibeta << " Day 0=" << f1RCLT[ibeta]->Eval(0) << " Day1500=" << f1RCLT[ibeta]->Eval(1500) << " Day3000=" << f1RCLT[ibeta]->Eval(3000) << endl;
	} //ibeta
	#endif //CHECKRICHREFRACINDEX
	
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		
		//outfile.mkdir(Form("Event_count%d", iQVer));
		/*#ifdef VARDIS
		#ifdef QSELTWIKI
		if (iQVer==0 || (iQVer%3==1 && iQVer>=3))
		#else
		if (iQVer==0 || iQVer%3==1)
		#endif //QSELTWIKI
		{
			sDirVD = Form(Form("Event_count%d/PreSelVD", iQVer));
			ev->BookVarDisInnerTrk(sDirVD.c_str());
			ev->BookVarDisExtTrk(sDirVD.c_str());
			ev->BookVarDisTof(sDirVD.c_str());
			
			//sDirVD = Form(Form("Event_count%d/PreSelVDvsRIG", iQVer));
			//ev->BookVarDisInnerTrkRIG(sDirVD.c_str());
			//ev->BookVarDisExtTrkRIG(sDirVD.c_str());
			//ev->BookVarDisTofRIG(sDirVD.c_str());
		}
		#endif //VARDIS*/
		
		for (it=viQSel.begin(); it!=viQSel.end(); it++)
		{
			#ifndef EFFSEC
			if (!ev->isreal && *it!=ev->mch) continue;
			#endif //EFFSEC
			
			iNbin = getNbin(*it);
			pBins = getBins(*it);
			outfile.mkdir(Form("Event_count%d/q%d", iQVer, *it));
			//for (int ir=1; ir<3; ir++)
			for (int ir=iSpanL; ir<iSpanU; ir++) //2021.02.07
			{
				//outfile.mkdir(Form("Event_count%d/q%d/%s", iQVer, *it, sSpan[ir].c_str()));
				//outfile.cd(Form("Event_count%d/q%d/%s", iQVer, *it, sSpan[ir].c_str()));
				
				//2020.12.31: loop for choutko and kalman rigidity
				//for (int irig=0; irig<2; irig++) //irig=0: choutco, irig=1: kalman
				for (int irig=0; irig<NRig; irig++) //irig=0: choutco, =1: GBL for PASS8, irig=2: kalman //2023.03.10
				{
					#ifdef SELCHECK
					if (irig!=0) continue;
					#endif ///SELCHECK

					#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
					if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
					#endif //PASS7GBL
					
					//string sRig="";
					//if (irig==1) sRig = "_kalman";
					string sRig=sRigName[irig]; //2023.03.10
					
					//new naming for different fitting algorithm//2023.03.10
					outfile.mkdir(Form("Event_count%d/q%d/%s%s", iQVer, *it, sSpan[ir].c_str(), sRig.c_str()));
					outfile.cd(Form("Event_count%d/q%d/%s%s", iQVer, *it, sSpan[ir].c_str(), sRig.c_str()));
					
					
					//apply additional cut //2021.05.23
					for (int ievcut=0; ievcut<4; ievcut++) //0: extra cut, 1: ntrack==1 || no 2nd cut: 2: ntrack==1, 3: no 2nd cut
					{
						#ifdef SELCHECK
						if (ievcut!=0) continue;
						#endif ///SELCHECK

						string sEvCut="";
						if (ievcut>0) sEvCut = Form("_evcut%d", ievcut);
						
						//--store the BeBin, HeBin, HZBin and the rebin event count
						for (int irigcor=0; irigcor<2; irigcor++)
						{
							#ifdef SELCHECK
							if(irigcor!=0) continue;
							#endif //SELCHECK
							string sRigCor = (irigcor==0)?"":"_noRigCor";
							histn = Form("rig_r%d_q%d_%.1fsigma%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//--BeBin
							histn = Form("rig_r%d_q%d_%.1fsigma_BeBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(3), getBinsPub(3)));
							
							//--HeBin
							histn = Form("rig_r%d_q%d_%.1fsigma_HeBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(8), getBinsPub(8)));
							
							//--HZBin
							histn = Form("rig_r%d_q%d_%.1fsigma_HZBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(14), getBinsPub(14)));
							
							//--FNaAl published binning
							histn = Form("rig_r%d_q%d_%.1fsigma_FBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(9), getBinsPub(9)));
							
							//--Fe published binning
							histn = Form("rig_r%d_q%d_%.1fsigma_FeBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), getNbinPub(26), getBinsPub(26)));
							
							#ifdef EVCOUNTINORBIT //2023.08.01
							for (int iRigRange=-1; iRigRange<3; iRigRange++) //iRigRange=-1: all, =0: <10, =1: 10-30, =2: >30
							{
								histn = Form("rigvsorbitm_r%d_q%d%s%s%s", ir, *it, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
								histn += Form("_RigRange%d", iRigRange);
								hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 370, -185, 185, 140, -70, 70));
								
								histn = Form("rigvsorbits_r%d_q%d%s%s%s", ir, *it, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
								histn += Form("_RigRange%d", iRigRange);
								//hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 370, -185, 185, 140, -70, 70));
								hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 370, -5, 365, 140, -70, 70));
							} //iRigRange
							
							//store the event count in different phi range //2023.08.13
							for (int ims=0; ims<2; ims++)
							{
								for (int iphirange=0; iphirange<10; iphirange++)
								{
									double dPhiL = -1.+0.2*iphirange;
									double dPhiU = -1.+0.2*(iphirange+1);
									histn = Form("rig_r%d_q%d%s%s%s", ir, *it, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
									histn += Form("_%.1f_%.1f_Theta", dPhiL, dPhiU);
									histn += (ims==0)?"m":"s";
									hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
								} //iphirange
							} //ims
							#endif //EVCOUNTINORBIT
						} //irigcor
					} //ievcut
					
					#ifdef INNERUNBIAEDL1Q
					if (ir==0)
					{
						histn = Form("UnbiasedL1Q_r%d_q%d%s", ir, *it, sRig.c_str());
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35));
						
						histn = Form("UnbiasedL1Q_r%d_q%d%s_cut", ir, *it, sRig.c_str());
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35));
						
						histn = Form("UnbiasedL1QvsRig_r%d_q%d%s", ir, *it, sRig.c_str());
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
						
						histn = Form("UnbiasedL1QvsRig_r%d_q%d%s_cut", ir, *it, sRig.c_str());
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
						
						#ifdef ISOTOPEBGSUB
						histn = Form("rig_r%d_q%d_%.1fsigma%s_noBGSub", ir, *it, 2.0, sRig.c_str());
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
						#endif //ISOTOPEBGSUB
					}
					#endif //INNERUNBIAEDL1Q
					
					//store Beta vs Rig
					#ifdef ISOTOPE
					//check rich refractive index
					#ifdef CHECKRICHREFRACINDEX
					const double dRefracIR[2][2]={{1.333,1.3345},{1.04,1.06}};
					const int NRefracI[2] = {150, 200};
					//const double dRigCut[3] = {50., 100. ,150.};
					const double dRigCut0=10; //2021.10.27
					//2021.10.26
					const int NRichRadBin=130*2;
					const double dRichRadL=-65, dRichRadU=65;
					//2022.09.17
					//int NDate=3200; //8hyr
					int NDate=3700; //10yr
					
					for (int ibeta=1; ibeta<3; ibeta++)
					{
						if (ir!=0) break;
						//if (irig!=1) break; //2021.11.29
						if (sRig!="_kalman") break; //2023.03.10
						
						//2021.10.26
						int NRichIndexCor=(ibeta==1)?400:1000;
						double dRichIndexCor=(ibeta==1)?0.0002:0.005;
						
						//outfile.mkdir(Form("Event_count%d/q%d/%s/RichIndex%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta));
						//outfile.cd(Form("Event_count%d/q%d/%s/RichIndex%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta));
						//2023.03.10
						outfile.mkdir(Form("Event_count%d/q%d/%s%s/RichIndex%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta));
						outfile.cd(Form("Event_count%d/q%d/%s%s/RichIndex%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta));
						
						for (int ievcut=0; ievcut<2; ievcut++)
						{
							#ifdef PLOTINDEX
							//--0: refractive index after correction, 1: raw refractive index
							for (int iIndex=0; iIndex<2; iIndex++)
							{
								//--vs day
								histn = Form("RefracIndex%dvsDate_beta%d_r%d_q%d_rig%d_evcut%d", iIndex, ibeta, ir, *it, irig, ievcut);
								hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDate, 0, NDate, NRefracI[iIndex], dRefracIR[ibeta-1][0], dRefracIR[ibeta-1][1]));
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("Index (%s)", (iIndex==1)?"raw":"corrected"));
								
								//--vs emission position
								histn = Form("RefracIndex%dvspos_beta%d_r%d_q%d_rig%d_evcut%d", iIndex, ibeta, ir, *it, irig, ievcut);
								hmanQ[iQVer].Add(new TProfile2D(histn.c_str(), histn.c_str(), NRichRadBin, dRichRadL, dRichRadU, NRichRadBin, dRichRadL, dRichRadU));
							} //iIndex
							
							//raw/corrected //2021.10.26
							histn = Form("RefracIndexRawToCorrectedvsDate_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDate, 0, NDate, NRichIndexCor, 1-dRichIndexCor, 1+dRichIndexCor));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("raw/corrected"));
							
							//--vs emission position
							histn = Form("RefracIndexRawToCorrectedvspos_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TProfile2D(histn.c_str(), histn.c_str(), NRichRadBin, dRichRadL, dRichRadU, NRichRadBin, dRichRadL, dRichRadU));
							#endif //PLOTINDEX
							
							//--beta distribution above certain rigidity
							double dBetaLow=0.35, dBetaUp=1.15;
							int iNBeta=800;
							if (ibeta==1) dBetaLow=0.77, dBetaUp=1.02, iNBeta=500;
							//else if (ibeta==2) dBetaLow=0.955, dBetaUp=1.01, iNBeta=1100;
							//else if (ibeta==2) dBetaLow=0.995, dBetaUp=1.005, iNBeta=10000; //2021.09.26
							else if (ibeta==2) dBetaLow=0.99, dBetaUp=1.01, iNBeta=1000; //2022.04.11
							double drBetaLow=0.35, drBetaUp=1.15;
							int iNrBeta=800;
							if (ibeta==1) drBetaLow=0.965, drBetaUp=1.035, iNrBeta=70;
							else if (ibeta==2) drBetaLow=0.985, drBetaUp=1.015, iNrBeta=60;
							
							for (int irigc=0; irigc<3; irigc++)
							{
								/*//--beta vs time
								histn = Form("Beta%dvsDate_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
								//hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDate, 0, NDate, iNBeta, dBetaLow, dBetaUp));
								hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), NDate, 0, NDate, iNBeta, dBetaLow, dBetaUp)); //2021.10.25
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));*/
								
								//--beta and beta vs Time, without and with long term time variation correction
								//for (int iCor=0; iCor<2; iCor++)
								for (int iCor=0; iCor<NLTCor; iCor++) //2022.04.11
								{
									string sRichCor=(iCor==0)?"":"_LongTermCor";
									
									for (int iws=0; iws<nWS; iws++) //iws=0: normal, iws=1: uniformity, iws=2: uniformity+weighted sum
									{
										string sWS=(iws==0)?"":Form("_ws%d", iws);
										//--beta vs time
										histn = Form("Beta%dvsDate_r%d_q%d_rig%d_evcut%d_RigCut%d%s%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str(), sWS.c_str());
										hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), NDate, 0, NDate, iNBeta, dBetaLow, dBetaUp));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
										
										//--beta
										histn = Form("Beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d%s%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str(), sWS.c_str());
										hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp));
										GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
										GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
										
										//--rbeta vs emisison distance //2022.09.11
										if (ibeta==2)
										{
											//--rbeta vs 
											histn = Form("rBeta%dvsEmission_r%d_q%d_rig%d_evcut%d_RigCut%d%s%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str(), sWS.c_str());
											hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), 650, 0, 65, iNrBeta, drBetaLow, drBetaUp));
											GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
											GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
										}
									} //iws
									
									//difference of beta with difference build
									if (nWS==3)
									{
										double dRebuildDiff=0.002;
										if (ibeta==1) dRebuildDiff=dRebuildDiff/5.;
										double dDiff=dRebuildDiff/5.;
										//--beta difference to before rebuild
										histn = Form("BetaDiffRebuild%dvsrig_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str());
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 10000, -dRebuildDiff, dRebuildDiff));
										
										//--beta difference to tileCorrection
										histn = Form("BetaDiffUniformity%dvsrig_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str());
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 10000, -dDiff, dDiff));
										
										//--beta difference to uniformity
										histn = Form("BetaDiffNpeReweighted%dvsrig_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str());
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 10000, -dDiff, dDiff));
									}
								} //iCor
								
								#ifdef PLOTPROFILE
								//--beta vs time, TProfile
								histn = Form("Beta%dvsDate_profile_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
								hmanQ[iQVer].Add(new TProfile(histn.c_str(), histn.c_str(), NDate, 0, NDate));
								GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
								GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
								#endif //PLOTPROFILE
								
								//--beta value as function of reconstructed emission point, using TProfile2D
								#ifdef CHECKRICHINDEXPOS
								histn = Form("Beta%dvsPos_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
								hmanQ[iQVer].Add(new TProfile2D(histn.c_str(), histn.c_str(), NRichRadBin, dRichRadL, dRichRadU, NRichRadBin, dRichRadL, dRichRadU));
								#endif //CHECKRICHINDEXPOS
								
								//--index after beta correction //2021.10.26
								#ifdef PLOTINDEX
								//--0: refractive index after correction, 1: raw refractive index
								for (int iIndex=0; iIndex<2; iIndex++)
								{
									histn = Form("RefracIndex_Beta%dCorvsDate_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
									hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), NDate, 0, NDate, NRefracI[iIndex], dRefracIR[ibeta-1][0], dRefracIR[ibeta-1][1]));
									
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("Index (%s)", (iIndex==1)?"raw":"corrected"));
									
									//--profile
									histn = Form("RefracIndex_Beta%dCorvsDate_profile_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
									hmanQ[iQVer].Add(new TProfile(histn.c_str(), histn.c_str(), NDate, 0, NDate));
									
									//--vs emission position
									histn = Form("RefracIndex_Beta%dCorvspos_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
									hmanQ[iQVer].Add(new TProfile2D(histn.c_str(), histn.c_str(), NRichRadBin, dRichRadL, dRichRadU, NRichRadBin, dRichRadL, dRichRadU));
								} //iIndex
								#endif //PLOTINDEX
								
								//2021.11.02
								#ifdef DIRECTIONDEP
								const int iNDirXY=140;
								const double dNDirXYL=-0.7, dNDirXYU=0.7;
								for (int ixy=0; ixy<2; ixy++)
								{
									//--event count as function of incoming direction
									histn = Form("rig_Beta%d_Dir_xy%d_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ixy, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
									hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNDirXY, dNDirXYL, dNDirXYU));
									
									//--beta distribution as function of incoming direction
									histn = Form("Beta%dvsDir_xy%d_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ixy, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
									hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), iNDirXY, dNDirXYL, dNDirXYU, iNBeta, dBetaLow, dBetaUp));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("v%s", ((ixy==0)?"x":"y")));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
								} //ixy
								#endif //DIRECTIONDEP
							} //irigc
							
							//----beta/beta0 above 10 GV vs time
							//--
							for (int iws=0; iws<nWS; iws++) //iws=0: normal, iws=1: uniformity, iws=2: uniformity+weighted sum
							{
								string sWS=(iws==0)?"":Form("_ws%d", iws);
								histn = Form("BetaToBeta0%dvsDate_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut0, sWS.c_str());
								hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), NDate, 0, NDate, iNBeta, dBetaLow, dBetaUp));
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							} //iws
							
							//--TProfile
							#ifdef PLOTPROFILE
							histn = Form("BetaToBeta0%dvsDate_profile_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
							hmanQ[iQVer].Add(new TProfile(histn.c_str(), histn.c_str(), NDate, 0, NDate));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
							GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("%s Beta", sBeta[ibeta].c_str()));
							#endif //PLOTPROFILE
							
							//--index after beta correction //2021.10.28
							#ifdef PLOTINDEX
							//--0: refractive index after correction, 1: raw refractive index
							for (int iIndex=0; iIndex<2; iIndex++)
							{
								histn = Form("RefracIndex_BetaToBeta0%dCorvsDate_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
								hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), NDate, 0, NDate, NRefracI[iIndex], dRefracIR[ibeta-1][0], dRefracIR[ibeta-1][1]));
								
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("day");
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("Index (%s)", (iIndex==1)?"raw":"corrected"));
								
								//--profile
								histn = Form("RefracIndex_BetaToBeta0%dCorvsDate_profile_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
								hmanQ[iQVer].Add(new TProfile(histn.c_str(), histn.c_str(), NDate, 0, NDate));
								
								//--vs emission position
								histn = Form("RefracIndex_BetaToBeta0%dCorvspos_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
								hmanQ[iQVer].Add(new TProfile2D(histn.c_str(), histn.c_str(), NRichRadBin, dRichRadL, dRichRadU, NRichRadBin, dRichRadL, dRichRadU));
							} //iIndex
							#endif //PLOTINDEX
							
						} //ievcut
					} //ibeta
					#endif //CHECKRICHREFRACINDEX
					
					//2021.12.21
					//2022.01.25: store the variable vs beta
					#ifdef RICHVARCHECK
					for (int ibeta=1; ibeta<3; ibeta++)
					{
						outfile.mkdir(Form("Event_count%d/q%d/%s%s/RichVarCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta));
						outfile.cd(Form("Event_count%d/q%d/%s%s/RichVarCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta));
						//--
						string sVar="";
						string sVarFull="";
						
						//double dBetaLow=0.35, dBetaUp=1.15;
						//int iNBeta=800;
						//if (ibeta==1) dBetaLow=0.77, dBetaUp=1.02, iNBeta=500;
						//else if (ibeta==2) dBetaLow=0.995, dBetaUp=1.005, iNBeta=10000;
						double dBetaLow=0.35, dBetaUp=1.15;
						int iNBeta=800;
						if (ibeta==1) dBetaLow=0.965, dBetaUp=1.035, iNBeta=70;
						else if (ibeta==2) dBetaLow=0.985, dBetaUp=1.015, iNBeta=60;
						
						for (int ievcut=0; ievcut<1; ievcut++)
						{
							//--No. of PMTs
							sVar="NoPMTs";
							sVarFull="No. PMTs";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 200, 0, 200));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 200, 0, 200));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--No. of Hits
							sVar="NoHits";
							sVarFull="No. Hits";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 100));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 100));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Kolmogorov
							sVar="Kolmogorov";
							sVarFull="Kolmogorov Probability";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 1));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 1));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Hit/PMT charge consistency
							sVar="HitPMTQCons";
							sVarFull="Hit/PMT charge consistency";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 1500, 0, 150));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 1500, 0, 150));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Npe(Ring)
							sVar="NpeRing";
							sVarFull="N_{pe}(Ring)";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 1000, 0, 1000));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Npe(Ring)/Z^2 //2022.01.25
							sVar="NormNpeRing";
							sVarFull="Norm N_{pe}(Ring)";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 100));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 100));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Npe(Total)
							sVar="NpeTotal";
							sVarFull="N_{pe}(Total)";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 1000, 0, 1000));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 1000, 0, 1000));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Npe(Total)/Z^2 //2022.01.25
							sVar="NormNpeTotal";
							sVarFull="Norm N_{pe}(Total)";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 100));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 100));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Npe(Ring)/Npe(Total)
							sVar="NpeRingToNpeTotal";
							sVarFull="N_{pe}(Ring)/N_{pe}(Total)";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 1));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 1));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--RichQ
							sVar="RichQ";
							sVarFull="Rich charge";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 3500, 0, 35));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 3500, 0, 35));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--|beta_tof-beta_rich|/beta_rich
							histn = Form("BetaDiffvsBetaRich_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 1000, 0, 1));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s #beta", sBetaFull[ibeta].c_str()));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("|#beta_{TOF}-#beta_{Rich}|"));
							
							//--|beta_LIP-beta_CIEMAT|
							sVar="BetaDiffRec";
							sVarFull="|#beta_{LIP}-#beta_{CIEMAT}|";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 0.2));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 0.2));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
							
							//--Expected Npe
							sVar="ExpectedNpe";
							sVarFull="Expected N_{pe}";
							histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 100, 0, 100));
							GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							
							#ifdef RICHVARCHECK2D
							//2d
							histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, 100, 0, 100));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull.c_str());
							#endif //RICHVARCHECK2D
						} //ievcut
					} //ibeta
					#endif //RICHVARCHECK
					
					//2022.08.02
					#ifdef RICHVARCHECKRESO
					for (int ibeta=1; ibeta<3; ibeta++)
					{
						//if (irig==1) continue;
						if (irig>=0) continue; //2023.03.10
						outfile.mkdir(Form("Event_count%d/q%d/%s%s/RichVarCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta));
						outfile.cd(Form("Event_count%d/q%d/%s%s/RichVarCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta));
						//--
						double dBetaLow=0.35, dBetaUp=1.15;
						int iNBeta=800;
						if (ibeta==1) dBetaLow=0.965, dBetaUp=1.035, iNBeta=70;
						else if (ibeta==2) dBetaLow=0.985, dBetaUp=1.015, iNBeta=60;
						
						//for (int ievcut=0; ievcut<2; ievcut++)
						for (int ievcut=1; ievcut<2; ievcut++)
						{
							outfile.mkdir(Form("Event_count%d/q%d/%s%s/RichVarCheck%s_Beta%d/evcut%d", iQVer, *it, sSpan[ir].c_str(),  sRig.c_str(), sRig.c_str(), ibeta, ievcut));
							outfile.cd(Form("Event_count%d/q%d/%s%s/RichVarCheck%s_Beta%d/evcut%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta, ievcut));
							for (int irvar=0; irvar<((ibeta==1)?8:9); irvar++)
							{
								int Nbin;
								double dBinL, dBinU;
								vector<double> vdELB;
								if (irvar==0) Nbin=200, dBinL=0, dBinU=200;
								else if (irvar==1) Nbin=100, dBinL=0, dBinU=1;
								else if (irvar==2) Nbin=100, dBinL=0, dBinU=1;
								else if (irvar==3) Nbin=1500, dBinL=0, dBinU=150;
								else if (irvar==4) Nbin=200, dBinL=0, dBinU=0.2;
								//else if (irvar==5) Nbin=3500, dBinL=0, dBinU=35;
								else if (irvar==5) Nbin=1000, dBinL=1.5, dBinU=11.5;
								//else if (irvar==6) Nbin=100, dBinL=0, dBinU=0.2;
								else if (irvar==6) //finner binning and even log bin//2022.09.09
								{
									Nbin=2000, dBinL=1e-4, dBinU=2*1e-1;
									vdELB.push_back(dBinL);
									for (int ibin=1; ibin<Nbin+1; ibin++)
									{
										double dBin=dBinL*pow(dBinU/dBinL, (double)ibin/Nbin);
										vdELB.push_back(dBin);
									} //ibin
								}
								//else if (irvar==7) Nbin=100, dBinL=0, dBinU=100;
								else if (irvar==7) Nbin=1000, dBinL=0, dBinU=100; //finner binning for Expected Npe, no need to be integer //2022.09.09
								else if (irvar==8) Nbin=800, dBinL=0, dBinU=80;
								
								for (int irichcor=0; irichcor<2; irichcor++) //irichcor=0: tileCorreciton, irichcor=1: fullUniformityCorrection
								{
									if (!ev->isreal && irichcor>0) break;
									string sRichBetaCor = (irichcor==0)?"":"_fullUniformityCor";
									
									//--
									histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut);
									histn += sRichBetaCor;
									//hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), Nbin, dBinL, dBinU));
									if (irvar!=6) hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), Nbin, dBinL, dBinU));
									else hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), Nbin, &vdELB[0]));
									GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull[irvar].c_str());
									
									//--
									#ifdef RICHVARCHECKRESO2D
									//2d
									//histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut);
									//hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, Nbin, dBinL, dBinU));
									//GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull[irvar].c_str());
									//GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(sVarFull[irvar].c_str());
									//--store for rigidity cut in all 3 range
									for (int irigcut=0; irigcut<3; irigcut++)
									{
										histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d_above%.1fGV", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut, dRigCut[irigcut]);
										histn += sRichBetaCor;
										if (irvar!=6) hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, Nbin, dBinL, dBinU));
										else hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta, dBetaLow, dBetaUp, Nbin, &vdELB[0]));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("1/Beta");
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(sVarFull[irvar].c_str());
										//--some special 2d plots //2022.09.09
										if (irvar==3) //charge consistence vs rich Q
										{
											histn = Form("%svsRichQ_beta%d_r%d_q%d_rig%d_evcut%d_above%.1fGV", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut, dRigCut[irigcut]);
											histn += sRichBetaCor;
											hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 1000, 1.5, 11.5, Nbin, dBinL, dBinU));
											GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rich charge [c.u.]");
											GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(sVarFull[irvar].c_str());
										}
										else if (irvar==4) //TofRichBetaDiff vs TofBeta and vs RichBeta
										{
											for (int itofrich=0; itofrich<2; itofrich++)
											{
												double dBeta0Low=0.35, dBeta0Up=1.15;
												int iNBeta0=800;
												//if (itofrich==1) dBeta0Low = dBetaLow, dBeta0Up = dBetaUp, iNBeta0 = iNBeta;
												if (itofrich==1)
												{
													if (ibeta==1) dBeta0Low=0.77, dBeta0Up=1.02, iNBeta0=500;
													else if (ibeta==2) dBeta0Low=0.955, dBeta0Up=1.01, iNBeta0=1100;
												}
												string sTofRich = (itofrich==0)?"TOF":"RICH";
												histn = Form("%svs%sbeta_beta%d_r%d_q%d_rig%d_evcut%d_above%.1fGV", sVar[irvar].c_str(), sTofRich.c_str(), ibeta, ir, *it, irig, ievcut, dRigCut[irigcut]);
												histn += sRichBetaCor;
												hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta0, dBeta0Low, dBeta0Up, Nbin, dBinL, dBinU));
												GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s beta", sBeta[(itofrich==0)?0:ibeta].c_str()));
												GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(sVarFull[irvar].c_str());
											} //itofrich
										}
									} //irigcut
									
									//--some special 2d plots //2022.09.09
									if (irvar==3) //charge consistence vs rich Q
									{
										histn = Form("%svsRichQ_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut);
										histn += sRichBetaCor;
										//hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 3500, 0, 35, Nbin, dBinL, dBinU));
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 1000, 1.5, 11.5, Nbin, dBinL, dBinU));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rich charge [c.u.]");
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(sVarFull[irvar].c_str());
									}
									else if (irvar==4) //TofRichBetaDiff vs TofBeta and vs RichBeta
									{
										for (int itofrich=0; itofrich<2; itofrich++)
										{
											double dBeta0Low=0.35, dBeta0Up=1.15;
											int iNBeta0=800;
											//if (itofrich==1) dBeta0Low = dBetaLow, dBeta0Up = dBetaUp, iNBeta0 = iNBeta;
											if (itofrich==1)
											{
												if (ibeta==1) dBeta0Low=0.77, dBeta0Up=1.02, iNBeta0=500;
												else if (ibeta==2) dBeta0Low=0.955, dBeta0Up=1.01, iNBeta0=1100;
											}
											string sTofRich = (itofrich==0)?"TOF":"RICH";
											histn = Form("%svs%sbeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), sTofRich.c_str(), ibeta, ir, *it, irig, ievcut);
											histn += sRichBetaCor;
											hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNBeta0, dBeta0Low, dBeta0Up, Nbin, dBinL, dBinU));
											GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s beta", sBeta[(itofrich==0)?0:ibeta].c_str()));
											GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(sVarFull[irvar].c_str());
										} //itofrich
									}
									#endif //RICHVARCHECKRESO2D
								} //irichcor
							} //irvar
						} //ievcut
					} //ibeta
					#endif //RICHVARCHECKRESO
					
					//--check the rich tile position using no. event vs hit pos //2022.09.17
					#ifdef CHECKRICHGEO
					for (int ibeta=1; ibeta<3; ibeta++)
					{
						if (ir!=0) break;
						//outfile.mkdir(Form("Event_count%d/q%d/%s/RichGeoCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta));
						//outfile.cd(Form("Event_count%d/q%d/%s/RichGeoCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta));
						//--
						double dBetaLow=0.35, dBetaUp=1.15;
						int iNBeta=800;
						if (ibeta==1) dBetaLow=0.965, dBetaUp=1.035, iNBeta=70;
						else if (ibeta==2) dBetaLow=0.985, dBetaUp=1.015, iNBeta=60;
						
						//--
						double dSizeL=-65, dSizeU=65;
						int NSize=1300;
						
						for (int ievcut=0; ievcut<1; ievcut++)
						{
							//--geometry check
							histn = Form("Event_count%d/q%d/%s%s/RichGeoCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta);
							histn += Form("/evcut%d", ievcut);
							outfile.mkdir(histn.c_str());
							//outfile.mkdir(Form("%s/Pos_in_PMT", histn.c_str())); //2022.11.30
							outfile.cd(histn.c_str());
							
							for (int irigcut=-1; irigcut<3; irigcut++)
							{
								string sRigCut = (irigcut==-1)?"":Form("_above%.1fGV", dRigCut[irigcut]);
								histn = Form("EmisPos_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
								histn += sRigCut;
								hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), NSize, dSizeL, dSizeU, NSize, dSizeL, dSizeU));
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Impact position in radiator X [cm]");
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("Impact position in radiator Y [cm]");
							} //irigcut
							
							//--check the portion of reflected hits and relation with beta
							double dBetaLow=0.35, dBetaUp=1.15;
							int iNBeta=800;
							if (ibeta==1) dBetaLow=0.77, dBetaUp=1.02, iNBeta=500;
							//else if (ibeta==2) dBetaLow=0.955, dBetaUp=1.01, iNBeta=1100;
							//else if (ibeta==2) dBetaLow=0.995, dBetaUp=1.005, iNBeta=10000; //2021.09.26
							else if (ibeta==2) dBetaLow=0.99, dBetaUp=1.01, iNBeta=1000; //2022.04.11
							double drBetaLow=0.35, drBetaUp=1.15;
							int iNrBeta=800;
							if (ibeta==1) drBetaLow=0.965, drBetaUp=1.035, iNrBeta=70;
							else if (ibeta==2) drBetaLow=0.985, drBetaUp=1.015, iNrBeta=60;
							
							for (int ivar=0; ivar<3; ivar++) //ivar=0: 1/beta vs no. reflected photon, =1: 1/beta vs no. reflected photon/no. used photon, =2: 1/beta vs no. reflected photon/no. photon on ring
							{
								string sRefl;
								if (ivar==0) sRefl= "noRefl";
								else if (ivar==1) sRefl = "noRefltonoUsed";
								else if (ivar==2) sRefl = "noRefltonoOnring";
								int NReflBin=100;
								double dReflL=0, dReflU=100;
								if (ivar>0) dReflU=1.;
								
								for (int irigcut=-1; irigcut<3; irigcut++)
								{
									string sRigCut = (irigcut==-1)?"":Form("_above%.1fGV", dRigCut[irigcut]);
									histn = Form("rbetavs%s_beta%d_r%d_q%d_rig%d_evcut%d", sRefl.c_str(), ibeta, ir, *it, irig, ievcut);
									histn += sRigCut;
									hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), NReflBin, dReflL, dReflU, iNrBeta, drBetaLow, drBetaUp));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s", sRefl.c_str()));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("1/(%s Beta)", sBeta[ibeta].c_str()));
								} //irigcut
							} //ivar
							
							//--use MC Truth to check the recongnization of direct, reflected and noise
							#ifdef CHECKMCCLUSTER
							for (int idrn=0; idrn<4; idrn++) //idrn=0: direct, =1: reflected, =2: noise, =3: no corresponding RichMCClusterR found
							{
								for (int igr=0; igr<2; igr++)
								{
									histn = Form("Eff_drn%d", idrn);
									histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
									if (igr==0) hmanQ[iQVer].Add(new TEfficiency(histn.c_str(), histn.c_str(), iNbinGenRig, pBinsGenRig));
									else hmanQ[iQVer].Add(new TEfficiency(histn.c_str(), histn.c_str(), getNbin(*it), getBins(*it)));
									//GetTEff(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("%s Rigidity [GV]", ((igr==0)?"Generated":"Reconstructed")));
								} //igr
							} //idrn
							
							//--store the angle between RichMCCluster origin to hit and emission point to hit, for direct and reflected separately
							for (int idr=0; idr<2; idr++)
							{
								for (int idrn=0; idrn<2; idrn++)
								{
									//--Distance vs drratio
									for (int ixy=0; ixy<3; ixy++) //ixy=0, 1: X and Y difference, =2: sqrt(x^2+y^2)
									{
										histn = Form("DistvsDRRatio_dr%d_xy%d", idr, ixy);
										histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
										histn += Form("_drn%d", idrn);
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 101, -0.01, 1., 400, -50., 50.));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("rich_usedm/rich_used"));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("Distance"));
									} //ixy
									
									//--Angle
									histn = Form("Angle_dr%d", idr);
									histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
									histn += Form("_drn%d", idrn);
									hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 350, -0.5, 3));
									GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("angle"));
									
									//--Angle vs drratio
									histn = Form("AnglevsDRRatio_dr%d", idr);
									histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
									histn += Form("_drn%d", idrn);
									hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 101, -0.01, 1., 350, -0.5, 3));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("rich_usedm/rich_used"));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("angle"));
									
									//--Angle vs rig
									int igr=0;
									histn = Form("AnglevsRig_dr%d", idr);
									histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
									histn += Form("_drn%d", idrn);
									hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), getNbin(*it), getBins(*it), 400., -2, 2));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("Generated Rigidity [GV]"));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("angle"));
									
									/*//--position in PMT from Origin point for hits in given no. pmt //2022.11.30
									histn = Form("Event_count%d/q%d/%s/RichGeoCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta);
									histn += Form("/evcut%d", ievcut);
									outfile.cd(Form("%s/Pos_in_PMT", histn.c_str())); //2022.11.30
									for (int ipmt=0; ipmt<200; ipmt++)
									//for (int ipmt=0; ipmt<680; ipmt++) //680 pmts in total, but too large
									{
										histn = Form("PMTPosFormOrigin_pmt%d_dr%d", ipmt, idr);
										histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 280, -70, 70, 280, -70, 70));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("X [cm]"));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("Y [cm]"));
									} //ipmt
									histn = Form("Event_count%d/q%d/%s/RichGeoCheck%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ibeta);
									histn += Form("/evcut%d", ievcut);
									outfile.cd(histn.c_str());*/
								} //drn
							} //idr
							
							//--
							for (int idr=0; idr<2; idr++)
							{
								for (int irec=-1; irec<2; irec++) //irec=-1: all, =0: correctly reconstructed, =1: wrongly reconstructed
								{
									for (int irigcut=0; irigcut<3; irigcut++)
									{
										//1/beta
										histn = Form("rBetavsDRRatio_dr%d", idr);
										if (irec>=0) histn += Form("_rec%d", irec);
										histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
										histn += Form("_above%.1fGV", dRigCutOnlyDR[irigcut]);
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 101, -0.01, 1., iNrBeta, drBetaLow, drBetaUp));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("rich_usedm/rich_used"));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("1/#beta"));
										
										//1-beta, i.e. true - rec
										double dBetaBiasR = (ibeta==1)?4e-2:4e-3;
										histn = Form("BetaBiasvsDRRatio_dr%d", idr);
										if (irec>=0) histn += Form("_rec%d", irec);
										histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
										histn += Form("_above%.1fGV", dRigCutOnlyDR[irigcut]);
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 101, -0.01, 1., 400, -dBetaBiasR, dBetaBiasR));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("rich_usedm/rich_used"));
										GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("1/#beta"));
									} //irigcut
								} //irec
							} //idr
							#endif //CHECKMCCLUSTER
							
							//--beta using only direct or reflected hits //2022.11.09
							#ifdef ONLYDRHITBETA
							if (irig!=0) continue;
							if (ibeta==1) dBetaLow=0.77, dBetaUp=1.02, iNBeta=500;
							//else if (ibeta==2) dBetaLow=0.99, dBetaUp=1.01, iNBeta=1000; //2022.04.11
							else if (ibeta==2) dBetaLow=0.99, dBetaUp=1.01, iNBeta=200; //2022.04.11
							//if (ibeta==1) drBetaLow=0.965, drBetaUp=1.035, iNrBeta=70;
							//else if (ibeta==2) drBetaLow=0.985, drBetaUp=1.015, iNrBeta=60;
							//--
							drBetaLow=0.9, drBetaUp=1.5;
							iNrBeta=600;
							//if (ibeta>=1) drBetaLow=0.99, drBetaUp=1.05, iNrBeta=1200;
							if (ibeta==2) drBetaLow=0.99, drBetaUp=1.01, iNrBeta=200;
							
							//--
							const int NDRRatio = 101;
							const double dDRRatioL = -0.01, dDRRatioU=1.;
							
							//--
							double dBetaDiffR=(ibeta==1)?0.03:0.006;
							
							//--
							const string sDR[3] = {"All", "Direct", "Reflected"};
							
							//--
							//for (int icluster=0; icluster<2; icluster++) //icluster=0: build ring only using direct or reflected hits, =1: build ring use both direct and reflected hits but use average only direct or reflected hits
							for (int icluster=1; icluster<2; icluster++) //icluster=0: build ring only using direct or reflected hits, =1: build ring use both direct and reflected hits but use average only direct or reflected hits
							{
								//for (int idr=0; idr<2; idr++) //idr=0: direct, idr=1: reflected
								for (int idr=-1; idr<2; idr++) //idr=-1: all hits (standard), 0: direct, idr=1: reflected
								{
									//for (int ire=0; ire<2; ire++) //ire=0: vs rig, ire=1: vs Ekn
									for (int ire=0; ire<1; ire++) //ire=0: vs rig, ire=1: vs Ekn
									{
										//--store the histograms in different direct/total region //2022.11.15 //TODO
										histn = Form("Event_count%d/q%d/%s%s/OnlyDR%s_Beta%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), sRig.c_str(), ibeta);
										histn += Form("/evcut%d", ievcut);
										histn += Form("/Clustering%d", icluster);
										histn += Form("/%s_%s", sDR[idr+1].c_str(), ((ire==0)?"Rig":"Ekn"));
										outfile.mkdir(histn.c_str());
										outfile.cd(histn.c_str());
										//for (int igr=0; igr<2; igr++)
										/*for (int igr=1; igr<2; igr++)
										{
											int iNbinGR=0;
											double *pBinsGR=NULL;
											if (ire==0)
											{
												iNbinGR=(igr==0)?iNbinGenRig:getNbin(*it);
												pBinsGR=(igr==0)?pBinsGenRig:getBins(*it);
											}
											else
											{
												iNbinGR=(igr==0)?iNbinGenEkn:iLiNBin;
												pBinsGR=(igr==0)?pBinsGenEkn:pLiBins;
											}
											
											for (int idrratio=-1; idrratio<NDRRatioRegion; idrratio++)
											{
												if (icluster==0 && idrratio>=0) break;
												string sDrRatio = (idrratio<0)?"":Form("_DrRatioBin%d", idrratio);
												//--beta vs energy variable
												histn = Form("Betavs%s", ((ire==0)?"Rig":"Ekn"));
												histn += Form("_cl%d", icluster);
												if (idr>=0) histn += Form("_dr%d", idr);
												histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
												histn += sDrRatio;
												hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGR, pBinsGR, iNBeta, dBetaLow, dBetaUp));
												
												//--1/beta vs energy variable
												histn = Form("rBetavs%s", ((ire==0)?"Rig":"Ekn"));
												histn += Form("_cl%d", icluster);
												if (idr>=0) histn += Form("_dr%d", idr);
												histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
												histn += sDrRatio;
												hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGR, pBinsGR, iNrBeta, drBetaLow, drBetaUp));
												
												//--beta diff vs energy variable
												if (idr<0) continue;
												for (int idiff=0; idiff<3; idiff++) //idiff=0: diff between direct-only and total, =1: diff between reflected-only and total, =2: diff between direct-only and reflected-only
												{
													histn = Form("BetaDiff%dvs%s", idiff, ((ire==0)?"Rig":"Ekn"));
													histn += Form("_cl%d_dr%d", icluster, idr);
													histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
													histn += sDrRatio;
													hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbinGR, pBinsGR, 600, -dBetaDiffR, dBetaDiffR));
												}
											} //idrratio
										} //igr*/
									} //ire
									
									//--
									for (int irigcut=0; irigcut<3; irigcut++)
									//for (int irigcut=2; irigcut<3; irigcut++)
									{
										string sRigCutOnlyDR = Form("_RigCut%.0fGV", dRigCutOnlyDR[irigcut]);
										for (int icor=0; icor<NRichCor; icor++) //2022.11.22
										{
											string sRichCor = (icor==0)?"":"_WithCor";
											
											//--
											histn = Form("BetavsDRRatio");
											histn += Form("_cl%d", icluster);
											if (idr>=0) histn += Form("_dr%d", idr);
											histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
											histn += sRigCutOnlyDR;
											histn += sRichCor; //2022.11.22
											hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDRRatio, dDRRatioL, dDRRatioU, iNBeta, dBetaLow, dBetaUp));
											
											//--1/beta vs energy variable
											histn = Form("rBetavsDRRatio");
											histn += Form("_cl%d", icluster);
											if (idr>=0) histn += Form("_dr%d", idr);
											histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
											histn += sRigCutOnlyDR;
											histn += sRichCor; //2022.11.22
											hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDRRatio, dDRRatioL, dDRRatioU, iNrBeta, drBetaLow, drBetaUp));
											
											//--beta diff vs energy variable
											if (idr<0) continue;
											for (int idiff=0; idiff<3; idiff++) //idiff=0: diff between direct-only and total, =1: diff between reflected-only and total, =2: diff between direct-only and reflected-only
											{
												//--
												histn = Form("BetaDiff%dvsDRRatio", idiff);
												histn += Form("_cl%d_dr%d", icluster, idr);
												histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
												histn += sRigCutOnlyDR;
												histn += sRichCor; //2022.11.22
												hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDRRatio, dDRRatioL, dDRRatioU, 600, -dBetaDiffR, dBetaDiffR));
												
												//--2d distribution
												for (int iDRRatioBin=0; iDRRatioBin<3; iDRRatioBin++)
												{
													if (idiff<2) histn = Form("BetaDR%dvsBeta", idiff);
													else histn = Form("BetaDR0vsBetaDR1");
													histn += Form("_cl%d_dr%d", icluster, idr);
													histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
													histn += sRigCutOnlyDR;
													histn += sRichCor; //2022.11.22
													histn += Form("_DRRatioBin%d", iDRRatioBin);
													hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNrBeta, drBetaLow, drBetaUp, iNrBeta, drBetaLow, drBetaUp));
												} //iDRRatioBin
											}
										} //icor
										
										//--no. direct and reflected vs DRRatio
										/*for (int idr=-1; idr<2; idr++)
										{
											histn = Form("NHitvsDRRatio");
											histn += Form("_cl%d", icluster);
											if (idr>=0) histn += Form("_dr%d", idr);
											histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
											histn += sRigCutOnlyDR;
											hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDRRatio, dDRRatioL, dDRRatioU, 100, 0, 100));
										} //idr*/
										histn = Form("NHitvsDRRatio");
										histn += Form("_cl%d", icluster);
										if (idr>=0) histn += Form("_dr%d", idr);
										histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
										histn += sRigCutOnlyDR;
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), NDRRatio, dDRRatioL, dDRRatioU, 100, 0, 100));
									} //irigcut
									
									//--no. direct and reflected vs beta
									for (int igr=0; igr<2; igr++)
									{
										/*for (int idr=-1; idr<2; idr++)
										{
											histn = Form("NHitvsBeta");
											histn += Form("_cl%d", icluster);
											if (idr>=0) histn += Form("_dr%d", idr);
											histn += Form("_gr%d", igr);
											histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
											hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 110, 0.9, 1.01, 100, 0, 100));
										} //idr*/
										histn = Form("NHitvsBeta");
										histn += Form("_cl%d", icluster);
										if (idr>=0) histn += Form("_dr%d", idr);
										histn += Form("_gr%d", igr);
										histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 110, 0.9, 1.01, 100, 0, 100));
									} //igr
								} //idr
							} //icluster
							#endif //ONLYDRHITBETA
						} //ievcut
					} //ibeta
					#endif //CHECKRICHGEO
					#endif //ISOTOPE
					
					//2021.09.28
					#ifdef TRDINTERCON
					if (ir!=0) continue;
					if (irig==0) outfile.mkdir(Form("Event_count%d/q%d/%s%s/TrdInterCon", iQVer, *it, sSpan[ir].c_str(), sRig.c_str()));
					outfile.cd(Form("Event_count%d/q%d/%s%s/TrdInterCon", iQVer, *it, sSpan[ir].c_str(), sRig.c_str()));
					
					for (int ievcut=0; ievcut<2; ievcut++) //0: extra cut, 1: ntrack==1 || no 2nd cut
					{
						for (int itrdcut=0; itrdcut<2; itrdcut++)
						{
							string sEvCut = (ievcut>0)?Form("_evcut%d", ievcut):"";
							sEvCut += Form("_TrdCut%d", itrdcut);
							
							//--event counts that without unbiased L1Q cut
							histn = Form("rig_r%d_q%d_TrdInterCon%s%s", ir, *it, sRig.c_str(), sEvCut.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							histn = Form("rig_r%d_q%d_TrdInterCon%s%s_noRigCor", ir, *it, sRig.c_str(), sEvCut.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
							
							//--
							for (int itrdqpart=0; itrdqpart<3; itrdqpart++) //itrdqpart=0: total, =1: upper 10 layer, =2: lower 10 layer
							{
								for (int itrdq=0; itrdq<3; itrdq++) //itrdq=0: dE/dX + delta ray, =1: dE/dX, =2: delta ray
								{
									if (itrdq>0 && itrdqpart>0) continue;
									//--L1Q vs Trd Q
									histn = Form("L1Qvstrdq%d_%s_r%d_q%d_TrdInterCon%s%s", itrdq, sTrdqPart[itrdqpart].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
									//hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
									hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, i2DBin, iQDL, iQDU));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle(Form("trdq%d_%s", itrdq, sTrdqPart[itrdqpart].c_str()));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("unbiased L1Q");
									
									//--Qrms vs L1Q
									if (itrdqpart!=0) continue;
									histn = Form("trdq%drms_%s_vsL1Q_r%d_q%d_TrdInterCon%s%s", itrdq, sTrdqPart[itrdqpart].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
									//hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 1000, 0, 10));
									hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 1000, 0, 10));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("unbiased L1Q");
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("trdq%d_%s_rms", itrdq, sTrdqPart[itrdqpart].c_str()));
									
									//--upper 10 Q lower 10 Q asymmetry
									if (itrdq!=0) continue;
									histn = Form("trdq%d_%s_%s_Asy_vsL1Q_r%d_q%d_TrdInterCon%s%s", itrdq, sTrdqPart[1].c_str(), sTrdqPart[2].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
									//hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 200, -1, 1));
									hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 200, -1, 1));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("unbiased L1Q");
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("trdq%d asymetry: (U10-L10)/(U10+L10)", itrdq));
									
									//--dE/dX only vs delta ray only asymmetry
									histn = Form("trdq%d_trdq%d_%s_Asy_vsL1Q_r%d_q%d_TrdInterCon%s%s", 1, 2, sTrdqPart[0].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
									//hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 200, -1, 1));
									hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), i1DBin, iQDL, iQDU, 200, -1, 1));
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("unbiased L1Q");
									GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("trdq asymetry: (dEdX-delta)/(dEdx+delta)"));
								} //itrdq
								
								//--Nhit
								histn = Form("Nhit_%s_vsL1Q_r%d_q%d_TrdInterCon%s%s", sTrdqPart[itrdqpart].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
								//hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 200, 0, 200));
								hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 200, 0, 200));
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("unbiased L1Q");
								GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("nhit_%s", sTrdqPart[itrdqpart].c_str()));
							} //itrdqpart
							
							//--ipch
							histn = Form("ipchvsL1Q_r%d_q%d_TrdInterCon%s%s", ir, *it, sRig.c_str(), sEvCut.c_str());
							//hmanQ[iQVer].Add(new TH2S(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 1000, 0, 10));
							hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), i2DBin, iQDL, iQDU, 1000, 0, 10));
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("unbiased L1Q");
							GetHist2d(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle(Form("ipch"));
							
						} //itrdcut
					} //ievcut
					#endif //TRDINTERCON
					
					if (irig>0) continue;
					
					//-- //2023.01.13
					#ifdef TRDADC
					#ifdef TRDADCOLD
					outfile.mkdir(Form("TRD_ADC%d/q%d/%s%s", iQVer, *it, sSpan[ir].c_str(), sRig.c_str()));
					
					for (int imatch=0; imatch<1; imatch++) //different method for match the TRD hit with particle, =0: TRD hit around with Tracker Track extrapolation (store already in MIT dst), =1: maximum hit in each TRD layer (not implemented yet)
					{
						outfile.mkdir(Form("TRD_ADC%d/q%d/%s%s/match%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), imatch));
						
						//for (int ievcut=0; ievcut<2; ievcut++)
						for (int ievcut=0; ievcut<1; ievcut++)
						{
							outfile.mkdir(Form("TRD_ADC%d/q%d/%s%s/match%d/evcut%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), imatch, ievcut));
							outfile.cd(Form("TRD_ADC%d/q%d/%s%s/match%d/evcut%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), imatch, ievcut));
							
							//for (int iplcor=0; iplcor<2; iplcor++) //path length correciton, iplcor=0: without correction, =1: after correction
							for (int iplcor=0; iplcor<1; iplcor++) //path length correciton, iplcor=0: without correction, =1: after correction
							{
								for (int il=0; il<20; il++)
								{
									/*//--adc
									histn = Form("TRDAdc%d_r%d_q%d_TrdL%d", imatch, ir, *it, il);
									histn += Form("_evcut%d", ievcut);
									hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), NTRDADC, 0, NTRDADC));*/
									
									//--adc vs rig
									histn = Form("TRDAdc%dvsRig_r%d_q%d_TrdL%d_PLCor%d", imatch, ir, *it, il, iplcor);
									histn += Form("_evcut%d", ievcut);
									hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, NTRDADC, 0, NTRDADC));
									
									//--adc vs InnerQ //2023.01.18
									histn = Form("TRDAdc%dvsInnerQ_r%d_q%d_TrdL%d_PLCor%d", imatch, ir, *it, il, iplcor);
									histn += Form("_evcut%d", ievcut);
									hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 500, 0, 10, NTRDADC, 0, NTRDADC));
								} //il
								
								//--average adc from all layer vs rig
								for (int isum=0; isum<NTRDADCSum; isum++) //isum=0: average, =1: average without min, =2: average without max, =3: average without both min and max
								{
									histn = Form("TRDAdc%dvsRig_r%d_q%d_PLCor%d", imatch, ir, *it, iplcor);
									histn += Form("_evcut%d", ievcut);
									histn += Form("_sum%d", isum);
									hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, NTRDADC, 0, NTRDADC));
									
									//--adc vs average adc //2023.01.18
									for (int il=0; il<20; il++)
									{
										histn = Form("TRDAdc%dvsAverAdc_r%d_q%d_TrdL%d_PLCor%d", imatch, ir, *it, il, iplcor);
										histn += Form("_evcut%d", ievcut);
										histn += Form("_sum%d", isum);
										hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 100, 0, 6000, NTRDADC, 0, NTRDADC));
									} //il
								} //isum
							} //iplcor
						} //ievcut
					} //imatch
					#endif  //TRDADCOLD
					
					//-- //2023.01.26
					#ifdef MAXADCDIS
					for (int ievcut=0; ievcut<1; ievcut++)
					{
						outfile.mkdir(Form("TRDMaxADC%d/q%d/%s%s/evcut%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ievcut));
						outfile.cd(Form("TRDMaxADC%d/q%d/%s%s/evcut%d", iQVer, *it, sSpan[ir].c_str(), sRig.c_str(), ievcut));
						
						//--adc vs hit no. ordered by amplitude
						for (int ithrcut=0; ithrcut<2; ithrcut++) //ithrcut=0: only take hits above threshold, =1: no cut
						{
							for (int iNLayerCut=0; iNLayerCut<2; iNLayerCut++)
							{
								histn = Form("TRDAdcvsAdcOrder_r%d_q%d", ir, *it);
								histn += Form("_evcut%d", ievcut);
								histn += Form("_ThrCut%d", ithrcut);
								histn += Form("_NLayerCut%d", iNLayerCut);
								//hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 21, -0.5, 20.5, NTRDADC, 0, NTRDADC));
								hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 21, -0.5, 20.5, 8000, 0, 8000));
							} //iNLayerCut
						} //ithrcut
						
						//--no. hit distributiomn
						for (int iseg=0; iseg<5; iseg++)
						{
							histn = Form("NoTRDHit_r%d_q%d", ir, *it);
							histn += Form("_evcut%d", ievcut);
							if (iseg==0) histn += Form("_Bottom"); //Y
							else if (iseg==1) histn += Form("_Middle"); //X
							else if (iseg==2) histn += Form("_Top"); //Y
							else if (iseg==3) histn += Form("_BottomTop"); //All Y
							else if (iseg==4) histn += Form("_BottomMiddleTop"); //Both X and Y
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 40, 0.5, 40.5));
						} //iseg
						
						//--until how many hits that satisfy the pattern requirement
						for (int ipattcut=0; ipattcut<2; ipattcut++) //ipattcut=0: 3X3Y, ipattcut=1: 3X2Y (release the requirement on bottom and top)
						{
							histn = Form("NoHitPassingPatt_r%d_q%d", ir, *it);
							histn += Form("_evcut%d", ievcut);
							histn += Form("_PattCut%d", ipattcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), 40, 0.5, 40.5));
						} //ipattcut
					} //ievcut
					#endif //MAXADCDIS
					
					//--
					#endif //TRDADC
				} //irig
				
				//--
				//--
				outfile.cd(Form("Event_count%d/q%d/%s", iQVer, *it, sSpan[ir].c_str()));
				
				//2020.11.14
				#ifdef LOOSEFV
				histn = Form("rig_r%d_q%d_%.1fsigma_TightFV", ir, *it, 2.0);
				hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				#endif //LOOSEFV
				
				#ifdef QYEVCOUNT
				//histn = Form("rig_r%d_q%d_%.1fsigma_QYEvCount", ir, *it, 2.0);
				//hmanQ[iQVer].Add(new TH1D(histn.c_str(), histn.c_str(), iNbin, pBins));
				
				//2020.11.14
				for (int iQYSel=0; iQYSel<2; iQYSel++)
				{
					histn = Form("rig_r%d_q%d_%.1fsigma_QYEvCount%d", ir, *it, 2.0, iQYSel);
					hmanQ[iQVer].Add(new TH1D(histn.c_str(), histn.c_str(), iNbin, pBins));
				} //iQYSel
				#endif //QYEVCOUNT
				
				//----
				/*#ifdef VARDIS
				#ifdef QSELTWIKI //2019.09.01: only store for Q sel with TWiki
				if (iQVer%3==1 && iQVer<3) continue;
				#endif //QSELTWIKI
				sDirVD = Form("Event_count%d/q%d/%s/VarDis", iQVer, *it, sSpan[ir].c_str());
				ev->BookVarDisInnerTrk(sDirVD.c_str());
				ev->BookVarDisExtTrk(sDirVD.c_str());
				ev->BookVarDisTof(sDirVD.c_str());
				
				//Var Dis vs Rig
				sDirVD = Form("Event_count%d/q%d/%s/VarDisvsRIG", iQVer, *it, sSpan[ir].c_str());
				ev->BookVarDisInnerTrkRIG(sDirVD.c_str(), *it);
				ev->BookVarDisExtTrkRIG(sDirVD.c_str(), *it);
				ev->BookVarDisTofRIG(sDirVD.c_str(), *it);
				#endif //VARDIS*/
				
				#ifdef EXTASYCHECK
				for (int iext=0; iext<2; iext++)
				{
					int il=(iext==0)?1:9;
					
					//------XQ vs YQ
					histn = Form("L%dQ_YvsX_r%d_q%d_qv%d", il, ir, *it, iQVer);
					hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), 350, 0, 35, 350, 0, 35));
					
					//------XQ YQ asymmetry vs Rigidity
					histn = Form("L%dQAsyvsRig_r%d_q%d_qv%d", il, ir, *it, iQVer);
					hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), iNbin, pBins, 1000,0,1));
					
					//------Hit position, without & with Asymmetry cut
					histn = Form("L%dHitPos_r%d_q%d_qv%d_noAsyCut", il, ir, *it, iQVer);
					hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), 100, -100, 100, 100, -100, 100));
					
					histn = Form("L%dHitPos_r%d_q%d_qv%d_withAsyCut", il, ir, *it, iQVer);
					hmanQ[iQVer].Add(new TH2I(histn.c_str(), histn.c_str(), 100, -100, 100, 100, -100, 100));
					
				} //iext
				#endif //EXTASYCHECK
				
				#ifdef CUTOFFSAFETYVAR
				for (int iSafety=0; iSafety<5; iSafety++)
				{
					if (!ev->isreal) break;
					histn = Form("rig_r%d_q%d_CutoffSafety%.1f", ir, *it, 1.+0.1*iSafety);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				} //iSafety
				#endif //CUTOFFSAFETYVAR
				
				/*//under cutoff study
				#ifdef UNDERCUTOFF
				outfile.mkdir(Form("Event_count%d/q%d/%s/UnderCutoff", iQVer, *it, sSpan[ir].c_str()));
				outfile.cd(Form("Event_count%d/q%d/%s/UnderCutoff", iQVer, *it, sSpan[ir].c_str()));
				
				histn = Form("rig_r%d_q%d_NoCutoff", ir, *it); //no cutoff
				hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
				GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("Nevent");
				
				histn = Form("thetamvsrig_r%d_q%d_NoCutoff", ir, *it);
				hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 320, -1.6, 1.6));
				GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
				GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("thetam");
				
				histn = Form("thetamvsrig_r%d_q%d_AboveCutoff", ir, *it);
				hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 320, -1.6, 1.6));
				GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
				GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("thetam");
				
				for (int itmr=0; itmr<4; itmr++)
				{
					histn = Form("rig_r%d_q%d_NoCutoff_thetamRegion%d", ir, *it, itmr); //no cutoff
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
					GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("Nevent");
					
					histn = Form("rig_r%d_q%d_AboveCutoff_thetamRegion%d", ir, *it, itmr); //above cutoff
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					GetHist(hmanQ[iQVer], histn.c_str())->GetXaxis()->SetTitle("Rigidity [GV]");
					GetHist(hmanQ[iQVer], histn.c_str())->GetYaxis()->SetTitle("Nevent");
				} //itmr
				#endif //UNDERCUTOFF*/
			} //ir
		} //it
		
		//--TRD adc, without separating charge //2023.01.26
		#ifdef TRDADC
		#ifdef TRDADCNEW
		for (int ir=iSpanL; ir<iSpanU; ir++)
		{
			//2023.03.10
			int irig=0;
			string sRig=sRigName[irig];
			
			for (int ievcut=0; ievcut<1; ievcut++)
			{
				outfile.mkdir(Form("TRD_ADC_New%d/%s%s/evcut%d", iQVer, sSpan[ir].c_str(), sRig.c_str(), ievcut));
				outfile.cd(Form("TRD_ADC_New%d/%s%s/evcut%d", iQVer, sSpan[ir].c_str(), sRig.c_str(), ievcut));
				
				//--adc vs InnerQ
				for (int iver=0; iver<2; iver++) //iver=0: old charge, iver=1: new charge (not avaliable for Z=1)
				{
					histn = Form("TRDAdcvsInnerQ_r%d", ir);
					histn += Form("_evcut%d", ievcut);
					histn += Form("_ChargeVer%d", iver);
					histn += sRig; //2023.03.10
					hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 500, 0, 10, NTRDADC, 0, NTRDADC));
				} //iver
				
				//--adc vs average adc
				for (int isum=0; isum<NTRDADCSum; isum++) //isum=0: average, =1: average without min, =2: average without max, =3: average without both min and max
				{
					histn = Form("TRDAdcvsAverAdc_r%d", ir);
					histn += Form("_evcut%d", ievcut);
					histn += Form("_sum%d", isum);
					histn += sRig; //2023.03.10
					hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), 100, 0, 6000, NTRDADC, 0, NTRDADC));
				} //isum
			} //ievcut
		} //ir
		#endif //TRDADCNEW
		#endif //TRDADC
		
		//2023.02.28
		#ifdef ISOTOPE
		//--Beta vs Rig
		#ifdef ISOTOPEEV
		BookHistoIsotopeEv(hmanQ[iQVer], *ev, outfile, iQVer, true);
		#endif //ISOTOPEEV
		
		//--Mass distribution
		#ifdef MASSDIS
		BookHistoMassDis(hmanQ[iQVer], *ev, outfile, iQVer, true);
		#endif //MASSDIS
		#endif //ISOTOPE
	} //iQVer
				
	//--chis check for GBL //2022.09.22
	#ifdef GBLCHISCHECK
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		BookHistoGBLChisCheck(hmanQ[iQVer], *ev, outfile, iQVer, true);
	} //iQVer
	cout << "Finish booking for GBLChisCheck" << endl;
	#endif //GBLCHISCHECK
	
	//rig vs estimated rig
	#ifdef RIGESTCHECK
	//--loading rigidity estimation calibration file
	string sRigEstCal = Form("%s/analisi/calib", sWDirOnline.c_str());
	sRigEstCal = Form("%s/amsd%dn/RigEstCal", sRigEstCal.c_str(), dstver);
	
	cout << "Rigidity estimation: " << sRigEstCal << endl;

	//BuildRigEstv2(sRigEstCal); //different rigidity calibration for different charge calibration
	BuildRigEstv(sRigEstCal, viQSel); //new function, now only read the charge in the list //2023.03.26
	#endif //RIGESTCHECK
	
	//--
	cout << "Finish booking histogram for event count, new tracker charge." << endl;
	#endif //RIG_NEWTRQ
	
	#ifdef ACCEPTANCE
	//2023.02.16
	//--set even log bin
	EvenLogBin(ev->mch);
	
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		BookHistoAcc(hmanQ[iQVer], *ev, outfile, iQVer, true);
		#ifdef TOICORRECTION
		BookHistoTOI(hmanQ[iQVer], *ev, outfile, iQVer, true);
		#endif //TOICORRECTION
		//2023.02.17
		#ifdef BELOWL1MC
		BookHistoBelowL1MC(hmanQ[iQVer], *ev, outfile, iQVer, true);
		#endif //BELOWL1MC
	} //iQVer
	cout << "Finish booking histogram for acceptance." << endl;
	#endif //ACCEPTANCE
	
	#ifdef CHARGECAL_NEWTRQ
	cout << "Start to book histogram for Charge Calibration using new tracker charge." << endl;
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		//BookChargeCal(hmanQ[iQVer], ev->isreal, outfile, iQVer, true, ev->mch);
		BookChargeCal(hmanQ[iQVer], *ev, outfile, iQVer, true); //2023.04.05
	}
	//2022.05.28
	cout << "Binning for template fit: Nbin=" << iNBL1TF << endl;
	for (int ibin=0; ibin<iNBL1TF+1; ibin++)
	{
		cout << dBL1TF[ibin] << " ";
	} //ibin
	cout << endl;
	
	cout << "Finish booking histogram for Charge Calibration using new tracker charge." << endl;
	#endif //CHARGECAL_NEWTRQ
	
	/*//decide charge calibration file according to ProdVer
	string sQCalfVer="QDisN8_RoughAll5";
	
	string sQCal = Form("%s/analisi/QCalib/amsd%dn/%s", sWDirOnline.c_str(), dstver, sQCalfVer.c_str());
	
	ReadMeanSigma(sQCal);*/
	
	//2023.04.23
	#ifdef L1QDISSEL
	ev->AddBranch_Trd(ch);
	
	cout << "Start to book histogram for L1Q Dis selection study." << endl;
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		BookL1QDisSel(hmanQ[iQVer], *ev, outfile, iQVer, true);
	} //iQVer
	cout << "Finish book histogram for L1Q Dis selection study." << endl;
	#endif //L1QDISSEL
	
	//--
	#ifdef TRRES
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		BookHistoTrRes(hmanQ[iQVer], *ev, outfile, iQVer, true);
	} //iQVer
	cout << "Finish booking histogram for Tracker Resolution." << endl;
	#endif //TRRES
	
	#ifdef ESTCAL
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		//BookRigEst(outfile, iQVer, true);
		BookRigEst(*ev, outfile, iQVer, true); //2023.03.10
	} //iQVer
	cout << "Finish booking histogram for Rigidity Estimator Calibration." << endl;
	#endif //ESTCAL
	
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		
		#ifdef TRIGEFF
		BookHistoTrigEff(hmanQ[iQVer], *ev, outfile, iQVer, true);
		cout << "Finish booking histogram for Trigger Efficiency, QVer=" << iQVer << "." << endl;
		#endif //TRIGEFF
		
		#ifdef TOFQEFF
		BookHistoTofQEff(hmanQ[iQVer], *ev, outfile, iQVer, true);
		cout << "Finish booking histogram for Tof Q Efficiency, QVer=" << iQVer << "." << endl;
		#endif //TOFQEFF
	
		#ifdef L1EFF
		//BookHistoL1Eff(hmanQ[iQVer], *ev, outfile, iQVer, true);
		//BookHistoL1Eff(hmanQ[iQVer], *ev, outfile, iQVer, true, 2); //2022.10.03: store GBL efficiency
		BookHistoL1Eff(hmanQ[iQVer], *ev, outfile, iQVer, true); //2023.03.11: no need to give NRig as parameter
		//2022.10.16 //consider put sName into GetHist //XXX
		/*#ifdef GBLCHISCHECKL1Eff
		for (int icut=0; icut<3; icut++)
		{
			BookHistoL1Eff(hmanQ[iQVer], *ev, outfile, iQVer, true, 2, icut);
		} //icut
		#endif //GBLCHISCHECKL1Eff*/
		cout << "Finish booking histogram for L1 Efficiency (BZ & pick up), QVer=" << iQVer << "." << endl;
		#endif //L1EFF
	
		#ifdef L9EFF
		BookHistoL9Eff(hmanQ[iQVer], *ev, outfile, iQVer, true);
		cout << "Finish booking histogram for L9 Efficiency, QVer=" << iQVer << "." << endl;
		#endif //L9EFF
		
		#ifdef TRACKEFFN
		BookHistoTkEffN(hmanQ[iQVer], *ev, outfile, iQVer, true);
		//BookHistoTkEffN(hmanQ[iQVer], *ev, outfile, 0, iQVer, true); //2023.02.20
		//set the rigidity estimation here
		//--loading rigidity estimation calibration file
		string sRigEstCal = Form("%s/analisi/calib", sWDirOnline.c_str());
		
		if (dstver==60) sRigEstCal = sRigEstCal+"/amsd60n/RigEstCal5";
		//else if (dstver>=63) sRigEstCal = Form("%s/amsd%dn/RigEstCal", sRigEstCal.c_str(), dstver);
		else if (dstver>=63)
		{
			//sRigEstCal = Form("%s/amsd%dn/RigEstCal", sRigEstCal.c_str(), dstver);
			//sRigEstCal = Form("%s/amsd%dn/RigEstiCal_10yr", sRigEstCal.c_str(), dstver%100);
			//2023.02.19
			//#if defined PHOSPHORUS || defined CHLORINE || defined POTASSIUM || defined NICKEL //2023.02.19
			//sRigEstCal = Form("%s/amsd%dn/RigEstiCal_10yr", sRigEstCal.c_str(), dstver%100);
			//#endif //PHOSPHORUS || CHLORINE || POTASSIUM || NICKEL
			//2023.04.15
			int iRigEstDstVer = dstver%100;
			if (iRigEstDstVer<68 && !ev->isreal) iRigEstDstVer = 64;
			sRigEstCal = Form("%s/amsd%dn/RigEstiCal_10yr", sRigEstCal.c_str(), iRigEstDstVer);
		}
		else  sRigEstCal = sRigEstCal+"/amsd60n/RigEstCal5"; //2019.10.16: for now use the result for amsd60n for all newer dst, will update later
		
		cout << "Rigidity estimation: " << sRigEstCal << endl;
		
		//BuildRigEstv2(sRigEstCal); //different rigidity calibration for different charge calibration
		BuildRigEst(sRigEstCal, viQSel); //new function, now only read the charge in the list //2023.03.26
		
		cout << "Finish booking histogram for Tracking Efficiency (new), QVer=" << iQVer << "." << endl;
		
		//InnerQ cut eff
		BookHistoTkQEff(hmanQ[iQVer], *ev, outfile, iQVer, true);
		//BookHistoTkQEff(hmanQ[iQVer], *ev, outfile, 0, iQVer, true); //2023.02.19
		#endif //TRACKEFFN
	} //iQVer
	//cout << " ****** Finish booking Efficiency histogram ****** " << endl;
	
	#ifdef EFFVAL
	BookHistoEffVal(hman1, ev->isreal, SCharge, MCharge, outfile, true);
	cout << "Finish booking histogram for Tracking Efficiency sample validation." << endl;
	#endif //EFFVAL
	
	#ifdef RIGRESO
	cout << "Start booking histogram for Rigidity resolution." << endl;
	//do rigidity resolution for different charge version
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		BookRigReso(hmanQ[iQVer], *ev, outfile, ev->mch, iQVer, true);
	} //iQVer
	
	//2021.09.17: move outside, to be used without requiring RIGRESO
	//#ifdef MASSKEYSPDF
	//string sBetaCor = Form("%s/analisi/calib/amsd%dn/BetaResoCal", sWDirOnline.c_str(), dstver);
	//if (dstver<=64) sBetaCor = Form("%s/analisi/calib/amsd%dn/BetaResoCal", sWDirOnline.c_str(), 64);
	//ReadBetaCor(sBetaCor);
	//#endif // MASSKEYSPDF
	cout << "Finish booking histogram for Rigidity resolution." << endl;
	#endif //RIGRESO
	
	#ifdef MASSKEYSPDF
	//string sBetaCor = Form("%s/analisi/calib/amsd%dn/BetaResoCal", sWDirOnline.c_str(), dstver);
	//if (dstver<=64) sBetaCor = Form("%s/analisi/calib/amsd%dn/BetaResoCal", sWDirOnline.c_str(), 64);
	//2021.10.25
	int iBetaCorDstVer = dstver;
	if (dstver>100) iBetaCorDstVer = dstver%100;
	string sBetaCor = Form("%s/analisi/calib/amsd%dn/BetaResoCal", sWDirOnline.c_str(), iBetaCorDstVer);
	ReadBetaCor(sBetaCor);
	#endif // MASSKEYSPDF
	
	//2019.10.14
	#ifdef L1L2SUR
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		outfile.mkdir(Form("L1L2Sur%d", iQVer));
		
		for (it=viQSel.begin(); it!=viQSel.end(); it++)
		{
			//if (!ev->isreal && *it!=ev->mch) continue;
			//2020.10.25
			#ifndef EFFSEC
			if (!ev->isreal && *it!=ev->mch) continue;
			#endif //EFFSEC
			
			iNbin = getNbin(*it);
			pBins = getBins(*it);
			outfile.mkdir(Form("L1L2Sur%d/q%d", iQVer, *it));
			for (int ir=1; ir<2; ir++)
			{
				outfile.mkdir(Form("L1L2Sur%d/q%d/%s", iQVer, *it, sSpan[ir].c_str()));
				outfile.cd(Form("L1L2Sur%d/q%d/%s", iQVer, *it, sSpan[ir].c_str()));
				
				//--separate the result into different trigger period
				for (int iperiod=0; iperiod<2; iperiod++)
				{
					outfile.mkdir(Form("L1L2Sur%d/q%d/%s/period%d", iQVer, *it, sSpan[ir].c_str(), iperiod));
					outfile.cd(Form("L1L2Sur%d/q%d/%s/period%d", iQVer, *it, sSpan[ir].c_str(), iperiod));
					
					//store the distribution with different UTofQ upper cut to eliminate heavier nuclei background
					for (int itofqcut=0; itofqcut<4; itofqcut++)
					{
						outfile.mkdir(Form("L1L2Sur%d/q%d/%s/period%d/tofqcut%d", iQVer, *it, sSpan[ir].c_str(), iperiod, itofqcut));
						outfile.cd(Form("L1L2Sur%d/q%d/%s/period%d/tofqcut%d", iQVer, *it, sSpan[ir].c_str(), iperiod, itofqcut));
						
						//------distribution without L1Q cut
						//----event count after non-charge and L1Q selection
						histn = Form("rig_r%d_q%d_L1L2Sur_noL1Qcut_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
						//----L1Q distribution
						histn = Form("L1QvsRig_r%d_q%d_L1L2Sur_noL1Qcut_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
					
						//----UTofQ distribution
						histn = Form("UTofQvsRig_r%d_q%d_L1L2Sur_noL1Qcut_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
					
						//----also store in Generated Rigidity for MC
						if (!ev->isreal)
						{
							//----event count after non-charge and L1Q selection
							histn = Form("rig_r%d_q%d_L1L2Sur_noL1Qcut_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
			
							//----InnerQ distribution
							histn = Form("L1QvsRig_r%d_q%d_L1L2Sur_noL1Qcut_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
						}
				
						//------after L1Q selection
						/*//----event count after L1Q selection
						histn = Form("rig_r%d_q%d_L1L2Sur_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
						//----InnerQ distribution
						histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
					
						//----UTofQ distribution
						histn = Form("UTofQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
					
						//----also store in Generated Rigidity for MC
						if (!ev->isreal)
						{
							//----event count after non-charge and L1Q selection
							histn = Form("rig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
							//----InnerQ distribution
							histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
						}*/
						
						//2020.11.02: store the distribution with tighten L1Q cut
						const int iNTighten=(itofqcut==0)?3:0;
						for (int isel=-1; isel<iNTighten; isel++)
						{
							if (itofqcut>0 && isel>=0) continue;
							
							string sTighten=(isel==-1)?"":Form("_tighten%d", isel);
							
							//----event count after and L1Q selection
							histn = Form("rig_r%d_q%d_L1L2Sur_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
							hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
							//----InnerQ distribution
							histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
						
							//----UTofQ distribution
							histn = Form("UTofQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
							hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
						
							//----also store in Generated Rigidity for MC
							if (!ev->isreal)
							{
								//----event count after non-charge and L1Q selection
								histn = Form("rig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
								hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
								//----InnerQ distribution
								histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
								hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, i1DBin, iQDL, iQDU));
							}
						} //isel
					} //iperiod
				} //itofqcut
			} //ir
		} //it
	} //iQVer
	cout << "Finish booking histogram for L1->L2 Survival Probability." << endl;
	#endif //L1L2SUR
	
	#ifdef L8L9SUR
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		outfile.mkdir(Form("L8L9Sur%d", iQVer));
		
		for (it=viQSel.begin(); it!=viQSel.end(); it++)
		{
			//if (!ev->isreal && *it!=ev->mch) continue;
			//2020.10.25
			#ifndef EFFSEC
			if (!ev->isreal && *it!=ev->mch) continue;
			#endif //EFFSEC
			
			iNbin = getNbin(*it);
			pBins = getBins(*it);
			
			#ifdef L8L9SURECALMIP
			//------L8->L9 survival probability
			outfile.mkdir(Form("L8L9Sur%d/q%d", iQVer, *it));
			for (int ir=2; ir<3; ir++)
			{
				outfile.mkdir(Form("L8L9Sur%d/q%d/%s", iQVer, *it, sSpan[ir].c_str()));
				
				for (int iperiod=0; iperiod<3; iperiod++) //iperiod=2: sum of two trigger periods
				{
					outfile.mkdir(Form("L8L9Sur%d/q%d/%s/period%d", iQVer, *it, sSpan[ir].c_str(), iperiod));
					outfile.cd(Form("L8L9Sur%d/q%d/%s/period%d", iQVer, *it, sSpan[ir].c_str(), iperiod));
					
					//------L1Inner no MIP sample (survival prob * L9 efficieny, use for survival prob)
					//--sample: L1Inner + L9 fiducial + Ecal fiducial
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8Sam", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8Sam_L9XY", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8Sam_L9XY", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
				
					//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8Sam_L9Sel", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					//----requring ECal max edep cell not in edge //2020.02.19
					//--sample: L1Inner + L9 fiducial + Ecal fiducial
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut_L9XY", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut_L9XY", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
				
					//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut_L9Sel", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					//------L1Inner MIP sample (purity * L9 efficiency, use for L9 efficiency)
					//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5)
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSam", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSam_L9Sel", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					//------L1Inner MIP sample with L9 selection (use for MIP sample purity)
					for (int isam=0; isam<2; isam++) //isam=0: MIP+unbiasd L9XY, isam=1: MIP+unbiasd L9XY+unbiased L9Q status
					{
						//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5) + unbiased L9hit preselection (different for isam=0 and =1)
						histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%d", ir, *it, iperiod, isam);
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%d", ir, *it, iperiod, isam);
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
					
						//--selection: sample + unbiased L9Q>Z-1.5
						histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%d_L9Sel", ir, *it, iperiod, isam);
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					} //isam
				
					//----not requring ECal max edep cell not in edge //2020.02.19
					//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5)
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamNoMaxEdepCut", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
					histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamNoMaxEdepCut_L9Sel", ir, *it, iperiod);
					hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
				
					//------L1Inner MIP sample with L9 selection (use for MIP sample purity)
					for (int isam=0; isam<2; isam++) //isam=0: MIP+unbiasd L9XY, isam=1: MIP+unbiasd L9XY+unbiased L9Q status
					{
						//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5) + unbiased L9hit preselection (different for isam=0 and =1)
						histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%dNoMaxEdepCut", ir, *it, iperiod, isam);
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					
						histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%dNoMaxEdepCut", ir, *it, iperiod, isam);
						hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
					
						//--selection: sample + unbiased L9Q>Z-1.5
						histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%dNoMaxEdepCut_L9Sel", ir, *it, iperiod, isam);
						hmanQ[iQVer].Add(new TH1F(histn.c_str(), histn.c_str(), iNbin, pBins));
					} //isam
				} //iperiod
			} //ir
			#endif //L8L9SURECALMIP
			
			//------L1Q and L2Q distribution for L9Q>Z-1.5 efficiency //2020.02.20
			for (int ir=1; ir<2; ir++)
			{
				for (int il=0; il<3; il++)
				{
					string sLQ;
					if (il==0) sLQ = "unbiasedL1Q";
					else sLQ = Form("L%dQ", il);
					outfile.mkdir(Form("L8L9Sur%d/q%d/%s/%s", iQVer, *it, sSpan[ir].c_str(), sLQ.c_str()));
					outfile.cd(Form("L8L9Sur%d/q%d/%s/%s", iQVer, *it, sSpan[ir].c_str(), sLQ.c_str()));
					
					//--LQ without charge status cut
					histn = Form("%svsRig_r%d_q%d_L8L9Sur_noQStatCut", sLQ.c_str(), ir, *it);
					hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
					
					//--LQ with charge status cut
					histn = Form("%svsRig_r%d_q%d_L8L9Sur_withQStatCut", sLQ.c_str(), ir, *it);
					hmanQ[iQVer].Add(new TH2F(histn.c_str(), histn.c_str(), iNbin, pBins, 3500, 0, 35));
				} //il
			} //ir
			
		} //it
	} //iQVer
	cout << "Finish booking histogram for L8->L9 Survival Probability." << endl;
	#endif //L8L9SUR
	
	//--apply RigDiffCut and check the change of rigidity resolution //2023.02.04
	#ifdef RIGDIFFCUT
	cout << "Start booking histogram for RigReso after RigDiff Cut." << endl;
	for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
	{
		if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
		BookRigDiffCut(hmanQ[iQVer], ev->isreal, outfile, ev->mch, iQVer, true);
	} //iQVer
	#endif //RIGDIFFCUT
	
	cout << " ****** Finish booking histogram ****** " << endl;
	
	//////
	long int nentries = ch.GetEntries();
	unsigned int RunT[2] = {0};
	
	//define bool variables //move declaration into SelEvent14.h, not as member of class SelEvent
	/*bool bTrig=false, bRun=false, bRTI=false, bRTIdL1L9=false, bPart=false, bBeta=false, bChis=false, bTk2nd=false;
	bool bRig[3], bTrack[3], bTkgeom[3];
	bool bRigEff[3];
	bool bCharge[3];*/
	
	//--
	int nrun2 = nentries;
	if (nrun != -1 && nrun<nrun2) nrun2 = nrun;
	int nrun3 = 0; //no. of runs we really runned
	
	int iLargeQ=0;
	
	int iMaxEvent=0, iMinEvent=1000000;
	
	#ifdef FIVEYEARS
	cout << "Only store 5 years result for ISS: [1305853512, 1456503197)" << endl;
	#endif //FIVEYEARS
	
	#ifdef TENYEARSEXT
	cout << "Only store extension to 10 years result for ISS: [1580122824, 1620024849]" << endl;
	#endif //TENYEARSEXT
	
	#ifdef EXTELEVENYEAR
	//cout << "Only store extension from 10 to 11.5 years result for ISS: (1620024849, 1635855691]" << endl;
	cout << "Only store extension from 10 to 11.5 years result for ISS: (1620024849, 1668126894]" << endl;
	#endif //TENYEARSEXT
	
	#ifdef FORCENEWTRIG
	//cout << "Only store new Trigger periods result for ISS: [1456503197, 1464298202]" << endl;
	cout << "Only store new Trigger periods result for ISS: [1456503197, 10.29.2019]" << endl;
	ev->iTrigSetting=2;
	//2020.12.09
	#elif defined FORCEOLDTRIG
	cout << "Only store old Trigger periods result for ISS: [1305853512, 1456503197)" << endl;
	ev->iTrigSetting=1;
	#endif //FORCENEWTRIG || FORCEOLDTRIG
	
	//2020.12.12
	#ifdef FORCENEWTRIGBR
	cout << "Only store new Trigger + Bartel rotation periods result for ISS: (1457048996, 10.29.2019]" << endl;
	ev->iTrigSetting=2;
	//2020.12.09
	#elif defined FORCEOLDTRIGBR
	cout << "Only store old Trigger + Bartel rotation periods result for ISS: [1305853512, 1457048996]" << endl;
	ev->iTrigSetting=1;
	#endif //FORCENEWTRIGBR || FORCEOLDTRIGBR
	
	//limit to 10 years result //2023.03.09
	#ifdef UPTOTENYEAR
	cout << "Only store up to 10yr: <=1620024849" << endl;
	#endif //UPTOTENYEAR
	
	//always set new trigger for Iron MC
	if (!ev->isreal && ev->mch==26)
	{
		cout << "Force new Trigger setting for Iron MC" << endl;
		ev->iTrigSetting=2;
	}
	
	//2020.11.14
	#ifdef LOOSEFV
	iFVTight=0;
	#endif //LOOSEFV
	
	//2020.12.31
	#ifdef KALMANRIG
	cout << " ****** Using Kalman rigidity" << endl;
	RIGALGO=2;
	#endif //KALMANRIG
	cout << " ****** Rigidity algorithm=" << RIGALGO << " " << " alignment=" << RIGPGC << " ****** " << endl;
	
	bool bChainChanged = false;
	
	for (int ievent=0; ievent<nrun2; ievent++)
	// (int ievent=2500000; ievent<nrun2; ievent++)
	{
		ch.GetEntry(ievent);
		if (ev->isreal && (ev->run<1305853512)) continue;
		
		#ifdef SEVENYEARSADD
		if (ev->isreal && (ev->run<=1464298202)) continue;
		#endif //SEVENYEARSADD
		
		#ifdef FIVEYEARS
		//if (ev->isreal && ((ev->run<1305853512) || (ev->run>1456503197))) continue;
		if (ev->isreal && ((ev->run<1305853512) || (ev->run>=1456503197))) continue; //2020.09.07
		#endif //FIVEYEARS
		
		#ifdef SEVENYEARFLUX
		if (ev->isreal && ((ev->run<1305853512) || (ev->run>1527490046))) continue;
		#endif //SEVENYEARFLUX
		
		#ifdef TENYEARSEXT
		if (ev->isreal && ((ev->run<1580122824) || (ev->run>1620024849))) continue;
		#endif //TENYEARSEXT
		
		#ifdef UPTOTENYEAR
		if (ev->isreal && (ev->run>1620024849)) continue;
		#endif //UPTOTENYEAR
		
		#ifdef EXTELEVENYEAR
		if (ev->isreal && (ev->run<=1620024849)) continue;
		//if (ev->isreal && (ev->run>1635855691)) continue; //10.5yr
		if (ev->isreal && (ev->run>1668126894)) continue;
		#endif //TENYEARSEXT
		
		/*#ifdef FORCENEWTRIG
		if (ev->isreal && (ev->TriggerPeriod()==0)) continue;
		//2020.12.09
		#elif defined FORCEOLDTRIG
		if (ev->isreal && (ev->TriggerPeriod()==1)) continue;
		#endif //FORCENEWTRIG || FORCEOLDTRIG*/
		//2020.12.12
		#if defined FORCENEWTRIG || defined FORCENEWTRIGBR
		if (ev->isreal && (ev->TriggerPeriod()==0)) continue;
		//2020.12.09
		#elif defined FORCEOLDTRIG || defined FORCEOLDTRIGBR
		if (ev->isreal && (ev->TriggerPeriod()==1)) continue;
		#endif //(FORCENEWTRIG || FORCENEWTRIGBR) || (FORCEOLDTRIG || FORCEOLDTRIGBR)
		
		if (ievent%100000==0)
		{
			check = time(NULL);
			cout << "no. of events finished:" << ievent << ", left events:" << nrun2 - ievent;
			cout << " (=" << (double)(nrun2 - ievent)/nrun2*100. << "%)" << endl;
			cout << check - start << "s used, " << ctime(&check);
			
			
			sFileDir = ch.GetFile()->GetPath();
			if (ievent>0)
			{
				cout << "Estimated remaining times: " << (double)((check - start)*(nrun2 - ievent)/ievent) << endl;
				//2020.12.12
				cout << "current run = " << ev->run << " trigger period = " << ev->TriggerPeriod() << endl;
				cout << "Current file: " << sFileDir << endl;
			}
		}
			
		//-- //to speed up in SDIAT, do not check ch.GetEntries, which is time consuming especially running in SDIAT with very large chain
		if (ievent%100000==0 || ievent==nrun2-1) //check for every 100000 events or the last event
		{
			#if defined LXPLUS || defined MLXPLUS
			Long64_t nrun_test = ch.GetEntries();
			if ((nrun_test!=nentries) || ievent>=nrun_test)
			{
				cout << " ****** chain changed: ievent=" << ievent << " nrun_test=" << nrun_test << " total entry=" << nentries << endl; //XXX
				bChainChanged = true;
			}
			#endif //LXPLUS || MLXPLUS
		}
		
		//--MCEvent TotalEvent //run+event
		if (!ev->isreal)
		{
			int pos=1;//Middel
		 	if(ievent==nrun2-1)pos=0;//End Fill MC
		 	
			static int prrun=-1,prev=-1;
			static int prcev=0,ccev;
			if(ev->event<prev||ev->run!=prrun||pos==0)
			{
				//cout<<"prrun="<<prrun<<" prev="<<prev<<" nrun="<<ev->run<<" nev="<<ev->event;
				if(prev!=-1)
				{
					ccev=(prev-prcev+1);//Total
					if(prcev<100)ccev=prev;//old
					//cout<<" cev="<<ccev;
					GetHist(hman1, Form("mcev_NGen%d", MCVer))->Fill(0.,double(ccev));
				}
				//cout<<endl;
				prcev=ev->event;
				//cout << "ievent " << ievent << endl;
			}
			
			//--maximum event in each run
			if (ev->run!=prrun||pos==0)
			{
				//cout<<"prrun="<<prrun<<" min event=" << iMinEvent << " max event="<<iMaxEvent<<" nrun="<<ev->run<<" nev="<<ev->event<<endl<<endl;
				if (iMaxEvent!=0)
				{
					GetHist(hman1, Form("mcev_NGen%d", MCVer))->Fill(1.,double(iMaxEvent));
					if (iMinEvent!=1000000)
					{
						GetHist(hman1, Form("mcev_MinimumEvent%d", MCVer))->Fill(iMinEvent);
						GetHist(hman1, Form("mcev_NGen%d", MCVer))->Fill(2.,double(iMaxEvent-iMinEvent+1));
						if (iMinEvent<=100) GetHist(hman1, Form("mcev_NGen%d", MCVer))->Fill(3.,double(iMaxEvent));
						else if (iMinEvent>100) GetHist(hman1, Form("mcev_NGen%d", MCVer))->Fill(3.,double(iMaxEvent-iMinEvent+1));
						iMinEvent=1000000;
					}
					iMaxEvent=0;
				}
			}
			prrun=ev->run;prev=ev->event;//Latest
			if (ev->event>=iMaxEvent) iMaxEvent = ev->event;
			if (ev->event<=iMinEvent) iMinEvent = ev->event;
			
			
		}
		//------Weight factor
		ww[0] = ww[1] = ww[2] = 1.;
		ww0[0]=ww0[1]=ww0[2]=1.;
		ww2[0]=ww2[1]=ww2[2]=1.;
		
		dExtRig=0., dExtww0=1.; //declared in Acceptance.h //2023.02.16 
		//ww = 1.;
		
		#ifdef MCREWEIGHT
		if (!ev->isreal)
		{
			int   tk_zm=ev->mch;
   			double mrig=ev->mmom/tk_zm;
			
			#ifdef UNFOLDINGCHECK
			ww0[0]=f1Flux[0]->Eval(mrig)/pow(mrig,-1)*dNorm[0];
			ww0[1]=f1Flux[1]->Eval(mrig)/pow(mrig,-1)*dNorm[1];
			ww0[2]=f1Flux[2]->Eval(mrig)/pow(mrig,-1)*dNorm[2];
			#else
			if (iFAiter==0) ww0[0]=ww0[1]=ww0[2]=1*dNorm[0];
			else
			{
				ww0[0]=f1Flux[0]->Eval(mrig)/pow(mrig,-1)*dNorm[0];
				ww0[1]=f1Flux[1]->Eval(mrig)/pow(mrig,-1)*dNorm[1];
				ww0[2]=f1Flux[2]->Eval(mrig)/pow(mrig,-1)*dNorm[2];
			}
			#endif
			
			if (MCVer==2) ww0[0]=ww0[1]=0;
			
			//------l1&l19 combine
			//ww2[0]=ww2[1]=ww2[2]=1;
			ww2[0]=ww2[1]=ww2[2]=dwn[0]; //2021.02.05: remove dependence on total generated events completely
			#ifndef NOMCCOMBINE //2020.07.16
			if (MCVer==2) ww2[0]=ww2[1]=0;
			if (mrig>=1. && mrig<2.) ww2[2] = dwn[0];
			else if (mrig>=2. && mrig<=2000.) ww2[2] = dwn[1];
			else if (mrig>2000. && mrig<=8000.) ww2[2] = dwn[2];
			#endif //NOMCCOMBINE
			
			//------l1&l19 combine + shape
			//for (int ir=0; ir<3; ir++) ww[ir]=ww0[ir]*ww2[ir];
			//for (int ir=0; ir<3; ir++) ww[ir]=ww0[ir]*ww2[ir]*dIsotopeMixture;
			//2021.02.05
			for (int ir=0; ir<3; ir++)
			{
				//ww[ir]=ww0[ir]*ww2[ir]*dIsotopeMixture;
				//cout << ir << " " << ww0[ir] << " " << ww2[ir] << " " << dIsotopeMixture << endl; //XXX
				ww[ir]=ww0[ir]*ww2[ir]/dNorm[ir]*dIsotopeMixture; //2021.02.10
				ww0[ir]=ww0[ir]*dIsotopeMixture;
				ww2[ir]=ww2[ir]*dIsotopeMixture;
			} //ir
			
  			histn = Form("rig_generated");
			GetHist(hman1, histn.c_str())->Fill(mrig);
			
			histn = Form("rig/rR_generated");
			GetHist(hman1, histn.c_str())->Fill(mrig, mrig);
			
			#ifdef NOREWEIGHT
			ww[0] = ww[1] = ww[2] = 1.;
			if (MCVer==2) ww[0]=ww[1]=0;
			#endif //NOREWEIGHT
			
			for (int ir=0; ir<3; ir++)
			{
				if (ww[ir]==0) continue;
				histn = Form("rig_generated_reweighted_r%d", ir);
				GetHist(hman1, histn.c_str())->Fill(mrig, ww[ir]);
			} //ir
			
			//extend l1
			if (MCVer==1 && (mrig>=1000. && mrig<=2000.))
			{
				dExtRig = f1GenR->GetRandom(2000, getBins(ev->mch,1)[getNbin(ev->mch,1)]);
				#ifdef UNFOLDINGCHECK
				dExtww0=f1Flux[1]->Eval(dExtRig)/pow(dExtRig,-1)*dNorm[1];
				#else
				if (iFAiter==0) dExtww0=1;
				else dExtww0=f1Flux[1]->Eval(dExtRig)/pow(dExtRig,-1)*dNorm[1];
				#endif //UNFOLDINGCHECK
			}
			
			//--MC events outside the production range //2023.05.10
			for (int ilu=0; ilu<2; ilu++)
			{
				histn = (ilu==0)?"Under":"Over";
				histn = Form("mcev_%sflowEvents_MCVer%d", histn.c_str(), MCVer);
				GetHist(hman1, histn.c_str())->Fill(mrig);
			} //ilu
		}
		
		#endif //MCREWEIGHT
		
		#if defined EXPOTCAL && not defined AFSRTI
		//--expo-time & MRC
		// if (ev->isreal)
		// {
		// 	RunT[1] = ev->time[0];
		// 	if (RunT[1] > RunT[0]) //store expo-time & MRC for later time
		// 	{
		// 		if (ev->Select_Run() && ev->Select_RTI())
		// 		{
		// 			for (it=viQSel.begin(); it!=viQSel.end(); it++)
		// 			{
		// 				for (int id=0; id<4; id++)
		// 				{
		// 					double nt=ev->rtilf*ev->rtinev/(ev->rtinev+ev->rtinerr);
		// 					double ucutr=1.2*ev->mcutoffi[id][1];//IGRF Cutoff  Recommended
						
		// 					//exposure time
		// 					histn = Form("expot_q%d_d%d", *it, id);
		// 					TH1F *h1expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
		// 					//--Above Cutoff LowBin>=Cutof T+ 
		// 					for(int ibr=1;ibr<=h1expot->GetNbinsX();ibr++) //Above CutOff Time++
		// 					{
		// 						if(h1expot->GetBinLowEdge(ibr)>=ucutr){
		// 							h1expot->AddBinContent(ibr,nt);
		// 						}
		// 					}
		// 					h1expot->SetEntries(h1expot->GetEntries()+1);
					
			
		// 					//mcut
		// 					histn = Form("mcut_q%d_d%d", *it, id);
		// 					GetHist(hman1, histn.c_str())->Fill(ev->mcutoffi[id][1]);
					
		// 				} //id
		// 			} //it
		// 		}
		// 		RunT[0] = RunT[1];
		// 	}
		// }
		#endif //EXPOTCAL && !AFSRTI

		#if defined EXPOTIME
		if (ev->isreal)
		{
			RunT[1] = ev->time[0];
			if (RunT[1] > RunT[0]) //store expo-time for later time
			{
				if (ev->Select_Run() && ev->Select_RTI())
				{
					for (it=viQSel.begin(); it!=viQSel.end(); it++)
					{
						for (int id=0; id<4; id++)
						{
							double nt=ev->rtilf*ev->rtinev/(ev->rtinev+ev->rtinerr);
							double ucutr= dGeoSafety*ev->mcutoffi[id][1];//IGRF Cutoff  Recommended
						
							//exposure time
							histn = Form("expotime_q%d_d%d", *it, id);
							TH1F *h1expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
							//--Above Cutoff LowBin>=Cutof T+ 
							for(int ibr=1;ibr<=h1expot->GetNbinsX();ibr++) //Above CutOff Time++
							{
								if(h1expot->GetBinLowEdge(ibr)>=ucutr){
								    h1expot->AddBinContent(ibr,nt);
								}
							}
							h1expot->SetEntries(h1expot->GetEntries()+1);
						
							//exposure time
							histn = Form("expotime_q%d_d%d_BeBin", *it, id);
							TH1F *h2expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
							//--Above Cutoff LowBin>=Cutof T+ 
							for(int ibr=1;ibr<=h2expot->GetNbinsX();ibr++) //Above CutOff Time++
							{
								if(h2expot->GetBinLowEdge(ibr)>=ucutr){
								    h2expot->AddBinContent(ibr,nt);
								}
							}
							h2expot->SetEntries(h2expot->GetEntries()+1);
						
							//exposure time
							histn = Form("expotime_q%d_d%d_HeBin", *it, id);
							TH1F *h3expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
							//--Above Cutoff LowBin>=Cutof T+ 
							for(int ibr=1;ibr<=h3expot->GetNbinsX();ibr++) //Above CutOff Time++
							{
								if(h3expot->GetBinLowEdge(ibr)>=ucutr){
								    h3expot->AddBinContent(ibr,nt);
								}
							}
							h3expot->SetEntries(h3expot->GetEntries()+1);
						
							//exposure time
							histn = Form("expotime_q%d_d%d_HZBin", *it, id);
							TH1F *h4expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
							//--Above Cutoff LowBin>=Cutof T+ 
							for(int ibr=1;ibr<=h4expot->GetNbinsX();ibr++) //Above CutOff Time++
							{
								if(h4expot->GetBinLowEdge(ibr)>=ucutr){
								    h4expot->AddBinContent(ibr,nt);
								}
							}
							h4expot->SetEntries(h4expot->GetEntries()+1);
						
							//exposure time
							histn = Form("expotime_q%d_d%d_FBin", *it, id);
							TH1F *h5expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
							//--Above Cutoff LowBin>=Cutof T+ 
							for(int ibr=1;ibr<=h5expot->GetNbinsX();ibr++) //Above CutOff Time++
							{
								if(h5expot->GetBinLowEdge(ibr)>=ucutr){
								    h5expot->AddBinContent(ibr,nt);
								}
							}
							h5expot->SetEntries(h5expot->GetEntries()+1);
						
							//exposure time
							histn = Form("expotime_q%d_d%d_FeBin", *it, id);
							TH1F *h6expot = static_cast<TH1F*>(GetHist(hman1, histn.c_str()));
						
							//--Above Cutoff LowBin>=Cutof T+ 
							for(int ibr=1;ibr<=h6expot->GetNbinsX();ibr++) //Above CutOff Time++
							{
								if(h6expot->GetBinLowEdge(ibr)>=ucutr){
								    h6expot->AddBinContent(ibr,nt);
								}
							}
							h6expot->SetEntries(h6expot->GetEntries()+1);
						
					
						} //id
					} //it
				}
				RunT[0] = RunT[1];
			}
		}
		continue;
		#endif //EXPOTIME
		// cout << " Analysis6 " << __LINE__ << endl;
		// printf("\trunT0:%d;\trunT1:%d\n", RunT[0], RunT[1]);
		//2021.10.14
		ev->InitCutoffpi();
		
		//------selection //charge the charge part to use new function in SelEvent13 which controls old or new tracker Q correctly //move into member function of SelEvent to set all these selection flag
		/*bTrig = ev->Select_Trigger();
		#ifdef FORCEOLDTRIGSETTING
		bTrig = ev->Select_TriggerRB(0,5);
		#endif //FORCEOLDTRIGSETTING
		bRun = ev->Select_Run();
		bRTI = ev->Select_RTI();
		bRTIdL1L9 = ev->Select_RTIdL1L9();
		bPart = ev->Select_Particle();
		bBeta = ev->Select_Beta();
		bTk2nd = ev->Select_Tk2nd();

		for (int ir=0; ir<3; ir++)
		{
			//--
			bRig[ir] = ev->Select_Rig(1, icffv, ir, dGeoSafety);
			
			#ifdef NOCUTOFFEFF
			bRigEff[ir]=true;
			#else
			if (!ev->isreal) bRigEff[ir]=true;
			else bRigEff[ir] = ev->Select_Rig(1, icffv, ir, dGeoSafety);
			#endif //NOCUTOFFEFF
			
			//--
			bTrack[ir] = ev->Select_TrTrack(ir);	//have L1Chis cut
			// bTrack[ir] = ev->Select_TrTrack(ir, 101);	//no L1Chis cut
			
			//--
			//bTkgeom[ir] = ev->Select_TkGeom(ir, 2);	//have inner pattern cut
			////bTkgeom[ir] = ev->Select_TkGeom(ir, 0);	//no inner pattern cut
			//#ifdef LOOSEFV
			//bTkgeom[ir] = ev->Select_TkGeom(ir, 0);	//no inner pattern cut
			//#endif //LOOSEFV
			
			//2020.11.14
			bTkgeom[ir] = ev->Select_TkGeom(ir, iFVTight);
			//2021.02.09: for ir=0, require the Tracker L1 geometry as well
			//if (ir==0) bTkgeom[ir] = ev->Select_TkGeom(1, iFVTight);
			//2021.10.22: for ir=0, require the Tracker L1 geometry as well if using unbiased L1Q cut
			#ifdef INNERUNBIAEDL1Q
			if (ir==0) bTkgeom[ir] = ev->Select_TkGeom(1, iFVTight);
			#endif //INNERUNBIAEDL1Q
		}*/
		ev->SelectEv();
		
		//--store the Processed file name and first run
		#ifdef USEQTOOL
		TTree *tEvTree=ch.GetTree();
		if(tEvTree->GetReadEvent()==0) //check if it's a new file
		{
			string sFileName = tEvTree->GetCurrentFile()->GetName();
			ev->qrm->ProcessFiles(sFileName, ev->run); //store the dst file name and first run as a map
		}
		
		if (bRun) ev->qrm->ProcessRun(ev->run);
		#endif //USEQTOOL
		
		#ifdef CHARGECAL_NEWTRQ
		if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta)
		{
			//ChargeCal
			for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
			{
				if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
				//SelChargeCal(hmanQ[iQVer], *ev, outfile, bRig, bTrack, bTkgeom, iQVer, ww0);
				SelChargeCal(hmanQ[iQVer], *ev, outfile, iQVer, ww0); //2023.04.06
			}
		}
		#endif //CHARGECAL_NEWTRQ
		
		#ifdef L1QDISSEL //2023.04.23
		if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta)
		{
			//ChargeCal
			for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
			{
				if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
				SelL1QDisSel(hmanQ[iQVer], *ev, outfile, iQVer, ww0); //2023.04.06
			}
		}
		#endif //L1QDISSEL
		
		//--trigger efficiency
		#ifdef TRIGEFF
		if (bRun && bRTI && bRTIdL1L9 && bPart && bBeta)
		{
			for (int ir=0; ir<3; ir++)
			{
				if (ir<2 && MCVer==2) continue;
				
				for (int irig=0; irig<NRig; irig++)
				{
					#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
					if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
					#endif //PASS7GBL
					
					//if (bRigEff[ir] && bTrack[ir] && bTkgeom[ir])
					if (bRigEff[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.10
					{
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							//if (!ev->isreal && *it!=ev->mch) continue;
							//2020.10.25
							#ifndef EFFSEC
							if (!ev->isreal && *it!=ev->mch) continue;
							#endif //EFFSEC
							
							int physbpatt=(ev->isreal)?ev->physbpatt2:ev->physbpatt1;
							int jmembpatt=(ev->isreal)?ev->jmembpatt2:ev->jmembpatt1;
							int nacc=0;
							for (int ibit=0; ibit<8; ibit++) if ((ev->antipatt&(1<<ibit))>0) nacc++;
							
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
								if (ev->Select_Charge(*it, ir, iQVer))
								{
									#ifdef INNERUNBIAEDL1Q
									bool bUnbiasedL1XY=true;
									bool bUnbiasedL1Q=true;
									if (ir==0)
									{
										bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
										bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
										
										if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
									}
									#endif //INNERUNBIAEDL1Q
									
									//SelTrigEff(hmanQ[iQVer], *ev, ir, *it, iQVer, ww[ir]);
									SelTrigEff(hmanQ[iQVer], *ev, ir, irig, *it, iQVer, ww[ir]); //2023.03.22
								} //charge
							} //iQVer
						} //it
					} //pattern
				} //irig
			} //ir
		}
		#endif //TRIGEFF
		
		#ifdef ACCEPTANCE
		//2023.02.16
		for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
		{
			if (ev->isreal) break;
			
			if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
			for (int irig=0; irig<NRig; irig++)
			{
				#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
				if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
				#endif //PASS7GBL
				
				SelAcc(hmanQ[iQVer], *ev, iQVer, irig);
				#ifdef TOICORRECTION
				SelTOI(hmanQ[iQVer], *ev, iQVer, irig);
				#endif //TOICORRECTION
				//2023.02.17
				#ifdef BELOWL1MC
				SelBelowL1MC(hmanQ[iQVer], *ev, iQVer, irig);
				#endif //BELOWL1MC
			} //irig
		} //iQVer
		#endif //ACCEPTANCE
		
		//pre-selection
		if (bTrig && bRun && bRTI && bRTIdL1L9)
		{
			//--tracking efficiency
			#ifdef TRACKEFFN
			for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
			{
				if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
				//SelTkEffN(hmanQ[iQVer], *ev, ww, bPart, bBeta, bTrack, bTkgeom, iQVer);
				//2023.03.27
				for (int irig=0; irig<NRig; irig++)
				{
					#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
					if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
					#endif //PASS7GBL
					
					SelTkEffN(hmanQ[iQVer], *ev, ww, irig, iQVer);
				} //irig
			}
			#endif //TRACKEFFN
			
			#ifdef EFFVAL
			SelEffVal(hman1, *ev, ev->isreal, SCharge, MCharge, ww, bPart, bBeta, bTrack, bTkgeom, bCharge, ProdVer);
			#endif //EFFVAL
		
			//non-pattern selection: Good particle & Beta
			if (bPart && bBeta)
			{
				//--tracker residual
				#ifdef TRRES
				bInAllHit=true;
				for (int il=0+1; il<9-1; il++) if (ev->HasTkLHitXY(il)<=0) bInAllHit=false; //require hits on all inner layer
				for (int ir=0; ir<3; ir++)
				{
					if (!bInAllHit) break;
					int irig=0; //2023.03.10
					//if (bRig[ir] && ev->Select_TrTrack(ir, 0) && bTkgeom[ir])
					if (bRig[ir][irig] && ev->Select_TrTrack(ir, 0, irig) && bTkgeom[ir]) //2023.03.10
					{
						for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
						{
							if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
							SelTrRes(hmanQ[iQVer], *ev, ir, ww[ir], iQVer);
						}
					} //pattern cut
				} //ir
				#endif //TRRES
				
				#ifdef ESTCAL
				for (int ir=1; ir<3; ir++)
				{
					for (int irig=0; irig<NRig; irig++)
					{
						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
						//if (bTrack[ir] && bTkgeom[ir]) //2022.06.10: should take out the cutoff selection when studing the rigidity estimation
						if (bTrack[ir][irig] && bTkgeom[ir]) //2023.03.20: do selection depends on algorithm
						{
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								//if (!ev->isreal && *it!=ev->mch) continue;
								//2020.10.25
								#ifndef EFFSEC
								if (!ev->isreal && *it!=ev->mch) continue;
								#endif //EFFSEC
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
									
									//if (ev->Select_Charge(*it, ir, iQVer)) StoreRigEst(outfile, *ev, iQVer, *it, ir, ww[ir]);
									if (ev->Select_Charge(*it, ir, iQVer)) StoreRigEst(outfile, *ev, iQVer, *it, ir, ww[ir], irig); //2023.03.10
								}
							}
						} //pattern cut
					} //irig
				} //ir
				#endif //ESTCAL
				
				#ifdef TRACKEFFN
				for (int ir=1; ir<3; ir++)
				{
					if (ir<2 && MCVer==2) continue;
					for (int irig=0; irig<NRig; irig++) //2023.03.21
					{
						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						//if (bRigEff[ir] && bTrack[ir] && bTkgeom[ir])
						if (bRigEff[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.10
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								//SelTkQEff(hmanQ[iQVer], *ev, ir, ww[ir], iQVer);
								SelTkQEff(hmanQ[iQVer], *ev, ir, irig, ww[ir], iQVer); //2023.02.19
							}
						} //pattern
					} //irig
				} //ir
				#endif //TRACKEFFN
				
				#ifdef TOFQEFF
				//for (int ir=1; ir<3; ir++)
				for (int ir=0; ir<3; ir++) //2021.03.22
				{
					if (ir<2 && MCVer==2) continue;
					//int irig=0;
					for (int irig=0; irig<NRig; irig++)
					{
						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						//if (bRigEff[ir] && bTrack[ir] && bTkgeom[ir])
						if (bRigEff[ir][irig] && bTrack[ir][irig] && bTkgeom[ir]) //2023.03.11
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								//SelTofQEff(hmanQ[iQVer], *ev, ir, ww[ir], iQVer);
								//SelTofQEff(hmanQ[iQVer], *ev, ir, 0, ww[ir], iQVer); //2023.02.19
								SelTofQEff(hmanQ[iQVer], *ev, ir, irig, ww[ir], iQVer); //2023.03.11
							}
						} //pattern
					} //irig
				} //ir
				#endif //TOFQEFF
				
				#ifdef L1EFF
				for (int ir=1; ir<3; ir++)
				{
					if (ir<2 && MCVer==2) continue;
					int iReferSpan=(ir==2)?3:0; //using InnerL9 rigidity
					
					/*#ifdef GBLCHISCHECK
					for (int irig=0; irig<2; irig++) //store GBL rigidity as well //2022.10.03
					#else
					for (int irig=0; irig<1; irig++)
					#endif //GBLCHISCHECK*/
					//2023.03.10
					for (int irig=0; irig<NRig; irig++)
					{
						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						#ifdef NOCUTOFFEFF
						//if (ev->Select_TrTrack(iReferSpan) && bTkgeom[ir])
						if (ev->Select_TrTrack(iReferSpan, 111, irig) && bTkgeom[ir]) //2022.10.03
						#else
						//if (ev->Select_Rig(1, icffv, iReferSpan) && ev->Select_TrTrack(iReferSpan) && bTkgeom[ir])
						if (ev->Select_Rig(1, icffv, iReferSpan, dGeoSafety, irig) && ev->Select_TrTrack(iReferSpan, 111, irig) && bTkgeom[ir]) //2022.10.03
						#endif //NOCUTOFFEFF
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								//SelL1Eff(hmanQ[iQVer], *ev, ir, ww[ir], iQVer);
								SelL1Eff(hmanQ[iQVer], *ev, ir, irig, ww[ir], iQVer);
								
								//--store L1 efficiency with different L1ChisY cut
								#ifdef GBLCHISCHECKL1Eff
								for (int icut=0; icut<3; icut++)
								{
									SelL1Eff(hmanQ[iQVer], *ev, ir, irig, ww[ir], iQVer, icut);
								} //icut
								#endif //GBLCHISCHECKL1Eff
							}
						}
					} //irig
				} //ir
				#endif //L1EFF
				
				#ifdef L9EFF
				for (int ir=2; ir<3; ir++)
				{
					if (ir<2 && MCVer==2) continue;
					
					//pattern selection
					int iReferSpan=(ir==2)?1:0; //using L1Inner rigidity
					
					//--
					for (int irig=0; irig<NRig; irig++)
					{
						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						#ifdef NOCUTOFFEFF //2019.10.18: correctly remove geomagnetic cutoff for efficiency sample with flag NOCUTOFFEFF
						//if (ev->Select_TrTrack(iReferSpan) && bTkgeom[ir]) //with L1Inner chisq cut
						if (ev->Select_TrTrack(iReferSpan, 111, irig) && bTkgeom[ir]) //2023.03.30
						#else
						//if (ev->Select_Rig(1, icffv, iReferSpan) && ev->Select_TrTrack(iReferSpan) && bTkgeom[ir]) //with L1Inner chisq cut
						if (ev->Select_Rig(1, icffv, iReferSpan, dGeoSafety, irig) && ev->Select_TrTrack(iReferSpan, 111, irig) && bTkgeom[ir]) //2023.03.30
						#endif //NOCUTOFFEFF
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								//SelL9Eff(hmanQ[iQVer], *ev, ir, ww[ir], iQVer);
								SelL9Eff(hmanQ[iQVer], *ev, ir, irig, ww[ir], iQVer); //2023.03.21
							}
						}
					} //irig
				} //ir
				#endif //L9EFF
				
				#ifdef RIGRESO
				for (int ir=0; ir<4; ir++)
				{
					//--apply full span fiducial volumn for innerL9, since when using InnerL9 for Full Span L1 efficiency we will require Full span fiduical volumn
					//for (int irig=0; irig<2; irig++)
					//2023.03.10
					for (int irig=0; irig<NRig; irig++)
					{
						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						bool bRigReso=false;
						#ifdef RIGRESOWOL1CHISQY
						//bRigReso=ev->Select_TrTrack(ir,101);
						//bRigReso=ev->Select_TrTrack(ir,101,2*irig); //2021.09.04
						bRigReso=ev->Select_TrTrack(ir,101,irig); //2023.03.27
						#else
						//bRigReso=ev->Select_TrTrack(ir);
						//bRigReso=ev->Select_TrTrack(ir,111,2*irig); //2021.09.04
						bRigReso=ev->Select_TrTrack(ir,111,irig); //2023.03.27
						#endif //RIGRESOWOL1CHISQY
						
						//if (ir==3) bRigReso=bRigReso && ev->Select_TkGeom(ir, 2);
						if (ir==3) bRigReso=bRigReso && ev->Select_TkGeom(ir, iFVTight); //2020.11.14
						else if (ir==0) bRigReso=bRigReso && ev->Select_TkGeom(1, iFVTight); //2021.03.13
						else bRigReso=bRigReso && bTkgeom[ir];
						
						
						if (bRigReso)
						{
							//double weight = (ir==3)?ww0[0]:ww0[ir]; //2019.05.15: use only the shape weight and double correctly
							//double weight = (ir==3)?ww2[0]:ww2[ir]; //2019.08.16: use only the l1 + l19 focus combine weight
							double weight = (ir==3)?ww[0]:ww[ir]; //2019.08.22: use the l1 + l19 focus combine + shape weight
							//SelRigReso(hman1, *ev, ir, weight);
							
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								//SelRigReso(hmanQ[iQVer], *ev, ir, iQVer, weight);
								//SelRigReso(hmanQ[iQVer], *ev, outfile, ir, iQVer, weight); //2021.09.03
								SelRigReso(hmanQ[iQVer], *ev, outfile, ir, irig, iQVer, weight); //2021.09.04
							} //iQVer
						} //pattern selection
					} //irig
				} //ir
				#endif //RIGRESO
				
				/*#ifdef RIG
				
				#ifdef VARDIS
				sDirVD = Form("Event_count/PreSelVD");
				ev->StoreVarDisInnerTrk(sDirVD.c_str(), 1);
				ev->StoreVarDisExtTrk(sDirVD.c_str(), 1);
				ev->StoreVarDisTof(sDirVD.c_str(), 1);
				#endif //VARDIS
				
				for (int ir=0; ir<3; ir++)
				{
					if (ir<2 && MCVer==2) continue;
					if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
					{
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							//if (!ev->isreal && *it!=ev->mch) continue;
							//2020.10.25
							#ifndef EFFSEC
							if (!ev->isreal && *it!=ev->mch) continue;
							#endif //EFFSEC
							
							if (ev->Select_Charge(*it, ir, QCutVer))
							{
								histn = Form("rig_r%d_q%d", ir, *it);
								GetHist(hman1, histn.c_str())->Fill(ev->GetRigidity(ir));
								
								if (!ev->isreal)
								{
									histn = Form("rig_r%d_q%d_reweight", ir, *it);
									GetHist(hman1, histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);	
								}
								#ifdef BADL1LADRIG
								if (ev->tk_lid[0]!=+811) histn = Form("rig_r%d_q%d_nobadl1lad", ir, *it);
								else histn = Form("rig_r%d_q%d_badl1lad", ir, *it);
								GetHist(hman1, histn.c_str())->Fill(ev->GetRigidity(ir));
								#endif //BADL1LADRIG
							
								//#ifdef VARDIS
								//sDirVD = Form("Event_count/q%d/%s/VarDis", *it, sSpan[ir].c_str());
								//ev->StoreVarDisInnerTrk(sDirVD.c_str());
								//ev->StoreVarDisExtTrk(sDirVD.c_str());
								//ev->StoreVarDisTof(sDirVD.c_str());
								//#endif //VARDIS
							}
						} //it
					} //pattern cut
				} //ir
				#endif //RIG*/
				
				#ifdef RIG_NEWTRQ
				/*#ifdef VARDIS
				for (int iQVer=QVeri; iQVer<nQVer; iQVer++) //2019.05.14
				{
					if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
					#ifdef QSELTWIKI //2019.09.01
					if (iQVer==0 || (iQVer%3==1 && iQVer>=3))
					#else
					if (iQVer==0 || iQVer%3==1)
					#endif //QSELTWIKI
					{
						sDirVD = Form("Event_count%d/PreSelVD", iQVer);
						ev->StoreVarDisInnerTrk(sDirVD.c_str());
						ev->StoreVarDisExtTrk(sDirVD.c_str());
						ev->StoreVarDisTof(sDirVD.c_str());
					}
				} //iQVer
				#endif //VARDIS*/
				
				//for (int ir=1; ir<3; ir++)
				double mrig=(ev->isreal)?-9999:ev->mmom/ev->mch; //2022.11.09
				for (int ir=iSpanL; ir<iSpanU; ir++) //2021.02.07
				{
					//2020.12.31: store Kalman rigidity as well
					//for (int irig=0; irig<2; irig++)
					//2023.03.10
					for (int irig=0; irig<NRig; irig++)
					{
						#ifdef SELCHECK
						if(irig!=0) continue;
						#endif //SELCHECK

						#ifndef PASS7GBL //store GBL for PASS7 as well, only when using my dst //2023.03.10
						if ((ev->amsdn%100)<68 && irig==1) continue; //skip irig=1 for PASS7 dst, which is Alcaraz instead of GBL
						#endif //PASS7GBL
						
						//int iRigAlgo=2*irig;
						int iRigAlgo=irig; //now kalman is irig=2, same as in dst //2023.03.10
						double dRig=ev->GetRigidity(ir,iRigAlgo);
						//string sRig="";
						//if (irig==1) sRig="_kalman";
						string sRig=sRigName[irig]; //2023.03.10
						
						//if (ev->Select_Rig(1, icffv, ir, dGeoSafety, iRigAlgo) && ev->Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir])
						//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
						if (bRig[ir][iRigAlgo] && bTrack[ir][iRigAlgo] && bTkgeom[ir]) //2023.03.10
						{
							//--use new charge selection function
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								#ifndef EFFSEC
								if (!ev->isreal && *it!=ev->mch) continue;
								#endif //EFFSEC
								
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
									
									//if (*it==1) cout << "run=" << ev->run << " event=" << ev->event << " new InnerQ=" << ev->GetTkInQNew(2,iQVer) << " old InnerQ=" << ev->GetTkInQNew(2,0) << endl; //XXX
									if (ev->Select_Charge(*it, ir, iQVer))
									{
										//2021.02.09
										#ifdef INNERUNBIAEDL1Q
										bool bUnbiasedL1XY=true; //put into functions and fill it around SelectEV() //TODO
										bool bUnbiasedL1Q=true;
										if (ir==0)
										{
											bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
											bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
											
											//unbiased L1Q distribution
											histn = Form("UnbiasedL1Q_r%d_q%d%s", ir, *it, sRig.c_str());
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetUBExtQTrkCor(0, 2, iQVer), ww[ir]);
											
											histn = Form("UnbiasedL1QvsRig_r%d_q%d%s", ir, *it, sRig.c_str());
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dRig, ev->GetUBExtQTrkCor(0, 2, iQVer), ww[ir]);
											
											if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
											
											//unbiased L1Q distribution, after selection
											histn = Form("UnbiasedL1Q_r%d_q%d%s_cut", ir, *it, sRig.c_str());
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetUBExtQTrkCor(0, 2, iQVer), ww[ir]);
											
											histn = Form("UnbiasedL1QvsRig_r%d_q%d%s_cut", ir, *it, sRig.c_str());
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dRig, ev->GetUBExtQTrkCor(0, 2, iQVer), ww[ir]);
											
											/*//2021.04.18
											#ifdef ISOTOPEBGSUB
											histn = Form("rig_r%d_q%d_%.1fsigma%s_noBGSub", ir, *it, 2.0, sRig.c_str());
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, ww[ir]);
											
											//if (!((ev->ntrack==1) || !ev->Select_Tk2ndRecon())) continue;
											if (!((ev->ntrack==1) || !ev->Select_Tk2nd())) continue; //2021.05.24
											#endif //ISOTOPEBGSUB*/
										}
										#endif //INNERUNBIAEDL1Q
										// printf("\tch:%d\tir:%d\tiQver:%d\t->Q:",*it, ir, iQVer);
										// cout << ev->GetTkInQNew(2,iQVer) << endl;
										// printf("\t Analysis!!! --- %d\n", __LINE__);
										
										/*histn = Form("rig_r%d_q%d_%.1fsigma%s", ir, *it, 2.0, sRig.c_str());
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, ww[ir]);
										
										histn = Form("rig_r%d_q%d_%.1fsigma%s_noRigCor", ir, *it, 2.0, sRig.c_str());
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir,iRigAlgo,2,false), ww[ir]);*/
					
										//apply additional cut //2021.05.24
										for (int ievcut=0; ievcut<4; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd cut, 2: ntrack=1, 3: no 2nd cut
										{
											#ifdef SELCHECK
											if(ievcut!=0) continue;
											#endif //SELCHECK
											
											bool bEvCut=true;
											if (ievcut==1) bEvCut = ev->ntrack==1 || ev->Select_Tk2nd();
											else if (ievcut==2) bEvCut = ev->ntrack==1;
											else if (ievcut==3) bEvCut = ev->Select_Tk2nd();
											if (!bEvCut) continue;
											
											string sEvCut="";
											if (ievcut>0) sEvCut = Form("_evcut%d", ievcut);
											
											//histn = Form("rig_r%d_q%d_%.1fsigma%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str());
											//GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, ww[ir]);
											//
											//histn = Form("rig_r%d_q%d_%.1fsigma%s%s_noRigCor", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str());
											//GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir,iRigAlgo,2,false), ww[ir]);
											
											//--2022.06.29
											//--store the BeBin, HeBin, HZBin and the rebin event count
											for (int irigcor=0; irigcor<2; irigcor++)
											{
												#ifdef SELCHECK
												if(irigcor!=0) continue;
												#endif //SELCHECK
												string sRigCor = (irigcor==0)?"":"_noRigCor";
												double dRig0 = (irigcor==0)?dRig:ev->GetRigidity(ir,iRigAlgo,2,false);
												histn = Form("rig_r%d_q%d_%.1fsigma%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig0, ww[ir]);
												
												//--BeBin
												histn = Form("rig_r%d_q%d_%.1fsigma_BeBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig0, ww[ir]);
												
												//--HeBin
												histn = Form("rig_r%d_q%d_%.1fsigma_HeBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig0, ww[ir]);
												
												//--HZBin
												histn = Form("rig_r%d_q%d_%.1fsigma_HZBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig0, ww[ir]);
												
												//--FNaAl published binning
												histn = Form("rig_r%d_q%d_%.1fsigma_FBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig0, ww[ir]);
												
												//--Fe published binning
												histn = Form("rig_r%d_q%d_%.1fsigma_FeBin%s%s%s", ir, *it, 2.0, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig0, ww[ir]);
												
												#ifdef EVCOUNTINORBIT //2023.08.01
												for (int iRigRange0=-1; iRigRange0<1; iRigRange0++) //iRigRange=-1: all, =0: <10, =1: 10-30, =2: >30
												{
													int iRigRange = iRigRange0;
													if (iRigRange0==0)
													{
														if (dRig0<10) iRigRange = 0;
														else if (dRig0<30) iRigRange = 1;
														else iRigRange = 2;
													}
													
													histn = Form("rigvsorbitm_r%d_q%d%s%s%s", ir, *it, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
													histn += Form("_RigRange%d", iRigRange);
													//GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->thetam/PI*180, ev->phim/PI*180, ww[ir]);
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->phim/PI*180, ev->thetam/PI*180, ww[ir]);
													
													histn = Form("rigvsorbits_r%d_q%d%s%s%s", ir, *it, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
													histn += Form("_RigRange%d", iRigRange);
													//GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->thetas/PI*180, ev->phis/PI*180, ww[ir]);
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->phis/PI*180, ev->thetas/PI*180, ww[ir]);
												} //iRigRange
												
												//store the event count in different phi range //2023.08.13
												for (int ims=0; ims<2; ims++)
												{
													double dPhi = (ims==0)?ev->thetam:ev->thetas;
													
													for (int iphirange=0; iphirange<10; iphirange++)
													{
														double dPhiL = -1.+0.2*iphirange;
														double dPhiU = -1.+0.2*(iphirange+1);
														if (!(dPhi>dPhiL && dPhi<dPhiU)) continue;
														
														histn = Form("rig_r%d_q%d%s%s%s", ir, *it, sRig.c_str(), sEvCut.c_str(), sRigCor.c_str());
														histn += Form("_%.1f_%.1f_Theta", dPhiL, dPhiU);
														histn += (ims==0)?"m":"s";
														GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, ww[ir]);
													} //iphirange
												} //ims
												#endif //EVCOUNTINORBIT
											} //irigcor
										} //ievcut
										
										#ifdef ISOTOPE
										#ifdef ISOTOPEEV
										//SelIsotopeEv(hmanQ[iQVer], *ev, ir, *it, iQVer);
										#endif //ISOTOPE
										#endif //ISOTOPE
										
										//2020.12.31
										if (irig>0) break;
										
										//2021.05.24
										#ifdef INNERUNBIAEDL1Q
										if (ir==0)
										{
											#ifdef ISOTOPEBGSUB
											histn = Form("rig_r%d_q%d_%.1fsigma%s_noBGSub", ir, *it, 2.0, sRig.c_str());
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, ww[ir]);
											
											if (!((ev->ntrack==1) || ev->Select_Tk2nd())) continue;
											#endif //ISOTOPEBGSUB
										}
										#endif //INNERUNBIAEDL1Q
										
										//2020.11.14
										#ifdef LOOSEFV
										histn = Form("rig_r%d_q%d_%.1fsigma_TightFV", ir, *it, 2.0);
										if (ev->Select_TkGeom(ir, 2)) GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
										#endif //LOOSEFV
										
										//rig vs estimated rig
										#ifdef RIGESTCHECK
										for (int imr=0; imr<3; imr++)
										{
											for (int iRigEstVer=0; iRigEstVer<4; iRigEstVer++)
											{
												if (imr==0 && iRigEstVer>0) continue;
												double dEstRig;
												if (imr==0) dEstRig = MPROTON*dMass[*it]*ev->tof_betah / sqrt(1 - pow(ev->tof_betah, 2)) / *it;
												else if (imr==1) dEstRig = getcutoffrig5(ev->mcutoffi[icffv][1], *it, iQVer, iRigEstVer); //for TkEff, using iRigEstVer=1 for cutoff
												else if (imr==2) dEstRig = getedeprig5( ev->ecal_en[2]/1000., *it, iQVer, iRigEstVer); //for TkEff, using iRigEstVer=2 for Ecal
												
												histn = Form("rigvsEstRig_r%d_q%d_mr%d_ver%d", ir, *it, imr, iRigEstVer);
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dEstRig, ev->GetRigidity(ir), ww[ir]);
											} //iRigEstVer
										} //imr
										#endif //RIGESTCHECK
										
										//----
										/*#ifdef VARDIS
										#ifdef QSELTWIKI //only store for Q sel with TWiki
										if (iQVer%3==1 && iQVer<3) continue;
										#endif //QSELTWIKI
										sDirVD = Form("Event_count%d/q%d/%s/VarDis", iQVer, *it, sSpan[ir].c_str());
										ev->StoreVarDisInnerTrk(sDirVD.c_str());
										ev->StoreVarDisExtTrk(sDirVD.c_str());
										ev->StoreVarDisTof(sDirVD.c_str());
										
										//Var Dis vs Rig
										sDirVD = Form("Event_count%d/q%d/%s/VarDisvsRIG", iQVer, *it, sSpan[ir].c_str());
										ev->StoreVarDisInnerTrkRIG(sDirVD.c_str(), ir);
										ev->StoreVarDisExtTrkRIG(sDirVD.c_str(), ir);
										ev->StoreVarDisTofRIG(sDirVD.c_str(), ir);
										#endif //VARDIS*/
										
										#ifdef EXTASYCHECK
										for (int iext=0; iext<2; iext++)
										{
											int il=(iext==0)?1:9;
											double dXQ = ev->GetTkLQNew(il-1, 0, iQVer), dYQ = ev->GetTkLQNew(il-1, 1, iQVer);
											double dQAsy = fabs((dXQ-dYQ)/(dXQ+dYQ));
											
											//------XQ vs YQ
											histn = Form("L%dQ_YvsX_r%d_q%d_qv%d", il, ir, *it, iQVer);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dXQ, dYQ, ww[ir]);
											
											//------XQ YQ asymmetry vs Rigidity
											histn = Form("L%dQAsyvsRig_r%d_q%d_qv%d", il, ir, *it, iQVer);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), dQAsy, ww[ir]);
											
											//------Hit position, without & with Asymmetry cut
											histn = Form("L%dHitPos_r%d_q%d_qv%d_noAsyCut", il, ir, *it, iQVer);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->tk_pos[il-1][0], ev->tk_pos[il-1][1], ww[ir]);
						
											histn = Form("L%dHitPos_r%d_q%d_qv%d_withAsyCut", il, ir, *it, iQVer);
											if (dQAsy<0.2) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->tk_pos[il-1][0], ev->tk_pos[il-1][1], ww[ir]);
										} //iext
										#endif //EXTASYCHECK
										
										//-- //2023.01.13
										#ifdef TRDADC
										#ifdef TRDADCOLD
										for (int imatch=0; imatch<1; imatch++) //different method for match the TRD hit with particle, =0: TRD hit around with Tracker Track extrapolation (store already in MIT dst), =1: maximum hit in each TRD layer (not implemented yet)
										{
											if (irig!=0) break; //now only Choutko
											
											//for (int ievcut=0; ievcut<2; ievcut++)
											for (int ievcut=0; ievcut<1; ievcut++)
											{
												if (ievcut==1)
												{
													if (!(ev->ntrack==1 || ev->Select_Tk2nd())) continue;
												}
												//for (int iplcor=0; iplcor<2; iplcor++) //path length correciton, iplcor=0: without correction, =1: after correction
												for (int iplcor=0; iplcor<1; iplcor++) //path length correciton, iplcor=0: without correction, =1: after correction
												{
													double dTotalADC=0;
													double dMinADC=99999, dMaxADC=-1;
													
													double dADC[20] = {-1.};
													for (int il=0; il<20; il++)
													{
														dADC[il] = -1.;
														if (imatch==0) dADC[il] = ev->trd_amplk[il];
														if (iplcor==1) dADC[il] = dADC[il]/ev->trd_pathlk[il];
														dTotalADC += dADC[il];
														if (dADC[il]<=dMinADC) dMinADC=dADC[il];
														if (dADC[il]>=dMaxADC) dMaxADC=dADC[il];
														
														/*//--adc for all rig
														histn = Form("TRDAdc%d_r%d_q%d_TrdL%d", imatch, ir, *it, il);
														histn += Form("_evcut%d", ievcut);
														GetHist(hmanQ[iQVer], histn.c_str())->Fill(dADC[il], ww[ir]);*/
														
														//--adc vs rig
														histn = Form("TRDAdc%dvsRig_r%d_q%d_TrdL%d_PLCor%d", imatch, ir, *it, il, iplcor);
														histn += Form("_evcut%d", ievcut);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dRig, dADC[il], ww[ir]);
														
														//--adc vs InnerQ //2023.01.18
														histn = Form("TRDAdc%dvsInnerQ_r%d_q%d_TrdL%d_PLCor%d", imatch, ir, *it, il, iplcor);
														histn += Form("_evcut%d", ievcut);
														//GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetTkInQNew(2, iQVer), dADC[il], ww[ir]);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetTkInQNew(2, (*it<=2)?0:iQVer), dADC[il], ww[ir]);
													} //il
													
													//--average adc from all layer vs rig
													for (int isum=0; isum<NTRDADCSum; isum++) //isum=0: average, =1: average without min, =2: average without max, =3: average without both min and max
													{
														double dAver = dTotalADC;
														int iTotalL = 20;
														if (isum==1 || isum==3) dAver -= dMinADC, iTotalL--;
														if (isum==2 || isum==3) dAver -= dMaxADC, iTotalL--;
														dAver = dAver/iTotalL;
														
														histn = Form("TRDAdc%dvsRig_r%d_q%d_PLCor%d", imatch, ir, *it, iplcor);
														histn += Form("_evcut%d", ievcut);
														histn += Form("_sum%d", isum);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dRig, dAver, ww[ir]);
														
														//--adc vs average adc //2023.01.18
														for (int il=0; il<20; il++)
														{
															histn = Form("TRDAdc%dvsAverAdc_r%d_q%d_TrdL%d_PLCor%d", imatch, ir, *it, il, iplcor);
															histn += Form("_evcut%d", ievcut);
															histn += Form("_sum%d", isum);
															GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dAver, dADC[il], ww[ir]);
														} //il
													} //isum
												} //iplcor
											} //ievcut
										} //imatch
										#endif  //TRDADCOLD
										
										//-- //2023.01.26
										#ifdef MAXADCDIS
										vector<pair<int, double> > vpidADC;
										vector<pair<int, double> > vpidADCNoCut;
										int NHitSeg[5] = {0};
										for (int il=0; il<20; il++)
										{
											double dADC = ev->trd_amplk[il];
											vpidADCNoCut.push_back(make_pair(dADC, il));
											if (dADC<TRDADCThrs[*it-1]) continue;
											vpidADC.push_back(make_pair(dADC, il));
											int iseg=0;
											if (il<=3) iseg=0;
											else if (il<=15) iseg=1;
											else if (il<=19) iseg=2;
											NHitSeg[iseg]++;
										} //il
										std::sort(vpidADC.begin(), vpidADC.end(), CompSignal);
										//std::reverse(vpidADC.begin(), vpidADC.end());
										std::sort(vpidADCNoCut.begin(), vpidADCNoCut.end(), CompSignal);
										NHitSeg[3] = NHitSeg[0]+NHitSeg[2]; //Bottom+Top, i.e. all Y
										NHitSeg[4] = NHitSeg[0]+NHitSeg[1]+NHitSeg[2]; //All
										
										//--check the pattern
										int NHitPattCut[2] = {-1};
										int NXY[2] = {0};
										for (int ic=0; ic<vpidADC.size(); ic++)
										{
											int il = vpidADC.at(ic).second;
											int side = (il>3 && il<=15)?0:1;
											NXY[side]++;
											
											for (int ipattcut=0; ipattcut<2; ipattcut++)
											{
												if (NHitPattCut[ipattcut]>0) continue; //fill the first hit fulfill the pattern
												int XCut=3, YCut=(ipattcut==0)?3:2;
												if (NXY[0]>=XCut && NXY[1]>=YCut) NHitPattCut[ipattcut] = ic;
											} //ipattcut
										} //ic
										
										for (int ievcut=0; ievcut<1; ievcut++)
										{
											if (ievcut==1)
											{
												if (!(ev->ntrack==1 || ev->Select_Tk2nd())) continue;
											}
											
											for (int ithrcut=0; ithrcut<2; ithrcut++)
											{
												int size = (ithrcut==0)?vpidADC.size():vpidADCNoCut.size();
												for (int iNLayerCut=0; iNLayerCut<2; iNLayerCut++)
												{
													if (iNLayerCut==1 && size<10) continue;
													histn = Form("TRDAdcvsAdcOrder_r%d_q%d", ir, *it);
													histn += Form("_evcut%d", ievcut);
													histn += Form("_ThrCut%d", ithrcut);
													histn += Form("_NLayerCut%d", iNLayerCut);
													//for (int ic=0; ic<vpidADC.size(); ic++)
													for (int ic=0; ic<size; ic++)
													{
														int ith = (ithrcut==0)?vpidADC.at(ic).first:vpidADCNoCut.at(ic).first;
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ic, ith, ww[ir]);
													} //ic
												} //iNLayerCut
											} //ithrcut
											
											//--no. hit distributiomn
											for (int iseg=0; iseg<5; iseg++)
											{
												histn = Form("NoTRDHit_r%d_q%d", ir, *it);
												histn += Form("_evcut%d", ievcut);
												if (iseg==0) histn += Form("_Bottom"); //Y
												else if (iseg==1) histn += Form("_Middle"); //X
												else if (iseg==2) histn += Form("_Top"); //Y
												else if (iseg==3) histn += Form("_BottomTop"); //All Y
												else if (iseg==4) histn += Form("_BottomMiddleTop"); //Both X and Y
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(NHitSeg[iseg], ww[ir]);
											} //iseg
											
											//--until how many hits that satisfy the pattern requirement
											for (int ipattcut=0; ipattcut<2; ipattcut++) //ipattcut=0: 3X3Y, ipattcut=1: 3X2Y (release the requirement on bottom and top)
											{
												histn = Form("NoHitPassingPatt_r%d_q%d", ir, *it);
												histn += Form("_evcut%d", ievcut);
												histn += Form("_PattCut%d", ipattcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(NHitPattCut[ipattcut], ww[ir]);
											} //ipattcut
										} //ievcut
										#endif //MAXADCDIS
										
										//--
										#endif //TRDADC
									}
										
									//--TRD adc, without separating charge //2023.01.26
									#ifdef TRDADC
									#ifdef TRDADCNEW
									for (int ievcut=0; ievcut<1; ievcut++)
									{
										if (irig!=0) break; //now only Choutko
										
										if (ievcut==1)
										{
											if (!(ev->ntrack==1 || ev->Select_Tk2nd())) continue;
										}
										
										//--adc vs InnerQ
										double dTotalADC=0;
										double dMinADC=99999, dMaxADC=-1;
										double dADC[20] = {-1.};
										for (int il=0; il<20; il++)
										{
											dADC[il] = ev->trd_amplk[il];
											dTotalADC += dADC[il];
											if (dADC[il]<=dMinADC) dMinADC=dADC[il];
											if (dADC[il]>=dMaxADC) dMaxADC=dADC[il];
											
											//--
											for (int iver=0; iver<2; iver++) //iver=0: old charge, iver=1: new charge (not avaliable for Z=1)
											{
												histn = Form("TRDAdcvsInnerQ_r%d", ir);
												histn += Form("_evcut%d", ievcut);
												histn += Form("_ChargeVer%d", iver);
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetTkInQNew(2, (iver==0)?0:iQVer), dADC[il], ww[ir]);
											} //iver
										} //il
										
										//--adc vs average adc
										for (int isum=0; isum<NTRDADCSum; isum++) //isum=0: average, =1: average without min, =2: average without max, =3: average without both min and max
										{
											double dAver = dTotalADC;
											int iTotalL = 20;
											if (isum==1 || isum==3) dAver -= dMinADC, iTotalL--;
											if (isum==2 || isum==3) dAver -= dMaxADC, iTotalL--;
											dAver = dAver/iTotalL;
											
											histn = Form("TRDAdcvsAverAdc_r%d", ir);
											histn += Form("_evcut%d", ievcut);
											histn += Form("_sum%d", isum);
											for (int il=0; il<20; il++) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dAver, dADC[il], ww[ir]);
										} //isum
									} //ievcut
									#endif //TRDADCNEW
									#endif //TRDADC
								} //iQVer
							} //it
						} //pattern cut
						
						#ifdef ISOTOPE
						for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
						{
							if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
							SelIsotope(hmanQ[iQVer], *ev, ir, iQVer, irig); //2023.02.28
						} //iQVer
						
						//check rich refractive index //2021.09.18
						#ifdef CHECKRICHREFRACINDEX
						//--get the day, which is the total no. days after May 20, 2011 (May 20, 2011 is day 0, 1305820800)
						//const unsigned int liStart=1305820800;
						const unsigned int liStart=1305849600; //2021.09.27: correct starting time in GMT
						const unsigned int liDay=24*60*60;
						int iday=(ev->time[0]-liStart)/liDay;
						//const double dRigCut[3] = {50., 100. ,150.};
						const double dRigCut0=10; //2021.10.27
						//if (ev->Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir] && ir==0) //not apply any cutoff
						//--use the Inner geometry without unbiased L1 hit cut (no geometry, no hit and no charge cut, not required to pass Tracker L1)
						if (ev->Select_TrTrack(ir,111,iRigAlgo) && ev->Select_TkGeom(1, iFVTight) && ir==0) //not apply any cutoff
						{
							//--use new charge selection function
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
									if (ev->Select_Charge(*it, ir, iQVer))
									{
										//2021.10.20: now use the pure Inner geometry
										#ifdef INNERUNBIAEDL1Q
										bool bUnbiasedL1XY=true;
										bool bUnbiasedL1Q=true;
										if (ir==0)
										{
											bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
											bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
											
											if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
										}
										#endif //INNERUNBIAEDL1Q
										
										for (int ibeta=1; ibeta<3; ibeta++)
										{
											if (irig!=1) break; //2021.11.29
											if (ibeta>0)
											{
												if ((ibeta==1 && !ev->rich_NaF) || (ibeta==2 && ev->rich_NaF)) continue;
												#ifdef LIPRICHBETA
												if (!ev->SelectRichB(*it)) continue;
												#else
												if (!ev->SelectRich(*it)) continue;
												#endif //LIPRICHBETA
											}
											
											double dBeta;
											//if (ibeta==0) dBeta = ev->tof_betah;
											if (ibeta==0) dBeta = ev->tof_betahmc; //2023.01.11
											//else dBeta = ev->rich_beta[0];
											else
											{
												#ifdef LIPRICHBETA
												dBeta = ev->rich_betap;
												#else
												dBeta = ev->rich_beta[0];
												#endif //LIPRICHBETA
											}
											double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
											double dMass = CalMass(*it, dBeta, dRig);
											
											#ifdef GETEVENTFAST
											bool bEventFast=true;
											#else
											bool bEventFast=false;
											#endif //GETEVENTFAST
											
											double dBetaWS[3] = {dBeta, -2, -4};
											#ifdef NPEWEIGHTED //XXX do only above certain rigidity
											//if (dRig>=dRigCut[0])
											if (dRig>=10) //2022.09.11
											{
												//cout << " ****** ievent=" << ievent << " get ams event, q=" << *it << ", rig=" << dRig << ", RigCut=" << dRigCut[0] << " irich=" << ev->irich << " ******" << endl; //XXX
												//set the flag before get event
												//RichRingR::shouldLoadCorrection=RichRingR::tileCorrection;
												RichRingR::shouldLoadCorrection=RichRingR::fullUniformityCorrection;
												RichOffline::RichRing::RingClusteringNpeWeight=false;
												//AMSEventR *amsev = ev->GetAMSEventR(true, false);
												//AMSEventR *amsev = ev->GetAMSEventR(true, true, bEventFast);
												AMSEventR *amsev = ev->GetAMSEventR(true, 1, bEventFast); //2022.12.07
												amsev->ReadHeader(amsev->Entry());
												
												RichRingR *ring = amsev->pRichRing(ev->irich);
												if (ring==NULL) cout << "*** can't found the ring: ievent=" << ievent << " irich=" << ev->irich << " ring=" << ring << endl;
												if (ring!=NULL)
												{
													//--beta with RichRingR::tileCorrection
													ring = ring->ReBuild();
													/*if (ring!=NULL)
													{
														if (ring->getBeta() != dBeta)
														{
															cout << " ****** ievent=" << ievent << " different beta before and after Rebuild: " << dBeta << " " << ring->getBeta() << endl;//XXX
															//dBetaWS[0] = ring->getBeta();
														}
													}*/
													//ring->getBeta() without rebuild will give the same result as dBeta, after rebuild, different results obtained, should due to different table loaded
													dBetaWS[0] = dBeta; //cannot change to different Correction for the same event, will keep the stored result for tileCorrection //2022.09.11
													
													//--beta with RichRingR::fullUniformityCorrection
													#ifdef UNIFORMITY
													RichRingR::shouldLoadCorrection=RichRingR::fullUniformityCorrection;
													//AMSEventR *amsev = ev->GetAMSEventR(true, true, bEventFast);
													AMSEventR *amsev = ev->GetAMSEventR(true, 1, bEventFast); //2022.12.07
													
													amsev->ReadHeader(amsev->Entry());
													ring = amsev->pRichRing(ev->irich);
													/*dBetaWS[1] = ring->getBeta(); //XXX
													cout << "dBetatile=" << dBetaWS[0]; //XXX
													cout << " dBeta=" << ring->getBeta(); //XXX
													cout << ", diff=" << dBetaWS[0]-ring->getBeta() << endl; //XXX*/
													ring = ring->ReBuild();
													/*cout << "after rebuild dBeta=" << ring->getBeta(); //XXX
													cout << ", diff=" << dBetaWS[1]-ring->getBeta() << endl; //XXX*/
													//dBetaWS[1] = ring->getBeta();
													if (ring!=NULL) dBetaWS[1] = ring->getBeta();
													#endif //UNIFORMITY
													
													//--beta with RichRingR::fullUniformityCorrection and weighted sum
													#ifdef WEIGHTEDNPE
													RichOffline::RichRing::RingClusteringNpeWeight=true;
													ring = amsev->pRichRing(ev->irich);
													ring = ring->ReBuild();
													//dBetaWS[2] = ring->getBeta();
													if (ring!=NULL) dBetaWS[2] = ring->getBeta();
													//if (dBetaWS[1]!=dBetaWS[2]) cout << "beta with different option: " << dBetaWS[0] << " " << dBetaWS[1] << " " << dBetaWS[2] << endl; //XXX
													//if (fabs(dBetaWS[1] - dBetaWS[2])>1e-7) dBetaWS[2] = 0;
													#endif //WEIGHTEDNPE
												}
											}
											#endif //NPEWEIGHTED
											
											//apply additional cut
											for (int ievcut=0; ievcut<2; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd track
											{
												if (ievcut==1)
												{
													if (!(ev->ntrack==1 || ev->Select_Tk2nd())) continue;
												}
												
												#ifdef PLOTINDEX
												//--0: refractive index after correction, 1: raw refractive index
												for (int iIndex=0; iIndex<2; iIndex++)
												{
													histn = Form("RefracIndex%dvsDate_beta%d_r%d_q%d_rig%d_evcut%d", iIndex, ibeta, ir, *it, irig, ievcut);
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, ev->rich_n[iIndex], ww[ir]);
													
													//--vs emission position
													histn = Form("RefracIndex%dvspos_beta%d_r%d_q%d_rig%d_evcut%d", iIndex, ibeta, ir, *it, irig, ievcut);
													static_cast<TProfile2D*>(GetHist(hmanQ[iQVer], histn.c_str()))->Fill(ev->rich_pos[0], ev->rich_pos[1], ev->rich_n[iIndex], ww[ir]);
												} //iIndex
												
												//raw/corrected //2021.10.26
												histn = Form("RefracIndexRawToCorrectedvsDate_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, ev->rich_n[1]/ev->rich_n[0], ww[ir]);
												
												//--vs emission position
												histn = Form("RefracIndexRawToCorrectedvspos_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
												static_cast<TProfile2D*>(GetHist(hmanQ[iQVer], histn.c_str()))->Fill(ev->rich_pos[0], ev->rich_pos[1], ev->rich_n[1]/ev->rich_n[0], ww[ir]);
												#endif //PLOTINDEX
												
												//--beta distribution above certain rigidity
												for (int irigc=0; irigc<3; irigc++)
												{
													if (dRig<dRigCut[irigc]) continue;
													/*histn = Form("Beta%dvsDate_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, dBeta, ww[ir]);*/
													
													//--beta and beta vs Time, without and with long term time variation correction
													//for (int iCor=0; iCor<2; iCor++)
													for (int iCor=0; iCor<NLTCor; iCor++) //2022.04.11
													{
														string sRichCor=(iCor==0)?"":"_LongTermCor";
														for (int iws=0; iws<nWS; iws++) //iws=0: normal, iws=1: uniformity, iws=2: uniformity+weighted sum
														{
															string sWS=(iws==0)?"":Form("_ws%d", iws);
															//double dBetaCor = dBeta;
															double dBetaCor = dBetaWS[iws];
															if (dBetaCor<0) continue; //remove events with "RichRingR::ReBuild() ---- failed" //2022.09.11
															//if (iCor==1 && f1RCLT[ibeta]!=NULL) dBetaCor = dBeta/(1+f1RCLT[ibeta]->Eval(iday));
															if (iCor==1 && f1RCLT[ibeta]!=NULL) dBetaCor = dBeta/(f1RCLT[ibeta]->Eval(iday));
															//--beta vs time
															histn = Form("Beta%dvsDate_r%d_q%d_rig%d_evcut%d_RigCut%d%s%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str(), sWS.c_str());
															GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, dBetaCor, ww[ir]);
													
															//--beta
															histn = Form("Beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d%s%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str(), sWS.c_str());
															GetHist(hmanQ[iQVer], histn.c_str())->Fill(dBetaCor, ww[ir]);
															//--rbeta vs emisison distance //2022.09.11
															if (ibeta==2)
															{
																//--rbeta vs 
																histn = Form("rBeta%dvsEmission_r%d_q%d_rig%d_evcut%d_RigCut%d%s%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str(), sWS.c_str());
																GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRichVar(8), dBetaCor, ww[ir]);
															}
														} //iws
														//difference of beta with difference build
														if (nWS==3)
														{
															//--beta difference to before rebuild
															histn = Form("BetaDiffRebuild%dvsrig_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str());
															GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, dBetaWS[0] - dBeta);
															
															//--beta difference to tileCorrection
															histn = Form("BetaDiffUniformity%dvsrig_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str());
															//GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, dBetaWS[1] - dBetaWS[0]);
															if (dBetaWS[1]>0) GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, dBetaWS[1] - dBetaWS[0]); //remove events with "RichRingR::ReBuild() ---- failed" //2022.09.11
															
															//--beta difference to uniformity
															histn = Form("BetaDiffNpeReweighted%dvsrig_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc], sRichCor.c_str());
															//GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, dBetaWS[2] - dBetaWS[1]);
															if (dBetaWS[1]>0 && dBetaWS[2]>0) GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, dBetaWS[2] - dBetaWS[1]); //remove events with "RichRingR::ReBuild() ---- failed" //2022.09.11
														}
													} //iCor
													
													//--beta vs time, TProfile
													#ifdef PLOTPROFILE
													histn = Form("Beta%dvsDate_profile_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
													GetHist(hmanQ[iQVer], histn.c_str())->Fill(iday, dBeta);
													#endif //PLOTPROFILE
													
													//--beta value as function of reconstructed emission point, using TProfile2D
													#ifdef CHECKRICHINDEXPOS
													histn = Form("Beta%dvsPos_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
													static_cast<TProfile2D*>(GetHist(hmanQ[iQVer], histn.c_str()))->Fill(ev->rich_pos[0], ev->rich_pos[1], dBeta, ww[ir]);
													#endif //CHECKRICHINDEXPOS
													//--index after beta correction //2021.10.26
													#ifdef PLOTINDEX
													//--0: refractive index after correction, 1: raw refractive index
													for (int iIndex=0; iIndex<2; iIndex++)
													{
														histn = Form("RefracIndex_Beta%dCorvsDate_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, ev->rich_n[iIndex]*dBeta, ww[ir]);
														
														//--profile
														histn = Form("RefracIndex_Beta%dCorvsDate_profile_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
														GetHist(hmanQ[iQVer], histn.c_str())->Fill(iday, ev->rich_n[iIndex]*dBeta);
														//--vs emission position
														histn = Form("RefracIndex_Beta%dCorvspos_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
														static_cast<TProfile2D*>(GetHist(hmanQ[iQVer], histn.c_str()))->Fill(ev->rich_pos[0], ev->rich_pos[1], ev->rich_n[iIndex]*dBeta, ww[ir]);
													} //iIndex
													#endif //PLOTINDEX
												
													//2021.11.02
													#ifdef DIRECTIONDEP
													for (int ixy=0; ixy<2; ixy++)
													{
														double dDir;
														if (ixy==0) dDir = sin(ev->rich_theta)*cos(ev->rich_phi);
														else if (ixy==1) dDir = sin(ev->rich_theta)*sin(ev->rich_phi);
														//--event count as function of incoming direction
														histn = Form("rig_Beta%d_Dir_xy%d_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ixy, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
														GetHist(hmanQ[iQVer], histn.c_str())->Fill(dDir, ww[ir]);
														
														//--beta distribution as function of incoming direction
														histn = Form("Beta%dvsDir_xy%d_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ixy, ir, *it, irig, ievcut, (int)dRigCut[irigc]);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDir, dBeta, ww[ir]);
													} //ixy
													#endif //DIRECTIONDEP
												} //irigc
												
												//----beta/beta0 above 10 GV vs time
												if (dRig>dRigCut0)
												{
													int iZ=*it, iMass=2*(*it);
													if (iZ==1) iMass=1;
													double dBeta0 = RigToBeta(iZ, iMass, dRig);
													
													//--
													for (int iws=0; iws<nWS; iws++) //iws=0: normal, iws=1: uniformity, iws=2: uniformity+weighted sum
													{
														string sWS=(iws==0)?"":Form("_ws%d", iws);
														histn = Form("BetaToBeta0%dvsDate_r%d_q%d_rig%d_evcut%d_RigCut%d%s", ibeta, ir, *it, irig, ievcut, (int)dRigCut0, sWS.c_str());
														//GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, dBeta/dBeta0, ww[ir]);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, dBetaWS[iws]/dBeta0, ww[ir]);
													} //iws
													
													//--TProfile
													#ifdef PLOTPROFILE
													histn = Form("BetaToBeta0%dvsDate_profile_r%d_q%d_rig%d_evcut%d_RigCut%d", ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
													GetHist(hmanQ[iQVer], histn.c_str())->Fill(iday, dBeta/dBeta0);
													#endif //PLOTPROFILE
													
													//--index after beta correction //2021.10.28
													#ifdef PLOTINDEX
													//--0: refractive index after correction, 1: raw refractive index
													for (int iIndex=0; iIndex<2; iIndex++)
													{
														histn = Form("RefracIndex_BetaToBeta0%dCorvsDate_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(iday, ev->rich_n[iIndex]*dBeta/dBeta0, ww[ir]);
														
														//--profile
														histn = Form("RefracIndex_BetaToBeta0%dCorvsDate_profile_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
														GetHist(hmanQ[iQVer], histn.c_str())->Fill(iday, ev->rich_n[iIndex]*dBeta/dBeta0);
														
														//--vs emission position
														histn = Form("RefracIndex_BetaToBeta0%dCorvspos_beta%d_r%d_q%d_rig%d_evcut%d_RigCut%d", iIndex, ibeta, ir, *it, irig, ievcut, (int)dRigCut0);
														static_cast<TProfile2D*>(GetHist(hmanQ[iQVer], histn.c_str()))->Fill(ev->rich_pos[0], ev->rich_pos[1], ev->rich_n[iIndex]*dBeta/dBeta0, ww[ir]);
													} //iIndex
													#endif //PLOTINDEX
												}
											} //ievcut
										} //ibeta
									} //charge cut
								} //iQVer
							} //*it
						} //pattern cut
						#endif //CHECKRICHREFRACINDEX
						
						//2021.12.21
						#ifdef RICHVARCHECK
						if (ev->Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir] && ir==0 && ev->isreal) //only store for Inner+Max L1Q to reduce storage and speed up
						{
							//--use new charge selection function
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
									if (ev->Select_Charge(*it, ir, iQVer))
									{
										#ifdef INNERUNBIAEDL1Q
										bool bUnbiasedL1XY=true;
										bool bUnbiasedL1Q=true;
										if (ir==0)
										{
											bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
											bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
											
											if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
										}
										#endif //INNERUNBIAEDL1Q
										
										//double dRigCut[3] = {50., 100., 150.};
										for (int ibeta=1; ibeta<3; ibeta++)
										{
											//if (irig!=1) break;
											if (ibeta>0)
											{
												//if (!ev->SelectRich(*it)) continue;
												//apply Rich geometry cut
												if ((ibeta==1 && !ev->rich_NaF) || (ibeta==2 && ev->rich_NaF)) continue;
												if (!ev->Select_RichGeom()) continue;
											}
											
											double dBeta;
											//if (ibeta==0) dBeta = ev->tof_betah;
											if (ibeta==0) dBeta = ev->tof_betahmc; //2023.01.11
											else dBeta = ev->rich_beta[0];
											double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
											double dMass = CalMass(*it, dBeta, dRig);
											
											//--
											string sVar="";
											string sVarFull="";
											
											//apply additional cut
											for (int ievcut=0; ievcut<1; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd track
											{
												//--No. of PMTs
												sVar="NoPMTs";
												sVarFull="No. PMTs";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_pmt);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_pmt);
												#endif //RICHVARCHECK2D
												
												//--No. of Hits
												sVar="NoHits";
												sVarFull="No. Hits";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												//GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_pmt);
												
												//--Kolmogorov
												sVar="Kolmogorov";
												sVarFull="Kolmogorov Probability";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_pb);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_pb);
												#endif //RICHVARCHECK2D
												
												//--Hit/PMT charge consistency
												sVar="HitPMTQCons";
												sVarFull="Hit/PMT charge consistency";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_PMTChargeConsistency);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_PMTChargeConsistency);
												#endif //RICHVARCHECK2D
												
												//--Npe(Ring)
												sVar="NpeRing";
												sVarFull="N_{pe}(Ring)";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_npe[0]);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_npe[0]);
												#endif //RICHVARCHECK2D
												
												//--Npe(Ring)/Z^2 //2022.01.25
												sVar="NormNpeRing";
												sVarFull="Norm N_{pe}(Ring)";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_npe[0]/pow(*it, 2));
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_npe[0]/pow(*it, 2));
												#endif //RICHVARCHECK2D
												
												//--Npe(Total)
												sVar="NpeTotal";
												sVarFull="N_{pe}(Total)";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_npe[2]);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_npe[2]);
												#endif //RICHVARCHECK2D
												
												//--Npe(Total)/Z^2 //2022.01.25
												sVar="NormNpeTotal";
												sVarFull="Norm N_{pe}(Total)";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_npe[2]/pow(*it, 2));
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_npe[2]/pow(*it, 2));
												#endif //RICHVARCHECK2D
												
												//--Npe(Ring)/Npe(Total)
												sVar="NpeRingToNpeTotal";
												sVarFull="N_{pe}(Ring)/N_{pe}(Total)";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_npe[0]/ev->rich_npe[2]);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_npe[0]/ev->rich_npe[2]);
												#endif //RICHVARCHECK2D
												
												//--RichQ
												sVar="RichQ";
												sVarFull="Rich charge";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(sqrt(ev->rich_q[0]));
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, sqrt(ev->rich_q[0]));
												#endif //RICHVARCHECK2D
												
												//--|beta_tof-beta_rich|/beta_rich
												histn = Form("BetaDiffvsBetaRich_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dBeta, dBeta*ev->get_betadiff());
												
												//--|beta_LIP-beta_CIEMAT|
												sVar="BetaDiffRec";
												sVarFull="|#beta_{LIP}-#beta_{CIEMAT}|";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_BetaConsistency);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_BetaConsistency);
												#endif //RICHVARCHECK2D
												
												//--Expected Npe
												sVar="ExpectedNpe";
												sVarFull="Expected N_{pe}";
												histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_npe[1]);
												
												#ifdef RICHVARCHECK2D
												//2d
												histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar.c_str(), ibeta, ir, *it, irig, ievcut);
												if (dRig>dRigCut[ibeta]) GetHist(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, ev->rich_npe[1]);
												#endif //RICHVARCHECK2D
											} //ievcut
										} //ibeta
									}
								} //iQVer
							} //*it
						}
						#endif //RICHVARCHECK
						//2022.08.02
						#ifdef RICHVARCHECKRESO
						if (ev->Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir] && ir==0) //only store for Inner+Max L1Q to reduce storage and speed up
						{
							//--use new charge selection function
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								if (irig==1) continue;
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
									if (ev->Select_Charge(*it, ir, iQVer))
									{
										#ifdef INNERUNBIAEDL1Q
										bool bUnbiasedL1XY=true;
										bool bUnbiasedL1Q=true;
										if (ir==0)
										{
											bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
											bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
											
											if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
										}
										#endif //INNERUNBIAEDL1Q
										
										//double dRigCut[3] = {50., 100., 150.};
										for (int ibeta=1; ibeta<3; ibeta++)
										{
											if (ibeta>0)
											{
												//apply Rich geometry cut
												if ((ibeta==1 && !ev->rich_NaF) || (ibeta==2 && ev->rich_NaF)) continue;
												if (!ev->Select_RichGeom()) continue;
											}
											
											double dBeta;
											//if (ibeta==0) dBeta = ev->tof_betah;
											if (ibeta==0) dBeta = ev->tof_betahmc; //2023.01.11
											else dBeta = ev->rich_beta[0];
											double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
											double dMass = CalMass(*it, dBeta, dRig);
											
											for (int irichcor=0; irichcor<2; irichcor++)
											{
												if (!ev->isreal && irichcor>0) break;
												string sRichBetaCor = (irichcor==0)?"":"_fullUniformityCor";
												double dBetaFullCor=dBeta;
												RichRingR *ring = NULL;
												if (irichcor==1)
												{
													RichRingR::shouldLoadCorrection=RichRingR::fullUniformityCorrection;
													RichOffline::RichRing::RingClusteringNpeWeight=false;
													//AMSEventR *amsev = ev->GetAMSEventR(true, true, true);
													AMSEventR *amsev = ev->GetAMSEventR(true, 1, true); //2022.12.07
													amsev->ReadHeader(amsev->Entry());
													
													ring = amsev->pRichRing(ev->irich);
													if (ring==NULL)
													{
														cout << "*** can't found the ring: ievent=" << ievent << " irich=" << ev->irich << " ring=" << ring << endl;
														dBetaFullCor=-2;
													}
													else dBetaFullCor = ring->getBeta();
												}
												
												//apply additional cut
												//for (int ievcut=0; ievcut<2; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd track
												for (int ievcut=1; ievcut<2; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd track
												{
													bool bEvCut=true;
													if (ievcut==1) bEvCut = ev->ntrack==1 || ev->Select_Tk2nd();
													if (!bEvCut) continue;
													for (int irvar=0; irvar<((ibeta==1)?8:9); irvar++)
													{
														if (!ev->Select_Rich_N1(irvar, *it)) continue;
														//--
														double dRichVar = ev->GetRichVar(irvar);
														//recaculate for TofRichBetaDiff and BetaDiffRec using new beta with fullUniformityCorrection
														if (irichcor==1 && ring!=NULL)
														{
															//if (sVar[irvar] == "TofRichBetaDiff") dRichVar = (ev->tof_betah-dBetaFullCor)/dBetaFullCor; //2023.01.11 update to tof_betahmc? XXX
															if (sVar[irvar] == "TofRichBetaDiff") dRichVar = (ev->tof_betahmc-dBetaFullCor)/dBetaFullCor;
															else if (sVar[irvar] == "BetaDiffRec") dRichVar = ring->getBetaConsistency();
														}
														
														//--
														histn = Form("%s_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut);
														histn += sRichBetaCor;
														GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRichVar, ww[ir]);
														
														//--
														#ifdef RICHVARCHECKRESO2D
														//2d
														//histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut);
														//if (dRig>dRigCut[ibeta]) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, dRichVar, ww[ir]);
														//--store for rigidity cut in all 3 range
														for (int irigcut=0; irigcut<3; irigcut++)
														{
															histn = Form("%svsrBeta_beta%d_r%d_q%d_rig%d_evcut%d_above%.1fGV", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut, dRigCut[irigcut]);
															histn += sRichBetaCor;
															if (dRig>dRigCut[irigcut]) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(1./dBetaFullCor, dRichVar, ww[ir]);
															
															//--some special 2d plots //2022.09.09
															if (irvar==3) //charge consistence vs rich Q
															{
																histn = Form("%svsRichQ_beta%d_r%d_q%d_rig%d_evcut%d_above%.1fGV", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut, dRigCut[irigcut]);
																histn += sRichBetaCor;
																if (dRig>dRigCut[irigcut]) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(sqrt(ev->rich_q[0]), dRichVar, ww[ir]);
															}
															else if (irvar==4) //TofRichBetaDiff vs TofBeta and vs RichBeta
															{
																for (int itofrich=0; itofrich<2; itofrich++)
																{
																	string sTofRich = (itofrich==0)?"TOF":"RICH";
																	histn = Form("%svs%sbeta_beta%d_r%d_q%d_rig%d_evcut%d_above%.1fGV", sVar[irvar].c_str(), sTofRich.c_str(), ibeta, ir, *it, irig, ievcut, dRigCut[irigcut]);
																	histn += sRichBetaCor;
																	//if (dRig>dRigCut[irigcut]) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill((itofrich==0)?ev->tof_betah:dBetaFullCor, dRichVar, ww[ir]); //2023.01.11: update to tof_betahmc? XXX
																	if (dRig>dRigCut[irigcut]) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill((itofrich==0)?ev->tof_betahmc:dBetaFullCor, dRichVar, ww[ir]); //2023.01.27
																} //itofrich
															}
														}
														
														//--some special 2d plots //2022.09.09
														if (irvar==3) //charge consistence vs rich Q
														{
															histn = Form("%svsRichQ_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), ibeta, ir, *it, irig, ievcut);
															histn += sRichBetaCor;
															GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(sqrt(ev->rich_q[0]), dRichVar, ww[ir]);
														}
														else if (irvar==4) //TofRichBetaDiff vs TofBeta and vs RichBeta
														{
															for (int itofrich=0; itofrich<2; itofrich++)
															{
																string sTofRich = (itofrich==0)?"TOF":"RICH";
																histn = Form("%svs%sbeta_beta%d_r%d_q%d_rig%d_evcut%d", sVar[irvar].c_str(), sTofRich.c_str(), ibeta, ir, *it, irig, ievcut);
																histn += sRichBetaCor;
																//GetHist2d(hmanQ[iQVer], histn.c_str())->Fill((itofrich==0)?ev->tof_betah:dBetaFullCor, dRichVar, ww[ir]); //2023.01.11: update to tof_betahmc? XXX
																GetHist2d(hmanQ[iQVer], histn.c_str())->Fill((itofrich==0)?ev->tof_betahmc:dBetaFullCor, dRichVar, ww[ir]); //2023.01.27
															} //itofrich
														}
														#endif //RICHVARCHECKRESO2D
													} //irvar
												} //ievcut
											} //irichcor
										} //ibeta
									}
								} //iQVer
							} //*it
						}
						#endif //RICHVARCHECKRESO
						
						//--check the rich tile position using no. event vs hit pos //2022.09.17
						#ifdef CHECKRICHGEO
						if (ev->Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir] && ir==0) //only store for Inner+Max L1Q to reduce storage and speed up
						{
							//--use new charge selection function
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								//if (irig==1) continue;
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
									if (ev->Select_Charge(*it, ir, iQVer))
									{
										#ifdef INNERUNBIAEDL1Q
										bool bUnbiasedL1XY=true;
										bool bUnbiasedL1Q=true;
										if (ir==0)
										{
											bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
											bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
											
											if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
										}
										#endif //INNERUNBIAEDL1Q
										
										for (int ibeta=1; ibeta<3; ibeta++)
										{
											if (ibeta>0)
											{
												//apply Rich geometry cut
												if ((ibeta==1 && !ev->rich_NaF) || (ibeta==2 && ev->rich_NaF)) continue;
												if (!ev->SelectRich(*it)) continue;
											}
											
											double dBeta;
											//if (ibeta==0) dBeta = ev->tof_betah;
											if (ibeta==0) dBeta = ev->tof_betahmc; //2023.01.11
											else dBeta = ev->rich_beta[0];
											double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
											double dMass = CalMass(*it, dBeta, dRig);
											for (int ievcut=0; ievcut<1; ievcut++)
											{
												bool bEvCut=true;
												if (ievcut==1) bEvCut = ev->ntrack==1 || ev->Select_Tk2nd();
												if (!bEvCut) continue;
												
												for (int irigcut=-1; irigcut<3; irigcut++)
												{
													if (irigcut!=-1)
													{
														if (dRig<=dRigCut[irigcut]) break;
													}
													
													string sRigCut = (irigcut==-1)?"":Form("_above%.1fGV", dRigCut[irigcut]);
													
													//--emission position
													histn = Form("EmisPos_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
													histn += sRigCut;
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->rich_pos[0], ev->rich_pos[1], ww[ir]);
													
													//--check the portion of reflected hits and relation with beta
													for (int ivar=0; ivar<3; ivar++) //ivar=0: 1/beta vs no. reflected photon, =1: 1/beta vs no. reflected photon/no. used photon, =2: 1/beta vs no. reflected photon/no. photon on ring
													{
														string sRefl;
														if (ivar==0) sRefl= "noRefl";
														else if (ivar==1) sRefl = "noRefltonoUsed";
														else if (ivar==2) sRefl = "noRefltonoOnring";
														double dRefl = ev->rich_usedm;
														if (ivar==1) dRefl /= (double)ev->rich_used;
														else if (ivar==2) dRefl /= (double)ev->rich_npe[0];
														histn = Form("rbetavs%s_beta%d_r%d_q%d_rig%d_evcut%d", sRefl.c_str(), ibeta, ir, *it, irig, ievcut);
														histn += sRigCut;
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dRefl, 1./dBeta, ww[ir]);
													} //ivar
												} //irigcut
												
												//--check the RichMCClusterR for hits in ring
												#ifdef CHECKMCCLUSTER
												//if (ev->rich_NaF)
												{
													//AMSEventR *amsev = ev->GetAMSEventR(true, true, true, sRawDir);
													AMSEventR *amsev = ev->GetAMSEventR(true, 1, false, sRawDir);
													//AMSEventR *amsev = ev->GetAMSEventR(true, 2, true, sRawDir); //2022.12.07
													
													const int NRichMCClusterR = amsev->nRichMCCluster();
													RichRingR *ring = amsev->pRichRing(ev->irich);
													ring = ring->ReBuild();
													if (ring!=NULL)
													{
														TVector3 v3RingIP(ring->getTrackEmissionPoint());
														double dDRRatio = ring->getReflectedHits()/(double)ring->getUsedHits();
														if (ring->getReflectedHits()==0) dDRRatio=-0.005;
														
														double dBetaDR[2][3] = {{0.}}; //[direct or refl][all, correct or wrongly rec]
														double dNpeDR[2][3] = {{0.}}; //[direct or refl][all, correct or wrongly rec]
														
														for (int ihit=0; ihit<ring->getUsedHits(); ihit++)
														{
															//--index of corresponding RichHitR
															int irichhit = ring->iRichHit(ihit);
															RichHitR *richhit = amsev->pRichHit(irichhit);
															int iPMT=richhit->Channel/16;
															
															//--index of corresponding raw beta container
															int irawhit=-1;
															for (int irawh=0; irawh<ring->fHitBeta.size(); irawh++)
															{
																if (ring->HitBeta(irawh)==irichhit)
																{
																      irawhit=irawh;
																      break;
																}
															} //irawh
															
															//--
															//string sRefl = (ring->BetaHit(ihit)<0)?"Reflected":"Direct";
															int idr = ring->UsedBeta(irawhit);
															if (idr>1) {cout << "****** warning: idr=" << idr << endl; continue;}
															string sRefl = (idr==0)?"Direct":"Reflected";
															TVector3 v3HitCoo(richhit->Coo);
															//if (ring->BetaHit(ihit)<0) cout << "ihit=" << ihit << " betahit=" << ring->BetaHit(ihit) << " " << sRefl << endl;
															//cout << " ****** ihit=" << ihit << " betahit=" << ring->BetaHit(ihit) << " HitBeta=" << ring->HitBeta(ihit) << " UsedBeta=" << ring->UsedBeta(ihit) << " " << sRefl << endl;
															//--search for the corresponding RichMCCluserR
															int drn=3;
															double dAngle = 0.;
															double dDist[3] = {0.};
															RichMCClusterR *richmccl = NULL;
															double dXOPMT=-100., dYOPMT=-100.;
															
															for (int irichmccl=0; irichmccl<NRichMCClusterR; irichmccl++)
															{
																richmccl = amsev->pRichMCCluster(irichmccl);
																if (richmccl->fRichHit==irichhit)
																{
																	TVector3 v3OCoo(richmccl->Origin);
																	TVector3 v3ODir(richmccl->Direction);
																	TVector3 v3OToHitDir = v3HitCoo-v3OCoo;
																	v3OToHitDir*=1./v3OToHitDir.Mag();
																	TVector3 v3OToPMT = v3OCoo;
																	v3OToPMT = v3OCoo + fabs((v3HitCoo[2] - v3OCoo[2])/v3ODir[2])*v3ODir;
																	//--distance to hit in PMT plane
																	TVector3 v3PMTDiff = v3OToPMT-v3HitCoo;
																	for (int ixy=0; ixy<2; ixy++) dDist[ixy] = v3PMTDiff[ixy];
																	dDist[2] = v3PMTDiff.Mag();
																	dXOPMT=v3OToPMT[0], dYOPMT=v3OToPMT[1];
																	
																	//--figure out the situation from RichMCCluster
																	if (richmccl->Id==-666) drn=2;
																	//else drn=0; //figure out whether it's direct or reflected
																	//a look on the distribution on the angle difference, it seems that within 0.2 the reconstruction might recognize the hit correctly as direct and reflected //2022.11.24
																	//--the lower size of the mirror is 134 cm in diameter, if the extrapolation point from origin is outside the mirror, than the hit must had been reflected
																	else drn=(sqrt(pow(v3OToPMT[0],2)+pow(v3OToPMT[1],2))>134/2.)?1:0;
																	
																	//--calculate the angle between RichMCCluster origin dir and origin point to hit
																	dAngle = acos(v3ODir*v3OToHitDir);
																	//--
																	//if (ring->BetaHit(ihit)<0)
																	//if (richmccl->GparentID!=1)
																	/*if (idr==0)
																	{
																		cout << "found matched RichMCCluster: ihit=" << ihit << " irichmccl=" << irichmccl << endl;
																		cout << "GparentID=" << richmccl->GparentID << endl;
																		cout << "GtrkID=" << richmccl->GtrkID << " Id=" << richmccl->Id << endl;
																		cout << "Status=" << richmccl->Status<< endl;
																		cout << "Ring impact point: (" << v3RingIP[0] << "," << v3RingIP[1] << "," << v3RingIP[2] << ")" << endl; //XXX
																		cout << "Origin point: (" << v3OCoo[0] << "," << v3OCoo[1] << "," << v3OCoo[2] << ")" << endl; //XXX
																		cout << "Origin dir: (" << v3ODir[0] << "," << v3ODir[1] << "," << v3ODir[2] << ")" << endl; //XXX
																		cout << "Hit pos: (" << v3HitCoo[0] << "," << v3HitCoo[1] << "," << v3HitCoo[2] << ")" << endl; //XXX
																		cout << "Origin to hit: (" << v3OToHitDir[0] << "," << v3OToHitDir[1] << "," << v3OToHitDir[2] << ")" << endl; //XXX
																		cout << "Extrapolation from origin to PMT plane: (" << v3OToPMT[0] << "," << v3OToPMT[1] << "," << v3OToPMT[2] << ")" << endl; //XXX
																		cout << "angle=" << dAngle << endl; //XXX
																	}*/
																	break;
																}
															} //irichmccl
															
															//cout << "drn=" << drn << endl; //XXX
															for (int idrn=0; idrn<4; idrn++) //idrn=0: direct, =1: reflected, =2: noise, =3: no corresponding RichMCClusterR found
															{
																for (int igr=0; igr<2; igr++)
																{
																	histn = Form("Eff_drn%d", idrn);
																	histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
																	GetTEff(hmanQ[iQVer], histn.c_str())->Fill((idrn==drn), ((igr==0)?(ev->mmom/ev->mch):dRig));
																} //igr
															} //idrn
															if (drn>1) continue;
															
															//--store the angle between RichMCCluster origin to hit and emission point to hit, for direct and reflected separately
															//--Distance vs drratio
															for (int ixy=0; ixy<3; ixy++)
															{
																histn = Form("DistvsDRRatio_dr%d_xy%d", idr, ixy);
																histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																histn += Form("_drn%d", drn);
																GetHist(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, dDist[ixy]);
															} //ixy
															
															//--Angle
															histn = Form("Angle_dr%d", idr);
															histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
															histn += Form("_drn%d", drn);
															//cout << histn << " " << GetHist(hmanQ[iQVer], histn.c_str()) << endl; //XXX
															GetHist(hmanQ[iQVer], histn.c_str())->Fill(dAngle);
															
															//--Angle vs drratio
															histn = Form("AnglevsDRRatio_dr%d", idr);
															histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
															histn += Form("_drn%d", drn);
															GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, dAngle);
															
															//--Angle vs rig
															int igr=0;
															histn = Form("AnglevsRig_dr%d", idr);
															histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
															histn += Form("_drn%d", drn);
															GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, dAngle);
															
															/*//--position in PMT from Origin point for hits in given no. pmt //2022.11.30
															if (iPMT>=200) continue;
															histn = Form("PMTPosFormOrigin_pmt%d_dr%d", iPMT, idr);
															histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
															GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dXOPMT, dYOPMT);*/
															
															//--store the average beta for direct and reflected, correct and wrongly reconstructed
															int irec = (drn==idr)?0:1;
															
															//--all hits
															dBetaDR[idr][0] += richhit->Npe*ring->BetaHit(ihit);
															dNpeDR[idr][0] += richhit->Npe;
															
															//--corret or wrongly reconstructed accordingly
															dBetaDR[idr][irec+1] += richhit->Npe*ring->BetaHit(ihit);
															dNpeDR[idr][irec+1] += richhit->Npe;
														} //ihit
														
														for (int idr=0; idr<2; idr++)
														{
															for (int irec=-1; irec<2; irec++)
															{
																if (dNpeDR[idr][irec+1]<=0) continue;
																
																//cout << "ievent=" << ievent << " idr=" << idr << " irec=" << irec << " beta=" << dBetaDR[idr][irec] << " " << dNpeDR[idr][irec] << endl; //XXX
																dBetaDR[idr][irec+1] /= dNpeDR[idr][irec+1];
																dBetaDR[idr][irec+1] *= ring->betaCorrection();
																 
																for (int irigcut=0; irigcut<3; irigcut++)
																{
																	if (dRig<dRigCutOnlyDR[irigcut]) continue;
																	//1/beta
																	histn = Form("rBetavsDRRatio_dr%d", idr);
																	if (irec>=0) histn += Form("_rec%d", irec);
																	histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																	histn += Form("_above%.1fGV", dRigCutOnlyDR[irigcut]);
																	GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, 1./dBetaDR[idr][irec+1]);
																	
																	//1-beta, i.e. true - rec
																	double dBetaBiasR = (ibeta==1)?2e-2:2e-3;
																	histn = Form("BetaBiasvsDRRatio_dr%d", idr);
																	if (irec>=0) histn += Form("_rec%d", irec);
																	histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																	histn += Form("_above%.1fGV", dRigCutOnlyDR[irigcut]);
																	GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, dBetaDR[idr][irec+1]-1);
																} //irigcut
															} //irec
														} //idr
													}
												}
												#endif //CHECKMCCLUSTER
											} //ievcut
										} //ibeta
										
										//--beta using only direct or reflected hits //2022.11.09
										#ifdef ONLYDRHITBETA
										for (int ibeta=1; ibeta<3; ibeta++)
										{
											if (irig!=0) break;
											if ((ibeta==1 && !ev->rich_NaF) || (ibeta==2 && ev->rich_NaF)) continue;
											//for (int icluster=0; icluster<2; icluster++) //icluster=0: build ring only using direct or reflected hits, =1: build ring use both direct and reflected hits but use average only direct or reflected hits
											for (int icluster=1; icluster<2; icluster++) //icluster=0: build ring only using direct or reflected hits, =1: build ring use both direct and reflected hits but use average only direct or reflected hits
											{
												double dDRRatio = ev->rich_usedm/(double)ev->rich_used;
												if (ev->rich_usedm==0) dDRRatio=-0.005;
												
												double dRichCor = 1.;
												#ifdef APPLYCOR
												Rad rad = (!ev->rich_NaF)?AGL:NAF;
												dRichCor = ModelManager::corrected_beta(1, rad, ev->run, ev->mch, ev->rich_pos[0], ev->rich_pos[1], ev->rich_theta, ev->rich_phi, ev->rich_usedm, ev->rich_used, int(!ev->isreal));
												#endif //APPLYCOR
												
												for (int ievcut=0; ievcut<1; ievcut++)
												{
													bool bEvCut=true;
													if (ievcut==1) bEvCut = ev->ntrack==1 || ev->Select_Tk2nd();
													if (!bEvCut) continue;
													
													int ibetaver=0;
													//int ibetaver=1;
													for (int icor=0; icor<NRichCor; icor++) //2022.11.22
													{
														string sRichCor = (icor==0)?"":"_WithCor";
														double dCor=(icor==0)?1.:dRichCor;
														
														//--
														double dBeta;
														//if (ibeta==0) dBeta = ev->tof_betah;
														if (ibeta==0) dBeta = ev->tof_betahmc; //2023.01.11
														else dBeta = ev->rich_beta[ibetaver];
														dBeta *= dCor; //2022.11.22
														double dEknRec = (1./sqrt(1-pow(dBeta,2))-1)*MPROTON;
														double dMassRec = CalMass(*it, dBeta, dRig);
														
														//--
														double dEknGen = RigToEkn(mrig, ev->mch, MCMass);
														
														//--
														double dBetaDR[3];
														double dEknRecDR[3];
														//for (int idr=0; idr<2; idr++)
														for (int idr=-1; idr<2; idr++)
														{
															if (ibeta>0)
															{
																bool bRichCut=false;
																if (idr==-1) bRichCut = ev->SelectRich(*it);
																else bRichCut = ev->SelectRichDR(icluster, idr, *it);
																//apply Rich cut accordingly
																if (bRichCut)
																{
																	dBetaDR[idr+1] = (idr>=0)?ev->GetBetaDR(icluster, idr, ibetaver):dBeta;
																	dBetaDR[idr+1] *= dCor; //2022.11.22
																	dEknRecDR[idr+1] = (1./sqrt(1-pow(dBetaDR[idr+1],2))-1)*MPROTON;
																}
																else
																{
																	dBetaDR[idr+1] = -9999;
																	dEknRecDR[idr+1] = -9999;
																}
															}
														} //idr
														//-- //XXX
														/*if (dBetaDR[1]>0 && fabs(dBetaDR[1]-1)>0.005 && dDRRatio==-0.005 && dRig>=200)
														{
															cout << "ievent=" << ievent << " run=" << ev->run << " event=" << ev->event << endl;
															cout << "dBetaDR[0]=" << dBetaDR[0] << endl;
															cout << "bSel=" << (ev->SelectRich(*it)?"TRUE":"FALSE") << endl;
															cout << "dBetaDR[1]=" << dBetaDR[1] << endl;
															cout << "bSel=" << (ev->SelectRichDR(icluster, 0, *it)?"TRUE":"FALSE") << endl;
															cout << "rich_usedm=" << ev->rich_usedm << " rich_used=" << ev->rich_used << endl; //XXX
														}*/
														
														double dBetaDRDiff[3];
														for (int idr=0; idr<2; idr++) dBetaDRDiff[idr] = (dBetaDR[idr]>0)?(dBetaDR[idr]-dBeta):-1;
														if (dBetaDR[0]>0 && dBetaDR[1]>0) dBetaDRDiff[2] = dBetaDR[0] - dBetaDR[1];
														else dBetaDRDiff[2] = -9999.;
														
														//--
														double dEVar[2][2][2]; //[idr (only diff for rec Ekn)][ire][igr]
														//for (int idr=0; idr<2; idr++)
														for (int idr=-1; idr<2; idr++)
														{
															for (int ire=0; ire<2; ire++)
															{
																for (int igr=0; igr<2; igr++)
																{
																	if (ire==0) dEVar[idr+1][ire][igr] = (igr==0)?mrig:dRig;
																	else if (ire==1) dEVar[idr+1][ire][igr] = (igr==0)?dEknGen:dEknRecDR[idr+1];
																} //igr
															} //ire
														} //idr
														
														int NRichHit[3] = {ev->rich_used, ev->rich_used-ev->rich_usedm, ev->rich_usedm};
														//for (int idr=0; idr<2; idr++) //idr=0: direct, idr=1: reflected
														for (int idr=-1; idr<2; idr++) //idr=-1: all hits (standard), 0: direct, idr=1: reflected
														{
															if (dBetaDR[idr+1]<0) continue;
															/*//for (int ire=0; ire<2; ire++) //ire=0: vs rig, ire=1: vs Ekn
															for (int ire=0; ire<1; ire++) //ire=0: vs rig, ire=1: vs Ekn
															{
																//for (int igr=0; igr<2; igr++)
																for (int igr=1; igr<2; igr++)
																{
																	for (int idrratio=-1; idrratio<NDRRatioRegion; idrratio++)
																	{
																		if (icluster==0 && idrratio>=0) break;
																		if (idrratio>=0)
																		{
																			//if (dDRRatio<=0.+0.25*idrratio || dDRRatio>0.+0.25*(idrratio+1)) continue;
																			if (dDRRatio<=dDRRatioStep*idrratio || dDRRatio>dDRRatioStep*(idrratio+1)) continue;
																		}
																		else if (idrratio==-1 && dDRRatio!=0) continue;
																		string sDrRatio = (idrratio<0)?"":Form("_DrRatioBin%d", idrratio);
																		//--beta vs energy var
																		histn = Form("Betavs%s", ((ire==0)?"Rig":"Ekn"));
																		histn += Form("_cl%d", icluster);
																		if (idr>=0) histn += Form("_dr%d", idr);
																		histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
																		histn += sDrRatio;
																		GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dEVar[idr+1][ire][igr], dBetaDR[idr+1]);
																		
																		//--1/Beta vs energy var
																		histn = Form("rBetavs%s", ((ire==0)?"Rig":"Ekn"));
																		histn += Form("_cl%d", icluster);
																		if (idr>=0) histn += Form("_dr%d", idr);
																		histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
																		histn += sDrRatio;
																		GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dEVar[idr+1][ire][igr], 1./dBetaDR[idr+1]);
																		
																		//--beta diff vs energy var
																		if (idr<0) continue;
																		for (int idiff=0; idiff<3; idiff++) //idiff=0: diff between direct-only and total, =1: diff between reflected-only and total, =2: diff between direct-only and reflected-only
																		{
																			if (dBetaDRDiff[idiff]<=-9999) continue;
																			histn = Form("BetaDiff%dvs%s", idiff, ((ire==0)?"Rig":"Ekn"));
																			histn += Form("_cl%d_dr%d", icluster, idr);
																			histn += Form("_gr%d_beta%d_r%d_q%d_rig%d_evcut%d", igr, ibeta, ir, *it, irig, ievcut);
																			histn += sDrRatio;
																			GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dEVar[idr+1][ire][igr], dBetaDRDiff[idiff]);
																		}
																	} //idrratio
																} //igr
															} //ire*/
															
															//--
															//if (dDRRatio<=0) dDRRatio=-0.005;
															for (int irigcut=0; irigcut<3; irigcut++)
															//for (int irigcut=2; irigcut<3; irigcut++)
															{
																if (dRig<dRigCutOnlyDR[irigcut]) continue;
																//if (!ev->rich_NaF && dDRRatio>0.5)
																//{
																//	cout << "ievent=" << ievent << " idr=" << idr << " dDRRatio=" << dDRRatio << " dBeta=" << dBetaDR[0] << " " << dBeta << " dBetaOnly=" << dBetaDR[idr+1] << endl; //XXX
																//}
																string sRigCutOnlyDR = Form("_RigCut%.0fGV", dRigCutOnlyDR[irigcut]);
																//--beta vs DRRatio
																histn = Form("BetavsDRRatio");
																histn += Form("_cl%d", icluster);
																if (idr>=0) histn += Form("_dr%d", idr);
																histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																histn += sRigCutOnlyDR;
																histn += sRichCor; //2022.11.22
																GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, dBetaDR[idr+1]);
																
																//--1/Beta vs energy var
																histn = Form("rBetavsDRRatio");
																histn += Form("_cl%d", icluster);
																if (idr>=0) histn += Form("_dr%d", idr);
																histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																histn += sRigCutOnlyDR;
																histn += sRichCor; //2022.11.22
																GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, 1./dBetaDR[idr+1]);
																
																//--no. direct and reflected vs DRRatio
																if (icor==0)
																{
																	histn = Form("NHitvsDRRatio");
																	histn += Form("_cl%d", icluster);
																	if (idr>=0) histn += Form("_dr%d", idr);
																	histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																	histn += sRigCutOnlyDR;
																	GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, NRichHit[idr+1]);
																}
																
																//--beta diff vs energy var
																if (idr<0) continue;
																for (int idiff=0; idiff<3; idiff++) //idiff=0: diff between direct-only and total, =1: diff between reflected-only and total, =2: diff between direct-only and reflected-only
																{
																	if (dBetaDRDiff[idiff]<=-9999) continue;
																	histn = Form("BetaDiff%dvsDRRatio", idiff);
																	histn += Form("_cl%d_dr%d", icluster, idr);
																	histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																	histn += sRigCutOnlyDR;
																	histn += sRichCor; //2022.11.22
																	GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dDRRatio, dBetaDRDiff[idiff]);
																	
																	//--2d distribution
																	int iDRRatioBin=-1;
																	if (dDRRatio<=0) iDRRatioBin=0;
																	else iDRRatioBin = (dDRRatio<0.5)?1:2;
																	if (idiff<2) histn = Form("BetaDR%dvsBeta", idiff);
																	else histn = Form("BetaDR0vsBetaDR1");
																	histn += Form("_cl%d_dr%d", icluster, idr);
																	histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																	histn += sRigCutOnlyDR;
																	histn += sRichCor; //2022.11.22
																	histn += Form("_DRRatioBin%d", iDRRatioBin);
																	if (idiff<2) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(1./dBeta, 1./dBetaDR[idiff+1]);
																	else if (idiff==2) GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(1./dBetaDR[1+1], 1./dBetaDR[0+1]);
																} //idiff
															} //irigcut
															
															//--no. direct and reflected vs beta
															for (int igr=0; igr<2; igr++)
															{
																histn = Form("NHitvsBeta");
																histn += Form("_cl%d", icluster);
																if (idr>=0) histn += Form("_dr%d", idr);
																histn += Form("_gr%d", igr);
																histn += Form("_beta%d_r%d_q%d_rig%d_evcut%d", ibeta, ir, *it, irig, ievcut);
																double dBetaGR=0;
																if (igr==1) dBetaGR = dBetaDR[idr+1];
																else dBetaGR = RigToBeta(ev->mch, MCMass, mrig);
																GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dBetaGR, NRichHit[idr+1]);
															} //igr
														} //idr
													} //icor
												} //ievcut
											} //icluster
										} //ibeta
										#endif //ONLYDRHITBETA
									}
								} //iQVer
							} //*it
						}
						#endif //CHECKRICHGEO
						#endif //ISOTOPE
						
						//2021.09.28
						#ifdef TRDINTERCON
						if (ir!=0) continue;
						if (ev->Select_Rig(1, icffv, ir, dGeoSafety, iRigAlgo) && ev->Select_TrTrack(ir,111,iRigAlgo) && bTkgeom[ir])
						{
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								#ifndef EFFSEC
								if (!ev->isreal && *it!=ev->mch) continue;
								#endif //EFFSEC
								
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
									
									if (ev->Select_Charge(*it, ir, iQVer))
									{
										#ifdef INNERUNBIAEDL1Q
										bool bUnbiasedL1XY=true;
										bool bUnbiasedL1Q=true;
										if (ir==0)
										{
											bUnbiasedL1XY = ev->Select_UnbiasedL1XY(iQVer);
											bUnbiasedL1Q = ev->Select_UnbiasedL1Q(*it, iQVer);
											
											//if (!(bUnbiasedL1XY && bUnbiasedL1Q)) continue;
											if (!bUnbiasedL1XY) continue;
										}
										#endif //INNERUNBIAEDL1Q
										
										double dUnbiasedL1Q = ev->GetUBExtQTrkCor(0, 2, iQVer);
										
										for (int ievcut=0; ievcut<2; ievcut++) //0: extra cut, 1: ntrack=1 || no 2nd cut
										{
											bool bEvCut=true;
											if (ievcut==1) bEvCut = ev->ntrack==1 || ev->Select_Tk2nd();
											if (!bEvCut) continue;
											
											for (int itrdcut=0; itrdcut<2; itrdcut++)
											{
												string sEvCut = (ievcut>0)?Form("_evcut%d", ievcut):"";
												sEvCut += Form("_TrdCut%d", itrdcut);
												
												//--trd cut
												if (itrdcut>0)
												{
													bool bTrdcut=(ev->trd_statk>0) && ev->trd_qnhk[0]>15 && ev->trd_qk[0]>0;
													if (!bTrdcut) continue;
													
												}
												
												//--event counts that without unbiased L1Q cut
												histn = Form("rig_r%d_q%d_TrdInterCon%s%s", ir, *it, sRig.c_str(), sEvCut.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(dRig, ww[ir]);
												
												histn = Form("rig_r%d_q%d_TrdInterCon%s%s_noRigCor", ir, *it, sRig.c_str(), sEvCut.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir,iRigAlgo,2,false), ww[ir]);
												
												//--
												for (int itrdqpart=0; itrdqpart<3; itrdqpart++) //itrdqpart=0: total, =1: upper 10 layer, =2: lower 10 layer
												{
													for (int itrdq=0; itrdq<3; itrdq++) //itrdq=0: dE/dX + delta ray, =1: dE/dX, =2: delta ray
													{
														if (itrdq>0 && itrdqpart>0) continue;
														//int ivar=itrdqpart+itrdq;
														//if (itrdqpart>0) ivar = itrdqpart+2;
														int ivar=itrdqpart;
														if (itrdq>0) ivar = itrdq+2;
														
														//--L1Q vs Trd Q
														histn = Form("L1Qvstrdq%d_%s_r%d_q%d_TrdInterCon%s%s", itrdq, sTrdqPart[itrdqpart].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->trd_qk[ivar], dUnbiasedL1Q, ww[ir]);
														
														//--Qrms vs L1Q
														if (itrdqpart!=0) continue;
														histn = Form("trdq%drms_%s_vsL1Q_r%d_q%d_TrdInterCon%s%s", itrdq, sTrdqPart[itrdqpart].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dUnbiasedL1Q, ev->trd_qrmsk[itrdq], ww[ir]);
														
														//--upper 10 Q lower 10 Q asymmetry
														if (itrdq!=0) continue;
														histn = Form("trdq%d_%s_%s_Asy_vsL1Q_r%d_q%d_TrdInterCon%s%s", itrdq, sTrdqPart[1].c_str(), sTrdqPart[2].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
														double dTrdULQAsy=(ev->trd_qk[1]-ev->trd_qk[2])/(ev->trd_qk[1]+ev->trd_qk[2]);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dUnbiasedL1Q, dTrdULQAsy, ww[ir]);
														
														//--dE/dX only vs delta ray only asymmetry
														histn = Form("trdq%d_trdq%d_%s_Asy_vsL1Q_r%d_q%d_TrdInterCon%s%s", 1, 2, sTrdqPart[0].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
														double dTrdQAsy=(ev->trd_qk[3]-ev->trd_qk[4])/(ev->trd_qk[3]+ev->trd_qk[4]);
														GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dUnbiasedL1Q, dTrdQAsy, ww[ir]);
													} //itrdq
													
													//--Nhit
													histn = Form("Nhit_%s_vsL1Q_r%d_q%d_TrdInterCon%s%s", sTrdqPart[itrdqpart].c_str(), ir, *it, sRig.c_str(), sEvCut.c_str());
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dUnbiasedL1Q, ev->trd_qnhk[itrdqpart], ww[ir]);
												} //itrdqpart
												
												//--ipch
												histn = Form("ipchvsL1Q_r%d_q%d_TrdInterCon%s%s", ir, *it, sRig.c_str(), sEvCut.c_str());
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(dUnbiasedL1Q, ev->trd_ipch, ww[ir]);
												
											} //itrdcut
										} //ievcut
									} //charge cut
								} //iQVer
							} //*it
						} //pattern cut
						#endif //TRDINTERCON
					} //irig
					
					//under cutoff study
					#ifdef UNDERCUTOFF
					//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
					//if (bTrack[ir] && bTkgeom[ir])
					if (bTrack[ir][0] && bTkgeom[ir]) //use choutko //2023.03.10
					{
						int ThetaMRegion=0;
						if (fabs(ev->thetam)<0.2) ThetaMRegion=0;
						else if (fabs(ev->thetam)<=0.8) ThetaMRegion=1;
						else if (fabs(ev->thetam)<0.9) ThetaMRegion=2;
						else ThetaMRegion=3;
						//--use new charge selection function
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							//if (!ev->isreal && *it!=ev->mch) continue;
							//2020.10.25
							#ifndef EFFSEC
							if (!ev->isreal && *it!=ev->mch) continue;
							#endif //EFFSEC
							
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
							
								if (ev->Select_Charge(*it, ir, iQVer))
								{
									//no cutoff
									histn = Form("rig_r%d_q%d_NoCutoff", ir, *it);
									GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
									
									
									histn = Form("rig_r%d_q%d_NoCutoff_thetamRegion%d", ir, *it, ThetaMRegion);
									GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
									
									histn = Form("thetamvsrig_r%d_q%d_NoCutoff", ir, *it);
									GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ev->thetam, ww[ir]);
									//above cutoff
									//if (bRig[ir])
									if (bRig[ir][0]) //2023.03.10
									{
										histn = Form("rig_r%d_q%d_AboveCutoff_thetamRegion%d", ir, *it, ThetaMRegion);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
										
										histn = Form("thetamvsrig_r%d_q%d_AboveCutoff", ir, *it);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ev->thetam, ww[ir]);
									}
								}
							} //iQVer
						} //it
					} //pattern cut
					#endif //UNDERCUTOFF
					
					#ifdef CUTOFFSAFETYVAR
					if (ev->isreal && bTrack[ir] && bTkgeom[ir])
					{
						//--use new charge selection function
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
							
								if (ev->Select_Charge(*it, ir, iQVer))
								{
									for (int iSafety=0; iSafety<5; iSafety++)
									{
										double dSafety=1.+0.1*iSafety;
										histn = Form("rig_r%d_q%d_CutoffSafety%.1f", ir, *it, dSafety);
										if (ev->Select_Rig(1, icffv, ir, dSafety)) GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
									} //iSafety
								}
							} //iQVer
						} //it
					} //pattern cut
					#endif //CUTOFFSAFETYVAR
					
					//--chis check for GBL //2022.09.22
					#ifdef GBLCHISCHECK
					for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
					{
						if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
						SelGBLChisCheck(hmanQ[iQVer], *ev, ir, iQVer);
					} //iQVer
					#endif //GBLCHISCHECK
					
					//--apply RigDiffCut and check the change of rigidity resolution //2023.02.04
					#ifdef RIGDIFFCUT
					//--
					for (int ialgo=1; ialgo<NAlgoCheck; ialgo++) //now only do for GBL //2023.02.05
					{
						if (ir!=1) break; //now noly do for L1Inner //2023.02.05
						//--preselection, fiducial volume and chis: same as event count selection
						bool bTrackSel = bTkgeom[ir] && ev->Select_TrTrack(ir,111,ialgo);
						if (!(bTrackSel)) continue;
						
						double weight = (ir==3)?ww[0]:ww[ir]; //2019.08.22: use the l1 + l19 focus combine + shape weight
						
						//--use new charge selection function
						for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
						{
							if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
							SelRigDiffCut(hmanQ[iQVer], *ev, outfile, ir, iQVer, weight, ialgo);
						} //iQVer
					} //ialgo
					#endif //RIGDIFFCUT
				} //ir
				#endif //RIG_NEWTRQ
				
				#ifdef L8L9SUR
				//------L1Q and L2Q distribution for L9Q>Z-1.5 efficiency 
				for (int ir=1; ir<2; ir++)
				{
					int iReferSpan=0;
					
					//------L1Q and L2Q, L1Inner rigidity
					iReferSpan=ir;
					if (ev->Select_Rig(1, icffv, iReferSpan) && ev->Select_TrTrack(iReferSpan) && bTkgeom[iReferSpan]) //cutoff && TrTrack && fiducial volume
					{
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								//if (!ev->isreal && *it!=ev->mch) continue;
								//2020.10.25
								#ifndef EFFSEC
								if (!ev->isreal && *it!=ev->mch) continue;
								#endif //EFFSEC
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
								for (int il=1; il<3; il++)
								{
							
									string sLQ = Form("L%dQ", il);
									double dLQ = ev->GetTkLQNew(il-1, 2, iQVer);
									
									//----XY hit
									bool bLQXY = (ev->HasTkLHitXY(il-1)==2); //for L1Q, already applied in Select_TrTrack
						
									//----charge cut
									bool bQCut = true;
									//--L1Q cut (L2Q)
									// if (il==2) bQCut = bQCut && (fabs(ev->GetSubDetQ(SelEvent::L1Q,iQVer)-*it)<0.5);
									if (il==2) bQCut = bQCut && (fabs(ev->GetSubDetQ(QDis::L1Q,iQVer)-*it)<0.5);
							
									//--uTOF 2 layers
									for (int itofl=0; itofl<2; itofl++) bQCut = bQCut && (fabs(ev->GetTofLQ(itofl)-*it)<0.5);
						
									//--InnerQ && InnerQrms (L1Q) or InnInnerQ (L2Q)
									// if (il==1) bQCut = bQCut && (fabs(ev->GetSubDetQ(SelEvent::InnerQ,iQVer)-*it)<0.5) && (ev->GetTkInQrmsNew(2, iQVer)<1);
									if (il==1) bQCut = bQCut && (fabs(ev->GetSubDetQ(QDis::InnerQ,iQVer)-*it)<0.5) && (ev->GetTkInQrmsNew(2, iQVer)<1);
									else if (il==2) bQCut = bQCut && (fabs(ev->CalTrunCharge2(2,iQVer)-*it)<0.5);
						
									if (bLQXY && bQCut)
									{
										//--without charge status cut
										histn = Form("%svsRig_r%d_q%d_L8L9Sur_noQStatCut", sLQ.c_str(), ir, *it);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dLQ, ww[ir]);
										
										//--with charge status cut
										if (ev->Select_TkLQStatus(il-1))
										{
											histn = Form("%svsRig_r%d_q%d_L8L9Sur_withQStatCut", sLQ.c_str(), ir, *it);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dLQ, ww[ir]);
										}
									}
								} //il
							} //iQVer
						} //*it
					}
					
					//------unbiased L1Q, Inner rigidity
					iReferSpan=ir-1;
					if (ev->Select_Rig(1, icffv, iReferSpan) && ev->Select_TrTrack(iReferSpan) && bTkgeom[ir]) //cutoff && TrTrack && fiducial volume
					{
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								//if (!ev->isreal && *it!=ev->mch) continue;
								//2020.10.25
								#ifndef EFFSEC
								if (!ev->isreal && *it!=ev->mch) continue;
								#endif //EFFSEC
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
								string sLQ = "unbiasedL1Q";
								double dLQ = ev->GetUBExtQTrkCor(0, 2, iQVer);
						
								//----XY hit
								bool bLQXY = ev->IsUBXYExtQTrkCor(0,iQVer);
						
								//----charge cut
								bool bQCut = true;
								//--uTOF 2 layers
								for (int itofl=0; itofl<2; itofl++) bQCut = bQCut && (fabs(ev->GetTofLQ(itofl)-*it)<0.5);
						
								//--InnerQ
								// bQCut = bQCut && (fabs(ev->GetSubDetQ(SelEvent::InnerQ,iQVer)-*it)<0.5) && (ev->GetTkInQrmsNew(2, iQVer)<1);
								bQCut = bQCut && (fabs(ev->GetSubDetQ(QDis::InnerQ,iQVer)-*it)<0.5) && (ev->GetTkInQrmsNew(2, iQVer)<1);
						
								if (bLQXY && bQCut)
								{
									//--without charge status cut
									histn = Form("%svsRig_r%d_q%d_L8L9Sur_noQStatCut", sLQ.c_str(), ir, *it);
									GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dLQ, ww[ir]);
							
									//--with charge status cut
									if ((ev->GetUBExtQStatusTrkCor(0) & 0x10013D)==0)
									{
										histn = Form("%svsRig_r%d_q%d_L8L9Sur_withQStatCut", sLQ.c_str(), ir, *it);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dLQ, ww[ir]);
									}
								}
							} //iQVer
						} //*it
					}
				} //ir
				#endif //L8L9SUR
			} //non-pattern selection: Good particle & Beta
		} //pre-selection
		
		#ifdef EVENTCOUNTING
		if (bTrig && bRun && bRTI && bRTIdL1L9)
		{
			#ifdef EVENTCOUNTING
			iNEV[0][0]++;
			#endif //EVENTCOUNTING
		
			//non-pattern selection: Good particle & Beta
			if (bPart && bBeta)
			{
				#ifdef EVENTCOUNTING
				iNEV[0][1]++;
				#endif //EVENTCOUNTING
				for (int ir=0; ir<3; ir++)
				{
					//if (bRig[ir] && bTrack[ir] && bTkgeom[ir])
					if (bRig[ir][0] && bTrack[ir][0] && bTkgeom[ir]) //2023.03.10
					{
						#ifdef EVENTCOUNTING
						iNEV[ir][2]++;
						#endif //EVENTCOUNTING
						
						if (ev->Select_Charge(26, ir, QCutVer))
						{
							#ifdef EVENTCOUNTING
							iNEV[ir][3]++;
							#endif //EVENTCOUNTING
						}
					}
				}
			}
		}
		#endif //EVENTCOUNTING
		
		//event selection test
		#ifdef RIG_NEWTRQ
		#ifdef QYEVCOUNT
		for (int ir=iSpanL; ir<iSpanU; ir++)
		{
			
			for (it=viQSel.begin(); it!=viQSel.end(); it++)
			{
				//--
				//if (!ev->isreal && *it!=ev->mch) continue;
				//2020.10.25
				#ifndef EFFSEC
				if (!ev->isreal && *it!=ev->mch) continue;
				#endif //EFFSEC
				for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
				{
					if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
					
					bool bQYEvCut = ev->CalLQAsy(0,iQVer)<0.2;
					if (ir==2) bQYEvCut = bQYEvCut && ev->CalLQAsy(8,iQVer)<0.2;
					
					//if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && ev->Select_Rig(1, 3, ir, dGeoSafety) && ev->Select_TkGeom(ir, 0) && bTrack[ir] && ev->Select_Charge(*it, ir, iQVer) && bQYEvCut)
					//{
					//	histn = Form("rig_r%d_q%d_%.1fsigma_QYEvCount", ir, *it, 2.0);
					//	GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
					//}
					//2020.11.14
					if (bTrig && bRun && bRTI && bRTIdL1L9 && bPart && bBeta && ev->Select_Rig(1, 3, ir, dGeoSafety) && ev->Select_TkGeom(ir, 0) && bTrack[ir] && ev->Select_Charge(*it, ir, iQVer))
					{
						histn = Form("rig_r%d_q%d_%.1fsigma_QYEvCount0", ir, *it, 2.0);
						GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
						
						if (bQYEvCut)
						{
							histn = Form("rig_r%d_q%d_%.1fsigma_QYEvCount1", ir, *it, 2.0);
							GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(ir), ww[ir]);
						}
					}
				} //iQVer
			} //*it
		} //ir
		#endif //QYEVCOUNT
		#endif //RIG_NEWTRQ
		
		//--L1L2SUR
		//2020.03.16: move the L1L2SUR outside since the MC will have different trigger selection
		//if (bTrig && bRun && bRTI && bRTIdL1L9)
		if (bRun && bRTI && bRTIdL1L9)
		{
			//non-pattern selection: Good particle & Beta
			if (bPart && bBeta)
			{
				//2019.10.14
				#ifdef L1L2SUR
				for (int ir=1; ir<2; ir++)
				{
					//apply trigger selection for data only, for MC will applied later
					if (ev->isreal && !bTrig) break;
					
					//--
					int iReferSpan;
					#ifdef L1L2SURNEW
					iReferSpan=(ir==1)?0:3; //using Inner rigidity for L1Inner,InnerL9 fo FullSpan
					//if (ev->Select_TrTrack(iReferSpan) && bTkgeom[ir] && (ev->tk_exqls[0]&0x10013D)==0) //L1BZ L1Inner geometry non-charge selection, unbiased L1 charge status cut
					if (ev->Select_Rig(1, icffv, iReferSpan) && ev->Select_TrTrack(iReferSpan) && bTkgeom[ir] && (ev->tk_exqls[0]&0x10013D)==0) //add geomagnetic cutoff
					#else
					iReferSpan=ir;
					if (bTrack[ir] && bTkgeom[ir] && (ev->tk_qls[0] & 0x10013D)==0) //normal L1Inner geometry non-charge selection and L1 charge status cut, take out geomagnetic cutoff
					#endif //L1L2SURNEW
					{
						//--use new charge selection function
						for (it=viQSel.begin(); it!=viQSel.end(); it++)
						{
							//if (!ev->isreal && *it!=ev->mch) continue;
							//2020.10.25
							#ifndef EFFSEC
							if (!ev->isreal && *it!=ev->mch) continue;
							#endif //EFFSEC
							for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
							{
								if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
								//------before L1Q selection
								double dL1Q, dInnerQ;
								#ifdef L1L2SURNEW
								dL1Q=ev->GetUBExtQTrkCor(0, 2, iQVer);
								if (!(ev->GetUBExtQTrkCor(0, 0, iQVer)>0 && ev->GetUBExtQTrkCor(0, 1, iQVer)>0)) continue; //L1XY
								#else
								dL1Q=ev->GetTkLQNew(0, 2, iQVer);
								#endif //L1L2SURNEW
								
								double dUTofQ = ev->GetSubDetQ(1, iQVer);
								const int iTrigPeriod=ev->TriggerPeriod();
								
								//recover two trigger setting for MC
								for (int iperiod=0; iperiod<2; iperiod++)
								{
									if  (ev->isreal)
									{
										if (iperiod!=iTrigPeriod) continue;
									}
									else
									{
										//2020.03.16: need to orginzie into a function //XXX
										int physbpatt=(ev->isreal)?ev->physbpatt2:ev->physbpatt1;
										int jmembpatt=(ev->isreal)?ev->jmembpatt2:ev->jmembpatt1;
										int nacc=0;
										for (int ibit=0; ibit<8; ibit++) if ((ev->antipatt&(1<<ibit))>0) nacc++;
										
										int inacc=(iperiod==0)?5:8;
										int PhysBPattRebuild=physbpatt;
										if ((jmembpatt&0x200L)!=0 && nacc<inacc) PhysBPattRebuild=PhysBPattRebuild|(1<<2);
										else PhysBPattRebuild=PhysBPattRebuild&(~(1<<2));
										bool bTrigRebuild = ((PhysBPattRebuild&0x3EL)!=0);
										if (!bTrigRebuild) continue;
									}
									
									for (int itofqcut=0; itofqcut<4; itofqcut++)
									{
										//apply UTofQ upper cut
										bool bTofQCut = (itofqcut==0) || dUTofQ<(*it-(4-itofqcut)*0.5);
										if (!bTofQCut) continue;
										
										//do charge status cut together with other non-charge cut
										//----event count after non-charge and L1Q selection
										histn = Form("rig_r%d_q%d_L1L2Sur_noL1Qcut_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), ww[ir]);
										
										//----L1Q distribution vs Rig
										histn = Form("L1QvsRig_r%d_q%d_L1L2Sur_noL1Qcut_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dL1Q, ww[ir]);
										
										//----UTofQ distribution vs Rig
										histn = Form("UTofQvsRig_r%d_q%d_L1L2Sur_noL1Qcut_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUTofQ, ww[ir]);
										
										//----for MC, also store in Generated Rigidity
										if (!ev->isreal)
										{
											//----event count after non-charge and L1Q selection
											histn = Form("rig_r%d_q%d_L1L2Sur_noL1Qcut_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, ww[ir]);
										
											//----InnerQ distribution vs Rig
											histn = Form("L1QvsRig_r%d_q%d_L1L2Sur_noL1Qcut_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, dL1Q, ww[ir]);
										}
									
										/*//------tight L1Q cut
										if (fabs(dL1Q-*it)<0.5)
										{
											//----event count after L1Q selection
											histn = Form("rig_r%d_q%d_L1L2Sur_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), ww[ir]);
										
											//----InnerQ distribution vs Rig
											histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), ev->GetTkInQNew(2, iQVer), ww[ir]);
											
											//----UTofQ distribution vs Rig
											histn = Form("UTofQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUTofQ, ww[ir]);
											
											//----for MC, also store in Generated Rigidity
											if (!ev->isreal)
											{
												//----event count after non-charge and L1Q selection
												histn = Form("rig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, ww[ir]);
										
												//----InnerQ distribution vs Rig
												histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d", ir, *it, iperiod, itofqcut);
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, ev->GetTkInQNew(2, iQVer), ww[ir]);
											}
										}*/
										
										//-- //2020.11.02
										const int iNTighten=(itofqcut==0)?3:0;
										for (int isel=-1; isel<iNTighten; isel++)
										{
											if (itofqcut>0 && isel>=0) continue;
											
											string sTighten=(isel==-1)?"":Form("_tighten%d", isel);
											
											bool bL1QSel=true;
											//if (isel==-1) bL1QSel=(fabs(dL1Q-*it)<0.5);
											if (isel==-1) bL1QSel = (dL1Q>*it-0.5) && (dL1Q<*it+0.5);
											else if (isel==0) bL1QSel = (dL1Q>*it-0.25) && (dL1Q<*it+0.5);
											else if (isel==1) bL1QSel = (dL1Q>*it-0.5) && (dL1Q<*it+0.25);
											else if (isel==2) bL1QSel = (dL1Q>*it-0.25) && (dL1Q<*it+0.25);
											
											if (bL1QSel)
											{
												//----event count after L1Q selection
												histn = Form("rig_r%d_q%d_L1L2Sur_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), ww[ir]);
											
												//----InnerQ distribution vs Rig
												histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), ev->GetTkInQNew(2, iQVer), ww[ir]);
												
												//----UTofQ distribution vs Rig
												histn = Form("UTofQvsRig_r%d_q%d_L1L2Sur_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
												GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUTofQ, ww[ir]);
												
												//----for MC, also store in Generated Rigidity
												if (!ev->isreal)
												{
													//----event count after non-charge and L1Q selection
													histn = Form("rig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
													GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, ww[ir]);
											
													//----InnerQ distribution vs Rig
													histn = Form("InnerQvsRig_r%d_q%d_L1L2Sur_GenRig_period%d_tofqcut%d%s", ir, *it, iperiod, itofqcut, sTighten.c_str());
													GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->mmom/ev->mch, ev->GetTkInQNew(2, iQVer), ww[ir]);
												}
											}
										} //isel
									} //itofqcut
								} //iperiod
							} //iQVer
						} //*it
					}
				} //ir
				#endif //L1L2SUR
				
				#ifdef L8L9SUR
				#ifdef L8L9SURECALMIP
				for (int ir=2; ir<3; ir++)
				{
					//set the MC reweight correctely
					//double dWeigh=ww[ir]; //l1+l19 combine & shape
					double dWeigh=ww0[ir]; //shape //2020.12.14: as now only have l19 focus
					
					//apply trigger selection for data only, for MC will applied later
					if (ev->isreal && !bTrig) break;
					
					//--common selection for all sample: L1Inner + L9 fiducial volume + ECal fiducial volume
					int iReferSpan = (ir==2)?1:-1;
					const int iTrigPeriod=ev->TriggerPeriod();
					if (ev->Select_Rig(1, icffv, iReferSpan) && ev->Select_TrTrack(iReferSpan) && bTkgeom[ir] && ev->IsPassEcal2(0,7,0))
					{
						for (int iperiod=0; iperiod<3; iperiod++)
						{
							if (iperiod==2) //all periods
							{
								if (!bTrig) continue;
							}
							else
							{
								if  (ev->isreal)
								{
									if (iperiod!=iTrigPeriod) continue;
								}
								else
								{
									int inacc=(iperiod==0)?5:8;
									int PhysBPattRebuild = ev->RebuildTrig(inacc);
									bool bTrigRebuild = ((PhysBPattRebuild&0x3EL)!=0);
									if (!bTrigRebuild) continue;
								}
							}
							
							for (it=viQSel.begin(); it!=viQSel.end(); it++)
							{
								for (int iQVer=QVeri; iQVer<nQVer; iQVer++)
								{
									//if (!ev->isreal && *it!=ev->mch) continue;
									//2020.10.25
									#ifndef EFFSEC
									if (!ev->isreal && *it!=ev->mch) continue;
									#endif //EFFSEC
									if (iQVer%3<ChargeVersion || iQVer%3>ChargeVersion+nQVerStore-1) continue;
								
									//--L1Inner charge cut: L1Q + Upper ToFQ + InnerQ
									if (!ev->Select_Charge(*it, iReferSpan, iQVer)) continue;
								
									//--
									double dUnbiasedL9Q=ev->GetUBExtQTrkCor(1, 2, iQVer);
								
									//------L1Inner no MIP sample (survival prob * L9 efficieny, use for survival prob)
									//--sample: L1Inner + L9 fiducial + Ecal fiducial
									histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8Sam", ir, *it, iperiod);
									GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
								
									//--unbiased L9XY
									if (ev->IsUBXYExtQTrkCor(1,iQVer))
									{
										histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8Sam_L9XY", ir, *it, iperiod);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
									
										histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8Sam_L9XY", ir, *it, iperiod);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUnbiasedL9Q, dWeigh);
									
										//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
										if (((ev->GetUBExtQStatusTrkCor(1) & 0x10013D)==0) && dUnbiasedL9Q>*it-1.5)
										{
											histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8Sam_L9Sel", ir, *it, iperiod);
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
										}
									}
								
									//------requring ECal max edep cell not in the 2 edge cell
									if (ev->ECalMaxEdepSel())
									{
										//--sample: L1Inner + L9 fiducial + Ecal fiducial
										histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut", ir, *it, iperiod);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
								
										//--unbiased L9XY
										if (ev->IsUBXYExtQTrkCor(1,iQVer))
										{
											histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut_L9XY", ir, *it, iperiod);
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
									
											histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut_L9XY", ir, *it, iperiod);
											GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUnbiasedL9Q, dWeigh);
									
											//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
											if (((ev->GetUBExtQStatusTrkCor(1) & 0x10013D)==0) && dUnbiasedL9Q>*it-1.5)
											{
												histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8SamWithMaxEdepCut_L9Sel", ir, *it, iperiod);
												GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
											}
										}
									}
								
									//------L1Inner MIP sample (purity * L9 efficiency, use for L9 efficiency)
									if (!ev->ECalMIPsSel()) continue;
								
									//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5)
									histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamNoMaxEdepCut", ir, *it, iperiod);
									GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
								
									//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
									if (ev->IsUBXYExtQTrkCor(1,iQVer) && ((ev->GetUBExtQStatusTrkCor(1) & 0x10013D)==0) && dUnbiasedL9Q>*it-1.5)
									{
										histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamNoMaxEdepCut_L9Sel", ir, *it, iperiod);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
									}
								
									//------L1Inner MIP sample with L9 selection (use for MIP sample purity)
									for (int isam=0; isam<2; isam++) //isam=0: MIP+unbiasd L9XY, isam=1: MIP+unbiasd L9XY+unbiased L9Q status
									{
										//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5) + unbiased L9hit preselection (different for isam=0 and =1)
										bool bL8MIPTight = ev->IsUBXYExtQTrkCor(1,iQVer);
										if (isam==1) bL8MIPTight = bL8MIPTight && ((ev->GetUBExtQStatusTrkCor(1) & 0x10013D)==0);
									
										if (!bL8MIPTight) continue;
									
										histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%dNoMaxEdepCut", ir, *it, iperiod, isam);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
									
										histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%dNoMaxEdepCut", ir, *it, iperiod, isam);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUnbiasedL9Q, dWeigh);
									
										//--selection: sample + unbiased L9Q>Z-1.5
										if (dUnbiasedL9Q>*it-1.5)
										{
											histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%dNoMaxEdepCut_L9Sel", ir, *it, iperiod, isam);
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
										}
									} //isam
								
									//------requring ECal max edep cell not in the 2 edge cell
									if (!ev->ECalMaxEdepSel()) continue;
								
									//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5)
									histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSam", ir, *it, iperiod);
									GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
								
									//--selection: sample + unbiased L9XY + L9Q>Z-1.5 + L9Q status
									if (ev->IsUBXYExtQTrkCor(1,iQVer) && ((ev->GetUBExtQStatusTrkCor(1) & 0x10013D)==0) && dUnbiasedL9Q>*it-1.5)
									{
										histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSam_L9Sel", ir, *it, iperiod);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
									}
								
									//------L1Inner MIP sample with L9 selection (use for MIP sample purity)
									for (int isam=0; isam<2; isam++) //isam=0: MIP+unbiasd L9XY, isam=1: MIP+unbiasd L9XY+unbiased L9Q status
									{
										//--sample: L1Inner + L9 fiducial + Ecal fiducial + ECal MIPs (S3/S5) + unbiased L9hit preselection (different for isam=0 and =1)
										bool bL8MIPTight = ev->IsUBXYExtQTrkCor(1,iQVer);
										if (isam==1) bL8MIPTight = bL8MIPTight && ((ev->GetUBExtQStatusTrkCor(1) & 0x10013D)==0);
									
										if (!bL8MIPTight) continue;
									
										histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%d", ir, *it, iperiod, isam);
										GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
									
										histn = Form("UnbiasedL9QvsRig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%d", ir, *it, iperiod, isam);
										GetHist2d(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dUnbiasedL9Q, dWeigh);
									
										//--selection: sample + unbiased L9Q>Z-1.5
										if (dUnbiasedL9Q>*it-1.5)
										{
											histn = Form("rig_r%d_q%d_period%d_L8L9Sur_L8MIPSamTight%d_L9Sel", ir, *it, iperiod, isam);
											GetHist(hmanQ[iQVer], histn.c_str())->Fill(ev->GetRigidity(iReferSpan), dWeigh);
										}
									} //isam
								} //iQVer
							} //it
						} //iperiod
					}
				} //ir
				#endif //L8L9SURECALMIP
				#endif //L8L9SUR
			} //non-pattern selection: Good particle & Beta
		} //pre-selection
	} //ievent
	
	//--store runlist
	#ifdef USEQTOOL
	if (ev->isreal) WriteList(sRunlogDir.c_str(), ev->qrm->qprocessfile); //only do for ISS //2023.03.09
	#endif //USEQTOOL
	
	#ifdef RIG
	#if defined EXPOTCAL && defined AFSRTI
	//--expo-time & MRC
	ch.GetEntry(0);
	if (ev->isreal)
	{
		RunT[0] = ev->time[0];
		ch.GetEntry(nrun2);
		RunT[1] = ev->time[0];
		cout << "starting time: " << RunT[0] << endl;
		cout << "Ending time: " << RunT[1] << endl;
		if (RunT[0]!=0 && RunT[1]!=0) {
			for (int ich=1; ich<=30; ich++) {
				#ifdef BZSELECTION
				if (std::find(viQSel.begin(), viQSel.end(), ich) == viQSel.end() && (ich<SCharge || ich>MCharge)) continue;
				#else
				if (ich<SCharge || ich>MCharge) continue;
				#endif //BZSELECTION
				#ifdef REFERENCE //check only 2, 6, 8 within 
				if (ich>=SCharge && ich<=MCharge) if (ich!=2 && ich!=6 && ich!=8) continue;
				#endif //REFERENCE
			
				for (int id=0; id<4; id++)
				{
					//exposure time
					histn = Form("expot_q%d_d%d", ich, id);
					ev->ProcessTime(RunT, static_cast<TH1F*>(GetHist(hman1, histn.c_str())), id);
			
					//mcut
					histn = Form("mcut_q%d_d%d", ich, id);
					ev->MCutoffDis(RunT, static_cast<TH1F*>(GetHist(hman1, histn.c_str())), id);
				} //id
			} //ich
		}
	}
	#endif //EXPOTCAL && AFSRTI
	#endif //RIG
	
	//2021.09.06 //2021.09.17: move outside RIGRESO
	#ifdef RIGRESO
	//#ifdef MASSKEYSPDF
	//ClearBetaCor(); //defined in include/ReadBetaCor.h
	//#endif // MASSKEYSPDF
	#endif //RIGRESO
	#ifdef MASSKEYSPDF
	ClearBetaCor(); //defined in include/ReadBetaCor.h
	#endif // MASSKEYSPDF
	
	//////--saving
	outfile.Write(); //save all histograms no matter filled or not
	outfile.Close();
	
	#ifdef EVENTCOUNTING
	cout << "selected 0: " << iNEV[0][0] << endl;
	cout << "selected 1: " << iNEV[0][1] << endl;
	for (int ir=1; ir<3; ir++)
	{
		cout << sSpan[ir] << endl;
		cout << "selected 2: " << iNEV[ir][2] << endl;
		cout << "selected 3: " << iNEV[ir][3] << endl;
	} //ir
	
	//--print the charge cut range //2020.09.06
	for (it=viQSel.begin(); it!=viQSel.end(); it++)
	{
		cout << "Q=" << *it << " " << sElementFull[*it-1] << endl;
		for (int iqdis=0; iqdis<6; iqdis++)
		{
			if (iqdis==3) continue;
			cout << sQDis[iqdis] << ": [Z-" << fabs(*it-ev->GetQSelRange(0,*it,iqdis,4)) << ",Z+" << fabs(*it-ev->GetQSelRange(1,*it,iqdis,4)) << "] = [" << ev->GetQSelRange(0,*it,iqdis,4) << "," << ev->GetQSelRange(1,*it,iqdis,4) << "]" << endl;
		} //iqdis
	} //*it
	#endif //EVENTCOUNTING
	
	#ifdef BZSELECTION
	viQSel.clear();
	#endif //BZSELECTION
	
	//2023.02.05
	#ifdef RIGDIFFCUT
	//ResetSigmavsRig();
	#endif //RIGDIFFCUT
	
	//2023.03.26
	#if defined TRACKEFFN || defined RIGESTCHECK
	ClearRigEst();
	#endif //TRACKEFFN || RIGESTCHECK
	
	delete [] hmanQ; //2023.02.08

	cout << "Outfile: " << endl;
	cout << outf_name.c_str() << endl;
	
	//2023.03.01
	#if defined LXPLUS || defined MLXPLUS
	cout << "Is TChain changed? " << ((bChainChanged)?"True":"False") << endl;
	#endif //LXPLUS || MLXPLUS
	
	end = time(NULL);
	cout << "Finish at " << ctime(&end) << endl;
	cout << "total processing time:" << end - start << endl;
	
	//2020.12.12
	#ifdef TRIGGER_BR_PERIOD
	cout << " ************************************************************************************************************ " << endl;
	cout << " ****** using the trigger + Bartel rotation run 1457048996 as separation, remeber to change back after ****** " << endl;
	cout << " ************************************************************************************************************ " << endl;
	#endif //TRIGGER_BR_PERIOD
	
	return 1;
} //analysis

