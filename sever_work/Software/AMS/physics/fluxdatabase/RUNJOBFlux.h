//  $Id: RUNJOBFlux.h,v 1.1 2020/04/26 09:29:27 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  RUNJOBFLUX
///\data  2016/06/28 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _RUNJOBFLUX_
#define _RUNJOBFLUX_
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
/// TGraphAsymmErrors*  FEFLUXERUNJOB : RUNJOB iron(Fe) flux VS Ekin

//#  Iron(Fe) Flux in Ekin/N[GeV/n] RUNJOB(1995-1999)
const int runjobfenb=3;

double RUNJOBFEENX[runjobfenb]={
711,1500,4330
};

double RUNJOBFEENXE[runjobfenb]={
0,0,0
};

double RUNJOBFEENY[runjobfenb]={
8.05e-08,8.61e-09,4.92e-10
};

double RUNJOBFEENYEL[runjobfenb]={
5.23e-08,4.66e-09,2.66e-10
};

double RUNJOBFEENYEH[runjobfenb]={
1.09e-07,8.33e-09,4.76e-10
};

TGraphAsymmErrors*  FEFLUXERUNJOB= new TGraphAsymmErrors(runjobfenb,RUNJOBFEENX,RUNJOBFEENY,RUNJOBFEENXE,RUNJOBFEENXE,RUNJOBFEENYEL,RUNJOBFEENYEH);
#endif
