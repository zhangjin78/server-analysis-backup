/// $Id: TkGeomN.h,v 1.19 2022/04/14 09:57:57 qyan Exp $ 
#ifndef __TkGeomN__
#define __TkGeomN__

//////////////////////////////////////////////////////////////////////////
///
///
/// TkGeomN Tracker software V6
///\brief Tracker software V6 for the calculation of the tracker coordinate: TkSensorN<->TkLadderN<->TkLayerN<->TkTrackN
///\ingroup tkgeomn
///
///\date  2020/11/18 QY  First version
///
/// $Revision: 1.19 $
///
//////////////////////////////////////////////////////////////////////////

#include <map>
#include <vector>
#include <TVector3.h>
#include <TSpline.h>
#include "typedefs.h"
#include "AMSPlane.h"

//! Tracker Sensor class for the calculation of the coordinate (New V6 version)
/*!
 * \author qyan@cern.ch
 */
class TkSensorN: public TkPlaneN{

 public:
  double dims_[3];///<strip demension (Sensor Center Position in Ladder)
  int nchan[2];///<number of channels in sensor 
  int pladchanx;///<ladchanx=chanx(senschanx)+pladchanx, ladchany=chany(senschany)

 public:
  TkSensorN();
  TkSensorN(int senid,int sentype=0);
  bool      IsK7(){return (type_==1);}
  double    GetLocalCooX(int chanx);
  double    GetLocalCooY(int chany);
  double    GetLocalCooXY(int chan,int ixy);
  /// Calculated coordinate in sensor
  /*!
   * \param[in] chanx  sensor channel ID in X, [-0.5, 191.5] for K5 and [-0.5, 223.5] for K7, chanx=-10 means in the middle of sensor in X 
   * \param[in] chany  sensor channel ID in Y, [-0.5, 639.5], chany=-10 means in the middle of sensor in Y
   * \return    coordinate in sensor
   */
  AMSPlaneM GetLocalCoo(double chanx,double chany);
  /// Check if the sensor chan is within the sensor strip range
  /*!
   * \param[in]  chan  the sensor channel 
   * \param[in]  ixy   0 in local X, 1 in local Y
   * \return  true if chan in X is within [-0.5, 191.5] for K5 sensor and [-0.5, 223.5] for K7 sensor. For chan in Y, true if chan is within [-0.5, 639.5]. 
   */
  bool      IsValidChan(double chan,int ixy);
  /// Find the sensor channel closest to the sensor local coordinate
  /*!
   * \param[out] chan  the closest sensor channel
   * \param[in]  ixy   0 in local X, 1 in local Y
   * \param[in]  coo   local coordinate in sensor
   * \return     distance to the closest channel, >0 (<0) outside the sensor area on right (left), 0 inside the sensor
   */
  double    FindLocalChan(double &chan,int ixy,double coo);
  /// Find the sensor channel closest to the particle trajectory by interpolation with particle coordinate (gcoo) and direction (gdir)
  /*!
   * \param[out] chanx  the closest sensor channel in X, [0, 191] for K5 and [0, 223] for K7
   * \param[out] chany  the closest sensor channel in Y, [0, 639] 
   * \param[in]  gcoo   particle position in global coordinate system
   * \param[in]  gdir   particle direction in global coordinate system
   * \return     distance to the closest channel, >0 outside the sensor area, 0 inside the sensor
   */
  double    FindLocalChan(double &chanx,double &chany,TVector3 gcoo,TVector3 gdir);
  /// Get the particle distance to the sensor readout/edge by interpolation with particle coordinate (gcoo) and direction (gdir)
  /*!
   * \param[in]  gcoo      particle position in global coordinate system
   * \param[in]  gdir      particle direction in global coordinate system
   * \param[in]  chanx     sensor measured channel ID in X. -2 means no measurement; X distance will be the distance to the sensor edge
   * \param[in]  chany     sensor measured channel ID in Y. -2 means no measurement; Y distance will be the distance to the sensor edge
   * \param[in]  striparea 1 judgement in sensor active area, 0 judgement in full sensor area
   * \param[in]  uxy       0 in X, 1 in Y, 2 in both X and Y
   * \return     distance to the sensor edge, >0 outside the sensor area, <=0 inside the sensor
   */
  double    GetDistance(TVector3 gcoo,TVector3 gdir,double chanx=-2,double chany=-2,bool striparea=0,int uxy=2);
  /// Calculated coordinate in ladder with sensor alignment, the same as TkLadderN::GetLocalCoo
  /*!
   * \param[in] chanx      sensor channel ID in X, [-0.5, 191.5] for K5 and [-0.5, 223.5] for K7, chanx=ladchanx-isen*SensorChannelX(192,224), chanx=-10 means in the middle of sensor in X
   * \param[in] chany      sensor channel ID in Y, [-0.5, 639.5], chany=-10 means in the middle of sensor in Y
   * \param[in] ualign
   * \return    local coordinate in sensor and global coordinate in ladder
   */
  AMSPlaneM GetInLadderCoo(double chanx,double chany,int ualign=1);
  /// Convert sensor channel ID to channel ID in ladder(X), ladchanx=chanx+isen*SensorChannelX(192,224)
  /*!
   * \param[in] chanx      sensor channel ID(X), [-0.5, 191.5] for K5 and [-0.5, 223.5] for K7, chanx=-10 means in the middle of sensor
   * \return    ladder channel ID(X), -1 outside the sensor
   */
  double    GetInLadderChanX(double chanx);
  /// Convert sensor channel ID to the DAQ channel ID (X)
  /*!
   * \param[in]  chanx     sensor channel ID(X), [-0.5, 191.5] for K5 and [-0.5, 223.5] for K7, chanx=-10 means in the middle of sensor 
   * \param[out] mult      multiplicity
   * \return     DAQ channel ID in X [639.5,1023.5], -1 outside the sensor, <=-2 error
   */
 double    GetDAQChanX(double chanx,int &mult);
  /// Convert ladder channel ID to channel ID in this sensor (X)
   /*!
    * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384; 
    * \return    sensor channel ID(X), -1 outside the sensor
    */
  double    GetSensorChanX(double ladchanx);
  /// Convert DAQ channel ID to the channel ID in this sensor (X)
  /*!
   * \param[in]  daqchanx  DAQ channel ID in X [639.5,1023.5]
   * \param[out] mult      the resolved DAQ multiplicity in ladder (X)
   * \return     sensor channel ID(X), -1 outside the sensor, <=-2 error
   */
  double    GetSensorChanXFromDAQ(double daqchanx, int &mult);
};


