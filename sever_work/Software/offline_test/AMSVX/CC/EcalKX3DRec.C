#include "EcalKX3DRec.h"
#include "commonsi.h"

TString EcalKX3DRec::AMSDataDir = "/cvmfs/ams.cern.ch/Offline/AMSDataDir/";
Int_t EcalKX3DRec::NumberOfCellsForFit=2;

double EcalKX3DRec::GetDistance2PMT(int l, int c){
	float posfiber, dist;
	const static float hlfiber = 32.4;
	if(l%4<2) posfiber = X0 + KX*(EcalKX3DRecDB::EcalZ[l] - Z0); // Y Projection
	else      posfiber = Y0 + KY*(EcalKX3DRecDB::EcalZ[l] - Z0); // X Projection
	if( (l%4<2 && c%4<2) || (l%4>=2 && c%4>=2) ) // PMT at -32.4 cm
		dist = posfiber + hlfiber;
	else  // PMT at +32.4 cm
		dist = hlfiber - posfiber;
	return dist;
}


Int_t EcalKX3DRec::DEBUG=0;
#define DoInclinationCorrection 1
//
// const double EcalXShift[18] = { // cm, MC, need to understand
// 	0.,      0,       0.050, 0.050,
// 	-0.045, -0.045,   0.073,  0.073,
// 	0.005, 0.005,     0.095,  0.095,
// 	-0.073, -0.073,   0.270, 0.270,
// 	-0.113, -0.113
// };


EcalKX3DRec::EcalKX3DRec(){
}

void EcalKX3DRec::SetCellStat(){
	// reset all status
	memset(CellStat, 0, sizeof(CellStat));
	CellStat[6][16] = CellStat[6][17] = CellStat[7][16] = CellStat[7][17] = EcalKX3DRecDB::DeadCell;
	CellStat[14][64] = CellStat[14][65] = CellStat[15][64] = CellStat[15][65] = EcalKX3DRecDB::DeadCell;
}
void EcalKX3DRec::SetCluster(Float_t ene_cell[][72], Int_t ecal_cellstat[][72], bool IsGeV){
	Reset();
	SetCellStat();
//	SmearMC(ene_cell, 1);
	NhitSat=0;
	ElossSat=0;
	NhitMask=0;
	TotNhits=0;
	for(Int_t i=0; i<18; i++){
		for(Int_t j=0; j<4; j++){
			CellDep[i][j] = 0;
			CellFlag[i][j] = 4;
			CellMask[i][j] = 0;
			SetBit( CellMask[i][j], EcalKX3DRecDB::ImaginaryCell );
			CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = 0;
		}
		for(Int_t j=4; j<76; j++){
//			if( ene_cell[i][j-4]<=0 ) continue;
			if( !IsGeV ) // MeV
				ene_cell[i][j-4] *= 1e-3;
			// --------------
			CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = ene_cell[i][j-4];
			CellFlag[i][j] = 0;
			CellMask[i][j] = 0;
			//Set Mask for bad Cell, Ped wrong Cell and high gain Satuated Cell
			//j-4 because there is 4 fake cell., stat_cell ==1 bad, 2,Ped wrong, 4 h/l wrong
			//16 Low Gain Saturation
			if( ecal_cellstat[i][j-4] & EcalKX3DRecDB::LowGainSaturated ){
				NhitSat++;
				SetBit( CellMask[i][j], EcalKX3DRecDB::SaturatedCell );
//				cout << "L=" << i << ", C=" << j-4 << ", saturated" << endl;
			}

			if( ecal_cellstat[i][j-4] & EcalKX3DRecDB::Dummy ){ // bad cell
				SetBit( CellMask[i][j], EcalKX3DRecDB::BadCell );
//				cout << "L=" << i << ", C=" << j-4 << ", set dummy" << endl;
			}
			if( (CellStat[i][j-4] & EcalKX3DRecDB::DeadCell) // dead cell
				|| ( (ecal_cellstat[i][j-4] & EcalKX3DRecDB::HighGainSaturated) && ( CellStat[i][j-4] & (EcalKX3DRecDB::LowGainBadPedestal  | EcalKX3DRecDB::LowGainNoisyChannel  ) ) )
				|| ( !(ecal_cellstat[i][j-4] & EcalKX3DRecDB::LowGainSaturated) && ( CellStat[i][j-4] & (EcalKX3DRecDB::HighGainBadPedestal | EcalKX3DRecDB::HighGainNoisyChannel ) ) )
					){
//				cout << "L=" << i << ", C=" << j-4 << ", is bad" << endl;
				SetBit( CellMask[i][j], EcalKX3DRecDB::BadCell ); // 2 noisy PMTs
				CellDep[i][j] = 0.;
			}

			if( CellDep[i][j]>0 ){
				TotNhits++;
			}
		}
		for(Int_t j=76; j<80; j++){
			CellDep[i][j] = 0;
			CellFlag[i][j] = 4;
			CellMask[i][j] = 0;
			SetBit( CellMask[i][j], EcalKX3DRecDB::ImaginaryCell );
			CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = 0;
		}
	}
	return;
}

void EcalKX3DRec::MaskCellsAbove(double energy_threshold){
	NhitMask=0;
	for(Int_t i=0; i<18; i++){
		for(Int_t j=4; j<76; j++){
			if( CellDep[i][j] > energy_threshold ){ // for non-lieanrity study
				NhitMask++;
				SetBit( CellMask[i][j], EcalKX3DRecDB::HighAmplitudeCell );
			}
		}
	}
}

void EcalKX3DRec::MaskCellsInLast2Layer(){
	NhitMask=0;
	for(Int_t i=16; i<18; i++){
		for(Int_t j=4; j<76; j++){
			if( CellDep[i][j] > 0 ){
				NhitMask++;
				CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = 0;
				SetBit( CellMask[i][j], EcalKX3DRecDB::HighAmplitudeCell );
			}
		}
	}
}

void EcalKX3DRec::MaskCellsInLast4Layer(){
	NhitMask=0;
	for(Int_t i=14; i<18; i++){
		for(Int_t j=4; j<76; j++){
			if( CellDep[i][j] > 0 ){
			  	NhitMask++;
				CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = 0;
				SetBit( CellMask[i][j], EcalKX3DRecDB::HighAmplitudeCell );
			}
		}
	}
}

void EcalKX3DRec::SetBit(int &a, int b){
	a |= b;
}
void EcalKX3DRec::ClearBit(int &a, int b){
	a &= ~b;
}

//----- smear MC -------------
// 0. nonlinear effect
// 1. cross-talk
// 2. calibration uncertainty
//----------------------------
void EcalKX3DRec::SmearMC(Float_t ene_cell[][72], int imethod){
//	const static double ka = 1.098, kb = 0.0102;
	const static double ct_lay[18] = {
		0.010, 0.003, 0.008, 0.016, 0.022, 0.028, 0.030, 0.040, 0.043, 0.048, 0.049, 0.050, 0.056, 0.060, 0.059, 0.066, 0.066, 0.061
	};
//	const static double calib_error = 0.03;

	double ct;
	Float_t ene_cell_ct[18][72],
			  ene_cell_sm[18][72];
	memset( ene_cell_ct, 0, sizeof( ene_cell_ct ) );
	memset( ene_cell_sm, 0, sizeof( ene_cell_sm ) );
	for(int l=0; l<18; l++){
		for(int c=0; c<72; c++){
			// nonlinear effect
//			ene_cell[l][c] *= TMath::Min( 1.0, ka/(1+kb*ene_cell[l][c]) );
			// cross-talk
			ct = ct_lay[l];
			// Int_t sl = l/2,
				Int_t	pm = c/2;
//			Int_t l_n = sl*2 + 1 - l%2;
			Int_t c_n = pm*2 + 1 - c%2;
			if( imethod==0 )
			  	ene_cell_ct[l][c] = (1-ct)*ene_cell[l][c] + ct*ene_cell[l][c_n];
			else if( imethod==1 ){
				int c_m=0;
				if( c>=1 && c<=70 ){
					c_n = c+1;
					c_m = c-1;
				}else if( c==0 ){
					c_n = c_m = c+1;
				}else if( c==71 ){
					c_n = c_m = c-1;
				}
				ene_cell_ct[l][c] = (1-ct)*ene_cell[l][c] + ct/2*(ene_cell[l][c_n] + ene_cell[l][c_m]);
			}
			//	ene_cell_ct[l][c] = (1-ct)*ene_cell[l][c] + ct/2*(ene_cell[l_n][c] + ene_cell[l][c_n]);
			// gaussian smearing
//			ene_cell_sm[l][c] = gRandom->Gaus( 1, calib_error )*ene_cell_ct[l][c];
		}
	}
	// overwrite the original one
	memcpy( ene_cell, ene_cell_ct, sizeof( ene_cell_ct ) );
}

void EcalKX3DRec::Reset(){
	NonlinearityCorrectionBias=0;
	Flag_overlap=0;
	sum_nsigma_x = sum_nsigma_y = 0;
	IsHadronLike=0;
	IsSkipCombineFit=0;
	LkhdTail1=0.;
	NhitTail1=0;
	D0=D1=D2=0;
	sx = sy = 0;
	EseedX=EseedY=0;
	Index = 0;
	IsTrackerUsed = 0;
	DataType = -1; // by default, not specified
	EmBDT = -2;
	memset(EmVar,       0,    sizeof(EmVar));
	memset(EmLkhd,       0,    sizeof(EmLkhd));
	memset(Cell_Med,     0,    sizeof(Cell_Med));
	memset(Shwr_Med,     0,    sizeof(Shwr_Med));
	memset(CellStat, 		0, 	sizeof(CellStat));
	memset(CellDep, 		0, 	sizeof(CellDep));
	memset(CellDepS0, 	0, 	sizeof(CellDepS0));
	memset(CellDepS1, 	0, 	sizeof(CellDepS1));
	memset(CellExp, 		0, 	sizeof(CellExp));
	memset(CellEtr, 		0, 	sizeof(CellEtr));
	memset(CellDepth, 	0, 	sizeof(CellDepth));
	memset(CellDist, 		0, 	sizeof(CellDist));
	memset(CellBeta, 		0, 	sizeof(CellBeta));
	memset(CellProb, 		0, 	sizeof(CellProb));
	memset(CellProbNorm,	0, 	sizeof(CellProbNorm));

	memset(ShwrFlag,     0,    sizeof(ShwrFlag));
	memset(ShwrDep,      0,    sizeof(ShwrDep));
	memset(ShwrFprt,     0,    sizeof(ShwrFprt));
	memset(ShwrExp,      0,    sizeof(ShwrExp));
	memset(ShwrDepth,    0,    sizeof(ShwrDepth));
	memset(ShwrDist,     0,    sizeof(ShwrDist));
	memset(ShwrEtr,      0,    sizeof(ShwrEtr));
	memset(ShwrBeta,     0,    sizeof(ShwrBeta));
	memset(ShwrPar,      0,    sizeof(ShwrPar));
	memset(ShwrParI,     0,    sizeof(ShwrParI));
	memset(ShwrLkhd,     0,    sizeof(ShwrLkhd));
	memset(ShwrLkhdI,    0,    sizeof(ShwrLkhdI));

	memset(ShwrCBit,     0,    sizeof(ShwrCBit));
	memset(ShwrEb,       0,    sizeof(ShwrEb));
	memset(ShwrApexI,    0,    sizeof(ShwrApexI));

	memset(LayDmax,      0,    sizeof(LayDmax));
	memset(LayEmax,      0,    sizeof(LayEmax));
	memset(LayCmax,      0,    sizeof(LayCmax));

	E0 = dE0 = 0.;
	B0 = dB0 = 0.;
	T0 = dT0 = 0.;
	A0 = dA0 = 0.;
	Z0 = dZ0 = 0.;
	X0 = dX0 = 0.;
	Y0 = dY0 = 0.;
	KX = dKX = 0.;
	KY = dKY = 0.;

	N_Shwr = I_Shwr = N_Dep = 0;
	Apex = 0;

	fB0 = 1;
	Z0 = 1;
	EleEne = EneL2Cor = 0.;

	Dmax = Dmax3 = 0.;
	Emax = Emax3 = Emaxc = Emaxc3 = Emaxt = 0.;
	Lmax = Cmax = -1;
	return;
}

Float_t EcalKX3DRec::cf_ed2ei[3628800]={0};
Bool_t  EcalKX3DRec::is_cf_ed2ei_loaded=0;
Double_t EcalKX3DRec::getCorrEd2Ei(double t, double d, double c, double kx, double beta, int flag){
	//  t: shower depth, t/zmax*10
	//  d: distance to shower axis, 0 - 8
	//  c: pedal coordinates, -32.4 - 32.4
	// kx: slope in fiber plane, -0.4 - 0.4
	if( !is_cf_ed2ei_loaded ){
		cout << "Try to load correction table" << endl;
		TFile *ftmp = gFile;
		TFile *finput = new TFile(AMSDataDir + AMSCommonsI::getversion() + "/EcalKX/attpar_ed2ei_20140915.root");
		if( finput == NULL ){
			cerr << "No Correction Table, Exit" << endl;
			exit(-1);
		}
		TTree *tree = dynamic_cast<TTree*>(finput->Get("attpar"));
		tree->SetBranchAddress("cf", cf_ed2ei);
		tree->GetEntry(0);
		is_cf_ed2ei_loaded=1;
		finput->Close();
		if(ftmp) ftmp->cd();
	}
	int i1, i2, i3, i4,
		 j1, j2, j3, j4;
	if( t <= 0 ){
		i1 = j1 = 0;
	} else if( t>=2.9 ){
		i1 = j1 = 29;
	} else{
		i1 = int(t*10);
		j1 = i1 + 1;
	}

	if( d >= 7.8 ){
		i2 = j2 = 39;
	} else {
		i2 = int(5*d);
		j2 = i2 + 1;
	}

	if( c<= -31.95){
		i3 = j3 = 0;
	} else if(c>=31.95){
		i3 = j3 = 143;
	} else{
		i3 = int((c+32.4)/0.45);
		j3 = i3 + 1;
	}

	if( kx <= -0.4 ){
		i4 = j4 = 0;
	} else if( kx >= 0.4 ){
		i4 = j4 = 20;
	} else{
		i4 = int( 25*kx+10 );
		j4 = i4+1;
	}

	int n1 = i1*120960 + i2*3024 + i3*21 + i4;
	int m1 = j1*120960 + j2*3024 + j3*21 + j4;

	double di1, di2, di3, di4;
	double dj1, dj2, dj3, dj4;
	// derivatives for depth t
	if ( i1 == j1 ) {
		if( i1==0 )
			di1 = dj1 = 0;//(10*t-i1)*(cf_ed2ei[n1+120960] - cf_ed2ei[n1]);
		else
			di1 = dj1 = 0;//(10*t-i1)*(cf_ed2ei[n1] - cf_ed2ei[n1-120960]);
	} else {
		di1 = (10*t-i1)*(cf_ed2ei[n1+120960] - cf_ed2ei[n1]);
		dj1 = (10*t-j1)*(cf_ed2ei[m1] - cf_ed2ei[m1-120960]);
	}
	// derivatives for distance d
	if ( i2 == j2 ) {
		di2 = dj2 = 0.;//(5*d - i2)*(cf_ed2ei[n1] - cf_ed2ei[n1-2877]);
	} else {
		di2 = (5*d-i2)*(cf_ed2ei[n1+3024] - cf_ed2ei[n1]);
		dj2 = (5*d-j2)*(cf_ed2ei[m1] - cf_ed2ei[m1-3024]);
	}
	// derivatives for coordinate c
	if ( i3 == j3 ) {
		di3 = dj3 = 0.;
	} else {
		di3 = ((c+32.4)/0.45-i3)*(cf_ed2ei[n1+21] - cf_ed2ei[n1]);
		dj3 = ((c+32.4)/0.45-j3)*(cf_ed2ei[m1] - cf_ed2ei[m1-21]);
	}
	// derivatives for tangent kx
	if ( i4 == j4 ) {
		di4 = dj4 = 0.;
	} else {
		di4 = (25*kx+10-i4)*(cf_ed2ei[n1+1] - cf_ed2ei[n1]);
		dj4 = (25*kx+10-j4)*(cf_ed2ei[m1] - cf_ed2ei[m1-1]);
	}

	double dist0 = di1*di1 + di2*di2 + di3*di3 + di4*di4,
			 dist1 = dj1*dj1 + dj2*dj2 + dj3*dj3 + dj4*dj4;
	double par0;
	if( dist1 + dist0 > 0 )
		par0 = (dist1*(cf_ed2ei[n1] + di1 + di2 + di3 + di4) + dist0*(cf_ed2ei[m1] + dj1 + dj2 + dj3 + dj4))/(dist0+dist1);
	else
		par0 = cf_ed2ei[n1];

	if( flag > 0 || par0<=0 || TMath::IsNaN(par0) ){
		cout << "===========================================" << endl;
		cout << Form("t %f, d %f, c %f, kx %f", t, d, c, kx) << endl;
		cout << Form("Index 0: %d %d %d %d", i1, i2, i3, i4) << endl;
		cout << Form("Index 1: %d %d %d %d", j1, j2, j3, j4) << endl;
		cout << Form("Base Values: %d %f %d %f", n1, cf_ed2ei[n1], m1, cf_ed2ei[m1])<<endl;
		printf("di3: cf_ed2ei[%d]=%g, cf_ed2ei[%d]=%g\n", n1+21, cf_ed2ei[n1+21], m1-21, cf_ed2ei[m1-21]);
		printf("di4: cf_ed2ei[%d]=%g, cf_ed2ei[%d]=%g\n", n1+1, cf_ed2ei[n1+1], m1-1, cf_ed2ei[m1-1]);
		cout << Form("Value 0: %f %f %f %f %f %f %f", cf_ed2ei[n1], di1, di2, di3, di4, dist1/(dist0+dist1), cf_ed2ei[n1] + di1 + di2 + di3 + di4 )<<endl;
		cout << Form("Value 1: %f %f %f %f %f %f %f", cf_ed2ei[m1], dj1, dj2, dj3, dj4, dist0/(dist0+dist1), cf_ed2ei[m1] + dj1 + dj2 + dj3 + dj4 )<<endl;
		cout << Form("Correction: %f ", par0)<<endl;
		par0 = 0.5*(cf_ed2ei[n1] + cf_ed2ei[m1]);
	}

	return par0;
}


Float_t EcalKX3DRec::cf_es2ed[3628800]={0};
Bool_t  EcalKX3DRec::is_cf_es2ed_loaded=0;
Double_t EcalKX3DRec::getCorrEs2Ed(double t, double d, double c, double kx, double beta, int flag){
	// correction for attenuation and leakage along the fiber
	//  t: shower depth, t/zmax*10
	//  d: distance to shower axis, 0 - 8
	//  c: pedal coordinates, -32.4 - 32.4
	// kx: slope in fiber plane, -0.4 - 0.4
	if( !is_cf_es2ed_loaded ){
		cout << "Try to load correction table" << endl;
		TFile *ftmp = gFile;
		TFile *finput = new TFile(AMSDataDir + AMSCommonsI::getversion() + "/EcalKX/attpar_es2ed_20140915.root");
		if( finput == NULL ){
			cerr << "No Correction Table, Exit" << endl;
			exit(-1);
		}
		TTree *tree = dynamic_cast<TTree*>(finput->Get("attpar"));
		tree->SetBranchAddress("cf", cf_es2ed);
		tree->GetEntry(0);
		is_cf_es2ed_loaded=1;
		finput->Close();
		if(ftmp) ftmp->cd();
	}
	int i1, i2, i3, i4,
		 j1, j2, j3, j4;
	if( t <= 0 ){
		i1 = j1 = 0;
	} else if( t>=2.9 ){
		i1 = j1 = 29;
	} else{
		i1 = int(t*10);
		j1 = i1 + 1;
	}

	if( d >= 7.8 ){
		i2 = j2 = 39;
	} else {
		i2 = int(5*d);
		j2 = i2 + 1;
	}

	if( c<= -31.95){
		i3 = j3 = 0;
	} else if(c>=31.95){
		i3 = j3 = 143;
	} else{
		i3 = int((c+32.4)/0.45);
		j3 = i3 + 1;
	}

	if( kx <= -0.4 ){
		i4 = j4 = 0;
	} else if( kx >= 0.4 ){
		i4 = j4 = 20;
	} else{
		i4 = int( 25*kx+10 );
		j4 = i4+1;
	}

	int n1 = i1*120960 + i2*3024 + i3*21 + i4;
	int m1 = j1*120960 + j2*3024 + j3*21 + j4;

	double di1, di2, di3, di4;
	double dj1, dj2, dj3, dj4;
	// derivatives for depth t
	if ( i1 == j1 ) {
		di1 = dj1 = 0;//(t*10-i1)*(cf_es2ed[n1+120960] - cf_es2ed[n1]);
	} else {
		di1 = (t*10-i1)*(cf_es2ed[n1+120960] - cf_es2ed[n1]);
		dj1 = (t*10-j1)*(cf_es2ed[m1] - cf_es2ed[m1-120960]);
	}
	// derivatives for distance d
	if ( i2 == j2 ) {
		di2 = dj2 = 0.;//(5*d - i2)*(cf_es2ed[n1] - cf_es2ed[n1-2877]);
	} else {
		di2 = (5*d-i2)*(cf_es2ed[n1+3024] - cf_es2ed[n1]);
		dj2 = (5*d-j2)*(cf_es2ed[m1] - cf_es2ed[m1-3024]);
	}
	// derivatives for coordinate c
	if ( i3 == j3 ) {
		di3 = dj3 = 0.;
	} else {
		di3 = ((c+32.4)/0.45-i3)*(cf_es2ed[n1+21] - cf_es2ed[n1]);
		dj3 = ((c+32.4)/0.45-j3)*(cf_es2ed[m1] - cf_es2ed[m1-21]);
	}
	// derivatives for tangent kx
	if ( i4 == j4 ) {
		di4 = dj4 = 0.;
	} else {
		di4 = (25*kx+10-i4)*(cf_es2ed[n1+1] - cf_es2ed[n1]);
		dj4 = (25*kx+10-j4)*(cf_es2ed[m1] - cf_es2ed[m1-1]);
	}

	double dist0 = di1*di1 + di2*di2 + di3*di3 + di4*di4,
			 dist1 = dj1*dj1 + dj2*dj2 + dj3*dj3 + dj4*dj4;
	double par0;
	if( dist1 + dist0 > 0 )
		par0 = (dist1*(cf_es2ed[n1] + di1 + di2 + di3 + di4) + dist0*(cf_es2ed[m1] + dj1 + dj2 + dj3 + dj4))/(dist0+dist1);
	else
		par0 = 0.5*(cf_es2ed[n1]+cf_es2ed[m1]);
	if( flag > 0 || par0<=0 || TMath::IsNaN(par0)){
		cout << "================ getCorrEs2Ed ===========================" << endl;
		cout << Form("t %f, d %f, c %f, kx %f", t, d, c, kx) << endl;
		cout << Form("Index 0: %d %d %d %d ", i1, i2, i3, i4) << endl;
		cout << Form("Index 1: %d %d %d %d ", j1, j2, j3, j4) << endl;
		cout << Form("Base Values: %d %d %f %f", n1, m1, cf_es2ed[n1], cf_es2ed[m1])<<endl;
		cout << Form("Value 0: %f %f %f %f %f %f %f ", cf_es2ed[n1], di1, di2, di3, di4, dist1/(dist0+dist1), cf_es2ed[n1] + di1 + di2 + di3 + di4 )<<endl;
		cout << Form("Value 1: %f %f %f %f %f %f %f ", cf_es2ed[m1], dj1, dj2, dj3, dj4, dist0/(dist0+dist1), cf_es2ed[m1] + dj1 + dj2 + dj3 + dj4 )<<endl;
		cout << Form("Correction: %f ", par0)<<endl;
	}

	return par0;
}

Float_t EcalKX3DRec::cf_ky_es2ed[25200] = {0};
Bool_t  EcalKX3DRec::is_cf_ky_es2ed_loaded = 0;
Double_t EcalKX3DRec::getKyCorrEs2Ed(Double_t t, Double_t d, Double_t kx, Double_t ky, Int_t flag){
	// correction for the different shower depth along the fiber in case of inlcined shower
	//  t: shower depth, t/zmax*10
	//  d: distance to shower axis, 0 - 8
	// kx: slope in fiber plane, -0.4 - 0.4
	if( !is_cf_ky_es2ed_loaded ){
		cout << "Try to load correction table" << endl;
		TFile *ftmp = gFile;
		TFile *finput = new TFile(AMSDataDir + AMSCommonsI::getversion() + "/EcalKX/es2ed_kycorr_20140915.root");
		if( finput == NULL ){
			cerr << "No Correction Table, Exit" << endl;
			exit(-1);
		}
		TTree *tree = dynamic_cast<TTree*>(finput->Get("t"));
		tree->SetBranchAddress("cf", cf_ky_es2ed);
		tree->GetEntry(0);
		is_cf_ky_es2ed_loaded=1;
		finput->Close();
		if(ftmp) ftmp->cd();
	}
	int i1, i2, i3,
		 j1, j2, j3;
	if( t <= 0 ){
		i1 = j1 = 0;
	} else if( t>=2.9 ){
		i1 = j1 = 29;
	} else{
		i1 = int(t*10);
		j1 = i1 + 1;
	}

	if( d >= 7.8 ){
		i2 = j2 = 39;
	} else {
		i2 = int(5*d);
		j2 = i2 + 1;
	}

	if( kx <= -0.4 ){
		i3 = j3 = 0;
	} else if( kx >= 0.4 ){
		i3 = j3 = 20;
	} else{
		i3 = int( 25*kx+10 );
		j3 = i3+1;
	}

	int n1 = i1*840 + i2*21 + i3;
	int m1 = j1*840 + j2*21 + j3;

	double di1, di2, di3;
	double dj1, dj2, dj3;
	// derivatives for depth t
	if ( i1 == j1 ) {
		if( i1==0 )
			di1 = dj1 = (t*10-i1)*(cf_ky_es2ed[n1+840] - cf_ky_es2ed[n1]);
		else
			di1 = dj1 = (t*10-i1)*(cf_ky_es2ed[n1] - cf_ky_es2ed[n1-840]);
	} else {
		di1 = (t*10-i1)*(cf_ky_es2ed[n1+840] - cf_ky_es2ed[n1]);
		dj1 = (t*10-j1)*(cf_ky_es2ed[m1] - cf_ky_es2ed[m1-840]);
	}
	// derivatives for distance d
	if ( i2 == j2 ) {
		di2 = dj2 = 0.;
	} else {
		di2 = (5*d-i2)*(cf_ky_es2ed[n1+21] - cf_ky_es2ed[n1]);
		dj2 = (5*d-j2)*(cf_ky_es2ed[m1] - cf_ky_es2ed[m1-21]);
	}
	// derivatives for tangent kx
	if ( i3 == j3 ) {
		di3 = dj3 = 0.;
	} else {
		di3 = (25*kx+10-i3)*(cf_ky_es2ed[n1+1] - cf_ky_es2ed[n1]);
		dj3 = (25*kx+10-j3)*(cf_ky_es2ed[m1] - cf_ky_es2ed[m1-1]);
	}

	double dist0 = di1*di1 + di2*di2 + di3*di3,
			 dist1 = dj1*dj1 + dj2*dj2 + dj3*dj3;
	double par0;
	if( dist0 + dist1 > 0 )
		par0 = (dist1*(cf_ky_es2ed[n1] + di1 + di2 + di3) + dist0*(cf_ky_es2ed[m1] + dj1 + dj2 + dj3))/(dist0+dist1);
	else
		par0 = cf_ky_es2ed[n1];
	if( flag > 0 ){
		cout << "===========================================" << endl;
		cout << Form("t %f, d %f, kx %f", t, d, kx) << endl;
		cout << Form("Index 0: %d %d %d ", i1, i2, i3) << endl;
		cout << Form("Index 1: %d %d %d ", j1, j2, j3) << endl;
		cout << Form("Base Values: %d %d %f %f", n1, m1, cf_ky_es2ed[n1], cf_ky_es2ed[m1])<<endl;
		cout << Form("Value 0: %f %f %f %f %f %f ", cf_ky_es2ed[n1], di1, di2, di3, dist1/(dist0+dist1), cf_ky_es2ed[n1] + di1 + di2 + di3 )<<endl;
		cout << Form("Value 1: %f %f %f %f %f %f ", cf_ky_es2ed[m1], dj1, dj2, dj3, dist0/(dist0+dist1), cf_ky_es2ed[m1] + dj1 + dj2 + dj3 )<<endl;
		cout << Form("Correction: %f ", par0)<<endl;
	}

	return TMath::Max( 1+par0*ky*ky, 0.1 );
}

/******************************************************************/
/**********                                            ************/
/**********   Shower shape and fluctuation parameters  ************/
/**********                                            ************/
/******************************************************************/
void EcalKX3DRec::SetPdfPar(Int_t ish, Double_t ene){
	if( ish > 2 ){
		cerr << "--EcalKX3DRec--Error--SetPdfPar--:Exceed maximum number of showers" << endl;
		return ;
	};

	double p0_0 = 78.8 - 7.15*log(ene+0.1);
	if(p0_0<5) p0_0 = 5; // 20TeV
	double p0_1 = 2.3 + 4.3*exp(-ene/2.5);
	for(int i=0; i<30; i++){
		double t = 0.1*i;
		P0[ish][i] = p0_0*(1+p0_1*exp(-t/0.5));
		P1[ish][i] = 1.85 + 0.35*atan(10*(t-1.25));
	}
}

