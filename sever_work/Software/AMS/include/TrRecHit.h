//  $Id: TrRecHit.h,v 1.68 2022/06/04 22:40:31 qyan Exp $
#ifndef __TrRecHitR__
#define __TrRecHitR__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRecHitR
///\brief A class to manage hit reconstruction in AMS Tracker
///\ingroup tkrec
///
/// TrRecHitR is a core of the hit reconstruction.
/// The ladder geometry (TkDBc) and calibration databases (TrCalDB) 
/// are used instead of the original TKDBc, TrIdSoft, and TrIdGeom.
/// V6 geometry is managed by TkSensorN, TkLadderN, TkLayerN, and TkTrackN. 
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/28 SH  First refinement (for TkTrack)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/02/19 AO  New data format 
///\date  2008/02/22 AO  Temporary clusters reference  
///\date  2008/02/26 AO  Local and global coordinate (TkCoo.h)
///\date  2008/03/06 AO  Changing some data members and methods
///\date  2008/04/12 AO  From XEta to XCofG(3) (better for inclination)
///\date  2008/11/29 SH  _dummyX added, _residual moved to TrTrack
///\date  2009/08/16 PZ  General revison -- new inheritance scheme - std printout
///\date  2017/04/27 QY  New position reconstruction algorithm
///\date  2022/02/07 QY  New V6 software
///
//////////////////////////////////////////////////////////////////////////

#include <string>
#include "TrCluster.h"
#include "point.h"
#include "TkCoo.h"
#include "TrElem.h"
#include "point.h"
#include "amsdbc.h"

#include "TMath.h"

#include "TrRecHitChargeH.h"
#include "AMSPlane.h"
#include "TkGeomN.h"

class TrRecHitR : public TrElem {

 public:

  enum { XONLY = 0x100, YONLY = 0x200, TASHIT = 0x400, ZSEED = 0x800,
	 REBLT = 0x1000, SHARED = 0x4000, CSEED = 0x10000, VERTEX = 0x20000, TKCAND=0x40000}; 
  /// the Hit charge object (implemented in 2018 by Hu LIU)
  TrRecHitChargeH hitcharge; //!

 protected:

  /// Pointer to the X (n-side) TrClusterR in the fTrClusterR collection
  TrClusterR*  _clusterX; //!
  /// Pointer to the Y (p-side) TrClusterR in the fTrClusterR collection
  TrClusterR*  _clusterY; //!
  /// TkLadder ID
  short int   _tkid;
  /// Hit multiplicity 
  int8 _mult;
  /// Multiplicity index (-1 means not yet resolved, >-1 resolved by tracking algorithm)
  int8 _imult;
  /// Dummy X-strip position for YONLY hit
  float _dummyX;
  /// Hit global coordinate  
  AMSPoint _coord;
  
  /// X Cluster index
  short int _iclusterX;
  /// Y Cluster index
  short int _iclusterY;
  /// Hit status 
  int   Status;

  /// Correlation parameters (DEPRECATED)
  static float GGpars[6];
  /// Correlation parameters (DEPRECATED)
  static float GGintegral;

  /// load the std::string sout with the info for a future output
  std::string _PrepareOutput(int full=0);

public:

  /**@name CONSTRUCTOR & C */
  /**@{*/
  //################# CONSTRUCTOR & C ########################
  /// Default constructor
  TrRecHitR(void);
  /// Copy constructor
  TrRecHitR(const TrRecHitR& orig);
  /// Constructor with clusters
  TrRecHitR(int tkid, TrClusterR* clX, TrClusterR* clY, int imult = -1, int status = 0);
  /// Destructor
  virtual ~TrRecHitR();
  /**@}*/	


  /**@name Accessors */	
  /**@{*/	
  //####################  ACCESSORS ##############################
  /// Get ladder TkId identifier 
  int   GetTkId()        const { return _tkid; }
  /// Get ladder layer J Scheme (1-9)
  int   GetLayerJ()       const { return TkDBc::Head->GetJFromLayer(abs(_tkid/100)); }
  /// Get ladder slot
  int   GetSlot()        const { return abs(_tkid%100); }
  /// Get ladder slot Side (0 == negative X, 1== positive X)
  int   GetSlotSide()        const { return (_tkid>=0)?1:0; }
  /// Access function to TrClusterR Object used; 
  /// \param xy 'x' for x projection; any other for y projection;
  /// Returns index in TrClusterR collection or -1 
  int iTrCluster(char xy) const { return (xy=='x')?_iclusterX:_iclusterY; }

