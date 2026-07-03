#ifndef __TrClusterR__
#define __TrClusterR__

#include "TkDBc.h"
#include "TkCoo.h"
#include "TrRawCluster.h"
#include "TrElem.h"
#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TrParDB.h" 
#include "TrLadPar.h" 
#include "TrGainDB.h"
#include "TrChargeLossDB.h"
#include "TrOccDB.h"
#include "TrEDepDB.h"
#include "TrMipDB.h"
#include "TrLinearDB.h"
#include "TrChargeYJ.h"

#include "edep.h"
#include "amsdbc.h"

#include "TSpline.h"

#include <cmath>
#include <vector>
#include <string>

#include "TrClusterChargeH.h"
#include "AMSPlane.h"
#include "TkGeomN.h"

/*!
 \class TrClusterR
 \brief A class to manage reconstructed cluster in AMS Tracker
 \ingroup tkrec

 TrClusterR, i.e. Tracker reconstructed cluster, is the core of the Tracker reconstruction. 
 This class gives accessors and memebers to retrieve all the reconstructed cluster 
 properties: signal (data members), calibration parameters (via TrCalDB), gains (via TrGainDB),
 charge corrections (via TrChargeLossDB) and coordinates (via TrLinearEtaDB, TkCoo or TkTrackN). 

 $Date: 2022/06/04 22:40:31 $

 $Revision: 1.79 $

*/


class TrClusterR :public TrElem{

 public:
  
  enum CorrectionOptions {
    /// No Correction Applied
    kNoCorr       =     0x0,       
    /// Signal Corr.: Cluster Asymmetry Correction (left/right)
    kAsym         =     0x1, 
    /// Signal Corr.: P-Strip Correction 
    kPStrip       =     0x2,
    /// Total Signal Corr.: Energy Loss Normalization at 300 um [cos(Theta)^-1]
    kAngle        =     0x4,
    /// Total Signal Corr.: Gain Correction
    kGain         =     0x8, 
    /// Total Signal Corr.: Charge Loss Correction 
    kLoss         =    0x10,
    /// Total Signal Corr.: Charge Loss Correction (alternative to kLoss)
    kLoss2        =    0x20, 
    /// Total Signal Corr.: Normalization to P-Side (probably not working, however not really needed)    
    kPN           =    0x40,
    /// Total Signal Corr.: Normalization to number of MIP
    kMIP          =    0x80,
    /// Total Signal Corr.: Multiply by 300 um MIP energy deposition (estimated to be 81 keV)
    kMeV          =   0x100,
    /// Total Signal Corr.: Beta correction 
    kBeta         =   0x200,
    /// Total Signal Corr.: Rigidity correction 
    kRigidity     =   0x400,
    /// Coordinate Corr.: Correct for the charge coupling (4%) 
    kCoupl        =  0x1000,
    /// Coordinate Corr.: Belau correction 
    kBelau        =  0x2000,
    /// Use old charge calibration 
    kOld          =  0x4000,
    /// Signal Corr.: New Cluster Asymmetry Correction (left/right)
    kAsymEta      =  0x8000,
    /// ADC->Q2 Correction: ADC to Q2(Z gain)
    kQ2Eta        =  0x10000,
    /// Charge calibration 2017 (for now used only for MC) 
    kTotSign2017  =  0x20000,
    /// Signal Corr.: Cluster asymmetry correction using TRMCFFKEY.Asymmetry
    kSimAsym      =  0x40000,  
    /// Total Signal Corr.: raw ADC to MIP scale for every VA 
    kSimSignal    =  0x80000,
    /// ADC Overflow Corr
    kOverflow     =  0x100000 
  };
  /// cluster status
  enum { ZSEED = 0x200, TASCLS = 0x400, kFlip = 0x800, VERTEX = 0x200000, TKCAND = 0x400000 };
 
 public:
  /// the cluster charge object
  TrClusterChargeH clustercharge;  //!
  
  /// TkLadder ID (layer *100 + slot)*side 
  short int    _tkid;
  /// First strip address (0-639 for p-side, 640-1023 for n-side)
  short int    _address;
  /// Number of strips
  short int    _nelem;
  /// Seed index in the cluster (without correction, 0<=_seedind<_nelem)
  short int    _seedind;
  /// ADC data array
  std::vector<float> _signal;
  /// tan(ThetaXZ) of the incoming track (used for optimization of the track fitting)
  float        _dxdz;
  /// tan(ThetaYZ) of the incoming track (used for optimization of the track fitting)
  float        _dydz;
  /// charge of the incoming track (used for optimization of the track fitting)
  float        _qtrk;
  /// Multiplicity (on p side should be 1 on n side it is ladder dependent)
  int8         _mult;   
  /// Cluster status 
  unsigned int Status;
 public:
  int mcZ;
 protected:
  
  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  /// Pointer to the parameters database
  static TrParDB* _trpardb;
  /// load the std::string sout with the info for a future output
  std::string _PrepareOutput(int full=0);

