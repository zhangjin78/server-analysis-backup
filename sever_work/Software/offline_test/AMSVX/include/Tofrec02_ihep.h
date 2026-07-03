//  $Id: Tofrec02_ihep.h,v 1.48 2023/05/04 09:30:28 qyan Exp $

//Author Qi Yan 2012/June/09 10:03 qyan@cern.ch  /*IHEP TOF version*/
#ifndef __AMSTOFREC02_IHEP__
#define __AMSTOFREC02_IHEP__
#include "root.h"
#include "TObject.h"
#include "TF1.h"
#ifndef __ROOTSHAREDLIBRARY__
//////////////////////////////////////////////////////////////////////////
class TOF2RawSide;
class AMSCharge;
class AMSTrTrack;
class AMSTRDTrack;
class AMSEcalShow;
#endif
class TofBetaPar;

//!  TOF IHEP Reconstruction+Calibration(BetaH version) Both Support Production and Root Mode Refit
/*!
 *  Root Mode ReFit User Need One function:  TofRecH::ReBuild() 
 *  Example of How to Use:  vdev/example/Tof_BetaH.C (AMSsoft)
 * \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class TofRecH{

/// Data part
protected:
/// light velocity
  static const double cvel;//c vel(cm/ns)
/// Fit Beta Par
  TofBetaPar betapar;
/// Process Event
  AMSEventR *ev;
/// Real or MC
  int realdata; 
/// TofRawSideR vector for TofClusterH build
   vector<TofRawSideR> tfraws;
#ifndef __ROOTSHAREDLIBRARY__
   vector<TOF2RawSide*> tf2raws;
#endif
protected:
/// TofClusterH 4layer vector  pointer
  vector<TofClusterHR*>tofclh[4];
/// TofClusterH UpPair+DownPair BetaH Raw
  vector<pair <TofClusterHR*,TofClusterHR* > >tofclp[2];
/// TofClusterH UpPair+DownPair BetaH Select Candidate
  vector<pair <TofClusterHR*,TofClusterHR* > >tofclc[2];

/// Track vector pointer
  vector<TrTrackR*> track;
#ifndef __ROOTSHAREDLIBRARY__
  vector<AMSTrTrack*> amstrack;  
#endif

/// TrdTrack vector
  vector<TrdTrackR>trdtrack;
#ifndef __ROOTSHAREDLIBRARY__
  vector<AMSTRDTrack*>amstrdtrack;
#endif

/// EcalShower vector
  vector<EcalShowerR> ecalshow;
#ifndef __ROOTSHAREDLIBRARY__
  vector<AMSEcalShower*>amsecalshow;
#endif

/// Charge Signal Type
public:
  enum  QSignalType{
    /// Convert to MIP Unit
    kQ2=0x1,
    /// Q2(ADC) Non Linear Corr
    kLinearCor=0x2,
    /// Attnueation Cor
    kAttCor=  0x4,
    /// Path Length dz/dl Cor
    kThetaCor=0x8,
    /// Birk Saturation Corr
    kBirkCor= 0x10,
    /// Invert Birk Saturation Corr
    kVBirkCor=0x20,
    /// Convert From Q2 To MeV
    kQ2MeV =  0x40, 
    /// Convert From MeV to Q2
    kMeVQ2 =  0x80,
    /// Conver From Q2 to Q
    kQ2Q =    0x100,
    /// Beta Cor
    kBetaCor= 0x200,
    /// Rigidity Cor
    kRigidityCor=0x400,
    /// Attnueation ReCor
    kReAttCor=0x800,
    /// Dynode-Anode Weight
    kDAWeight=0x1000,
    /// Beta+Rig Version=1 Cor
    kBetaRigV1Cor=0x2000,
    /// Beta+Rig Version=2 Cor
    kBetaRigV2Cor=0x4000,
 };

/** @name Sum ReBuild Part
 * @{
 */
public:
/// default construction
  TofRecH();
  virtual ~TofRecH(){}
  static  TofRecH *Head;
#ifndef __PPC64
#pragma omp threadprivate (Head)  
#endif
  static TofRecH *GetHead();
/// TDV Build Control Key
  static bool BuildKey;
/// Build Option //0-normal Build, 1-Exclude Track-Association Build, 2-Exclude Track+Trd Association, 3-Build Continue After Track Association Finding, 11110 Trk-Trd-Ecal-TOF-Other, 31110 Trk(Track Find Continue)-Trd-Ecal-TOF-Other, -1 Calibration Build
  static int  BuildOpt;
/// Root Mode Refit //if want to rebuild BetaH in Root Mode, Only need this function//(charge is dummy)
  static int  ReBuild(int charge=1);
/// Build TofClusterH function
  static int  BuildTofClusterH();
/// Build BetaH function (0 mask output)
  static int  BuildBetaH(int verse=0);
/// Do Advanced TrTrack Fitting by using correct TkInnerZ 
  static int  DoAdvancedTrackFit();