void EcalKX3DRec::SetLatPar(Int_t ish, Double_t ene){
	if( ish > 2 ){
		cerr << "--EcalKX3DRec--Error--SetLatPar--:Exceed maximum number of showers" << endl;
		return ;
	};
	// will be called at the beginning of fitting
	// the parameters will be fixed to speed up
	double logene = log(ene+0.1);
	if(logene<0) logene=0;

	double qc_p0 = 2.41 - 0.02*(logene-5),
			 qc_p1 = 0.6,
			 qc_p2 = 0.900 - 0.024*(logene-5),

			 rc_p0 = -0.0205 + 0.0023*(logene-5),
			 rc_p1 = 0.37,

			 rt_p0 = 0.97,
			 rt_p1 = 0.51,
			 rt_p2 = -2.6,
			 rt_p3 = 0.683 + 0.023*(logene-5);
//		rt_p3 = 0.64 + 0.01*(logene-5); // tune the DepTail/ExpTail ~ 1
		// MC shape narrower than data
	if( DataType==EcalKX3DRecDB::_MC_DATA ){
//			rc_p1 *= 0.92; qc_p0 *= 1.01;  // simply scale the shower shape from data
			// tuned based on MC B1091
		qc_p0 = 2.424 - 0.0103*(logene-5);
		qc_p1 = 0.6;
		qc_p2 = 0.925 - 0.0132*(logene-5);

		rc_p0 = -0.02 + 0.0023*(logene-5);
		rc_p1 = 0.34;

		rt_p0 = 1.0;
		rt_p1 = 0.51;
		rt_p2 = -2.6;
		rt_p3 = 0.621 + 0.019*(logene-5);
	}

	if( qc_p0<1.0 ) qc_p0=1.0;
	if( qc_p1<0.1 ) qc_p1=0.1;
	if( rt_p0<0.1 ) rt_p0=0.1;
	for(int i=0; i<30; i++){
		double t = 0.1*i;
		Fc[ish][i] = qc_p0*exp( (qc_p1-t)/qc_p2 - exp((qc_p1-t)/qc_p2) );
		Rc[ish][i] = rc_p0 + rc_p1*t + 0.2*exp(-t/0.2);
		Rt[ish][i] = rt_p0*( exp( rt_p2*(t-rt_p1) ) + exp( rt_p3*(t-rt_p1) ) );

		if( Rt[ish][i] > 10 ) Rt[ish][i] = 10.;
	}
	return;
}

void EcalKX3DRec::CellRatioParMC(double ene, double &par0, double &par1){
	double p_0[3] = {-6.68, 360, 0.124};
	double p_1[3] = {0.145, 0.378, 1.71};
	double log10ene = log10(ene);
	if( log10ene < 1.2 ) log10ene = 1.2;
	par0 = p_0[0]+p_0[1]*exp(-log10ene/p_0[2]);
	par1 = p_1[0]+p_1[1]*exp(-log10ene/p_1[2]);
}

void EcalKX3DRec::CellRatioParToyMC(double ene, double &par0, double &par1){
	double p_0[3] = {-6.54, 1.75, 1.16};
	double p_1[3] = {0.164, 0.328, 0.714};
	double log10ene = log10(ene);
	if( log10ene < 1.2 ) log10ene = 1.2;
	par0 = p_0[0]+p_0[1]*exp(-log10ene/p_0[2]);
	par1 = p_1[0]+p_1[1]*exp(-log10ene/p_1[2]);
}

void EcalKX3DRec::CellRatioParISS(double ene, double &par0, double &par1){
	double p_0[3] = {-7.669,  7.828, 0.132  };
	double p_1[3] = { 0.539, -0.480, -0.0551};
	if( ene < 2 ) ene = 2.;
	par0 = p_0[0] + p_0[1]/(ene+0.1) + p_0[2]*log(ene+1);
	par1 = p_1[0] + p_1[1]/(ene+0.1) + p_1[2]*log(ene+1);
}


//
/****************************************************************************
 *                                                                          *
 *             Estimate initial shower parameters for a given               *
 *             energy depositions in the ECAL                               *
 *                                                                          *
 *               Input: - ecal_cell_dep[18][72] map of energy depositions   *
 *                                                                          *
 *               Output: - X0, dX0, Y0, dY0, Z0, dZ0, KX, dKY, KY, dKY      *
 *                         shower axis and corresponding errors             *
 *                         E0, dE0, B0, dB0, T0, dT0              *
 *                         shower parameters and errors                     *
 *               Author:   AK                                               *
 *                                                                          *
 ****************************************************************************/
Int_t EcalKX3DRec::ShowerEstimate(){
	ShowerClustering(); // pattern recognition
	CalcCoG(0);    //
	ParabolaZ0(); // Z0
	ShowerAxis(); // KX,X0,KY,Y0
	DefineFootprint(); // T0, B0, E0
/*
	CalcCoG(1);    // Use CellRatio
	ParabolaZ0(); // Z0
	ShowerAxis(); // KX,KY,X0,Y0
	DefineFootprint(); // T0, B0
*/
//	Shower1DFit_AK();

	return XSeed+YSeed;
}

