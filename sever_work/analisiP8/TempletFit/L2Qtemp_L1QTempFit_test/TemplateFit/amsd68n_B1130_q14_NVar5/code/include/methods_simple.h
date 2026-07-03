#ifndef METHODS_SIMPLE_H
#define METHODS_SIMPLE_H
//----------
//updated from: methods3.h
//updated date: 2021.03.23
//keeping simplest functions
//----------
//----------
//2021.11.15
#include "parameter.h"
#include "database.h"

//------declaration
//------calculate correct abscissa for given rigidity bin
double CalAbs(const double lbin, const double ubin, const double index=-2.7);

//2021.04.01
int GetISOMass(const int charge, const int iISO);
//2021.11.15
string GetISOName(const int charge, const int iISO);

//--variable transform //2021.11.15
double CalGamma(const double dBeta);
double CalMass(const int iZ, const double dBeta, const double dRig);
double BetaToRig(const int iZ, const int iMass, const double dBeta);
double RigToBeta(const int iZ, const int iMass, const double dRig);
double EknToBeta(const double dEkn);
double EknToRig(const int iZ, const int iMass, const double dEkn) {return BetaToRig(iZ, iMass, EknToBeta(dEkn));} //2021.11.22

//------definition
//------calculate correct abscissa for given rigidity bin
double CalAbs(const double lbin, const double ubin, const double index)
{
	double Abs=0.;
	//2022.07.25
	if (index==0) //take the bin center
	{
		Abs = (lbin+ubin)/2.;
	}
	else if (index==-1)
	{
		Abs=(ubin-lbin)/log(ubin/lbin); //x^-1 need to treat differently
	}
	else
	{
		Abs=(pow(ubin, index+1)-pow(lbin, index+1))/(index+1.)/(ubin-lbin);
		Abs=pow(Abs, 1/index);
	}
	return Abs;
}

//double CalAbs(const double lbin, const double ubin, const double index)
//{
//	double Abs=0.;
//	Abs=(pow(ubin, index+1)-pow(lbin, index+1))/(index+1.)/(ubin-lbin);
//	Abs=pow(Abs, 1/index);
//	return Abs;
//}

//2021.04.01
//----isotope
int GetISOMass(const int charge, const int iISO)
{
	if (iISO<0)
	{
		cout << " ****** wrong iISO=" << iISO << ", should be >=0 ****** " << endl;
		return -1;
	}
	
	int iMass=0;
	//int NISO=(charge==4)?3:2;
	int NISO=(charge==4 || charge==6 || charge==8)?3:2; //2021.10.26: consider the C and O isotope
	if (iISO>NISO-1)
	{
		cout << " ****** iISO=" << iISO << ", while charge " << charge << " only have " << NISO << " isotopes ****** " << endl;
		return -1;
	}
	if (charge==4)
	{
		if (iISO==0) iMass = 7;
		else if (iISO==1) iMass = 9;
		else  iMass = 10;
	}
	else if (charge==3 || charge==5 || charge==7) iMass = 2*charge+iISO;
	//2021.10.26
	else if (charge==6 || charge==8)
	{
		iMass = 2*charge+iISO;
	}
	else
	{
		cout << "charge " << charge << " isotope is not considered, return 0" << endl;
	}
	
	return iMass;
}

//2021.11.15
string GetISOName(const int charge, const int iISO)
{
	string sISOName="";
	
	int iMass=GetISOMass(charge, iISO);
	if (iMass<=0)
	{
		cout << " ****** wrong iISO=" << iISO << ", should be >=0 ****** " << endl;
		return sISOName;
	}
	
	sISOName = Form("%d%s", iMass, sElementShort[charge-1].c_str());
	return sISOName;
}

//-- //2022.07.28
string IntSeparator(const int iNumber, const int nDigitPerSeparator=3, const string sSeperator="'")
{
	string sEvCount;
	int nDigitGroup=(int)(log(iNumber)/log(10.)/3.);
	int iNumber0=iNumber;
	for (int igroup=nDigitGroup; igroup>=0; igroup--)
	{
		int iDigit=(int)iNumber0/pow(10,igroup*3);
		iNumber0-=iDigit*pow(10,igroup*3);
		//sEvCount = Form("%s%d", sEvCount.c_str(), iDigit);
		//2020.03.26
		if (igroup==nDigitGroup) sEvCount = Form("%d", iDigit);
		else sEvCount = Form("%s%03d", sEvCount.c_str(), iDigit);
		if (igroup>0) sEvCount = sEvCount+sSeperator;
	} //igroup
	return sEvCount;
}

double CalPowError(double dValue, double dError, double dPower)
{
	//--currently preserve the same relative error, will update to correct formula later //TODO
	return dError/dValue*pow(dValue, dPower);
}

//--variable transform //2021.11.15
double CalGamma(const double dBeta)
{
	return sqrt(1./(1.-pow(dBeta,2)));
}

double CalMass(const int iZ, const double dBeta, const double dRig)
{
	//return iZ*dRig/(dBeta*CalGamma(dBeta));
	return iZ*dRig/(dBeta*CalGamma(dBeta))/MPROTON;
}

double BetaToRig(const int iZ, const int iMass, const double dBeta)
{
	//return (iMass*dBeta*CalGamma(dBeta))/iZ;
	return (iMass*MPROTON*dBeta*CalGamma(dBeta))/iZ;
}

double RigToBeta(const int iZ, const int iMass, const double dRig)
{
	//double dZtoM = (double)iZ/(double)iMass;
	double dZtoM = (double)iZ/(iMass*MPROTON);
	return dZtoM*sqrt(1./(pow(1./dRig,2)+pow(dZtoM,2)));
}

double EknToBeta(const double dEkn)
{
	return sqrt(pow(dEkn,2)+2*dEkn*MPROTON)/(dEkn+MPROTON);
}

//--pair comparison, simplified from TrRecon //2023.01.27
static bool CompSignal(const pair<double, int> &a,const pair<double, int> &b){return a.first>b.first;}
#endif //METHODS_SIMPLE_H
