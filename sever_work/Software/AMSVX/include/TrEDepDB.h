// $Id: TrEDepDB.h,v 1.6 2017/06/27 07:59:12 oliva Exp $

#ifndef __TrEDepDB__
#define __TrEDepDB__


#include "TkDBc.h"
#include "TkLadder.h"
#include "MonoSpline.h"


#include "TObject.h"
#include "TFile.h"
#include "typedefs.h"
#include "TH2D.h"
#include "TMath.h"
#include "TGraph.h" 


#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <map>


//! \c TrEDepTableI is a common interface to E_{Dep} parametrisation tables
class TrEDepTableI : public TObject {

 public: 

  //! Number of EDep profiles in the table
  int  fNCharges;
  //! Charges array (fNCharges)
  int* fZ;

 public:

  //! Constructor 
  TrEDepTableI();
  //! Default destructor
  virtual ~TrEDepTableI() { Clear(); }
  //! Clear
  virtual void Clear(Option_t* option = "");
  //! Clear and initialization
  void Init(int nch);

  //! Get number of EDep profiles in the table
  int  GetNCharges() { return fNCharges; }
  //! Get i-th profile charge value
  int  GetZ(int ich);
  //! Set i-th profile charge value
  bool SetZ(int ich, int z);

  //! Interpolate for a given Q, beta or beta/rigidity correction 
  virtual double GetCorrectedValue(double Q, double beta, double rigidity, double mass_on_Z, int type) = 0;
  //! Get the Q for a give Z in case of beta or beta/rigidity parametrization [c.u.]
  virtual double GetQ(int Z, double beta, double rigidity , double mass_on_Z, int type) = 0;
  //! Find the beta corresponding to Q along Z line 
  virtual double FindBeta(int Z, double Q, int type, double beta_min, double beta_max) = 0;

  //! line + plolynomial + line
  static double line_to_line_fun(double *x, double *par);
  //! log line-to-line + exponential tail
  static double edep_correction_function(double *x, double *par);

  //! Naive evaluation of mass over Z from Z (minimize the mass problem)
  static double GetNaiveMassOnZ(double Q);

  ClassDef(TrEDepTableI,1);
};


//! \c TrEDepTable contains single charge table for the betagamma correction.
class TrEDepTable : public TrEDepTableI {

 public: 

  //! Number of parameters for EDep profile functional description
  int fNPars;
  //! Parameters array (fNPars*fNCharges)
  double* fPars; 

 public:

  //! Constructor 
  TrEDepTable();
  //! Default destructor
  ~TrEDepTable() { Clear(); }
  //! Clear
  void    Clear(Option_t* option = "");
  //! Clear and initialization
  void    Init(int nch, int npar);

  //! Get number of parameters for EDep profile functional description
  int     GetNPars() { return fNPars; }
  //! Get parameters of a specific profile
  double  GetPar(int ich, int ipar);
  //! Set parameters of a specific profile
  bool    SetPar(int ich, int ipar, double par);

  //! Read table from a txt file
  bool    InitTableFromTxtFile(char* filename);

  //! Interpolate for a given Q, beta or beta/rigidity correction (if mass_on_Z <= 0 make an educated guess for mass_on_Z) [c.u.]
  double  GetCorrectedValue(double Q, double beta, double rigidity = 0, double mass_on_Z = 0, int type = 0);
  //! Get the Q for a give Z in case of beta or beta/rigidity parametrization [c.u.]
  double  GetQ(int Z, double beta, double rigidity = 0, double mass_on_Z = 0, int type = 0);
  //! Find the beta corresponding to Q along Z line
  double  FindBeta(int Z, double Q, int type = 0, double beta_min = 0.2, double beta_max = 0.94);

  //! Calculate the betagamma used for fitting/correcting values
  static double GetBetaGamma(double beta, double rigidity, double mass_on_Z, int type);
  //! Calculate the log10(betagamma) used for fitting/correcting values  
  static double GetLogBetaGamma(double beta, double rigidity, double mass_on_Z, int type);

