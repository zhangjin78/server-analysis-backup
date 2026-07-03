#ifndef BINS_H
#define BINS_H
//-----------------
//-----------------
#include "binning.h"

//int iBinVersion=1;
int iBinVersion=0; //2020.01.10: default iBinVersion=0, decide binning according to charge

//--
const int NPubBin=5;
const string sPubBin[NPubBin+1] = {"", "_BeBin", "_HeBin", "_HZBin", "_FBin", "_FeBin"};
const int iQPubBin[NPubBin] = {3, 8, 14, 9, 26};

//for Acceptance in Generated Rigidity, use Even log bin //will be used other than Acceptance, put into bin.h //2023.02.16
int iNbinGenRig=0;
double *pBinsGenRig=NULL;
//Even log bin for Generated Rig, up to 8 TV
int iNbinGenRigGeom=0;
double *pBinsGenRigGeom=NULL;
//Even log bin for Ekn
int iNbinGenEkn=0;
double *pBinsGenEkn=NULL;

vector<double> vdELB;
vector<double> vdELBGeom;
vector<double> vdELBEkn; //2023.02.28

//--
//use opt to control whether always using certain binning
//opt=0: binning according to charge
//   =1: He, C, O binning
//   =2: Ne, Mg, Si binning
int getNbin(int q, int opt=iBinVersion);
double* getBins(int q, int opt=iBinVersion);

//--binning for spectral index
int getNbinSI(int q);
double* getBinsSI(int q);

//--function to set even log bin
int EvenLogBin(const int charge);
int EvenLogBinGeom();

//--function to set even log bin in Ekn
int EvenLogBinEkn();

//------
int getNbin(int q, int opt)
{
	if (opt==0)
	{
		/*if (q==1 || q==2 || (q>=6 && q<=8)) return nuclei_nbin;
		//else if (q>=10) return nuclei_nbinBZ;
		else if (q>=9) return nuclei_nbinBZ; //2020.02.04*/
		//2021.03.18
		if (q==1 || q==2 || q==6 || q==8) return nuclei_nbin;
		else if ((q>=3 && q<=5) || q==7) return nuclei_nbin2;
		else if (q>=9) return nuclei_nbinBZ;
	}
	else if (opt==1) return nuclei_nbin;
	else if (opt==2) return nuclei_nbinBZ;
	return 0;
}

double* getBins(int q, int opt)
{
	if (opt==0)
	{
		/*if (q==1 || q==2 || (q>=6 && q<=8)) return nuclei_binsP;
		//else if (q>=10) return nuclei_binsBZ;
		else if (q>=9) return nuclei_binsBZ; //2020.02.04*/
		//2021.03.18
		if (q==1 || q==2 || q==6 || q==8) return nuclei_binsP;
		else if ((q>=3 && q<=5) || q==7) return nuclei_bins2;
		else if (q>=9) return nuclei_binsBZ;
	}
	else if (opt==1) return nuclei_binsP;
	else if (opt==2) return nuclei_binsBZ;
	return NULL;
}

//------binning in the publication //2021.05.04
int getNbinPub(int q)
{
	if (q==1 || q==2 || q==6 || q==8) return nuclei_nbin;
	else if ((q>=3 && q<=5) || q==7) return nuclei_nbin2;
	else if (q==10 || q==12 || q==14) return nuclei_nbinBZ;
	else if (q==9 || q==11 || q==13) return nuclei_nbinFNaAlPub; //2022.06.29
	else if (q==26) return nuclei_nbinFePub;
	return 0;
}

double* getBinsPub(int q)
{
	if (q==1 || q==2 || q==6 || q==8) return nuclei_binsP;
	else if ((q>=3 && q<=5) || q==7) return nuclei_bins2;
	else if (q==10 || q==12 || q==14) return nuclei_binsBZ;
	else if (q==9 || q==11 || q==13) return nuclei_binsFNaAlPub; //2022.06.29
	else if (q==26) return nuclei_binsFePub;
	return NULL;
}

int getNbin2(int q)
{
	if (q==1 || q==2 || (q>=6 && q<=8)) return nuclei_nbin3;
	else return nuclei_nbin4;
}

double* getBins2(int q)
{
	if (q==1 || q==2 || (q>=6 && q<=8)) return nuclei_bins3;
	else return nuclei_bins4;
}

//------binning for spectral index
int getNbinSI(int q)
{
	if (q==1 || q==2 || q==6 || q==8) return nbinSI;
	else if (q==7) return nbinSIN;
	else if (q>=9 && q<=14)
	{
		if (q%2==0) return nbinSIHZ;
		else if (q==9) return nbinSIF;
		else if (q==11 || q==13) return nbinSINaAl;
	}
	else if (q==26) return nbinSIHZ;
	else if (q<8)
	{
		//if (iBinVersion==2) return nbinSIHZ;
		if (iBinVersion==2) return nbinSILiBeB;
	}
	
	cout << "No specify the spectrum index binning for Q=" << q << ", return 0" << endl;
	return 0;
}

