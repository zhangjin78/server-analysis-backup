#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>

//--
const double PI=3.14159265359;

//--nucleon mass, GeV
const double MPROTON=0.938;

//--ISM information
const double FracH=0.9;
const double FracHe=0.1;

//--no. of isotope for each nuclei
int NIsotope(const int charge)
{
	if (charge==2) return 2;		//He
	else if (charge==3) return 2;		//Li
	else if (charge==4) return 3;		//Be
	else if (charge==5) return 2;		//B
	else if (charge==6) return 3;		//C
	else if (charge==7) return 2;		//N
	else if (charge==8) return 3;		//O
	else if (charge==10) return 3;	//Ne
	else if (charge==12) return 3;	//Mg
	else if (charge==14) return 3;	//Si
	else if (charge==16) return 4;	//S
	else if (charge==26) return 3;	//Fe
	else return -1;
}

//--atomic number of each isotope
int AtomicNIsotope(const int charge, const int iISO)
{
	if (iISO<0 || iISO>=NIsotope(charge))
	{
		std::cout << " ****** AtomicNIsotope: wrong iISO=" << iISO << ", while for charge=" << charge << ", the acceptable range is [0," << NIsotope(charge)-1 << "] ****** " << std::endl;
		return -1;
	}
	
	if (NIsotope(charge)==0)
	{
		std::cout << " ****** AtomicNIsotope: charge=" << charge << " have no isotope information. ****** "  << std::endl;
		return -2;
	}
	
	int A=0;//--Li
	if (charge==3) A=6+iISO;
	//--Be
	else if (charge==4)
	{
		if (iISO==0) A=7;
		else if (iISO==1) A=9;
		else if (iISO==2) A=10;
	}
	//--B
	else if (charge==5) A=10+iISO;
	//--C
	else if (charge==6) A=12+iISO;
	//--N
	else if (charge==7) A=14+iISO;
	//--O
	else if (charge==8) A=16+iISO;
	//--Ne
	else if (charge==10) A=20+iISO;
	//--Mg
	else if (charge==12) A=24+iISO;
	//--Si
	else if (charge==14) A=28+iISO;
	//--S
	else if (charge==16)
	{
		if (iISO==0) A=32;
		else if (iISO==1) A=33;
		else if (iISO==2) A=34;
		else if (iISO==3) A=36;
	}
	//--Fe
	else if (charge==26) A=56+iISO;
	else A=0;
	
	return A;
}

//--fraction of each isotope
double IsotopeFracA(const int charge, const int A)
{
	double dFrac=0.;
	
	//--Li
	if (charge==3)
	{
		if (A==6) dFrac=0.56;
		else if (A==7) dFrac=0.44;
	}
	//--Be
	else if (charge==4)
	{
		if (A==7) dFrac=0.63;
		else if (A==9) dFrac=0.30;
		else if (A==10) dFrac=0.06;
	}
	//--B
	else if (charge==5)
	{
		if (A==10) dFrac=0.33;
		else if (A==11) dFrac=0.67;
	}
	//--C
	else if (charge==6)
	{
		if (A==12) dFrac=0.9;
		else if (A==13) dFrac=0.1;
		else if (A==14) dFrac=0.0002;
	}
	//--N
	else if (charge==7)
	{
		if (A==14) dFrac=0.54;
		else if (A==15) dFrac=0.46;
	}
	//--O
	else if (charge==8)
	{
		if (A==16) dFrac=0.944;
		else if (A==17) dFrac=0.028;
		else if (A==18) dFrac=0.027;
	}
	//--Ne
	else if (charge==10)
	{
		if (A==20) dFrac=0.547;
		else if (A==21) dFrac=0.106;
		else if (A==22) dFrac=0.347;
	}
	//--Mg
	else if (charge==12)
	{
		if (A==24) dFrac=0.648;
		else if (A==25) dFrac=0.175;
		else if (A==26) dFrac=0.177;
	}
	//--Si
	else if (charge==14)
	{
		if (A==28) dFrac=0.887;
		else if (A==29) dFrac=0.059;
		else if (A==30) dFrac=0.054;
	}
	//--S
	else if (charge==16)
	{
		if (A==32) dFrac=0.699;
		else if (A==33) dFrac=0.145;
		else if (A==34) dFrac=0.156;
		else if (A==36) dFrac=0.;
	}
	//--Fe
	else if (charge==26)
	{
		if (A==56) dFrac=0.931;
		else if (A==57) dFrac=0;
		else if (A==58) dFrac=0.0145;
	}
	else dFrac=0;
	
	return dFrac;
}

double IsotopeFrac(const int charge, const int iISO)
{
	return IsotopeFracA(charge, AtomicNIsotope(charge, iISO));
}

//--average over isotope
double AverOverIso(vector<double> vdValue, const int charge)
{
	if ((int)vdValue.size()!=NIsotope(charge))
	{
		std::cout << " ****** AverOverIso: vdValue size (" << vdValue.size() << ") not match with the no. isotope of charge=" << charge << " (" << NIsotope(charge) << ") ****** " << std::endl;
		return -1;
	}
	
	if (NIsotope(charge)==0)
	{
		std::cout << " ****** AverOverIso: charge=" << charge << " have no isotope information. ****** "  << std::endl;
		return -2;
	}
	
	double dAverage=0.;
	
	for (int iISO=0; iISO<NIsotope(charge); iISO++) dAverage += vdValue.at(iISO)*IsotopeFracA(charge, AtomicNIsotope(charge, iISO));
	
	return dAverage;
}

//--check whether ghost channel exist
bool CheckGhostCh(const int Z, const int A, int &ZGhost, int &AGhost, double &dBranchRatio)
{
	bool bGhostCh=false;
	//--6Li, ghost channel: 6He
	if (Z==3 && A==6)
	{
		ZGhost=2, AGhost=6, dBranchRatio=1.;
		bGhostCh=true;
	}
	//10B, ghost channel: 10C
	else if (Z==5 && A==10)
	{
		ZGhost=6, AGhost=10, dBranchRatio=1.;
		bGhostCh=true;
	}
	//11B, ghost channel: 11C
	else if (Z==5 && A==11)
	{
		ZGhost=6, AGhost=11, dBranchRatio=1.;
		bGhostCh=true;
	}
	
	return bGhostCh;
}
#endif //DATABASE_H
