// $Id: TrEDepDB.C,v 1.13 2022/06/16 10:30:27 qyan Exp $


#include "TrEDepDB.h"
#include "TrChargeLossDB.h" // for cubic monotonic spline
#include "commonsi.h"


#define EDEP_ZMAX 26
#define BETA_MAX 0.999
#define BETAGAMMA_MIN 0.001
#define LOGBETAGAMMA_MIN -3


ClassImp(TrEDepDB);
ClassImp(TrEDepTable);


///////////////////////////
// TrEDepDB
///////////////////////////


TrEDepDB* TrEDepDB::fHead = 0;
map<int,TrEDepTableI*> TrEDepDB::fTrEDepMap;
TrEDepTableI** TrEDepDB::fTrEDepArray = 0;
bool TrEDepDB::fInitDone = false;
int TrEDepDB::fUsedVersion = 1;
int TrEDepDB::fBoostBetaCorrection = -1;


TrEDepDB* TrEDepDB::GetHead() {
  if (IsNull()) {
    printf("TrEDepDB::GetHead()-V TrEDepDB singleton initialization.\n");
    fHead = new TrEDepDB(); 
  } 
  return fHead;
}


TrEDepDB::~TrEDepDB() { 
  Clear();
  if (fHead) delete fHead;
  fHead = 0;
}


void TrEDepDB::Init(bool force) {
  if (IsNull()) return;
  if ( (fInitDone)&&(!force) ) return;  
  if (LoadDefaultTablesVer1()) printf("TrEDepDB::GetHead()-V TrEDepDBver1 correction loaded, %d tables read.\n",(int)fTrEDepMap.size());
  else                         printf("TrEDepDB::GetHead()-V TrEDepDBver1 correction NOT loaded.\n"); 
  if (LoadDefaultTablesVer2()) printf("TrEDepDB::GetHead()-V TrEDepDBver2 correction loaded, %d tables read.\n",(int)fTrEDepMap.size());
  else                         printf("TrEDepDB::GetHead()-V TrEDepDBver2 correction NOT loaded.\n");
  fInitDone = true;
} 


void TrEDepDB::Clear(Option_t* option) { 
  for (map<int,TrEDepTableI*>::iterator it=fTrEDepMap.begin(); it!=fTrEDepMap.end(); it++) {
    if (it->second) {
      it->second->Clear();
      delete it->second;
    }
    it->second = 0;
  }
  fTrEDepMap.clear();
  delete [] fTrEDepArray; 
  fTrEDepArray = 0;
  fInitDone = false;
}


bool TrEDepDB::AddTable(int index, TrEDepTableI* table) {
  if (table==0) return false;
  pair<map<int,TrEDepTableI*>::iterator,bool> ret;
  ret = fTrEDepMap.insert(pair<int,TrEDepTableI*>(index,table));
  if (ret.second==false) fTrEDepMap.find(index)->second = table; 
  if (!fTrEDepArray) {
    fTrEDepArray = new TrEDepTableI*[100];
    for (int i=0; i<100; i++) fTrEDepArray[i] = 0;
  }
  assert((index>=0)&&(index<100));
  fTrEDepArray[index] = fTrEDepMap.find(index)->second;
  return true;
}


TrEDepTableI* TrEDepDB::GetTable(int index) {
  assert((index>=0)&&(index<100));
  return fTrEDepArray[index]; 
  // map<int,TrEDepTableI*>::iterator it = fTrEDepMap.find(index);
  // return (it!=fTrEDepMap.end()) ? it->second : 0;
}


bool TrEDepDB::LoadDefaultTablesVer1(char* dirname) {
  for (int il=0; il<9; il++) {
    TrEDepTable* tableX = new TrEDepTable();
    bool loadX = tableX->InitTableFromTxtFile(Form("%s/%s/TrEDepDBver1/TrEDepTable_JLayer%1d_XSide.txt",dirname,AMSCommonsI::getversion(),il+1));//the directory has been changed to v6.00
    if (loadX) AddTable(tableX,0,il+1,1);
    TrEDepTable* tableY = new TrEDepTable();
    bool loadY = tableY->InitTableFromTxtFile(Form("%s/%s/TrEDepDBver1/TrEDepTable_JLayer%1d_YSide.txt",dirname,AMSCommonsI::getversion(),il+1));
    if (loadY) AddTable(tableY,1,il+1,1);
  }
  return (int(fTrEDepMap.size())>0);
}