Int_t EcalKX3DRec::ShowerClustering(){
	Double_t v, val;
	Int_t lx, mx, ly, my, nm, nl, ncl, nm0;
	Int_t l, m;
	Int_t nw=0;
	SeedLayerX = SeedLayerY = 0;
	//
	// maximal amplitudes in Y-projection
	//
define_mY:
	ly = 0;
	my = 0;
	nl = 0;
	v  = EcalKX3DRecDB::Cutoff_MIP;
	for (l=0; l<18; l++) {
		if( l%4>=2 ) continue;
		SeedCell[l] = 0;
		for (m=4; m<76; m++) {
			if ( CellDepS1[l][m]>v ) {
				v = CellDepS1[l][m];
				ly = l;
				my = m;
			}
		}
	}

	// consistency of the found Y maximum
	if ( ly<=1 || ly==17) nm = nm0 = 2;
	else                  nm = nm0 = 0;
	val=0.;
	if ( my > 0 ) {
		val = 0.;
		for (l=0; l<18; l+=4) {
			//      printf("LY %02d, MY %02d, E-1 %8.3f, E %8.3f, E+1 %8.3f\n", l, my, CellDepS1[l][my-1], CellDepS1[l][my], CellDepS1[l][my+1]);
			//      printf("LY %02d, MY %02d, E-1 %8.3f, E %8.3f, E+1 %8.3f\n", l+1, my, CellDepS1[l+1][my-1], CellDepS1[l+1][my], CellDepS1[l+1][my+1]);
			if( l/2 == ly/2 ) nw = 2;
			else              nw = 3;
			//if ( l>=(ly-5) && l<=(ly+5) )
			{
				ncl=0;
				for (m=TMath::Max(3,my-nw); m<=TMath::Min(76,my+nw); m++) {
					if ( (l != ly) || (m != my) ) {
						if ( CellDepS1[l][m]>0. ) {
							nm  += 1;
							ncl ++;
							val += CellDepS1[l][m];
						}
						if ( ((l != ly) || (m != my)) && CellDepS0[l][m]>0. ) {
							nm0++;
						}
					}
				}
				if( ncl > 0 ) nl++;
				if(DEBUG)cout << "l=" << l << ", nm=" << nm << ", nm0=" << nm0 << ", ncl=" << ncl << ", nl=" << nl << endl;
			}
			//if ( (l+1)>=(ly-5) && (l+1)<=(ly+5) )
			{
				ncl=0;
				for (m=TMath::Max(3,my-nw); m<=TMath::Min(76,my+nw); m++) {
					if ( (((l+1) != ly) || (m != my)) && CellDepS1[l+1][m]>0. ) {
						nm  += 1;
						ncl ++;
						val += CellDepS1[l+1][m];
					}
					if ( (((l+1) != ly) || (m != my)) && CellDepS0[l+1][m]>0. ) {
						nm0++;
					}
				}
				if( ncl > 0 ) nl++;
				if(DEBUG)cout << "l=" << l+1 << ", nm=" << nm << ", nm0=" << nm0 << ", ncl=" << ncl << ", nl=" << nl << endl;
			}
		}
		//    printf("LY %02d, MY %02d, Nhits %02d, Emax %8.3f, Eout %8.3f\n", ly, my, nm, CellDepS1[ly][my], val);
		if ((nl < 3) || (v > 2*val) || nm < 6 || CellDepS1[ly][my-1]+CellDepS1[ly][my+1]<=0.001 ) { // bad seed
			CellDepS1[ly][my] = EcalKX3DRecDB::Cutoff_MIP;
			if(DEBUG){
				printf("-->ShowerClustering(): set L=%d,C=%d to MIP, nl=%d, nm=%d, v=%8.3f, val=%8.3f, e+/-1=%8.3f\n", ly, my, nl, nm, v, val, CellDepS1[ly][my-1]+CellDepS1[ly][my+1]);
			}
			goto define_mY;
		}
	}
	SeedLayerY = ly;
	fEY = nm;
	fEY0 = nm0;
	if( DEBUG ){
		printf("-->ShowerClustering(): seed found ly=%d, my=%d, v=%g, val=%g, nl=%d, nm=%d\n", ly, my, v, val, nl, nm);
	}

	// define Y footprint
	if ( my > 0 ) {
		dEY = v;
		// go upstream
		nm = my;
		for (l=(ly&0xFE)+1; l>0; l-=4) {
			v = 0.;
			nw = 3;
			for (m=my-nw; m<=my+nw; m++) {
				if ( CellDepS1[l][m]>v && (CellDepS0[l][m-1]+CellDepS0[l][m+1])>0 ) { // high quality seed first
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}
			if( SeedCell[l]<=0 ){
				for(m=my-nw; m<=my+nw; m++){ // the single cell seed
					if ( CellDepS1[l][m]>v ){
						v = CellDepS1[l][m];
						SeedCell[l] = m;
					}
				}
			}
			if ( SeedCell[l] > 0 && (CellDepS1[l][SeedCell[l]-1]>0 || CellDepS1[l][SeedCell[l]+1]>0) ){
				my = SeedCell[l];
				nw = 2; // smaller window since they are in the same super layer, larger winwdow when going upstream to allow more flucutation
			}
			if(DEBUG){
				printf("l=%2d, v=%6.3f, SeedCell=%d, nw=%d\n", l, v, SeedCell[l], nw);
			}

			v = 0.;
			for (m=my-nw; m<=my+nw; m++) {
				if ( CellDepS1[l-1][m]>v && (CellDepS0[l-1][m-1]+CellDepS0[l-1][m+1])>0 ) { // high quality seed first
					v = CellDepS1[l-1][m];
					SeedCell[l-1] = m;
				}
			}
			if( SeedCell[l-1]<=0 ){
				for(m=my-nw; m<=my+nw; m++){ // the single cell seed
					if ( CellDepS1[l-1][m]>v ){
						v = CellDepS1[l-1][m];
						SeedCell[l-1] = m;
					}
				}
			}

			if(DEBUG){
				printf("l=%2d, v=%6.3f, SeedCell=%d, nw=%d\n", l-1, v, SeedCell[l-1], nw);
			}
			if ( SeedCell[l-1] > 0 && (CellDepS1[l-1][SeedCell[l-1]-1]>0 || CellDepS1[l-1][SeedCell[l-1]+1]>0) )  my = SeedCell[l-1];
			if( SeedCell[l]>0 && SeedCell[l-1]>0 && TMath::Abs(SeedCell[l]-SeedCell[l-1])>3 ) SeedCell[l] = SeedCell[l-1] = 0;
		}

		// go downstream
		my = nm;
		for (l=(ly&0xFE)+4; l<18; l+=4) {
			v = 0.;
			nw = 3;
			for (m=my-nw; m<=my+nw; m++) {
				if ( CellDepS1[l][m]>v && (CellDepS0[l][m-1] + CellDepS0[l][m+1])>0 ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}
			if( SeedCell[l]<=0 ){
				for (m=my-nw; m<=my+nw; m++) {
					if ( CellDepS1[l][m]>v ){
						v = CellDepS1[l][m];
						SeedCell[l] = m;
					}
				}
			}
			if ( SeedCell[l] > 0 && (CellDepS1[l][SeedCell[l]-1] + CellDepS1[l][SeedCell[l]+1]) >0 ){
				my = SeedCell[l];
				nw = 1; // smaller window since they are in the same super layer
			}

			v = 0.;
			for (m=my-nw; m<=my+nw; m++) {
				if ( CellDepS1[l+1][m]>v && (CellDepS0[l+1][m-1] + CellDepS0[l+1][m+1])>0 ) { // high quality seed first
					v = CellDepS1[l+1][m];
					SeedCell[l+1] = m;
				}
			}
			if( SeedCell[l+1]<=0 ){ // single cell seed
				for (m=my-nw; m<=my+nw; m++) {
					if ( CellDepS1[l+1][m]>v ){
						v = CellDepS1[l+1][m];
						SeedCell[l+1] = m;
					}
				}
			}
			if ( SeedCell[l+1] > 0 && (CellDepS1[l+1][SeedCell[l+1]-1]>0 || CellDepS1[l+1][SeedCell[l+1]+1]>0) )  my = SeedCell[l+1];
		}
		my = nm;                                            // restore my
	} else {
		dEY = 0.;
	}

	// seed layer in X projection
define_mX:
	v  = EcalKX3DRecDB::Cutoff_MIP;// maximum energy
	lx = 0; // lay
	mx = 0; // cell
	nl = 0;
	for (l=0; l<18; l++) {
		if( l%4 < 2 ) continue;
		SeedCell[l] = 0;
		for (m=4; m<76; m++) {
			if ( CellDepS1[l][m]>v ) {
				v = CellDepS1[l][m];
				lx = l;
				mx = m;
			}
		}
	}
	// check the consistence of the found X seed
	if ( lx==2 || lx==15) nm = nm0 = 1;
	else                  nm = nm0 = 0;
	if ( mx > 0 ) {
		val = 0.;
		for (l=2; l<16; l+=4) {
			if( l/2 == lx/2 ) nw = 2;
			else              nw = 3;
			//if ( l>(lx-5) && l<(lx+5) )
			{
				ncl=0;
				for (m=TMath::Max(3,mx-nw); m<=TMath::Min(76,mx+nw); m++) {
					if ( ((l != lx) || (m != mx)) && CellDepS1[l][m]>0. ) {
						nm  += 1;
						ncl ++;
						val += CellDepS1[l][m];
					}
					if ( ((l != lx) || (m != mx)) && CellDepS0[l][m]>0. ) {
						nm0++;
					}
				}
				if( ncl > 0 ) nl++;
				if(DEBUG)cout << "l=" << l << ", nm=" << nm << ", nm0=" << nm0 << ", ncl=" << ncl << ", nl=" << nl << endl;
			}
			//if ( (l+1)>(lx-5) && (l+1)<(lx+5) )
			{
				ncl=0;
				for (m=TMath::Max(3,mx-nw); m<=TMath::Min(76,mx+nw); m++) {
					if ( (((l+1) != lx) || (m != mx)) && CellDepS1[l+1][m]>0. ) {
						nm  += 1;
						ncl ++;
						val += CellDepS1[l+1][m];
					}
					if ( (((l+1) != lx) || (m != mx)) && CellDepS0[l+1][m]>0. ) {
						nm0++;
					}
				}
				if( ncl > 0 ) nl++;
				if(DEBUG)cout << "l=" << l+1 << ", nm=" << nm << ", nm0=" << nm0 << ", ncl=" << ncl << ", nl=" << nl << endl;
			}
		}
		if ((nl < 3) || (v > 1.5*val) || nm < 6 || (ly>0 && TMath::Abs(lx-ly)>5) || CellDepS1[ly][my-1]+CellDepS1[ly][my+1]<=0.001 ){ // bad seed
			CellDepS1[lx][mx] = EcalKX3DRecDB::Cutoff_MIP;
			if(DEBUG){
				printf("-->ShowerClustering(): set L=%d,C=%d to MIP, nl=%d, nm=%d, v=%8.3f, val=%8.3f\n", lx, mx, nl, nm, v, val);
			}
			goto define_mX;
		}
	}
	SeedLayerX = lx;
	fEX = nm; // matched cells +/- 5 layers and +/- nw cells around the seed cell
	fEX0 = nm0;
	if( DEBUG ){
		printf("-->ShowerClustering(): seed found lx=%d, mx=%d, v=%g, val=%g, nl=%d, nm=%d\n", lx, mx, v, val, nl, nm);
	}

	// define X footprint
	if ( mx > 0 ) {
		dEX = v;
		// go upstream
		nm = mx;
		for (l=(lx&0xFE)+1; l>1; l-=4) {
			v = 0.;
			nw = 3;
			for (m=mx-nw; m<=mx+nw; m++) {
				if ( CellDepS1[l][m]>v ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l] > 0 && (CellDepS1[l][SeedCell[l]-1]>0 || CellDepS1[l][SeedCell[l]+1]>0) ){
			  	mx = SeedCell[l];
				nw = 1; // smaller window since they are in the same super layer
			}

			v = 0.;
			for (m=mx-nw; m<=mx+nw; m++) {
				if ( CellDepS1[l-1][m]>v ) {
					v = CellDepS1[l-1][m];
					SeedCell[l-1] = m;
				}
			}
			if ( SeedCell[l-1] > 0 && (CellDepS1[l-1][SeedCell[l-1]-1]>0 || CellDepS1[l-1][SeedCell[l-1]+1]>0) )  mx = SeedCell[l-1];
		}

		// go downstream
		mx = nm;
		for (l=(lx&0xFE)+4; l<16; l+=4) {
			v = 0.;
			nw = 3;
			for (m=mx-nw; m<=mx+nw; m++) {
				if ( CellDepS1[l][m]>v ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l] > 0 && (CellDepS1[l][SeedCell[l]-1]>0 || CellDepS1[l][SeedCell[l]+1]>0) ){
			  	mx = SeedCell[l];
				nw = 1; // smaller window since they are in the same super layer
			}

			v = 0.;
			ncl = 0;
			for (m=mx-nw; m<=mx+nw; m++) {
				if ( CellDepS1[l+1][m]>v ) {
					v = CellDepS1[l+1][m];
					SeedCell[l+1] = m;
					ncl++;
				}
			}

			if ( SeedCell[l+1] > 0 && (CellDepS1[l+1][SeedCell[l+1]-1]>0 || CellDepS1[l+1][SeedCell[l+1]+1]>0) )  mx = SeedCell[l+1];
		}
		mx = nm;                                            // restore mx
	} else {
		dEX = 0.;
	}

	XSeed = mx; // to be used in DefineFootprint
	YSeed = my;
	return 0;
}

Int_t EcalKX3DRec::CalcCoG(Int_t flag){
	Int_t l, nm=0;
	Double_t lrrpar, cr;

	if( flag ){ // use cell-ratio
		CellRatioParISS(E0, fLrrPar0, fLrrPar1);
	}

	fX0 = fX1 = 0;
	fY0 = fY1 = 0;

	for (l=0; l<18; l++) {
		nm = SeedCell[l];
		//if(DEBUG) cout << "l=" << l << ", nm=" << nm << endl;
		if ( nm > 0 ) {
			LayAmp[l] = CellDepS1[l][nm-1] + CellDepS1[l][nm] + CellDepS1[l][nm+1];

			if( flag && nm > 4 && nm <= 74 && CellDepS1[l][nm-1]>0&&CellDepS1[l][nm+1]>0){
				cr = TMath::Log(CellDepS1[l][nm-1]/CellDepS1[l][nm+1]);
				lrrpar = fLrrPar0 + fLrrPar1*TMath::Max(3., l*sqrt(1+KX*KX+KY*KY));
				LayCrd[l] = (cr/lrrpar + (nm-39.5) )*EcalKX3DRecDB::Cell_Size;
			}else
				LayCrd[l] = EcalKX3DRecDB::Cell_Size*((nm-40.5)*CellDepS1[l][nm-1] + (nm-39.5)*CellDepS1[l][nm] + (nm-38.5)*CellDepS1[l][nm+1])/LayAmp[l];
			// check the quality of the individual projections using the original energy depositions
			if ( CellDepS0[l][nm]>0.8*CellDepS0[l][nm-1] && CellDepS0[l][nm]>0.8*CellDepS0[l][nm+1]
					&& (CellDepS0[l][nm-1]>0.001 || CellDepS0[l][nm+1]>0.001)
				) { // energy map after subtruction of identified showers
				if ( (l/2)%2 == 1 ) fX0 += 1;
				else                fY0 += 1;
			}
			if ( CellDepS1[l][nm]>0.8*CellDepS1[l][nm-1] && CellDepS1[l][nm]>0.8*CellDepS1[l][nm+1]
					 && ( CellDepS1[l][nm-1]>0 || CellDepS1[l][nm+1]>0 )
					) { // energy map after subtraction and fluctuation supression
				if ( (l/2)%2 == 1 ) fX1 += 1;
				else                fY1 += 1;
			}
//			else if( l<2 && (CellDepS1[l][nm]==Cutoff_MIP || (CellDepS1[l][nm]>Cutoff_MIP && CellDep[l][nm]>0.008)) ) fY1++;
			if(DEBUG){
				cout << Form("l=%d, nm=%d, CellDepS1[nm]=%.4f, %.4f, %.4f, CellDepS0[nm]=%.4f, %.4f, %.4f, CellDep[nm]=%.4f, %.4f, %.4f, fX1=%d, fY1=%d", l, nm, CellDepS1[l][nm-1], CellDepS1[l][nm], CellDepS1[l][nm+1], CellDepS0[l][nm-1], CellDepS0[l][nm], CellDepS0[l][nm+1], CellDep[l][nm-1], CellDep[l][nm], CellDep[l][nm+1], fX1, fY1 ) << endl;
			}
		} else {
			LayAmp[l] = 0.;
			LayCrd[l] = 0.;
		}
	}
//	if( SeedLayerY<2  ){ // improve the efficiency for well-defined low energy seed
//		l = SeedLayerY;
//		nm = SeedCell[l];
//		if( CellDepS1[l][nm]>0.8*CellDepS1[l][nm-1] && CellDepS1[l][nm]>0.8*CellDepS1[l][nm+1] && (CellDepS1[l][nm-1]+CellDepS1[l][nm+1])>0 )
//			fY1++;
//	}
	return 0;
}

Int_t EcalKX3DRec::ParabolaZ0(){
	//
	// Define coordinate of the shower maximum
	//
	fZ0 = 1;
	fB0 = 1;
	Double_t _LayAmp[18];
	memcpy( _LayAmp, LayAmp, sizeof(_LayAmp) );

	Int_t l, nm = 0;
	Double_t v  = 0.;
	for (l=0; l<18; l++) {
		if ( _LayAmp[l] > v ) {
			v  = _LayAmp[l];
			nm = l;
		}
		if( _LayAmp[l]==0 ){
			if( l>=2 && l<=15 ){
				_LayAmp[l] = 0.5*(_LayAmp[l-2] + _LayAmp[l+2]);
			}else if( l<2 ){
				_LayAmp[l] = 0.5*_LayAmp[l+2];
			}else{
				_LayAmp[l]= 0.8*_LayAmp[l-2];
			}
		}
	}

	// calculate inverse matrix a_ij
	Double_t m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, det;
	Double_t val, arg;
	m11 = m12 = m22 = m23 = m33 = b1 = b2 = b3 = 0.;

	if ( nm < 3 ) {
		val = v;
		arg =  1.0 + EcalKX3DRecDB::EcalZ[0] - EcalKX3DRecDB::EcalZ[nm];
		//    printf("L -1, arg %8.3f, val %8.3f\n", arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;
	}

	// fit Z0 using +-3 layers
	for(l=TMath::Max(0,nm-3); l<TMath::Min(18,nm+4); l++ ) {
		val = v - _LayAmp[l];
		arg = EcalKX3DRecDB::EcalZ[l] - EcalKX3DRecDB::EcalZ[nm];
		//    printf("L %2d, arg %8.3f, val %8.3f\n", l, arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;
	}

	if ( nm > 15 ) {
		val = v- _LayAmp[2*nm-18];
		arg =  EcalKX3DRecDB::EcalZ[17] - 1.0 - EcalKX3DRecDB::EcalZ[nm];
		//    printf("L 18, arg %8.3f, val %8.3f\n", arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;

		fZ0 = 0;
		fB0 = 0;
	}

	// m22 == m13 is used explicitely
	det = m11*m22*m33 + 2.*m12*m22*m23 - m22*m22*m22 - m12*m12*m33 - m23*m23*m11;

	if ( det != 0. ) {
		a11 = m22*m33 - m23*m23;
		a12 = m22*m23 - m12*m33;
		a13 = m12*m23 - m22*m22;
		a22 = m11*m33 - m22*m22;
		a23 = m12*m22 - m11*m23;

		arg = b1*a11 + b2*a12 + b3*a13;
		if ( arg > 0. ) {
			val = -0.5*(b1*a12 + b2*a22 + b3*a23)/arg;
		} else {
			val = 0.;
		}
	} else {
		val = 0.;
	}

	if ( TMath::Abs(val)<2.5 ) {
		Z0 = val + EcalKX3DRecDB::EcalZ[nm];
		dZ0 = 1.0;
	} else {
		Z0 = EcalKX3DRecDB::EcalZ[nm];
		dZ0 = 2.5;
	}

	Apex = 0;
//	for(l=nm; l>=0; l--) {		if( LayAmp[l] < Cutoff_MIP*2 ){ Apex = l+1; break; }	}
	for(l=0; l<=nm; l++) {		if( LayAmp[l] > EcalKX3DRecDB::Cutoff_MIP*2 ){ Apex = l; break; }	}

	return 0;
}

Int_t EcalKX3DRec::ShowerAxis(){
	vector<Double_t> vx, vz, vw;
	// fit XZ
	for(Int_t l=0; l<18; l++){
		if( l%4>=2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalKX3DRecDB::EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KX, X0);
	else
		KX = X0 = 0.;
//		KX = X0 = -90.;

	vx.clear();
	vz.clear();
	vw.clear();
	for(Int_t l=0; l<18; l++){
		if( l%4<2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalKX3DRecDB::EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KY, Y0);
	else
		KY = Y0 = 0.;
//		KY = Y0 = -90.;

	return 0;
}

void EcalKX3DRec::LinearFit(Int_t n, vector<Double_t> vx, vector<Double_t> vy, vector<Double_t> vw, Double_t &kx, Double_t &x0){

	Double_t m11, m12, m22, b1, b2, det;
	m11 = m12 = m22 = b1 = b2 = 0.;

	for(Int_t i=0; i<n; i++){
		if ( vw[i] > 0 ) {
			m11 += vy[i]*vy[i]*vw[i];
			m12 += vy[i]*vw[i];
			m22 += vw[i];
			b1  += vx[i]*vy[i]*vw[i];
			b2  += vx[i]*vw[i];
		}
	}
	det = m11*m22 - m12*m12;
	if ( det != 0. ) {
		kx = (b1*m22 - b2*m12) / det;
		x0  = (b2*m11 - b1*m12) / det;
	} else {
		kx = -1.;
		x0 =  0.;
	}
	return;
}
/******************************************
 *  footprint for fit
 *  shower initial parameters
 *******************************************/
Int_t EcalKX3DRec::DefineFootprint(){

	Double_t e_x, e_y, v;
	Double_t x, y, z;
	Int_t l, k;
	Int_t nm, m;
	Int_t nbadcell=0;
	Double_t wbadcell=0.;

	static const int nw_fl = 1; // front layers (layer<3)
	int nw    = NumberOfCellsForFit; // number of cells around the shower axis to be used in fit

	int nww;

	E0 = EX = EY = 0.;
	El2f = 0.;
	X1 = Y1 = e_x = e_y = 0.;

	for (l=0; l<18; l++) {
		nww = l<3?nw_fl:nw;

		z = EcalKX3DRecDB::EcalZ[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		nm = 0;
		if ( TMath::Abs(x)<36. && TMath::Abs(y)<36. ) {
			if ( (l/2)%2 && XSeed >0 )           nm = (Int_t) ((x +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
			else if ( ((l/2)%2)==0 && YSeed >0 ) nm = (Int_t) ((y +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
		}

		m =  SeedCell[l];
		if ( m > 0 ) {
			if (nm>0 && nm<m) Cell_Min[l] = TMath::Max( 3, nm-nww);
			else              Cell_Min[l] = TMath::Max( 3,  m-nww);
			if (nm>0 && nm>m) Cell_Max[l] = TMath::Min(76, nm+nww);
			else              Cell_Max[l] = TMath::Min(76,  m+nww);
		} else if ( nm>0 ) {
			Cell_Min[l] = TMath::Max( 3, nm-nww);
			Cell_Max[l] = TMath::Min(76, nm+nww);
		} else {
			Cell_Min[l] = 76;
			Cell_Max[l] = 3;
		}

		v = 0.;
		for (k=Cell_Min[l]; k<=Cell_Max[l]; k++){
			if( CellMask[l][k] & EcalKX3DRecDB::BadCell ){ // bad cell
				nbadcell ++;
				if( l>=2 && l<=15 ){
					wbadcell = 0.5*(LayAmp[l-2] + LayAmp[l+2]);
				}else if( l<2 ){
					wbadcell = 0.5*LayAmp[l+2];
				}else{
					wbadcell = 0.8*LayAmp[l-2];
				}
				if( k==nm ){ // axis
					v += 0.80*wbadcell;
				}else{
					v += 0.20*wbadcell;
				}
			}else{
				v += CellDepS1[l][k];
			}
		}
		E0 += v;
		if( l>=16 ) El2f += v;
		if ( TMath::Abs(z-Z0) < 5. ) { // +/- 5 cm
			if ( (l/2)%2 == 1 ) {
				EX += v;
				for (k=Cell_Min[l]; k<=Cell_Max[l]; k++) {
					X1 += (k-39.5)*CellDepS0[l][k];
					e_x += CellDepS0[l][k];
				}
			} else {
				EY += v;
				for (k=Cell_Min[l]; k<=Cell_Max[l]; k++) {
					Y1 += (k-39.5)*CellDepS0[l][k];
					e_y += CellDepS0[l][k];
				}
			}
		}
		//    printf("Layer %02d, Cmin %02d, Cmax %02d, En %6.2f, Mmax %02d, Mfit %02d\n", l, Cell_Min[l], Cell_Max[l], v, SeedCell[l], nm);
	}

	if ( e_x > 0. ) X1 /= e_x;
	if ( e_y > 0. ) Y1 /= e_y;

//	printf("-->DefineFootprint(): X1 %8.3f, Y1 %8.3f\n", X1, Y1);

	if( E0>0 ){
	  E0f = E0;
  	  El2f/=E0f;
	  // l2r correction
	  if( nw==1 ) E0 *= 1.23*(1 + 15*El2f*El2f);
	  else if( nw==2 ) E0 *= 1.14*(1 + 14.7*El2f*El2f);
	}
	// fiber saturation
	E0 *= TMath::Max(1.0, 0.952*(1 + 2.25e-4*E0));

	if( NhitSat>0 ){ // adhoc ADC saturation correction, important for fit of TeV events
		E0 += 60*(NhitSat + 0.1*NhitSat*NhitSat); // 60 GeV per cell
	}

	// extend fit footprint for E > 500 GeV
	if( NhitSat>0 || NhitMask>0 ){
		if( DEBUG ){
			cout << "Extend the fit footprint E0=" << E0 << ", NhitSat=" << NhitSat << endl;
		}
		int nexpand=1;
		if( NhitSat>5 || NhitMask>5 ){ nexpand = 2; }
		if( NhitSat>10 || NhitMask>10 ){ nexpand = 3; }
		if( NhitMask>20 || NhitMask>20 ){ nexpand = 4; }
		if( NhitMask>30 || NhitMask>30 ){ nexpand = 5; }
		for(l=0; l<EcalKX3DRecDB::kNL; l++){
			Cell_Min[l] = TMath::Max( 3, Cell_Min[l]-nexpand);
			Cell_Max[l] = TMath::Min(76, Cell_Max[l]+nexpand);
		}
	}


	E0f = E0;
//	if ( XSeed>0 && YSeed>0 )	E0  *= 1.4;                            // 70% of energy in the footprint on average
//	else                			E0  *= 2.8;                            // factor 2 for one projection only
	Double_t sqrt_E0 = sqrt(E0);

	dE0 = 0.2*sqrt_E0 + 0.03*E0;

	B0 = 0.65;
	//B0  = 1.84*(1-0.0812*log(E0))/(1+0.149*log(E0)); // tuned to make E0/Ebeam close to 1
	//B0  = 8.18464e-01 + -2.29270e-02*log(E0); // mean value of free fit
	//if( B0 < 0.2 ) B0 = 0.2;
	//if( B0 > 3.0 ) B0 = 3.0;
	dB0 = 0.1;

	//A0 = -141.0;
	A0 = EcalKX3DRecDB::EcalZ[Apex]+2.25;
	dA0 = 1.0;

	T0 = (A0-Z0)*sqrt(1+KX*KX+KY*KY);
	dT0 = 1.0;

	dX0 = 1.0/sqrt_E0 + 0.10;
	dY0 = 1.0/sqrt_E0 + 0.10;
	dKX = 0.1/sqrt_E0 + 0.01;
	dKY = 0.1/sqrt_E0 + 0.01;


	return 0;
}

void EcalKX3DRec::CalcShowerSize(){
	Int_t IsLeak=0, m;
	Double_t d=0;
	double x, y, z, _esx = 0., _esy=0.;
	sx = sy = 0;
	int k = TMath::Max(5, (Int_t) (5 + 2.6*TMath::Log(E0+1)));
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		z = EcalKX3DRecDB::EcalZ[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		if ( (l/2)%2 ) m = (Int_t) ((x +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
		else           m = (Int_t) ((y +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
		for(Int_t c=TMath::Max(0, m-k); c<=TMath::Min(EcalKX3DRecDB::kNC-1,m+k); c++){
			if( CellDep[l][c]>0 ){
				Int_t c_m = 2*m - c;
				if( c_m < 4 || c_m > EcalKX3DRecDB::kNC-4 ) IsLeak = 1;
				else 								  IsLeak = 0;
				d = (c-39.5)*EcalKX3DRecDB::Cell_Size;

				if( l%4<2 ){
					d = sqrt( (d-y)*(d-y) + (z-Z0)*(z-Z0) );
				  	sy += ( (1+IsLeak)*CellDep[l][c]*d );
					_esy += ( (1+IsLeak)*CellDep[l][c] );
				}else{
					d = sqrt( (d-x)*(d-x) + (z-Z0)*(z-Z0) );
		 			sx += ( (1+IsLeak)*CellDep[l][c]*d );
					_esx += ( (1+IsLeak)*CellDep[l][c] );
				}
			}
		}
	}
	if( _esx>0 ) sx /= _esx;
	if( _esy>0 ) sy /= _esy;
}

/************************************/
// 1D longitudinal fit //
void EcalKX3DRec::Parabolic1DFit(Double_t &par, Double_t &error, Double_t &L0, Double_t &L1) {

	Int_t i, n1, n2, flag, iter;

	Double_t param[7], lhood[7], pmin, lmin;

	Double_t m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, a33, det, alpha, val, err;

	flag = iter = 0;

	do{

		// parameter - central value
		param[3] = par;
		lhood[3] = ShowerLongProfile();
		if ( (val=T0-2.)                               < 0. ) lhood[3] += val*val;
		if ( (val=Z0+T0/sqrt(1.+KX*KX+KY*KY)+130.) > 0. ) lhood[3] += val*val;
		if ( (val=Z0+142.)                             > 0. ) lhood[3] += val*val;
		if ( (val=Z0+160.)                             < 0. ) lhood[3] += val*val;

		if ( iter == 0 ) L0 = lhood[3];

		// parameter - low values
		n1 = 3;
		while ( n1 > 0 ) {
			n1 -= 1;
			param[n1] = param[n1+1] - (3.-n1)*(error);

			par = param[n1];
			lhood[n1] = ShowerLongProfile();
			if ( (val=T0-2.)                               < 0. ) lhood[n1] += val*val;
			if ( (val=Z0+T0/sqrt(1.+KX*KX+KY*KY)+130.) > 0. ) lhood[n1] += val*val;
			if ( (val=Z0+142.)                             > 0. ) lhood[n1] += val*val;
			if ( (val=Z0+160.)                             < 0. ) lhood[n1] += val*val;

			if ( lhood[n1] > lhood[n1+1] ) break;
		}

		// parameter - high values
		n2 = 3;
		while ( n2 < 6 ) {
			n2 += 1;
			param[n2] = param[n2-1] + (n2-3.)*(error);

			par = param[n2];
			lhood[n2] = ShowerLongProfile();
			if ( (val=T0-2.)                               < 0. ) lhood[n2] += val*val;
			if ( (val=Z0+T0/sqrt(1.+KX*KX+KY*KY)+130.) > 0. ) lhood[n2] += val*val;
			if ( (val=Z0+142.)                             > 0. ) lhood[n2] += val*val;
			if ( (val=Z0+160.)                             < 0. ) lhood[n2] += val*val;

			if ( lhood[n2] > lhood[n2-1] ) break;
		}

		// normalize parameters and likelihoods
		pmin = param[3];
		lmin = lhood[3];
		for (i=n1; i<=n2; i++ ) if ( lhood[i] < lmin ) lmin = lhood[i];
		for (i=n1; i<=n2; i++ ) {
			param[i] -= pmin;
			lhood[i] -= lmin;
		}

		// calculate inverse matrix a_ij
		m11 = m12 = m22 = m23 = m33 = b1 = b2 = b3 = 0.;
		for (i=n1; i<=n2; i++ ) {
			m11 += param[i]*param[i]*param[i]*param[i];
			m12 += param[i]*param[i]*param[i];
			m22 += param[i]*param[i];
			m23 += param[i];
			m33 += 1.;
			b1  += param[i]*param[i]*lhood[i];
			b2  += param[i]*lhood[i];
			b3  += lhood[i];
			//    printf("Ind %d, Par %9.4f / %8.3f, Likelihood %8.3f / %8.3f\n", i, param[i], param[i]+pmin, lhood[i], lhood[i]+lmin);
		}
		// m22 == m13 is used explicitely
		det = m11*m22*m33 + 2.*m12*m22*m23 - m22*m22*m22 - m12*m12*m33 - m23*m23*m11;

		if ( det != 0. ) {
			a11 = m22*m33 - m23*m23;
			a12 = m22*m23 - m12*m33;
			a13 = m12*m23 - m22*m22;
			a22 = m11*m33 - m22*m22;
			a23 = m12*m22 - m11*m23;
			a33 = m11*m22 - m12*m12;

			alpha = b1*a11 + b2*a12 + b3*a13;
			if ( alpha > 0. ) {
				val = -0.5*(b1*a12 + b2*a22 + b3*a23)/alpha;
				err = sqrt(det/alpha);
				//      printf("alpha %8.3f, det %f, val %8.3f, err %8.3f\n", alpha, det, val, err);
				flag = 1;
			} else {
				if ( lhood[n1] < lhood[n2] ) {
					val = param[n1];
				} else if ( lhood[n2] < lhood[n1] ) {
					val = param[n2];
				} else {
					val = param[3];
				}
				err = 2.*(error);
			}
		} else {
			if ( lhood[n1] < lhood[n2] ) {
				val = param[n1];
			} else if ( lhood[n2] < lhood[n1] ) {
				val = param[n2];
			} else {
				val = param[3];
			}
			err = 2.*(error);
		}

		if ( TMath::Abs(val-param[3]) > 6.*(error) ) {
			flag = 0;
			if ( lhood[n1] < lhood[n2] ) {
				val = param[n1];
			} else {
				val = param[n2];
			}
			err = 2.*(error);
		}

		par   = val + pmin;
		error = err;
		iter++;
	}while (flag == 0 && iter < EcalKX3DRecDB::MaxIteration );

	L1 = ShowerLongProfile();
	if ( (val=T0-2.)                               < 0. ) L1 += val*val;
	if ( (val=Z0+T0/sqrt(1.+KX*KX+KY*KY)+130.) > 0. ) L1 += val*val;
	if ( (val=Z0+142.)                             > 0. ) L1 += val*val;
	if ( (val=Z0+160.)                             < 0. ) L1 += val*val;
	//  printf("Flag %d, Par %8.3f, Error %8.3f, L0 %8.3f, L1 %8.3f\n", flag, *par, err, L0, L1);

	return;
}

Int_t EcalKX3DRec::Shower1DFit_AK(){
	memset(LayDep, 0, sizeof(LayDep));
	for(int l=0; l<18; l++){
		for(int c=0; c<80; c++){
			LayDep[l]+=CellDep[l][c];
		}
	}
	Double_t l0=0, l1=0;
	Double_t l_start, l_end;
	Int_t iter = 0;
	do{
		iter++;
		//Parabolic1DFit(E0,  dE0, l0, l1);
		l_start = l0;
		Parabolic1DFit(B0, dB0, l0, l1);
		Parabolic1DFit(T0, dT0, l0, l1);
		l_end = l1;
		//		cout<<"EcalKX3DRec::Shower1DFit_AK:"<<E0<<" "<< T0<<endl;
	} while( l_start - l_end >= 0.5 && iter < 20 );

	LkhdLong = l_end;
//	cout << B0 << endl;

	//   T0  =  T0*sqrt(1.+KX*KX+KY*KY);
	//   dT0 =  dT0*sqrt(1.+KX*KX+KY*KY);
	//   B0  =  B0/sqrt(1.+KX*KX+KY*KY);
	return iter;
}

Double_t EcalKX3DRec::ShowerLongProfile() {
	Int_t    l;
	Double_t t1, t2;
	Double_t reg, arg, val, var;
	Double_t ecal_pe[18];
	const static Double_t beta[18] = {
		20, 20, 20, 20, 20,
		20, 20, 20, 20, 22,
		24, 26, 28, 32, 40,
		41, 35, 30
	};

	Double_t K = sqrt( 1 + KX*KX + KY*KY );
	var = 0.;
	for (l=0; l<18; l++) {
		if ( LayDep[l] > 0. ) {
			t1 = (A0 - EcalKX3DRecDB::EcalZ[l]-0.4625)*K;
			t2 = (A0 - EcalKX3DRecDB::EcalZ[l]+0.4625)*K;
			//	 t1 = Z0 + T0 - EcalKX3DRecDB::EcalZ[l]-0.46;
			//	 t2 = Z0 + T0 - EcalKX3DRecDB::EcalZ[l]+0.46;
			if ( t1 > 0. )
				ecal_pe[l] = E0*(TMath::Gamma(B0*T0+1., B0*t2) - TMath::Gamma(B0*T0+1., B0*t1));
			//ecal_pe[l] = E0/1.08*B0/TMath::Gamma(B0*T0+1.)*pow(B0*reg,B0*T0)*exp(-B0*reg);
			else
				ecal_pe[l] = 0.01; // MIP

			LayExp[l] = ecal_pe[l];

			reg = beta[l]*LayDep[l];                     // measured energy
			arg = beta[l]*ecal_pe[l];                     // expected energy
//			if( flag == 1 ){ // get baseline likelihood
//				reg = arg;
//				//reg = arg<0.5?0:-0.5 - 0.0643*TMath::Power((arg+0.5), -1.23) + arg; // Apro. peak of Poisson distribution
//			}

			val = arg - reg*TMath::Log(arg);
			if ( reg < 10. ) val += log( TMath::Gamma(reg+1.) );
			else             val += reg*log(reg) - reg + log(reg*(1.+4.*reg*(1.+2.*reg)))/6. + 0.57236494;
			var += val;
		}
	}
	return var;
}



/****************************************************************************
 *                                                                          *
 *            Expected light output of a single fiber at depth, distance    *
 *            and coord in the electromagnetic shower defined by parameters *
 *            E0, B0, T0                                                    *
 *                                                                          *
 *               Input: -  depth, dist, coord, E0, B0, T0                   *
 *                                                                          *
 *               Output: - light output - return value                      *
 *                                                                          *
 ****************************************************************************/
Double_t EcalKX3DRec::ShowerShapeIdeal(Double_t depth, Double_t dist, Double_t fc, Double_t rc, Double_t rt, Double_t &fsat){
	// no lateral leakage along fiber
	Double_t rc2 = rc*rc,
				rt2 = rt*rt,
				d2  = dist*dist;
	Double_t varc = 3.14159265358979312*rc2/(rc2+d2)/sqrt(rc2+d2);
	Double_t vart = 3.14159265358979312*rt2/(rt2+d2)/sqrt(rt2+d2);
	Double_t e=0;
	Double_t e0=0;
	fsat = 1.;
	static Double_t F_BS = 0.0025; // 0.25% energy in back-splash

	if ( E0 > 0.3 && depth>0 ) { // Shower
		e0 = EcalKX3DRecDB::fNorm*(E0*B0*GammaFast(depth, T0))*(1-F_BS);
		e = e0*(fc*varc+(1.-fc)*vart) ;
	} else if( dist<0.15 ) { // MIP
		e = 0.003; // 3 MeV per fiber, 2~5 fibers passed by MIP
	}
	if( F_BS > 0 && depth>0 && depth<5 ){// back-splash, to improve the model for first few layers at high energy
		static const Double_t Rb = 4.; // radius, cm
		static const Double_t B0bs = 2.5, T0bs = 2.0;
		double e1 = EcalKX3DRecDB::fNorm*F_BS*E0*B0bs*GammaFast_par2(depth, T0bs);
		e += e1/Rb*exp(-dist/Rb);
	}
	return e;
}

Int_t EcalKX3DRec::GammaFast_FirstCall = 1;
Double_t EcalKX3DRec::GammaFast(Double_t depth, Double_t t0){
	// use table to calculate gamma function
	// within 1<=T0<31, 0=<t<30
	static Double_t gamma_table[300][300];
	if( GammaFast_FirstCall ){ // generate table
		Double_t _T0, _Alpha, _B0=0.65, _GammaAlpha;
		printf("Call GammaFast() First Time, Generating table for B0=%g\n", _B0);
		Double_t _depth;
		for(Int_t i=0; i<300; i++){
			_T0 = 0.1*(i)+1;
			_Alpha = _B0*_T0;
			_GammaAlpha = TMath::Gamma(_Alpha+1);
			for(Int_t j=0; j<300; j++){
				_depth = 0.1*(j);
				gamma_table[i][j] = pow(_B0*_depth, _Alpha)*exp(-_B0*_depth)/_GammaAlpha;
			}
		}
		GammaFast_FirstCall = 0;
	}

	t0=10*(t0-1);
	depth*=10;
	int i1, j1,
		 i2, j2;
	if( t0 <= 0 ){
		t0 = 0.;
		i1 = i2 = 0;
	} else if( t0>=299 ){
		i1 = i2 = 299;
	} else{
		i1 = int(t0);
		i2 = i1 + 1;
	}

	if( depth >= 299 ){
		j1 = j2 = 299;
	} else {
		j1 = int(depth);
		j2 = j1 + 1;
	}

	Double_t b1 = gamma_table[i1][j1],
				b2 = gamma_table[i2][j1] - gamma_table[i1][j1],
				b3 = gamma_table[i1][j2] - gamma_table[i1][j1],
				b4 = gamma_table[i2][j2] - gamma_table[i1][j2] - gamma_table[i2][j1] + gamma_table[i1][j1];
	return b1 + b2*(t0-i1) + b3*(depth-j1) + b4*(t0-i1)*(depth-j1);
}

Int_t EcalKX3DRec::GammaFast_par2_FirstCall = 1;

Double_t EcalKX3DRec::GammaFast_par2(Double_t depth, Double_t t0){
	// use table to calculate gamma function
	// within T0=2.0, 0<=t<30
	static Double_t gamma_table_par2[300];
	if( GammaFast_par2_FirstCall ){ // generate table
		Double_t _T0=2.0, _Alpha, _B0=2.5, _GammaAlpha;
		printf("Call GammaFast() First Time, Generating table for B0=%g\n", _B0);
		Double_t _depth;

		_Alpha = _B0*_T0;
		_GammaAlpha = TMath::Gamma(_Alpha+1);
		for(Int_t j=0; j<300; j++){
			_depth = 0.1*(j);
			gamma_table_par2[j] = pow(_B0*_depth, _Alpha)*exp(-_B0*_depth)/_GammaAlpha;
		}

		GammaFast_par2_FirstCall = 0;
	}
	int j1, j2;
	depth*=10;
	if( depth >= 299 ){
		j1 = j2 = 299;
	} else {
		j1 = int(depth);
		j2 = j1 + 1;
	}

	Double_t b1 = gamma_table_par2[j1],
				b3 = gamma_table_par2[j2] - gamma_table_par2[j1];

	return b1 + b3*(depth-j1);
}


Double_t EcalKX3DRec::ShowerShape(Int_t slay, Double_t depth, Double_t dist, Double_t &fsat){
	fsat = 1.;
	if( E0 <= 0 ) return 0.00001;

	Int_t i1, i2;
	Double_t qc, rc, rt;
	Double_t varc, vart;

//	use t/T0 parameterization
	vart   = depth/T0+1; // parameterized as a function of t/T0, 0~2
	i1 = int(vart*10);
	if(i1<0) i1=0;
	if(i1>28) i1=28;
	i2 = i1+1;
	varc = vart*10 - i1;
	if( varc > 1 ) varc = 1.;

	depth += T0;

	// Core fraction
	qc = Fc[I_Shwr][i1] + varc*(Fc[I_Shwr][i2] - Fc[I_Shwr][i1]);
	// Core radius
	rc = Rc[I_Shwr][i1] + varc*(Rc[I_Shwr][i2] - Rc[I_Shwr][i1]);
	// Tail radius
	rt = Rt[I_Shwr][i1] + varc*(Rt[I_Shwr][i2] - Rt[I_Shwr][i1]);

	if( Index>0 ){
		// to fit the lateral parameters
		static const Double_t rl = 0.95, rh = 1.05;
		if( Index == 1 ) qc *= rl;
		if( Index == 2 ) qc *= rh;
		if( Index == 3 ) rc *= rl;
		if( Index == 4 ) rc *= rh;
		if( Index == 5 ) rt *= 0.9;
		if( Index == 6 ) rt *= 1.1;
	}
	if( qc > 1.0 ) qc = 1.;
	if( qc < 0.01 ) qc = 0.01;
	if( rc < 0.1 ) rc = 0.1;
	if( rt > 10 ) rt = 10.;

	return ShowerShapeIdeal( depth, dist, qc, rc, rt, fsat );
}

/**************************************************************************************
 *                                                                                    *
 *         Calculate expected energy deposition for                                   *
 *         an electromagnetic shower                                                  *
 *                                                                                    *
 *         Input: - X0, Y0, Z0, KX, XY0 defines shower axis                           *
 *                                                                                    *
 *         Output: - ene_cell_exp[18][72], dpt_cell_exp[18][72], dst_cell_exp[18][72] *
 *                     expected energy depositions, average depth                     *
 *                     and average radius for exposed cells                           *
 *                                                                                    *
 **************************************************************************************/
Int_t EcalKX3DRec::ShowerModel(Int_t index, Int_t flag){
	// flag = 1, calculate ideal energy deposition, for energy estimation
	// flag = 0, only calculate "real" energy deposition, for fit
	Index = index;
	Int_t i, j, k, l, m, m0, m1;
	Double_t eff0, eff1;
	Double_t x, y, z, v, d, eff, t, c, a, fsat;
	memset(nfbd, 0, sizeof(nfbd)); // invisible fiber
	memset(nfbs, 0, sizeof(nfbs)); // shared fiber

	Int_t F_Min, F_Max;
	if( KX==-90 || X0==-90|| KY==-90|| Y0==-90)return -1;

	// shower footprint based on data
	memset(CellExp, 0, sizeof(CellExp));
	memset(CellEtr, 0, sizeof(CellEtr));
	memset(CellDepth, 0, sizeof(CellDepth));
	memset(CellDist, 0, sizeof(CellDist));
	memset(CellBeta, 0, sizeof(CellBeta));

	Double_t sqrt_1_KXKX = sqrt(1+KX*KX);
	Double_t sqrt_1_KYKY = sqrt(1+KY*KY);
	Double_t sqrt_1_KXKX_KYKY = sqrt(1+KX*KX+KY*KY);

	T0 = A0>Z0?(A0-Z0)*sqrt_1_KXKX_KYKY:0.1;
	//Alpha = B0*T0;
	//GammaAlpha = TMath::Gamma( Alpha + 1 );

	// calculate energy deposition in the footprint
	for (l=0; l<EcalKX3DRecDB::nFibLayer; l++) {
		z = EcalKX3DRecDB::F_Zcoord[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		Int_t l_ecal = l/5;
		if( Cell_Min[l_ecal] > Cell_Max[l_ecal] ) continue;
		//      printf("C_Min %d, C_Max %d, kX0 %f, X0 %f, kY0 %f, Y0 %f, Z0 %f, Energy %f\n", Cell_Min[l/5], Cell_Max[l/5], KX, X0, KY, Y0, Z0, E0);
		//		cout << "ShowerModel: Cell_Min "<<Cell_Min[l/5] <<", Cell_Max " << Cell_Max[l/5] << endl;
		//		continue;

		bool IsXProjection = (l/10)%2;
		if(IsXProjection){ // X projection - wires along Y, 4 superlayers
			F_Min = (Int_t) ((EcalKX3DRecDB::Cell_Size*(Cell_Min[l_ecal]-40) + y*EcalKX3DRecDB::F_Rotation[l_ecal] - EcalKX3DRecDB::F_Offset[l])/EcalKX3DRecDB::F_Diameter + 267.5);
			F_Max = (Int_t) ((EcalKX3DRecDB::Cell_Size*(Cell_Max[l_ecal]-39) + y*EcalKX3DRecDB::F_Rotation[l_ecal] - EcalKX3DRecDB::F_Offset[l])/EcalKX3DRecDB::F_Diameter + 266.5);
		}else{
			F_Min = (Int_t) ((EcalKX3DRecDB::Cell_Size*(Cell_Min[l_ecal]-40) + x*EcalKX3DRecDB::F_Rotation[l_ecal] - EcalKX3DRecDB::F_Offset[l])/EcalKX3DRecDB::F_Diameter + 267.5);
			F_Max = (Int_t) ((EcalKX3DRecDB::Cell_Size*(Cell_Max[l_ecal]-39) + x*EcalKX3DRecDB::F_Rotation[l_ecal] - EcalKX3DRecDB::F_Offset[l])/EcalKX3DRecDB::F_Diameter + 266.5);
		}
		if ( F_Min <   0 ) F_Min =   0;
		if ( F_Min > EcalKX3DRecDB::nFibPerLay-1 ) F_Min = EcalKX3DRecDB::nFibPerLay-1;
		if ( F_Max <   0 ) F_Max =   0;
		if ( F_Max > EcalKX3DRecDB::nFibPerLay-1 ) F_Max = EcalKX3DRecDB::nFibPerLay-1;

		for (k=F_Min; k<=F_Max; k++) {
			if( IsXProjection ) {
				v = EcalKX3DRecDB::F_Diameter*(k-266.5) - y*EcalKX3DRecDB::F_Rotation[l_ecal] + EcalKX3DRecDB::F_Offset[l];    // fiber coordinate
				c = y + KX*KY*(v-x)/(KX*KX+1.);                         // Y coordinate on the fiber
				d = TMath::Abs(v-x)/sqrt_1_KXKX;                          // distance to the shower axis
				t = (KX*(X0-v)+(Z0-z))*sqrt_1_KXKX_KYKY/(KX*KX+1.); // shower depth, could be negative, point-line distance
			}else{
				v = EcalKX3DRecDB::F_Diameter*(k-266.5) - x*EcalKX3DRecDB::F_Rotation[l_ecal] + EcalKX3DRecDB::F_Offset[l];    // fiber coordinate
				c = x + KX*KY*(v-y)/(KY*KY+1.);                            // X coordinate on the fiber, pedal
				d = TMath::Abs(v-y)/sqrt_1_KYKY;                          // distance to the shower axis
				t = (KY*(Y0-v)+(Z0-z))*sqrt_1_KXKX_KYKY/(KY*KY+1.); // shower depth, could be negative
			}
			a = ShowerShape(l/10, t, d, fsat);
			if( a <= 0 ){
				//					printf("-->ShowerModel(): t=%.3f, d=%.3f, c=%.3f, eff=%.3f, a=%.5f, B0=%.3f, A0=%.3f\n", t, d, c, eff, a, B0, A0);
				continue;
			}
			a *= fsat;

			GetPMTEfficiencyFromTable(l, k, m0, m1, eff0, eff1);

			if( eff0<=0 && eff1<=0 ) { // not readout by any PMT
				//printf( "Line 1917: l_ecal %d, m0 %d, m1 %d, not readout by any PMT\n", l_ecal, m0, m1);
				nfbd[l_ecal] ++;
				continue;
			}

			if( int(m0/2)!=int(m1/2) ){ // readout by 2 PMT
				//					printf("--> Readout by 2 PMTs: l_ecal %d, fiber %d, pmt0 %d, pmt1 %d\n", l_ecal, k, m0/2, m1/2);
				double cf0=0., cf1=0.;
				double nshare = 0.;
				double a0=0., a1=0.;
				if( m0 >= Cell_Min[l_ecal] && m0<= Cell_Max[l_ecal] ){
					if( eff0<0 ){ printf("Line 1920: l=%d, k=%d, l_ecal=%d, m0=%d\n", l, k, l_ecal, m0 ); }
					// PMT0 at "-" end, by definition
					if( IsXProjection ) cf0 = getCorrEs2Ed((t/T0+1), d, c, KY, B0)*getKyCorrEs2Ed((t/T0+1), d, KY, KX);
					else                cf0 = getCorrEs2Ed((t/T0+1), d, c, KX, B0)*getKyCorrEs2Ed((t/T0+1), d, KX, KY);
					a0 = a*eff0*cf0;
					CellExp[l_ecal][m0] += a0;
					CellDepth[l_ecal][m0] += a0*t;                                        // amplitude-averaged depth
					CellDist[l_ecal][m0] += a0*d;                                        // amplitude-averaged distance to axis
					nshare++;
				}
				if( m1 >= Cell_Min[l_ecal] && m1 <= Cell_Max[l_ecal] ){
					if( eff1<0 ){ printf("Line 1923: l=%d, k=%d, l_ecal=%d, m0=%d\n", l, k, l_ecal, m1 ); }
					// PMT1 at "+" end, by definition
					if( IsXProjection ) cf1 = getCorrEs2Ed((t/T0+1), d, -c, -KY, B0)*getKyCorrEs2Ed((t/T0+1), d, -KY, KX);
					else                cf1 = getCorrEs2Ed((t/T0+1), d, -c, -KX, B0)*getKyCorrEs2Ed((t/T0+1), d, -KX, KY);
					a1 = a*eff1*cf1;
					CellExp[l_ecal][m1] += a1;
					CellDepth[l_ecal][m1] += a1*t;                                        // amplitude-averaged depth
					CellDist[l_ecal][m1] += a1*d;                                        // amplitude-averaged distance to axis
					nshare++;
				}
				if( nshare > 0 ){
					nfbs[l_ecal]++;
					if(flag){ // ideal energy deposition, split into 2 parts
						if( IsXProjection ){
							cf0 = getCorrEd2Ei((t/T0+1), d, c, KY, B0);
							cf1 = getCorrEd2Ei((t/T0+1), d, -c, -KY, B0);
						}else{
							cf0 = getCorrEd2Ei((t/T0+1), d, c, KX, B0);
							cf1 = getCorrEd2Ei((t/T0+1), d, -c, -KX, B0);
						}
						if( eff0>0 && cf0>0 ) CellEtr[l_ecal][m0] += a0*cf0/eff0/fsat/nshare;
						if( eff1>0 && cf1>0 ) CellEtr[l_ecal][m1] += a1*cf1/eff1/fsat/nshare;
					}else{
						if( eff0>0 ) CellEtr[l_ecal][m0] += a0/eff0/fsat/nshare;
						if( eff1>0 ) CellEtr[l_ecal][m1] += a1/eff1/fsat/nshare;
					}
				}
			}else{
				if( l_ecal/2%2 == m0/2%2 ){ // PMT at "-" end
					m = m0;
				}else{  // PMT at "+" end
					m = m1;
				}
				if ( m < Cell_Min[l_ecal] || m > Cell_Max[l_ecal] ){ // because of fiber rotation
					continue;
				}
				eff = eff0;
				if( eff<=0 ){
					printf("Line 1972: l=%d, k=%d, l_ecal=%d, m0=%d\n", l, k, l_ecal, m );
					continue;
				}
				a *= eff;

				// apply correction of inclination, lateral leakage
				float cf = 1;
				if( l/10%2 == m/2%2 ){ // PMT at "-" end
					if( IsXProjection ) cf = getCorrEs2Ed((t/T0+1), d, c, KY, B0)*getKyCorrEs2Ed((t/T0+1), d, KY, KX);
					else                cf = getCorrEs2Ed((t/T0+1), d, c, KX, B0)*getKyCorrEs2Ed((t/T0+1), d, KX, KY);
				}else{  // PMT at "+" end
					if( IsXProjection ) cf = getCorrEs2Ed((t/T0+1), d, -c, -KY, B0)*getKyCorrEs2Ed((t/T0+1), d, -KY, KX);
					else                cf = getCorrEs2Ed((t/T0+1), d, -c, -KX, B0)*getKyCorrEs2Ed((t/T0+1), d, -KX, KY);
				}
				a *= cf;

				// deposited energy and weighted coordinates of the cell
				CellExp[l_ecal][m] += a;                                           // collected light
				CellDepth[l_ecal][m] += a*t;                                       // amplitude-averaged depth
				CellDist[l_ecal][m] += a*d;                                        // amplitude-averaged distance to axis

				if(flag){ // called for energy calculation, after fits are done
					// ideal energy deposition
					if( l/10%2 == m/2%2 ){ // PMT at "-" end
						if( IsXProjection ) cf = getCorrEd2Ei((t/T0+1), d, c, KY, B0);
						else                cf = getCorrEd2Ei((t/T0+1), d, c, KX, B0);
					}else{  // PMT at "+" end
						if( IsXProjection ) cf = getCorrEd2Ei((t/T0+1), d, -c, -KY, B0);
						else                cf = getCorrEd2Ei((t/T0+1), d, -c, -KX, B0);
					}
					a = a*cf/eff/fsat;
					CellEtr[l_ecal][m] += a;
				}else{
					CellEtr[l_ecal][m] += a/eff/fsat;
				}
			}
		}
	}

	// print number of dark fibers
	//for(int i=0; i<18; i++)	printf("l_ecal %d, dark fibers %d\n", i, ndark[i]) ;

	// position of the cell wrt the shower maximum
	Int_t i1, i2;
	Double_t p0, p1, reg;
	for (i=0; i<EcalKX3DRecDB::kNL; i++) {
		for (j=Cell_Min[i]; j<=Cell_Max[i]; j++) {
			if ( TMath::IsNaN( CellExp[i][j] ) ){
				cout << "1540: I_Shwr=" << I_Shwr << ", l=" << i << ", m=" << j << endl;
			}
			if ( CellExp[i][j] > 0. ) {
				CellDepth[i][j] /= CellExp[i][j];
				CellDist[i][j] /= CellExp[i][j];
				reg = CellDepth[i][j];                            // shower depth

				i1 = int((1+reg/T0)*10);
				if(i1<0) i1=0;
				if(i1>28) i1=28;
				i2=i1+1;
				reg = ((1+reg/T0)*10 - i1);
				if(reg>1) reg=1;
				if(reg<-1) reg=-1;

				// parameters
				p0 = P0[I_Shwr][i1] + reg*(P0[I_Shwr][i2]-P0[I_Shwr][i1]);
				p1 = P1[I_Shwr][i1] + reg*(P1[I_Shwr][i2]-P1[I_Shwr][i1]);
				if(p1<0) p1=0;
				reg = CellDist[i][j];                            // distance from the shower axis

				CellBeta[i][j] = p0*log(p1+reg);
				if( CellBeta[i][j]<0 || TMath::IsNaN( CellBeta[i][j] ) )
					printf("-->ShowerModel(): L=%d, C=%d, Dep=%8.3f, Exp=%.3f, Beta=%.3f\n", i, j, CellDepS1[i][j], CellExp[i][j], CellBeta[i][j]);
			} else {
				if( CellExp[i][j]<0 )
					printf("-->ShowerModel(): L=%d, C=%d, Dep=%8.3f, Exp=%.3f\n", i, j, CellDepS1[i][j], CellExp[i][j]);
			}
		}
	}
	if(DataType==EcalKX3DRecDB::_TB_DATA || DataType==EcalKX3DRecDB::_ISS_DATA) NonlinearityCorrection();
	SetBetaLimit();
	return 0;
}

/******************************************************************************
 *                                                                         	*
 *     Probability according to Poisson distribution for the        	 			*
 *     actual e-m energy deposition ene_cell_dep[18][80] and expected   		*
 *     deposition of ene_cell_exp[18][80] at depth dpt_cell_exp[18][80] and   *
 *     radial distance of depth dst_cell_exp[18][80]                     		*
 *                                                                  				*
 *       Input: -  ene_cell_dep[18][80], ene_cell_exp[18][80], 					*
 *       			 dpt_cell_exp[18][80]  and  dst_cell_exp[18][80] 				*
 *                                                                  				*
 *       Output: - prob_cell[18][80]                                   			*
 *                                                                          	*
 *****************************************************************************/
Int_t EcalKX3DRec::ShowerProbability(Int_t flag) {

	Int_t k, l, m;

	Double_t arg, val, reg, beta;

	for (l=0; l<18; l++) {
		for (m=0; m<80; m++) {
			CellProb[l][m]=0.;
		}
		for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
			if ( CellMask[l][m]==0 ) {

				arg = CellBeta[l][m]*CellExp[l][m];                // expected number of particles from the current shower
				val = CellExp[l][m];                             // expected energy from the current shower

				// estimated number of particles and average beta
				for (k=0; k<N_Shwr; k++) {
					if(k==I_Shwr)continue;
					val += ShwrExp[k][l][m];
					arg += ShwrBeta[k][l][m]*ShwrExp[k][l][m];
				}

				if ( val == 0. ) {
					arg  = 0.0001;
					beta = 80.;
				} else {
					beta = arg/val;
				}

				reg = beta*CellDep[l][m];                       // measured energy
				if( CellMask[l][m] ) reg = arg; // overflow/bad/noisy

				// probability calculation
				val = arg - reg*log(arg);
				if ( reg < 10. ) CellProb[l][m] = val + TMath::LnGamma(reg+1.);//log( TMath::Gamma(reg+1.) );
				else             CellProb[l][m] = val + reg*log(reg) - reg + log(reg*(1.+4.*reg*(1.+2.*reg)))/6. + 0.57236494;
				if( flag ){ // should be called after the fit is done
					// likelihood ratio
					val =  arg - reg;
					if ( reg > 0. ) val += reg * (log(reg) - log(arg));
					CellProbNorm[l][m] = val;
				}
				if( CellProb[l][m] < 0 ) printf("-->ShowerProbability(): L %d, C %d, Eexp %f, Edep %f, reg %f, arg %f, Mask %d\n", l, m, CellExp[l][m], CellDep[l][m], reg, arg, CellMask[l][m]);
				if(DEBUG>10) printf("-->ShowerProbability():L %d, Cell %d, E %f, Eexp %f, Prob %f, N %f, Nexp %f\n", l, m, CellDep[l][m], CellExp[l][m], CellProb[l][m], reg, arg);

			}
		}
	}
	return 0;
}

Int_t EcalKX3DRec::TransFit(Double_t &par, Double_t &par_err, Double_t &L0, Double_t &L1, Double_t &L2){

	Int_t i, n1, n2, flag, iter;

	Double_t param[7], lhood[7], pmin, lmin;

	Double_t lkhd1, lkhd2;

	flag = iter = 0;
	if(KX==-90||X0==-90||KY==-90||Y0==-90)return -1;

fit_repeat:

	// parameter - central value
	param[3] = par;
	ShowerModel();
	lhood[3] = GetLkhd(lkhd1, lkhd2);
	if(DEBUG) printf("-->TransFit(): param[%d]=%8.4f, l1=%8.4f, l2=%8.4f\n", 3, par, lkhd1, lkhd2);
//	if(lhood[3] ==-9999) return -1;
	if ( iter == 0 ) L0 = lhood[3];
	if ( par_err == 0 ){ L1 = L0; L2 = lkhd2; return 0; }

	// parameter - low values
	n1 = 3;
	while ( n1 > 0 ) {
		n1 -= 1;
		param[n1] = param[n1+1] - (3.-n1)*(par_err);
		par = param[n1];
		ShowerModel();
		lhood[n1] = GetLkhd(lkhd1, lkhd2);
		if(DEBUG) printf("-->TransFit(): param[%d]=%8.4f, l1=%8.4f, l2=%8.4f\n", n1, par, lkhd1, lkhd2);
		if ( lhood[n1] > lhood[n1+1] ) break;
	}

	// parameter - high values
	n2 = 3;
	while ( n2 < 6 ) {
		n2 += 1;
		param[n2] = param[n2-1] + (n2-3.)*(par_err);
		par = param[n2];
		ShowerModel();
		lhood[n2] = GetLkhd(lkhd1, lkhd2);
		if(DEBUG) printf("-->TransFit(): param[%d]=%8.4f, l1=%8.4f, l2=%8.4f\n", n2, par,lkhd1, lkhd2);
		if ( lhood[n2] > lhood[n2-1] ) break;
	}
// normalize parameters and likelihoods
	pmin = param[3];
	lmin = lhood[3];
	for (i=n1; i<=n2; i++ ) {
		if ( lhood[i] < lmin ) {
			pmin = param[i];
			lmin = lhood[i];
		}
	}
	for (i=n1; i<=n2; i++ ) {
		param[i] -= pmin;
		lhood[i] -= lmin;
	}

	flag = ParabolaMin(n1, n2, param, lhood, L1, par, par_err);
	par += pmin;
	L1  += lmin;

	if(DEBUG) printf("-->TransFit(): iter=%02d, par=%8.4f +/- %8.4f\n", iter, par, par_err);
	iter++;
	if( flag == 0 && iter < EcalKX3DRecDB::MaxIteration_Element ) goto fit_repeat;


	// check the lkhd calculated from fitted parameter
	ShowerModel();
	L1 = GetLkhd(lkhd1, lkhd2);
	if ( L1 > lmin ) {
		// parabola minimum is slightly shifted
		if(DEBUG) printf("-->TransFit(set to pmin): lmin=%8.4f, L1=%8.4f, L2=%8.4f, par=%8.4f +/- %8.4f\n", lmin, L1, lkhd2, par, par_err);
		L1       = lmin;
		par      = pmin;
	}
	L2 = lkhd2;

	return 0;
}

/***********************************************
 * Calculate energy deposition for a give shower
 ***********************************************/
Int_t EcalKX3DRec::EcalShwrDef(Int_t k, Int_t flag) {
	// by default, flag=0
	Int_t l, m;

	// E0
	E0  = ShwrPar[k][0][0];
	dE0 = ShwrPar[k][0][1];
	// B0
	B0  = ShwrPar[k][1][0];
	dB0 = ShwrPar[k][1][1];
	// A0
	A0  = ShwrPar[k][2][0];
	dA0 = ShwrPar[k][2][1];
	// X0
	X0  = ShwrPar[k][3][0];
	dX0 = ShwrPar[k][3][1];
	// Y0
	Y0  = ShwrPar[k][4][0];
	dY0 = ShwrPar[k][4][1];
	// Z0
	Z0  = ShwrPar[k][5][0];
	dZ0 = ShwrPar[k][5][1];
	// KX
	KX  = ShwrPar[k][6][0];
	dKX = ShwrPar[k][6][1];
	// KY
	KY  = ShwrPar[k][7][0];
	dKY = ShwrPar[k][7][1];

	ShowerModel(0, flag); // default lateral parameters

	// copy expected values
	for (l=0; l<18; l++) {
		for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
			ShwrExp[k][l][m] = CellExp[l][m];
			ShwrBeta[k][l][m] = CellBeta[l][m];
			if(flag==1){ // after all the fits done
			  	ShwrEtr[k][l][m] = CellEtr[l][m];
				ShwrDepth[k][l][m] = CellDepth[l][m];
				ShwrDist[k][l][m] = CellDist[l][m];
			}
		}
	}
	return 0;
}

Int_t EcalKX3DRec::TransFitKx(Double_t &L0, Double_t &L1, Double_t &L2){
	Int_t i, n1, n2, flag, iter;

	Double_t param[7], lhood[7], pmin, lmin;

	Double_t lkhd1, lkhd2;

	flag = iter = 0;
	if(KX==-90||X0==-90||KY==-90||Y0==-90)return -1;

fit_repeat:

	// parameter - central value
	param[3] = KX;
	lhood[3] = GetLkhd(lkhd1, lkhd2);
	if ( iter == 0 ) L0 = lhood[3];

	// parameter - low values
	n1 = 3;
	while ( n1 > 0 ) {
		n1 -= 1;
		param[n1] = param[n1+1] - (3.-n1)*(dKX);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][0]==1) {
				ShwrPar[i][6][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n1] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n1] > lhood[n1+1] ) break;
	}

	// parameter - high values
	n2 = 3;
	while ( n2 < 6 ) {
		n2 += 1;
		param[n2] = param[n2-1] + (n2-3.)*(dKX);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][0]==1) {
				ShwrPar[i][6][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n2] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n2] > lhood[n2-1] ) break;
	}
// normalize parameters and likelihoods
	pmin = param[3];
	lmin = lhood[3];
	for (i=n1; i<=n2; i++ ) {
		if ( lhood[i] < lmin ) {
			pmin = param[i];
			lmin = lhood[i];
		}
	}
	for (i=n1; i<=n2; i++ ) {
		param[i] -= pmin;
		lhood[i] -= lmin;
	}

	flag = ParabolaMin(n1, n2, param, lhood, L1, KX, dKX);
	KX += pmin;
	L1 += lmin;
	for (i=N_Shwr-1; i>-1; i--) {                                           // order matters
		if (ShwrDep[i][0]==1) {
			ShwrPar[i][6][0] = KX;
			ShwrPar[i][6][1] = dKX;
		}
	}
	iter++;
	for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
		if (ShwrDep[i][0]==1) {
			EcalShwrDef(i);
		}
	}
	if( flag == 0 && iter < EcalKX3DRecDB::MaxIteration_Element ) goto fit_repeat;

	// check the lkhd calculated from fitted parameter
	L1 = GetLkhd(lkhd1, lkhd2);
	if ( L1 > lmin ) {
		KX   = pmin;
		dKX *= 0.7;
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][0]==1) {
				ShwrPar[i][6][0] = KX;
				ShwrPar[i][6][1] = dKX;
				EcalShwrDef(i);
			}
		}
		L1 = GetLkhd(lkhd1, lkhd2);
	}
	L2 = lkhd2;

	return 0;
}

Int_t EcalKX3DRec::TransFitKy(Double_t &L0, Double_t &L1, Double_t &L2){
	Int_t i, n1, n2, flag, iter;

	Double_t param[7], lhood[7], pmin, lmin;

	Double_t lkhd1, lkhd2;

	flag = iter = 0;
	if(KX==-90||X0==-90||KY==-90||Y0==-90)return -1;

fit_repeat:

	// parameter - central value
	param[3] = KY;
	lhood[3] = GetLkhd(lkhd1, lkhd2);
	if ( iter == 0 ) L0 = lhood[3];

	// parameter - low values
	n1 = 3;
	while ( n1 > 0 ) {
		n1 -= 1;
		param[n1] = param[n1+1] - (3.-n1)*(dKY);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][1]==1) {
				ShwrPar[i][7][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n1] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n1] > lhood[n1+1] ) break;
	}

	// parameter - high values
	n2 = 3;
	while ( n2 < 6 ) {
		n2 += 1;
		param[n2] = param[n2-1] + (n2-3.)*(dKY);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][1]==1) {
				ShwrPar[i][7][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n2] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n2] > lhood[n2-1] ) break;
	}
