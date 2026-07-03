// $Id: TrLinearDB.h,v 1.15 2022/02/12 17:10:36 qyan Exp $

#ifndef __TrLinearDB__
#define __TrLinearDB__


#include "TkDBc.h"
#include "TkLadder.h"


#include "TObject.h"
#include "TFile.h"
#include "typedefs.h"
#include "TH2D.h"
#include "TSpline.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

/// @cond CALDOX
//! \c TrLinearElem contains informations about p-strip linearization of a single element (VA, ladder, layer or tracker)
class TrLinearElem : public TObject {

 private: 

  //! Element index (0: whole tracker; 1, ...,9: layer; (0, ..., 192)+10: ladder+10, ((0, ..., 192)+10)*100 + (0, ..., 9): VA)
  int    fIndex;
  //! Performed fit was succesfull 
  int    fSucc;
  //! Chi squared of the fit 
  double fChisq;
  //! Parameters
  double fPars[5];
  //! Temperature correction on fPars
  double fTcor[5];

 public:
  
  //! c-tor
  TrLinearElem(int index, int succ, double chisq, double* pars, double* tcor = 0);
  //! c-tor
  TrLinearElem(int index, int succ, float chisq, float* pars, float* tcor = 0);
  //! c-tor (default)
  TrLinearElem() : fIndex(-1), fSucc(-1), fChisq(99999) {
    for (int i = 0; i < 5; i++) fPars[i] = fTcor[i] = 0;
  }
  //! d-tor
  ~TrLinearElem() {}
  //! Info
  void   Info();
  //! Get index
  int    GetIndex() { return fIndex; } 
  //! Is the correction OK?
  int    GetSucc()  { return fSucc; } 
  //! Get chisquare of the fit 
  double GetChisq() { return fChisq; } 
  //! Get parameter pointer
  double GetPar(int ipar)  { return ( (ipar>=0)&&(ipar<5) ) ? fPars[ipar] : -9999; } 
  //! Is the element valid (good description of the correction) ?
  bool   IsValid();
  //! Return corrected value with temperature correction
  double GetLinearityCorrected(double ADC, double temp = 0); 
  //! Return application of non-linearity
  double ApplyNonLinearity(double ADC);
  //! Method for linerity correction inversion 
  void   GetInterval(double y, double xmin, double xmax, int nsteps, double& xmin_step, double& xmax_step);

  //! Size of the TDV entry 
  static int GetSize() { return 1+1+1+5; }
  //! Interface to TDV linear database
  bool DBToLinear(float* offset);
  //! Interface to TDV linear database
  bool LinearToDB(float* offset);


  using TObject::Info;
  ClassDef(TrLinearElem,2);
};


//! \c TrLinearDB contains informations about p-strip linearization (mostly for the ions resolution improvement).
class TrLinearDB : public TObject {

 private:

  //! Map for fast binary search
  static map<int,TrLinearElem*> fTrLinearElemIndexMap;
  //! Singleton
  static TrLinearDB* fHead;

  //! Interface for TDV storing
  static float* fLinear; 
  //! Clear contents or not when loading from TDV
  static bool fClear;
  //! Run number in case of applying for the temperature correction
  static int fRun;
  //! Inner tracker temperature (sensor A)
  static double fTemp;

 public:

  //! Default correction depth 
  static int DefaultCorrDepth;

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrLinearDB() { Clear(); }
  //! Destructor
  ~TrLinearDB(); 
  //! Get self-pointer
  static TrLinearDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Set run number for temperature correction
  void   SetRun(int run) { fRun = run; }
  //! Needed init (loading interface for GBATCH)  
  void   Init();
  //! Clean up the table database
  void   Clear(Option_t* option = "");
  //! Info
  void   Info();
  //! Create index from TkId and VA number for requested depth 
  int    CreateIndex(int tkid, int iva, int depth = DefaultCorrDepth);
  //! Add element to the database
  bool   AddElem(int index, TrLinearElem* elem);
  //! Get element from index
  TrLinearElem* GetElem(int index); 
  //! Get element from TkId, VA and depth
  TrLinearElem* GetElem(int tkid, int iva, int depth = DefaultCorrDepth);
  //! Get valid element from TkId, VA and depth
  TrLinearElem* GetValidElem(int tkid, int iva, int depth = DefaultCorrDepth);
  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer0(char* dirname = getenv("AMSDataDir"));
  //! Get the linearity correction at different level (depth: 0: tracker; 1: layer; 2: ladder; 3: VA) 
  double GetLinearityCorrected(double ADC, int tkid, int iva, int depth = DefaultCorrDepth);
  //! Return application of non-linearity (depth: 0: tracker; 1: layer; 2: ladder; 3: VA)
  double ApplyNonLinearity(double ADC, int tkid, int iva, int depth = DefaultCorrDepth);