/// Do Primary TrTrack Tag
/// \param direction   prefered direction to filter primary track, 1 (-1) means higher priority for downward (upward) going track, 0 means the same priority for downward and upward tracks 
  static int  DoPrimaryTrackTag(int direction=1);
/// TDV initial function
  int  Init();
/// Build TofClusterH function
  int  BuildTofClusterHI();
/// Build BetaH function (0 mask output)
  int  BuildBetaHI(int verse=0);
/// Clear TofClusterH build vector
  int  ClearBuildTofClH();
/// Clear BetaH build vector
  int  ClearBuildBetaH();
/**@}*/

/** @name TofClusterH Build Part
 * @{
 */
public:
/// Finding LT from TofRawSide 
  int  TofSideRec(TofRawSideR *ptr,number &adca, integer &nadcd,number adcd[],number &sdtm,uinteger &sstatus,
                        vector<number>&ltdcw, vector<number>&htdcw, vector<number>&shtdcw,int urealdata,int unbiase=0);
/// Time build from TofRawSide
   int  TimeCooRec(int idsoft,number sdtm[],number adca[],number tms[2],number &tm,number &etm,number &lcoo,number &elcoo,uinteger &status);
/// Energy build Module
   int  EdepRec(int idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],number lcoo,number q2pa[],number q2pd[][TOF2GC::PMTSMX],number &edepa,number &edepd,uinteger sstatus[2]);
/// Energy build Module-1
   int  EdepRecR(int ilay,int ibar,geant adca[],geant adcd[][TOF2GC::PMTSMX],number lcoo,geant q2pa[],geant q2pd[][TOF2GC::PMTSMX],geant &edepa,geant &edepd);
/// Refind LT if many LT not Associate with HT
   int  LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status,vector<number> ltdcw[ ]);  

/// Get QSignal with different Correction
  number GetQSignal(int idsoft,int isanode,int optc,number signal,number lcoo=0,number cosz=1,number beta=0.98,number rig=0);
/// Get MC tune Q2
  number GetMCtuneQ2(number rawq2,int id);
/// Adding Gain to Convert To Q*Q (Proton Mip Unit)
  number CoverToQ2(int idsoft,int isanode,number adc);
/// Q2(ADC) Non-Linear Correction
  number NonLinearCor(int idsoft,int isanode,number q2);
/// Scintillator Attunation Correction To Counter Central
  number SciAttCor(int idsoft,number lpos,number q2);
/// Scintillator Re-Attunation Correction Using Carbon to ReTune
  number SciReAttCor(int idsoft,number lpos,number q2,int qopt=0);
/// Scintillator Birk Correction (opt=1 normal 0 invert)
  number BirkCor(int idsoft,number q2,int opt=1);
/// Beta Correction
  number BetaCor(int idsoft,number q2,number beta,number rig=0,int isanode=0,int ncor=0);
/// Beta Correction Factor For Charge(opt=0) or Cal Edep Beta(opt=1)
  number GetBetaCalCh(int idsoft,int opt,number beta,number q2,int charge,number rig=0,int isanode=0,int ncor=0);
/// Beta Q2-Correction Factor For ChargeIndex(opt=0) or Cal Edep Beta(opt=1)
  number GetBetaCalI(int idsoft,int opt,number beta,number q2norm,int chindex,number rig=0,int isanode=0,int ncor=0);
/// Rigidity Correction
  number RigCor(int idsoft,number q2,number rig,int isanode=0,int ncor=0);
/// Rigidity Correction Factor For Charge
  number GetRigCalCh(int idsoft,int charge,number rig,int isanode=0,int ncor=0);
/// Rigidity Correction Factor For ChargeIndex
  number GetRigCalI(int idsoft,int chindex,number rig,int isanode=0,int ncor=0); 
/// Sum Anode Signal To Counter Signal
  number SumSignalA(int idsoft,number signal[],int useweight=1);
/// Sum Dynode Signal To Counter Signal
  number SumSignalD(int idsoft,number signal[][TOFCSN::NPMTM],int useweight=1,bool minpmcut=1);
/// Anode Dynode Weight
  number GetWeightDA(int idsoft,int isanode,number q2); 
/// Get Proton Anode Mip Adc for local lpos
  number GetProMipAdc(int idsoft,number lpos);
/// Birk Function
  static TF1 *BirkFun;
#pragma omp threadprivate (BirkFun)
/// Birk Function
  static TF1 *GetBirkFun(int idsoft);
/// BetaCor Function
  static TF1 *BetaFun;
#pragma omp threadprivate (BetaFun)
/// BetaCor Function
  static TF1 *GetBetaFun();


/**@}*/

/** @name BetaH Build Part
 * @{
 */
public:
/// Find TofClusterH for ilay with Track
#if defined (_PGTRACK_) || defined (__ROOTSHAREDLIBRARY__)
  int  BetaFindTOFCl(TrTrackR *ptrack,   int ilay,TofClusterHR** tfhit,number &tklen,number &tklcoo,number &tkcosz,number cres[2],int &pattern,int opt=0);
