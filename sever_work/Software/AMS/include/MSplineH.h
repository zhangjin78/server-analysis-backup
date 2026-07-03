#ifndef __MSplineH_h__
#define __MSplineH_h__

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TFitResult.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TMath.h"
#include "TSpline.h"
#include "TGraphSmooth.h"
//#include "Math/Minimizer.h"
//#include "Math/Factory.h"
//#include "Math/Functor.h"
//#include "Math/Integrator.h"
//#include "Math/IntegratorMultiDim.h"
//#include "Math/AllIntegrationTypes.h"
//#include "Math/GaussIntegrator.h"
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

// class implementing my monotonic cubic sp-line 
class MSplineH : public TNamed { 

 public: 

  //! extrapolations kinds
  enum ExtraType { kLinear = 0, kFlat = 1, kLinearFree = 2 };

  //! number of knots 
  int     fN;
  //! number of knots+1 (only for array storage) 
  int     fM; 
  //! logx scale
  bool    fLogx;
  //! logy scale
  bool    fLogy;
  //! lower extrapolation kind 
  int     fExtraLow;
  //! upper extrapolation kind 
  int     fExtraHig;

  //! X knots (stored applying scale) 
  double* fX; //[fN]
  //! Y knots (stored applying scale) 
  double* fY; //[fN]
  
  //! vector fA[fN+1] of the spline
  double* fA; //[fM]
  //! vector fB[fN+1] of the spline
  double* fB; //[fM]
  //! vector fC[fN+1] of the spline
  double* fC; //[fM]
  //! vector fD[fN+1] of the spline
  double* fD; //[fM]
  //! vector X0[fN+1] of the spline
  double* fX0; //[fM]

  //! angular coefficient for linear extrapolation before first knot 
  double fBlow;
  //! angular coefficient for linear extrapolation after last knot
  double fBhig; 

  //! vector of spectral index integrals fSII[fN]
  double* fSII; //[fN]

  //! to disable monotonic behaviouur
  bool NonMonotonic;

  //! tf1 (for fitting)
  TF1*    fFunction; //! 
  //! result of the fit
  double fitquality[4];
  ROOT::Fit::FitResult* fFitResult; //!

  //! graph (for drawing) 
  TGraph* fGraph; //!  
  //! graph (for drawing) 
  TGraph* fShade; //!

  //! enlarging fitting window in case of no range indication
  static double EnlargeFitWindow;
  //! minimum for integration
  static double IntegrationMinimum; 

 public: 

  //! c-tor
  MSplineH() { Init(); }
  //! d-tor
  virtual ~MSplineH() { Release(); }
 
  //! c-tor with x knots
  MSplineH(int nknots, double* xknots, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.) { 
    Init();
    Set(nknots,xknots,logx,logy,extralow,extrahig,blow,bhig); 
  } 
  //! c-tor with xy knots
  MSplineH(int nknots, double* xknots, double* yknots, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.) { 
    Init();
    Set(nknots,xknots,yknots,logx,logy,extralow,extrahig,blow,bhig);
  }
  //! c-tor with TGraph
  MSplineH(int nknots, double* xknots, TGraph* graph, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.) { 
    Init();
    Set(nknots,xknots,graph,logx,logy,extralow,extrahig,blow,bhig);
  }
  //! c-tor with interpolations of TH1
  MSplineH(int nknots, double* xknots, TH1* histo, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.) { 
    Init();
    Set(nknots,xknots,histo,logx,logy,extralow,extrahig,blow,bhig);
  }
  //! c-tor with TH1 (or its errors)
  MSplineH(TH1* histo, bool logx = false, bool logy = false, bool error = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.) {
    Init();
    Set(histo,logx,logy,error,extralow,extrahig,blow,bhig);
  } 
  //! c-tor with TGraph
  MSplineH(TGraph* graph, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.) {
    Init();
    Set(graph->GetN(),graph->GetX(),graph,logx,logy,extralow,extrahig,blow,bhig);
  }

  //! initializer
  void    Init();

  //! set from x knots
  void    Set(int nknots, double* xknots, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.); 
  //! set from xy knots
  void    Set(int nknots, double* xknots, double* yknots, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.);
  //! set from a TGraph
  void    Set(int nknots, double* xknots, TGraph* graph, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.);
  //! set from interpolations of TH1
  void    Set(int nknots, double* xknots, TH1* histo, bool logx = false, bool logy = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.);
  //! set from a TH1 (or its errors)
  void    Set(TH1* histo, bool logx = false, bool logy = false, bool error = false, int extralow = kLinear, int extrahig = kLinear, double blow = 0., double bhig = 0.);

