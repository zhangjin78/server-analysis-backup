//  $Id: Balloon.h,v 1.1 2020/04/26 09:29:26 qyan Exp $
//////////////////////////////////////////////////////////////////////////
///
///\file  BalloonFlux
///\data  2016/06/28 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _BalloonFlux_
#define _BalloonFlux_
#include "TGraphErrors.h"
/// TGraphErrors*  SIFLUXEBalloon87   : Balloon87 silicon flux VS Ekin
/// TGraphErrors*  FEFLUXEBalloon87   : Balloon87 iron(Fe) flux VS Ekin

//#  Silicon Flux in Ekin/N[GeV/n] Balloon(1987/05+1988/05+1989/05+1991/05)
const int balloon87sinb=20;

double Balloon87SIENX[balloon87sinb]={
4.47,5.62,7.08,8.91,11.2,14.2,17.8,22.4,28.2,35.5,44.7,56.2,70.8,89.1,112,142,178,251,398,708
};

double Balloon87SIENXE[balloon87sinb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

double Balloon87SIENY[balloon87sinb]={
0.0214,0.0153,0.0093,0.00797,0.00426,0.00271,0.00158,0.000846,0.000508,0.000235,0.000107,7.26e-05,3.94e-05,1.84e-05,8.99e-06,5.87e-06,2.45e-06,5.89e-07,2.22e-07,2.7e-08
};

double Balloon87SIENYE[balloon87sinb]={
0.00332,0.00213,0.00128,0.000928,0.000569,0.000358,0.000223,0.000139,9.27e-05,2.94e-05,1.65e-05,1.14e-05,6.41e-06,3.74e-06,2.25e-06,1.64e-06,9.18e-07,2.59e-07,1.27e-07,2.7e-08
};

TGraphErrors*  SIFLUXEBalloon87= new TGraphErrors(balloon87sinb,Balloon87SIENX,Balloon87SIENY,Balloon87SIENXE,Balloon87SIENYE);


//#  Iron(Fe) Flux in Ekin/N[GeV/n] Balloon(1987/05+1988/05+1989/05+1991/05)
const int balloon87fenb=21;

double Balloon87FEENX[balloon87fenb]={
4.47,5.62,7.08,8.91,11.2,14.2,17.8,22.4,28.2,35.5,44.7,56.2,70.8,89.1,112,142,178,251,398,631,1120
};

double Balloon87FEENXE[balloon87fenb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

double Balloon87FEENY[balloon87fenb]={
0.0263,0.016,0.0109,0.00599,0.0032,0.00182,0.00112,0.000648,0.000299,0.000216,0.000126,5.74e-05,3.68e-05,2.2e-05,1.03e-05,5.62e-06,2.47e-06,1.37e-06,3.86e-07,1.39e-07,2.58e-08
};

double Balloon87FEENYE[balloon87fenb]={
0.00356,0.00213,0.00135,0.000802,0.000474,0.000292,0.000194,0.000126,7.6e-05,5.47e-05,3.6e-05,2.15e-05,6.88e-06,4.67e-06,2.85e-06,1.89e-06,1.11e-06,4.79e-07,1.93e-07,9.81e-08,2.58e-08
};

TGraphErrors*  FEFLUXEBalloon87= new TGraphErrors(balloon87fenb,Balloon87FEENX,Balloon87FEENY,Balloon87FEENXE,Balloon87FEENYE);

#endif
