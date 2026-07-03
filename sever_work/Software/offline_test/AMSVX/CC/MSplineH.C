#include "MSplineH.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// MSplineH
///////////////////////////////////////////////////////////////////////////////////////////////////////


double MSplineH::EnlargeFitWindow = 0.2; 
double MSplineH::IntegrationMinimum = 0.1;


void MSplineH::Init() {
  fN = 0;
  fM = 0;
  fLogx = 0;
  fLogy = 0;
  fExtraLow = 0;
  fExtraHig = 0;
  fX = 0; 
  fY = 0; 
  fSII = 0; 
  fA = 0; 
  fB = 0; 
  fC = 0; 
  fD = 0; 
  fX0 = 0; 
  fBlow = 0;
  fBhig = 0;
  fFunction = 0;
  fitquality[0] = 0;
  fitquality[1] = 0;
  fitquality[2] = 0;
  fitquality[3] = 0;
  fFitResult = 0;
  fGraph = 0;
  fShade = 0;
  NonMonotonic = false;
}


void MSplineH::Allocate(int nknots, double* xknots, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig) {
  fN = nknots;
  fM = nknots+1;
  fLogx = logx;
  fLogy = logy;
  fExtraLow = extralow;
  fExtraHig = extrahig;
  fX = new double[fN];
  fY = new double[fN];
  fSII = new double[fN];
  for (int i=0; i<fN; i++) {
    if ( (fLogx)&&(xknots[i]<=0) ) printf("MSplineH::MSplineH-W set Logx but x knot is <= 0 (%f)\n",xknots[i]);
    fX[i] = (fLogx) ? log10(xknots[i]) : xknots[i];
    fY[i] = 0;
    fSII[i] = 0;
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
  fBlow = blow;
  fBhig = bhig;
  fFunction = 0;
  fFitResult = 0;
  fGraph = 0;
  fShade = 0;
  NonMonotonic = false;
}


void MSplineH::Release() {
  fN = 0; 
  fLogx = 0;
  fLogy = 0;
  fExtraLow = 0;
  fExtraHig = 0;
  if (fX)  { delete [] fX;  fX = 0; }
  if (fY)  { delete [] fY;  fY = 0; }
  if (fSII){ delete [] fSII;fSII = 0; }
  if (fA)  { delete [] fA;  fA = 0; }
  if (fB)  { delete [] fB;  fB = 0; }
  if (fC)  { delete [] fC;  fC = 0; } 
  if (fD)  { delete [] fD;  fD = 0; } 
  if (fX0) { delete [] fX0; fX0 = 0; }
  fBlow = 0;
  fBhig = 0;  
  if (fFunction) delete fFunction; 
  if (fFitResult) delete fFitResult; 
  if (fGraph) delete fGraph;  
  if (fShade) delete fShade;
  fFunction = 0;
  fFitResult = 0;
  fGraph = 0;
  fShade = 0;
  NonMonotonic = false;
}


void MSplineH::Print() {
  printf("MSplineH::Print: Log(%1d,%1d) Extra(%1d,%1d) B(%f,%f) %1d\n",fLogx,fLogy,fExtraLow,fExtraHig,fBlow,fBhig,NonMonotonic);
  for (int i=0; i<fN; i++)   printf("(%f,%f)\n",fX[i],fY[i]);
  for (int i=0; i<fN+1; i++) printf("(%f,%f,%f,%f,%f)\n",fA[i],fB[i],fC[i],fD[i],fX0[i]);  
  printf("-------------------------------------------------------\n");
}


void MSplineH::Smooth() {
  TGraphSmooth smoother;
  TGraph* graph = new TGraph(); 
  for (int i=0; i<fN; i++) graph->SetPoint(i,fX[i],fY[i]); 
  TGraph* smooth = smoother.SmoothSuper(graph);
  for (int i=0; i<fN; i++) fY[i] = smooth->GetY()[i];
  delete graph;
  // delete smooth; // produced smoothed graph belongs to TGraphSmooth
  CalculateCoefficients();
}


void MSplineH::CalculateCoefficients() {
  // lower extrapolation 
  fA[0] = fY[0];
  fB[0] = 0;
  if (fExtraLow==kLinear) fB[0] = (fY[1]-fY[0])/(fX[1]-fX[0]);
  else if (fExtraLow==kLinearFree) fB[0] = fBlow; 
  fC[0] = 0;
  fD[0] = 0;
  fX0[0] = fX[0];
  // upper extrapolation 
  fA[fN] = fY[fN-1]; 
  fB[fN] = 0;
  fD[fN] = 0;
  fC[fN] = 0;
  fX0[fN] = fX[fN-1];
  if (fExtraHig==kLinear) { 
    fA[fN] = fY[fN-2]; 
    fB[fN] = (fY[fN-1]-fY[fN-2])/(fX[fN-1]-fX[fN-2]); 
    fX0[fN] = fX[fN-2]; 
  } 
  else if (fExtraHig==kLinearFree) fB[fN] = fBhig; 
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
    y1[1] = 0;
    if (fExtraLow==kLinear) y1[1] = s[1];
    else if (fExtraLow==kLinearFree) y1[1] = fBlow; 
    // set for last bin 
    y1[2] = 0;
    if (fExtraHig==kLinear) y1[2] = s[1];
    else if (fExtraHig==kLinearFree) y1[2] = fBhig;
    // monotonic correction 
    if (!NonMonotonic) {    
      for (int i=imin+1; i<imax; i++) {
        p[i]  = (s[i-1]*h[i]+s[i]*h[i-1])/(h[i]+h[i-1]);
        y1[i] = (S[i-1]+S[i])*(TMath::Min(TMath::Min(fabs(s[i-1]),fabs(s[i])),0.5*fabs(p[i])));
      }
    }
    fA[ibin+1] = fY[ibin];
    fB[ibin+1] = y1[1];
    fC[ibin+1] = (3*s[1] - 2*y1[1] - y1[2])/h[1];
    fD[ibin+1] = (y1[1] + y1[2] - 2*s[1])/(h[1]*h[1]);
    fX0[ibin+1] = fX[ibin];
  }
  // calculate tables of integrals
  // CalculateSpectralIndexIntegral();
}