bool TrEDepDB::LoadDefaultTablesVer2(char* dirname) {
  for (int il=0; il<9; il++) {
    TrEDepTableSpline* tableX = new TrEDepTableSpline();
    bool loadX = tableX->InitTableFromTxtFile(Form("%s/%s/TrEDepDBver2/TrEDepTable_JLayer%1d_XSide_step1.txt",dirname,AMSCommonsI::getversion(),il+1),0);
    loadX &= tableX->InitTableFromTxtFile(Form("%s/%s/TrEDepDBver2/TrEDepTable_JLayer%1d_XSide_step2.txt",dirname,AMSCommonsI::getversion(),il+1),1);
    if (loadX) AddTable(tableX,0,il+1,2);
    TrEDepTableSpline* tableY = new TrEDepTableSpline();
    bool loadY = tableY->InitTableFromTxtFile(Form("%s/%s/TrEDepDBver2/TrEDepTable_JLayer%1d_YSide_step1.txt",dirname,AMSCommonsI::getversion(),il+1),0);
    loadY &= tableY->InitTableFromTxtFile(Form("%s/%s/TrEDepDBver2/TrEDepTable_JLayer%1d_YSide_step2.txt",dirname,AMSCommonsI::getversion(),il+1),1);
    if (loadY) AddTable(tableY,1,il+1,2);
  }
  return (int(fTrEDepMap.size())>0);
}


double TrEDepDB::GetEDepCorrectedValue(int jlayer, double Q, double beta, double rigidity, double mass_on_Z, int iside, int ver) {
  // type
  TrEDepTableI* table = GetTable(iside,jlayer,ver);
  static int maxerr = 0;
  if (!table) {
    if (maxerr<100) { 
      printf("TrEDepDB::GetEDepCorrectedValue-E no table (iside=%d, jlayer=%d, ver=%d) found, return unchanged value (only displayed 100 times).\n",iside,jlayer,ver);
      maxerr++;
    }
    return Q; 
  }
  // boost (used for MC)
  double boost = ( (fBoostBetaCorrection>0)&&(beta<1) ) ? pow(fabs(beta),-TMath::Max(0.,TMath::Min(0.1,(fBoostBetaCorrection-3)*0.02))) : 1; 
  // use beta correction 
  int type = 0;
  // if rigidity not null use beta/rigidity correction
  if (fabs(rigidity)>1e-6) type = 1;
  // calculation delivered by table
  if (beta>=0) return table->GetCorrectedValue(Q,beta,rigidity,mass_on_Z,type)*boost;
  // if up-going make the jlayer folding plus some gain correction between thee two layers
  int jlayer_upgoing = 9-jlayer+1;
  TrEDepTableI* table_downgoing = table;
  TrEDepTableI* table_upgoing = GetTable(iside,jlayer_upgoing,ver);
  if ( (!table_downgoing)||(!table_upgoing) ) {
    printf("TrEDepDB::GetEDepCorrectedValue-E downgoing or upgoing table not available (iside=%d, jlayer=%d, ver=%d) found. Return 0.\n",iside,jlayer,ver);
    return 0;
  }
  double value = table_upgoing->GetCorrectedValue(Q,beta,rigidity,mass_on_Z,type);
  double den = table_upgoing->GetCorrectedValue(Q,0.94,0,0,0);
  double num = table_downgoing->GetCorrectedValue(Q,0.94,0,0,0);
  double gain = (fabs(den)<1e-6) ? 1 : num/den; 
  // some stretching function to be implemented ... upside/down is not ok in scale ...      
  return value*gain*boost; 
}


double TrEDepDB::GetTOIBeta(double beta, int Z, int inn_jlayer, int iside, int ver) {   
  TrEDepTableI* table_top = GetTable(iside,1,ver); 
  TrEDepTableI* table_inn = GetTable(iside,inn_jlayer,ver); 
  double Q = table_top->GetQ(Z,beta,0,0,0);
  // gain normalization
  Q *= table_inn->GetQ(Z,1,0,0,0)/table_top->GetQ(Z,1,0,0,0); 
  double beta_top = table_inn->FindBeta(Z,Q,0,0.1,0.99);
  return (beta_top>beta) ? beta_top : beta;
}


