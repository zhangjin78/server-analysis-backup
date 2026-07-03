//  $Id: CREAMFlux.h,v 1.9 2020/04/26 09:29:26 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  CREAMFlux
///\data  2014/09/30 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _CREAMFlux_
#define _CREAMFlux_
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
/// TGraphAsymmErrors* PRFLUXECREAM  : CREAM-I  proton flux VS Ekin
/// TGraphAsymmErrors* HEFLUXECREAM  : CREAM-I  helium flux VS Ekin
/// TGraphAsymmErrors* CFLUXECREAM   : CREAM-II carbon flux VS Ekin
/// TGraphErrors*      BCRATIOECREAM : CREAM-I  boron/carbon  ratio VS Ekin
/// TGraphAsymmErrors* NFLUXECREAM   : CREAM-II nitrogen flux VS Ekin
/// TGraphAsymmErrors* OFLUXECREAM   : CREAM-II oxygen flux VS Ekin
/// TGraphErrors*      CORATIOECREAM : CREAM-I  carbon/oxygen ratio VS Ekin
/// TGraphAsymmErrors* SIFLUXECREAM  : CREAM-II silicon flux VS Ekin
/// TGraphAsymmErrors* FEFLUXECREAM  : CREAM-II iron(fe) flux VS Ekin

//#   proton Flux in Ekin[GeV] CREAM-I(2004/12-2005/01)
const int creamprnb=10;

double CREAMPRENX[creamprnb]={
3161.34,5019.02,7936.31,12648.2,19999.1,31621.9,50198.7,79371.6,126490,199999
};

double CREAMPRENXE[creamprnb]={
0,0,0,0,0,0,0,0,0,0
};

double CREAMPRENY[creamprnb]={
3.72e-06,1.1e-06,3.19e-07,9.47e-08,2.8e-08,8.1e-09,2.2e-09,6.1e-10,1.8e-10,4.2e-11
};

double CREAMPRENYEL[creamprnb]={
1e-07,4e-08,1.9e-08,8e-09,3.5e-09,1.5e-09,6e-10,2.2e-10,9e-11,3.4e-11
};

double CREAMPRENYEH[creamprnb]={
1e-07,4e-08,1.9e-08,8e-09,3.5e-09,1.5e-09,6e-10,2.6e-10,1.2e-10,5.4e-11
};

TGraphAsymmErrors*  PRFLUXECREAM= new TGraphAsymmErrors(creamprnb,CREAMPRENX,CREAMPRENY,CREAMPRENXE,CREAMPRENXE,CREAMPRENYEL,CREAMPRENYEH);


//#   He Flux in Ekin/N[GeV/n] CREAM-I(2004/12-2005/01)
const int creamhenb=10;

double CREAMHEENX[creamhenb]={
789.638,1254.06,1983.38,3161.34,4999.07,7904.77,12549,19842.2,31621.8,49999.1
};

double CREAMHEENXE[creamhenb]={
0,0,0,0,0,0,0,0,0,0
};

double CREAMHEENY[creamhenb]={
1.42e-05,4.35e-06,1.31e-06,3.83e-07,1.27e-07,4.19e-08,1.15e-08,3.4e-09,8.2e-10,2.9e-10
};

double CREAMHEENYEL[creamhenb]={
4e-07,1.6e-07,7e-08,3.1e-08,1.4e-08,6.4e-09,2.7e-09,1e-09,3.8e-10,1.5e-10
};

double CREAMHEENYEH[creamhenb]={
4e-07,1.6e-07,7e-08,3.1e-08,1.4e-08,6.4e-09,2.7e-09,1.1e-09,4.9e-10,2.4e-10
};

TGraphAsymmErrors*  HEFLUXECREAM= new TGraphAsymmErrors(creamhenb,CREAMHEENX,CREAMHEENY,CREAMHEENXE,CREAMHEENXE,CREAMHEENYEL,CREAMHEENYEH);


//#  Carbon Flux in Ekin/N[GeV/n] CREAM-II(2005/12-2006/01)
const int creamcnb=9;

double CREAMCENX[creamcnb]={
86.2,141.6,228.2,397.2,691,1201.5,2088.7,3630.5,7415.1
};

double CREAMCENXE[creamcnb]={
0,0,0,0,0,0,0,0,0
};

double CREAMCENY[creamcnb]={
0.000167,3.37e-05,9.42e-06,2.28e-06,5.17e-07,1.31e-07,3.6e-08,1.04e-08,1.49e-09
};

double CREAMCENYEL[creamcnb]={
1.3e-05,2.9e-06,6.4e-07,2e-07,6.3e-08,2.4e-08,1e-08,4e-09,6.3e-10
};

double CREAMCENYEH[creamcnb]={
1.3e-05,2.9e-06,6.4e-07,2e-07,6.3e-08,2.4e-08,1.3e-08,5.8e-09,9.8e-10
};

TGraphAsymmErrors*  CFLUXECREAM= new TGraphAsymmErrors(creamcnb,CREAMCENX,CREAMCENY,CREAMCENXE,CREAMCENXE,CREAMCENYEL,CREAMCENYEH);


//#  Boron/Carbon Ratio in Ekin/N[GeV/n] CREAM-I(2004/12-2005/01)
const int creambcnb=6;

double CREAMBCENX[creambcnb]={
1.4,5.7,23,91,363,1450
};

double CREAMBCENXE[creambcnb]={
0.2,0.9,3,14,54,217
};

double CREAMBCENY[creambcnb]={
0.32,0.225,0.155,0.101,0.071,0.033
};

