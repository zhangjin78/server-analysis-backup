//  $Id: ATICFlux.h,v 1.8 2020/04/26 09:29:26 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  ATICFlux
///\data  2014/09/30 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _ATICFlux_
#define _ATICFlux_
#include "TGraphErrors.h"
/// TGraphErrors*  PRFLUXEATIC02  : ATIC02 proton flux VS Ekin
/// TGraphErrors*  HEFLUXEATIC02  : ATIC02 helium flux VS Ekin
/// TGraphErrors*  CFLUXEATIC02   : ATIC02 carbon flux VS Ekin 
/// TGraphErrors*  BCRATIOEATIC02 : ATIC02 boron/carbon  ratio VS Ekin 
/// TGraphErrors*  OFLUXEATIC02   : ATIC02 oxygen flux VS Ekin 
/// TGraphErrors*  CORATIOEATIC02 : ATIC02 carbon/oxygen ratio VS Ekin
/// TGraphErrors*  SIFLUXEATIC02  : ATIC02 silicon flux VS Ekin
/// TGraphErrors*  FEFLUXEATIC02  : ATIC02 iron(Fe) flux VS Ekin

//#   proton Flux in Ekin[GeV] ATIC02(2003/01)
const int atic02prnb=15;

double ATIC02PRENX[atic02prnb]={
48.262,76.962,122.062,195.062,309.062,491.062,778.062,1229.06,1959.06,3099.06,4919.06,7789.06,12299.1,19599.1,30999.1
};