///////////////////////////
// TrEDepTable
///////////////////////////


TrEDepTable::TrEDepTable() {
  fNPars = 0;
  fPars = 0;
}


void TrEDepTable::Init(int nch, int npar) { 
  TrEDepTableI::Init(nch);
  fNPars = npar;
  if (fPars) delete [] fPars; 
  fPars = new double[GetNCharges()*GetNPars()]; 
  for (int ich=0; ich<GetNCharges(); ich++) {
    for (int ipar=0; ipar<GetNPars(); ipar++) {
      SetPar(ich,ipar,0.); 
    }
  }
}


void TrEDepTable::Clear(Option_t* option) {
  TrEDepTableI::Clear(option);
  fNPars = 0;
  if (fPars) delete [] fPars;  
  fPars = 0; 
}


double TrEDepTable::GetPar(int ich, int ipar) {
  if (!fPars) return 0;
  if ( (ich<0)||(ich>=GetNCharges()) ) return 0;
  if ( (ipar<0)||(ipar>=GetNPars()) ) return 0; 
  return fPars[ich*GetNPars()+ipar];
}


bool TrEDepTable::SetPar(int ich, int ipar, double par) {
  if (!fPars) return false;
  if ( (ich<0)||(ich>=GetNCharges()) ) return false;
  if ( (ipar<0)||(ipar>=GetNPars()) ) return false;
  fPars[ich*GetNPars()+ipar] = par;
  return true;
}


bool TrEDepTable::InitTableFromTxtFile(char* filename) { 
  FILE* file = 0;
  file = fopen(filename,"r");
  if (!file) return false;
  int dummy,ncharges,npars;
  fscanf(file,"%d%d%d",&dummy,&ncharges,&npars);
  Init(ncharges,npars);
  for (int ich=0; ich<GetNCharges(); ich++) {
    int z;
    int ret = fscanf(file,"%d",&z);
    if (ret<=0) return false;
    SetZ(ich,z);
    for (int ipar=0; ipar<GetNPars(); ipar++) {
      float par;
      int ret = fscanf(file,"%f",&par);
      if (ret<=0) continue;
      SetPar(ich,ipar,par); 
    }
  }
  fclose(file);
  return true;
}


double TrEDepTable::GetCorrectedValue(double Q, double beta, double rigidity, double mass_on_Z, int type) {
  double pars[8] = {0};
  double x0 = Q; 
  double x[100] = {0};
  double y[100] = {0};
  int    n = 0;
  x[0] = 0;
  y[0] = 0;
  n++;
  double previous = 0;
  double monz = mass_on_Z;
  double bg = 1.0;
  if (mass_on_Z>1e-6) bg = GetBetaGamma(beta,rigidity,monz,type);
  for (int ich=0; ich<GetNCharges(); ich++) {
    int Z = GetZ(ich);
    if (mass_on_Z <= 1e-6) {
      monz  = GetNaiveMassOnZ(Z);
      bg = GetBetaGamma(beta,rigidity,monz,type);
    }
    for (int ipar=0; ipar<8; ipar++) pars[ipar] = (type==0) ? GetPar(ich,ipar) : GetPar(ich,ipar+8);
    double Z_sqrt_edep = Z*sqrt(edep_correction_function(&bg,pars));
    if (Z_sqrt_edep<previous) continue;
    x[n] = Z_sqrt_edep;
    y[n] = Z;
    previous = x[n];
    n++; 
  }
  double y0 = monotonic_cubic_interpolation(x0,n,x,y);
  return y0;
}


