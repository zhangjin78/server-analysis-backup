#ifndef QSELRANGE_H
#define QSELRANGE_H

//define a namespace for QDisId
namespace QDis
{
	enum QDisId{InnerQ, UTofQ, LTofQ, InnInnerQ, L1Q, L9Q};

	//----
	//function to transform the order of detector (L1, UTof, Inner, LTof, L9) to the old index of detector (Inner, UTof, LTof, InnInner, L1, L9)
	int DetToQdis(int idet);

	//inverse of DetToQdis
	int QdisToDet(int iqdis);

	//----charge selection range with updated charge cut, ilu=0, lower limit; ilu=1, upper limit
	//--charge selection range for proton and Helium, for Pass7: https://twiki.cern.ch/twiki/bin/view/AMS/PHeNucleiPass7
	double GetQSelRangePHe(const int ilu, const int ich, const int iqdis, const bool bL1QUCut=false);

	//--charge selection range for 2<Q<=8, https://twiki.cern.ch/twiki/bin/view/AMS/PHeNucleiPass7
	double GetQSelRangeQ8(const int ilu, const int ich, const int iqdis, const bool bL1QUCut=false);

	//--charge selection range for Q>=9, https://twiki.cern.ch/twiki/bin/view/AMS/PHeNucleiPass7
	double GetQSelRangeTwiki(const int ilu, const int ich, const int iqdis, const bool bL1QUCut=false);

	//--charge selection range for Iron, same as QY's presentation, 2020.05.15, https://indico.cern.ch/event/918422/
	//different compare to Twiki: looser in lower cut limit for TOFQ, from 24.99 to 24.8
	double GetQSelRangeFe(const int ilu, const int ich, const int iqdis, const bool bL1QUCut=false); 

	//--charge selection range for Nickel
	//different compare to Twiki: looser in lower cut limit for TOFQ, from 26.99 to 26.8, simliar to Iron
	//--TODO might need to adjust the L1Q cut depends on the below L1 BG study, as the lower limit is 24.95 and might have charge confusion from Iron
	double GetQSelRangeNi(const int ilu, const int ich, const int iqdis, const bool bL1QUCut=false); 

	//--
	double GetQSelRange(const int ilu, const int ich, const int iqdis, const bool bL1QUCut=false);
	//return charge selection range using:
	//[1,2]:	GetQSelRangePHe
	//[3,8]:	GetQSelRangeQ8
	//[9,25]:	GetQSelRangeTwiki
	//26:		GetQSelRangeFe
	//28:		GetQSelRangeNi

	//------
	//function to transform the order of detector (L1, UTof, Inner, LTof, L9) to the old index of detector (Inner, UTof, LTof, InnInner, L1, L9)
	int DetToQdis(int idet)
	{
		int iqdis=-1;
		if (idet==0) iqdis=4;		//L1Q
		else if (idet==1) iqdis=1;	//UTofQ
		else if (idet==2) iqdis=0;	//InnerQ
		else if (idet==3) iqdis=2;	//LTofQ
		else if (idet==4) iqdis=5;	//L9Q
		return iqdis;
	}

	//inverse of DetToQdis
	int QdisToDet(int iqdis)
	{
		int idet=-1;
		if (iqdis==0) idet=2;		//InnerQ
		else if (iqdis==1) idet=1;	//UTofQ
		else if (iqdis==2) idet=3;	//LTofQ
		else if (iqdis==4) idet=0;	//L1Q
		else if (iqdis==5) idet=4;	//L9Q
		return idet;
	}

	//------
	double GetQSelRangePHe(const int ilu, const int ich, const int iqdis, const bool bL1QUCut)
	{
		if (ich>2) return -1;
		
		double dQlu[2]={0.};
		
		//--get charge and charge cut
		if (iqdis==0) //InnerQ
		{
			dQlu[0] = ich-0.3, dQlu[1] = ich+0.4;
		}
		else if (iqdis==1) //UTofQ
		{
			dQlu[0] = ich-0.5, dQlu[1] = ich+1.5;
		}
		else if (iqdis==2) //LTofQ
		{
			dQlu[0] = ich-0.5, dQlu[1] = 9999;
		}
		else if (iqdis==3) //InnInnerQ
		{
		}
		else if (iqdis==4) //L1Q
		{
			dQlu[0] = ich-0.4, dQlu[1] = ich+1.;
			//if (!isreal && !bL1QUCut) dQlu[1]=9999;
			if (!bL1QUCut) dQlu[1]=9999;
		}
		else if (iqdis==5) //L9Q
		{
			dQlu[0] = ich-0.4, dQlu[1] = ich+1.;
		}
		return dQlu[ilu];
	} //GetQSelRangePHe

