//  $Id: EcalShowerProfileFit.C,v 1.1 2015/06/22 22:11:57 nzimmerm Exp $
// Extracted from ecalrec.C on 17.06.2015, by nzimmerm.

#include <float.h>
#include <fenv.h>
#include <cmath>
#include "EcalShowerProfileFit.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "commons.h"
#endif

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);
extern "C" void d01amf_(void *alfun, number &bound, integer &inf, number &epsa, number &epsr, number &result, number &abserr, number w[], int &lw, int iw[], int &liw , int &ifail, void *p );

EcalShowerProfileFit::EcalShowerProfileFit()
  : _Direction(0)
  , _iflag(0)
  , _Dz(0.0)
  , _Et(0.0)
  , _ShowerMax(0) {

  for (int i = 0; i < 10; ++i)
    _ProfilePar[i] = 0;

  for (int i = 0; i < 2*_ECSLMX; ++i) {
    _Edep[i] = 0.0;
    _Ez[i] = 0.0;
    _Zcorr[i] = 0.0;
  }
}

void EcalShowerProfileFit::ProfileFit() {

#ifndef NO_NAG
  void (*palfun)(int &n, double x[], double &f, EcalShowerProfileFit *p)=&EcalShowerProfileFit::gamfun;
  void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
    &EcalShowerProfileFit::monit;
  void (*psalfun)(double &x, double &f, EcalShowerProfileFit *p)=&EcalShowerProfileFit::gamfunr;
#endif

#ifndef __DARWIN__
  int env=0;
  env=fegetexcept();

  static integer MISCFFKEY_RaiseFPE=0; // pass6 settingE
#ifndef __ROOTSHAREDLIBRARY__
  MISCFFKEY_RaiseFPE = MISCFFKEY.RaiseFPE;
#endif

  if(MISCFFKEY_RaiseFPE<=2){
    fedisableexcept(FE_OVERFLOW);
    fedisableexcept(FE_INVALID);
  }
#endif
  // NowFit
  const integer Maxrow=_LayersNo;

  integer ifail=1;
  const integer mp=4;
  number f=0,x[mp];
  x[0]=1;
  x[1]=_Dz*_ShowerMax;
  x[2]=1;
  _Direction=0;
#ifndef NO_NAG
  integer n=3;
  number tol=3.99e-2;
  integer maxcal=25000;
  number w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
  integer iw=n+1;
  e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
#endif     
  if(ifail==0){
    _ProfilePar[0]=x[0];    
    _ProfilePar[1]=x[1];    
    _ProfilePar[2]=x[2]!=0?1./x[2]:FLT_MAX;    
    _ProfilePar[4]=f;
    //    cout << "ecalshower::profilefit finished "<<ifail<<" "<<f<<endl;
    _iflag=3;
    ifail=1;
#ifndef NO_NAG
    integer one=1;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
#endif
    // Leak Estimation
#ifndef NO_NAG
    const integer lwc=1000;
    const integer liwc=lwc/4;
    number ww[lwc];
    integer iww[liwc];
#endif
    number bound,epsa,epsr,result=0;
    integer inf;
    ifail=1;
    for(int i=Maxrow-1;i>=0;i--){
      if(_Edep[i]){
    bound=_Ez[i]-_Ez[0]+_Dz/2;
    break;
      }
    }
    epsa=1.e-4;
    epsr=1.e-3;
    inf=1;
#ifndef NO_NAG
    int liw=liwc;
    int lw=lwc;
    number abserr;
    d01amf_((void*)psalfun, bound, inf, epsa, epsr,result,abserr,ww,lw,iww,liw,ifail,this);
#endif
    if(ifail==0){
      _ProfilePar[3]=result;
    }
    else{
      _ProfilePar[3]=-1;
    }
  }
  else{
    _ProfilePar[0]=0;
    _ProfilePar[1]=0;
    _ProfilePar[2]=0;
    _ProfilePar[4]=FLT_MAX;
  }
  // Try To inverted fit
  for(int i=0;i<(Maxrow+1)/2;i++){
    number tmpz=-_Ez[i];
    number tmpe=_Edep[i];
    _Ez[i]=-_Ez[Maxrow-1-i];
    _Edep[i]=_Edep[Maxrow-1-i];
    _Ez[Maxrow-1-i]=tmpz;
    _Edep[Maxrow-1-i]=tmpe;
  }
  x[0]=1;
  x[1]=_Dz*(Maxrow-_ShowerMax);
  x[2]=1;
  ifail=1;
#ifndef NO_NAG
  e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
#endif
  //    cout << "ecalshower::profilefit finished "<<ifail<<" "<<f<<endl;
  if(ifail==0){
    _ProfilePar[5]=x[0];    
    _ProfilePar[6]=x[1];    
    _ProfilePar[7]=x[2]!=0?1./x[2]:FLT_MAX;    
    _ProfilePar[9]=f;

    number bound,epsa,epsr,result=0;
    integer inf;
    ifail=1;
    epsa=1.e-4;
    epsr=1.e-3;
    inf=1;
    for(int i=Maxrow-1;i>=0;i--){
      if(_Edep[i]){
    bound=_Ez[i]-_Ez[0]+_Dz/2;
    break;
      }
    }
    ifail=1;
#ifndef NO_NAG     
    const integer lwc=1000;
    const integer liwc=lwc/4;
    number abserr, ww[lwc];
    integer iww[liwc];
    int liw=liwc;
    int lw=lwc;
    d01amf_((void*)psalfun, bound, inf, epsa, epsr,result,abserr,ww,lw,iww,liw,ifail,this);
#endif
    if(ifail==0){
      _ProfilePar[8]=result;
    }
    else _ProfilePar[8]=-1; 
  }
  else{
    _ProfilePar[5]=0;
    _ProfilePar[6]=0;
    _ProfilePar[7]=0;
    _ProfilePar[9]=FLT_MAX;
  }
  if(_ProfilePar[9]<_ProfilePar[4]){
    _iflag=3;
    ifail=1;
#ifndef NO_NAG
    integer one=1;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
#endif
    _Direction=1;
  }
#ifndef __DARWIN__
  feclearexcept(FE_OVERFLOW);
  feclearexcept(FE_INVALID);
  if(env){
    feenableexcept(env);        
  }
#endif    
}

