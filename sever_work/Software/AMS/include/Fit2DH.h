#ifndef __Fit2DH_h__
#define __Fit2DH_h__

#include "MSplineH.h"

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TF12.h"

#include "TPaveText.h"

//#include "Tools.h"

#include <fstream>
#include <cmath>
#include <cfloat>

using namespace std;

class Fit2DH : public TNamed {

  public:
  static int  SkipOpt;
  static bool ErrCal;

  enum ExtraType { kLinear = 0, kFlat = 1, kLinearFree = 2 };
  bool xfirst;

  int      fN;  //number of knots for the second axis
  double   val_sec[1000];
  TH1D*    hImport1D[1000];
  TGraphErrors*  gImport1D[1000];
  MSplineFitSamplingH* fMSplineH_fir[1000];
  MSplineH* fMSplineH_sec;
  TH2D*    hImport2D;

  TH2D* fitresidual[2];

  TF2*     fFunction;

  TF12*     fXFunction;
  TF12*     fYFunction;

  double min_fir;
  double max_fir;
  double min_sec;
  double max_sec;

  //int nbins_sec;
  //double bins_sec[100];
  bool logx_sec;
  bool logy_sec;
  int extralow_sec;
  int extrahig_sec;
  double blow_sec;
  double bhig_sec;

  void Init();
  void Release();
  Fit2DH(){ Init(); }
  Fit2DH(int nbins, double* bins,double ymin,double ymax, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.){
     Init();
     min_sec=ymin;
     max_sec=ymax;
     //nbins_sec=nbins;
     //for(int i=0;i<=nbins_sec;i++) bins_sec[i]=bins[i];
     logx_sec=logx;
     logy_sec=logy;
     extralow_sec=extralow;
     extrahig_sec=extrahig;
     blow_sec=blow;
     bhig_sec=bhig;
  }
  virtual ~Fit2DH() { Release(); }

  void Add(TGraphErrors* graph, double val0_sec, int nbins, double* bins, double xmin=0, double xmax=0, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.);
  void Add(TH1D* histo        , double val0_sec, int nbins, double* bins, double xmin=0, double xmax=0, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.);
  void Add(TH2D* histo, int nxbin, double* xbins, int nybin, double *ybins, double xmin=0, double xmax=0, double ymin=0,double ymax=0, int rebinx=1, int rebiny=1, bool logx = false, bool logy = false, bool logz=false, int extraxlow=kLinear, int extraxhig=kLinear, double bxlow=0., double bxhig=0., int extraylow=kLinear, int extrayhig=kLinear, double bylow=0., double byhig=0.);
  int  SearchIndex(double value_sec);
 
  void reordering();

  double Eval(double &error,double xx,double yy,bool UseNewAlgo=false);
  double operator() (double *x, double *par);
  void BuildFunction(double xmin=0, double xmax=0,double ymin=0,double ymax=0,int Np=20);
  TH2D* GetFitRes(bool sigmaunits=false);
  int IsFittingGood(double maxlimit,bool sigmaunits=false);
  void DrawXYFitting(bool sigmaunits=false);
  void DrawXFitting(double yy,bool drawfitting=true);
  void DrawXFitting(int biny,bool drawfitting=true);
  void DrawYFitting(double xx,bool drawfitting=true);
  void DrawYFitting(int binx,bool drawfitting=true);
  double GetChi2Max(int bin=-1,int type=0);

  bool DumpMap(char* filename);
  bool ReadMap(char* filename);

  ClassDef(Fit2DH,1);
};

#endif