  /// Access function to TrClusterR Object used; 
  /// \param xy 'x' for x projection; any other for y projection;
  /// Returns index in TrClusterR collection or -1 
  TrClusterR* pTrCluster(char xy)  { return (xy=='x')?GetXCluster():GetYCluster(); }

  /// Return a string with hit infos (used for the event display)
  const char *Info(int iRef=0);
  
  /// Get the pointer to X cluster
  TrClusterR* GetXCluster();
  /// Get the pointer to Y cluster
  TrClusterR* GetYCluster();
  /// Get the index of X cluster
  int GetXClusterIndex() const { return _iclusterX; }
  /// Get the index of Y cluster
  int GetYClusterIndex() const { return _iclusterY; }

  bool OnlyX () const { return checkstatus(XONLY); }
  bool OnlyY () const { return checkstatus(YONLY); }
  bool TasHit() const { return checkstatus(TASHIT); }
  bool Reblt () const { return checkstatus(REBLT); }
  // AMSDBc::USED = 32; (0x0020)
  bool Used  () const { return checkstatus(AMSDBc::USED); }
  // AMSDBc::FalseX = 8192; (0x2000)
  bool FalseX() const { return checkstatus(AMSDBc::FalseX); }
  bool HighZ () const { return checkstatus(ZSEED); }
  bool HighC () const { return checkstatus(CSEED);}
  bool Shared() const { return checkstatus(SHARED); }  
  bool TkCandi()const { return checkstatus(TKCAND);}
  /**@}*/	


  /**@name Coordinates*/	
  /**@{*/		
  /// Get the hit multiplicity 
  int GetMultiplicity()      { return _mult; }
  /// Get the resolved multiplicity index (-1 if not resolved)
  int   GetResolvedMultiplicity() { return _imult; }

  /// Returns the hit coordinate
  /// \li if  force=0 and imult=-1 (default)options, it returns the stored coordinate as it is (with multiplicity 0 if mult is not resolved by track)
  /// \li if  force=0 and imult>0, it returns the calculated requested multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS PG
  /// \li if  force=1 and imult=-1, it returns the calculated resolved  multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS PG
  /// \li if  force=1 and imult>0,  it returns the calculated requested multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS PG
  /// \li if  force=2 and imult=-1, it returns the calculated resolved multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS CIEMAT
  /// \li if  force=2 and imult>0,  it returns the calculated requested multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS CIEMAT
  /// \li if  force=3 and imult=-1, it returns the calculated resolved  multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS (CIEMAT+PG)/2
  /// \li if  force=3 and imult>0,  it returns the calculated requested multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS (CIEMAT+PG)/2
  /// \li if  force=4 and imult=-1, it returns the calculated resolved  multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS (CIEMAT-PG)
  /// \li if  force=4 and imult>0,  it returns the calculated requested multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS (CIEMAT-PG)
  /// \li if  force=5 and imult=-1, it returns the calculated resolved  multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS NO EXT ALINGN
  /// \li if  force=5 and imult>0,  it returns the calculated requested multiplicty coordinates IF HIT IS ON EXT LAYERS the POSITION IS NO EXT ALINGN

  const AMSPoint GetCoord(int imult=-1, int force=0){
    //sanity check on input pars
    if(imult>=_mult|| imult<-1) return AMSPoint(0,0,0);
    if((force<0&&force!=-6)|| force>6) return AMSPoint(0,0,0);
    
    int reqmult=imult;
    if(imult==-1) reqmult=GetResolvedMultiplicity();
    if(GetLayer()<8 && force>=1 && force<6) force =1;

    if(force ==0){
      if(imult<0) return _coord;
      else   return GetGlobalCoordinate(imult);
    }
    if(force==1){//PG recalculated
      return GetGlobalCoordinate(reqmult);

    }else if(force ==2){ //CIEMAT recalculated
      return GetGlobalCoordinate(reqmult,"AM");
  
    }else if( force ==3){ //(PG+CIEMAT)/2
      AMSPoint pg=GetGlobalCoordinate(reqmult);
      AMSPoint md=GetGlobalCoordinate(reqmult,"AM");
      AMSPoint av=pg+md;
      return av*0.5;
    }else if( force ==4){ //(PG+CIEMAT)/2
      AMSPoint pg=GetGlobalCoordinate(reqmult);
      AMSPoint md=GetGlobalCoordinate(reqmult,"AM");
      AMSPoint av=md-pg;
      return av;
    }else if( force ==5){ //No Ext Align
      return GetGlobalCoordinate(reqmult,"AZ");
    }else if( abs(force) ==6){ //Mc if available
      return MCCoo(force<0);
    }else 
      return AMSPoint (0,0,0);
  }