void MSplineH::CalculateSpectralIndexIntegral() { 
  if ( (!fLogx)||(fLogy) ) return;
  double integral = 0;
  for (int i=0; i<fN; i++) {
    double X1 = (i==0) ? log10(IntegrationMinimum) : fX[i-1];
    double X2 = fX[i];
    double I1 = fA[i]*(X1-fX0[i])+fB[i]*pow(X1-fX0[i],2.)/2.+2*fC[i]*pow(X1-fX0[i],3.)/3.+fD[i]*pow(X1-fX0[i],4.)/4.;
    double I2 = fA[i]*(X2-fX0[i])+fB[i]*pow(X2-fX0[i],2.)/2.+2*fC[i]*pow(X2-fX0[i],3.)/3.+fD[i]*pow(X2-fX0[i],4.)/4.;
    integral += I2-I1; 
    fSII[i] = integral;
  }  
}

int MSplineH::FindBin(double x) {
  double X = (fLogx) ? log10(x) : x; 
  if (X< fX[0]) return 0;
  if (X>=fX[fN-1]) return fN; 
  int l = (fN-1);
  int f = 0;
  int i = int(f+(l-f)/2);
  while (!((X>=fX[i])&&(X<fX[i+1])) ) {
    if (X< fX[i])   { l = i;   i = int(f+(l-f)/2); }
    if (X>=fX[i+1]) { f = i+1; i = int(f+(l-f)/2); }
  }
  return i+1;
}
int MSplineH::FindBinY(double y) {   //assuming it is monotonic and has positive slope
  double Y = (fLogy) ? log10(y) : y;
  if (Y< fY[0]) return 0;
  if (Y>=fY[fN-1]) return fN;
  int l = (fN-1);
  int f = 0;
  int i = int(f+(l-f)/2);
  while (!((Y>=fY[i])&&(Y<fY[i+1])) ) {
    if (Y< fY[i])   { l = i;   i = int(f+(l-f)/2); }
    if (Y>=fY[i+1]) { f = i+1; i = int(f+(l-f)/2); }
  }
  return i+1;
}


double MSplineH::Eval(double x) {
  if ( (fLogx)&&(x<0) ) return 0.;
  int    i = FindBin(x);
  double X = (fLogx) ? log10(x) : x;
  double Y = fA[i]+fB[i]*(X-fX0[i])+fC[i]*pow(X-fX0[i],2.)+fD[i]*pow(X-fX0[i],3.); 
  double y = (fLogy) ? pow(10.,Y) : Y; 
  return y;
}


double MSplineH::EvalDerivative(double x) {
  if ( (fLogx)&&(x<0) ) return 0.;
  int    i = FindBin(x);
  double X = (fLogx) ? log10(x) : x;
  double dYdX = fB[i]+2*fC[i]*(X-fX0[i])+3*fD[i]*pow(X-fX0[i],2.);
  double y = Eval(x);
  double j = 1;
  if (fLogx) j *= 1./(x*log(10));
  if (fLogy) j *= y*log(10);
  return j*dYdX;
}


double MSplineH::EvalSpectralIndex(double x) {
  if ( (fLogx)&&(x<0) ) return 0.;
  int    i = FindBin(x);
  double X = (fLogx) ? log10(x) : x;
  double dYdX = fB[i]+2*fC[i]*(X-fX0[i])+3*fD[i]*pow(X-fX0[i],2.);
  double y = Eval(x);
  double j = 1;
  if (!fLogx) j *= x*log(10);
  if (!fLogy) j *= 1./(y*log(10));
  return j*dYdX;
}


double MSplineH::EvalSpectralIndexIntegral(double x) {
  if ( (!fLogx)||(fLogy) ) return 0;
  if ( (fLogx)&&(x<0) ) return 0.;
  int i = FindBin(x);
  double X1 = (i==0) ? log10(IntegrationMinimum) : fX[i-1];
  double X2 = log10(x);
  double I1 = fA[i]*(X1-fX0[i])+fB[i]*pow(X1-fX0[i],2.)/2.+2*fC[i]*pow(X1-fX0[i],3.)/3.+fD[i]*pow(X1-fX0[i],4.)/4.;
  double I2 = fA[i]*(X2-fX0[i])+fB[i]*pow(X2-fX0[i],2.)/2.+2*fC[i]*pow(X2-fX0[i],3.)/3.+fD[i]*pow(X2-fX0[i],4.)/4.;
  double integral = (i==0) ? 0 : fSII[i-1];   
  integral += I2-I1;
  return pow(10,integral);
}


void MSplineH::SetY(const double* yknots) {
  for (int i=0; i<fN; i++) fY[i] = (fLogy) ? log10(yknots[i]) : yknots[i];
  CalculateCoefficients();
}


void MSplineH::SetY(TGraph* graph) {
  if (!graph) return;
  double* y = new double[fN];
  for (int i=0; i<fN; i++) y[i] = (fLogx) ? graph->Eval(pow(10,fX[i])) : graph->Eval(fX[i]);   
  SetY(y);
  delete [] y; 
}   


void MSplineH::SetY(TGraphErrors* graph) {
  if (!graph) return;
  double* y = new double[fN];
  for (int i=0; i<fN; i++) y[i] = (fLogx) ? graph->Eval(pow(10,fX[i])) : graph->Eval(fX[i]);
  SetY(y);
  delete [] y;
}


void MSplineH::SetY(TH1* histo) {
  if (!histo) return;
  double* y = new double[fN];
  for (int i=0; i<fN; i++) y[i] = (fLogx) ? histo->Interpolate(pow(10,fX[i])) : histo->Interpolate(fX[i]); 
  SetY(y);
  delete [] y; 
}


void MSplineH::Set(int nknots, double* xknots, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig) {
  Release();
  Allocate(nknots,xknots,logx,logy,extralow,extrahig,blow,bhig);
}


void MSplineH::Set(int nknots, double* xknots, double* yknots, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig) {
  Release();
  Allocate(nknots,xknots,logx,logy,extralow,extrahig,blow,bhig);
  SetY(yknots);
}


