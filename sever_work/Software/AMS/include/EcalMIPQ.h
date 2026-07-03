//-- AMS ECAL Q Class.

struct MIPQ{
	MIPQ(){};
	~MIPQ(){};
	void    Clear(){ NPnt = 0; RMS = 1000; Mean = 0;}
	int 	NPnt;	//-- No. of point used in MIP Q calculation.
	float RMS;	//-- Combined Q rms.
	float Mean;   //-- Q Mean value.
	float dE[18]; // Table of dE, after corrections, to compute dE/dX
	float dX[18]; // Table of dX to compute dE/dX
};