// normalize parameters and likelihoods
	pmin = param[3];
	lmin = lhood[3];
	for (i=n1; i<=n2; i++ ) {
		if ( lhood[i] < lmin ) {
			pmin = param[i];
			lmin = lhood[i];
		}
	}
	for (i=n1; i<=n2; i++ ) {
//		if( DEBUG ) printf("-->TransFitKy(): param[%d]=%8.3f, lhood[%d]=%8.3f\n", i, param[i], i, lhood[i] );
		param[i] -= pmin;
		lhood[i] -= lmin;
	}

	flag = ParabolaMin(n1, n2, param, lhood, L1, KY, dKY);
	KY += pmin;
	L1 += lmin;
	for (i=N_Shwr-1; i>-1; i--) {                                           // order matters
		if (ShwrDep[i][1]==1) {
			ShwrPar[i][7][0] = KY;
			ShwrPar[i][7][1] = dKY;
		}
	}
	iter++;

	for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
		if (ShwrDep[i][1]==1) {
			EcalShwrDef(i);
		}
	}
	if( flag == 0 && iter < EcalKX3DRecDB::MaxIteration_Element ) goto fit_repeat;

	// check the lkhd calculated from fitted parameter
	L1 = GetLkhd(lkhd1, lkhd2);
	if ( L1 > lmin ) {
		KY   = pmin;
		dKX *= 0.7;
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][1]==1) {
				ShwrPar[i][7][0] = KY;
				ShwrPar[i][7][1] = dKY;
				EcalShwrDef(i);
			}
		}
		L1 = GetLkhd(lkhd1, lkhd2);
	}
	L2 = lkhd2;

	return 0;
}

Int_t EcalKX3DRec::ParabolaMin(Int_t n1, Int_t n2, Double_t param[], Double_t lhood[], Double_t &L1, Double_t &par, Double_t &error){
	Int_t i, flag = 0;

	Double_t m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, a33, det, alpha, val, err;
	// calculate inverse matrix a_ij
	m11 = m12 = m22 = m23 = m33 = b1 = b2 = b3 = 0.;
	for (i=n1; i<=n2; i++ ) {
		m11 += param[i]*param[i]*param[i]*param[i];
		m12 += param[i]*param[i]*param[i];
		m22 += param[i]*param[i];
		m23 += param[i];
		m33 += 1.;
		b1  += param[i]*param[i]*lhood[i];
		b2  += param[i]*lhood[i];
		b3  += lhood[i];
	}
	// m22 == m13 is used explicitely
	det = m11*m22*m33 + 2.*m12*m22*m23 - m22*m22*m22 - m12*m12*m33 - m23*m23*m11;

	if ( det != 0. ) {
		a11 = m22*m33 - m23*m23;
		a12 = m22*m23 - m12*m33;
		a13 = m12*m23 - m22*m22;
		a22 = m11*m33 - m22*m22;
		a23 = m12*m22 - m11*m23;
		a33 = m11*m22 - m12*m12;

		alpha = b1*a11 + b2*a12 + b3*a13;
		if ( alpha > 0. ) {
			val = -0.5*(b1*a12 + b2*a22 + b3*a23)/alpha;
			err = 0.7*sqrt(det/alpha);
			L1  = (b1*a13 + b2*a23 + b3*a33 - val*val*alpha)/det;
			flag = 1;
		} else {
			if ( lhood[n1] < lhood[n2] ) {
				val = param[n1];
				L1  = lhood[n1];
			} else if ( lhood[n2] < lhood[n1] ) {
				val = param[n2];
				L1  = lhood[n2];
			} else {
				val = param[3];
				L1  = lhood[3];
			}
			err = 2.*error;
		}
	} else {
		if ( lhood[n1] < lhood[n2] ) {
			val = param[n1];
			L1  = lhood[n1];
		} else if ( lhood[n2] < lhood[n1] ) {
			val = param[n2];
			L1  = lhood[n2];
		} else {
			val = param[3];
			L1  = lhood[3];
		}
		err = 2.*error;
	}

	if ( TMath::Abs(val-param[3]) > 6.*(error) ) {
		flag = 0;
		if ( lhood[n1] < lhood[n2] ) {
			val = param[n1];
			L1  = lhood[n1];
		} else {
			val = param[n2];
			L1  = lhood[n2];
		}
		err = 2.*error;
	}

	par = val;
	error = err;
	return flag;
}

Double_t EcalKX3DRec::GetLkhd(Double_t &L1, Double_t &L2){
	// L1: from shower fluctuation
	// L2: from boundary conditions
	ShowerProbability();
	Int_t i, j;
	Double_t x, y, z, val=0;
	L1 = L2 = 0;
	for (i=0; i<18; i++){
		if( Cell_Max[i] < Cell_Min[i] ) continue;
		for (j=Cell_Min[i]; j<=Cell_Max[i]; j++) {
			if ( CellProb[i][j] > 0. )	L1 += CellProb[i][j];
		}
		// ad hoc boundary conditions, to contain shower axis within footprint
		z = EcalKX3DRecDB::EcalZ[i];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		if ( (i/2)%2 ) {
			if      ( (val=x-(Cell_Min[i]-40)*EcalKX3DRecDB::Cell_Size) < 0. )  L2 += val*val;
			else if ( (val=(Cell_Max[i]-39)*EcalKX3DRecDB::Cell_Size-x) < 0. )  L2 += val*val;
		} else {
			if      ( (val=y-(Cell_Min[i]-40)*EcalKX3DRecDB::Cell_Size) < 0. )  L2 += val*val;
			else if ( (val=(Cell_Max[i]-39)*EcalKX3DRecDB::Cell_Size-y) < 0. )  L2 += val*val;
		}
	}
	// ad hoc boundary conditions
	if ( (val=A0-Z0-3.)  < 0. ) L2 += val*val;
	if ( (val=A0+138.)   > 0. ) L2 += val*val*val*val;
	if ( (val=A0+155.)   < 0. ) L2 += val*val*1e2;
	if ( (val=Z0+142.)   > 0. ) L2 += val*val*1e2;
	if ( (val=Z0+160.)   < 0. ) L2 += val*val;
	if ( (val=E0-0.20)   < 0. ) L2 += val*val*1e4;
	return L1+L2;
}

/***********************************************
 * footprint to caluclate the expected energy
 * after the fit is done
 ***********************************************/