  /// Conversion between sqrt(ADC) and number of MIPs
  static TSpline3* sqrtadc_to_sqrtmip_spline[2];
  /// Conversion between "almost" MIPs and MIPs (refinement of the previous) 
  static TSpline3* sqrtref_to_sqrtmip_spline[2][9];
 
 public:
   
  /// Cluster asymmetry (strip cross talk)
  static float Asymmetry[2];
  /// Silicon intrinsic corrections
  static int DefaultCorrOpt;
  /// Corrections for coordinate calculation considering non-linearity on Y
  static int DefaultLinearityCorrOpt;
  /// ta Linearization
  static int LinearUsedStrips;
  /// Best set of options for coordinate determination
  static int DefaultBestResidualOpt;
  /// Normalization up to Z^2 scale 
  static int DefaultChargeCorrOpt;
  /// Energy deposition corrections      
  static int DefaultEdepCorrOpt;
  /// Default number of strips used for CofG
  static int DefaultUsedStrips;
  /// _dxdz threshold for 2/3-steip CofG
  static float TwoStripThresholdX;
  /// _dydz threshold for 2/3-steip CofG
  static float TwoStripThresholdY;

 public:

  /** @name   CONSTRUCTORS & C. */
  /**@{*/	

  /// Default constructor 
  TrClusterR(void);
  /// Constructor with data
  TrClusterR(int tkid, int side, int add, int nelem, int seedind, float* adc, unsigned int status);
  /// Constructor divided is several instructions 
  TrClusterR(int tkid, int side, int add, int seedind, unsigned int status);
  
  /// Copy constructor
  TrClusterR(const TrClusterR& orig);
  /// Destructor
  virtual ~TrClusterR();
  /// Clear
  void Clear();
  /// Clear signal vector
  void ClearSig() { _signal.clear(); }
  /**@}*/


  /** @name   Cluster Structure  */
  /**@{*/	