void MSplineH::Set(int nknots, double* xknots, TGraph* graph, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig) {
  Release();
  Allocate(nknots,xknots,logx,logy,extralow,extrahig,blow,bhig);
  SetY(graph);
}


void MSplineH::Set(int nknots, double* xknots, TH1* histo, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig) {
  Release();
  Allocate(nknots,xknots,logx,logy,extralow,extrahig,blow,bhig);
  SetY(histo);
}


void MSplineH::Set(TH1* histo, bool logx, bool logy, bool error, int extralow, int extrahig, double blow, double bhig) {
  Release();
  if (!histo) return;
  int nknots = histo->GetNbinsX();
  double* xknots = new double[nknots];
  double* yknots = new double[nknots];
  for (int i=1; i<=histo->GetNbinsX(); i++) {
    xknots[i-1] = (!logx) ? histo->GetXaxis()->GetBinCenter(i) : histo->GetXaxis()->GetBinCenterLog(i);
    yknots[i-1] = (!error) ? histo->GetBinContent(i) : histo->GetBinError(i); 
  }
  Allocate(nknots,xknots,logx,logy,extralow,extrahig,blow,bhig); 
  SetY(yknots);   
  delete [] xknots;
  delete [] yknots; 
}


double MSplineH::operator() (double *x, double *par) {
  int ndof = GetNDofF();
  for (int i=0; i<fN; i++) fY[i] = par[i];
  if (fExtraLow==kLinearFree) fBlow = par[fN];
  if (fExtraHig==kLinearFree) fBhig = par[ndof-1];
  CalculateCoefficients();
  return Eval(x[0]);
}


void MSplineH::Draw(const char* opt, int color, double width, int npx, double xmin, double xmax) { 
  BuildGraph(npx,xmin,xmax);
  fGraph->SetLineColor(color);
  fGraph->SetLineWidth(width);
  fShade->SetFillStyle(3001);
  fShade->SetFillColor(color);
  fShade->Draw("F");
  fGraph->Draw(opt);
}


void MSplineH::Fit(TGraphErrors* graph, double xmin, double xmax, int nfix, int* ind_fix, double* par_fix) {
  if (xmin>=xmax) { 
    // if no indicated limits enlarge a bit the 
    // fitting window with respect to first and last knot
    xmin = (fLogx) ? pow(10,(1-EnlargeFitWindow)*fX[0])    : (1-EnlargeFitWindow)*fX[0];
    xmax = (fLogx) ? pow(10,(1+EnlargeFitWindow)*fX[fN-1]) : (1+EnlargeFitWindow)*fX[fN-1];
  } 
  if (!graph) return; 
  SetY(graph);
  BuildFunction(xmin,xmax); 
  if (fFitResult) delete fFitResult; 
  if (nfix>0) for (int ifix = 0; ifix<nfix; ifix++) fFunction->FixParameter(ind_fix[ifix],par_fix[ifix]);  
  TFitResultPtr ptr = graph->Fit(fFunction,"SQNR","",xmin,xmax); 
  fFitResult = new ROOT::Fit::FitResult(*((ROOT::Fit::FitResult*)ptr.Get()));
  int ndof = GetNDofF();
  if (fExtraLow==kLinearFree) fBlow = fFunction->GetParameter(fN);
  if (fExtraHig==kLinearFree) fBhig = fFunction->GetParameter(ndof-1);
  for (int i=0; i<fN; i++) fY[i] = fFunction->GetParameter(i);
  CalculateCoefficients();

  fitquality[0]=fFitResult->Ndf()>0?(fFitResult->Chi2()/fFitResult->Ndf()):fFitResult->Chi2();
  for(int ii=0;ii<graph->GetN();ii++){
     if(graph->GetX()[ii]<xmin||graph->GetX()[ii]>xmax) continue;
     double vorig=graph->GetY()[ii];
     double evorig=graph->GetEY()[ii];
     double vfit=Eval(graph->GetX()[ii]);
     double quality1=(vorig!=0)?fabs(vfit/vorig-1):0;
     double quality2=(evorig!=0)?fabs((vorig-vfit)/evorig):0;
     double quality3=(vorig!=0)?fabs(evorig/vorig):0;
     if(quality1>fitquality[1]) fitquality[1]=quality1;
     if(quality2>fitquality[2]) fitquality[2]=quality2;
     if(quality3>fitquality[3]) fitquality[3]=quality3;
  }
}

double MSplineH::GetChi2(int index){
   double result=0;
   if(index==0) result= (fFitResult) ? ( fFitResult->Ndf()>0?(fFitResult->Chi2()/fFitResult->Ndf()):fFitResult->Chi2() ) : fitquality[0];
   else if(index<4) result=fitquality[index];
   return result;
}

double MSplineH::FitEval(double x) { 
  if (!fFunction) return 0.;
  if ( (fLogx)&&(x<0) ) return 0.;
  return fFunction->Eval(x); 
}