  AMSPoint MCCoo(bool primary);
  /// Returns the errors on the computed global coordinate (if resolved)
  AMSPoint GetECoord() {return AMSPoint(0.002,0.003,0.015);}
  /// Get X local coordinate (ladder reference frame)
  float GetXloc(int imult = 0, int nstrips = TrClusterR::DefaultUsedStrips, int opt = TrClusterR::DefaultBestResidualOpt);
  /// Get Y local coordinate (ladder reference frame)
  float GetYloc(int nstrips = TrClusterR::DefaultUsedStrips, int opt = TrClusterR::DefaultBestResidualOpt);
	
  /// Get local coordinate (ladder reference frame, Z is zero by definition)
  AMSPoint GetLocalCoordinate(int imult = 0, 
			      int nstripsx = TrClusterR::DefaultUsedStrips,
			      int nstripsy = TrClusterR::DefaultUsedStrips,
                              int optx = TrClusterR::DefaultBestResidualOpt,
                              int opty = TrClusterR::DefaultBestResidualOpt) { 
    return AMSPoint(GetXloc(imult,nstripsx,optx),GetYloc(nstripsy,opty),0.); }

  /// Get global coordinate with ALIGNMENT (AMS reference system) 
  AMSPoint GetGlobalCoordinateA(int imult = 0){
    return GetGlobalCoordinate( imult, "A");
  }
  /// Get global coordinate without  ALIGNMENT (AMS reference system) 
  AMSPoint GetGlobalCoordinateN(int imult = 0){
    return GetGlobalCoordinate( imult, "");
  }
  /// Returns the (minimal) distance between two hits on the selcted coo (x 0, y 1, z 2)
  float HitDist(TrRecHitR & B,int coo);
  /// Returns an AMSPoint with the minimal distance between an Hit and a  given point
  /// \param coo the input point in form of AMSPoint
  /// \param mult gives back the closest hit multiplicity
  const AMSPoint HitPointDist(const AMSPoint& coo,int & mult);
  /// Returns an AMSPoint with the minimal distance between an Hit and a  given point
  /// \param coo the input pointin form of float vector
  /// \param mult gives back the closest hit multiplicity
  const AMSPoint HitPointDist(float* coo,int& mult){
    return HitPointDist(AMSPoint(coo[0],coo[1],coo[2]),mult);
  }
  /**@}*/		


  /**@name Signals */			
  /**@{*/			
  /// Returns the hit signal (0: x, 1: y, 2: x or y if no x, 3: weighted mean (not fully implemented), 4: mean (not so reasonable))
  float GetSignalCombination(int iside, int opt = TrClusterR::DefaultCorrOpt, float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult=-2, float dxdz=-2, float dydz=-2);
  /// Get energy deposition (MeV) (iside = 0: x, 1: y, 2: x, y if no x)
  float GetEdep(int iside) { return GetSignalCombination(iside,TrClusterR::DefaultEdepCorrOpt); }
  /// Get floating charge estimation (iside = 0: x, 1: y, 2: x, y if no x)
  float GetQ(int iside, float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult=-2, float dxdz=-2, float dydz=-2) { return sqrt(GetSignalCombination(iside,TrClusterR::DefaultChargeCorrOpt,beta,rigidity,mass_on_Z,res_mult,dxdz,dydz)); }
  /// Get floating charge estimation by YJ
  TrQYJHit GetQYJ_all(int iside, float beta = 1, float rigidity = 0, int res_mult=-2, float dxdz=-2, float dydz=-2, float qgain=-1, int qopt=TrChargeYJ::DefaultCorOpt);
  /// Get floating charge estimation by YJ
  float GetQYJ(int iside, float beta = 1, float rigidity = 0, int res_mult=-2, float dxdz=-2, float dydz=-2, float qgain=-1, int qopt=TrChargeYJ::DefaultCorOpt);
  /// Add additional signal status
  int   GetQStatusYJ(int nstrip_from_seed = 1);
  /// Get the signal status (0th, ..., 11th bit X-side, 12th, ..., 23rd Y-side. Bit explanation in TrClusterR::GetQStatus()) 
  int   GetQStatus(int nstrip_from_seed = 1);
  /// Returns the hit correlation for a gaussian p-value (test performed without corrections and used in reconstruction)
  float GetCorrelationProb();