void EcalShowerProfileFit::gamfunr(number& x, number &fc, EcalShowerProfileFit *p){
  fc=0;
  if(x>0){
    number et=p->_Et;
    number xc[3];
    for(int i=0;i<3;i++){
      xc[i]=p->_ProfilePar[i+5*p->_Direction];
    }
   
    number aa=-x/xc[2];
   
    if(et && xc[2]!=0){
      fc=pow(x,xc[1]/xc[2])*exp(aa);
      fc*=xc[0]/et;
    }
  }
}

void EcalShowerProfileFit::gamfun(integer &n, number xc[], number &fc, EcalShowerProfileFit *p){
  /*
    PROTOCALLSFFUN4(DOUBLE,DGAUSS,dgauss,ROUTINE,DOUBLE,DOUBLE,DOUBLE)
    #define DGAUSS(A2,A3,A4,A5) CCALLSFFUN4(DGAUSS,dgauss,,ROUTINE,DOUBLE,DOUBLE,DOUBLE,A2,A3,A4,A5)
  */
  const integer Maxrow=_LayersNo;
  fc=0;
  for(int i=0;i<n;i++){
    if(xc[i]<0){
      fc=FLT_MAX;
      return;
    }
  }
  if(xc[1]>p->_Dz*Maxrow){
    fc=FLT_MAX;
    return;
  }
  {
    number et=0;
    const integer nint=7;
    for (int i=2;i<Maxrow;i++){
      number edep=0;
      number dz=i<Maxrow-1?p->_Ez[i+1]-p->_Ez[i]:p->_Dz;
      for(int j=0;j<nint;j++){
    number x1=p->_Ez[i]-p->_Ez[0]+dz*(j)/nint;
    number x2=p->_Ez[i]-p->_Ez[0]+dz*(j+1)/nint;
    /*
      number p1=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
      number p2=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
    */
    number p1=x1>0?exp(log(x1)*xc[1]*xc[2]-xc[2]*x1):0;
    number p2=x2>0?exp(log(x2)*xc[1]*xc[2]-xc[2]*x2):0;

    edep+= (p1+p2)*0.5*dz/nint;
      }
      et+=edep;
    } 
    for (int i=2;i<Maxrow;i++){
      if(p->_Edep[i]){
    number edep=0;
    number dz=i<Maxrow-1?p->_Ez[i+1]-p->_Ez[i]:p->_Dz;
    for(int j=0;j<nint;j++){
      number x1=p->_Ez[i]-p->_Ez[0]+dz*(j)/nint;
      number x2=p->_Ez[i]-p->_Ez[0]+dz*(j+1)/nint;
      /*
        number p1=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
        number p2=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
      */
      number p1=x1>0?exp(log(x1)*xc[1]*xc[2]-xc[2]*x1):0;
      number p2=x2>0?exp(log(x2)*xc[1]*xc[2]-xc[2]*x2):0;
      edep+= (p1+p2)*0.5*dz/nint;
    }
    fc+=(edep/et*xc[0]-p->_Edep[i])*(edep/et*xc[0]-p->_Edep[i])/p->_Edep[i]*70;
      }
    }
    // cout <<" xc "<<xc[0]<< " "<<xc[1]<<" "<<xc[2]<<" "<<fc<<endl;  
  }
  if(p->_iflag==3){
    p->_Et=0;
    const integer nint=7;
    for (int i=0;i<Maxrow;i++){
      number edep=0;
      number edepz=0;
      number dz=i<Maxrow-1?p->_Ez[i+1]-p->_Ez[i]:p->_Dz;
      for(int j=0;j<nint;j++){
    number x1=p->_Ez[i]-p->_Ez[0]+dz*(j)/nint;
    number x2=p->_Ez[i]-p->_Ez[0]+dz*(j+1)/nint;
    //    number f3=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
    //    number f4=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
    number f1=x1>0?exp(min(number(log(FLT_MAX/2)),-xc[2]*x1+xc[1]*xc[2]*log(x1))):0;
    number f2=x2>0?exp(min(number(log(FLT_MAX/2)),-xc[2]*x2+xc[1]*xc[2]*log(x2))):0;
    //    cout<<" f4/f2 "<<f4/f2<<endl;
    edep+= (f1+f2)*0.5;
    p->_Et+=edep*dz/nint;
    edepz+=(f1*(x2+2*x1)+f2*(x1+2*x2))/6.;
      }
      number zcorr=-(edepz/edep-(p->_Ez[i]-p->_Ez[0]+dz/2));
      if(fabs(zcorr)/fabs(dz)<1){
    p->_Zcorr[i]=-(edepz/edep-(p->_Ez[i]-p->_Ez[0]+dz/2));
      }
      else{
    p->_Zcorr[i]=0;
    cerr<<"EcalShowerProfileFit::Energyfit-W-TooBigZcorrAttempted "<<zcorr<<" "<<dz<<endl;
      }
    }
    p->_iflag=4;
  }
}