double ATIC02PRENXE[atic02prnb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

double ATIC02PRENY[atic02prnb]={
0.321,0.089,0.025,0.00707,0.00201,0.000595,0.000178,5.24e-05,1.58e-05,4.83e-06,1.4e-06,4.23e-07,1.37e-07,3.89e-08,9.16e-09
};

double ATIC02PRENYE[atic02prnb]={
0.003,0.0005,0.0001,5e-05,2e-05,7e-06,3e-06,1.4e-06,5e-07,2.3e-07,1e-07,3.9e-08,1.5e-08,7e-09,2.48e-09
};

TGraphErrors*  PRFLUXEATIC02= new TGraphErrors(atic02prnb,ATIC02PRENX,ATIC02PRENY,ATIC02PRENXE,ATIC02PRENYE);


//#   He Flux in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02henb=15;

double ATIC02HEENX[atic02henb]={
11.3679,18.5429,29.8179,48.0679,76.5679,122.068,193.818,306.568,489.068,774.068,1229.07,1946.57,3074.07,4899.07,7749.07
};

double ATIC02HEENXE[atic02henb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

double ATIC02HEENY[atic02henb]={
0.692,0.2056,0.0608,0.0184,0.00564,0.001736,0.000528,0.0001556,4.8e-05,1.568e-05,5e-06,1.656e-06,5.4e-07,1.312e-07,3.624e-08
};

double ATIC02HEENYE[atic02henb]={
0.008,0.0012,0.0004,0.00012,4e-05,2e-05,8e-06,4e-07,1.6e-06,6.4e-07,3.6e-07,1.28e-07,6.4e-08,2.44e-08,1.036e-08
};

TGraphErrors*  HEFLUXEATIC02= new TGraphErrors(atic02henb,ATIC02HEENX,ATIC02HEENY,ATIC02HEENXE,ATIC02HEENYE);


//#  Carbon Flux in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02cnb=8;

double ATIC02CENX[atic02cnb]={
18.3185,35.4852,69.8185,140.735,290.735,601.568,1232.4,2515.74
};

double ATIC02CENXE[atic02cnb]={
0,0,0,0,0,0,0,0
};

double ATIC02CENY[atic02cnb]={
0.005808,0.0011484,0.0001932,3.408e-05,4.98e-06,6.804e-07,1.476e-07,4.668e-08
};

double ATIC02CENYE[atic02cnb]={
0.00012,2.76e-05,7.2e-06,2.04e-06,5.4e-07,1.392e-07,4.2e-08,1.68e-08
};

TGraphErrors*  CFLUXEATIC02= new TGraphErrors(atic02cnb,ATIC02CENX,ATIC02CENY,ATIC02CENXE,ATIC02CENYE);


//#  Boron/Carbon ratio in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02bcnb=5;

double ATIC02BCENX[atic02bcnb]={
19.9,38.3,74.3,149,307
};

double ATIC02BCENXE[atic02bcnb]={
0,0,0,0,0
};

double ATIC02BCENY[atic02bcnb]={
0.18,0.169,0.119,0.156,0.064
};

double ATIC02BCENYE[atic02bcnb]={
0.011,0.015,0.029,0.053,0.063
};

TGraphErrors*  BCRATIOEATIC02= new TGraphErrors(atic02bcnb,ATIC02BCENX,ATIC02BCENY,ATIC02BCENXE,ATIC02BCENYE);


//#  Oxygen Flux in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02onb=8;

double ATIC02OENX[atic02onb]={
14.1313,27.0688,52.9438,105.944,217.194,449.694,930.319,1905.32,
};

double ATIC02OENXE[atic02onb]={
0,0,0,0,0,0,0,0
};

double ATIC02OENY[atic02onb]={
0.011296,0.002336,0.0004224,7.424e-05,9.264e-06,1.5904e-06,2.96e-07,2.976e-08
};

double ATIC02OENYE[atic02onb]={
0.000144,4.8e-05,1.12e-05,3.2e-06,8.16e-07,2.272e-07,6.72e-08,1.616e-08
};

TGraphErrors*  OFLUXEATIC02= new TGraphErrors(atic02onb,ATIC02OENX,ATIC02OENY,ATIC02OENXE,ATIC02OENYE);


//#  Carbon/Oxygen ratio in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02conb=5;

double ATIC02COENX[atic02conb]={
19.9,38.3,74.3,149,307
};

double ATIC02COENXE[atic02conb]={
0,0,0,0,0
};

double ATIC02COENY[atic02conb]={
1.02,1.087,0.933,0.934,1.022
};

double ATIC02COENYE[atic02conb]={
0.026,0.043,0.06,0.105,0.227
};

TGraphErrors*  CORATIOEATIC02= new TGraphErrors(atic02conb,ATIC02COENX,ATIC02COENY,ATIC02COENXE,ATIC02COENYE);

//#  Silicon Flux in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02sinb=7;

double ATIC02SIENX[atic02sinb]={
8.49785,16.3907,31.6764,62.2836,124.426,255.141,527.641,
};

double ATIC02SIENXE[atic02sinb]={
0,0,0,0,0,0,0,
};

double ATIC02SIENY[atic02sinb]={
0.006272,0.0015988,0.0003248,5.824e-05,8.54e-06,1.456e-06,2.4864e-07,
};

double ATIC02SIENYE[atic02sinb]={
0.00014,4.76e-05,1.68e-05,4.48e-06,1.176e-06,3.472e-07,9.8e-08,
};

TGraphErrors*  SIFLUXEATIC02= new TGraphErrors(atic02sinb,ATIC02SIENX,ATIC02SIENY,ATIC02SIENXE,ATIC02SIENYE);

//#  Iron(Fe) Flux in Ekin/N[GeV/n] ATIC02(2003/01)
const int atic02fenb=8;

double ATIC02FEENX[atic02fenb]={
3.89102,8.69459,18.1767,37.2839,75.4982,151.57,302.641,606.212
};

double ATIC02FEENXE[atic02fenb]={
0,0,0,0,0,0,0,0
};

double ATIC02FEENY[atic02fenb]={
0.0168,0.0045584,0.0009688,0.0001764,2.9288e-05,3.472e-06,4.9056e-07,1.344e-07
};

double ATIC02FEENYE[atic02fenb]={
0.000224,7.84e-05,2.8e-05,7.84e-06,2.296e-06,5.544e-07,1.4784e-07,5.4768e-07
};

TGraphErrors*  FEFLUXEATIC02= new TGraphErrors(atic02fenb,ATIC02FEENX,ATIC02FEENY,ATIC02FEENXE,ATIC02FEENYE);
#endif