  //! Set datacard option
  static void SetUseNonLinearity(int opt, int readfromfile = 0);

  //! Linear database pointer
  static float* GetLinear() { return fLinear; }
  //! Size of the linear database 
  static int GetSize() { return (192*10+192+9)*TrLinearElem::GetSize(); }
  //! Size of the TDV entry (bytes)
  static int GetLinearSize() { return GetSize()*sizeof(float); }
  //! Interface to TDV linear database
  bool DBToLinear();
  //! Interface to TDV linear database
  bool LinearToDB();
  //! Dump the linear database
  void PrintLinear(); 

  //! Save in TDV 
  bool SaveInTDV(long int start_time, long int validity = 31536000, int isReal = 1);
  //! Load from TDV
  int  LoadFromTDV(long int time, int clear  = 0,
		                  int isReal = 1, bool force = false);

  using TObject::Info;
  ClassDef(TrLinearDB,1);
};

//! Functional description of the p-side strip behavior
double p_strip_behavior(double* x, double* par);
/// @endcond

class AMSTimeID;


//! \c TrLinearEtaDB tracker p-strip eta linearization
/*!
 * \author qyan@cern.ch
 */
class TrLinearEtaDB : public TObject {

 private:
   static TrLinearEtaDB* fHead;
#ifdef __ROOTSHAREDLIBRARY__
#ifndef __PPC64
#pragma omp threadprivate (fHead) 
#endif
#endif
   static const int nMaxZ=30;
//-----asymmetry
   double parasym[2][9];//TkIdS/Layer
   double aligsym[2][9];//TkIdS/Layer
//-----mip
   double parlaymip[9][12]; 
   int    nvamip;
   int    vamip[2000];
   double parvamip[2000][12];
   map<int, int > vamipi;
//-----mip2
   static const int nMaxZ2=8;//Z=1-8
   int    nzmip2;
   int    zmip2[nMaxZ2];
   int    nvamip2;
   int    vamip2[2000];
   double parvamip2[2000][nMaxZ2];
   map<int, int > vamip2i;
//-----coupling
   int    nzcoup[2];
   int    zcoup[2][nMaxZ];
   double parcoup[2][nMaxZ][9][2];//TkIdS/Z/Layer/par
//-----linear model
   int    nzcor[2];
   int    zcor[2][nMaxZ];
   double parcor[2][nMaxZ][9][3];//TkIdS/Z/Layer/par
//-----parabolic model
   int    nzcorp[2];
   int    zcorp[2][nMaxZ];
   double parcorp[2][nMaxZ][9][2];//TkIdS/Z/Layer/par
   int    nzcor2[2];
   int    zcor2[2][nMaxZ];
   double parcor2[2][nMaxZ][9][3];//TkIdS/Z/Layer/par
   map<int, int> zcori[2];//(z,index+z*100+10000*imodel)
 public:
   const char *TDVName;
   int    TDVfile[20];
   float *TDVBlock;
   int    TDVSize;
   int    GetTDVLength(){return TDVSize*sizeof(float);}
   AMSTimeID *tdvload;
 public:
  //! Constructor (declared public because of ROOT I/O), not to be used
  TrLinearEtaDB();
  //! Destructor
   ~TrLinearEtaDB();
  /// Version
  /*!
   2: asymmetry-Eta \n
   3: symmetry-Eta with symmetric DAQ R/L correction(default) \n
  */
  static int Version;
  //! Get self-pointer
  static TrLinearEtaDB* GetHead();
  //! Head Load TDVPar
  static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
  /// Active TrCluster coordinate reconstruction by LinearEta algorithm
  /*!
    \param[in] opt    0:    default mode
		      !=0:  manual  mode(TRCLFFKEY.ClusterCofGOpt=opt)
    \param[in] force  0:    event by event
		      1:    all events(TRCLFFKEY_DEF::ReadFromFile=0) 
    \return    TRCLFFKEY.ClusterCofGOpt
   */
  static int  SetLinearCluster(int opt = 0, int force = 0);
  //! Swith off TrCluster coordinate reconstruction with LinearEta algorithm
  static int  UnSetLinearCluster();
  //! Recalculate all hit coordinates using Qtrk/QXCluster/QYCluster with LinearEta algorithm
  static int  RecalAllHitCoo();
  //! Needed init (loading interface for GBATCH) 
  void   Init();
  //! Clean up the table database
  void   Clear(Option_t* option = "");
  //! Load parameters index from TDVBlock
  int  LoadZCorI();
  //! Load parameters from TDVBlock
  int  LoadTDVPar();
  //! Load from file
  int  LoadFromFile(const char *file[12]);//read data from file->TDVBlock
  //! Print TDV Block
  int  PrintTDVBlock();
  //! Save in TDV
  int  SaveInTDV(long int start_time, long int end_time, int isReal = 1);
  //! Load from TDV
  int  LoadFromTDV(long int time, int clear  = 0,
                                  int isReal = 1, bool force = false);
  //! Is Valid
  bool IsValid();
 public:
  //! Get Layer(0-8) and Ladder side(0-1, left-right) from  TkId
  int    ConvertLadderId(int tkid,int &il,int &is);
  //! Get VA Id
  int    GetVAId(int tkid,int address);
  //! Get Asymmetry Correction
  double GetAsymCorADC(double adc,double ladc,double radc,int tkid);
  //! Get Asymmetry Alignment offset
  double GetAsymAlign(int tkid);
  //! Get MIP0 Q->sqrt(ADC)
  double GetMIP0(double* x, double* par);
  //! Get MIP1 Q->sqrt(ADC)
  double GetMIP1(double* x, double* par);
  //! Get MIP Q->ADC
  double GetQMIP(double q,int vaid);
  //! Get Q2 Correction2 Z by Z
  double GetQ2Cor2(int z,int vaid);
  //! Is Good VA
  bool   IsGoodVA(int tkid,int address);
  //! ADC->Q2 with gain correction
  double GetLinearQ2(double adc,int tkid,int address,int z);
  //! Get Corrected Z index
  int    GetZCorI(int z, int is, int iz[2]);
  //! Decoupling second strip adc with seed strip adc 
  double GetCorSadc(double sadc,double cadc,double tadc,double dydz,int z,int il,int is);
  //!  Get Parabolic model parameter0
  double GetP2PV(double dydz,int z,int il,int is);
  //! Get Linear model Linearized Eta(no signed 0-0.5, 0: seed position, 0.5: sadc=cadc)
  double GetLinearEta1(double cadc,double sadc,double tadc,double dydz,int z,int il,int is);
  //! Get Parabolic model Linearized Eta(no signed 0-0.5, 0: seed position, 0.5: sadc=cadc)
  double GetLinearEta2(double cadc,double sadc,double tadc,double dydz,int z,int il,int is);
  //! Get Linearized Eta(no signed 0-0.5, 0: seed position, 0.5: sadc=cadc)
  double GetLinearEta(double cadc,double sadc,double tadc,double dydz,int z,int il,int is);
  //! Get Linearized Eta(no signed 0-0.5, 0: seed position, 0.5: sadc=cadc)
  double GetLinearEta(double cadc,double sadc,double tadc,double dydz,int z,int tkid);
  //! Get XCoo with Linearized Eta
  double GetLinearX(double cpos,double spos,double cadc,double sadc,double tadc,double dydz,int z,int tkid);
  using TObject::Info;
  ClassDef(TrLinearEtaDB,1);
};