double MSplineH::FitEvalError(double x, double cl) {
  if (!fFitResult) return 0.;
  if (!fFunction) return 0.;
  if ( (fLogx)&&(x<0) ) return 0.;
  // internal gradient calculation
  int ndof = GetNDofF();    
  double* dfdp = new double[ndof];
  for (int i=0; i<ndof; i++) dfdp[i] = 0;
  for (int i=0; i<fN; i++) {
    double y = fY[i];
    double h = fFitResult->Error(i); 
    fY[i] = y+2*h; CalculateCoefficients(); double f0 = Eval(x);
    fY[i] = y+1*h; CalculateCoefficients(); double f1 = Eval(x);
    fY[i] = y-1*h; CalculateCoefficients(); double f2 = Eval(x);
    fY[i] = y-2*h; CalculateCoefficients(); double f3 = Eval(x);
    dfdp[i] = (fabs(h)>0) ? (-f0+8*f1-8*f2+f3)/(12.*h) : 0; 
    fY[i] = y; CalculateCoefficients();
  }
  if (fExtraLow==kLinearFree) {
    double blow = fBlow;
    double h = fFitResult->Error(fN);
    fBlow = blow+2*h; CalculateCoefficients(); double f0 = Eval(x);
    fBlow = blow+1*h; CalculateCoefficients(); double f1 = Eval(x);
    fBlow = blow-1*h; CalculateCoefficients(); double f2 = Eval(x);
    fBlow = blow-2*h; CalculateCoefficients(); double f3 = Eval(x);
    dfdp[fN] = (fabs(h)>0) ? (-f0+8*f1-8*f2+f3)/(12.*h) : 0;
    fBlow = blow; CalculateCoefficients(); 
  }
  if (fExtraHig==kLinearFree) {
    double bhig = fBhig;
    double h = fFitResult->Error(ndof-1);
    fBhig = bhig+2*h; CalculateCoefficients(); double f0 = Eval(x);
    fBhig = bhig+1*h; CalculateCoefficients(); double f1 = Eval(x);
    fBhig = bhig-1*h; CalculateCoefficients(); double f2 = Eval(x);
    fBhig = bhig-2*h; CalculateCoefficients(); double f3 = Eval(x);
    dfdp[ndof-1] = (fabs(h)>0) ? (-f0+8*f1-8*f2+f3)/(12.*h) : 0;
    fBhig = bhig; CalculateCoefficients();
  }
  // propagation of errors
  double ey = 0; 
  for (int i=0; i<ndof; i++) 
    for (int j=0; j<ndof; j++) 
      ey += fFitResult->CovMatrix(i,j)*dfdp[i]*dfdp[j]; 
  ey = sqrt(ey); 
  // more propagation of errors 
  if (fLogy) {
    double y = Eval(x);  
    ey *= pow(10,y)*log(10); 
  }
  // clean-up    
  delete [] dfdp;
  return ey;
}


void MSplineH::BuildGraph(int npx, double xmin, double xmax) {
  if (fGraph) delete fGraph;
  if (xmin>=xmax) {
    xmin = (fLogx) ? pow(10,fX[0]) : fX[0];
    xmax = (fLogx) ? pow(10,fX[fN-1]) : fX[fN-1];
  }
  fGraph = new TGraph(npx);
  fShade = new TGraph(npx*2);
  for (int i=0; i<=npx; i++) {
    double x = (fLogx) ?
      xmin*pow(10,i*(log10(xmax)-log10(xmin))/npx):
      xmin + i*(xmax-xmin)/npx;
    double y = Eval(x);
    fGraph->SetPoint(i,x,y);
    double ey = FitEvalError(x);
    fShade->SetPoint(i,x,y+ey);
    fShade->SetPoint(npx*2-i-1,x,y-ey);
  }
}


int MSplineH::GetNDofF() {
  int ndof = fN;
  if (fExtraLow==kLinearFree) ndof++;
  if (fExtraHig==kLinearFree) ndof++;
  return ndof;
}


void MSplineH::BuildFunction(double xmin, double xmax) {
  static int function_index = 0; 
  if (fFunction) delete fFunction;
  int ndof = GetNDofF();
  fFunction = new TF1(Form("mspline_fit_%05d",function_index++),this,xmin,xmax,ndof,"MSplineH"); 
  for (int i=0; i<fN; i++) fFunction->SetParameter(i,fY[i]); 
  if (fExtraLow==kLinearFree) fFunction->SetParameter(fN,fBlow);
  if (fExtraHig==kLinearFree) fFunction->SetParameter(ndof-1,fBhig);
} 

/*
double MSplineH::Integral(double xmin, double xmax) { 
  ROOT::Math::Functor1D function(this,&MSplineH::Eval);
  ROOT::Math::Integrator integrator(ROOT::Math::IntegrationOneDim::kADAPTIVESINGULAR,1e-12,1e-12);
  integrator.SetFunction(function);
  return integrator.Integral(xmin,xmax);
}
*/