double TrEDepTable::GetQ(int Z, double beta, double rigidity, double mass_on_Z, int type) {
  double pars[8] = {0};
  double x0 = Z;
  double x[100] = {0};
  double y[100] = {0};
  int    n = 0;
  x[0] = 0;
  y[0] = 0;
  n++;
  double previous = 0;
  double monz = mass_on_Z;
  double bg = 1.0;
  if (mass_on_Z>1e-6) bg = GetBetaGamma(beta,rigidity,monz,type);
  for (int ich=0; ich<GetNCharges(); ich++) {
    int z = GetZ(ich);
    if (mass_on_Z <= 1e-6) {
      monz  = GetNaiveMassOnZ(Z); // upper or lower case z??
      bg = GetBetaGamma(beta,rigidity,monz,type);
    }
    for (int ipar=0; ipar<8; ipar++) pars[ipar] = (type==0) ? GetPar(ich,ipar) : GetPar(ich,ipar+8);
    double z_sqrt_edep = z*sqrt(edep_correction_function(&bg,pars));
    if (z_sqrt_edep<previous) continue;
    x[n] = z;
    y[n] = z_sqrt_edep;
    previous = y[n];
    n++;
  }
  double y0 = monotonic_cubic_interpolation(x0,n,x,y);
  return y0;
}


double TrEDepTable::FindBeta(int Z, double Q, int type, double beta_min, double beta_max) {
  double default_beta = 0;
  double xf = beta_min;
  double yf = GetQ(Z,xf,0,0,0); 
  double xl = beta_max;
  double yl = GetQ(Z,xl,0,0,0);  
  if (yl>yf) return default_beta; // inverse monotonic
  if ( (Q<yl)||(Q>yf) ) return default_beta; // outside
  int max_steps = 50;
  for (int istep=0; istep<max_steps; istep++) {
    double xm = xf+(xl-xf)/2;
    double ym = GetQ(Z,xm,0,0,0);
    if (fabs(Q-ym)<1e-6) return xm;
    if (ym>yf) return default_beta; // inverse monotonic
    if (ym<yl) return default_beta; // inverse monotonic
    if ( (Q<ym)&&(Q>=yl) ) { 
      xf = xm;
      yf = ym;
    } 
    else if ( (Q>=ym)&&(Q<yf) ) {
      xl = xm;
      yl = ym;
    }
    else {
      return default_beta;
    }
  }
  return default_beta; 
}


double TrEDepTable::GetBetaGamma(double beta, double rigidity, double mass_on_Z, int type) {
  beta = fabs(beta);
  double betar=beta;
  if (beta>BETA_MAX) beta = BETA_MAX;
  double betacut=0.95;
  double betagammacut  = betacut/sqrt(1.-betacut*betacut);
  double betagammabeta = beta/sqrt(1.-beta*beta);
  if (type == 0 || (beta<betacut && beta > 0.01)) {
    return betagammabeta;
  }
  else {
    rigidity = fabs(rigidity);
    double betagammarig=  (rigidity>0) ? rigidity/mass_on_Z : BETAGAMMA_MIN;
    if   ((fabs(betar-1)>1e-6)&&(beta>=betacut)&&(betagammarig<betagammacut)){return betagammabeta;}
    else                                                                     {return betagammarig; }
  }
}


double TrEDepTable::GetLogBetaGamma(double beta, double rigidity, double mass_on_Z, int type) {
  return log10(GetBetaGamma(beta, rigidity, mass_on_Z, type));
}


///////////////////////////
// TrEDepTableSpline
///////////////////////////


TrEDepTableSpline::TrEDepTableSpline() {
  fSplineFoldBeta = 0;
  fSplineRigidity = 0;
}


void TrEDepTableSpline::Init(int nch, int which) {
  TrEDepTableI::Init(nch);
  if (which==0) {
    if (fSplineFoldBeta) delete [] fSplineFoldBeta;
    fSplineFoldBeta = new MonoSpline[GetNCharges()];
  }
  else if (which==1) {
    if (fSplineRigidity) delete [] fSplineRigidity;
    fSplineRigidity = new MonoSpline[GetNCharges()];
  }
}


void TrEDepTableSpline::Clear(Option_t* option) {
  TrEDepTableI::Clear(option);
  if (fSplineFoldBeta) delete [] fSplineFoldBeta;
  fSplineFoldBeta = 0; 
  if (fSplineRigidity) delete [] fSplineRigidity;
  fSplineRigidity = 0; 
}