  /// Get RawCluster in the same Ladder (idis=0: seed rawcluster, idis=-1: seed left rawcluster, idis=1: seed right rawcluster,idis=-2,2...) 
  TrRawClusterR *          pRawCluster(int idis=0);
  /// Get ladder TkId identifier 
  int   GetTkId()          const { return _tkid; }
  /// Get ladder layer J number scheme
  int   GetLayerJ()        const { return TkDBc::Head->GetJFromLayer(abs(_tkid/100)); }
  /// Is a K7 cluster?
  int   IsK7()             const { return ( (TkDBc::Head->FindTkId(GetTkId())->IsK7())&&(GetSide()==0) ); } 
  /// Get ladder slot
  int   GetSlot()          const { return abs(_tkid%100); }
  /// Get ladder slot Side (0: negative X, 1: positive X)
  int   GetSlotSide()      const { return (_tkid>=0) ? 1 : 0; }
  /// Returns the silicon face on which the cluster is:
  /*! 
    - Value = 0: n-side aka K-side, X-coordinate 
    - Value = 1: p-side aka S-side, Y-coordinate
  */
  int   GetSide()          const { return (_address<640) ? 1 : 0; }
  /// Get the cluster first strip number   
  int   GetAddress()       const { return _address; }
  /// Get i-th strip address
  int   GetAddress(int ii);
  ///  Check ADC Overflow (current only work for n-strip)
  bool  IsOverflowADC(int ii,int opt);
  /// Get the i-th strip address on sensor (S: 0-639, K5: 0-191, K7: 0-223), if wrong gives back -1
  int   GetSensorAddress(int& sens, int ii = 0, int mult = 0, int multseed_index = -1, int copt=1, int verbose = 1);
  /// Get the local coordinate for i-th strip
  float GetX(int ii, int imult = 0) { 
    return TkCoo::GetLocalCoo(GetTkId(),GetAddress(ii),imult); 
  }
  /// Get the cluster strip multiplicity
  int   GetNelem()         const { return _nelem; }
  /// Get the cluster strip multiplicity
  int   GetLength()              { return GetNelem(); }
  /// Get the seed index and second index (return saturated strips number)
  int   GetSeedSecondIndex(int &seed_index,int &second_index,int opt = DefaultCorrOpt);
  /// Get implant strips type for seed and second (mult is defined by multseed)
  int   GetSeedSecondPairType(int mult, int multseed_index, int opt = DefaultCorrOpt, int copt=1);
  /// Get the seed index 
  int   GetSeedIndex(int opt = DefaultCorrOpt);
  /// Get the seed address 
  int   GetSeedAddress(int opt = DefaultCorrOpt, int seedIndex = -1) { if (seedIndex == -1) {seedIndex = GetSeedIndex(opt);} return GetAddress(seedIndex); }
  /// Get seed signal 
  float GetSeedSignal(int opt = DefaultCorrOpt, int seedIndex = -1)  { if (seedIndex == -1) {seedIndex = GetSeedIndex(opt);} return GetSignal(seedIndex,opt); }
  /// Get seed noise 
  float GetSeedNoise(int opt = DefaultCorrOpt, int seedIndex = -1)   { if (seedIndex == -1) {seedIndex = GetSeedIndex(opt);} return GetNoise(seedIndex); }
  /// Get seed signal to noise ratio 
  float GetSeedSN(int opt = DefaultCorrOpt, int seedIndex = -1)      { if (seedIndex == -1) {seedIndex = GetSeedIndex(opt);} return GetSN(seedIndex); }
  /// Get the numeber of strips on the left of the seed strip
  int   GetLeftLength(int opt = DefaultCorrOpt, int seedIndex = -1)  { if (seedIndex == -1) {seedIndex = GetSeedIndex(opt);} return seedIndex; }
  /// Get the number of strips on the right of the seed strip
  int   GetRightLength(int opt = DefaultCorrOpt, int seedIndex = -1) { if (seedIndex == -1) {seedIndex = GetSeedIndex(opt);} return GetNelem() - seedIndex - 1; }
  /// Get i-th strip status (from calibration)
  short GetStatus(int ii);
  /// Get the strip cross talk factor 
  static float GetAsymmetry(int iside) { return ( (iside>=0)&&(iside<=1) ) ? Asymmetry[iside] : 0; }
	
  /// Is a TAS cluster? (check the status bit)
  bool  TasCls() const { return checkstatus(TASCLS); }
  /// Used for track AMSDBc::USED = 32; (0x0020)
  bool  Used() const { return checkstatus(AMSDBc::USED); }	
  /// Used for ZSEED 
  bool  IsZSEED () const { return checkstatus(ZSEED); }
  /// Track-candi
  bool  TkCandi()const { return checkstatus(TKCAND);}
  /// Check cluster consistency 
  bool Check(int verbosity=0);	
  /// Check if a K7 cluster is well shaped (it can happen that is not)
  bool CheckK7(int mult = -1, int seed_index = -1, int multseed_index=-1, int verbosity=0);

  /**@}*/


  /** @name   SIGNALS & AMPLITUDE */
  /**@{*/	

  /// Get cluster amplitude, with corrections
  float         GetTotSignal(int opt = DefaultCorrOpt, float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult = -1, float dxdz=-2, float dydz=-2);

  /// Convert an ADC signal to the ADC scale of p-side
  float         ConvertToPSideScale(float adc/*n-side*/);
  /// Convert an ADC signal to the ADC scale of n-side 
  float         ConvertToNSideScale(float adc/*p-side*/);
  /// Conversion between ADC and MIPs 
  float         GetNumberOfMIPs(float adc) { return GetNumberOfMIPs_ISS(adc); }
  /// Conversion between ADC and MIPs (derived from 2003 Ion Test Beam data)
  float         GetNumberOfMIPs_TB_2003(float adc);
  /// Conversion between ADC and MIPs (derived from ISS data)
  float         GetNumberOfMIPs_ISS(float adc);
  /// Beta correction 
  float         BetaCorrection(float beta) { return BetaCorrection_ISS_2011(beta); } 
  /// Beta correction (estimated with on ground Muons, 2010)
  float         BetaCorrection_Muons_2010(float beta);
  /// Beta correction (derived from ISS proton data 2011)
  float         BetaCorrection_ISS_2011(float beta);
  /// Rigidity correction (estimated with ISS data), dependent on the A/Z guess
  float         RigidityCorrection(float rigidity, float mass_on_Z = 0.938); 
  /// Beta and rigidity correction (best ranges selected), dependent on the A/Z guess
  float         BetaRigidityCorrection(float beta, float rigidity, float mass_on_z = 0.938);