#else
  int  BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,TofClusterHR** tfhit,number &tklen,number &tklcoo,number &tkcosz,number cres[2],int &pattern,int opt=0);
#endif
/// Search TofClusterH
  int  TOFClSel(TofClusterHR* tfhit[4],TofClusterHR* tfhitb[4],TofClusterHR* tfhits[4],number tklcoo[4],number tklcoob[4],number tkcosz[4],number tkcoszb[4]);

/// Recover Time information if One Side lost Signal
  int  TRecover(TofClusterHR *tfhit[4],number tklcoo[4],int partten[4],int mode=0);//using hassid to recover other side
  int  TRecover(int idsoft,geant trlcoo,geant tms[2],geant &tm,geant &etm,uinteger &status,int hassid);
/// ReBuild Attenuation Correction From TkCoo 
  int EdepTkAtt(TofClusterHR *tfhit[4],number tklcoo[4],number tkcosz[4],TofBetaPar &par);
/// Coo Chi2 Fit function 
  int  BetaFitC(TofClusterHR *tfhit[4],number res[4][2],int partten[4],TofBetaPar &par,int mode);//
/// Beta Fit function
  int  BetaFitT(TofClusterHR *tfhit[4],number len[4],int partten[4],TofBetaPar &par,int mode=1,int verse=0);//mode same etime weight(0) or not(1)
  int  BetaFitT(number time[],number etime[],number len[],const int nhits,TofBetaPar &par,int mode=1,int verse=0);//mode same etime weight(0) or not(1)
/// Beta Check function
  int  BetaFitCheck(TofClusterHR *tfhit[4],number res[4][2],number lenr[4],int pattern[4],TofBetaPar &par,int mode);//if this is normal value
#ifndef __ROOTSHAREDLIBRARY__
  number BetaCorr(number zint,number z0,number part,uinteger &status);//to BetaC Vitaly
#endif
/// Mass Cal function
  int  MassRec(TofBetaPar &par,number rig=0,number charge=0,number evrig=0,int isubetac=0);//evrig=E(1/rig)

/// BetaH Self Track Build Part
/// Up And Down TOFCluster Make Pair
  int TOFClMakePair(int ilay0,int ilay1,int isdown);
/// Pair PreSelection
  int TOFPairPreSel(int ud,number coref[],AMSDir diref,number cutangle);
/// Find TOFCl Algorithem
  int TOFPairSel(int ud,TofClusterHR* tfhit[2]);
/// Ecal Search TOF Down Part
  int EcalSearchD(TofClusterHR* tfhitu[2], TofClusterHR* tfhitd[2],number cooshow[3]);  
/// TofTrack Fit 
  int TofTrackFit(TofClusterHR *tfhit[4],TofBetaPar &par,int attrefit=0,int verse=0);
/// Line Fit //y=ax+b
  int LineFit(int nhits,number x[],number y[],number ey[],number &a,number &b);
/// Up Down Self Match Candidate
  bool PairMatchUD(pair<TofClusterHR*,TofClusterHR*> upair,pair<TofClusterHR*,TofClusterHR*> dpair,number &edis);
/// Up Down Best Match Pair
  int  PairSearchUD(pair<TofClusterHR*,TofClusterHR*> sedpair,int sedud,int opt);//sed to search(opt=0 pair opt=1 counter)
/// Erase Hit From vector
  int  TOFClErase(TofClusterHR* tfhit[4]);
/**@}*/ 

/// Other function
public:
/// Sort TofRawSide accoding to BarId
  static bool SideCompare(const TofRawSideR& a,const TofRawSideR& b){return a.swid<b.swid;}
/// Sort TofRawSide Index 
template <class T>
  static bool IdCompare(const pair<integer,T> &a,const pair<integer,T> &b){return a.second<b.second;}
/// Sort TofClusterHR Pair-Index
  static bool PairCompare(const pair<TofClusterHR*,TofClusterHR*> &a,const pair<TofClusterHR*,TofClusterHR*> &b);
/// ParticleR ChargeR Build Link index to BetaH
  int  BetaHLink(TrTrackR* ptrack,TrdTrackR *trdtrack,EcalShowerR *ecalshow);
/// ParticleR ReBuild Link index to TofTrack BetaH
  int  BetaHReLink();
/// Do Advanced TrTrack Fitting by using correct TkInnerZ
  int  DoAdvancedTrackFitI();
/// Do Primary TrTrack Tag
/// \param direction   prefered direction to filter primary track, 1 (-1) means higher priority for downward (upward) going track, 0 means the same priority for downward and upward tracks 
  int  DoPrimaryTrackTagI(int direction=1);

/// friend access
  friend class TofClusterHR;
  friend class BetaHR; 
  friend class TofBetaPar; 
#ifdef __ROOTSHAREDLIBARY__
  ClassDef(TofRecH,10)
#endif      
};

/////////////////////////////////////////////////////////////////////////
#endif
