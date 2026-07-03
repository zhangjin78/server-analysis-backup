#ifndef SPLINEFIT3_H
#define SPLINEFIT3_H
//----------
//update from: splineFit2.h
//update date: 2019.09.09
//----------
//Class SplineFit containing C function which use TSpline3
//----------
//2019.09.09
//	1) add new member, geFit, as the TGraphErrors to be fit
//	2) add new member function, doFit, to fit the member geFit with current node position
//	2) add new member function, CalConfInt, to return the TGraph with error as confidence interval
//	4) add new member function, GetConfIntBand, to return the TGraph indicating the confidence interval band
//	5) implement the effect of verbose parameter
//	6) remove deprecated functions:
//		- cfSplineFit_order
//		- cfSplineFitAlong and BuildTF1Along
//	7) add new member function, SetNodesQuantile, to set nodes according to Quantiles
//
//2019.09.10
//	1) add new members, RangeL and RangeU, indicating the rigidity range
//	2) add new member function, SetRange, to set the rigidity range
//
//2019.09.11
//	1) add new member, gPull, as the pull of the fit
//	2) add new member function, CalPull, to calculate the pull of the f1SplineFit compare to geFit
//
//2019.09.12
//	1) remove all NULL test before delete
//	2) add new member function, SetNodesEvenLog, to set event log nodes at certatin range
//	3) add new member function, SetNodesByX, to set the nodes giving doulbe array of X node positions
//
//2019.10.07
//	1) add new function, GetCIError, to store the relative error of the fit
//
//2019.11.01
//	1) add new function, GetDeviationFromConst, to store the deviation from linear fit for efficiency as function of generated rigidity
//
//2019.11.12
//	1) add new function, CalConfIntVD, to be able to read the confidence interval depends on a given double vector
//	2) add new function, CalConfIntEvenLog, to be able to set a even logly nodes for confidence interval and call CalConfIntVD
//
//2020.01.14
//	1) add new function, cfSplineScalePowerLaw and BuildSplineScalePowerLaw, to get the spline multiply by a single power law
//
//2020.01.16
//	1) add new function, ChangeNode(int ip, double dX, double dY), to change given node
//
//2020.01.20
//	1) add new member functions, cfSplineError and BuildSplineError, which is a spline and is able to move the y node positions with or without error controled by parameter
//	2) add new member, vdYRelError, the vector for the relative error of spline
//	3) add new member function, DrawSplineError, to plot spline with and without error
//	4) add new member functions, StoreError and ReadError, to store and read spline error
//
//2020.01.27
//	1) add new member function, SplineEval, to get the function value 1) at given x, or 2) at the x calculated using power law for given bin
//
//2020.02.06
//	1) add new member functions, cfFluxExpot and BuildFluxExpot, which calculate the spline times another spline which is the parametrization of the exposure time
//
//2020.11.04
//	1) add new member function, GetFun, which is the getter function of f1SplineFit
//
//2021.11.15
//	1) add new member functions, cfSplineBeta and cfSplineEkn, which transfers the spline in rigidity into beta and Ekn
//	2) add new member functions, BuildTF1Beta and BuildTF1Ekn, which build the TF1 as function of beta and Ekn
//	3) add new members, Charge and NAtomic
//	4) add new member function, SetAZ, to set Charge and NAtomic
//	5) set only one static TSpline3, that only updates the parameter if it had been change, to avoid creating a TSpine3 everytime evaluating the function TODO XXX
//----------
#include "TSpline.h"
#include "TF1.h"
#include "file_io.h"
//#include "drawvariable.h"
#include "methods_simple.h" //2021.03.23

double kontsQuant4[4]={.05, .35, .65, .95};
double kontsQuant5[5]={.05, .275, .5, .725, .95};

class SplineFit
{
	public:
	enum FitMod
	{
		LinearXY=0x1, //y=f(X)
		LogX=0x2,// y=f(Log(X))
		LogY=0x4,// Log(Y)=f(X)
		LogX2=0x8,// y=f(Log(X))//sqrt(bl*bh)
		ExtrapolateLB=0x10,//Linear Extrapolate Begin
		ExtrapolateLE=0x20,//Linear Extrapolate End
		ExtrapolateFlux=0x40,//Y=CX^a
	};
	
	public:
	//------parameters
	int FitOpt;
	string sBoundary;
	bool verbose;
	
	int nnode;
	
	double RangeL;
	double RangeU;
	
	//2021.11.15
	int Charge;
	int NAtomic;
	
	//------member
	TF1 *f1SplineFit=NULL;
	TF1 *f1FluxIndex=NULL;
	TF1 *f1Der=NULL;
	
	TF1 *f1SplineError=NULL; //2020.01.20
	
	TSpline3 *sp3Expot=NULL; //2020.02.06
	
	//2019.09.09
	TGraphErrors *geFit=NULL; //TGraphErrors needs to be fit, not belongs to the class 
	TGraphErrors *geConfInt=NULL; //TGraphErrors of confidence interval with points the fit result, error the confidence interval, belongs to this class
	TGraph *gConfIntBand=NULL; //TGraph of confidence interval band, belongs to this class
	//2022.05.06
	TGraphErrors *geConfIntsamepos=NULL; //TGraphErrors of confidence interval with points the fit result, error the confidence interval, same position as geFit
	
	//2019.09.11
	TGraph *gPull=NULL; //TGraph of pull, belongs to this class
	
	//----spline as function of different variable
	TF1 *f1SplineEkn=NULL;
	
	TFitResultPtr frpFitResult;
	
	vector<double> vdX, vdY;
	vector<double> vdYRelError; //vector for error on Y node //2020.01.20
	
	//------constructer & destructor
	SplineFit() {verbose=false;}
	
	//SplineFit(int iNnode, string boundary="b1e1", int opt=LogX|ExtrapolateLB|ExtrapolateLE) : nnode(iNnode), sBoundary(boundary), FitOpt(opt) 
	//{
	//	verbose=false;
	//}
	//2021.11.15
	SplineFit(int iNnode, string boundary="b1e1", int opt=LogX|ExtrapolateLB|ExtrapolateLE, const int Q=1, const int A=1) : nnode(iNnode), sBoundary(boundary), FitOpt(opt), Charge(Q), NAtomic(A)
	{
		verbose=false;
	}
	
	SplineFit(string sIn, int opt=0, bool bVB=false)
	{
		verbose=bVB;
		if (verbose) cout << "Read parameters from " << sIn << endl;
		BuildFunFromFile(sIn, opt);
	}
	
	~SplineFit()
	{
		/*if (f1SplineFit!=NULL) {delete f1SplineFit; f1SplineFit=NULL;}
		if (f1FluxIndex!=NULL) {delete f1FluxIndex; f1FluxIndex=NULL;}
		if (f1Der!=NULL) {delete f1Der; f1Der=NULL;} //2019.09.10*/
		
		//2019.09.12
		//------function
		delete f1SplineFit; f1SplineFit=NULL;
		delete f1FluxIndex; f1FluxIndex=NULL;
		delete f1Der; f1Der=NULL;
		//2021.11.23
		delete f1SplineError; f1SplineError=NULL;
		delete sp3Expot; sp3Expot=NULL;
		
		//----spline as function of different variable
		delete f1SplineEkn; f1SplineEkn=NULL;
		
		//------pull
		delete gPull; gPull=NULL;
		
		//------confidence interval
		resetConfInt(); //2019.09.10
	}
	
	//------getter //2020.11.04
	TF1 *GetFun() {return f1SplineFit;}
	
	//------reset member
	void resetConfInt()
	{
		/*if (geConfInt!=NULL)
		{
			delete geConfInt;
			geConfInt=NULL;
		}*/
		//2019.09.12
		delete geConfInt; geConfInt=NULL;
		delete gConfIntBand; gConfIntBand=NULL;
		//2022.05.06
		delete geConfIntsamepos; geConfIntsamepos=NULL;
	}
	
	//------set A and Z //2021.11.15
	bool SetAZ(const int Q, const int A) {Charge=Q, NAtomic=A; return true;}

