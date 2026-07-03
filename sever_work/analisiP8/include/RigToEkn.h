#ifndef RIGTOEKN_h
#define RIGTOEKN_h

#include "drawvariable.h"

double RigToEkn(const double dRig, const int Z, const int A)
{
	double dM=A*MPROTON;
	return (sqrt(pow(Z*dRig,2)+pow(dM,2))-dM)/A;
}

//the flux renormalization factor when transform from given rig bin into corresponding Ekn bin
double FluxRigToEkn(double &dEkn, const double dRigL, const double dRigU, const int Z, const int A, const double index=-2.7)
{
	if (dRigL==dRigU)
	{
		cout << "*** FluxRigToEkn: Rigidity bin width equal 0!!! dRigL=" << dRigL << " dRigU=" << dRigU << ". stop calculation." << endl;
		return -1;
	}
	
	double dEknL=RigToEkn(dRigL, Z, A), dEknU=RigToEkn(dRigU, Z, A);
	dEkn = CalAbs(dEknL, dEknU, index);
	
	return (dRigU-dRigL)/(dEknU-dEknL);
}

#endif //RIGTOEKN_h