  //! number of knots
  int     GetN() { return fN; } 
  //! x knots (as stored)
  double* GetX() { return fX; }
  //! y knots (as stored) 
  double* GetY() { return fY; } 
  //! x knots 
  double  GetXknot(int i) { if ( (i>=fN)||(i<0) ) return 0.; return (fLogx) ? pow(10.,fX[i]) : fX[i]; }
  //! y knots
  double  GetYknot(int i) { if ( (i>=fN)||(i<0) ) return 0.; return (fLogy) ? pow(10.,fY[i]) : fY[i]; } 
  //! set linear extrapolation
  double  GetBlow() { return fBlow; }
  //! set linear extrapolation  
  double  GetBhig() { return fBhig; }
  //! set y knots 
  void    SetY(const double* yknots);
  //! set y knots from a graph  
  void    SetY(TGraph* graph);
  //! set y knots from a graph  
  void    SetY(TGraphErrors* graph);
  //! set y knots from an histogram
  void    SetY(TH1* histo);
  //! set linear extrapolation
  void    SetBlow(double blow) { fBlow = blow; CalculateCoefficients(); } 
  //! set linear extrapolation  
  void    SetBhig(double bhig) { fBhig = bhig; CalculateCoefficients(); }
  //! calculate coefficients 
  void    CalculateCoefficients();
  //! calculate integral
  void    CalculateSpectralIndexIntegral(); 
  //! find bin 
  int     FindBin(double x);
  //! find bin from y, assuming monotonic and positive slope
  int     FindBinY(double y);
  //! evaluate 
  double  Eval(double x);
  //! evaluate error (only != 0 after fit) 
  double  EvalError(double x) { return FitEvalError(x); } 
  //! eval derivative, dy/dx
  double  EvalDerivative(double x);
  //! eval spectral index, dlog_{10}y/dlog_{10}x
  double  EvalSpectralIndex(double x);
  //! eval indefinite integral
  double  EvalSpectralIndexIntegral(double x);
  //! fitting operator 
  double  operator() (double *x, double *par); 
  //! draw (as a graph)  
  void    Draw(Option_t *opt, int color, double width, int npx = 1000, double xmin = 0, double xmax = 0);
  //! fit a graph in a range  
  void    Fit(TGraphErrors* graph, double xmin = 0, double xmax = 0, int nfix = 0, int* ind_fix = 0, double* par_fix = 0);
  //! get chi2 
  double  GetChi2(int index=0);
  //! fit degrees of freedom
  int     GetNDofF();
  //! print 
  void    Print();
  //! smooth 
  void    Smooth();
  //! integral
//  double  Integral(double xmin, double xmax); 
  //! evaluate multiple inverse solution 
  void    EvalInverseMultiple(double y, int& n, double* x,double x0=0);
  //! evaluate simple inverse solution, assuming monotonic and positive slope
  double  EvalInverseSimple(double y);
  TGraph* GetInverse(int Np,double x0=0);
  double GetSlope(int binx);
  double GetSlope(double x);
  TGraph* GetSlope();

  //! real solutions of quadratic equation
  static void solution_quadratic(double* par, int& nrealsol, double* x);
  //! real solutions of cubic equation     
  static void solution_cubic(double* par, int& nrealsol, double* x);

  bool MultipleLimit(double &ylow,double &yhig,double &xlow,double &xhig,double &xtmin,double &xtmax);
  bool Revise(bool changeknot=false);

 public: // private: 
  
  //! allocate memory 
  void    Allocate(int nknots, double* xknots, bool logx = false, bool logy = false, int extralow = 0, int extrahig = 0, double blow = 0., double bhig = 0.);
  //! release memory
  void    Release(); 
  //! build graph
  void    BuildGraph(int npx = 1000, double xmin = 0, double xmax = 0);
  //! build function 
  void    BuildFunction(double xmin, double xmax);
  //! eval fit == eval (only for check) 
  double  FitEval(double x);
  //! eval fit error
  double  FitEvalError(double x, double cl = 0.6827);

 public:

  using TObject::Draw;
  using TObject::Print;
  ClassDef(MSplineH,1);

};


// class for fitting procedure 
class MSplineFitSamplingH : public TNamed {

 public:

  //! graph to be fitted 
  TGraphErrors* fGraph;

  //! number of samples 
  int           fnSamples;
  //! samples 
  MSplineH**     fSamples; //[fnSamples]
  //! flag 
  int*          fFlags; //[fnSamples]           

  //! graph (for drawing) 
  TGraphErrors* fDrawGraph; //!
  //! error shade (for drawing) 
  TGraph*       fDrawShade; //! 
  TF1*          fFunction;

  //! maximum allowed chisquared 
  static double fChi2Max;
  //! use of average instead of best fit
  static bool   fUseAverage;    

  //! random generator 
  static TRandom3* fRandom; 

 public: 

  //! c-tor
  MSplineFitSamplingH(TGraphErrors* graph);
  //! d-tor 
  virtual ~MSplineFitSamplingH();

  //! perform fit 
  void   Fit(int nbins, double* bins, bool logx, bool logy, int nsamples, double xmin, double xmax, int extralow = MSplineH::kLinear, int extrahig = MSplineH::kLinear, double blow = 0., double bhig = 0., int flag = 0, bool IsRandom=true);
  //! are the fits performed? (something better?)
  bool   IsThereAnyFlag(int flag = 0);
  //! best chi2 fit
  MSplineH* GetBestFit(int flag = 0);
  //! eval
  double Eval(double x, int flag = 0);
  //! eval statistical error
  double EvalErrorStatistics(double x, int flag = 0);
  //! eval sampling error 
  double EvalErrorSampling(double x, int flag = 0);
  //! eval discard between fit sets
  double EvalErrorSystematics(double x, int flag = 0);
  //! eval error (sum of the previous) 
  double EvalError(double x, int flag = 0);
  //! fit result
  TGraphErrors* GetFitGraph();
  //! draw (as fitting procedure result)
  void DrawResultPanel(double xmin, double xmax, bool logx = false, bool logy = false); 
  //! draw (as a TGraphErrors)  
  void Draw(int color, int color_area, int fill_style, double width, int npx, double xmin, double xmax);

 public: // private:

  //! release memory
  void Release();
  //! build graph
  void BuildGraph(int npx, double xmin, double xmax);

  double operator() (double *x,double *par);
  void BuildFunction(int npx, double xmin, double xmax);

 public:
  
  using TObject::Draw;
  ClassDef(MSplineFitSamplingH,1); 

}; 


#endif 