//! Tracker Ladder class for the calculation of the coordinate (New V6 version)
/*!
 * \author qyan@cern.ch
 */
class TkLadderN: public TkPlaneN{

 public:
  int nchans[2];///<ladder total strip channels(sum of all sensors) for X and Y
  vector<TkSensorN> sensors_;///<sensors in this ladder

 public:
  TkLadderN();
  TkLadderN(int tkid,int ladtype);//K5,K7
  int        AddSensors(int nsensors);
  TkSensorN *AddSensor(TkSensorN sensor);
  /// Get number of sensors in this ladder
  int        nSensors(){return sensors_.size();}
  /// Get the pointer of i-th sensor in this ladder
  TkSensorN *pSensor(int i){return (i>=0&&i<int(sensors_.size()))?&(sensors_[i]):0;}
  bool       IsK7(){return (type_==1);}
  /// Get maximal multiplicity (0<=imul<=GetMaxMult) 
  /*!
   * \param[in]  daqchanx  DAQ channel ID in X [639.5,1023.5] 
   * \return     maximal multiplicity
   */
  int        GetMaxMult(double daqchanx);
  /// Convert channel ID in ladder to the DAQ channel ID (X)
  /*!
   * \param[in]  ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384; 
   * \param[out] mult      multiplicity
   * \return     DAQ channel ID(X), -2 if error
   */
  static double GetDAQChanX(double ladchanx,int &mult);
  /// Convert DAQ channel ID to the channel ID in ladder (X)
  /*!
   * \param[in]  daqchanx  DAQ channel ID(X)
   * \param[out] mult      multiplicity
   * \return     channel ID in ladder(X), -2 error
   */
  static double GetLadderChanX(double daqchanx,int mult);
  /// Get the pointer of the sensor in this ladder
  /*!
   * \param[in]  ladchanx   channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder
   * \param[out] senschanx  channel ID in sensor(X)
   * \return     pointer of the sensor
   */
  TkSensorN *GetSensor(double ladchanx,double &senschanx);
  /// Find the sensor in this ladder closest to the particle trajectory by interpolation with particle coordinate (gcoo) and direction (gdir)
  /*!
   * \param[out] dist       distance to the closest sensor, <=0 trajectory go though inside the sensor area, >0 trajectory outside of sensors area
   * \param[in]  gcoo       particle position in global coordinate system
   * \param[in]  gdir       particle direction in global coordinate system
   * \param[in]  striparea  1 judgement in sensor active area, 0 judgement in full sensor area
   * \param[in]  daqchanx   DAQ channel in X, -2 means no measurement
   * \param[in]  daqchany   DAQ channel in Y, -2 means no measurement
   * \return     pointer of the closest sensor 
   */
  TkSensorN *FindSensor(double &dist,TVector3 gcoo,TVector3 gdir,bool striparea=0,double daqchanx=-2,double daqchany=-2);
  /// Get the particle distance to the ladder edge (rough estimation) by interpolation with particle coordinate (gcoo) and direction (gdir)
  /*!
   * \param[in]  gcoo  particle position in global coordinate system
   * \param[in]  gdir  particle direction in global coordinate system
   * \return     distance to the ladder edge, >0 outside the ladder area, <=0 inside the ladder
   */
  double     GetDistToEdgeRough(TVector3 gcoo,TVector3 gdir);
  /// Calculated coordinate in ladder with sensor alignment, the same as TkSensorN::GetGetInLadderCoo
  /*!
   * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder in X
   * \param[in] ladchany  channel ID in ladder(Y), [-0.5, 639.5], ladchany=-10 means in the middle of ladder in Y
   * \param[in] ualign
   * \return    local coordinate in sensor and global coordinate in ladder
  */
  AMSPlaneM  GetLocalCoo(double ladchanx,double ladchany,int ualign=1);
  /// Calculated coordinate in layer with sensor+ladder alignment, the same as TkLayerN::GetLocalCoo
  /*!
   * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder in X
   * \param[in] ladchany  channel ID in ladder(Y), [-0.5, 639.5], ladchany=-10 means in the middle of ladder in Y
   * \param[in] ualign
   * \return    local coordinate in sensor and global coordinate in layer
   */
  AMSPlaneM  GetInLayerCoo(double ladchanx,double ladchany,int ualign=11);
};