/// @cond CALDOX
//! \c TrLinearXEtaDB tracker n-strip eta linearization 
/*!
 * \author qyan@cern.ch
 */
class TrLinearXEtaDB : public TObject {

 private:
  static TrLinearXEtaDB* fHead;
#ifdef __ROOTSHAREDLIBRARY__
#ifndef __PPC64
#pragma omp threadprivate (fHead) 
#endif
#endif
  static const int nPatt=4;//patt(2/3/4/5 strips)
  static const int nMaxZ=10;//charge
  static const int nMaxK=4;//type
  static const int nMaxA=9;//angle
  static const int nMaxP=10;//parameters
//-----asymmetry
  double parasym[9];//Layer
  double aligsym[9];//Layer
//----xnodes
  int    nxcor[nPatt];
  double xcor[nPatt][nMaxP];
//----z
  int    nzcor[nPatt];
  int    zcor[nPatt][nMaxZ];
//----angle
  int    ndxz[nPatt][nMaxZ][9][nMaxK];
  double udxz[nPatt][nMaxZ][9][nMaxK][nMaxA];
//----corv
  double ycor[nPatt][nMaxZ][9][nMaxK][nMaxA][nMaxP];
  TSpline3 *splinec[nPatt][nMaxZ][9][nMaxK][nMaxA];
  map<int, int> zcori[nPatt];
 public:
  const char *TDVName;
  int    TDVfile[20];
  float *TDVBlock;
  int    TDVSize;
  int    GetTDVLength(){return TDVSize*sizeof(float);}
  AMSTimeID *tdvload;
 public:
  //! Constructor (declared public because of ROOT I/O), not to be used
  TrLinearXEtaDB();
  //! Destructor
   ~TrLinearXEtaDB();
  /// Version
  static int Version;
  //! Get self-pointer
  static TrLinearXEtaDB* GetHead();
  //! Head Load TDVPar
  static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
  //! Needed init (loading interface for GBATCH) 
  void   Init();
  //! Clean up the table database
  void   Clear(Option_t* option = "");
  //! Load parameters index from TDVBlock
  int  LoadZCorI();
  //! Load parameters from TDVBlock
  int  LoadTDVPar();
  //! Load from file
  int  LoadFromFile(const char *file[6]);//read data from file->TDVBlock
  //! Print TDV Block
  int  PrintTDVBlock();
  //! Save in TDV
  int  SaveInTDV(long int start_time, long int end_time, int isReal = 1);
  //! Load from TDV
  int  LoadFromTDV(long int time, int clear  = 0,
                                  int isReal = 1, bool force = false);
  //! Is Valid
  bool IsValid();
 public:
  //! Get Layer(0-8) and Ladder side(0-1, left-right) from  TkId
  int    ConvertLadderId(int tkid,int &il,int &is);
  //! Get Asymmetry Correction
  double GetAsymCorADC(double adc,double ladc,double radc,int tkid);
  //! Get Asymmetry Alignment offset
  double GetAsymAlign(int tkid);
  //! Get Corrected Z index
  int    GetZCorI(int z, int ip, int iz[2]);
  //! Get Linearized CofG(signed -0.5-0.5,  0: seed position, 0.5: sadc=cadc)
  double GetLinearCofG0(double xcoo,int ip,double dxdz,int z,int il,int ik);
  //! Get Linearized CofG(signed -0.5-0.5,  0: seed position, 0.5: sadc=cadc)
  double GetLinearCofG(double xcoo,int upatt,double dxdz,int z,int tkid,int striptype);
  //! Get Linearized CofG(signed -0.50-0.5, 0: seed position, 0.5: sadc=cadc)
  double GetLinearCofG(double adc[5],int patt,int &upatt,double dxdz,int z,int tkid,int striptype);
  //! Get XCoo with Linearized Eta
  double GetLinearX(double adc[5],double pos[5],int patt,int &upatt,double dxdz,int z,int tkid,int striptype);
  using TObject::Info;
  ClassDef(TrLinearXEtaDB,1); 
};
/// @endcond

#endif