  /// Get new energy deposition (MeV) (iside = 0: x, 1: y, 2: x&y combined. Implemented in 2018 by Hu LIU.) Z0 is the seed charge, qopt=-1 use the default charge option
  float GetEdepH(int iside, float dxdz=-2, float dydz=-2, float Z0=-1, int qopt=-1);
  /// Get new floating charge estimation (iside = 0: x, 1: y, 2: x&y combined. Implemented in 2018 by Hu LIU.)
  float GetQH(int iside, float beta = 1, float rigidity = 0, int res_mult=-2, float dxdz=-2, float dydz=-2, float Z0=-1, int qopt=-1);

  /// Returns the signal of the Y cluster (DEPRECATED) 
  float Sum() { return (GetYCluster()) ? GetYCluster()->GetTotSignal() : 0; }
  /// Returns the signal sum of the X and Y clusters when they exist (or only X or only Y, or in the worse case 0) (DEPRECATED)
  float GetTotSignal() {
    return
      ((GetXCluster())? GetXCluster()->GetTotSignal():0) +
      ((GetYCluster())? GetYCluster()->GetTotSignal():0);
  }
  /// Returns the hit signal signal (on side p scale) (DEPRECATED)
  float GetSignalDifference();
  /// Get correlation between the X and Y clusters (DEPRECATED)
  float GetCorrelation();
  /// Get probability of correlation between the X and Y clusters (DEPRECATED)
  float GetProb();
  /**@}*/	

	
  /**@name Reconstruction & Special methods */			
  /**@{*/		
  /// Get global coordinate (AMS reference system) 
  /// default: nominal position, A: with alignement correction MA: Aligned but CIEMAT ext alignment AZ: alingned but no ext layers dyn alignment
  AMSPoint GetGlobalCoordinate(int imult = 0, const char* options = "A",
			       int nstripsx = TrClusterR::DefaultUsedStrips,
			       int nstripsy = TrClusterR::DefaultUsedStrips,
                               int optx = TrClusterR::DefaultBestResidualOpt,
                               int opty = TrClusterR::DefaultBestResidualOpt);
  /// Get the pointer of the whole AMS tracker geometric manager (New V6 version, new alignment 2022)
  TkTrackN  *GetTkTrack() const {return TkTrackN::GetHead();}
  /// Get the pointer of the ladder (New V6 version, new alignment 2022)
  TkLadderN *GetLadder() const {return TkTrackN::GetHead()->GetLadder(_tkid);}
  /// Get number of possible sensor candidates for Hit coordinate calculation (New V6 version, new alignment 2022)
  /*!
   * \param[in] xaddress      dummy X readout strip address [639.5,1023.5] used to locate OnlyY hit activated only when xaddress>=0.
   * \return    number of possible sensor candidates. For the hit with XCluster or xaddress>=0, this is equivalent to the number of multiplicity; while for the hit with OnlyY (xaddress<0), the number of sensor candidates is identical to the number of sensors in the ladder. 
   */
  int nSensor(double xaddress=-1);
  /// Get the pointer of possible sensor candidate for the Hit coordinate calculation (New V6 version, new alignment 2022)
  /*!
   * \param[out] senschan[2]   seed strip channel ID in sensor for X side and Y side, [0, 191] for K5 and [0, 223] for K7 in X side, [0, 639] in Y side. -10 means unknown
   * \param[in]  isen         index of sensor [0,nSensor()-1]. For the hit with XCluster, isen=imult is equivalent to the multiplicity; while for the hit with OnlyY, isen>=0 is equivalent to the sensor index in ladder [0, GetLadder()->nSensors()-1]. isen=-1 means using GetResolvedMultiplicity()
   * \param[in]  chrg         particle charge, 0 means the stored particle charge from latest track fitting namely GetCluster()->GetQtrk()
   * \param[in]  dxdz         particle incoming angle dx/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetCluster()->GetDxdz()
   * \param[in]  dydz         particle incoming angle dy/dz in local sensor coordinate system, -2 means the stored dy/dz from latest track fitting namely GetCluster()->GetDydz()
   * \param[in]  xaddress     dummy X readout strip address [639.5,1023.5] used to locate OnlyY hit activated only when xaddress>=0, in this case isen=imult would become the multiplicity.
   * \return     pointer of the sensor
   */
  TkSensorN *GetSensor(double senschan[2], int isen, float chrg=0, float dxdz=-2, float dydz=-2, double xaddress=-1);
  /// Get the pointer of the ladder and the ladder channel ID of the hit (New V6 version, new alignment 2022)
  /*!
   * \param[out] ladchan[2]   ladder channel ID for X side and Y side, -10 means unknown
   * \param[in]  isen         index of sensor [0,nSensor()-1]. For the hit with XCluster, isen=imult is equivalent to the multiplicity; while for the hit with OnlyY, isen>=0 is equivalent to the sensor index in ladder [0, GetLadder()->nSensors()-1]. isen=-1 means using GetResolvedMultiplicity()
   * \param[in]  chrg         particle charge, 0 means the stored particle charge from latest track fitting namely GetCluster()->GetQtrk()
   * \param[in]  dxdz         particle incoming angle dx/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetCluster()->GetDxdz()
   * \param[in]  dydz         particle incoming angle dy/dz in local sensor coordinate system, -2 means the stored dy/dz from latest track fitting namely GetCluster()->GetDydz()
   * \param[in]  xaddress     dummy X readout strip address [639.5,1023.5] used to locate OnlyY hit activated only when xaddress>=0, in this case isen=imult would become the multiplicity.
   * \return     pointer of the ladder 
   */
  TkLadderN *GetLadder(double ladchan[2], int isen, float chrg=0, float dxdz=-2, float dydz=-2, double xaddress=-1);
  /// Get Hit coordinate (New V6 version, new alignment 2022)
  /*!
   * \param[in] isen          index of sensor [0,nSensor()-1]. For the hit with XCluster, isen=imult is equivalent to the multiplicity; while for the hit with OnlyY, isen>=0 is equivalent to the sensor index in ladder [0, GetLadder()->nSensors()-1]. isen=-1 means using GetResolvedMultiplicity() together with _dummyX (used for OnlyY hit) to get sensor 
   * \param[in] ualign        111 with all alignment (sensor+ladder+layer)
   * \param[in] dzshiftondxy  1 with Z positon shift correction for heavy nuclei, 0 without
   * \param[in] chrg          particle charge, 0 means the stored particle charge from latest track fitting namely GetCluster()->GetQtrk()
   * \param[in] dxdz          particle incoming angle dx/dz in local sensor coordinate system, -2 means the stored dx/dz from latest track fitting namely GetCluster()->GetDxdz()
   * \param[in] dydz          particle incoming angle dy/dz in local sensor coordinate system, -2 means the stored dy/dz from latest track fitting namely GetCluster()->GetDydz()
   * \param[in] xaddress      dummy X readout strip address [639.5,1023.5] used to locate OnlyY hit activated only when xaddress>=0, in this case isen=imult would become the multiplicity
   * \return    local coordinate in sensor (getM) and global coordinate in tracker (getMGlobal)
   */
  AMSPlaneM GetGCoordN(int isen, int ualign=111, bool dzshiftondxy=1, float chrg=0, float dxdz=-2, float dydz=-2, double xaddress=-1);
  /// Find multiplicity by matching between the track trajectory and the hit with minimal distance (New V6 version, new alignment 2022)
  /*!
   * \param[out] dist           the minimal distance to the track trajectory
   * \param[out] mult           the resolved multiplicity by matching with track, -2 error
   * \param[out] xaddress       the closest possible dummy X readout strip address [639.5,1023.5] resolved for OnlyY hit by matching with track, xaddress=_dummyX+640
   * \param[in]  gcoo           particle position in global coordinate system
   * \param[in]  gdir           particle direction in global coordinate system
   * \param[in]  ii             For the side with measured cluster, its position is defined by the cluster CofG with ii=-1 (slower but more precise) or by the seed strip with ii=-2 (faster but less precise) 
   * \param[in]  chrg           particle charge, 0 means the stored particle charge from latest track fitting namely GetCluster()->GetQtrk(), which is only used for calculation of position with ii=-1
   * \return     the closest possible 2D measurement in sensor. For the side with measured cluster, its position is defined by the CofG with ii=-1 or by the seed strip with ii=-2. For the side without measurement, its position is the closest possible position in sensor with respect to the track
   */
  AMSPlaneM FindMult(double &dist, int &mult, double &xaddress, TVector3 gcoo, TVector3 gdir, int ii=-1, float chrg=0);
  /// Set the resolved multiplicity index (-1 if not resolved)
  void  SetResolvedMultiplicity(int im) { 
    if (im < 0) im = 0;
//    if (im >= _mult) im = _mult-1;
    _imult = im; 
    _coord=GetGlobalCoordinate(_imult>=_mult?_mult-1: _imult);
  }
  /// Set clusters index
  void SetiTrCluster(int iclsx, int iclsy);
  //PZ removed to save space  /// Returns the computed global coordinate (if resolved)
  //  AMSPoint GetBField() { return ( (0<=_imult) && (_imult<_mult) ) 
  //			   ? GetBField(_imult) : AMSPoint(0, 0, 0); }
  /// Get the computed global coordinate by multiplicity index
  //  AMSPoint GetBField(int imult) { if(_coord.empty()) BuildCoordinates();
  //     return (0<=imult && imult<_mult) ? _bfield.at(imult) : AMSPoint(0,0,0); }
	