double* getBinsSI(int q)
{
	if (q==1 || q==2 || q==6 || q==8) return binsSI;
	else if (q==7) return binsSIN;
	else if (q>=9 && q<=14)
	{
		if (q%2==0) return binsSIHZ;
		else if (q==9) return binsSIF;
		else if (q==11 || q==13) return binsSINaAl;
	}
	else if (q==26) return binsSIHZ;
	else if (q<8)
	{
		//if (iBinVersion==2) return binsSIHZ;
		if (iBinVersion==2) return binsSILiBeB;
	}
	
	cout << "No specify the spectrum index binning for Q=" << q << ", return 0" << endl;
	return NULL;
}

//--function to set even log bin
int EvenLogBin(const int charge)
{
	cout << "****** Even Log Bin for Acceptance in Gen-Rig******" << endl;
	//more bins at low rigidity
	//L1Inner: [1, .2000.]GV, Full Span: [1.,last bin] GV
	//separete into 4 range: [1,2) [2,3), [3,2000) [2000,~3000)
	double dRigRange[4+1]={1.,2.,3.,2000.,getNbin(charge,1)[getBins(charge,1)]};
	int nBinELB[4]={10,10,70,5};
	vdELB.clear();
	vdELB.push_back(dRigRange[0]);
	for (int iRange=0; iRange<4; iRange++)
	{
		iNbinGenRig+=nBinELB[iRange];
		for (int ibin=1; ibin<nBinELB[iRange]+1; ibin++)
		{
			double dBin=dRigRange[iRange]*pow(dRigRange[iRange+1]/dRigRange[iRange], (double)ibin/nBinELB[iRange]);
			vdELB.push_back(dBin);
		} //ibin
	} //iRange
	//iNbinGenRig=nBinELB[0]+nBinELB[1]+nBinELB[2]+nBinELB[3];
	pBinsGenRig=&vdELB[0];
	cout << "Even log bin in rig: " << iNbinGenRig << " " << pBinsGenRig << " " << vdELB.size() << endl;
	return iNbinGenRig;
}

int EvenLogBinGeom()
{
	cout << "****** Even Log Bin for Acceptance in Gen-Rig, up to 8 TV ******" << endl;
	//more bins at low rigidity
	//L1Inner: [1, .2000.]GV, Full Span: [1.,last bin] GV
	//separete into 4 range: [1,2) [2,3), [3,2000) [2000,8000)
	double dRigRange[4+1]={1.,2.,3.,2000.,8000.};
	int nBinELB[4]={10,10,70,30};
	vdELBGeom.clear();
	vdELBGeom.push_back(dRigRange[0]);
	for (int iRange=0; iRange<4; iRange++)
	{
		iNbinGenRigGeom+=nBinELB[iRange];
		for (int ibin=1; ibin<nBinELB[iRange]+1; ibin++)
		{
			double dBin=dRigRange[iRange]*pow(dRigRange[iRange+1]/dRigRange[iRange], (double)ibin/nBinELB[iRange]);
			vdELBGeom.push_back(dBin);
		} //ibin
	} //iRange
	pBinsGenRigGeom=&vdELBGeom[0];
	cout << "Even log bin in rig: " << iNbinGenRigGeom << " " << pBinsGenRigGeom << " " << vdELBGeom.size() << endl;
	return iNbinGenRigGeom;
}

//--function to set even log bin in Ekn
int EvenLogBinEkn()
{
	cout << "****** Even Log Bin in Gen-Ekn******" << endl;
	//more bins at low rigidity
	//const int nEknRange=1;
	//double dEknRange[nEknRange+1]={0.1, 12.};
	//int nBinELBEkn[nEknRange]={50};
	//2021.09.17
	const int nEknRange=4;
	double dEknRange[nEknRange+1]={0.15, 0.5, 1.2, 5, 12.};
	int nBinELBEkn[nEknRange]={20, 20, 20, 30};
	
	vdELBEkn.push_back(dEknRange[0]);
	iNbinGenEkn=0;
	for (int iRange=0; iRange<nEknRange; iRange++)
	{
		iNbinGenEkn+=nBinELBEkn[iRange];
		for (int ibin=1; ibin<nBinELBEkn[iRange]+1; ibin++)
		{
			double dBin=dEknRange[iRange]*pow(dEknRange[iRange+1]/dEknRange[iRange], (double)ibin/nBinELBEkn[iRange]);
			vdELBEkn.push_back(dBin);
		} //ibin
	} //iRange
	pBinsGenEkn=&vdELBEkn[0];
	cout << "Even log bin in Ekn: " << iNbinGenEkn << " " << pBinsGenEkn << " " << vdELBEkn.size() << endl;
	return iNbinGenEkn;
}
#endif                                  
