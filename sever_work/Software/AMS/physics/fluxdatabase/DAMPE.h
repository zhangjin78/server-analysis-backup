//  $Id: DAMPE.h,v 1.1 2019/11/12 09:43:56 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  DAMPEFlux
///\data  2019/11/12 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _DAMPEFlux_
#define _DAMPEFlux_
#include <iostream>
#include "TGraphErrors.h"
/// TGraphErrors*  PRFLUXEDAMPE   : DAMPE proton flux VS Ekin
const int dampeprnb=17;

double DAMPENX[dampeprnb]={
49.8, 78.9, 125.1, 198.3, 314.3, 498.1, 789.5, 1251, 1983, 3143, 4981, 7895, 12512, 19830, 31429, 49812, 78946,
};

double DAMPENXE[dampeprnb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

double DAMPENY[dampeprnb]={
2.97e-1,8.43e-2,2.38e-2,6.64e-3,1.89e-3,5.39e-4,1.60e-4,4.81e-5,1.45e-5,4.45e-6,1.36e-6,4.06e-7,1.20e-7,3.35e-8,9.03e-9,2.47e-9,6.50e-10,
};

double DAMPENYES[dampeprnb]={
0,      0,      0,      0,      0,      0.01e-4,0,      0.01e-5,0.01e-5,0.02e-6,0.01e-6,0.04e-7,0.02e-7,0.07e-8,0.26e-9,0.11e-9,0.40e-10,
};

double DAMPENYEA[dampeprnb]={
0.14e-1,0.40e-2,0.11e-2,0.31e-3,0.09e-3,0.25e-4,0.07e-4,0.23e-5,0.07e-5,0.21e-6,0.06e-6,0.19e-7,0.06e-7,0.17e-8,0.48e-9,0.15e-9,0.50e-10,
};

double DAMPENYEH[dampeprnb]={
0.20e-1,0.56e-2,0.16e-2,0.44e-3,0.12e-3,0.36e-4,0.11e-4,0.33e-5,0.13e-5,0.44e-6,0.13e-6,0.40e-7,0.12e-7,0.33e-8,0.89e-9,0.24e-9,0.64e-10,
};

double DAMPENYEAT[dampeprnb]={
0.014,0.004,0.0011,0.00031,9e-05,2.502e-05,7e-06,2.30217e-06,7.07107e-07,2.1095e-07,6.08276e-08,1.94165e-08,6.32456e-09,1.83848e-09,5.45894e-10,1.86011e-10,6.40312e-11,
};

double DAMPENYEHT[dampeprnb]={
0.0244131,0.00688186,0.00194165,0.000538238,0.00015,4.38406e-05,1.30384e-05,4.02368e-06,1.47986e-06,4.87955e-07,1.43527e-07,4.44635e-08,1.35647e-08,3.77757e-09,1.04408e-09,3.03645e-10,9.05318e-11,
};

//TGraphErrors*  PRFLUXEDAMPES= new TGraphErrors(dampeprnb,DAMPENX,DAMPENY,DAMPENXE,DAMPENYES);
//TGraphErrors*  PRFLUXEDAMPEA= new TGraphErrors(dampeprnb,DAMPENX,DAMPENY,DAMPENXE,DAMPENYEAT);
TGraphErrors*  PRFLUXEDAMPE= new TGraphErrors(dampeprnb,DAMPENX,DAMPENY,DAMPENXE,DAMPENYEHT);


/*void DAMPE(){
  for(int ip=0;ip<dampeprnb;ip++){
//    cout<<sqrt(pow(DAMPENYES[ip],2.)+pow(DAMPENYEA[ip],2.))<<",";
    cout<<sqrt(pow(DAMPENYES[ip],2.)+pow(DAMPENYEA[ip],2.)+pow(DAMPENYEH[ip],2))<<",";
  }
  cout<<endl;
}*/

#endif
