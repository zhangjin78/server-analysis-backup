//-----------------
//2018.06.12
//	1) define proper binning for Cutoff and Edep distribution. See detail in "rigidity estimation" work log
//	2) put the 
//-----------------

//--Cutoff
const int NCutoffRebin = 24;
double CutoffRebins[NCutoffRebin+1] = 
{0.8, 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 16., 17., 18., 19., 20., 21., 22., 23., 24., 25.3};
int CutoffBinNo[NCutoffRebin+1] = 
{8, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 253};

//--Edep
////--larger bin from 300GeV to 850GeV, 150GeV per bin
//const int NEdepRebin = 18;
//const double EdepRebins[NEdepRebin+1] = 
//{1, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 450, 600, 850};
//const int EdepBinNo[NEdepRebin+1] = 
//{1, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 450, 600, 850};

//--finer bin from 300GeV to 850GeV, 100GeV per bin
const int NEdepRebin = 20;
double EdepRebins[NEdepRebin+1] = 
{1, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 400, 500, 600, 700, 850};
int EdepBinNo[NEdepRebin+1] = 
{1, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 400, 500, 600, 700, 850};

////--finer bin from 300GeV to 850GeV, 50GeV per bin
//const int NEdepRebin = 26;
//const double EdepRebins[NEdepRebin+1] = 
//{1, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850};
//const int EdepBinNo[NEdepRebin+1] = 
//{1, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850};