bool TrEDepTableSpline::InitTableFromTxtFile(char* filename, int which) {
  FILE* file = 0;
  file = fopen(filename,"r");
  if (!file) return false;
  int ncharges;
  fscanf(file,"%d",&ncharges);
  Init(ncharges,which);
  for (int ich=0; ich<GetNCharges(); ich++) {
    int z,extralow,extrahig,nknots;
    int ret = fscanf(file,"%d%d%d%d",&z,&extralow,&extrahig,&nknots);
    if (ret<=0) return false;
    SetZ(ich,z);
    double* xknots = new double[nknots];
    double* yknots = new double[nknots];
    for (int ipar=0; ipar<nknots*2; ipar++) {
      float par;
      int ret = fscanf(file,"%f",&par);
      if (ret<=0) continue;
      if ((ipar%2)==0) xknots[int(ipar/2)] = par;
      else             yknots[int(ipar/2)] = par; 
    }
    if (which==0) fSplineFoldBeta[ich].Set(nknots,xknots,yknots,extralow,extrahig);
    else          fSplineRigidity[ich].Set(nknots,xknots,yknots,extralow,extrahig); 
    delete [] xknots;
    delete [] yknots;
  }
  fclose(file);
  return true;
}


double TrEDepTableSpline::GetCorrectedValue(double Q, double beta, double rigidity, double mass_on_Z, int type) {
  if ( (beta==0)&&(rigidity==0) ) return 0;
  double x0 = Q;
  double* x = new double[GetNCharges()];
  double* y = new double[GetNCharges()];
  // correction with beta
  int n = 0;
  x[0] = 0;
  y[0] = 0;
  n++;
  double previous = 0;
  for (int ich=0; ich<GetNCharges(); ich++) {
    int Z = GetZ(ich);
    if (fabs(beta)<1e-6) {
      // if no beta use rigidity for the beta correction 
      if (mass_on_Z==0) mass_on_Z = GetNaiveMassOnZ(Z);
      double eta = rigidity/mass_on_Z;
      if(eta!=0)beta = 1./sqrt(1.+1./eta/eta);
    }
//    double beta_fold = (fabs(beta)>1) ? 1-(fabs(beta)-1) : fabs(beta);
    double beta_fold = (fabs(beta)>1) ? 1 : fabs(beta);
    double Z_sqrt_edep = (beta_fold>0) ? Z*sqrt(TMath::Max(0.3,fSplineFoldBeta[ich].Eval(beta_fold)))/beta_fold : 0;
    if (Z_sqrt_edep<previous) continue;
    x[n] = Z_sqrt_edep;
    y[n] = Z;
    previous = x[n];
    n++;
  }
  double y0 = monotonic_cubic_interpolation(x0,n,x,y);
  // correction with rigidity (after beta)
  if (fabs(rigidity)>1e-6) {
    x0 = y0;
    n = 0;
    x[0] = 0;
    y[0] = 0;
    n++;
    previous = 0;
    for (int ich=0; ich<GetNCharges(); ich++) {
      int Z = GetZ(ich);
      double Z_sqrt_edep = Z*sqrt(TMath::Max(0.3,fSplineRigidity[ich].Eval(log10(fabs(rigidity)))));
      if (Z_sqrt_edep<previous) continue;
      x[n] = Z_sqrt_edep;
      y[n] = Z;
      previous = x[n];
      n++;
    }
    y0 = monotonic_cubic_interpolation(x0,n,x,y);
  }
  delete [] x;
  delete [] y; 
  return y0;
}


///////////////////////////
// TrEDepTableI
///////////////////////////


TrEDepTableI::TrEDepTableI() {
  fNCharges = 0;
  fZ = 0;
}


void TrEDepTableI::Init(int nch) {
  fNCharges = nch;
  if (fZ) delete [] fZ;
  fZ = new int[GetNCharges()];
  for (int ich=0; ich<GetNCharges(); ich++) SetZ(ich,0);
}


void TrEDepTableI::Clear(Option_t* option) {
  fNCharges = 0;
  if (fZ) delete [] fZ;
  fZ = 0;
}


int TrEDepTableI::GetZ(int ich) {
  if (!fZ) return 0;
  if ( (ich<0)||(ich>=GetNCharges()) ) return 0;
  return fZ[ich];
}