void MSplineH::EvalInverseMultiple(double y, int& n, double* x,double x0) {
  int ibin=-1,ibin_min=-1,ibin_max=-1;
  if(x0!=0) ibin=FindBin(x0);
  if(ibin>=0){
     for(int ii=ibin;ii>=0;ii--){
        //double xx0,xx1,yy0,yy1;
        //if(ii==0){
        //   xx0=fX[0];yy0=fY[0];
        //   xx1=fX[1];yy1=fY[1];
        //}
        //else if(ii==fN){
        //   xx0=fX[fN-2];yy0=fY[fN-2];
        //   xx1=fX[fN-1];yy1=fY[fN-1];
        //}
        //else{
        //   xx0=fX[ii-1];yy0=fY[ii-1];
        //   xx1=fX[ii];yy1=fY[ii];
        //}
        //bool IsInc=(yy1-yy0)*(xx1-xx0)>0;
        //if(IsInc) ibin_min=ii;
        //else break;
        if(GetSlope(ii)>0.3) ibin_min=ii;
        else break;
     }
     for(int ii=ibin;ii<=fN;ii++){
        //double xx0,xx1,yy0,yy1;
        //if(ii==0){
        //   xx0=fX[0];yy0=fY[0];
        //   xx1=fX[1];yy1=fY[1];
        //}
        //else if(ii==fN){
        //   xx0=fX[fN-2];yy0=fY[fN-2];
        //   xx1=fX[fN-1];yy1=fY[fN-1];
        //}
        //else{
        //   xx0=fX[ii-1];yy0=fY[ii-1];
        //   xx1=fX[ii];yy1=fY[ii];
        //}
        //bool IsInc=(yy1-yy0)*(xx1-xx0)>0;
        //if(IsInc) ibin_max=ii;
        //else break;
        if(GetSlope(ii)>0.3) ibin_max=ii;
        else break;
     }

     if(ibin_min<0||ibin_max<0) {n=0; return;}
     //else printf("x0=%f binmin=%d(%f,%f) binmax=%d(%f,%f)\n",x0,ibin_min,(ibin_min<1)?fX[0]:fX[ibin_min-1],(ibin_min<1)?fY[0]:fY[ibin_min-1],ibin_max,(ibin_max==fN)?fX[fN-1]:fX[ibin_max],(ibin_max==fN)?fY[fN-1]:fY[ibin_max]);
  }

  int imin=0;
  int imax=fN;
  if(ibin>=0) {imin=ibin_min; imax=ibin_max;}

  n = 0;
  for (int i=imin; i<=imax; i++) {
    double X = 0;
    if ( (i==0)||(i==fN) ) {
      if (fB[i]!=0) X = (y-fA[i])/fB[i]+fX0[i];
      if ( ( (i== 0)&&(X< fX[   0]) ) ||
           ( (i==fN)&&(X>=fX[fN-1]) ) ){
         x[n++] = X;
      }
    }
    else {
      int nrealsol = 0;
      double realsol[3] = {0};
      double par[4] = {fD[i],fC[i],fB[i],fA[i]-y};
      solution_cubic(par,nrealsol,realsol);
      int nok = 0;
      double result = 0;
      for (int j=0; j<nrealsol; j++) {
        double x = realsol[j] + fX0[i];
        if ( (x>=fX[i-1])&&(x<fX[i]) ) { nok++; X = x; }
      }
      if      (nok>1) cerr << "MSplineH::EvalInverseMultiple-W-NoMultipleAllowedSolutionsInASingleInterval" << endl;
      else if (nok==1) x[n++] = X;

      if(ibin>=0&&i==imin&&y<fY[i-1]){
         nok=1;
         double xx=(y-fY[i-1])/(fY[i]-fY[i-1])*(fX[i]-fX[i-1])+fX[i-1];
         if(xx<fX[i-1]) x[n++]=xx;
      }
      if(ibin>=0&&i==imax&&y>=fY[i]){
         nok=1;
         double xx=(y-fY[i-1])/(fY[i]-fY[i-1])*(fX[i]-fX[i-1])+fX[i-1];
         if(xx>=fX[i]) x[n++]=xx;
      }
      //if(nok==1) printf("sol: %d (%f,%f) (%f,%f) (%f,%f)\n",i,fX[i-1],fY[i-1],fX[i],fY[i],x[n-1],y);
    }
  }
}
double MSplineH::EvalInverseSimple(double y){  //assuming it is monotonic and has positive slope
   int nrealsol=0;
   double realsol[5]={-1,-1,-1,-1,-1};
   double X=-1;
   int i=FindBinY(y);
   double Y = (fLogy) ? log10(y) : y;
   if ( (i==0)||(i==fN) ) {
     if (fB[i]!=0) X = (Y-fA[i])/fB[i]+fX0[i];
     if ( ( (i== 0)&&(X< fX[   0]) ) ||
          ( (i==fN)&&(X>=fX[fN-1]) ) ){
        realsol[nrealsol++]=(fLogx) ? pow(10,X) : X;
     }
   }
   else {
     double par[4] = {fD[i],fC[i],fB[i],fA[i]-y};
     int nsol;
     double xsol[5];
     solution_cubic(par,nsol,xsol);
     for (int j=0; j<nsol; j++) {
       X = xsol[j] + fX0[i];
       if ( (X>=fX[i-1])&&(X<fX[i]) ) {realsol[nrealsol++]=(fLogx) ? pow(10,X) : X;}
     }
   }
   if(nrealsol!=1) return -1.;
   else return realsol[0];
}

TGraph* MSplineH::GetInverse(int Np,double x0){
   double ymin=1.0e20;
   double ymax=-1.0e20;
   for(int ii=0;ii<fN;ii++){
      if(fY[ii]<ymin) ymin=fY[ii];
      if(fY[ii]>ymax) ymax=fY[ii];
   }
   ymin*=0.5;
   ymax*=2;
   int np=0;
   TGraph* gr=new TGraph();
   for(int ii=0;ii<Np;ii++){
      double yy=0,xx=0;
      if(!fLogy) yy=ymin+(ymax-ymin)/(Np-1)*ii;
      else{
         if(ymin<=0||ymax<=0) continue;
         yy=log10(ymin)+(log10(ymax)-log10(ymin))/(Np-1)*ii;
         yy=pow(10,yy);
      }
      int nsol;
      double xsol[10];
      EvalInverseMultiple(yy,nsol,xsol,x0);
      if(nsol==1) xx=xsol[0];
      else continue;
      gr->SetPoint(np,xx,yy);
      np++;
   }
   if(np<=0) {delete gr; return 0;}
   return gr;
}
double MSplineH::GetSlope(int binx){
   if(binx<=0) binx=1;
   if(binx>=fN) binx=fN-1;
   double x0=fX[binx-1];
   double x1=fX[binx];
   double y0=fY[binx-1];
   double y1=fY[binx];
   if(x0==0||y0==0) return 1.0e20;
   return log10(fabs(y1/y0))/log10(fabs(x1/x0));
}
double MSplineH::GetSlope(double x){
   int ibin=FindBin(x);
   if(ibin==0) ibin=1;
   if(ibin==fN) ibin=fN-1;
   double x0=fX[ibin-1];
   double x1=fX[ibin];
   double y0=fY[ibin-1];
   double y1=fY[ibin];
   if(x0==0||y0==0) return 1.0e20;
   return log10(fabs(y1/y0))/log10(fabs(x1/x0));
}
TGraph* MSplineH::GetSlope(){
   int np=0;
   TGraph* gr=new TGraph();
   for(int ii=0;ii<fN-1;ii++){
      double x0=fX[ii];
      double x1=fX[ii+1];
      double y0=fY[ii];
      double y1=fY[ii+1];
      if(y0==0||x0==0) continue;
      //double slope=(y1-y0)/y0/(x1-x0);
      //double slope=(y1-y0)/y0;
      double slope=log10(fabs(y1/y0))/log10(fabs(x1/x0));
      //double slope=(y1/y0)/(x1/x0);
      gr->SetPoint(np,x0,slope);
      np++;
   }
   if(np<=0) {delete gr; return 0;}
   return gr;
}

void MSplineH::solution_quadratic(double* par, int& nrealsol, double* x) {
  nrealsol = 0;
  double a = par[0];
  double b = par[1];
  double c = par[2];
  if (fabs(a)<1e-20) {
    if (fabs(b)<1e-20) return;
    nrealsol = 1;
    x[0] = -c/b;
    return;
  }
  double disc = b*b-4*a*c;
  if (disc<0) return;
  if (fabs(b)<1e-20) {
    nrealsol = 2;
    x[0] = -sqrt(-c/a);
    x[1] =  sqrt(-c/a);
    return;
  }
  nrealsol = 2;
  x[0] = 0.5*(-b+sqrt(disc))/a;
  x[1] = 0.5*(-b-sqrt(disc))/a;
}

