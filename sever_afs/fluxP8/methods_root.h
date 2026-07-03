#ifndef METHODS_ROOT_H
#define METHODS_ROOT_H
//----------
//create date: 2021.03.23
//functions using root classes
//----------
//----------
#include <TROOT.h>
#include <TH1.h>
#include <TGraphErrors.h>

//------declaration
TGraphErrors *geFromHistCut(TH1 &h1, const string sName="GE", const double dRangeL=-1, const double dRangeU=-1, const bool bPosY=true, const double dScale=0., const double dIndex=-2.7, TH1F *h1RError=NULL); //build TGraphErrors from TH1, able to cut away points above or below certain range
TGraphErrors *geFromHist(TH1 &h1, const string sName="GE", const double dScale=0., const double dIndex=-2.7, TH1F *h1RError=NULL);

//------definition
TGraphErrors *geFromHistCut(TH1 &h1, const string sName, const double dRangeL, const double dRangeU, const bool bPosY, const double dScale, const double dIndex, TH1F *h1RError)
{
	TGraphErrors *ge = new TGraphErrors();
	ge->SetNameTitle(sName.c_str(), sName.c_str());
	for (int ibin=1; ibin<h1.GetNbinsX()+1; ibin++)
	{
		double dX = CalAbs(h1.GetBinLowEdge(ibin), h1.GetBinLowEdge(ibin+1), dIndex);
		if (dRangeL>0 && dX<dRangeL) continue;
		if (dRangeU>0 && dX>dRangeU) continue;
		
		double dY = h1.GetBinContent(ibin), dEY = h1.GetBinError(ibin);
		if (bPosY && dY<=0) continue;
		//--remove the errors from given histogram which stored the relateive errors
		if (h1RError!=NULL)
		{
			double dRError = h1RError->GetBinContent(h1RError->FindBin(dX));
			dEY *= sqrt(1-pow(dRError,2.));
		}
		ge->SetPoint(ge->GetN(), dX, dY*pow(dX,dScale));
		ge->SetPointError(ge->GetN()-1, 0, dEY*pow(dX,dScale));
	} //ibin
	return ge;
}

TGraphErrors *geFromHist(TH1 &h1, const string sName, const double dScale, const double dIndex, TH1F *h1RError)
{
	return geFromHistCut(h1, sName, -1, -1, true, dScale, dIndex, h1RError);
}
#endif //METHODS_ROOT_H