//! Tracker Layer class for the calculation of the coordinate (New V6 version)
/*!
 * \author qyan@cern.ch
 */
class TkLayerN: public TkPlaneN{

 public:
  map<int, TkLadderN> ladders_;///<ladders in this layer

 public:
  TkLayerN();
  TkLayerN(int layid,int laytype);
  TkLadderN *AddLadder(TkLadderN lad);
  /// Get number of ladders in this layer
  int        nLadders(){return ladders_.size();}
  /// Get the pointer of the ladder in this layer, tkid: ladder ID
  TkLadderN *GetLadder(int tkid);
  /// Find the sensor in this layer closest to the particle trajectory by interpolation with particle coordinate (gcoo) and direction (gdir)
  /*!
   * \param[out] dist       distance to the closest sensor, <=0 trajectory go though inside the sensor area, >0 trajectory outside of sensors area
   * \param[in]  gcoo       particle position in global coordinate system
   * \param[in]  gdir       particle direction in global coordinate system
   * \param[in]  striparea  1 judgement in sensor active area, 0 judgement in full sensor area
   * \return     pointer of the closest sensor 
   */
  TkSensorN *FindSensor(double &dist,TVector3 gcoo,TVector3 gdir,bool striparea=0);
  /// Calculated coordinate in layer with sensor+ladder alignment, the same as TkLadderN::GetInLayerCoo
  /*!
   * \param[in] tkid      ladder ID
   * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder in X
   * \param[in] ladchany  channel ID in ladder(Y), [-0.5, 639.5], ladchany=-10 means in the middle of ladder in Y
   * \param[in] ualign
   * \return    local coordinate in sensor and global coordinate in layer
   */
  AMSPlaneM  GetLocalCoo(int tkid,double ladchanx,double ladchany,int ualign=11);
  /// Calculated coordinate in tracker with sensor+ladder+layer alignment, the same as TkTrackN::GetGlobalCoo
  /*!
   * \param[in] tkid      ladder ID
   * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder in X 
   * \param[in] ladchany  channel ID in ladder(Y), [-0.5, 639.5], ladchany=-10 means in the middle of ladder in Y
   * \param[in] ualign
   * \return    local coordinate in sensor and global coordinate in tracker
   */
  AMSPlaneM  GetGlobalCoo(int tkid,double ladchanx,double ladchany,int ualign=111);
};


//! A class used for storing tracker alignment data for a time-bin (New V6 version)
/*!
 * \author qyan@cern.ch
 */
