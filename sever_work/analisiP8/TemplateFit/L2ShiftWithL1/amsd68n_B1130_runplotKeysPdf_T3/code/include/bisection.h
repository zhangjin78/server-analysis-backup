//----------
//created date: 2019.01.15
//functions to solve problems related to zero point searching using bisection method
//----------
//----------

//------point when two functions are equal
double EqualPoint(TF1 *f1, TF1 *f2, double &dLow, double &dUp, const double error=1e-10)
{
	double dMid=(dLow+dUp)/2.;
	while (abs(f1->Eval(dMid)-1./dMid)>error)
	{
		if ( ((f1->Eval(dLow)-1./dLow)*(f1->Eval(dUp)-1./dUp))>0 ) break;
		if ( ((f1->Eval(dLow)-1./dLow)*(f1->Eval(dMid)-1./dMid))<0 ) dUp=dMid;
		else if ( ((f1->Eval(dMid)-1/dMid)*(f1->Eval(dUp)-1./dUp))<0 ) dLow=dMid;
		dMid=(dLow+dUp)/2.;
	}
	return dMid;
} //EqualPoint
