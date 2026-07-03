#include "MonoSpline.h"

ClassImp(MonoSpline);

void MonoSpline::Init() {
  fN = 0;
  fX = 0;
  fY = 0;
  fA = 0;
  fB = 0;
  fC = 0;
  fD = 0;
  fX0 = 0;
  fExtraLow = 0;
  fExtraHig = 0;
}

void MonoSpline::Allocate(int nknots) { 
  fN = nknots;
  fX = new double[fN];
  fY = new double[fN];
  for (int i=0; i<fN; i++) {
    fX[i] = 0;
    fY[i] = 0;
  }
  fA = new double[fN+1];
  fB = new double[fN+1];
  fC = new double[fN+1];
  fD = new double[fN+1];
  fX0 = new double[fN+1];
  for (int i=0; i<fN+1; i++) {
    fA[i] = 0;
    fB[i] = 0;
    fC[i] = 0;
    fD[i] = 0;
    fX0[i] = 0;
  }
  fExtraLow = 0;
  fExtraHig = 0;
}

void MonoSpline::Release() {
  fN = 0; 
  if (fX)  { delete [] fX;  fX = 0; }
  if (fY)  { delete [] fY;  fY = 0; }
  if (fA)  { delete [] fA;  fA = 0; }
  if (fB)  { delete [] fB;  fB = 0; }
  if (fC)  { delete [] fC;  fC = 0; } 
  if (fD)  { delete [] fD;  fD = 0; } 
  if (fX0) { delete [] fX0; fX0 = 0; }
  fExtraLow = 0;
  fExtraHig = 0; 
}

void MonoSpline::Set(int nknots, double* xknots, double* yknots, int extralow, int extrahig) {
  if (nknots!=fN) {
    Release();
    Allocate(nknots);
  }
  for (int i=0; i<fN; i++) {
    fX[i] = xknots[i];
    fY[i] = yknots[i];
  }
  fExtraLow = extralow;
  fExtraHig = extrahig;
  CalculateCoefficients();
}

void MonoSpline::RemoveKnot(int iknot) {
  int n = fN-1;
  double* x = new double[n];
  double* y = new double[n];
  int index = 0;
  for (int i=0; i<fN; i++) {
    if (i==iknot) continue;
    x[index] = fX[i];
    y[index] = fY[i];
    index++;
  }
  Set(n,x,y);
  delete [] x;
  delete [] y;
}

void MonoSpline::CalculateCoefficients() {
  if (fN<=1) return; 
  // lower extrapolation 
  fA[0] = fY[0];
  fB[0] = (fExtraLow==1) ? 0 : (fY[1]-fY[0])/(fX[1]-fX[0]);
  fC[0] = 0;
  fD[0] = 0;
  fX0[0] = fX[0];
  // upper extrapolation 
  fA[fN] = fY[fN-2]; 
  fB[fN] = (fY[fN-1]-fY[fN-2])/(fX[fN-1]-fX[fN-2]); 
  fC[fN] = 0;
  fD[fN] = 0;
  fX0[fN] = fX[fN-2]; 
  if (fExtraHig==1) {
    fA[fN] = fY[fN-1];
    fB[fN] = 0;
    fX0[fN] = fX[fN-1];
  }
  // loop on all other bins 
  for (int ibin=0; ibin<fN-1; ibin++) {
    // Monotonic Cubic SP-line
    // M. Steffen, Astron. Astrophys. 239, 443-450 (1990)
    // A simple method for monotonic interpolation in one dimension
    // special treatment for boundary for extrapolation smooth matching
    int imin = (ibin==0)    ? 1 : 0;
    int imax = (ibin==fN-2) ? 2 : 3;
    double h[3]  = {0,0,0};
    double s[3]  = {0,0,0};
    double S[3]  = {0,0,0};
    double p[3]  = {0,0,0};
    double y1[3] = {0,0,0};
    for (int i=imin; i<imax; i++) {
      h[i] = fX[ibin-1+i+1]-fX[ibin-1+i];
      s[i] = (fY[ibin-1+i+1]-fY[ibin-1+i])/h[i];
      S[i] = (s[i]>=0) ? 1 : -1;
    }
    // set for first bin 
    y1[1] = (fExtraLow==1) ? 0 : s[1];
    // set for last bin 
    y1[2] = (fExtraHig==1) ? 0 : s[1];
    // monotonic correction 
    for (int i=imin+1; i<imax; i++) {
      p[i]  = (s[i-1]*h[i]+s[i]*h[i-1])/(h[i]+h[i-1]);
      y1[i] = (S[i-1]+S[i])*(TMath::Min(TMath::Min(fabs(s[i-1]),fabs(s[i])),0.5*fabs(p[i])));
    }
    fA[ibin+1] = fY[ibin];
    fB[ibin+1] = y1[1];
    fC[ibin+1] = (3*s[1] - 2*y1[1] - y1[2])/h[1];
    fD[ibin+1] = (y1[1] + y1[2] - 2*s[1])/(h[1]*h[1]);
    fX0[ibin+1] = fX[ibin];
  }
}