  /// Clear data members
  void Clear();
  /// Rebuild the current coordinate; _coord
  void BuildCoordinate() { if (_imult>=0) _coord=GetGlobalCoordinate(_imult); }
  // Rebuild the current coordinate using DynAlignment class for the external layers
  //void BuildCoordinateDynExt();
  /// Get dummy strip position
  float GetDummyX() { return _dummyX; }
  /// Set dummy strip position
  void SetDummyX(float dumx) { 
    _dummyX = dumx; 
    float xaddr=640;
    TrClusterR* clX= GetXCluster();
    if(clX!=0)
      xaddr =  clX->GetAddress();
    else if(_dummyX>=-0.5){
      int idummyX=(_dummyX>=383.5)?383:int(_dummyX+0.5);
      xaddr += idummyX;
    }
    _mult = (TasHit()) ? 1 : TkCoo::GetMaxMult(GetTkId(), xaddr)+1;
  } 

  /// Set track-candi
  void SetTkCand();
  /// Clear track-candi status
  void ClearTkCand();
  /// Set as used
  void SetUsed();
  /// Clear used status
  void ClearUsed(int opt=0);

  /// chek some bits into cluster status
  uinteger checkstatus(integer checker) const{return Status & checker;}
  /// Get cluster status
  uinteger getstatus() const{return Status;}
  /// Set cluster status
  void     setstatus(uinteger status){Status=Status | status;}
  /// Clear cluster status
  void     clearstatus(uinteger status){Status=Status & ~status;}
  /**@}*/

	
  /**@name Alternative & deprecated accessors
     STD GBATCH compatibility layer */
  /**@{*/
  /// Get ladder layer OLD Scheme (1-9)
  int   GetLayer()       const { return abs(_tkid/100); }
  /// Get ladder layer New J-Scheme (1-9)
  int   layJ()       const { return GetLayerJ(); }
  /// Get ladder layer old Scheme (1-9)
  int   lay()       const { return GetLayer(); }
 
 
  ///STD GBATCH compatibility layer
  int lad() const {return GetSlot();}
  
  /// Compatibility with default Gbatch
  AMSPoint getHit(bool = true) { return GetCoord(); }
  number   getsum()            { return GetTotSignal(); }
  /**@}*/
	

  /**@name Printout */
  /**@{*/	
  /// Print clusterRec hit  basic information  on a given stream 
  std::ostream& putout(std::ostream &ostr = std::cout);
  friend std::ostream &operator << (std::ostream &ostr,  TrRecHitR &hit){
    return hit.putout(ostr);}
  /// Print hit info (verbose if opt !=0 )
  void  Print(int opt=0);	
  /// ROOT definition
  ClassDef(TrRecHitR,5)
  /**@}*/
};

#endif