Int_t EcalKX3DRec::GetFootPrintSize(Double_t energy){
	return 4.5*TMath::Log(E0+6);// footprint size // Mar. 9, 2017
}
Int_t EcalKX3DRec::ShowerFootprint(bool IsExtend) {

	Int_t    k, l, m;
	Double_t x, y, z;

	//
	// Define shower footprint
	//
//	k = 4.0 + 2.6*TMath::Log(E0+2);// footprint size
	k = GetFootPrintSize(E0);
	for (l=0; l<18; l++) {
		z = EcalKX3DRecDB::EcalZ[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		if ( (l/2)%2 ) m = (Int_t) ((x +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
		else           m = (Int_t) ((y +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
		if ( m>=0 && m<EcalKX3DRecDB::kNC ) {
			Cell_Med[l] = m;
			Shwr_Med[I_Shwr][l] = m;
			Cell_Min[l] = TMath::Max( 0, m-k);
			Cell_Max[l] = TMath::Min(79, m+k);
		} else {
			Cell_Med[l] = Shwr_Med[I_Shwr][l] = -1;
			Cell_Min[l] = 79;
			Cell_Max[l] = 0;
		}
	}
	if( IsExtend ){
		// to include the secondary showers, to be called in Summary()
		for(Int_t ish=0; ish<N_Shwr; ish++){
			double _E0 = ShwrPar[ish][0][0],
					 _X0 = ShwrPar[ish][3][0],
					 _Y0 = ShwrPar[ish][4][0],
					 _Z0 = ShwrPar[ish][5][0],
					 _KX = ShwrPar[ish][6][0],
					 _KY = ShwrPar[ish][7][0];
			k = GetFootPrintSize(_E0);
			for(l=0; l<18; l++){
				if( ish==0 ){ // Initialization
					Cell_Med[l] = Shwr_Med[I_Shwr][l] = -1;
					Cell_Min[l] = 79;
					Cell_Max[l] = 0;
				}
				z = EcalKX3DRecDB::EcalZ[l];
				x = _X0 + _KX*(z-_Z0);
				y = _Y0 + _KY*(z-_Z0);
				if ( (l/2)%2 ) m = (Int_t) ((x +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
				else           m = (Int_t) ((y +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
				if ( m>=0 && m<EcalKX3DRecDB::kNC ) {
					Shwr_Med[ish][l] = m;
					if( ish == I_Shwr ) Cell_Med[l] = m; // primary shower
					int min = TMath::Max( 0, m-k);
					int max = TMath::Min(79, m+k);
					if( min < Cell_Min[l] ) Cell_Min[l] = min;
					if( max > Cell_Max[l] ) Cell_Max[l] = max;
				}
			}
		}
		if( DEBUG ){
			cout << "----------- ShowerFootprint(Extended) -------------" << endl;
			for(l=0; l<18; l++){
				cout << Form("l %d, Cell_Min=%d, Cell_Med=%d, Cell_Max=%d", l, Cell_Min[l], Cell_Med[l], Cell_Max[l]) << endl;
			}
			cout << "---------------------------------------------------" << endl;
		}
	}
	return 0;
}

/************************************************************
 *  main fit function
 *  1) find shower
 *  2) fit shower parameters
 *  3) subtract fited shower
 ************************************************************/
Int_t EcalKX3DRec::ShowerRec(Int_t n){
	Int_t    l, m, flag_x, flag_y, Iter, Iter_Sum=0;
	Double_t l_start=FLT_MAX, l_end, converge_limit;
	N_Shwr = n;
	while ( N_Shwr < EcalKX3DRecDB::kNS ) {
		dEX = EX = 0.;
		fX1 = fX0 = fEX = fEX0 = 0;
		dEY = EY = 0.;
		fY1 = fY0 = fEY = fEY0 = 0;
		// define new shower
		if ( ShowerEstimate() == 0 ) break;
		SetLatPar( N_Shwr, E0 );
		SetPdfPar( N_Shwr, E0 );
		//if( ShwrApexI[0] > 1 ) break;
		if(DEBUG) printf("Check footprint: I_Shwr=%d,\n Cell_Min[0]=%d, Cell_Max[0]=%d, Cell_Min[17]=%d, Cell_Max[17]=%d,\n Cell_Min[2]=%d, Cell_Max[2]=%d, Cell_Min[15]=%d, Cell_Max[15]=%d\n", N_Shwr, Cell_Min[0], Cell_Max[0], Cell_Min[17], Cell_Max[17], Cell_Min[2], Cell_Max[2], Cell_Min[15], Cell_Max[15]);

		// quality of the new shower
		if      ( dEX>EcalKX3DRecDB::Cutoff_SEED && fX1>=EcalKX3DRecDB::Cutoff_NPL && fEX>=EcalKX3DRecDB::Cutoff_NNB ) flag_x = 4;
		else if ( dEX>EcalKX3DRecDB::Cutoff_SEED && fX0>=EcalKX3DRecDB::Cutoff_NPL && fX1>EcalKX3DRecDB::Cutoff_NPL/2 && fEX>=EcalKX3DRecDB::Cutoff_NNB ) flag_x = 3;
		else if ( dEX>EcalKX3DRecDB::Cutoff_SEED && fX1>EcalKX3DRecDB::Cutoff_NPL/2 && fEX>=EcalKX3DRecDB::Cutoff_NNB && fEX0>=EcalKX3DRecDB::Cutoff_NNB) flag_x = 2;
		else if ( dEX>EcalKX3DRecDB::Cutoff_MIP  && fX0>=2 && fX1>=2 &&  EX>dEX		 ) flag_x = 1;
		else                                           				  	   flag_x = 0;

		if      ( dEY>EcalKX3DRecDB::Cutoff_SEED && fY1>=EcalKX3DRecDB::Cutoff_NPL && fEY>=EcalKX3DRecDB::Cutoff_NNB ) flag_y = 4;
		else if ( dEY>EcalKX3DRecDB::Cutoff_SEED && fY0>=EcalKX3DRecDB::Cutoff_NPL && fY1>EcalKX3DRecDB::Cutoff_NPL/2 && fEY>EcalKX3DRecDB::Cutoff_NNB ) flag_y = 3;
		else if ( dEY>EcalKX3DRecDB::Cutoff_SEED && fY1>EcalKX3DRecDB::Cutoff_NPL/2 && fEY>=EcalKX3DRecDB::Cutoff_NNB && fEY0>=EcalKX3DRecDB::Cutoff_NNB) flag_y = 2;
		else if ( dEY>EcalKX3DRecDB::Cutoff_MIP  && fY0>=2 && fY1>=2 && EY>dEY       ) flag_y = 1; // increase MIP reconstruction efficiency
		else                                          				       flag_y = 0;

		ShwrFlag[N_Shwr] = flag_x*10 + flag_y;
		if( flag_y>=2 && flag_x==1 && EX/EY>0.4 && EX/EY<2.5 && dEX/dEY>0.4 && dEX/dEY<2.5 ){// well matched X cluster
		  	flag_x = flag_y;
			ShwrFlag[N_Shwr] += 100;
		}
		if( EX<=0 || dEX<=0 ){ flag_x = -1; X0 = 40; KX = 0; }
		if( EY<=0 || dEY<=0 ) flag_y = -1;
		if( N_Shwr>0 ){
		  	if( flag_x < flag_y && dEX/dEY<0.2 ){ // bad X cluster, set to the primary shower
				flag_x = 0; X0 = ShwrPar[0][3][0]; KX = ShwrPar[0][6][0];
			}
			if( flag_y < flag_x && dEY/dEX<0.2 ){ // bad Y cluster, set to the primary shower
				flag_y = 0; Y0 = ShwrPar[0][4][0]; KY = ShwrPar[0][7][0];
			}
		}
//		if( EX<2*dEX ) flag_x = 0; // if the seed is over-hot
//		if( EY<2*dEY ) flag_y = 0;
		//
		if(DEBUG){
			printf("-->ShowerRec(%d)\n", N_Shwr);
			printf("X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, T0 %8.3f\n", X0, KX, Y0, KY, Z0, E0, A0, T0);
			printf("dX %8.3f, dKX %8.3f, dY %8.3f, dKY %8.3f, dZ %8.3f, dE %8.2f, dA %8.3f, dT %8.3f\n", dX0, dKX, dY0, dKY, dZ0, dE0, dA0, dT0);
			printf("fX0 %02d, fX1 %02d, fEX %02d, fEX0 %02d, EX %7.3f, dEX %7.3f, flagX %d\n", fX0, fX1, fEX, fEX0, EX, dEX, flag_x);
			printf("fY0 %02d, fY1 %02d, fEY %02d, fEY0 %02d, EY %7.3f, dEY %7.3f, flagY %d\n", fY0, fY1, fEY, fEY0, EY, dEY, flag_y);
		}
		//
		// is there a compelling shower?
		if( N_Shwr==0 && flag_x+flag_y<2 ) break;
		if( N_Shwr>0  && flag_x<2 && flag_y<2 ) break;
//		if( N_Shwr>0  && EX<EcalKX3DRecDB::Cutoff_ETOT/2 && EY<EcalKX3DRecDB::Cutoff_ETOT/2 ) break;

		if( N_Shwr==0 ){ // preselection to reduce the CPU time of processing hadronic showers
			D0 = LayAmp[0]; D1 = LayAmp[1]; D2 = LayAmp[2];
			EseedX = dEX/EX;			EseedY = dEY/EY;
			if( LayAmp[0] + LayAmp[1] < 0.01*(log(E0)-1) 					  ) IsHadronLike |= 2;
			if( LayAmp[2] < 0.03*(log(E0)-1)             					  ) IsHadronLike |= 4;
			if( TotNhits - 80*log(2+E0) > 80                              ) IsHadronLike |= 8;
			if( Z0/(1-0.024*(KX*KX+KY*KY)) + 1.03*log(E0+1) + 146.5 < -5  ) IsHadronLike |= 16;
			if( El2f > 0.05 + 0.025*log(1+E0)                             ) IsHadronLike |= 32;
			if( EseedX > 0.3 + 0.25*exp(-E0/5.) + 0.25*exp(-E0)           ) IsHadronLike |= 64;
			if( EseedY > 0.3 + 0.35*exp(-E0/5.) + 0.25*exp(-E0)           ) IsHadronLike |= 128;
			CalcShowerSize();
			if( sx > 4.5 + 0.5*exp(-E0/10.) + 0.5*exp(-E0/3.)             ) IsHadronLike |= 256;
			if( sy > 4.5 + 1.5*exp(-E0/10.) + 1.5*exp(-E0/3.)             ) IsHadronLike |= 512;
		}

//		if( N_Shwr>0 && A0 < -146 ) break; // first 4 layers
		converge_limit = EcalKX3DRecDB::MinDeltaLkhd + EcalKX3DRecDB::DeltaLkhd_LowEnergy*exp(-E0/EcalKX3DRecDB::DeltaLkhd_TransEnergy);

		// define parameters of the new shower
		if ( N_Shwr == 0 ) {
			// no dependences for the first shower
			ShwrDep[N_Shwr][0] = 0;
			ShwrDep[N_Shwr][1] = 0;
		} else {
			// Corrections for not-well-defined showers - X dependences
			if ( flag_x>=2 ) {
				if ( TMath::Abs(ShwrPar[0][3][0]-X0) > 2. ) { // distance between 2 showers is larger than 2 cm
					// dependences
					ShwrDep[N_Shwr][0] = 0;
				} else {
					ShwrDep[0][0]      = 1;
					ShwrDep[N_Shwr][0] = 1;
					// KX correction, assuming both showers have the same direction
					KX  = ShwrPar[0][6][0];
					dKX = ShwrPar[0][6][1];
					// combine footprints
					for (l=2; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				}
			} else if ( flag_x==1 ) {
				// dependences
				ShwrDep[0][0]      = 1;
				ShwrDep[N_Shwr][0] = 1;
				// KX correction
				KX  = ShwrPar[0][6][0];
				dKX = ShwrPar[0][6][1];
				// combine footprints
				for (l=2; l<18; l+=4) {
					Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
					Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
					Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
					Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
				}
			} else {
				// dependences
				ShwrDep[0][0]      = 1;
				ShwrDep[N_Shwr][0] = 1;
				// KX correction
				KX  = ShwrPar[0][6][0];
				dKX = ShwrPar[0][6][1];
				// X0 correction
				if ( EX>EcalKX3DRecDB::Cutoff_SEED && TMath::Abs(ShwrPar[0][3][0]-X1)<2.) {
					X0   = 0.5*(X0+X1);
					dX0 += 0.5;
					// combine footprints
					for (l=2; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				} else {
					X0  = ShwrPar[0][3][0];
					dX0 = ShwrPar[0][3][1]+0.5;
					// redefine footprint
					for (l=2; l<18; l+=4) {
						Cell_Min[l]   = ShwrFprt[0][l][0];
						Cell_Max[l]   = ShwrFprt[0][l][1];
						Cell_Min[l+1] = ShwrFprt[0][l+1][0];
						Cell_Max[l+1] = ShwrFprt[0][l+1][1];
					}
				}
			}

			// Corrections for not-well-defined showers - Y dependences
			if ( flag_y>=2 ) {
				if ( TMath::Abs(ShwrPar[0][4][0]-Y0) > 2. ) {
					ShwrDep[N_Shwr][1] = 0;
				} else {
					ShwrDep[0][1]      = 1;
					ShwrDep[N_Shwr][1] = 1;
					// KX correction
					KY  = ShwrPar[0][7][0];
					dKY = ShwrPar[0][7][1];
					// combine footprints
					for (l=0; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				}
			} else if ( flag_y==1 ) {
				// dependences
				ShwrDep[0][1]      = 1;
				ShwrDep[N_Shwr][1] = 1;
				// KY correction
				KY  = ShwrPar[0][7][0];
				dKY = ShwrPar[0][7][1];
				// combine footprints
				for (l=0; l<18; l+=4) {
					Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
					Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
					Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
					Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
				}
			} else {
				// dependences
				ShwrDep[0][1]      = 1;
				ShwrDep[N_Shwr][1] = 1;
				// KY correction
				KY  = ShwrPar[0][7][0];
				dKY = ShwrPar[0][7][1];
				// Y0 correction
				if ( EY>EcalKX3DRecDB::Cutoff_SEED && TMath::Abs(ShwrPar[0][4][0]-Y1)<2.) {
					Y0   = 0.5*(Y0+Y1);
					dY0 += 0.5;
					// combine footprints
					for (l=0; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				} else {
					Y0  = ShwrPar[0][4][0];
					dY0 = ShwrPar[0][4][1]+0.5;
					// redefine footprint
					for (l=0; l<18; l+=4) {
						Cell_Min[l]   = ShwrFprt[0][l][0];
						Cell_Max[l]   = ShwrFprt[0][l][1];
						Cell_Min[l+1] = ShwrFprt[0][l+1][0];
						Cell_Max[l+1] = ShwrFprt[0][l+1][1];
					}
				}
			}
		}

		// perform initial fit of the shower
		I_Shwr = N_Shwr; // index of current shower
		ShwrApexI[I_Shwr] = Apex;
		// Initial values
		ShwrE0f[N_Shwr]  = E0;
		ShwrX0f[N_Shwr]  = X0;
		ShwrY0f[N_Shwr]  = Y0;
		ShwrZ0f[N_Shwr]  = Z0;
		ShwrKXf[N_Shwr]  = KX;
		ShwrKYf[N_Shwr]  = KY;
		ShwrEl2f[N_Shwr] = El2f;
		// start fit
		Double_t L0, L1, L2;
		Iter = 0;
		if( IsHadronLike ) Iter = EcalKX3DRecDB::MaxIteration-1; // only allow one iteration for hadronic shower
rec_refit:
		Iter++;

		// E0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit E0 \n", I_Shwr);
		TransFit( E0,  dE0, L0, L1, L2);
		l_start = L0;

		if( fZ0 ){
			// Z0 fit
			if(DEBUG) printf("-->ShowerRec(%d): Fit Z0 \n", I_Shwr);
			double Z0_old = Z0;
			TransFit( Z0,  dZ0, L0, L1, L2);
			X0 += KX*(Z0 - Z0_old); Y0 += KY*(Z0 - Z0_old);
		}

		// A0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit A0 \n", I_Shwr);
		TransFit( A0,  dA0, L0, L1, L2);

		if(flag_x>=1){
			// X0 fit
			if(DEBUG) printf("-->ShowerRec(%d): Fit X0 \n", I_Shwr);
			TransFit( X0,  dX0, L0, L1, L2);

			// KX fit, for first shower and non-overlaped secondary shower
			if(DEBUG) printf("-->ShowerRec(%d): Fit KX \n", I_Shwr);
			if ( ShwrDep[N_Shwr][0]==0 ) TransFit( KX, dKX, L0, L1, L2);
		}
		if(flag_y>=1){
			// Y0 fit
			if(DEBUG) printf("-->ShowerRec(%d): Fit Y0 \n", I_Shwr);
			TransFit( Y0,  dY0, L0, L1, L2);

			// KY fit, for first shower and non-overlaped secondary shower
			if(DEBUG) printf("-->ShowerRec(%d): Fit KY \n", I_Shwr);
			if ( ShwrDep[N_Shwr][1]==0 ) TransFit( KY, dKY, L0, L1, L2);
		}
		l_end = L1;
		if(DEBUG)printf("First fit, Shower %d, Iteration %04d, Likelihood Start: %8.3f, End %8.3f, Wall %8.3f\n", N_Shwr, Iter, l_start, l_end, L2);
		if ( l_start - l_end > converge_limit && Iter < EcalKX3DRecDB::MaxIteration ) goto rec_refit;
		if( N_Shwr==0 )      Iter_Sum += Iter;
		else if( N_Shwr==1 ) Iter_Sum += Iter*100;
		else if( N_Shwr==2 ) Iter_Sum += Iter*10000;

		//
		// **************************
		//  store shower parameters
		// **************************
		// E0
		ShwrPar[N_Shwr][0][0] =  E0;
		ShwrPar[N_Shwr][0][1] = dE0;
		// B0
		ShwrPar[N_Shwr][1][0] =  B0;
		ShwrPar[N_Shwr][1][1] = dB0;
		// A0
		ShwrPar[N_Shwr][2][0] =  A0;
		ShwrPar[N_Shwr][2][1] = dA0;
		// X0
		ShwrPar[N_Shwr][3][0] =  X0;
		ShwrPar[N_Shwr][3][1] = dX0;
		// Y0
		ShwrPar[N_Shwr][4][0] =  Y0;
		ShwrPar[N_Shwr][4][1] = dY0;
		// Z0
		ShwrPar[N_Shwr][5][0] =  Z0;
		ShwrPar[N_Shwr][5][1] = dZ0;
		// KX
		ShwrPar[N_Shwr][6][0] =  KX;
		ShwrPar[N_Shwr][6][1] = dKX;
		// KY
		ShwrPar[N_Shwr][7][0] =  KY;
		ShwrPar[N_Shwr][7][1] = dKY;

		// Likelihood
		ShwrLkhd[N_Shwr]      =  L2;
		ShwrLkhdI[N_Shwr]     =  L1;

		// Buffer E0
		ShwrEb[N_Shwr][0] = E0;
		ShwrEb[N_Shwr][1] = dE0;

		// Cell Min and Max, clear ShwrExp[]
		for (l=0; l<18; l++) {
			ShwrFprt[N_Shwr][l][0] = Cell_Min[l];
			ShwrFprt[N_Shwr][l][1] = Cell_Max[l];
			for (m=3; m<=76; m++) ShwrExp[N_Shwr][l][m] = 0.;
		}

		// increment number of showers
		N_Shwr += 1;

		// subtract the newly defined shower
		ShowerFootprint(); // larger than the footprint for fit, which was defined in ecal_shower_estimate()
		ShowerModel(0, 0); // default parameters
		if(N_Shwr==2){
			ShowerProbability(1);
			LkhdTail1 = 0;
			NhitTail1 = 0;
			for(l=2; l<18; l++){
				if( Cell_Med[l]<0 ) continue;
				for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
					if( CellProb[l][m]>0 ){
						Int_t m_m = 2*Cell_Med[l] - m;
						bool IsLeak = ( m_m<Cell_Min[l] || m_m>Cell_Max[l] );
						Bool_t IsTail=1;
						// Check all fitted showers
						for(Int_t ish=0; ish<N_Shwr; ish++){
							if( m==Shwr_Med[ish][l] || m==Shwr_Med[ish][l]-1 || m==Shwr_Med[ish][l]+1 ){
								IsTail = 0;
							}
						}
						if( IsTail ){
							LkhdTail1 += ( (1+IsLeak)*CellProbNorm[l][m] );
							NhitTail1 += ( 1+IsLeak );
						}
					}
				}
			}
			Double_t Esum = ShwrPar[0][0][0] + ShwrPar[1][0][0];
			if( LkhdTail1/NhitTail1 > 0.9 - 0.2*exp(-Esum/20.) ){
			  	IsHadronLike |= 1024; //
			}
		}

		// test whether more showers needed (energy asymmetry, MIP near apex, ....)
		if ( N_Shwr>0 ) {
			// general
			if( N_Shwr==1 && (E0<=0.3 || Z0<EcalKX3DRecDB::EcalZ[17] || Z0>EcalKX3DRecDB::EcalZ[1] || A0<EcalKX3DRecDB::EcalZ[3]) ) IsHadronLike |= 2048;
			if( IsHadronLike )	IsSkipCombineFit = 1; //skip combinefit for hardon-like shower
//			if( IsHadronLike>1 && N_Shwr>=2 ) break; // external hadronic flag
		}


		for (l=0; l<18; l++) {
			for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
				ShwrExp[N_Shwr-1][l][m] = CellExp[l][m];
				ShwrEtr[N_Shwr-1][l][m] = CellEtr[l][m];
				ShwrBeta[N_Shwr-1][l][m] = CellBeta[l][m];
			}
		}

		for (l=0; l<18; l++) {
			for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
				// subtract defined shower
				if ( CellBeta[l][m] > 0. && CellExp[l][m]>1e-4 ) { // 0.1 MeV
					double sigmaE = sqrt(CellExp[l][m]/CellBeta[l][m]);
					CellDepS1[l][m] -= CellExp[l][m] + EcalKX3DRecDB::NSigma_Subtraction*sigmaE; // to suppress fluctuations
					CellDepS0[l][m] -= CellExp[l][m];
					// subtract more for central cells
					if( m == Cell_Med[l] || m+1 == Cell_Med[l] || m-1 == Cell_Med[l] ){
						CellDepS1[l][m] -= sigmaE;
					}
					if ( CellDepS1[l][m] < 0. ) CellDepS1[l][m] = 0.;
					if ( CellDepS0[l][m] < 0. ) CellDepS0[l][m] = 0.;
				}
			}
		}

	} // end of while loop, finished shower finding

	if ( N_Shwr > 1 ) DefineCommonFootprint(); // for combined fit
	else{
		for(l=0; l<EcalKX3DRecDB::kNL; l++){
			Cell_Min[l] = TMath::Max(    4, ShwrFprt[0][l][0]-2);
			Cell_Max[l] = TMath::Min(EcalKX3DRecDB::kNC-4, ShwrFprt[0][l][1]+2);
		}
	}
	// restore single shower parameters
	memcpy(ShwrParI, ShwrPar, sizeof(ShwrPar));
	return Iter_Sum;
}


Int_t EcalKX3DRec::ShowerCombineFit(){
	if( IsSkipCombineFit ) return 0;

	Int_t    l, m, k, n, Iter=0;
	Double_t l_start=FLT_MAX, l_end;
	Double_t L0, L1, L2;

	Double_t Esum = ShwrPar[0][0][0] + ShwrPar[1][0][0];

	Double_t converge_limit = EcalKX3DRecDB::MinDeltaLkhd + EcalKX3DRecDB::DeltaLkhd_LowEnergy*exp(-Esum/EcalKX3DRecDB::DeltaLkhd_TransEnergy);

	// only to improve the accuracy for 2-shower events
	// 3 shower events are almost hadronic one, no need to waste time
	if ( N_Shwr>=2 && (ShwrDep[0][0]>0 || ShwrDep[0][1]>0) ) {

		if(DEBUG) printf("Check footprint: Cell_Min[0]=%d, Cell_Max[0]=%d, Cell_Min[15]=%d, Cell_Max[15]=%d,\n Cell_Min[2]=%d, Cell_Max[2]=%d, Cell_Min[17]=%d, Cell_Max[17]=%d\n", Cell_Min[0], Cell_Max[0], Cell_Min[15], Cell_Max[15], Cell_Min[2], Cell_Max[2], Cell_Min[17], Cell_Max[17]);
		// define energy deposition and fluctuation for all the showers, the footprint is common for all the showers
		for (n=0; n<N_Shwr; n++) {
			if ( ShwrDep[n][0]>0 || ShwrDep[n][1]>0 ) EcalShwrDef(n);
			if ( ShwrDep[n][0]>0 ) N_Dep += 10;
			if ( ShwrDep[n][1]>0 ) N_Dep += 1;
		}

		Iter = 0;

shower_refit:
		//    printf("\n");

		Iter += 1;

		for (n=0; n<N_Shwr; n++) {

			if ( ShwrDep[n][0]>0 || ShwrDep[n][1]>0 ) {

				// E0
				E0  = ShwrPar[n][0][0];
				dE0 = ShwrPar[n][0][1];
				// B0
				B0  = ShwrPar[n][1][0];
				dB0 = ShwrPar[n][1][1];
				// A0
				A0  = ShwrPar[n][2][0];
				dA0 = ShwrPar[n][2][1];
				// X0
				X0  = ShwrPar[n][3][0];
				dX0 = ShwrPar[n][3][1];
				// Y0
				Y0  = ShwrPar[n][4][0];
				dY0 = ShwrPar[n][4][1];
				// Z0
				Z0  = ShwrPar[n][5][0];
				dZ0 = ShwrPar[n][5][1];
				// KX
				KX  = ShwrPar[n][6][0];
				dKX = ShwrPar[n][6][1];
				// KY
				KY  = ShwrPar[n][7][0];
				dKY = ShwrPar[n][7][1];

				I_Shwr = n;

				// E0 fit
				TransFit( E0,  dE0, L0, L1, L2);
				ShwrPar[n][0][0] =  E0;
				ShwrPar[n][0][1] = dE0;
				if(DEBUG)printf("-->ShowerCombineFit() Fit E0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);


				if ( n==0 ) { //
					l_start = L0; // fluctuation+boundary likelihood of first shower
					// + boundary likelihoods of other showers
					for (k=1; k<N_Shwr; k++) if ( ShwrDep[k][0]>0 || ShwrDep[k][1]>0 ) l_start += ShwrLkhd[k];
				}

				if( ShwrFlag[n]>=10 ){ // x defined
					// X0 fit
					TransFit( X0,  dX0, L0, L1, L2);
					ShwrPar[n][3][0] =  X0;
					ShwrPar[n][3][1] = dX0;
					if(DEBUG)printf("-->ShowerCombineFit() Fit X0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

					// KX fit
					if ( ShwrDep[n][0] == 0 ) TransFit( KX, dKX, L0, L1, L2);
					else if ( n == 0 )         TransFitKx(L0, L1, L2); // only fit for first shower, commonly applied to other showers
					ShwrPar[n][6][0] =  KX;
					ShwrPar[n][6][1] = dKX;
					if(DEBUG)printf("-->ShowerCombineFit() Fit KX:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);
				}
				if( ShwrFlag[n]%10 >= 0 ){ // y defined
					// Y0 fit
					TransFit( Y0,  dY0, L0, L1, L2);
					ShwrPar[n][4][0] =  Y0;
					ShwrPar[n][4][1] = dY0;
					if(DEBUG)printf("-->ShowerCombineFit() Fit Y0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

					// KY fit
					if ( ShwrDep[n][1] == 0 ) TransFit( KY, dKY, L0, L1, L2);
					else if ( n == 0 )         TransFitKy(L0, L1, L2);
					ShwrPar[n][7][0] =  KY;
					ShwrPar[n][7][1] = dKY;
					if(DEBUG)printf("-->ShowerCombineFit() Fit KY:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);
				}
				// Z0 fit
				TransFit( Z0,  dZ0, L0, L1, L2);
				ShwrPar[n][5][0] =  Z0;
				ShwrPar[n][5][1] = dZ0;
				if(DEBUG)printf("-->ShowerCombineFit() Fit Z0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

				// A0 fit
				TransFit( A0,  dA0, L0, L1, L2);
				ShwrPar[n][2][0] =  A0;
				ShwrPar[n][2][1] = dA0;

				if(DEBUG)printf("-->ShowerCombineFit() Fit A0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);
				// copy expected values
				for (l=0; l<18; l++) {
					for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
						ShwrExp[n][l][m] = CellExp[l][m];
						ShwrBeta[n][l][m] = CellBeta[l][m];
						if( TMath::IsNaN( CellExp[l][m] ) ){
							printf("Line 3274: l=%d, m=%d, CellExp=%g\n", l, m, CellExp[l][m]);
						}
					}
				}

				ShwrLkhd[n] = L2; // restore boundary likelihoods
				//  ShowerPrint(n);
			}
		}

		l_end = L1 - L2; // only the cell fluctuation
		for (k=0; k<N_Shwr; k++) if ( ShwrDep[k][0]>0 || ShwrDep[k][1]>0 ) l_end += ShwrLkhd[k]; // including boundary conditions

		if(DEBUG)printf("-->ShowerCombineFit(): Iteration %04d, Likelihood Start: %8.3f, End %8.3f\n", Iter, l_start, l_end);
		if ( l_start - l_end > converge_limit && Iter < EcalKX3DRecDB::MaxIteration ) goto shower_refit;
	}
	return Iter;
}

void EcalKX3DRec::PrintShwrPar(){
	for(Int_t n=0; n<N_Shwr; n++){
		printf("Shower %d\n", n);
		printf("X0 %8.3f, KX0 %8.3f, Y0 %8.3f, KY0 %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f B0 %8.3f\n",
				ShwrPar[n][3][0], ShwrPar[n][6][0], ShwrPar[n][4][0], ShwrPar[n][7][0], ShwrPar[n][5][0], ShwrPar[n][0][0], ShwrPar[n][2][0], ShwrPar[n][1][0]);
		printf("dX %8.3f, dKX %8.3f, dY %8.3f, dKY %8.3f, dZ %8.3f, dE %8.2f, dA %8.3f dB %8.3f\n",
				ShwrPar[n][3][1], ShwrPar[n][6][1], ShwrPar[n][4][1], ShwrPar[n][7][1], ShwrPar[n][5][1], ShwrPar[n][0][1], ShwrPar[n][2][1], ShwrPar[n][1][1]);
	}
}

Int_t EcalKX3DRec::ShowerPrint(Int_t n){

	Int_t i, j, k;
	Double_t v, w;

	Int_t F_Min, F_Max;

	F_Min = 79;
	F_Max = 0;
	for (i=0; i<18; i+=4) {
		if ( Cell_Min[i] < F_Min ) F_Min = Cell_Min[i];
		if ( Cell_Max[i] > F_Max ) F_Max = Cell_Max[i];
		if ( Cell_Min[i+1] < F_Min ) F_Min = Cell_Min[i+1];
		if ( Cell_Max[i+1] > F_Max ) F_Max = Cell_Max[i+1];
	}

	printf(" L");
	for (j=F_Min; j<=F_Max; j++) printf(" %6d", j);
	printf("     Etot  Cmin  Cmax\n");

	for (i=0; i<18; i+=4) {
		printf("%2d", i);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {printf("%7.3f", CellDepS1[i][j]); v += CellDepS1[i][j];}
		printf("  %7.3f\n", v);
		printf("%2d", i);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {
			w = 0.;
			if ( n >= 0 ) w += ShwrExp[n][i][j];
			else for (k=0; k<N_Shwr; k++) w +=  ShwrExp[k][i][j];
			printf("%7.3f", w);
			v += w;
		}
		printf("  %7.3f  %4d  %4d\n", v, Cell_Min[i], Cell_Max[i]);
		printf("%2d", i+1);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {printf("%7.3f", CellDepS1[i+1][j]); v += CellDepS1[i+1][j];}
		printf("  %7.3f\n", v);
		printf("%2d", i+1);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {

			w = 0.;
			if ( n >= 0 ) w += ShwrExp[n][i+1][j];
			else for (k=0; k<N_Shwr; k++) w +=  ShwrExp[k][i+1][j];
			printf("%7.3f", w);
			v += w;
		}
		printf("  %7.3f  %4d  %4d\n", v, Cell_Min[i+1], Cell_Max[i+1]);
	}

	F_Min = 79;
	F_Max = 0;
	for (i=2; i<18; i+=4) {
		if ( Cell_Min[i] < F_Min ) F_Min = Cell_Min[i];
		if ( Cell_Max[i] > F_Max ) F_Max = Cell_Max[i];
		if ( Cell_Min[i+1] < F_Min ) F_Min = Cell_Min[i+1];
		if ( Cell_Max[i+1] > F_Max ) F_Max = Cell_Max[i+1];
	}

	printf(" L");
	for (j=F_Min; j<=F_Max; j++) printf(" %6d", j);
	printf("     Etot  Cmin  Cmax\n");

	for (i=2; i<18; i+=4) {
		printf("%2d", i);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {printf("%7.3f", CellDepS1[i][j]); v += CellDepS1[i][j];}
		printf("  %7.3f\n", v);
		printf("%2d", i);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {
			w = 0.;
			if ( n >= 0 ) w += ShwrExp[n][i][j];
			else for (k=0; k<N_Shwr; k++) w +=  ShwrExp[k][i][j];
			printf("%7.3f", w);
			v += w;
		}
		printf("  %7.3f  %4d  %4d\n", v, Cell_Min[i], Cell_Max[i]);
		printf("%2d", i+1);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {printf("%7.3f", CellDepS1[i+1][j]); v += CellDepS1[i+1][j];}
		printf("  %7.3f\n", v);
		printf("%2d", i+1);
		v = 0.;
		for (j=F_Min; j<=F_Max; j++) {
			w = 0.;
			if ( n >= 0 ) w += ShwrExp[n][i+1][j];
			else for (k=0; k<N_Shwr; k++) w +=  ShwrExp[k][i+1][j];
			printf("%7.3f", w);
			v += w;
		}
		printf("  %7.3f  %4d  %4d\n", v, Cell_Min[i+1], Cell_Max[i+1]);
	}

	printf("\n\n");

	return 0;
}

Double_t EcalKX3DRec::ElectronEnergy(){
	return EleEne;
}

Double_t EcalKX3DRec::PhotonEnergy(){
	// NOT implemented, just use the EleEne
	return EleEne;
}

Double_t EcalKX3DRec::ProtonEnergy(){
	// NOT implemented, just use 3*EleEne
	return 3*EleEne;
}

Double_t EcalKX3DRec::ShowerEnergy(Int_t id){
	if( id==0 ) return ElectronEnergy();
	else if(id==1) return ProtonEnergy();
	else if(id==2) return PhotonEnergy();
	else{
		cerr << "==EcalKX3DRec::ShowerEnergy()==Option NOT defined"<<endl;
		return -1;
	}
	return 0.;
}

void EcalKX3DRec::SortShowers(){
	Double_t tmpShwrPar[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNpar][2];
	memcpy( tmpShwrPar, ShwrPar, sizeof(ShwrPar) );

	Double_t tmpShwrE0f[EcalKX3DRecDB::kNS], tmpShwrX0f[EcalKX3DRecDB::kNS], tmpShwrY0f[EcalKX3DRecDB::kNS], tmpShwrZ0f[EcalKX3DRecDB::kNS], tmpShwrKXf[EcalKX3DRecDB::kNS], tmpShwrKYf[EcalKX3DRecDB::kNS];
	memcpy( tmpShwrE0f, ShwrE0f, sizeof(ShwrE0f) );
	memcpy( tmpShwrX0f, ShwrX0f, sizeof(ShwrX0f) );
	memcpy( tmpShwrY0f, ShwrY0f, sizeof(ShwrY0f) );
	memcpy( tmpShwrZ0f, ShwrZ0f, sizeof(ShwrZ0f) );
	memcpy( tmpShwrKXf, ShwrKXf, sizeof(ShwrKXf) );
	memcpy( tmpShwrKYf, ShwrKYf, sizeof(ShwrKYf) );

	Int_t index_Sorted[EcalKX3DRecDB::kNS];
	memset( index_Sorted, 0, sizeof(index_Sorted) );
	Double_t emax=0.;
	// primary shower
	for(int i=0; i<N_Shwr; i++){
		if( emax < ShwrPar[i][0][0] ){
			emax = ShwrPar[i][0][0];
			index_Sorted[0] = i;
		}
	}
	if( N_Shwr==2 ) index_Sorted[1] = N_Shwr-1 - index_Sorted[0];
	else{
		emax = 0.;
		for(int i=0; i<N_Shwr; i++){
			if( i==index_Sorted[0] ) continue;
			if( emax < ShwrPar[i][0][0] ){
				emax = ShwrPar[i][0][0];
				index_Sorted[1] = i;
			}
		}
		for(int i=0; i<N_Shwr; i++){
			if( i==index_Sorted[0] || i==index_Sorted[1] ) continue;
			index_Sorted[2] = i;
		}
	}
	// modify ShwrPar
	I_Shwr = index_Sorted[0]; // index of the most energetic shower
	for(int i=0; i<N_Shwr; i++){
		for(int j=0; j<EcalKX3DRecDB::kNpar; j++){
			ShwrPar[i][j][0] = tmpShwrPar[ index_Sorted[i] ][j][0];
			ShwrPar[i][j][1] = tmpShwrPar[ index_Sorted[i] ][j][1];
		}
		ShwrE0f[i] = tmpShwrE0f[ index_Sorted[i] ];
		ShwrX0f[i] = tmpShwrX0f[ index_Sorted[i] ];
		ShwrY0f[i] = tmpShwrY0f[ index_Sorted[i] ];
		ShwrZ0f[i] = tmpShwrZ0f[ index_Sorted[i] ];
		ShwrKXf[i] = tmpShwrKXf[ index_Sorted[i] ];
		ShwrKYf[i] = tmpShwrKYf[ index_Sorted[i] ];
	//	cout << "i=" << i << ", i_sort=" << index_Sorted[i] << ", E=" << ShwrPar[i][0][0] << endl;
	}
}

void EcalKX3DRec::Summary(){
	if( N_Shwr<1 ) return;
	int flag=1; //
	// 1. re-define footprint according to the most energetic shower
	// (1) find the most energetic shower
	E0 = 0.; // Total fitted shower energy
	E1 = 0.; // highest fitted shower energy
	Int_t DEBUG2=0;
	if(DEBUG2) cout << "--EcalKX3DRec::Summary: N_Shwr=" << N_Shwr << endl;
	if( N_Shwr>1 ) SortShowers(); // the most energetic shower is with index=0
	for(Int_t k=0; k<N_Shwr; k++){
		E0 += ShwrPar[k][0][0];
	}
	E1 = ShwrPar[0][0][0];

	if(DEBUG2)cout << "--EcalKX3DRec::Summary: E0=" << E0 << ", E1=" << E1 << endl;
	// (2) re-define footprint
	B0 = ShwrPar[0][1][0];
	A0 = ShwrPar[0][2][0];
	X0 = ShwrPar[0][3][0];
	Y0 = ShwrPar[0][4][0];
	Z0 = ShwrPar[0][5][0];
	KX = ShwrPar[0][6][0];
	KY = ShwrPar[0][7][0];
	T0  = (A0-Z0)*sqrt(1+KX*KX+KY*KY);
	if(DEBUG2)cout << "X0=" << X0 << ", Y0=" << Y0 << endl;
	ShowerFootprint(1);  // extend to include secondary showers

	// 2. calculate energy deposition of each shower
	memset(ShwrExp, 0, sizeof(ShwrExp));
	memset(ShwrEtr, 0, sizeof(ShwrEtr));
	memset(ShwrDist, 0, sizeof(ShwrDist));
	memset(ShwrDepth, 0, sizeof(ShwrDepth));
	// low energy shower first
	for(Int_t k=N_Shwr-1; k>=0; k--){
		// reset shower shape and fluctuation parameters according to reconstructed energy
		SetLatPar(k, ShwrPar[k][0][0]);
		SetPdfPar(k, ShwrPar[k][0][0]);
		if(DEBUG2){
			printf("A0=%g, X0=%g, Y0=%g, Z0=%g, KX=%g, KY=%g, E0=%g\n", ShwrPar[k][2][0], ShwrPar[k][3][0], ShwrPar[k][4][0], ShwrPar[k][5][0], ShwrPar[k][6][0], ShwrPar[k][7][0], ShwrPar[k][0][0]);
		}
		I_Shwr = k;
		EcalShwrDef( k, flag ); // calculate ideal energy deposition
	}

	// 3. calculate final probability
	ShowerProbability( flag ); // calculate normalized probability

	// 4. sum expected energy from identified showers
	memset(CellExp, 0, sizeof(CellExp));
	memset(CellEtr, 0, sizeof(CellEtr));
	memset(CellBeta, 0, sizeof(CellBeta));
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			for(Int_t k=0; k<N_Shwr; k++){
				CellExp[l][m] += ShwrExp[k][l][m];
				CellEtr[l][m] += ShwrEtr[k][l][m];
				CellBeta[l][m] += ShwrBeta[k][l][m]*ShwrExp[k][l][m]; // number of particles
			}
		}
	}
	// 6. calculate EM energy
	EMEnergy();
	MatchTrackerAndShower();
	// 5. calculate EM likelihood
	EMEstimatorVars();
	EMEstimatorLkhd();
	EMEstimatorBDT();
	//
//	CalculateDmax3();
//	CalculateD8NF();
}

void EcalKX3DRec::EMEnergy(){
	// (1) correct energy in cells
	// transversal leakage cross fiber is accounted for by adding more cell
	// transversal leakage along fiber by infinite integral
	TotEne = 0.;
	TotDep = 0.;
	TotEmask = 0.; // total energy of masked cells
	TotExp = 0.;
	L2Ene  = 0.;
	L2Dep  = 0.;
	L2Exp  = 0.;
	EneSat = 0.;
	NhitLatLeak = 0;
	FracLatLeak = 0.;
	FracRearLeak = 0.;
	memset(CellDepCor, 0, sizeof(CellDepCor));
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		if(DEBUG) printf("--EMEnergy: l=%d, Cell_Min=%d, Cell_Max=%d\n", l, Cell_Min[l], Cell_Max[l]);
		for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			if( CellExp[l][m] < 1e-100 && CellExp[l][m]>0 ){
				cout << "Line3977: l=" << l << ", m=" << m << ", r=" << CellDepCor[l][m] << ", et=" << CellEtr[l][m] << ", e=" << CellExp[l][m] << endl;
			  	continue; // why this happen???
			}
			if( CellMask[l][m] & EcalKX3DRecDB::ImaginaryCell ){ // imaginary cells
			  	CellDepCor[l][m] = CellEtr[l][m];
				NhitLatLeak++;
				FracLatLeak += CellDepCor[l][m];
			}else if( CellMask[l][m] ){ // bad cell, saturation, ...
			  	CellDepCor[l][m] = CellEtr[l][m];
				EneSat += CellExp[l][m] - CellDep[l][m];
			}else if( CellExp[l][m] > 0 )	// cell-level energy correction
				CellDepCor[l][m] = CellDep[l][m]*CellEtr[l][m]/CellExp[l][m];
			TotEne += CellDepCor[l][m];
			TotExp += CellExp[l][m];
			if( CellMask[l][m] & EcalKX3DRecDB::SaturatedCell ) ElossSat += (CellExp[l][m] - CellDep[l][m]);
			if( CellMask[l][m]){
				TotEmask += CellExp[l][m];
			  	TotDep += CellExp[l][m];
			}
			else                TotDep += CellDep[l][m];
			if(l>=16){
			  	L2Ene += CellDepCor[l][m];
				L2Exp += CellExp[l][m];
			  	if( CellMask[l][m] ) L2Dep += CellExp[l][m];
				else                 L2Dep += CellDep[l][m];
			}
		}
	}
	// (2) rear leakge, as a function of (Z0,Kx,Ky)
	if( TotEne>0 ){
		FracLatLeak /= TotEne;
		L2Ene /= TotEne;
		TotEne /= 1.159; // PMT PW=0.3, MinEff=0.6
		EneL2Cor = TotEne/GetRearLeakageL2R(TotEne, L2Ene);
		EleEne = TotEne/GetRearLeakage(T0, Z0, KX, KY);
		FracRearLeak = 1 - TotEne/EleEne;
	}else{
		TotEne = E0;
		EleEne = E0;
		EneL2Cor = E0;
		L2Ene = 0.;
	}
	// (3) energy loss before ECAL, determined from electron MC
	double E_toi = 0.;
	if( N_Shwr==1 ) E_toi = 0.105; // ~100MeV
	else            E_toi = 0.057; // ~60MeV
	EleEne   += E_toi;
	EneL2Cor += E_toi;
}

double EcalKX3DRec::GetRearLeakageL2R(double einit, double l2r){
	return (1-1.5*l2r)*1.024;
}

void EcalKX3DRec::EMEstimatorVars(){
	// reset variables
	Chi2E = Nsigma = 0.; // of the fit footprint
	Npos = Nneg = 0;
	LkhdAxis = DepAxis = ExpAxis = 0.;
	AxisEmask = 0.;
	LkhdCore = DepCore = ExpCore = 0.;
	LkhdTail = DepTail = ExpTail = 0.;
	LkhdBump = DepBump = ExpBump = 0.;
	LkhdBump2 = DepBump2 = ExpBump2 = 0.;
	LkhdBump4 = DepBump4 = ExpBump4 = 0.;
	NhitAxis = NhitCore = NhitTail = NhitBump = 0;
	NhitBump2 = NhitBump4 = 0;
	E0M = E1M = E2M = 0.;
	Sx = Sy = 0;
	SizeX = SizeY = 0;
	D0bs = D1bs = E0bs = E1bs = 0.;
	NhitMiss=0; 
	memset(LayLkhd, 0, sizeof(LayLkhd));
	memset(LayLkhdA, 0, sizeof(LayLkhdA));
	memset(LayNhit, 0, sizeof(LayNhit));
	memset(LayDep,  0, sizeof(LayDep) );
	memset(LayExp,  0, sizeof(LayExp) );
	memset(LaySize, 0, sizeof(LaySize));
	memset(LayAxisDepExp, 0, sizeof(LayAxisDepExp));
	memset(LayCoreDepExp, 0, sizeof(LayCoreDepExp));

	Double_t LayAxisDep[EcalKX3DRecDB::kNL], LayAxisExp[EcalKX3DRecDB::kNL], LayCoreDep[EcalKX3DRecDB::kNL], LayCoreExp[EcalKX3DRecDB::kNL];
	memset(LayAxisDep,  0, sizeof(LayAxisDep) );
	memset(LayAxisExp,  0, sizeof(LayAxisExp) );
	memset(LayCoreDep,  0, sizeof(LayCoreDep) );
	memset(LayCoreExp,  0, sizeof(LayCoreExp) );
	NhitAll = 0.; // Any cell
	EdepAll = 0.; // Any cell
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		if( Cell_Med[l]<0 ) continue;
		if( Cell_Med[l]<40 ){
			for(Int_t c=Cell_Med[l]+1; c<EcalKX3DRecDB::kNC; c++){
				if(CellDep[l][c]>0){
					NhitAll+=2;
					EdepAll+=2*CellDep[l][c];
				}
			}
		}else{
			for(Int_t c=0; c<Cell_Med[l]; c++){
				if(CellDep[l][c]>0){
					NhitAll+=2;
					EdepAll+=2*CellDep[l][c];
				}
			}
		}
		NhitAll++;
		EdepAll+=CellDep[l][Cell_Med[l]];
	}

	// calculate discriminate variables
	Int_t cell_rad = 0;
	//int ntail[18]; memset(ntail, 0, sizeof(ntail));
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		if( Cell_Med[l]<0 ) continue;
		// check the largest radius
		cell_rad = TMath::Max( Cell_Med[l] - Cell_Min[l], Cell_Max[l] - Cell_Med[l] );
		for(Int_t m=Cell_Med[l]-cell_rad; m<Cell_Med[l]+cell_rad+1; m++){
			int l_found = -1, m_found = -1;
			if( m>= Cell_Min[l] && m<=Cell_Max[l] && (CellMask[l][m]==0||CellMask[l][m]==EcalKX3DRecDB::HighAmplitudeCell)){ // no leak and not bad cell
				l_found = l;
				m_found = m;
			}else{ // bad cell correction
				Int_t m_m = 2*Cell_Med[l]-m,// mirror to shower axis, if not exceeding ECAL range
						l_n = l+1-2*(l%2),    // use the neighboring layer in the same superlayer
						l_p = l%2?l+1:l-1,    // nearest layer in a different superlayer
						l_q = l%4<1?(l/4)*4 - 4:(l/4)*4 - 2;    // nearest layer in a different superlayer
				if( l==0 ) l_p = 2;
				if( l>15 ) l_p = 15;
				if( l<4  ) l_q = l%4<1?4:6;

				Int_t m_n   = m   - Cell_Med[l] + Cell_Med[l_n], // same distance to shower axis
						m_n_m = m_m - Cell_Med[l] + Cell_Med[l_n]; // mirror in the neighboring layer
				Int_t m_p   = m   - Cell_Med[l] + Cell_Med[l_p], // same distance to shower axis
						m_p_m = m_m - Cell_Med[l] + Cell_Med[l_p]; // mirror
				Int_t m_q   = m   - Cell_Med[l] + Cell_Med[l_q], // same distance to shower axis
						m_q_m = m_m - Cell_Med[l] + Cell_Med[l_q]; // mirror

				if( DEBUG ){
					cout << "l=" << l << ", l_n=" << l_n << ", l_p=" << l_p << endl;
					cout << "m=" << m << ", m_m=" << m_m << ", m_n=" << m_n << ", m_n_m=" << m_n_m << ", m_p=" << m_p << ", m_p_m=" << m_p_m << endl;
				}

				if( m==Cell_Med[l] ){ // on axis, can not use mirror
					if( CellMask[l_n][m_n]==0 ){
						l_found = l_n;
						m_found = m_n;
					}else if( CellMask[l_p][m_p]==0 ){
						l_found = l_p;
						m_found = m_p;
					}else if( CellMask[l_q][m_q]==0 ){
						l_found = l_q;
						m_found = m_q;
					}else{
						NhitMiss++;
//						cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--0--" << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_n, m_n, CellMask[l_n][m_n]) << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_p, m_p, CellMask[l_p][m_p]) << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_q, m_q, CellMask[l_q][m_q]) << endl;
//						cout << "--------------------------------------------------------" << endl;
					}
				}else{ // not on axis, use mirror
					if( m_m>=Cell_Min[l] && m_m<=Cell_Max[l] && CellMask[l][m_m]==0 ){ // mirror in this layer
						l_found = l;
						m_found = m_m;
					}else if( m_n>=Cell_Min[l_n] && m_n<=Cell_Max[l_n] && CellMask[l_n][m_n]==0 ){ // neighbor layer
						l_found = l_n;
						m_found = m_n;
					}else if( m_n_m>=Cell_Min[l_n] && m_n_m<=Cell_Max[l_n] && CellMask[l_n][m_n_m]==0 ){ // mirror in neighbor layer
						l_found = l_n;
						m_found = m_n_m;
					}else if( m_p>=Cell_Min[l_p] && m_p<=Cell_Max[l_p] && CellMask[l_p][m_p]==0 ){ // nearby layer
						l_found = l_p;
						m_found = m_p;
					}else if( m_p_m>=Cell_Min[l_p] && m_p_m<=Cell_Max[l_p] && CellMask[l_p][m_p_m]==0 ){ // mirror nearby layer
						l_found = l_p;
						m_found = m_p_m;
					}else if( m_q>=Cell_Min[l_q] && m_q<=Cell_Max[l_q] && CellMask[l_q][m_q]==0 ){ // previous superlayer
						l_found = l_q;
						m_found = m_q;
					}else if( m_q_m>=Cell_Min[l_q] && m_q_m<=Cell_Max[l_q] && CellMask[l_q][m_q_m]==0 ){ // mirror in previous superlayer
						l_found = l_q;
						m_found = m_q_m;
					}
					if( DEBUG && (l_found<0||m_found<0) ){
						NhitMiss++;
//						cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--1--" << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l, m_m, CellMask[l][m_m]) << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_n, m_n, CellMask[l_n][m_n]) << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_n, m_n_m, CellMask[l_n][m_n_m]) << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_p, m_p, CellMask[l_p][m_p]) << endl;
//						cout<<Form("CellMask[%d][%d]=%d", l_p, m_p_m, CellMask[l_p][m_p_m]) << endl;
//						cout << "--------------------------------------------------------" << endl;
					}
				}
			}
			if( l_found>=0 && m_found>=0 ){
				LayLkhd[l] += CellProbNorm[l_found][m_found];
				LayNhit[l] ++;
				if(DEBUG && (l_found!=l ||  m_found!=m)) cout << Form("leakmirror: (%d, %d) mirrored to (%d, %d)", l, m, l_found, m_found) << endl;
			}else{
				if(DEBUG)
					cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--3--" << "lay=" << l << ", cell=" << m << endl;
				continue;
			}

			Bool_t IsCore(0), IsTail(1), IsAxis(0);

			// Check all fitted showers
			for(Int_t ish=0; ish<N_Shwr; ish++){
				if( m==Shwr_Med[ish][l] || m==Shwr_Med[ish][l]-1 || m==Shwr_Med[ish][l]+1 ){
					IsCore = 1;
					IsTail = 0;
				}
			}
			if( m==Cell_Med[l] ) IsAxis = 1; // axis is defined as the primary shower

			if( IsAxis ){
				LkhdAxis += CellProbNorm[l_found][m_found];
				NhitAxis ++;
				DepAxis  += CellDep[l_found][m_found];
				ExpAxis  += CellExp[l_found][m_found];
				if( CellMask[l][m] ) AxisEmask += CellExp[l_found][m_found];
				if( CellMask[l][m] && DEBUG ){
					cout << Form("ShowerAxis: l%02d, c%02d, CellMask=%d, l_found %02d, m_found %02d, CellDep[l][m]=%g, CellDep[l_found][m_found]=%g, CellExp[l][m]=%g, CellExp[l_found][m_found]=%g", l, m, CellMask[l][m], l_found, m_found, CellDep[l][m], CellDep[l_found][m_found], CellExp[l][m], CellExp[l_found][m_found] ) << endl;
				}
				LayAxisDep[l] += CellDep[l_found][m_found];
				LayAxisExp[l] += CellExp[l_found][m_found];
				LayLkhdA[l] += CellProbNorm[l_found][m_found];
			}
			if( IsCore && CellBeta[l_found][m_found]>0 ){
				DepCore  += CellDep[l_found][m_found];
				LkhdCore += CellProbNorm[l_found][m_found];
				NhitCore ++;
				ExpCore  += CellExp[l_found][m_found];
				LayCoreDep[l] += CellDep[l_found][m_found];
				LayCoreExp[l] += CellExp[l_found][m_found];

				double sigmaE = CellExp[l_found][m_found]/sqrt(CellBeta[l_found][m_found] );
				double deltaE = CellDep[l_found][m_found] - CellExp[l_found][m_found];
				if( sigmaE>0 ){
					Chi2E += deltaE*deltaE;
					Nsigma += deltaE*deltaE/sigmaE/sigmaE;
				}
			}
			if( IsTail && CellBeta[l_found][m_found]>0 ){
				double sigmaE = CellExp[l_found][m_found]/sqrt( CellBeta[l_found][m_found] ); // E/sqrt(N)
				double deltaE = CellDep[l_found][m_found] - CellExp[l_found][m_found];
				if( deltaE>0 ) 	Npos ++;
				if( deltaE<0 )  	Nneg ++;

				// tail region
				DepTail  += CellDep[l_found][m_found];
				LkhdTail += CellProbNorm[l_found][m_found];
				NhitTail ++;
				//ntail[l] ++;
				ExpTail  += CellExp[l_found][m_found];

				if( CellDep[l_found][m_found] > 0.004 && CellDep[l_found][m_found] > CellExp[l_found][m_found] + 3*sigmaE ){
					// large bump, "neutrons"
					NhitBump ++ ;
					LkhdBump += CellProbNorm[l_found][m_found];
					if(CellMask[l_found][m_found]){
						DepBump  += CellExp[l_found][m_found];
					}else{
						DepBump  += CellDep[l_found][m_found];
					}
					ExpBump  += CellExp[l_found][m_found];
				}
				if( CellDep[l_found][m_found] > 0.002 && CellDep[l_found][m_found] > CellExp[l_found][m_found] + 2*sigmaE ){
					// large bump, "neutrons"
					NhitBump2 ++;
					LkhdBump2 += CellProbNorm[l_found][m_found];
					if(CellMask[l_found][m_found]){
						DepBump2  += CellExp[l_found][m_found];
					}else{
						DepBump2  += CellDep[l_found][m_found];
					}
					ExpBump2  += CellExp[l_found][m_found];
				}
				if( CellDep[l_found][m_found] > 0.002 && CellDep[l_found][m_found] > CellExp[l_found][m_found] + 4*sigmaE ){
					// large bump, "neutrons"
					NhitBump4 ++;
					LkhdBump4 += CellProbNorm[l_found][m_found];
					if(CellMask[l_found][m_found]){
						DepBump4  += CellExp[l_found][m_found];
					}else{
						DepBump4  += CellDep[l_found][m_found];
					}
					ExpBump4  += CellExp[l_found][m_found];
				}
			} // end of tail region
		} // loop of cells
//		cout << "l=" << l << ", ntail[l]=" << ntail[l] << endl;
	} // loop of layers

	double dx = abs(X0)-20.;
	double dy = abs(Y0)-20.;
	if( NhitOut>0 ){
		double edepout_avg = EdepOut/NhitOut;
		if( dx > 0 ){
			NhitOut *= (1 + 0.040*dx);
			EdepOut = NhitOut*edepout_avg;
		}
		if( dy > 0 ){
			NhitOut *= (1 + 0.015*dy);
			EdepOut = NhitOut*edepout_avg;
		}
	}

	double cell_edep=0.;
	bool IsLeak=0;
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			if( CellExp[l][m] <= 0 ) continue;
			if( CellMask[l][m] ) cell_edep = CellExp[l][m];
			else                 cell_edep = CellDep[l][m];
			Int_t m_m = 2*Cell_Med[l] - m;
			if( m_m < 4 || m_m > EcalKX3DRecDB::kNC-4 ) IsLeak = 1;
			else 								  IsLeak = 0;
			Double_t LeakCorrectionFactor = (1+IsLeak);
			LayDep[l]  += LeakCorrectionFactor*cell_edep;
			LayExp[l]  += LeakCorrectionFactor*CellExp[l][m];

			if( (l/2)%2 ) Sx += LeakCorrectionFactor*cell_edep;
			else          Sy += LeakCorrectionFactor*cell_edep;

			for(Int_t k=0; k<N_Shwr; k++){
				LaySize[l] += LeakCorrectionFactor*cell_edep*ShwrExp[k][l][m]*ShwrDist[k][l][m]/CellExp[l][m];
				if( (l/2)%2 ) SizeX += LeakCorrectionFactor*cell_edep*ShwrExp[k][l][m]*sqrt( ShwrDist[k][l][m]*ShwrDist[k][l][m] + ShwrDepth[k][l][m]*ShwrDepth[k][l][m] )/CellExp[l][m];
				else          SizeY += LeakCorrectionFactor*cell_edep*ShwrExp[k][l][m]*sqrt( ShwrDist[k][l][m]*ShwrDist[k][l][m] + ShwrDepth[k][l][m]*ShwrDepth[k][l][m] )/CellExp[l][m];
				if( TMath::IsNaN( SizeX ) ){
					cout << "Line 4189: k=" << k << ", l=" << l << ", m=" << m << ", LeakCorrectionFactor=" << LeakCorrectionFactor << ", cell_edep=" << cell_edep << ", ShwrExp=" << ShwrExp[k][l][m] << ", ShwrDist=" << ShwrDist[k][l][m] << ", CellExp=" << CellExp[l][m] << endl;
				}
			}
		}
	}

	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		Int_t m = Cell_Med[l]; // Only the most energetic shower
		if( m<4 || m>=EcalKX3DRecDB::kNC-4 ) continue;
		if( l==0 ){
			E0M += CellDep[l][m];
			if( CellDep[l][m+1] > CellDep[l][m-1] ) E0M += CellDep[l][m+1];
			else                                    E0M += CellDep[l][m-1];
		}else if( l==1 ){
			E1M += CellDep[l][m];
			if( CellDep[l][m+1] > CellDep[l][m-1] ) E1M += CellDep[l][m+1];
			else                                    E1M += CellDep[l][m-1];
		}else if( l==2 ){
			E2M += CellDep[l][m];
			if( CellDep[l][m+1] > CellDep[l][m-1] ) E2M += CellDep[l][m+1];
			else                                    E2M += CellDep[l][m-1];
		}
		if( LayDep[l] > 0 ) LaySize[l] /= LayDep[l];
	} // loop of layers
	if( Sx>0 ) SizeX /= Sx;
	if( Sy>0 ) SizeY /= Sy;

	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		if(LayAxisExp[l]>0) LayAxisDepExp[l] = LayAxisDep[l]/LayAxisExp[l];
		if(LayCoreExp[l]>0) LayCoreDepExp[l] = LayCoreDep[l]/LayCoreExp[l];
	}
	// back-splash energy
	D0bs = LayDep[0] - LayAxisDep[0];
	E0bs = LayExp[0] - LayAxisExp[0];
	D1bs = LayDep[1] - LayAxisDep[1];
	E1bs = LayExp[1] - LayAxisExp[1];
}

void EcalKX3DRec::EMEstimatorLkhd(){
	int l;
	// double p0, p1, p2, p3, p4, v, t, V=0, W=0, U=0, Q=0;
	double p1, p2, p3, v, t, V=0, W=0, U=0, Q=0;
	double p1_0, p1_1, p1_2,
			 p2_0, p2_1, p2_2, p2_3;
	double logene = log(1+EleEne);
	if(N_Shwr<1){
		EmLkhd[0] = EmLkhd[1] = EmLkhd[2] = 0.;
		return ;
	}
	//0: Z0
	double _A0 = ShwrPar[0][2][0],
//			 _X0 = ShwrPar[0][3][0],
//			 _Y0 = ShwrPar[0][4][0],
			 _Z0 = ShwrPar[0][5][0],
			 _KX = ShwrPar[0][6][0],
			 _KY = ShwrPar[0][7][0],
			 _E0 = ShwrPar[0][0][0];

	double logE0 = log(1+_E0);

	v = -_Z0/(1-0.025*(_KX*_KX+_KY*_KY));
	if( N_Shwr==1 ){
		p1 = 1 - 0.6*exp(-EleEne/5);
		p2 = 146 + 1.02*logene + 0.4*exp(-EleEne/6.32) - 0.95*exp(-EleEne/1.78);
		p3 = 2.24 + 4.89*exp(-EleEne/3.12);
	}else{
		p1 = 1 - 0.6*exp(-EleEne/5);
		p2 = 146.4 + 1.02*logE0 - 0.964*exp(-EleEne/1.87);
		p3 = 1.44 + 4.1*exp(-EleEne/5.75);
		if(p3<0.5) p3=0.5; // ~10TeV
	}
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[0] = v;
	V += v;

	//2: LkhdTail/NhitTail, all showers
	if( NhitTail>0 ){
		v = log(LkhdTail);
		if( N_Shwr==1 ){
			p1 = 4.63 + 0.21*logene - 0.341*exp(-EleEne/19.8);
			p2 = 0.105+0.88/(1+EleEne);
		}else{
			p1 = 4.60 + 0.22*logene - 0.445*exp(-EleEne/9.73);
			p2 = 0.124+0.725/(1+EleEne);
		}
		v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
	}else
		v = 1e3;
	EmVar[1] = v;
	V += v;

	//3: ExpAxis/DepAxis, primary shower
	if( DepAxis>0 && ExpAxis>0 ){
		v = ExpAxis/DepAxis;
		if( N_Shwr==1 ){
			p1 = 66.3 - 28*exp(-_E0/32.1) - 5.63*log(_E0+200);
			p2 = 0.992 - 0.073*exp(-_E0/2) + 0.002*logE0;
		}else{
			p1 = 43.5-16.3*exp(-_E0/11.4)-4*log(_E0+200);
			p2 = 0.974 - 0.2*exp(-_E0/0.6) + 0.006*logE0;
		}
		p3 = 2.5 - 1*exp(-_E0/50);
		v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	}else
		v = 1e3;
	EmVar[2] = v;
	V += v;

	//4: DepTail/ExpTail
	v = ExpTail>0.?DepTail/ExpTail:0;
	if( N_Shwr==1 ){
		p1 = 10 - 6.54*exp(-EleEne/80);
	  	p2 = 0.955;
	}
	else{
		p1 = 10 - 6*exp(-EleEne/80);
		p2 = 0.996 - 0.115*exp(-EleEne/7.7);
	}
	p3 = 4.62 - 4.50*exp(-EleEne/12.8);
	if( v>0 )
		v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	else
		v = 1e3;
	EmVar[3] = v;
	V += v;

	//5: E0M + E1M: First 2 layer deposition
	if( E0M+E1M > 0 ){
		_KX = ShwrPar[0][6][0];
		_KY = ShwrPar[0][7][0];
		v = (E0M + E1M)/(1+1.63*(_KX*_KX+_KY*_KY)); // to correct angular dependence
	}else
		v = 0.0005; // 1 ADC ~ 0.5 MeV
	v = -log(v);
	if( N_Shwr==1 ){
		p1 = 1.74 - 0.106*logE0;
		if(p1<0.1) p1=0.1;
		p2 = -0.407 + 3.42*pow(_E0+1, -0.284);
		p3 = 2.80 - 1.56*exp(-_E0/10);
	}
	else{
		p1 = 2.;
		p2 = -0.3 + 3.23*pow(_E0+1, -1/3.);
		p3 = 0.9;
	}
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[4] = v;
	V += v;

	// 6. E2M, Layer 2 energy deposition
	if(E2M>0){
		_KX = ShwrPar[0][6][0];
		_KY = ShwrPar[0][7][0];
		v = E2M/(1+1.67*(_KX*_KX+_KY*_KY)); // to correct angular dependence
	}else
		v = 0.0005; // 1 ADC ~ 0.5 MeV
	v = -log(v);
	if( N_Shwr==1 ){
		p1 = 2.39 - 0.188*logE0;
		if(p1<0.1)p1=0.1;
		p2 = -2.94 + 5.29*pow(_E0+0.1, -0.15);
		p3 = 3 - 2.9/sqrt(1+_E0);
	}else{
		p1 = 2.2 - 0.0615*logE0;
		if(p1<0.1)p1=0.1;
		p2 = -1.78 + 3.83*pow(_E0+1, -0.22);
		p3 = 1.54 - 1.04*exp(-_E0);
	}
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[5] = v;
	V += v;

	//7. TotDep/TotExp
	v = TotDep>0?TotExp/TotDep:0;
	p1 = 1.018 - 0.0183*exp(-EleEne/4) - 0.0112*exp(-EleEne/250);
	p2 = -0.0153 + 0.0922/sqrt(EleEne+1) + 0.00361*logene;
	v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
	EmVar[6] = v;
	V += v;

	//8. LkhdBump 3-4 sigma bump
	p1 = 2;
	if(N_Shwr==1) p2 = -3.16 + 2.41/sqrt(EleEne+1);
	else          p2 = -3.44 + 2.24*pow(EleEne+1, -0.35);
	p3 = 1;
	if( LkhdBump - LkhdBump4>0 )
		v = -log(LkhdBump - LkhdBump4);
	else
		v = p2;
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[7] = v;
	V += v;

	//9. LkhdBump2, 2-3 sigma bump
	p1 = 1.8;
	if(N_Shwr==1) p2 = -3.71 - 2.25/(EleEne+1) + 4.47/sqrt(EleEne+1);
	else          p2 = -3.95 - 4.21/(EleEne+1) + 5.81/sqrt(EleEne+1);
	p3 = 3.3-2.39*exp(-EleEne/16.1);
	if( LkhdBump2 - LkhdBump>0 )
		v = -log(LkhdBump2 - LkhdBump);
	else
		v = p2;
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[8] = v;
	V += v;

	// 10. LkhdBump4, >4 sigma bump
	p1 = 0.9-0.516*exp(-EleEne/2.6);
	if(N_Shwr==1) p2 = -4.09 - 0.774*exp(-EleEne/3.48) + 0.542*exp(-EleEne/21.8);
	else          p2 = -4.39 + 0.566*exp(-EleEne/28.7);
   p3 = 4;
	if( LkhdBump4>0 )
		v = -log(LkhdBump4);
	else
		v = p2;
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[9] = v;
	V += v;

	//11. SizeX + SizeY
	v = SizeX + SizeY;
	p1 = 2.66 - 1.58*exp(-EleEne/30.8);
	if(N_Shwr==1) p2 = 6.176 - 0.69*exp(-EleEne/2.59) - 0.427*exp(-EleEne/14.5);
	else          p2 = 6.221 - 0.491*exp(-EleEne/3.61) - 0.359*exp(-EleEne/22.7);
	p3 = 3.02 - 2.59*exp(-EleEne/2.41);
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[10] = v;
	V += v;

	//12. LayDep[0]
	if( LayDep[0]>0 )	v = LayDep[0]/(1+1.63*(_KX*_KX+_KY*_KY));
	else v = 0.0002; // 0.5 ADC
	v = -log(v);
	p1 = 1.25;
	if( N_Shwr==1 ) p2 = 3.495- 0.12*exp(-EleEne/100) - 0.35*log(EleEne);
	else            p2 = 3.15 - 0.312*log(EleEne) - 0.191/(1+EleEne);
	p3 = 0.706 + 0.459*log(EleEne+5);
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[11] = v;
	V += v;

	//13. LayDep[1]
	if( LayDep[1]>0 ) v = LayDep[1]/(1+1.67*(_KX*_KX+_KY*_KY));
	else v = 0.0002; // 0.5 ADC
	v = -log(v);
	p1 = 1.2;
	if( N_Shwr==1 ) p2 = 1.70 - 0.268*log(EleEne) + 1.37/sqrt(EleEne+1);
	else            p2 = 1.66 - 0.269*log(EleEne) + 1.07/sqrt(EleEne+1);
	p3 = 3.35 - 2.53*exp(-EleEne/2.);
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[12] = v;
	V += v;

	//14. LkhdCore
	v = log( (LkhdCore-LkhdAxis)/(NhitCore-NhitAxis) );
	if( TMath::IsNaN(v) ){
		v = 1e3;
	}else{
		if(N_Shwr==1) p1 = -0.817 + 0.214*exp(-EleEne/23.7) - 0.515*exp(-EleEne/1.56);
		else          p1 = -0.517 - 0.959*exp(-EleEne/1.21);
		p2 =  0.27 + 0.162*exp(-EleEne/0.5) + 0.00393*log(EleEne+1);
	}
	v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
	EmVar[13] = v;
	V += v;

	// 15. L2Dep/L2Exp, last 2 layers
	if( EleEne>2 ){
		p1 = 0.952 + 0.405*logene;
		p2 = 1.005 - 1.39*exp(-EleEne/1.87) + 0.0554*exp(-EleEne/110);
		p3 = 22.8-22.3*exp(-EleEne/124);
		if( L2Exp>0 )
			v = L2Dep/L2Exp;
		else
			v = p2;
		v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
		EmVar[14] = v;
		V += v;
	}

	// 16. A0
	v = 141 + ShwrPar[0][2][0];
	if( N_Shwr==1 ){
		p1 = 1.60;
		p2 = -0.162 - exp(-_E0/2) - 0.3*exp(-_E0/50);
		p3 = 0.705 - 0.557*exp(-_E0/10);
	}else {
		p1 = 1.0;
		p2 = 0.495 - 0.309*exp(-_E0/5.66) - 0.892*exp(-_E0/92.9);
		p3 = 0.702 - 0.687*exp(-EleEne/2.57);
	}
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[15] = v;
	V += v;

	//1: LkhdAxis of primary shower
	v = log(1+LkhdAxis);
	p1 = 1;
	if(N_Shwr==1){
	  	p2 = 0.791 + 0.508*exp(-_E0/100) + 0.204*log(_E0+100);
		p3 = 5.18 + 2.29*exp(-_E0/80);
	}
	else{
		p2 = 2.36 - 0.147*exp(-_E0);
		p3 = 2.98 + 3.92*exp(-_E0/80);
	}
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[16] = v;
	Q += v;

	// 17. Nsigma of core, similar to LkhdCore
	v = Nsigma;
	if(N_Shwr==1){
		p1 = 0.11;
		p2 = 40.9 + 10*exp(-EleEne/31.5) - 14.9*exp(-EleEne/2.57);
	}else{
		p1 = 0.06;
		p2 = 72.4 + 481*exp(-EleEne/2.93) - 496*exp(-EleEne/2.64);
	}
	p3 = 0.615 - 0.31*exp(-EleEne/16.7);
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[17] = v;
	Q += v;

	// NhitBump
	if( N_Shwr==1 ){
		p1 = 0.1;
	  	p2 = 10 - 5.23/sqrt(EleEne+1) + 1*logene;
		p3 = 3.61 + 10.2/sqrt(EleEne+1);
	}else{
		p1 = 0.1;
		p2 = 18 - 4.63*exp(-EleEne/7.2) - 6.42*exp(-EleEne/45);
		p3 = 3.61 + 8.4/sqrt(EleEne+1);
	}
	if( NhitBump<1 ) v = p2;
	else             v = NhitBump;
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[18] = v;
	Q += v;

	// Npos(=Dep>Exp)
	v = Npos;
	if( N_Shwr==1 ) p1 = -45.7 - 29.7/(EleEne+1) + 40.7*log(EleEne+6);
	else            p2 = -65 - 45*log(EleEne+6);
	p2 = -0.98 + 3.5*log(EleEne+6);
	v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
	EmVar[19] = v;
	Q += v;

	//
	if( N_Shwr>1 ){
		//17: T0 of second shower
		_A0 = ShwrPar[1][2][0];
		_Z0 = ShwrPar[1][5][0];
		_KX = ShwrPar[1][6][0];
		_KY = ShwrPar[1][7][0];
		_E0 = ShwrPar[1][0][0];
		v = -_Z0/(1-0.025*(_KX*_KX+_KY*_KY)); // T0[1]
		p1 = 1-0.9*exp(-_E0/2);
		p2 = 146.67 + 1.02*log(_E0+1) - 1.73*exp(-_E0/1.11);
		p3 = 0.5 + 5/(_E0+1);
		v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
		EmVar[20] = v;
		Q += v;
	}

	if( IsTrackerUsed ){
		// Combine Tracker and ECAL information
		_E0 = ShwrPar[I_ShwrTrk][0][0];
		v = ShwrPar[I_ShwrTrk][3][0] - TrkX0;
		p1 = 0.;
		p2 = TMath::Max( 0.023, 4.40281e-01/(1+_E0) + 1.21468e-01/sqrt(1+_E0) + 1.31453e-02 );
		v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
		EmVar[21] = v;
		U += v;

		v = ShwrPar[I_ShwrTrk][6][0] - TrkKX;
		p1 = 0.;
		p2 = TMath::Max( 4.5e-3, 9.57554e-02/(1+_E0) + 3.52373e-02/sqrt(1+_E0) + 2.45411e-03 );
		v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
		EmVar[22] = v;
		U += v;
/*
		v = ShwrPar[I_ShwrTrk][7][0] - TrkKY;
		p1 = 0.;
		p2 = 0.176576/(_E0+1) + 0.0344155/sqrt(_E0+1) + 0.0020567;
		v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
		EmVar[18] = v;
		U += v;
*/
	}
	//
	//
	//==== Re-weight layer likelihoods
	// LayLkhd is fitted by function: f(x)=[0]*[1]*exp(-[1]*x)*pow([1]*x,[2])/TMath::Gamma([2]+1)
	// Integral f(x) = [0]
	// [1]: Scale factor
	// [2]: Degree of freedom

	//	double logene = TMath::Log( EleEne );
	memset( LayLkhdReNorm, 0, sizeof( LayLkhdReNorm ));
	double sqrtene = sqrt(EleEne+1);

	// tuning upto 2 TeV, 2017-03-01
	if(N_Shwr==1){
		p1_0 = 2.09 + 0.165*logene - 0.84/sqrtene;
		p1_1 = 0.022 - 0.15/(EleEne+1) - 0.5/sqrtene;
	}else{
		p1_0 = 2.04 + 0.169*logene - 0.23*exp(-EleEne/14.6);
		p1_1 = 0.022 - 0.06/(EleEne+1) - 0.41/sqrtene;
	}
	p1_2 = 1.64 - 0.37*exp(-EleEne/5.4);

	p2_0 = 0.175 + 0.0814*exp(-EleEne/20) + 0.0962*exp(-EleEne/250) - 0.153*exp(-EleEne/2); // irregular...
	p2_1 = 0.38 + 0.95*exp(-EleEne/3);
	p2_2 = 0.063 + 1.08*exp(-EleEne/2) + 0.0895*exp(-EleEne/40);
	p2_3 = -2.76 + 2.22*exp(-EleEne/2);
	if(N_Shwr>1) p2_0 *= 1.05;

	_A0 = ShwrPar[0][2][0];
//	_X0 = ShwrPar[0][3][0];
//	_Y0 = ShwrPar[0][4][0];
	_Z0 = ShwrPar[0][5][0];
	_KX = ShwrPar[0][6][0];
	_KY = ShwrPar[0][7][0];
	Int_t n_missing=0;
	for(l=0; l<18; l++){
		t = (_Z0 - EcalKX3DRecDB::EcalZ[l])/(_A0-_Z0)+1; // depth/T0 + 1
		if( t < 0.3 ) t = 0.3;
		if( t > 3.5 ) t = 3.5;

		// use gaussian distribution of -log(LayLkhd/LayNhit)
		p1 = p1_0*(1 + p1_1*(t - p1_2)*(t - p1_2)); // mean
		p2 = p2_0*(exp(p2_2*(t-p2_1)) + exp(p2_3*(t-p2_1))); // sigma

		if( LayLkhd[l]>0 ){
			v = log(LayLkhd[l]);
			v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
		}else{
			n_missing++;;
		}

		LayLkhdReNorm[l] = v;
		//if(l%4>=2) printf("l=%d, LayLkhd=%.3f, LayLkhdReNorm=%.3f\n", l, LayLkhd[l], LayLkhdReNorm[l]);
		W += v; // renormalized total likelihood
	}
	if( n_missing>0 ) W *= 18./(18. - n_missing);

	// log(W) dependence on X0/Y0/KX/KY
//	double c_X0 = _X0<32.4?1+1.45e-2*exp(3.1e-3*_X0*_X0):1.;
//	double c_Y0 = _Y0<32.4?1+1.45e-2*exp(3.1e-3*_Y0*_Y0):1.;
//	double c_KXKY = 1+0.32*(_KX*_KX + _KY*_KY);

	EmLkhd[0] = TMath::Log(V);
	EmLkhd[1] = TMath::Log(W);//c_X0/c_Y0/c_KXKY;
	EmLkhd[2] = TMath::Log(U);
	EmLkhd[3] = TMath::Log(Q);

}

void EcalKX3DRec::EMEstimatorBDT(){
	// BDT trained by test beam electron(120 GeV + 180 GeV) and proton (400 GeV)
	static bool IsBDTReaderInitialized = 0;
	static TMVA::Reader *reader[2]={0};
	const int nvar=16;
	Float_t LogEmVar[nvar], _energy, _lkhd0, _lkhd1;
	if( IsBDTReaderInitialized==0 ){
		IsBDTReaderInitialized = 1;
		for(int j=0; j<2; j++){
			reader[j] = new TMVA::Reader( "!Color:!Silent:!V:!H" );
			for(int i=0; i<nvar; i++){
				reader[j]->AddVariable( Form("Var%d:=log(EmVar[%d])", i, i), &(LogEmVar[i]) );
			}
			reader[j]->AddVariable("Lkhd1", &_lkhd1);
			reader[j]->AddSpectator("Lkhd0", &_lkhd0);
			reader[j]->AddSpectator("Lkhd1", &_lkhd1);
			reader[j]->AddSpectator("Ene", &_energy);
		}
		reader[0]->BookMVA( "EmBDT", AMSDataDir + AMSCommonsI::getversion() + "/EcalKX/TMVAClassification_BDTG_ODD.weights.xml" );
		reader[1]->BookMVA( "EmBDT", AMSDataDir + AMSCommonsI::getversion() + "/EcalKX/TMVAClassification_BDTG_EVEN.weights.xml" );
	}
	for(int i=0; i<nvar; i++){
		if( EmVar[i]>0 ){
			LogEmVar[i] = TMath::Log( EmVar[i] );
		}else{
			LogEmVar[i] = 0;
		}
	}
	_lkhd0  = EmLkhd[0];
	_lkhd1  = EmLkhd[1];
	_energy = EleEne;
	if( reader[0]!=0 && event%2==0 ){ // trained by event%2==1
		EmBDT = reader[0]->EvaluateMVA("EmBDT");
	}else if( reader[1]!=0 && event%2>0 ){ // trained by event%2==0
		EmBDT = reader[1]->EvaluateMVA("EmBDT");
	}else{
		EmBDT = -2;
	}
}

Double_t EcalKX3DRec::GetRearLeakage(Double_t zmax, Double_t z0, Double_t kx, Double_t ky){
	//! from ToyMC, assuming B0=0.62 !
	//! Need improvement !
	static Double_t p0[16][32] = {
		{0.999, 0.999, 0.998, 0.998, 0.997, 0.996, 0.995, 0.994, 0.992, 0.990,
			0.988, 0.984, 0.980, 0.975, 0.968, 0.960, 0.950, 0.939, 0.924, 0.905,
			0.884, 0.859, 0.828, 0.792, 0.750, 0.702, 0.649, 0.588, 0.522, 0.450,
			0.378, 0.302},
		{0.998, 0.998, 0.997, 0.997, 0.996, 0.995, 0.994, 0.992, 0.990, 0.987,
			0.984, 0.980, 0.975, 0.969, 0.961, 0.952, 0.941, 0.928, 0.911, 0.892,
			0.870, 0.843, 0.811, 0.775, 0.734, 0.689, 0.637, 0.581, 0.519, 0.456,
			0.388, 0.321},
		{0.997, 0.997, 0.996, 0.996, 0.995, 0.993, 0.992, 0.990, 0.987, 0.984,
			0.980, 0.975, 0.970, 0.963, 0.954, 0.944, 0.932, 0.917, 0.900, 0.880,
			0.856, 0.829, 0.798, 0.761, 0.722, 0.678, 0.628, 0.575, 0.519, 0.458,
			0.397, 0.333},
		{0.997, 0.996, 0.995, 0.994, 0.993, 0.992, 0.990, 0.987, 0.984, 0.981,
			0.976, 0.971, 0.964, 0.956, 0.948, 0.936, 0.924, 0.908, 0.890, 0.868,
			0.845, 0.817, 0.786, 0.751, 0.712, 0.668, 0.623, 0.572, 0.518, 0.460,
			0.404, 0.345},
		{0.996, 0.995, 0.994, 0.993, 0.991, 0.989, 0.987, 0.984, 0.981, 0.977,
			0.972, 0.966, 0.959, 0.950, 0.940, 0.928, 0.915, 0.898, 0.880, 0.859,
			0.835, 0.806, 0.775, 0.740, 0.702, 0.659, 0.615, 0.567, 0.517, 0.461,
			0.410, 0.353},
		{0.994, 0.993, 0.992, 0.991, 0.989, 0.987, 0.984, 0.981, 0.977, 0.973,
			0.968, 0.961, 0.953, 0.944, 0.933, 0.921, 0.906, 0.890, 0.871, 0.848,
			0.824, 0.797, 0.766, 0.731, 0.695, 0.654, 0.610, 0.563, 0.515, 0.465,
			0.414, 0.363},
		{0.993, 0.992, 0.990, 0.989, 0.987, 0.985, 0.982, 0.978, 0.974, 0.969,
			0.963, 0.956, 0.948, 0.938, 0.927, 0.914, 0.899, 0.881, 0.862, 0.840,
			0.815, 0.787, 0.757, 0.724, 0.688, 0.648, 0.606, 0.560, 0.517, 0.466,
			0.418, 0.369},
		{0.991, 0.990, 0.989, 0.987, 0.984, 0.982, 0.979, 0.975, 0.970, 0.965,
			0.959, 0.951, 0.943, 0.932, 0.921, 0.907, 0.891, 0.874, 0.854, 0.831,
			0.807, 0.779, 0.748, 0.717, 0.680, 0.643, 0.601, 0.561, 0.515, 0.470,
			0.423, 0.374},
		{0.990, 0.988, 0.986, 0.984, 0.982, 0.979, 0.976, 0.971, 0.967, 0.961,
			0.954, 0.946, 0.937, 0.927, 0.914, 0.900, 0.884, 0.866, 0.846, 0.823,
			0.800, 0.773, 0.743, 0.710, 0.676, 0.639, 0.599, 0.558, 0.514, 0.471,
			0.425, 0.381},
		{0.988, 0.986, 0.984, 0.982, 0.979, 0.976, 0.972, 0.968, 0.963, 0.956,
			0.949, 0.941, 0.932, 0.920, 0.908, 0.894, 0.878, 0.859, 0.839, 0.817,
			0.793, 0.766, 0.736, 0.705, 0.671, 0.634, 0.595, 0.555, 0.516, 0.472,
			0.428, 0.385},
		{0.986, 0.984, 0.982, 0.979, 0.976, 0.973, 0.969, 0.964, 0.959, 0.952,
			0.945, 0.936, 0.926, 0.915, 0.902, 0.887, 0.871, 0.853, 0.833, 0.811,
			0.785, 0.759, 0.730, 0.699, 0.666, 0.630, 0.593, 0.555, 0.515, 0.474,
			0.432, 0.391},
		{0.984, 0.982, 0.979, 0.977, 0.974, 0.970, 0.965, 0.960, 0.954, 0.948,
			0.940, 0.931, 0.921, 0.909, 0.896, 0.881, 0.865, 0.847, 0.826, 0.805,
			0.779, 0.753, 0.724, 0.694, 0.661, 0.626, 0.591, 0.553, 0.514, 0.475,
			0.435, 0.395},
		{0.982, 0.980, 0.977, 0.974, 0.970, 0.966, 0.962, 0.956, 0.950, 0.943,
			0.935, 0.926, 0.916, 0.905, 0.891, 0.876, 0.859, 0.841, 0.820, 0.797,
			0.774, 0.747, 0.719, 0.689, 0.658, 0.624, 0.589, 0.553, 0.515, 0.477,
			0.437, 0.398},
		{0.980, 0.977, 0.974, 0.971, 0.967, 0.963, 0.958, 0.953, 0.946, 0.939,
			0.931, 0.921, 0.910, 0.898, 0.885, 0.870, 0.853, 0.835, 0.814, 0.792,
			0.768, 0.742, 0.715, 0.685, 0.654, 0.620, 0.587, 0.551, 0.515, 0.477,
			0.440, 0.401},
		{0.977, 0.975, 0.972, 0.968, 0.964, 0.959, 0.955, 0.949, 0.942, 0.935,
			0.926, 0.916, 0.906, 0.893, 0.880, 0.865, 0.848, 0.829, 0.809, 0.787,
			0.763, 0.739, 0.710, 0.682, 0.651, 0.619, 0.585, 0.551, 0.514, 0.480,
			0.441, 0.405},
		{0.975, 0.972, 0.969, 0.965, 0.961, 0.956, 0.951, 0.945, 0.938, 0.930,
			0.921, 0.912, 0.900, 0.888, 0.874, 0.859, 0.842, 0.824, 0.804, 0.783,
			0.758, 0.734, 0.706, 0.678, 0.647, 0.617, 0.584, 0.550, 0.515, 0.479,
			0.444, 0.408}
	};
	static Double_t p1[16][32] = {
		{0.002, 0.003, 0.003, 0.004, 0.005, 0.006, 0.006, 0.009, 0.010, 0.012,
			0.014, 0.016, 0.020, 0.023, 0.027, 0.032, 0.039, 0.047, 0.056, 0.067,
			0.079, 0.093, 0.109, 0.123, 0.137, 0.150, 0.158, 0.158, 0.147, 0.122,
			0.068, -0.027},
		{0.004, 0.004, 0.005, 0.006, 0.006, 0.008, 0.009, 0.011, 0.013, 0.016,
			0.018, 0.022, 0.026, 0.032, 0.037, 0.045, 0.053, 0.062, 0.073, 0.086,
			0.099, 0.111, 0.124, 0.136, 0.148, 0.156, 0.159, 0.154, 0.139, 0.108,
			0.054, 0.000},
		{0.005, 0.006, 0.006, 0.007, 0.009, 0.010, 0.012, 0.014, 0.017, 0.020,
			0.024, 0.029, 0.034, 0.041, 0.049, 0.056, 0.065, 0.076, 0.087, 0.099,
			0.111, 0.123, 0.135, 0.146, 0.154, 0.159, 0.159, 0.151, 0.131, 0.099,
			0.050, -0.006},
		{0.006, 0.007, 0.008, 0.010, 0.012, 0.013, 0.016, 0.018, 0.022, 0.026,
			0.030, 0.036, 0.042, 0.050, 0.057, 0.066, 0.077, 0.087, 0.098, 0.110,
			0.122, 0.133, 0.144, 0.151, 0.158, 0.158, 0.156, 0.145, 0.125, 0.093,
			0.042, -0.034},
		{0.008, 0.009, 0.011, 0.013, 0.015, 0.017, 0.020, 0.024, 0.027, 0.032,
			0.037, 0.043, 0.050, 0.058, 0.066, 0.076, 0.087, 0.097, 0.107, 0.118,
			0.130, 0.140, 0.147, 0.154, 0.160, 0.159, 0.152, 0.142, 0.119, 0.087,
			0.041, 0.001},
		{0.011, 0.013, 0.015, 0.016, 0.018, 0.022, 0.025, 0.028, 0.033, 0.037,
			0.044, 0.050, 0.057, 0.066, 0.074, 0.083, 0.095, 0.104, 0.115, 0.126,
			0.136, 0.144, 0.150, 0.156, 0.160, 0.156, 0.150, 0.138, 0.115, 0.081,
			0.034, -0.029},
		{0.014, 0.015, 0.017, 0.020, 0.023, 0.026, 0.030, 0.034, 0.038, 0.044,
			0.051, 0.056, 0.064, 0.074, 0.082, 0.091, 0.102, 0.110, 0.122, 0.131,
			0.140, 0.147, 0.155, 0.158, 0.158, 0.155, 0.146, 0.132, 0.109, 0.076,
			0.032, -0.023},
		{0.017, 0.019, 0.021, 0.025, 0.027, 0.031, 0.035, 0.040, 0.044, 0.051,
			0.056, 0.063, 0.071, 0.079, 0.088, 0.098, 0.107, 0.116, 0.127, 0.135,
			0.142, 0.150, 0.154, 0.158, 0.157, 0.151, 0.143, 0.126, 0.103, 0.071,
			0.027, -0.010},
		{0.020, 0.023, 0.025, 0.029, 0.032, 0.036, 0.040, 0.044, 0.050, 0.057,
			0.063, 0.069, 0.077, 0.086, 0.093, 0.102, 0.112, 0.121, 0.131, 0.139,
			0.145, 0.151, 0.156, 0.157, 0.155, 0.149, 0.140, 0.122, 0.100, 0.066,
			0.024, -0.024},
		{0.024, 0.026, 0.029, 0.032, 0.036, 0.041, 0.045, 0.050, 0.056, 0.062,
			0.068, 0.075, 0.083, 0.091, 0.099, 0.107, 0.116, 0.126, 0.133, 0.140,
			0.147, 0.152, 0.156, 0.155, 0.154, 0.147, 0.134, 0.118, 0.094, 0.062,
			0.021, -0.026},
		{0.027, 0.030, 0.033, 0.037, 0.040, 0.045, 0.051, 0.055, 0.061, 0.067,
			0.074, 0.082, 0.088, 0.097, 0.104, 0.114, 0.120, 0.128, 0.136, 0.143,
			0.149, 0.153, 0.155, 0.154, 0.151, 0.143, 0.131, 0.114, 0.089, 0.057,
			0.018, -0.048},
		{0.030, 0.034, 0.037, 0.040, 0.045, 0.051, 0.055, 0.061, 0.067, 0.073,
			0.079, 0.086, 0.094, 0.101, 0.110, 0.117, 0.124, 0.132, 0.138, 0.144,
			0.149, 0.153, 0.155, 0.153, 0.147, 0.141, 0.127, 0.109, 0.084, 0.054,
			0.014, -0.049},
		{0.035, 0.038, 0.042, 0.046, 0.050, 0.055, 0.060, 0.066, 0.073, 0.078,
			0.084, 0.091, 0.099, 0.106, 0.113, 0.121, 0.126, 0.134, 0.141, 0.146,
			0.150, 0.152, 0.152, 0.151, 0.145, 0.136, 0.123, 0.104, 0.081, 0.049,
			0.010, -0.039},
		{0.039, 0.042, 0.046, 0.051, 0.056, 0.060, 0.065, 0.070, 0.077, 0.082,
			0.090, 0.097, 0.103, 0.111, 0.118, 0.123, 0.130, 0.136, 0.143, 0.148,
			0.151, 0.152, 0.152, 0.148, 0.143, 0.133, 0.119, 0.102, 0.076, 0.045,
			0.008, -0.029},
		{0.042, 0.046, 0.050, 0.056, 0.059, 0.064, 0.069, 0.075, 0.082, 0.089,
			0.094, 0.102, 0.109, 0.114, 0.122, 0.129, 0.134, 0.141, 0.145, 0.150,
			0.151, 0.151, 0.150, 0.147, 0.139, 0.129, 0.116, 0.097, 0.073, 0.041,
			0.004, -0.050},
		{0.046, 0.049, 0.054, 0.059, 0.063, 0.068, 0.074, 0.080, 0.086, 0.093,
			0.100, 0.105, 0.111, 0.118, 0.125, 0.131, 0.136, 0.141, 0.146, 0.150,
			0.151, 0.151, 0.148, 0.147, 0.137, 0.126, 0.112, 0.093, 0.068, 0.039,
			0.000, -0.029}
	};


	int i1, j1,
		 i2, j2;
	zmax = zmax-4.5;
	z0 = (-(z0)-144.25)*2;
	i1 = int( zmax );
	if( i1<0 ){
		i1 = i2 =0;
	}else if(i1>14){
		i1 = i2 = 15;
	}else{
		i2 = i1+1;
	}
	j1 = int( z0 );
	if( j1<0 ){
		j1 = j2 = 0;
	}else if(j1>30){
		j1 = j2 = 31;
	}else{
		j2 = j1+1;
	}

	Double_t b1, b2, b3, b4;
	Double_t c1, c2;
	// p0
	b1 = p0[i1][j1];
	b2 = p0[i2][j1] - p0[i1][j1];
	b3 = p0[i1][j2] - p0[i1][j1];
	b4 = p0[i2][j2] - p0[i1][j2] - p0[i2][j1] + p0[i1][j1];
	c1 = b1+b2*(zmax-i1)+b3*(z0-j1)+b4*(zmax-i1)*(z0-j1);

	// p1
	b1 = p1[i1][j1];
	b2 = p1[i2][j1] - p1[i1][j1];
	b3 = p1[i1][j2] - p1[i1][j1];
	b4 = p1[i2][j2] - p1[i1][j2] - p1[i2][j1] + p1[i1][j1];
	c2 = b1+b2*(zmax-i1)+b3*(z0-j1)+b4*(zmax-i1)*(z0-j1);

	return c1*(1+c2*(kx*kx+ky*ky));
}

void EcalKX3DRec::ShowerDisplay(){
	gStyle->SetPadTopMargin(0.1);
	gStyle->SetPadBottomMargin(0.15);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetPadRightMargin(0.1);
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	gStyle->SetLabelSize(0.06, "XYZ");
	gStyle->SetTitleSize(0.06, "XYZ");
	gStyle->SetTitleFont(62,"XYZ");
	gStyle->SetLabelFont(62,"XYZ");
	gStyle->SetNdivisions(505,"XYZ");

	TFile *fOutput = new TFile( Form("showerfit_run%010d_event_%010d.root", run, event), "recreate");
	const int nvar=16;
	TH1D *hEmVar = new TH1D("hEmVar", "EmVar;Index;EmVar", nvar, 0, nvar);
	for(int i=0;i<nvar;i++){
		if( EmVar[i]>0 )
			hEmVar->SetBinContent(i+1, log(EmVar[i]));
	}
	TH1D *hEmLkhd = new TH1D("hEmLkhd", "EmLkhd;Index;EmLkhd", 4, 0, 4);
	for(int i=0;i<4;i++){
		hEmLkhd->SetBinContent(i+1, EmLkhd[i]);
	}
	int color[5] = { 2, 4, 6, kGreen+2, 1 };
//	int fstyle[5] = { 0, 3002, 3004, 3005, 3006 };
	// measure energy map
	TH2D *h2d_meas_mix = new TH2D("h2d_meas_mix", "measured energy;Layer;Cell", EcalKX3DRecDB::kNL, 0, EcalKX3DRecDB::kNL, 72, 0, 72);
	TH2D *h2d_meas = new TH2D("h2d_meas", "measured energy", EcalKX3DRecDB::kNL, 0, EcalKX3DRecDB::kNL, EcalKX3DRecDB::kNC, 0, EcalKX3DRecDB::kNC);
	h2d_meas->SetLineColor(color[0]);
	h2d_meas->SetMarkerColor(color[0]);
	// model prediction
	TH2D *h2d_pred[EcalKX3DRecDB::kNS], *h2d_pred_sum;
	TH1D *h1d_meas = new TH1D("h1d_meas", "data;Layer;Deposited energy[GeV]", EcalKX3DRecDB::kNL, 0, EcalKX3DRecDB::kNL);
	TH1D *h1d_pred = new TH1D("h1d_pred", "model;Layer;Deposited energy[GeV]", EcalKX3DRecDB::kNL, 0, EcalKX3DRecDB::kNL);
	h2d_pred_sum = new TH2D( "h2d_pred_sum", "predicted energy", EcalKX3DRecDB::kNL, 0, EcalKX3DRecDB::kNL, EcalKX3DRecDB::kNC, 0, EcalKX3DRecDB::kNC);
	for(int i=0; i<N_Shwr; i++){
		h2d_pred[i] = new TH2D( Form("h2d_pred_shwr%d", i), "predicted energy", EcalKX3DRecDB::kNL, 0, EcalKX3DRecDB::kNL, EcalKX3DRecDB::kNC, 0, EcalKX3DRecDB::kNC);
		h2d_pred[i]->SetLineColor( color[i+1] );
		h2d_pred[i]->SetLineWidth( 2 );
	}
	// fill histogram
	int m_x0 = (Int_t) ((X0 +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
	int m_y0 = (Int_t) ((Y0 +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
	for(int l=0; l<EcalKX3DRecDB::kNL; l++){
		for(int c=0; c<EcalKX3DRecDB::kNC; c++){
			if( CellDep[l][c]>0 ){
				h2d_meas->SetBinContent( l+1, c+1, CellDep[l][c] );
				if( l%4<2 ) h2d_meas_mix->SetBinContent( l+1, c+1 - m_y0+36, CellDep[l][c] );
				else        h2d_meas_mix->SetBinContent( l+1, c+1 - m_x0+36, CellDep[l][c] );
				h1d_meas->Fill( l+0.5, CellDep[l][c] );
				if( l==12 && c==30 ){
					cout << "L " << l << ", C " << c << ", Dep = " << CellDep[l][c] << endl;
				}
			}
			for(int i=0; i<N_Shwr; i++){
				h2d_pred[i]->SetBinContent( l+1, c+1, ShwrExp[i][l][c] ); // MeV
				h1d_pred->Fill( l+0.5, ShwrExp[i][l][c] );
				if( l==12 && c==30 ){
					cout << "L " << l << ", C " << c << ", Exp = " << ShwrExp[i][l][c] << endl;
				}
			}
		}
	}
	for(int i=0; i<N_Shwr; i++){
		h2d_pred_sum->Add( h2d_pred[i] );
	}
	h2d_pred_sum->SetLineColor( color[4] );
	h2d_pred_sum->SetLineWidth( 2 );
	for(int i=0; i<EcalKX3DRecDB::kNL; i++){
		h1d_meas->SetBinError(i+1, h1d_meas->GetBinError(i+1)/5);
	}

	// make plot
	TCanvas *c1 = new TCanvas( Form("c1_run%010d_event%010d", run, event), Form("run=%10d, event=%10d", run, event), 1200, 800);
	c1->Divide(4,5);
	TH1D *htmp=0;
	TH1D *htmp2=0;
	for(int l=0; l<EcalKX3DRecDB::kNL; l++){
		for(int c=0; c<EcalKX3DRecDB::kNC; c++){
			h2d_meas->SetBinError(l+1, c+1, h2d_meas->GetBinError(l+1,c+1)/5);
		}
	}

	for(int l=0; l<EcalKX3DRecDB::kNL; l++){
		c1->cd(l+1);
		gPad->SetLogy();
		htmp = (TH1D*) h2d_meas->ProjectionY( Form("h2d_meas_l%02d", l), l+1, l+1 );
		htmp->SetMarkerStyle(20);
		htmp->SetMarkerSize(0.8);
		htmp->SetTitle( Form("Lay %02d;Cell;Edep[GeV]", l) );
		htmp->GetXaxis()->SetRange( htmp->FindFirstBinAbove(0)-2, htmp->FindLastBinAbove(0)+2 );
		htmp->Draw("E");
		htmp->GetYaxis()->SetRangeUser( 1e-3, htmp->GetMaximum()*1.3 );
		for(int i=0; i<N_Shwr; i++){
			htmp2 = (TH1D*) h2d_pred[i]->ProjectionY( Form("h2d_pred_shwr%d_l%02d", i, l), l+1, l+1 );
			htmp2->SetLineWidth(2);
			htmp2->SetFillColor( h2d_pred[i]->GetLineColor() );
			htmp2->SetFillStyle( 3001+i );
			htmp2->Draw("SAME");
		}
		h2d_pred_sum->ProjectionY( Form("h2d_pred_shwr_l%02d", l), l+1, l+1 )->Draw("SAME");
	}
	c1->cd(EcalKX3DRecDB::kNL+1);
	hEmVar->Draw();
	c1->cd(EcalKX3DRecDB::kNL+2);
	hEmLkhd->Draw();
	TCanvas *c2 = new TCanvas("c2", "c2", 1000, 700);
	h1d_meas->SetMarkerStyle(20);
	h1d_meas->SetMarkerColor(color[0]);
	h1d_meas->SetLineColor(color[0]);
	h1d_meas->SetMarkerSize(1);
	h1d_pred->SetLineColor(color[4]);
	h1d_pred->SetLineWidth(2);
	h1d_pred->Draw("");
	h1d_meas->Draw("ESAME");
	fOutput->cd();
	c1->Write();
	h2d_meas->Write();
	h2d_meas_mix->Write();
	c2->Write();
	fOutput->Close();
}


void EcalKX3DRec::SetTrackerPath(float trk_p0[], float trk_dir[]){
	IsTrackerUsed = 1;
	TrkKX = trk_dir[0]/trk_dir[2];
	TrkKY = trk_dir[1]/trk_dir[2];
	TrkX0 = trk_p0[0];
	TrkY0 = trk_p0[1];
	TrkZ0 = trk_p0[2];
}

void EcalKX3DRec::MatchTrackerAndShower(){
	if( !IsTrackerUsed ) return ;
	I_ShwrTrk=0;
	double dx, dy;
	double dist=1e100, delta=0.;
	const static double shift_TB[2] = { 0.17, -0.07 }; // TB
	const static double shift_ISS[2] = { 0.134, -0.07 }; // ISS
	const static double kshift_ISS[2] = { -0.00084, -0.0010 }; // ISS
	// alignment
	if( DataType == EcalKX3DRecDB::_ISS_DATA || DataType==EcalKX3DRecDB::_MC_DATA ){
		TrkKX -= kshift_ISS[0];
		TrkKY -= kshift_ISS[1];
		TrkX0 -= shift_ISS[0];
		TrkY0 -= shift_ISS[1];
	}else if( DataType == EcalKX3DRecDB::_TB_DATA ){
		TrkX0 -= shift_TB[0];
		TrkY0 -= shift_TB[1];
	}
	// find the nearest shower
	for(int ish=0; ish<N_Shwr; ish++){
		dx = TrkX0 + TrkKX*(ShwrPar[ish][5][0] - TrkZ0) - ShwrPar[ish][3][0];
		dy = TrkY0 + TrkKY*(ShwrPar[ish][5][0] - TrkZ0) - ShwrPar[ish][4][0];
		delta = dx*dx + dy*dy;
		if( dist > delta ){
		  	dist = delta;
			I_ShwrTrk = ish;
		}
	}
	TrkX0 = TrkX0 + TrkKX*(ShwrPar[I_ShwrTrk][5][0] - TrkZ0);
	TrkY0 = TrkY0 + TrkKY*(ShwrPar[I_ShwrTrk][5][0] - TrkZ0);
	TrkZ0 = ShwrPar[I_ShwrTrk][5][0];
}

void EcalKX3DRec::NonlinearityCorrection(){
	// from fit: ka = 1.089 +/- 0.001, kb = 0.0098 +/- 0.0001
//	static const double ka = 1.098, kb = 0.0102*1.05; // correction based on CellExp
//	static const double ka = 1.092, kb = 0.00996; // correction based on CellEtr
	double ka = 1.091, kb = 0.00976; // correction based on CellEtr
	if( NonlinearityCorrectionBias!=0 ){
		static const double dka = 0.0018, dkb = 0.00012;
		ka += NonlinearityCorrectionBias*dka;
		kb -= NonlinearityCorrectionBias*dkb;
	}
	Int_t l, c;
	Emax=Bmax=Dmax=Emaxc=Emaxt=0;
	Eloss=0.;
	Double_t nonlinear_corr;
	memset( LayEmax, 0, sizeof(LayEmax) );
	memset( LayDmax, 0, sizeof(LayDmax) );
	memset( LayCmax, 0, sizeof(LayCmax) );
	memset( CellExpNosat, 0, sizeof(CellExpNosat) ); // no saturation correction
	for (l=0; l<EcalKX3DRecDB::kNL; l++) {
		for (c=Cell_Min[l]; c<=Cell_Max[l]; c++) {
			nonlinear_corr = TMath::Min( 1.0, ka/(1+kb*CellEtr[l][c]) );
			if( DataType==EcalKX3DRecDB::_MC_DATA ) nonlinear_corr = 1.;
			if( CellEtr[l][c] > Emaxt ){
				Emaxt = CellEtr[l][c];
				Emax = CellExp[l][c];
				Emaxc = CellExp[l][c]*nonlinear_corr;
				Dmax = CellDep[l][c];
				Lmax = l;
				Cmax = c;
				Bmax = CellBeta[l][c];
			}
			CellExpNosat[l][c] = CellExp[l][c]; // no saturation correction
			if( nonlinear_corr < 1 ){
				Eloss += CellExp[l][c]*(1-nonlinear_corr); // nonlinearity loss
				CellExp[l][c] *= nonlinear_corr;
			}

			if( CellExp[l][c] > LayEmax[l] ){
				LayEmax[l] = CellExp[l][c];
				LayDmax[l] = CellDep[l][c];
				LayCmax[l] = c;
			}
		}
	}

	dPMT = GetDistance2PMT( Lmax, Cmax-4 );
}

void EcalKX3DRec::SetBetaLimit(){
	// introduce the calibration precision limit
	// CellBeta*CellExp is the number of secondary particles to describe th fluctuation
	double nsecpart=0.;
	for (Int_t l=0; l<EcalKX3DRecDB::kNL; l++) {
		for (Int_t c=Cell_Min[l]; c<=Cell_Max[l]; c++) {
			nsecpart = CellBeta[l][c]*CellExp[l][c];
			CellBeta[l][c] /= sqrt( 1 + 6.5e-5*nsecpart*nsecpart ); // 8% best resolution
		}
	}
}

void EcalKX3DRec::CalculateDmax3(){
	Emax=0;
	if( Emax==0 ){
		for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++) {
			for(Int_t c=Cell_Min[l]; c<=Cell_Max[l]; c++) {
				if( CellExp[l][c] > Emax ){
					Emax = CellExp[l][c];
					Lmax = l;
					Cmax = c;
				}
			}
		}
		if( Lmax<0 || Lmax>EcalKX3DRecDB::kNL-1 || Cmax<0 || Cmax>EcalKX3DRecDB::kNC-1 ) return;
		Emaxt = CellEtr[Lmax][Cmax];
		Dmax = CellDep[Lmax][Cmax];
		Bmax = CellBeta[Lmax][Cmax];
	}
	if( Lmax<0 || Lmax>EcalKX3DRecDB::kNL-1 || Cmax<0 || Cmax>EcalKX3DRecDB::kNC-1 ) return;
	Emax3=Dmax3=Emaxc3=0;
	if( CellMask[Lmax][Cmax] == 0 || CellMask[Lmax][Cmax] == 8 ){
		Dmax3 = CellDep[Lmax][Cmax];
	}else{
		cout << Form("CellMask[%d][%d]=%d", Lmax, Cmax, CellMask[Lmax][Cmax]) << endl;
		Dmax = CellExp[Lmax][Cmax];
		Dmax3 = CellExp[Lmax][Cmax];
	}
	if( Cmax-4 > 1 && Cmax-4<71 ){
		if( CellMask[Lmax][Cmax-1] == 0 || CellMask[Lmax][Cmax-1] == 8 ){
			Dmax3 += CellDep[Lmax][Cmax-1];
		}else{
			Dmax3 += CellExp[Lmax][Cmax-1];
		}
		if( CellMask[Lmax][Cmax+1] == 0 || CellMask[Lmax][Cmax+1] == 8 ){
			Dmax3 += CellDep[Lmax][Cmax+1];
		}else{
			Dmax3 += CellExp[Lmax][Cmax+1];
		}

		Emax3 = CellExpNosat[Lmax][Cmax-1] + CellExpNosat[Lmax][Cmax] + CellExpNosat[Lmax][Cmax+1];
		Emaxc3 = CellExp[Lmax][Cmax-1] + CellExp[Lmax][Cmax] + CellExp[Lmax][Cmax+1];
	}else if( Cmax-4>1 ){
		if( CellMask[Lmax][Cmax-1] == 0 || CellMask[Lmax][Cmax-1] == 8 ){
			Dmax3 += CellDep[Lmax][Cmax-1]*2;
		}else{
			Dmax3 += CellExp[Lmax][Cmax-1]*2;
		}

		Emax3 = CellExpNosat[Lmax][Cmax-1]*2 + CellExpNosat[Lmax][Cmax];
		Emaxc3 = CellExp[Lmax][Cmax-1]*2 + CellExp[Lmax][Cmax];
	}else{
		if( CellMask[Lmax][Cmax+1] == 0 || CellMask[Lmax][Cmax+1] == 8 ){
			Dmax3 += CellDep[Lmax][Cmax+1]*2;
		}else{
			Dmax3 += CellExp[Lmax][Cmax+1]*2;
		}

		Emax3 = CellExpNosat[Lmax][Cmax+1]*2 + CellExpNosat[Lmax][Cmax];
		Emaxc3 = CellExp[Lmax][Cmax+1]*2 + CellExp[Lmax][Cmax];
	}
}

void EcalKX3DRec::CalculateD8NF(){
	dPMTx = dPMTy = 0.;
	emx = emy = 0.;
	emx2 = emy2 = 0.;

	double dpmt=0;
	for(int l=0; l<EcalKX3DRecDB::kNL; l++){
		for(int c=Cell_Min[l]; c<=Cell_Max[l]; c++) {
			if( CellDep[l][c] > 0 ){
				dpmt = GetDistance2PMT(l,c);
				if( l%4>=2 ){
					if( emx < CellDep[l][c] ){
						emx = CellDep[l][c];
						dPMTx = dpmt;
					}
				}else{
					if( emy < CellDep[l][c] ){
						emy = CellDep[l][c];
						dPMTy = dpmt;
					}
				}
			}
		}
	}
}

double EcalKX3DRec::GetPMTEfficiency(Int_t l_fiber, Int_t k_fiber, Int_t l_ecal, Int_t cell){
	double ret=0.;
	double v_pmt=0., dc=0.;
	if( (l_ecal%4<2 && (cell)%4<2) || (l_ecal%4>=2 && (cell)%4>=2) ) // PMT at -32.4 cm
		v_pmt = EcalKX3DRecDB::F_Diameter*(k_fiber-266.5) + 32.4*EcalKX3DRecDB::F_Rotation[l_ecal] + EcalKX3DRecDB::F_Offset[l_fiber];
	else  // PMT at +32.4 cm
		v_pmt = EcalKX3DRecDB::F_Diameter*(k_fiber-266.5) - 32.4*EcalKX3DRecDB::F_Rotation[l_ecal] + EcalKX3DRecDB::F_Offset[l_fiber];

	dc = TMath::Abs(v_pmt/EcalKX3DRecDB::Cell_Size-cell+39.5);
	if( dc > 0.5 ){
	  	ret = -1; // outside the acceptance of the given cell
	}
	else{
		dc -= EcalKX3DRecDB::L_Eff_PW; // PW=0.32, original value, AK
		if ( dc > 0. ) ret = EcalKX3DRecDB::L_Eff[l_fiber%5]*(1. - EcalKX3DRecDB::L_Eff_Par*dc); // --> minimum efficiency 28%, AK
		else           ret = EcalKX3DRecDB::L_Eff[l_fiber%5];
		if( ret <= 0 || TMath::IsNaN(ret) ){
			printf("Line 4910: l_fiber=%d, k_fiber=%d, l_ecal=%d, cell=%d, dc=%g, L_Eff_Par=%g, ret=%g\n", l_fiber, k_fiber, l_ecal, cell, dc, EcalKX3DRecDB::L_Eff_Par, ret);
			ret = 0;
		}
	}
	return ret;
}

void EcalKX3DRec::GetPMTEfficiencyFromTable(int l_fb, int k_fb, int &cm, int &cp, double &em, double &ep){
	static int IsPmtTableInitialized = 0;
	static int cell_minus[EcalKX3DRecDB::nFibLayer][EcalKX3DRecDB::nFibPerLay],
				  cell_plus[EcalKX3DRecDB::nFibLayer][EcalKX3DRecDB::nFibPerLay];
	static double eff_minus[EcalKX3DRecDB::nFibLayer][EcalKX3DRecDB::nFibPerLay],
					  eff_plus[EcalKX3DRecDB::nFibLayer][EcalKX3DRecDB::nFibPerLay];
	if( IsPmtTableInitialized == 0 ){
		double v0, v1, eff0, eff1;
		int m, m0, m1, l_ecal;
		memset( cell_minus, 0, sizeof(cell_minus) );
		memset( cell_plus,  0, sizeof(cell_plus)  );
		memset( eff_minus,  0, sizeof(eff_minus)  );
		memset( eff_plus,   0, sizeof(eff_plus)   );
		for(int l=0; l<EcalKX3DRecDB::nFibLayer; l++){
			l_ecal = l/5;
			for(int k=0; k<EcalKX3DRecDB::nFibPerLay; k++){
				v0 = EcalKX3DRecDB::F_Diameter*(k-266.5) + 32.4*EcalKX3DRecDB::F_Rotation[l_ecal] + EcalKX3DRecDB::F_Offset[l]; // readout at "-" end
				v1 = EcalKX3DRecDB::F_Diameter*(k-266.5) - 32.4*EcalKX3DRecDB::F_Rotation[l_ecal] + EcalKX3DRecDB::F_Offset[l]; // readout at "+" end
				m0 = (Int_t) ((v0 + 40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size); // "-" end
				m1 = (Int_t) ((v1 + 40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size); // "+" end

				if( ( l_ecal/2%2 != m0/2%2 ) && (l_ecal/2%2 == m1/2%2) ){ // not readout by any PMT
					eff0 = eff1 = 0;
				}
				else{
					if( int(m0/2)!=int(m1/2) ){ // readout by 2 cells from 2 PMT
						eff0 = GetPMTEfficiency( l, k, l_ecal, m0 );
						eff1 = GetPMTEfficiency( l, k, l_ecal, m1 );
					}else{ // readout by single cell
						if( l_ecal/2%2 == m0/2%2 ){ // PMT at "-" end
							m = m0;
						}else{  // PMT at "+" end
							m = m1;
						}
						eff0 = eff1 = GetPMTEfficiency( l, k, l_ecal, m );
					}
				}
				cell_minus[l][k] = m0;
				cell_plus[l][k] = m1;
				eff_minus[l][k] = eff0;
				eff_plus[l][k] = eff1;
			}
		}
		IsPmtTableInitialized=1;
	}
	if( l_fb>=0 && l_fb<EcalKX3DRecDB::nFibLayer && k_fb>=0 && k_fb<EcalKX3DRecDB::nFibPerLay ){
		cm = cell_minus[l_fb][k_fb];
		cp = cell_plus[l_fb][k_fb];
		em = eff_minus[l_fb][k_fb];
		ep = eff_plus[l_fb][k_fb];
	}else{
		cm = cp = em = ep = 0;
	}
}

void EcalKX3DRec::DefineCommonFootprint(){
	Int_t l, n;
	// check whether combined fit is needed
	if ( ShwrDep[0][0]>0 || ShwrDep[0][1]>0 ) {
		// define common fit footprint for the first shower,
		// the footprint of first shower is enlarged to contain the footprints of secondary showers
		for (l=0; l<18; l++) {
			Cell_Min[l] = 76;
			Cell_Max[l] = 3;
			if ( (l/2)%2 == 1 ) {
				// X projection
				for (n=0; n<N_Shwr; n++) {
					if ( ShwrDep[n][0]==1 ) {
						Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]-1);
						Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]+1);
					} else if ( ShwrDep[n][0]==0 ) {
						Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]);
						Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]);
					}
				}
			} else {
				// Y projection
				for (n=0; n<N_Shwr; n++) {
					if ( ShwrDep[n][1]==1 ) {
						Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]-1);
						Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]+1);
					} else if ( ShwrDep[n][1]==0 ) {
						Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]);
						Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]);
					}
				}
			}
			Cell_Min[l] = TMath::Max(  3, Cell_Min[l]);
			Cell_Max[l] = TMath::Min( 76, Cell_Max[l]);
		}


		for(Int_t k=0; k<N_Shwr; k++){
			SetLatPar(k, ShwrPar[k][0][0]);
			SetPdfPar(k, ShwrPar[k][0][0]);
		}
	}
}