bool TrEDepTableI::SetZ(int ich, int z) {
  if (!fZ) return false;
  if ( (ich<0)||(ich>=GetNCharges()) ) return false;
  fZ[ich] = z;
  return true;
}


/* Line-to-line parametrization 
 *        | M*x + Q                 x>x1
 * f(x) = | sum_{i}^{N}  a_i x^i    x0<x<=x1
 *        | m*x + q                 x<=x0 
 * - pars (M,m,Q,q,a_0,...,a_N,x0,x1) = 2 + 2 + (N+1) + 2 = N + 7
 * - match in continuity and derivative continuity, 4 conditions -> N+3 pars.
 * - pars let free (M,Q,m,q,a_4,...,a_N,x0,x1)  
 * - par[0] is used as the degree of the polynomial 
 */
double TrEDepTableI::line_to_line_fun(double *x, double *par) {
  // function degree
  int degree = int(par[0]);
  // parameters
  double x0 = par[1];
  double m  = par[2];
  double q  = par[3];
  double x1 = par[4];
  double M  = par[5];
  double Q  = par[6];
  // external lines
  double xx = log10(x[0]);
  if (xx>=x1) return M*xx + Q;
  if (xx<=x0) return m*xx + q;
  // polynomial
  double a[100];
  for (int i=4; i<=degree; i++) a[i] = par[i+3];
  double S0 = 0;
  double S1 = 0;
  double s0 = 0;
  double s1 = 0;
  double pow_x0 = x0*x0;
  double pow_x1 = x1*x1;
  for (int i=4; i<=degree; i++) {
    pow_x0 *= x0;         // x0^3 for i == 4, x0^4 for i == 5, ...
    pow_x1 *= x1;         // x1^3 for i == 4, x1^4 for i == 5, ...
    s0 += i*pow_x0*a[i];  // 4 * x0^3 * a[4], ...
    s1 += i*pow_x1*a[i];  // 4 * x1^3 * a[4], ...
    S0 += pow_x0*x0*a[i]; //     x0^4 * a[4], ...
    S1 += pow_x1*x1*a[i]; //     x1^4 * a[4], ...
  }
  a[3] = ( (m-M)*(x1+x0)+2*(q-Q)+2*(S1-S0)-(s0+s1)*(x1-x0) )/((x1-x0)*(x1-x0)*(x1-x0));
  a[2] = ( (M-m)-3*a[3]*(x1-x0)*(x1+x0)-(s1-s0) )/( 2*(x1-x0) );
  a[1] = ( (m*x1-M*x0)+3*a[3]*x0*x1*(x1-x0)-(s0*x1-s1*x0) )/(x1-x0);
  a[0] = (M*x1+Q)-S1-a[3]*x1*x1*x1-a[2]*x1*x1-a[1]*x1;
  // calculate
  double value = 0;
  double pow_xx = 1.0;
  for (int i=0; i<=degree; i++) {
    value += pow_xx*a[i];
    if (i!=degree) pow_xx *= xx;
  }
  return value;
}


double TrEDepTableI::edep_correction_function(double *x, double *par) {
  //
  // this used to be (with x[0] == log10(betagamma)):
  //
  // double pars[] = {4,par[0],par[1],par[2],par[3],par[4],par[5],par[6]};
  // double betagamma = pow(10.,x[0])
  // double beta = sqrt(1.0/(1.0/betagamma/betagamma + 1.0));
  // return exp(par[7]*beta+ln10*line_to_line_fun(x,pars));
  //
  // which can be written more efficiently (with x[0] == betagamma):
  //
  static const double ln10 = log(10);
  double pars[] = {4,par[0],par[1],par[2],par[3],par[4],par[5],par[6]};
  double betagamma = x[0];
  double beta = sqrt(1.0/(1.0/betagamma/betagamma + 1.0));
  return exp(par[7]*beta+ln10*line_to_line_fun(x,pars));
}


double TrEDepTableI::GetNaiveMassOnZ(double Q) {
  double mass_on_Z_proton = 0.938;
  double mass_on_Z_carbon = 0.932*2;
  if      (Q<=1) return mass_on_Z_proton;
  else if (Q>=2) return mass_on_Z_carbon;
  else           return mass_on_Z_proton + (Q-1)*(mass_on_Z_carbon-mass_on_Z_proton);
}

