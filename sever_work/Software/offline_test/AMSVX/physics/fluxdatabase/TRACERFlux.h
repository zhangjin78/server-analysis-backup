//  $Id: TRACERFlux.h,v 1.4 2020/04/26 09:29:27 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  TRACERFlux
///\data  2016/06/28 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _TRACERFlux_
#define _TRACERFlux_
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
/// TGraphAsymmErrors*  CFLUXETRACER    : TRACER06 carbon flux VS Ekin
/// TGraphAsymmErrors*  BFLUXETRACER    : TRACER06 boron  flux VS Ekin
/// TGraphAsymmErrors*  BCRATIOETRACER  : TRACER06 boron/carbon  ratio VS Ekin
/// TGraphAsymmErrors*  OFLUXETRACER    : TRACER06 oxygen flux VS Ekin
/// TGraphAsymmErrors*  CORATIOETRACER  : TRACER06 carbon/oxygen ratio VS Ekin
/// TGraphAsymmErrors*  SIFLUXETRACER99 : TRACER99 silicon flux VS Ekin
/// TGraphAsymmErrors*  SIFLUXETRACER   : TRACER03 silicon flux VS Ekin
/// TGraphAsymmErrors*  FEFLUXETRACER99 : TRACER99 iron(Fe) flux VS Ekin
/// TGraphAsymmErrors*  FEFLUXETRACER   : TRACER03 iron(Fe) flux VS Ekin
/// TGraphAsymmErrors*  FEFLUXETRACER06 : TRACER06 iron(Fe) flux VS Ekin

//#  Carbon Flux in Ekin/N[GeV/n] TRACER06(2006/07)
const int tracercnb=6;

double TRACERCENX[tracercnb]={
0.9,1.1,1.5,26,181,2300
};

double TRACERCENXE[tracercnb]={
0,0,0,0,0,0
};

double TRACERCENY[tracercnb]={
3.19,2.19,1.19,0.00217,1.7e-05,2.2e-08
};

double TRACERCENYEL[tracercnb]={
0.107703,0.104403,0.10198,6.08276e-05,7.01142e-06,1.58114e-08,
};

double TRACERCENYEH[tracercnb]={
0.107703,0.104403,0.10198,6.08276e-05,5.01597e-06,1.27279e-08
};

TGraphAsymmErrors*  CFLUXETRACER= new TGraphAsymmErrors(tracercnb,TRACERCENX,TRACERCENY,TRACERCENXE,TRACERCENXE,TRACERCENYEL,TRACERCENYEH);


//#  Boron Flux in Ekin/N[GeV/n] TRACER06(2006/07)
const int tracerbnb=6;

double TRACERBENX[tracerbnb]={
0.9,1.1,1.5,23.8,173,2070
};

double TRACERBENXE[tracerbnb]={
0,0,0,0,0,0
};

double TRACERBENY[tracerbnb]={
1.13,0.73,0.38,0.00049,1.6e-06,4e-09
};

double TRACERBENYEL[tracerbnb]={
0.104403,0.0223607,0.0141421,0.000100499,4.12311e-07,3.60555e-09
};

double TRACERBENYEH[tracerbnb]={
0.104403,0.0223607,0.0141421,0.000100499,4.12311e-07,8.24621e-09
};

TGraphAsymmErrors*  BFLUXETRACER= new TGraphAsymmErrors(tracerbnb,TRACERBENX,TRACERBENY,TRACERBENXE,TRACERBENXE,TRACERBENYEL,TRACERBENYEH);


//#  Boron/Carbon ratio in Ekin/N[GeV/n] TRACER06(2006/07)
const int tracerbcnb=6;

double TRACERBCENX[tracerbcnb]={
0.903,1.117,1.412,23.682,169.818,2075.45
};

double TRACERBCENXE[tracerbcnb]={
0,0,0,0,0,0
};

double TRACERBCENY[tracerbcnb]={
0.354,0.334,0.321,0.178,0.0825,0.121
};