Int_t EcalKX3DRec::OverlapShowerRec(){
	if(N_Shwr!=1) return -1; // only for single shower
//	if( N_Shwr<1 || N_Shwr>=3 ) return -1; //

	Int_t flag_x=0, flag_y=0;
	Flag_overlap = OverlapShowerSearch(flag_x, flag_y);
	if( flag_x<2 && flag_y<2 ){
		return 0; // no overlap shower found
	}
	// copy the first shower parameters to the second
	for(Int_t i=0; i<EcalKX3DRecDB::kNpar; i++){
		ShwrPar[N_Shwr][i][0] = ShwrPar[0][i][0];
		ShwrPar[N_Shwr][i][1] = ShwrPar[0][i][1];
	}
	for(Int_t l=0;l<EcalKX3DRecDB::kNL;l++){
		ShwrFprt[N_Shwr][l][0] = ShwrFprt[0][l][0];
		ShwrFprt[N_Shwr][l][1] = ShwrFprt[0][l][1];
	}
	// share the energy
	double frac = 0.1 + 0.35*exp(-ShwrPar[0][0][0]/50);
//	double frac = 0.4;
	ShwrPar[0][0][0] *= 1-frac;
	ShwrPar[0][0][1] *= 1-frac;
	ShwrPar[N_Shwr][0][0] *= frac;
	ShwrPar[N_Shwr][0][1] = 0.2*ShwrPar[N_Shwr][0][0]; // 20% resolution

	// move Z0 according to log(E)
	double deltaZ = 1.02*log(ShwrPar[0][0][0] / ShwrPar[N_Shwr][0][0]);
	ShwrPar[N_Shwr][3][0] += deltaZ*ShwrPar[N_Shwr][6][0];
	ShwrPar[N_Shwr][4][0] += deltaZ*ShwrPar[N_Shwr][7][0];
	ShwrPar[N_Shwr][5][0] += deltaZ;

	// move the shower axis by +/- 0.45 cm
	if( flag_x>=2 ){
		ShwrPar[0][3][0] += 0.45;
		ShwrPar[N_Shwr][3][0] -= 0.45;
	}
	if( flag_y>=2 ){
		ShwrPar[0][4][0] += 0.45;
		ShwrPar[N_Shwr][4][0] -= 0.45;
	}

	if(DEBUG){
		printf("Shower 0: E0=%8.2f, B0=%8.2f, A0=%8.2f, X0=%8.2f, Y0=%8.2f, Z0=%8.2f, KX=%8.2f, KY=%8.2f\n", ShwrPar[0][0][0], ShwrPar[0][1][0], ShwrPar[0][2][0], ShwrPar[0][3][0], ShwrPar[0][4][0], ShwrPar[0][5][0], ShwrPar[0][6][0], ShwrPar[0][7][0]);
		printf("Shower 1: E0=%8.2f, B0=%8.2f, A0=%8.2f, X0=%8.2f, Y0=%8.2f, Z0=%8.2f, KX=%8.2f, KY=%8.2f\n", ShwrPar[N_Shwr][0][0], ShwrPar[N_Shwr][1][0], ShwrPar[N_Shwr][2][0], ShwrPar[N_Shwr][3][0], ShwrPar[N_Shwr][4][0], ShwrPar[N_Shwr][5][0], ShwrPar[N_Shwr][6][0], ShwrPar[N_Shwr][7][0]);
	}
	ShwrDep[0][0] = ShwrDep[N_Shwr][0] = 1;
	ShwrDep[0][1] = ShwrDep[N_Shwr][1] = 1;
	IsSkipCombineFit=0;
	N_Shwr++;
	// reset the shower shape parameters
	for(Int_t k=0; k<N_Shwr; k++){
		SetLatPar( k, ShwrPar[k][0][0] );
		SetPdfPar( k, ShwrPar[k][0][0] );
	}

	ShowerCombineFit();

	return 1;
}