class TkAlignParN {

 public:
  unsigned int atime[2];///<begin and end time
  map<int, pair<TVector3,TVector3> > apar;///<alignment parameters, format: detid, alignment parameters
  map<int, pair<TVector3,TVector3> > apae;///<alignment parameter errors, format: detid, alignment parameter errors
  
 public:
  TkAlignParN();
  int GetSize(){return 2+12*apar.size()+12*apae.size();}///<Calculate the data size
  int Clear();///<Clear the data
  static int ConvertId(int uid,int opt=0);///<opt=0 label->detid, opt=1 detid->label
};


//! A class used for storing tracker alignment data function for a time period (New V6 version)
/*!
 * \author qyan@cern.ch
 */
class TkAlignParS {

 private:
  TSpline3 *spaf;///<alignment parameters time-dependent function 

 public:
  int label;///<detid 
  unsigned int atime[2];///<begin and end time
  map<unsigned int, double> spar;///<time,var are the fitting nodes of the spline function

 public:
  TkAlignParS();
  int GetSize(){return 3+2*spar.size();}///<Calculate the data size
  int Clear();///<Clear the data
  virtual ~TkAlignParS(){Clear();}
  TkAlignParS& operator=(const TkAlignParS &);
  int BuildFun();///<Build the alignment function using the stored parameters
  double GetAlignPar(double ptime);///<Calculate time-dependent alignment parameter
};


class AMSTimeID;
//! A class used for managing the whole tracker alignment data (New V6 version)
/*!
 * \author qyan@cern.ch
 */
class TkAlignParDB {
 
 public:
  /// Alignment parameters (external or inner alignment), format: time, alignment pars
  map<unsigned int, TkAlignParN> aparns;
  /// Alignment parameter functions (external alignment), format: detid, map(time, alignment pars)
  map<int, map<unsigned int, TkAlignParS> >aparss;
  TkAlignParN  aparbias;///<alignment global-geometry bias
  double       ainvbias;///<rigidity-scale bias
  int          abiasvers;///<bias version
  TkAlignParN *pparn;///<current alignment parameters from aparns
  map<int, TkAlignParS*> ppars;///<current alignment parameter functions from aparss, format: map(detid, alignment pars)
  TkAlignParN ppart;///<current additional time-dependent rigidity-scale correction
  pair<pair<double,int>, TkAlignParN> ppara[2];///<current total alignment parameters, format: (time,um), pars

 public:
  int    TDVVers;///<TDV version
  int    TDVSize;///<size of TDVBlock
  float  TDVBlock[40000];///<TDVBlock
  int    GetTDVLength(){return TDVSize*sizeof(TDVBlock[0]);}
  char   TDVName[2][100];///<TDV name
  unsigned int TDVTime[2];///<TDV time stamp
  AMSTimeID *tdv;

 public:
  TkAlignParDB();
  virtual ~TkAlignParDB();
  TkAlignParDB& operator=(const TkAlignParDB &);
  /// Calculate the required total size of TDV to store the alignment parameters
  /*!
   * \param[in] im  0 calculate the size of aparns, 1 calculate the size of aparss
   * \return    the required TDV size
   */
  int GetSize(int im);
  /// Clear alignment parameter database
  /*!
   * \param[in] opt  0||%10>=2 clear aparns, 1||%10>=2 clear aparss, >=10 clear aparbias and ainvbias 
   */
  int Clear(int opt);
  int InitTDV(const char *tname,int tvers,int tsize);
  /// Get rigidity-scale time dependent correction and convert to the alignment parameters and rigidity
  /*!
   * \param[out] pcor       alignment parameter correction 
   * \param[in]  rig        raw rigidity in GV
   * \param[in]  ptime      JMDC time
   * \param[in]  opt        0 raw-rigidity to corrected-rigidity, 1 corrected-rigidity to raw-rigidity
   * \param[in]  scalebias  additional rigidity-scale bias
   * \return     corrected rigidity in GV
   */
  static double GetRigTimeCor(TkAlignParN &pcor,double rig,double ptime,int opt=0,double scalebias=0);
  /// Calculate time-dependent alignment parameters and load to ppara[0]
  /*!
   * \param[in] ptime  JMDC time  
   * \param[in] force  1 force to load, 0 automatic
   * \param[in] um     apply alinment with DCBA(A-aparns,B-aparss,C-aparbias,D-rig-scale), -1 with all corrections but without time-dependent rigidity-scale correction, -2 with all corrections
   * \return    0 without update, 1 with update
   */
  int UpdateAlignPar(double ptime,int force,int um=-1);
  /// Copy alignment parameters to TDVBlock
  int CopyToTDVBlock(int wopt=0);
  /// Load alignment parameters from the file to the database
  int LoadAlignPar(const char *falign=0,bool usetdv=0,unsigned int rtime=1,int force=0,unsigned int etimeb=0,int wopt=0);
  /// Load alignment parameters from TDV to the database
  int LoadAlignTDV(long int rtime,int isreal=1,int force=0);//ua have to change to -1
  /// Copy TDV and alignment parameters from other
  int CopyAlignTDV(const TkAlignParDB &);
  /// Write alignment parameters from TDVBlock to the TDV file
  int WriteAlignTDV(long int btime,long int etime,int vers=1,int isreal=1);
  /// Write alignment parameters from TDVBlock to the file or iostream
  int StreamAlignTDV(const char *ofile=0,int wopt=0);
};