double TRACERBCENYEL[tracerbcnb]={
0.0148,0.01789,0.03942,0.00961,0.01126,0.08139
};

double TRACERBCENYEH[tracerbcnb]={
0.01811,0.01562,0.03535,0.0062,0.01202,0.144
};

TGraphAsymmErrors*  BCRATIOETRACER= new TGraphAsymmErrors(tracerbcnb,TRACERBCENX,TRACERBCENY,TRACERBCENXE,TRACERBCENXE,TRACERBCENYEL,TRACERBCENYEH);


//#  Oxygen Flux in Ekin/N[GeV/n] TRACER06(2006/07)
const int traceronb=6;

double TRACEROENX[traceronb]={
0.9,1.1,1.5,26,181,2300
};

double TRACEROENXE[traceronb]={
0,0,0,0,0,0
};

double TRACEROENY[traceronb]={
3.26,2.17,1.28,0.00225,1.91e-05,1.9e-08
};

double TRACEROENYEL[traceronb]={
0.0984886,0.0583095,0.0538517,6.08276e-05,8.00999e-06,1.253e-08
};

double TRACEROENYEH[traceronb]={
0.0984886,0.0583095,0.0538517,6.08276e-05,6.01332e-06,1e-08
};

TGraphAsymmErrors*  OFLUXETRACER= new TGraphAsymmErrors(traceronb,TRACEROENX,TRACEROENY,TRACEROENXE,TRACEROENXE,TRACEROENYEL,TRACEROENYEH);


//#  Carbon/Oxygen ratio in Ekin/N[GeV/n] TRACER06(2006/07)
const int tracerconb=6;

double TRACERCOENX[tracerconb]={
0.9,1.1,1.5,26,181,2300
};

double TRACERCOENXE[tracerconb]={
0,0,0,0,0,0
};

double TRACERCOENY[tracerconb]={
0.978528,1.00922,0.929688,0.964445,0.890052,1.15789
};

double TRACERCOENYEL[tracerconb]={
0.0443333,0.0552284,0.0887554,0.037559,0.523526,1.12943
};

double TRACERCOENYEH[tracerconb]={
0.0443333,0.0552284,0.0887554,0.037559,0.384044,0.905618
};

TGraphAsymmErrors*  CORATIOETRACER= new TGraphAsymmErrors(tracerconb,TRACERCOENX,TRACERCOENY,TRACERCOENXE,TRACERCOENXE,TRACERCOENYEL,TRACERCOENYEH);

//#  Silicon Flux in Ekin/N[GeV/n] TRACER99
const int tracer99sinb=4;

double TRACER99SIENX[tracer99sinb]={
12.7,115,480,1880
};

double TRACER99SIENXE[tracer99sinb]={
0,0,0,0
};

double TRACER99SIENY[tracer99sinb]={
0.003668,9.128e-06,3.836e-07,5.04e-09
};

double TRACER99SIENYEL[tracer99sinb]={
4.76e-05,7.84e-07,1.204e-07,4.2e-09
};

double TRACER99SIENYEH[tracer99sinb]={
4.76e-05,7.84e-07,1.624e-07,1.148e-08
};

TGraphAsymmErrors*  SIFLUXETRACER99= new TGraphAsymmErrors(tracer99sinb,TRACER99SIENX,TRACER99SIENY,TRACER99SIENXE,TRACER99SIENXE,TRACER99SIENYEL,TRACER99SIENYEH);


//#  Silicon Flux in Ekin/N[GeV/n] TRACER03(2003/12)
const int tracersinb=8;

double TRACERSIENX[tracersinb]={
0.9,1.1,1.5,28,138,627,946,2088,
};

double TRACERSIENXE[tracersinb]={
0,0,0,0,0,0,0,0,
};

double TRACERSIENY[tracersinb]={
0.307,0.227,0.139,0.000523,7.2e-06,1.3e-07,4e-08,2.2e-09,
};

