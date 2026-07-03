//  $Id: JACEEFlux.h,v 1.1 2020/04/26 09:29:27 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  JACEEFLUX
///\data  2016/06/28 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _JACEEFLUX_
#define _JACEEFLUX_
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
/// TGraphAsymmErrors*  FEFLUXEJACEE : JACEE iron(Fe) flux VS Ekin

//#  Iron(Fe) Flux in Ekin/N[GeV/n] JACEE(1979+1980+1982+1983+1984+1986+1987+1988+1990+1994+1995)
const int jaceefenb=7;

double JACEEFEENX[jaceefenb]={
470,700,1040,1600,3400,11000,95000
};

double JACEEFEENXE[jaceefenb]={
0,0,0,0,0,0,0
};

double JACEEFEENY[jaceefenb]={
4.31e-07,1.33e-07,2.99e-08,7.39e-09,3.57e-09,1.03e-10,2.31e-12
};

double JACEEFEENYEL[jaceefenb]={
2.39e-07,4.86e-08,1.22e-08,2.8e-09,1.19e-09,5.42e-11,1.85e-12
};

double JACEEFEENYEH[jaceefenb]={
2.39e-07,4.86e-08,1.22e-08,2.8e-09,1.19e-09,5.42e-11,2.31e-12
};

TGraphAsymmErrors*  FEFLUXEJACEE= new TGraphAsymmErrors(jaceefenb,JACEEFEENX,JACEEFEENY,JACEEFEENXE,JACEEFEENXE,JACEEFEENYEL,JACEEFEENYEH);
#endif