void MSplineH::solution_cubic(double* par, int& nrealsol, double* x) {
  double a = par[0];
  double b = par[1];
  double c = par[2];
  double d = par[3];
  if (fabs(a)<1e-20) {
    solution_quadratic(&par[1],nrealsol,x);
    return;
  }
  if (fabs(d)<1e-20) {
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

bool MSplineH::MultipleLimit(double &ylow,double &yhig,double &xlow,double &xhig,double &xtmin,double &xtmax){
  bool multisol=false;
  ylow=1.0e20;
  yhig=-1.0e20;
  xlow=1.0e20;
  xhig=-1.0e20;

  //find the region with multiple solution
  for(int ii=0;ii<fN-1;ii++){
     double y1=(fY[ii]<fY[ii+1])?fY[ii]:fY[ii+1];
     double x1=(fY[ii]<fY[ii+1])?fX[ii]:fX[ii+1];
     double y2=(fY[ii]<fY[ii+1])?fY[ii+1]:fY[ii];
     double x2=(fY[ii]<fY[ii+1])?fX[ii+1]:fX[ii];
     for(int i2=ii+1;i2<fN-1;i2++){
        double y3=(fY[i2]<fY[i2+1])?fY[i2]:fY[i2+1];
        double x3=(fY[i2]<fY[i2+1])?fX[i2]:fX[i2+1];
        double y4=(fY[i2]<fY[i2+1])?fY[i2+1]:fY[i2];
        double x4=(fY[i2]<fY[i2+1])?fX[i2+1]:fX[i2];
        int casei=-1;
        if(y1<=y3){
           if(y2>y3&&y2<=y4) casei=0;
           if(y2>y4) casei=1;
        }
        else if(y1<y4){
           if(y2<=y4) casei=2;
           if(y2>y4) casei=3;
        }
        if(casei>=0){
           double yminmax[2]={-1,-1};
           double xminmax[2]={-1,-1};
           if(casei==0) {yminmax[0]=y3; yminmax[1]=y2;}
           if(casei==1) {yminmax[0]=y3; yminmax[1]=y4;}
           if(casei==2) {yminmax[0]=y1; yminmax[1]=y2;}
           if(casei==3) {yminmax[0]=y1; yminmax[1]=y4;}
           for(int iloop=0;iloop<2;iloop++){
              double yy;
              if(casei==0){
                 if(iloop==0){
                    if(x1<x2) yy=y3;
                    else {xminmax[iloop]=x2; continue;}
                 }
                 else{
                    if(x3<x4) yy=y2;
                    else {xminmax[iloop]=x3; continue;}
                 }
              }
              if(casei==1){
                 if(iloop==0){
                    if(x1<x2) yy=y3;
                    else yy=y4;
                 }
                 else{
                    xminmax[iloop]=TMath::Max(x3,x4); continue;
                 }
              }
              if(casei==2){
                 if(iloop==1){
                    if(x3<x4) yy=y2;
                    else yy=y1;
                 }
                 else{
                    xminmax[iloop]=TMath::Min(x1,x2); continue;
                 }
              }
              if(casei==3){
                 if(iloop==0){
                    if(x1>x2) yy=y4;
                    else {xminmax[iloop]=x1; continue;}
                 }
                 else{
                    if(x3>x4) yy=y1;
                    else {xminmax[iloop]=x4; continue;}
                 }
              }

              double result = 0;
              int index=(iloop==0)?ii:i2;

              int nrealsol = 0; 
              double realsol[3] = {0};
              double par[4]={fD[index+1],fC[index+1],fB[index+1],fA[index+1]-yy};
              solution_cubic(par,nrealsol,realsol);
              int nok = 0;
              for (int j=0; j<nrealsol; j++) {
                 double x = realsol[j]+fX0[index+1];
                 if ( (x>=fX[index]*0.9999)&&(x<=fX[index+1]*1.0001) ) { nok++; result = x; }
                 //printf("loop%i j%i x=%g fX0=%g fX1=%g\n",iloop,j,x,fX[index],fX[index+1]);
              }
              if(nok<1){
                 cerr << "MSplineH::EvalInverseMultiple-W-NoMultipleAllowedSolutionsInASingleInterval2" << endl;
                 xminmax[iloop]=(iloop==0)?fX[ii]:fX[i2+1];
              }
              else{
                 xminmax[iloop]=result;
              }
           }
           //printf("i1=%i i2=%i ymin=%g ymax=%g xmin=%g xmax=%g\n",ii,i2,yminmax[0],yminmax[1],xminmax[0],xminmax[1]);
           if(yminmax[0]>=0&&yminmax[1]>=0&&xminmax[0]>=0&&xminmax[1]>=0){
              if(yminmax[1]>yhig) yhig=yminmax[1];
              if(yminmax[0]<ylow) ylow=yminmax[0];
              if(xminmax[1]>xhig) xhig=xminmax[1];
              if(xminmax[0]<xlow) xlow=xminmax[0];
           }
           multisol=true;
        }
     }
  }
  //find the first and last monotonic region
  xtmin=fX[fN-1]+1;
  for(int ii=0;ii<fN-1;ii++){
     if(GetSlope(ii+1)<0.3){
        xtmin=fX[ii]; break;
     }
  }
  xtmax=fX[0]-1;
  for(int ii=fN-1;ii>0;ii--){
     if(GetSlope(ii)<0.3){
        xtmax=fX[ii]; break;
     }
  }

  return multisol;
}

bool MSplineH::Revise(bool changeknot){
   double xmin,xmax,ymin,ymax,xtmin,xtmax;
   bool multisol=MultipleLimit(ymin,ymax,xmin,xmax,xtmin,xtmax);
   if(!changeknot) return multisol;
   if(!multisol) return false;

   if(fLogx) {xmin=pow(10,xmin); xmax=pow(10,xmax);} 
   if(fLogy) {ymin=pow(10,ymin); ymax=pow(10,ymax);}
   
   int nknot=0;
   double xx[100];
   double yy[100];
   bool insert=false;
   for(int ii=0;ii<fN;ii++){
      double xknot=(fLogx)?pow(10,fX[ii]):fX[ii];
      double yknot=(fLogy)?pow(10,fY[ii]):fY[ii];
      if(xknot<xmin*0.999||xknot>xmax*1.001){
         xx[nknot]=xknot;
         yy[nknot]=yknot;
         nknot++;
      }
      else if(!insert){
         xx[nknot]=xmin;
         yy[nknot]=ymin;
         nknot++;
         xx[nknot]=xmax;
         yy[nknot]=ymax;
         nknot++;
         insert=true;
      }
      //if(nknot-1>=0) printf("knots: %i %g %g (%g,%g,%g,%g)\n",nknot-1,sqrt(xx[nknot-1]),yy[nknot-1],ymin,ymax,xmin,xmax);
   }
   Set(nknot,xx,yy,fLogx,fLogy,fExtraLow,fExtraHig,fBlow,fBhig);
   return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// MSplineFitSamplingH
///////////////////////////////////////////////////////////////////////////////////////////////////////


double    MSplineFitSamplingH::fChi2Max = 1e+10;
bool      MSplineFitSamplingH::fUseAverage = false;


TRandom3* MSplineFitSamplingH::fRandom = 0; 


MSplineFitSamplingH::MSplineFitSamplingH(TGraphErrors* graph) {
  fGraph = (graph)?((TGraphErrors*) graph->Clone(Form("mspline_%s",graph->GetName()))):0;
  fnSamples = 0;  
  fSamples = 0;
  fFlags = 0;
  fDrawGraph = 0;
  fDrawShade = 0;
  fFunction = 0;
  if (!fRandom) fRandom = new TRandom3();  
}


MSplineFitSamplingH::~MSplineFitSamplingH() { 
  Release();
}


void MSplineFitSamplingH::Release() {
  if (fGraph) delete fGraph; 
  fGraph = 0; 
  for (int isample=0; isample<fnSamples; isample++) if (fSamples[isample]) {delete fSamples[isample];}
  if (fSamples) delete [] fSamples; 
  if (fFlags) delete [] fFlags; 
  fnSamples = 0;
  fSamples = 0; 
  fFlags = 0;
  if (fDrawGraph) delete fDrawGraph;
  if (fDrawShade) delete fDrawShade;
  if (fFunction)  delete fFunction;
  fDrawGraph = 0;
  fDrawShade = 0;
  fFunction  = 0;
}


void MSplineFitSamplingH::Fit(int nbins, double* bins, bool logx, bool logy, int nsamples, double xmin, double xmax, int extralow, int extrahig, double blow, double bhig, int flag, bool IsRandom) { 
  if (!fGraph) return; 
  // resize the array
  int offset = fnSamples;
  fnSamples += nsamples; 
  if (offset>0) { 
    MSplineH** copy_container = new MSplineH*[offset]; 
    int*      copy_flags     = new int[offset];
    for (int isample=0; isample<offset; isample++) {
      copy_container[isample] = fSamples[isample]; 
      copy_flags[isample] = fFlags[isample];
    }
    delete [] fSamples; 
    delete [] fFlags;
    fSamples = new MSplineH*[nsamples+offset];
    fFlags = new int[nsamples+offset];
    for (int isample=0; isample<offset;   isample++) {
      fSamples[isample] = copy_container[isample];  
      fFlags[isample] = copy_flags[isample];
    }
    for (int isample=0; isample<nsamples; isample++) {
      fSamples[isample+offset] = 0;
      fFlags[isample+offset] = 0;
    }
    delete [] copy_container; 
    delete [] copy_flags;
  }
  else {
    fSamples = new MSplineH*[nsamples];
    fFlags = new int[nsamples];
    for (int isample=0; isample<nsamples; isample++) {
      fSamples[isample] = 0;
      fFlags[isample] = 0;
    }
  }
  // sampling
  fRandom->SetSeed(1234);
  for (int isample=0; isample<nsamples; isample++) {
    // knots
    int nknot=0;
    double* xknots = new double[nbins];
    for (int iknot=0; iknot<nbins; iknot++) {
      if(!IsRandom){
         xknots[nknot]=bins[iknot];
         nknot++;
         continue;
      }
      double x1 = (logx) ? log10(bins[iknot])   : bins[iknot]; 
      double x2 = (logx) ? log10(bins[iknot+1]) : bins[iknot+1];
      double x = x1 + (x2-x1)*fRandom->Uniform();
      if (logx) x = pow(10,x);
      if(nknot<=0){xknots[nknot]=x; nknot++;}
      else{
         int npgraph=0;
         for(int ii=0;ii<fGraph->GetN();ii++){
            double xgraph=fGraph->GetX()[ii];
            if(xgraph>=xknots[nknot-1]&&xgraph<x) npgraph++;
         }
         if(npgraph>0){
            xknots[nknot] = x;
            nknot++;
         }
      }
    }
    // fit
    fSamples[isample+offset] = new MSplineH(nknot,xknots,fGraph,logx,logy,extralow,extrahig,blow,bhig);
    fSamples[isample+offset]->Fit(fGraph,xmin,xmax);
    fFlags[isample+offset] = flag;
    delete [] xknots;
  }
} 


bool MSplineFitSamplingH::IsThereAnyFlag(int flag) {
  for (int isample=0; isample<fnSamples; isample++) if (fFlags[isample]==flag) return true;
  return false;
}


MSplineH* MSplineFitSamplingH::GetBestFit(int flag) { 
  double chi2_best = fChi2Max;
  int ibest = -1;    
  for (int isample=0; isample<fnSamples; isample++) {
    double chi2 = fSamples[isample]->GetChi2();
    if ( (chi2<chi2_best)&&( (fFlags[isample]==flag)||(flag<0) ) ) {
      chi2_best = chi2;
      ibest = isample;
    }
  }
  return (ibest<0) ? 0 : fSamples[ibest]; 
}


double MSplineFitSamplingH::Eval(double x, int flag) {
  if (!fUseAverage){
     if((fnSamples==1)&&fSamples[0]) return fSamples[0]->Eval(x);
     else return (GetBestFit(flag)) ? GetBestFit(flag)->Eval(x) : 0;
  }
  double m = 0;
  int    n = 0;
  for (int isample=0; isample<fnSamples; isample++) {
    if (!fSamples[isample]) continue;
    if ( (fFlags[isample]!=flag)&&(flag>=0) ) continue;
    if (fSamples[isample]->GetChi2()>fChi2Max) continue;
    if (fSamples[isample]->EvalError(x)<=0) continue;
    m += fSamples[isample]->Eval(x);
    n++;
  }
  if (n>0) m /= n;
  return m;
}


double MSplineFitSamplingH::EvalErrorStatistics(double x, int flag) {
  if (!fUseAverage) return (GetBestFit(flag)) ? GetBestFit(flag)->EvalError(x) : 0;
  // average of fitting errors (100% correlation) 
  double m = 0;
  int    n = 0;
  for (int isample=0; isample<fnSamples; isample++) {
    if (!fSamples[isample]) continue;
    if ( (fFlags[isample]!=flag)&&(flag>=0) ) continue;
    if (fSamples[isample]->GetChi2()>fChi2Max) continue;
    if (fSamples[isample]->EvalError(x)<=0) continue;
    m += fSamples[isample]->EvalError(x);
    n++;
  }
  if (n>0) m /= n;
  return m;
}


double MSplineFitSamplingH::EvalErrorSampling(double x, int flag) {
  double y1 = Eval(x,flag);
  double s = 0;
  int    n = 0;
  for (int isample=0; isample<fnSamples; isample++) {
    if (!fSamples[isample]) continue;
    if ( (fFlags[isample]!=flag)&&(flag>=0) ) continue;
    if (fSamples[isample]->GetChi2()>fChi2Max) continue;
    if (fSamples[isample]->EvalError(x)<=0) continue;
    double y2 = fSamples[isample]->Eval(x);
    s += pow(y2-y1,2);
    n++;
  }
  if (n>0) s /= n;
  if (s>0) s = sqrt(s);
  return s;
}


double MSplineFitSamplingH::EvalErrorSystematics(double x, int flag) {
  double y1 = Eval(x,flag);
  double s = 0;
  for (int iflag=0; iflag<10; iflag++) {
    if (iflag==flag) continue; 
    if (!IsThereAnyFlag(iflag)) continue;
    double y2 = Eval(x,iflag); 
    s += pow(fabs(y1-y2),2.);
  }
  if (s>0) s = sqrt(s);
  return s; 
}


double MSplineFitSamplingH::EvalError(double x, int flag) {
  double ey1 = EvalErrorStatistics(x,flag);
  double ey2 = EvalErrorSampling(x,flag);
  double ey3 = EvalErrorSystematics(x,flag);
  return sqrt(ey1*ey1+ey2*ey2+ey3*ey3);
}


TGraphErrors* MSplineFitSamplingH::GetFitGraph() {
  TGraphErrors* graph = (TGraphErrors*) fGraph->Clone(Form("fit_%s",fGraph->GetName()));
  for (int i=0; i<graph->GetN(); i++) {
    double x,y;
    graph->GetPoint(i,x,y);
    graph->SetPoint(i,x,Eval(x));
    graph->SetPointError(i,0,EvalError(x)); 
  }
  return graph; 
}


void MSplineFitSamplingH::DrawResultPanel(double xmin, double xmax, bool logx, bool logy) {
  TCanvas fit_canvas("fit_canvas","Fit Canvas",1000,700);
  if (!fGraph) return;
  fGraph->SetMarkerStyle(20);
  fGraph->SetMarkerSize(1.1);
  fGraph->SetLineWidth(2);
  fGraph->Draw("AP");
  for (int isample=0; isample<fnSamples; isample++) {
    if (!fSamples[isample]) continue;
    fSamples[isample]->Draw("L SAME",kBlue+1,2,1000,xmin,xmax);
  }
  Draw(kOrange+1,kOrange+1,3001,2,1000,xmin,xmax);
  fGraph->Draw("P");
  fit_canvas.SetLogx(logx);
  fit_canvas.SetLogy(logy);
  fit_canvas.Update();
  fit_canvas.WaitPrimitive();
}


void MSplineFitSamplingH::Draw(int color, int color_area, int fill_style, double width, int npx, double xmin, double xmax) {
  BuildGraph(npx,xmin,xmax);
  fDrawShade->SetFillStyle(fill_style);
  fDrawShade->SetFillColor(color_area);
  fDrawGraph->SetLineColor(color);
  fDrawGraph->SetLineWidth(width);
  fDrawShade->Draw("F");
  fDrawGraph->Draw("LX");
}


void MSplineFitSamplingH::BuildGraph(int npx, double xmin, double xmax) {
  if (fDrawGraph) delete fDrawGraph;
  if (fDrawShade) delete fDrawShade; 
  fDrawGraph = new TGraphErrors(npx);
  fDrawShade = new TGraph(npx*2);
  for (int i=0; i<=npx; i++) {
    double x = (GetBestFit()->fLogx) ?
      xmin*pow(10,i*(log10(xmax)-log10(xmin))/npx):
      xmin + i*(xmax-xmin)/npx;
    double y = Eval(x);
    double ey = EvalError(x);
    fDrawGraph->SetPoint(i,x,y);
    fDrawGraph->SetPointError(i,0,ey);
    fDrawShade->SetPoint(i,x,y+ey);
    fDrawShade->SetPoint(npx*2-i-1,x,y-ey);
  }
} 

double MSplineFitSamplingH::operator() (double *x,double *par){
  double xx=x[0];
  return Eval(xx);
}

void MSplineFitSamplingH::BuildFunction(int npx,double xmin,double xmax){
   static int function_index2 =0 ;
   if(fFunction) delete fFunction;
   fFunction = new TF1(Form("msplinefitsample_%05d",function_index2++),this,xmin,xmax,0,"MSplineFitSamplingH");
   fFunction->SetNpx(npx);
}


