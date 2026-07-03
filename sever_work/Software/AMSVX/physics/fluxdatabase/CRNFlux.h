//  $Id: CRNFlux.h,v 1.5 2020/04/26 09:29:26 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  CRNFlux
///\data  2016/06/28 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _CRNFlux_
#define _CRNFlux_
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
/// TGraphErrors*      CFLUXECRN    : CRN-Spacelab2 carbon flux VS Ekin
/// TGraphErrors*      BFLUXECRN    : CRN-Spacelab2 boron  flux VS Ekin
/// TGraphErrors*      BCRATIOECRN  : CRN-Spacelab2 boron/carbon  ratio VS Ekin
/// TGraphErrors*      NFLUXECRN    : CRN-Spacelab2 nitrogen flux VS Ekin
/// TGraphErrors*      OFLUXECRN    : CRN-Spacelab2 oxygen flux VS Ekin
/// TGraphErrors*      CORATIOECRN  : CRN-Spacelab2 carbon/oxygen ratio VS Ekin
/// TGraphAsymmErrors* SIFLUXECRN   : CRN-Spacelab2 silicon flux VS Ekin
/// TGraphAsymmErrors* FEFLUXECRN   : CRN-Spacelab2 iron(Fe) flux VS Ekin

//#  Carbon Flux in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crncnb=4;

double CRNCENX[crncnb]={
72.6,102.6,206.8,1503
};

double CRNCENXE[crncnb]={
0,0,0,0
};

double CRNCENY[crncnb]={
0.000154,6.26e-05,1.04e-05,4.35e-08
};

double CRNCENYE[crncnb]={
2e-05,3.7e-05,1.1e-06,1e-08
};

TGraphErrors*  CFLUXECRN= new TGraphErrors(crncnb,CRNCENX,CRNCENY,CRNCENXE,CRNCENYE);


//#  Boron Flux in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnbnb=3;

double CRNBENX[crnbnb]={
73,103,207
};

double CRNBENXE[crnbnb]={
0,0,0
};

double CRNBENY[crnbnb]={
1.8e-05,5.9e-06,6.4e-07
};

double CRNBENYE[crnbnb]={
7e-06,2.6e-06,2.8e-07
};

TGraphErrors*  BFLUXECRN= new TGraphErrors(crnbnb,CRNBENX,CRNBENY,CRNBENXE,CRNBENYE);


//#  Boron/Carbon ratio in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnbcnb=3;

double CRNBCENX[crnbcnb]={
73,103,207
};

double CRNBCENXE[crnbcnb]={
0,0,0
};

double CRNBCENY[crnbcnb]={
0.119,0.0943,0.0615
};

double CRNBCENYE[crnbcnb]={
0.0479,0.0695,0.0277
};

TGraphErrors*  BCRATIOECRN= new TGraphErrors(crnbcnb,CRNBCENX,CRNBCENY,CRNBCENXE,CRNBCENYE);


//#  Nitrogen Flux in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnnnb=3;

double CRNNENX[crnnnb]={
73,103,207,
};

double CRNNENXE[crnnnb]={
0,0,0,
};

double CRNNENY[crnnnb]={
1.74e-05,5.5e-06,1.3e-06,
};

double CRNNENYE[crnnnb]={
2.1e-06,1.1e-06,3e-07,
};

TGraphErrors*  NFLUXECRN= new TGraphErrors(crnnnb,CRNNENX,CRNNENY,CRNNENXE,CRNNENYE);


//#  Oxygen Flux in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnonb=4;

double CRNOENX[crnonb]={
72.6,102.6,206.8,1503
};

double CRNOENXE[crnonb]={
0,0,0,0
};

double CRNOENY[crnonb]={
0.000188,7.38e-05,1.16e-05,5.9e-08
};

double CRNOENYE[crnonb]={
1.8e-05,1e-05,1.2e-06,1.2e-08
};

TGraphErrors*  OFLUXECRN= new TGraphErrors(crnonb,CRNOENX,CRNOENY,CRNOENXE,CRNOENYE);


//#  Carbon/Oxygen ratio in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnconb=4;

double CRNCOENX[crnconb]={
72.6,102.6,206.8,1503
};

double CRNCOENXE[crnconb]={
0,0,0,0
};

double CRNCOENY[crnconb]={
0.819149,0.848238,0.896552,0.737288
};

double CRNCOENYE[crnconb]={
0.132168,0.514361,0.132643,0.226306
};

TGraphErrors*  CORATIOECRN= new TGraphErrors(crnconb,CRNCOENX,CRNCOENY,CRNCOENXE,CRNCOENYE);

//#  Silicon Flux in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnsinb=5;

double CRNSIENX[crnsinb]={
50,61.3,82.7,155.1,1028,
};

double CRNSIENXE[crnsinb]={
0,0,0,0,0,
};

double CRNSIENY[crnsinb]={
9.46e-05,4.94e-05,2.13e-05,3.72e-06,5.6e-09,
};

double CRNSIENYEL[crnsinb]={
1.54e-05,9.3e-06,3.9e-06,4.7e-07,4.6e-09
};

double CRNSIENYEH[crnsinb]={
1.54e-05,9.3e-06,3.9e-06,4.7e-07,1.26e-08,
};

TGraphAsymmErrors*  SIFLUXECRN= new TGraphAsymmErrors(crnsinb,CRNSIENX,CRNSIENY,CRNSIENXE,CRNSIENXE,CRNSIENYEL,CRNSIENYEH);

//#  Iron(Fe) Flux in Ekin/N[GeV/n] CRN-Spacelab2(1985/07-1985/08)
const int crnfenb=8;

double CRNFEENX[crnfenb]={
52.6,59.2,68.6,81.6,115.4,222,649,1563
};

double CRNFEENXE[crnfenb]={
0,0,0,0,0,0,0,0
};

double CRNFEENY[crnfenb]={
6.23e-05,4.43e-05,3.29e-05,1.88e-05,7.5e-06,1.5e-06,9.24e-08,9.23e-09
};

double CRNFEENYEL[crnfenb]={
1.11e-05,6.7e-06,5.7e-06,3e-06,1e-06,2.8e-07,3.02e-08,5.01e-09
};

double CRNFEENYEH[crnfenb]={
1.11e-05,6.7e-06,5.7e-06,3e-06,1e-06,2.8e-07,4.12e-08,8.77e-09
};

TGraphAsymmErrors*  FEFLUXECRN= new TGraphAsymmErrors(crnfenb,CRNFEENX,CRNFEENY,CRNFEENXE,CRNFEENXE,CRNFEENYEL,CRNFEENYEH);

#endif
