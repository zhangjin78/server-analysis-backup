#ifndef __MonoSpline_h__
#define __MonoSpline_h__

#include "TObject.h"
#include "TNamed.h"
#include "TMath.h"

#include <iostream> 

using namespace std;

// class implementing my monotonic cubic sp-line 
class MonoSpline : public TNamed { 

 public: 

  //! number of knots 
  int     fN;
  //! X knots  
  double* fX; 
  //! Y knots  
  double* fY;
  //! fA[fN+1] coeff.
  double* fA; 
  //! fB[fN+1] coeff. 
  double* fB; 
  //! fC[fN+1] coeff.
  double* fC; 
  //! fD[fN+1] coeff.
  double* fD; 
  //! fX0[fN+1] coeff.
  double* fX0; 

  //! lower extrapolation (0: use linear extrapolation of last 2 knots, 1: flat extrapolation)
  int fExtraLow;
  //! upper extrapolation (0: use linear extrapolation of last 2 knots, 1: flat extrapolation)
  int fExtraHig;

 public: 

  //! c-tor
  MonoSpline() { Init(); }
  //! c-tor with xy knots
  MonoSpline(int nknots, double* xknots, double* yknots, int extralow = 0, int extrahig = 0) { Init(); Set(nknots,xknots,yknots,extralow,extrahig); }
  //! d-tor
  ~MonoSpline() { Release(); }

  //! initializer
  void    Init();
  //! allocate memory 
  void    Allocate(int nknots);
  //! release memory
  void    Release();
  //! set x/y knots
  void    Set(int nknots, double* xknots, double* yknots, int extralow = 0, int extrahig = 0);
  //! remove point
  void    RemoveKnot(int iknot);  

  //! calculate coefficients 
  void    CalculateCoefficients();
  //! find bin 
  int     FindBin(double x);
  //! evaluate 
  double  Eval(double x);

  //! is a monotonic function?
  bool    IsMonotonic();
  //! find bin inverse 
  int     FindBinInverse(double y);
  //! evaluate inverse
  double  EvalInverse(double y);

  //! dump
  void    Dump() const; 

  //! real solutions of quadratic equation
  static void solution_quadratic(double* par, int& nrealsol, double* x);
  //! real solutions of cubic equation     
  static void solution_cubic(double* par, int& nrealsol, double* x);

  ClassDef(MonoSpline,1);
};

#endif 