int MonoSpline::FindBin(double x) {
  if (x< fX[0]) return 0;
  if (x>=fX[fN-1]) return fN; 
  int l = (fN-1);
  int f = 0;
  int i = int(f+(l-f)/2);
  int ntry = 0;
  while (!((x>=fX[i])&&(x<fX[i+1])) ) {
    if (x< fX[i])   { l = i;   i = int(f+(l-f)/2); }
    if (x>=fX[i+1]) { f = i+1; i = int(f+(l-f)/2); }
    if (ntry++>fN) {
       cerr << "MonoSpline::FindBin-E-InfiniteCycle " << ntry << "/" << fN << " --> " << x <<endl;
       for (int i=0; i<fN; i++) cerr<<"MonoSpline::FindBin-E-InfiniteCycle: fX[" << i << "] = " << fX[i] <<endl;
       return 0;
    }
  }
  return i+1;
}

double MonoSpline::Eval(double x) {
  if (x!=x){
    cerr << "MonoSpline::Eval-E-NaNInput " << endl;
    return 0;
  }
  int i = FindBin(x);
  return fA[i]+fB[i]*(x-fX0[i])+fC[i]*pow(x-fX0[i],2.)+fD[i]*pow(x-fX0[i],3.); 
}

bool MonoSpline::IsMonotonic() {
  bool itis = true;
  int init_status = 0;
  for (int i=0; i<fN-1; i++) {
    if (init_status==0) init_status = (fY[i+1]>fY[i]) ? 1 : -1;
    else {
      int this_status = (fY[i+1]>fY[i]) ? 1 : -1;
      if (this_status!=init_status) itis = false;
    }
  }
  return itis;
}

int MonoSpline::FindBinInverse(double y) {
  // int init_status = 0;
  if (!IsMonotonic()) return 0; 
  if (y< fY[0]) return 0;
  if (y>=fY[fN-1]) return fN;
  int l = (fN-1);
  int f = 0;
  int i = int(f+(l-f)/2);
  int ntry = 0;
  while (!((y>=fY[i])&&(y<fY[i+1]))) {
    if (y< fY[i])   { l = i;   i = int(f+(l-f)/2); }
    if (y>=fY[i+1]) { f = i+1; i = int(f+(l-f)/2); }
    if (ntry++>fN) {
       cerr << "MonoSpline::FindBinInverse-E-InfiniteCycle " << ntry << "/" << fN << " --> " << y <<endl;
       for (int i=0; i<fN; i++) cerr<<"MonoSpline::FindBin-E-InfiniteCycle: fY[" << i << "] = " << fY[i] <<endl;
       return 0;
    }
  }
  return i+1;
}

double MonoSpline::EvalInverse(double y) {
  if (y!=y){
    cerr << "MonoSpline::EvalInverse-E-NaNInput " << endl;
    return 0;
  }
  int i = FindBinInverse(y);
  if ( (i==0)||(i==fN) ) {
    if (fB[i]==0) return 0; 
    return (y-fA[i])/fB[i]+fX0[i];
  }
  int nrealsol = 0;
  double realsol[3] = {0};
  double par[4] = {fD[i],fC[i],fB[i],fA[i]-y};
  solution_cubic(par,nrealsol,realsol);
  int nok = 0;
  double result = 0;
  for (int j=0; j<nrealsol; j++) {
    double x = realsol[j] + fX0[i];
    if ( (x>=fX[i-1])&&(x<fX[i]) ) { nok++; result = x; } 
  }
  if (nok>1) cerr << "MonoSpline::EvalInverse-W-MultipleAllowedSolutions" << endl;
  return result; 
} 

void MonoSpline::solution_quadratic(double* par, int& nrealsol, double* x) {
  nrealsol = 0;
  double a = par[0];
  double b = par[1];
  double c = par[2];
  if (a==0) {
    if (b==0) return;
    nrealsol = 1; 
    x[0] = -c/b; 
  }
  double disc = b*b-4*a*c;
  if (disc<0) return;
  double q = -0.5*(b+(b/fabs(b))*sqrt(disc));
  x[0] = q/a;
  x[1] = c/q; 
}

void MonoSpline::solution_cubic(double* par, int& nrealsol, double* x) {
  double a = par[0];
  double b = par[1];
  double c = par[2];
  double d = par[3];
  if (a==0) {
    solution_quadratic(&par[1],nrealsol,x); 
    return;  
  }
  if (d==0) {
    solution_quadratic(&par[0],nrealsol,x);
    x[nrealsol++] = 0;
    return;   
  }
  b /= a;
  c /= a;
  d /= a;
  a = b;
  b = c;
  c = d;  
  double Q = (a*a-3.*b)/9.;
  double R = (2.*a*a*a-9.*a*b+27.*c)/54.;
  if (R*R<Q*Q*Q) { 
    nrealsol = 3; 
    double theta = acos(R/sqrt(Q*Q*Q));
    x[0] = -2.*sqrt(Q)*cos(theta/3.)-a/3.;
    x[1] = -2.*sqrt(Q)*cos((theta+2*3.14159265359)/3.)-a/3.;
    x[2] = -2.*sqrt(Q)*cos((theta-2*3.14159265359)/3.)-a/3.;
  }
  else {
    nrealsol = 1; 
    double A = -(R/fabs(R))*pow(fabs(R)+sqrt(R*R-Q*Q*Q),1./3.);
    double B = (A==0) ? 0 : Q/A; 
    x[0] = A+B-a/3.; 
  }
}

void MonoSpline::Dump() const {
  for (int i=0; i<fN; i++) printf("(%f,%f)\n",fX[i],fY[i]);
  for (int i=0; i<fN+1; i++) printf("(%f,%f,%f,%f,%f)\n",fA[i],fB[i],fC[i],fD[i],fX0[i]); 
}