Int_t EcalKX3DRec::OverlapShowerSearch(int &flag_x, int &flag_y){
	flag_x = flag_y = 0;
	Int_t nx=0, ny=0;
	Double_t x, y, z;
	Int_t l, m, c;

	for(Int_t k=N_Shwr-1; k>=0; k--){
		EcalShwrDef(k);
	}
	memset(CellExp, 0, sizeof(CellExp));
	memset(CellBeta, 0, sizeof(CellBeta));
	for(Int_t l=0; l<EcalKX3DRecDB::kNL; l++){
		if(DEBUG) printf("EcalKX3DRec::OverlapShowerSearch--l=%d, Cell_Min=%d, Cell_Max=%d\n", l, Cell_Min[l], Cell_Max[l]);
		for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			for(Int_t k=0; k<N_Shwr; k++){
				CellExp[l][m] += ShwrExp[k][l][m];
				CellBeta[l][m] += ShwrBeta[k][l][m]*ShwrExp[k][l][m]; // number of particles
			}
		}
	}
	sum_nsigma_x = sum_nsigma_y = 0;
	const static double thres = 0.5;
	// only check if the primary shower is overlap by an un-defined shower
	for(l=0; l<EcalKX3DRecDB::kNL; l++){
		z = EcalKX3DRecDB::EcalZ[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		if ( (l/2)%2 ) m = (Int_t) ((x +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);
		else           m = (Int_t) ((y +40*EcalKX3DRecDB::Cell_Size)/EcalKX3DRecDB::Cell_Size);

		Int_t nc=0;
		Double_t nsigma[7]={0,0,0,0,0,0,0};
		Double_t min_nsigma = 0, sum_nsigma = 0.;
		Int_t i=0, i_min_nsigma=0;;
		for(c=m-3; c<=m+3; c++){
			if( CellMask[l][c] ) continue;
			if( CellDep[l][c]<= 0 ) continue;
			if( CellExp[l][c]<= 0 ) continue;
			if( c>=4 && c<EcalKX3DRecDB::kNC-4 && (CellExp[l][c] > 0.020 || CellDep[l][c]>0.020) && CellDep[l][c] > 0 ){ // 20 MeV
				nsigma[i] = (CellDep[l][c] - CellExp[l][c]) / CellExp[l][c] * sqrt(CellBeta[l][c]); // dE/sigmaE = dE/E *sqrt(N)
				if( nsigma[i] < min_nsigma && c>=m-1 && c<=m+1 ){
					min_nsigma = nsigma[i];
					i_min_nsigma = i;
				}
			}
			i++;
		}

		if(DEBUG){
			printf("l=%d, m=%d, c=%d, sigma=(%5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f)\n", l, m, i_min_nsigma, nsigma[0], nsigma[1], nsigma[2], nsigma[3], nsigma[4], nsigma[5], nsigma[6]);
		}

		if( i_min_nsigma>=2 && i_min_nsigma<=4 ) m = i_min_nsigma; // update to max negative signficant cell
		else                                     m = 3;

		if( nsigma[m] < -1 && nsigma[m+1]>0 && nsigma[m-1]>0 && nsigma[m+1]+nsigma[m-1]>3  ) nc++;
		if( nsigma[m] < 0 && nsigma[m+1]<0 && nsigma[m]+nsigma[m+1]<-2 && nsigma[m-1]>1 && nsigma[m+2]>1 ) nc++;
		if( nsigma[m] < 0 && nsigma[m-1]<0 && nsigma[m]+nsigma[m-1]<-2 && nsigma[m-2]>1 && nsigma[m+1]>1 ) nc++;

		if(nsigma[m]<-thres && nsigma[m-1]>thres && nsigma[m+1]>thres){
			for(c=m-1; c<=m+1; c++) sum_nsigma += nsigma[c]*nsigma[c]/1.732;
		}else if(nsigma[m]<-thres && nsigma[m-1]<-thres && nsigma[m-2]>thres && nsigma[m+1]>thres){
			for(c=m-2; c<=m+1; c++) sum_nsigma += nsigma[c]*nsigma[c]/2;
		}else if(nsigma[m]<-thres && nsigma[m+1]<-thres && nsigma[m-1]>thres && nsigma[m+2]>thres){
			for(c=m-1; c<=m+2; c++) sum_nsigma += nsigma[c]*nsigma[c]/2;
		}
		if( (l/2)%2 ) sum_nsigma_x += sum_nsigma;
		else          sum_nsigma_y += sum_nsigma;

		if(nc>0){
			if( (l/2)%2 ) nx++;
			else          ny++;
		}
		if(DEBUG) printf("nc=%d, sum_nsigma_x=%8.2f, sum_nsigma_y=%8.2f\n", nc, sum_nsigma_x, sum_nsigma_y);
	}
	if(DEBUG){
		cout << "--EcalKX3DRec::CheckOverlapShower()-- overlap x = " << nx << ", y=" << ny << ", nsigma_x=" << sum_nsigma_x << ", nsigma_y=" << sum_nsigma_y << endl;
	}
	const static int opt=2;
	if(opt==0){
		flag_x = nx;
		flag_y = ny;
	}else{
		if(opt==1){
			if( sum_nsigma_x>10 ) flag_x=2;
			if( sum_nsigma_y>10 ) flag_y=2;
		}else if(opt==2){
			if( sum_nsigma_x>10 ) flag_x=1;
			if( sum_nsigma_x>16 ) flag_x=2;
			if( sum_nsigma_y>10 ) flag_y=1;
			if( sum_nsigma_y>16 ) flag_y=2;
			flag_x += nx;
			flag_y += ny;
		}
	}
	return nx*100 + ny;
}