double CREAMBCENYE[creambcnb]={
0.0308058,0.0203961,0.0148661,0.0148661,0.0269258,0.0269258
};

TGraphErrors*  BCRATIOECREAM= new TGraphErrors(creambcnb,CREAMBCENX,CREAMBCENY,CREAMBCENXE,CREAMBCENYE);

//#  Nitrogen Flux in Ekin/N[GeV/n] CREAM-II(2005/12-2006/01)
const int creamnnb=3;

double CREAMNENX[creamnnb]={
95.7,295.3,826.3,
};

double CREAMNENXE[creamnnb]={
0,0,0,
};

double CREAMNENY[creamnnb]={
7.28e-06,5.39e-07,3.8e-08,
};

double CREAMNENYEL[creamnnb]={
7.9e-07,6.8e-08,9e-09,
};

double CREAMNENYEH[creamnnb]={
7.9e-07,6.8e-08,1.1e-08,
};

TGraphAsymmErrors*  NFLUXECREAM= new TGraphAsymmErrors(creamnnb,CREAMNENX,CREAMNENY,CREAMNENXE,CREAMNENXE,CREAMNENYEL,CREAMNENYEH);

//#  Oxygen Flux in Ekin/N[GeV/n] CREAM-II(2005/12-2006/01)
const int creamonb=9;

double CREAMOENX[creamonb]={
64,120.8,228,396.9,690.5,1200.5,2087,3627.4,7287.1
};

double CREAMOENXE[creamonb]={
0,0,0,0,0,0,0,0,0
};

double CREAMOENY[creamonb]={
0.000348,4.82e-05,7.33e-06,1.9e-06,4.51e-07,1.25e-07,2.9e-08,8.3e-09,8.4e-10
};

double CREAMOENYEL[creamonb]={
2.3e-05,2.5e-06,4.7e-07,1.7e-07,6e-08,2.5e-08,9e-09,3.3e-09,5.4e-10
};

double CREAMOENYEH[creamonb]={
2.3e-05,2.5e-06,4.7e-07,1.7e-07,6e-08,2.5e-08,1.2e-08,4.9e-09,9.7e-10
};

TGraphAsymmErrors*  OFLUXECREAM= new TGraphAsymmErrors(creamonb,CREAMOENX,CREAMOENY,CREAMOENXE,CREAMOENXE,CREAMOENYEL,CREAMOENYEH);


//#  Carbon/Oxygen Ratio in Ekin/N[GeV/n] CREAM-I(2004/12-2005/01)
const int creamconb=6;

double CREAMCOENX[creamconb]={
1.4,5.7,23,91,363,1450
};

double CREAMCOENXE[creamconb]={
0.2,0.9,3,14,54,217
};

double CREAMCOENY[creamconb]={
1.1,1.11,1.16,1.25,1.25,0.66
};

double CREAMCOENYE[creamconb]={
0.100499,0.100499,0.104403,0.141421,0.335261,0.422019
};

TGraphErrors*  CORATIOECREAM= new TGraphErrors(creamconb,CREAMCOENX,CREAMCOENY,CREAMCOENXE,CREAMCOENYE);


//#  Silicon Flux in Ekin/N[GeV/n] CREAM-II(2005/12-2006/01)
const int creamsinb=9;

double CREAMSIENX[creamsinb]={
27,47,74.9,130.8,228,397,690.6,1200.7,2418.3,
};

double CREAMSIENXE[creamsinb]={
0,0,0,0,0,0,0,0,0,
};

double CREAMSIENY[creamsinb]={
0.000454,0.000135,3.03e-05,5.04e-06,1.5e-06,3.88e-07,1.15e-07,1.9e-08,3.2e-09,
};

double CREAMSIENYEL[creamsinb]={
5.6e-05,1.7e-05,2.8e-06,5.3e-07,2e-07,7.9e-08,3.5e-08,9e-09,1.8e-09,
};

double CREAMSIENYEH[creamsinb]={
5.6e-05,1.7e-05,2.8e-06,5.3e-07,2e-07,7.9e-08,4.8e-08,1.7e-08,3.6e-09,
};

TGraphAsymmErrors*  SIFLUXECREAM= new TGraphAsymmErrors(creamsinb,CREAMSIENX,CREAMSIENY,CREAMSIENXE,CREAMSIENXE,CREAMSIENYEL,CREAMSIENYEH);


//#  Iron(Fe) Flux in Ekin/N[GeV/n] CREAM-II(2005/12-2006/01)
const int creamfenb=8;

double CREAMFEENX[creamfenb]={
25.4,47,74.9,131,228.2,397.2,690.9,1406.9
};

double CREAMFEENXE[creamfenb]={
0,0,0,0,0,0,0,0
};

double CREAMFEENY[creamfenb]={
0.000519,8.38e-05,2.06e-05,4.46e-06,1.36e-06,4e-07,1.09e-07,1.6e-08
};

double CREAMFEENYEL[creamfenb]={
7.70612e-05,1.23398e-05,3.26574e-06,9.64081e-07,3.85802e-07,1.36252e-07,4.92708e-08,8.16477e-09
};

double CREAMFEENYEH[creamfenb]={
7.70612e-05,1.23398e-05,3.26574e-06,9.64081e-07,4.80461e-07,1.84566e-07,7.97785e-08,1.40948e-08
};

TGraphAsymmErrors*  FEFLUXECREAM= new TGraphAsymmErrors(creamfenb,CREAMFEENX,CREAMFEENY,CREAMFEENXE,CREAMFEENXE,CREAMFEENYEL,CREAMFEENYEH);
#endif
