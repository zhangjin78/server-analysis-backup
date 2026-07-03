const int L1QTF_Nbin = 16; //2019.08.09: correct nbin
//const int L1QTF_Nbin = 17;
double L1QTF_Bins[L1QTF_Nbin+1] = {0.8,1.92,2.67,3.29,4.02,4.88,5.9,7.09,8.48,10.1,12,14.1,16.6,21.1,28.8,52.2,3000};

//2019.10.02
const int L1QTF_Nbin2 = 9;
double L1QTF_Bins2[L1QTF_Nbin2+1] = {0.8,1.92,3.29,4.88,7.09,10.1,14.1,21.1,52.2,3000};

const int L1QTF_NbinSmall = 5;
double L1QTF_BinsSmall[L1QTF_NbinSmall+1] = {0.8,2.15,4.02,38.9,379,3000};

/*//2020.01.21: binning for L2QTemp for all charge from 8-17, finner binning
const int L1QTF_NbinAllQ = 62;
double L1QTF_BinsAllQ[L1QTF_NbinAllQ+1] = {	0.8,1.92,
								3.29,3.64,4.02,4.43,4.88,5.37,5.90,6.47,7.09,	//10-19
								7.76,8.48,9.26,10.1,11.0,12.0,13.0,14.1,15.3,16.6,	//20-29
								18.0,19.5,21.1,22.8,24.7,26.7,28.8,31.1,33.5,36.1,	//30-39
								38.9,41.9,45.1,48.5,52.2,56.1,60.3,64.8,69.7,74.9,	//40-49
								80.5,86.5,93.0, 100.,108.,116.,125.,135.,147.,160.,	//50-59
								175.,192.,211.,233.,259.,291.,330.,379.,441.,525.,	//60-69
								660.,3000.};*/

//2020.02.01: binning for L2QTemp for all charge from 8-17, rebin to have at least 1000 events for F
/*const int L1QTF_NbinAllQ = 39;
double L1QTF_BinsAllQ[L1QTF_NbinAllQ+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.40,2.67,2.97,3.29,
3.64,4.02,4.43,4.88,5.37,5.90,6.47,7.09,7.76,8.48,
9.26,10.1,11.0,12.0,13.0,14.1,15.3,16.6,18.0,19.5,
21.1,22.8,24.7,26.7,31.1,36.1,41.9,52.2,74.9,3000
};*/
//2020.09.16: group all bins with rig>30 GV into one bin
const int L1QTF_NbinAllQ = 35;
double L1QTF_BinsAllQ[L1QTF_NbinAllQ+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.40,2.67,2.97,3.29,
3.64,4.02,4.43,4.88,5.37,5.90,6.47,7.09,7.76,8.48,
9.26,10.1,11.0,12.0,13.0,14.1,15.3,16.6,18.0,19.5,
21.1,22.8,24.7,26.7,31.1,3000
};
//2020.09.22: finner binning at high rigidity for the fit
const int L1QTF_NbinAllQFit = 40;
double L1QTF_BinsAllQFit[L1QTF_NbinAllQFit+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.40,2.67,2.97,3.29,
3.64,4.02,4.43,4.88,5.37,5.90,6.47,7.09,7.76,8.48,
9.26,10.1,11.0,12.0,13.0,14.1,15.3,16.6,18.0,19.5,
21.1,22.8,24.7,26.7,31.1,45.1,64.8,116.,233.,441.,
3000
};

//2020.09.30: finner binning at high rigidity for secondary
const int L1QTF_NbinSec = 48;
double L1QTF_BinsSec[L1QTF_NbinSec+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.40,2.67,2.97,3.29,
3.64,4.02,4.43,4.88,5.37,5.90,6.47,7.09,7.76,8.48,
9.26,10.1,11.0,12.0,13.0,14.1,15.3,16.6,18.0,19.5,
21.1,22.8,24.7,26.7,31.1,45.1,64.8,93.0,108.,125.,
147.,175.,211.,259.,330.,441.,660.,1200,3000
};

//--2020.10.24: fewer binning at lower rigidity
const int L1QTF_NbinAllQ2 = 23;
double L1QTF_BinsAllQ2[L1QTF_NbinAllQ2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,3000
};
const int L1QTF_NbinSec2 = 36;
double L1QTF_BinsSec2[L1QTF_NbinSec2+1] = {
0.80,1.33,1.51,1.71,1.92,2.15,2.67,3.29,4.02,4.88,
5.37,5.90,6.47,7.09,7.76,8.48,9.26,10.1,12.0,14.1,
18.0,22.8,31.1,45.1,64.8,93.0,108.,125.,147.,175.,
211.,259.,330.,441.,660.,1200,3000
};

//2020.06.07: binning for Iron, 
const int L1QTF_NbinFe = 17;
double L1QTF_BinsFe[L1QTF_NbinFe+1] = {
0.80,2.67,3.29,4.02,4.43,4.88,5.37,5.90,6.47,7.09,
8.48,10.1,12.0,15.3,21.1,31.1,64.8,3000
};
//2020.06.23: further rebin below 10
const int L1QTF_NbinFe2 = 12;
double L1QTF_BinsFe2[L1QTF_NbinFe2+1] = {
0.80,2.67,4.02,4.88,5.90,7.09,10.1,12.0,15.3,21.1,31.1,64.8,3000
};
//2020.07.08: merge into one charge template above 30 GV
const int L1QTF_NbinFe3 = 11;
double L1QTF_BinsFe3[L1QTF_NbinFe3+1] = {
0.80,2.67,4.02,4.88,5.90,7.09,10.1,12.0,15.3,21.1,31.1,3000
};
/*//2020.07.12: binning for Iron template fit, finner at high rig
const int L1QTF_NbinFeFit = 15;
double L1QTF_BinsFeFit[L1QTF_NbinFeFit+1] = {
0.80,2.67,4.02,4.88,5.90,7.09,10.1,12.0,15.3,21.1,31.1,45.1,64.8,160.,441.,3000
};*/
/*//2020.07.13: binning for Iron template fit, finner at high rig
const int L1QTF_NbinFeFit = 17;
double L1QTF_BinsFeFit[L1QTF_NbinFeFit+1] = {
0.80,2.67,4.02,4.88,5.90,7.09,10.1,12.0,15.3,21.1,31.1,45.1,64.8,116.,160.,233.,441.,3000
};*/
//2020.07.14: binning for Iron template fit, finner at high rig
const int L1QTF_NbinFeFit = 16;
double L1QTF_BinsFeFit[L1QTF_NbinFeFit+1] = {
0.80,2.67,4.02,4.88,5.90,7.09,10.1,12.0,15.3,21.1,31.1,45.1,64.8,116.,233.,441.,3000
};