  ClassDef(TrEDepTable,2);
};


//! \c TrEDepTableSpline contains a single charge table for the betagamma correction.
class TrEDepTableSpline : public TrEDepTableI {

 public:

  //! Monotonic splines array (1st round) 
  MonoSpline* fSplineFoldBeta;
  //! Monotonic splines array (2nd round) 
  MonoSpline* fSplineRigidity;

 public:

  //! Constructor 
  TrEDepTableSpline();
  //! Default destructor
  ~TrEDepTableSpline() { Clear(); }
  //! Clear
  void Clear(Option_t* option = "");
  //! Clear and initialization
  void Init(int nch, int which);

  //! Read table from a txt file
  bool   InitTableFromTxtFile(char* filename, int which);
  //! Interpolate for a given Q, beta or beta/rigidity correction (if mass_on_Z <= 0 make an educated guess for mass_on_Z) [c.u.]
  double GetCorrectedValue(double Q, double beta, double rigidity = 0, double mass_on_Z = 0, int type = 0);
  //! Get the Q for a give Z in case of beta or beta/rigidity parametrization [c.u.]
  double GetQ(int Z, double beta, double rigidity = 0, double mass_on_Z = 0, int type = 0) { return 0; }
  //! Find the beta corresponding to Q along Z line
  double FindBeta(int Z, double Q, int type = 0, double beta_min = 0.2, double beta_max = 0.94) { return 0; }

  ClassDef(TrEDepTableSpline,1);
};


//! \c TrEDepDB contains all the charge loss informations.
class TrEDepDB : public TObject {

 public:
 
  //! Map for fast binary search
  static map<int,TrEDepTableI*> fTrEDepMap;
  //! Array for fast lookup
  static TrEDepTableI** fTrEDepArray; 
  //! Singleton
  static TrEDepDB* fHead;
  //! Init done
  static bool fInitDone;

 public:

  //! Fix version
  static int fUsedVersion; 
  //! Boosting correction
  static int fBoostBetaCorrection; 
#pragma omp threadprivate(fBoostBetaCorrection)

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrEDepDB() { Clear(); }
  //! Destructor
  ~TrEDepDB(); 
  //! Get self-pointer
  static TrEDepDB* GetHead();
  //! Existence checker
  static bool  IsNull() { return (fHead==0) ? true : false; } 
  //! Needed init (loading interface for GBATCH)  
  void  Init(bool force = false);
  //! Clean up the table database
  void  Clear(Option_t* option = "");
 
  //! Create index
  int   CreateIndex(int iside, int jlayer, int ver = fUsedVersion) { return jlayer + iside*9 + ver*2*9; }
  //! Add table to the database
  bool  AddTable(int index, TrEDepTableI* table);
  //! Add table to the database by type and Z
  bool  AddTable(TrEDepTableI* table, int iside, int jlayer, int ver = fUsedVersion) { return AddTable(CreateIndex(iside,jlayer,ver),table); }
  //! Retrieve table 
  TrEDepTableI* GetTable(int index);
  //! Retrieve table by type and Z
  TrEDepTableI* GetTable(int iside, int jlayer, int ver = fUsedVersion) { return GetTable(CreateIndex(iside,jlayer,ver)); }

  //! Get the corrected value
  double GetEDepCorrectedValue(int jlayer, double Q, double beta, double rigidity = 0, double mass_on_Z = 0, int iside = 0, int ver = fUsedVersion);

  //! Top of the intrument correction (top of layer 1), by default use layer 1 and 5 Z=1 tables
  double GetTOIBeta(double beta, int Z = 1, int inn_jlayer = 5, int iside = 0, int ver = fUsedVersion);

  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer1(char* dirname = getenv("AMSDataDir"));
  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer2(char* dirname = getenv("AMSDataDir"));

  ClassDef(TrEDepDB,2);
};

#endif