  /// Get energy deposition (MeV)
  float GetEdep() { return GetTotSignal(TrClusterR::DefaultEdepCorrOpt); }
  /// Get floating charge estimation
  float GetQ(float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult = -1, float dxdz=-2, float dydz=-2) {
    return sqrt(GetTotSignal(TrClusterR::DefaultChargeCorrOpt,beta,rigidity,mass_on_Z,res_mult,dxdz,dydz));
  }
  /// Get floating charge estimation by YJ
  TrQYJCluster GetQYJ_allZGain(float beta = 1, float rigidity = 0, int res_mult = 0, float dxdz=-2, float dydz=-2, float xcof=0, int zgain=1, int qopt=TrChargeYJ::DefaultCorOpt);
  /// Get floating charge estimation by YJ
  TrQYJCluster GetQYJ_all(float beta = 1, float rigidity = 0, int res_mult = 0, float dxdz=-2, float dydz=-2, float xcof=0, float qgain=-1, int qopt=TrChargeYJ::DefaultCorOpt);
  /// Get floating charge estimation by YJ
  float GetQYJ(float beta = 1, float rigidity = 0, int res_mult = 0, float dxdz=-2, float dydz=-2, float xcof=0,float qgain=-1, int qopt=TrChargeYJ::DefaultCorOpt);
  /// Add additional signal status 
  int   GetQStatusYJ(int nstrip_from_seed = 1, int mult = -1);
  /// Dead channels to be add to status; 
  int   GetDeadChannelStatusYJ(int nstrip_from_seed, int mult = 0, int seed_index = -1);

  /// Get new floating charge estimation (implemented in 2018 by Hu LIU), Z0 is the seed charge, qopt=-1 use the default charge option
  float GetQH(float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult = -1, float dxdz=-2, float dydz=-2,float Z0=-1, int qopt=-1);
  /// Get new energy deposition (MeV) (implemented in 2018 by Hu LIU)
  float GetEdepH(int res_mult = -1, float dxdz=-2, float dydz=-2,float Z0=-1, int qopt=-1);