double TRACERSIENYEL[tracersinb]={
0.006,0.004,0.002,3e-06,2e-07,3e-08,8e-09,9e-10,
};

double TRACERSIENYEH[tracersinb]={
0.006,0.004,0.002,3e-06,2e-07,3e-08,8e-09,1.3e-09,
};

TGraphAsymmErrors*  SIFLUXETRACER= new TGraphAsymmErrors(tracersinb,TRACERSIENX,TRACERSIENY,TRACERSIENXE,TRACERSIENXE,TRACERSIENYEL,TRACERSIENYEH);

//#  Iron(Fe) Flux in Ekin/N[GeV/n] TRACER99
const int tracer99fenb=6;

double TRACER99FEENX[tracer99fenb]={
6.72,22.5,64.3,195,512,1240
};

double TRACER99FEENXE[tracer99fenb]={
0,0,0,0,0,0
};

double TRACER99FEENY[tracer99fenb]={
0.009229,0.0005303,3.595e-05,1.59e-06,5.768e-08,2.464e-08
};

double TRACER99FEENYEL[tracer99fenb]={
0.0001568,2.296e-05,3.248e-06,3.64e-07,4.76e-08,2.072e-08
};

double TRACER99FEENYEH[tracer99fenb]={
0.0001568,2.296e-05,3.248e-06,4.536e-07,1.322e-07,5.6e-08
};

TGraphAsymmErrors*  FEFLUXETRACER99= new TGraphAsymmErrors(tracer99fenb,TRACER99FEENX,TRACER99FEENY,TRACER99FEENXE,TRACER99FEENXE,TRACER99FEENYEL,TRACER99FEENYEH);


//#  Iron(Fe) Flux in Ekin/N[GeV/n] TRACER03(2003/12)
const int tracerfenb=11;

double TRACERFEENX[tracerfenb]={
0.9,1.1,1.5,2,16,30,60,183,566,733,1421
};

double TRACERFEENXE[tracerfenb]={
0,0,0,0,0,0,0,0,0,0,0
};

double TRACERFEENY[tracerfenb]={
0.221,0.165,0.102,0.062,0.00122,0.00028,4.7e-05,1.9e-06,1.3e-07,5.2e-08,7.4e-09
};

double TRACERFEENYEL[tracerfenb]={
0.005,0.004,0.002,0.002,2e-05,4e-06,1e-06,1e-07,4e-08,1.5e-08,2.2e-09
};

double TRACERFEENYEH[tracerfenb]={
0.005,0.004,0.002,0.002,2e-05,4e-06,1e-06,1e-07,6e-08,1.5e-08,2.2e-09
};

TGraphAsymmErrors*  FEFLUXETRACER= new TGraphAsymmErrors(tracerfenb,TRACERFEENX,TRACERFEENY,TRACERFEENXE,TRACERFEENXE,TRACERFEENYEL,TRACERFEENYEH);

//#  Iron(Fe) Flux in Ekin/N[GeV/n] TRACER06(2006/07)
const int tracer06fenb=7;

double TRACER06FEENX[tracer06fenb]={
0.9,1.1,1.5,23,60,159,1800
};

double TRACER06FEENXE[tracer06fenb]={
0,0,0,0,0,0,0
};

double TRACER06FEENY[tracer06fenb]={
0.41,0.278,0.179,0.00048,4.7e-05,3.2e-06,5e-09
};

double TRACER06FEENYEL[tracer06fenb]={
0.0223607,0.0134536,0.0211896,0.000130384,1.71172e-05,1.43178e-06,5e-09
};

double TRACER06FEENYEH[tracer06fenb]={
0.0223607,0.0134536,0.0211896,0.000130384,1.71172e-05,1.43178e-06,1.0198e-08
};

TGraphAsymmErrors*  FEFLUXETRACER06= new TGraphAsymmErrors(tracer06fenb,TRACER06FEENX,TRACER06FEENY,TRACER06FEENXE,TRACER06FEENXE,TRACER06FEENYEL,TRACER06FEENYEH);
#endif