	double GetQSelRangeQ8(const int ilu, const int ich, const int iqdis, const bool bL1QUCut)
	{
		if (ich>8 || ich<=2) return -1;
		
		double dQlu[2]={0.};
		
		//--get charge and charge cut
		if (iqdis==0) //InnerQ
		{
			dQlu[0] = ich-0.45;dQlu[1] = ich+0.45;
		}
		else if (iqdis==1) //UTofQ
		{
			dQlu[0] = ich-0.6;dQlu[1] = ich+1.5;
		}
		else if (iqdis==2) //LTofQ
		{
			dQlu[0] = ich-0.6;dQlu[1] = 9999;
		}
		else if (iqdis==3) //InnInnerQ
		{
		}
		else if (iqdis==4) //L1Q
		{
			dQlu[0] = ich-0.46 - (ich-3)*0.16;dQlu[1] = (ich<=5)?ich+0.65:ich+0.65+(ich-5)*0.03;
			//if (!isreal && !bL1QUCut) dQlu[1]=9999;
			if (!bL1QUCut) dQlu[1]=9999;
		}
		else if (iqdis==5) //L9Q
		{
			dQlu[0] = ich-0.5;dQlu[1] = ich+1.5+(ich-3)*0.06;
		}
		return dQlu[ilu];
	} //GetQSelRangeQ8

	double GetQSelRangeTwiki(int ilu, int ich, int iqdis, const bool bL1QUCut)
	{
		double dQlu[2]={0.};
		if (ich<=8 || ich==26) return -1;
		if (iqdis==0)
		{
			double dQCut = ((ich>=14)?0.5:(0.0075*pow(ich,1.414)+0.198));
			dQlu[0] = ich-dQCut, dQlu[1] = ich+dQCut;
		}
		else if (iqdis==1)
		{
			dQlu[0] = ich-0.625-0.0225*(ich-9), dQlu[1] = ich+1.5;
		}
		else if (iqdis==2)
		{
			dQlu[0] = ich-0.625-0.0225*(ich-9), dQlu[1] = 9999;
		}
		else if (iqdis==4)
		{
			dQlu[0] = ich-0.0585*pow(ich,1.15)-0.35, dQlu[1] = ich+0.0334*pow(ich,1.15)+0.20;
			//if (!isreal && !bL1QUCut) dQlu[1] = 9999;
			if (!bL1QUCut) dQlu[1] = 9999;
		}
		else if (iqdis==5)
		{
			dQlu[0] = ich-0.0284*pow(ich,1.15)-0.17, dQlu[1] = ich+0.0585*pow(ich,1.15)+0.35;
		}
		
		return dQlu[ilu];
	}

	double GetQSelRangeFe(const int ilu, const int ich, const int iqdis, const bool bL1QUCut)
	{
		if (ich!=26) return -1;
		
		double dQlu[2]={0.};
		
		if (iqdis==0)
		{
			double dQCut = 0.5;
			dQlu[0] = ich-dQCut, dQlu[1] = ich+dQCut;
		}
		else if (iqdis==1)
		{
			dQlu[0] = ich-1.2, dQlu[1] = ich+1.5;
		}
		else if (iqdis==2)
		{
			dQlu[0] = ich-1.2, dQlu[1] = 9999;
		}
		else if (iqdis==4)
		{
			dQlu[0] = ich-2.83, dQlu[1] = ich+1.62;
			//if (!isreal && !bL1QUCut) dQlu[1] = 9999;
			if (!bL1QUCut) dQlu[1] = 9999;
		}
		else if (iqdis==5)
		{
			dQlu[0] = ich-1.37, dQlu[1] = ich+2.83;
		}
		
		return dQlu[ilu];
	}

	double GetQSelRangeNi(const int ilu, const int ich, const int iqdis, const bool bL1QUCut)
	{
		if (ich!=28) return -1;
		
		double dQlu[2]={GetQSelRangeTwiki(0, ich, iqdis, bL1QUCut), GetQSelRangeTwiki(1, ich, iqdis, bL1QUCut)};
		if (iqdis==QDis::UTofQ || iqdis==QDis::LTofQ) dQlu[0] = 26.8;
		
		return dQlu[ilu];
	}

	double GetQSelRange(int ilu, int ich, int iqdis, const bool bL1QUCut)
	{
		double dQCut=0;
		
		//--P He
		if (ich==1 || ich==2) dQCut = GetQSelRangePHe(ilu, ich, iqdis, bL1QUCut);
		//--3-8
		else if (ich<=8) dQCut = GetQSelRangeQ8(ilu, ich, iqdis, bL1QUCut);
		//--Iron
		else if (ich==26) dQCut = GetQSelRangeFe(ilu, ich, iqdis, bL1QUCut);
		//--Nickel
		else if (ich==28) dQCut = GetQSelRangeNi(ilu, ich, iqdis, bL1QUCut);
		//--HZ
		else dQCut = GetQSelRangeTwiki(ilu, ich, iqdis, bL1QUCut);
		
		//do not apply upper limits cut on L1Q for MC, should be ensured in each GetQSelRange functions
		//if (ilu==1 && iqdis==4 && !isreal && !bL1QUCut) dQCut=9999;
		if (ilu==1 && iqdis==4 && !bL1QUCut) dQCut=9999;
		
		//--
		return dQCut;
	}
}
#endif //QSELRANGE_H