  /// Get i-th strip signal
  float GetSignal(int ii, int opt = DefaultCorrOpt, int zgain = -1);
  /// Get i-th strip noise (from calibration)
  float GetSigma(int ii) { return GetNoise(ii);  }
  /// Get i-th strip noise (from calibration)
  float GetNoise(int ii);   
  /// Get i-th signal-to-noise ratio 
  float GetSN(int ii, int opt = DefaultCorrOpt) { 
    return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii,opt)/GetNoise(ii); 
  }
  /// Get the signal-to-noise ratio of the cluster (sum-signals/sqrt(sum-sigma^2))
  float GetClusterSN(int opt = 0);

  /**@}*/	


  /** @name  Coordinates */
  /**@{*/	
	
  /// Get multiplicity
  int   GetMultiplicity();
  /// Get strip index which is used to define imult (multseed_index) for the Cluster, current is using GetSeedIndex()
  int   GetMultSeedIndex();
  /// Get multiplicity for i-th strip (mult is defined by multseed)
  int   GetResolvedMultiplicity(int ii, int mult, int multseed_index);
  /// Get local coordinate by multiplicity index
  float GetCoord(int imult) { return GetXCofG(DefaultUsedStrips,imult); }
  /// Get global coordinate by multiplicity index
  float GetGCoord(int imult);
  /// Get the pointer of the whole AMS tracker geometric manager (New V6 version, new alignment 2022)
  TkTrackN  *GetTkTrack() const {return TkTrackN::GetHead();}
  /// Get the pointer of the ladder (New V6 version, new alignment 2022)
  TkLadderN *GetLadder() const {return TkTrackN::GetHead()->GetLadder(_tkid);}
  /// Get the ladder channel ID for the ii-th strip in cluster (New V6 version, new alignment 2022)
  /*!
   * \param[in] imult          cluster multiplicity
   * \param[in] ii             ii-th strip [0, GetNelem()-1]
   * \param[in] multseed_index strip index which is used to define imult, -1 means by default using GetMultSeedIndex()
   * \return    ladder channel, -2 underflow error 
   */
  double GetLadderChan(int imult, double ii, int multseed_index=-1);
  /// Get the pointer of the ladder and the ladder channel ID of the cluster (New V6 version, new alignment 2022)
  /*!
   * \param[out] ladchan        ladder channel ID       
   * \param[in]  imult          cluster multiplicity
   * \param[in]  ii             -1 cluster center (CofG), >=0 the ii-th strip [0, GetNelem()-1], -2 the seed strip
   * \param[in]  multseed_index strip index which is used to define imult, -1 means by default using GetMultSeedIndex()
   * \return     pointer of the ladder 
  */
  TkLadderN *GetLadder(double &ladchan, int imult, int ii=-1, float chrg=0, float dxdz=-2, float dydz=-2, int multseed_index=-1);
  /// Get number of possible sensor candidates for the cluster coordinate calculation (New V6 version, new alignment 2022)
  /*!
   * \param[in] ii             sensor candidates for, -1 cluster center (CofG), >=0 the ii-th strip [0, GetNelem()-1], -2 the seed strip
   * \param[in] xaddress       dummy X readout strip address [639.5,1023.5] used for YCluster.
   * \param[in] multseed_index strip index which is used to define imult, -1 means by default using GetMultSeedIndex()
   * \return    number of possible sensor candidates, which is equivalent to the number of multiplicity for XCluster or xaddress>=0 and is equivalent to the number of sensors in ladder for YCluster 
   */
  int   nSensor(int ii=-1, double xaddress=-1, int multseed_index=-1);
  /// Get the sensor channel ID for the ii-th strip in cluster (New V6 version, new alignment 2022)
  /*!
   * \param[out] senschan       strip channel ID in sensor. [0, 191] for K5 and [0, 223] for K7 for XCluster, and [0, 639] for YCluster.
   * \param[in]  isen           index of sensor [0,nSensor()-1]. For XCluster isen=imult is equivalent to the multiplicity which is defined by the seed strip; while for YCluster isen is equivalent to the sensor index in ladder [0, GetLadder()->nSensors()-1]
   * \param[in]  ii             ii-th strip in cluster [0, GetNelem()-1]
   * \param[in]  xaddress       dummy X readout strip address [639.5,1023.5] used for YCluster, When xaddress>=0, isen=imult would become the multiplicity
   * \param[in]  multseed_index strip index which is used to define imult, -1 means by default using GetMultSeedIndex()
   * \return     pointer of the sensor
   */
  TkSensorN *GetSensorChan(double &senschan, int isen, double ii, double xaddress=-1, int multseed_index=-1);
  /// Get the pointer of possible sensor candidate for the cluster coordinate calculation (New V6 version, new alignment 2022)
  /*!
   * \param[out] senschan      strip channel ID in sensor. [0, 191] for K5 and [0, 223] for K7 for XCluster, and [0, 639] for YCluster
   * \param[in] isen           index of sensor [0,nSensor()-1]. For XCluster isen=imult is equivalent to the multiplicity which is defined by the seed strip; while for YCluster isen is equivalent to the sensor index in ladder [0, GetLadder()->nSensors()-1]
   * \param[in] ii             sensor candidates for, -1 cluster center (CofG), >=0 the ii-th strip [0, GetNelem()-1], -2 the seed strip
   * \param[in] chrg           particle charge, 0 means the stored particle charge from latest track fitting namely GetQtrk()
   * \param[in] dxdz           particle incoming angle dx/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetDxDz()
   * \param[in] dydz           particle incoming angle dy/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetDxDz()
   * \param[in] xaddress       dummy X readout strip address [639.5,1023.5] used for YCluster, When xaddress>=0, isen=imult would become the multiplicity
   * \param[in] multseed_index strip index which is used to define imult, -1 means by default using GetMultSeedIndex()
   * \return    pointer of the sensor
   */
  TkSensorN *GetSensor(double &senschan, int isen, int ii=-1, float chrg=0, float dxdz=-2, float dydz=-2, double xaddress=-1, int multseed_index=-1);
  /// Get the cluster coordinate (New V6 version, new alignment 2022)
  /*!
   * \param[in] isen           index of sensor [0,nSensor()-1]. For XCluster isen=imult is equivalent to the multiplicity which is defined by the seed strip; while for YCluster isen is equivalent to the sensor index in ladder[0, GetLadder()->nSensors()-1]
   * \param[in] ii             -1 cluster coordinate at center (CofG), >=0 coordinate for the ii-th strip [0, GetNelem()-1], -2 coordinate for the seed strip
   * \param[in] ualign         111 with all alignment (sensor+ladder+layer)
   * \param[in] dzshiftondxy   1 with Z positon shift correction for heavy nuclei, 0 without
   * \param[in] chrg           particle charge, 0 means the stored particle charge from latest track fitting namely GetQtrk()
   * \param[in] dxdz           particle incoming angle dx/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetDxDz()
   * \param[in] dydz           particle incoming angle dy/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetDxDz()
   * \param[in] xaddress       dummy X readout strip address [639.5,1023.5] used for YCluster. When xaddress>=0, isen=imult would become the multiplicity
   * \param[in] multseed_index strip index which is used to define imult, -1 means by default using GetMultSeedIndex()
   * \return    local coordinate in sensor (getM) and global coordinate in tracker (getMGlobal)
   */
  AMSPlaneM GetGCoordN(int isen, int ii=-1, int ualign=111, bool dzshiftondxy=1, float chrg=0, float dxdz=-2, float dydz=-2, double xaddress=-1, int multseed_index=-1);
  /// Find multiplicity by matching between the track trajectory and the cluster with minimal distance (New V6 version, new alignment 2022) 
  /*!
   * \param[out] dist           the minimal distance to the track trajectory
   * \param[out] mult           the resolved multiplicity by matching with track, -2 error
   * \param[out] xaddress       the closest possible dummy X readout strip address [639.5,1023.5] resolved for YCluster by matching with track.
   * \param[in]  gcoo           particle position in global coordinate system
   * \param[in]  gdir           particle direction in global coordinate system
   * \param[in]  ii             -1 the coordinate at center (CofG) as the cluster position measurement (slow), -2  the seed strip as the cluster measurement (fast), >=0 the ii-th strip as the cluster measurement[0, GetNelem()-1]
   * \param[in]  chrg           particle charge, 0 means the stored particle charge from latest track fitting namely GetQtrk(), which is only used for calculation of position with ii=-1
   * \param[in]  multseed_index strip index which is used to define mult, -1 means by default using GetMultSeedIndex()
   * \return     the closest possible 2D measurement in sensor. For the measured side, its position is defined by the CofG with ii=-1 or by ii-th strip with ii!=-1. For the side without measurement, its position is the closest possible position in sensor with respect to the track.
   */ 
  AMSPlaneM FindMult(double &dist, int &mult, double &xaddress, TVector3 gcoo, TVector3 gdir, int ii=-1, float chrg=0, int multseed_index=-1);
  /// Get cluster bounds for a given number of strips (gerarchic order)  
  void  GetBoundsAsym(int &leftindex, int &rightindex, int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get cluster bounds for a given number of strips (symmetric order)  
  void  GetBoundsSymm(int &leftindex, int &rightindex, int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt, int seedIndex = -1);
  /// Get the Center of Gravity with the n highest consecutive strips 
  float GetCofG(int nstrips = DefaultUsedStrips, int opt = DefaultBestResidualOpt);
  /// Get the Center of Gravity with the n highest consecutive strips by LinearEta algorythm(QY)
  float GetLinearCofG(int nstrips = DefaultUsedStrips, int imult = 0, int opt = DefaultBestResidualOpt, int z = 0, float dxdz=-2, float dydz=-2);
  /// Get local coordinate with center of gravity on nstrips (imult refer to the old cofg) 
  double GetXCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt, float chrg=0, float dxdz=-2, float dydz=-2, int copt=1);
  /// Get local coordinate with simple center of gravity on nstrips
  double GetSimpleXCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt, float dxdz=-2, float dydz=-2, int copt=1);
  /// Get local coordinate with center of gravity on n-nstrips by LinearEta algorythm(QY)
  double GetLinearXCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt, int z = 0, float dxdz=-2, float dydz=-2, int copt=1);
  /// Get local coordinate with center of gravity on p-nstrips by LinearEta algorythm(QY)
  double GetLinearYCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt, int z = 0, float dxdz=-2, float dydz=-2, int copt=1);
  /// Get local coordinate with center of gravity on nstrips 
  float GetXCofG_old(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt) { 
    return TkCoo::GetLocalCoo(GetTkId(),GetSeedAddress(opt)+GetCofG(nstrips,opt),imult); 
  }  
  /// Get eta (center of gravity with the two higher strips) 
  float GetEta(int opt = DefaultBestResidualOpt);
  /// Get eta (center of gravity with the two higher strips) by CofG algorythm
  float GetEta_CofG(int opt = DefaultBestResidualOpt) { float eta = GetCofG(2,opt); return (eta>=0) ? eta : eta + 1.; }
  //  Get eta by LinearEta algorythm(QY)
  float GetEta_LinearCofG(int opt = DefaultBestResidualOpt, int imult = 0, int z = 0, float dxdz=-2, float dydz=-2) { float eta = GetLinearCofG(-1,imult,opt,z,dxdz,dydz); return (eta>=0) ? eta : eta + 1.; }
  static double GetdZShiftondXYG(int ixy, int z, float dxdz, float dydz, int vers=3);
  //  Get dz shift (shielding effect) on moving X(dx) and Y(dy) for z charged nuclei
  double GetdZShiftondXY(float chrg=0, float dxdz=-2, float dydz=-2, int vers=3);
  /// Set linearity correction (brute force method)
  static void SetLinearityCorrection() { DefaultCorrOpt = kAsym|kAngle|kGain|kPStrip; DefaultBestResidualOpt = kAsym|kAngle|kGain|kPStrip; }
  /// Unset linearity correction (brute force method)
  static void UnsetLinearityCorrection() { DefaultCorrOpt = kAsym|kAngle; DefaultBestResidualOpt = -1; } 
  /**@}*/ 


  /** @name Reconstruction & Special Methods (Experts only) */
  /**@{*/	

  /// Get ladder layer OLD Numbering
  int   GetLayer()         const { return abs(_tkid/100); }

  /// Insert a strip in the cluster
  void  push_back(float adc);

  /// Set track interpolation angle tan(thetaXZ) (rad)
  inline void  SetDxDz(float dxdz) { _dxdz = dxdz; }
  /// Set track interpolation angle tan(thetaYZ) (rad)
  inline void  SetDyDz(float dydz) { _dydz = dydz; }
  /// Get track interpolation angle tan(thetaXZ) (rad)
  inline float GetDxDz()  { return _dxdz; }
  /// Get track interpolation angle tan(thetaYZ) (rad)
  inline float GetDyDz()  { return _dydz; }
  /// Get track impact angle (degrees) 
  inline float GetImpactAngle() { return (GetSide()==0) ? atan(_dxdz)*180./3.14159265 : atan(_dydz)*180./3.14159265; }
  /// Get the zenith angle cosine
  inline float GetCosTheta() { return sqrt(1./(1.+GetDxDz()*GetDxDz()+GetDyDz()*GetDyDz())); } 

  /// Get track charge (used for optimization of the track fitting) 
  float GetQtrk() { return _qtrk; }
  /// Set track charge (used for optimization of the track fitting)  
  void SetQtrk(float q) { _qtrk = q; }

  /// chek some bits into cluster status
  uinteger checkstatus(integer checker) const { return Status & checker; }
  /// Get cluster status
  uinteger getstatus() const            { return Status; }
  /// Set cluster status
  void     setstatus(uinteger status)   { Status = Status | status; }
  /// Clear cluster status
  void     clearstatus(uinteger status) { Status = Status & ~status; }
  /// Set track-candi
  void     SetTkCand()   { setstatus(TKCAND);}
  /// Clear track-candi status
  void     ClearTkCand() { clearstatus(TKCAND);}
  /// Set as used
  void     SetUsed()     { setstatus(AMSDBc::USED); }
  /// Clear used status
  void     ClearUsed(int opt=0)  { clearstatus(AMSDBc::USED); if(opt>=1){SetDxDz(0);SetDyDz(0);SetQtrk(1);} }
  /// change back and forth the flip status
  void ChangeFlipStatus(){
    if (checkstatus(kFlip))
      clearstatus(kFlip);
    else 
      setstatus(kFlip);
  }
     
      
  /// Get the current parameter database
  TrParDB* GetTrParDB() { return _trpardb; }
  /// Get the current calibration database
  TrCalDB* GetTrCalDB() { return _trcaldb; }

  /**@}*/	


  /// Using this calibration database
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Using this parameter database
  static void UsingTrParDB(TrParDB* trpardb) { _trpardb = trpardb; }
 	
  ///  Get DefaultCorrOpt
  static int  GetDefaultCorrOpt()        { return DefaultCorrOpt; }
  /// Get DefaultUsedStrips
  static int  GetDefaultUsedStrips()     { return DefaultUsedStrips; }
  /// Set DefaultCorrOpt
  static void SetDefaultCorrOpt(int def) { DefaultCorrOpt = def; }
  /// Set DefaultUsedStrips
  static void SetDefaultUsedStrips(int def) {DefaultUsedStrips=def;}

		
  /** @name Printout */
  /**@{*/	

  /// Print cluster basic information
  std::ostream& putout(std::ostream &ostr = std::cout);
  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr,  TrClusterR &cls) { 
    return cls.putout(ostr); 
  }

  /// Print cluster strip variables (printop >0 --> verbose) 
  void Print(int printopt =0);
  /// Return a string with some info (used for event display)
  const char* Info(int iRef);

  /**@}*/


  /** @name More cluster quantities */
  /**@{*/

  /// Get number of implant strips between the two highest readout strips
  /// N%10 = n. interstrip (ex. S: 3, K5: 1 or 2 (edges), K7: 1 or 0 ...)
  /// N/10 = structure of K7 (1|2|3)   
  int   GetNInterstrip(int mult = 0);
  /// Get number of strips with a particular calibration status mask for this cluster (checks also outside of cluster) 
  int   GetNStripWithCalibrationStatus(int nstrip_from_seed, int mask, int mult = 0, int seed_index = -1, int multseed_index = -1);
  /// Get number of strips with a particular occupancy status mask for this cluster (checks also outside of cluster)
  int   GetNStripWithOccupancyStatus(int nstrip_from_seed, int mask, int mult = 0, int seed_index = -1, int multseed_index = -1);
  /// Get number of strips with a particular gain status mask for this cluster (checks also outside of cluster)
  int   GetNStripWithGainStatus(int nstrip_from_seed, int mask, int mult = 0, int seed_index = -1, int multseed_index = -1);
  /// Get number of strips on the edge of the sensor (checks also outside of cluster)
  int   GetNStripOnTheEdgeOfSensor(int nstrip_from_seed, int mult = 0, int seed_index = -1, int multseed_index = -1);
  /// Get number of strips on the edge of VA (checks also outside of cluster)
  int   GetNStripOnTheEdgeOfVA(int nstrip_from_seed, int mult = 0, int seed_index = -1, int multseed_index = -1);

  /// Check cluster morfology: monothonic behaviour? (extend to how many strips you want around the seed)
  bool  IsMonotonic(int nstrip_from_seed);
  /// Check cluster morfology: monothonic behaviour? (extend to all the strips around seed over threshold)
  bool  IsMonotonicWithThreshold(float threshold);
  /// Check cluster morfology: are the strips over threshold?
  bool  IsOverThreshold(int nstrip_from_seed, float threshold = 1);
  /// Check cluster VA status(-1 check all strip)
  bool  IsGoodVA(int nstrip_from_seed=-1);
  /// Get the ratio between signal of few strips around seed with respect to total signal (no corrections).
  float GetSignalToSignalRatio(int nstrip_from_seed);

  /// \brief Create a cluster status with information about the cluster quality 
  /// \return 
  /// \retval  0-bit: >0 dead strips 
  /// \retval  1-bit: >0 noisy strips 
  /// \retval  2-bit: >0 second step bad strips
  /// \retval  3-bit: >0 strips with bad region flag
  /// \retval  4-bit: >0 dead occupancy strips 
  /// \retval  5-bit: >0 noisy occupancy strips
  /// \retval  6-bit: >0 bad strips from gain database (not gold)
  /// \retval  7-bit: >0 bad strips from gain database (not silver)
  /// \retval  8-bit: >0 strips on the edge of the sensor 
  /// \retval  9-bit: >0 strips on the edge of VA
  /// \retval 10-bit: check if the cluster is well constructed
  /// \retval 11-bit: check if the cluster K7 is well constructed
  int   GetQStatus(int nstrip_from_seed = 1, int mult = -1);

  /// \brief Create a cluster status with valuable information about cluster morfology
  /// \return 
  /// \retval 0-bit: 2 strip around the seed have a non-monotonic behaviour
  /// \retval 1-bit: 3 strip around the seed have a non-monotonic behaviour
  /// \retval 2-bit: strips over SN=0.0 thereshold have a non-monotonic behaviour
  /// \retval 3-bit: strips over SN=1.5 thereshold have a non-monotonic behaviour 
  /// \retval 4-bit: strips over SN=3.0 thereshold have a non-monotonic behaviour 
  /// \retval 5-bit: strips over SN=4.5 thereshold have a non-monotonic behaviour
  int   GetMorfologyStatus();

  /**@}*/


  /// ROOT definition
  ClassDef(TrClusterR, 6)
  //ClassDef(TrClusterR, 7) //YC
};


#endif