	//------set the TGrahpErrors to be fit //2019.09.09
	void SetGraph(TGraphErrors *ge) {geFit=ge;}
	
	//------set the rigidity range //2019.09.10
	//void SetRange(const double dRL, const double dRU) {RangeL=dRL, RangeU=dRU;}
	void SetRange(const double dRL, const double dRU) //2019.09.19: also update the f1SplineFit range if f1SplineFit is built
	{
		RangeL=dRL, RangeU=dRU;
		if (f1SplineFit!=NULL) f1SplineFit->SetRange(RangeL, RangeU);
	}
	
	//------functions to be fit
	//reference: https://root-forum.cern.ch/t/using-tspline-for-fit/18255/3
	double cfSplineFit(double *x, double *p)
	{
		//--parameters:
		//0: no. nodes (fixed)
		//1 - n: x node position (fixed or determind by fit)
		//n+1 - 2n: y node position (determind by fit)
	
		double xx=x[0];
		if (FitOpt&LogX) xx=log(x[0]);
		int nnode=p[0];
		double node[nnode], ynode[nnode];
		for (int inode=0; inode<nnode; inode++)
		{
			node[inode]=(FitOpt&LogX)?log(p[inode+1]):p[inode+1];
			ynode[inode]=(FitOpt&LogY)?log(p[inode+1+nnode]):p[inode+1+nnode];
			//if (verbose) cout << inode << " " << node[inode] << " " << ynode[inode] << " " << nnode << endl;
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		double value = sp3.Eval(xx);
		if (FitOpt&ExtrapolateLE && xx>node[nnode-1])
		{
			value = sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*(xx-node[nnode-1]);
		}
		else if (FitOpt&ExtrapolateLB && xx<node[0])
		{
			value = sp3.Eval(node[0]) + sp3.Derivative(node[0])*(xx-node[0]);
		}
		return (FitOpt&LogY)?exp(value):value;
	}
	
	//--gamma as function of log(R)
	double cfSplineFitFlux(double *x, double *p)
	{
		//--parameters:
		//0: no. nodes (fixed)
		//1 - n: x node position (fixed or determind by fit)
		//n+1 - 2n: Flux position (determind by fit)
		//2n+1: constant
	
		double xx=x[0];
		const int nnode=p[0];
		double node[nnode], ynode[nnode], dConst=p[2*nnode+1];
		for (int inode=0; inode<nnode; inode++)
		{
			node[inode]=log(p[inode+1]); //gamma as function of log(R)
			ynode[inode]=log(p[inode+1+nnode]/dConst)/log(p[inode+1]);
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		double value = dConst*pow(xx, sp3.Eval(log(xx)));
		if (FitOpt&ExtrapolateLE && log(xx)>node[nnode-1])
		{
			value = dConst*pow(xx, sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*(log(xx)-node[nnode-1]));
//			value = dConst*pow(xx, sp3.Eval(node[nnode-1]));
		}
		else if (FitOpt&ExtrapolateLB && log(xx)<node[0])
		{
			value = dConst*pow(xx, sp3.Eval(node[0]) + sp3.Derivative(node[0])*(log(xx)-node[0]));
//			value = dConst*pow(xx, sp3.Eval(node[0]));
		}
		return value;
	}
	
	//--gamma as function of log(R) with ynodes the value of gamma directly
	double cfSplineFitFlux2(double *x, double *p)
	{
		//--parameters:
		//0: no. nodes (fixed)
		//1 - n: x node position (fixed or determind by fit)
		//n+1 - 2n: gamma position (determind by fit)
		//2n+1: constant
	
		double xx=x[0];
		const int nnode=p[0];
		double node[nnode], ynode[nnode], dConst=p[2*nnode+1];
		for (int inode=0; inode<nnode; inode++)
		{
			node[inode]=log(p[inode+1]); //gamma as function of log(R)
			ynode[inode]=p[inode+1+nnode]; //gamma position
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		double value = dConst*pow(xx, sp3.Eval(log(xx)));
		//for extrapolation, use the spectrum index at boundary and do single power law extrapolation
		if (FitOpt&ExtrapolateLE && log(xx)>node[nnode-1])
		{
			//value = dConst*pow(xx, sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*(log(xx)-node[nnode-1]));
			value = dConst*pow(xx, sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*node[nnode-1]);
		}
		else if (FitOpt&ExtrapolateLB && log(xx)<node[0])
		{
			//value = dConst*pow(xx, sp3.Eval(node[0]) + sp3.Derivative(node[0])*(log(xx)-node[0]));
			value = dConst*pow(xx, sp3.Eval(node[0]) + sp3.Derivative(node[0])*node[0]);
		}
		return value;
	}
	
	double cfFluxIndex(double *x, double *p)
	{
		//--parameters:
		//0: no. nodes (fixed)
		//1 - n: x node position
		//n+1 - 2n: y node position
	
		double xx=x[0];
		int nnode=p[0];
		double node[nnode], ynode[nnode];
		for (int inode=0; inode<nnode; inode++) {
			node[inode]=log(p[inode+1]);
			ynode[inode]=p[inode+1+nnode];
			if (verbose) cout << inode << " " << node[inode] << " " << ynode[inode] << " " << nnode << endl;
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		double value = sp3.Eval(log(xx));
		if (FitOpt&ExtrapolateLE && log(xx)>node[nnode-1])
		{
			//value = sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*(log(xx)-node[nnode-1]);
			value = sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*node[nnode-1];
		}
		else if (FitOpt&ExtrapolateLB && log(xx)<node[0])
		{
			//value = sp3.Eval(node[0] + sp3.Derivative(node[0])*(log(xx)-node[0]));
			value = sp3.Eval(node[0]) + sp3.Derivative(node[0])*node[0];
		}
		return value;
	}
	
	double cfFluxIndex2(double *x, double *p)
	{
		//--parameters:
		//0: no. nodes (fixed)
		//1 - n: x node position
		//n+1 - 2n: y node position
	
		double xx=x[0];
		int nnode=p[0];
		double node[nnode], ynode[nnode];
		for (int inode=0; inode<nnode; inode++) {
			node[inode]=log(p[inode+1]);
			ynode[inode]=p[inode+1+nnode];
			if (verbose) cout << inode << " " << node[inode] << " " << ynode[inode] << " " << nnode << endl;
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		double value = sp3.Eval(log(xx));
		if (FitOpt&ExtrapolateLE && log(xx)>node[nnode-1])
		{
			//value = sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*(log(xx)-node[nnode-1]);
			value = sp3.Eval(node[nnode-1]) + sp3.Derivative(node[nnode-1])*node[nnode-1];
		}
		else if (FitOpt&ExtrapolateLB && log(xx)<node[0])
		{
			//value = sp3.Eval(node[0] + sp3.Derivative(node[0])*(log(xx)-node[0]));
			value = sp3.Eval(node[0]) + sp3.Derivative(node[0])*node[0];
		}
		return value;
	}
	
	//------
	double cfSplineFitDer(double *x, double *p)
	{
		double xx=x[0];
		double eps=0.0001;
		//d(logY)/d(logX) as X/Y*dY/dX
		double dh = (f1SplineFit->Eval(xx+eps)-f1SplineFit->Eval(xx-eps))/2/eps;
		eps=eps/2;
		double dh2 = (f1SplineFit->Eval(xx+eps)-f1SplineFit->Eval(xx-eps))/2/eps;
		double der = (4*dh2-dh)/3;
		return xx/f1SplineFit->Eval(xx)*der;
//		//d(logY)/d(logX) directly
//		double dh = ( log(f1SplineFit->Eval(xx*exp(eps)))-log(f1SplineFit->Eval(xx/exp(eps))) )/2/eps;
//		eps=eps/2;
//		double dh2 = ( log(f1SplineFit->Eval(xx*exp(eps)))-log(f1SplineFit->Eval(xx/exp(eps))) )/2/eps;
//		double der = (4*dh2-dh)/3;
//		if (verbose) cout << xx << " " << der << " " << f1SplineFit->Derivative(xx) << endl;
//		return der;
		
//		return xx/f1SplineFit->Eval(xx)*f1SplineFit->Derivative(xx);
		
		//dY/dX
//		cout << xx << " " << f1SplineFit->Derivative(xx) << endl;
//		return f1SplineFit->Derivative(xx);
		
//		double eps=0.001;
//		double dh = (f1SplineFit->Eval(xx+eps)-f1SplineFit->Eval(xx-eps))/2/eps;
//		eps=eps/2;
//		double dh2 = (f1SplineFit->Eval(xx+eps)-f1SplineFit->Eval(xx-eps))/2/eps;
//		double der = (4*dh2-dh)/3;
//		cout << xx << " " << der << " " << f1SplineFit->Derivative(xx) << endl;
//		return f1SplineFit->Derivative(xx);
	}
	
	double cfSplineFitDer2(double *x, double *p)
	{
		double xx=x[0];
		if (FitOpt&LogX) xx=log(x[0]);
		int nnode=p[0];
		double node[nnode], ynode[nnode];
		for (int inode=0; inode<nnode; inode++)
		{
			node[inode]=(FitOpt&LogX)?log(p[inode+1]):p[inode+1];
			ynode[inode]=(FitOpt&LogY)?log(p[inode+1+nnode]):p[inode+1+nnode];
			if (verbose) cout << inode << " " << node[inode] << " " << ynode[inode] << " " << nnode << endl;
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		double value = sp3.Derivative(xx);
		if (FitOpt&ExtrapolateLE && xx>node[nnode-1])
		{
			value = sp3.Derivative(node[nnode-1]);
		}
		else if (FitOpt&ExtrapolateLB && xx<node[0])
		{
			value = sp3.Derivative(node[0]);
		}
		return value;
	}
	
	//------create TF1 based on spline fit function
	//TF1 *BuildTF1(const string sName, const double xmin=0.1, const double xmax=100000)
	TF1 *BuildTF1(const string sName) //Set range using RangeL and RangeU //2019.09.10
	{
		if (nnode<=0) {cout << "Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		//f1SplineFit = new TF1(sName.c_str(), this, &SplineFit::cfSplineFit, xmin, xmax, 2*nnode+1);
		f1SplineFit = new TF1(sName.c_str(), this, &SplineFit::cfSplineFit, RangeL, RangeU, 2*nnode+1); //2019.09.10
		f1SplineFit->SetNpx(100000);
		f1SplineFit->SetParameter(0, nnode);
		f1SplineFit->SetParLimits(0, nnode, nnode);
		f1SplineFit->SetParName(0, "nnode");
		return f1SplineFit;
	}
	
	//TF1 *BuildTF1Flux(const string sName, const double xmin=0.1, const double xmax=100000)
	TF1 *BuildTF1Flux(const string sName) //Set range using RangeL and RangeU //2019.09.10
	{
		if (nnode<=0) {cout << "Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		//f1SplineFit = new TF1(sName.c_str(), this, &SplineFit::cfSplineFitFlux, xmin, xmax, 2*nnode+2);
		f1SplineFit = new TF1(sName.c_str(), this, &SplineFit::cfSplineFitFlux, RangeL, RangeU, 2*nnode+2); //2019.09.10
		f1SplineFit->SetNpx(100000);
		f1SplineFit->SetParameter(0, nnode);
		f1SplineFit->SetParLimits(0, nnode, nnode);
		f1SplineFit->SetParName(0, "nnode");
		return f1SplineFit;
	}
	
	//TF1 *BuildTF1Flux2(const string sName, const double xmin=0.1, const double xmax=100000)
	TF1 *BuildTF1Flux2(const string sName) //Set range using RangeL and RangeU //2019.09.10
	{
		if (nnode<=0) {cout << "Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		//f1SplineFit = new TF1(sName.c_str(), this, &SplineFit::cfSplineFitFlux2, xmin, xmax, 2*nnode+2);
		f1SplineFit = new TF1(sName.c_str(), this, &SplineFit::cfSplineFitFlux2, RangeL, RangeU, 2*nnode+2); //2019.09.10
		f1SplineFit->SetNpx(100000);
		f1SplineFit->SetParameter(0, nnode);
		f1SplineFit->SetParLimits(0, nnode, nnode);
		f1SplineFit->SetParName(0, "nnode");
		return f1SplineFit;
	}
	
	TF1 *UpdateFluxIndex()
	{
		if (nnode<=0) {cout << "Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		if (f1SplineFit==NULL) {cout << "spline function do not existed!!! Called SplineFit::BuildTF1Flux first before calling SplineFit::UpdateFluxIndex" << endl; return NULL;}
		
		if (f1FluxIndex==NULL) //create f1FluxIndex if this function is called the first time
		{
			//cout << "Building Flux Index function." << endl;
			if (verbose) cout << "Building Flux Index function." << endl; //2019.09.09
			//double xmin=0, xmax=0;
			//f1SplineFit->GetRange(xmin, xmax);
			//f1FluxIndex = new TF1(Form("%s_index",f1SplineFit->GetName()), this, &SplineFit::cfFluxIndex, xmin, xmax, 2*nnode+1);
			f1FluxIndex = new TF1(Form("%s_index",f1SplineFit->GetName()), this, &SplineFit::cfFluxIndex, RangeL, RangeU, 2*nnode+1); //Set range using RangeL and RangeU //2019.09.10
			
			f1FluxIndex->SetNpx(10000);
			f1FluxIndex->SetParameter(0, nnode);
			f1FluxIndex->SetParLimits(0, nnode, nnode);
			f1FluxIndex->SetParName(0, "nnode");
		}
		
		//cout << "Updating parameters for Flux Index function." << endl;
		if (verbose) cout << "Updating parameters for Flux Index function." << endl; //2019.09.09
		for (int inode=0; inode<nnode; inode++)
		{
			double dConst = f1SplineFit->GetParameter(2*nnode+1);
			double dx = f1SplineFit->GetParameter(1+inode);
			double dy = log(f1SplineFit->GetParameter(1+nnode+inode)/dConst)/log(dx);
			f1FluxIndex->SetParameter(1+inode, dx);
			f1FluxIndex->SetParameter(1+nnode+inode, dy);
		}
		return f1FluxIndex;
	}
	
	TF1 *UpdateFluxIndex2()
	{
		if (nnode<=0) {cout << "Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		if (f1SplineFit==NULL) {cout << "spline function do not existed!!! Called SplineFit::BuildTF1Flux first before calling SplineFit::UpdateFluxIndex" << endl; return NULL;}
		if (f1FluxIndex==NULL) //create f1FluxIndex if this function is called the first time
		{
			//cout << "Building Flux Index function." << endl;
			if (verbose) cout << "Building Flux Index function." << endl; //2019.09.09
			//double xmin=0, xmax=0;
			//f1SplineFit->GetRange(xmin, xmax);
			//f1FluxIndex = new TF1(Form("%s_index",f1SplineFit->GetName()), this, &SplineFit::cfFluxIndex, xmin, xmax, 2*nnode+1);
			f1FluxIndex = new TF1(Form("%s_index",f1SplineFit->GetName()), this, &SplineFit::cfFluxIndex, RangeL, RangeU, 2*nnode+1); //Set range using RangeL and RangeU //2019.09.10
			f1FluxIndex->SetNpx(10000);
			f1FluxIndex->SetParameter(0, nnode);
			f1FluxIndex->SetParLimits(0, nnode, nnode);
			f1FluxIndex->SetParName(0, "nnode");
		}
		//cout << "Updating parameters for Flux Index function." << endl;
		if (verbose) cout << "Updating parameters for Flux Index function." << endl; //2019.09.09
		for (int inode=0; inode<nnode; inode++)
		{
			double dConst = f1SplineFit->GetParameter(2*nnode+1);
			double dx = f1SplineFit->GetParameter(1+inode);
			double dy = f1SplineFit->GetParameter(1+nnode+inode);
			f1FluxIndex->SetParameter(1+inode, dx);
			f1FluxIndex->SetParameter(1+nnode+inode, dy);
		}
		return f1FluxIndex;
	}
	
	TF1 *BuildDer(int opt=0) //add new parameter opt to control what function used to build the derivate
	{
		if (f1SplineFit==NULL) {cout << "spline function do not existed!!! Called SplineFit::BuildTF1Flux first before calling SplineFit::BuildDer" << endl; return NULL;}
		if (f1Der==NULL)
		{
			//cout << "Building Derivative." << endl;
			if (verbose) cout << "Building Derivative." << endl; //2019.09.09
			//double xmin=0, xmax=0;
			//f1SplineFit->GetRange(xmin, xmax);
			
			//if (opt==0) f1Der = new TF1(Form("%s_der_opt%d",f1SplineFit->GetName(), opt), this, &SplineFit::cfSplineFitDer, xmin+0.1, xmax-1., 0);
			//else if (opt==1) f1Der = new TF1(Form("%s_der_opt%d",f1SplineFit->GetName(), opt), this, &SplineFit::cfSplineFitDer2, xmin+0.1, xmax-1., 0);
			//Set range using RangeL and RangeU //2019.09.10
			if (opt==0) f1Der = new TF1(Form("%s_der_opt%d",f1SplineFit->GetName(), opt), this, &SplineFit::cfSplineFitDer, RangeL+0.1, RangeU-1., 0);
			else if (opt==1) f1Der = new TF1(Form("%s_der_opt%d",f1SplineFit->GetName(), opt), this, &SplineFit::cfSplineFitDer2, RangeL+0.1, RangeU-1., 0);
			else
			{
				cout << "Please choose correct opt: opt=0, cfSplineFitDer; opt=1, cfSplineFitDer2. Derivative is not built." << endl;
				return NULL;
			}
			f1Der->SetNpx(10000);
			f1Der->SetLineColor(kBlack); //2020.01.14
		}
		return f1Der;
	}
	
	TF1* GetSpline() const {return f1SplineFit;}
	
	//----spline in different variable
	double cfSplineBeta(double *x, double *p)
	{
		double dBeta = x[0];
		double dRig = BetaToRig(Charge, NAtomic, dBeta);
		return cfSplineFit(&dRig, p);
	}
	
	double cfSplineEkn(double *x, double *p)
	{
		double dEkn = x[0];
		double dRig = EknToRig(Charge, NAtomic, dEkn);
		if (f1SplineFit==NULL) return -1;
		return f1SplineFit->Eval(dRig);
	}
	
	TF1 *BuildTF1Ekn(const string sName)
	{
		if (nnode<=0) {cout << "SplineFit::BuildTF1Ekn: Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		if (f1SplineFit==NULL) {cout << "SplineFit::BuildTF1Ekn: f1SplineFit is not built, run SplineFit::BuildTF1 first" << endl; return NULL;}
		delete f1SplineEkn;
		f1SplineEkn = new TF1(sName.c_str(), this, &SplineFit::cfSplineEkn, 0.1, 20, 0);
		f1SplineEkn->SetNpx(100000);
		return f1SplineEkn;
	}
	
	//----spline with error //2020.01.20
	double cfSplineError(double *x, double *p)
	{
		//--parameters:
		//0: no. nodes (fixed)
		//1 - n: x node position (fixed or determind by fit)
		//n+1 - 2n: y node position (determind by fit)
		//2n+1: whether with error or not, -1: y-y_error, 0: y, +1: y+y_error
		
		double pnode[2*nnode+1];
		
		//--
		int iE=0;
		if (p[2*nnode+1]<0) iE=-1;
		else if (p[2*nnode+1]>0) iE=1;
		else iE=0;
		
		//no. nodes and x node
		for (int inode=0; inode<nnode+1; inode++) pnode[inode]=p[inode];
		//y node, with or without error
		for (int inode=nnode+1; inode<2*nnode+1; inode++) //consider retrieve the vdYRelError by member function
		{
			//absolute error
			//if (FitOpt&LogY) pnode[inode]=p[inode]*exp(iE*vdYRelError[inode-nnode-1]);
			//else pnode[inode]=p[inode]+iE*vdYRelError[inode-nnode-1];
			
			//relative error
			if (FitOpt&LogY) pnode[inode]=exp(log(p[inode])*(1+iE*vdYRelError[inode-nnode-1]));
			else pnode[inode]=p[inode]*(1+iE*vdYRelError[inode-nnode-1]);
		} //iode
		
		return cfSplineFit(x, pnode);
	}
	
	TF1 *BuildSplineError(const string sName="SplineError")
	{
		if (nnode<=0) {cout << "SplineFit::BuildSplineError - Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		if (vdYRelError.size()!=(unsigned int)nnode) {cout << "SplineFit::BuildSplineError - vdYRelError has different size as nnode" << endl; return NULL;}
		
		f1SplineError = new TF1(sName.c_str(), this, &SplineFit::cfSplineError, RangeL, RangeU, 2*nnode+2);
		f1SplineError->SetNpx(1000);
		f1SplineError->SetParameter(0, nnode);
		f1SplineError->SetParLimits(0, nnode, nnode);
		f1SplineError->SetParName(0, "nnode");
		
		//set the x node and y node if f1SplineFit has built
		if (f1SplineFit!=NULL)
		{
			for (int inode=1; inode<2*nnode+1; inode++)
			{
				f1SplineError->SetParameter(inode, f1SplineFit->GetParameter(inode));
			} //inode
		}
		
		//parameter controling with error or not
		f1SplineError->SetParameter(2*nnode+1, 0);
		f1SplineError->SetParName(2*nnode+1, "ErrorControl");
		
		return f1SplineError;
	}
	
	//----derivative for spline with error
	double cfSplineErrorDer(double *x, double *p)
	{
		/*//d(logY)/d(logX) as X/Y*dY/dX
		double xx=x[0];
		double eps=0.0001;
		double dh = (f1SplineFit->Eval(xx+eps)-f1SplineFit->Eval(xx-eps))/2/eps;
		eps=eps/2;
		double dh2 = (f1SplineFit->Eval(xx+eps)-f1SplineFit->Eval(xx-eps))/2/eps;
		double der = (4*dh2-dh)/3;
		return xx/f1SplineFit->Eval(xx)*der;*/
		
		//directly call the TSpline3::Derivative()
		int iE=0;
		if (p[2*nnode+1]<0) iE=-1;
		else if (p[2*nnode+1]>0) iE=1;
		else iE=0;
		
		double xx=x[0];
		if (FitOpt&LogX) xx=log(x[0]);
		
		int nnode=p[0];
		
		double node[nnode], ynode[nnode];
		for (int inode=0; inode<nnode; inode++)
		{
			node[inode]=(FitOpt&LogX)?log(p[inode+1]):p[inode+1];
			
			ynode[inode]=(FitOpt&LogY)?log(p[inode+1+nnode]):p[inode+1+nnode];
			ynode[inode]=ynode[inode]*(1+iE*vdYRelError.at(inode));
		}
	
		TSpline3 sp3("sp3", node, ynode, nnode, sBoundary.c_str());
	
		return sp3.Derivative(xx);
	}
	
	TF1 *BuildSplineErrorDer(const string sName="SplineErrorDer")
	{
		if (nnode<=0) {cout << "SplineFit::BuildSplineErrorDer - Invalid nnode!!! nnode must be large than zero while nnode=" << nnode << endl; return NULL;}
		if (vdYRelError.size()!=(unsigned int)nnode) {cout << "SplineFit::BuildSplineError - vdYRelError has different size as nnode" << endl; return NULL;}
		
		TF1 *f1SplineErrorDer = new TF1(sName.c_str(), this, &SplineFit::cfSplineErrorDer, RangeL, RangeU, 2*nnode+2);
		f1SplineErrorDer->SetNpx(1000);
		f1SplineErrorDer->SetParameter(0, nnode);
		f1SplineErrorDer->SetParLimits(0, nnode, nnode);
		f1SplineErrorDer->SetParName(0, "nnode");
		
		//set the x node and y node if f1SplineFitDer has built
		if (f1SplineError!=NULL)
		{
			for (int inode=1; inode<2*nnode+1; inode++)
			{
				f1SplineErrorDer->SetParameter(inode, f1SplineError->GetParameter(inode));
			} //inode
		}
		
		//parameter controling with error or not
		f1SplineErrorDer->SetParameter(2*nnode+1, 0);
		f1SplineErrorDer->SetParName(2*nnode+1, "ErrorControl");
		
		return f1SplineErrorDer;
	}
	
	//------reset and update the vdX and vdY
	void resetNodes()
	{
		vdX.clear();
		vdY.clear();
	}
	
	//----update the nodes as the current f1SplineFit parameter
	void updateNodesFromTF1()
	{
		resetNodes();
		if (f1SplineFit==NULL) {cout << "SplineFit::updateNodesFromTF1: TF1 not built!!! Stop setting Xnode" << endl; return;}
		for (int inode=0; inode<nnode; inode++)
		{
			vdX.push_back(f1SplineFit->GetParameter(1+inode));
			vdY.push_back(f1SplineFit->GetParameter(1+nnode+inode));
		} //inode
	}
	
	//------set nodes according to Quantiles
	void SetNodesQuantile(const int iknots=4, const bool bFixFirstNode=true)
	{
		if (geFit==NULL) {cout << "SplineFit::SetNodesQuantile: TGraphErrors not built!!! Stop fitting" << endl; return;}
		double *dQuant=NULL;
		if (iknots==4) dQuant=kontsQuant4;
		else if (iknots==5) dQuant=kontsQuant5;
		
		resetNodes();
		int inodeC=0;
		//------find the first node within [RangeL, RangeU]
		int ip0=0;
		double dX, dY;
		for (int ip=0; ip<geFit->GetN(); ip++)
		{
			geFit->GetPoint(ip, dX, dY);
			double dX1 = geFit->GetX()[ip+1];
			if (dX<=RangeL && dX1>RangeL)
			{
				ip0=ip;
				if (bFixFirstNode)
				{
					vdX.push_back(dX), vdY.push_back(dY);
					if (verbose) cout << inodeC << " " << ip << " " << vdX[inodeC] << " " << vdY[inodeC] << " " << dQuant[inodeC] << endl;
					inodeC++;
				}
				break;
			}
		} //ip
		
		//------set nodes
		double dN=geFit->GetN()-(ip0+1);
		for (int ip=ip0; ip<geFit->GetN(); ip++)
		{
			int ip1=ip-ip0;
			if ((double)(ip1+1)/dN<=dQuant[inodeC] && (double)(ip1+2)/dN>dQuant[inodeC])
			{
				geFit->GetPoint(ip, dX, dY);
				vdX.push_back(dX), vdY.push_back(dY);
				if (verbose) cout << inodeC << " " << ip << " " << vdX[inodeC] << " " << vdY[inodeC] << " " << dQuant[inodeC] << endl;
				inodeC++;
			}
		} //ip
		
		//------set the parameter of f1SplineFit
		SetNode();
	}
	
	/*//2019.09.12: add two more parameters to be able to set quantile within certain range
	void SetNodesQuantile(const int iknots=4, const double xl=-1, const double xu=-1, const bool bFixFirstNode=true)
	{
		if (geFit==NULL) {cout << "SplineFit::SetNodesQuantile: TGraphErrors not built!!! Stop fitting" << endl; return;}
		double *dQuant=NULL;
		if (iknots==4) dQuant=kontsQuant4;
		else if (iknots==5) dQuant=kontsQuant5;
		
		resetNodes();
		int inodeC=0;
		//------range to set the nodes
		double dXL=RangeL, dXU=RangeU;
		if (xl!=-1) dXL=xl;
		if (xu!=-1) dXU=xu;
		
		if (verbose) cout << "Set quantile within: [" << dXL << "," << dXU << "]" << endl;
		//------find the first node within [RangeL, RangeU]
		int ipL=0, ipU=geFit->GetN();
		double dX, dY;
		for (int ip=0; ip<geFit->GetN(); ip++)
		{
			geFit->GetPoint(ip, dX, dY);
			double dX1 = geFit->GetX()[ip+1];
			if (dX<=RangeL && dX1>RangeL)
			{
				ipL=ip;
				if (bFixFirstNode)
				{
					vdX.push_back(dX), vdY.push_back(dY);
					cout << inodeC << " " << ip << " " << vdX[inodeC] << " " << vdY[inodeC] << " " << dQuant[inodeC] << endl;
					inodeC++;
				}
				break;
			}
		} //ip
		
		//------set nodes
		double dN=geFit->GetN()-(ipL+1);
		for (int ip=ipL; ip<geFit->GetN(); ip++)
		{
			int ip1=ip-ipL;
			if ((double)(ip1+1)/dN<=dQuant[inodeC] && (double)(ip1+2)/dN>dQuant[inodeC])
			{
				geFit->GetPoint(ip, dX, dY);
				vdX.push_back(dX), vdY.push_back(dY);
				cout << inodeC << " " << ip << " " << vdX[inodeC] << " " << vdY[inodeC] << " " << dQuant[inodeC] << endl;
				inodeC++;
			}
		} //ip
		
		//------set the parameter of f1SplineFit
		SetNode();
	}*/
	
	//------set node even-logrithmly
	void SetNodesEvenLog(const double xl=-1, const double xu=-1)
	{
		if (geFit==NULL) {cout << "SplineFit::SetNodesEvenLog: TGraphErrors not built!!! Stop fitting" << endl; return;}
		
		resetNodes();
		
		//------range to set the nodes
		double dXL=RangeL, dXU=RangeU;
		if (xl!=-1) dXL=xl;
		if (xu!=-1) dXU=xu;
		
		if (verbose) cout << "Set even log bin within: [" << dXL << "," << dXU << "]" << endl;
		
		//------set nodes even logly
		double dX, dY;
		for (int inode=0; inode<nnode; inode++)
		{
			dX = dXL*pow(dXU/dXL, (double)inode/(nnode-1));
			dY = geFit->Eval(dX);
			vdX.push_back(dX), vdY.push_back(dY);
			if (verbose) cout << inode << " " << dX << " " << dY << endl;
		} //inode
		
		//------set the parameter of f1SplineFit
		SetNode();
	}
	
	//------set node by Xnode position
	void SetNodesByX(double *xnode)
	{
		if (geFit==NULL) {cout << "SplineFit::SetNodesByX: TGraphErrors not built!!! Stop fitting" << endl; return;}
		
		resetNodes();
		
		//------set nodes by given X node position
		double dX, dY;
		for (int inode=0; inode<nnode; inode++)
		{
			dX = xnode[inode];
			dY = geFit->Eval(dX);
			vdX.push_back(dX), vdY.push_back(dY);
		} //inode
		
		//------set the parameter of f1SplineFit
		SetNode();
	}
	
	//------set parameter value & limits
	//----xnodes
	int SetXnode(double *xnode)
	{
		if (f1SplineFit==NULL) {cout << "TF1 not built!!! Stop setting Xnode" << endl; return -1;}
		for (int inode=0; inode<nnode; inode++)
		{
			f1SplineFit->SetParameter(1+inode, xnode[inode]);
			f1SplineFit->SetParName(1+inode, Form("node%d",inode));
		}
		return nnode;
	}
	
	int SetXnodeLimits(int opt=0, double *xnodel=NULL, double *xnodeu=NULL)
	{
		if (f1SplineFit==NULL) {cout << "TF1 not built!!! Stop setting Xnode Limits" << endl; return -1;}
		for (int inode=0; inode<nnode; inode++)
		{
			if (opt==0) f1SplineFit->SetParLimits(1+inode, f1SplineFit->GetParameter(1+inode), f1SplineFit->GetParameter(1+inode));
			else if (opt==1) //set node range within the first and last nodes, fix first and last nodes
			{
				if (inode==0 || inode==nnode-1) f1SplineFit->SetParLimits(1+inode, f1SplineFit->GetParameter(1+inode), f1SplineFit->GetParameter(1+inode));
				else f1SplineFit->SetParLimits(1+inode, f1SplineFit->GetParameter(1), f1SplineFit->GetParameter(1+nnode));
			}
			else f1SplineFit->SetParLimits(1+inode, xnodel[inode], xnodeu[inode]);
		}
		return nnode;
	}
	
	//----ynodes
	int SetYnode(double *ynode)
	{
		if (f1SplineFit==NULL) {cout << "TF1 not built!!! Stop setting Ynode" << endl; return -1;}
		for (int inode=0; inode<nnode; inode++)
		{
			f1SplineFit->SetParameter(1+nnode+inode, ynode[inode]);
			f1SplineFit->SetParName(1+nnode+inode, Form("ynode%d",inode));
		}
		return nnode;
	}
	
	int SetYnodeLimits(int opt=0, double *ynodel=NULL, double *ynodeu=NULL)
	{
		if (f1SplineFit==NULL) {cout << "TF1 not built!!! Stop setting Ynode Limits" << endl; return -1;}
		for (int inode=0; inode<nnode; inode++)
		{
			if (opt==0) f1SplineFit->SetParLimits(1+nnode+inode, f1SplineFit->GetParameter(1+nnode+inode), f1SplineFit->GetParameter(1+nnode+inode));
			else f1SplineFit->SetParLimits(1+nnode+inode, ynodel[inode], ynodeu[inode]);
		}
		return nnode;
	}
	
	//----set nodes according to vdX & vdY
	int SetNode()
	{
		if ((int)vdX.size()!=nnode) {cout << "SplineFit::SetNode - vdX has different length to nnode!!! Stop setting node" << endl; return -1;}
		if ((int)vdY.size()!=nnode) {cout << "SplineFit::SetNode - vdY has different length to nnode!!! Stop setting node" << endl; return -1;}
		if (f1SplineFit==NULL) {cout << "SplineFit::SetNode - TF1 not built!!! Stop setting node" << endl; return -1;}
		
		for (int inode=0; inode<nnode; inode++)
		{
			f1SplineFit->SetParameter(1+inode, vdX[inode]);
			f1SplineFit->SetParName(1+inode, Form("xnode%d",inode));
			
			f1SplineFit->SetParameter(1+nnode+inode, vdY[inode]);
			f1SplineFit->SetParName(1+nnode+inode, Form("ynode%d",inode));
		}
		return nnode;
	}
	
	//------charnge given node //2020.01.16
	int ChangeNode(int inode, double dX, double dY)
	{
		if (inode<0 || inode>=nnode) {cout << "SplineFit::ChangeNode - invalid inode!!!" << endl; return -1;}
		if (f1SplineFit==NULL) {cout << "SplineFit::ChangeNode - TF1 not built!!!" << endl; return -1;}
		
		vdX[inode]=dX;
		f1SplineFit->SetParameter(1+inode, vdX[inode]);
		
		vdY[inode]=dY;
		f1SplineFit->SetParameter(1+nnode+inode, vdY[inode]);
		
		return inode;
	}
	
	//------fitting
	//----do fit
	//void doFit(const string fopt="FQ0", const string gopt="", const double dFitL=-1, const double dFitU=-1)
	void doFit(const double dFitL=-1, const double dFitU=-1, const string fopt="FQ0", const string gopt="") //2019.09.22
	{
		if (f1SplineFit==NULL) {cout << "SplineFit::doFit: TF1 not built!!! Stop fitting" << endl; return;}
		if (geFit==NULL) {cout << "SplineFit::doFit: TGraphErrors not built!!! Stop fitting" << endl; return;}
		
		double dFL=RangeL, dFU=RangeU;
		//if (dFitL!=-1 && dFitU!=-1) dFL=dFitL, dFU=dFitU;
		if (dFitL!=-1) dFL=dFitL;
		if (dFitU!=-1) dFU=dFitU;
		frpFitResult = geFit->Fit(f1SplineFit, fopt.c_str(), gopt.c_str(), dFL, dFU);
		
		//reset the Confidence interval every time we perform a new fit
		resetConfInt();
		
		//update the vdX & vdY to fit result
		updateNodesFromTF1();
	}
	
	//----confidence interval
	TGraphErrors *CalConfInt(const double dCI, const int iNCINode=200) //2021.09.24
	//TGraphErrors *CalConfInt(const double dCI)
	//TGraphErrors *CalConfInt(const double dCI, const int charge) //2019.09.13
	{
		//if (geConfInt!=NULL) return geConfInt;
		if (geConfInt==NULL) //2022.05.06
		{
			geConfInt = new TGraphErrors();
			//geConfInt->SetTitle(Form("%.2f conf. interval", dCI));
			//2022.05.06
			histn = Form("%.2f conf. interval", dCI);
			geConfInt->SetTitle(histn.c_str());
			//if (!bSamePos) sConfInt = "evenlog";
			//else sConfInt = "samepos";
			//dConfInt = dCI;
			
			/*geConfInt->SetPoint(0, RangeL, 0);
			for (int i=1; i<geFit->GetN()+1; i++) geConfInt->SetPoint(i, geFit->GetX()[i-1], 0);*/
			//for (int i=0; i<geFit->GetN(); i++) geConfInt->SetPoint(i, geFit->GetX()[i], 0);
			//2019.09.24: only get the confidence band within range
			//geConfInt->SetPoint(0, RangeL, 0);
			//for (int i=0; i<geFit->GetN(); i++)
			//{
			//	if (geFit->GetX()[i]<=RangeL) continue;
			//	if (geFit->GetX()[i]>=RangeU) break;
			//	geConfInt->SetPoint(geConfInt->GetN(), geFit->GetX()[i], 0);
			//}
			//geConfInt->SetPoint(geConfInt->GetN(), RangeU, 0);
			//2021.09.24: set even log bin in the range
			for (int i=0; i<iNCINode; i++)
			{
				double dX = RangeL*pow(RangeU/RangeL, (double)i/(iNCINode-1));
				geConfInt->SetPoint(i, dX, 0);
			}
			(TVirtualFitter::GetFitter())->GetConfidenceIntervals(geConfInt, dCI);
		}
		
		//store both the even log bin and the same no. bin when use to calculate pull //2022.05.06
		if (geConfIntsamepos==NULL)
		{
			geConfIntsamepos = new TGraphErrors();
			histn = Form("%.2f conf. interval", dCI);
			geConfIntsamepos->SetTitle(histn.c_str());
			geConfIntsamepos->SetPoint(0, RangeL, 0);
			for (int i=0; i<geFit->GetN(); i++)
			{
				if (geFit->GetX()[i]<=RangeL) continue;
				if (geFit->GetX()[i]>=RangeU) break;
				geConfIntsamepos->SetPoint(geConfIntsamepos->GetN(), geFit->GetX()[i], 0);
			}
			geConfIntsamepos->SetPoint(geConfIntsamepos->GetN(), RangeU, 0);
			(TVirtualFitter::GetFitter())->GetConfidenceIntervals(geConfIntsamepos, dCI);
		}
		
		return geConfInt;
	}
	
	TGraph *GetConfIntBand(const int iFillStyle=3001, const int iColor=kBlack)
	{
		if (geConfInt==NULL) {cout << " *** SplineFit::GetConfIntBand: geConfInt not built!!! *** " << endl; return NULL;}
		
		int np = geConfInt->GetN();
		gConfIntBand = new TGraph(np*2);
		for (int i=0; i<np; i++)
		{
			double x1 = geConfInt->GetX()[i];
			double y1 = geConfInt->GetY()[i], ey1l = geConfInt->GetErrorY(i);
			double x2 = geConfInt->GetX()[np-i-1];
			double y2 = geConfInt->GetY()[np-i-1], ey2u = geConfInt->GetErrorY(np-i-1);
		
			gConfIntBand->SetPoint(i, x1, y1-ey1l);
			gConfIntBand->SetPoint(np+i, x2, y2+ey2u);
		}
		gConfIntBand->SetFillColor(iColor);
		gConfIntBand->SetFillStyle(iFillStyle);
		gConfIntBand->SetLineColor(iColor);
		return gConfIntBand;
	}
	
	//2019.11.12
	TGraphErrors *CalConfIntVD(const double dCI, vector<double> vdX)
	{
		if (geConfInt!=NULL) return geConfInt;
		geConfInt = new TGraphErrors();
		geConfInt->SetTitle(Form("%.2f conf. interval", dCI));
		//only get the confidence band within range
		geConfInt->SetPoint(0, RangeL, 0);
		for (unsigned int i=0; i<vdX.size(); i++)
		{
			if (vdX[i]<=RangeL) continue;
			if (vdX[i]>=RangeU) break;
			geConfInt->SetPoint(geConfInt->GetN(), vdX[i], 0);
		}
		geConfInt->SetPoint(geConfInt->GetN(), RangeU, 0);
		(TVirtualFitter::GetFitter())->GetConfidenceIntervals(geConfInt, dCI);
		
		return geConfInt;
	}
	
	TGraphErrors *CalConfIntEvenLog(const double dCI, double dL, double dU, int Np)
	{
		vector<double> vdEvenLog;
		for (int inode=0; inode<Np; inode++)
		{
			vdEvenLog.push_back(dL*pow(dU/dL,inode/(Np-1.)));
		} //inode
		
		return CalConfIntVD(dCI, vdEvenLog);
	}
	
	//----calculate the pull
	TGraph *CalPull(const string sName, const int iMarkerStyle=3, const int iMarkerColor=kBlack)
	{
		if (f1SplineFit==NULL) {cout << "SplineFit::CalPull: TF1 not built!!!" << endl; return NULL;}
		if (geFit==NULL) {cout << "SplineFit::CalPull: TGraphErrors not built!!!" << endl; return NULL;}
		if (geConfIntsamepos==NULL) {cout << " *** SplineFit::CalPull: geConfIntsamepos not built!!! *** " << endl; return NULL;}
		
		delete gPull;
		gPull = new TGraph();
		gPull->SetNameTitle(sName.c_str(), sName.c_str());
		
		//cout << "SplineFit::CalPull " << geFit->GetN() << " " << geConfInt->GetN() << endl;
		/*for (int ip=0; ip<geFit->GetN(); ip++)
		{
			double dPull;
			//dPull = ((geFit->GetY())[ip]-f1SplineFit->Eval((geFit->GetX())[ip]))/sqrt(pow(geFit->GetErrorY(ip),2)+pow(geConfInt->GetErrorY(ip),2));
			double dX=geFit->GetX()[ip];
			double dYd=geFit->GetY()[ip], dYe=f1SplineFit->Eval(dX);
			double dEYd=geFit->GetErrorY(ip), dEYe=geConfInt->GetErrorY(ip);
			dPull = (dYd-dYe)/sqrt(pow(dEYd,2)+pow(dEYe,2));
			if (geFit->GetX()[ip] != geConfInt->GetX()[ip]) cout << geFit->GetX()[ip] << " " << geConfInt->GetX()[ip] << endl; //2019.09.13
			gPull->SetPoint(ip, dX, dPull);
		} //ip*/
		//2019.09.24: only cal the pull within the range, loop over geConfInt to find the correspond point
		for (int ip=0; ip<geFit->GetN(); ip++)
		{
			if (geFit->GetX()[ip]<RangeL) continue;
			if (geFit->GetX()[ip]>RangeU) break;
			
			int ipConfInt=-1;
			for (int ip0=0; ip0<geConfIntsamepos->GetN(); ip0++) if (geFit->GetX()[ip]==geConfIntsamepos->GetX()[ip0]) ipConfInt=ip0;
			if (ipConfInt==-1) {cout << "Missing points: " << geFit->GetX()[ip] << " sName=" << sName << endl; continue;}
			
			double dPull;
			double dX=geFit->GetX()[ip];
			double dYd=geFit->GetY()[ip], dYe=f1SplineFit->Eval(dX);
			double dEYd=geFit->GetErrorY(ip), dEYe=geConfIntsamepos->GetErrorY(ipConfInt);
			dPull = (dYd-dYe)/sqrt(pow(dEYd,2)+pow(dEYe,2));
			if (geFit->GetX()[ip] != geConfIntsamepos->GetX()[ipConfInt]) cout << "SplineFit::CalPull warning: wrong confidence interval used:" << geFit->GetX()[ip] << " " << geConfIntsamepos->GetX()[ipConfInt] << endl; //2019.09.13
			gPull->SetPoint(ip, dX, dPull);
		} //ip
		gPull->SetMarkerStyle(iMarkerStyle);
		gPull->SetMarkerColor(iMarkerColor);
		return gPull;
	}
	
	//------calculate the relative error from the Confidence Interval Band
	TH1F *GetCIError(const int Nbin, const double *Bins, const double dCI)
	{
		TH1F *hCIError = new TH1F("CIError", "CIError", Nbin, Bins);
		(TVirtualFitter::GetFitter())->GetConfidenceIntervals(hCIError, dCI);
		for (int ibin=1; ibin<hCIError->GetNbinsX()+1; ibin++)
		{
			//if (hCIError->GetBinCenter(ibin)<RangeL) continue; //2019.11.01 //2019.11.05: comment out and set it to zero after
			//double dFit=hCIError->GetBinContent(ibin);
			//use the absolute value of center value
			double dFit=abs(hCIError->GetBinContent(ibin));
			double dCIError=hCIError->GetBinError(ibin);
			if (hCIError->GetBinCenter(ibin)<RangeL) hCIError->SetBinContent(ibin, 0);
			else hCIError->SetBinContent(ibin, dCIError/dFit);
			hCIError->SetBinError(ibin, 0);
		} //ibin
		return hCIError;
	}
	
	//------calculate the deviation //2019.11.01
	//deviation of the function value from the value at the last node, where the efficiency is assumed to be constant //2020.01.16
	TH1F *GetDeviationFromConst(const int Nbin, const double *Bins, const double dRigConst=-1)
	{
		double dRigLimit=f1SplineFit->GetParameter(nnode);
		if (dRigConst!=-1) dRigLimit=dRigConst;
		
		//2020.01.16
		double dValueConstant=f1SplineFit->Eval(dRigLimit);
		
		TH1F *hDevConst = new TH1F("DeviationFromConst", "DeviationFromConst", Nbin, Bins);
		for (int ip=0; ip<geFit->GetN(); ip++)
		{
			double dRig=geFit->GetX()[ip];
			if (dRig<dRigLimit) continue;
			/*double dValue=geFit->GetY()[ip];
			hDevConst->SetBinContent(hDevConst->FindBin(dRig), abs(dValue-f1SplineFit->Eval(dRig))/f1SplineFit->Eval(dRig));
			hDevConst->SetBinError(ip, 0); //need to think how to assign the error*/
			//2020.01.16
			double dValue=f1SplineFit->Eval(dRig);
			hDevConst->SetBinContent(hDevConst->FindBin(dRig), abs(dValue-dValueConstant)/dValueConstant);
		} //ip
		return hDevConst;
	}
	
	//------spline multiplied by single power law //2020.01.14
	double cfSplineScalePowerLaw(double *x, double *p)
	{
		double xx=x[0];
		return f1SplineFit->Eval(xx)*pow(xx,p[0]);
	}
	
	TF1 *BuildSplineScalePowerLaw(const double index=2.7)
	{
		if (f1SplineFit==NULL) {cout << "SplineFit::BuildSplineScalePowerLaw: TF1 not built!!!" << endl; return NULL;}
		
		TF1 *f1SplineFitScalePowerLaw = new TF1(Form("Spline_ScalePowerLaw"), this, &SplineFit::cfSplineScalePowerLaw, RangeL, RangeU, 1);
		f1SplineFitScalePowerLaw->SetParameter(0,index);
		f1SplineFitScalePowerLaw->SetNpx(100000);
		return f1SplineFitScalePowerLaw;
	}
	
	//------spline with error multiplied by single power law //2020.01.20
	double cfSplineErrorScalePowerLaw(double *x, double *p)
	{
		double xx=x[0];
		return cfSplineError(x,p)*pow(xx,p[2*nnode+2]);
	}
	
	TF1 *BuildSplineErrorScalePowerLaw(const double index=2.7)
	{
		if (f1SplineError==NULL) {cout << "SplineFit::BuildSplineErrorScalePowerLaw: f1SplineError not built!!!" << endl; return NULL;}
		
		TF1 *f1SplineErrorScalePowerLaw = new TF1(Form("SplineError_ScalePowerLaw"), this, &SplineFit::cfSplineErrorScalePowerLaw, RangeL, RangeU, 2*nnode*3);
		for (int inode=0; inode<2*nnode+2; inode++) f1SplineErrorScalePowerLaw->SetParameter(inode,f1SplineError->GetParameter(inode));
		f1SplineErrorScalePowerLaw->SetParameter(2*nnode+2,index);
		f1SplineErrorScalePowerLaw->SetNpx(1000);
		return f1SplineErrorScalePowerLaw;
	}
	
	//------flux times exposure time //2020.02.06
	bool SetExpot(TSpline3 *sp3T)
	{
		if (sp3T!=sp3Expot)
		{	delete sp3Expot;
			sp3Expot = sp3T;
		}
		return true;
	}
	
	double cfFluxExpot(double *x, double *p)
	{
		double xx=x[0];
		double dValue=f1SplineFit->Eval(xx)*sp3Expot->Eval(xx)/sp3Expot->Eval(40);
		return (dValue<0)?0:dValue;
	}
	
	TF1 *BuildFluxExpot(TSpline3 *sp3T)
	{
		if (f1SplineFit==NULL) {cout << "SplineFit::BuildFluxExpot: TF1 not built!!!" << endl; return NULL;}
		
		SetExpot(sp3T);
		TF1 *f1FluxExpot = new TF1(Form("Spline_FluxExpot"), this, &SplineFit::cfFluxExpot, RangeL, RangeU, 0);
		f1FluxExpot->SetNpx(100000);
		return f1FluxExpot;
	}
	
	//------store function's parameters, Fit option & boundary conditions
	int StoreFun(const string sOutName)
	{
		FILE *fOut = fopen(sOutName.c_str(),"w");
		int npar = OutPar2(*f1SplineFit, fOut);
		fprintf(fOut,"%d\n", FitOpt);
		fprintf(fOut,"%s\n", sBoundary.c_str());
		fclose(fOut);
		return npar;
	}
	
	int BuildFunFromFile(const string sInName, int opt=0)
	{
		FILE *fIn = fopen(sInName.c_str(),"r");
		if (fIn==NULL) return 0; //2023.05.03
		
		int npar=0;
		int stat = fscanf(fIn,"%d\n",&npar);
		if (npar==0)
		{
			cout << "***NPar=0, stop reading***" << endl;
			return 0;
		}
		
		double element;
		char cBoundary[1000];
		
		//--read the parameters
		for (int ipar=0; ipar<npar; ipar++)
		{
			stat = fscanf(fIn,"%lf",&element);
			if (ipar==0)
			{
				nnode=element;
				if (opt==0) BuildTF1("");
				else if (opt==1) BuildTF1Flux("");
				else if (opt==2) BuildTF1Flux2("");
				
			}
			f1SplineFit->SetParameter(ipar, element);
		}
		stat = fscanf(fIn,"%d",&FitOpt);
		stat = fscanf(fIn,"%s",cBoundary);
		sBoundary.append(cBoundary);
		if (stat==-1) sBoundary="";
		fclose(fIn);
		return npar;
	}
	
	//2020.01.20
	//------store the error of y node
	int StoreError(const string sOutName)
	{
		FILE *fOut = fopen(sOutName.c_str(),"w");
		for (int inode=0; inode<nnode; inode++) fprintf(fOut,"%15e", vdYRelError.at(inode));
		fclose(fOut);
		return nnode;
	}
	
	int ReadError(const string sInName)
	{
		FILE *fIn = fopen(sInName.c_str(),"r");
		
		vdYRelError.clear();
		double element;
		for (int inode=0; inode<nnode; inode++) {
			int stat = fscanf(fIn,"%lf",&element);
			vdYRelError.push_back(element);
		}
		
		fclose(fIn);
		return nnode;
	}
	
	//2020.01.20
	//------plot spline with and without error
	bool DrawSplineError()
	{
		if (f1SplineError==NULL) {cout << "SplineFit::DrawSplineError - Spline with error not built!!!" << endl; return false;}
		
		f1SplineError->SetLineStyle(2);
		f1SplineError->SetLineColor(kRed);
		f1SplineError->SetParameter(2*nnode+1, -1);
		f1SplineError->DrawCopy("SAME");
		f1SplineError->SetParameter(2*nnode+1, +1);
		f1SplineError->DrawCopy("SAME");
		
		f1SplineError->SetLineStyle(1);
		f1SplineError->SetLineColor(kBlue);
		f1SplineError->SetParameter(2*nnode+1, 0);
		f1SplineError->DrawCopy("SAME");
		
		return true;
	}
	
	//------plot spline with and without error
	bool DrawSplineErrorRa(const double index=2.7)
	{
		TF1 *f1SplineErrorRa = BuildSplineErrorScalePowerLaw(index);
		if (f1SplineErrorRa==NULL) {cout << "SplineFit::DrawSplineErrorRa - Error building BuildSplineErrorScalePowerLaw!!!" << endl; return false;}
		
		f1SplineErrorRa->SetLineStyle(2);
		f1SplineErrorRa->SetLineColor(kRed);
		f1SplineErrorRa->SetParameter(2*nnode+1, -1);
		f1SplineErrorRa->DrawCopy("SAME");
		f1SplineErrorRa->SetParameter(2*nnode+1, +1);
		f1SplineErrorRa->DrawCopy("SAME");
		
		f1SplineErrorRa->SetLineStyle(1);
		f1SplineErrorRa->SetLineColor(kBlue);
		f1SplineErrorRa->SetParameter(2*nnode+1, 0);
		f1SplineErrorRa->DrawCopy("SAME");
		
		delete f1SplineErrorRa;
		return true;
	}
	
	//------plot the derivative of spline with and without error
	bool DrawSplineErrorDer()
	{
		TF1 *f1SplineErrorDer = BuildSplineErrorDer();
		if (f1SplineErrorDer==NULL) {cout << "SplineFit::DrawSplineErrorDer - Error building BuildSplineErrorDer!!!" << endl; return false;}
		
		f1SplineErrorDer->SetLineStyle(2);
		f1SplineErrorDer->SetLineColor(kRed);
		f1SplineErrorDer->SetParameter(2*nnode+1, -1);
		f1SplineErrorDer->DrawCopy("SAME");
		f1SplineErrorDer->SetParameter(2*nnode+1, +1);
		f1SplineErrorDer->DrawCopy("SAME");
		
		f1SplineErrorDer->SetLineStyle(1);
		f1SplineErrorDer->SetLineColor(kBlue);
		f1SplineErrorDer->SetParameter(2*nnode+1, 0);
		f1SplineErrorDer->DrawCopy("SAME");
		
		delete f1SplineErrorDer;
		return true;
	}
	
	//------Eval method //2020.01.27
	double SplineEval(const double dX)
	{
		if (f1SplineFit==NULL) {cout << "SplineFit::SplineEval - f1SplineFit not built!!!" << endl; return -9999;}
		return f1SplineFit->Eval(dX);
	}
	
	double SplineEval(const double dXL, const double dXU, const double index=-2.7)
	{
		double dX = CalAbs(dXL, dXU, index);
		return SplineEval(dX);
	}
};
#endif //SPLINEFIT3_H