//! Tracker class for the calculation of the coordinate (New V6 version). This class manages the whole AMS tracker geometry.
/*!
 * accessor TkTrackN->TkLayerN->TkLadderN->TkSensorN, reverse-accessor by X->getmother() TkSensorN->TkLadderN->TkLayerN->TkTrackN
 * \author qyan@cern.ch
 */
class TkTrackN: public TkPlaneN{

 public:
  map<int, TkLayerN> layers_;///<tracker layers
  map<int, TkPlaneN*> modules_;///<pointers of all the tracker modules (sensors, ladders, and layers)
  static const int nalig=2;///<different alignment
  TkAlignParDB aligndb[nalig];///<alignment parameters database
  map<int, TkPlaneN*> modulea[nalig];///<pointers of the aligned tracker modules

 private:
  static int DefaultTDVVers[nalig];///<default TDV version
  static TkTrackN* fHead;
#pragma omp threadprivate (fHead) 
  static TkTrackN* fHeadG;

 public:
  TkTrackN();
  static TkTrackN* GetHead();///<threadprivate header
  static TkTrackN* GetHeadG();///<threadpublic header
  TkLayerN  *AddLayer(TkLayerN layer);
  /// Construct all the tracker modules with initial geometry parameters (without alignment)
  int        ConstructTracker();
  /// Get the pointer of the detector module (TkLayerN, TkLadderN, or TkSensorN) with detid 
  TkPlaneN  *GetModule(int detid);
  /// Get number of tracker layers
  int        nLayers(){return layers_.size();}
  /// Get the pointer of the layer, layid: 0-8(layJ)
  TkLayerN  *GetLayer(int layid);
  /// Get the pointer of the ladder, tkid: ladder ID
  TkLadderN *GetLadder(int tkid);
  /// Get the pointer of the sensor
  /*!
   * \param[in] tkid      ladder ID
   * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder
   * \return    pointer of the sensor
   */
  TkSensorN *GetSensor(int tkid,double ladchanx);
  /// Calculated coordinate in tracker with sensor+ladder+layer alignment, the same as TkLayerN::GetGlobalCoo
  /*!
    * \param[in] tkid      ladder ID
    * \param[in] ladchanx  channel ID in ladder(X), ladchanx=daqchanx+mult*384, ladchanx=-10 means in the middle of ladder 
    * \param[in] ladchany  channel ID in ladder(Y), [-0.5, 639.5], ladchany=-10 means in the middle of ladder in Y 
    * \param[in] ualign
    * \return    local coordinate in sensor and global coordinate in tracker
    */
  AMSPlaneM  GetGlobalCoo(int tkid,double ladchanx,double ladchany,int ualign=111);
  /// Calculated coordinate in tracker with sensor+ladder+layer alignment
  /*!
   * \param[in] tkid      ladder ID
   * \param[in] daqchanx  DAQ channel ID in X [639.5,1023.5] 
   * \param[in] mult      multiplicity in X
   * \param[in] daqchany  DAQ channel ID in Y [-0.5, 639.5]
   * \param[in] ualign
   * \return    local coordinate in sensor and global coordinate in tracker
   */
  AMSPlaneM  GetGlobalCoo(int tkid,double daqchanx,int mult,double daqchany,int ualign=111);
  int        UpdateModule();///<please UpdateModule() after copy constructor to reset pointers
  /// Update all detector planes O,U,V with the latest alignment parameters, ualign=-1 (or 111) updating all modules with all alignment
  int        UpdateAllDetOUV(int ualign=-1);
  /// Clear detector alignment parameters
  /*!
   * \param[in] ia  index of modulea, 0 or 1
   * \return    number of the cleared detector modules
   */
  int        ClearAlignPar(int ia);
  int        UseAlignVersion(int vers=0,int ua=-1);///<Use V6 alignment version
  /*!< 
   for ua=-1||ua=1 (inner+external static alignment): \n
   vers=-1:no alignment \n
   vers=0: default V6 static alignment version current using vers=7 \n
   vers=1: layer+ladder alignment based on 400 GeV/c test beam \n
   vers=2: layer+ladder+part sensor(L2-L8: u_s+v_s) alignment based on 400 GeV/c test beam \n
   vers=3: layer+ladder+part sensor(L2-L8: u_s+v_s and L1/L9: v_s) alignment based on 400 GeV/c test beam \n
   vers=4: layer+ladder+part sensor(L2-L8: u_s+v_s and L1/L9: u_s+v_s) alignment based on 400 GeV/c test beam \n
   vers=5: layer+ladder+part sensor(L2-L8: u_s+v_s+g_s and L1/L9: u_s+v_s) alignment based on 400 GeV/c test beam \n
   vers=6: layer+ladder+sensor ISS alignment(1st residual alignment only,bb8tp1c4he), bias in curvature which can only be used for external layer alignment \n
   vers=7: layer+ladder+sensor ISS alignment(1st residual+2nd curvature alignment,sa2441qnobias1g5) \n
   vers=8: layer+ladder+sensor ISS alignment(1st residual+2nd curvature alignment,sa2843qnobias1g5b) \n
   for ua=-1||ua=10 (iss external layer dynamic alignment+time-dependent rig-scale correction): \n
   vers=-1:no alignment \n
   vers=0: default V6 dynamic alignment version current using vers=2 \n
   vers=1: L1/L9 dynamic alignment based on "bb8tp1c4he" \n
   vers=2: L1/L9 dynamic alignment based on "sa2441qnobias1g5" \n
   vers=3: L1/L9 dynamic alignment based on "sa2843qnobias1g5b" \n
  */
  /// Load alignment external bias correction parameters
  /*!
   * \param[in] ia      1 index of aligndb, 0 or 1
   * \param[in] force   1 force to load, 0 automatic (only when alignment version changed)
   * \return    number of the updated aligndb
   */
  int      LoadAlignExtbiasCor(int ia,int force=0);
  /// Update time-dependent alignment parameters and load to the detector modules
  /*!
   * \param[in] rtime   JMDC time
   * \param[in] ia      index of aligndb, 0 or 1
   * \param[in] force   1 force to load, 0 automatic
   * \param[in] um      apply alinment with DCBA(A-aparns,B-aparss,C-aparbias,D-rig-scale), -1 with all corrections but without time-dependent rigidity-scale correction, -2 with all corrections
   * \param[in] biascor 1 with external bias correction (will call LoadAlignExtbiasCor), 0 without
   * \return    number of the updated detector modules
   */
  int        UpdateAlignModule(unsigned int rtime,int ia,int force=0,int um=-2);
  /// Load time-dependent alignment parameters from the file and update the detector modules
  int        LoadAlignModulePar(const char *falign=0,unsigned int rtime=1,int ia=0,int force=0,unsigned int etimeb=0,int wopt=0,int um=-2,bool biascor=0);
  /// Load time-dependent alignment parameters from TDV and update the detector modules
  /*!
   * \param[in] rtime   JMDC time
   * \param[in] ua      1 update aligndb[0] equivalent to ia=0, 10 update aligndb[1] equivalent to ia=1, <0 update all aligndb
   * \param[in] isreal  1 data, 0 mc
   * \param[in] force   1 recreate TDV and update alignment parameters, 0 automatic 
   * \param[in] um      apply alinment with DCBA(A-aparns,B-aparss,C-aparbias,D-rig-scale), -1 with all corrections but without time-dependent rigidity-scale correction, -2 with all corrections
   * \param[in] biascor 1 with external bias correction (will call LoadAlignExtbiasCor), 0 without 
   * \return    number of the updated detector modules, <0 error
   */
  int        LoadAlignModuleTDV(unsigned int rtime,int ua=1,int isreal=1,int force=0,int um=-2,bool biascor=1);
  int        CopyAlignTDVModule(const TkTrackN *other,unsigned int rtime,int um=-2,bool biascor=1);
};

#endif
