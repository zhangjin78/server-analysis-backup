#ifndef FILE_IO_H
#define FILE_IO_H
//----------
//create date: 2019.01.17
//----------
//functions to write (read) vectors to (from) files
//----------
//2019.01.17
//	1) define functions OutVector & InVector to write and read arrays
//
//2019.02.13
//	1) define functions to create TGraph from the output of R spline fit
//
//2019.02.20
//	1) define functions to store the parameter of given TF1
//
//2019.03.22
//	1) define OutPar2 & InPar2 which takes TF1 & FILE as parameter
//----------
//#include "include/drawvariable.h"
#include <vector>
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
vector<double>::iterator itd;

typedef double (*CFunc)(double*, double*);

//--output
int OutArray(const int size, const double *dArray, FILE *fOut)
{
	if (size==0) {
		cout << "***empty vector, stop writting***" << endl;
		return -1;
	}
	
	//--write the size of vector
	fprintf(fOut, "%d\n", size);
	
	//--write the element of vector
	for (int i=0; i<size; i++) {
		fprintf(fOut,"%15e", dArray[i]);
//		fprintf(fOut,"%15lf", dArray[i]);
	}
	fprintf(fOut, "\n");
	return size;
}

int OutVector(vector<double> vdOut, FILE *fOut)
{
	int size=vdOut.size();
	if (size==0) {
		cout << "***empty vector, stop writting***" << endl;
		return -1;
	}
	
	//--write the size of vector
	fprintf(fOut, "%d\n", size);
	
	//--write the element of vector
	for (itd=vdOut.begin(); itd!=vdOut.end(); itd++) {
		fprintf(fOut,"%15e", *itd);
//		fprintf(fOut,"%15lf", *itd);
	}
	fprintf(fOut, "\n");
	return size;
}

int OutPar(TF1 &f1, const string sOut)
{
	int npar=f1.GetNpar();
	if (npar==0) {
		cout << "***TF1 have no parameter, stop writting***" << endl;
		return -1;
	}
	FILE *fOut = fopen(sOut.c_str(),"w");
	
	//--write the number of parameters
	fprintf(fOut, "%d\n", npar);
	
	//--write the element of vector
	for (int ipar=0; ipar<npar; ipar++) fprintf(fOut,"%15e", f1.GetParameter(ipar));
	fprintf(fOut, "\n");
	
	fclose(fOut);
	return npar;
}

int OutPar2(TF1 &f1, FILE *fOut)
{
	int npar=f1.GetNpar();
	if (npar==0) {
		cout << "***TF1 have no parameter, stop writting***" << endl;
		return -1;
	}
//	FILE *fOut = fopen(sOut.c_str(),"w");
	
	//--write the number of parameters
	fprintf(fOut, "%d\n", npar);
	
	//--write the element of vector
	for (int ipar=0; ipar<npar; ipar++) fprintf(fOut,"%15e", f1.GetParameter(ipar));
	fprintf(fOut, "\n");
	
//	fclose(fOut);
	return npar;
}

//--input
int InVector(vector<double> &vdOut, FILE *fIn)
{
	int size;
	double element;
	int stat = fscanf(fIn,"%d\n",&size);
	if (size==0) {
		cout << "***empty vector, stop reading***" << endl;
		return -1;
	}
	
	//--read the element of vector
	for (int i=0; i<size; i++) {
		stat = fscanf(fIn,"%lf",&element);
		vdOut.push_back(element);
	}
	return size;
}

TGraph* InGraph(FILE *fIn, int opt=2)
{
	int size;
	double element;
//	int stat = fscanf(fIn,"%d\n",&size);
//	if (size==0) {
//		cout << "***empty vector, stop reading***" << endl;
//		return NULL;
//	}
//	cout << "size=" << size << endl;
	
	TGraph *gIn = new TGraph();
	
	//--read the element of vector
	double logR=0, f=0;
//	for (int i=0; i<size; i++) {
//		stat = fscanf(fIn,"%lf%lf",&logR, &f);
//		gIn->SetPoint(gIn->GetN(), exp(logR), f);
//	}
	
	int i=0;
	while (fscanf(fIn,"%lf %lf\n",&logR, &f)==2) {
		if (opt==0) gIn->SetPoint(gIn->GetN(), pow(10, logR), f); //log10
		else if (opt==1) gIn->SetPoint(gIn->GetN(), exp(logR), f); //log
		else if (opt==2) gIn->SetPoint(gIn->GetN(), logR, f); //linear
	}
	return gIn;
}

TGraphAsymmErrors* InGraphCL(FILE *fIn)
{
//	rewind(fIn); //rewind input files first, since might read the same files many times
	TGraphAsymmErrors *gaeIn = new TGraphAsymmErrors();
	
	//--read the element of vector
	double Rig=0, f=0, lwr=0, upr=0;
	
	int i=0;
	while (fscanf(fIn,"%lf %lf %lf %lf\n",&Rig, &f, &lwr, &upr)==4) {
		gaeIn->SetPoint(gaeIn->GetN(), Rig, f); //linear
		gaeIn->SetPointError(gaeIn->GetN()-1, 0, 0, f-lwr, upr-f);
		if (i==0) {
			cout << Rig << " " << f << endl;
			i++;
		}
	}
	return gaeIn;
}

TGraphAsymmErrors* InGraphCL(const string sFile)
{
	FILE *fIn=fopen(sFile.c_str(), "r");
	TGraphAsymmErrors *gaeIn = InGraphCL(fIn);
	fclose(fIn);
	return gaeIn;
}

TF1* InPar(CFunc cf, const string sFile)
{
	FILE *fIn = fopen(sFile.c_str(),"r");
	
	int npar=0;
	int stat = fscanf(fIn,"%d\n",&npar);
	if (npar==0) {
		cout << "***NPar=0, stop reading***" << endl;
		return NULL;
	}
	
	TF1 *f1 = new TF1("", cf, 0.1, 10000., npar);
	double element;
	//--read the element of vector
	for (int ipar=0; ipar<npar; ipar++) {
		stat = fscanf(fIn,"%lf",&element);
		f1->SetParameter(ipar, element);
	}
	
	fclose(fIn);
	return f1;
}

TF1* InPar2(TF1 *f1, FILE *fIn)
{
//	FILE *fIn = fopen(sFile.c_str(),"r");
	
	int npar=0;
	int stat = fscanf(fIn,"%d\n",&npar);
	if (npar==0) {
		cout << "***NPar=0, stop reading***" << endl;
		return NULL;
	}
	
//	TF1 *f1 = new TF1("", cf, 0.1, 10000., npar);
	double element;
	//--read the element of vector
	for (int ipar=0; ipar<npar; ipar++) {
		stat = fscanf(fIn,"%lf",&element);
		f1->SetParameter(ipar, element);
	}
	
//	fclose(fIn);
	return f1;
}

//2019.06.19
TF1* InPar3(TF1 *f1, const string sFile)
{
	FILE *fIn = fopen(sFile.c_str(),"r");
	
	int npar=0;
	int stat = fscanf(fIn,"%d\n",&npar);
	if (npar==0) {
		cout << "***NPar=0, stop reading***" << endl;
		return NULL;
	}
	
	//TF1 *f1 = new TF1("", cf, 0.1, 10000., npar);
	double element;
	//--read the element of vector
	for (int ipar=0; ipar<npar; ipar++)
	{
		stat = fscanf(fIn,"%lf",&element);
		f1->SetParameter(ipar, element);
	}
	
	fclose(fIn);
	return f1;
}
#endif //FILE_IO_H
